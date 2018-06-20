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

/** \file     TComPrediction.h
    \brief    prediction class (header)
*/

#ifndef __TCOMPREDICTION__
#define __TCOMPREDICTION__


// Include files
#include "TComYuv.h"
#include "TComInterpolationFilter.h"
#include "TComWeightPrediction.h"

// forward declaration
class TComMv;
class TComTU; 

//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// prediction class
typedef enum PRED_BUF_E
{
  PRED_BUF_UNFILTERED=0,
  PRED_BUF_FILTERED=1,
  NUM_PRED_BUF=2
} PRED_BUF;

static const UInt MAX_INTRA_FILTER_DEPTHS=5;

class TComPrediction : public TComWeightPrediction
{
private:
  static const UChar m_aucIntraFilter[MAX_NUM_CHANNEL_TYPE][MAX_INTRA_FILTER_DEPTHS];

protected:
  Pel*      m_piYuvExt[MAX_NUM_COMPONENT][NUM_PRED_BUF];
  Int       m_iYuvExtSize;

  TComYuv   m_acYuvPred[NUM_REF_PIC_LIST_01];
  TComYuv   m_cYuvPredTemp;
  TComYuv m_filteredBlock[LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS];
  TComYuv m_filteredBlockTmp[LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS];

  TComInterpolationFilter m_if;

  Pel*   m_pLumaRecBuffer;       ///< array for downsampled reconstructed luma sample
  Int    m_iLumaRecStride;       ///< stride of #m_pLumaRecBuffer array

  Void xPredIntraAng            ( Int bitDepth, const Pel* pSrc, Int srcStride, Pel* pDst, Int dstStride, UInt width, UInt height, ChannelType channelType, UInt dirMode, const Bool bEnableEdgeFilters );
#if RM_4DLF_MI_INTRA_MODE_DC_3x3
  Void xPred4DLFMI_DC_3x3       ( Int bitDepth, const Pel* pSrc, Int srcStride, Pel* pTrueDst, Int dstStrideTrue, UInt uiWidth, UInt uiHeight, ChannelType channelType, UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize, UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY, UInt totalNumberOfSAIs, UInt uiAbsPartIdxInRaster, UInt uiPosX, UInt uiPosY, ComponentID compID );
#endif
#if RM_4DLF_MI_INTRA_MODE_LOCO_I
  Void xPred4DLFMI_LOCO_I       ( Int bitDepth, const Pel* pSrc, Int srcStride, Pel* pTrueDst, Int dstStrideTrue, UInt uiWidth, UInt uiHeight, ChannelType channelType, UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize, UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY, UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster, UInt uiPosX, UInt uiPosY, ComponentID compID );
  Int  median_3x3				( Pel* p4DLFMI, UInt const current_pixel_pos, UInt const stride );
#endif
#if RM_4DLF_MI_INTRA_MODE_GAP
  Void xPred4DLFMI_GAP       ( Int bitDepth, const Pel* pSrc, Int srcStride, Pel* pTrueDst, Int dstStrideTrue, UInt uiWidth, UInt uiHeight, ChannelType channelType, UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize, UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY, UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster, UInt uiPosX, UInt uiPosY, ComponentID compID );
#endif
#if RM_4DLF_MI_INTRA_MODE_AGSP
  Void xPred4DLFMI_AGSP       ( Int bitDepth, const Pel* pSrc, Int srcStride, Pel* pTrueDst, Int dstStrideTrue, UInt uiWidth, UInt uiHeight, ChannelType channelType, UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize, UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY, UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster, UInt uiPosX, UInt uiPosY, ComponentID compID );
#endif
#if RM_4DLF_MI_INTRA_MODE_LSP
  Void xPred4DLFMI_LSP       ( Int bitDepth, const Pel* pSrc, Int srcStride, Pel* pTrueDst, Int dstStrideTrue, UInt uiWidth, UInt uiHeight, ChannelType channelType, UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize, UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY, UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster, UInt uiPosX, UInt uiPosY, ComponentID compID );
  Double* trainSpiralLSP( Int current_SAI, Int total_number_of_SAIS, Pel* p4DLFMI, Int const origin_pixel_pos, Int const current_pixel_pos, Int const stride );
  Double* trainRasterLSP( Int* causalSupportX, Int* causalSupportY, Int current_SAI, Int miSize, Pel* p4DLFMI, Int const origin_pixel_pos, Int const current_pixel_pos, Int const stride, Int W, Int H );
  Void leastSquares( Double **C, Double *y, Double *a, Int m, Int n );
  UInt LSP( Double *lspCoefs, Int M, Pel* p4DLFMI, Int current_SAI, Int total_number_of_SAIS, Int current_pixel_pos, Int stride);
  UInt LSP3( Double *lspCoefs, Int M, Pel* p4DLFMI, Int current_SAI, Int total_number_of_SAIS, Int current_pixel_pos, Int stride);;
  UInt LSPM( Int* causalSupportX, Int* causalSupportY, Double *lspCoefs, Int M, Pel* p4DLFMI, Int current_SAI, Int mi, Int current_pixel_pos, Int stride, Int bit_depth);
#define TINY 1.0e-20
  Int ludcmp(Double **a, Int n, Int *indx, Double *d);
  Void lubksb(Double **a, Int n, Int *indx, Double b[]);
  double *doublevector(long n);
  double **doublematrix(long nr, long nc);
  void free_doublematrix(double **matrix, int nr, int nc);
#endif
#if RM_4DLF_MI_BUFFER
  Void getCausalSupportFromSpiral_AGSP( Int* w, Int* ww, Int* n, Int* nn, Int* nw, Int* ne, Int* nne, Int* nww, Int* nnw, Int current_SAI, Int total_number_of_SAIS, Pel* p4DLFMI, Int const current_pixel_pos, Int const stride );
  Int AGSP( Int w, Int ww, Int n, Int nn, Int nw, Int ne, Int nee, Int nww, Int nnw );
  Void getCausalSupportFromSpiral_GAP( Int* w, Int* ww, Int* n, Int* nn, Int* nw, Int* ne, Int *nee, Int current_SAI, Int total_number_of_SAIS, Pel* p4DLFMI, Int const current_pixel_pos, Int const stride );
  Int GAP( Int w, Int ww, Int n, Int nn, Int nw, Int ne, Int nee );
  Void getCausalSupportFromSpiral_LOCO_I( Int* a, Int* b, Int* c, Int current_SAI, Int total_number_of_SAIS, Pel* p4DLFMI, Int const current_pixel_pos, Int const stride );
  Int  LOCO_I					( Int a, Int b, Int c );
  Int spiral					(UInt idx, UInt size, UInt* x, UInt* y);
  Int getCausalSupportAdaptive( Int M, Int MExt, Int* causalSupportX, Int* causalSupportY, Int currentSAI, Int origin_pixel_pos_MI, Int current_pixel_pos, Int mi, Int stride );
#endif
  Void xPredIntraPlanar         ( const Pel* pSrc, Int srcStride, Pel* rpDst, Int dstStride, UInt width, UInt height );

  // motion compensation functions
  Void xPredInterUni            ( TComDataCU* pcCU,                          UInt uiPartAddr,               Int iWidth, Int iHeight, RefPicList eRefPicList, TComYuv* pcYuvPred, Bool bi=false          );
  Void xPredInterBi             ( TComDataCU* pcCU,                          UInt uiPartAddr,               Int iWidth, Int iHeight,                         TComYuv* pcYuvPred          );
  Void xPredInterBlk(const ComponentID compID, TComDataCU *cu, TComPicYuv *refPic, UInt partAddr, TComMv *mv, Int width, Int height, TComYuv *dstPic, Bool bi, const Int bitDepth );
  Void xWeightedAverage         ( TComYuv* pcYuvSrc0, TComYuv* pcYuvSrc1, Int iRefIdx0, Int iRefIdx1, UInt uiPartAddr, Int iWidth, Int iHeight, TComYuv* pcYuvDst, const BitDepths &clipBitDepths  );

  Void xGetLLSPrediction ( const Pel* pSrc0, Int iSrcStride, Pel* pDst0, Int iDstStride, UInt uiWidth, UInt uiHeight, UInt uiExt0, const ChromaFormat chFmt  DEBUG_STRING_FN_DECLARE(sDebug) );

  Void xDCPredFiltering( const Pel* pSrc, Int iSrcStride, Pel* pDst, Int iDstStride, Int iWidth, Int iHeight, ChannelType channelType );
  Bool xCheckIdenticalMotion    ( TComDataCU* pcCU, UInt PartAddr);
  Void destroy();

public:
  TComPrediction();
  virtual ~TComPrediction();

  Void    initTempBuff(ChromaFormat chromaFormatIDC);

  ChromaFormat getChromaFormat() const { return m_cYuvPredTemp.getChromaFormat(); }

  // inter
  Void motionCompensation         ( TComDataCU*  pcCU, TComYuv* pcYuvPred, RefPicList eRefPicList = REF_PIC_LIST_X, Int iPartIdx = -1 );

  // motion vector prediction
  Void getMvPredAMVP              ( TComDataCU* pcCU, UInt uiPartIdx, UInt uiPartAddr, RefPicList eRefPicList, TComMv& rcMvPred );

  // Angular Intra
  Void predIntraAng               ( const ComponentID compID, UInt uiDirMode, Pel *piOrg /* Will be null for decoding */, UInt uiOrgStride, Pel* piPred, UInt uiStride, TComTU &rTu, const Bool bUseFilteredPredSamples, const Bool bUseLosslessDPCM = false );

  Pel  predIntraGetPredValDC      ( const Pel* pSrc, Int iSrcStride, UInt iWidth, UInt iHeight);

  Pel*  getPredictorPtr           ( const ComponentID compID, const Bool bUseFilteredPredictions )
  {
    return m_piYuvExt[compID][bUseFilteredPredictions?PRED_BUF_FILTERED:PRED_BUF_UNFILTERED];
  }

  // This function is actually still in TComPattern.cpp
  /// set parameters from CU data for accessing intra data
  Void initIntraPatternChType ( TComTU &rTu,
                              const ComponentID compID, const Bool bFilterRefSamples
                              DEBUG_STRING_FN_DECLARE(sDebug)
                              );

  static Bool filteringIntraReferenceSamples(const ComponentID compID, UInt uiDirMode, UInt uiTuChWidth, UInt uiTuChHeight, const ChromaFormat chFmt, const Bool intraReferenceSmoothingDisabled);

  static Bool UseDPCMForFirstPassIntraEstimation(TComTU &rTu, const UInt uiDirMode);
};

//! \}

#endif // __TCOMPREDICTION__
