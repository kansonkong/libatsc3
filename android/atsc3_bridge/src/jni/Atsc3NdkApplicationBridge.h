//
// Created by Jason Justman on 2019-09-27.
//

#include <jni.h>
#include <string.h>
#include <thread>
#include <map>
#include <queue>
#include <vector>
#include <mutex>
#include <semaphore.h>
#include <list>
#include <sys/types.h>

using namespace std;

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#ifndef LIBATSC3_ATSC3NDKAPPLICATIONBRIDGE_H
#define LIBATSC3_ATSC3NDKAPPLICATIONBRIDGE_H

#define MODULE_NAME "Atsc3NdkApplicationBridge"

#include "Atsc3LoggingUtils.h"
#include "Atsc3JniEnv.h"

// libatsc3 type imports here
#include <atsc3_utils.h>
#include <atsc3_logging_externs.h>
#include <atsc3_lls_types.h>
#include <atsc3_alc_rx.h>
#include <atsc3_pcap_type.h>
#include <atsc3_monitor_events_alc.h>
#include <atsc3_route_package_utils.h>
#include <application/IAtsc3NdkApplicationBridge.h>

#include "Atsc3BridgeNdkStaticJniLoader.h"

#include <atsc3_core_service_player_bridge.h>


class Atsc3NdkApplicationBridge : public IAtsc3NdkApplicationBridge
{
public:
    Atsc3NdkApplicationBridge(JNIEnv* env, jobject jni_instance);
    
    void LogMsg(const char *msg) override;
    void LogMsg(const std::string &msg) override;
    void LogMsgF(const char *fmt, ...) override;

    /** atsc3 service methods **/
    int atsc3_slt_selectService(int service_id) override;

    void atsc3_lls_sls_alc_on_package_extract_completed_callback_jni(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload_t) override;

    int atsc3_slt_alc_select_additional_service(int service_id) override;

    int atsc3_slt_alc_clear_additional_service_selections() override;

    vector<string> atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(int service_id, const char* to_match_content_type) override;

    vector<string> atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id(int service_id) override;

    void atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni(uint16_t service_id, uint32_t tsi, uint32_t toi, char* s_tsid_content_location, char* s_tsid_content_type, char* cache_file_path) override;

    void atsc3_lls_sls_alc_on_route_mpd_patched_jni(uint16_t service_id) override;

    void atsc3_onSlsTablePresent(const char *sls_payload_xml) override;
    void atsc3_onAeatTablePresent(const char* aeat_payload_xml) override;
    void atsc3_onSlsHeldEmissionPresent(uint16_t service_id, const char *held_payload) override;

    void atsc3_onAlcObjectStatusMessage(const char *fmt, ...) override;
	void atsc3_onAlcObjectClosed(uint16_t service_id, uint32_t tsi, uint32_t toi, char* s_tsid_content_location, char* s_tsid_content_type, char* cache_file_path) override;

    std::string get_android_temp_folder() override;


    //application bridge to phy instance callbacks for PLP selection change
    void set_plp_settings(jint *a_plp_ids, jsize sa_plp_size) override;

    void atsc3_phy_notify_plp_selection_change_set_callback(atsc3_phy_notify_plp_selection_change_f atsc3_phy_notify_plp_selection_change, void* context) override;
    void atsc3_phy_notify_plp_selection_change_clear_callback() override;
    void atsc3_phy_notify_plp_selection_changed(vector<uint8_t> plps_to_listen) override;

private:
    JNIEnv* env = nullptr;
    jobject jni_instance_globalRef = nullptr;
    jclass jni_class_globalRef = nullptr;

    std::thread mhRxThread;

    //alc service monitoring
    //vector<int>                     atsc3_slt_alc_additional_services_monitored;

    //global env.Get()->NewGlobalRef(jobjectByteBuffer); for c alloc'd MFU's and NAL's
    std::vector<jobject> global_jobject_mfu_refs;
    std::vector<jobject> global_jobject_nal_refs;

public:
    JavaVM* mJavaVM = nullptr;    // Java VM, if we don't have a pinned thread context for dispatch

    void setJniClassReference(string jclass_name) {
        if(env) {
            jclass jclass_local = env->FindClass(jclass_name.c_str());
            jni_class_globalRef = reinterpret_cast<jclass>(env->NewGlobalRef(jclass_local));
        }
    }

    jclass getJniClassReference() {
        return jni_class_globalRef;
    }

    int pinConsumerThreadAsNeeded() override;
    int releasePinnedConsumerThreadAsNeeded() override;

    jmethodID mOnLogMsgId = nullptr;

    jmethodID atsc3_onSlsTablePresent_ID = nullptr; //push LLS SLT table update
    jmethodID atsc3_onAeatTablePresent_ID = nullptr;
    jmethodID atsc3_onSlsHeldEmissionPresent_ID = nullptr;

    jmethodID atsc3_lls_sls_alc_on_route_mpd_patched_ID = nullptr;
    jmethodID atsc3_on_alc_object_status_message_ID = nullptr;

	jmethodID atsc3_on_alc_object_closed_ID = nullptr;
    jmethodID atsc3_lls_sls_alc_on_package_extract_completed_ID = nullptr;

    jclass packageExtractEnvelopeMetadataAndPayload_jclass_init_env = nullptr;
    jclass packageExtractEnvelopeMetadataAndPayload_jclass_global_ref = nullptr;

    jclass packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_init_env = nullptr;
    jclass packageExtractEnvelopeMetadataAndPayload_MultipartRelatedPayload_jclass_global_ref = nullptr;

    //todo: refactor this out - ala https://gist.github.com/qiao-tw/6e43fb2311ee3c31752e11a4415deeb1
    jclass      jni_java_util_ArrayList = nullptr;
    jmethodID   jni_java_util_ArrayList_cctor = nullptr;
    jmethodID   jni_java_util_ArrayList_add = nullptr;


    atsc3_phy_notify_plp_selection_change_f   atsc3_phy_notify_plp_selection_change;
    void*                                      atsc3_phy_notify_plp_selection_change_context;

protected:
    Atsc3JniEnv* bridgeConsumerJniEnv = nullptr;


};

#define _NDK_APPLICATION_BRIDGE_ERROR(...)   	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _NDK_APPLICATION_BRIDGE_WARN(...)   	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _NDK_APPLICATION_BRIDGE_INFO(...)   	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define _NDK_APPLICATION_BRIDGE_DEBUG(...)   	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define _NDK_APPLICATION_BRIDGE_TRACE(...)   	__LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);



#endif //LIBATSC3_ATSC3NDKAPPLICATIONBRIDGE_H
