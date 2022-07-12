/*************************************************************************** */
/* Saankhya Confidential                                                     */
/* COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved           */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided “AS IS”, WITH ALL FAULTS.           */
/*                                                                           */
/* Saankhya Labs does not represent or warrant that the LICENSED MATERIALS   */
/* provided here under is free of infringement of any third party patents,   */
/* copyrights, trade secrets or other intellectual property rights.          */
/* ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE EXCLUDED TO  */
/* THE FULLEST EXTENT PERMITTED BY LAW                                       */
/* NOTICE: All information contained herein is, and remains the property of  */
/* Saankhya Labs Pvt. Ltd and its suppliers, if any. The intellectual and    */
/* technical concepts contained herein are proprietary to Saankhya Labs & its*/
/* suppliers and may be covered by U.S. and Foreign Patents, patents in      */
/* process, and are protected by trade secret or copyright law. Dissemination*/
/* of this information or reproduction of this material is strictly forbidden*/
/* unless prior written permission is obtained from Saankhya Labs Pvt Ltd.   */
/*                                                                           */
/*  File Name   :   sl_demod.h                                               */
/*  version     :   0.11                                                     */
/*  Date        :   24/11/2021                                               */
/*  Description :   SLDemod API Header File                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_DEMOD_H_
#define _SL_DEMOD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */

    /* Defines */
    #define EXT_LNA_NOT_PRESENT         (0x00000000)
    #define EXT_LNA_PRESENT             (0x00000001)
    #define EXT_LNA_MODE_AUTO           (0x00000000)
    #define EXT_LNA_MODE_MANUAL         (0x00000002)
    #define EXT_LNA_MODE_MANUAL_BYPASS  (0x00000000)
    #define EXT_LNA_MODE_MANUAL_ENABLE  (0x00000004)

/* Typedefs */
typedef enum
{
    SL_DEMODSTD_ATSC3_0 = 0,
    SL_DEMODSTD_ATSC1_0,
    SL_DEMODSTD_DVB_T,
    SL_DEMODSTD_DVB_T2,
    SL_DEMODSTD_DTMB,
    SL_DEMODSTD_DVB_S,
    SL_DEMODSTD_DVB_S2,
    SL_DEMODSTD_DVB_C,
    SL_DEMODSTD_ISDB_C,
    SL_DEMODSTD_ISDB_T,
    SL_DEMODSTD_US_C,
    SL_DEMODSTD_INT_CALIBRATION = 100
}SL_DemodStd_t;

    typedef enum
    {
    SL_CMD_CONTROL_IF_I2C,
    SL_CMD_CONTROL_IF_SPI,
    SL_CMD_CONTROL_IF_SDIO
}SL_CmdControlIf_t;

typedef enum
{
    SL_CONFIGTYPE_AFEIF = 0,
        SL_CONFIGTYPE_IQ_OFFSET_CORRECTION,
    SL_CONFIGTYPE_OUTPUTIF,
    SL_CONFIGTYPE_TUNED_RF_FREQ,
        SL_CONFIGTYPE_TUNER_CRYSTAL_FREQ,
        SL_CONFIGTYPE_EXT_LNA
    }SL_ConfigType_t;

typedef enum
{
    SL_IFTYPE_ZIF = 0,
    SL_IFTYPE_LIF
}SL_IfType_t;

typedef enum
{
    SL_SPECTRUM_NORMAL = 0,
    SL_SPECTRUM_INVERTED
}SL_Spectrum_t;

typedef enum
{
    SL_IQSWAP_DISABLE = 0,
    SL_IQSWAP_ENABLE
}SL_IQSwap_t;

typedef enum
{
    SL_IPOL_SWAP_DISABLE = 0,
    SL_IPOL_SWAP_ENABLE
}SL_IPolaritySwap_t;

typedef enum
{
    SL_QPOL_SWAP_DISABLE = 0,
    SL_QPOL_SWAP_ENABLE
}SL_QPolaritySwap_t;

typedef struct
{
    float iCoeff1;
    float qCoeff1;
    float iCoeff2;
    float qCoeff2;
}SL_IQOffsetCorrectionParams_t;

typedef struct
{
    SL_IfType_t iftype;
    SL_IQSwap_t iqswap;
    SL_IPolaritySwap_t  iswap;
    SL_QPolaritySwap_t  qswap;
    SL_Spectrum_t spectrum;
    double ifreq; // unit MHz
    unsigned int agcRefValue;  // unit mV
}SL_AfeIfConfigParams_t;

typedef enum
{
    SL_OUTPUTIF_TSPARALLEL_LSB_FIRST = 0,
    SL_OUTPUTIF_TSPARALLEL_MSB_FIRST,
    SL_OUTPUTIF_TSSERIAL_LSB_FIRST,
    SL_OUTPUTIF_TSSERIAL_MSB_FIRST,
    SL_OUTPUTIF_SDIO,
    SL_OUTPUTIF_SPI
}SL_OutputIf_t;

typedef enum
{
    SL_TSO_CLK_INV_OFF = 0,
    SL_TSO_CLK_INV_ON
}SL_TsoClockInv_t;

typedef struct
{
    SL_OutputIf_t    oif;
    SL_TsoClockInv_t TsoClockInvEnable;
}SL_OutIfConfigParams_t;

    typedef enum
    {
        SL_EXT_LNA_CFG_MODE_NOT_PRESENT   = EXT_LNA_NOT_PRESENT,
        SL_EXT_LNA_CFG_MODE_AUTO          = EXT_LNA_PRESENT | EXT_LNA_MODE_AUTO,
        SL_EXT_LNA_CFG_MODE_MANUAL_BYPASS = EXT_LNA_PRESENT | EXT_LNA_MODE_MANUAL | EXT_LNA_MODE_MANUAL_BYPASS,
        SL_EXT_LNA_CFG_MODE_MANUAL_ENABLE = EXT_LNA_PRESENT | EXT_LNA_MODE_MANUAL | EXT_LNA_MODE_MANUAL_ENABLE
    }SL_ExtLnaModeConfig_t;
    
    typedef struct
    {
        SL_ExtLnaModeConfig_t         lnaMode;
        unsigned int                  lnaGpioNum;
    }SL_ExtLnaConfigParams_t;

    typedef struct    /* Generic */
    {
    SL_DemodStd_t          std;
    SL_AfeIfConfigParams_t afeIfInfo;
    SL_IQOffsetCorrectionParams_t iqOffCorInfo;
    SL_OutIfConfigParams_t oifInfo;
        SL_ExtLnaModeConfig_t         extLnaMode;
    }SL_DemodConfigInfo_t;

    typedef enum
    {
        SL_DEMOD_POWER_MODE_ON = 0,
        SL_DEMOD_POWER_MODE_STANDBY
    }SL_DemodPowerMode_t;

    typedef enum
    {
    SL_DEMOD_STATUS_TYPE_BOOT = 0,
    SL_DEMOD_STATUS_TYPE_LOCK,
        SL_DEMOD_STATUS_TYPE_CPU
    }SL_DemodStatusType_t;

typedef enum
{
    SL_DEMOD_BOOT_STATUS_INPROGRESS = 0,
    SL_DEMOD_BOOT_STATUS_COMPLETE,
    SL_DEMOD_BOOT_STATUS_ERROR
}SL_DemodBootStatus_t;

    typedef unsigned int SL_DemodLockStatus_t;

    typedef unsigned int SL_DemodCpuStatus_t;

typedef enum
{
    SL_OK                     = 0,
    SL_ERR_OPERATION_FAILED   = -1,
    SL_ERR_TOO_MANY_INSTANCES = -2,
    SL_ERR_CODE_DWNLD         = -3,
    SL_ERR_INVALID_ARGUMENTS  = -4,
    SL_ERR_CMD_IF_FAILURE     = -5,
    SL_ERR_NOT_SUPPORTED      = -6,
	SL_ERR_ALREADY_STARTED    = -7
}SL_Result_t;

    typedef enum
    {
        /* ATSC 3.0 */
        SL_DEMOD_DIAG_TYPE_ATSC3P0_PERF = 0,
        SL_DEMOD_DIAG_TYPE_ATSC3P0_BSR,
        SL_DEMOD_DIAG_TYPE_ATSC3P0_L1B,
        SL_DEMOD_DIAG_TYPE_ATSC3P0_L1D,
        SL_DEMOD_DIAG_TYPE_ATSC3P0_PLP_LLS_INFO,
        /* ATSC 1.0 */
        SL_DEMOD_DIAG_TYPE_ATSC1P0_PERF,
        /* DVB-T */
        SL_DEMOD_DIAG_TYPE_DVBT_PERF,
        SL_DEMOD_DIAG_TYPE_DVBT_L1,
        /* DVB-T2 */
        SL_DEMOD_DIAG_TYPE_DVBT2_PERF,
        SL_DEMOD_DIAG_TYPE_DVBT2_L1,
        SL_DEMOD_DIAG_TYPE_DVBT2_PLP_COMMON,
        SL_DEMOD_DIAG_TYPE_DVBT2_PLP_USER,
        /* ISDB-T*/
        SL_DEMOD_DIAG_TYPE_ISDBT_PERF,
        SL_DEMOD_DIAG_TYPE_ISDBT_L1
    }SL_DemodDiagType_t;

    typedef enum
    {
        SL_BW_6MHZ = 6,
        SL_BW_7MHZ = 7,
        SL_BW_8MHZ = 8
    }SL_Bandwidth_t;

    typedef enum
    {
        SL_CCI_TYPE_PALI    = 0,
        SL_CCI_TYPE_PALBG   = 1,
        SL_CCI_TYPE_PALD1   = 2,
        SL_CCI_TYPE_SECAML  = 3,
        SL_CCI_TYPE_NTSC_MN = 4,
        SL_CCI_TYPE_PALB    = 5
    }SL_CciType_t;

typedef enum
{
	UNLOCKED = 0,
	LOCKED
}LockStatus_t;

typedef struct
{
	LockStatus_t locked;//1:locked ,0:unlocked
	int rssi;
	double snr;
	double ber;
	double per;
	double confidence;
	LockStatus_t plpVaild[4];

}SignalInfo_t;


/* External Interfaces */

    SL_Result_t SL_DemodCreateInstance(int *instance);
    SL_Result_t SL_DemodGetInstance(int *instance);
    SL_Result_t SL_DemodDeleteInstance(int instance);
    SL_Result_t SL_DemodInit(int instance, SL_CmdControlIf_t ctrl, SL_DemodStd_t std);
    SL_Result_t SL_DemodUnInit(int instance);
    SL_Result_t SL_DemodConfigure(int instance, SL_ConfigType_t type, void *cfgParams);
    SL_Result_t SL_DemodConfigureEx(int instance, SL_DemodStd_t std, void *stdParams);
    SL_Result_t SL_DemodSetPowerMode(int instance, SL_DemodStd_t std, SL_DemodPowerMode_t powerMode);
    SL_Result_t SL_DemodGetConfiguration(int instance, SL_DemodConfigInfo_t *cfg);
    SL_Result_t SL_DemodGetConfigurationEx(int instance, SL_DemodStd_t std, void *stdParams);
    SL_Result_t SL_DemodGetStatus(int instance, SL_DemodStatusType_t type, void *status);
    SL_Result_t SL_DemodStart(int instance);
    SL_Result_t SL_DemodStop(int instance);
    SL_Result_t SL_DemodGetSoftwareVersion(int instance, int *MajorNo, int *MinorNo);
    SL_Result_t SL_DemodGetDiagnostics(int instance, SL_DemodDiagType_t type, void *diagStr);
    SL_Result_t SL_DemodStartCalibration(int instance, unsigned int calibrationBlockSize);
    SL_Result_t SL_DemodGetCalibrationStatus(int instance, int *status);
    SL_Result_t SL_DemodGetIQOffSet(int instance, unsigned int *iValue, unsigned int *qValue);
    SL_Result_t SL_WriteBytes(int instance, unsigned int address, unsigned int lenBytes, void *buf);
    SL_Result_t SL_ReadBytes(int instance, unsigned int address, unsigned int lenBytes, void *buf);	
#ifdef __cplusplus
}; //extern "C"
#endif

#endif
