/*
 * IAtsc3NdkApplicationBridge.h
 *
 *  Created on: Aug 10, 2020
 *      Author: jjustman
 */
#include <string>
#include <vector>

#ifndef SRC_APPLICATION_IATSC3NDKAPPLICATIONBRIDGE_H_
#define SRC_APPLICATION_IATSC3NDKAPPLICATIONBRIDGE_H_

using namespace std;

typedef void(*atsc3_phy_notify_plp_selection_change_f)(vector<uint8_t> plp, void* context);

class IAtsc3NdkApplicationBridge {

    public:
        //jni management
        virtual int pinConsumerThreadAsNeeded() = 0;
        virtual int releasePinnedConsumerThreadAsNeeded() = 0;

        //logging
        virtual void LogMsg(const char *msg) = 0;
        virtual void LogMsg(const std::string &msg) = 0;
        virtual void LogMsgF(const char *fmt, ...) = 0;

        virtual void atsc3_onAeatTablePresent(const char* aeat_payload_xml) = 0;

        /** atsc3 service methods **/

        virtual int atsc3_slt_select_service(int service_id) = 0;  //jjustman-2021-01-21 - renamed to match c/c++ _ style instead of camelCase
        virtual int atsc3_slt_alc_select_additional_service(int service_id) = 0;
        virtual int atsc3_slt_alc_clear_additional_service_selections() = 0;

        virtual void atsc3_onSltTablePresent(const char* slt_payload_xml) = 0;
        virtual void atsc3_onSlsHeldEmissionPresent(uint16_t service_id, const char *held_payload) = 0;

        virtual void atsc3_lls_sls_alc_on_package_extract_completed_callback_jni(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload_t) = 0;

        virtual vector<string> atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(int service_id, const char* to_match_content_type) = 0;

        virtual vector<string> atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id(int service_id) = 0;

        virtual void atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_jni(uint16_t service_id, uint32_t tsi, uint32_t toi, char* s_tsid_content_location, char* s_tsid_content_type, char* cache_file_path) = 0;

        virtual void atsc3_lls_sls_alc_on_route_mpd_patched_jni(uint16_t service_id) = 0;

        virtual void atsc3_onAlcObjectStatusMessage(const char *fmt, ...) = 0;
        virtual void atsc3_onAlcObjectClosed(uint16_t service_id, uint32_t tsi, uint32_t toi, char* s_tsid_content_location, char* s_tsid_content_type, char* cache_file_path) = 0;

        virtual string get_android_temp_folder() = 0;

        //application bridge to phy instance callbacks for PLP selection change
        virtual void atsc3_phy_notify_plp_selection_change_set_callback(atsc3_phy_notify_plp_selection_change_f atsc3_phy_notify_plp_selection_change, void* context) = 0;
        virtual void atsc3_phy_notify_plp_selection_change_clear_callback() = 0;
        virtual void atsc3_phy_notify_plp_selection_changed(vector<uint8_t> plps_to_listen) = 0;

};



#endif /* SRC_APPLICATION_IATSC3NDKAPPLICATIONBRIDGE_H_ */
