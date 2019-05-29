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

/** \file     TComPrediction.cpp
    \brief    prediction class
*/

#include <memory.h>
#include "TComPrediction.h"
#include "TComPic.h"
#include "TComTU.h"

//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Tables
// ====================================================================================================================

#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE > 0
const Bool TComPrediction::m_2WayRAProfileLF3[2][3][3] = {
							// N
							{
									{1, 1, 1},
									{1, 1, 1},
									{0, 0, 0}
							},
							// S
							{
									{0, 0, 0},
									{1, 1, 1},
									{1, 1, 1}
							},
};
const Bool TComPrediction::m_2WayRAProfileLF7[2][7][7] = {
							// N
							{
									{1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// S
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1}
							},
};
const Bool TComPrediction::m_2WayRAProfileLF13[2][13][13] = {
							// N
							{
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// S
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
									{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
							},
};

const Bool TComPrediction::m_4WayRAProfileLF3[4][3][3] = {
							// NW
							{
									{1, 1, 0},
									{1, 1, 0},
									{0, 0, 0}
							},
							// NE
							{
									{0, 1, 1},
									{0, 1, 1},
									{0, 0, 0}
							},
							// SW
							{
									{0, 0, 0},
									{1, 1, 0},
									{1, 1, 0}
							},
							// SE
							{
									{0, 0, 0},
									{0, 1, 1},
									{0, 1, 1}
							}
};
const Bool TComPrediction::m_4WayRAProfileLF7[4][7][7] = {
							// NW
							{
									{1, 1, 1, 1, 0, 0, 0},
									{1, 1, 1, 1, 0, 0, 0},
									{1, 1, 1, 1, 0, 0, 0},
									{1, 1, 1, 1, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// NE
							{
									{0, 0, 0, 1, 1, 1, 1},
									{0, 0, 0, 1, 1, 1, 1},
									{0, 0, 0, 1, 1, 1, 1},
									{0, 0, 0, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// SW
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 0, 0, 0},
									{1, 1, 1, 1, 0, 0, 0},
									{1, 1, 1, 1, 0, 0, 0},
									{1, 1, 1, 1, 0, 0, 0}
							},
							// SE
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 1, 1, 1, 1},
									{0, 0, 0, 1, 1, 1, 1},
									{0, 0, 0, 1, 1, 1, 1},
									{0, 0, 0, 1, 1, 1, 1}
							}
};
const Bool TComPrediction::m_4WayRAProfileLF13[4][13][13] = {
							// NW
							{
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// NE
							{
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// SW
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}
							},
							// SE
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1}
							}
};

const Bool TComPrediction::m_9WayRAProfileLF3[9][3][3] = {
							// NW
							{
									{1, 0, 0},
									{0, 0, 0},
									{0, 0, 0}
							},
							// N
							{
									{0, 1, 0},
									{0, 0, 0},
									{0, 0, 0}
							},
							// NE
							{
									{0, 0, 1},
									{0, 0, 0},
									{0, 0, 0}
							},
							// W
							{
									{0, 0, 0},
									{1, 0, 0},
									{0, 0, 0}
							},
							// C
							{
									{0, 0, 0},
									{0, 1, 0},
									{0, 0, 0}
							},
							// E
							{
									{0, 0, 0},
									{0, 0, 1},
									{0, 0, 0}
							},
							// SW
							{
									{0, 0, 0},
									{0, 0, 0},
									{1, 0, 0}
							},
							// S
							{
									{0, 0, 0},
									{0, 0, 0},
									{0, 1, 0}
							},
							// SE
							{
									{0, 0, 0},
									{0, 0, 0},
									{0, 0, 1}
							}

};
const Bool TComPrediction::m_9WayRAProfileLF7[9][7][7] = {
							// NW
							{
									{1, 1, 1, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// N
							{
									{0, 0, 1, 1, 1, 0, 0},
									{0, 0, 1, 1, 1, 0, 0},
									{0, 0, 1, 1, 1, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// NE
							{
									{0, 0, 0, 0, 1, 1, 1},
									{0, 0, 0, 0, 1, 1, 1},
									{0, 0, 0, 0, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// W
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// C
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 1, 1, 1, 0, 0},
									{0, 0, 1, 1, 1, 0, 0},
									{0, 0, 1, 1, 1, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// E
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1},
									{0, 0, 0, 0, 1, 1, 1},
									{0, 0, 0, 0, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0}
							},
							// SW
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0},
									{1, 1, 1, 0, 0, 0, 0}
							},
							// S
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 1, 1, 1, 0, 0},
									{0, 0, 1, 1, 1, 0, 0},
									{0, 0, 1, 1, 1, 0, 0}
							},
							// SE
							{
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1},
									{0, 0, 0, 0, 1, 1, 1},
									{0, 0, 0, 0, 1, 1, 1}
							}

};
const Bool TComPrediction::m_9WayRAProfileLF13[9][13][13] = {
							// NW
							{
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// N
							{
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// NE
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// W
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// C
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// E
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// SW
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
									{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}
							},
							// S
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
									{0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0}
							},
							// SE
							{
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
									{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1}
							}

};
#endif

const UChar TComPrediction::m_aucIntraFilter[MAX_NUM_CHANNEL_TYPE][MAX_INTRA_FILTER_DEPTHS] =
{
  { // Luma
    10, //4x4
    7, //8x8
    1, //16x16
    0, //32x32
    10, //64x64
  },
  { // Chroma
    10, //4xn
    7, //8xn
    1, //16xn
    0, //32xn
    10, //64xn
  }

};

// ====================================================================================================================
// Constructor / destructor / initialize
// ====================================================================================================================

TComPrediction::TComPrediction()
: m_pLumaRecBuffer(0)
, m_iLumaRecStride(0)
{
  for(UInt ch=0; ch<MAX_NUM_COMPONENT; ch++)
  {
    for(UInt buf=0; buf<2; buf++)
    {
      m_piYuvExt[ch][buf] = NULL;
    }
  }
}

TComPrediction::~TComPrediction()
{
  destroy();
}

Void TComPrediction::destroy()
{
  for(UInt ch=0; ch<MAX_NUM_COMPONENT; ch++)
  {
    for(UInt buf=0; buf<NUM_PRED_BUF; buf++)
    {
      delete [] m_piYuvExt[ch][buf];
      m_piYuvExt[ch][buf] = NULL;
    }
  }

  for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
  {
    m_acYuvPred[i].destroy();
  }

  m_cYuvPredTemp.destroy();

  if( m_pLumaRecBuffer )
  {
    delete [] m_pLumaRecBuffer;
    m_pLumaRecBuffer = 0;
  }
  m_iLumaRecStride = 0;

  for (UInt i = 0; i < LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS; i++)
  {
    for (UInt j = 0; j < LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS; j++)
    {
      m_filteredBlock[i][j].destroy();
    }
    m_filteredBlockTmp[i].destroy();
  }
}

Void TComPrediction::initTempBuff(ChromaFormat chromaFormatIDC)
{
  // if it has been initialised before, but the chroma format has changed, release the memory and start again.
  if( m_piYuvExt[COMPONENT_Y][PRED_BUF_UNFILTERED] != NULL && m_cYuvPredTemp.getChromaFormat()!=chromaFormatIDC)
  {
    destroy();
  }

  if( m_piYuvExt[COMPONENT_Y][PRED_BUF_UNFILTERED] == NULL ) // check if first is null (in which case, nothing initialised yet)
  {
    Int extWidth  = MAX_CU_SIZE + 16;
    Int extHeight = MAX_CU_SIZE + 1;

    for (UInt i = 0; i < LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS; i++)
    {
      m_filteredBlockTmp[i].create(extWidth, extHeight + 7, chromaFormatIDC);
      for (UInt j = 0; j < LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS; j++)
      {
        m_filteredBlock[i][j].create(extWidth, extHeight, chromaFormatIDC);
      }
    }

    m_iYuvExtSize = (MAX_CU_SIZE*2+1) * (MAX_CU_SIZE*2+1);
    for(UInt ch=0; ch<MAX_NUM_COMPONENT; ch++)
    {
      for(UInt buf=0; buf<NUM_PRED_BUF; buf++)
      {
        m_piYuvExt[ch][buf] = new Pel[ m_iYuvExtSize ];
      }
    }

    // new structure
    for(UInt i=0; i<NUM_REF_PIC_LIST_01; i++)
    {
      m_acYuvPred[i] .create( MAX_CU_SIZE, MAX_CU_SIZE, chromaFormatIDC );
    }

    m_cYuvPredTemp.create( MAX_CU_SIZE, MAX_CU_SIZE, chromaFormatIDC );
  }


  if (m_iLumaRecStride != (MAX_CU_SIZE>>1) + 1)
  {
    m_iLumaRecStride =  (MAX_CU_SIZE>>1) + 1;
    if (!m_pLumaRecBuffer)
    {
      m_pLumaRecBuffer = new Pel[ m_iLumaRecStride * m_iLumaRecStride ];
    }
  }
}

// ====================================================================================================================
// Public member functions
// ====================================================================================================================

// Function for calculating DC value of the reference samples used in Intra prediction
//NOTE: Bit-Limit - 25-bit source
Pel TComPrediction::predIntraGetPredValDC( const Pel* pSrc, Int iSrcStride, UInt iWidth, UInt iHeight)
{
  assert(iWidth > 0 && iHeight > 0);
  Int iInd, iSum = 0;
  Pel pDcVal;

  for (iInd = 0;iInd < iWidth;iInd++)
  {
    iSum += pSrc[iInd-iSrcStride];
  }
  for (iInd = 0;iInd < iHeight;iInd++)
  {
    iSum += pSrc[iInd*iSrcStride-1];
  }

  pDcVal = (iSum + iWidth) / (iWidth + iHeight);

  return pDcVal;
}

// Function for deriving the angular Intra predictions

/** Function for deriving the simplified angular intra predictions.
 * \param bitDepth           bit depth
 * \param pSrc               pointer to reconstructed sample array
 * \param srcStride          the stride of the reconstructed sample array
 * \param pTrueDst           reference to pointer for the prediction sample array
 * \param dstStrideTrue      the stride of the prediction sample array
 * \param uiWidth            the width of the block
 * \param uiHeight           the height of the block
 * \param channelType        type of pel array (luma/chroma)
 * \param format             chroma format
 * \param dirMode            the intra prediction mode index
 * \param blkAboveAvailable  boolean indication if the block above is available
 * \param blkLeftAvailable   boolean indication if the block to the left is available
 * \param bEnableEdgeFilters indication whether to enable edge filters
 *
 * This function derives the prediction samples for the angular mode based on the prediction direction indicated by
 * the prediction mode index. The prediction direction is given by the displacement of the bottom row of the block and
 * the reference row above the block in the case of vertical prediction or displacement of the rightmost column
 * of the block and reference column left from the block in the case of the horizontal prediction. The displacement
 * is signalled at 1/32 pixel accuracy. When projection of the predicted pixel falls inbetween reference samples,
 * the predicted value for the pixel is linearly interpolated from the reference samples. All reference samples are taken
 * from the extended main reference.
 */
//NOTE: Bit-Limit - 25-bit source
Void TComPrediction::xPredIntraAng(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, const Bool bEnableEdgeFilters
                                  )
{
  Int width=Int(uiWidth);
  Int height=Int(uiHeight);

  // Map the mode index to main prediction direction and angle
  assert( dirMode != PLANAR_IDX ); //no planar
  const Bool modeDC        = dirMode==DC_IDX;

  // Do the DC prediction
  if (modeDC)
  {
    const Pel dcval = predIntraGetPredValDC(pSrc, srcStride, width, height);

    for (Int y=height;y>0;y--, pTrueDst+=dstStrideTrue)
    {
      for (Int x=0; x<width;) // width is always a multiple of 4.
      {
        pTrueDst[x++] = dcval;
      }
    }
  }
  else // Do angular predictions
  {
    const Bool       bIsModeVer         = (dirMode >= 18);
    const Int        intraPredAngleMode = (bIsModeVer) ? (Int)dirMode - VER_IDX :  -((Int)dirMode - HOR_IDX);
    const Int        absAngMode         = abs(intraPredAngleMode);
    const Int        signAng            = intraPredAngleMode < 0 ? -1 : 1;
    const Bool       edgeFilter         = bEnableEdgeFilters && isLuma(channelType) && (width <= MAXIMUM_INTRA_FILTERED_WIDTH) && (height <= MAXIMUM_INTRA_FILTERED_HEIGHT);

    // Set bitshifts and scale the angle parameter to block size
    static const Int angTable[9]    = {0,    2,    5,   9,  13,  17,  21,  26,  32};
    static const Int invAngTable[9] = {0, 4096, 1638, 910, 630, 482, 390, 315, 256}; // (256 * 32) / Angle
    Int invAngle                    = invAngTable[absAngMode];
    Int absAng                      = angTable[absAngMode];
    Int intraPredAngle              = signAng * absAng;

    Pel* refMain;
    Pel* refSide;

    Pel  refAbove[2*MAX_CU_SIZE+1];
    Pel  refLeft[2*MAX_CU_SIZE+1];

    // Initialize the Main and Left reference array.
    if (intraPredAngle < 0)
    {
      const Int refMainOffsetPreScale = (bIsModeVer ? height : width ) - 1;
      const Int refMainOffset         = height - 1;
      for (Int x=0;x<width+1;x++)
      {
        refAbove[x+refMainOffset] = pSrc[x-srcStride-1];
      }
      for (Int y=0;y<height+1;y++)
      {
        refLeft[y+refMainOffset] = pSrc[(y-1)*srcStride-1];
      }
      refMain = (bIsModeVer ? refAbove : refLeft)  + refMainOffset;
      refSide = (bIsModeVer ? refLeft  : refAbove) + refMainOffset;

      // Extend the Main reference to the left.
      Int invAngleSum    = 128;       // rounding for (shift by 8)
      for (Int k=-1; k>(refMainOffsetPreScale+1)*intraPredAngle>>5; k--)
      {
        invAngleSum += invAngle;
        refMain[k] = refSide[invAngleSum>>8];
      }
    }
    else
    {
      for (Int x=0;x<2*width+1;x++)
      {
        refAbove[x] = pSrc[x-srcStride-1];
      }
      for (Int y=0;y<2*height+1;y++)
      {
        refLeft[y] = pSrc[(y-1)*srcStride-1];
      }
      refMain = bIsModeVer ? refAbove : refLeft ;
      refSide = bIsModeVer ? refLeft  : refAbove;
    }

    // swap width/height if we are doing a horizontal mode:
    Pel tempArray[MAX_CU_SIZE*MAX_CU_SIZE];
    const Int dstStride = bIsModeVer ? dstStrideTrue : MAX_CU_SIZE;
    Pel *pDst = bIsModeVer ? pTrueDst : tempArray;
    if (!bIsModeVer)
    {
      std::swap(width, height);
    }

    if (intraPredAngle == 0)  // pure vertical or pure horizontal
    {
      for (Int y=0;y<height;y++)
      {
        for (Int x=0;x<width;x++)
        {
          pDst[y*dstStride+x] = refMain[x+1];
        }
      }

      if (edgeFilter)
      {
        for (Int y=0;y<height;y++)
        {
          pDst[y*dstStride] = Clip3 (0, ((1 << bitDepth) - 1), pDst[y*dstStride] + (( refSide[y+1] - refSide[0] ) >> 1) );
        }
      }
    }
    else
    {
      Pel *pDsty=pDst;

      for (Int y=0, deltaPos=intraPredAngle; y<height; y++, deltaPos+=intraPredAngle, pDsty+=dstStride)
      {
        const Int deltaInt   = deltaPos >> 5;
        const Int deltaFract = deltaPos & (32 - 1);

        if (deltaFract)
        {
          // Do linear filtering
          const Pel *pRM=refMain+deltaInt+1;
          Int lastRefMainPel=*pRM++;
          for (Int x=0;x<width;pRM++,x++)
          {
            Int thisRefMainPel=*pRM;
            pDsty[x+0] = (Pel) ( ((32-deltaFract)*lastRefMainPel + deltaFract*thisRefMainPel +16) >> 5 );
            lastRefMainPel=thisRefMainPel;
          }
        }
        else
        {
          // Just copy the integer samples
          for (Int x=0;x<width; x++)
          {
            pDsty[x] = refMain[x+deltaInt+1];
          }
        }
      }
    }

    // Flip the block if this is the horizontal mode
    if (!bIsModeVer)
    {
      for (Int y=0; y<height; y++)
      {
        for (Int x=0; x<width; x++)
        {
          pTrueDst[x*dstStrideTrue] = pDst[x];
        }
        pTrueDst++;
        pDst+=dstStride;
      }
    }
  }
}

#if RM_4DLF_MI_INTRA_MODE_DC_3x3
Void TComPrediction::xPred4DLFMI_DC_3x3(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{

	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	Int window = 1;
	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			UInt predictor = 0;
			UInt availablePixels = 0;
			if(currentSAI)
			{
				for (Int j=-window; j<=window; j++)
				{
					for (Int i=-window; i<=window; i++)
					{
						if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - 1 + x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - 1 + y*(Int)mi > 0) // if valid (inside frame)
						{
							if(p4DLFMI[(Int)firstPixelPos + i + x*mi + (j + y*mi)*(Int)ui4DLFMIStride] && !( j == i && j == 0 )) // if available
							{
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							if(xCheckIfInTheSameRegion(currentRegionRA, currentSAI, currentSAIsSpiralPosX+i, currentSAIsSpiralPosY+j))
							{
#endif
								predictor += p4DLFMI[(Int)firstPixelPos + i + x*mi + (j + y*mi)*(Int)ui4DLFMIStride];
								availablePixels++;
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							}
#endif
							}
						}
					}
				}
			}
			if(!availablePixels)
				availablePixels = 1;
			predictor /= availablePixels;
			pTrueDst[x + y*dstStrideTrue] = predictor;
		}
	}

}
#endif

#if RM_4DLF_MI_INTRA_MODE_LOCO_I
Void TComPrediction::xPred4DLFMI_LOCO_I(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{

	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int a = NOT_VALID;
	Int b = NOT_VALID;
	Int c = NOT_VALID;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			// firstPixelPosX = currentSAIsSpiralPosX + uiPosX * mi
			// firstPixelPosY = currentSAIsSpiralPosY + uiPosY * mi
			// currentPixelOffsetX = x*mi
			// currentPixelOffsetY = y*mi
			// -1 / -1 -> ULEFT pixel
			// ULEFT pixel = (firstPixelPosX + currentPixelPosX - 1, firstPixelPosY + currentPixelPosY - 1)
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - 1 + x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - 1 + y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				getCausalSupportFromSpiral_LOCO_I( &a, &b, &c, (Int)currentSAI, (Int)totalNumberOfSAIs, p4DLFMI, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
				);
				//a = p4DLFMI[firstPixelPos + (-1) + x*mi + (y*mi)*ui4DLFMIStride] ? p4DLFMI[firstPixelPos + (-1) + x*mi + (y*mi)*ui4DLFMIStride] : NOT_VALID; // Left
				//b = p4DLFMI[firstPixelPos + x*mi + ((-1) + y*mi)*ui4DLFMIStride] ? p4DLFMI[firstPixelPos + x*mi + ((-1) + y*mi)*ui4DLFMIStride] : NOT_VALID; // Up
				//c = p4DLFMI[firstPixelPos + (-1) + x*mi + ((-1) + y*mi)*ui4DLFMIStride] ? p4DLFMI[firstPixelPos + (-1) + x*mi + ((-1) + y*mi)*ui4DLFMIStride] : NOT_VALID; // ULeft

				if(a == NOT_VALID && b == NOT_VALID && c == NOT_VALID)
					predictor = 0;
				//else if(a == NOT_VALID || b == NOT_VALID || c == NOT_VALID) // median_3x3
					//predictor = median_3x3( p4DLFMI, firstPixelPos + x*mi + (y*mi)*ui4DLFMIStride, ui4DLFMIStride );
				else // LOCO-I
					predictor = LOCO_I( a, b, c );
			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}

}


Int TComPrediction::median_3x3( Pel* p4DLFMI, UInt const current_pixel_pos, UInt const stride )
{

	vector<UInt> mArray;

	// get all non zero elements
	for (Int j=-1; j<=1; j++)
		for (Int i=-1; i<=1; i++)
			if(p4DLFMI[current_pixel_pos + i + j * stride])
				mArray.push_back(p4DLFMI[current_pixel_pos + i + j * stride]);

	if(mArray.size())
	{
		if(mArray.size() > 1)
		{	// sort array & return median
			std::sort(mArray.begin(), mArray.end());
			return mArray.size() % 2 ? mArray[mArray.size() / 2] : (mArray[mArray.size() / 2 - 1] + mArray[mArray.size() / 2]) / 2;
		}
		else
			return mArray[0]; // if only one value that value is the median
	}
	else
		return 0; // if no values are larger than 0 than the median is 0
}

#endif

#if RM_4DLF_MI_INTRA_MODE_GAP
Void TComPrediction::xPred4DLFMI_GAP(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{

	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int w = NOT_VALID;
	Int ww = NOT_VALID;
	Int n = NOT_VALID;
	Int nn = NOT_VALID;
	Int nw = NOT_VALID;
	Int ne = NOT_VALID;
	Int nne = NOT_VALID;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			// firstPixelPosX = currentSAIsSpiralPosX + uiPosX * mi
			// firstPixelPosY = currentSAIsSpiralPosY + uiPosY * mi
			// currentPixelOffsetX = x*mi
			// currentPixelOffsetY = y*mi
			// -2 / -2 -> 2ULEFT pixel
			// 2ULEFT pixel = (firstPixelPosX + currentPixelPosX - 2, firstPixelPosY + currentPixelPosY - 2)
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - 2 + (Int)x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - 2 + (Int)y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				getCausalSupportFromSpiral_GAP( &w, &ww, &n, &nn, &nw, &ne, &nne, (Int)currentSAI, (Int)totalNumberOfSAIs, p4DLFMI, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
						,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
				);
				if(currentSAI < 9)// LOCO-I
				{
					getCausalSupportFromSpiral_LOCO_I( &w, &n, &nw, (Int)currentSAI, (Int)totalNumberOfSAIs, p4DLFMI, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
					);
					predictor = LOCO_I( w, n, nw );
				}
				else
					predictor = GAP( bitDepth, w, ww, n, nn, nw, ne, nne );
				if(w == NOT_VALID && ww == NOT_VALID && n == NOT_VALID && nn == NOT_VALID && nw == NOT_VALID && ne == NOT_VALID && nne == NOT_VALID)
					predictor = 0;

			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}

}
#endif
#if RM_4DLF_MI_INTRA_MODE_AGSP
Void TComPrediction::xPred4DLFMI_AGSP(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{

	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int w = NOT_VALID;
	Int ww = NOT_VALID;
	Int n = NOT_VALID;
	Int nn = NOT_VALID;
	Int nw = NOT_VALID;
	Int ne = NOT_VALID;
	Int nne = NOT_VALID;
	Int nww = NOT_VALID;
	Int nnw = NOT_VALID;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			// firstPixelPosX = currentSAIsSpiralPosX + uiPosX * mi
			// firstPixelPosY = currentSAIsSpiralPosY + uiPosY * mi
			// currentPixelOffsetX = x*mi
			// currentPixelOffsetY = y*mi
			// -2 / -2 -> 2ULEFT pixel
			// 2ULEFT pixel = (firstPixelPosX + currentPixelPosX - 2, firstPixelPosY + currentPixelPosY - 2)
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - 2 + (Int)x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - 2 + (Int)y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				getCausalSupportFromSpiral_AGSP( &w, &ww, &n, &nn, &nw, &ne, &nne, &nww, &nnw, (Int)currentSAI, (Int)totalNumberOfSAIs, p4DLFMI, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
						,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
				);
				if(currentSAI < 9)// LOCO-I
				{
					getCausalSupportFromSpiral_LOCO_I( &w, &n, &nw, (Int)currentSAI, (Int)totalNumberOfSAIs, p4DLFMI, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
					);
					predictor = LOCO_I( w, n, nw );
				}
				else
					predictor = AGSP( w, ww, n, nn, nw, ne, nne, nww, nnw );
				if(w == NOT_VALID && ww == NOT_VALID && n == NOT_VALID && nn == NOT_VALID && nw == NOT_VALID && ne == NOT_VALID && nne == NOT_VALID && nww == NOT_VALID && nnw == NOT_VALID )
					predictor = 0;

			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}

}
#endif
#if RM_4DLF_MI_INTRA_MODE_LSP
Void TComPrediction::xPred4DLFMI_LSP3(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{
	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int originMI = floor(mi/2);
	Double *lspCoefs;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	Int H = pcPic4DLFMI->getHeight(compID);
	Int W = pcPic4DLFMI->getWidth(compID);
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	UInt originPixelMI = originMI + uiPosX * mi + (originMI + uiPosY * mi) * ui4DLFMIStride;
	Int predOrder = 3;
	Int causalSupportX[predOrder];
	Int causalSupportY[predOrder];
	Int availablePixels = getCausalSupportAdaptive( predOrder, 0, causalSupportX, causalSupportY, (Int)currentSAI, (Int)originPixelMI, (Int)firstPixelPos, (Int)mi, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
			,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
	);
	Int pixelMargin = (predOrder + 1) / 2 + mi; // +mi ext training area

	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - pixelMargin + (Int)x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - pixelMargin + (Int)y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				if(currentSAI > availablePixels && availablePixels > 0 && currentSAI > predOrder)
				{
					lspCoefs = trainRasterLSP(causalSupportX, causalSupportY, (Int)currentSAI, (Int)mi, p4DLFMI, (Int)originPixelMI + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, W, H, predOrder, 0
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
					);
					predictor = LSPM( causalSupportX, causalSupportY, lspCoefs, predOrder, p4DLFMI, (Int)currentSAI, (Int)mi, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, bitDepth);
				}
			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}
}

Void TComPrediction::xPred4DLFMI_LSP5(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{
	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int originMI = floor(mi/2);
	Double *lspCoefs;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	Int H = pcPic4DLFMI->getHeight(compID);
	Int W = pcPic4DLFMI->getWidth(compID);
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	UInt originPixelMI = originMI + uiPosX * mi + (originMI + uiPosY * mi) * ui4DLFMIStride;
	Int predOrder = 5;
	Int causalSupportX[predOrder];
	Int causalSupportY[predOrder];
	Int availablePixels = getCausalSupportAdaptive( predOrder, 0, causalSupportX, causalSupportY, (Int)currentSAI, (Int)originPixelMI, (Int)firstPixelPos, (Int)mi, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
			,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
	);
	Int pixelMargin = (predOrder + 1) / 2 + mi; // +mi ext training area

	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - pixelMargin + (Int)x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - pixelMargin + (Int)y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				if(currentSAI > availablePixels && availablePixels > 0 && currentSAI > predOrder)
				{
					lspCoefs = trainRasterLSP(causalSupportX, causalSupportY, (Int)currentSAI, (Int)mi, p4DLFMI, (Int)originPixelMI + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, W, H, predOrder, 0
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
					);
					predictor = LSPM( causalSupportX, causalSupportY, lspCoefs, predOrder, p4DLFMI, (Int)currentSAI, (Int)mi, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, bitDepth);
				}
			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}
}

Void TComPrediction::xPred4DLFMI_LSP7(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{
	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int originMI = floor(mi/2);
	Double *lspCoefs;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	Int H = pcPic4DLFMI->getHeight(compID);
	Int W = pcPic4DLFMI->getWidth(compID);
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	UInt originPixelMI = originMI + uiPosX * mi + (originMI + uiPosY * mi) * ui4DLFMIStride;
	Int predOrder = 7;
	Int causalSupportX[predOrder];
	Int causalSupportY[predOrder];
	Int availablePixels = getCausalSupportAdaptive( predOrder, 0, causalSupportX, causalSupportY, (Int)currentSAI, (Int)originPixelMI, (Int)firstPixelPos, (Int)mi, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
			,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
	);
	Int pixelMargin = (predOrder + 1) / 2 + mi; // +mi ext training area

	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - pixelMargin + (Int)x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - pixelMargin + (Int)y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				if(currentSAI > availablePixels && availablePixels > 0 && currentSAI > predOrder)
				{
					lspCoefs = trainRasterLSP(causalSupportX, causalSupportY, (Int)currentSAI, (Int)mi, p4DLFMI, (Int)originPixelMI + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, W, H, predOrder, 0
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
					);
					predictor = LSPM( causalSupportX, causalSupportY, lspCoefs, predOrder, p4DLFMI, (Int)currentSAI, (Int)mi, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, bitDepth);
				}
			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}
}

Void TComPrediction::xPred4DLFMI_LSP9(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{
	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int originMI = floor(mi/2);
	Double *lspCoefs;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	Int H = pcPic4DLFMI->getHeight(compID);
	Int W = pcPic4DLFMI->getWidth(compID);
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	UInt originPixelMI = originMI + uiPosX * mi + (originMI + uiPosY * mi) * ui4DLFMIStride;
	Int predOrder = 9;
	Int causalSupportX[predOrder];
	Int causalSupportY[predOrder];
	Int availablePixels = getCausalSupportAdaptive( predOrder, 0, causalSupportX, causalSupportY, (Int)currentSAI, (Int)originPixelMI, (Int)firstPixelPos, (Int)mi, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
			,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
	);
	Int pixelMargin = (predOrder + 1) / 2 + mi; // +mi ext training area

	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - pixelMargin + (Int)x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - pixelMargin + (Int)y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				if(currentSAI > availablePixels && availablePixels > 0 && currentSAI > predOrder)
				{
					lspCoefs = trainRasterLSP(causalSupportX, causalSupportY, (Int)currentSAI, (Int)mi, p4DLFMI, (Int)originPixelMI + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, W, H, predOrder, 0
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
					);
					predictor = LSPM( causalSupportX, causalSupportY, lspCoefs, predOrder, p4DLFMI, (Int)currentSAI, (Int)mi, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, bitDepth);
				}
			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}
}

Void TComPrediction::xPred4DLFMI_LSPM(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID, Int predOrder
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  , Int currentRegionRA
#endif
)
{
	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int originMI = floor(mi/2);
	Double *lspCoefs;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	Int H = pcPic4DLFMI->getHeight(compID);
	Int W = pcPic4DLFMI->getWidth(compID);
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	UInt originPixelMI = originMI + uiPosX * mi + (originMI + uiPosY * mi) * ui4DLFMIStride;
	Int causalSupportX[predOrder];
	Int causalSupportY[predOrder];
	Int availablePixels = getCausalSupportAdaptive( predOrder, 0, causalSupportX, causalSupportY, (Int)currentSAI, (Int)originPixelMI, (Int)firstPixelPos, (Int)mi, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
			,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
	);
	Int pixelMargin = (predOrder + 1) / 2 + mi; // +mi ext training area

	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - pixelMargin + (Int)x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - pixelMargin + (Int)y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				if(currentSAI > availablePixels && availablePixels > 0 && currentSAI > predOrder)
				{
					lspCoefs = trainRasterLSP(causalSupportX, causalSupportY, (Int)currentSAI, (Int)mi, p4DLFMI, (Int)originPixelMI + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, W, H, predOrder, 0
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
					);
					predictor = LSPM( causalSupportX, causalSupportY, lspCoefs, predOrder, p4DLFMI, (Int)currentSAI, (Int)mi, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, bitDepth);
				}
			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}
}

Void TComPrediction::xPred4DLFMI_LSP(       Int bitDepth,
                                    const Pel* pSrc,     Int srcStride,
                                          Pel* pTrueDst, Int dstStrideTrue,
                                          UInt uiWidth, UInt uiHeight, ChannelType channelType,
                                          UInt dirMode, TComPicYuv *const pcPic4DLFMI, UInt miSize,
										  UInt currentSAIsSpiralPosX, UInt currentSAIsSpiralPosY,
										  UInt totalNumberOfSAIs, UInt currentSAI, UInt uiAbsPartIdxInRaster,
										  UInt uiPosX, UInt uiPosY, ComponentID compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentRegionRA
#endif
)
{
	Int width=Int(uiWidth);
	Int height=Int(uiHeight);
	Int mi=Int(miSize);
	Int originMI = floor(mi/2);
	Double *lspCoefs;
	UInt const ui4DLFMIStride = pcPic4DLFMI->getStride(compID);
	Pel* const p4DLFMI = pcPic4DLFMI->getAddr( compID );
	Int H = pcPic4DLFMI->getHeight(compID);
	Int W = pcPic4DLFMI->getWidth(compID);
	// first pixel location in the 4DLF MI buffer
	UInt firstPixelPos = currentSAIsSpiralPosX + uiPosX * mi + (currentSAIsSpiralPosY + uiPosY * mi ) * ui4DLFMIStride;
	UInt originPixelMI = originMI + uiPosX * mi + (originMI + uiPosY * mi) * ui4DLFMIStride;
	Int predOrder = RM_4DLF_MI_INTRA_MODE_LSP_PRED_ORDER;
	Int predOrderExt = RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT * RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_PRED_ORDER;
	Int causalSupportX[predOrder + predOrderExt];
	Int causalSupportY[predOrder + predOrderExt];
	Int availablePixels = getCausalSupportAdaptive( predOrder, predOrderExt, causalSupportX, causalSupportY, (Int)currentSAI, (Int)originPixelMI, (Int)firstPixelPos, (Int)mi, (Int)ui4DLFMIStride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
			,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
	);
	Int pixelMargin = (RM_4DLF_MI_INTRA_MODE_LSP_PRED_ORDER + 1) / 2;
#if RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT
	pixelMargin += mi;
#endif
#if RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_SEARCH // save original values
	Int causalSupportXOrg[predOrder + predOrderExt];
	Int causalSupportYOrg[predOrder + predOrderExt];
	for(Int m=0; m<predOrder + predOrderExt; m++)
	{
		causalSupportXOrg[m] = causalSupportX[m];
		causalSupportYOrg[m] = causalSupportY[m];
	}
#endif


	// Create individual pixel predictor for each pixel
	for (Int y=0; y<height; y++)
	{
		for (Int x=0; x<width; x++)
		{
			Int predictor = 0;
			if((Int)currentSAIsSpiralPosX + (Int)uiPosX * (Int)mi - pixelMargin + (Int)x*(Int)mi > 0 && (Int)currentSAIsSpiralPosY + (Int)uiPosY * (Int)mi - pixelMargin + (Int)y*(Int)mi > 0) // if ULeft pixel is inside frame
			{
				if(currentSAI > availablePixels && availablePixels > 0 && currentSAI > predOrder)
				{
#if RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_SEARCH
					for(Int m=0; m<predOrder + predOrderExt; m++) // restore the support to the default for each pixel
					{
						causalSupportX[m] = causalSupportXOrg[m];
						causalSupportY[m] = causalSupportYOrg[m];
					}
					if(currentSAI > RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_SEARCH_TEMPLATE_SIZE)
						extendedSupportSearch(p4DLFMI, predOrder, predOrderExt, causalSupportX, causalSupportY, (Int)currentSAI, (Int)originPixelMI, (Int)firstPixelPos, (Int)mi, (Int)ui4DLFMIStride );
#endif
					lspCoefs = trainRasterLSP(causalSupportX, causalSupportY, (Int)currentSAI, (Int)mi, p4DLFMI, (Int)originPixelMI + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, W, H, predOrder, predOrderExt
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							,currentSAIsSpiralPosX, currentSAIsSpiralPosY, currentRegionRA
#endif
					);
					predictor = LSPM( causalSupportX, causalSupportY, lspCoefs, predOrder + predOrderExt, p4DLFMI, (Int)currentSAI, (Int)mi, (Int)firstPixelPos + x*(Int)mi + (y*(Int)mi)*(Int)ui4DLFMIStride, (Int)ui4DLFMIStride, bitDepth);
				}
			}
			else
				predictor = 0;
			pTrueDst[x + y*dstStrideTrue] = UInt(predictor);
		}
	}
}

Double* TComPrediction::trainRasterLSP( Int* causalSupportX, Int* causalSupportY, Int current_SAI, Int miSize, Pel* p4DLFMI, Int const origin_pixel_pos, Int const current_pixel_pos, Int const stride, Int W, Int H, Int predOrder, Int predOrderExt
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentSAIsSpiralPosX, Int currentSAIsSpiralPosY, Int currentRegionRA
#endif
)
{
	Int dir_idx = 0;
	//char current_direction[] = "RDLU";
	UInt i=0, j=0;
	Int I=0, J=0;
	double *y, *yValid,*a,**C, **CValidt;
	//Int predOrder = RM_4DLF_MI_INTRA_MODE_LSP_PRED_ORDER;
	//Int predOrderExt = RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_PRED_ORDER * RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT;
	Int spiralOffset = floor(miSize/2); // relative spiral offset
	Int pixelOffset = 0;
	//Int supportPixelPosOffset[predOrder];
	Int validIdx = 0;
	Bool supportIncomplete = false;
	Int miOffset[9];

	a=doublevector(predOrder + predOrderExt);
	y=doublevector(current_SAI*(RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_TRAINING_AREA+1));
	C=doublematrix(current_SAI*(RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_TRAINING_AREA+1),predOrder + predOrderExt);

	miOffset[0] = 0; 				// Current
#if RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_TRAINING_AREA
	miOffset[1] = -miSize;			// LEFT
	miOffset[2] = -stride*miSize;	// UP
	miOffset[3] = miSize;			// RIGHT
	miOffset[4] = stride*miSize;	// DOWN
	miOffset[5] = -miSize -stride*miSize; // UL
	miOffset[6] = miSize -stride*miSize;// UR
	miOffset[7] = miSize +stride*miSize;// DR
	miOffset[8] = -miSize +stride*miSize;// DL
#endif

	for(Int mi = 0; mi<=RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_TRAINING_AREA; mi++)
	{
		// if MI is available
		if(current_pixel_pos + miOffset[mi] >= 0 &&
		   current_pixel_pos + miOffset[mi] < stride*H)
		{
			// LSP training along ALL the available previous SAIs
			for(Int idx = 0; idx < current_SAI; idx++)
			{
				supportIncomplete = false;
				dir_idx = spiral(idx, miSize, &i, &j);
#if RM_SCALABLE
				dir_idx = spiralScalable(idx, miSize, &i, &j);
#endif
				I = (Int)i - spiralOffset;
				J = (Int)j - spiralOffset;
				pixelOffset = I + J*stride;
				for(Int m=0; m<predOrder + predOrderExt; m++)
				{
#if !(RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE)
					C[validIdx][m] 	= p4DLFMI[origin_pixel_pos + miOffset[mi] + pixelOffset + causalSupportX[m] + causalSupportY[m]*stride];
#else
					C[validIdx][m] 	=  xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + causalSupportX[m], currentSAIsSpiralPosY + causalSupportY[m]) ? p4DLFMI[origin_pixel_pos + miOffset[mi] + pixelOffset + causalSupportX[m] + causalSupportY[m]*stride] : 0;
#endif
					if(!C[validIdx][m])
					{
						supportIncomplete = true;
						break;
					}
				}
				if(supportIncomplete)
					continue;
				y[validIdx] = p4DLFMI[origin_pixel_pos + miOffset[mi] + pixelOffset];
				validIdx++;
			}
		}
	}

	if(validIdx)
	{
		CValidt=doublematrix(predOrder + predOrderExt,validIdx);
		yValid=doublevector(validIdx);
		for(Int idx = 0; idx < validIdx; idx++)
		{
			for(Int m=0; m<predOrder + predOrderExt; m++)
				CValidt[m][idx] = C[idx][m];
			yValid[idx] = y[idx];
		}

		/*for(Int idx = 0; idx < validIdx; idx++)
		{
			for(Int m=0; m<predOrder + predOrderExt; m++)
			{
				cout << CValidt[m][idx] << "\t";
			}
			cout << endl;
		}

		for(Int idx = 0; idx < validIdx; idx++)
		{
			cout << yValid[idx] << endl;
		}*/

		leastSquares(CValidt,yValid,a,predOrder + predOrderExt,validIdx);
		free(yValid);
		free_doublematrix(CValidt,predOrder + predOrderExt, validIdx);
	}
	else
	{
		for(Int m=0; m<predOrder + predOrderExt; m++)
			a[m] = 1/(Double)(predOrder + predOrderExt);
	}
	free(y);
	free_doublematrix(C, current_SAI*(RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_TRAINING_AREA+1), predOrder + predOrderExt);
	//if(validIdx > 2)
	//	cout << current_SAI << "\t\t" << validIdx << "\t\t" << a[0] << "\t\t" << a[1] << "\t\t" << a[2] << endl;

	return a;
}

Double* TComPrediction::trainSpiralLSP( Int current_SAI, Int miSize, Pel* p4DLFMI, Int const origin_pixel_pos, Int const current_pixel_pos, Int const stride )
{
	Int dir_idx = 0;
	char current_direction[] = "RDLU";
	UInt i=0, j=0;
	Int I=0, J=0;
	double *y,*a,**C,**Ct;
	Int predOrder = 9;
	Int spiralOffset = floor(miSize/2); // relative spiral offset
	Int pixelOffset = 0;

	a=doublevector(predOrder);
	y=doublevector(current_SAI);
	C=doublematrix(current_SAI,predOrder);
	Ct=doublematrix(predOrder,current_SAI);

	// LSP training along all the available previous SAIs
	for(Int idx = 0; idx < current_SAI; idx++)
	{
		dir_idx = spiral(idx, miSize, &i, &j);
#if RM_SCALABLE
		dir_idx = spiralScalable(idx, miSize, &i, &j);
#endif
		I = (Int)i - spiralOffset;
		J = (Int)j - spiralOffset;
		pixelOffset = I + J*stride;
		y[idx] = p4DLFMI[origin_pixel_pos + pixelOffset];
		if(current_direction[dir_idx] == 'R')
		{
			C[idx][0] = p4DLFMI[origin_pixel_pos + pixelOffset + 1]; // RIGHT
			C[idx][1] = p4DLFMI[origin_pixel_pos + pixelOffset + 1]; // 2RIGHT
			C[idx][2] = p4DLFMI[origin_pixel_pos + pixelOffset + stride]; // DOWN
			C[idx][3] = p4DLFMI[origin_pixel_pos + pixelOffset + 2*stride]; // 2DOWN
			C[idx][4] = p4DLFMI[origin_pixel_pos + pixelOffset + 1 + stride]; // DOWN RIGHT
			C[idx][5] = p4DLFMI[origin_pixel_pos + pixelOffset - 1 + stride]; // DOWN LEFT
			C[idx][6] = p4DLFMI[origin_pixel_pos + pixelOffset - 1 + 2*stride]; // 2DOWN LEFT
			C[idx][7] = p4DLFMI[origin_pixel_pos + pixelOffset + 2 + stride]; // DOWN 2RIGHT
			C[idx][8] = p4DLFMI[origin_pixel_pos + pixelOffset + 1 + 2*stride];; // 2DOWN RIGHT
		}
		else if(current_direction[dir_idx] == 'D')
		{
			C[idx][0] = p4DLFMI[origin_pixel_pos + pixelOffset + stride]; // DOWN
			C[idx][1] = p4DLFMI[origin_pixel_pos + pixelOffset + 2*stride]; // 2DOWN
			C[idx][2] = p4DLFMI[origin_pixel_pos + pixelOffset - 1]; // LEFT
			C[idx][3] = p4DLFMI[origin_pixel_pos + pixelOffset - 2]; // 2LEFT
			C[idx][4] = p4DLFMI[origin_pixel_pos + pixelOffset - 1 + stride]; // DOWN LEFT
			C[idx][5] = p4DLFMI[origin_pixel_pos + pixelOffset - 1 - stride]; // UP LEFT
			C[idx][6] = p4DLFMI[origin_pixel_pos + pixelOffset - 2 - stride]; // UP 2LEFT
			C[idx][7] = p4DLFMI[origin_pixel_pos + pixelOffset - 1 + 2*stride]; // 2DOWN LEFT
			C[idx][8] = p4DLFMI[origin_pixel_pos + pixelOffset - 2 + stride]; // DOWN 2LEFT
		}
		else if(current_direction[dir_idx] == 'L')
		{
			C[idx][0] = p4DLFMI[origin_pixel_pos + pixelOffset - 1]; // LEFT
			C[idx][1] = p4DLFMI[origin_pixel_pos + pixelOffset - 2]; // 2LEFT
			C[idx][2] = p4DLFMI[origin_pixel_pos + pixelOffset - stride]; // UP
			C[idx][3] = p4DLFMI[origin_pixel_pos + pixelOffset - 2*stride]; // 2UP
			C[idx][4] = p4DLFMI[origin_pixel_pos + pixelOffset - 1 - stride]; // UP LEFT
			C[idx][5] = p4DLFMI[origin_pixel_pos + pixelOffset + 1 - stride]; // UP RIGHT
			C[idx][6] = p4DLFMI[origin_pixel_pos + pixelOffset + 1 - 2*stride]; // 2UP RIGHT
			C[idx][7] = p4DLFMI[origin_pixel_pos + pixelOffset - 2 - stride]; // UP 2LEFT
			C[idx][8] = p4DLFMI[origin_pixel_pos + pixelOffset - 1 - 2*stride];// 2UP LEFT
		}
		else
		{
			C[idx][0] = p4DLFMI[origin_pixel_pos + pixelOffset - stride]; // UP
			C[idx][1] = p4DLFMI[origin_pixel_pos + pixelOffset - 2*stride]; // 2UP
			C[idx][2] = p4DLFMI[origin_pixel_pos + pixelOffset + 1]; // RIGHT
			C[idx][3] = p4DLFMI[origin_pixel_pos + pixelOffset + 2]; // 2RIGHT
			C[idx][4] = p4DLFMI[origin_pixel_pos + pixelOffset + 1 - stride]; // UP RIGHT
			C[idx][5] = p4DLFMI[origin_pixel_pos + pixelOffset + 1 + stride]; // DOWN RIGHT
			C[idx][6] = p4DLFMI[origin_pixel_pos + pixelOffset + 2 + stride]; // DOWN 2RIGHT
			C[idx][7] = p4DLFMI[origin_pixel_pos + pixelOffset + 1 - 2*stride]; // 2UP RIGHT
			C[idx][8] = p4DLFMI[origin_pixel_pos + pixelOffset + 2 - stride];; // UP 2RIGHT
		}
		// making sure all the pixels have an assigned value
		if( !C[idx][6] )
			C[idx][6] = C[idx][5];
		if( !C[idx][3] )
		{
			C[idx][3] = C[idx][2];
			if( !C[idx][2] )
			{
				C[idx][3] = C[idx][5];
				C[idx][2] = C[idx][5];
			}
		}
		if( !C[idx][8] )
		{
			C[idx][8] = C[idx][4];
			if( !C[idx][4] )
			{
				C[idx][4] = C[idx][2];
				C[idx][8] = C[idx][2];
			}
		}
		if( !C[idx][7] )
		{
			C[idx][7] = C[idx][4];
			if( !C[idx][4] )
			{
				C[idx][4] = C[idx][2];
				C[idx][7] = C[idx][2];
			}
		}
		if( !C[idx][1] )
		{
			C[idx][1] = C[idx][0];
			if( !C[idx][0] )
			{
				C[idx][1] = C[idx][4];
				C[idx][0] = C[idx][4];
			}
		}
	}
	// transpose C
	for(Int idx = 0; idx < current_SAI; idx++)
	{
		for(Int c=0; c<predOrder; c++)
		{
			Ct[c][idx] = C[idx][c];
		}
	}

	leastSquares(Ct,y,a,predOrder,current_SAI);

	free(y);
	free_doublematrix(C, current_SAI, predOrder);
	free_doublematrix(Ct,predOrder, current_SAI);

	return a;

}

UInt TComPrediction::LSP( Double *lspCoefs, Int N, Pel* p4DLFMI, Int current_SAI, Int mi, Int current_pixel_pos, Int stride)
{
	Double predictor = 0;
	UInt i, j;
	Int dir_idx = 0;
	char current_direction[] = "RDLU";

	dir_idx = spiral(current_SAI, mi, &i, &j);
#if RM_SCALABLE
	dir_idx = spiralScalable(current_SAI, mi, &i, &j);
#endif
	if(current_direction[dir_idx] == 'R')
	{
		predictor = lspCoefs[0] * (Double)p4DLFMI[current_pixel_pos + 1]; // RIGHT
		predictor += lspCoefs[1] * (Double)p4DLFMI[current_pixel_pos + 1]; // 2RIGHT
		predictor += lspCoefs[2] * (Double)p4DLFMI[current_pixel_pos + stride]; // DOWN
		predictor += lspCoefs[3] * (Double)p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
		predictor += lspCoefs[4] * (Double)p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
		predictor += lspCoefs[5] * (Double)p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
		predictor += lspCoefs[6] * (Double)p4DLFMI[current_pixel_pos - 1 + 2*stride]; // 2DOWN LEFT
		predictor += lspCoefs[7] * (Double)p4DLFMI[current_pixel_pos + 2 + stride]; // DOWN 2RIGHT
		predictor += lspCoefs[8] * (Double)p4DLFMI[current_pixel_pos + 1 + 2*stride];; // 2DOWN RIGHT
	}
	else if(current_direction[dir_idx] == 'D')
	{
		predictor = lspCoefs[0] * (Double)p4DLFMI[current_pixel_pos + stride]; // DOWN
		predictor += lspCoefs[1] * (Double)p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
		predictor += lspCoefs[2] * (Double)p4DLFMI[current_pixel_pos - 1]; // LEFT
		predictor += lspCoefs[3] * (Double)p4DLFMI[current_pixel_pos - 2]; // 2LEFT
		predictor += lspCoefs[4] * (Double)p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
		predictor += lspCoefs[5] * (Double)p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
		predictor += lspCoefs[6] * (Double)p4DLFMI[current_pixel_pos - 2 - stride]; // UP 2LEFT
		predictor += lspCoefs[7] * (Double)p4DLFMI[current_pixel_pos - 1 + 2*stride]; // 2DOWN LEFT
		predictor += lspCoefs[8] * (Double)p4DLFMI[current_pixel_pos - 2 + stride]; // DOWN 2LEFT
	}
	else if(current_direction[dir_idx] == 'L')
	{
		predictor = lspCoefs[0] * (Double)p4DLFMI[current_pixel_pos - 1]; // LEFT
		predictor += lspCoefs[1] * (Double)p4DLFMI[current_pixel_pos - 2]; // 2LEFT
		predictor += lspCoefs[2] * (Double)p4DLFMI[current_pixel_pos - stride]; // UP
		predictor += lspCoefs[3] * (Double)p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
		predictor += lspCoefs[4] * (Double)p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
		predictor += lspCoefs[5] * (Double)p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
		predictor += lspCoefs[6] * (Double)p4DLFMI[current_pixel_pos + 1 - 2*stride]; // 2UP RIGHT
		predictor += lspCoefs[7] * (Double)p4DLFMI[current_pixel_pos - 2 - stride]; // UP 2LEFT
		predictor += lspCoefs[8] * (Double)p4DLFMI[current_pixel_pos - 1 - 2*stride];// 2UP LEFT
	}
	else
	{
		predictor = lspCoefs[0] * (Double)p4DLFMI[current_pixel_pos - stride]; // UP
		predictor += lspCoefs[1] * (Double)p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
		predictor += lspCoefs[2] * (Double)p4DLFMI[current_pixel_pos + 1]; // RIGHT
		predictor += lspCoefs[3] * (Double)p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
		predictor += lspCoefs[4] * (Double)p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
		predictor += lspCoefs[5] * (Double)p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
		predictor += lspCoefs[6] * (Double)p4DLFMI[current_pixel_pos + 2 + stride]; // DOWN 2RIGHT
		predictor += lspCoefs[7] * (Double)p4DLFMI[current_pixel_pos + 1 - 2*stride]; // 2UP RIGHT
		predictor += lspCoefs[8] * (Double)p4DLFMI[current_pixel_pos + 2 - stride];; // UP 2RIGHT
	}

	if(predictor < 0) predictor = 0;
	else if(predictor > 1023) predictor = 1023;

	free(lspCoefs);

	return (UInt)predictor;
}

UInt TComPrediction::LSP3( Double *lspCoefs, Int M, Pel* p4DLFMI, Int current_SAI, Int mi, Int current_pixel_pos, Int stride)
{
	Double predictor = 0;
	UInt i, j;
	Int dir_idx = 0;
	char current_direction[] = "RDLU";

	dir_idx = spiral(current_SAI, mi, &i, &j);
#if RM_SCALABLE
	dir_idx = spiralScalable(current_SAI, mi, &i, &j);
#endif

	if(current_direction[dir_idx] == 'R')
	{
		predictor = lspCoefs[0] * (Double)p4DLFMI[current_pixel_pos - 1]; // LEFT
		predictor += lspCoefs[1] * (Double)p4DLFMI[current_pixel_pos + stride]; // DOWN
		predictor += lspCoefs[2] * (Double)p4DLFMI[current_pixel_pos -1 + stride]; // DOWN LEFT
	}
	else if(current_direction[dir_idx] == 'D')
	{
		predictor = lspCoefs[0] * (Double)p4DLFMI[current_pixel_pos -1]; // LEFT
		predictor += lspCoefs[1] * (Double)p4DLFMI[current_pixel_pos -stride]; // UP
		predictor += lspCoefs[2] * (Double)p4DLFMI[current_pixel_pos - 1 -stride]; // UP LEFT
	}
	else if(current_direction[dir_idx] == 'L')
	{
		predictor = lspCoefs[0] * (Double)p4DLFMI[current_pixel_pos + 1]; // RIGHT
		predictor += lspCoefs[1] * (Double)p4DLFMI[current_pixel_pos - stride]; // UP
		predictor += lspCoefs[2] * (Double)p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
	}
	else
	{
		predictor = lspCoefs[0] * (Double)p4DLFMI[current_pixel_pos +1]; // RIGHT
		predictor += lspCoefs[1] * (Double)p4DLFMI[current_pixel_pos +stride]; // DOWN
		predictor += lspCoefs[2] * (Double)p4DLFMI[current_pixel_pos +1 +stride]; // DOWN RIGHT
	}

	if(predictor < 0) predictor = 0;
	else if(predictor > 1023) predictor = 1023;

	free(lspCoefs);

	return (UInt)predictor;
}

UInt TComPrediction::LSPM( Int* causalSupportX, Int* causalSupportY, Double *lspCoefs, Int M, Pel* p4DLFMI, Int current_SAI, Int mi, Int current_pixel_pos, Int stride, Int bit_depth)
{
	Double predictor = 0;
	Int max_value = (1<<bit_depth) - 1;

	for(Int m=0; m<M; m++)
		predictor += lspCoefs[m] * (Double)p4DLFMI[current_pixel_pos + causalSupportX[m] + causalSupportY[m]*stride];

	if(predictor < 0) predictor = 0;
	else if(predictor > max_value) predictor = max_value;

	free(lspCoefs);

	return (UInt)predictor;
}

/*!
 ********************************************************************************************
 * \brief
 *    Allocates memory for a vector of variable of type double
 *
 * \param n
 *   vector size
 *
 * \return
 *    a poiter to a double vector (double *)
 *
 ********************************************************************************************
*/
double * TComPrediction::doublevector(long n)
{
  double *m;

  m=(double *)calloc(n, sizeof(double));
  if (!m)
  {
	printf("doublevector() - allocation failure 1 \n");
	exit(1);
  }
  return m;
}

/*!
 ********************************************************************************************
 * \brief
 *    Allocates memory for a matrix of variables of type double
 *
 * \param nr
 *    number of rows
 * \param nc
 *     number of columns
 *
 * \return
 *    a poiter to a float matrix (double **)
 *
 ********************************************************************************************
*/
double ** TComPrediction::doublematrix(long nr, long nc)
{
  long i;
  double **m;

  m=(double **)malloc(nr*nc*sizeof(double *));
  if (!m)
  {
	printf("doublematrix() - allocation failure 1 \n");
	exit(1);
  }
  for(i=0;i<nr;i++)
  {
	m[i]=(double *)calloc(nc, sizeof(double));
	if (!m[i])
	{
		printf("doublematrix() - allocation failure 2 \n");
		exit(1);
	}
  }
  return m;
}

/*!
 ********************************************************************************************
 * \brief
 *    Deallocates memory for a matrix of variables of type double
 *
 * \param matrix
 *   matrix to be deleted
 * \param nr
 *    number of rows
 * \param nc
 *    number of columns
 *
 * \return
 *    none.
 *
 ********************************************************************************************
*/
void TComPrediction::free_doublematrix(double **matrix, int nr, int nc)
{
  int i;

  for (i=0; i<nr; i++)
	free(matrix[i]);
  free(matrix);
}

/*
Cx = y (1)

(Cx - y)²= mean square error = 0 (2)
(Cx)'Cx - (Cx)'y - y'Cx + y'y = 0

substituting (Cx) for y, we have

(Cx)'Cx - (y)'y - y'Cx + y'y = 0
(Cx)'Cx = y'Cx

dividing by x:

(Cx)'C = y'C

and transposing the equation:

C'Cx = C'y
"Ax=b", where A is a square matrix, x and b are vectors
 ********************************************************************************************
 */
Void TComPrediction::leastSquares( Double **C, Double *y, Double *a, Int m, Int n )
{
	int i,j,k,*index,flag;
	double **cov,d,*col;

	index=(int *)malloc((m+1)*sizeof(int));
	col=(double *)malloc((m+1)*sizeof(double));
	cov=(double **)malloc((m+1)*sizeof(double));
	for (i=0;i<m+1;i++)
		cov[i]=(double *)malloc((m+1)*sizeof(double));
	for(i=0;i<m;i++)
		for(j=i;j<m;j++)
		{
			cov[i+1][j+1]=0;
			for(k=0;k<n;k++)
				cov[i+1][j+1]+=C[i][k]*C[j][k];
		}
	for(i=0;i<m;i++)
		for(j=0;j<=i;j++)
			cov[i+1][j+1]=cov[j+1][i+1];
	for(i=0;i<m;i++)
	{
		col[i+1]=0;
		for(j=0;j<n;j++)
			col[i+1]+=C[i][j]*y[j];
	}
	flag=ludcmp(cov,m,index,&d);
	if(flag==0)
	{
		lubksb(cov,m,index,col);
		for(i=1;i<=m;i++)
			a[i-1]=col[i];
	}
	else
		for(i=1;i<=m;i++)
			a[i-1]=1.0/m;

	free(index);free(col);
	for(i=0;i<m+1;i++)
		free(cov[i]);
	free(cov);
}
/*!
 ********************************************************************************************
 * \brief
 *    LU decomposition : write a matrix as a product of two matrices, L (lower) and U (upper)
 *    Crout's algorithm is the decomposition in place. The follwoing algorithm doesn't actually
 *    decompose the matrix A in LU form; it rather decompose a rowwise permutation of the input
 *    matrix. The permutation is recorded in the index output vector
 *
 *  \param input
 *	double matrix a
 *	size of matrix n (A[nxn])
 *  \param output
 *	double matrix a (decomposed matrix)
 *	int vector index (permutation is recorded here)
 *	d = +1 (even rows interchanged), -1 (odd rows interchanged)
 *
 ********************************************************************************************
 */
Int TComPrediction::ludcmp(Double **a, Int n, Int *indx, Double *d)
{
	int i,j,k;
	int imax = 0;
	double big,dum,sum,temp;
	double *vv;

	vv=(double *)malloc((n+1)*sizeof(double));
	*d=1.0;
	for(i=1;i<=n;i++)
	{
		big=0.0;
		for(j=1;j<=n;j++)
			if((temp=fabs(a[i][j]))>big)
				big=temp;
		if(big==0.0)
		{
			free(vv);
			return(1);
		}
		vv[i]=1.0/big;
	}
	for(j=1;j<=n;j++)
	{
		for(i=1;i<j;i++)
		{
			sum=a[i][j];
			for(k=1;k<i;k++)
				sum-=a[i][k]*a[k][j];
			a[i][j]=sum;
		}
		big=0.0;
		for(i=j;i<=n;i++)
		{
			sum=a[i][j];
			for(k=1;k<j;k++)
				sum-=a[i][k]*a[k][j];
			a[i][j]=sum;
			if((dum=vv[i]*fabs(sum))>=big)
			{
				big=dum;
				imax=i;
			}
		}
		if(j!=imax)
		{
			for(k=1;k<=n;k++)
			{
				dum=a[imax][k];
				a[imax][k]=a[j][k];
				a[j][k]=dum;
			}
			*d=-(*d);
			vv[imax]=vv[j];
		}
		indx[j]=imax;
		if(a[j][j]==0.0)
			a[j][j]=TINY;
		if(j!=n)
		{
			dum=1.0/(a[j][j]);
			for(i=j+1;i<=n;i++)
				a[i][j]*=dum;
		}
	}
	free(vv);
	return(0);
}
/*!
 ********************************************************************************************
 *  \brief
 *      The lubksb algorithm (for forward substitution and backward substitution) solves the
 *      set of N linear equations Ax=b, where A is the output of the ludcmp algorithm.
 *
 * \param input:
 *	double matrix a (output from ludcmp, it is not altered by the algorithm)
 *	size n of matrix (A[nxn])
 *	int vector indx (permutations)
 *	double vector b
 *   \param output:
 *	double vector b (includes the solution of the matrix
 ********************************************************************************************
 */
Void TComPrediction::lubksb(Double **a, Int n, Int *indx, Double b[])
{
	int i,ii=0,ip,j;
	double sum;

	for(i=1;i<=n;i++)
	{
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if(ii)
			for(j=ii;j<=i-1;j++)
				sum-=a[i][j]*b[j];
		else if
		(sum) ii=i;
		b[i]=sum;
	}
	for(i=n;i>=1;i--)
	{
		sum=b[i];
		for(j=i+1;j<=n;j++)
			sum-=a[i][j]*b[j];
		b[i]=sum/a[i][i];
	}
}

#endif

#if RM_4DLF_MI_BUFFER
Int TComPrediction::getCausalSupportAdaptive( Int M, Int MExt, Int* causalSupportX, Int* causalSupportY, Int currentSAI, Int origin_pixel_pos_MI, Int current_pixel_pos, Int mi, Int stride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentSAIsSpiralPosX, Int currentSAIsSpiralPosY, Int currentRegionRA
#endif
)
{
	Int numPixelsMI = mi*mi;
	Int l1Distance[numPixelsMI];
	Int spiralBorderOffset = floor(mi/2);
	Int currentPixelPosY = (current_pixel_pos / stride) - (origin_pixel_pos_MI / stride);
	Int currentPixelPosX = (current_pixel_pos % stride) - (origin_pixel_pos_MI % stride);
	Int minValue = MAX_INT;
	Int minValuePosY = 0, minValuePosX = 0;
	Int numValidPos = 0;
	Bool atLeastOneValidPos = false;
	UInt i=0,j=0, dir_idx;
	Int miOffsetX[8], miOffsetY[8];

#if RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT
	miOffsetX[0] = -mi;	miOffsetY[0] = 0;	// LEFT
	miOffsetX[1] = 0;	miOffsetY[1] = -mi;	// UP
	miOffsetX[2] = mi;	miOffsetY[2] = 0;	// RIGHT
	miOffsetX[3] = 0;	miOffsetY[3] = mi;	// DOWN
	miOffsetX[4] = -mi; miOffsetY[4] = -mi; // UL
	miOffsetX[5] = mi; 	miOffsetY[5] = -mi;	// UR
	miOffsetX[6] = mi; 	miOffsetY[6] = mi;	// DR
	miOffsetX[7] = -mi, miOffsetY[7] = mi;	// DL
#endif

	// init distance
	for(Int i=0; i<numPixelsMI; i++)
		l1Distance[i] = NOT_VALID;

	// limit support to inside the MI
	//for(Int y=-spiralBorderOffset; y<=spiralBorderOffset; y++)
		//for(Int x=-spiralBorderOffset; x<=spiralBorderOffset; x++)
		//{
	for(Int idx=0; idx<currentSAI; idx++)
	{
		dir_idx = spiral(idx, mi, &i, &j);
#if RM_SCALABLE
		dir_idx = spiralScalable(idx, mi, &i, &j);
#endif
		l1Distance[j*mi + i] = abs(currentPixelPosX - ((Int)i - spiralBorderOffset)) + abs(currentPixelPosY - ((Int)j - spiralBorderOffset));
			//if(l1Distance[(spiralBorderOffset+y)*mi + spiralBorderOffset+x] == 0)
			//	cout << "WRONG DISTANCE" << endl;

	}
		//}
		//}

	// search min distance
	for(Int m=0; m<M; m++)
	{
		atLeastOneValidPos = false;
		minValue = MAX_INT;
		for(Int y=-spiralBorderOffset; y<=spiralBorderOffset; y++)
		{
			for(Int x=-spiralBorderOffset; x<=spiralBorderOffset; x++)
			{
				if(l1Distance[(spiralBorderOffset+y)*mi + spiralBorderOffset+x] >= 0 &&
				   l1Distance[(spiralBorderOffset+y)*mi + spiralBorderOffset+x] < minValue &&
				   !(x == y && x == 0))
				{
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
					if(xCheckIfInTheSameRegion(currentRegionRA, currentSAI, currentSAIsSpiralPosX + x, currentSAIsSpiralPosY + y))
					{
#endif
					atLeastOneValidPos = true;
					minValue = l1Distance[(spiralBorderOffset+y)*mi + spiralBorderOffset+x];
					minValuePosY = y - currentPixelPosY;
					minValuePosX = x - currentPixelPosX;
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
					}
#endif
				}
			}
		}
		if(atLeastOneValidPos)
		{
			l1Distance[(spiralBorderOffset+minValuePosY+currentPixelPosY)*mi + spiralBorderOffset+minValuePosX+currentPixelPosX] = NOT_VALID;
			causalSupportX[m] = minValuePosX;
			causalSupportY[m] = minValuePosY;
			if(numValidPos < RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_PRED_ORDER) // extend support to other MIs
			{
				for(Int miExt=0; miExt<RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT; miExt++)
				{ // M -> jump to beg of extended list of support pixels // m -> individual MI idx // miExt -> ext MI position index
					causalSupportX[M+m+miExt*RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_PRED_ORDER] = minValuePosX + miOffsetX[miExt];
					causalSupportY[M+m+miExt*RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_PRED_ORDER] = minValuePosY + miOffsetY[miExt];
					//cout << M+m+miExt*RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_PRED_ORDER << endl;
				}
			}
			numValidPos++;
		}
		else
			break;
	}

	return numValidPos; // number of available pixels for the support
}
#if RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_SEARCH
Void TComPrediction::extendedSupportSearch( Pel* p4DLFMI, Int M, Int MExt, Int* causalSupportX, Int* causalSupportY, Int currentSAI, Int origin_pixel_pos_MI, Int current_pixel_pos, Int mi, Int stride )
{
	Int templateSize = RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_SEARCH_TEMPLATE_SIZE;
	Int templateSupportX[templateSize];
	Int templateSupportY[templateSize];
	getCausalSupportAdaptive( templateSize, 0, templateSupportX, templateSupportY, currentSAI, origin_pixel_pos_MI, current_pixel_pos, mi, stride );
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
#endif
	Int miOffsetX[8], miOffsetY[8];
	Int maxDisp = RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_SEARCH_WINDOW;
	Int extendedSupportMIs = RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT;
	Int extendedSupportPredOrder = RM_4DLF_MI_INTRA_MODE_LSP_EXTEND_SUPPORT_PRED_ORDER;
	Int bestSSD = MAX_INT, currentSSD = MAX_INT;
	Int bestDisparityX = 0, bestDisparityY = 0;
	Int currentPixelDif, currentPixelExtended, currentPixelRef;

	miOffsetX[0] = -mi;	miOffsetY[0] = 0;	// LEFT
	miOffsetX[1] = 0;	miOffsetY[1] = -mi;	// UP
	miOffsetX[2] = mi;	miOffsetY[2] = 0;	// RIGHT
	miOffsetX[3] = 0;	miOffsetY[3] = mi;	// DOWN
	miOffsetX[4] = -mi; miOffsetY[4] = -mi; // UL
	miOffsetX[5] = mi; 	miOffsetY[5] = -mi;	// UR
	miOffsetX[6] = mi; 	miOffsetY[6] = mi;	// DR
	miOffsetX[7] = -mi, miOffsetY[7] = mi;	// DL

	for(Int m=0; m<extendedSupportMIs; m++)
	{
		for(Int j=-maxDisp; j<=maxDisp; j++)
		{
			for(Int i=-maxDisp; i<=maxDisp; i++)
			{
				currentSSD = 0;
				for(Int p=0; p<templateSize; p++)
				{
					currentPixelExtended = p4DLFMI[current_pixel_pos + templateSupportX[p] + miOffsetX[m] + i + (templateSupportY[p] + miOffsetY[m] + j)*stride];
					currentPixelRef = p4DLFMI[current_pixel_pos + templateSupportX[p] + (templateSupportY[p])*stride];
					if(currentPixelExtended)
					{
						currentPixelDif = (currentPixelRef - currentPixelExtended);
						currentSSD += currentPixelDif * currentPixelDif;
					}
					else
					{
						currentSSD = MAX_INT;
						break;
					}
				}
				if(currentSSD < bestSSD)
				{
					bestSSD = currentSSD;
					bestDisparityX = i;
					bestDisparityY = j;
				}
			}
		}
		for(Int n=0; n<extendedSupportPredOrder; n++)
		{
			causalSupportX[M + m*extendedSupportMIs + n] += bestDisparityX;
			causalSupportY[M + m*extendedSupportMIs + n] += bestDisparityY;
		}
	}

}
#endif

Void TComPrediction::getCausalSupportFromSpiral_LOCO_I( Int* a, Int* b, Int* c, Int current_SAI, Int total_number_of_SAIS, Pel* p4DLFMI, Int const current_pixel_pos, Int const stride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,Int currentSAIsSpiralPosX, Int currentSAIsSpiralPosY, Int currentRegionRA
#endif
)
{
	Int A=0, B=0, C=0;
	Int dir_idx = 0;
	char current_direction[] = "RDLU";
	UInt x=0, y=0;

	if(current_SAI == 0) // no predictor is calculated
	{
		A = NOT_VALID;
		B = NOT_VALID;
		C = NOT_VALID;
	}
	else if(current_SAI == 1) // predictor is A
	{
		A = p4DLFMI[current_pixel_pos - 1]; // LEFT
		B = A;
		C = A;
	}
	else{
		// find current spiral direction
		dir_idx = spiral(current_SAI, sqrt(total_number_of_SAIS), &x, &y);
#if RM_SCALABLE
		dir_idx = spiralScalable(current_SAI, sqrt(total_number_of_SAIS), &x, &y);
#endif
#if !(RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE)
		if(current_direction[dir_idx] == 'R')
		{
			A = p4DLFMI[current_pixel_pos - 1]; 			// LEFT
			C = p4DLFMI[current_pixel_pos - 1 + stride]; 	// DOWN LEFT
			B = p4DLFMI[current_pixel_pos + stride] ? p4DLFMI[current_pixel_pos + stride] : C; // DOWN (may not be available)
		}
		else if(current_direction[dir_idx] == 'D')
		{
			B = p4DLFMI[current_pixel_pos - stride]; 		// UP
			C = p4DLFMI[current_pixel_pos - 1 - stride];	// UP LEFT
			A = p4DLFMI[current_pixel_pos - 1] ? p4DLFMI[current_pixel_pos - 1] : C; // LEFT (may not be available)
		}
		else if(current_direction[dir_idx] == 'L')
		{
			A = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			C = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			B = p4DLFMI[current_pixel_pos - stride] ? p4DLFMI[current_pixel_pos - stride] : C; // UP (may not be available)
		}
		else
		{
			B = p4DLFMI[current_pixel_pos + stride]; // DOWN
			C = p4DLFMI[current_pixel_pos + 1 + stride]; // RIGHT DOWN
			A = p4DLFMI[current_pixel_pos + 1] ? p4DLFMI[current_pixel_pos + 1] : C; // RIGHT (may not be available)
		}
#else
		if(current_direction[dir_idx] == 'R')
		{
			A = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 1] : NOT_VALID;
			C = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos - 1 + stride] : NOT_VALID;
			B = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 1) ? (p4DLFMI[current_pixel_pos + stride] ? p4DLFMI[current_pixel_pos + stride] : C) : NOT_VALID;
		}
		else if(current_direction[dir_idx] == 'D')
		{
			B = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - stride] : NOT_VALID; 		// UP
			C = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - 1 - stride] : NOT_VALID;	// UP LEFT
			A = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY) ? (p4DLFMI[current_pixel_pos - 1] ? p4DLFMI[current_pixel_pos - 1] : C) : NOT_VALID; // LEFT (may not be available)
		}
		else if(current_direction[dir_idx] == 'L')
		{
			A = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 1] : NOT_VALID; // RIGHT
			C = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos + 1 - stride] : NOT_VALID; // UP RIGHT
			B = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 1) ? (p4DLFMI[current_pixel_pos - stride] ? p4DLFMI[current_pixel_pos - stride] : C) : NOT_VALID; // UP (may not be available)
		}
		else
		{
			B = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + stride] : NOT_VALID; // DOWN
			C = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + 1 + stride] : NOT_VALID; // RIGHT DOWN
			A = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY) ? (p4DLFMI[current_pixel_pos + 1] ? p4DLFMI[current_pixel_pos + 1] : C) : NOT_VALID; // RIGHT (may not be available)
		}
#endif
	}

	*a = A;
	*b = B;
	*c = C;
}

Int TComPrediction::LOCO_I( Int a, Int b, Int c )
{
	if(c >= max(a,b))
		return min(a,b);
	else if(c <= min(a,b))
		return max(a,b);
	else
		return a + b - c;
}


Void TComPrediction::getCausalSupportFromSpiral_GAP( Int* w, Int* ww, Int* n, Int* nn, Int* nw, Int* ne, Int *nne, Int current_SAI, Int total_number_of_SAIS, Pel* p4DLFMI, Int const current_pixel_pos, Int const stride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
		  ,Int currentSAIsSpiralPosX, Int currentSAIsSpiralPosY, Int currentRegionRA
#endif
)
{
	Int W=NOT_VALID, WW=NOT_VALID, N=NOT_VALID, NN=NOT_VALID, NW=NOT_VALID, NE=NOT_VALID, NNE=NOT_VALID;
	Int dir_idx = 0;
	char current_direction[] = "RDLU";
	UInt x=0, y=0;

	if(current_SAI < 9) // no predictor is calculated
	{
		W = NOT_VALID;
		WW = NOT_VALID;
		N = NOT_VALID;
		NN = NOT_VALID;
		NW = NOT_VALID;
		NE = NOT_VALID;
		NNE = NOT_VALID;
	}
	else{
		// find current spiral direction
		dir_idx = spiral(current_SAI, sqrt(total_number_of_SAIS), &x, &y);
#if RM_SCALABLE
		dir_idx = spiralScalable(current_SAI, sqrt(total_number_of_SAIS), &x, &y);
#endif
#if !(RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE)
#if !RM_BUG_CORRECTION_PIXEL_MODELS_29042019
		if(current_direction[dir_idx] == 'R')
		{
			W = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			WW = p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
			N = p4DLFMI[current_pixel_pos + stride]; // DOWN
			NN = p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
			NW = p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
			NE = p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
			NNE = p4DLFMI[current_pixel_pos - 1 + 2*stride]; // 2DOWN LEFT
		}
		else if(current_direction[dir_idx] == 'D')
		{
			W = p4DLFMI[current_pixel_pos + stride]; // DOWN
			WW = p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
			N = p4DLFMI[current_pixel_pos - 1]; // LEFT
			NN = p4DLFMI[current_pixel_pos - 2]; // 2LEFT
			NW = p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
			NE = p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
			NNE = p4DLFMI[current_pixel_pos - 2 - stride]; // UP 2LEFT
		}
		else if(current_direction[dir_idx] == 'L')
		{
			W = p4DLFMI[current_pixel_pos - 1]; // LEFT
			WW = p4DLFMI[current_pixel_pos - 2]; // 2LEFT
			N = p4DLFMI[current_pixel_pos - stride]; // UP
			NN = p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
			NW = p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
			NE = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			NNE = p4DLFMI[current_pixel_pos + 1 - 2*stride]; // 2UP RIGHT
		}
		else
		{
			W = p4DLFMI[current_pixel_pos - stride]; // UP
			WW = p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
			N = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			NN = p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
			NW = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			NE = p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
			NNE = p4DLFMI[current_pixel_pos + 2 + stride]; // DOWN 2RIGHT
		}
#else
		if(current_direction[dir_idx] == 'R') // NO UP
		{
			W = p4DLFMI[current_pixel_pos - 1]; // LEFT
			WW = p4DLFMI[current_pixel_pos - 2]; // 2LEFT
			N = p4DLFMI[current_pixel_pos + stride]; // DOWN
			NN = p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
			NW = p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
			NE = p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
			NNE = p4DLFMI[current_pixel_pos + 1 + 2*stride]; // 2DOWN RIGHT
		}
		else if(current_direction[dir_idx] == 'D') // NO RIGHT
		{
			W = p4DLFMI[current_pixel_pos - stride]; // UP
			WW = p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
			N = p4DLFMI[current_pixel_pos - 1]; // LEFT
			NN = p4DLFMI[current_pixel_pos - 2]; // 2LEFT
			NW = p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
			NE = p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
			NNE = p4DLFMI[current_pixel_pos - 2 + stride]; // DOWN 2LEFT
		}
		else if(current_direction[dir_idx] == 'L') // NO DOWN
		{
			W = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			WW = p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
			N = p4DLFMI[current_pixel_pos - stride]; // UP
			NN = p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
			NW = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			NE = p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
			NNE = p4DLFMI[current_pixel_pos - 1 - 2*stride]; // 2UP LEFT
		}
		else // NO LEFT
		{
			W = p4DLFMI[current_pixel_pos + stride]; // DOWN
			WW = p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
			N = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			NN = p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
			NW = p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
			NE = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			NNE = p4DLFMI[current_pixel_pos + 2 - stride]; // UP 2RIGHT
		}
#endif
#else
		if(current_direction[dir_idx] == 'R')
		{
			W = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 1] : NOT_VALID; // RIGHT
			WW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 2, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 2] : NOT_VALID; // 2RIGHT
			N = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + stride] : NOT_VALID; // DOWN
			NN = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 2) ? p4DLFMI[current_pixel_pos + 2*stride] : NOT_VALID; // 2DOWN
			NW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + 1 + stride] : NOT_VALID; // DOWN RIGHT
			NE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos - 1 + stride] : NOT_VALID; // DOWN LEFT
			NNE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY + 2) ? p4DLFMI[current_pixel_pos - 1 + 2*stride] : NOT_VALID; // 2DOWN LEFT
		}
		else if(current_direction[dir_idx] == 'D')
		{
			W = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + stride] : NOT_VALID; // DOWN
			WW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 2) ? p4DLFMI[current_pixel_pos + 2*stride] : NOT_VALID; // 2DOWN
			N = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 1] : NOT_VALID; // LEFT
			NN = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 2, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 2] : NOT_VALID; // 2LEFT
			NW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos - 1 + stride] : NOT_VALID; // DOWN LEFT
			NE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - 1 - stride] : NOT_VALID; // UP LEFT
			NNE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 2, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - 2 - stride] : NOT_VALID; // UP 2LEFT
		}
		else if(current_direction[dir_idx] == 'L')
		{
			W = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 1] : NOT_VALID; // LEFT
			WW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 2, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 2] : NOT_VALID; // 2LEFT
			N = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - stride] : NOT_VALID; // UP
			NN = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 2) ? p4DLFMI[current_pixel_pos - 2*stride] : NOT_VALID; // 2UP
			NW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - 1 - stride] : NOT_VALID; // UP LEFT
			NE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos + 1 - stride] : NOT_VALID; // UP RIGHT
			NNE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY - 2) ? p4DLFMI[current_pixel_pos + 1 - 2*stride] : NOT_VALID; // 2UP RIGHT
		}
		else
		{
			W = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - stride] : NOT_VALID; // UP
			WW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 2) ? p4DLFMI[current_pixel_pos - 2*stride] : NOT_VALID; // 2UP
			N = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 1] : NOT_VALID; // RIGHT
			NN = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 2, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 2] : NOT_VALID; // 2RIGHT
			NW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos + 1 - stride] : NOT_VALID; // UP RIGHT
			NE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + 1 + stride] : NOT_VALID; // DOWN RIGHT
			NNE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 2, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + 2 + stride] : NOT_VALID; // DOWN 2RIGHT
		}
#endif
	}
	// making sure all the pixels have an assigned value
	if( !NNE )
		NNE = NE;
	if( !NN )
	{
		NN = N;
		if( !N )
		{
			NN = NE;
			N = NE;
		}
	}
	if( !NW )
	{
		NW = N;
	}
	if( !WW )
	{
		WW = W;
		if( !W )
		{
			WW = NW;
			W = NW;
		}
	}
	//if(current_SAI >= 9)
		//if(W == NOT_VALID || WW == NOT_VALID || N == NOT_VALID || NN == NOT_VALID || NW == NOT_VALID ||	NE == NOT_VALID || NNE == NOT_VALID)
			//cout << "at least one pixel is not available" << endl;

	*w = W;
	*ww = WW;
	*n = N;
	*nn = NN;
	*nw = NW;
	*ne = NE;
	*nne = NNE;
}

Int TComPrediction::GAP( Int bitDepth, Int w, Int ww, Int n, Int nn, Int nw, Int ne, Int nne )
{
	Int bitDepthMultiplier = (bitDepth > 8 ? 4 : 1);

	Int dx = abs(w - ww) + abs(n - nw) + abs(ne - n);
	Int dy = abs(w - nw) + abs(n - nn) + abs(ne - nne);

	if(dy - dx > 80 * bitDepthMultiplier)
		return w;
	else if(dy - dx < -80 * bitDepthMultiplier)
		return n;
	else
	{
		Double aux = (w + n)/2 + (ne - nw)/4;
		if(dy - dx > 32 * bitDepthMultiplier)
			return (Int)((aux + w)/2);
		else if(dy - dx < -32 * bitDepthMultiplier)
			return (Int)((aux + n)/2);
		else if(dy - dx > 8 * bitDepthMultiplier)
			return (Int)((3*aux + w)/4);
		else if(dy - dx < -8 * bitDepthMultiplier)
			return (Int)((3*aux + n)/4);
		else
			return (Int)(aux);
	}
}

Void TComPrediction::getCausalSupportFromSpiral_AGSP( Int* w, Int* ww, Int* n, Int* nn, Int* nw, Int* ne, Int *nne, Int* nww, Int* nnw, Int current_SAI, Int total_number_of_SAIS, Pel* p4DLFMI, Int const current_pixel_pos, Int const stride
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
		  ,Int currentSAIsSpiralPosX, Int currentSAIsSpiralPosY, Int currentRegionRA
#endif
)
{
	Int W=NOT_VALID, WW=NOT_VALID, N=NOT_VALID, NN=NOT_VALID, NW=NOT_VALID, NE=NOT_VALID, NNE=NOT_VALID, NWW=NOT_VALID, NNW=NOT_VALID;
	Int dir_idx = 0;
	char current_direction[] = "RDLU";
	UInt x=0, y=0;

	if(current_SAI < 9) // no predictor is calculated
	{
		W = NOT_VALID;
		WW = NOT_VALID;
		N = NOT_VALID;
		NN = NOT_VALID;
		NW = NOT_VALID;
		NE = NOT_VALID;
		NNE = NOT_VALID;
		NWW = NOT_VALID;
		NNW = NOT_VALID;
	}
	else{
		// find current spiral direction
		dir_idx = spiral(current_SAI, sqrt(total_number_of_SAIS), &x, &y);
#if RM_SCALABLE
		dir_idx = spiralScalable(current_SAI, sqrt(total_number_of_SAIS), &x, &y);
#endif
#if !(RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE)
#if !RM_BUG_CORRECTION_PIXEL_MODELS_29042019
		if(current_direction[dir_idx] == 'R')
		{
			W = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			WW = p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
			N = p4DLFMI[current_pixel_pos + stride]; // DOWN
			NN = p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
			NW = p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
			NE = p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
			NNE = p4DLFMI[current_pixel_pos - 1 + 2*stride]; // 2DOWN LEFT
			NWW = p4DLFMI[current_pixel_pos + 2 + stride]; // DOWN 2RIGHT
			NNW = p4DLFMI[current_pixel_pos + 1 + 2*stride];; // 2DOWN RIGHT
		}
		else if(current_direction[dir_idx] == 'D')
		{
			W = p4DLFMI[current_pixel_pos + stride]; // DOWN
			WW = p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
			N = p4DLFMI[current_pixel_pos - 1]; // LEFT
			NN = p4DLFMI[current_pixel_pos - 2]; // 2LEFT
			NW = p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
			NE = p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
			NNE = p4DLFMI[current_pixel_pos - 2 - stride]; // UP 2LEFT
			NWW = p4DLFMI[current_pixel_pos - 1 + 2*stride]; // 2DOWN LEFT
			NNW = p4DLFMI[current_pixel_pos - 2 + stride]; // DOWN 2LEFT
		}
		else if(current_direction[dir_idx] == 'L')
		{
			W = p4DLFMI[current_pixel_pos - 1]; // LEFT
			WW = p4DLFMI[current_pixel_pos - 2]; // 2LEFT
			N = p4DLFMI[current_pixel_pos - stride]; // UP
			NN = p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
			NW = p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
			NE = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			NNE = p4DLFMI[current_pixel_pos + 1 - 2*stride]; // 2UP RIGHT
			NWW = p4DLFMI[current_pixel_pos - 2 - stride]; // UP 2LEFT
			NNW = p4DLFMI[current_pixel_pos - 1 - 2*stride];// 2UP LEFT
		}
		else
		{
			W = p4DLFMI[current_pixel_pos - stride]; // UP
			WW = p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
			N = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			NN = p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
			NW = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			NE = p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
			NNE = p4DLFMI[current_pixel_pos + 2 + stride]; // DOWN 2RIGHT
			NWW = p4DLFMI[current_pixel_pos + 1 - 2*stride]; // 2UP RIGHT
			NNW = p4DLFMI[current_pixel_pos + 2 - stride];; // UP 2RIGHT
		}
#else
		if(current_direction[dir_idx] == 'R')
		{
			W = p4DLFMI[current_pixel_pos - 1]; // LEFT
			WW = p4DLFMI[current_pixel_pos - 2]; // 2LEFT
			N = p4DLFMI[current_pixel_pos + stride]; // DOWN
			NN = p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
			NW = p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
			NE = p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
			NNE = p4DLFMI[current_pixel_pos + 1 + 2*stride]; // 2DOWN RIGHT
			NWW = p4DLFMI[current_pixel_pos - 2 + stride]; // DOWN 2LEFT
			NNW = p4DLFMI[current_pixel_pos - 1 + 2*stride];; // 2DOWN LEFT
		}
		else if(current_direction[dir_idx] == 'D')
		{
			W = p4DLFMI[current_pixel_pos - stride]; // UP
			WW = p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
			N = p4DLFMI[current_pixel_pos - 1]; // LEFT
			NN = p4DLFMI[current_pixel_pos - 2]; // 2LEFT
			NW = p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
			NE = p4DLFMI[current_pixel_pos - 1 + stride]; // DOWN LEFT
			NNE = p4DLFMI[current_pixel_pos - 2 + stride]; // DOWN 2LEFT
			NWW = p4DLFMI[current_pixel_pos - 1 - 2*stride]; // 2UP LEFT
			NNW = p4DLFMI[current_pixel_pos - 2 - stride]; // UP 2LEFT
		}
		else if(current_direction[dir_idx] == 'L')
		{
			W = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			WW = p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
			N = p4DLFMI[current_pixel_pos - stride]; // UP
			NN = p4DLFMI[current_pixel_pos - 2*stride]; // 2UP
			NW = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			NE = p4DLFMI[current_pixel_pos - 1 - stride]; // UP LEFT
			NNE = p4DLFMI[current_pixel_pos - 1 - 2*stride]; // 2UP LEFT
			NWW = p4DLFMI[current_pixel_pos + 2 - stride]; // UP 2RIGHT
			NNW = p4DLFMI[current_pixel_pos + 1 - 2*stride];// 2UP RIGHT
		}
		else
		{
			W = p4DLFMI[current_pixel_pos + stride]; // DOWN
			WW = p4DLFMI[current_pixel_pos + 2*stride]; // 2DOWN
			N = p4DLFMI[current_pixel_pos + 1]; // RIGHT
			NN = p4DLFMI[current_pixel_pos + 2]; // 2RIGHT
			NW = p4DLFMI[current_pixel_pos + 1 + stride]; // DOWN RIGHT
			NE = p4DLFMI[current_pixel_pos + 1 - stride]; // UP RIGHT
			NNE = p4DLFMI[current_pixel_pos + 2 - stride]; // UP 2RIGHT
			NWW = p4DLFMI[current_pixel_pos + 1 + 2*stride]; // 2DOWN RIGHT
			NNW = p4DLFMI[current_pixel_pos + 2 + stride];; // DOWN 2RIGHT
		}
#endif
#else
		// xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY) ?
		if(current_direction[dir_idx] == 'R')
		{
			W = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 1] : NOT_VALID; // RIGHT
			WW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 2, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 2] : NOT_VALID; // 2RIGHT
			N = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + stride] : NOT_VALID; // DOWN
			NN = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 2) ? p4DLFMI[current_pixel_pos + 2*stride] : NOT_VALID; // 2DOWN
			NW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + 1 + stride] : NOT_VALID; // DOWN RIGHT
			NE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos - 1 + stride] : NOT_VALID; // DOWN LEFT
			NNE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY + 2) ? p4DLFMI[current_pixel_pos - 1 + 2*stride] : NOT_VALID; // 2DOWN LEFT
			NWW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 2, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + 2 + stride] : NOT_VALID; // DOWN 2RIGHT
			NNW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY + 2) ? p4DLFMI[current_pixel_pos + 1 + 2*stride] : NOT_VALID; // 2DOWN RIGHT
		}
		else if(current_direction[dir_idx] == 'D')
		{
			W = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + stride] : NOT_VALID; // DOWN
			WW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY + 2) ? p4DLFMI[current_pixel_pos + 2*stride] : NOT_VALID; // 2DOWN
			N = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 1] : NOT_VALID; // LEFT
			NN = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 2, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 2] : NOT_VALID; // 2LEFT
			NW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos - 1 + stride] : NOT_VALID; // DOWN LEFT
			NE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - 1 - stride] : NOT_VALID; // UP LEFT
			NNE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 2, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - 2 - stride] : NOT_VALID; // UP 2LEFT
			NWW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY + 2) ? p4DLFMI[current_pixel_pos - 1 + 2*stride] : NOT_VALID; // 2DOWN LEFT
			NNW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 2, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos - 2 + stride] : NOT_VALID; // DOWN 2LEFT
		}
		else if(current_direction[dir_idx] == 'L')
		{
			W = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 1] : NOT_VALID; // LEFT
			WW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 2, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos - 2] : NOT_VALID; // 2LEFT
			N = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - stride] : NOT_VALID; // UP
			NN = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 2) ? p4DLFMI[current_pixel_pos - 2*stride] : NOT_VALID; // 2UP
			NW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - 1 - stride] : NOT_VALID; // UP LEFT
			NE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos + 1 - stride] : NOT_VALID; // UP RIGHT
			NNE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY - 2) ? p4DLFMI[current_pixel_pos + 1 - 2*stride] : NOT_VALID; // 2UP RIGHT
			NWW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 2, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - 2 - stride] : NOT_VALID; // UP 2LEFT
			NNW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX - 1, currentSAIsSpiralPosY - 2) ? p4DLFMI[current_pixel_pos - 1 - 2*stride] : NOT_VALID;// 2UP LEFT
		}
		else
		{
			W = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos - stride] : NOT_VALID; // UP
			WW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX, currentSAIsSpiralPosY - 2) ? p4DLFMI[current_pixel_pos - 2*stride] : NOT_VALID; // 2UP
			N = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 1] : NOT_VALID; // RIGHT
			NN = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 2, currentSAIsSpiralPosY) ? p4DLFMI[current_pixel_pos + 2] : NOT_VALID; // 2RIGHT
			NW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos + 1 - stride] : NOT_VALID; // UP RIGHT
			NE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + 1 + stride] : NOT_VALID; // DOWN RIGHT
			NNE = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 2, currentSAIsSpiralPosY + 1) ? p4DLFMI[current_pixel_pos + 2 + stride] : NOT_VALID; // DOWN 2RIGHT
			NWW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 1, currentSAIsSpiralPosY - 2) ? p4DLFMI[current_pixel_pos + 1 - 2*stride] : NOT_VALID; // 2UP RIGHT
			NNW = xCheckIfInTheSameRegion(currentRegionRA, current_SAI, currentSAIsSpiralPosX + 2, currentSAIsSpiralPosY - 1) ? p4DLFMI[current_pixel_pos + 2 - stride] : NOT_VALID; // UP 2RIGHT
		}
#endif
	}
	// making sure all the pixels have an assigned value
	if( !NNE )
		NNE = NE;
	if( !NN )
	{
		NN = N;
		if( !N )
		{
			NN = NE;
			N = NE;
		}
	}
	if( !NNW )
	{
		NNW = NW;
		if( !NW )
		{
			NW = N;
			NNW = N;
		}
	}
	if( !NWW )
	{
		NWW = NW;
		if( !NW )
		{
			NW = N;
			NWW = N;
		}
	}
	if( !WW )
	{
		WW = W;
		if( !W )
		{
			WW = NW;
			W = NW;
		}
	}
	//if(current_SAI >= 9)
		//if(W == NOT_VALID || WW == NOT_VALID || N == NOT_VALID || NN == NOT_VALID || NW == NOT_VALID ||	NE == NOT_VALID || NNE == NOT_VALID)
			//cout << "at least one pixel is not available" << endl;

	*w = W;
	*ww = WW;
	*n = N;
	*nn = NN;
	*nw = NW;
	*ne = NE;
	*nne = NNE;
	*nww = NWW;
	*nnw = NNW;
}

Int TComPrediction::AGSP( Int w, Int ww, Int n, Int nn, Int nw, Int ne, Int nne, Int nww, Int nnw )
{
	Double dh = (2*abs(w - ww) + 2*abs(n - nw) + 2*abs(n - ne) + abs(nn - nnw) + abs(nn - nne) + abs(nw - nww)) / 9 + 1;
	Double dv = (2*abs(w - nw) + 2*abs(n - nn) + abs(ne - nne) + abs(ww - nww) + abs(nw - nnw)) / 7 + 1;
	Double dplus = (2*abs(w - n) + 2*abs(n - nne) + abs(ww - nw) + abs(nw - nn)) / 6 + 1;
	Double dminus = (2*abs(w - nww) + 2*abs(n - nnw) + abs(ne - nn)) / 5 + 1;

	Double Dmin1 = min(min(min(dh, dv), dplus), dminus);
	Double Dmin2 = 0;
	Int Dmin1dir = 0;
	Int Dmin2dir = 0;
	Int Cmin1 = 0;
	Int Cmin2 = 0;

	if(Dmin1 == dh)
	{
		Dmin1dir = 0;
		Dmin2 = (Double)min(min(dv, dplus), dminus);
		if(Dmin2 == dv) 		Dmin2dir = 1;
		else if(Dmin2 == dplus) Dmin2dir = 2;
		else 					Dmin2dir = 3;
	}
	else if(Dmin1 == dv)
	{
		Dmin1dir = 1;
		Dmin2 = (Double)min(min(dh, dplus), dminus);
		if(Dmin2 == dh) 		Dmin2dir = 0;
		else if(Dmin2 == dplus) Dmin2dir = 2;
		else 					Dmin2dir = 3;
	}
	else if(Dmin1 == dplus)
	{
		Dmin1dir = 2;
		Dmin2 = (Double)min(min(dh, dv), dminus);
		if(Dmin2 == dv) 		Dmin2dir = 0;
		else if(Dmin2 == dplus) Dmin2dir = 1;
		else 					Dmin2dir = 3;
	}
	else
	{
		Dmin1dir = 3;
		Dmin2 = (Double)min(min(dh, dv), dplus);
		if(Dmin2 == dh) 		Dmin2dir = 0;
		else if(Dmin2 == dv) 	Dmin2dir = 1;
		else 					Dmin2dir = 2;
	}

	if(Dmin1dir == 0) 		Cmin1 = w;
	else if(Dmin1dir == 1) 	Cmin1 = n;
	else if(Dmin1dir == 2) 	Cmin1 = ne;
	else					Cmin1 = nw;

	if(Dmin2dir == 0) 		Cmin2 = w;
	else if(Dmin2dir == 1) 	Cmin2 = n;
	else if(Dmin2dir == 2) 	Cmin2 = ne;
	else					Cmin2 = nw;

	return (Int)(Dmin1*(Double)Cmin2 + Dmin2*(Double)Cmin1)/(Dmin1 + Dmin2);
}

Int TComPrediction::spiral(UInt idx, UInt size, UInt* x, UInt* y)
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
	Int last_dir_idx = 0;
	while(current_idx < idx)
	{
		last_dir_idx = dir_idx;
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

	return last_dir_idx; // return last direction
}

Int TComPrediction::spiralScalable(UInt idx, UInt size, UInt* x, UInt* y)
{
	Int layerMask13[13][13] = { {6, 6, 4, 6, 3, 6, 4, 6, 3, 6, 4, 6, 6},
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
	Int layerMask7[7][7] =    {{5, 4, 3, 4, 3, 4, 5 },
		                  	   {4, 2, 4, 2, 4, 2, 4 },
							   {3, 4, 3, 4, 3, 4, 3 },
							   {4, 2, 4, 1, 4, 2, 4 },
							   {3, 4, 3, 4, 3, 4, 3 },
							   {4, 2, 4, 2, 4, 2, 4 },
							   {5, 4, 3, 4, 3, 4, 5 }};
	Int layerMask3[3][3] =    {{2, 2, 2},
							   {2, 1, 2},
							   {2, 2, 2}};

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
	Int last_dir_idx = 0;

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
			last_dir_idx = 0;
			while(x_pos >= 0 && x_pos < size && y_pos >= 0 && y_pos < size)
			{
				last_dir_idx = dir_idx;
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
				if(size == 3)
				{
					if(layerMask3[y_pos][x_pos] == l)
					{
						current_idx++;
						if(current_idx == idx)
							break;
					}
				}
				if(size == 7)
				{
					if(layerMask7[y_pos][x_pos] == l)
					{
						current_idx++;
						if(current_idx == idx)
							break;
					}
				}
				if(size == 13)
				{
					if(layerMask13[y_pos][x_pos] == l)
					{
						current_idx++;
						if(current_idx == idx)
							break;
					}
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

	return last_dir_idx; // return last direction
}
#endif

Void TComPrediction::predIntraAng( const ComponentID compID, UInt uiDirMode, Pel* piOrg /* Will be null for decoding */, UInt uiOrgStride, Pel* piPred, UInt uiStride, TComTU &rTu, const Bool bUseFilteredPredSamples, const Bool bUseLosslessDPCM )
{
  const ChannelType    channelType = toChannelType(compID);
  const TComRectangle &rect        = rTu.getRect(isLuma(compID) ? COMPONENT_Y : COMPONENT_Cb);
  const Int            iWidth      = rect.width;
  const Int            iHeight     = rect.height;

  assert( g_aucConvertToBit[ iWidth ] >= 0 ); //   4x  4
  assert( g_aucConvertToBit[ iWidth ] <= 5 ); // 128x128
  //assert( iWidth == iHeight  );

        Pel *pDst = piPred;

  // get starting pixel in block
  const Int sw = (2 * iWidth + 1);

  if ( bUseLosslessDPCM )
  {
    const Pel *ptrSrc = getPredictorPtr( compID, false );
    // Sample Adaptive intra-Prediction (SAP)
    if (uiDirMode==HOR_IDX)
    {
      // left column filled with reference samples
      // remaining columns filled with piOrg data (if available).
      for(Int y=0; y<iHeight; y++)
      {
        piPred[y*uiStride+0] = ptrSrc[(y+1)*sw];
      }
      if (piOrg!=0)
      {
        piPred+=1; // miss off first column
        for(Int y=0; y<iHeight; y++, piPred+=uiStride, piOrg+=uiOrgStride)
        {
          memcpy(piPred, piOrg, (iWidth-1)*sizeof(Pel));
        }
      }
    }
    else // VER_IDX
    {
      // top row filled with reference samples
      // remaining rows filled with piOrd data (if available)
      for(Int x=0; x<iWidth; x++)
      {
        piPred[x] = ptrSrc[x+1];
      }
      if (piOrg!=0)
      {
        piPred+=uiStride; // miss off the first row
        for(Int y=1; y<iHeight; y++, piPred+=uiStride, piOrg+=uiOrgStride)
        {
          memcpy(piPred, piOrg, iWidth*sizeof(Pel));
        }
      }
    }
  }
  else
  {
    const Pel *ptrSrc = getPredictorPtr( compID, bUseFilteredPredSamples );

    if ( uiDirMode == PLANAR_IDX )
    {
      xPredIntraPlanar( ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight );
    }
    else
    {
      // Create the prediction
            TComDataCU *const pcCU              = rTu.getCU();
      const UInt              uiAbsPartIdx      = rTu.GetAbsPartIdxTU();
      const Bool              enableEdgeFilters = !(pcCU->isRDPCMEnabled(uiAbsPartIdx) && pcCU->getCUTransquantBypass(uiAbsPartIdx));
#if O0043_BEST_EFFORT_DECODING
      const Int channelsBitDepthForPrediction = rTu.getCU()->getSlice()->getSPS()->getStreamBitDepth(channelType);
#else
      const Int channelsBitDepthForPrediction = rTu.getCU()->getSlice()->getSPS()->getBitDepth(channelType);
#endif
      xPredIntraAng( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode, enableEdgeFilters );

#if RM_4DLF_MI_BUFFER
#if !RM_4DLF_MI_BUFFER_MULTI_MODE
      if( uiDirMode == 3 ) // less probable modes - 3, 7, 11, 15, 19, 23, 27, 31
      {
    	  TComPic *const pcPic = rTu.getCU()->getPic();
    	  TComPicYuv *const pcPic4DLFMI = pcPic->getPicYuv4DLFMI();
    	  UInt const miSize = pcPic->getMicroImageSize();
    	  UInt const currentSAIsSpiralPosX = pcPic->getCurrentSAIsSpiralPosX();
    	  UInt const currentSAIsSpiralPosY = pcPic->getCurrentSAIsSpiralPosY();
    	  UInt const totalNumberOfSAIs = pcPic->getTotalNumberOfSAIs();
    	  UInt const currentSAI = pcPic->getCurrentSAI();
    	  UInt const pcCUAbsPartIdx = pcCU->getZorderIdxInCtu();
    	  UInt const uiAbsPartIdxInRaster = g_auiZscanToRaster[pcCUAbsPartIdx];
    	  UInt const uiPosX = (uiAbsPartIdxInRaster % 16) * 4 + (pcCU->getCtuRsAddr() % pcPic->getFrameWidthInCtus()) * 64;
    	  UInt const uiPosY = (uiAbsPartIdxInRaster / 16) * 4 + (pcCU->getCtuRsAddr() / pcPic->getFrameWidthInCtus()) * 64;
#if RM_4DLF_MI_INTRA_MODE_DC_3x3
    	  xPred4DLFMI_DC_3x3( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    			  	  	      	  	  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID );
#endif
#if RM_4DLF_MI_INTRA_MODE_LOCO_I
    	  xPred4DLFMI_LOCO_I( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      			  	  	      pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID );
#endif
#if RM_4DLF_MI_INTRA_MODE_GAP
    	  xPred4DLFMI_GAP( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      	      			  	  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID );
#endif
#if RM_4DLF_MI_INTRA_MODE_AGSP
    	  xPred4DLFMI_AGSP( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      	      			  	  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID );
#endif
#if RM_4DLF_MI_INTRA_MODE_LSP
    	  xPred4DLFMI_LSP( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    			  	  	  	  	  	  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID );
#endif
      }
#else
      TComPic *const pcPic = rTu.getCU()->getPic();
      TComPicYuv * pcPic4DLFMI = pcPic->getPicYuv4DLFMI();
      UInt  miSize = pcPic->getMicroImageSize();
      UInt  currentSAIsSpiralPosX = pcPic->getCurrentSAIsSpiralPosX();
      UInt  currentSAIsSpiralPosY = pcPic->getCurrentSAIsSpiralPosY();
      UInt  totalNumberOfSAIs = pcPic->getTotalNumberOfSAIs();
      UInt  currentSAI = pcPic->getCurrentSAI();
      UInt const pcCUAbsPartIdx = pcCU->getZorderIdxInCtu();
      UInt const uiAbsPartIdxInRaster = g_auiZscanToRaster[pcCUAbsPartIdx];
      UInt const uiPosX = (uiAbsPartIdxInRaster % 16) * 4 + (pcCU->getCtuRsAddr() % pcPic->getFrameWidthInCtus()) * 64;
      UInt const uiPosY = (uiAbsPartIdxInRaster / 16) * 4 + (pcCU->getCtuRsAddr() / pcPic->getFrameWidthInCtus()) * 64;

#if RM_SCALABLE
     // cout << currentSAI << " " << currentSAIsSpiralPosY << " " << currentSAIsSpiralPosX << endl;
     // Int spiralScanOrder7[7][7], spiralScanOrder13[miSize][miSize];
     // UInt sy, sx;
      if (currentSAI < 9)
      {
    	  miSize = 3;
    	  pcPic4DLFMI = pcPic->getPicYuv4DLFMISCL3();
    	  spiralScalable(currentSAI, pcPic->getMicroImageSize(), &currentSAIsSpiralPosX, &currentSAIsSpiralPosY);
    	  currentSAIsSpiralPosX = floor(currentSAIsSpiralPosX/4);
    	  currentSAIsSpiralPosY = floor(currentSAIsSpiralPosY/4);
      }
      else if(currentSAI >= 9 && currentSAI <= 44 )
      {
    	  miSize = 7;
    	  pcPic4DLFMI = pcPic->getPicYuv4DLFMISCL7();
    	  spiralScalable(currentSAI, pcPic->getMicroImageSize(), &currentSAIsSpiralPosX, &currentSAIsSpiralPosY);
    	  currentSAIsSpiralPosX = floor(currentSAIsSpiralPosX/2);
    	  currentSAIsSpiralPosY = floor(currentSAIsSpiralPosY/2);
      }
      else // 45 -> 168
      {
    	  miSize = pcPic->getMicroImageSize();
    	  pcPic4DLFMI = pcPic->getPicYuv4DLFMISCL13();
    	  spiralScalable(currentSAI, pcPic->getMicroImageSize(), &currentSAIsSpiralPosX, &currentSAIsSpiralPosY);

      }
      totalNumberOfSAIs = miSize*miSize;
      //cout << currentSAI << " " << currentSAIsSpiralPosY << " " << currentSAIsSpiralPosX << endl;
#if RM_RANDOM_ACCESS_PROFILE
      Int currentRegionRA = getCurrentRegion(currentSAI, currentSAIsSpiralPosY, currentSAIsSpiralPosX);
#endif
#endif

#if RM_4DLF_MI_BUFFER_MULTI_MODE == 1
      if( uiDirMode == 3 ) // less probable modes - 3, 7, 11, 15, 19, 23, 27, 31
    	  xPred4DLFMI_DC_3x3( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      			  	  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,currentRegionRA
#endif
    	  );
      else if( uiDirMode == 7 )
    	  xPred4DLFMI_LOCO_I( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      	      		  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,currentRegionRA
#endif
    	  );
      else if( uiDirMode == 11 )
    	  xPred4DLFMI_GAP( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      	      	      pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,currentRegionRA
#endif
    	  );
      else if( uiDirMode == 15 )
    	  xPred4DLFMI_AGSP( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      	      	      pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID

#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,currentRegionRA
#endif
							  );
      else if( uiDirMode == 19 )
    	  xPred4DLFMI_LSP3( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      			  	  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,currentRegionRA
#endif
    	  );
      else if( uiDirMode == 23 )
    	  xPred4DLFMI_LSP5( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      			  	  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,currentRegionRA
#endif
    	  );
      else if( uiDirMode == 27 )
    	  xPred4DLFMI_LSP7( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      			  	  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID
#if RM_SCALABLE && RM_RANDOM_ACCESS_PROFILE
							  ,currentRegionRA
#endif
    	  );

      //else if( uiDirMode == 31 ) // commented because the paper doesn't include the LSP9
       //   xPred4DLFMI_LSP9( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
        //  	      			  pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID );
#endif
#if RM_4DLF_MI_BUFFER_MULTI_MODE == 2
      if( !((uiDirMode + 1) % 4) && uiDirMode <= 31 ) // less probable modes - 3, 7, 11, 15, 19, 23, 27, 31
      {
    	  Int predOrder = (Int)(0.25 * (Double)uiDirMode + 1.25);
    	  xPred4DLFMI_LSPM( channelsBitDepthForPrediction, ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType, uiDirMode,
    	      	      		pcPic4DLFMI, miSize, currentSAIsSpiralPosX, currentSAIsSpiralPosY, totalNumberOfSAIs, currentSAI, uiAbsPartIdxInRaster, uiPosX, uiPosY, compID, predOrder );
      }
#endif
#endif
#endif
      if( uiDirMode == DC_IDX )
      {
        xDCPredFiltering( ptrSrc+sw+1, sw, pDst, uiStride, iWidth, iHeight, channelType );
      }
    }
  }

}

#if RM_RANDOM_ACCESS_PROFILE
Bool TComPrediction::xCheckIfInTheSameRegion(Int currentRegion, Int currentSAI, Int currentPixelSupportPosX, Int currentPixelSupportPosY)
{

	if(currentSAI < 9) // L1 L2
	{
		if(RM_RANDOM_ACCESS_PROFILE == 1) // 2 way
		{
			if(m_2WayRAProfileLF3[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 2) // 4 way
		{
			if(m_4WayRAProfileLF3[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 3) // 9 way
		{
			if(m_9WayRAProfileLF3[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
	}
	else if(currentSAI >= 9 && currentSAI < 45) // L3 L4
	{
		if(RM_RANDOM_ACCESS_PROFILE == 1) // 2 way
		{
			if(m_2WayRAProfileLF7[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 2) // 4 way
		{
			if(m_4WayRAProfileLF7[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 3) // 9 way
		{
			if(m_9WayRAProfileLF7[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
	}
	else // L5 L6
	{
		if(RM_RANDOM_ACCESS_PROFILE == 1) // 2 way
		{
			if(m_2WayRAProfileLF13[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 2) // 4 way
		{
			if(m_4WayRAProfileLF13[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 3) // 9 way
		{
			if(m_9WayRAProfileLF13[currentRegion][currentPixelSupportPosY][currentPixelSupportPosX])
				return true;
		}
	}
	return false;
}

Int TComPrediction::getCurrentRegion(Int currentSAI, Int currentPixelPosY, Int currentPixelPosX)
{
	if(currentSAI < 9) // L1 L2
	{
		if(RM_RANDOM_ACCESS_PROFILE == 1) // 2 way
		{
			for(int r=0; r<2; r++)
				if(m_2WayRAProfileLF3[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 2) // 4 way
		{
			for(int r=0; r<4; r++)
				if(m_4WayRAProfileLF3[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 3) // 9 way
		{
			for(int r=0; r<9; r++)
				if(m_9WayRAProfileLF3[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
	}
	else if(currentSAI >= 9 && currentSAI < 45) // L3 L4
	{
		if(RM_RANDOM_ACCESS_PROFILE == 1) // 2 way
		{
			for(int r=0; r<2; r++)
				if(m_2WayRAProfileLF7[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 2) // 4 way
		{
			for(int r=0; r<4; r++)
				if(m_4WayRAProfileLF7[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 3) // 9 way
		{
			for(int r=0; r<9; r++)
				if(m_9WayRAProfileLF7[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
	}
	else // L5 L6
	{
		if(RM_RANDOM_ACCESS_PROFILE == 1) // 2 way
		{
			for(int r=0; r<2; r++)
				if(m_2WayRAProfileLF13[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 2) // 4 way
		{
			for(int r=0; r<4; r++)
				if(m_4WayRAProfileLF13[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
		else if(RM_RANDOM_ACCESS_PROFILE == 3) // 9 way
		{
			for(int r=0; r<9; r++)
				if(m_9WayRAProfileLF13[r][currentPixelPosY][currentPixelPosX])
					return r;
		}
	}
	return 0;
}

#endif

/** Check for identical motion in both motion vector direction of a bi-directional predicted CU
  * \returns true, if motion vectors and reference pictures match
 */
Bool TComPrediction::xCheckIdenticalMotion ( TComDataCU* pcCU, UInt PartAddr )
{
  if( pcCU->getSlice()->isInterB() && !pcCU->getSlice()->getPPS()->getWPBiPred() )
  {
    if( pcCU->getCUMvField(REF_PIC_LIST_0)->getRefIdx(PartAddr) >= 0 && pcCU->getCUMvField(REF_PIC_LIST_1)->getRefIdx(PartAddr) >= 0)
    {
      Int RefPOCL0 = pcCU->getSlice()->getRefPic(REF_PIC_LIST_0, pcCU->getCUMvField(REF_PIC_LIST_0)->getRefIdx(PartAddr))->getPOC();
      Int RefPOCL1 = pcCU->getSlice()->getRefPic(REF_PIC_LIST_1, pcCU->getCUMvField(REF_PIC_LIST_1)->getRefIdx(PartAddr))->getPOC();
      if(RefPOCL0 == RefPOCL1 && pcCU->getCUMvField(REF_PIC_LIST_0)->getMv(PartAddr) == pcCU->getCUMvField(REF_PIC_LIST_1)->getMv(PartAddr))
      {
        return true;
      }
    }
  }
  return false;
}

Void TComPrediction::motionCompensation ( TComDataCU* pcCU, TComYuv* pcYuvPred, RefPicList eRefPicList, Int iPartIdx )
{
  Int         iWidth;
  Int         iHeight;
  UInt        uiPartAddr;
  const TComSlice *pSlice    = pcCU->getSlice();
  const SliceType  sliceType = pSlice->getSliceType();
  const TComPPS   &pps       = *(pSlice->getPPS());

  if ( iPartIdx >= 0 )
  {
    pcCU->getPartIndexAndSize( iPartIdx, uiPartAddr, iWidth, iHeight );
    if ( eRefPicList != REF_PIC_LIST_X )
    {
      if( (sliceType == P_SLICE && pps.getUseWP()) || (sliceType == B_SLICE && pps.getWPBiPred()))
      {
        xPredInterUni (pcCU, uiPartAddr, iWidth, iHeight, eRefPicList, pcYuvPred, true );
        xWeightedPredictionUni( pcCU, pcYuvPred, uiPartAddr, iWidth, iHeight, eRefPicList, pcYuvPred );
      }
      else
      {
        xPredInterUni (pcCU, uiPartAddr, iWidth, iHeight, eRefPicList, pcYuvPred );
      }
    }
    else
    {
      if ( xCheckIdenticalMotion( pcCU, uiPartAddr ) )
      {
        xPredInterUni (pcCU, uiPartAddr, iWidth, iHeight, REF_PIC_LIST_0, pcYuvPred );
      }
      else
      {
        xPredInterBi  (pcCU, uiPartAddr, iWidth, iHeight, pcYuvPred );
      }
    }
    return;
  }

  for ( iPartIdx = 0; iPartIdx < pcCU->getNumPartitions(); iPartIdx++ )
  {
    pcCU->getPartIndexAndSize( iPartIdx, uiPartAddr, iWidth, iHeight );

    if ( eRefPicList != REF_PIC_LIST_X )
    {
      if( (sliceType == P_SLICE && pps.getUseWP()) || (sliceType == B_SLICE && pps.getWPBiPred()))
      {
        xPredInterUni (pcCU, uiPartAddr, iWidth, iHeight, eRefPicList, pcYuvPred, true );
        xWeightedPredictionUni( pcCU, pcYuvPred, uiPartAddr, iWidth, iHeight, eRefPicList, pcYuvPred );
      }
      else
      {
        xPredInterUni (pcCU, uiPartAddr, iWidth, iHeight, eRefPicList, pcYuvPred );
      }
    }
    else
    {
      if ( xCheckIdenticalMotion( pcCU, uiPartAddr ) )
      {
        xPredInterUni (pcCU, uiPartAddr, iWidth, iHeight, REF_PIC_LIST_0, pcYuvPred );
      }
      else
      {
        xPredInterBi  (pcCU, uiPartAddr, iWidth, iHeight, pcYuvPred );
      }
    }
  }
  return;
}

Void TComPrediction::xPredInterUni ( TComDataCU* pcCU, UInt uiPartAddr, Int iWidth, Int iHeight, RefPicList eRefPicList, TComYuv* pcYuvPred, Bool bi )
{
  Int         iRefIdx     = pcCU->getCUMvField( eRefPicList )->getRefIdx( uiPartAddr );           assert (iRefIdx >= 0);
  TComMv      cMv         = pcCU->getCUMvField( eRefPicList )->getMv( uiPartAddr );
  pcCU->clipMv(cMv);

  for (UInt comp=COMPONENT_Y; comp<pcYuvPred->getNumberValidComponents(); comp++)
  {
    const ComponentID compID=ComponentID(comp);
    xPredInterBlk  (compID,  pcCU, pcCU->getSlice()->getRefPic( eRefPicList, iRefIdx )->getPicYuvRec(), uiPartAddr, &cMv, iWidth, iHeight, pcYuvPred, bi, pcCU->getSlice()->getSPS()->getBitDepth(toChannelType(compID)) );
  }
}

Void TComPrediction::xPredInterBi ( TComDataCU* pcCU, UInt uiPartAddr, Int iWidth, Int iHeight, TComYuv* pcYuvPred )
{
  TComYuv* pcMbYuv;
  Int      iRefIdx[NUM_REF_PIC_LIST_01] = {-1, -1};

  for ( UInt refList = 0; refList < NUM_REF_PIC_LIST_01; refList++ )
  {
    RefPicList eRefPicList = (refList ? REF_PIC_LIST_1 : REF_PIC_LIST_0);
    iRefIdx[refList] = pcCU->getCUMvField( eRefPicList )->getRefIdx( uiPartAddr );

    if ( iRefIdx[refList] < 0 )
    {
      continue;
    }

    assert( iRefIdx[refList] < pcCU->getSlice()->getNumRefIdx(eRefPicList) );

    pcMbYuv = &m_acYuvPred[refList];
    if( pcCU->getCUMvField( REF_PIC_LIST_0 )->getRefIdx( uiPartAddr ) >= 0 && pcCU->getCUMvField( REF_PIC_LIST_1 )->getRefIdx( uiPartAddr ) >= 0 )
    {
      xPredInterUni ( pcCU, uiPartAddr, iWidth, iHeight, eRefPicList, pcMbYuv, true );
    }
    else
    {
      if ( ( pcCU->getSlice()->getPPS()->getUseWP()       && pcCU->getSlice()->getSliceType() == P_SLICE ) ||
           ( pcCU->getSlice()->getPPS()->getWPBiPred()    && pcCU->getSlice()->getSliceType() == B_SLICE ) )
      {
        xPredInterUni ( pcCU, uiPartAddr, iWidth, iHeight, eRefPicList, pcMbYuv, true );
      }
      else
      {
        xPredInterUni ( pcCU, uiPartAddr, iWidth, iHeight, eRefPicList, pcMbYuv );
      }
    }
  }

  if ( pcCU->getSlice()->getPPS()->getWPBiPred()    && pcCU->getSlice()->getSliceType() == B_SLICE  )
  {
    xWeightedPredictionBi( pcCU, &m_acYuvPred[REF_PIC_LIST_0], &m_acYuvPred[REF_PIC_LIST_1], iRefIdx[REF_PIC_LIST_0], iRefIdx[REF_PIC_LIST_1], uiPartAddr, iWidth, iHeight, pcYuvPred );
  }
  else if ( pcCU->getSlice()->getPPS()->getUseWP() && pcCU->getSlice()->getSliceType() == P_SLICE )
  {
    xWeightedPredictionUni( pcCU, &m_acYuvPred[REF_PIC_LIST_0], uiPartAddr, iWidth, iHeight, REF_PIC_LIST_0, pcYuvPred );
  }
  else
  {
    xWeightedAverage( &m_acYuvPred[REF_PIC_LIST_0], &m_acYuvPred[REF_PIC_LIST_1], iRefIdx[REF_PIC_LIST_0], iRefIdx[REF_PIC_LIST_1], uiPartAddr, iWidth, iHeight, pcYuvPred, pcCU->getSlice()->getSPS()->getBitDepths() );
  }
}

/**
 * \brief Generate motion-compensated block
 *
 * \param compID     Colour component ID
 * \param cu         Pointer to current CU
 * \param refPic     Pointer to reference picture
 * \param partAddr   Address of block within CU
 * \param mv         Motion vector
 * \param width      Width of block
 * \param height     Height of block
 * \param dstPic     Pointer to destination picture
 * \param bi         Flag indicating whether bipred is used
 * \param  bitDepth  Bit depth
 */


Void TComPrediction::xPredInterBlk(const ComponentID compID, TComDataCU *cu, TComPicYuv *refPic, UInt partAddr, TComMv *mv, Int width, Int height, TComYuv *dstPic, Bool bi, const Int bitDepth )
{
  Int     refStride  = refPic->getStride(compID);
  Int     dstStride  = dstPic->getStride(compID);
  Int shiftHor=(2+refPic->getComponentScaleX(compID));
  Int shiftVer=(2+refPic->getComponentScaleY(compID));

  Int     refOffset  = (mv->getHor() >> shiftHor) + (mv->getVer() >> shiftVer) * refStride;

  Pel*    ref     = refPic->getAddr(compID, cu->getCtuRsAddr(), cu->getZorderIdxInCtu() + partAddr ) + refOffset;

  Pel*    dst = dstPic->getAddr( compID, partAddr );

  Int     xFrac  = mv->getHor() & ((1<<shiftHor)-1);
  Int     yFrac  = mv->getVer() & ((1<<shiftVer)-1);
  UInt    cxWidth  = width  >> refPic->getComponentScaleX(compID);
  UInt    cxHeight = height >> refPic->getComponentScaleY(compID);

  const ChromaFormat chFmt = cu->getPic()->getChromaFormat();

  if ( yFrac == 0 )
  {
    m_if.filterHor(compID, ref, refStride, dst,  dstStride, cxWidth, cxHeight, xFrac, !bi, chFmt, bitDepth);
  }
  else if ( xFrac == 0 )
  {
    m_if.filterVer(compID, ref, refStride, dst, dstStride, cxWidth, cxHeight, yFrac, true, !bi, chFmt, bitDepth);
  }
  else
  {
    Int   tmpStride = m_filteredBlockTmp[0].getStride(compID);
    Pel*  tmp       = m_filteredBlockTmp[0].getAddr(compID);

    const Int vFilterSize = isLuma(compID) ? NTAPS_LUMA : NTAPS_CHROMA;

    m_if.filterHor(compID, ref - ((vFilterSize>>1) -1)*refStride, refStride, tmp, tmpStride, cxWidth, cxHeight+vFilterSize-1, xFrac, false,      chFmt, bitDepth);
    m_if.filterVer(compID, tmp + ((vFilterSize>>1) -1)*tmpStride, tmpStride, dst, dstStride, cxWidth, cxHeight,               yFrac, false, !bi, chFmt, bitDepth);
  }
}

Void TComPrediction::xWeightedAverage( TComYuv* pcYuvSrc0, TComYuv* pcYuvSrc1, Int iRefIdx0, Int iRefIdx1, UInt uiPartIdx, Int iWidth, Int iHeight, TComYuv* pcYuvDst, const BitDepths &clipBitDepths )
{
  if( iRefIdx0 >= 0 && iRefIdx1 >= 0 )
  {
    pcYuvDst->addAvg( pcYuvSrc0, pcYuvSrc1, uiPartIdx, iWidth, iHeight, clipBitDepths );
  }
  else if ( iRefIdx0 >= 0 && iRefIdx1 <  0 )
  {
    pcYuvSrc0->copyPartToPartYuv( pcYuvDst, uiPartIdx, iWidth, iHeight );
  }
  else if ( iRefIdx0 <  0 && iRefIdx1 >= 0 )
  {
    pcYuvSrc1->copyPartToPartYuv( pcYuvDst, uiPartIdx, iWidth, iHeight );
  }
}

// AMVP
Void TComPrediction::getMvPredAMVP( TComDataCU* pcCU, UInt uiPartIdx, UInt uiPartAddr, RefPicList eRefPicList, TComMv& rcMvPred )
{
  AMVPInfo* pcAMVPInfo = pcCU->getCUMvField(eRefPicList)->getAMVPInfo();

  if( pcAMVPInfo->iN <= 1 )
  {
    rcMvPred = pcAMVPInfo->m_acMvCand[0];

    pcCU->setMVPIdxSubParts( 0, eRefPicList, uiPartAddr, uiPartIdx, pcCU->getDepth(uiPartAddr));
    pcCU->setMVPNumSubParts( pcAMVPInfo->iN, eRefPicList, uiPartAddr, uiPartIdx, pcCU->getDepth(uiPartAddr));
    return;
  }

  assert(pcCU->getMVPIdx(eRefPicList,uiPartAddr) >= 0);
  rcMvPred = pcAMVPInfo->m_acMvCand[pcCU->getMVPIdx(eRefPicList,uiPartAddr)];
  return;
}

/** Function for deriving planar intra prediction.
 * \param pSrc        pointer to reconstructed sample array
 * \param srcStride   the stride of the reconstructed sample array
 * \param rpDst       reference to pointer for the prediction sample array
 * \param dstStride   the stride of the prediction sample array
 * \param width       the width of the block
 * \param height      the height of the block
 * \param channelType type of pel array (luma, chroma)
 * \param format      chroma format
 *
 * This function derives the prediction samples for planar mode (intra coding).
 */
//NOTE: Bit-Limit - 24-bit source
Void TComPrediction::xPredIntraPlanar( const Pel* pSrc, Int srcStride, Pel* rpDst, Int dstStride, UInt width, UInt height )
{
  assert(width <= height);

  Int leftColumn[MAX_CU_SIZE+1], topRow[MAX_CU_SIZE+1], bottomRow[MAX_CU_SIZE], rightColumn[MAX_CU_SIZE];
  UInt shift1Dhor = g_aucConvertToBit[ width ] + 2;
  UInt shift1Dver = g_aucConvertToBit[ height ] + 2;

  // Get left and above reference column and row
  for(Int k=0;k<width+1;k++)
  {
    topRow[k] = pSrc[k-srcStride];
  }

  for (Int k=0; k < height+1; k++)
  {
    leftColumn[k] = pSrc[k*srcStride-1];
  }

  // Prepare intermediate variables used in interpolation
  Int bottomLeft = leftColumn[height];
  Int topRight   = topRow[width];

  for(Int k=0;k<width;k++)
  {
    bottomRow[k]  = bottomLeft - topRow[k];
    topRow[k]     <<= shift1Dver;
  }

  for(Int k=0;k<height;k++)
  {
    rightColumn[k]  = topRight - leftColumn[k];
    leftColumn[k]   <<= shift1Dhor;
  }

  const UInt topRowShift = 0;

  // Generate prediction signal
  for (Int y=0;y<height;y++)
  {
    Int horPred = leftColumn[y] + width;
    for (Int x=0;x<width;x++)
    {
      horPred += rightColumn[y];
      topRow[x] += bottomRow[x];

      Int vertPred = ((topRow[x] + topRowShift)>>topRowShift);
      rpDst[y*dstStride+x] = ( horPred + vertPred ) >> (shift1Dhor+1);
    }
  }
}

/** Function for filtering intra DC predictor.
 * \param pSrc pointer to reconstructed sample array
 * \param iSrcStride the stride of the reconstructed sample array
 * \param pDst reference to pointer for the prediction sample array
 * \param iDstStride the stride of the prediction sample array
 * \param iWidth the width of the block
 * \param iHeight the height of the block
 * \param channelType type of pel array (luma, chroma)
 *
 * This function performs filtering left and top edges of the prediction samples for DC mode (intra coding).
 */
Void TComPrediction::xDCPredFiltering( const Pel* pSrc, Int iSrcStride, Pel* pDst, Int iDstStride, Int iWidth, Int iHeight, ChannelType channelType )
{
  Int x, y, iDstStride2, iSrcStride2;

  if (isLuma(channelType) && (iWidth <= MAXIMUM_INTRA_FILTERED_WIDTH) && (iHeight <= MAXIMUM_INTRA_FILTERED_HEIGHT))
  {
    //top-left
    pDst[0] = (Pel)((pSrc[-iSrcStride] + pSrc[-1] + 2 * pDst[0] + 2) >> 2);

    //top row (vertical filter)
    for ( x = 1; x < iWidth; x++ )
    {
      pDst[x] = (Pel)((pSrc[x - iSrcStride] +  3 * pDst[x] + 2) >> 2);
    }

    //left column (horizontal filter)
    for ( y = 1, iDstStride2 = iDstStride, iSrcStride2 = iSrcStride-1; y < iHeight; y++, iDstStride2+=iDstStride, iSrcStride2+=iSrcStride )
    {
      pDst[iDstStride2] = (Pel)((pSrc[iSrcStride2] + 3 * pDst[iDstStride2] + 2) >> 2);
    }
  }

  return;
}

/* Static member function */
Bool TComPrediction::UseDPCMForFirstPassIntraEstimation(TComTU &rTu, const UInt uiDirMode)
{
  return (rTu.getCU()->isRDPCMEnabled(rTu.GetAbsPartIdxTU()) ) &&
          rTu.getCU()->getCUTransquantBypass(rTu.GetAbsPartIdxTU()) &&
          (uiDirMode==HOR_IDX || uiDirMode==VER_IDX);
}

//! \}
