/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2019 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
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
/*  File Name   :   sl_tuner.h                                               */
/*  version     :   0.6                                                      */
/*  Date        :   24/11/2021                                               */
/*  Description :   SLTuner API Header File                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef SL_TUNER_H_
#define SL_TUNER_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* Include */

    /* Defines */

    /* Typedefs */
    typedef enum
    {
        SL_TUNERSTD_ATSC3_0,
        SL_TUNERSTD_ATSC1_0,
        SL_TUNERSTD_DVB_T,
        SL_TUNERSTD_DVB_T2,
        SL_TUNERSTD_DTMB,
        SL_TUNERSTD_DVB_S,
        SL_TUNERSTD_DVB_S2,
        SL_TUNERSTD_DVB_C,
        SL_TUNERSTD_ISDB_C,
        SL_TUNERSTD_ISDB_T,
        SL_TUNERSTD_US_C
    }SL_TunerStd_t;

    typedef enum
    {
        SL_TUNER_BW_6MHZ = 6,
        SL_TUNER_BW_7MHZ = 7,
        SL_TUNER_BW_8MHZ = 8
    }SL_TunerBW_t;

    typedef struct
    {
        SL_TunerStd_t   std;
        SL_TunerBW_t  bandwidth;
    }SL_TunerConfig_t;

    typedef enum
    {
        SL_TUNER_STATUS_NOT_LOCKED = 0,
        SL_TUNER_STATUS_LOCKED
    }SL_TunerLockStatus_t;

    typedef struct
    {
        SL_TunerLockStatus_t status;
        double signalStrength; //RSSI(dBm?)
    }SL_TunerSignalInfo_t;

    typedef struct
    {
        unsigned int iOffSet;
        unsigned int qOffSet;
    }SL_TunerDcOffSet_t;

    typedef enum
    {
        SL_TUNER_OK                        = 0,
        SL_TUNER_ERR_OPERATION_FAILED      = -1,
        SL_TUNER_ERR_INVALID_ARGS          = -2,
        SL_TUNER_ERR_NOT_SUPPORTED         = -3,
        SL_TUNER_ERR_MAX_INSTANCES_REACHED = -4
    }SL_TunerResult_t;

    /* External Interfaces */
    SL_TunerResult_t SL_TunerCreateInstance(int *instance);
    SL_TunerResult_t SL_TunerDeleteInstance(int instance);
    SL_TunerResult_t SL_TunerInit(int instance);
    SL_TunerResult_t SL_TunerUnInit(int instance);
    SL_TunerResult_t SL_TunerConfigure(int instance, SL_TunerConfig_t *cfg);
    SL_TunerResult_t SL_TunerGetConfiguration(int instance, SL_TunerConfig_t *cfg);
    SL_TunerResult_t SL_TunerSetFrequency(int instance, unsigned int frequency);
    SL_TunerResult_t SL_TunerGetFrequency(int instance, unsigned int *frequency);
    SL_TunerResult_t SL_TunerGetStatus(int instance, SL_TunerSignalInfo_t *tsInfo);
    SL_TunerResult_t SL_TunerExSetDcOffSet(int instance, SL_TunerDcOffSet_t *dcoffset);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif