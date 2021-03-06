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

/** \file     TComPic.h
    \brief    picture class (header)
*/

#ifndef __TCOMPIC__
#define __TCOMPIC__

// Include files
#include "CommonDef.h"
#include "TComPicSym.h"
#include "TComPicYuv.h"
#include "TComBitStream.h"
#if RM_4DLF_MI_BUFFER
#include <cmath>
#endif

//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// picture class (symbol + YUV buffers)

class TComPic
{
public:
  typedef enum { PIC_YUV_ORG=0, PIC_YUV_REC=1, PIC_YUV_TRUE_ORG=2, NUM_PIC_YUV=3 } PIC_YUV_T;
     // TRUE_ORG is the input file without any pre-encoder colour space conversion (but with possible bit depth increment)
  TComPicYuv*   getPicYuvTrueOrg()        { return  m_apcPicYuv[PIC_YUV_TRUE_ORG]; }

private:
  UInt                  m_uiTLayer;               //  Temporal layer
  Bool                  m_bUsedByCurr;            //  Used by current picture
  Bool                  m_bIsLongTerm;            //  IS long term picture
  TComPicSym            m_picSym;                 //  Symbol
  TComPicYuv*           m_apcPicYuv[NUM_PIC_YUV];
#if RM_4DLF_MI_BUFFER
  TComPicYuv*			m_pcPic4DLFMI;
  UInt					m_uiMicroImageSize;
  UInt					m_uiTotalNumberOfSAIs;
  UInt					m_uiCurrentSAIsSpiralPosX;
  UInt					m_uiCurrentSAIsSpiralPosY;
  UInt 					m_uiCurrentSAI;
#if RM_4DLF_SAI_BUFFER
  TComPicYuv* 			m_pcPic4DLFSAI;
#endif
#if RM_SCALABLE
  TComPicYuv* 			m_pcPic4DLFMISCL3;
  TComPicYuv* 			m_pcPic4DLFMISCL7;
  TComPicYuv* 			m_pcPic4DLFMISCL13;
#endif
#endif
  TComPicYuv*           m_pcPicYuvPred;           //  Prediction
  TComPicYuv*           m_pcPicYuvResi;           //  Residual

  Bool                  m_bReconstructed;
  Bool                  m_bNeededForOutput;
  UInt                  m_uiCurrSliceIdx;         // Index of current slice
  Bool                  m_bCheckLTMSB;

  Bool                  m_isTop;
  Bool                  m_isField;

  std::vector<std::vector<TComDataCU*> > m_vSliceCUDataLink;

  SEIMessages  m_SEIs; ///< Any SEI messages that have been received.  If !NULL we own the object.

public:
  TComPic();
  virtual ~TComPic();

  Void          create( const TComSPS &sps, const TComPPS &pps, const Bool bIsVirtual /*= false*/ );

  virtual Void  destroy();

  UInt          getTLayer() const               { return m_uiTLayer;   }
  Void          setTLayer( UInt uiTLayer ) { m_uiTLayer = uiTLayer; }

  Bool          getUsedByCurr() const            { return m_bUsedByCurr; }
  Void          setUsedByCurr( Bool bUsed ) { m_bUsedByCurr = bUsed; }
  Bool          getIsLongTerm() const            { return m_bIsLongTerm; }
  Void          setIsLongTerm( Bool lt ) { m_bIsLongTerm = lt; }
  Void          setCheckLTMSBPresent     (Bool b ) {m_bCheckLTMSB=b;}
  Bool          getCheckLTMSBPresent     () { return m_bCheckLTMSB;}

  TComPicSym*   getPicSym()                        { return  &m_picSym;    }
  const TComPicSym* getPicSym() const              { return  &m_picSym;    }
  TComSlice*    getSlice(Int i)                    { return  m_picSym.getSlice(i);  }
  const TComSlice* getSlice(Int i) const           { return  m_picSym.getSlice(i);  }
  Int           getPOC() const                     { return  m_picSym.getSlice(m_uiCurrSliceIdx)->getPOC();  }
  TComDataCU*   getCtu( UInt ctuRsAddr )           { return  m_picSym.getCtu( ctuRsAddr ); }
  const TComDataCU* getCtu( UInt ctuRsAddr ) const { return  m_picSym.getCtu( ctuRsAddr ); }

  TComPicYuv*   getPicYuvOrg()        { return  m_apcPicYuv[PIC_YUV_ORG]; }
  TComPicYuv*   getPicYuvRec()        { return  m_apcPicYuv[PIC_YUV_REC]; }
#if RM_4DLF_MI_BUFFER
  TComPicYuv*   getPicYuv4DLFMI()       				{ return  m_pcPic4DLFMI; }
  Void          setPicYuv4DLFMI( TComPicYuv* pcPicYuv ) { m_pcPic4DLFMI = pcPicYuv; }
  UInt   		getMicroImageSize()       				{ return  m_uiMicroImageSize; }
  Void          setMicroImageSize( UInt uiMISize ) { m_uiMicroImageSize = uiMISize; }
  UInt   		getTotalNumberOfSAIs()       				{ return  m_uiTotalNumberOfSAIs; }
  Void          setTotalNumberOfSAIs( UInt uiNumOfSAIs ) { m_uiTotalNumberOfSAIs = uiNumOfSAIs; }
  UInt   		getCurrentSAIsSpiralPosX()       				{ return  m_uiCurrentSAIsSpiralPosX; }
  Void          setCurrentSAIsSpiralPosX( UInt posX ) { m_uiCurrentSAIsSpiralPosX = posX; }
  UInt   		getCurrentSAIsSpiralPosY()       				{ return  m_uiCurrentSAIsSpiralPosY; }
  Void          setCurrentSAIsSpiralPosY( UInt posY ) { m_uiCurrentSAIsSpiralPosY = posY; }
  Bool 			writePlane(ostream& fd, Pel* src, Bool is16bit, UInt stride444, UInt width444, UInt height444, ComponentID compID, ChromaFormat srcFormat, ChromaFormat fileFormat, UInt fileBitDepth);
  Void 			spiral(UInt idx, UInt size, UInt* x, UInt* y);
  Void          setCurrentSAI( UInt sai ) { m_uiCurrentSAI = sai; }
  UInt   		getCurrentSAI()       		{ return  m_uiCurrentSAI; }
#if RM_4DLF_SAI_BUFFER
  TComPicYuv*   getPicYuv4DLFSAI()       				{ return  m_pcPic4DLFSAI; }
  Void        	setPicYuv4DLFSAI( TComPicYuv* pcPicYuv ) { m_pcPic4DLFSAI = pcPicYuv; }
#endif
#if RM_SCALABLE
  Void 			spiralScalable(UInt idx, UInt size, UInt* x, UInt* y);
  Void 			genIntermediarySAI7x7(TComPicYuv* pcPic4DLFMISCL7, UInt miSize);
  Void 			genIntermediarySAI13x13(TComPicYuv* pcPic4DLFMISCL13, UInt miSize);
  Void 			disparityCompensationSAI(Pel* SAI0, Pel* SAI1, Pel *disparityMapX, Pel *disparityMapY, UInt W, UInt H, Int searchWindow[2][2]);
  Void 			compensateSAI(Pel* SAI0, Pel *disparityMapX, Pel *disparityMapY, Pel *outputSAI, UInt W, UInt H);
  Void 			getSAIFrom4DLFMI(TComPicYuv* pcPic4DLFMI, Pel* SAI, UInt miSize, UInt W, UInt H, UInt xPos, UInt yPos, ComponentID id);
  TComPicYuv*   getPicYuv4DLFMISCL3()       				{ return  m_pcPic4DLFMISCL3; }
  Void        	setPicYuv4DLFMISCL3( TComPicYuv* pcPicYuv ) { m_pcPic4DLFMISCL3 = pcPicYuv; }
  TComPicYuv*   getPicYuv4DLFMISCL7()       				{ return  m_pcPic4DLFMISCL7; }
  Void        	setPicYuv4DLFMISCL7( TComPicYuv* pcPicYuv ) { m_pcPic4DLFMISCL7 = pcPicYuv; }
  TComPicYuv*   getPicYuv4DLFMISCL13()       				{ return  m_pcPic4DLFMISCL13; }
  Void        	setPicYuv4DLFMISCL13( TComPicYuv* pcPicYuv ) { m_pcPic4DLFMISCL13 = pcPicYuv; }
#endif
#endif
  TComPicYuv*   getPicYuvPred()       { return  m_pcPicYuvPred; }
  TComPicYuv*   getPicYuvResi()       { return  m_pcPicYuvResi; }
  Void          setPicYuvPred( TComPicYuv* pcPicYuv )       { m_pcPicYuvPred = pcPicYuv; }
  Void          setPicYuvResi( TComPicYuv* pcPicYuv )       { m_pcPicYuvResi = pcPicYuv; }

  UInt          getNumberOfCtusInFrame() const     { return m_picSym.getNumberOfCtusInFrame(); }
  UInt          getNumPartInCtuWidth() const       { return m_picSym.getNumPartInCtuWidth();   }
  UInt          getNumPartInCtuHeight() const      { return m_picSym.getNumPartInCtuHeight();  }
  UInt          getNumPartitionsInCtu() const      { return m_picSym.getNumPartitionsInCtu();  }
  UInt          getFrameWidthInCtus() const        { return m_picSym.getFrameWidthInCtus();    }
  UInt          getFrameHeightInCtus() const       { return m_picSym.getFrameHeightInCtus();   }
  UInt          getMinCUWidth() const              { return m_picSym.getMinCUWidth();          }
  UInt          getMinCUHeight() const             { return m_picSym.getMinCUHeight();         }

  Int           getStride(const ComponentID id) const          { return m_apcPicYuv[PIC_YUV_REC]->getStride(id); }
  Int           getComponentScaleX(const ComponentID id) const    { return m_apcPicYuv[PIC_YUV_REC]->getComponentScaleX(id); }
  Int           getComponentScaleY(const ComponentID id) const    { return m_apcPicYuv[PIC_YUV_REC]->getComponentScaleY(id); }
  ChromaFormat  getChromaFormat() const                           { return m_apcPicYuv[PIC_YUV_REC]->getChromaFormat(); }
  Int           getNumberValidComponents() const                  { return m_apcPicYuv[PIC_YUV_REC]->getNumberValidComponents(); }

  Void          setReconMark (Bool b) { m_bReconstructed = b;     }
  Bool          getReconMark () const      { return m_bReconstructed;  }
  Void          setOutputMark (Bool b) { m_bNeededForOutput = b;     }
  Bool          getOutputMark () const      { return m_bNeededForOutput;  }

  Void          compressMotion();
  UInt          getCurrSliceIdx() const           { return m_uiCurrSliceIdx;                }
  Void          setCurrSliceIdx(UInt i)      { m_uiCurrSliceIdx = i;                   }
  UInt          getNumAllocatedSlice() const      {return m_picSym.getNumAllocatedSlice();}
  Void          allocateNewSlice()           {m_picSym.allocateNewSlice();         }
  Void          clearSliceBuffer()           {m_picSym.clearSliceBuffer();         }

  const Window& getConformanceWindow() const { return m_picSym.getSPS().getConformanceWindow(); }
  Window        getDefDisplayWindow() const  { return m_picSym.getSPS().getVuiParametersPresentFlag() ? m_picSym.getSPS().getVuiParameters()->getDefaultDisplayWindow() : Window(); }

  Bool          getSAOMergeAvailability(Int currAddr, Int mergeAddr);

  UInt          getSubstreamForCtuAddr(const UInt ctuAddr, const Bool bAddressInRaster, TComSlice *pcSlice);

  /* field coding parameters*/

   Void              setTopField(Bool b)                  {m_isTop = b;}
   Bool              isTopField()                         {return m_isTop;}
   Void              setField(Bool b)                     {m_isField = b;}
   Bool              isField()                            {return m_isField;}

  /** transfer ownership of seis to this picture */
  Void setSEIs(SEIMessages& seis) { m_SEIs = seis; }

  /**
   * return the current list of SEI messages associated with this picture.
   * Pointer is valid until this->destroy() is called */
  SEIMessages& getSEIs() { return m_SEIs; }

  /**
   * return the current list of SEI messages associated with this picture.
   * Pointer is valid until this->destroy() is called */
  const SEIMessages& getSEIs() const { return m_SEIs; }
};// END CLASS DEFINITION TComPic

//! \}

#endif // __TCOMPIC__
