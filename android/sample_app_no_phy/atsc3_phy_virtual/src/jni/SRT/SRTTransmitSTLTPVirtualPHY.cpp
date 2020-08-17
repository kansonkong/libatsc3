#include <string.h>
#include <jni.h>
#include <Atsc3LoggingUtils.h>
#include <Atsc3JniEnv.h>
#include <atsc3_utils.h>

#include <Atsc3SRT_live_transmit.h>


extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    //api = new Atsc3NdkPHYBridge(vm);
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

//    jclass jClazz = env->FindClass("org/ngbp/libatsc3/middleware/Atsc3NdkPHYBridge");
//    if (jClazz == NULL) {
//        NDK_PHY_BRIDGE_ERROR("PHY_BRIDGE::JNI_OnLoad - Cannot find org/ngbp/libatsc3/middleware/Atsc3NdkPHYBridge java class");
//        return -1;
//    }
//
//    api->mOnLogMsgId = env->GetMethodID(jClazz, "onLogMsg", "(Ljava/lang/String;)I");
//    if (api->mOnLogMsgId == NULL) {
//        NDK_PHY_BRIDGE_ERROR("PHY_BRIDGE::JNI_OnLoad - Cannot find 'onLogMsg' method id");
//        return -1;
//    }
//
//    api->atsc3_rf_phy_status_callback_ID = env->GetMethodID(jClazz, "atsc3_rf_phy_status_callback", "(IIIIIIIIIIIIIII)I");
//    if (api->atsc3_rf_phy_status_callback_ID == NULL) {
//        NDK_PHY_BRIDGE_ERROR("PHY_BRIDGE::JNI_OnLoad - 'atsc3_rf_phy_status_callback' method id");
//        return -1;
//    }
//
//    //atsc3_update_rf_bw_stats_ID
//    api->atsc3_update_rf_bw_stats_ID = env->GetMethodID(jClazz, "atsc3_updateRfBwStats", "(JJI)I");
//    if (api->atsc3_update_rf_bw_stats_ID == NULL) {
//        NDK_PHY_BRIDGE_ERROR("PHY_BRIDGE::JNI_OnLoad - Cannot find 'atsc3_update_rf_bw_stats_ID' method id");
//        return -1;
//    }

    //NDK_PHY_BRIDGE_INFO("SRTTransmitSTLTPVirtualPHY::JNI_OnLoad complete, vm: %p", vm);
    return JNI_VERSION_1_6;
}

//org.ngbp.libatsc3.middleware.android.phy.virtual.srt.SRTTransmitSTLTPVirtualPHY
extern "C" JNIEXPORT jint JNICALL
Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTTransmitSTLTPVirtualPHY_ApiInit(JNIEnv *env, jobject instance)
{
    printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTTransmitSTLTPVirtualPHY_ApiInit: start init, env: %p\n", env);

    atsc3srt_live_transmit_startup();
    printf("Java_org_ngbp_libatsc3_middleware_android_phy_virtual_srt_SRTTransmitSTLTPVirtualPHY_ApiInit: return, env: %p\n", env);

    //printf("Atsc3NdkPHYBridge_Init: with jniInstance: %p", api->getJniInstance());
    return 0;
}

