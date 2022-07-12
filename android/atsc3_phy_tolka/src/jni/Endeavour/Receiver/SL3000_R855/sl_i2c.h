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
/*  File Name   :   sl_i2c.h                                                 */
/*  version     :   0.3                                                      */
/*  Date        :   29/11/2021                                               */
/*  Description :   SL Platform API: I2c Header File                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_I2C_H_
#define _SL_I2C_H_
#include "../../../tolka/IT9300.h"
#ifdef __cplusplus
extern "C" {
#endif

    /* Typedefs */
    typedef enum
    {
        SL_I2C_OK                   = 0,
        SL_I2C_ERR_TRANSFER_FAILED  = -1,
        SL_I2C_ERR_NOT_INITIALIZED  = -2,
        SL_I2C_ERR_BUS_TIMEOUT      = -3,
        SL_I2C_ERR_LOST_ARBITRATION = -4
    }SL_I2cResult_t;

    /* External Interfaces */
    SL_I2cResult_t SL_I2cPreInit();
	SL_I2cResult_t SL_I2cInit(Endeavour* endeavour, Byte i2cBus);
    SL_I2cResult_t SL_I2cUnInit();
    SL_I2cResult_t SL_I2cWrite(unsigned char i2cAddr, unsigned int wLen, unsigned char *data);
    SL_I2cResult_t SL_I2cRead(unsigned char i2cAddr, unsigned int rlen, unsigned char *data);
    SL_I2cResult_t SL_I2cWriteNoStop(unsigned char  i2cAddr, unsigned char i2cSubAddr, unsigned int wlen, unsigned char *data);
    SL_I2cResult_t SL_I2cReadNoStop(unsigned char i2cAddr, unsigned char i2cSubAddr, unsigned int rlen, unsigned char *data);


    /* I2c dispatcher method datatypes */
    typedef struct
    {
        SL_I2cResult_t(*SL_I2cPreInit)();
        SL_I2cResult_t(*SL_I2cInit)();
        SL_I2cResult_t(*SL_I2cUnInit)();
        SL_I2cResult_t(*SL_I2cWrite)(unsigned char i2cAddr, unsigned int wLen, unsigned char *data);
        SL_I2cResult_t(*SL_I2cRead)(unsigned char i2cAddr, unsigned int rlen, unsigned char *data);
        SL_I2cResult_t(*SL_I2cWriteNoStop)(unsigned char  i2cAddr, unsigned char i2cSubAddr, unsigned int wlen, unsigned char *data);
        SL_I2cResult_t(*SL_I2cReadNoStop)(unsigned char i2cAddr, unsigned char i2cSubAddr, unsigned int rlen, unsigned char *data);
    } SL_I2cDispatcherMethods_t;

//	SL_I2cDispatcherMethods_t slI2cDispatcherMethods;

#ifdef __cplusplus
}; //extern "C"
#endif

#endif
