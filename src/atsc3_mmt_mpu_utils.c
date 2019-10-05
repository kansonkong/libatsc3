/*
 * atsc3_mmt_mpu_utils.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_mpu_utils.h"

//jjustman-2019-09-05 - todo: rename this to _MMT_MPU_UTILS_DEBUG_ENABLED
int _MMT_MPU_DEBUG_ENABLED = 0;
int _MMT_MPU_TRACE_ENABLED = 0;

udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container_t_init() {
	udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container = calloc(1, sizeof(udp_flow_latest_mpu_sequence_number_container_t));
	return udp_flow_latest_mpu_sequence_number_container;
}

udp_flow_latest_mpu_sequence_number_container_t* udp_flow_find_matching_flows(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, udp_flow_t* udp_flow_to_search) {
	udp_flow_latest_mpu_sequence_number_container_t* my_matching_flows = udp_flow_latest_mpu_sequence_number_container_t_init();

	//do 2 passes, one to collect the matching counts and a second to build a collection** without havint to remalloc
	my_matching_flows->udp_flows_n = 0;

	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
		if(udp_flow_match_from_udp_flow_t(udp_flow_packet_id_mpu_sequence_tuple, udp_flow_to_search)) {
			my_matching_flows->udp_flows_n++;
		}
	}

	if(!my_matching_flows->udp_flows_n) {
		return NULL;
	}

	my_matching_flows->udp_flows = calloc(my_matching_flows->udp_flows_n, sizeof(my_matching_flows->udp_flows));

	int flow_position = 0;
	//copy the actual flows over
	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
		if(udp_flow_match_from_udp_flow_t(udp_flow_packet_id_mpu_sequence_tuple, udp_flow_to_search)) {
			my_matching_flows->udp_flows[flow_position] = calloc(1, sizeof(udp_flow_packet_id_mpu_sequence_tuple_t));
			memcpy(my_matching_flows->udp_flows[flow_position], udp_flow_packet_id_mpu_sequence_tuple, sizeof(udp_flow_packet_id_mpu_sequence_tuple_t));

			flow_position++;
		}
	}
	return my_matching_flows;
}


void udp_flow_latest_mpu_sequence_number_container_t_release(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container) {

	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
		if(udp_flow_packet_id_mpu_sequence_tuple) {
			free(udp_flow_packet_id_mpu_sequence_tuple);
			udp_flow_latest_mpu_sequence_number_container->udp_flows[i] = NULL;
		}
	}
	udp_flow_latest_mpu_sequence_number_container->udp_flows_n = 0;

	free(udp_flow_latest_mpu_sequence_number_container);
}

/*
 * will return the highest sequence number stored
 */

udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_latest_mpu_sequence_number_from_packet_id(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, udp_packet_t* udp_packet, uint32_t packet_id) {

	udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple_in_collection = NULL;

	for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_in_collection = udp_flow_latest_mpu_sequence_number_container->udp_flows[i];
		if(udp_flow_match(udp_flow_packet_id_mpu_sequence_tuple_in_collection, udp_packet) && (udp_flow_packet_id_mpu_sequence_tuple_in_collection->packet_id == packet_id)) {
			return udp_flow_packet_id_mpu_sequence_tuple_in_collection;
		}
	}
	return NULL;
}

/*
 *  2019-02-20 - in general, the tuple of <dst_ip, dst_port, dst_packet_id> should have mpu_sequence_numbers that increment by one and only one for every completed MPU.
 *  In cases of packet loss or retransmission, we may see larger gaps that may break this n < n+1 model.
 *
 *  Additionally, running MMT loops will cause the mpu_sequence_numbers to loop around and cause a failure of evictions.
 *  Instead, we will use a discontinuity window of at least a 2 mpu_sequence gap,
 *  	e.g. udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number - mmtp_mpu_packet->mpu_sequence_number >=4,
 *  	with a cumulative count of at least 50 fragments before going back in time.
 *
 * See:
		#define __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_THRESHOLD 2
		#define __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_FRAGMENT_RECV_THRESHOLD 50

		Also we should wait until we get to a new stream access point but will need more evidence of how to handle this case
 *
 */

udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_latest_mpu_sequence_number_add_or_replace_and_check_for_rollover(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,
		udp_packet_t* udp_packet,
		mmtp_mpu_packet_t* mmtp_mpu_packet,
		lls_slt_monitor_t* lls_slt_monitor,
		lls_sls_mmt_session_t* matching_lls_sls_mmt_session,
		mmtp_flow_t* mmtp_flow) {

    udp_flow_packet_id_mpu_sequence_tuple_t** udp_flow_packet_id_mpu_sequence_tuple_in_collection = NULL;
    udp_flow_packet_id_mpu_sequence_tuple_t** udp_flow_packet_id_mpu_sequence_matching_pkt_id = NULL;


    for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
        udp_flow_packet_id_mpu_sequence_tuple_in_collection = &udp_flow_latest_mpu_sequence_number_container->udp_flows[i];

        if(udp_flow_match((*udp_flow_packet_id_mpu_sequence_tuple_in_collection), udp_packet) && ((*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->packet_id == mmtp_mpu_packet->mmtp_packet_id)) {
            udp_flow_packet_id_mpu_sequence_matching_pkt_id = udp_flow_packet_id_mpu_sequence_tuple_in_collection;
        }
    }

    //if we have a candidate <dip, dport, packet_id>, then check who has the largest mpu_sequence_number
    if(udp_flow_packet_id_mpu_sequence_matching_pkt_id) {
        if((*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number > mmtp_mpu_packet->mpu_sequence_number) {

            int mpu_sequence_number_negative_discontinuity_gap = (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number - mmtp_mpu_packet->mpu_sequence_number;

            if(mpu_sequence_number_negative_discontinuity_gap >= __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_THRESHOLD) {
                __MMT_MPU_WARN("Negative mpu_sequence_number discontinuity detected: packet_id: %u, UDP FLOW persisted mpu_sequence_number: %u, current mmtp_packet mpu_sequence_number: %u, fragment recv threshold: %u",
                        mmtp_mpu_packet->mmtp_packet_id,
                        (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number,
                        mmtp_mpu_packet->mpu_sequence_number,
                        ++(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity_received_fragments);

                if((*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity_received_fragments > __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_FRAGMENT_RECV_THRESHOLD) {

                    //change our mpu_sequence_number to to the lesser value and clear out our discontinuity flags
                    __MMT_MPU_WARN("Negative mpu_sequence_number discontinuity switchover change: packet_id: %u, UDP FLOW abandoning mpu_sequence_number: %u, updating back to mpu_sequence_number: %u, fragment recv threshold: %u",
                                   mmtp_mpu_packet->mmtp_packet_id,
                                                        (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number,
                                                        (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity,
                                                        ++(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity_received_fragments);

                    udp_flow_force_negative_mpu_discontinuity_value(*udp_flow_packet_id_mpu_sequence_matching_pkt_id, (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity, mmtp_mpu_packet, lls_slt_monitor, matching_lls_sls_mmt_session, mmtp_flow, udp_packet);

                } else {
                    (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity = mmtp_mpu_packet->mpu_sequence_number;
                    __MMT_MPU_WARN("Negative mpu_sequence_number discontinuity detected: UDP FLOW persisted mpu_sequence_number: %u, current mmtp_packet mpu_sequence_number: %u, fragment recv threshold: %u",
                                        (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number,
                                        mmtp_mpu_packet->mpu_sequence_number,
                                        ++(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity_received_fragments);
                }
            }  else {
                udp_flow_reset_negative_mpu_discontinuity_counters(*udp_flow_packet_id_mpu_sequence_matching_pkt_id);
            }

            return *udp_flow_packet_id_mpu_sequence_matching_pkt_id;
        } else {
            //update the tuple with the new mpu_sequence_number, only set this if its changed so we can sete a watch
            if((*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number != mmtp_mpu_packet->mpu_sequence_number) {
                (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number = mmtp_mpu_packet->mpu_sequence_number;
            }
            return *udp_flow_packet_id_mpu_sequence_matching_pkt_id;
        }
    } else {
        if(udp_flow_latest_mpu_sequence_number_container->udp_flows_n) {
            udp_flow_latest_mpu_sequence_number_container->udp_flows = realloc(udp_flow_latest_mpu_sequence_number_container->udp_flows, (udp_flow_latest_mpu_sequence_number_container->udp_flows_n + 1) * sizeof(udp_flow_latest_mpu_sequence_number_container->udp_flows));
            //realloc here
        } else {
            udp_flow_latest_mpu_sequence_number_container->udp_flows = calloc(1, sizeof(*udp_flow_latest_mpu_sequence_number_container->udp_flows));
        }
        udp_flow_latest_mpu_sequence_number_container->udp_flows[udp_flow_latest_mpu_sequence_number_container->udp_flows_n] = calloc(1, sizeof(udp_flow_packet_id_mpu_sequence_tuple_t));
        udp_flow_packet_id_mpu_sequence_tuple_in_collection = &udp_flow_latest_mpu_sequence_number_container->udp_flows[udp_flow_latest_mpu_sequence_number_container->udp_flows_n];
        (*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->udp_flow.src_ip_addr = udp_packet->udp_flow.src_ip_addr;
        (*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->udp_flow.dst_ip_addr = udp_packet->udp_flow.dst_ip_addr;
        (*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->udp_flow.src_port = udp_packet->udp_flow.src_port;
        (*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->udp_flow.dst_port = udp_packet->udp_flow.dst_port;
        (*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->packet_id = mmtp_mpu_packet->mmtp_packet_id;
        (*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->mpu_sequence_number = mmtp_mpu_packet->mpu_sequence_number;
        (*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->mpu_sequence_number_evict_range_start = mmtp_mpu_packet->mpu_sequence_number;

        udp_flow_latest_mpu_sequence_number_container->udp_flows_n++;
    }

    return (*udp_flow_packet_id_mpu_sequence_tuple_in_collection);
}

udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_tuple_clone(udp_flow_packet_id_mpu_sequence_tuple_t* from_udp_flow_packet_id_mpu_sequence_tuple) {
    udp_flow_packet_id_mpu_sequence_tuple_t* to_udp_flow_packet_id_mpu_sequence_tuple = (udp_flow_packet_id_mpu_sequence_tuple_t*) calloc(1, sizeof(udp_flow_packet_id_mpu_sequence_tuple_t));
    assert(to_udp_flow_packet_id_mpu_sequence_tuple);
    memcpy(to_udp_flow_packet_id_mpu_sequence_tuple, from_udp_flow_packet_id_mpu_sequence_tuple, sizeof(udp_flow_packet_id_mpu_sequence_tuple_t));
    
    return to_udp_flow_packet_id_mpu_sequence_tuple;
}


void udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(udp_flow_packet_id_mpu_sequence_tuple_t** to_udp_flow_packet_id_mpu_sequence_tuple_p, udp_flow_packet_id_mpu_sequence_tuple_t* from_udp_flow_packet_id_mpu_sequence_tuple) {
    udp_flow_packet_id_mpu_sequence_tuple_t* to_udp_flow_packet_id_mpu_sequence_tuple = *to_udp_flow_packet_id_mpu_sequence_tuple_p;
    
    if(to_udp_flow_packet_id_mpu_sequence_tuple) {
        free(to_udp_flow_packet_id_mpu_sequence_tuple);
    }
    *to_udp_flow_packet_id_mpu_sequence_tuple_p = udp_flow_packet_id_mpu_sequence_tuple_clone(from_udp_flow_packet_id_mpu_sequence_tuple);
}

//jjustman-2019-09-25 - TODO: fix this for mpu sequence number rollover

////this is important as we need to clean up our pending eviction fragments and our current mpu_sequence_number for a clean rollover
void udp_flow_force_negative_mpu_discontinuity_value(udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_matching_pkt_id,
		uint32_t new_old_mpu_sequence_number_to_force,
		mmtp_mpu_packet_t* mmtp_mpu_packet,
        lls_slt_monitor_t* lls_slt_monitor,
		lls_sls_mmt_session_t* matching_lls_sls_mmt_session,
        mmtp_flow_t *mmtp_flow,
        udp_packet_t *udp_packet) {

	//slightly more complex than needed
	//jjustman-2019-10-05: TODO - refactor this selector logic for mmtp_packet_id_packets_container
    mmtp_asset_flow_t* mmtp_asset_flow = mmtp_flow_find_or_create_from_udp_packet(mmtp_flow, udp_packet);
    mmtp_asset_t* mmtp_asset = mmtp_asset_flow_find_or_create_asset_from_lls_sls_mmt_session(mmtp_asset_flow, matching_lls_sls_mmt_session);

    //todo: jjustman-2019-10-05 - clear any mpu_sequence_numbers >  mmtp_mpu_packet->mpu_sequence_number based upon packet_id
    mmtp_packet_id_packets_container_t* mmtp_packet_id_packets_container =  mmtp_asset_find_or_create_packets_container_from_mmt_mpu_packet(mmtp_asset, mmtp_mpu_packet);

    if(lls_slt_monitor->lls_sls_mmt_monitor->audio_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
    	mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio->mpu_sequence_number);
        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
		udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_audio, udp_flow_packet_id_mpu_sequence_matching_pkt_id);
    } else if(lls_slt_monitor->lls_sls_mmt_monitor->video_packet_id == mmtp_mpu_packet->mmtp_packet_id) {
    	mpu_sequence_number_mmtp_mpu_packet_collection_t* mpu_sequence_number_mmtp_mpu_packet_collection = mmtp_packet_id_packets_container_find_mpu_sequence_number_mmtp_mpu_packet_collection_from_mpu_sequence_number(mmtp_packet_id_packets_container, matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video->mpu_sequence_number);
        mmtp_packet_id_packets_container_remove_mpu_sequence_number_mmtp_mpu_packet_collection(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection);
		udp_flow_packet_id_mpu_sequence_tuple_free_and_clone(&matching_lls_sls_mmt_session->last_udp_flow_packet_id_mpu_sequence_tuple_video, udp_flow_packet_id_mpu_sequence_matching_pkt_id);
    }
}

void udp_flow_reset_negative_mpu_discontinuity_counters(udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_matching_pkt_id) {
	udp_flow_packet_id_mpu_sequence_matching_pkt_id->mpu_sequence_number_negative_discontinuity = 0;
	udp_flow_packet_id_mpu_sequence_matching_pkt_id->mpu_sequence_number_negative_discontinuity_received_fragments = 0;
}

//
//int atsc3_mmt_mpu_clear_data_unit_from_packet_subflow(mmtp_payload_fragments_union_t* mmtp_payload_fragments_union, uint32_t evict_range_start, uint32_t evict_range_end) {
//
//	int evicted_count = 0;
//	mmtp_sub_flow_t* mmtp_sub_flow = mmtp_payload_fragments_union->mmtp_mpu_packet->mmtp_sub_flow;
//	mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
//	mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //techincally this is mpu_fragments->media_fragment_unit_vector
//
//	mpu_fragments_t* mpu_fragments = NULL;
//
//	if(mmtp_sub_flow) {
//		 mpu_fragments = mpu_fragments_get_or_set_packet_id(mmtp_sub_flow, mmtp_payload_fragments_union->mmtp_mpu_packet->mmtp_packet_id);
//	}
//
//	if(mpu_fragments && evict_range_start) {
//
//		for(; evict_range_start < evict_range_end; evict_range_start++) {
//			data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mpu_fragments->media_fragment_unit_vector, evict_range_start);
//
//			if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.data) {
//				data_unit_payload_fragments = &data_unit_payload_types->timed_fragments_vector;
//				if(data_unit_payload_fragments) {
//					__MMT_MPU_INFO("Beginning eviction pass for mpu: %u, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size: %lu", evict_range_start, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size);
//					int evicted_count = atsc3_mmt_mpu_clear_data_unit_payload_fragments(mmtp_payload_fragments_union->mmtp_mpu_packet->mmtp_packet_id, mmtp_sub_flow, mpu_fragments, data_unit_payload_fragments);
//					__MMT_MPU_INFO("Eviction pass for mpu: %u resulted in %u", evict_range_start, evicted_count);
//				}
//			}
//		}
//	}
//
//	return evicted_count;
//}
//
//int atsc3_mmt_mpu_remove_packet_fragment_from_flows(mmtp_sub_flow_t* mmtp_sub_flow, mpu_fragments_t* mpu_fragments, mmtp_payload_fragments_union_t* packet) {
//
//    //clear out matching packets from allpackets,
//    ssize_t all_packets_index = 0;
//    int evicted_count = 0;
//
//    //free the sub-flow fragment
//    atsc3_vector_index_of(&mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector, packet, &all_packets_index);
//    __MMT_MPU_TRACE("atsc3_vector_index_of container: mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector: %p, payload : %p, packet_id: %u, packet_counter: %u, mpu_sequence_number: %u, at index: %ld",
//                    &mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector,
//                    packet,
//                    packet->mmtp_mpu_packet->mmtp_packet_id,
//                    packet->mmtp_mpu_packet->packet_counter,
//                    packet->mmtp_mpu_packet->mpu_sequence_number,
//                    all_packets_index);
//
//    if(all_packets_index >-1) {
//        __MMT_MPU_TRACE("freeing payload from all_mpu_fragments_vector via vector_remove_noshrkink at index: %ld", all_packets_index);
//        atsc3_vector_remove_noshrink(&mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector, all_packets_index);
//        evicted_count++;
//    }
//
//    if(mpu_fragments) {
//        //free global fragment
//        atsc3_vector_index_of(&mpu_fragments->all_mpu_fragments_vector, packet, &all_packets_index);
//        __MMT_MPU_TRACE("atsc3_vector_index_of container: mpu_fragments->all_mpu_fragments_vector: %p, payload : %p, packet_id: %u, packet_counter: %u, mpu_sequence_number: %u, at index: %ld",
//                        &mpu_fragments->all_mpu_fragments_vector,
//                        packet,
//                        packet->mmtp_mpu_packet->mmtp_packet_id,
//                        packet->mmtp_mpu_packet->packet_counter,
//                        packet->mmtp_mpu_packet->mpu_sequence_number,
//                        all_packets_index);
//
//        if(all_packets_index >-1) {
//            __MMT_MPU_TRACE("packet flow all_mpu_Fragments_vector is: %p, size: %lu, mmtp_sub_flow->mpu_fragments is: %p, global mpu_fragments->all_mpu_fragments is: %p, and size is: %lu",
//                            &packet->mmtp_mpu_packet->mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector,
//                            packet->mmtp_mpu_packet->mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size,
//                            &mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector,
//                            &mpu_fragments->all_mpu_fragments_vector,
//                            mpu_fragments->all_mpu_fragments_vector.size);
//
//            atsc3_vector_remove_noshrink(&mpu_fragments->all_mpu_fragments_vector, all_packets_index);
//            evicted_count++;
//        }
//
//        /**
//
//         free from mpu types
//
//
//         //todo - fix me against types
//         //    if(mmtp_mpu_packet->mpu_fragment_type == 0x00) {
//         //        my_evicted_count += atsc3_mmt_mpu_remove_packet_fragment_from_flows(mmtp_payload->mmtp_packet_header->mmtp_sub_flow, &mmtp_payload->mmtp_packet_header->mmtp_sub_flow->mpu_fragments->mpu_metadata_fragments_vector, mmtp_payload);
//         //    } else if(mmtp_mpu_packet->mpu_fragment_type == 0x01) {
//         //        my_evicted_count += atsc3_mmt_mpu_remove_packet_fragment_from_flows(mmtp_payload->mmtp_packet_header->mmtp_sub_flow, &mmtp_payload->mmtp_packet_header->mmtp_sub_flow->mpu_fragments->movie_fragment_metadata_vector, mmtp_payload);
//         //    } if(mmtp_mpu_packet->mpu_fragment_type == 0x02) {
//         //        my_evicted_count += atsc3_mmt_mpu_remove_packet_fragment_from_flows(mmtp_payload->mmtp_packet_header->mmtp_sub_flow, &mmtp_payload->mmtp_packet_header->mmtp_sub_flow->mpu_fragments->media_fragment_unit_vector, mmtp_payload);
//         //    }
//
//         **/
//    }
//    return evicted_count;
//}
//
//int atsc3_mmt_mpu_clear_data_unit_payload_fragments(uint16_t to_filter_packet_id, mmtp_sub_flow_t* mmtp_sub_flow, mpu_fragments_t* mpu_fragments, mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments) {
//
//    uint32_t evicted_count = 0;
//	//clear out any mfu's in queue if we are here
//	//remove our packets from the subflow and free block allocs, as mpu_push_to_output_buffer_no_locking will copy the p_buffer to a slab
//	for(int i=0; i < data_unit_payload_fragments->size; i++) {
//
//		mmtp_payload_fragments_union_t* packet = data_unit_payload_fragments->data[i];
//        if(packet && packet->mmtp_packet_header->mmtp_packet_id == to_filter_packet_id) {
//            uint32_t my_evicted_count = atsc3_mmt_mpu_remove_packet_fragment_from_flows(mmtp_sub_flow, mpu_fragments, packet);
//            evicted_count += my_evicted_count;
//            __MMT_MPU_TRACE("atsc3_mmt_mpu_clear_data_unit_payload_fragments: index: %u, packet: %p, packet_id: %u, packet_counter: %u, resulted in %u evictions, total evictions: %u", i, packet, packet->mmtp_packet_header->mmtp_packet_id, packet->mmtp_packet_header->packet_counter, my_evicted_count, evicted_count);
//
//            //clear out any block_t allocs
//            //mmtp_payload_fragments_union_free(&packet);
//            mmt_mpu_free_payload(packet);
//            free(packet);
//            //force clean up?
//            data_unit_payload_fragments->data[i] = NULL;
//        } else {
//            __MMT_MPU_ERROR("atsc3_mmt_mpu_clear_data_unit_payload_fragments: remaining packet in data_unit_payload_fragments: %p, index: %u", data_unit_payload_fragments, i);
//        }
//	}
//	mpu_fragments_vector_shrink_to_fit(mmtp_sub_flow->mpu_fragments);
//
//	if(mpu_fragments) {
//		mpu_fragments_vector_shrink_to_fit(mpu_fragments);
//	}
//
//	//clear out all of the data uint fragments here...
//	atsc3_vector_clear(data_unit_payload_fragments);
//	return evicted_count;
//}


void mmtp_mpu_dump_header(mmtp_mpu_packet_t* mmtp_mpu_packet) {

	__MMT_MPU_DEBUG("------------------");

	if(mmtp_mpu_packet->mpu_timed_flag) {
		__MMT_MPU_DEBUG("mmt_mpu_packet (timed), packet: %p", mmtp_mpu_packet);
		__MMT_MPU_DEBUG("-----------------");
		__MMT_MPU_DEBUG(" mpu_fragmentation_indicator: %d", mmtp_mpu_packet->mpu_fragment_type);
		__MMT_MPU_DEBUG(" movie_fragment_seq_num: %u", mmtp_mpu_packet->movie_fragment_sequence_number);
		__MMT_MPU_DEBUG(" sample_num: %u", mmtp_mpu_packet->sample_number);
		__MMT_MPU_DEBUG(" offset: %u", mmtp_mpu_packet->offset);
		__MMT_MPU_DEBUG(" pri: %d", mmtp_mpu_packet->priority);
		__MMT_MPU_DEBUG(" mpu_sequence_number: %u",mmtp_mpu_packet->mpu_sequence_number);

	}
	__MMT_MPU_DEBUG("-----------------");
}

void mmtp_mpu_dump_flow(uint32_t dst_ip, uint16_t dst_port, mmtp_mpu_packet_t* mmtp_mpu_packet) {
	//sub_flow_vector is a global
	mmtp_mpu_dump_header(mmtp_mpu_packet);

	__MMT_MPU_DEBUG("mmtp_mpu_dump_flow: file dump to: %d.%d.%d.%d:%d-p:%d.s:%d.ft:%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_mpu_packet->mmtp_packet_id,
			mmtp_mpu_packet->mpu_sequence_number,
			mmtp_mpu_packet->mpu_fragment_type);

	char *myFilePathName = calloc(64, sizeof(char*));
	snprintf(myFilePathName, 64, "mpu/%d.%d.%d.%d,%d-p.%d.s,%d.ft,%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_mpu_packet->mmtp_packet_id,
			mmtp_mpu_packet->mpu_sequence_number,

			mmtp_mpu_packet->mpu_fragment_type);



	FILE *f = fopen(myFilePathName, "a");
	if(!f) {
		__MMT_MPU_ERROR("mmtp_mpu_dump_flow: unable to open file: %s", myFilePathName);
			return;
	}

	block_t* du_block_to_write = NULL;

	if(mmtp_mpu_packet->du_mpu_metadata_block) {
		__MMT_MPU_DEBUG("mmtp_mpu_dump_flow: du_mpu_metadata_block   (0x0), file dump to: %s, size: %d", myFilePathName, block_Remaining_size(mmtp_mpu_packet->du_mpu_metadata_block));
		du_block_to_write = mmtp_mpu_packet->du_mpu_metadata_block;
	} else if(mmtp_mpu_packet->du_movie_fragment_block) {
		__MMT_MPU_DEBUG("mmtp_mpu_dump_flow: du_movie_fragment_block (0x1), file dump to: %s, size: %d", myFilePathName, block_Remaining_size(mmtp_mpu_packet->du_movie_fragment_block));
		du_block_to_write = mmtp_mpu_packet->du_movie_fragment_block;
	} else if(mmtp_mpu_packet->du_mfu_block) {
		__MMT_MPU_DEBUG("mmtp_mpu_dump_flow: du_mfu_block            (0x2), file dump to: %s, size: %d", myFilePathName, block_Remaining_size(mmtp_mpu_packet->du_mfu_block));
		du_block_to_write = mmtp_mpu_packet->du_mfu_block;
	}

	int blocks_written = fwrite(du_block_to_write->p_buffer, block_Remaining_size(du_block_to_write), 1, f);
	if(blocks_written != 1) {
		__MMT_MPU_WARN("Incomplete block written for %s", myFilePathName);
	}


	fclose(f);
}

////assumes in-order delivery
//void mpu_dump_reconstitued(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload) {
//	//sub_flow_vector is a global
//	mmtp_mpu_dump_header(mmtp_payload);
//
//	__MMT_MPU_DEBUG("::dump_mpu_reconstitued ******* file dump file: %d.%d.%d.%d:%d-p:%d.s:%d.ft:%d",
//			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
//			dst_port,
//			mmtp_mpu_packet->mmtp_packet_id,
//			mmtp_mpu_packet->mpu_sequence_number,
//
//			mmtp_mpu_packet->mpu_fragment_type);
//
//	char *myFilePathName = calloc(64, sizeof(char*));
//	snprintf(myFilePathName, 64, "mpu/%d.%d.%d.%d,%d-p.%d.s,%d.ft",
//			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
//			dst_port,
//			mmtp_mpu_packet->mmtp_packet_id,
//			mmtp_mpu_packet->mpu_sequence_number);
//
//
//	__MMT_MPU_DEBUG("::dumpMfu ******* file dump file: %s", myFilePathName);
//
//		FILE *f = fopen(myFilePathName, "a");
//	if(!f) {
//		__MMT_MPU_ERROR("::dumpMpu ******* UNABLE TO OPEN FILE %s", myFilePathName);
//			return;
//	}
//
//	int blocks_written = fwrite(mmtp_mpu_packet->mpu_data_unit_payload, mmtp_mpu_packet->mpu_data_unit_payload->i_pos, 1, f);
//	if(blocks_written != 1) {
//		__MMT_MPU_WARN("Incomplete block written for %s", myFilePathName);
//	}
//
//	fclose(f);
//}

//TODO: jjustman-2019-08-10 - switch to generic pipe_buffer per packet_id
// * callee is responsible for freeing mpu_data_unit_payload
void mmtp_mfu_push_to_output_buffer(pipe_ffplay_buffer_t* pipe_ffplay_buffer, mmtp_mpu_packet_t* mmtp_mpu_packet) {

	bool should_signal = false;
	if(mmtp_mpu_packet->mmtp_payload_type != 0x0) {
		__MMT_MPU_WARN("mmtp_mfu_push_to_output_buffer: incorrect mmtp_payload_type type: expected: 0x0, actual: 0x%x", mmtp_mpu_packet->mmtp_payload_type);
		goto cleanup;
	}

	if(mmtp_mpu_packet->mpu_fragment_type != 0x2) {
		__MMT_MPU_WARN("mmtp_mfu_push_to_output_buffer: incorrect mpu_fragment_type type: expected: 0x2 (mfu), actual: 0x%x", mmtp_mpu_packet->mpu_fragment_type);
		goto cleanup;
	}

	if(!mmtp_mpu_packet->du_mfu_block) {
		__MMT_MPU_WARN("mmtp_mfu_push_to_output_buffer: mmtp_mpu_packet->du_mfu_block is NULL");
		goto cleanup;
	}

	__MMT_MPU_DEBUG("mmtp_mfu_push_to_output_buffer: pushing du_mfu_block: %p, packet_id: %d, mpu_sequence_number: %d, sample_number: %d, mpu_fragment_counter: %d",
			mmtp_mpu_packet->du_mfu_block,
			mmtp_mpu_packet->mmtp_packet_id,
			mmtp_mpu_packet->mpu_sequence_number,
			mmtp_mpu_packet->sample_number,
			mmtp_mpu_packet->mpu_fragment_counter);

	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
	pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, mmtp_mpu_packet->du_mfu_block->p_buffer, mmtp_mpu_packet->du_mfu_block->p_size);
	pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer);
	pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

cleanup: ;
	//noop

}
