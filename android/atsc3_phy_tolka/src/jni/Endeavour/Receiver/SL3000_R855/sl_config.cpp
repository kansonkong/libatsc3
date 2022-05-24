/*****************************************************************************/
/*  Saankhya Confidential                                                    */
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
/*  File Name   :   sl_config.c                                              */
/*  version     :   0.7                                                      */
/*  Date        :   07/10/2021                                               */
/*  Description :   Contains SL HwPlatform and Tuner Slection Implementation */
/*                                                                           */
/*****************************************************************************/
#include <stddef.h>
#include "sl_config.h"
//#include "sl_log.h"
#include "sl_utils.h"

/* Defines */

/* Typedefs */
typedef enum
{
    SL_PLF_CONFIG_NOT_DONE = 0,
    SL_PLF_CONFIG_DONE
}SL_PlfConfigStatus_t;

/* Static Variables  */
static SL_BbCapture_t            slBbCap = BB_CAPTURE_DISABLE;
//static SL_PlatFormConfigParams_t cPlfParams = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
static SL_PlatFormConfigParams_t cPlfParams ;
static SL_PlfConfigStatus_t      plfCfgStatus = SL_PLF_CONFIG_NOT_DONE;
SL_ConfigResult_t SL_ConfigSetPlatform2(SL_PlatFormConfigParams_t cfgInfo)
{
    SL_ConfigResult_t          retVal = SL_CONFIG_OK;
	SL_Printf("[SL_ConfigSetPlatform2]\n");
	
	cPlfParams.boardType= cfgInfo.boardType;
	cPlfParams.chipRev= cfgInfo.chipRev;
	cPlfParams.chipType= cfgInfo.chipType;
	cPlfParams.tunerType= cfgInfo.tunerType;
	cPlfParams.demodControlIf= cfgInfo.demodControlIf;
	cPlfParams.demodOutputIf= cfgInfo.demodOutputIf;
	cPlfParams.demodI2cAddr= cfgInfo.demodI2cAddr;
	cPlfParams.slsdkPath= cfgInfo.slsdkPath;
	
	plfCfgStatus = SL_PLF_CONFIG_DONE;


	return retVal;
}

SL_ConfigResult_t SL_ConfigSetPlatform(SL_PlatFormConfigParams_t cfgInfo)
{
    SL_PlatFormConfigParams_t  tPlfParams ;
	//SL_PlatFormConfigParams_t  tPlfParams = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
    SL_ConfigResult_t          retVal = SL_CONFIG_OK;

    /* Set Board Type*/
    switch (cfgInfo.boardType)
    {
    case SL_EVB_3000:
        tPlfParams.boardType = SL_EVB_3000;

        /* Set Chip Type*/
        if (cfgInfo.chipType == SL_CHIP_3000)
        {
            tPlfParams.chipType = SL_CHIP_3000;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Chip Revision type */
        if ((cfgInfo.chipRev == SL_CHIP_REV_AA) || (cfgInfo.chipRev == SL_CHIP_REV_BB))
        {
            tPlfParams.chipRev = cfgInfo.chipRev;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Tuner Type*/
        if (cfgInfo.tunerType == TUNER_R855)
        {
            tPlfParams.tunerType = TUNER_R855;
        }		
        else if (cfgInfo.tunerType == TUNER_NXP)
        {
            tPlfParams.tunerType = TUNER_NXP;
        }
        else if (cfgInfo.tunerType == TUNER_SI)
        {
            tPlfParams.tunerType = TUNER_SI;
        }
        else if (cfgInfo.tunerType == TUNER_SI_P)
        {
            tPlfParams.tunerType = TUNER_SI_P;
        }
        else if (cfgInfo.tunerType == TUNER_SILABS)
        {
            tPlfParams.tunerType = TUNER_SILABS;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Command Control Interface Type*/
        if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
        {
            tPlfParams.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Output Interface Type*/
        if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;

            /* Set GPIO Pin used for Selected Interface*/
            if (cfgInfo.cpldResetGpioPin != 0xFF)
            {
                tPlfParams.cpldResetGpioPin = cfgInfo.cpldResetGpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_SDIO;

            /* Set GPIO Pin used for Selected Interface*/
            if (cfgInfo.demodI2cAddr3GpioPin != 0xFF)
            {
                tPlfParams.demodI2cAddr3GpioPin = cfgInfo.demodI2cAddr3GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }

            if (cfgInfo.demodBootMode1GpioPin != 0xFF)
            {
                tPlfParams.demodBootMode1GpioPin = cfgInfo.demodBootMode1GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }
        break;

    case SL_EVB_3010:
        tPlfParams.boardType = SL_EVB_3010;

        /* Set Chip Type*/
        if (cfgInfo.chipType == SL_CHIP_3010)
        {
            tPlfParams.chipType = SL_CHIP_3010;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Chip Revision type */
        if ((cfgInfo.chipRev == SL_CHIP_REV_AA) || (cfgInfo.chipRev == SL_CHIP_REV_BB))
        {
            tPlfParams.chipRev = cfgInfo.chipRev;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Tuner Type*/
        if (cfgInfo.tunerType == TUNER_NXP)
        {
            tPlfParams.tunerType = TUNER_NXP;
        }
        else if (cfgInfo.tunerType == TUNER_SI)
        {
            tPlfParams.tunerType = TUNER_SI;
        }
        else if (cfgInfo.tunerType == TUNER_SI_P)
        {
            tPlfParams.tunerType = TUNER_SI_P;
        }
        else if (cfgInfo.tunerType == TUNER_SILABS)
        {
            tPlfParams.tunerType = TUNER_SILABS;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Command Control Interface Type*/
        if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
        {
            tPlfParams.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Output Interface Type*/
        if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_SDIO;

            /* Set GPIO Pin used for Selected Interface*/
            if (cfgInfo.demodI2cAddr3GpioPin != 0xFF)
            {
                tPlfParams.demodI2cAddr3GpioPin = cfgInfo.demodI2cAddr3GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }

            if (cfgInfo.demodBootMode1GpioPin != 0xFF)
            {
                tPlfParams.demodBootMode1GpioPin = cfgInfo.demodBootMode1GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }
        break;

    case SL_EVB_4000:
        tPlfParams.boardType = SL_EVB_4000;

        /* Set Chip Type*/
        if (cfgInfo.chipType == SL_CHIP_4000)
        {
            tPlfParams.chipType = SL_CHIP_4000;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Chip Revision type */
        if ((cfgInfo.chipRev == SL_CHIP_REV_AA) || (cfgInfo.chipRev == SL_CHIP_REV_BB))
        {
            tPlfParams.chipRev = cfgInfo.chipRev;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Tuner Type*/
        if (cfgInfo.tunerType == TUNER_SI)
        {
            tPlfParams.tunerType = TUNER_SI;
        }
        else if (cfgInfo.tunerType == TUNER_SI_P)
        {
            tPlfParams.tunerType = TUNER_SI_P;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
        }

        /* Set Command Control Interface Type*/
        if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
        {
            tPlfParams.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Output Interface Type*/
        if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;

            /* Set GPIO Pin used for Selected Interface*/
            if (cfgInfo.cpldResetGpioPin != 0xFF)
            {
                tPlfParams.cpldResetGpioPin = cfgInfo.cpldResetGpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_SDIO;

            /* Set GPIO Pin used for Selected Interface*/
            if (cfgInfo.demodI2cAddr3GpioPin != 0xFF)
            {
                tPlfParams.demodI2cAddr3GpioPin = cfgInfo.demodI2cAddr3GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }

            if (cfgInfo.demodBootMode1GpioPin != 0xFF)
            {
                tPlfParams.demodBootMode1GpioPin = cfgInfo.demodBootMode1GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }
        break;

    case SL_KAILASH_DONGLE:
        tPlfParams.boardType = SL_KAILASH_DONGLE;

        /* Set Chip Type*/
        if (cfgInfo.chipType == SL_CHIP_3010)
        {
            tPlfParams.chipType = SL_CHIP_3010;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Chip Rev*/
        if (cfgInfo.chipRev == SL_CHIP_REV_AA)
        {
            tPlfParams.chipRev = SL_CHIP_REV_AA;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Tuner Type*/
        if (cfgInfo.tunerType == TUNER_SI)
        {
            tPlfParams.tunerType = TUNER_SI;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Command Control Interface Type*/
        if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
        {
            tPlfParams.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Output Interface Type*/
        if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }
        break;

    case SL_KAILASH_DONGLE_2:
        tPlfParams.boardType = SL_KAILASH_DONGLE_2;

        /* Set Chip Type*/
        if (cfgInfo.chipType == SL_CHIP_3010)
        {
            tPlfParams.chipType = SL_CHIP_3010;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Chip Rev*/
        if (cfgInfo.chipRev == SL_CHIP_REV_BB)
        {
            tPlfParams.chipRev = SL_CHIP_REV_BB;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Tuner Type*/
        if (cfgInfo.tunerType == TUNER_SI_P)
        {
            tPlfParams.tunerType = TUNER_SI_P;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Command Control Interface Type*/
        if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
        {
            tPlfParams.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Output Interface Type*/
        if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }
        break;

    case SL_SILISA_DONGLE:
        tPlfParams.boardType = SL_SILISA_DONGLE;

        /* Set Chip Type*/
        if (cfgInfo.chipType == SL_CHIP_3000)
        {
            tPlfParams.chipType = SL_CHIP_3000;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Chip Revision type */
        if (cfgInfo.chipRev == SL_CHIP_REV_BB)
        {
            tPlfParams.chipRev = cfgInfo.chipRev;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Tuner Type*/
        if (cfgInfo.tunerType == TUNER_SILABS)
        {
            tPlfParams.tunerType = TUNER_SILABS;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Command Control Interface Type*/
        if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
        {
            tPlfParams.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
        }
        else
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }

        /* Set Output Interface Type*/
        if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_SDIO;

            /* Set GPIO Pin used for Selected Interface*/
            if (cfgInfo.demodI2cAddr3GpioPin != 0xFF)
            {
                tPlfParams.demodI2cAddr3GpioPin = cfgInfo.demodI2cAddr3GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }

            if (cfgInfo.demodBootMode1GpioPin != 0xFF)
            {
                tPlfParams.demodBootMode1GpioPin = cfgInfo.demodBootMode1GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }
        }
        else
        {
           // retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
           
		   tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_TS;
            break;
        }
        break;

    case SL_YOGA_DONGLE:
        tPlfParams.boardType = SL_YOGA_DONGLE;

        /* Set Chip Type*/
        if (cfgInfo.chipType == SL_CHIP_4000)
        {
            tPlfParams.chipType = SL_CHIP_4000;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Chip Revision type */
        if (cfgInfo.chipRev == SL_CHIP_REV_BB)
        {
            tPlfParams.chipRev = cfgInfo.chipRev;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Tuner Type*/
        if (cfgInfo.tunerType == TUNER_SI_P)
        {
            tPlfParams.tunerType = TUNER_SI_P;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Command Control Interface Type*/
        if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
        {
            tPlfParams.demodControlIf = SL_DEMOD_CMD_CONTROL_IF_I2C;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }

        /* Set Output Interface Type*/
        if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
        {
            tPlfParams.demodOutputIf = SL_DEMOD_OUTPUTIF_SDIO;

            /* Set GPIO Pin used for Selected Interface*/
            if (cfgInfo.demodI2cAddr3GpioPin != 0xFF)
            {
                tPlfParams.demodI2cAddr3GpioPin = cfgInfo.demodI2cAddr3GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }

            if (cfgInfo.demodBootMode1GpioPin != 0xFF)
            {
                tPlfParams.demodBootMode1GpioPin = cfgInfo.demodBootMode1GpioPin;
            }
            else
            {
                retVal = SL_CONFIG_ERR_INVALID_ARGS;
            }
        }
        else if (cfgInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SPI)
        {
            retVal = SL_CONFIG_ERR_NOT_SUPPORTED;
            break;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
            break;
        }
        break;

    default:
        retVal = SL_CONFIG_ERR_INVALID_ARGS;
        break;
    }

    /* Set Chip Revision type */
    if ((cfgInfo.chipRev == SL_CHIP_REV_AA) || (cfgInfo.chipRev == SL_CHIP_REV_BB))
    {
        tPlfParams.chipRev = cfgInfo.chipRev;
    }
    else
    {
        retVal = SL_CONFIG_ERR_INVALID_ARGS;
    }

    /* Demod I2C Address */
    tPlfParams.demodI2cAddr = cfgInfo.demodI2cAddr;

    /* Reset GPIO GPIO Config*/
    if (cfgInfo.demodResetGpioPin != 0xFF)
    {
        tPlfParams.demodResetGpioPin = cfgInfo.demodResetGpioPin;
    }
    else
    {
        retVal = SL_CONFIG_ERR_INVALID_ARGS;
    }

    /* Reset Tuner GPIO Config*/
    if (cfgInfo.chipRev == SL_CHIP_REV_BB && cfgInfo.chipType == SL_CHIP_4000)
    {
        /* Tuner Reset GPIO pin Conneted Demod GPIO Pin-22, No Need to Configure GPIO on Host Processor*/
        tPlfParams.tunerResetGpioPin = 0xFF;
    }
    else
    {
        if (cfgInfo.tunerResetGpioPin != 0xFF)
        {
            tPlfParams.tunerResetGpioPin = cfgInfo.tunerResetGpioPin;
        }
        else
        {
            retVal = SL_CONFIG_ERR_INVALID_ARGS;
        }
    }

    /* Set slsdk path */
    if (cfgInfo.slsdkPath != NULL)
    {
        tPlfParams.slsdkPath = cfgInfo.slsdkPath;
    }
    else
    {
        retVal = SL_CONFIG_ERR_INVALID_ARGS;
    }

    if (retVal == SL_CONFIG_OK)
    {
        /* Save final params in cPlfParams */
		
        cPlfParams = tPlfParams;
        plfCfgStatus = SL_PLF_CONFIG_DONE;
    }
    else
    {
        plfCfgStatus = SL_PLF_CONFIG_NOT_DONE;
    }
    return retVal;
}

SL_ConfigResult_t SL_ConfigGetPlatform(SL_PlatFormConfigParams_t *cfgInfo)
{
    SL_ConfigResult_t retVal = SL_CONFIG_OK;
	//SL_Printf("[SL_ConfigGetPlatform]plfCfgStatus=%d\n",plfCfgStatus);

    if (plfCfgStatus == SL_PLF_CONFIG_DONE)
    {
        *cfgInfo = cPlfParams;
    }
    else
    {
        retVal = SL_CONFIG_ERR_NOT_CONFIGURED;
    }
    return (retVal);
}

SL_ConfigResult_t SL_ConfigSetBbCapture(SL_BbCapture_t val)
{
    SL_ConfigResult_t retVal = SL_CONFIG_OK;

    if (val == BB_CAPTURE_DISABLE || val == BB_CAPTURE_ENABLE)
    {
        slBbCap = val;
    }
    else
    {
        retVal = SL_CONFIG_ERR_INVALID_ARGS;
    }
    return retVal;
}

SL_ConfigResult_t SL_ConfigGetBbCapture(SL_BbCapture_t *val)
{
    SL_ConfigResult_t retVal = SL_CONFIG_OK;

    *val = slBbCap;

    return retVal;
}
