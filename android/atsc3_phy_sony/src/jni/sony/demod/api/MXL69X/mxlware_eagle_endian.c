/*****************************************************************************************   
 * FILE NAME          : mxlware_eagle_endian.c
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 23/11/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains host message endianness helper functions
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
#include "mxl_datatypes.h"
#include "mxlware_eagle_endian.h"
#include "mxlware_eagle_utils.h"


/*****************************************************************************************
Global Variables
****************************************************************************************/
extern MXL_BOOL_E gIsBigEndian;

/*****************************************************************************************
    Global Variables
 ****************************************************************************************/
extern MXL_BOOL_E gIsBigEndian;


/*****************************************************************************************
    Functions
 ****************************************************************************************/
/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_ENDIAN_SwapWords
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 23/11/2016
*
* \brief          This function is a helper function for swapping words endianness
* 
* \param[in]      pBuffer               Pointer to buffer being swapped
* \param[in]      numOfWords            Num of words to swap
*
* \returns        void 
*-------------------------------------------------------------------------------------*/
void MxLWare_EAGLE_ENDIAN_SwapWords(UINT8 *pBuffer, UINT32 numOfWords)
{
  UINT32 ix;

  for (ix=numOfWords; ix>0; ix--)
  {
	  MxL_EAGLE_UTILS_EndiannessMemCpy(pBuffer, pBuffer, sizeof(UINT16), MXL_EAGLE_LITTLE_ENDIAN);
    pBuffer += sizeof(UINT16);
  }
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_ENDIAN_SwapDwords
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 23/11/2016
*
* \brief          This function is a helper function for swapping dwords endianness
* 
* \param[in]      pBuffer               Pointer to buffer being swapped
* \param[in]      numOfDwords           Num of dwords to swap
*
* \returns        void 
*-------------------------------------------------------------------------------------*/
void MxLWare_EAGLE_ENDIAN_SwapDwords(UINT8 *pBuffer, UINT32 numOfDwords)
{
  UINT32 ix;

  for (ix=numOfDwords; ix>0; ix--)
  {
	  MxL_EAGLE_UTILS_EndiannessMemCpy(pBuffer, pBuffer, sizeof(UINT32), MXL_EAGLE_LITTLE_ENDIAN);
    pBuffer += sizeof(UINT32);
  }
}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_ENDIAN_TransmitSwap
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 23/11/2016
*
* \brief          This function swaps the endianness of TX request based on the opcode
* 
* \param[in]      opcode                Opcode associated with the transmission
* \param[in]      pBuffer               Pointer to buffer being sent
*
* \returns        void 
*-------------------------------------------------------------------------------------*/
void MxLWare_EAGLE_ENDIAN_TransmitSwap(MXL_EAGLE_OPCODE_E opcode, UINT8 *pBuffer)
{

    //skip API header
    MXLWARE_ENDIAN_SKIP_BYTES(8);

    //swap API structs
    switch (opcode)
    {
      //DEVICE
      case MXL_EAGLE_OPCODE_DEVICE_DEMODULATOR_TYPE_SET:
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_MPEG_OUT_PARAMS_SET:
        MXLWARE_ENDIAN_SKIP_BYTES(12);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_POWERMODE_SET:
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_GPIO_DIRECTION_SET:
        MXLWARE_ENDIAN_SKIP_BYTES(2);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_GPO_LEVEL_SET:
        MXLWARE_ENDIAN_SKIP_BYTES(2);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_GPI_LEVEL_GET:  //set and get API
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_IO_MUX_SET:
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_INTR_MASK_SET:
        MXLWARE_ENDIAN_SWAP_DWORDS(1);
        MXLWARE_ENDIAN_SKIP_BYTES(3);
        break;

      //TUNER
      case MXL_EAGLE_OPCODE_TUNER_CHANNEL_TUNE_SET:
        MXLWARE_ENDIAN_SWAP_DWORDS(1);
        MXLWARE_ENDIAN_SKIP_BYTES(2);
        break;

      //ATSC
      case MXL_EAGLE_OPCODE_ATSC_INIT_SET:
        //no swapping
        break;

      case MXL_EAGLE_OPCODE_ATSC_ACQUIRE_CARRIER_SET:
        //no swapping
        break;

      case MXL_EAGLE_OPCODE_ATSC_EQUALIZER_FILTER_DFE_TAPS_GET:
      case MXL_EAGLE_OPCODE_ATSC_EQUALIZER_FILTER_FFE_TAPS_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        break;

      //QAM
      case MXL_EAGLE_OPCODE_QAM_PARAMS_SET:
        MXLWARE_ENDIAN_SKIP_BYTES(5);
        MXLWARE_ENDIAN_SWAP_DWORDS(2);
        break;

      case MXL_EAGLE_OPCODE_QAM_RESTART_SET:
        //no swapping
        break;

      //OOB
      case MXL_EAGLE_OPCODE_OOB_PARAMS_SET:
        MXLWARE_ENDIAN_SKIP_BYTES(3);
        break;

      case MXL_EAGLE_OPCODE_OOB_RESTART_SET:
        //no swapping
        break;

      //SMA
      case MXL_EAGLE_OPCODE_SMA_INIT_SET:
        //no swapping
        break;

      case MXL_EAGLE_OPCODE_SMA_PARAMS_SET:
        //no swapping
        break;

      case MXL_EAGLE_OPCODE_SMA_TRANSMIT_SET:
        MXLWARE_ENDIAN_SWAP_DWORDS(1);
        break;

	  case MXL_EAGLE_OPCODE_INTERNAL:
		  switch (*pBuffer)
		  {
		  case MXL_EAGLE_CUSTOMER_OPCODE_9:
			  MXLWARE_ENDIAN_SKIP_BYTES(2);
			  MXLWARE_ENDIAN_SWAP_WORDS(1);
			  break;

		  case MXL_EAGLE_CUSTOMER_OPCODE_11:
			  MXLWARE_ENDIAN_SKIP_BYTES(2);
			  MXLWARE_ENDIAN_SWAP_DWORDS(2);
			  break;

		  case MXL_EAGLE_CUSTOMER_OPCODE_12:
			  MXLWARE_ENDIAN_SKIP_BYTES(2);
			  MXLWARE_ENDIAN_SWAP_DWORDS(1);
			  break;

		  default:
			  break;
		  }
		  break;

      default:
        //no swapping - all get opcodes
        break;
    }

}

/**----------------------------------------------------------------------------------------
* FUNCTION NAME : MxLWare_EAGLE_ENDIAN_ReceiveSwap
*
* AUTHOR        : Rotem Kryzewski
* 
* DATE CREATED  : 23/11/2016
*
* \brief          This function swaps the endianness of RX response based on the opcode
* \param[in]      internalOpcode        Internal Opcode associated with the received buffer
* \param[in]      opcode                Opcode associated with the received buffer
* \param[in]      pBuffer               Pointer to buffer being received
*
* \returns        void 
*-------------------------------------------------------------------------------------*/
void MxLWare_EAGLE_ENDIAN_ReceiveSwap(MXL_EAGLE_OPCODE_E opcode, UINT8 internalOpcode, UINT8 *pBuffer)
{


    //skip API header
    MXLWARE_ENDIAN_SKIP_BYTES(8);

    //swap API structs
    switch (opcode)
    {
      //DEVICE
      case MXL_EAGLE_OPCODE_DEVICE_VERSION_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(11);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_STATUS_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(4);
        break;

      case MXL_EAGLE_OPCODE_DEVICE_GPI_LEVEL_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        break;

      //TUNER
      case MXL_EAGLE_OPCODE_TUNER_LOCK_STATUS_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(2);
        break;

      case MXL_EAGLE_OPCODE_TUNER_AGC_STATUS_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        MXLWARE_ENDIAN_SWAP_WORDS(2);
        break;

      //ATSC
      case MXL_EAGLE_OPCODE_ATSC_STATUS_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(2);
        MXLWARE_ENDIAN_SWAP_DWORDS(1);
        MXLWARE_ENDIAN_SKIP_BYTES(3);
        break;

      case MXL_EAGLE_OPCODE_ATSC_ERROR_COUNTERS_GET:
        MXLWARE_ENDIAN_SWAP_DWORDS(3);
        break;

      case MXL_EAGLE_OPCODE_ATSC_EQUALIZER_FILTER_DFE_TAPS_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(48);
        break;

      case MXL_EAGLE_OPCODE_ATSC_EQUALIZER_FILTER_FFE_TAPS_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(24);
        break;

      //QAM
      case MXL_EAGLE_OPCODE_QAM_STATUS_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(8);
        MXLWARE_ENDIAN_SWAP_WORDS(2);
        MXLWARE_ENDIAN_SWAP_DWORDS(1);
        break;

      case MXL_EAGLE_OPCODE_QAM_ERROR_COUNTERS_GET:
        MXLWARE_ENDIAN_SWAP_DWORDS(7);
        break;

      case MXL_EAGLE_OPCODE_QAM_CONSTELLATION_VALUE_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(24);
        break;

      case MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_FFE_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(17);
        break;

      case MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_SPUR_START_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(24);
        break;

      case MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_SPUR_END_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(8);
        break;

      case MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_TAPS_NUMBER_GET:
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        break;

      case MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_START_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(24);
        break;

      case MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_MIDDLE_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(24);
        break;

      case MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_END_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(24);
        break;

      //OOB
      case MXL_EAGLE_OPCODE_OOB_ERROR_COUNTERS_GET:
        MXLWARE_ENDIAN_SWAP_DWORDS(3);
        break;

      case MXL_EAGLE_OPCODE_OOB_STATUS_GET:
        MXLWARE_ENDIAN_SWAP_WORDS(2);
        MXLWARE_ENDIAN_SWAP_DWORDS(1);
        MXLWARE_ENDIAN_SKIP_BYTES(4);
        break;

      //SMA
      case MXL_EAGLE_OPCODE_SMA_RECEIVE_GET:
        MXLWARE_ENDIAN_SWAP_DWORDS(1);
        MXLWARE_ENDIAN_SKIP_BYTES(1);
        break;

	  case MXL_EAGLE_OPCODE_INTERNAL:
		  switch (internalOpcode)
		  {
		  case MXL_EAGLE_CUSTOMER_OPCODE_1:
			  MXLWARE_ENDIAN_SWAP_DWORDS(1);
			  break;

		  case MXL_EAGLE_CUSTOMER_OPCODE_12:
			  MXLWARE_ENDIAN_SWAP_DWORDS(1);
			  break;

		  default:
			  break;
		  }
		  break;

      default:
        //no swapping - all set opcodes
        break;
    }

}

