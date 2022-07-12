/*****************************************************************************/
/* Saankhya Confidential                                                     */
/*  COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided "AS IS", WITH ALL FAULTS.           */
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
/*  File Name   :   sl_config.h                                              */
/*  version     :   0.10                                                      */
/*  Date        :   29/11/2021                                               */
/*  Description :   SL Demod and Tuner Configuration file                    */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_CONFIG_H_
#define _SL_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* Defines */
#define SL_DEMOD_I2C_ADDR                (0x30)   /* SL Demod I2C Address */

/* Typedefs */
    typedef enum
    {
        SL_CONFIG_OK                 = 0,
        SL_CONFIG_ERR_INVALID_ARGS   = -1,
        SL_CONFIG_ERR_NOT_SUPPORTED  = -2,
        SL_CONFIG_ERR_NOT_CONFIGURED = -3
    }SL_ConfigResult_t;

    typedef enum
    {
        SL_EVB_3000 = 0,
        SL_EVB_3010,
        SL_EVB_4000,
        SL_KAILASH_DONGLE,   //SL3010+SiTune+FX3S
        SL_KAILASH_DONGLE_2, //SL3010_BB+SiTune_P+FX3S
        SL_SILISA_DONGLE,    //SL3000_BB+SiLabsTuner+FX3S
        SL_YOGA_DONGLE,      //SL4000_BB+SiTune_P+FX3S
        SL_BOARDTYPE_INVALID = 0xff
    }SL_BoardType_t;

    typedef enum
    {
        SL_CHIP_3000 = 0,
        SL_CHIP_3010,
        SL_CHIP_4000,
        SL_CHIPTYPE_INVALID = 0xff
    }SL_ChipType_t;

    typedef enum
    {
        SL_CHIP_REV_AA = 0,
        SL_CHIP_REV_BB,
        SL_CHIPREV_INVALID = 0xff
    }SL_ChipRev_t;

    typedef enum
    {
        TUNER_NXP = 0,
        TUNER_SI,
        TUNER_SI_P,
        TUNER_SILABS,
        TUNER_R855,
        SL_TUNERTYPE_INVALID = 0xff
    }SL_TunerType_t;

    typedef enum
    {
        SL_DEMOD_CMD_CONTROL_IF_I2C = 0,
        SL_DEMOD_CMD_CONTROL_IF_SDIO,
        SL_DEMOD_CMD_CONTROL_IF_SPI,
        SL_DEMOD_CMD_CONTROL_INVALID = 0xff
    }SL_DemodCmdControlIf_t;

    typedef enum
    {
        SL_DEMOD_OUTPUTIF_TS = 0,
        SL_DEMOD_OUTPUTIF_SDIO,
        SL_DEMOD_OUTPUTIF_SPI,
        SL_DEMOD_OUTPUTIF_INVALID = 0xff
    }SL_DemodOutputIf_t;

    typedef enum
    {
        BB_CAPTURE_DISABLE = 0,
        BB_CAPTURE_ENABLE
    }SL_BbCapture_t;

    typedef void(*SL_DispatcherConfig_t)(void);

    typedef struct
    {
        SL_BoardType_t         boardType;
        SL_ChipType_t          chipType;
        SL_ChipRev_t           chipRev;
        SL_DemodCmdControlIf_t demodControlIf;
        SL_DemodOutputIf_t     demodOutputIf;
        unsigned char          demodI2cAddr;
        unsigned char          demodResetGpioPin;
        unsigned char          cpldResetGpioPin;      /* Relevant only for SL_EVB_3000 and SL_EVB_4000 TS Output Interface */
        unsigned char          demodI2cAddr3GpioPin;  /* Relevant only for SDIO Output Interface of AA chip Revision and to update SDIO Data Read Status to Demod */
        unsigned char          demodBootMode1GpioPin; /* Relevant only for SDIO Output Interface and get to know SDIO data ready intrupt from demod  */
        SL_TunerType_t         tunerType;
        unsigned char          tunerResetGpioPin;
        char*                  slsdkPath;
        SL_DispatcherConfig_t  dispConf;              /* Configures dispatcher functions defined for selected platform */

    }SL_PlatFormConfigParams_t;

#define SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER \
{ \
    .boardType             = SL_BOARDTYPE_INVALID, \
    .chipType              = SL_CHIPTYPE_INVALID, \
    .chipRev               = SL_CHIPREV_INVALID, \
    .demodControlIf        = SL_DEMOD_CMD_CONTROL_INVALID, \
    .demodOutputIf         = SL_DEMOD_OUTPUTIF_INVALID, \
    .demodI2cAddr          = 0, \
    .demodResetGpioPin     = 0, \
    .cpldResetGpioPin      = 0, \
    .demodI2cAddr3GpioPin  = 0, \
    .demodBootMode1GpioPin = 0, \
    .tunerType             = SL_TUNERTYPE_INVALID, \
    .tunerResetGpioPin     = 0, \
    .slsdkPath             = NULL, \
    .dispConf              = NULL \
};

    /* External Interfaces - used for dispatcher de-coupling from plf/cust */
SL_ConfigResult_t SL_ConfigSetPlatform2(SL_PlatFormConfigParams_t cfgInfo);
    SL_ConfigResult_t SL_ConfigSetPlatform(SL_PlatFormConfigParams_t cfgInfo);
    SL_ConfigResult_t SL_ConfigGetPlatform(SL_PlatFormConfigParams_t *cfgInfo);
    SL_ConfigResult_t SL_ConfigSetBbCapture(SL_BbCapture_t val);
    SL_ConfigResult_t SL_ConfigGetBbCapture(SL_BbCapture_t *val);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
