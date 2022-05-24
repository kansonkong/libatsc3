/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2019 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
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
/*  File Name   :   sl_i2c.c                                                 */
/*  version     :   0.3                                                      */
/*  Date        :   08/11/2019                                               */
/*  Description :   SL Platform API: I2c Implementation                      */
/*                                                                           */
/*****************************************************************************/

#include "sl_i2c.h"
#include "sl_config.h"
//#include "sl_log.h"
#include "sl_utils.h"



/* Defines */
#define SL_DEMOD_MEM_CMD_SIZE  (0x0A)
#define SL_DEMOD_MEM_READ_CMD  (0xAA)
#define SL_DEMOD_MEM_WRITE_CMD (0x55)

Endeavour*	endeavour_sl3000;
Byte		endeavour_sl3000_i2cBus;
//Byte		sl3000_i2cAddr;
/* Static API Function */
static void SL_I2cErr(SL_I2cResult_t err);
SL_I2cDispatcherMethods_t slI2cDispatcherMethods;


SL_I2cResult_t SL_I2cPreInit()
{
    if (slI2cDispatcherMethods.SL_I2cPreInit != NULL)
    {
        return slI2cDispatcherMethods.SL_I2cPreInit();
    }
    else
    {
        return SL_I2C_ERR_NOT_INITIALIZED;
    }
}

SL_I2cResult_t SL_I2cInit(Endeavour* endeavour, Byte i2cBus)
{
    SL_I2cResult_t retVal = SL_I2C_OK;
	endeavour_sl3000 = endeavour;
	endeavour_sl3000_i2cBus = i2cBus;
	//printf("\n[SL_I2cInit] i2c init ok!\n");
	return retVal;
}

SL_I2cResult_t SL_I2cUnInit()
{
	SL_I2cResult_t retVal = SL_I2C_OK;
    
    if (slI2cDispatcherMethods.SL_I2cUnInit != NULL)
    {
        return slI2cDispatcherMethods.SL_I2cUnInit();
    }
    else
    {
        return SL_I2C_ERR_NOT_INITIALIZED;
    }
}

SL_I2cResult_t SL_I2cWrite(unsigned char i2cAddr, unsigned int wLen, unsigned char *data)
{
    SL_I2cResult_t retVal = SL_I2C_OK;
	Dword error = BR_ERR_NO_ERROR;
	if (wLen > 254)
	{
		printf("[SL_I2cWrite]I2C write length > 254 bytes!!!!\n");
		return SL_I2C_ERR_TRANSFER_FAILED;
	}
	//retVal = (SL_I2cResult_t)IT9300_ExtI2C_write(endeavour_sl3000, 0, endeavour_sl3000_i2cBus, (Byte)(i2cAddr<<1), (Byte)wLen, data, False);
	error = IT9300_ExtI2C_write(endeavour_sl3000, 0, endeavour_sl3000_i2cBus, (Byte)(i2cAddr << 1), (Byte)wLen, data, False);

	if (!error)
		retVal = SL_I2C_OK;
	else
	{
		printf("[SL_I2cWrite]error=0x%x\n", error);
		retVal = SL_I2C_ERR_TRANSFER_FAILED;
	}
	return retVal;
}

SL_I2cResult_t SL_I2cRead(unsigned char i2cAddr, unsigned int rLen, unsigned char*data)
{
    SL_I2cResult_t retVal = SL_I2C_OK;
	Dword error = BR_ERR_NO_ERROR;

	Byte		buf[255];
	int 		len=0;
	if (rLen > 255)
	{
		printf("[SL_I2cRead]I2C write length > 254 bytes!!!!\n");
		return SL_I2C_ERR_TRANSFER_FAILED;
	}

#if 0
	for(int i=0;i<rLen;i++)
		SL_Printf("[SL_I2cRead]data[%d] =0x%x \n",i,data[i]);
#endif	

	

	error = IT9300_ExtI2C_read(endeavour_sl3000, 0, endeavour_sl3000_i2cBus, (Byte)((i2cAddr<<1)+1), rLen, data);
	if (!error)
		retVal = SL_I2C_OK;
	else
	{
		printf("[SL_I2cRead]error=0x%x\n", error);
		retVal = SL_I2C_ERR_TRANSFER_FAILED;
	}
	return retVal;
}

SL_I2cResult_t SL_I2cWriteNoStop(unsigned char i2cAddr, unsigned char i2cSubAddr, unsigned int wLen, unsigned char*data)
{
    if (slI2cDispatcherMethods.SL_I2cWriteNoStop != NULL)
    {
        return slI2cDispatcherMethods.SL_I2cWriteNoStop(i2cAddr, i2cSubAddr, wLen, data);
    }
    else
    {
        return SL_I2C_ERR_NOT_INITIALIZED;
    }

}

SL_I2cResult_t SL_I2cReadNoStop(unsigned char i2cAddr, unsigned char i2cSubAddr, unsigned int rLen, unsigned char*data)
{
    if (slI2cDispatcherMethods.SL_I2cReadNoStop != NULL)
    {
        return slI2cDispatcherMethods.SL_I2cReadNoStop(i2cAddr, i2cSubAddr, rLen, data);
    }
    else
    {
        return SL_I2C_ERR_NOT_INITIALIZED;
    }
}