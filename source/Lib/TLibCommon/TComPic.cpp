/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2016, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TComPic.cpp
    \brief    picture class
*/

#include "TComPic.h"
#include "SEI.h"

//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Constructor / destructor / create / destroy
// ====================================================================================================================

TComPic::TComPic()
: m_uiTLayer                              (0)
, m_bUsedByCurr                           (false)
, m_bIsLongTerm                           (false)
, m_pcPicYuvPred                          (NULL)
, m_pcPicYuvResi                          (NULL)
, m_bReconstructed                        (false)
, m_bNeededForOutput                      (false)
, m_uiCurrSliceIdx                        (0)
, m_bCheckLTMSB                           (false)
{
  for(UInt i=0; i<NUM_PIC_YUV; i++)
  {
    m_apcPicYuv[i]      = NULL;
  }
}

TComPic::~TComPic()
{
  destroy();
}

Void TComPic::create( const TComSPS &sps, const TComPPS &pps, const Bool bIsVirtual)
{
  destroy();

  const ChromaFormat chromaFormatIDC = sps.getChromaFormatIdc();
  const Int          iWidth          = sps.getPicWidthInLumaSamples();
  const Int          iHeight         = sps.getPicHeightInLumaSamples();
  const UInt         uiMaxCuWidth    = sps.getMaxCUWidth();
  const UInt         uiMaxCuHeight   = sps.getMaxCUHeight();
  const UInt         uiMaxDepth      = sps.getMaxTotalCUDepth();

  m_picSym.create( sps, pps, uiMaxDepth );
  if (!bIsVirtual)
  {
    m_apcPicYuv[PIC_YUV_ORG    ]   = new TComPicYuv;  m_apcPicYuv[PIC_YUV_ORG     ]->create( iWidth, iHeight, chromaFormatIDC, uiMaxCuWidth, uiMaxCuHeight, uiMaxDepth, true );
    m_apcPicYuv[PIC_YUV_TRUE_ORG]  = new TComPicYuv;  m_apcPicYuv[PIC_YUV_TRUE_ORG]->create( iWidth, iHeight, chromaFormatIDC, uiMaxCuWidth, uiMaxCuHeight, uiMaxDepth, true );
  }
  m_apcPicYuv[PIC_YUV_REC]  = new TComPicYuv;  m_apcPicYuv[PIC_YUV_REC]->create( iWidth, iHeight, chromaFormatIDC, uiMaxCuWidth, uiMaxCuHeight, uiMaxDepth, true );

  // there are no SEI messages associated with this picture initially
  if (m_SEIs.size() > 0)
  {
    deleteSEIs (m_SEIs);
  }
  m_bUsedByCurr = false;
}

Void TComPic::destroy()
{
  m_picSym.destroy();

  for(UInt i=0; i<NUM_PIC_YUV; i++)
  {
    if (m_apcPicYuv[i])
    {
      m_apcPicYuv[i]->destroy();
      delete m_apcPicYuv[i];
      m_apcPicYuv[i]  = NULL;
    }
  }

  deleteSEIs(m_SEIs);
}

Void TComPic::compressMotion()
{
  TComPicSym* pPicSym = getPicSym();
  for ( UInt uiCUAddr = 0; uiCUAddr < pPicSym->getNumberOfCtusInFrame(); uiCUAddr++ )
  {
    TComDataCU* pCtu = pPicSym->getCtu(uiCUAddr);
    pCtu->compressMV();
  }
}

Bool  TComPic::getSAOMergeAvailability(Int currAddr, Int mergeAddr)
{
  Bool mergeCtbInSliceSeg = (mergeAddr >= getPicSym()->getCtuTsToRsAddrMap(getCtu(currAddr)->getSlice()->getSliceCurStartCtuTsAddr()));
  Bool mergeCtbInTile     = (getPicSym()->getTileIdxMap(mergeAddr) == getPicSym()->getTileIdxMap(currAddr));
  return (mergeCtbInSliceSeg && mergeCtbInTile);
}

UInt TComPic::getSubstreamForCtuAddr(const UInt ctuAddr, const Bool bAddressInRaster, TComSlice *pcSlice)
{
  UInt subStrm;
  const bool bWPPEnabled=pcSlice->getPPS()->getEntropyCodingSyncEnabledFlag();
  const TComPicSym &picSym            = *(getPicSym());

  if ((bWPPEnabled && picSym.getFrameHeightInCtus()>1) || (picSym.getNumTiles()>1)) // wavefronts, and possibly tiles being used.
  {
    if (bWPPEnabled)
    {
      const UInt ctuRsAddr                = bAddressInRaster?ctuAddr : picSym.getCtuTsToRsAddrMap(ctuAddr);
      const UInt frameWidthInCtus         = picSym.getFrameWidthInCtus();
      const UInt tileIndex                = picSym.getTileIdxMap(ctuRsAddr);
      const UInt numTileColumns           = (picSym.getNumTileColumnsMinus1()+1);
      const TComTile *pTile               = picSym.getTComTile(tileIndex);
      const UInt firstCtuRsAddrOfTile     = pTile->getFirstCtuRsAddr();
      const UInt tileYInCtus              = firstCtuRsAddrOfTile / frameWidthInCtus;
      // independent tiles => substreams are "per tile"
      const UInt ctuLine                  = ctuRsAddr / frameWidthInCtus;
      const UInt startingSubstreamForTile =(tileYInCtus*numTileColumns) + (pTile->getTileHeightInCtus()*(tileIndex%numTileColumns));
      subStrm = startingSubstreamForTile + (ctuLine - tileYInCtus);
    }
    else
    {
      const UInt ctuRsAddr                = bAddressInRaster?ctuAddr : picSym.getCtuTsToRsAddrMap(ctuAddr);
      const UInt tileIndex                = picSym.getTileIdxMap(ctuRsAddr);
      subStrm=tileIndex;
    }
  }
  else
  {
    // dependent tiles => substreams are "per frame".
    subStrm = 0;
  }
  return subStrm;
}
#if RM_4DLF_MI_BUFFER
Void TComPic::spiral(UInt idx, UInt size, UInt* x, UInt* y)
{
	Int init = floor(size/2);
	Int current_idx = 0;
	char current_direction[] = "RDLU";
	Int dir_idx = 0;
	Int original_steps_to_update = 1;
	Int original_steps_to_change_dir = 1;
	Int steps_to_update = 2;
	Int steps_to_change_dir = 1;
	Int step = 0;
	Int x_pos = init;
	Int y_pos = init;
	while(current_idx < idx)
	{
		if(current_direction[dir_idx] == 'R') // RIGHT
			x_pos++;
		else if(current_direction[dir_idx] == 'D') // DOWN
			y_pos++;
		else if(current_direction[dir_idx] == 'L') // LEFT
			x_pos--;
		else // UP
			y_pos--;
		if(step == steps_to_change_dir-1)
		{
			step = 0;
			steps_to_change_dir = original_steps_to_change_dir;
			dir_idx++;
			if(dir_idx > 3)
				dir_idx = 0;
		}
		else
			step++;
		current_idx++;
		steps_to_update--;
		if(steps_to_update == 0)
		{
			original_steps_to_change_dir++;
			steps_to_change_dir = original_steps_to_change_dir;
			original_steps_to_update++;
			steps_to_update = original_steps_to_update * 2; // 1, 2, 4, 6, 8, 10...
		}
	}

	*x = x_pos;
	*y = y_pos;
}

Void TComPic::spiralScalable(UInt idx, UInt size, UInt* x, UInt* y)
{

	Int layerMask[size][size] = { {6, 6, 4, 6, 3, 6, 4, 6, 3, 6, 4, 6, 6},
								  {6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6},
							      {4, 6, 2, 6, 4, 6, 2, 6, 4, 6, 2, 6, 4},
								  {6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6},
								  {3, 6, 4, 6, 3, 6, 4, 6, 3, 6, 4, 6, 3},
								  {6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6},
								  {4, 6, 2, 6, 4, 6, 1, 6, 4, 6, 2, 6, 4},
								  {6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6},
								  {3, 6, 4, 6, 3, 6, 4, 6, 3, 6, 4, 6, 3},
								  {6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6},
								  {4, 6, 2, 6, 4, 6, 2, 6, 4, 6, 2, 6, 4},
								  {6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6},
								  {6, 6, 4, 6, 3, 6, 4, 6, 3, 6, 4, 6, 6} };
	Int init = floor(size/2);
	Int current_idx = 0;
	char current_direction[] = "RDLU";
	Int dir_idx = 0;
	Int original_steps_to_update = 1;
	Int original_steps_to_change_dir = 1;
	Int steps_to_update = 2;
	Int steps_to_change_dir = 1;
	Int step = 0;
	Int x_pos = init;
	Int y_pos = init;

	while(current_idx < idx)
	{
		for(Int l = 2; l <= 6; l++)
		{
			original_steps_to_update = 1;
			original_steps_to_change_dir = 1;
			steps_to_update = 2;
			steps_to_change_dir = 1;
			dir_idx = 0;
			step = 0;
			x_pos = init;
			y_pos = init;
			while(x_pos >= 0 && x_pos < size && y_pos >= 0 && y_pos < size)
			{
				if(current_direction[dir_idx] == 'R') // RIGHT
					x_pos++;
				else if(current_direction[dir_idx] == 'D') // DOWN
					y_pos++;
				else if(current_direction[dir_idx] == 'L') // LEFT
					x_pos--;
				else // UP
					y_pos--;
				if(step == steps_to_change_dir-1)
				{
					step = 0;
					steps_to_change_dir = original_steps_to_change_dir;
					dir_idx++;
					if(dir_idx > 3)
						dir_idx = 0;
				}
				else
					step++;
				if(layerMask[y_pos][x_pos] == l)
				{
					current_idx++;
					if(current_idx == idx)
						break;
				}
				steps_to_update--;
				if(steps_to_update == 0)
				{
					original_steps_to_change_dir++;
					steps_to_change_dir = original_steps_to_change_dir;
					original_steps_to_update++;
					steps_to_update = original_steps_to_update * 2; // 1, 2, 4, 6, 8, 10...
				}
			}
			if(current_idx == idx)
				break;
		}
	}

	*x = x_pos;
	*y = y_pos;
}
#if RM_DEBUG_FILES
Bool TComPic::writePlane(std::ostream& fd, Pel* src, Bool is16bit,
                       UInt stride444,
                       UInt width444, UInt height444,
                       ComponentID compID,
                       ChromaFormat srcFormat,
                       ChromaFormat fileFormat,
                       UInt fileBitDepth)
{
	/*// COMMENTED TO MAKE IT WORK IN TCOMPIC
  const UInt csx_file =getComponentScaleX(compID, fileFormat);
  const UInt csy_file =getComponentScaleY(compID, fileFormat);
  const UInt csx_src  =getComponentScaleX(compID, srcFormat);
  const UInt csy_src  =getComponentScaleY(compID, srcFormat);
  */
	const UInt csx_file =getComponentScaleX(compID);
	const UInt csy_file =getComponentScaleY(compID);
	const UInt csx_src  =getComponentScaleX(compID);
	const UInt csy_src  =getComponentScaleY(compID);

  const UInt stride_src      = stride444>>csx_src;

  const UInt stride_file      = (width444 * (is16bit ? 2 : 1)) >> csx_file;
  const UInt width_file       = width444 >>csx_file;
  const UInt height_file      = height444>>csy_file;

  std::vector<UChar> bufVec(stride_file);
  UChar *buf=&(bufVec[0]);

  if (compID!=COMPONENT_Y && (fileFormat==CHROMA_400 || srcFormat==CHROMA_400))
  {
    if (fileFormat!=CHROMA_400)
    {
      const UInt value=1<<(fileBitDepth-1);

      for(UInt y=0; y< height_file; y++)
      {
        if (!is16bit)
        {
          UChar val(value);
          for (UInt x = 0; x < width_file; x++)
          {
            buf[x]=val;
          }
        }
        else
        {
          UShort val(value);
          for (UInt x = 0; x < width_file; x++)
          {
            buf[2*x+0]= (val>>0) & 0xff;
            buf[2*x+1]= (val>>8) & 0xff;
          }
        }

        fd.write(reinterpret_cast<const TChar*>(buf), stride_file);
        if (fd.eof() || fd.fail() )
        {
          return false;
        }
      }
    }
  }
  else
  {
    const UInt mask_y_file=(1<<csy_file)-1;
    const UInt mask_y_src =(1<<csy_src )-1;
    for(UInt y444=0; y444<height444; y444++)
    {
      if ((y444&mask_y_file)==0)
      {
        // write a new line
        if (csx_file < csx_src)
        {
          // eg file is 444, source is 422.
          const UInt sx=csx_src-csx_file;
          if (!is16bit)
          {
            for (UInt x = 0; x < width_file; x++)
            {
              buf[x] = (UChar)(src[x>>sx]);
            }
          }
          else
          {
            for (UInt x = 0; x < width_file; x++)
            {
              buf[2*x  ] = (src[x>>sx]>>0) & 0xff;
              buf[2*x+1] = (src[x>>sx]>>8) & 0xff;
            }
          }
        }
        else
        {
          // eg file is 422, src is 444.
          const UInt sx=csx_file-csx_src;
          if (!is16bit)
          {
            for (UInt x = 0; x < width_file; x++)
            {
              buf[x] = (UChar)(src[x<<sx]);
            }
          }
          else
          {
            for (UInt x = 0; x < width_file; x++)
            {
              buf[2*x  ] = (src[x<<sx]>>0) & 0xff;
              buf[2*x+1] = (src[x<<sx]>>8) & 0xff;
            }
          }
        }

        fd.write(reinterpret_cast<const TChar*>(buf), stride_file);
        if (fd.eof() || fd.fail() )
        {
          return false;
        }
      }

      if ((y444&mask_y_src)==0)
      {
        src += stride_src;
      }

    }
  }
  return true;
}
#endif
#endif


//! \}
