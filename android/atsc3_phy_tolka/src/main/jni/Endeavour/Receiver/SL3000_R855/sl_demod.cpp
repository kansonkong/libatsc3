/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
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
/*  File Name   :   sl_demod.c                                               */
/*  version     :   0.13                                                     */
/*  Date        :   24/11/2021                                               */
/*  Description :   Contains SLDemod API Implementation                      */
/*                                                                           */
/*****************************************************************************/

#include "sl_utils.h"
#include "sl_demod.h"
#include "sl_demod_int.h"
#include "sl_config.h"
#include "sl_log.h"
#include "SL_atsc1_fw.h"
#include "SL_atsc3_fw.h"
#include <string.h>

/* Defines */
#define SL_DEMOD_FADC_64MHZ        (64.0714285714286)
#define SL_DEMOD_FADC_32MHZ        (32.09375)
#define Q_11                       (2048)             /*Q.11 formate 2^11*/
#define Q_14                       (16384)            /*Q.14 formate 2^14*/
#define Q_31                       (2147483648)       /*Q.31 formate 2^31*/
#define MHZ                        (1000000)          /* 1MHz */
#define KHZ                        (1000)             /* 1KHz */

/* Typedef */

/* Demod Global Variable */
static int slDemodInstance = 0;
SL_DemodBlock_t demodBlock[SL_DEMOD_MAX_INSTANCES];

/* Demod Constants */
const unsigned char cmdLen = 16;
const unsigned char cmdLenIdx = 8;

static SL_Result_t SL_DemodUpdateFileLengthInINFOCommond(const char * fileName, unsigned char *commmand)
{
    void *fp;

    fp = SL_FileOpen(fileName, "rb");
    if (fp == NULL)
    {
        return SL_ERR_INVALID_ARGUMENTS;
    }
    else
    {
        SL_FileSeek(fp, 0L, SL_SEEK_END);
        long int len = ftell((FILE *)fp);
        SL_FileClose(fp);

        for (int idx = 0; idx < 4; idx++)
        {
            commmand[cmdLenIdx + idx] = (unsigned char)((len >> (8 * idx)) & 0xFF);
        }
    }
    return SL_OK;
}

static SL_Result_t SL_DemodGpioReset(int instance)
{
    SL_Result_t retVal = SL_OK;

    return retVal;
}
static SL_Result_t  ITE_SL_DemodDownloadHexFile(int instance, const char fileName[],unsigned int len)
{
    SL_Result_t retVal = SL_OK;
    unsigned char data[127];
    const unsigned int bulkReadLen = 112;
    int bytesRead;
    int exitFlag = 0;

	//int arr_len=sizeof(fileName)/sizeof(fileName[0]);

	for(int i=0;i<len/bulkReadLen;i++)
	{
		if (i<((len / bulkReadLen) - 1))
	        		retVal = SL_DemodSendData(instance, bulkReadLen,(unsigned char *) (fileName+i *bulkReadLen));
		else
			retVal = SL_DemodSendData(instance, len-i *bulkReadLen,(unsigned char *)  (fileName+i *bulkReadLen));
	    if (retVal != SL_OK)
	    {
	        retVal = SL_ERR_CODE_DWNLD;
	        break;
	    }

	}


    return retVal;
}

static SL_Result_t SL_DemodDownloadHexFile(int instance, const char* fileName)
{
    SL_Result_t retVal = SL_OK;
    unsigned char data[127];
    const unsigned int bulkReadLen = 112;
    int bytesRead;
    int exitFlag = 0;
    void *fp;

    fp = SL_FileOpen(fileName, "rb");
    if (fp == NULL)
    {
        return SL_ERR_OPERATION_FAILED;
    }

    while (!exitFlag)
    {
        bytesRead = SL_FileRead(data, 1, bulkReadLen, fp);
        if (bytesRead != bulkReadLen)
        {
            exitFlag = 1; // Last Packet from the File. 
        }

        retVal = SL_DemodSendData(instance, bytesRead, data);
        if (retVal != SL_OK)
        {
            retVal = SL_ERR_CODE_DWNLD;
            break;
        }
    }
    SL_FileClose(fp);
    return retVal;
}

static SL_Result_t SL_DemodReset(int instance)
{
    SL_Result_t retVal = SL_OK;

    retVal = SL_DemodGpioReset(instance);
    return retVal;
}

static SL_Result_t SL_DemodTunerReset(int instance)
{
    SL_Result_t retVal = SL_OK;

        /* SL Tuner MCM Chip Reset */
    if (demodBlock[instance].boardInfo.chipType == SL_CHIP_4000)
        {
        if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_BB)
            {
                unsigned int setVal = 0x0000CCCC;
                retVal = SL_DemodWriteBytes(instance, SET_MCM_TUNER_RESET, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
            }
            else
            {

            }
      
    }
    return retVal;
}

static SL_Result_t SL_DemodCodedownload(int instance, char* iccmFile, char* dccmFile, char* secondaryFile)
{
    SL_Result_t retVal = SL_OK;

    retVal = SL_DemodUpdateFileLengthInINFOCommond(iccmFile, demodBlock[instance].iccmCmdInfo);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "ICCM File Not Found");
        return retVal;
    }

	for(int i=0;i<cmdLen;i++)
		SL_Printf(" iccmCmdInfo[%d]=0x%x\n",i, demodBlock[instance].iccmCmdInfo[i]);

    SL_SleepMS(100);
    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].iccmCmdInfo);

    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "ICCM Commnad Information Tranfer Failed");
        return retVal;
    }

    retVal = SL_DemodDownloadHexFile(instance, iccmFile);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "ICCM File Download Failed");
        return retVal;
    }

    retVal = SL_DemodUpdateFileLengthInINFOCommond(dccmFile, demodBlock[instance].dccmCmdInfo);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "DCCM File Not Found");
        return retVal;
    }
	for(int i=0;i<cmdLen;i++)
		SL_Printf(" dccmCmdInfo[%d]=0x%x\n",i, demodBlock[instance].dccmCmdInfo[i]);

    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].dccmCmdInfo);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "DCCM Commnad Information Tranfer Failed");
        return retVal;
    }
    retVal = SL_DemodDownloadHexFile(instance, dccmFile);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "DCCM File Download Failed");
        return retVal;
    }
	for(int i=0;i<cmdLen;i++)
		SL_Printf(" executeCmdInfo[%d]=0x%x\n",i, demodBlock[instance].executeCmdInfo[i]);

    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].executeCmdInfo);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "Execute Commnad Information Tranfer Failed");
        return retVal;
    }

    SL_SleepMS(1000); // 2000;
	SL_Printf(" secondaryFile=%s\n",secondaryFile);

    retVal = SL_DemodDownloadHexFile(instance, secondaryFile);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "Secondary File Download Failed");
        return retVal;
    }
    return retVal;
}



#if 1
static SL_Result_t  ITE_SL_DemodCodedownload(int instance, char* iccmFile, char* dccmFile, char* secondaryFile,unsigned int* fileLen)
{
    SL_Result_t retVal = SL_OK;

    for (int idx = 0; idx < 4; idx++)
    {
        demodBlock[instance].iccmCmdInfo[cmdLenIdx + idx] = (unsigned char)((fileLen[0] >> (8 * idx)) & 0xFF);
		//SL_Printf("\n[SL_DemodCodedownload_ITE]iccmCmdInfo[%d] =%x\n",cmdLenIdx + idx,demodBlock[instance].iccmCmdInfo[cmdLenIdx + idx] );
    }

    SL_SleepMS(100);
    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].iccmCmdInfo);


    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "ICCM Commnad Information Tranfer Failed");
        return retVal;
    }


    retVal =  ITE_SL_DemodDownloadHexFile(instance, iccmFile,fileLen[0]);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "ICCM File Download Failed");
        return retVal;
    }


	for (int idx = 0; idx < 4; idx++)
	{
		demodBlock[instance].dccmCmdInfo[cmdLenIdx + idx] = (unsigned char)((fileLen[1] >> (8 * idx)) & 0xFF);
		//SL_Printf("\n[SL_DemodUpdateFileLengthInINFOCommond]dccmCmdInfo[%d] =%x\n",cmdLenIdx + idx,demodBlock[instance].dccmCmdInfo[cmdLenIdx + idx] );
	}

    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].dccmCmdInfo);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "DCCM Commnad Information Tranfer Failed");
        return retVal;
    }

    retVal =  ITE_SL_DemodDownloadHexFile(instance, dccmFile,fileLen[1]);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "DCCM File Download Failed");
        return retVal;
    }


    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].executeCmdInfo);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "Execute Commnad Information Tranfer Failed");
        return retVal;
    }

    SL_SleepMS(1000); // 2000;


    retVal =  ITE_SL_DemodDownloadHexFile(instance, secondaryFile,fileLen[2]);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "Secondary File Download Failed");
        return retVal;
    }
#if 0		
	int i=0;
	for(i=0;i<cmdLen;i++)
		SL_Printf(" iccmCmdInfo[%d]=0x%x\n",i, demodBlock[instance].iccmCmdInfo[i]);
	for(i=0;i<cmdLen;i++)
		SL_Printf(" dccmCmdInfo[%d]=0x%x\n",i, demodBlock[instance].dccmCmdInfo[i]);	
	for(i=0;i<cmdLen;i++)
		SL_Printf(" executeCmdInfo[%d]=0x%x\n",i, demodBlock[instance].executeCmdInfo[i]);	
#endif	
    return retVal;
}
#else
static SL_Result_t  ITE_SL_DemodCodedownload(int instance, char* iccmFile, char* dccmFile, char* secondaryFile,unsigned int* fileLen)
{
    SL_Result_t retVal = SL_OK;

	LARGE_INTEGER t1, t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12, ts;
	QueryPerformanceFrequency(&ts);
	QueryPerformanceCounter(&t1);

    for (int idx = 0; idx < 4; idx++)
    {
        demodBlock[instance].iccmCmdInfo[cmdLenIdx + idx] = (unsigned char)((fileLen[0] >> (8 * idx)) & 0xFF);
		//SL_Printf("\n[SL_DemodCodedownload_ITE]iccmCmdInfo[%d] =%x\n",cmdLenIdx + idx,demodBlock[instance].iccmCmdInfo[cmdLenIdx + idx] );
    }
	QueryPerformanceCounter(&t2);
	
	printf("after iccm FileLengthInINFO: %lf(s)\n",(t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart));

    SL_SleepMS(100);
	QueryPerformanceCounter(&t3);
	
	printf("after 100ms : %lf(s)\n",(t3.QuadPart-t2.QuadPart)/(double)(ts.QuadPart));

    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].iccmCmdInfo);



    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "ICCM Commnad Information Tranfer Failed");
        return retVal;
    }
	QueryPerformanceCounter(&t4);
	
	printf("after iccm  cmd : %lf(s)\n",(t4.QuadPart-t3.QuadPart)/(double)(ts.QuadPart));


    retVal =  ITE_SL_DemodDownloadHexFile(instance, iccmFile,fileLen[0]);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "ICCM File Download Failed");
        return retVal;
    }
	QueryPerformanceCounter(&t5);
	
	printf("after iccm  download : %lf(s)\n",(t5.QuadPart-t4.QuadPart)/(double)(ts.QuadPart));


	for (int idx = 0; idx < 4; idx++)
	{
		demodBlock[instance].dccmCmdInfo[cmdLenIdx + idx] = (unsigned char)((fileLen[1] >> (8 * idx)) & 0xFF);
		//SL_Printf("\n[SL_DemodUpdateFileLengthInINFOCommond]dccmCmdInfo[%d] =%x\n",cmdLenIdx + idx,demodBlock[instance].dccmCmdInfo[cmdLenIdx + idx] );
	}
	QueryPerformanceCounter(&t6);
	
	printf("after DCCM FileLengthInINFO: %lf(s)\n",(t6.QuadPart-t5.QuadPart)/(double)(ts.QuadPart));

    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].dccmCmdInfo);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "DCCM Commnad Information Tranfer Failed");
        return retVal;
    }
	QueryPerformanceCounter(&t7);
	
	printf("after DCCM  cmd : %lf(s)\n",(t7.QuadPart-t6.QuadPart)/(double)(ts.QuadPart));

    retVal =  ITE_SL_DemodDownloadHexFile(instance, dccmFile,fileLen[1]);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "DCCM File Download Failed");
        return retVal;
    }

	QueryPerformanceCounter(&t8);
	
	printf("after DCCM  download : %lf(s)\n",(t8.QuadPart-t7.QuadPart)/(double)(ts.QuadPart));

    retVal = SL_DemodSendData(instance, cmdLen, demodBlock[instance].executeCmdInfo);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "Execute Commnad Information Tranfer Failed");
        return retVal;
    }
	QueryPerformanceCounter(&t9);
	
	printf("after excute cmd : %lf(s)\n",(t9.QuadPart-t8.QuadPart)/(double)(ts.QuadPart));

    SL_SleepMS(1000); // 2000;
#if 0    
	if (demodBlock[instance].comPrtcl == SL_CMD_CONTROL_IF_I2C)
	{
		SL_I2cResult_t retVal = SL_I2cSetBitRate(SL_I2C_1MHz);
		if (retVal != SL_I2C_OK)
		{
			SL_Log(SL_LOGTYPE_ERROR, "Failed to set I2C bitrate");
			return SL_ERR_OPERATION_FAILED;
		}
	}
#endif	
	QueryPerformanceCounter(&t10);
	
	printf("after 1s : %lf(s)\n",(t10.QuadPart-t9.QuadPart)/(double)(ts.QuadPart));


    retVal =  ITE_SL_DemodDownloadHexFile(instance, secondaryFile,fileLen[2]);
    if (retVal != SL_OK)
    {
        SL_Log(SL_LOGTYPE_ERROR, "Secondary File Download Failed");
        return retVal;
    }
	QueryPerformanceCounter(&t11);
	
	printf("after Secondary  download : %lf(s)\n",(t11.QuadPart-t10.QuadPart)/(double)(ts.QuadPart));	
#if 0    

    if (demodBlock[instance].comPrtcl == SL_CMD_CONTROL_IF_I2C)
    {
		SL_I2cResult_t retVal = SL_I2cSetBitRate(SL_I2C_DEFAULUT);
        if (retVal != SL_I2C_OK)
        {
            SL_Log(SL_LOGTYPE_ERROR, "Failed to set I2C bitrate");
			return SL_ERR_OPERATION_FAILED;
       }
    }	
	#endif	
#if 0		
	int i=0;
	for(i=0;i<cmdLen;i++)
		SL_Printf(" iccmCmdInfo[%d]=0x%x\n",i, demodBlock[instance].iccmCmdInfo[i]);
	for(i=0;i<cmdLen;i++)
		SL_Printf(" dccmCmdInfo[%d]=0x%x\n",i, demodBlock[instance].dccmCmdInfo[i]);	
	for(i=0;i<cmdLen;i++)
		SL_Printf(" executeCmdInfo[%d]=0x%x\n",i, demodBlock[instance].executeCmdInfo[i]);	
#endif	
    return retVal;
}

#endif
static SL_Result_t SL_DemodSetParams(int instance)
{
    SL_Result_t retVal = SL_OK;
    //SL_PlatFormConfigParams_t demodConfigParam = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t demodConfigParam ;

    if (SL_ConfigGetPlatform(&demodConfigParam) == SL_CONFIG_OK)
    {
        demodBlock[instance].hexFilePath = demodConfigParam.slsdkPath;
        demodBlock[instance].boardInfo = demodConfigParam;

        if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_AA)
        {
            demodBlock[instance].baseAddr = SL_DEMOD_AA_REG_BASE_ADDR;
        }
        else if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_BB)
        {
            demodBlock[instance].baseAddr = SL_DEMOD_BB_REG_BASE_ADDR;
        }
        else
        {
            retVal = SL_ERR_OPERATION_FAILED;
        }

        /* Set Firmware Download Location */
        /* ICCM Command Info */
        demodBlock[instance].iccmCmdInfo[0] = 0x57;
        demodBlock[instance].iccmCmdInfo[1] = 0x13;
        demodBlock[instance].iccmCmdInfo[2] = 0xf1;
        demodBlock[instance].iccmCmdInfo[3] = 0xf1;

        demodBlock[instance].iccmCmdInfo[4] = 0x00;
        demodBlock[instance].iccmCmdInfo[5] = 0x00;
        demodBlock[instance].iccmCmdInfo[6] = 0x00;
        demodBlock[instance].iccmCmdInfo[7] = 0x20;

        /* DCCM Command Info */
        demodBlock[instance].dccmCmdInfo[0] = 0x57;
        demodBlock[instance].dccmCmdInfo[1] = 0x13;
        demodBlock[instance].dccmCmdInfo[2] = 0xD3;
        demodBlock[instance].dccmCmdInfo[3] = 0xD3;

        demodBlock[instance].dccmCmdInfo[4] = (unsigned char)((demodBlock[instance].baseAddr & 0xFF) >> 0);
        demodBlock[instance].dccmCmdInfo[5] = (unsigned char)((demodBlock[instance].baseAddr & 0xFF00) >> 8);
        demodBlock[instance].dccmCmdInfo[6] = (unsigned char)((demodBlock[instance].baseAddr & 0xFF0000) >> 16);
        demodBlock[instance].dccmCmdInfo[7] = (unsigned char)((demodBlock[instance].baseAddr & 0xFF000000) >> 24);

        /* Execute Command Info */
        demodBlock[instance].executeCmdInfo[0] = 0x68;
        demodBlock[instance].executeCmdInfo[1] = 0x24;
        demodBlock[instance].executeCmdInfo[2] = 0xf1;
        demodBlock[instance].executeCmdInfo[3] = 0xf1;

        demodBlock[instance].executeCmdInfo[4] = demodBlock[instance].iccmCmdInfo[4];
        demodBlock[instance].executeCmdInfo[5] = demodBlock[instance].iccmCmdInfo[5];
        demodBlock[instance].executeCmdInfo[6] = demodBlock[instance].iccmCmdInfo[6];
        demodBlock[instance].executeCmdInfo[7] = demodBlock[instance].iccmCmdInfo[7];

    }
    else
    {
        retVal = SL_ERR_OPERATION_FAILED;
    }
    return retVal;
}

static SL_Result_t SL_DemodSetPrtcl(int instance, SL_CmdControlIf_t mod)
{
    switch (mod)
    {
    case SL_CMD_CONTROL_IF_I2C:
        demodBlock[instance].comPrtcl = mod;
        return SL_OK;

    case SL_CMD_CONTROL_IF_SPI:
        demodBlock[instance].comPrtcl = mod;
        return SL_ERR_NOT_SUPPORTED;

    case SL_CMD_CONTROL_IF_SDIO:
        demodBlock[instance].comPrtcl = mod;
        return SL_ERR_NOT_SUPPORTED;

    default:
        return SL_ERR_INVALID_ARGUMENTS;
    }
}

static SL_Result_t ITE_SL_DemodSetStandard(int instance, SL_DemodStd_t std)
{
    SL_Result_t retVal = SL_OK;
	switch (std)
	{
		case SL_DEMODSTD_ATSC1_0:
			SL_Printf("\n[ITE_SL_DemodSetStandard]Set ATSC1 Stadard\n");
			demodBlock[instance].std = std;
			demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
			break;
		case SL_DEMODSTD_ATSC3_0:
			SL_Printf("\n[ITE_SL_DemodSetStandard]Set ATSC3 Stadard\n");
			demodBlock[instance].std = std;
			demodBlock[instance].fAdc = SL_DEMOD_FADC_64MHZ;
			break;

		default:
			SL_Printf("[ITE_SL_DemodSetStandard]SL_ERR_INVALID_ARGUMENTS!\n");
			retVal = SL_ERR_INVALID_ARGUMENTS;
			break;
	}

    return retVal;

}
static SL_Result_t SL_DemodSetStandard(int instance, SL_DemodStd_t std)
{
    SL_Result_t retVal = SL_OK;
    demodBlock[instance].std = std;
    //SL_PlatFormConfigParams_t chipRevInfo = SL_PLATFORM_CONFIG_PARAMS_NULL_INITIALIZER;
	SL_PlatFormConfigParams_t chipRevInfo ;
    SL_BbCapture_t isBaseBand;

    if (SL_ConfigGetPlatform(&chipRevInfo) == SL_CONFIG_OK)
    {
    switch (std)
    {
    case SL_DEMODSTD_ATSC3_0:
            if (chipRevInfo.chipRev == SL_CHIP_REV_AA)
            {
                if (SL_ConfigGetBbCapture(&isBaseBand) == SL_CONFIG_OK)
                {
                    if (isBaseBand)
                    {
                        demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/atsc3_aa/iccmbbc.hex");
                        demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/atsc3_aa/dccmbbc.hex");
                        demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/atsc3_aa/atsc3bbc.hex");
                    }
                    else
                    {
                        demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/atsc3_aa/iccm.hex");
                        demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/atsc3_aa/dccm.hex");
                        demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/atsc3_aa/atsc3.hex");
                    }
                }
                else
                {
                    retVal = SL_ERR_OPERATION_FAILED;
                } 
            }
            else if (chipRevInfo.chipRev == SL_CHIP_REV_BB)
            {
                if (SL_ConfigGetBbCapture(&isBaseBand) == SL_CONFIG_OK)
                {
                    if (isBaseBand)
                    {
                        demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver/SL3000/fx3s/std_bin/atsc3_bb/iccmbbc_bb.hex");
                        demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver/SL3000/fx3s/std_bin/atsc3_bb/dccmbbc_bb.hex");
                        demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver/SL3000/fx3s/std_bin/atsc3_bb/atsc3bbc_bb.hex");
                    }
                    else
                    {
                        //demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver/SL3000/fx3s/std_bin/atsc3_bb/iccm_bb.hex");
                        //demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver/SL3000/fx3s/std_bin/atsc3_bb/dccm_bb.hex");
                        //demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver/SL3000/fx3s/std_bin/atsc3_bb/atsc3_bb.hex");
                        demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc3_bb\\iccm_bb.hex");
						demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc3_bb\\dccm_bb.hex");
						demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc3_bb\\atsc3_bb.hex");
                    }
                }
                else
                {
                    retVal = SL_ERR_OPERATION_FAILED;
                }
		
            }

			
            demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc3_bb\\iccm_bb.hex");
            demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc3_bb\\dccm_bb.hex");
            demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc3_bb\\atsc3_bb.hex");	
			SL_Printf(" demodBlock[instance].iccmFile=%s\n", demodBlock[instance].iccmFile);

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_64MHZ;
            break;

        case SL_DEMODSTD_ATSC1_0:
#if 0
            if (chipRevInfo.chipRev == SL_CHIP_REV_AA)
            {
                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver.SL3000.fx3s.std_bin.atsc1_bb/iccm.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver.SL3000.fx3s.std_bin.atsc1_bb/dccm.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver.SL3000.fx3s.std_bin.atsc1_bb/atsc1.hex");
            }
            else if (chipRevInfo.chipRev == SL_CHIP_REV_BB)
            {
                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver.SL3000.fx3s.std_bin.atsc1_bb/iccm_bb.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver.SL3000.fx3s.std_bin.atsc1_bb/dccm_bb.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver.SL3000.fx3s.std_bin.atsc1_bb/atsc1_bb.hex");
        }

                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "//api//Receiver//SL3000//fx3s//std_bin//atsc1_bb//iccm_bb.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver/SL3000/fx3s/std_bin/atsc1_bb/dccm_bb.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/api/Receiver/SL3000/fx3s/std_bin/atsc1_bb/atsc1_bb.hex");		
#endif
            demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc1_bb\\iccm_bb.hex");
            demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc1_bb\\dccm_bb.hex");
            demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "atsc1_bb\\atsc1_bb.hex");		
                
			/* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
			SL_Printf(" demodBlock[instance].iccmFile=%s\n", demodBlock[instance].iccmFile);
            break;

        case SL_DEMODSTD_DVB_T:
            if (chipRevInfo.chipRev == SL_CHIP_REV_BB)
            {
                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt_bb/iccm_bb.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt_bb/dccm_bb.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt_bb/dvbt_bb.hex");
            }
            else
            {
                retVal = SL_ERR_NOT_SUPPORTED;
            }

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
            break;

    case SL_DEMODSTD_DVB_T2:
            if (chipRevInfo.chipRev == SL_CHIP_REV_AA)
            {
                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt2_aa/iccm.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt2_aa/dccm.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt2_aa/dvbt2.hex");
            }
            else if (chipRevInfo.chipRev == SL_CHIP_REV_BB)
            {
                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt2_bb/iccm_bb.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt2_bb/dccm_bb.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/dvbt2_bb/dvbt2_bb.hex");
            }

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
            break;

    case SL_DEMODSTD_DTMB:
            retVal = SL_ERR_NOT_SUPPORTED;
            break;

    case SL_DEMODSTD_DVB_S:
            retVal = SL_ERR_NOT_SUPPORTED;
            break;

    case SL_DEMODSTD_DVB_S2:
            retVal = SL_ERR_NOT_SUPPORTED;
            break;

    case SL_DEMODSTD_DVB_C:
            retVal = SL_ERR_NOT_SUPPORTED;
            break;

    case SL_DEMODSTD_ISDB_C:
            retVal = SL_ERR_NOT_SUPPORTED;
            break;

    case SL_DEMODSTD_ISDB_T:
            if (chipRevInfo.chipRev == SL_CHIP_REV_BB)
            {
                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/isdbt_bb/iccm_bb.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/isdbt_bb/dccm_bb.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/isdbt_bb/isdbt_bb.hex");
            }
            else
            {
                retVal = SL_ERR_NOT_SUPPORTED;
            }

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
            break;

    case SL_DEMODSTD_US_C:
            retVal = SL_ERR_NOT_SUPPORTED;
            break;

    case SL_DEMODSTD_INT_CALIBRATION:
        if (SL_ConfigGetPlatform(&chipRevInfo) == SL_CONFIG_OK)
        {
            if (chipRevInfo.chipRev == SL_CHIP_REV_AA)
            {
                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/calib_aa/iccm.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/calib_aa/dccm.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/calib_aa/calib.hex");
            }
            else if (chipRevInfo.chipRev == SL_CHIP_REV_BB)
            {
                demodBlock[instance].iccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/calib_bb/iccm_bb.hex");
                demodBlock[instance].dccmFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/calib_bb/dccm_bb.hex");
                demodBlock[instance].secondaryFile = SL_StrAppend(demodBlock[instance].hexFilePath, "/slapi/bin/calib_bb/calib_bb.hex");
            }
        }
        else
        {
                retVal = SL_ERR_OPERATION_FAILED;
            }

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_64MHZ;
            break;

        default:
            retVal = SL_ERR_INVALID_ARGUMENTS;
        }
    }
    else
    {
        retVal = SL_ERR_OPERATION_FAILED;
    }
    return retVal;
}
#if 0
static SL_Result_t SL_DemodSetStandard(int instance, SL_DemodStd_t std)
{
    SL_Result_t retVal = SL_OK;
    SL_BbCapture_t isBaseBand;
    char stdName[10] = { 0 };
    char interfaceInfo[25] = { 0 };
    char chipRev[5] = { 0 };
    char bbCap[8] = { 0 };
    demodBlock[instance].std = std;

    SL_StrCat(interfaceInfo, "_");
    // 1. Set Code Download Interface
    if (demodBlock[instance].boardInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
    {
        SL_StrCat(interfaceInfo, "i2c");
    }
    else if (demodBlock[instance].boardInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
    {
        SL_StrCat(interfaceInfo, "sdio");
    }
    else if (demodBlock[instance].boardInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
    {
        SL_StrCat(interfaceInfo, "spi");
    }
    else
    {
        SL_StrCat(interfaceInfo, "NA");
    }

    SL_StrCat(interfaceInfo, "_");
    // 2. Set Control Interface
    if (demodBlock[instance].boardInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_I2C)
    {
        SL_StrCat(interfaceInfo, "i2c");
    }
    else if (demodBlock[instance].boardInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SDIO)
    {
        SL_StrCat(interfaceInfo, "sdio");
    }
    else if (demodBlock[instance].boardInfo.demodControlIf == SL_DEMOD_CMD_CONTROL_IF_SPI)
    {
        SL_StrCat(interfaceInfo, "spi");
    }
    else
    {
        SL_StrCat(interfaceInfo, "NA");
    }

    SL_StrCat(interfaceInfo, "_");
    // 3. Set Output Interface
    if (demodBlock[instance].boardInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_TS)
    {
        SL_StrCat(interfaceInfo, "ts");
    }
    else if (demodBlock[instance].boardInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SDIO)
    {
        SL_StrCat(interfaceInfo, "sdio");
    }
    else if (demodBlock[instance].boardInfo.demodOutputIf == SL_DEMOD_OUTPUTIF_SPI)
    {
        SL_StrCat(interfaceInfo, "spi");
    }
    else
    {
        SL_StrCat(interfaceInfo, "NA");
    }

    SL_StrCat(interfaceInfo, "_");
    // 4. Set chip type ( 30x0 or 4000)
    if (demodBlock[instance].boardInfo.chipType == SL_CHIP_4000)
    {
        SL_StrCat(interfaceInfo, "4000");
    }
    else
    {
        SL_StrCat(interfaceInfo, "30x0");
    }

    // 5. Set chip Rev ( AA or BB)
    if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_AA)
    {
        SL_StrCat(chipRev, "aa");
    }
    else if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_BB)
    {
        SL_StrCat(chipRev, "bb");
    }
    else
    {
        SL_StrCat(chipRev, "NA");
    }

    //  6. Set BaseBand Capture 
    if (SL_ConfigGetBbCapture(&isBaseBand) == SL_CONFIG_OK)
    {
        if (isBaseBand)
        {
            SL_StrCat(bbCap, "_");
            SL_StrCat(bbCap, "bbcap");
        }
    }
    else
    {
        retVal = SL_ERR_OPERATION_FAILED;
        }

    if (retVal == SL_OK)
    {
        /* Update Demod Standard */
        switch (demodBlock[instance].std)
        {
        case SL_DEMODSTD_ATSC3_0:
            SL_StrCopy(stdName, "atsc3");

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_64MHZ;
            break;

        case SL_DEMODSTD_ATSC1_0:
            SL_StrCopy(stdName, "atsc1");

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
            break;

        case SL_DEMODSTD_DVB_T:
            SL_StrCopy(stdName, "dvbt");

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
            break;

        case SL_DEMODSTD_DVB_T2:
            SL_StrCopy(stdName, "dvbt2");

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
            break;

        case SL_DEMODSTD_DTMB:
        case SL_DEMODSTD_DVB_S:
        case SL_DEMODSTD_DVB_S2:
        case SL_DEMODSTD_DVB_C:
        case SL_DEMODSTD_ISDB_C:
        case SL_DEMODSTD_US_C:
            retVal = SL_ERR_NOT_SUPPORTED;
            break;

        case SL_DEMODSTD_ISDB_T:
            SL_StrCopy(stdName, "isdbt");

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_32MHZ;
            break;

        case SL_DEMODSTD_INT_CALIBRATION:
            SL_StrCopy(stdName, "calib");

            /* Set FADC Value*/
            demodBlock[instance].fAdc = SL_DEMOD_FADC_64MHZ;
        break;

    default:
            retVal = SL_ERR_INVALID_ARGUMENTS;
        }
    }
#if 0
    if (demodBlock[instance].std == SL_DEMODSTD_ATSC3_0)
    {
        /* Update FW file path*/
        // 1. PMEM (ICCM) File
        memset(demodBlock[instance].iccmFile, 0, sizeof(demodBlock[instance].iccmFile) / sizeof(char));
        SL_StrCat(demodBlock[instance].iccmFile, demodBlock[instance].boardInfo.slsdkPath);
        SL_StrCat(demodBlock[instance].iccmFile, "/slapi/bin/");
        SL_StrCat(demodBlock[instance].iccmFile, stdName);
        SL_StrCat(demodBlock[instance].iccmFile, "/");
        SL_StrCat(demodBlock[instance].iccmFile, chipRev);
        SL_StrCat(demodBlock[instance].iccmFile, "/");
        SL_StrCat(demodBlock[instance].iccmFile, stdName);
        SL_StrCat(demodBlock[instance].iccmFile, "Ctl_p");
        SL_StrCat(demodBlock[instance].iccmFile, interfaceInfo);
        SL_StrCat(demodBlock[instance].iccmFile, bbCap);
        SL_StrCat(demodBlock[instance].iccmFile, ".hex");
        

        // 2. DMEM (DCCM) File
        memset(demodBlock[instance].dccmFile, 0, sizeof(demodBlock[instance].dccmFile) / sizeof(char));
        SL_StrCat(demodBlock[instance].dccmFile, demodBlock[instance].boardInfo.slsdkPath);
        SL_StrCat(demodBlock[instance].dccmFile, "/slapi/bin/");
        SL_StrCat(demodBlock[instance].dccmFile, stdName);
        SL_StrCat(demodBlock[instance].dccmFile, "/");
        SL_StrCat(demodBlock[instance].dccmFile, chipRev);
        SL_StrCat(demodBlock[instance].dccmFile, "/");
        SL_StrCat(demodBlock[instance].dccmFile, stdName);
        SL_StrCat(demodBlock[instance].dccmFile, "Ctl_d");
        SL_StrCat(demodBlock[instance].dccmFile, interfaceInfo);
        SL_StrCat(demodBlock[instance].dccmFile, bbCap);
        SL_StrCat(demodBlock[instance].dccmFile, ".hex");

        // 3. Demod Standard (DSP) File
        memset(demodBlock[instance].secondaryFile, 0, sizeof(demodBlock[instance].secondaryFile) / sizeof(char));
        SL_StrCat(demodBlock[instance].secondaryFile, demodBlock[instance].boardInfo.slsdkPath);
        SL_StrCat(demodBlock[instance].secondaryFile, "/slapi/bin/");
        SL_StrCat(demodBlock[instance].secondaryFile, stdName);
        SL_StrCat(demodBlock[instance].secondaryFile, "/");
        SL_StrCat(demodBlock[instance].secondaryFile, chipRev);
        SL_StrCat(demodBlock[instance].secondaryFile, "/");
        SL_StrCat(demodBlock[instance].secondaryFile, stdName);
        SL_StrCat(demodBlock[instance].secondaryFile, bbCap);
        SL_StrCat(demodBlock[instance].secondaryFile, "_");
        SL_StrCat(demodBlock[instance].secondaryFile, chipRev);
        SL_StrCat(demodBlock[instance].secondaryFile, ".hex");
    }
#endif
    if (demodBlock[instance].std == SL_DEMODSTD_ATSC3_0)
    {
        memset(demodBlock[instance].iccmFile, 0, sizeof(demodBlock[instance].iccmFile) / sizeof(char));
        memset(demodBlock[instance].dccmFile, 0, sizeof(demodBlock[instance].dccmFile) / sizeof(char));
        memset(demodBlock[instance].secondaryFile, 0, sizeof(demodBlock[instance].secondaryFile) / sizeof(char));
        
        if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_AA)
            {
                if (SL_ConfigGetBbCapture(&isBaseBand) == SL_CONFIG_OK)
                {
                    if (isBaseBand)
                    {
                        SL_StrCat(demodBlock[instance].iccmFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].iccmFile,"/slapi/bin/atsc3_aa/iccmbbc_AA.hex" );
                        SL_StrCat(demodBlock[instance].dccmFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].dccmFile, "/slapi/bin/atsc3_aa/dccmbbc_AA.hex");
                        SL_StrCat(demodBlock[instance].secondaryFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].secondaryFile, "/slapi/bin/atsc3_aa/atsc3bbc_AA.hex");
                    }
                    else
                    {
                        SL_StrCat(demodBlock[instance].iccmFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].iccmFile, "/slapi/bin/atsc3_aa/iccm_AA.hex");
                        SL_StrCat(demodBlock[instance].dccmFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].dccmFile, "/slapi/bin/atsc3_aa/dccm_AA.hex");
                        SL_StrCat(demodBlock[instance].secondaryFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].secondaryFile, "/slapi/bin/atsc3_aa/atsc3_AA.hex");
                    }
                }
                else
                {
                    return SL_ERR_OPERATION_FAILED;
                }
            }
            else if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_BB)
            {
                if (SL_ConfigGetBbCapture(&isBaseBand) == SL_CONFIG_OK)
                {
                    if (isBaseBand)
                    {
                        SL_StrCat(demodBlock[instance].iccmFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].iccmFile, "/slapi/bin/atsc3_bb/iccmbbc_BB.hex");
                        SL_StrCat(demodBlock[instance].dccmFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].dccmFile, "/slapi/bin/atsc3_bb/dccmbbc_BB.hex");
                        SL_StrCat(demodBlock[instance].secondaryFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].secondaryFile, "/slapi/bin/atsc3_bb/atsc3bbc_BB.hex");
                    }
                    else
                    {
                        SL_StrCat(demodBlock[instance].iccmFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].iccmFile, "/slapi/bin/atsc3_bb/iccm_BB.hex");
                        SL_StrCat(demodBlock[instance].dccmFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].dccmFile, "/slapi/bin/atsc3_bb/dccm_BB.hex");
                        SL_StrCat(demodBlock[instance].secondaryFile, demodBlock[instance].boardInfo.slsdkPath);
                        SL_StrCat(demodBlock[instance].secondaryFile, "/slapi/bin/atsc3_bb/atsc3_BB.hex");
                    }
                }
                else
                {
                    return SL_ERR_OPERATION_FAILED;
                }
            }
    }
    else
    {
        /* Update FW file path*/
        // 1. PMEM (ICCM) File
        memset(demodBlock[instance].iccmFile, 0, sizeof(demodBlock[instance].iccmFile) / sizeof(char));
        SL_StrCat(demodBlock[instance].iccmFile, demodBlock[instance].boardInfo.slsdkPath);
        SL_StrCat(demodBlock[instance].iccmFile, "/slapi/bin/");
        SL_StrCat(demodBlock[instance].iccmFile, stdName);
        SL_StrCat(demodBlock[instance].iccmFile, "_");
        SL_StrCat(demodBlock[instance].iccmFile, chipRev);
        SL_StrCat(demodBlock[instance].iccmFile, "/");
        SL_StrCat(demodBlock[instance].iccmFile, "iccm");
        if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_BB)
        {
            SL_StrCat(demodBlock[instance].iccmFile, "_");
            SL_StrCat(demodBlock[instance].iccmFile, chipRev);
        }
        SL_StrCat(demodBlock[instance].iccmFile, ".hex");

        // 2. DMEM (DCCM) File
        memset(demodBlock[instance].dccmFile, 0, sizeof(demodBlock[instance].dccmFile) / sizeof(char));
        SL_StrCat(demodBlock[instance].dccmFile, demodBlock[instance].boardInfo.slsdkPath);
        SL_StrCat(demodBlock[instance].dccmFile, "/slapi/bin/");
        SL_StrCat(demodBlock[instance].dccmFile, stdName);
        SL_StrCat(demodBlock[instance].dccmFile, "_");
        SL_StrCat(demodBlock[instance].dccmFile, chipRev);
        SL_StrCat(demodBlock[instance].dccmFile, "/");
        SL_StrCat(demodBlock[instance].dccmFile, "dccm");
        if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_BB)
        {
            SL_StrCat(demodBlock[instance].dccmFile, "_");
            SL_StrCat(demodBlock[instance].dccmFile, chipRev);
        }
        SL_StrCat(demodBlock[instance].dccmFile, ".hex");

        // 3. Demod Standard (DSP) File
        memset(demodBlock[instance].secondaryFile, 0, sizeof(demodBlock[instance].secondaryFile) / sizeof(char));
        SL_StrCat(demodBlock[instance].secondaryFile, demodBlock[instance].boardInfo.slsdkPath);
        SL_StrCat(demodBlock[instance].secondaryFile, "/slapi/bin/");
        SL_StrCat(demodBlock[instance].secondaryFile, stdName);
        SL_StrCat(demodBlock[instance].secondaryFile, "_");
        SL_StrCat(demodBlock[instance].secondaryFile, chipRev);
        SL_StrCat(demodBlock[instance].secondaryFile, "/");
        SL_StrCat(demodBlock[instance].secondaryFile, stdName);
        if (demodBlock[instance].boardInfo.chipRev == SL_CHIP_REV_BB)
        {
            SL_StrCat(demodBlock[instance].secondaryFile, "_");
            SL_StrCat(demodBlock[instance].secondaryFile, chipRev);
        }
        SL_StrCat(demodBlock[instance].secondaryFile, ".hex");
    }
    return retVal;
}

#endif

static SL_Result_t SL_DemodGetStandard(int instance, SL_DemodConfigInfo_t *demodStd)
{
    SL_Result_t retVal = SL_OK;

    retVal = SL_DemodReadBytes(instance, GET_DEMOD_STD, sizeof(SL_DemodStd_t), (SL_DemodConfigInfo_t*)&demodStd->std);
    return retVal;
}

static SL_Result_t SL_DemodGetAfeIfConfigInfo(int instance, SL_AfeIfConfigParams_t *afeIf)
{
    SL_Result_t retVal = SL_OK;
    int getVal = 0;

    retVal = SL_DemodReadBytes(instance, SET_IF_SIGNAL_CHAR, SL_DEMOD_REG_SIZE, (int*)&getVal);
    if (retVal == SL_OK)
    {
		afeIf->iftype = (SL_IfType_t)(getVal & 0xFF);
        getVal = (getVal & 0xFF00) >> 8;
		afeIf->iswap = (SL_IPolaritySwap_t)(getVal & 0x01);
		afeIf->qswap = (SL_QPolaritySwap_t)((getVal >> 1) & 0x01);

        retVal = SL_DemodReadBytes(instance, SET_IQ_SWAP, SL_DEMOD_REG_SIZE, (SL_IQSwap_t*)&afeIf->iqswap);
    }

    if (retVal == SL_OK)
    {
        retVal = SL_DemodReadBytes(instance, SET_TUNER_IF_FREQ, SL_DEMOD_REG_SIZE, (int*)&getVal);
    }

    if (retVal == SL_OK)
    {
        double ncoVal = ((double)getVal / Q_31);
        ((SL_AfeIfConfigParams_t*)afeIf)->ifreq = ncoVal * demodBlock[instance].fAdc;

        retVal = SL_DemodReadBytes(instance, SET_AGC_REF_VALUE, SL_DEMOD_REG_SIZE, (int*)&getVal);
        if (retVal == SL_OK)
        {
            double afcRefVal = (0xFFFF & getVal); // Lower 16 bit contains AGC Reference 
            ((SL_AfeIfConfigParams_t*)afeIf)->agcRefValue = (unsigned int)((afcRefVal / Q_11) * 1000);
        }
    }
    return retVal;
}

static SL_Result_t SL_DemodGetIQOffsetCorrConfigInfo(int instance, SL_IQOffsetCorrectionParams_t *iqOffset)
{
    SL_Result_t retVal = SL_OK;
    unsigned int getVal = 0;

    retVal = SL_DemodReadBytes(instance, SET_I_COEFFICIENT_1, SL_DEMOD_REG_SIZE, (unsigned int*)&getVal);
    if (retVal == SL_OK)
    {
        iqOffset->iCoeff1 = ((float)((short)getVal) / Q_14);
        retVal = SL_DemodReadBytes(instance, SET_Q_COEFFICIENT_1, SL_DEMOD_REG_SIZE, (unsigned int*)&getVal);
    }

    if (retVal == SL_OK)
    {
        iqOffset->qCoeff1 = ((float)((short)getVal) / Q_14);
        retVal = SL_DemodReadBytes(instance, SET_I_COEFFICIENT_2, SL_DEMOD_REG_SIZE, (unsigned int*)&getVal);
    }

    if (retVal == SL_OK)
    {
        iqOffset->iCoeff2 = ((float)((short)getVal) / Q_14);
        retVal = SL_DemodReadBytes(instance, SET_Q_COEFFICIENT_2, SL_DEMOD_REG_SIZE, (unsigned int*)&getVal);
    }

    if (retVal == SL_OK)
    {
        iqOffset->qCoeff2 = ((float)((short)getVal) / Q_14);
    }
    return retVal;
}

static SL_Result_t SL_DemodGetOutIfConfigInfo(int instance, SL_OutIfConfigParams_t *outIf)
{
    SL_Result_t retVal = SL_OK;
    int getVal = 0;

    retVal = SL_DemodReadBytes(instance, SET_TSO_CLK_INV, SL_DEMOD_REG_SIZE, (SL_TsoClockInv_t*)&outIf->TsoClockInvEnable);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodReadBytes(instance, SET_OUT_CONFIG, SL_DEMOD_REG_SIZE, (int*)&getVal);
        if (retVal == SL_OK)
        {
            if (getVal == 0x0F00)
            {
                outIf->oif = SL_OUTPUTIF_SDIO;
            }
            else if (getVal == 0x1F00)
            {
                outIf->oif = SL_OUTPUTIF_SPI;
            }
            else if (getVal == 0x2F01)
            {
                outIf->oif = SL_OUTPUTIF_TSPARALLEL_LSB_FIRST;
            }
            else if (getVal == 0x2F21)
            {
                outIf->oif = SL_OUTPUTIF_TSPARALLEL_MSB_FIRST;
            }
            else if (getVal == 0x2F00)
            {
                outIf->oif = SL_OUTPUTIF_TSSERIAL_LSB_FIRST;
            }
            else if (getVal == 0x2F20)
            {
                outIf->oif = SL_OUTPUTIF_TSSERIAL_MSB_FIRST;
            }
        }
    }
    return retVal;
}

static SL_Result_t SL_DemodGetLnaMode(int instance, SL_DemodConfigInfo_t *demodCfgInfo)
{
    SL_Result_t retVal = SL_OK;

    retVal = SL_DemodReadBytes(instance, SET_LNA_MODE, SL_DEMOD_REG_SIZE, (SL_DemodConfigInfo_t*)&demodCfgInfo->extLnaMode);
    return retVal;
}

static SL_Result_t SL_DemodSetAtsc3Config(int instance, SL_Atsc3p0ConfigParams_t *atsc3Params)
{
    SL_Result_t retVal = SL_OK;
    int setRegionVal;

    retVal = SL_DemodWriteBytes(instance, SET_CONFIG_DEMOD_PLP_ID, SL_DEMOD_REG_SIZE, (SL_Atsc3p0PlpConfigParams_t*)&atsc3Params->plpConfig);
    if (retVal == SL_OK)
    {
        if (!demodBlock[instance].isStarted)
        {
            if (atsc3Params->region == SL_ATSC3P0_REGION_US)
            {
                setRegionVal = 0x11115555;
            }
            else if (atsc3Params->region == SL_ATSC3P0_REGION_KOREA)
            {
                setRegionVal = 0x44440000;
            }
            else
            {
                retVal = SL_ERR_INVALID_ARGUMENTS;
            }

            if (retVal == SL_OK)
            {
                retVal = SL_DemodWriteBytes(instance, SET_ATSC3_REGION, SL_DEMOD_REG_SIZE, (int *)&setRegionVal);
            }
        }
    }
    return retVal;
}

static SL_Result_t SL_DemodGetAtsc3Config(int instance, SL_Atsc3p0ConfigParams_t *atsc3Params)
{
    SL_Result_t retVal;
    int getRegionVal;

    retVal = SL_DemodReadBytes(instance, SET_CONFIG_DEMOD_PLP_ID, SL_DEMOD_REG_SIZE, (SL_Atsc3p0PlpConfigParams_t*)&atsc3Params->plpConfig);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodReadBytes(instance, SET_ATSC3_REGION, SL_DEMOD_REG_SIZE, (int*)&getRegionVal);
        if (retVal == SL_OK)
        {
            if (getRegionVal == 0x11115555)
            {
                atsc3Params->region = SL_ATSC3P0_REGION_US;
            }
            else if (getRegionVal == 0x44440000)
            {
                atsc3Params->region = SL_ATSC3P0_REGION_KOREA;
            }
        }
    }
    return retVal;
}


static SL_Result_t SL_DemodGetAtsc3LlsPlpList(int instance, SL_Atsc3p0LlsPlpInfo_t *llsPlpList)
{
    SL_Result_t retVal;
    unsigned int llsList[2];
    int l1bFecFrameCnt = 0;
    unsigned int numRetries = 10; // Maximum Time Out Count

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        //Wait till initial L1B frames are decoded.
          //After this will the LLS PLP info will become available for read.
           //Time out if L1B frames are not available for decode
        
        do
        {
            SL_SleepMS(200);
            retVal = SL_DemodReadBytes(instance, GET_ATSC3_DECODE_PERFORMANCE + offsetof(SL_Atsc3p0Perf_Diag_t, NumFecFrameL1b), SL_DEMOD_REG_SIZE, (int*)&l1bFecFrameCnt);
            if (retVal != SL_OK)
            {
                // Failure found, so break from loop 
                break;
            }
            numRetries--;
        } while (l1bFecFrameCnt == 0 && numRetries != 0);

        if (retVal == SL_OK)
        {
            retVal = SL_DemodReadBytes(instance, GET_MULTIPLP_LLS_INFO, 0x08, (char *)llsList);
            *llsPlpList = (unsigned long long) llsList[1];
            *llsPlpList = (*llsPlpList << 32) | (unsigned long long)(llsList[0]);
        }
    }
    else
    {
        retVal = SL_ERR_INVALID_ARGUMENTS;
    }
    return retVal;
}

static SL_Result_t SL_DemodSetAtsc1Config(int instance, SL_Atsc1p0ConfigParams_t *atsc1Params)
{
    SL_Result_t retVal;

    retVal = SL_DemodWriteBytes(instance, SET_ATSC1_BW, SL_DEMOD_REG_SIZE, (SL_Bandwidth_t*)&atsc1Params->bw);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodWriteBytes(instance, SET_ATSC1_BLOCK_SIZE, SL_DEMOD_REG_SIZE, (unsigned int*)&atsc1Params->blockSize);
    }
    return retVal;
}

static SL_Result_t SL_DemodGetAtsc1Config(int instance, SL_Atsc1p0ConfigParams_t *atsc1Params)
{
    SL_Result_t retVal;

    retVal = SL_DemodReadBytes(instance, SET_ATSC1_BW, SL_DEMOD_REG_SIZE, (SL_Bandwidth_t*)&atsc1Params->bw);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodReadBytes(instance, SET_ATSC1_BLOCK_SIZE, SL_DEMOD_REG_SIZE, (unsigned int*)&atsc1Params->blockSize);
    }
    return retVal;
}
/*
static SL_Result_t SL_DemodSetDvbtConfig(int instance, SL_DvbtConfigParams_t *dvbtParams)
{
    SL_Result_t retVal;

    retVal = SL_DemodWriteBytes(instance, SET_DVBT_BW, SL_DEMOD_REG_SIZE, (SL_Bandwidth_t*)&dvbtParams->bw);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodWriteBytes(instance, SET_DVBT_CCI_TYPE, SL_DEMOD_REG_SIZE, (SL_CciType_t*)&dvbtParams->ccitype);
    }
    return retVal;
}

static SL_Result_t SL_DemodGetDvbtConfig(int instance, SL_DvbtConfigParams_t *dvbtParams)
{
    SL_Result_t retVal;

    retVal = SL_DemodReadBytes(instance, SET_DVBT_BW, SL_DEMOD_REG_SIZE, (SL_Bandwidth_t*)&dvbtParams->bw);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodReadBytes(instance, SET_DVBT_CCI_TYPE, SL_DEMOD_REG_SIZE, (SL_CciType_t*)&dvbtParams->ccitype);
    }
    return retVal;
}

static SL_Result_t SL_DemodSetDvbt2Config(int instance, SL_Dvbt2ConfigParams_t *dvbt2Params)
{
    SL_Result_t retVal;

    retVal = SL_DemodWriteBytes(instance, SET_DVBT2_BW, SL_DEMOD_REG_SIZE, (SL_Bandwidth_t*)&dvbt2Params->bw);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodWriteBytes(instance, SET_DVBT2_CCI_TYPE, SL_DEMOD_REG_SIZE, (SL_CciType_t*)&dvbt2Params->ccitype);
    }

    if (retVal == SL_OK)
    {
        retVal = SL_DemodWriteBytes(instance, SET_CONFIG_DEMOD_PLP_ID, SL_DEMOD_REG_SIZE, (SL_DvbT2PlpConfigParams_t*)&dvbt2Params->plpConfig);
    }
    return retVal;
}

static SL_Result_t SL_DemodGetDvbt2Config(int instance, SL_Dvbt2ConfigParams_t *dvbt2Params)
{
    SL_Result_t retVal;

    retVal = SL_DemodReadBytes(instance, SET_DVBT2_BW, SL_DEMOD_REG_SIZE, (SL_Bandwidth_t*)&dvbt2Params->bw);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodReadBytes(instance, SET_DVBT2_CCI_TYPE, SL_DEMOD_REG_SIZE, (SL_CciType_t*)&dvbt2Params->ccitype);
    }

    if (retVal == SL_OK)
    {
        retVal = SL_DemodReadBytes(instance, SET_CONFIG_DEMOD_PLP_ID, SL_DEMOD_REG_SIZE, (SL_DvbT2PlpConfigParams_t*)&dvbt2Params->plpConfig);
    }
    return retVal;
}

static SL_Result_t SL_DemodSetIsdbtConfig(int instance, SL_IsdbtConfigParams_t *isdbtParams)
{
    SL_Result_t retVal;

    retVal = SL_DemodWriteBytes(instance, SET_ISDBT_BW, SL_DEMOD_REG_SIZE, (SL_Bandwidth_t*)&isdbtParams->bw);
    if (retVal == SL_OK)
    {
        retVal = SL_DemodWriteBytes(instance, SET_ISDBT_CCI_TYPE, SL_DEMOD_REG_SIZE, (SL_CciType_t*)&isdbtParams->ccitype);
    }

    if (retVal == SL_OK)
        {
        retVal = SL_DemodWriteBytes(instance, SET_ISDBT_SEGMENT_INFO, SL_DEMOD_REG_SIZE, (unsigned int*)&isdbtParams->segment);
        }
    return retVal;
}

static SL_Result_t SL_DemodGetIsdbtConfig(int instance, SL_IsdbtConfigParams_t *isdbtParams)
{
    SL_Result_t retVal = SL_OK;

    retVal = SL_DemodReadBytes(instance, SET_ISDBT_BW, SL_DEMOD_REG_SIZE, (SL_Bandwidth_t*)&isdbtParams->bw);
    if (retVal == SL_OK)
        {
        retVal = SL_DemodReadBytes(instance, SET_ISDBT_CCI_TYPE, SL_DEMOD_REG_SIZE, (SL_CciType_t*)&isdbtParams->ccitype);
        }

    if (retVal == SL_OK)
    {
        retVal = SL_DemodReadBytes(instance, SET_ISDBT_SEGMENT_INFO, SL_DEMOD_REG_SIZE, (unsigned int*)&isdbtParams->segment);
    }
    return retVal;
}
*/
SL_Result_t SL_DemodCreateInstance(int *instance)
{
    if (instance != NULL && slDemodInstance < SL_DEMOD_MAX_INSTANCES)
    {
        *instance = slDemodInstance;
        slDemodInstance++;
        demodBlock[*instance].isCreated     = SL_DEMOD_TRUE;
        demodBlock[*instance].isInitialized = SL_DEMOD_FALSE;
        demodBlock[*instance].isConfigured  = SL_DEMOD_FALSE;
        demodBlock[*instance].isStarted     = SL_DEMOD_FALSE;

        return SL_OK;
    }
    else
    {
        return SL_ERR_TOO_MANY_INSTANCES;
    }
}

SL_Result_t SL_DemodGetInstance(int *instance)
{
    if ((instance != NULL) && (slDemodInstance > 0))
    {
        if (demodBlock[slDemodInstance - 1].isCreated == SL_DEMOD_TRUE)
        {
            *instance = slDemodInstance - 1; //Create instance would have incremented it by 1.
            return SL_OK;
        }
    }
    return SL_ERR_OPERATION_FAILED;
}

SL_Result_t SL_DemodDeleteInstance(int instance)
{
    if (instance < SL_DEMOD_MAX_INSTANCES  && instance >= 0)
    {
        if (demodBlock[instance].isCreated == SL_DEMOD_TRUE)
        {
            slDemodInstance--;
            memset(&demodBlock[instance], 0, sizeof(SL_DemodBlock_t));
            return SL_OK;
        }
    }
    return SL_ERR_INVALID_ARGUMENTS; //instance not yet created or not matching
}

SL_Result_t SL_DemodInit(int instance, SL_CmdControlIf_t ctrl, SL_DemodStd_t std)
{
    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0 && demodBlock[instance].isCreated)
    {
        SL_Result_t retVal = SL_OK;
        demodBlock[instance].isInitialized = SL_DEMOD_FALSE;
		demodBlock[instance].isStarted = SL_DEMOD_FALSE;

        retVal = SL_DemodSetParams(instance);
        if (retVal != SL_OK)
        {
			SL_Printf("Demod Set plf Specific parameter Unsuccessful");
            return retVal;
        }
#if 0

        retVal = SL_DemodReset(instance);
        if (retVal != SL_OK)
        {
            SL_Printf("Demod Reset Unsuccessful");
            return retVal;
        }
#endif
        retVal = SL_DemodSetPrtcl(instance, ctrl);
        if (retVal != SL_OK)
        {
            SL_Printf("Demod Interface Configuration Failed");
            return retVal;
        }

#if 0 //replace download fw function.
        retVal = SL_DemodSetStandard(instance, std);
        if (retVal != SL_OK)
        {
            SL_Printf("Demod Broadcast Standard Configuration Failed");
            return retVal;
        }
         SL_Printf("Demod Code download started...\n");
        retVal = SL_DemodCodedownload(instance, demodBlock[instance].iccmFile, demodBlock[instance].dccmFile, demodBlock[instance].secondaryFile);
	        if (retVal != SL_OK)
        {
             SL_Printf("Demod Code Download Failed");
            return retVal;
        }
         SL_Printf("Demod Code Download Completed");	
#else
		 
		retVal = ITE_SL_DemodSetStandard(instance, std);
		if (retVal != SL_OK)
		{
			SL_Printf("Demod Broadcast Standard Configuration Failed");
			return retVal;
		}

		switch (std)
		{
		case SL_DEMODSTD_ATSC1_0:
			//SL_Printf("Demod Code download started...\n");
			//retVal = SL_DemodCodedownload(instance, demodBlock[instance].iccmFile, demodBlock[instance].dccmFile, demodBlock[instance].secondaryFile);
			retVal =  ITE_SL_DemodCodedownload(instance, (char *)&sl3000_ATSC1_iccm_bb_fw[0], (char *)&sl3000_ATSC1_dccm_bb_fw[0], (char *)&sl3000_ATSC1_bb_fw[0], (unsigned int*)sl3000_ATSC1_FW_Len);		 	
			break;		 
		case SL_DEMODSTD_ATSC3_0:
			retVal =  ITE_SL_DemodCodedownload(instance, (char *)&sl3000_ATSC3_iccm_bb_fw[0], (char *)&sl3000_ATSC3_dccm_bb_fw[0], (char *)&sl3000_ATSC3_bb_fw[0], (unsigned int*)sl3000_ATSC3_FW_Len);		 			 	
			break;
		default:
			SL_Printf("[SL_DemodInit]SL_ERR_INVALID_ARGUMENTS!\n");
			retVal = SL_ERR_INVALID_ARGUMENTS;
			break;
		}

		if (retVal != SL_OK)
		{
			SL_Printf("Demod Code Download Failed");
			return retVal;
		}
			SL_Printf("Demod Code Download Completed");
#endif


        retVal = SL_DemodTunerReset(instance);


        if (retVal == SL_OK)
        {
            demodBlock[instance].isInitialized = SL_DEMOD_TRUE;
        }

        return retVal;
    }
    return SL_ERR_INVALID_ARGUMENTS;
}

SL_Result_t SL_DemodUnInit(int instance)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            demodBlock[instance].isInitialized = SL_DEMOD_FALSE;
            demodBlock[instance].isConfigured = SL_DEMOD_FALSE;
            demodBlock[instance].isStarted = SL_DEMOD_FALSE;
            retVal = SL_DemodReset(instance);
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

SL_Result_t SL_DemodConfigure(int instance, SL_ConfigType_t type, void *cfgParams)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            unsigned int setVal = 0x00000000;
            demodBlock[instance].isConfigured = SL_DEMOD_FALSE;

            switch (type)
            {
            case SL_CONFIGTYPE_AFEIF:
                setVal = (((SL_AfeIfConfigParams_t*)cfgParams)->iftype) | (((SL_AfeIfConfigParams_t*)cfgParams)->iswap << 8) | (((SL_AfeIfConfigParams_t*)cfgParams)->qswap << 9);
                retVal = SL_DemodWriteBytes(instance, SET_IF_SIGNAL_CHAR, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                if (retVal == SL_OK)
                {
                    setVal = ((SL_AfeIfConfigParams_t*)cfgParams)->iqswap;
                    retVal = SL_DemodWriteBytes(instance, SET_IQ_SWAP, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                }

                if (retVal == SL_OK)
                {
                    /* Set up the NCO value on the demodulator based on the Centre frequency of incoming IF Signal from the tuner.    *
                     * The following calculation needs to be done on the frequency value before setting the value to the demodulator. */
                    double Fif = ((SL_AfeIfConfigParams_t*)cfgParams)->ifreq;
                    double ncoVal = (double)(((2 * ((SL_AfeIfConfigParams_t*)cfgParams)->spectrum) - 1) * (Fif / demodBlock[instance].fAdc)); // NCO Computation   
                    ncoVal *= Q_31;                                                                      // Q.31 formate (ans * 2^31)
                    setVal = (int)ncoVal;
                    retVal = SL_DemodWriteBytes(instance, SET_TUNER_IF_FREQ, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                }

                if (retVal == SL_OK)
                {
                    /* Provide the Reference AGC setting for the demodulator Analog front end. */
                    float agcVal = (float)((SL_AfeIfConfigParams_t*)cfgParams)->agcRefValue;
                    agcVal = (agcVal / 1000) * Q_11;   // AGC Reference value conveterd to Volt and Q.11 formate (ans * 2^11)                                                   // Q.11 formate (ans * 2^11)
                    setVal = (int)agcVal;
                    retVal = SL_DemodWriteBytes(instance, SET_AGC_REF_VALUE, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                }
                break;

            case SL_CONFIGTYPE_IQ_OFFSET_CORRECTION:
                setVal = (int)(((SL_IQOffsetCorrectionParams_t*)cfgParams)->iCoeff1 * Q_14);
                setVal = setVal & 0xFFFF;
                retVal = SL_DemodWriteBytes(instance, SET_I_COEFFICIENT_1, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);

                if (retVal == SL_OK)
                {
                    setVal = (int)(((SL_IQOffsetCorrectionParams_t*)cfgParams)->qCoeff1 * Q_14);
                    setVal = setVal & 0xFFFF;
                    retVal = SL_DemodWriteBytes(instance, SET_Q_COEFFICIENT_1, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                }

                if (retVal == SL_OK)
                {
                    setVal = (int)(((SL_IQOffsetCorrectionParams_t*)cfgParams)->iCoeff2 * Q_14);
                    setVal = setVal & 0xFFFF;
                    retVal = SL_DemodWriteBytes(instance, SET_I_COEFFICIENT_2, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                }

                if (retVal == SL_OK)
                {
                    setVal = (int)(((SL_IQOffsetCorrectionParams_t*)cfgParams)->qCoeff2 * Q_14);
                    setVal = setVal & 0xFFFF;
                    retVal = SL_DemodWriteBytes(instance, SET_Q_COEFFICIENT_2, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                }
                break;

            case SL_CONFIGTYPE_OUTPUTIF:

                setVal = ((SL_OutIfConfigParams_t*)cfgParams)->TsoClockInvEnable;
                retVal = SL_DemodWriteBytes(instance, SET_TSO_CLK_INV, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);

                if (retVal == SL_OK)
                {
                    if (((SL_OutIfConfigParams_t*)cfgParams)->oif == SL_OUTPUTIF_SDIO)
                    {
                        setVal = 0x0F00;
                    }
                    else if (((SL_OutIfConfigParams_t*)cfgParams)->oif == SL_OUTPUTIF_SPI)
                    {
                        setVal = 0x1F00;
                    }
                    else if (((SL_OutIfConfigParams_t*)cfgParams)->oif == SL_OUTPUTIF_TSPARALLEL_LSB_FIRST)
                    {
                        setVal = 0x2F01;
                    }
                    else if (((SL_OutIfConfigParams_t*)cfgParams)->oif == SL_OUTPUTIF_TSPARALLEL_MSB_FIRST)
                    {
                        setVal = 0x2F21;
                    }
                    else if (((SL_OutIfConfigParams_t*)cfgParams)->oif == SL_OUTPUTIF_TSSERIAL_LSB_FIRST)
                    {
                        setVal = 0x2F00;
                        retVal = SL_DemodWriteBytes(instance, SET_OUT_CONFIG, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                    }
                    else if (((SL_OutIfConfigParams_t*)cfgParams)->oif == SL_OUTPUTIF_TSSERIAL_MSB_FIRST)
                    {
                        setVal = 0x2F20;
                    }
                    retVal = SL_DemodWriteBytes(instance, SET_OUT_CONFIG, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                }
                break;

            case SL_CONFIGTYPE_TUNED_RF_FREQ:
                setVal = *((unsigned int*)cfgParams) / MHZ; //  Convert RF Frequency from Hz to MHz 
                retVal = SL_DemodWriteBytes(instance, SET_TUNER_RF_FREQUENCY, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal); // Configure RF Frequency in MHz
                break;

            case SL_CONFIGTYPE_TUNER_CRYSTAL_FREQ:
                setVal = *((unsigned int*)cfgParams) / KHZ; //  Convert Crytsal Frequency from KHz to MHz 
                retVal = SL_DemodWriteBytes(instance, SET_TUNER_CRYSTAL_FREQUENCY, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal); // COnfigTuner Crystal Frequency in MHz
                break;

            case SL_CONFIGTYPE_EXT_LNA:
                setVal = *((unsigned int*)cfgParams);
                retVal = SL_DemodWriteBytes(instance, SET_LNA_MODE, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
                break;

            default:
                retVal = SL_ERR_INVALID_ARGUMENTS;
            }
	//SL_Printf("\n [SL_DemodConfigure]type=%d setVal=%d\n", type,setVal);

            if (retVal == SL_OK)
            {
                demodBlock[instance].isConfigured = SL_DEMOD_TRUE;
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

SL_Result_t SL_DemodConfigureEx(int instance, SL_DemodStd_t std, void *stdParams)
{
    SL_Result_t retVal = SL_OK;
    demodBlock[instance].isExConfigured = SL_DEMOD_FALSE;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            if (demodBlock[instance].std == std)
            {
                switch (std)
                {
                case SL_DEMODSTD_ATSC3_0:
                    retVal = SL_DemodSetAtsc3Config(instance, (SL_Atsc3p0ConfigParams_t*)stdParams);
                    break;

                case SL_DEMODSTD_ATSC1_0:
                    retVal = SL_DemodSetAtsc1Config(instance, (SL_Atsc1p0ConfigParams_t*)stdParams);
                    break;
					/*
                case SL_DEMODSTD_DVB_T:
                    retVal = SL_DemodSetDvbtConfig(instance, (SL_DvbtConfigParams_t*)stdParams);
                    break;

                case SL_DEMODSTD_DVB_T2:
                    retVal = SL_DemodSetDvbt2Config(instance, (SL_Dvbt2ConfigParams_t*)stdParams);
                    break;
					*/
                case SL_DEMODSTD_DTMB:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_DVB_S:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_DVB_S2:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_DVB_C:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case    SL_DEMODSTD_ISDB_C:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;
					/*
                case SL_DEMODSTD_ISDB_T:
                    retVal = SL_DemodSetIsdbtConfig(instance, (SL_IsdbtConfigParams_t*)stdParams);
                    break;
					*/
                case    SL_DEMODSTD_US_C:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_INT_CALIBRATION:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                default:
                    retVal = SL_ERR_INVALID_ARGUMENTS;
                }

                if (retVal == SL_OK)
                {
                    demodBlock[instance].isExConfigured = SL_DEMOD_TRUE;
                }
                else
                {
                    demodBlock[instance].isExConfigured = SL_DEMOD_FALSE;
                }
            }
            else
            {
                retVal = SL_ERR_INVALID_ARGUMENTS;
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

SL_Result_t SL_DemodSetPowerMode(int instance, SL_DemodStd_t std, SL_DemodPowerMode_t powerMode)
{
    SL_Result_t retVal = SL_OK;
    static SL_Atsc3p0ConfigParams_t  atsc3ConfigInfo;
    static SL_Atsc3p0LlsPlpInfo_t    llsPlpInfo = 0;
    static SL_Atsc3p0LlsPlpInfo_t    llsPlpMask = 0x1;
    static int                       plpInfoVal = 0, plpllscount = 0;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            if (std == SL_DEMODSTD_ATSC3_0)
            {
                retVal = SL_DemodGetDiagnostics(instance, SL_DEMOD_DIAG_TYPE_ATSC3P0_PLP_LLS_INFO, &llsPlpInfo);
                if (retVal == SL_OK)
                {
                    if (powerMode == SL_DEMOD_POWER_MODE_STANDBY)
                    {
                        atsc3ConfigInfo.plpConfig.plp0 = (char)0xFF;
                        atsc3ConfigInfo.plpConfig.plp1 = (char)0xFF;
                        atsc3ConfigInfo.plpConfig.plp2 = (char)0xFF;
                        atsc3ConfigInfo.plpConfig.plp3 = (char)0xFF;
                        retVal = SL_DemodWriteBytes(instance, SET_CONFIG_DEMOD_PLP_ID, SL_DEMOD_REG_SIZE, (SL_Atsc3p0PlpConfigParams_t*)&atsc3ConfigInfo.plpConfig);
                    }
                    else if (powerMode == SL_DEMOD_POWER_MODE_ON)
                    {
                        plpllscount = 0;
                        for (int plpIndx = 0; (plpIndx < 64) && (plpllscount < 4); plpIndx++)
                        {
							plpInfoVal = ((llsPlpInfo & (llsPlpMask << plpIndx)) == pow((double)2, plpIndx)) ? 0x01 : 0xFF;
                            if (plpInfoVal == 0x01)
                            {
                                plpllscount++;
                                if (plpllscount == 1)
                                {
                                    atsc3ConfigInfo.plpConfig.plp0 = (char)plpIndx;
                                }
                                else if (plpllscount == 2)
                                {
                                    atsc3ConfigInfo.plpConfig.plp1 = (char)plpIndx;
                                }
                                else if (plpllscount == 3)
                                {
                                    atsc3ConfigInfo.plpConfig.plp2 = (char)plpIndx;
                                }
                                else if (plpllscount == 4)
                                {
                                    atsc3ConfigInfo.plpConfig.plp3 = (char)plpIndx;
                                }
                                else
                                {
                                    plpllscount++;
                                }
                            }
                        }
                        retVal = SL_DemodWriteBytes(instance, SET_CONFIG_DEMOD_PLP_ID, SL_DEMOD_REG_SIZE, (SL_Atsc3p0PlpConfigParams_t*)&atsc3ConfigInfo.plpConfig);
                    }
                    else
                    {
                        retVal = SL_ERR_INVALID_ARGUMENTS;
                    }
                }
            }
            else
            {
                retVal = SL_ERR_NOT_SUPPORTED;
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

SL_Result_t SL_DemodGetConfiguration(int instance, SL_DemodConfigInfo_t *cfg)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isConfigured)
        {
            retVal = SL_DemodGetStandard(instance, (SL_DemodConfigInfo_t*)cfg);
            if (retVal == SL_OK)
            {
                retVal = SL_DemodGetLnaMode(instance, (SL_DemodConfigInfo_t*)cfg);
            }

            if (retVal == SL_OK)
            {
                retVal = SL_DemodGetAfeIfConfigInfo(instance, (SL_AfeIfConfigParams_t*)&cfg->afeIfInfo);
            }

            if (retVal == SL_OK)
            {
                retVal = SL_DemodGetIQOffsetCorrConfigInfo(instance, (SL_IQOffsetCorrectionParams_t *)&cfg->iqOffCorInfo);
            }

            if (retVal == SL_OK)
            {
                retVal = SL_DemodGetOutIfConfigInfo(instance, (SL_OutIfConfigParams_t*)&cfg->oifInfo);
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

SL_Result_t SL_DemodGetConfigurationEx(int instance, SL_DemodStd_t std, void *stdParams)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isExConfigured)
        {
            if (demodBlock[instance].std == std)
            {
                switch (std)
                {

                case SL_DEMODSTD_ATSC3_0:
                    retVal = SL_DemodGetAtsc3Config(instance, (SL_Atsc3p0ConfigParams_t*)stdParams);
                    break;

                case SL_DEMODSTD_ATSC1_0:
                    retVal = SL_DemodGetAtsc1Config(instance, (SL_Atsc1p0ConfigParams_t*)stdParams);
                    break;

                case SL_DEMODSTD_DVB_T:
					retVal = SL_ERR_NOT_SUPPORTED;
                    //retVal = SL_DemodGetDvbtConfig(instance, (SL_DvbtConfigParams_t*)stdParams);
                    break;
                case SL_DEMODSTD_DVB_T2:
                    //retVal = SL_DemodGetDvbt2Config(instance, (SL_Dvbt2ConfigParams_t*)stdParams);
					retVal = SL_ERR_NOT_SUPPORTED;
                    break;
                case SL_DEMODSTD_DTMB:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_DVB_S:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_DVB_S2:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_DVB_C:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case    SL_DEMODSTD_ISDB_C:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_ISDB_T:
                    //retVal = SL_DemodGetIsdbtConfig(instance, (SL_IsdbtConfigParams_t*)stdParams);
					retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_US_C:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                case SL_DEMODSTD_INT_CALIBRATION:
                    retVal = SL_ERR_NOT_SUPPORTED;
                    break;

                default:
                    retVal = SL_ERR_INVALID_ARGUMENTS;
                }
            }
            else
            {
                retVal = SL_ERR_INVALID_ARGUMENTS;
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

SL_Result_t SL_DemodGetStatus(int instance, SL_DemodStatusType_t type, void *status)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            switch (type)
            {
            case SL_DEMOD_STATUS_TYPE_BOOT:
                retVal = SL_DemodReadBytes(instance, GET_DEMOD_BOOT_STATUS, sizeof(SL_DemodBootStatus_t), (SL_DemodBootStatus_t*)status);
                break;

            case SL_DEMOD_STATUS_TYPE_LOCK:
                if (demodBlock[instance].isInitialized)
                {
                    retVal = SL_DemodReadBytes(instance, GET_DEMOD_LOCK_STATUS, sizeof(SL_DemodLockStatus_t), (SL_DemodLockStatus_t*)status);
                }
                else
                {
                    retVal = SL_ERR_OPERATION_FAILED; // Demod needs to be in running state
                }
                break;

            case SL_DEMOD_STATUS_TYPE_CPU:
                if (demodBlock[instance].isInitialized)
                {
                    retVal = SL_DemodReadBytes(instance, GET_CPU_STATUS, SL_DEMOD_REG_SIZE, (unsigned int *)status);
                }
                else
                {
                    retVal = SL_ERR_OPERATION_FAILED; // Demod needs to be in running state
                }
                break;
            default:
                retVal = SL_ERR_INVALID_ARGUMENTS;
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

SL_Result_t SL_DemodGetDiagnostics(int instance, SL_DemodDiagType_t type, void *diagStr)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isStarted)
        {
            switch (demodBlock[instance].std)
            {
            case SL_DEMODSTD_ATSC3_0:
				
                if (type == SL_DEMOD_DIAG_TYPE_ATSC3P0_PERF)
                {
                    retVal = SL_DemodReadBytes(instance, GET_ATSC3_DECODE_PERFORMANCE, sizeof(SL_Atsc3p0Perf_Diag_t), (SL_Atsc3p0Perf_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_ATSC3P0_BSR)
                {
                    retVal = SL_DemodReadBytes(instance, GET_ATSC3_BS, sizeof(SL_Atsc3p0Bsr_Diag_t), (SL_Atsc3p0Bsr_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_ATSC3P0_L1B)
                {
                    retVal = SL_DemodReadBytes(instance, GET_ATSC3_L1B, sizeof(SL_Atsc3p0L1B_Diag_t), (SL_Atsc3p0L1B_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_ATSC3P0_L1D)
                {
                    retVal = SL_DemodReadBytes(instance, GET_ATSC3_L1D, sizeof(SL_Atsc3p0L1D_Diag_t), (SL_Atsc3p0L1D_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_ATSC3P0_PLP_LLS_INFO)
                {
                    retVal = SL_DemodGetAtsc3LlsPlpList(instance, (SL_Atsc3p0LlsPlpInfo_t*)diagStr);
                }
                else
                {
                    retVal = SL_ERR_INVALID_ARGUMENTS;
                }
				
                break;
			
            case SL_DEMODSTD_ATSC1_0:
                if (type == SL_DEMOD_DIAG_TYPE_ATSC1P0_PERF)
                {
                    retVal = SL_DemodReadBytes(instance, GET_ATSC1_DECODE_PERFORMANCE, sizeof(SL_Atsc1p0Perf_Diag_t), (SL_Atsc1p0Perf_Diag_t*)diagStr);
                }
                else
                {
                    retVal = SL_ERR_INVALID_ARGUMENTS;
                }
                break;
/*
            case SL_DEMODSTD_DVB_T:
				
                if (type == SL_DEMOD_DIAG_TYPE_DVBT_PERF)
                {
                    retVal = SL_DemodReadBytes(instance, GET_DVBT_DECODE_PERFORMANCE, sizeof(SL_DvbtPerf_Diag_t), (SL_DvbtPerf_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_DVBT_L1)
                {
                    retVal = SL_DemodReadBytes(instance, GET_DVBT_L1, sizeof(SL_DvbtL1_Diag_t), (SL_DvbtL1_Diag_t*)diagStr);
                }
                else
                {
                    retVal = SL_ERR_INVALID_ARGUMENTS;
                }
                break;

            case SL_DEMODSTD_DVB_T2:
                if (type == SL_DEMOD_DIAG_TYPE_DVBT2_PERF)
                {
                    retVal = SL_DemodReadBytes(instance, GET_DVBT2_DECODE_PERFORMANCE, sizeof(SL_Dvbt2Perf_Diag_t), (SL_Dvbt2Perf_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_DVBT2_L1)
                {
                    retVal = SL_DemodReadBytes(instance, GET_DVBT2_L1, sizeof(SL_Dvbt2L1_Diag_t), (SL_Dvbt2L1_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_DVBT2_PLP_COMMON)
        {
                    retVal = SL_DemodReadBytes(instance, GET_DVBT2_PLP_COMMON, sizeof(SL_Dvbt2Plp_Diag_t), (SL_Dvbt2Plp_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_DVBT2_PLP_USER)
            {
                    retVal = SL_DemodReadBytes(instance, GET_DVBT2_PLP_USER, sizeof(SL_Dvbt2Plp_Diag_t), (SL_Dvbt2Plp_Diag_t*)diagStr);
                }
                else
                {
                    retVal = SL_ERR_INVALID_ARGUMENTS;
                }
                    break;
*/
            case SL_DEMODSTD_DTMB:
                retVal = SL_ERR_NOT_SUPPORTED;
                    break;

            case SL_DEMODSTD_DVB_S:
                retVal = SL_ERR_NOT_SUPPORTED;
                    break;

            case SL_DEMODSTD_DVB_S2:
                retVal = SL_ERR_NOT_SUPPORTED;
                    break;

            case SL_DEMODSTD_DVB_C:
                retVal = SL_ERR_NOT_SUPPORTED;
                break;

            case    SL_DEMODSTD_ISDB_C:
                retVal = SL_ERR_NOT_SUPPORTED;
                break;
				/*
            case SL_DEMODSTD_ISDB_T:
                if (type == SL_DEMOD_DIAG_TYPE_ISDBT_PERF)
                {
                    retVal = SL_DemodReadBytes(instance, GET_ISDBT_DECODE_PERFORMANCE, sizeof(SL_IsdbtPerf_Diag_t), (SL_IsdbtPerf_Diag_t*)diagStr);
                }
                else if (type == SL_DEMOD_DIAG_TYPE_ISDBT_L1)
                {
                    retVal = SL_DemodReadBytes(instance, GET_ISDBT_L1, sizeof(SL_IsdbtL1_Diag_t), (SL_IsdbtL1_Diag_t*)diagStr);
            }
            else
            {
                    retVal = SL_ERR_INVALID_ARGUMENTS;
                }
                break;
				*/
            case    SL_DEMODSTD_US_C:
                retVal = SL_ERR_NOT_SUPPORTED;
                break;

            case SL_DEMODSTD_INT_CALIBRATION:
                retVal = SL_ERR_NOT_SUPPORTED;
                break;

            default:
                retVal = SL_ERR_INVALID_ARGUMENTS;
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

SL_Result_t SL_DemodStart(int instance)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isConfigured)
        {
			if (demodBlock[instance].isStarted == SL_DEMOD_FALSE)
			{
				unsigned int setVal = 0x00007845;
				retVal = SL_DemodWriteBytes(instance, SET_DEMOD_START, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
				if (retVal == SL_OK)
                {
					demodBlock[instance].isStarted = SL_DEMOD_TRUE;
			}
            }
			else
			{
				retVal = SL_ERR_ALREADY_STARTED;
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

SL_Result_t SL_DemodStop(int instance)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if ((demodBlock[instance].isInitialized) && (demodBlock[instance].isStarted) && (demodBlock[instance].isConfigured))
        {
            unsigned int setVal = 0x000000FF;

            retVal = SL_DemodWriteBytes(instance, SET_DEMOD_STOP, SL_DEMOD_REG_SIZE, (unsigned int*)&setVal);
            if (retVal == SL_OK)
            {
                demodBlock[instance].isStarted = SL_DEMOD_FALSE;
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

SL_Result_t SL_DemodGetSoftwareVersion(int instance, int *MajorNo, int *MinorNo)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isInitialized)
        {
            retVal = SL_DemodReadBytes(instance, GET_SW_MAJOR_NUM, SL_DEMOD_REG_SIZE, (int *)MajorNo);
            if (retVal == SL_OK)
            {
                retVal = SL_DemodReadBytes(instance, GET_SW_MINOR_NUM, SL_DEMOD_REG_SIZE, (int *)MinorNo);
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

SL_Result_t SL_DemodGetBaseBandParams(int instance, unsigned int *agcGain, unsigned int *bbRms)
{
    SL_Result_t retVal = SL_OK;

    if (instance < SL_DEMOD_MAX_INSTANCES && instance >= 0)
    {
        if (demodBlock[instance].isStarted)
            {
            retVal = SL_DemodReadBytes(instance, SET_AGC_REF_VALUE, SL_DEMOD_REG_SIZE, (unsigned int *)agcGain);
            if (retVal == SL_OK)
            {
                *agcGain >>= 16; // Higher 16 bit contains AGC Output Gain
                retVal = SL_DemodReadBytes(instance, GET_BB_RMS, SL_DEMOD_REG_SIZE, (unsigned int *)bbRms);
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

SL_Result_t SL_WriteBytes(int instance, unsigned int address, unsigned int lenBytes, void *buf)
{
	//SL_Result_t retVal = SL_OK;
	if (demodBlock[instance].isCreated)
	{
		if (lenBytes >= 4)
			return SL_DemodWriteBytes(instance, address, lenBytes, (unsigned int*)buf);
		else
			return SL_ERR_INVALID_ARGUMENTS;
	}
	else
		return SL_ERR_OPERATION_FAILED;


}
SL_Result_t SL_ReadBytes(int instance, unsigned int address, unsigned int lenBytes, void *buf)
{
	if (demodBlock[instance].isCreated)
	{
		if (lenBytes >= 4)
			return SL_DemodReadBytes(instance, address, lenBytes, (unsigned int*)buf);
		else
			return SL_ERR_INVALID_ARGUMENTS;
	}
	else
		return SL_ERR_OPERATION_FAILED;

}


