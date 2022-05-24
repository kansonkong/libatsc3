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
/*  File Name   :   sl_demod_regs.c                                          */
/*  version     :   0.4                                                      */
/*  Date        :   28/08/2021                                               */
/*  Description :   Contains SLDemod Register Functions                      */
/*                                                                           */
/*****************************************************************************/

#include "sl_demod.h"
#include "sl_demod_int.h"
#include "sl_i2c.h"
#include "sl_utils.h"

/* Defines */
#define SL_DEMOD_MEM_CMD_SIZE      (0x0A)

/* Typedef */

/* Extranal Valribale */
extern SL_DemodBlock_t demodBlock[SL_DEMOD_MAX_INSTANCES];

/* Demod Constants */
const unsigned char wCommand = 0x55;
const unsigned char rCommand = 0xAA;

/* Global Variable */
static volatile SL_DemodBool_t regRWInProgress = SL_DEMOD_FALSE; /* Demod Register Read/Write progress status flag */

static SL_Result_t SL_DemodWriteReg(int instance, unsigned int address, int len, unsigned char *value)
{
    SL_Result_t retVal = SL_OK;

    /* Check demod register read/write status */
    while (regRWInProgress)
    {
        SL_SleepMS(1); // Wait some time
    }

    regRWInProgress = SL_DEMOD_TRUE; // Make flag true to indicate that demod register write/read in progress 
    unsigned char  buffer[SL_DEMOD_MEM_CMD_SIZE] =
    {
            wCommand,
            demodBlock[instance].boardInfo.demodI2cAddr,
            (unsigned char)(((demodBlock[instance].baseAddr + address) & 0xFF000000) >> 24),
            (unsigned char)(((demodBlock[instance].baseAddr + address) & 0xFF0000) >> 16),
            (unsigned char)(((demodBlock[instance].baseAddr + address) & 0xFF00) >> 8),
            (unsigned char)(((demodBlock[instance].baseAddr + address) & 0xFF) >> 0),
            (unsigned char)((len & 0xFF000000) >> 24),
            (unsigned char)((len & 0xFF0000) >> 16),
            (unsigned char)((len & 0xFF00) >> 8),
            (unsigned char)((len & 0xFF) >> 0),
    };

    retVal = SL_DemodSendData(instance, SL_DEMOD_MEM_CMD_SIZE, buffer);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodSendData(instance, len, value);
    }

    regRWInProgress = SL_DEMOD_FALSE; // Make flag false to indicate that another process can access demod register
    return retVal;
}

static SL_Result_t SL_DemodReadReg(int instance, unsigned int address, int len, unsigned char *value)
{
    SL_Result_t retVal = SL_OK;

    /* Check demod register read/write status */
    while (regRWInProgress)
    {
        SL_SleepMS(1); // Wait some time
    }
    regRWInProgress = SL_DEMOD_TRUE; // Make flag true to indicate that demod register write/read in progress 

    unsigned char  buffer[SL_DEMOD_MEM_CMD_SIZE] =
    {
            rCommand,
            demodBlock[instance].boardInfo.demodI2cAddr,
            (unsigned char)(((demodBlock[instance].baseAddr + address) & 0xFF000000) >> 24),
            (unsigned char)(((demodBlock[instance].baseAddr + address) & 0xFF0000) >> 16),
            (unsigned char)(((demodBlock[instance].baseAddr + address) & 0xFF00) >> 8),
            (unsigned char)(((demodBlock[instance].baseAddr + address) & 0xFF) >> 0),
            (unsigned char)((len & 0xFF000000) >> 24),
            (unsigned char)((len & 0xFF0000) >> 16),
            (unsigned char)((len & 0xFF00) >> 8),
            (unsigned char)((len & 0xFF) >> 0),
    };

    retVal = SL_DemodSendData(instance, SL_DEMOD_MEM_CMD_SIZE, buffer);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodRecieveData(instance, len, value);
    }

    regRWInProgress = SL_DEMOD_FALSE; // Make flag false to indicate that another process can access demod register
    return retVal;
}

SL_Result_t SL_DemodSendData(int instance, unsigned int len, unsigned char *buf)
{
    SL_Result_t retVal = SL_OK;

    if (demodBlock[instance].comPrtcl == SL_CMD_CONTROL_IF_I2C)
    {
        retVal = (SL_Result_t)SL_I2cWrite(demodBlock[instance].boardInfo.demodI2cAddr, len, buf);
        if (retVal != SL_OK)
        {
            retVal = SL_ERR_CMD_IF_FAILURE;
        }
    }
    else if (demodBlock[instance].comPrtcl == SL_CMD_CONTROL_IF_SPI)
    {
        retVal = SL_ERR_NOT_SUPPORTED;  // sl spi write call
    }
    else if (demodBlock[instance].comPrtcl == SL_CMD_CONTROL_IF_SDIO)
    {
        retVal = SL_ERR_NOT_SUPPORTED;      // sl SDIO write call
    }
    return retVal;
}

SL_Result_t SL_DemodRecieveData(int instance, unsigned int len, unsigned char *buf)
{
    SL_Result_t retVal = SL_OK;

    if (demodBlock[instance].comPrtcl == SL_CMD_CONTROL_IF_I2C)
    {
        retVal = (SL_Result_t)SL_I2cRead(demodBlock[instance].boardInfo.demodI2cAddr, len, buf);
        if (retVal != SL_OK)
        {
            retVal = SL_ERR_CMD_IF_FAILURE;
        }
    }
    else if (demodBlock[instance].comPrtcl == SL_CMD_CONTROL_IF_SPI)
    {
        retVal = SL_ERR_NOT_SUPPORTED;      // sl spi read call
    }
    else if (demodBlock[instance].comPrtcl == SL_CMD_CONTROL_IF_SDIO)
    {
        retVal = SL_ERR_NOT_SUPPORTED;      // sl SDIO read call
    }
    return retVal;
}

SL_Result_t SL_DemodWriteBytes(int instance, unsigned int address, unsigned int lenBytes, void *buf)
{
    SL_Result_t retVal = SL_OK;

    unsigned char *dataBuffer = (unsigned char*)buf;

    while (lenBytes > 100)
    {
        retVal = SL_DemodWriteReg(instance, address, 100, dataBuffer);
        if (retVal == SL_OK)
        {
            lenBytes = lenBytes - 100;
            dataBuffer = dataBuffer + 100;
            address = address + 100;
        }
        else
        {
            /* Failure found, so break from loop */
            break;
        }
    }

    if ((retVal == SL_OK) && (lenBytes <= 100))
    {
        retVal = SL_DemodWriteReg(instance, address, lenBytes, dataBuffer);
    }
    return retVal;
}

SL_Result_t SL_DemodReadBytes(int instance, unsigned int address, unsigned int lenBytes, void *buf)
{
    SL_Result_t retVal = SL_OK;
    unsigned char *dataBuffer = (unsigned char*)buf;

    while (lenBytes > 100)
    {
        retVal = SL_DemodReadReg(instance, address, 100, dataBuffer);
        if (retVal == SL_OK)
        {
            lenBytes = lenBytes - 100;
            dataBuffer = dataBuffer + 100;
            address = address + 100;
        }
        else
        {
            /* Failure found, so break from loop */
            break;
        }
    }

    if ((retVal == SL_OK) && (lenBytes <= 100))
    {
        retVal = SL_DemodReadReg(instance, address, lenBytes, dataBuffer);
    }
    return retVal;
}