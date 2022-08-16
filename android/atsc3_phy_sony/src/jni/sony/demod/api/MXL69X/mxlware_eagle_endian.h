/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_endian.h
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 23/Nov/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains host message endianness helper functions header
 *
 ****************************************************************************************
 * This file is licensed under GNU General Public license, except that if you
 * have entered into a signed, written license agreement with MaxLinear covering
 * this file, that agreement applies to this file instead of the GNU General
 * Public License.
 *
 * This file is free software: you can redistribute and/or modify it under the
 * terms of the GNU General Public License, Version 2, as published by the Free
 * Software Foundation, unless a different license applies as provided above.
 *
 * This program is distributed in the hope that it will be useful, but AS-IS and
 * WITHOUT ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, or NONINFRINGEMENT. Redistribution,
 * except as permitted by the GNU General Public License or another license
 * agreement between you and MaxLinear, is prohibited.
 *
 * You should have received a copy of the GNU General Public License, Version 2
 * along with this file; if not, see<http://www.gnu.org/licenses/>.
 ****************************************************************************************
 *                                        Copyright (c) 2016, MaxLinear, Inc.
 ****************************************************************************************/
#ifndef __MXLWARE_EAGLE_ENDIAN_H__
#define __MXLWARE_EAGLE_ENDIAN_H__

#include "mxl_datatypes.h"
#include "mxl_eagle_apis.h"




/*****************************************************************************************
    Defines
 ****************************************************************************************/
#define MXLWARE_ENDIAN_SKIP_BYTES(N)        pBuffer += N;
#define MXLWARE_ENDIAN_SWAP_WORDS(N)        MxLWare_EAGLE_ENDIAN_SwapWords(pBuffer, N);   pBuffer += N*sizeof(UINT16);
#define MXLWARE_ENDIAN_SWAP_DWORDS(N)       MxLWare_EAGLE_ENDIAN_SwapDwords(pBuffer, N);  pBuffer += N*sizeof(UINT32);


/*****************************************************************************************
    Functions Prototype
 ****************************************************************************************/
void MxLWare_EAGLE_ENDIAN_TransmitSwap(MXL_EAGLE_OPCODE_E opcode, UINT8 *pBuffer);
void MxLWare_EAGLE_ENDIAN_ReceiveSwap(MXL_EAGLE_OPCODE_E opcode, UINT8 internalOpcode, UINT8 *pBuffer);
void MxLWare_EAGLE_ENDIAN_SwapWords(UINT8 *pBuffer, UINT32 numOfWords);
void MxLWare_EAGLE_ENDIAN_SwapDwords(UINT8 *pBuffer, UINT32 numOfDwords);




#endif  //__MXLWARE_EAGLE_ENDIAN_H__

