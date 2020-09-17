//
// Created by Jason Justman on 2019-09-27.
//
#ifndef __JJ_PHY_MMT_PLAYER_BRIDGE_DISABLED

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <ftw.h>
#include <mutex>

using namespace std;

#ifndef LIBATSC3_ATSC3CORESERVICEPLAYERBRIDGE_H
#define LIBATSC3_ATSC3CORESERVICEPLAYERBRIDGE_H

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"

#include "atsc3_listener_udp.h"
#include "atsc3_alp_types.h"
#include "atsc3_alc_rx.h"

#include "atsc3_lls_types.h"
#include "atsc3_sl_tlv_demod_type.h"

#include "atsc3_lls.h"
#include "atsc3_lls_slt_parser.h"
#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
#include "atsc3_lls_types.h"

#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_mmtp_ntp32_to_pts.h"
#include "atsc3_mmt_mpu_utils.h"
#include "atsc3_mmt_context_mfu_depacketizer.h"

#include "atsc3_hevc_nal_extractor.h"

#include "atsc3_lls_alc_utils.h"
#include "atsc3_alc_rx.h"
#include "atsc3_alp_types.h"

//runtime app interface includes (e.g. I/F for android, etc)
#include "application/IAtsc3NdkApplicationBridge.h"
#include "phy/IAtsc3NdkPHYBridge.h"
#include "phy/IAtsc3NdkPHYClient.h"

//c++ linkage methods

string atsc3_route_service_context_temp_folder_name(int service_id);
string atsc3_ndk_cache_temp_folder_path_get(int service_id);

#if defined (__cplusplus)
extern "C" {
#endif

//methods

void atsc3_core_service_application_bridge_init(IAtsc3NdkApplicationBridge* atsc3NdkApplicationBridge);
void atsc3_core_service_application_bridge_reset_context();
IAtsc3NdkApplicationBridge* atsc3_ndk_application_bridge_get_instance();

void atsc3_core_service_phy_bridge_init(IAtsc3NdkPHYBridge* atsc3NdkPHYBridge);
IAtsc3NdkPHYBridge* atsc3_ndk_phy_bridge_get_instance();

//jjustman-2020-08-18 - signature match for typedef void(*atsc3_phy_rx_udp_packet_process_callback_f)(uint8_t plp_num, block_t* block);

void atsc3_core_service_bridge_process_packet_from_plp_and_block(uint8_t plp_num, block_t* block);
void atsc3_core_service_bridge_process_packet_phy(block_t* packet);

//change SLT service and wire up a single montior
atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_set_single_monitor_a331_service_id(int service_id);

//add additional alc monitor service_id's for supplimentary MMT or ROUTE flows
atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_add_monitor_a331_service_id(int service_id);
atsc3_lls_slt_service_t* atsc3_phy_mmt_player_bridge_remove_monitor_a331_service_id(int service_id);
//TODO: wire up ROUTE/ALC and MBMS/FDT event callback hooks for close_object emission (including delivery metrics w.r.t ALC DU loss)

lls_sls_alc_monitor_t* atsc3_lls_sls_alc_monitor_get_from_service_id(int service_id);

//ALC/ROUTE: use case: parse out the atsc3_mbms_metadata_envelope to get MPD from metadataURI
atsc3_sls_metadata_fragments_t* atsc3_slt_alc_get_sls_metadata_fragments_from_monitor_service_id(int service_id);

//ALC/ROUTE: use case: get the app-based/esg service corresponding efdt atsc3_fdt_file content_location value
atsc3_route_s_tsid_t* atsc3_slt_alc_get_sls_route_s_tsid_from_monitor_service_id(int service_id);

//free our old atsc3_link_mapping_table_t* if returned
atsc3_link_mapping_table_t* atsc3_phy_jni_bridge_notify_link_mapping_table(atsc3_link_mapping_table_t* atsc3_link_mapping_table_pending);

int atsc3_ndk_cache_temp_folder_purge(char *path);

void atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_ndk(uint32_t tsi, uint32_t toi, char* content_location);
void atsc3_lls_sls_alc_on_route_mpd_patched_ndk(uint16_t service_id);

void atsc3_lls_sls_alc_on_package_extract_completed_callback_ndk(atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload_t);

//#1569
void atsc3_sls_on_held_trigger_received_callback_impl(uint16_t service_id, block_t* held_payload);

/*
 *
 * internally defined _f callback handlers wired up in: atsc3_core_service_player_bridge_init

    lls_slt_monitor->atsc3_lls_on_sls_table_present_callback = &atsc3_lls_on_sls_table_present_ndk;

    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete = &atsc3_mmt_mpu_mfu_on_sample_complete_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt = &atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk;
    //todo: search thru NAL's as needed here and discard anything that intra-NAL..
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header = &atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_missing = &atsc3_mmt_mpu_mfu_on_sample_missing_ndk;


    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor = &atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_ndk;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor  = &atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_ndk;

    //extract out one trun sampleduration for essence timing
    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present = &atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk;

    void atsc3_lls_on_sls_table_present_ndk(lls_table_t* lls_table);
    void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata);
    */

void atsc3_lls_on_sls_table_present_ndk(lls_table_t* lls_table);
void atsc3_lls_on_aeat_table_present_ndk(lls_table_t* lls_table);

void atsc3_mmt_mpu_on_sequence_mpu_metadata_present_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_mpu_metadata);
void atsc3_mmt_mpu_mfu_on_sample_complete_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_rebuilt);
void atsc3_mmt_mpu_mfu_on_sample_corrupt_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
void atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number, block_t* mmt_mfu_sample, uint32_t mfu_fragment_count_expected, uint32_t mfu_fragment_count_rebuilt);
void atsc3_mmt_mpu_mfu_on_sample_missing_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, uint32_t sample_number);
void atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor_ndk(uint16_t video_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
void atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor_ndk(uint16_t audio_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
void atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor_ndk(uint16_t stpp_packet_id, uint32_t mpu_sequence_number, uint64_t mpu_presentation_time_ntp64, uint32_t mpu_presentation_time_seconds, uint32_t mpu_presentation_time_microseconds);
void atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present_ndk(uint16_t packet_id, uint32_t mpu_sequence_number, block_t* mmt_movie_fragment_metadata);


#if defined (__cplusplus)
}
#endif

#endif //LIBATSC3_ATSC3CORESERVICEPLAYERBRIDGE_H

#endif
