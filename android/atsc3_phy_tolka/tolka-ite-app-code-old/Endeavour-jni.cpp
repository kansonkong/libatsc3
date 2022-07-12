#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <jni.h>
#include "Endeavour-jni.h"
#include "tolka/IT9300.h"
#include "tolka/brError.h"

/* import Demod and tuner  */
#include "Receiver/SL3000_R855/sl_i2c.h"
#include "Receiver/SL3000_R855/R855.h"
#include "Receiver/SL3000_R855/I2C_Sys.h"
#include "Receiver/SL3000_R855/sl_i2c.h"
#include "Receiver/SL3000_R855/atsc1.h"
#include "Receiver/SL3000_R855/atsc3.h"

/* USER channel configure */
#define IT9303_TYPE      1       // 1: IT9303, 0: IT9305

static JavaVM* m_pVm = NULL;
static const char* className = "com/api/Endeavour";
static int jniVersion = JNI_VERSION_1_6;

// SL3000 R855
typedef struct
{
    SL_TunerConfig_t            tunerCfg;
    SL_PlatFormConfigParams_t   PlfConfig;
    R855_Set_Info               R855_Info;
} SL3000_R855_instance_t;
SL3000_R855_instance_t      SL3000_R855_driver[4];

typedef enum MODE{
    ATSC3,
    ATSC1,
    DVB_T_T2,
    ISDBT,
    J83B,
    DVBC
};

// IT9300
Endeavour endeavour;

jlong
Java_com_init (JNIEnv* env, jobject thiz, int tvStandard)
{
    long error = 0;

    uint8_t                             id;
    uint8_t                             i2c_bus, demod_i2cAdd, tuner_i2cAdd;
    uint8_t                             i2c_r855_w_addr;
    int                                 num_of_tuner;
    int                                 i;
    long data_cnt;
    uint8_t tmp0[1];
    uint8_t tmp1[1];

    // Init SL3000 + R855
    R855_ErrCode R855_result = R855_Success;
    SL_Result_t sl3000_result = SL_OK;

    BrUser_createCriticalSection();

    //jjustman-2022-05-19 - memset our endeavour struct to 0 just to be safe

    memset(&endeavour, 0, sizeof(Endeavour));

    //Init Endeavour
    endeavour.ctrlBus = BUS_USB;
    endeavour.maxBusTxSize = 63;
    endeavour.bypassScript = True;
    endeavour.bypassBoot = False;
    endeavour.chipCount = 1;
    endeavour.gator[0].existed = True;
    endeavour.gator[0].outDataType = OUT_DATA_USB_DATAGRAM;
    endeavour.gator[0].outTsPktLen = PKT_LEN_188;

    //jjustman-2022-05-19: reg_ts0_tag_len - 0 -> 188
    endeavour.tsSource[0][0].tsPktLen = PKT_LEN_188;
    endeavour.tsSource[0][0].syncByte = 0x47;


    endeavour.gator[0].booted = False;
    endeavour.gator[0].initialized = False;
    #if IT9303_TYPE
    endeavour.tsSource[0][0].existed = True;
    endeavour.tsSource[0][0].tsType  = TS_SERIAL;
    endeavour.tsSource[0][0].tsPort  = TS_PORT_0;
    endeavour.tsSource[0][1].existed = False;
    endeavour.tsSource[0][1].tsType  = TS_SERIAL;
    endeavour.tsSource[0][1].tsPort  = TS_PORT_4;
    #else
    endeavour.tsSource[0][0].existed = True;
    endeavour.tsSource[0][0].tsType  = TS_SERIAL;
    endeavour.tsSource[0][0].tsPort  = TS_PORT_1;
    endeavour.tsSource[0][1].existed = True;//False;
    endeavour.tsSource[0][1].tsType  = TS_SERIAL;
    endeavour.tsSource[0][1].tsPort  = TS_PORT_2;
    endeavour.tsSource[0][2].existed = True;//False;
    endeavour.tsSource[0][2].tsType  = TS_SERIAL;
    endeavour.tsSource[0][2].tsPort  = TS_PORT_3;
    endeavour.tsSource[0][3].existed = True;//False;
    endeavour.tsSource[0][3].tsType  = TS_SERIAL;
    endeavour.tsSource[0][3].tsPort  = TS_PORT_4;
    #endif

    num_of_tuner = 0;
    for (i = 0; i < 4; i++)
    {
        if (endeavour.tsSource[0][i].existed == True)
            num_of_tuner++;
    }

    IT9300_readRegister(&endeavour, 0, 0xDA98 , tmp0);
    IT9300_readRegister(&endeavour, 0, 0xDA99 , tmp1);
    data_cnt = 4*((tmp1[0] * 256) + tmp0[0]);

    error = IT9300_getFirmwareVersion (&endeavour, 0);
    if (error) {
        printf("IT9300_getFirmwareVersion fail! \n");
        goto exit;
    }

    if (endeavour.firmwareVersion != 0) {
        //Switch to TS mode for cleaning PSB buffer
        printf("\n--- Clean PSB buffer --- \n");
        error = IT9300_writeRegister(&endeavour, 0, p_br_mp2if_mpeg_ser_mode, 1);
        if (error)
            goto exit;

        // Reset Rx and Read USB for no data issue
            printf("--- RESET IT9300 --- \n");
            error = IT9300_writeRegister(&endeavour, 0, p_br_reg_top_gpioh2_en, 0x01);
            if (error)
                goto exit;
            error = IT9300_writeRegister(&endeavour, 0, p_br_reg_top_gpioh2_on, 0x01);
            if (error)
                goto exit;
            error = IT9300_writeRegister(&endeavour, 0, p_br_reg_top_gpioh2_o, 0x0);
            if (error)
                goto exit;

        usleep(50 * 1000);
    }

    IT9300_readRegister(&endeavour, 0, 0xDA98 , tmp0);
    IT9300_readRegister(&endeavour, 0, 0xDA99 , tmp1);
    data_cnt = 4*((tmp1[0] * 256) + tmp0[0]);

    error = IT9300_initialize(&endeavour, 0);
    if(error)
    {
        printf("IT9300 initialize failed, error = 0x%lx\n", error);
        goto exit;
    }

    for (i = 0; i < num_of_tuner; i++)¡
    {
        if (endeavour.tsSource[0][i].existed == True)
        {
            error = IT9300_enableTsPort(&endeavour, 0, i);
            if (error) goto exit;
        }

        //jjustman-2022-05-19
        //set this register by hand, as the impl assigns the value of 1 - error = IT9300_setSyncByteMode(&endeavour, 0, 0);
        //instead of using a struct member for the proper value extraction in the default impl

        error = IT9300_writeRegister (&endeavour, 0, p_br_reg_ts0_aggre_mode + i, 0);//0:tag 1:sync  2:remap
        if(error)
        {
            printf("IT9300 set ignore fail pin failed, error = 0x%lx\n", error);
            goto exit;
        }

        //ts0_tei_modify?
        error = IT9300_writeRegister (&endeavour, 0, p_br_mp2if_ts0_tei_modify + i, 0);//0:tag 1:sync  2:remap
        if(error)
        {
            printf("IT9300 set ignore fail pin failed, error = 0x%lx\n", error);
            goto exit;
        }



        //                (endeavour->tsSource[chip][tsSrcIdx]).tsPktLen);
        error = IT9300_setInTsPktLen(&endeavour, 0, i);
        if(error)
        {
            printf("IT9300 set IT9300_setInTsPktLen failed, error = 0x%lx\n", error);
            goto exit;
        }

//how to handle ts 0x47 sync byte wrangling...?
        //IT9300_setSyncByteMode
//        error = IT9300_setSyncByteMode(&endeavour, 0, i);
//        if(error)
//        {
//            printf("IT9300 set IT9300_setSyncByteMode failed, error = 0x%lx\n", error);
//            goto exit;
//        }

#if 0
        endeavour.tsSource[0][0].syncByte = 0x40;
            endeavour.tsSource[0][1].syncByte = 0x41;
            endeavour.tsSource[0][2].syncByte = 0x42;
            endeavour.tsSource[0][3].syncByte = 0x43;
            error = IT9300_setSyncByteMode(&endeavour, 0, 0);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&endeavour, 0, 1);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&endeavour, 0, 2);
            if (error) goto exit;
            error = IT9300_setSyncByteMode(&endeavour, 0, 3);
            if (error) goto exit;
            error = IT9300_enableTsPort(&endeavour, 0, 0);
            if (error) goto exit;
            error = IT9300_enableTsPort(&endeavour, 0, 1);
            if (error) goto exit;
            error = IT9300_enableTsPort(&endeavour, 0, 2);
            if (error) goto exit;
            error = IT9300_enableTsPort(&endeavour, 0, 3);
            if (error) goto exit;
#endif

        //Ignore TS fail pin
        error = IT9300_writeRegister(&endeavour, 0, p_br_reg_ts_fail_ignore, 0x1F); //necessary
        if(error)
        {
            printf("IT9300 set ignore fail pin failed, error = 0x%lx\n", error);
            goto exit;
        }

        printf("IT9300 initialize ok\n");
    }

    // Init R855
    printf("R855 initialize. \n");
    SL3000_R855_driver[0].R855_Info.R855_Standard = R855_ATSC_IF_5M;
    R855_result = Init_R855(&endeavour, &SL3000_R855_driver[0].R855_Info);
    if(R855_result != R855_Success) {
        printf("R855 Init error = %d\n", R855_result);
    } else {
        printf("R855 Init succeeful = %d\n", R855_result);
    }
    error = R855_result;

    // Init SL3000
    printf("SL3000 initialize. \n");
    if(tvStandard == ATSC3) {
        printf("Init TVStandard = %d\n", tvStandard);
        SL3000_R855_driver[0].tunerCfg.std = SL_TUNERSTD_ATSC3_0;
        sl3000_result = SL3000_atsc3_init(&endeavour, &SL3000_R855_driver[0].tunerCfg, &SL3000_R855_driver[0].PlfConfig, SL_DEMODSTD_ATSC3_0);
    } else if(tvStandard == ATSC1) {
        printf("Init TVStandard = %d\n", tvStandard);
        SL3000_R855_driver[0].tunerCfg.std = SL_TUNERSTD_ATSC1_0;
        sl3000_result = SL3000_atsc1_init(&endeavour, &SL3000_R855_driver[0].tunerCfg, &SL3000_R855_driver[0].PlfConfig, SL_DEMODSTD_ATSC1_0);
    } else {
        printf("SL3000 Unknown tvStandard = %d\n", tvStandard);
        error = SL_ERR_INVALID_ARGUMENTS;
        goto exit;
    }

    if(sl3000_result != SL_OK)
        printf("SL3000 Init error = %d\n", sl3000_result);
    else
        printf("SL3000 Init succeeful = %d\n", sl3000_result);
    error = sl3000_result;

    exit:
    if(error)
        printf("IT9300 error = 0x%lx\n", error);

    return error;
}

// SL3000_R855
jlong
Java_com_TunerDemod_Atsc3Tune(JNIEnv* env, jobject thiz,
        jint        id,
        jint        frequencyKHz,
        jint        bandwidthKHz)
{
    JNIEnv* jniEnv = NULL;
    R855_ErrCode R855_result = R855_Success;
    SL_Result_t sl3000_result = SL_OK;
    SignalInfo_t sigInfo;
    jlong result = 0;
    int monitor_timer = 7; // monitor locked signal 7 time

    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    printf("%s %d, id = %d, frequency = %d, bandwidthKHz = %d\n", __func__, __LINE__, id, frequencyKHz, bandwidthKHz);

    if (frequencyKHz != 0) {
        switch (bandwidthKHz) {
            case 6000:
                SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
                break;
            case 7000:
                SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_7MHZ;
                break;
            case 8000:
                SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_8MHZ;
                break;
            default:
                SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
                break;
        }
        SL3000_R855_driver[0].R855_Info.RF_KHz = (UINT32) frequencyKHz; // unit: kHz
        SL_Result_t slres = SL_OK;

        //IF AGC select
        SL3000_R855_driver[0].R855_Info.R855_IfAgc_Select = R855_IF_AGC1;
        R855_result = Tune_R855(&SL3000_R855_driver[0].R855_Info);
        if(R855_result != R855_Success) {
            printf("R855_tune() error = %d\n", R855_result);
            return -1;      // R855_Fail
        } else {
            printf("R855_tune() OK = %d\n", R855_result);
        }

        sl3000_result = SL3000_atsc3_tune(&SL3000_R855_driver[0].tunerCfg,
                                  &SL3000_R855_driver[0].PlfConfig);
        if(sl3000_result != SL_OK) {
            printf("SL3000_atsc3_tune() error = %d\n", sl3000_result);
            return sl3000_result;
        } else {
            printf("SL3000_atsc3_tune() OK = %d\n", sl3000_result);
        }

        while(monitor_timer) {
            Monitor_SL3000_ATSC3_Signal(&sigInfo, SL3000_R855_driver[0].R855_Info.RF_KHz, SL3000_R855_driver[0].R855_Info.R855_Standard);
            if (sigInfo.locked == LOCKED) {
                printf("Monitor_SL3000_ATSC3_Signal() Locked, RSSI: %d db\n", sigInfo.rssi);
                result = 0;
                return result;
            } else {
                printf("Monitor_SL3000_ATSC3_Signal() Unlock, RSSI: %d db\n", sigInfo.rssi);
                result = -1;
            }
            monitor_timer--;
            usleep(2000000);
        }
    }

    return result;
}

// SL3000_R855
jlong
Java_com_TunerDemod_Atsc3SetPLP(JNIEnv* env, jobject thiz,
                           jint        id,
                           jchar       plpMask,
                           jcharArray  plpID)
{
    JNIEnv* jniEnv = NULL;
    SL_Result_t result;
    char* temp = new char[4];

    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }
    jchar   *charplpID = jniEnv->GetCharArrayElements(plpID, 0);

    for(int i = 0; i < 4 ; i++)
        temp[i] = charplpID[i];

    result = SL3000_atsc3_setPLP(plpMask, (char*) temp);

    jniEnv->ReleaseCharArrayElements(plpID, charplpID, 0);
    return result;
}

jlong
Java_com_TunerDemod_Atsc3GetStatus(JNIEnv* env, jobject thiz,
                                   jint            id,
                                   jintArray       confidence,
                                   jbooleanArray   tsLockState,
                                   jbooleanArray   unlockDetected,
                                   jdoubleArray    snr,
                                   jintArray       plpValid,
                                   jintArray       rssi)
{
    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

 //   printf("%s %d\n", __func__, __LINE__);

    jint        *intConfidence              = jniEnv->GetIntArrayElements(confidence, 0);
    jboolean    *booleanTsLockState         = jniEnv->GetBooleanArrayElements(tsLockState, 0);
    jboolean    *booleanUnlockDetected      = jniEnv->GetBooleanArrayElements(unlockDetected, 0);
    jdouble     *doubleSnr                  = jniEnv->GetDoubleArrayElements(snr, 0);
    jint        *intplpTableValid           = jniEnv->GetIntArrayElements(plpValid, 0);
    jint        *intRssi                    = jniEnv->GetIntArrayElements(rssi, 0);

    long   result = 0;
    SignalInfo_t sigInfo;

    //Lock
    Monitor_SL3000_ATSC3_Signal(&sigInfo, SL3000_R855_driver[0].R855_Info.RF_KHz, SL3000_R855_driver[0].R855_Info.R855_Standard);

    if(sigInfo.locked == LOCKED) {
        *booleanTsLockState = 1;
        *booleanUnlockDetected = 0;
    } else {
        *booleanTsLockState = 0;
        *booleanUnlockDetected = 1;
    }
    *intRssi = sigInfo.rssi;
    *doubleSnr = sigInfo.snr;
    intplpTableValid[0] =  sigInfo.plpVaild[0];
    intplpTableValid[1] =  sigInfo.plpVaild[1];
    intplpTableValid[2] =  sigInfo.plpVaild[2];
    intplpTableValid[3] =  sigInfo.plpVaild[3];
    printf("SNR             : %3.2f dB\n\n", sigInfo.snr);
 /*   printf("Tuner RSSI		: %d dBm\n", sigInfo.rssi);
    printf("sigInfo.plpVaild[0]=%d\n",sigInfo.plpVaild[0]);
    printf("sigInfo.plpVaild[1]=%d\n",sigInfo.plpVaild[1]);
    printf("sigInfo.plpVaild[2]=%d\n",sigInfo.plpVaild[2]);
    printf("sigInfo.plpVaild[3]=%d\n",sigInfo.plpVaild[3]);
*/
    jniEnv->ReleaseIntArrayElements(confidence, intConfidence, 0);
    jniEnv->ReleaseBooleanArrayElements(tsLockState, booleanTsLockState, 0);
    jniEnv->ReleaseBooleanArrayElements(unlockDetected, booleanUnlockDetected, 0);
    jniEnv->ReleaseDoubleArrayElements(snr, doubleSnr, 0);
    jniEnv->ReleaseIntArrayElements(plpValid, intplpTableValid, 0);
    jniEnv->ReleaseIntArrayElements(rssi, intRssi, 0);

    return result;
}

jlong
Java_com_TunerDemod_Atsc1Tune(JNIEnv* env, jobject thiz,
        jint        id,
        jint        frequencyKHz)
{
    JNIEnv* jniEnv = NULL;
    R855_ErrCode R855_result = R855_Success;
    SL_Result_t sl3000_result = SL_OK;
    jlong result = 0;
    SignalInfo_t sigInfo;
    int monitor_timer = 7; // monitor locked signal 7 time

    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    printf("%s %d, id = %d, frequency = %d\n", __func__, __LINE__, id, frequencyKHz);
    if (frequencyKHz != 0)
    {
        SL3000_R855_driver[0].tunerCfg.bandwidth = SL_TUNER_BW_6MHZ;
        SL3000_R855_driver[0].R855_Info.RF_KHz = (UINT32)frequencyKHz; // unit: kHz
        //IF AGC select
        SL3000_R855_driver[0].R855_Info.R855_IfAgc_Select = R855_IF_AGC1;
        R855_result = Tune_R855(&SL3000_R855_driver[0].R855_Info);
        if(R855_result != R855_Success) {
            printf("R855_tune() error = %d\n", R855_result);
            return -1;      // R855_Fail
        } else {
            printf("R855_tune() OK = %d\n", R855_result);
        }

        sl3000_result = SL3000_atsc1_tune(&endeavour, &SL3000_R855_driver[0].tunerCfg, &SL3000_R855_driver[0].PlfConfig);
        if(sl3000_result != SL_OK) {
            printf("SL3000_atsc1_tune() error = %d\n", sl3000_result);
            return sl3000_result;
        } else {
            printf("SL3000_atsc1_tune() OK = %d\n", sl3000_result);
        }

        while(monitor_timer) {
            Monitor_SL3000_ATSC1_Signal(&sigInfo, SL3000_R855_driver[0].R855_Info.RF_KHz, SL3000_R855_driver[0].R855_Info.R855_Standard);
            if (sigInfo.locked == LOCKED) {
                printf("Monitor_SL3000_ATSC1_Signal() Locked, RSSI: %d db\n", sigInfo.rssi);
                result = 0;
                return result;
            } else {
                printf("Monitor_SL3000_ATSC1_Signal() Unlock, RSSI: %d db\n", sigInfo.rssi);
                result = -1;
            }
            monitor_timer--;
            usleep(2000000);
        }
    }

    return result;
}

jlong
Java_com_TunerDemod_Atsc1GetStatus(JNIEnv* env, jobject thiz,
                                   jint            id,
                                   jintArray       confidence,
                                   jbooleanArray   tsLockState,
                                   jbooleanArray   unlockDetected,
                                   jintArray       snr,
                                   jdoubleArray      ber,
                                   jdoubleArray       per,
                                   jintArray       rssi)
{
    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    printf("%s %d\n", __func__, __LINE__);

    jint    *intConfidence                  = jniEnv->GetIntArrayElements(confidence, 0);
    jboolean    *booleanTsLockState         = jniEnv->GetBooleanArrayElements(tsLockState, 0);
    jboolean    *booleanUnlockDetected      = jniEnv->GetBooleanArrayElements(unlockDetected, 0);
    jint        *intSnr                     = jniEnv->GetIntArrayElements(snr, 0);
    jdouble     *doubleBer                  = jniEnv->GetDoubleArrayElements(ber, 0);
    jdouble     *doublePer                  = jniEnv->GetDoubleArrayElements(per, 0);
    jint        *intRssi                    = jniEnv->GetIntArrayElements(rssi, 0);

    long   result = 0;
    SignalInfo_t sigInfo;

    //Lock
    Monitor_SL3000_ATSC1_Signal(&sigInfo, SL3000_R855_driver[0].R855_Info.RF_KHz, SL3000_R855_driver[0].R855_Info.R855_Standard);

    if(sigInfo.locked == LOCKED) {
        *booleanTsLockState = 1;
        *booleanUnlockDetected = 0;
    } else {
        *booleanTsLockState = 0;
        *booleanUnlockDetected = 1;
    }
    *intRssi = sigInfo.rssi;
    *intSnr = sigInfo.snr;
    *doubleBer = sigInfo.ber;
    *doublePer = sigInfo.per;
    *intConfidence = sigInfo.confidence;
    printf("Tuner RSSI		: %d dBm\n", sigInfo.rssi);

    jniEnv->ReleaseIntArrayElements(confidence, intConfidence, 0);
    jniEnv->ReleaseBooleanArrayElements(tsLockState, booleanTsLockState, 0);
    jniEnv->ReleaseBooleanArrayElements(unlockDetected, booleanUnlockDetected, 0);
    jniEnv->ReleaseIntArrayElements(snr, intSnr, 0);
    jniEnv->ReleaseDoubleArrayElements(ber, doubleBer, 0);
    jniEnv->ReleaseDoubleArrayElements(per, doublePer, 0);
    jniEnv->ReleaseIntArrayElements(rssi, intRssi, 0);

    return result;
}

jlong
Java_com_Demod_Atsc1WriteRegisters(JNIEnv* env, jobject thiz,
        jint        id,
        jint        registerAddress,
        jint        bufferLength,
        jbyteArray   buffer)
{
    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }
    printf("%s %d\n", __func__, __LINE__);

    jbyte   *byteBuffer = jniEnv->GetByteArrayElements(buffer, 0);
    Byte*   temp = new Byte[bufferLength];

    for(int i = 0; i < bufferLength; i++)
    {
        temp[i] = byteBuffer[i];
    }
    long error = SL_ATSC1_WriteBytes((unsigned int) registerAddress, (unsigned int) bufferLength, temp);

    jniEnv->ReleaseByteArrayElements(buffer, byteBuffer, 0);
    delete[] temp;

    return error;
}

jlong
Java_com_Demod_Atsc1ReadRegisters(JNIEnv* env, jobject thiz,
        jint        id,
        jint        registerAddress,
        jint        bufferLength,
        jbyteArray   buffer)
{
    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }
    printf("%s %d\n", __func__, __LINE__);

    jbyte    *byteBuffer          = jniEnv->GetByteArrayElements(buffer, 0);
    Byte* temp = new Byte[bufferLength];

    long error = SL_ATSC1_ReadBytes((unsigned int) registerAddress, (unsigned int) bufferLength, (void*) temp);

    for(int i = 0; i < bufferLength; i++)
    {
        byteBuffer[i] = temp[i];
    }

    jniEnv->ReleaseByteArrayElements(buffer, byteBuffer, 0);
    delete[] temp;

    return error;
}

// IT9300 (Endeavour)
jlong
Java_com_IT9300_writeRegisters (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        registerAddress,
        jint        bufferLength,
        jintArray   buffer)
{
    //printf("%s", __func__);
    Byte* temp = new Byte[bufferLength];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    //printf("%s", __func__);
    for(int i = 0; i < bufferLength; i++)
    {
        temp[i] = intArray[i];
        //printf("\ttemp[%d] = 0x%x", i, temp[i]);
    }

    long error = IT9300_writeRegisters(&endeavour, (Byte)chip, (Dword)registerAddress, (Byte)bufferLength , temp);

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_writeRegister (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        registerAddress,
        jint        value)
{
    //printf("%s", __func__);
    return IT9300_writeRegister(&endeavour, (Byte)chip, (Dword)registerAddress, (Byte)value);
}

jlong
Java_com_IT9300_writeEepromValues (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        registerAddress,
        jint        bufferLength,
        jintArray   buffer)
{
    printf("%s", __func__);
    Byte* temp = new Byte[bufferLength];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    //printf("%s", __func__);
    for(int i = 0; i < bufferLength; i++)
    {
        temp[i] = intArray[i];
        //printf("\ttemp[%d] = 0x%x", i, temp[i]);
    }

    long error = IT9300_writeEepromValues(&endeavour, (Byte)chip, (Dword)registerAddress, (Byte)bufferLength , temp);

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_writeRegisterBits (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        registerAddress,
        jint        position,
        jint        length,
        jint        value)
{
    printf("%s", __func__);
    return IT9300_writeRegisterBits(&endeavour, (Byte)chip, (Dword)registerAddress, (Byte)position, (Byte)length, (Byte)value);
}

jlong
Java_com_IT9300_readRegisters (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        registerAddress,
        jint        bufferLength,
        jintArray   buffer)
{
    //printf("%s", __func__);
    Byte* temp = new Byte[bufferLength];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    long error = IT9300_readRegisters(&endeavour, (Byte)chip, (Dword)registerAddress, (Byte)bufferLength, temp);
    //printf("%s", __func__);

    //printf("bufferLength = %d", bufferLength);
    for(int i = 0; i < bufferLength; i++)
    {
        intArray[i] = temp[i];
        //printf("\tintArray[%d] = 0x%x", i, intArray[i]);
    }

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_readRegister (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        registerAddress,
        jintArray   buffer)
{
    //printf("%s", __func__);
    Byte* temp = new Byte[1];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    long error = IT9300_readRegister(&endeavour, (Byte)chip, (Dword)registerAddress, temp);
    //printf("%s", __func__);

    intArray[0] = temp[0];
    //printf("\tintArray[%d] = 0x%x", i, intArray[i]);

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_readEepromValues (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        registerAddress,
        jint        bufferLength,
        jintArray   buffer)
{
    printf("%s", __func__);
    Byte* temp = new Byte[bufferLength];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    long error = IT9300_readEepromValues(&endeavour, (Byte)chip, (Dword)registerAddress, (Byte)bufferLength, temp);
    //printf("%s", __func__);

    //printf("bufferLength = %d", bufferLength);
    for(int i = 0; i < bufferLength; i++)
    {
        intArray[i] = temp[i];
        //printf("\tintArray[%d] = 0x%x", i, intArray[i]);
    }

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_readRegisterBits (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        registerAddress,
        jint        position,
        jint        length,
        jintArray   value)
{
    printf("%s", __func__);
    Byte* temp = new Byte[1];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(value, 0);
    long error = IT9300_readRegisterBits(&endeavour, (Byte)chip, (Dword)registerAddress, (Byte)position, (Byte)length, temp);
    intArray[0] = temp[0];

    jniEnv->ReleaseIntArrayElements(value, intArray, 0);
    delete[] temp;
    return error;
}

// IT9300_getFirmwareVersion
// IT9300_configOutput
// IT9300_initialize
// IT9300_setOutTsPktLen
// IT9300_setOutTsType
// IT9300_load601Timing

jlong
Java_com_IT9300_setInTsPktLen (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    printf("%s", __func__);
    return IT9300_setInTsPktLen(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}

jlong
Java_com_IT9300_setSyncByteMode (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    printf("%s", __func__);
    return IT9300_setSyncByteMode(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}

jlong
Java_com_IT9300_setTagMode (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    printf("%s", __func__);
    return IT9300_setTagMode(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}

jlong
Java_com_IT9300_setPidRemapMode (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    printf("%s", __func__);
    return IT9300_setPidRemapMode(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}

jlong
Java_com_IT9300_enableTsPort (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    printf("%s", __func__);
    return IT9300_enableTsPort(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}

jlong
Java_com_IT9300_disableTsPort (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    printf("%s", __func__);
    return IT9300_disableTsPort(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}

jlong
Java_com_IT9300_setInTsType (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    printf("%s", __func__);
    return IT9300_setInTsType(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}

jlong
Java_com_IT9300_enPidFilterAT (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx,
        jint        tableindex)
{
    printf("%s", __func__);
    return IT9300_enPidFilterAT(&endeavour, (Byte)chip, (Byte)tsSrcIdx, (Byte)tableindex);
}

jlong
Java_com_IT9300_disPidFilterAT (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx,
        jint        tableindex)
{
    printf("%s", __func__);
    return IT9300_disPidFilterAT(&endeavour, (Byte)chip, (Byte)tsSrcIdx, (Byte)tableindex);
}

jlong
Java_com_IT9300_enPidFilter (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    return IT9300_enPidFilter(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}

jlong
Java_com_IT9300_setExternalclock (JNIEnv* env, jobject thiz,
        jint        chip,
        jboolean    bvalue)
{
    printf("%s", __func__);
    return IT9300_setExternalclock(&endeavour, (Byte)chip, (Bool)bvalue);
}

jlong
Java_com_IT9300_setNullpacket (JNIEnv* env, jobject thiz,
        jint        chip,
        jboolean    bvalue)
{
    printf("%s", __func__);
    return IT9300_setNullpacket(&endeavour, (Byte)chip, (Bool)bvalue);
}

jlong
Java_com_IT9300_modigyTEIerror (JNIEnv* env, jobject thiz,
        jint        chip,
        jboolean    bvalue)
{
    printf("%s", __func__);
    return IT9300_modigyTEIerror(&endeavour, (Byte)chip, (Bool)bvalue);
}

jlong
Java_com_IT9300_setIgnoreFail (JNIEnv* env, jobject thiz,
        jint        chip,
        jboolean    bvalue)
{
    printf("%s", __func__);
    return IT9300_setIgnoreFail(&endeavour, (Byte)chip, (Bool)bvalue);
}

jlong
Java_com_IT9300_setOutputTsType (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsoutType)
{
    printf("%s", __func__);
    //TS_PARALLEL = 0,
    //TS_SERIAL = 1,
    return IT9300_setOutputTsType(&endeavour, (Byte)chip, (TsType)tsoutType);
}

jlong
Java_com_IT9300_setOutputclock (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        value)
{
    printf("%s", __func__);
    return IT9300_setOutputclock(&endeavour, (Byte)chip, value);
}

jlong
Java_com_IT9300_settestmode (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx,
        jint        mode)
{
    printf("%s", __func__);
    return IT9300_settestmode(&endeavour, (Byte)chip, (Byte)tsSrcIdx, (Byte)mode);
}

jlong
Java_com_IT9300_setDataRate (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx,
        jint        value)
{
    printf("%s", __func__);
    return IT9300_setDataRate(&endeavour, (Byte)chip, (Byte)tsSrcIdx, (Byte)value);
}

jlong
Java_com_IT9300_writeGenericRegisters (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        bus,
        jint        slaveAddress,
        jint        bufferLength,
        jintArray   buffer)
{
    printf("%s", __func__);
    Byte* temp = new Byte[bufferLength];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    //printf("%s", __func__);
    for(int i = 0; i < bufferLength; i++)
    {
        temp[i] = intArray[i];
        //printf("\ttemp[%d] = 0x%x", i, temp[i]);
    }

    long error = IT9300_writeGenericRegisters(&endeavour, (Byte)chip, (Byte)bus, (Byte)slaveAddress, (Byte)bufferLength, temp);

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_readGenericRegisters (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        bus,
        jint        slaveAddress,
        jint        bufferLength,
        jintArray   buffer)
{
    printf("%s", __func__);
    Byte* temp = new Byte[bufferLength];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    long error = IT9300_readGenericRegisters(&endeavour, (Byte)chip, (Byte)bus, (Byte)slaveAddress, (Byte)bufferLength, temp);
    //printf("%s", __func__);

    //printf("bufferLength = %d", bufferLength);
    for(int i = 0; i < bufferLength; i++)
    {
        intArray[i] = temp[i];
        //printf("\tintArray[%d] = 0x%x", i, intArray[i]);
    }

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_getIrCode (JNIEnv* env, jobject thiz,
        jint        chip,
        jintArray   code)
{
    printf("%s", __func__);
    Dword* temp = new Dword[1];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(code, 0);
    long error = IT9300_getIrCode(&endeavour, (Byte)chip, temp);
    intArray[0] = temp[0];

    jniEnv->ReleaseIntArrayElements(code, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_reboot (JNIEnv* env, jobject thiz,
        jint        chip)
{
    printf("%s", __func__);
    long error = IT9300_reboot(&endeavour, (Byte)chip);

    BrUser_deleteCriticalSection();

    return error;
}

jlong
Java_com_IT9300_setUARTBaudrate (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        baudrate)
{
    printf("%s", __func__);
    //UART_BAUDRATE_9600 = 0,
    //UART_BAUDRATE_19200 = 1,
    //UART_BAUDRATE_38400 = 2
    return IT9300_setUARTBaudrate(&endeavour, (Byte)chip, (UartBaudRate)baudrate);
}

jlong
Java_com_IT9300_sentUARTData (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        bufferLength,
        jintArray   buffer)
{
    //printf("%s", __func__);
    Byte* temp = new Byte[bufferLength];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    //printf("%s", __func__);
    for(int i = 0; i < bufferLength; i++)
    {
        temp[i] = intArray[i];
        //printf("\ttemp[%d] = 0x%x", i, temp[i]);
    }

    long error = IT9300_sentUARTData(&endeavour, (Byte)chip, (Byte)bufferLength, temp);
    //printf("%s, error = 0x%lx", __func__, error);

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_SetSuspend (JNIEnv* env, jobject thiz,
        jint        chip)
{
    printf("%s", __func__);
    return IT9300_SetSuspend(&endeavour, (Byte)chip);
}

jlong
Java_com_IT9300_SetSuspendback (JNIEnv* env, jobject thiz,
        jint        chip)
{
    printf("%s", __func__);
    return IT9300_SetSuspendback(&endeavour, (Byte)chip);
}

jlong
Java_com_IT9300_readGenericRegistersExtend (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        bus,
        jint        slaveAddress,
        jint        repeatStartEnable,
        jint        wBufferLength,
        jintArray   wBuffer,
        jint        rBufferLength,
        jintArray   rBuffer)
{
    printf("%s", __func__);
    Byte* rTemp = new Byte[rBufferLength];
    Byte* wTemp = new Byte[wBufferLength];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intWArray = jniEnv->GetIntArrayElements(wBuffer, 0);
    //printf("%s", __func__);
    for(int i = 0; i < wBufferLength; i++)
    {
        wTemp[i] = intWArray[i];
        //printf("\ttemp[%d] = 0x%x", i, temp[i]);
    }

    jint *intRArray = jniEnv->GetIntArrayElements(rBuffer, 0);
    long error = IT9300_readGenericRegistersExtend(&endeavour, (Byte)chip, (Byte)bus, (Byte)slaveAddress, (Byte)repeatStartEnable,
            (Byte)wBufferLength, wTemp, (Byte)rBufferLength, rTemp);
    //printf("%s", __func__);

    //printf("bufferLength = %d", bufferLength);
    for(int i = 0; i < rBufferLength; i++)
    {
        intRArray[i] = rTemp[i];
        //printf("\tintArray[%d] = 0x%x", i, intArray[i]);
    }

    jniEnv->ReleaseIntArrayElements(wBuffer, intWArray, 0);
    jniEnv->ReleaseIntArrayElements(rBuffer, intRArray, 0);
    delete[] wTemp;
    delete[] rTemp;
    return error;
}

jlong
Java_com_IT9300_setTsEncryptKey (JNIEnv* env, jobject thiz,
        jint        chip,
        jintArray   buffer)
{
    printf("%s", __func__);
    Byte* temp = new Byte[15];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArray = jniEnv->GetIntArrayElements(buffer, 0);
    //printf("%s", __func__);
    for(int i = 0; i < 15; i++)
    {
        temp[i] = intArray[i];
        //printf("\ttemp[%d] = 0x%x", i, temp[i]);
    }

    long error = IT9300_setTsEncryptKey(&endeavour, (Byte)chip, temp);

    jniEnv->ReleaseIntArrayElements(buffer, intArray, 0);
    delete[] temp;
    return error;
}

jlong
Java_com_IT9300_setTsEncrypt (JNIEnv* env, jobject thiz,
        jint        chip,
        jboolean    benable)
{
    printf("%s", __func__);
    return IT9300_setTsEncrypt(&endeavour, (Byte)chip, (Bool)benable);
}

jlong
Java_com_IT9300_simplePidFilter_AddPid (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx,
        jint        pid,
        jint        index)
{
    printf("%s", __func__);
    return IT9300_simplePidFilter_AddPid(&endeavour, (Byte)chip, (Byte)tsSrcIdx, (Word)pid, (Byte)index);
}

jlong
Java_com_IT9300_simplePidFilter_RemovePid (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx,
        jint        index)
{
    printf("%s", __func__);
    return IT9300_simplePidFilter_RemovePid(&endeavour, (Byte)chip, (Byte)tsSrcIdx, (Byte)index);
}

jlong
Java_com_IT9300_simplePidFilter_ResetPidTable (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx)
{
    printf("%s", __func__);
    return IT9300_simplePidFilter_ResetPidTable(&endeavour, (Byte)chip, (Byte)tsSrcIdx);
}


jlong
Java_com_IT9300_simplePidFilter_DumpPidTable (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx,
        jintArray   pidMode,
        jintArray   pid,
        jintArray   enable)
{
    printf("%s", __func__);
    SimplePidTable* simplePidTable = new SimplePidTable[1];

    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jint *intArrayPidMode = jniEnv->GetIntArrayElements(pidMode, 0);
    jint *intArrayPid = jniEnv->GetIntArrayElements(pid, 0);
    jint *intArrayEnable = jniEnv->GetIntArrayElements(enable, 0);

    long error = IT9300_simplePidFilter_DumpPidTable(&endeavour, chip, tsSrcIdx, simplePidTable);

    intArrayPidMode[0] = simplePidTable[0].pidMode;
    for(int i = 0; i < 64; i++)
    {
        intArrayPid[i] = simplePidTable[0].pid[i];
        intArrayEnable[i] = simplePidTable[0].enable[i];
    }

    jniEnv->ReleaseIntArrayElements(pidMode, intArrayPidMode, 0);
    jniEnv->ReleaseIntArrayElements(pid, intArrayPid, 0);
    jniEnv->ReleaseIntArrayElements(enable, intArrayEnable, 0);

    delete[] simplePidTable;
    return error;
}

jlong
Java_com_IT9300_simplePidFilter_SetMode (JNIEnv* env, jobject thiz,
        jint        chip,
        jint        tsSrcIdx,
        jint        pidMode)
{
    printf("%s", __func__);
    return IT9300_simplePidFilter_SetMode(&endeavour, (Byte)chip, (Byte)tsSrcIdx, (PID_FILTER_MODE)pidMode);
}

// callback
jlong
busTx(Dword bufferLength, Byte* buffer)
{
    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jclass dpclazz = jniEnv->FindClass(className);
    if (dpclazz == 0)
        printf("not find class! %s", className);

    jmethodID methodID = jniEnv->GetStaticMethodID(dpclazz,"busTx","(I[B)J");
    if (methodID == 0)
        printf("not find method busTx()!");

    jbyteArray byteArray = jniEnv->NewByteArray(bufferLength);
    jniEnv->SetByteArrayRegion(byteArray, 0, bufferLength, (jbyte*)buffer);

    long error = BR_ERR_NO_ERROR;
    error = jniEnv->CallStaticLongMethod(dpclazz, methodID, bufferLength, byteArray);
    if (error == 0)
    {
        jbyte* byteArrayElements = jniEnv->GetByteArrayElements(byteArray, 0);
        memcpy(buffer, byteArrayElements, bufferLength);
        jniEnv->ReleaseByteArrayElements(byteArray, byteArrayElements, 0);
    }

    jniEnv->DeleteLocalRef(byteArray);
    jniEnv->DeleteLocalRef(dpclazz);
    return error;
}

jlong
busRx(Dword bufferLength, Byte* buffer)
{
    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return BR_ERR_USB_NULL_HANDLE;
    }

    jclass dpclazz = jniEnv->FindClass(className);
    if (dpclazz == 0)
        printf("not find class! %s", className);


    jmethodID methodID = jniEnv->GetStaticMethodID(dpclazz,"busRx","(I[B)J");
    if (methodID == 0)
        printf("not find method busRx()!");

    jbyteArray byteArray = jniEnv->NewByteArray(bufferLength);

    long error = BR_ERR_NO_ERROR;
    error = jniEnv->CallStaticLongMethod(dpclazz, methodID, bufferLength, byteArray);
    if (error == 0)
    {
        jbyte* byteArrayElements = jniEnv->GetByteArrayElements(byteArray, 0);
        memcpy(buffer, byteArrayElements, bufferLength);
        jniEnv->ReleaseByteArrayElements(byteArray, byteArrayElements, 0);
    }

    jniEnv->DeleteLocalRef(byteArray);
    jniEnv->DeleteLocalRef(dpclazz);
    return error;
}

// test
jstring
Java_com_testJ2CString(JNIEnv* env, jobject thiz)
{
    //return m_pEnv->NewStringUTF("Hello from JNI !");
    //return env->NewStringUTF("Hello from JNI !");
    JNIEnv* jniEnv = NULL;
    if (m_pVm->GetEnv((void**) &jniEnv, jniVersion) != JNI_OK)
    {
        printf("%s %d not found JNIEnv", __FILE__, __LINE__);
        return env->NewStringUTF("not found JNIEnv");
    }

    return jniEnv->NewStringUTF("Hello from JNI !");
}

void
Java_com_testC2JCallback(JNIEnv * env, jobject obj)
{
    jclass dpclazz = env->FindClass(className);
    if (dpclazz == 0)
        printf("not find class!");
    else
        printf("find class");
    jmethodID methodID = env->GetStaticMethodID(dpclazz,"Add","(II)I");
    if (methodID == 0)
        printf("not find method Add()!");
    else
        printf("find method");
    env->CallStaticIntMethod(dpclazz, methodID, 3l, 4l);

    env->DeleteLocalRef(dpclazz);
}


//JNI_OnLoad Function begin.
/*
 * Register several native methods for one class.
 */
static int
registerMethods(JNIEnv* env)
{
    static JNINativeMethod gMethods[] =
    {
        {"init",                        "(I)J",          (void*)Java_com_init },

        // SL3000_R855
        {"TunerDemod_Atsc3Tune",           "(III)J",                (void*)Java_com_TunerDemod_Atsc3Tune},
        {"TunerDemod_Atsc3SetPLP",         "(IC[C)J",               (void*)Java_com_TunerDemod_Atsc3SetPLP},
        {"TunerDemod_Atsc3GetStatus",      "(I[I[Z[Z[D[I[I)J",      (void*)Java_com_TunerDemod_Atsc3GetStatus},
        {"TunerDemod_Atsc1Tune",           "(II)J",                 (void*)Java_com_TunerDemod_Atsc1Tune},
        {"TunerDemod_Atsc1GetStatus",      "(I[I[Z[Z[I[D[D[I)J",    (void*)Java_com_TunerDemod_Atsc1GetStatus},
        {"Demod_Atsc1WriteRegisters",       "(III[B)J",              (void*)Java_com_Demod_Atsc1WriteRegisters},
        {"Demod_Atsc1ReadRegisters",        "(III[B)J",              (void*)Java_com_Demod_Atsc1ReadRegisters},

        // Endeavour IT9300
        {"IT9300_writeRegisters",       "(III[I)J",     (void*)Java_com_IT9300_writeRegisters },
        {"IT9300_writeRegister",        "(III)J",       (void*)Java_com_IT9300_writeRegister },
        {"IT9300_writeEepromValues",    "(III[I)J",     (void*)Java_com_IT9300_writeEepromValues },
        {"IT9300_writeRegisterBits",    "(IIIII)J",     (void*)Java_com_IT9300_writeRegisterBits },
        {"IT9300_readRegisters",        "(III[I)J",     (void*)Java_com_IT9300_readRegisters },
        {"IT9300_readRegister",         "(II[I)J",      (void*)Java_com_IT9300_readRegister },
        {"IT9300_readEepromValues",     "(III[I)J",     (void*)Java_com_IT9300_readEepromValues },
        {"IT9300_readRegisterBits",     "(IIII[I)J",    (void*)Java_com_IT9300_readRegisterBits },
        {"IT9300_setInTsPktLen",        "(II)J",        (void*)Java_com_IT9300_setInTsPktLen },
        {"IT9300_setSyncByteMode",      "(II)J",        (void*)Java_com_IT9300_setSyncByteMode },
        {"IT9300_setTagMode",           "(II)J",        (void*)Java_com_IT9300_setTagMode },
        {"IT9300_setPidRemapMode",      "(II)J",        (void*)Java_com_IT9300_setPidRemapMode },
        {"IT9300_enableTsPort",         "(II)J",        (void*)Java_com_IT9300_enableTsPort },
        {"IT9300_disableTsPort",        "(II)J",        (void*)Java_com_IT9300_disableTsPort },
        {"IT9300_setInTsType",          "(II)J",        (void*)Java_com_IT9300_setInTsType },
        {"IT9300_enPidFilterAT",        "(III)J",       (void*)Java_com_IT9300_enPidFilterAT },
        {"IT9300_disPidFilterAT",       "(III)J",       (void*)Java_com_IT9300_disPidFilterAT },
        {"IT9300_enPidFilter",          "(II)J",        (void*)Java_com_IT9300_enPidFilter },
        {"IT9300_setExternalclock",     "(IZ)J",        (void*)Java_com_IT9300_setExternalclock },
        {"IT9300_setNullpacket",        "(IZ)J",        (void*)Java_com_IT9300_setNullpacket },
        {"IT9300_modigyTEIerror",       "(IZ)J",        (void*)Java_com_IT9300_modigyTEIerror },
        {"IT9300_setIgnoreFail",        "(IZ)J",        (void*)Java_com_IT9300_setIgnoreFail },
        {"IT9300_setOutputTsType",      "(II)J",        (void*)Java_com_IT9300_setOutputTsType },
        {"IT9300_setOutputclock",       "(II)J",        (void*)Java_com_IT9300_setOutputclock },
        {"IT9300_settestmode",          "(III)J",       (void*)Java_com_IT9300_settestmode },
        {"IT9300_setDataRate",          "(III)J",       (void*)Java_com_IT9300_setDataRate },
        {"IT9300_writeGenericRegisters","(IIII[I)J",    (void*)Java_com_IT9300_writeGenericRegisters },
        {"IT9300_readGenericRegisters", "(IIII[I)J",    (void*)Java_com_IT9300_readGenericRegisters },
        {"IT9300_getIrCode",            "(I[I)J",       (void*)Java_com_IT9300_getIrCode },
        {"IT9300_reboot",               "(I)J",         (void*)Java_com_IT9300_reboot },
        {"IT9300_setUARTBaudrate",      "(II)J",        (void*)Java_com_IT9300_setUARTBaudrate },
        {"IT9300_sentUARTData",         "(II[I)J",      (void*)Java_com_IT9300_sentUARTData },
        {"IT9300_SetSuspend",           "(I)J",         (void*)Java_com_IT9300_SetSuspend },
        {"IT9300_SetSuspendback",       "(I)J",         (void*)Java_com_IT9300_SetSuspendback },
        {"IT9300_readGenericRegistersExtend",   "(IIIII[II[I)J",(void*)Java_com_IT9300_readGenericRegistersExtend },
        {"IT9300_setTsEncryptKey",      "(I[I)J",       (void*)Java_com_IT9300_setTsEncryptKey },
        {"IT9300_setTsEncrypt",         "(IZ)J",        (void*)Java_com_IT9300_setTsEncrypt },
        {"IT9300_simplePidFilter_AddPid",       "(IIII)J",    (void*)Java_com_IT9300_simplePidFilter_AddPid },
        {"IT9300_simplePidFilter_RemovePid",    "(III)J",     (void*)Java_com_IT9300_simplePidFilter_RemovePid },
        {"IT9300_simplePidFilter_ResetPidTable","(II)J",      (void*)Java_com_IT9300_simplePidFilter_ResetPidTable },
        {"IT9300_simplePidFilter_DumpPidTable", "(II[I[I[I)J",(void*)Java_com_IT9300_simplePidFilter_DumpPidTable },
        {"IT9300_simplePidFilter_SetMode",      "(III)J",     (void*)Java_com_IT9300_simplePidFilter_SetMode },

        // test
        {"testJ2CString",               "()Ljava/lang/String;", (void*)Java_com_testJ2CString },
        {"testC2JCallback",             "()V",                  (void*)Java_com_testC2JCallback },
    };

    jclass clazz;
    /* look up the class */
    clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        return -1;
    }

    /* register all the methods */
    if (env->RegisterNatives(clazz, gMethods,sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK)
    {
      return -1;
    }
    /* fill out the rest of the ID cache */
    return 0;
}
/*
 * This is called by the VM when the shared library is first loaded.
 */
jint
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    //JNIEnv* env = new JNIEnv();
    jint result = -1;
    //DST("%s %d", __FILE__, __LINE__);
    //int res = vm->AttachCurrentThread(&env, NULL);

    if (vm->GetEnv((void**) &env, jniVersion) != JNI_OK)
    {
        printf("%s %d", __FILE__, __LINE__);
        goto fail;
    }
    if (registerMethods(env) != 0)
    {
        printf("%s %d", __FILE__, __LINE__);
        goto fail;
    }
    /* success -- return valid version number */
    result = jniVersion;
    m_pVm = vm;
    //m_pEnv = env;

    fail: return result;
}

void
JNI_OnUnload(JavaVM* vm, void* reserved)
{
    //DST("%s %d", __FILE__, __LINE__);
    JNIEnv *env;
    if (vm->GetEnv((void**) &env, jniVersion)) {
        return;
    }

    //env->DeleteWeakGlobalRef(Class_C);
    return;
}

