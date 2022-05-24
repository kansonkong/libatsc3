/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided “AS IS? WITH ALL FAULTS.           */
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
/*  File Name   :   sl_demod_calib.c                                         */
/*  version     :   0.3                                                      */
/*  Date        :   07/12/2020                                               */
/*  Description :   Contains SLDemod IQ Calibration API                      */
/*                                                                           */
/*****************************************************************************/

#include "sl_utils.h"
#include "sl_demod.h"
#include "sl_demod_int.h"
#include "sl_config.h"
//#include "sl_gpio.h"
#include "sl_i2c.h"
//#include "sl_log.h"

/* Extranal Valribale */
extern SL_DemodBlock_t demodBlock[SL_DEMOD_MAX_INSTANCES];

SL_Result_t SL_DemodStartCalibration(int instance, unsigned int calibrationBlockSize)
{
    SL_Result_t retVal = SL_OK;
    int setVal;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            if (!demodBlock[instance].isStarted)
            {
                setVal = 0x00007845;
                retVal = SL_DemodWriteBytes(instance, SET_DEMOD_CALIB_START, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                if (retVal == SL_OK)
                    demodBlock[instance].isStarted = SL_DEMOD_TRUE;
            }

            if (retVal == SL_OK)
            {
                setVal = 0x00000000;
                retVal = SL_DemodWriteBytes(instance, SET_DC_OFF_CALIB_BLKSIZE, SL_DEMOD_REG_SIZE, &calibrationBlockSize);
            }

            if (retVal == SL_OK)
            {
                setVal = 0x00000000;
                retVal = SL_DemodWriteBytes(instance, SET_DC_OFFSET_STATCOM, SL_DEMOD_REG_SIZE, &setVal);
            }

            if (retVal == SL_OK)
            {
                setVal = 0x00000001;
                retVal = SL_DemodWriteBytes(instance, SET_DC_OFFSET_CALIB_START, SL_DEMOD_REG_SIZE, &setVal);
            }
        }
        else
        {
            retVal = SL_ERR_OPERATION_FAILED;
        }
    }
    else
    {
        retVal = SL_ERR_INVALID_ARGUMENTS;
    }
    return retVal;
}

SL_Result_t SL_DemodGetCalibrationStatus(int instance, int *status)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            retVal = SL_DemodReadBytes(instance, SET_DC_OFFSET_STATCOM, SL_DEMOD_REG_SIZE, status);
        }
        else
        {
            retVal = SL_ERR_OPERATION_FAILED;
        }
    }
    else
    {
        retVal = SL_ERR_INVALID_ARGUMENTS;
    }
    return retVal;
}

SL_Result_t SL_DemodGetIQOffSet(int instance, unsigned int *iValue, unsigned int *qValue)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            retVal = SL_DemodReadBytes(instance, GET_DC_IREG, SL_DEMOD_REG_SIZE, iValue);
            if (retVal == SL_OK)
            {
                retVal = SL_DemodReadBytes(instance, GET_DC_QREG, SL_DEMOD_REG_SIZE, qValue);
            }
        }
        else
        {
            retVal = SL_ERR_OPERATION_FAILED;
        }
    }
    else
    {
        retVal = SL_ERR_INVALID_ARGUMENTS;
    }
    return retVal;
}