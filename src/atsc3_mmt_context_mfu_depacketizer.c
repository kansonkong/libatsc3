/*
 * atsc3_mmt_reconstitution_from_media_sample.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 *
 *
 *
 *      TODO:
 *

A/331: 8.1.2.3 Synchronization:
The synchronization of MPUs shall be done by using timestamps referencing UTC
delivered by ATSC 3.0 PHY layer.

The MPU_timestamp_descriptor as defined in subclause 10.5.2 of ISO/IEC 23008-1 [37]
shall be used to represent the presentation time of the first media sample in
presentation order in each MPU.

The presentation time of each media sample of an MPU shall be calculated by
adding the presentation time of the first media sample of an MPU to the value
of the composition time of each sample in the MPU.

The rule to calculate the value of the composition time of media samples in an MPU
shall be be calculated by using the rule in the ISO BMFF specification [34].


23008-1 reference:

CRI table:

CRI descriptor: 0x0000

MPU_timestamp_descriptor: 0x0001

 */

#include "atsc3_mmt_context_mfu_depacketizer.h"
#include "atsc3_mmtp_packet_types.h"

int _MMT_CONTEXT_MPU_SIGNAL_INFO_ENABLED = 0;
int _MMT_CONTEXT_MPU_DEBUG_ENABLED = 0;
int _MMT_CONTEXT_MPU_TRACE_ENABLED = 0;

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window, atsc3_mmt_mfu_mpu_timestamp_descriptor);
ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_mmt_mfu_mpu_timestamp_descriptor);


atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_internal_flows_new() {
    atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = calloc(1, sizeof(atsc3_mmt_mfu_context_t));

    //internal data structs
    atsc3_mmt_mfu_context->mmtp_flow = mmtp_flow_new();
    atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container = udp_flow_latest_mpu_sequence_number_container_t_init();

    //internal related callbacks
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_internal = &atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_callback_internal;

//    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_subset			= &atsc3_mmt_signalling_information_on_mp_table_subset_noop;
//    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete 		= &atsc3_mmt_signalling_information_on_mp_table_complete_noop;

    //by default, update our atsc3_mmt_mfu_context->mmtp_packet_id_packets_container with our asset_type value and mp_table entry w/ atsc3_mmt_mp_table_asset_row_duplicate
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id 	= &atsc3_mmt_signalling_information_on_audio_essence_packet_id_callback_internal;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id 	= &atsc3_mmt_signalling_information_on_video_essence_packet_id_callback_internal;
    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_essence_packet_id 	= &atsc3_mmt_signalling_information_on_stpp_essence_packet_id_callback_internal;

    //helper methods
    atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number                                           = atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number; //&atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe;
    atsc3_mmt_mfu_context->get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential = atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential; //&atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe;

    return atsc3_mmt_mfu_context;
}


atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number) {
//    __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number: entries: %d, looking for packet_id: %d, mpu_sequence_number: %d",  atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count, packet_id, mpu_sequence_number);

    for(int i=0; i < atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count; i++) {
        atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.data[i];
        if(atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id == packet_id && atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number == mpu_sequence_number) {
//                __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number: retuning matching: %lu, looking for packet_id: %d, mpu_sequence_number: %d",
//                                        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value,
//                                        packet_id,
//                                        mpu_sequence_number);
                return atsc3_mmt_mfu_mpu_timestamp_descriptor;
        }
    }

    return NULL;
}

atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mmtp_timestamp, uint32_t mpu_sequence_number) {
//    __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential: entries: %d, looking for packet_id: %d, mmtp_timestamp: %u, mpu_sequence_number: %d",  atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count, packet_id, mmtp_timestamp, mpu_sequence_number);

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor_max = NULL;

    for(int i=0; i < atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count; i++) {
        atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.data[i];
        if(atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id == packet_id) {
            atsc3_mmt_mfu_mpu_timestamp_descriptor_max = atsc3_mmt_mfu_mpu_timestamp_descriptor;
            if(atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number == mpu_sequence_number) {
                __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential: found matching packet_id: %d, mpu_sequence_number: %d, computed_from_recovery: %d, mpu_presentation_time_as_us_value: %" PRIu64,
                                        packet_id, mpu_sequence_number, atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_computed_from_recovery_mmtp_timestamp_flag, atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value);

                return atsc3_mmt_mfu_mpu_timestamp_descriptor;
            }
        }
    }

    if(atsc3_mmt_mfu_mpu_timestamp_descriptor_max != NULL) {
        //compute our mmtp_timestamp differental(s)
        uint16_t mmtp_timestamp_differential_s = 0;
        uint16_t mmtp_timestamp_differential_ms = 0;

        uint32_t mpu_presentation_time_seconds_adjusted_from_mmtp_timestamp_differential = 0;
        uint32_t mpu_presentation_time_microseconds_adjusted_from_mmtp_timestamp_differential = 0;
        uint64_t mpu_presentation_time_as_us_value_adjusted_from_mmtp_timestamp_differential = 0;

        uint32_t mmtp_timestamp_differential_ntp32 = mmtp_timestamp - atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mmtp_timestamp;

        compute_ntp32_to_seconds_microseconds(mmtp_timestamp_differential_ntp32, &mmtp_timestamp_differential_s, &mmtp_timestamp_differential_ms);
        if(mmtp_timestamp_differential_s > 60) {
            __MMT_CONTEXT_MPU_ERROR("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential: computed differental of %d.%03d is too large, bailing!"
                                    "matching packet_id: %d, mpu_sequence_number: %u, using: atsc3_mmt_mfu_mpu_timestamp_descriptor_max: mmtp_timestamp: %u, mpu_sequence_number: %u, mpu_presentation_time_as_us_value: %" PRIu64 " from_recovery: %d, our current mmtp_timestamp: %u",
                                    mmtp_timestamp_differential_s,
                                    mmtp_timestamp_differential_ms,
                                    packet_id,
                                    mpu_sequence_number,
                                    atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mmtp_timestamp,
                                    atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_sequence_number,
                                    atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_as_us_value,
                                    atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_computed_from_recovery_mmtp_timestamp_flag,
                                    mmtp_timestamp);
            return NULL;
        }

        //See https://tools.ietf.org/html/rfc5905#section-6, ntp64 has 32bit seconds and 32bit fractional which resolves to 232 picoseconds.

        uint64_t mpu_presentation_time_ntp64_adjusted_from_mmtp_timestamp_differential = atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_ntp64 + ((uint64_t)(mmtp_timestamp_differential_s) << 32) + ((uint32_t)(mmtp_timestamp_differential_ms * 232));
        compute_ntp64_to_seconds_microseconds(mpu_presentation_time_ntp64_adjusted_from_mmtp_timestamp_differential, &mpu_presentation_time_seconds_adjusted_from_mmtp_timestamp_differential, &mpu_presentation_time_microseconds_adjusted_from_mmtp_timestamp_differential);

        mpu_presentation_time_as_us_value_adjusted_from_mmtp_timestamp_differential = compute_seconds_microseconds_to_scalar64(mpu_presentation_time_seconds_adjusted_from_mmtp_timestamp_differential, mpu_presentation_time_seconds_adjusted_from_mmtp_timestamp_differential);

        //otherwise, prepare this new atsc3_mmt_mfu_mpu_timestamp_descriptor
        __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_mmtp_timestamp_recovery_differential: missing matching packet_id: %d, mpu_sequence_number: %u, using: "
                                "atsc3_mmt_mfu_mpu_timestamp_descriptor_max: packet_id: %d, mmtp_timestamp: %u, mpu_sequence_number: %u, mpu_presentation_time_as_us_value: %" PRIu64 " from recovery flag: %d "
                                "new computed timestamp using mmtp_timestamp: %u, mpu_presentation_time_ntp64_adjusted: %" PRIu64 " mpu_presentation_time_as_us_value_adjusted_from_mmtp_timestamp_differential: %" PRIu64,
                                packet_id,
                                mpu_sequence_number,
                                atsc3_mmt_mfu_mpu_timestamp_descriptor_max->packet_id,
                                atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mmtp_timestamp,
                                atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_sequence_number,
                                atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_as_us_value,
                                atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_computed_from_recovery_mmtp_timestamp_flag,
                                mmtp_timestamp,
                                mpu_presentation_time_ntp64_adjusted_from_mmtp_timestamp_differential,
                                mpu_presentation_time_as_us_value_adjusted_from_mmtp_timestamp_differential);

        //borrowed from atsc3_mmt_signalling_information_callbacks_internal.c:  atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_callback_internal
        atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_mpu_timestamp_descriptor_new();
        atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id = packet_id;

        //set our recovery anchor attributes
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_computed_from_recovery_mmtp_timestamp_flag = true;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->recovery_mmtp_timestamp              = atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mmtp_timestamp;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->recovery_mpu_sequence_number         = atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_sequence_number;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->recovery_mpu_presentation_time_ntp64 = atsc3_mmt_mfu_mpu_timestamp_descriptor_max->recovery_mpu_presentation_time_ntp64;

        atsc3_mmt_mfu_mpu_timestamp_descriptor->mmtp_timestamp = mmtp_timestamp;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number = mpu_sequence_number;

        //shift and add our differential into our mpu_presentation_time_ntp64
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64_adjusted_from_mmtp_timestamp_differential;

        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_seconds = atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_seconds + mmtp_timestamp_differential_s;
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_microseconds = atsc3_mmt_mfu_mpu_timestamp_descriptor_max-> mpu_presentation_time_microseconds;

        //jjustman-2020-11-19 - make sure to coerce our uS scalar (1000000) as long, otherwise our value will be implicity coerced into (uint32_t) instead of uint64_t 	mpu_presentation_time_as_us_value
        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value = mpu_presentation_time_as_us_value_adjusted_from_mmtp_timestamp_differential;

        atsc3_mmt_mfu_mpu_timestamp_descriptor_rolling_window_add_atsc3_mmt_mfu_mpu_timestamp_descriptor(&atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window, atsc3_mmt_mfu_mpu_timestamp_descriptor);

        return atsc3_mmt_mfu_mpu_timestamp_descriptor;
    } else {
        return NULL;
    }
}

atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, uint16_t packet_id, uint32_t mpu_sequence_number) {
    __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe: entries: %d, looking for packet_id: %d, mpu_sequence_number: %d",
						   atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count,
						   packet_id,
						   mpu_sequence_number);

    atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor_max = NULL;

	for(int i=0; i < atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.count; i++) {
		atsc3_mmt_mfu_mpu_timestamp_descriptor_t* atsc3_mmt_mfu_mpu_timestamp_descriptor = atsc3_mmt_mfu_context->packet_id_mpu_timestamp_descriptor_window.atsc3_mmt_mfu_mpu_timestamp_descriptor_v.data[i];
		if(atsc3_mmt_mfu_mpu_timestamp_descriptor->packet_id == packet_id) {
            atsc3_mmt_mfu_mpu_timestamp_descriptor_max = atsc3_mmt_mfu_mpu_timestamp_descriptor;
		    if(atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_sequence_number == mpu_sequence_number) {
                __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number_with_last_failsafe: retuning matching: %" PRIu64 " , looking for packet_id: %d, mpu_sequence_number: %d",
                                        atsc3_mmt_mfu_mpu_timestamp_descriptor->mpu_presentation_time_as_us_value,
                                        packet_id,
                                        mpu_sequence_number);
                return atsc3_mmt_mfu_mpu_timestamp_descriptor;
            }
		}
	}

	if(atsc3_mmt_mfu_mpu_timestamp_descriptor_max != NULL) {
//	    //TODO: hack because we lost the MPT clock reference...
        //atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_as_us_value;// += 1000000;
        __MMT_CONTEXT_MPU_DEBUG("atsc3_get_mpu_timestamp_from_packet_id_mpu_sequence_number: retuning fallback max: %" PRIu64 ", looking for packet_id: %d, mpu_sequence_number: %d",
                                atsc3_mmt_mfu_mpu_timestamp_descriptor_max->mpu_presentation_time_as_us_value,
                                packet_id,
                                mpu_sequence_number);
        return atsc3_mmt_mfu_mpu_timestamp_descriptor_max;

    }
//	}

	return NULL;
}



/*
 *
 * jjustman-2020-10-06: NOTE: if you are not calling atsc3_lls_slt_monitor_free, you will need to call BEFORE atsc3_mmt_mfu_context_free
 *

    if(lls_slt_monitor && atsc3_mmt_mfu_context->matching_lls_sls_mmt_session) {
		lls_sls_mmt_session_flows_remove_lls_sls_mmt_session(lls_slt_monitor, &atsc3_mmt_mfu_context->matching_lls_sls_mmt_session);
	}

 */

void atsc3_mmt_mfu_context_free(atsc3_mmt_mfu_context_t** atsc3_mmt_mfu_context_p) {
    if(atsc3_mmt_mfu_context_p) {
        atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = *atsc3_mmt_mfu_context_p;
        if(atsc3_mmt_mfu_context) {
            if(atsc3_mmt_mfu_context->udp_flow) {
                free(atsc3_mmt_mfu_context->udp_flow);
                atsc3_mmt_mfu_context->udp_flow = NULL;
            }

            if(atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container) {
                udp_flow_latest_mpu_sequence_number_container_t_release(atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container);
                atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container = NULL;
            }

            if(atsc3_mmt_mfu_context->mmtp_flow) {
                mmtp_flow_free_mmtp_asset_flow(atsc3_mmt_mfu_context->mmtp_flow);
                free(atsc3_mmt_mfu_context->mmtp_flow);
                atsc3_mmt_mfu_context->mmtp_flow = NULL;
            }

            if(atsc3_mmt_mfu_context->mp_table_last) {
                //jjustman-2020-08-31: todo - free inner impl
                free(atsc3_mmt_mfu_context->mp_table_last);
                atsc3_mmt_mfu_context->mp_table_last = NULL;
            }

            free(atsc3_mmt_mfu_context);
            atsc3_mmt_mfu_context = NULL;
        }
        *atsc3_mmt_mfu_context_p = NULL;
    }
}

mmtp_asset_t* atsc3_mmt_mfu_context_mfu_depacketizer_context_update_find_or_create_mmtp_asset(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, udp_packet_t* udp_packet, lls_slt_monitor_t* lls_slt_monitor, lls_sls_mmt_session_t* matching_lls_sls_mmt_session) {
    mmtp_asset_flow_t* mmtp_asset_flow = NULL;
    mmtp_asset_t* mmtp_asset = NULL;

    //jjustman-2020-12-24 - we need to clone this instance, as udp_packet is transient and udp_flow is an instance field in the struct, not a ptr
    atsc3_mmt_mfu_context->udp_flow = atsc3_udp_flow_clone_from_udp_packet(udp_packet);

    atsc3_mmt_mfu_context->transients.lls_slt_monitor = lls_slt_monitor;
    atsc3_mmt_mfu_context->matching_lls_sls_mmt_session = matching_lls_sls_mmt_session;

    mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(atsc3_mmt_mfu_context->mmtp_flow, udp_packet);
    atsc3_mmt_mfu_context->transients.mmtp_asset_flow = mmtp_asset_flow;

    //jjustman-2020-12-24 - reset context is causing a doublefree here...
    mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);
    atsc3_mmt_mfu_context->transients.mmtp_asset = mmtp_asset;

    return mmtp_asset;
}

mmtp_packet_id_packets_container_t* atsc3_mmt_mfu_context_mfu_depacketizer_update_find_or_create_mmtp_packet_id_packets_container(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmtp_asset_t* mmtp_asset, mmtp_packet_header_t* mmtp_packet_header) {
    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = NULL;

    mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmtp_packet_header(mmtp_asset, mmtp_packet_header);
    atsc3_mmt_mfu_context->mmtp_packet_id_packets_container = mmtp_packet_id_packets_container;

    return mmtp_packet_id_packets_container;
}

/*
 * note: atsc3_mmt_mfu_context->lls_slt_monitor may _not_ relate to a LLS SLS service,
 *          it is assumed that filtering will occur on the udp flow (e.g. ip:port) rather than lls_slt service_id
 */
void mmtp_mfu_process_from_payload_with_context(udp_packet_t *udp_packet, mmtp_mpu_packet_t* mmtp_mpu_packet, atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context) {

    if(mmtp_mpu_packet->mmtp_payload_type != 0x0) {
        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_process_from_payload_with_context: got incorrect payload type of: %d for flow: %d:%d, packet_id: %d, psn: %d", mmtp_mpu_packet->mmtp_payload_type, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->packet_sequence_number);
        atsc3_global_statistics->packet_counter_mmt_unknown++;
        return;
    }

    if(mmtp_mpu_packet->mpu_timed_flag != 0x1) {
        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_process_from_payload_with_context: got incorrect mpu_timed_flag type of: %d for flow: %d:%d, packet_id: %d, mpu_sequence_number: %d, psn: %d,", mmtp_mpu_packet->mpu_timed_flag, udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->packet_sequence_number);
        atsc3_global_statistics->packet_counter_mmt_nontimed_mpu++;
        return;
    }

    atsc3_global_statistics->packet_counter_mmt_mpu++;
    atsc3_global_statistics->packet_counter_mmt_timed_mpu++;

	atsc3_mmt_mfu_context->udp_flow = atsc3_udp_flow_clone_from_udp_packet(udp_packet);

	//borrow from our context
	mmtp_flow_t *mmtp_flow = atsc3_mmt_mfu_context->mmtp_flow;
	lls_slt_monitor_t* lls_slt_monitor = atsc3_mmt_mfu_context->transients.lls_slt_monitor;
	lls_sls_mmt_session_t* matching_lls_sls_mmt_session = atsc3_mmt_mfu_context->matching_lls_sls_mmt_session;
	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container = atsc3_mmt_mfu_context->udp_flow_latest_mpu_sequence_number_container;

    //forward declare so our goto will compile
    mmtp_asset_flow_t* mmtp_asset_flow = NULL;
    mmtp_asset_t* mmtp_asset = NULL;
    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container = NULL;
    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = NULL;

    lls_sls_mmt_monitor_t *matching_lls_sls_mmt_monitor = lls_sls_mmt_monitor_find_from_service_id(lls_slt_monitor, matching_lls_sls_mmt_session->service_id);

    if (!matching_lls_sls_mmt_monitor) {
        //we may not be monitoring this atsc3 service_id, so discard

        __MMT_CONTEXT_MPU_TRACE("mmtp_mfu_process_from_payload_with_context: service_id: %u, packet_id: %u, lls_slt_monitor size: %u, matching_lls_sls_mmt_monitor is NULL!",
                                matching_lls_sls_mmt_session->service_id,
                                mmtp_mpu_packet->mmtp_packet_id,
                                lls_slt_monitor->lls_sls_mmt_monitor_v.count);

        return;
    }

    if (!(matching_lls_sls_mmt_monitor->transients.atsc3_lls_slt_service->service_id == matching_lls_sls_mmt_session->service_id &&
          matching_lls_sls_mmt_session->sls_destination_ip_address == udp_packet->udp_flow.dst_ip_addr &&
          matching_lls_sls_mmt_session->sls_destination_udp_port == udp_packet->udp_flow.dst_port)) {
        __MMT_CONTEXT_MPU_TRACE("mmtp_mfu_process_from_payload_with_context: sls monitor flow: %d:%d, service_id: %d not matching for flow: %d:%d, service_id: %d, packet_id: %d, mpu_sequence_number: %d, psn: %d",
                                matching_lls_sls_mmt_session->sls_destination_ip_address, matching_lls_sls_mmt_session->sls_destination_udp_port, matching_lls_sls_mmt_monitor->transients.atsc3_lls_slt_service->service_id,
                                udp_packet->udp_flow.dst_ip_addr, udp_packet->udp_flow.dst_port, matching_lls_sls_mmt_session->service_id, mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->packet_sequence_number);
        return;
    }

    //assign our mmtp_mpu_packet to asset/packet_id/mpu_sequence_number flow
    mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
    mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);
    mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);
    mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_or_create_mpu_sequence_number_mmtp_mpu_packet_collection_from_mmt_mpu_packet(mmtp_packet_id_packets_container, mmtp_mpu_packet);

    //persist our mmtp_mpu_packet for mpu reconstitution as per original libatsc3 design
    mpu_sequence_number_mmtp_mpu_packet_collection_add_mmtp_mpu_packet(mpu_sequence_number_mmtp_mpu_packet_collection, mmtp_mpu_packet);

    udp_flow_packet_id_mpu_sequence_tuple_t* last_flow_reference = udp_flow_latest_mpu_sequence_number_add_or_replace_and_check_for_rollover(udp_flow_latest_mpu_sequence_number_container, udp_packet, mmtp_mpu_packet, lls_slt_monitor, matching_lls_sls_mmt_session, mmtp_flow);

    //jjustman-2020-11-12 - TODO - fix me to avoid "fixed" mapping of video/audio/stpp packet_id mapping for mpu_sequence_number changes

    //check for rollover for any remaining emissions from our last mpu_sequence tuple, free, and then current mpu_sequence rebuild
    mpu_sequence_number_mmtp_mpu_packet_collection_t* previous_mpu_sequence_number_mmtp_mpu_packet_collection = NULL;
    while((previous_mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_not_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, mmtp_mpu_packet->mpu_sequence_number))) {
        //rebuild any straggler DU's
        mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, mmtp_packet_id_packets_container, previous_mpu_sequence_number_mmtp_mpu_packet_collection->mpu_sequence_number, 0, true);

        //notify context of the sequence number change
        if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change) {
            atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_number_change(atsc3_mmt_mfu_context, mmtp_mpu_packet->mmtp_packet_id, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number, mmtp_mpu_packet->mpu_sequence_number);
        }

        //remove last mpu_sequence_number reference from our packets conntainenr
        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection_non_vector_builder(mmtp_packet_id_packets_container, previous_mpu_sequence_number_mmtp_mpu_packet_collection);

        //udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, last_flow_reference);
    }

    mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context, mmtp_packet_id_packets_container, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->sample_number, false);

    return;
}

/**
 //jjustman-2019-10-23: do not optimisticly emit mmthsample_header du payload, otehrwise we may lose partial MFU emission flush below...
    if(false && mmtp_mpu_packet->mmthsample_header) {
        __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: new MFU.MMTHSample: packet_id: %u, mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, mfu packet size: %u, fragmentation_indicator: %u",
            mmtp_mpu_packet->mmtp_packet_id,
            mmtp_mpu_packet->mpu_sequence_number,
            mmtp_mpu_packet->mmthsample_header->samplenumber,
            mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number,
            mmtp_mpu_packet->mmthsample_header->length,
            mmtp_mpu_packet->du_mfu_block ? mmtp_mpu_packet->du_mfu_block->p_size : 0,
            mmtp_mpu_packet->mpu_fragmentation_indicator);

        //in the case of audio (or video P frame) packets, our du mfu packet size should be equal to the mmthsample_header->length value,
        if(mmtp_mpu_packet->du_mfu_block->p_size == mmtp_mpu_packet->mmthsample_header->length) {
            block_t* du_mfu_block_duplicated_for_context_callback_invocation = block_Duplicate(mmtp_mpu_packet->du_mfu_block);
            atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mmthsample_header->samplenumber, du_mfu_block_duplicated_for_context_callback_invocation, 1);
            block_Destroy(&du_mfu_block_duplicated_for_context_callback_invocation);
        } else if(mmtp_mpu_packet->mpu_fragmentation_indicator == 0x00) {
            //otherwise, check mpu_fragmentation_indicator...only process if we are marked as fi=0x00 (complete data unit)
            //let DU rebuild handle any other packets
            __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_process_from_payload_with_context: mmthsample mismatch with du_mfu_block and frag_indicator==0x00, mpu_fragmentation_indicator == 0x00, but du_mfu_block.size (%u) != mmthsample_header->length (%u), packet_id: %u, mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, mfu packet size: %u, fragmentation_indicator: %u",
                mmtp_mpu_packet->du_mfu_block->p_size,
                mmtp_mpu_packet->mmthsample_header->length,
                mmtp_mpu_packet->mmtp_packet_id,
                mmtp_mpu_packet->mpu_sequence_number,
                mmtp_mpu_packet->mmthsample_header->samplenumber,
                mmtp_mpu_packet->mmthsample_header->movie_fragment_sequence_number,
                mmtp_mpu_packet->mmthsample_header->length,
                mmtp_mpu_packet->du_mfu_block ? mmtp_mpu_packet->du_mfu_block->p_size : 0,
                mmtp_mpu_packet->mpu_fragmentation_indicator);
            
            block_t* du_mfu_block_duplicated_for_context_callback_invocation = block_Duplicate(mmtp_mpu_packet->du_mfu_block);
            atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(mmtp_mpu_packet->mmtp_packet_id, mmtp_mpu_packet->mpu_sequence_number, mmtp_mpu_packet->mmthsample_header->samplenumber, du_mfu_block_duplicated_for_context_callback_invocation, 1);
            block_Destroy(&du_mfu_block_duplicated_for_context_callback_invocation);
        }

    } else {
 */

/*
 mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number
 
    mfu_sample_number_from_current_du may be 0 if:
    flush_all_fragments: set to true when mpu_sequence_number rolls over, otherwise set to false
 

     we only care about:
        mfu's that start with a sample header (and fi=0): mmtp_mpu_packet->mmthsample_header
     
        otherwise, check and recon if necessary:
            mmtp_mpu_packet->mpu_fragment_type=0x0 - mpu_metadata
            mmtp_mpu_packet->mpu_fragment_type=0x2 - mfu
 
 //todo: jjustman-2019-10-03 - how to handle missing first NAL or partial DU's?

*/
void mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number(atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context, mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container, uint32_t mpu_sequence_number, uint32_t mfu_sample_number_from_current_du, bool flush_all_fragments) {

    block_t* du_mpu_metadata_block_rebuilt = NULL;      //MPU_fragment_type == 0x0 - for MPU metadata (e.g. ftyp/mmpu/moov)
    block_t* du_mfu_block_to_rebuild = NULL;            //MPU_fragment_type == 0x2 - for MFU's in OOO mode (e.g. sample data)

    block_t* du_movie_fragment_block_rebuilt = NULL;    //MPU_fragment_type == 0x1 - lastly, the movie fragment metadata (e.g. TRUN) which we don't care about as we parse the MMTHSampleHeader

    int du_mfu_block_rebuild_index_start = -1;
    int32_t mfu_fragment_counter_mmthsample_header_start = 0;
    int32_t mfu_fragment_counter_missing_mmthsample_header_start = 0;
    int32_t mfu_fragment_counter_position = 0;
    int32_t mfu_fragment_count_rebuilt = 0;

    if(!atsc3_mmt_mfu_context) {
        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, atsc3_mmt_mfu_context was NULL, mpu_sequence_number: %d",
                               mpu_sequence_number);
        return;
    }

    if(!mmtp_packet_id_packets_container) {
        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, mmtp_packet_id_packets_container was NULL, mpu_sequence_number: %d",
                               mpu_sequence_number);
        return;
    }

    /* start by attempting to re-assemble our MPU Metadata (mpu_fragment_type == 0x0) */
    mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, mpu_sequence_number);
    if(!mpu_sequence_number_mmtp_mpu_packet_collection) {
        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number returned NULL for packet_id: %d, mpu_sequence_number: %d",
                               mmtp_packet_id_packets_container->packet_id,
                               mpu_sequence_number);
        return;
    }

    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t *mmtp_mpu_init_packet_to_rebuild = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        if(!mmtp_mpu_init_packet_to_rebuild) {
            __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, mmtp_mpu_init_packet_to_rebuild at i: %d was NULL for packet_id: %d, mpu_sequence_number: %d", i, mmtp_packet_id_packets_container->packet_id, mpu_sequence_number);
            continue;
        }

        if (mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed) {
            continue;
        }

        //mpu_fragment_type: 0x0 -> MPU metadata
        //process mpu_metadata, don't send incomplete payloads for init box (e.g. isnt FI==0x00 or endns in 0x03)
        if (mmtp_mpu_init_packet_to_rebuild->mpu_fragment_type == 0x0) {
            //mark this DU as completed for purging at the end of this method

            if (!mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block) {
                __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: MPU Metadata, missing du_mpu_metadata_block i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                        i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                        atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                        atsc3_mmt_mfu_context->udp_flow->dst_port,
                        mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                        mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                        mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                continue;
            }

            __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: MPU Metadata: init box: packet_id: %u, mpu_sequence_number: %u, mmtp_mpu_packet.samplenumber: %u, mmtp_mpu_packet.mpu_fragment_counter: %u, mmtp_mpu_packet.offset: %u, du_mpu_metadata_block packet size: %u, fragmentation_indicator: %u",
                    mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                    mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                    mmtp_mpu_init_packet_to_rebuild->sample_number,
                    mmtp_mpu_init_packet_to_rebuild->mpu_fragment_counter,
                    mmtp_mpu_init_packet_to_rebuild->offset,
                    mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block ? mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block->p_size : 0,
                    mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);

            //mpu_fragmentation_indicator: 0x00 -> payload contains one or more complete data units
            //one (or more) DU, so mark this as reassembeled, and send this off immediately,
            if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x00) {
                mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed = true;

                block_t *du_mpu_metadata_block_duplicated_for_context_callback_invocation = block_Duplicate(mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block);
                block_Rewind(du_mpu_metadata_block_duplicated_for_context_callback_invocation);
                if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present) {
                    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present(atsc3_mmt_mfu_context, mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number, du_mpu_metadata_block_duplicated_for_context_callback_invocation);
                }
                block_Destroy(&du_mpu_metadata_block_duplicated_for_context_callback_invocation);
            } else if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x03) {
                //mpu_fragmentation_indicator: 0x03 -> payload contains the last fragment of data unit
                //only if we have reached the last fragment of the data unit, and we have a du_mpu_metadata_block_rebuilt, send if off,
                if (du_mpu_metadata_block_rebuilt != NULL &&
                    mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block) {
                    mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed = true;

                    __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: Appending MPU Metadata, i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                            i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                            atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                            atsc3_mmt_mfu_context->udp_flow->dst_port,
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);

                    block_Merge(du_mpu_metadata_block_rebuilt, mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block);
                    block_Rewind(du_mpu_metadata_block_rebuilt);
                    if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_mpu_metadata_present(atsc3_mmt_mfu_context, mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number, du_mpu_metadata_block_rebuilt);
                    }
                    block_Destroy(&du_mpu_metadata_block_rebuilt);
                } else {
                    __MMT_CONTEXT_MPU_WARN( "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: missing proceeding MPU Metadata i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                            i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                            atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                            atsc3_mmt_mfu_context->udp_flow->dst_port,
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                }
            } else {
                //mpu_fragmentation_indicator: 0x01 -> paylaod contains the first fragment of a data unit
                //otherwise, prepare our first data unit, clearing out any (possible) duplicate starting du -
                if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x01) {
                    if (du_mpu_metadata_block_rebuilt) {
                        block_Destroy(&du_mpu_metadata_block_rebuilt);
                    }
                    du_mpu_metadata_block_rebuilt = block_Duplicate(mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block);
                } else if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x02) {
                    //mpu_fragmentation_indicator: 0x02 -> payload contains a fragment of a data unit that is neither the first or the last part
                    //so we MUST have a du_mpu_metadata_block_rebuilt pending
                    if (du_mpu_metadata_block_rebuilt) {
                        block_Merge(du_mpu_metadata_block_rebuilt, mmtp_mpu_init_packet_to_rebuild->du_mpu_metadata_block);
                    } else {
                        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number Missing initial MPU Metadata i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                atsc3_mmt_mfu_context->udp_flow->dst_port,
                                mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                                mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                                mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                    }
                }
            }
        }
    }
    //jjustman-2020-10-13 - clean up our MPU metadata rebuilt block if we have created one
    if(du_mpu_metadata_block_rebuilt) {
        block_Destroy(&du_mpu_metadata_block_rebuilt);
    }

    //now, begin processing our MFU's

    //make sure our DU for this is set to i_pos == p_size, so we can opportunisticly alloc and make sure block_AppendFull works properly
    //jjustman-2019-10-24: todo - fix me! mmtp_mpu_packet_to_rebuild->du_mfu_block->i_pos = mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size;

    //regardless of mpu_fragmentation_indicator== as it may be lost in emission... and compute relative DU offset for rebuilding MFU
    //first DU in MFU should contain MMTHSample, but may not if its a lost DU
    //last DU in MFU should contain mpu_fragment_counter == 0, , but may not if its a lost DU

    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t *mmtp_mpu_packet_to_rebuild = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        if(!mmtp_mpu_packet_to_rebuild) {
            __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, mmtp_mpu_packet_to_rebuild at i: %d was NULL for packet_id: %d, mpu_sequence_number: %d", i, mmtp_packet_id_packets_container->packet_id, mpu_sequence_number);
            continue;
        }

        if (mmtp_mpu_packet_to_rebuild->mfu_reassembly_performed) {
            continue;
        }

        //mpu_fragment_type: 0x2 -> MFU - sample or subsample of timed media data
        //only rebuild MFU DU's here
        if(mmtp_mpu_packet_to_rebuild->mpu_fragment_type == 0x2) {

            //MFU rebuild any pending packets less than our sample_number and create a dummy block of len 0
            if (!mmtp_mpu_packet_to_rebuild->du_mfu_block) {
                __MMT_CONTEXT_MPU_WARN("MFU: missing du_mfu_block! creating dummy 0 byte block: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                                                i, mmtp_mpu_packet_to_rebuild->packet_sequence_number,
                                                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                                                atsc3_mmt_mfu_context->udp_flow->dst_port,
                                                                mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                                                mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                                                mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);
                mmtp_mpu_packet_to_rebuild->du_mfu_block = block_Alloc(0);
            }

            //mpu_fragmentation_indicator: 0x00 -> payload contains one or more complete data units
            if(mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator == 0x00) {
                //otherwise, check mpu_fragmentation_indicator...only process if we are marked as fi=0x00 (complete data unit)
                //let DU rebuild handle any other packets

//                __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: mpu_fragmentation_indicator==0x00,  du_mfu_block.size: %u, mmthsample_header->length: %u, packet_id: %u, mpu_sequence_number: %u, mmthsample.samplenumber: %u, mmthsample.movie_fragment_sequence_number: %u, mmthsample.length: %u, fragmentation_indicator: %u",
//                                        mmtp_mpu_packet_to_rebuild->du_mfu_block ? mmtp_mpu_packet_to_rebuild->du_mfu_block->p_size : 0,
//                                        mmtp_mpu_packet_to_rebuild->mmthsample_header ? mmtp_mpu_packet_to_rebuild->mmthsample_header->length : 0,
//                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
//                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
//                                        mmtp_mpu_packet_to_rebuild->mmthsample_header->samplenumber,
//                                        mmtp_mpu_packet_to_rebuild->mmthsample_header->movie_fragment_sequence_number,
//                                        mmtp_mpu_packet_to_rebuild->mmthsample_header->length,
//                                        mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator);

                block_Rewind(mmtp_mpu_packet_to_rebuild->du_mfu_block);
                block_t* du_mfu_block_duplicated_for_context_callback_invocation = block_Duplicate(mmtp_mpu_packet_to_rebuild->du_mfu_block);
                if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete) {
                    atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(atsc3_mmt_mfu_context, mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mmtp_timestamp, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, du_mfu_block_duplicated_for_context_callback_invocation, 1);
                }
                mmtp_mpu_packet_to_rebuild->mfu_reassembly_performed = true;
                block_Destroy(&du_mfu_block_duplicated_for_context_callback_invocation);

                continue;
            }

            int mmtp_mpu_starting_index = i;
            int mmtp_mpu_ending_index = -1;

            //otherwise, walk thru a consecutive set of mfu samples to rebuild
            for(int j=mmtp_mpu_starting_index; j < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count && mmtp_mpu_ending_index == -1; j++) {
                mmtp_mpu_packet_t *mmtp_mpu_packet_to_rebuild_next = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[j];
                if(!mmtp_mpu_packet_to_rebuild_next) {
                    __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, mmtp_mpu_packet_to_rebuild_next at j: %d was NULL for packet_id: %d, mpu_sequence_number: %d", j, mmtp_packet_id_packets_container->packet_id, mpu_sequence_number);
                    continue;
                }
                if (mmtp_mpu_packet_to_rebuild->sample_number != mmtp_mpu_packet_to_rebuild_next->sample_number) {
                    mmtp_mpu_ending_index = j;
                }
            }
            //handle force flush, <-1
            if(mmtp_mpu_ending_index == -1 && flush_all_fragments) {
                mmtp_mpu_ending_index = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count;
            } else if(mmtp_mpu_ending_index == -1) {
                continue;
            }

            //todo - compute gap here between sample_numbers for tracking our MFU loss
            if(mmtp_mpu_ending_index == -1) {
                //exit from loop
                __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: exiting loop from mmtp_mpu_starting_index: %d, mmtp_mpu_ending_index: %d, count: %d, flush_all_fragments: %d, packet_id: %u, mpu_sequence_number: %u",
                                        mmtp_mpu_starting_index, mmtp_mpu_ending_index,
                                        mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count,
                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                        flush_all_fragments);
                i = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count;
                continue;
            } else {
                __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: iterating from mmtp_mpu_starting_index: %d, mmtp_mpu_ending_index: %d, count: %d, flush_all_fragments: %d, packet_id: %u, mpu_sequence_number: %u",
                                        mmtp_mpu_starting_index, mmtp_mpu_ending_index,
                                        mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count,
                                        mmtp_mpu_packet_to_rebuild->mmtp_packet_id,
                                        mmtp_mpu_packet_to_rebuild->mpu_sequence_number,
                                        flush_all_fragments);
            }

            //rebuild here as we have rolled over to the next sample number
            mfu_fragment_counter_mmthsample_header_start = 0;
            mfu_fragment_counter_missing_mmthsample_header_start = 0;
            mfu_fragment_count_rebuilt = 0;
            uint32_t mfu_mmth_sample_header_size_to_shift_offset = 0;

            //clear out our in-flight du_mfu_block_to_rebuild
            //jjustman-2020-10-13 - todo - sanity check, we should never have to do this unless we were able to recover missing DU's via FEC (e.g. RaptorQ)
            if(du_mfu_block_to_rebuild) {
                block_Destroy(&du_mfu_block_to_rebuild);
            }

            for(int j=mmtp_mpu_starting_index; j < mmtp_mpu_ending_index; j++) {
                mmtp_mpu_packet_t *mmtp_mpu_packet_to_rebuild_from_du = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[j];
                if(!mmtp_mpu_packet_to_rebuild_from_du) {
                    __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du at j: %d was NULL for packet_id: %d, mpu_sequence_number: %d", j, mmtp_packet_id_packets_container->packet_id, mpu_sequence_number);
                    continue;
                }
                if(!mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block) {
                    continue;
                }
                mmtp_mpu_packet_to_rebuild_from_du->mfu_reassembly_performed = true;
                mfu_fragment_count_rebuilt++;

                //__MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number:\trebuilding from packet_id: %u, mpu_sequence_number: %u, sample_number: %d, fragment: %d, du_mfu_size: %d, offset: %d",
                //                        mmtp_mpu_packet_to_rebuild_from_du->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_from_du->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du->sample_number, mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->p_size, mmtp_mpu_packet_to_rebuild_from_du->offset);

                //for single MFU (0x00) and first DU (0x01) of MFU, process the MMTHSampleHeader
                if(mmtp_mpu_packet_to_rebuild_from_du->mpu_fragmentation_indicator == 0x00 || mmtp_mpu_packet_to_rebuild_from_du->mpu_fragmentation_indicator == 0x01) {
                    mfu_fragment_counter_position = mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter;

                    //jjustman-2020-11-17 - TODO: mmtp_mpu_packet_to_rebuild_from_du->mpu_fragmentation_indicator == 0x00  should not block_alloc based upon mmthsample_header, as we are a single MMTP du length
                    if(mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header && mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->length) {
                        mfu_fragment_counter_mmthsample_header_start = mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter;

                        mfu_mmth_sample_header_size_to_shift_offset = mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->mfu_mmth_sample_header_size;

                        //use the original mmthsample_length, only shift the offset of the non-prefixed fragments
                        du_mfu_block_to_rebuild = block_Alloc(mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->length); //- mfu_mmth_sample_header_size_to_shift_offset);
                        //__MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number:\tblock_alloc\tto\tsize:\t%d\t(mmthsample_header->length),\tmfu_mmth_sample_header_size,\tfrom\tpacket_id:\t%u,\tmpu_sequence_number:\t%u,\tsample_number:\t%d,\tfragment:\t%d,\tdu_mfu_size:\t%d",
                        //                         mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->length, mmtp_mpu_packet_to_rebuild_from_du->mmthsample_header->mfu_mmth_sample_header_size, mmtp_mpu_packet_to_rebuild_from_du->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_from_du->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du->sample_number, mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block->p_size);

                        block_AppendFull(du_mfu_block_to_rebuild, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);
                    } else {
                        //weird, we should ALWAYS have a MMTHSampleHeader, but just in case, use our full du_mfu block as a fallback
                        du_mfu_block_to_rebuild = block_Duplicate(mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);
                        mfu_fragment_counter_missing_mmthsample_header_start = mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter;
                    }
                } else {
                    //for neither-first-nor-last 0x2 (10)  and last fragment 0x3 (11) of the data unit

                    //we should have a workable du_mfu_block_to_rebuild, but if not, clone from our current DU and mark this as missing the MMTHSampleHeader

                    if(!du_mfu_block_to_rebuild) {

                        du_mfu_block_to_rebuild = block_Duplicate(mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);
                        mfu_fragment_counter_missing_mmthsample_header_start = mmtp_mpu_packet_to_rebuild_from_du->mpu_fragment_counter;
                    } else {
                        //jjustman-2019-10-25 - If we have a GAP between MFU sample DU fragments, we can't just append with a pre-allocated block (e.g N null bytes) to meet our MMTHSampleHeader expectations for length.
                        // we will need truncate any proceeding open NAL's, and discard any preceeding NAL's that are already open
                        // or develop a more robust missing DU strategy for error concealment - hevc doesn't like large null blocks when its expecting nals...
                        block_AppendFull(du_mfu_block_to_rebuild, mmtp_mpu_packet_to_rebuild_from_du->du_mfu_block);

                        //__MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number:\tafter block_appendFull: packet_id: %u, mpu_sequence_number: %u, sample_number: %d, offset: %d, du_mfu_block_to_rebuild: %p, pos: %d, size: %d",
                        //mmtp_mpu_packet_to_rebuild_from_du->mmtp_packet_id, mmtp_mpu_packet_to_rebuild_from_du->mpu_sequence_number, mmtp_mpu_packet_to_rebuild_from_du->sample_number, mmtp_mpu_packet_to_rebuild_from_du->offset, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild->i_pos, du_mfu_block_to_rebuild->p_size);
                    }
                }
            }

            __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number before callbacks for: packet_id: %u, mpu_sequence_number: %u, sample_number: %u, fragment_counter: %u, psn: %u, du_mfu_block_to_rebuild: %p, pos: %d, p_size: %u",
                    mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->i_pos : -1, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : -1);

            //todo: impl meta cases for corrupt/missing samples/packets, etc

            //jjustman-2020-11-11 - fix this check
            if(du_mfu_block_to_rebuild) {
                //if we were from alloc, trim off the null tail from i_ptr
                if(block_IsAlloc(du_mfu_block_to_rebuild)) {
                    if(du_mfu_block_to_rebuild->i_pos > du_mfu_block_to_rebuild->p_size) {
                        //this is bad, should never happen but lets clamp anyways
                        __MMT_CONTEXT_MPU_ERROR("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, du_mfu_block_to_rebuild is past size, du_mfu_block_to_rebuild->i_pos: %d > du_mfu_block_to_rebuild->p_size: %d, packet_id: %u, mpu_sequence_number: %u, sample_number: %u, fragment_counter: %u, psn: %u, du_mfu_block_to_rebuild: %p, pos: %d, p_size: %u",
                                                du_mfu_block_to_rebuild->i_pos,
                                                du_mfu_block_to_rebuild->p_size,
                                                mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->i_pos : -1, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : -1);
                        du_mfu_block_to_rebuild->i_pos = du_mfu_block_to_rebuild->p_size;
                    } else {
                        du_mfu_block_to_rebuild->p_size = du_mfu_block_to_rebuild->i_pos;
                    }
                }

                //if we have our MMTHSampleHeader, then mfu_fragment_counter_mmthsample_header_start should tell us how many DU's we need to be completely recovered for this MFU
                if (mfu_fragment_counter_mmthsample_header_start) {
                    if (mfu_fragment_counter_mmthsample_header_start - (mfu_fragment_count_rebuilt - 1) == 0) {
                        //REQUIRED
                        if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete) {
                            //jjustman-2021-01-19 - todo: use the mmtp_timestamp from the first packet emission, not the "last"
                            atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_complete(atsc3_mmt_mfu_context, mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mmtp_timestamp, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, du_mfu_block_to_rebuild, mfu_fragment_count_rebuilt);
                        }
                        //__MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number MFU DU with MMTHSample, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, du_mfu_block_to_rebuild: %p, ->p_size: %u, mfu_fragment_counter_mmthsample_header_start: %d, mfu_fragment_count_rebuilt: %d",
                        //       mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : 0, mfu_fragment_counter_mmthsample_header_start, mfu_fragment_count_rebuilt);
                    } else {
                        //OPTIONAL
                        if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt) {
                            atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt(atsc3_mmt_mfu_context, mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mmtp_timestamp, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, du_mfu_block_to_rebuild, mfu_fragment_counter_mmthsample_header_start, mfu_fragment_count_rebuilt);
                        }
                        __MMT_CONTEXT_MPU_INFO("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number MFU DU with corrupt payload, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u,  du_mfu_block_to_rebuild: %p, du_mfu_block_to_rebuild->p_size: %u, mfu_fragment_counter_mmthsample_header_start: %d, mfu_fragment_count_rebuilt: %d", mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : 0, mfu_fragment_counter_mmthsample_header_start, mfu_fragment_count_rebuilt);
                    }
                } else {
                    //OPTIONAL
                    if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_mfu_on_sample_corrupt_mmthsample_header(atsc3_mmt_mfu_context, mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mmtp_timestamp, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, du_mfu_block_to_rebuild, mfu_fragment_counter_missing_mmthsample_header_start, mfu_fragment_count_rebuilt);
                    }
                    __MMT_CONTEXT_MPU_DEBUG("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number MFU DU with missing mmthsample header, packet_id: %u, mpu_sequence_number: %u, building emission: sample_number: %u, fragment_counter: %u, psn: %u, flow with %u:%u, fi: %u, du_mfu_block_to_rebuild: %p, du_mfu_block_to_rebuild->p_size: %u", mmtp_mpu_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_packet_to_rebuild->mpu_sequence_number, mmtp_mpu_packet_to_rebuild->sample_number, mmtp_mpu_packet_to_rebuild->mpu_fragment_counter, mmtp_mpu_packet_to_rebuild->packet_sequence_number, atsc3_mmt_mfu_context->udp_flow->dst_ip_addr, atsc3_mmt_mfu_context->udp_flow->dst_port, mmtp_mpu_packet_to_rebuild->mpu_fragmentation_indicator, du_mfu_block_to_rebuild, du_mfu_block_to_rebuild ? du_mfu_block_to_rebuild->p_size : 0);
                }
                block_Destroy(&du_mfu_block_to_rebuild);
            }

            //iterate our parent loop forward
            i = mmtp_mpu_ending_index;
        }
    }

    //jjustman-2019-10-29 - in the spirit of OOO MFU, process the movie fragment metadata as a last resort to extract the sample duration until mmt_atsc3_message support is functional

    for(int i=0; i < mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.count; i++) {
        mmtp_mpu_packet_t *mmtp_mpu_init_packet_to_rebuild = mpu_sequence_number_mmtp_mpu_packet_collection->mmtp_mpu_packet_v.data[i];

        if(!mmtp_mpu_init_packet_to_rebuild) {
            __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number, mmtp_mpu_init_packet_to_rebuild at i: %d was NULL for packet_id: %d, mpu_sequence_number: %d", i, mmtp_packet_id_packets_container->packet_id, mpu_sequence_number);
            continue;
        }

        if (mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed) {
            continue;
        }
        //process movie fragment metadata, don't send incomplete payloads for moof box (e.g. isnt FI==0x00 or ends in 0x03)
        if (mmtp_mpu_init_packet_to_rebuild->mpu_fragment_type == 0x1) {
            //mark this DU as completed for purging at the end of this method

            if (!mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block) {
                __MMT_CONTEXT_MPU_WARN(
                        "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: mmtp_mpu_init_packet_to_rebuild: movie fragment metadata Metadata, missing du_movie_fragment_block i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                        i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                        atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                        atsc3_mmt_mfu_context->udp_flow->dst_port,
                        mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                        mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                        mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                continue;
            }

            __MMT_CONTEXT_MPU_DEBUG(
                    "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: Movie fragment metadata: moof: packet_id: %u, mpu_sequence_number: %u, mmtp_mpu_packet.samplenumber: %u, mmtp_mpu_packet.mpu_fragment_counter: %u, mmtp_mpu_packet.offset: %u, du_mpu_metadata_block packet size: %u, fragmentation_indicator: %u",
                    mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                    mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                    mmtp_mpu_init_packet_to_rebuild->sample_number,
                    mmtp_mpu_init_packet_to_rebuild->mpu_fragment_counter,
                    mmtp_mpu_init_packet_to_rebuild->offset,
                    mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block ? mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block->p_size : 0,
                    mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);

            //one (or more) complete DU, so send it off
            if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x00) {
                mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed = true;

                block_t *du_movie_fragment_block_duplicated_for_context_callback_invocation = block_Duplicate(mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block);
                block_Rewind(du_movie_fragment_block_duplicated_for_context_callback_invocation);
                if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present) {
                    atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present(atsc3_mmt_mfu_context, mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number, du_movie_fragment_block_duplicated_for_context_callback_invocation);
                }
                block_Destroy(&du_movie_fragment_block_duplicated_for_context_callback_invocation);
            } else if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x03) {
                //ending fragment of movie fragment metadata, so send it off..
                if (du_movie_fragment_block_rebuilt != NULL && mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block) {
                    mmtp_mpu_init_packet_to_rebuild->mfu_reassembly_performed = true;

                    __MMT_CONTEXT_MPU_DEBUG( "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: Appending movie fragment metadata, i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                            i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                            atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                            atsc3_mmt_mfu_context->udp_flow->dst_port,
                            mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                            mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                            mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);

                    block_Merge(du_movie_fragment_block_rebuilt, mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block);
                    block_Rewind(du_movie_fragment_block_rebuilt);

                    //jjustman-2020-10-13 - fix: was calling atsc3_mmt_mpu_on_sequence_mpu_metadata_present, but
                    // we are rebuilding the movie_fragment metadata, thus we should be invoking atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present
                    if(atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present) {
                        atsc3_mmt_mfu_context->atsc3_mmt_mpu_on_sequence_movie_fragment_metadata_present(atsc3_mmt_mfu_context, mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id, mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number, du_movie_fragment_block_rebuilt);
                    }
                    block_Destroy(&du_movie_fragment_block_rebuilt);
                } else {
                    //can't send if off if we don't have our block_rebuilt or are missing our du_movie_fragment_block for this DU
                    if(!mmtp_mpu_init_packet_to_rebuild->mmtp_mpu_init_packet_missing_du_movie_fragment_block_warning_logged) {
                        __MMT_CONTEXT_MPU_WARN("mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: Missing proceeding movie fragment metadata i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                atsc3_mmt_mfu_context->udp_flow->dst_port,
                                mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                                mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                                mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                            mmtp_mpu_init_packet_to_rebuild->mmtp_mpu_init_packet_missing_du_movie_fragment_block_warning_logged = true;
                    } else {
                        //noop
                    }
                }
            } else {
                //first fragment of DU, so clear out any (improper) du_movie_fragment_block_rebuilt that may be garbage
                if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x01) {
                    if (du_movie_fragment_block_rebuilt) {
                        block_Destroy(&du_movie_fragment_block_rebuilt);
                    }
                    du_movie_fragment_block_rebuilt = block_Duplicate(mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block);
                } else if (mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator == 0x02) {
                    //neither-first-nor-last fragment of DU, we must have a pending du_movie_fragment_block_rebuilt that may be garbage

                    if (du_movie_fragment_block_rebuilt) {
                        block_Merge(du_movie_fragment_block_rebuilt, mmtp_mpu_init_packet_to_rebuild->du_movie_fragment_block);
                    } else {
                        __MMT_CONTEXT_MPU_WARN(
                                "mmtp_mfu_rebuild_from_packet_id_mpu_sequence_number: Missing initial movie fragment metadata block_t: du_movie_fragment_block_rebuilt: i: %u, psn: %u, with %u:%u and packet_id: %u, mpu_sequence_number: %u, fragment_indicator: %u",
                                i, mmtp_mpu_init_packet_to_rebuild->packet_sequence_number,
                                atsc3_mmt_mfu_context->udp_flow->dst_ip_addr,
                                atsc3_mmt_mfu_context->udp_flow->dst_port,
                                mmtp_mpu_init_packet_to_rebuild->mmtp_packet_id,
                                mmtp_mpu_init_packet_to_rebuild->mpu_sequence_number,
                                mmtp_mpu_init_packet_to_rebuild->mpu_fragmentation_indicator);
                    }
                }
            }
        }
    }

    //jjustman-2020-10-13 - cleanup, just to be sure

    if(du_mpu_metadata_block_rebuilt) {
        block_Destroy(&du_mpu_metadata_block_rebuilt);
    }
    if(du_mfu_block_to_rebuild) {
        block_Destroy(&du_mfu_block_to_rebuild);
    }
    if(du_movie_fragment_block_rebuilt) {
        block_Destroy(&du_movie_fragment_block_rebuilt);
    }
}


/*
 * jjustman-2019-10-03: todo: filter by:
 * 	    ignore atsc3_mmt_mfu_context->lls_slt_monitor (or mmtp_flow, matching_lls_sls_mmt_session)
 *
 * 	NOTE: if adding a new fourcc asset_type for context callbacks, make sure it is also added
 * 		   in atsc3_mmt_signalling_message.c/mmt_signalling_message_update_lls_sls_mmt_session
 *
 */

void mmt_signalling_message_dispatch_context_notification_callbacks(udp_packet_t *udp_packet, mmtp_signalling_packet_t* mmtp_signalling_packet, atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context) {
	atsc3_mmt_mfu_context->udp_flow = atsc3_udp_flow_clone_from_udp_packet(udp_packet);

	for(int i=0; i < mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.count; i++) {
		mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmtp_signalling_packet->mmt_signalling_message_header_and_payload_v.data[i];

		//MPT_message, mp_table, dispatch either complete or subset of MPT_message table
		if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MPT_message) {
			mp_table_t* mp_table = &mmt_signalling_message_header_and_payload->message_payload.mp_table;

			//dispatched when message_id >= 0x11 (17) && message_id <= 0x19 (31)
			if(mmt_signalling_message_header_and_payload->message_header.message_id >= MPT_message_start && mmt_signalling_message_header_and_payload->message_header.message_id < MPT_message_end) {
				__MMSM_TRACE("mmt_signalling_message_dispatch_context_notification_callbacks: partial mp_table, message_id: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
				if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_subset) {
				    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_subset(atsc3_mmt_mfu_context, mp_table);
				}

			} else if(mmt_signalling_message_header_and_payload->message_header.message_id == MPT_message_end) {
				__MMSM_TRACE("mmt_signalling_message_dispatch_context_notification_callbacks: complete mp_table, message_id: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
                if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete) {
                    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_mp_table_complete(atsc3_mmt_mfu_context, mp_table);
                }
			} else {
				__MMSM_ERROR("mmt_signalling_message_dispatch_context_notification_callbacks: MESSAGE_id_type == MPT_message but message_id not in bounds: 0x%02x", mmt_signalling_message_header_and_payload->message_header.message_id);
			}

			if(mp_table->number_of_assets) {
				for(int i=0; i < mp_table->number_of_assets; i++) {
					//slight hack, check the asset types and default_asset = 1
					mp_table_asset_row_t* mp_table_asset_row = &mp_table->mp_table_asset_row[i];

					uint32_t* mpu_sequence_number_p 			 = NULL;
					uint64_t* mpu_presentation_time_ntp64_p 	 = NULL;
					uint32_t  mpu_presentation_time_seconds 	 = 0;
					uint32_t  mpu_presentation_time_microseconds = 0;

					//try and extract mpu_presentation_timestamp first
					if(mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor && mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n) {
						__MMSM_DEBUG("MPT message: checking packet_id: %u, mmt_signalling_message_mpu_timestamp_descriptor count is: %u",
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n);

						for(int l=0; l < mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple_n; l++) {
							mmt_signalling_message_mpu_tuple_t* mmt_signaling_message_mpu_tuple = &mp_table_asset_row->mmt_signalling_message_mpu_timestamp_descriptor->mpu_tuple[l];

							mpu_sequence_number_p = &mmt_signaling_message_mpu_tuple->mpu_sequence_number;
							mpu_presentation_time_ntp64_p = &mmt_signaling_message_mpu_tuple->mpu_presentation_time;

							compute_ntp64_to_seconds_microseconds(mmt_signaling_message_mpu_tuple->mpu_presentation_time, &mpu_presentation_time_seconds, &mpu_presentation_time_microseconds);

							__MMSM_DEBUG("packet_id: %u, mpu_sequence_number: %u to mpu_presentation_time: %" PRIu64 ", seconds: %u, ms: %u",
									mp_table_asset_row->mmt_general_location_info.packet_id,
									mmt_signaling_message_mpu_tuple->mpu_sequence_number,
									mmt_signaling_message_mpu_tuple->mpu_presentation_time,
									mpu_presentation_time_seconds,
									mpu_presentation_time_microseconds);
						}
					}

					//resolve packet_id's to matching essence types
					__MMSM_DEBUG("atsc3_mmt_context_mfu_depacketizer: MPT message: checking packet_id: %u, asset_type: %s, default: %u, identifier: %s", mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row->asset_type, mp_table_asset_row->default_asset_flag, mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					//mp_table_asset_row->asset_type == HEVC or H264
					if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID, mp_table_asset_row->asset_type, 4) == 0 ||
					    strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_H264_ID, mp_table_asset_row->asset_type, 4) == 0) {

					    if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id) {
                            atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_essence_packet_id(atsc3_mmt_mfu_context, mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row);
                        }
						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_internal(atsc3_mmt_mfu_context,
																																		   mp_table_asset_row->mmt_general_location_info.packet_id,
																																		   mmtp_signalling_packet->mmtp_timestamp,
																																		   *mpu_sequence_number_p,
																																		   *mpu_presentation_time_ntp64_p,
																																		   mpu_presentation_time_seconds,
																																		   mpu_presentation_time_microseconds);

							if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor) {
                                atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_video_packet_id_with_mpu_timestamp_descriptor(atsc3_mmt_mfu_context,
                                                                                                                                         mp_table_asset_row->mmt_general_location_info.packet_id,
                                                                                                                                         *mpu_sequence_number_p,
                                                                                                                                         *mpu_presentation_time_ntp64_p,
                                                                                                                                         mpu_presentation_time_seconds,
                                                                                                                                         mpu_presentation_time_microseconds);
                            }
						}
						__MMSM_DEBUG("MPT message: mmtp_flow: %p, setting video_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
								atsc3_mmt_mfu_context->mmtp_flow,
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->asset_type,
								mp_table_asset_row->default_asset_flag,
								mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					} else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_AC_4_ID, mp_table_asset_row->asset_type, 4) == 0 ||
					          strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MHM1_ID, mp_table_asset_row->asset_type, 4) == 0 ||
                              strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MHM2_ID, mp_table_asset_row->asset_type, 4) == 0 ||
                              strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_MP4A_ID, mp_table_asset_row->asset_type, 4) == 0) {

						//mp_table_asset_row->asset_type ==  MP4A || AC-4
						if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id) {
                            atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_essence_packet_id(atsc3_mmt_mfu_context, mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row);
                        }

						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_internal(atsc3_mmt_mfu_context,
																																		   mp_table_asset_row->mmt_general_location_info.packet_id,
																																		   mmtp_signalling_packet->mmtp_timestamp,
																																		   *mpu_sequence_number_p,
																																		   *mpu_presentation_time_ntp64_p,
																																		   mpu_presentation_time_seconds,
																																		   mpu_presentation_time_microseconds);


							if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor) {
                                atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_audio_packet_id_with_mpu_timestamp_descriptor(atsc3_mmt_mfu_context, mp_table_asset_row->mmt_general_location_info.packet_id,
                                                                                                                                         *mpu_sequence_number_p,
                                                                                                                                         *mpu_presentation_time_ntp64_p,
                                                                                                                                         mpu_presentation_time_seconds,
                                                                                                                                         mpu_presentation_time_microseconds);
                            }
						}
						//matching_lls_sls_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						__MMSM_DEBUG("MPT message: mmtp_flow: %p, setting audio_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
								atsc3_mmt_mfu_context->mmtp_flow,
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->asset_type,
								mp_table_asset_row->default_asset_flag,
								mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					} else if(strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_IMSC1_ID, mp_table_asset_row->asset_type, 4) == 0) {
					    if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_essence_packet_id) {
                            atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_essence_packet_id(atsc3_mmt_mfu_context, mp_table_asset_row->mmt_general_location_info.packet_id, mp_table_asset_row);
                        }

						if(mpu_sequence_number_p && mpu_presentation_time_ntp64_p) {
							atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_packet_id_with_mpu_timestamp_descriptor_internal(atsc3_mmt_mfu_context,
																																				 mp_table_asset_row->mmt_general_location_info.packet_id,
                                                                                                                                                 mmtp_signalling_packet->mmtp_timestamp,
                                                                                                                                                 *mpu_sequence_number_p,
																																				 *mpu_presentation_time_ntp64_p,
																																				 mpu_presentation_time_seconds,
																																				 mpu_presentation_time_microseconds);
                            if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor) {
                                atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_stpp_packet_id_with_mpu_timestamp_descriptor(atsc3_mmt_mfu_context, mp_table_asset_row->mmt_general_location_info.packet_id,
                                                                                                                                        *mpu_sequence_number_p,
                                                                                                                                        *mpu_presentation_time_ntp64_p,
                                                                                                                                        mpu_presentation_time_seconds,
                                                                                                                                        mpu_presentation_time_microseconds);
                            }
						}
						//matching_lls_sls_mmt_session->audio_packet_id = mp_table_asset_row->mmt_general_location_info.packet_id;
						__MMSM_DEBUG("MPT message: mmtp_flow: %p, setting stpp_packet_id: packet_id: %u, asset_type: %s, default: %u, identifier: %s",
								atsc3_mmt_mfu_context->mmtp_flow,
								mp_table_asset_row->mmt_general_location_info.packet_id,
								mp_table_asset_row->asset_type,
								mp_table_asset_row->default_asset_flag,
								mp_table_asset_row->identifier_mapping.asset_id.asset_id ? (const char*)mp_table_asset_row->identifier_mapping.asset_id.asset_id : "");

					}
				}
			}
		} else if(mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type == MMT_ATSC3_MESSAGE_ID) {
            if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_route_component) {
                //dispatch our ROUTEComponent notification here
                if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_routecomponent_message_present) {
                    //jjustman-2020-12-08 - TODO - fixme so we don't free this pinned instance
                    bool assign_routecomponent_payload_to_context = atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_routecomponent_message_present(atsc3_mmt_mfu_context, mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_route_component);
                    if(assign_routecomponent_payload_to_context) {
                        //jjustman-2020-08-05 - also atsc3_sls_on_held_trigger_received_with_version_callback
                        atsc3_mmt_mfu_context->mmt_atsc3_route_component_monitored = mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_route_component;
                        mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_route_component->__is_pinned_to_context = true;
                    }
                }
            }

            if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_held_message) {
                //dispatch our HELD component here
                if(atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_held_message_present) {
                    atsc3_mmt_mfu_context->atsc3_mmt_signalling_information_on_held_message_present(atsc3_mmt_mfu_context, mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.mmt_atsc3_held_message);
                }

            }



		} else {
            __MMSM_TRACE("mmt_signalling_message_update_lls_sls_mmt_session: Ignoring signal: 0x%x", mmt_signalling_message_header_and_payload->message_header.MESSAGE_id_type);
        }
	}
}

/*
 *
 * from iso 14496-12
 *
 aligned(8) class TrackRunBox extends FullBox(‘trun’, version, tr_flags) {
    unsigned int(32)  sample_count;
    // the following are optional fields
    signed int(32) data_offset;
    unsigned int(32)  first_sample_flags;
    // all fields in the following array are optional
    {
      unsigned int(32)  sample_duration;
      unsigned int(32)  sample_size;
      unsigned int(32)  sample_flags
      if (version == 0)
         { unsigned int(32) sample_composition_time_offset }
      else
         { signed int(32) sample_composition_time_offset }
    }[ sample_count ]
}

aligned(8) class FullBox(unsigned int(32) boxtype, unsigned int(8) v, bit(24) f) extends Box(boxtype) {
    unsigned int(8)   version = v;
    bit(24)           flags = f;
}
*/

/*
 *
 * jjustman-2020-11-18 - TODO:
 *
 *  from GAM with AC-4 audio
 *
 *  1.) parse out moof/mfhd/traf/tfhd to determine if default sample duration is set
 *
         [moof] size=8+504
          [mfhd] size=12+4
            sequence number = 1
          [traf] size=8+428
            [tfhd] size=12+16, flags=2002a
              track ID = 2
              sample description index = 1
              default sample duration = 1601
              default sample flags = 0
            [tfdt] size=12+8, version=1
              base media decode time = 0
 *
 * 2.) re-base over init metadata, specifically in moov/mvhd/trak/mdia/mdhd@timescale:
 *  [ftyp] size=8+16
  major_brand = mpuf
  minor_version = 0
  compatible_brand = isom
  compatible_brand = mpuf
[mmpu] size=8+29
[moov] size=8+1021
  [mvhd] size=12+96
    timescale = 1000000
    duration = 0
    duration(ms) = 0
  [trak] size=8+454
    [tkhd] size=12+80, flags=7
      enabled = 1
      id = 2
      duration = 0
      volume = 256
      layer = 0
      alternate_group = 0
      matrix_0 = 1.000000
      matrix_1 = 0.000000
      matrix_2 = 0.000000
      matrix_3 = 0.000000
      matrix_4 = 1.000000
      matrix_5 = 0.000000
      matrix_6 = 0.000000
      matrix_7 = 0.000000
      matrix_8 = 16384.000000
      width = 0.000000
      height = 0.000000
    [mdia] size=8+354
      [mdhd] size=12+20
        timescale = 48000


 *   1.) check our tfhd box first if default sample duration is set
            0x000008 default‐sample‐duration‐present
            (avoid duration‐is‐empty flag)
    2.) otherwise, fallback to our trun box,


 */
uint32_t atsc3_mmt_movie_fragment_extract_sample_duration_us(block_t* mmt_movie_fragment_metadata, uint32_t decoder_configuration_timebase) {
    uint32_t sample_duration_unrebased = 0;
    uint32_t sample_duration_rebased_us = 0;

    atsc3_isobmff_tfhd_box_t* atsc3_isobmff_tfhd_box = NULL;
    atsc3_isobmff_trun_box_t* atsc3_isobmff_trun_box = NULL;

    if(!mmt_movie_fragment_metadata) {
        __MMSM_ERROR("atsc3_mmt_movie_fragment_extract_sample_duration_us: mmt_movie_fragment_metadata was NULL!");
        return 0;
    }

    //we need at least 20 bytes for tfhd box...
    if(block_Remaining_size(mmt_movie_fragment_metadata) < 20) {
        __MMSM_ERROR("atsc3_mmt_movie_fragment_extract_sample_duration_us: block_Remaining_size(mmt_movie_fragment_metadata): %d less than 20 bytes!",
                     block_Remaining_size(mmt_movie_fragment_metadata));
        return 0;
    }

    __MMSM_TRACE("atsc3_mmt_movie_fragment_extract_sample_duration_us: extracting from %p, pos: %d, length: %d, decoder_configuration_timebase: %d",
            mmt_movie_fragment_metadata, mmt_movie_fragment_metadata->i_pos, mmt_movie_fragment_metadata->p_size, decoder_configuration_timebase);

    atsc3_isobmff_tfhd_box = atsc3_isobmff_box_parser_tools_parse_tfhd_from_block_t(mmt_movie_fragment_metadata);
    if(atsc3_isobmff_tfhd_box && atsc3_isobmff_tfhd_box->flag_default_sample_duration) {
        sample_duration_unrebased = atsc3_isobmff_tfhd_box->default_sample_duration;
        __MMSM_TRACE("atsc3_mmt_movie_fragment_extract_sample_duration_us: using tfhd default_sample_duration: %d (unbased)", sample_duration_unrebased);
    } else {
        atsc3_isobmff_trun_box = atsc3_isobmff_box_parser_tools_parse_trun_from_block_t(mmt_movie_fragment_metadata);
        if(atsc3_isobmff_trun_box && atsc3_isobmff_trun_box->flag_sample_duration_present && atsc3_isobmff_trun_box->sample_count) {
            //grab the first sample here
            sample_duration_unrebased = atsc3_isobmff_trun_box->sample_duration;
        }
    }

    if(sample_duration_unrebased) {
        if(decoder_configuration_timebase != 1000000) {
            sample_duration_rebased_us = ((1000000L) * sample_duration_unrebased) / decoder_configuration_timebase;
            __MMSM_TRACE("atsc3_mmt_movie_fragment_extract_sample_duration_us: unrebased: %d, decoder_configuration_timebase: %d, sample_duration_rebased_us: %d",
                         sample_duration_unrebased, decoder_configuration_timebase, sample_duration_rebased_us);
        } else {
            sample_duration_rebased_us = sample_duration_unrebased;
        }
    }

    if(atsc3_isobmff_tfhd_box) {
        atsc3_isobmff_tfhd_box_free(&atsc3_isobmff_tfhd_box);
    }

    if(atsc3_isobmff_trun_box) {
        atsc3_isobmff_trun_box_free(&atsc3_isobmff_trun_box);
    }

    return sample_duration_rebased_us;
}


