/*****************************************************************************************   
 * FILE NAME          : mxl_eagle_apis.h
 * 
 * AUTHOR             : Rotem Kryzewski
 * DATE CREATED       : 05/Oct/2016
 * LAST MODIFIED      : 
 *
 * DESCRIPTION        : This file contains Mxlware APIs header
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


/*!***************************************************************************************
 *  \addtogroup api
 *  @{
 *  \addtogroup common
 *  @{
 *
 *  \file          mxl_eagle_apis.h
 *  \details       Header file for Eagle MxLWare APIs
 *  \author        Rotem Kryzewski
 *  \date          Created: 10/1/2016
 *  \copyright     Copyright (c) 2016, MaxLinear, Inc.
 *
 ****************************************************************************************/

#ifndef __MXL_EAGLE_APIS_H__
#define __MXL_EAGLE_APIS_H__

#include "mxl_datatypes.h"

#include "IT9300.h"

/*****************************************************************************************
    Defines
 ****************************************************************************************/
#define MXL_EAGLE_INTERNAL_MESSAGE_HEADER_SIZE      2
#define MXL_EAGLE_INTERNAL_MESSAGE_BUFFER_SIZE      46  //(MXL_EAGLE_MAX_I2C_PACKET_SIZE - MXL_EAGLE_I2C_PHEADER_SIZE - MXL_EAGLE_HOST_MSG_HEADER_SIZE - MXL_EAGLE_INTERNAL_MESSAGE_HEADER_SIZE)
#define MXL_EAGLE_HOST_MSG_HEADER_SIZE  8
#define MXL_EAGLE_FW_MAX_SIZE_IN_KB     76
#define MXL_EAGLE_QAM_FFE_TAPS_LENGTH   16
#define MXL_EAGLE_QAM_SPUR_TAPS_LENGTH  32
#define MXL_EAGLE_QAM_DFE_TAPS_LENGTH   72
#define MXL_EAGLE_ATSC_FFE_TAPS_LENGTH  4096
#define MXL_EAGLE_ATSC_DFE_TAPS_LENGTH  384
#define MXL_EAGLE_VERSION_SIZE          5     //A.B.C.D-RCx
#define MXL_EAGLE_DEFAULT_SYMBOLRATE_ANNEX_A        6890000U
#define MXL_EAGLE_DEFAULT_SYMBOLRATE_ANNEX_B_QAM64  5056941U
#define MXL_EAGLE_DEFAULT_SYMBOLRATE_ANNEX_B_QAM256 5360537U



#ifdef WIN32
  #ifdef MXLWAREDLL_EXPORTS
    #define MXLWAREDLL_API __declspec(dllexport)
  #elif MXLWAREDLL_COM_EXPORTS
    #define MXLWAREDLL_API __stdcall
  #else
    #define MXLWAREDLL_API
  #endif
#else
  #define MXLWAREDLL_API
#endif


/*****************************************************************************************
    Typedefs
 ****************************************************************************************/
//ENUMS
/*! Enum of Eagle family devices */
typedef enum
{
	MXL_EAGLE_HOST_SLEEP_FW_LOAD_MSEC = 100,
	MXL_EAGLE_HOST_SLEEP_XTAL_SETTLE_MSEC = 70,
	MXL_EAGLE_HOST_SLEEP_DEVICE_SETTLE_MSEC = 100,
} MXL_EAGLE_HOST_SLEEP_E;

typedef enum
{
  MXL_EAGLE_DEVICE_691 = 1,                             //!< Device Mxl691
  MXL_EAGLE_DEVICE_248 = 2,                             //!< Device Mxl248
  MXL_EAGLE_DEVICE_692 = 3,                             //!< Device Mxl692
  MXL_EAGLE_DEVICE_MAX,                                 //!< No such device
} MXL_EAGLE_DEVICE_E;

/*! Enum of Host to Eagle I2C protocol opcodes */
typedef enum
{
  //DEVICE
  MXL_EAGLE_OPCODE_DEVICE_XTAL_CALIBRATION_SET,
  MXL_EAGLE_OPCODE_DEVICE_DEMODULATOR_TYPE_SET,
  MXL_EAGLE_OPCODE_DEVICE_MPEG_OUT_PARAMS_SET,
  MXL_EAGLE_OPCODE_DEVICE_POWERMODE_SET,
  MXL_EAGLE_OPCODE_DEVICE_GPIO_DIRECTION_SET,
  MXL_EAGLE_OPCODE_DEVICE_GPO_LEVEL_SET,
  MXL_EAGLE_OPCODE_DEVICE_INTR_MASK_SET,
  MXL_EAGLE_OPCODE_DEVICE_IO_MUX_SET,
  MXL_EAGLE_OPCODE_DEVICE_VERSION_GET,
  MXL_EAGLE_OPCODE_DEVICE_STATUS_GET,
  MXL_EAGLE_OPCODE_DEVICE_GPI_LEVEL_GET,

  //TUNER
  MXL_EAGLE_OPCODE_TUNER_CHANNEL_TUNE_SET,
  MXL_EAGLE_OPCODE_TUNER_LOCK_STATUS_GET,
  MXL_EAGLE_OPCODE_TUNER_AGC_STATUS_GET,

  //ATSC
  MXL_EAGLE_OPCODE_ATSC_INIT_SET,
  MXL_EAGLE_OPCODE_ATSC_ACQUIRE_CARRIER_SET,
  MXL_EAGLE_OPCODE_ATSC_STATUS_GET,
  MXL_EAGLE_OPCODE_ATSC_ERROR_COUNTERS_GET,
  MXL_EAGLE_OPCODE_ATSC_EQUALIZER_FILTER_DFE_TAPS_GET,
  MXL_EAGLE_OPCODE_ATSC_EQUALIZER_FILTER_FFE_TAPS_GET,

  //QAM
  MXL_EAGLE_OPCODE_QAM_PARAMS_SET,
  MXL_EAGLE_OPCODE_QAM_RESTART_SET,
  MXL_EAGLE_OPCODE_QAM_STATUS_GET,
  MXL_EAGLE_OPCODE_QAM_ERROR_COUNTERS_GET,
  MXL_EAGLE_OPCODE_QAM_CONSTELLATION_VALUE_GET,
  MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_FFE_GET,
  MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_SPUR_START_GET,
  MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_SPUR_END_GET,
  MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_TAPS_NUMBER_GET,
  MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_START_GET,
  MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_MIDDLE_GET,
  MXL_EAGLE_OPCODE_QAM_EQUALIZER_FILTER_DFE_END_GET,

  //OOB
  MXL_EAGLE_OPCODE_OOB_PARAMS_SET,
  MXL_EAGLE_OPCODE_OOB_RESTART_SET,
  MXL_EAGLE_OPCODE_OOB_ERROR_COUNTERS_GET,
  MXL_EAGLE_OPCODE_OOB_STATUS_GET,

  //SMA
  MXL_EAGLE_OPCODE_SMA_INIT_SET,
  MXL_EAGLE_OPCODE_SMA_PARAMS_SET,
  MXL_EAGLE_OPCODE_SMA_TRANSMIT_SET,
  MXL_EAGLE_OPCODE_SMA_RECEIVE_GET,

  //DEBUG
  MXL_EAGLE_OPCODE_INTERNAL,

  MXL_EAGLE_OPCODE_MAX = 70,
} MXL_EAGLE_OPCODE_E;

/*! Enum of Host to Eagle I2C protocol internal opcodes */
typedef enum
{
	//INTERNAL USAGE
	MXL_EAGLE_CUSTOMER_OPCODE_1 = 1,
	MXL_EAGLE_CUSTOMER_OPCODE_9 = 9,
	MXL_EAGLE_CUSTOMER_OPCODE_11 = 11,
	MXL_EAGLE_CUSTOMER_OPCODE_12 = 12,
} MXL_EAGLE_INTERNAL_OPCODE_NUMBER_E;

/*! Enum of Callabck function types */
typedef enum
{
  MXL_EAGLE_CB_FW_DOWNLOAD = 0,                         //!< Callback called during FW download
} MXL_EAGLE_CB_TYPE_E; 

/*! Enum of power supply types */
typedef enum
{
  MXL_EAGLE_POWER_SUPPLY_SOURCE_SINGLE,                 //!< Single supply of 3.3V
  MXL_EAGLE_POWER_SUPPLY_SOURCE_DUAL,                   //!< Dual supply of 1.8V & 3.3V
} MXL_EAGLE_POWER_SUPPLY_SOURCE_E;

/*! Enum of I/O pad drive modes */
typedef enum
{
  MXL_EAGLE_IO_MUX_DRIVE_MODE_1X,                       //!< I/O Mux drive 1X
  MXL_EAGLE_IO_MUX_DRIVE_MODE_2X,                       //!< I/O Mux drive 2X
  MXL_EAGLE_IO_MUX_DRIVE_MODE_3X,                       //!< I/O Mux drive 3X
  MXL_EAGLE_IO_MUX_DRIVE_MODE_4X,                       //!< I/O Mux drive 4X
  MXL_EAGLE_IO_MUX_DRIVE_MODE_5X,                       //!< I/O Mux drive 5X
  MXL_EAGLE_IO_MUX_DRIVE_MODE_6X,                       //!< I/O Mux drive 6X
  MXL_EAGLE_IO_MUX_DRIVE_MODE_7X,                       //!< I/O Mux drive 7X
  MXL_EAGLE_IO_MUX_DRIVE_MODE_8X,                       //!< I/O Mux drive 8X
} MXL_EAGLE_IO_MUX_DRIVE_MODE_E;                       

/*! Enum of demodulator types. Used for selection of demodulator type in relevant devices, e.g. ATSC vs. QAM in Mxl691 */
typedef enum
{
  MXL_EAGLE_DEMOD_TYPE_QAM,                             //!< Demodulator type QAM (Mxl248 or Mxl692)
  MXL_EAGLE_DEMOD_TYPE_OOB,                             //!< Demodulator type OOB (Mxl248 only) 
  MXL_EAGLE_DEMOD_TYPE_ATSC                             //!< Demodulator type ATSC (Mxl691 or Mxl692)  
} MXL_EAGLE_DEMOD_TYPE_E;

/*! Enum of power modes. Used for initial activation, or for activating sleep mode */
typedef enum
{
  MXL_EAGLE_POWER_MODE_SLEEP,                           //!< Sleep mode: running on XTAL clock, all possible blocks clocked off
  MXL_EAGLE_POWER_MODE_ACTIVE                           //!< Active mode: running on PLL clock, relevant blocks clocked on
} MXL_EAGLE_POWER_MODE_E;

/*! Enum of GPIOs, used in device GPIO APIs */
typedef enum
{
  MXL_EAGLE_GPIO_NUMBER_0,                              //!< GPIO0
  MXL_EAGLE_GPIO_NUMBER_1,                              //!< GPIO1
  MXL_EAGLE_GPIO_NUMBER_2,                              //!< GPIO2
  MXL_EAGLE_GPIO_NUMBER_3,                              //!< GPIO3
  MXL_EAGLE_GPIO_NUMBER_4,                              //!< GPIO4
  MXL_EAGLE_GPIO_NUMBER_5,                              //!< GPIO5
  MXL_EAGLE_GPIO_NUMBER_6                               //!< GPIO6
} MXL_EAGLE_GPIO_NUMBER_E;

/*! Enum of GPIO directions, used in GPIO direction configuration API */
typedef enum
{
  MXL_EAGLE_GPIO_DIRECTION_INPUT,                       //!< GPIO direction is input (GPI)
  MXL_EAGLE_GPIO_DIRECTION_OUTPUT                       //!< GPIO direction is output (GPO)
} MXL_EAGLE_GPIO_DIRECTION_E;

/*! Enum of GPIO level, used in device GPIO APIs */
typedef enum
{
  MXL_EAGLE_GPIO_LEVEL_LOW,                             //!< GPIO level is low ("0")
  MXL_EAGLE_GPIO_LEVEL_HIGH,                            //!< GPIO level is high ("1")
} MXL_EAGLE_GPIO_LEVEL_E;

/*! Enum of I/O Mux function, used in device I/O mux configuration API */
typedef enum
{
  MXL_EAGLE_IOMUX_FUNC_FEC_LOCK,                        //!< Select FEC_LOCK, overriding other pin functions (JTAG_CLK, MDAT4, GPIO3) 
  MXL_EAGLE_IOMUX_FUNC_MERR,                            //!< Select MERR, overriding other pin functions (JTAG_TDO, MDAT3, UART_RX, GPIO4) 
} MXL_EAGLE_IOMUX_FUNCTION_E;


/*! Enum of MPEG Data format, used in MPEG and OOB output configuration */
typedef enum
{
  MXL_EAGLE_DATA_SERIAL_LSB_1ST = 0,
  MXL_EAGLE_DATA_SERIAL_MSB_1ST,

  MXL_EAGLE_DATA_SYNC_WIDTH_BIT = 0,
  MXL_EAGLE_DATA_SYNC_WIDTH_BYTE
} MXL_EAGLE_MPEG_DATA_FORMAT_E;

/*! Enum of MPEG Clock format, used in MPEG and OOB output configuration */
typedef enum
{
  MXL_EAGLE_CLOCK_ACTIVE_HIGH = 0,
  MXL_EAGLE_CLOCK_ACTIVE_LOW,

  MXL_EAGLE_CLOCK_POSITIVE  = 0,
  MXL_EAGLE_CLOCK_NEGATIVE,

  MXL_EAGLE_CLOCK_IN_PHASE = 0,
  MXL_EAGLE_CLOCK_INVERTED,
} MXL_EAGLE_MPEG_CLOCK_FORMAT_E;

/*! Enum of MPEG Clock speeds, used in MPEG output configuration */
typedef enum
{
  MXL_EAGLE_MPEG_CLOCK_54MHz,
  MXL_EAGLE_MPEG_CLOCK_40_5MHz,
  MXL_EAGLE_MPEG_CLOCK_27MHz,
  MXL_EAGLE_MPEG_CLOCK_13_5MHz,
} MXL_EAGLE_MPEG_CLOCK_RATE_E;

/*! Enum of Interrupt mask bit, used in host interrupt configuration */
typedef enum
{
  MXL_EAGLE_INTR_MASK_DEMOD = 0,                        //!< Demodulator locked or lost lock
  MXL_EAGLE_INTR_MASK_SMA_RX = 1,                       //!< Smart antenna message received
  MXL_EAGLE_INTR_MASK_WDOG = 31                         //!< Watchdog expired
} MXL_EAGLE_INTR_MASK_BITS_E;

/*! Enum of QAM Demodulator type, used in QAM configuration */
typedef enum
{
  MXL_EAGLE_QAM_DEMOD_ANNEX_B,                          //!< J.83B 
  MXL_EAGLE_QAM_DEMOD_ANNEX_A,                          //!< DVB-C
} MXL_EAGLE_QAM_DEMOD_ANNEX_TYPE_E;

/*! Enum of QAM Demodulator modulation, used in QAM configuration and status */
typedef enum
{   
  MXL_EAGLE_QAM_DEMOD_QAM16,                            //!< QAM 16 modulation
  MXL_EAGLE_QAM_DEMOD_QAM64,                            //!< QAM 64 modulation
  MXL_EAGLE_QAM_DEMOD_QAM256,                           //!< QAM 256 modulation
  MXL_EAGLE_QAM_DEMOD_QAM1024,                          //!< QAM 1024 modulation
  MXL_EAGLE_QAM_DEMOD_QAM32,                            //!< QAM 32 modulation 
  MXL_EAGLE_QAM_DEMOD_QAM128,                           //!< QAM 128 modulation 
  MXL_EAGLE_QAM_DEMOD_QPSK,                             //!< QPSK modulation 
  MXL_EAGLE_QAM_DEMOD_AUTO,                             //!< Automatic modulation
} MXL_EAGLE_QAM_DEMOD_QAM_TYPE_E;

/*! Enum of Demodulator IQ setup, used in QAM, OOB configuration and status */
typedef enum
{
  MXL_EAGLE_DEMOD_IQ_NORMAL,                            //!< Normal I/Q 
  MXL_EAGLE_DEMOD_IQ_FLIPPED,                           //!< Flipped I/Q 
  MXL_EAGLE_DEMOD_IQ_AUTO,                              //!< Automatic I/Q
} MXL_EAGLE_IQ_FLIP_E;

/*! Enum of OOB Demodulator symbol rates, used in OOB configuration */
typedef enum
{
  MXL_EAGLE_OOB_DEMOD_SYMB_RATE_0_772MHz,               //!< OOB ANSI/SCTE 55-2 0.772 MHz
  MXL_EAGLE_OOB_DEMOD_SYMB_RATE_1_024MHz,               //!< OOB ANSI/SCTE 55-1 1.024 MHz
  MXL_EAGLE_OOB_DEMOD_SYMB_RATE_1_544MHz,               //!< OOB ANSI/SCTE 55-2 1.544 MHz
} MXL_EAGLE_OOB_DEMOD_SYMB_RATE_E;

/*! Enum of tuner channel tuning mode */
typedef enum
{
  MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_VIEW,               //!< Normal "view" mode - optimal performance
  MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_SCAN,               //!< Fast "scan" mode - faster tune time
} MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_E;

/*! Enum of tuner bandwidth */
typedef enum
{
  MXL_EAGLE_TUNER_BW_6MHz,
  MXL_EAGLE_TUNER_BW_7MHz,
  MXL_EAGLE_TUNER_BW_8MHz,
} MXL_EAGLE_TUNER_BW_E;

/*! Enum of tuner bandwidth */
typedef enum
{
  MXL_EAGLE_JUNCTION_TEMPERATURE_BELOW_0_CELSIUS          = 0,    //!        Temperature < 0C
  MXL_EAGLE_JUNCTION_TEMPERATURE_BETWEEN_0_TO_14_CELSIUS  = 1,    //!  0C <= Temperature < 14C
  MXL_EAGLE_JUNCTION_TEMPERATURE_BETWEEN_14_TO_28_CELSIUS = 3,    //! 14C <= Temperature < 28C
  MXL_EAGLE_JUNCTION_TEMPERATURE_BETWEEN_28_TO_42_CELSIUS = 2,    //! 28C <= Temperature < 42C
  MXL_EAGLE_JUNCTION_TEMPERATURE_BETWEEN_42_TO_57_CELSIUS = 6,    //! 42C <= Temperature < 57C
  MXL_EAGLE_JUNCTION_TEMPERATURE_BETWEEN_57_TO_71_CELSIUS = 7,    //! 57C <= Temperature < 71C
  MXL_EAGLE_JUNCTION_TEMPERATURE_BETWEEN_71_TO_85_CELSIUS = 5,    //! 71C <= Temperature < 85C
  MXL_EAGLE_JUNCTION_TEMPERATURE_ABOVE_85_CELSIUS         = 4,    //! 85C <= Temperature
} MXL_EAGLE_JUNCTION_TEMPERATURE_E;

//STRUCTS
/*! Struct passed in optional callback used during FW download */
typedef struct
{
  UINT32  totalLen;                                     //!< Total length of FW file being downloaded
  UINT32  downloadedLen;                                //!< Downloaded length so far. Percentage = downloadedLen / totalLen * 100
} MXL_EAGLE_FW_DOWNLOAD_CB_PAYLOAD_T;   


//PACKED STRUCTS
//#ifdef WIN32
//  #pragma pack (push, com_structs, 1)
//#endif

#ifdef __GNUC__
	#define PACKED( class_to_pack ) class_to_pack __attribute__((__packed__))
#else
	#pragma pack (push, com_structs, 1)
#endif


typedef __PACK_PREFIX__ struct
{
	UINT8   internalOpcode;
	UINT8   payloadLength;
	UINT8   payloadBuffer[MXL_EAGLE_INTERNAL_MESSAGE_BUFFER_SIZE];
} __PACK__ MXL_EAGLE_INTERNAL_MESSSAGE_T;
#define MXL_EAGLE_INTERNAL_MESSSAGE_SIZE (MXL_EAGLE_INTERNAL_MESSAGE_BUFFER_SIZE + 2)

/*! Struct used of I2C protocol between host and Eagle, internal use only */
typedef __PACK_PREFIX__ struct
{
  UINT8   opcode;
  UINT8   seqNum;
  UINT8   payloadSize;
  UINT8   status;
  UINT32  checksum;
} __PACK__ MXL_EAGLE_HOST_MSG_HEADER_T;


/*! Device version information struct */
typedef __PACK_PREFIX__ struct
{
  UINT8   chipId;                                       //!< See Enum MXL_EAGLE_DEVICE_E
  UINT8   firmwareVer[MXL_EAGLE_VERSION_SIZE];          //!< Firmware version, e.g. 1.0.0.0.(RC)1
  UINT8   mxlWareVer[MXL_EAGLE_VERSION_SIZE];           //!< MxLWare version, e.g. 1.0.0.0.(RC)1
} __PACK__ MXL_EAGLE_DEV_VER_T;
#define MXL_EAGLE_DEV_VER_SIZE (MXL_EAGLE_VERSION_SIZE*2 + 1)

/*! Xtal configuration struct */
typedef __PACK_PREFIX__ struct
{
  UINT8   xtalCap;                                      //!< XTAL capacitance, accepted range is 1..31 pF.  Default capacitance value is 26 pF.
  UINT8   clkOutEnable;                                 //!< See MXL_BOOL_E
  UINT8   clkOutDivEnable;                              //!< Clock out frequency is divided by 6 relative to the XTAL frequency value. see MXL_BOOL_E
  UINT8   xtalSharingEnable;                            //!< When Crystal sharing mode is enabled crystal capacitance value should be set to 25 pF. See MXL_BOOL_E
  UINT8   xtalCalibrationEnable;                        //!< Crystal calibration. Default value should be enable. See MXL_BOOL_E
                                                        //!< For Master Eagle devices set this parameter to enable
                                                        //!< For Slave Eagle devices, set this parameter to disable
} __PACK__ MXL_EAGLE_DEV_XTAL_T;
#define MXL_EAGLE_DEV_XTAL_SIZE 5

/*! GPIO direction struct, internally used in GPIO configuration API */
typedef __PACK_PREFIX__ struct
{
  UINT8   gpioNumber;
  UINT8   gpioDirection;
} __PACK__ MXL_EAGLE_DEV_GPIO_DIRECTION_T;
#define MXL_EAGLE_DEV_GPIO_DIRECTION_SIZE 2

/*! GPO level struct, internally used in GPIO configuration API */
typedef __PACK_PREFIX__ struct
{
  UINT8   gpioNumber;
  UINT8   gpoLevel;
} __PACK__ MXL_EAGLE_DEV_GPO_LEVEL_T;
#define MXL_EAGLE_DEV_GPO_LEVEL_SIZE 2

/*! Device Status struct */
typedef __PACK_PREFIX__ struct
{
  UINT8   temperature;                                  //!< See MXL_EAGLE_JUNCTION_TEMPERATURE_E
  UINT8   demodulatorType;                              //!< See MXL_EAGLE_DEMOD_TYPE_E
  UINT8   powerMode;                                    //!< See MXL_EAGLE_POWER_MODE_E
  UINT8   cpuUtilizationPercent;                        //!< Average CPU utilization [percent]
} __PACK__ MXL_EAGLE_DEV_STATUS_T;
#define MXL_EAGLE_DEV_STATUS_SIZE 4

/*! Device interrupt configuration struct */
typedef __PACK_PREFIX__ struct
{
  UINT32  intrMask;                                     //!< Interrupt mask is a bit mask, bits described in MXL_EAGLE_INTR_MASK_BITS_E
  UINT8   edgeTrigger;                                  //!< See MXL_BOOL_E
  UINT8   positiveTrigger;                              //!< See MXL_BOOL_E
  UINT8   globalEnableInterrupt;                        //!< See MXL_BOOL_E
} __PACK__ MXL_EAGLE_DEV_INTR_CFG_T;
#define MXL_EAGLE_DEV_INTR_CFG_SIZE 7

/*! MPEG pad drive parameters, used on MPEG output configuration */
typedef __PACK_PREFIX__ struct
{
  UINT8   padDrvMpegSyn;                                //!< See MXL_EAGLE_IO_MUX_DRIVE_MODE_E
  UINT8   padDrvMpegDat;                                //!< See MXL_EAGLE_IO_MUX_DRIVE_MODE_E
  UINT8   padDrvMpegVal;                                //!< See MXL_EAGLE_IO_MUX_DRIVE_MODE_E
  UINT8   padDrvMpegClk;                                //!< See MXL_EAGLE_IO_MUX_DRIVE_MODE_E
} __PACK__ MXL_EAGLE_MPEG_PAD_DRIVE_T;
#define MXL_EAGLE_MPEG_PAD_DRIVE_SIZE 4

/*! MPEGOUT parameter struct, used in MPEG output configuration */
typedef __PACK_PREFIX__ struct
{
  UINT8   mpegIsParallel;                               //!< If enabled, selects serial mode vs. parallel                                            
  UINT8   lsbOrMsbFirst;                                //!< In Serial mode, transmit MSB first or LSB. See MXL_EAGLE_MPEG_DATA_FORMAT_E
  UINT8   mpegSyncPulseWidth;                           //!< In serial mode, it can be configured with either 1 bit or 1 byte. See MXL_EAGLE_MPEG_DATA_FORMAT_E 
  UINT8   mpegValidPol;                                 //!< VALID polarity, active high or low. See MXL_EAGLE_MPEG_CLOCK_FORMAT_E
  UINT8   mpegSyncPol;                                  //!< SYNC byte(0x47) indicator, Active high or low. See MXL_EAGLE_MPEG_CLOCK_FORMAT_E
  UINT8   mpegClkPol;                                   //!< Clock polarity, Active high or low. See MXL_EAGLE_MPEG_CLOCK_FORMAT_E
  UINT8   mpeg3WireModeEnable;                          //!< In Serial mode, 0: disable 3 wire mode 1: enable 3 wire mode.
  UINT8   mpegClkFreq;                                  //!< see MXL_EAGLE_MPEG_CLOCK_RATE_E  
  MXL_EAGLE_MPEG_PAD_DRIVE_T  mpegPadDrv;               //!< Configure MPEG output pad drive strength
} __PACK__ MXL_EAGLE_MPEGOUT_PARAMS_T;
#define MXL_EAGLE_MPEGOUT_SIZE (MXL_EAGLE_MPEG_PAD_DRIVE_SIZE + 8)

/*! QAM Demodulator parameters struct, used in QAM params configuration */
typedef __PACK_PREFIX__ struct
{
  UINT8   annexType;                                    //!< See MXL_EAGLE_QAM_DEMOD_ANNEX_TYPE_E
  UINT8   qamType;                                      //!< See MXL_EAGLE_QAM_DEMOD_QAM_TYPE_E
  UINT8   iqFlip;                                       //!< See MXL_EAGLE_IQ_FLIP_E  
  UINT8   searchRangeIdx;                               //!< Equalizer frequency search range. Accepted range is 0..15. The search range depends on the current symbol rate, see documentation
  UINT8   spurCancellerEnable;                          //!< Spur cancellation enable/disable
  UINT32  symbolRateHz;                                 //!< For any QAM type in Annex-A mode, For QAM64 in Annex-B mode. Range = [2.0MHz-7.2MHz]
  UINT32  symbolRate256QamHz;                           //!< Symbol rate for QAM256 in Annex-B mode. In Annex-A, this should be the same as symbRateInHz. Range as above.
} __PACK__ MXL_EAGLE_QAM_DEMOD_PARAMS_T; 
#define MXL_EAGLE_QAM_DEMOD_PARAMS_SIZE 13

/*! QAM Demodulator status */
typedef __PACK_PREFIX__ struct
{
  UINT8   annexType;                                    //!< See MXL_EAGLE_QAM_DEMOD_ANNEX_TYPE_E
  UINT8   qamType;                                      //!< See MXL_EAGLE_QAM_DEMOD_QAM_TYPE_E
  UINT8   iqFlip;                                       //!< See MXL_EAGLE_IQ_FLIP_E  
  UINT8   interleaverDepthI;                            //!< Interleaver I depth.
  UINT8   interleaverDepthJ;                            //!< Interleaver J depth.
  UINT8   isQamLocked;                                  //!< See MXL_BOOL_E
  UINT8   isFecLocked;                                  //!< See MXL_BOOL_E
  UINT8   isMpegLocked;                                 //!< See MXL_BOOL_E
  UINT16  snrDbTenth;                                   //!< Current SNR value. The returned value is in x10dB format (241 = 24.1dB)
  SINT16  timingOffset;                                 //!< Timing recovery offset in units of parts per million (ppm)
  SINT32  carrierOffsetHz;                              //!< Current frequency offset value in [Hz]
} __PACK__ MXL_EAGLE_QAM_DEMOD_STATUS_T;  
#define MXL_EAGLE_QAM_DEMOD_STATUS_SIZE 16

/*! QAM Demodulator error counters */
typedef __PACK_PREFIX__ struct
{
  UINT32  correctedCodeWords;                           //!< Corrected code words
  UINT32  uncorrectedCodeWords;                         //!< Uncorrected code words
  UINT32  totalCodeWordsReceived;                       //!< Total received code words
  UINT32  correctedBits;                                //!< Counter for corrected bits
  UINT32  errorMpegFrames;                              //!< Counter for error MPEG frames
  UINT32  mpegFramesReceived;                           //!< Counter for received MPEG frames
  UINT32  erasures;                                     //!< Counter for erasures
} __PACK__ MXL_EAGLE_QAM_DEMOD_ERROR_COUNTERS_T;
#define MXL_EAGLE_QAM_DEMOD_ERROR_COUNTERS_SIZE 28

/*! QAM Demodulator constellation point */
typedef __PACK_PREFIX__ struct
{
  SINT16  iValue[12];                                   //!< 12 I samples
  SINT16  qValue[12];                                   //!< 12 Q samples
} __PACK__ MXL_EAGLE_QAM_DEMOD_CONSTELLATION_VAL_T;
#define MXL_EAGLE_QAM_DEMOD_CONSTELLATION_VAL_SIZE 48

/*! QAM Demodulator equalizer filter taps */
typedef __PACK_PREFIX__ struct
{
  SINT16  ffeTaps[MXL_EAGLE_QAM_FFE_TAPS_LENGTH];       //!< FFE filter
  SINT16  spurTaps[MXL_EAGLE_QAM_SPUR_TAPS_LENGTH];     //!< Spur filter
  SINT16  dfeTaps[MXL_EAGLE_QAM_DFE_TAPS_LENGTH];       //!< DFE filter
  UINT8   ffeLeadingTapIndex;                           //!< Location of leading tap (in FFE)
  UINT8   dfeTapsNumber;                                //!< Number of taps in DFE
} __PACK__ MXL_EAGLE_QAM_DEMOD_EQU_FILTER_T;

/*! OOB Demodulator parameters struct, used in OOB params configuration */
typedef __PACK_PREFIX__ struct
{
  UINT8   symbolRate;                                   //!< See MXL_EAGLE_OOB_DEMOD_SYMB_RATE_E
  UINT8   iqFlip;                                       //!< See MXL_EAGLE_IQ_FLIP_E
  UINT8   clockPolarity;                                //!< See MXL_EAGLE_MPEG_CLOCK_FORMAT_E
} __PACK__ MXL_EAGLE_OOB_DEMOD_PARAMS_T;   
#define MXL_EAGLE_OOB_DEMOD_PARAMS_SIZE 3

/*! OOB Demodulator error counters */
typedef __PACK_PREFIX__ struct
{
  UINT32  correctedPackets;                             //!< Corrected packets             
  UINT32  uncorrectedPackets;                           //!< Uncorrected packets           
  UINT32  totalPacketsReceived;                         //!< Total received packets        
} __PACK__ MXL_EAGLE_OOB_DEMOD_ERROR_COUNTERS_T;  
#define MXL_EAGLE_OOB_DEMOD_ERROR_COUNTERS_SIZE 12

/*! OOB Demodulator status */
typedef __PACK_PREFIX__ struct
{
  UINT16  snrDbTenth;                                   //!< Current SNR value. The returned value is in x10dB format (241 = 24.1dB)
  SINT16  timingOffset;                                 //!< Timing recovery offset in units of parts per million (ppm)
  SINT32  carrierOffsetHz;                              //!< Current frequency offset value in [Hz]
  UINT8   isQamLocked;                                  //!< See MXL_BOOL_E
  UINT8   isFecLocked;                                  //!< See MXL_BOOL_E
  UINT8   isMpegLocked;                                 //!< See MXL_BOOL_E
  UINT8   iqFlip;                                       //!< See MXL_EAGLE_IQ_FLIP_E
} __PACK__ MXL_EAGLE_OOB_DEMOD_STATUS_T;
#define MXL_EAGLE_OOB_DEMOD_STATUS_SIZE 12

/*! ATSC Demodulator status */
typedef __PACK_PREFIX__ struct
{
  SINT16  snrDbTenths;                                  //!< Current SNR value. The returned value is in x10dB format (241 = 24.1dB)
  SINT16  timingOffset;                                 //!< Timing recovery offset in units of parts per million (ppm)
  SINT32  carrierOffsetHz;                              //!< Current frequency offset value in [Hz].
  UINT8   isFrameLock;                                  //!< See MXL_BOOL_E
  UINT8   isAtscLock;                                   //!< See MXL_BOOL_E
  UINT8   isFecLock;                                    //!< See MXL_BOOL_E
} __PACK__ MXL_EAGLE_ATSC_DEMOD_STATUS_T;
#define MXL_EAGLE_ATSC_DEMOD_STATUS_SIZE 11


/*! ATSC Demodulator error counters */
typedef __PACK_PREFIX__ struct
{
  UINT32  errorPackets;                                 //!< Number of error packets
  UINT32  totalPackets;                                 //!< Total number of packets
  UINT32  errorBytes;                                   //!< Number of error bytes (pre-FEC)
} __PACK__ MXL_EAGLE_ATSC_DEMOD_ERROR_COUNTERS_T;
#define MXL_EAGLE_ATSC_DEMOD_ERROR_COUNTERS_SIZE 12

/*! ATSC Demodulator equalizers filter taps */
typedef __PACK_PREFIX__ struct
{
  SINT16  ffeTaps[MXL_EAGLE_ATSC_FFE_TAPS_LENGTH];      //!< Frequency domain feed-forward complex filter
  SINT8   dfeTaps[MXL_EAGLE_ATSC_DFE_TAPS_LENGTH];      //!< Decision feed-back filter
} __PACK__ MXL_EAGLE_ATSC_DEMOD_EQU_FILTER_T;

/*! Tuner AGC Status */
typedef __PACK_PREFIX__ struct
{
  UINT8   isLocked;                                     //!< AGC lock indication, see MXL_BOOL_E
  UINT16  rawAgcGain;                                   //!< AGC gain [dB] = rawAgcGain / 2^6
  SINT16  rxPowerDbHundredths;                          //!< Current Rx power. The returned value is in x100dB format (241 = 2.41dB)
} __PACK__ MXL_EAGLE_TUNER_AGC_STATUS_T;
#define MXL_EAGLE_TUNER_AGC_STATUS_SIZE 5

/*! Tuner channel tune parameters */
typedef __PACK_PREFIX__ struct
{
	UINT32  freqInHz;                                     //!< Channel Center Frequency in HZ, Range = [44MHz-1006MHz]
  UINT8   tuneMode;                                     //!< see MXL_EAGLE_TUNER_CHANNEL_TUNE_MODE_E (view vs. scan)
  UINT8   bandWidth;                                    //!< see MXL_EAGLE_TUNER_BW_E (6MHz to 8MHz)
} __PACK__ MXL_EAGLE_TUNER_CHANNEL_PARAMS_T;
#define MXL_EAGLE_TUNER_CHANNEL_PARAMS_SIZE 6

/*! Tuner channel lock indications */
typedef __PACK_PREFIX__ struct
{
  UINT8   isRfPllLocked;                                //!< Status of Tuner RF synthesizer Lock
  UINT8   isRefPllLocked;                               //!< Status of Tuner Ref synthesizer Lock
} __PACK__ MXL_EAGLE_TUNER_LOCK_STATUS_T;
#define MXL_EAGLE_TUNER_LOCK_STATUS_SIZE 2

/*! Smart antenna parameters struct, used in Smart antenna params configuration */
typedef __PACK_PREFIX__ struct
{
  UINT8   fullDuplexEnable;                             //!< See MXL_BOOL_E, half duplex disables RX while transmitting, full duplex intended mainly for debug
  UINT8   rxDisable;                                    //!< See MXL_BOOL_E, e.g ANSI-CTA-909B Mode A operation doesn't require RX, saves power
  UINT8   idleLogicHigh;                                //!< See MXL_BOOL_E, set polarity of both TX and RX signals to be idle logic high
} __PACK__ MXL_EAGLE_SMA_PARAMS_T;
#define MXL_EAGLE_SMA_PARAMS_SIZE 3

/*! Smart antenna message format */
typedef __PACK_PREFIX__ struct
{
  UINT32  payloadBits;                                  //!< Payload bits. Payload bits are sent and received LSB first
  UINT8   totalNumBits;                                 //!< Number of valid bits in the payload, in the range [8..31]
} __PACK__ MXL_EAGLE_SMA_MESSAGE_T;
#define MXL_EAGLE_SMA_MESSAGE_SIZE 5


#ifdef WIN32
  #pragma pack(pop, com_structs)
#endif

/** @} common */
/** @} api */


//STRUCTS
typedef struct
{
	UINT8               seqNum;
	MXL_BOOL_E          isInitialized;
	MXL_EAGLE_DEVICE_E  deviceType;


	Endeavour*			endeavour_mxl69x;
	unsigned char		endeavour_mxl69x_i2cBus;
	unsigned char		endeavour_mxl69x_i2cAdd;
	
	UINT8				mode;

} MXL_EAGLE_DEV_CONTEXT_T;

/*****************************************************************************************
    Functions Prototype
 ****************************************************************************************/
// Driver
MXL_STATUS_E MxLWare_EAGLE_API_CfgDrvInit							(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, void *pOemData, MXL_EAGLE_DEVICE_E deviceType, unsigned char i2c_bus, unsigned char i2c_add);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDrvUnInit							(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device);

// Device
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevReset							(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevPowerRegulators				(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_POWER_SUPPLY_SOURCE_E powerSupply);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevXtal							(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEV_XTAL_T *pDevXtalStruct);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevLoopthrough					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_BOOL_E enableRfLoopThru);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevFwDownload						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pFwBuffer, UINT32 bufferLen, MXL_CALLBACK_FN_T pCallbackFn);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevDemodType						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEMOD_TYPE_E demodType);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevMpegOutParams					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_MPEGOUT_PARAMS_T *pMpegOutParamStruct);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevPowerMode						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_POWER_MODE_E powerMode);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevIoMux							(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_IOMUX_FUNCTION_E func);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevIntrMask						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEV_INTR_CFG_T *pIntrCfgStruct);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevGpioDirection					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_GPIO_NUMBER_E gpioNumber, MXL_EAGLE_GPIO_DIRECTION_E gpioDirection);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevGpoLevel						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_GPIO_NUMBER_E gpioNumber, MXL_EAGLE_GPIO_LEVEL_E gpoLevel);
MXL_STATUS_E MxLWare_EAGLE_API_CfgDevRegister						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 regAddress, UINT32 regValue);
MXL_STATUS_E MxLWare_EAGLE_API_ReqDevRegister						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 regAddress, UINT32 *pRegValue);
MXL_STATUS_E MxLWare_EAGLE_API_ReqDevVersionInfo					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEV_VER_T *pDevVerStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqDevStatus							(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_DEV_STATUS_T *pStatusStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqDevGpiLevel						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_GPIO_NUMBER_E gpioNumber, MXL_EAGLE_GPIO_LEVEL_E *pGpiLevel);
MXL_STATUS_E MxLWare_EAGLE_API_ReqDevIntrStatus						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT32 *pIntrStatus);

// Tuner
MXL_STATUS_E MxLWare_EAGLE_API_CfgTunerChannelTune					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_TUNER_CHANNEL_PARAMS_T *pChannelTuneStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqTunerLockStatus					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_TUNER_LOCK_STATUS_T *pTunerLockStatusStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqTunerAgcStatus					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_TUNER_AGC_STATUS_T *pTunerAgcStatusStruct);

// ATSC
MXL_STATUS_E MxLWare_EAGLE_API_CfgAtscDemodInit						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device);
MXL_STATUS_E MxLWare_EAGLE_API_CfgAtscDemodAcquireCarrier			(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device);
MXL_STATUS_E MxLWare_EAGLE_API_ReqAtscDemodStatus					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_ATSC_DEMOD_STATUS_T *pAtscStatStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqAtscDemodErrorCounters			(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_ATSC_DEMOD_ERROR_COUNTERS_T *pErrCountersStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqAtscDemodEqualizerFilter			(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_ATSC_DEMOD_EQU_FILTER_T *pEquFilterStruct);

// QAM
MXL_STATUS_E MxLWare_EAGLE_API_CfgQamDemodParams					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_PARAMS_T *pQamParamsStruct);
MXL_STATUS_E MxLWare_EAGLE_API_CfgQamDemodRestart					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device);
MXL_STATUS_E MxLWare_EAGLE_API_ReqQamDemodStatus					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_STATUS_T *pQamStatusStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqQamDemodErrorCounters				(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_ERROR_COUNTERS_T *pErrCountersStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqQamDemodConstellationValue		(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_CONSTELLATION_VAL_T *pConstValStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqQamDemodEqualizerFilter			(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_QAM_DEMOD_EQU_FILTER_T *pEquFilterStruct);

// OOB
MXL_STATUS_E MxLWare_EAGLE_API_CfgOobDemodParams					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_OOB_DEMOD_PARAMS_T *pOobParamsStruct);
MXL_STATUS_E MxLWare_EAGLE_API_CfgOobDemodRestart					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device);
MXL_STATUS_E MxLWare_EAGLE_API_ReqOobDemodErrorCounters				(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_OOB_DEMOD_ERROR_COUNTERS_T *pErrCountersStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqOobDemodStatus					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_OOB_DEMOD_STATUS_T *pOobStatusStruct);

// SMA
MXL_STATUS_E MxLWare_EAGLE_API_CfgSmaInit							(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device);
MXL_STATUS_E MxLWare_EAGLE_API_CfgSmaParams							(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_SMA_PARAMS_T *pSmaParamsStruct);
MXL_STATUS_E MxLWare_EAGLE_API_CfgSmaMsgTransmit					(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_SMA_MESSAGE_T *pSmaMessageStruct);
MXL_STATUS_E MxLWare_EAGLE_API_ReqSmaMsgReceive						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, MXL_EAGLE_SMA_MESSAGE_T *pSmaMessageStruct);

// DEBUG
MXL_STATUS_E MxLWare_EAGLE_API_ReqDebugBuffer						(UINT8 devId, MXL_EAGLE_DEV_CONTEXT_T* pMxL_EAGLE_Device, UINT8 *pBuffer, UINT8 bufferSize, UINT8 debugNumber);

#endif  //__MXL_EAGLE_APIS_H__
