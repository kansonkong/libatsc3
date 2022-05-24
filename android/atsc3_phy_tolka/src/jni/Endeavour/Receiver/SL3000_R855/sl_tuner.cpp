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
/*  File Name   :   sl_tuner.c                                               */
/*  version     :   0.6                                                      */
/*  Date        :   11/08/2021                                               */
/*  Description :   SLTuner API: Generic Implementation. Calls Specific Tuner*/
/*                               Based on Slection                           */
/*                                                                           */
/*****************************************************************************/

#include "sl_tuner.h"
#include "sl_config.h"
//#include "sl_nxptuner.h"
//#include "sl_situner.h"
//#include "sl_silabstuner.h"

SL_TunerResult_t SL_TunerCreateInstance(int *instance)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo ;
#if 0
    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
			retVal = (SL_TunerResult_t)SL_NxpTunerCreateInstance(instance);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            retVal = SL_SiTunerCreateInstance(instance);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            retVal = SL_SilabsTunerCreateInstance(instance);
        }
        else
        {
            /* Customer code here */
            /* ...*/

            /* ...*/
            /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif
    return retVal;
}

SL_TunerResult_t SL_TunerDeleteInstance(int instance)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo  ;
#if 0	
    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_R855)
        {
            //retVal = SL_NxpTunerDeleteInstance(instance);
        }    
        else if (tunerinfo.tunerType == TUNER_NXP)
        {
            //retVal = SL_NxpTunerDeleteInstance(instance);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            //retVal = SL_SiTunerDeleteInstance(instance);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            //retVal = SL_SilabsTunerDeleteInstance(instance);
        }
        else
        {
            /* Customer code here */
            /* ...*/

            /* ...*/
            /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif	
    return retVal;
}

SL_TunerResult_t SL_TunerInit(int instance)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo;
#if 0	
    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
			retVal = (SL_TunerResult_t)SL_NxpTunerInit(instance);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            retVal = SL_SiTunerInit(instance);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            retVal = SL_SilabsTunerInit(instance);
        }
        else
        {
            /* Customer code here */
            /* ...*/

            /* ...*/
            /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif	
    return retVal;
}

SL_TunerResult_t SL_TunerUnInit(int instance)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo;
#if 0	
    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
			retVal = (SL_TunerResult_t)SL_NxpTunerUnInit(instance);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            retVal = SL_SiTunerUnInit(instance);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            retVal = SL_SilabsTunerUnInit(instance);
        }
        else
        {
            /* Customer code here */
            /* ...*/

            /* ...*/
            /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif	
    return retVal;
}

SL_TunerResult_t SL_TunerConfigure(int instance, SL_TunerConfig_t *cfg)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo;
#if 0

    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
			//retVal = (SL_TunerResult_t)SL_NxpTunerConfigure(instance, cfg);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            //retVal = SL_SiTunerConfigure(instance, cfg);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            //retVal = SL_SilabsTunerConfigure(instance, cfg);
        }
        else
        {

            /* Customer code here */
            /* ...*/

            /* ...*/
            /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif	
    return retVal;
}

SL_TunerResult_t SL_TunerGetConfiguration(int instance, SL_TunerConfig_t *cfg)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo;
#if 0

    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
			//retVal = (SL_TunerResult_t)SL_NxpTunerGetConfiguration(instance, cfg);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            //retVal = SL_SiTunerGetConfiguration(instance, cfg);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            //retVal = SL_SilabsTunerGetConfiguration(instance, cfg);
        }
        else
        {
            /* Customer code here */
            /* ...*/

            /* ...*/
            /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif	
    return retVal;
}

SL_TunerResult_t SL_TunerSetFrequency(int instance, unsigned int frequency)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo;
#if 0

    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
			//retVal = (SL_TunerResult_t)SL_NxpTunerSetFrequency(instance, frequency);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            //retVal = SL_SiTunerSetFrequency(instance, frequency);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            //retVal = SL_SilabsTunerSetFrequency(instance, frequency);
        }
        else
        {
            /* Customer code here */
            /* ...*/

            /* ...*/
            /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif	
    return retVal;
}

SL_TunerResult_t SL_TunerGetFrequency(int instance, unsigned int *frequency)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo;
#if 0

    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
			//retVal = (SL_TunerResult_t)SL_NxpTunerGetFrequency(instance, frequency);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            //retVal = SL_SiTunerGetFrequency(instance, frequency);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            //retVal = SL_SilabsTunerGetFrequency(instance, frequency);
        }
        else
        {
            /* Customer code here */
            /* ...*/

            /* ...*/
            /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif	
    return retVal;
}

SL_TunerResult_t SL_TunerGetStatus(int instance, SL_TunerSignalInfo_t *tsInfo)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    //SL_PlatFormConfigParams_t tunerinfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t tunerinfo;
#if 0	
    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
			//retVal = (SL_TunerResult_t)SL_NxpTunerGetStatus(instance, tsInfo);
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            //retVal = SL_SiTunerGetStatus(instance, tsInfo);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            //retVal = SL_SilabsTunerGetStatus(instance, tsInfo);
        }
        else
        {
            /* Customer code here */
                /* ...*/

                /* ...*/
                /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif
    return retVal;
}

SL_TunerResult_t SL_TunerExSetDcOffSet(int instance, SL_TunerDcOffSet_t *dcoffset)
{
    SL_TunerResult_t retVal = SL_TUNER_OK;
    SL_PlatFormConfigParams_t tunerinfo;
#if 0

    if (SL_ConfigGetPlatform(&tunerinfo) == SL_CONFIG_OK)
    {
        if (tunerinfo.tunerType == TUNER_NXP)
        {
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
        else if (tunerinfo.tunerType == TUNER_SI || tunerinfo.tunerType == TUNER_SI_P)
        {
            retVal = SL_SiTunerExSetDcOffSet(instance, dcoffset);
        }
        else if (tunerinfo.tunerType == TUNER_SILABS)
        {
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
        else
        {
            /* Customer code here */
                /* ...*/

                /* ...*/
                /* End of Customer code here */
            retVal = SL_TUNER_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        retVal = SL_TUNER_ERR_OPERATION_FAILED;
    }
#endif	
    return retVal;
}
