/*
 * atsc3_mmt_mpu_utils.c
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_mpu_utils.h"

int _MMT_MPU_DEBUG_ENABLED = 0;


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

udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_last_mpu_sequence_number_from_packet_id(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container, udp_packet_t* udp_packet, uint32_t packet_id) {

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
 *  	e.g. udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number - mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number >=4,
 *  	with a cumulative count of at least 50 fragments before going back in time.
 *
 * See:
		#define __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_THRESHOLD 2
		#define __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_FRAGMENT_RECV_THRESHOLD 50

		Technically we should wait until we get to a new stream access point but will need more evidence of how to handle this "rare scenario"

 *
 */

udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_latest_mpu_sequence_number_add_or_replace(udp_flow_latest_mpu_sequence_number_container_t* udp_flow_latest_mpu_sequence_number_container,  udp_packet_t* udp_packet, mmtp_payload_fragments_union_t* mmtp_packet) {
	udp_flow_packet_id_mpu_sequence_tuple_t** udp_flow_packet_id_mpu_sequence_tuple_in_collection = NULL;
	udp_flow_packet_id_mpu_sequence_tuple_t** udp_flow_packet_id_mpu_sequence_matching_pkt_id = NULL;


    for(int i=0; i < udp_flow_latest_mpu_sequence_number_container->udp_flows_n; i++) {
		udp_flow_packet_id_mpu_sequence_tuple_in_collection = &udp_flow_latest_mpu_sequence_number_container->udp_flows[i];

		if(udp_flow_match((*udp_flow_packet_id_mpu_sequence_tuple_in_collection), udp_packet) && ((*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->packet_id == mmtp_packet->mmtp_mpu_type_packet_header.mmtp_packet_id)) {
			udp_flow_packet_id_mpu_sequence_matching_pkt_id = udp_flow_packet_id_mpu_sequence_tuple_in_collection;
		}
	}

	//if we have a candidate <dip, dport, packet_id>, then check who has the largest mpu_sequence_number
	if(udp_flow_packet_id_mpu_sequence_matching_pkt_id) {
		if((*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number > mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number) {

			int mpu_sequence_number_negative_discontinuity_gap = (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number - mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number;

			if(mpu_sequence_number_negative_discontinuity_gap >= __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_THRESHOLD) {
				__MMT_MPU_WARN("Negative mpu_sequence_number discontinuity detected: UDP FLOW persisted mpu_sequence_number: %u, current mmtp_packet mpu_sequence_number: %u, fragment recv threshold: %u",
						(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number,
						mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number,
						++(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity_received_fragments);

				if((*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity_received_fragments > __MPU_FLOW_NEGATIVE_DISCONTINUITY_SEQUENCE_GAP_FRAGMENT_RECV_THRESHOLD) {

					//change our mpu_sequence_number to to the lesser value and clear out our discontinuity flags
					__MMT_MPU_WARN("Negative mpu_sequence_number discontinuity switchover change: UDP FLOW persisted mpu_sequence_number: %u, updating back to  mpu_sequence_number: %u, fragment recv threshold: %u",
														(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number,
														(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity,
														++(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity_received_fragments);

					udp_flow_force_negative_mpu_discontinuity_value(*udp_flow_packet_id_mpu_sequence_matching_pkt_id, (*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity, mmtp_packet);

				} else {
					(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity = mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number;
					__MMT_MPU_WARN("Negative mpu_sequence_number discontinuity detected: UDP FLOW persisted mpu_sequence_number: %u, current mmtp_packet mpu_sequence_number: %u, fragment recv threshold: %u",
										(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number,
										mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number,
										++(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number_negative_discontinuity_received_fragments);
				}
			}  else {
				udp_flow_reset_negative_mpu_discontinuity_counters(*udp_flow_packet_id_mpu_sequence_matching_pkt_id);
			}

			return *udp_flow_packet_id_mpu_sequence_matching_pkt_id;
		} else {
			//update the tuple with the new mpu_sequence_number
			(*udp_flow_packet_id_mpu_sequence_matching_pkt_id)->mpu_sequence_number = mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number;
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
		(*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->packet_id = mmtp_packet->mmtp_mpu_type_packet_header.mmtp_packet_id;
		(*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->mpu_sequence_number = mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number;
		(*udp_flow_packet_id_mpu_sequence_tuple_in_collection)->mpu_sequence_number_evict_range_start = mmtp_packet->mmtp_mpu_type_packet_header.mpu_sequence_number;

		udp_flow_latest_mpu_sequence_number_container->udp_flows_n++;
	}

	return (*udp_flow_packet_id_mpu_sequence_tuple_in_collection);
}

//this is important as we need to clean up our pending eviction fragments and our current mpu_sequence_number for a clean rollover
void udp_flow_force_negative_mpu_discontinuity_value(udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_matching_pkt_id, uint32_t new_old_mpu_sequence_number_to_force, mmtp_payload_fragments_union_t* mmtp_packet_fragments_to_evict) {
	uint32_t eviction_range_start = udp_flow_packet_id_mpu_sequence_matching_pkt_id->mpu_sequence_number_evict_range_start;
	uint32_t eviction_range_end = mmtp_packet_fragments_to_evict->mmtp_mpu_type_packet_header.mpu_sequence_number;

	atsc3_mmt_mpu_clear_data_unit_from_packet_subflow(mmtp_packet_fragments_to_evict, eviction_range_start, eviction_range_end);

	//for(int i=eviction_range_start)

		//force the "old" packet id
	udp_flow_packet_id_mpu_sequence_matching_pkt_id->mpu_sequence_number = new_old_mpu_sequence_number_to_force;

	udp_flow_packet_id_mpu_sequence_matching_pkt_id->mpu_sequence_number_evict_range_start = new_old_mpu_sequence_number_to_force;
	udp_flow_packet_id_mpu_sequence_matching_pkt_id->mpu_sequence_number_last_refragmentation_flush = new_old_mpu_sequence_number_to_force;

	udp_flow_reset_negative_mpu_discontinuity_counters(udp_flow_packet_id_mpu_sequence_matching_pkt_id);


}

void udp_flow_reset_negative_mpu_discontinuity_counters(udp_flow_packet_id_mpu_sequence_tuple_t* udp_flow_packet_id_mpu_sequence_matching_pkt_id) {
	udp_flow_packet_id_mpu_sequence_matching_pkt_id->mpu_sequence_number_negative_discontinuity = 0;
	udp_flow_packet_id_mpu_sequence_matching_pkt_id->mpu_sequence_number_negative_discontinuity_received_fragments = 0;
}


int atsc3_mmt_mpu_clear_data_unit_from_packet_subflow(mmtp_payload_fragments_union_t* mmtp_payload_fragments_union, uint32_t evict_range_start, uint32_t evict_range_end) {

	int evicted_count = 0;
	mmtp_sub_flow_t* mmtp_sub_flow = mmtp_payload_fragments_union->mmtp_mpu_type_packet_header.mmtp_sub_flow;
	mpu_data_unit_payload_fragments_t* data_unit_payload_types = NULL;
	mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments = NULL; //techincally this is mpu_fragments->media_fragment_unit_vector

	mpu_fragments_t* mpu_fragments = NULL;

	if(mmtp_sub_flow) {
		 mpu_fragments = mpu_fragments_get_or_set_packet_id(mmtp_sub_flow, mmtp_payload_fragments_union->mmtp_mpu_type_packet_header.mmtp_packet_id);

	}

	if(mpu_fragments && evict_range_start) {

		for(; evict_range_start < evict_range_end; evict_range_start++) {
			data_unit_payload_types = mpu_data_unit_payload_fragments_find_mpu_sequence_number(&mpu_fragments->media_fragment_unit_vector, evict_range_start);

			if(data_unit_payload_types && data_unit_payload_types->timed_fragments_vector.data) {
				data_unit_payload_fragments = &data_unit_payload_types->timed_fragments_vector;
				if(data_unit_payload_fragments) {
					__MMT_MPU_INFO("Beginning eviction pass for mpu: %u, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size: %lu", evict_range_start, mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size)
					int evicted_count = atsc3_mmt_mpu_clear_data_unit_payload_fragments(mmtp_sub_flow, mpu_fragments, data_unit_payload_fragments);
					__MMT_MPU_INFO("Eviction pass for mpu: %u resulted in %u", evict_range_start, evicted_count);
				}
			}
		}
	}

	return evicted_count;
}


int atsc3_mmt_mpu_clear_data_unit_payload_fragments(mmtp_sub_flow_t* mmtp_sub_flow, mpu_fragments_t* mpu_fragments, mpu_data_unit_payload_fragments_timed_vector_t* data_unit_payload_fragments) {
	//clear out matching packets from allpackets,
	ssize_t* all_packets_index = (long*) calloc(1, sizeof(ssize_t));
	int evicted_count = 0;

	//clear out any mfu's in queue if we are here
	//remove our packets from the subflow and free block allocs, as mpu_push_to_output_buffer_no_locking will copy the p_buffer to a slab
	for(int i=0; i < data_unit_payload_fragments->size; i++) {

		mmtp_payload_fragments_union_t* packet = data_unit_payload_fragments->data[i];


		//free the sub-flow fragment
		atsc3_vector_index_of(&mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector, packet, all_packets_index);
		__MMT_MPU_DEBUG("freeing container: mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector: %p, payload : %p, packet_counter: %u, mpu_sequence_number: %u, at index: %ld",
										&mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector,
										packet,
										packet->mmtp_mpu_type_packet_header.packet_counter,
										packet->mmtp_mpu_type_packet_header.mpu_sequence_number,
										*all_packets_index);

		if(*all_packets_index >-1) {
			__MMT_MPU_DEBUG("freeing payload from all_mpu_fragments_vector via vector_remove_noshrkink at index: %ld", *all_packets_index);
			atsc3_vector_remove_noshrink(&mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector, *all_packets_index);
			evicted_count++;
		}

		if(mpu_fragments) {
			//free global fragment
			atsc3_vector_index_of(&mpu_fragments->all_mpu_fragments_vector, packet, all_packets_index);
			__MMT_MPU_DEBUG("freeing container: mpu_fragments->all_mpu_fragments_vector: %p, payload : %p, packet_counter: %u, mpu_sequence_number: %u, at index: %ld",
													&mpu_fragments->all_mpu_fragments_vector,
													packet,
													packet->mmtp_mpu_type_packet_header.packet_counter,
													packet->mmtp_mpu_type_packet_header.mpu_sequence_number,
													*all_packets_index);

			if(*all_packets_index >-1) {
				atsc3_vector_remove_noshrink(&mpu_fragments->all_mpu_fragments_vector, *all_packets_index);
				evicted_count++;
			}
			__MMT_MPU_DEBUG("packet flow all_mpu_Fragments_vector is: %p, size: %lu, mmtp_sub_flow->mpu_fragments is: %p, global mpu_fragments->all_mpu_fragments is: %p, and size is: %lu",
					&packet->mmtp_mpu_type_packet_header.mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector,
					packet->mmtp_mpu_type_packet_header.mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector.size,
					&mmtp_sub_flow->mpu_fragments->all_mpu_fragments_vector,
					&mpu_fragments->all_mpu_fragments_vector,
					mpu_fragments->all_mpu_fragments_vector.size);

		}

		//clear out any block_t allocs
		mmtp_payload_fragments_union_free(&packet);
	}

	mpu_fragments_vector_shrink_to_fit(mmtp_sub_flow->mpu_fragments);

	if(mpu_fragments) {
		mpu_fragments_vector_shrink_to_fit(mpu_fragments);
	}

	//clear out all of the data uint fragments here...
	atsc3_vector_clear(data_unit_payload_fragments);
	return evicted_count;
}


void mpu_dump_header(mmtp_payload_fragments_union_t* mmtp_payload) {

	__MMT_MPU_DEBUG("------------------");

	if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_timed_flag) {
		__MMT_MPU_DEBUG("MFU Packet (Timed)");
		__MMT_MPU_DEBUG("-----------------");
		__MMT_MPU_DEBUG(" mpu_fragmentation_indicator: %d", mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_fragment_type);
		__MMT_MPU_DEBUG(" movie_fragment_seq_num: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.movie_fragment_sequence_number);
		__MMT_MPU_DEBUG(" sample_num: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.sample_number);
		__MMT_MPU_DEBUG(" offset: %u", mmtp_payload->mpu_data_unit_payload_fragments_timed.offset);
		__MMT_MPU_DEBUG(" pri: %d", mmtp_payload->mpu_data_unit_payload_fragments_timed.priority);
		__MMT_MPU_DEBUG(" mpu_sequence_number: %u",mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number);

	} else {
		__MMT_MPU_DEBUG("MFU Packet (Non-timed)");
		__MMT_MPU_DEBUG("---------------------");
		__MMT_MPU_DEBUG(" mpu_fragmentation_indicator: %d", mmtp_payload->mpu_data_unit_payload_fragments_nontimed.mpu_fragment_type);
		__MMT_MPU_DEBUG(" non_timed_mfu_item_id: %u", mmtp_payload->mpu_data_unit_payload_fragments_nontimed.non_timed_mfu_item_id);

	}

	__MMT_MPU_DEBUG("-----------------");
}

void mpu_dump_flow(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload) {
	//sub_flow_vector is a global
	mpu_dump_header(mmtp_payload);

	__MMT_MPU_DEBUG("::dumpMfu ******* file dump file: %d.%d.%d.%d:%d-p:%d.s:%d.ft:%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);

	char *myFilePathName = calloc(64, sizeof(char*));
	snprintf(myFilePathName, 64, "mpu/%d.%d.%d.%d,%d-p.%d.s,%d.ft,%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);


	__MMT_MPU_DEBUG("::dumpMfu ******* file dump file: %s", myFilePathName);

	FILE *f = fopen(myFilePathName, "a");
	if(!f) {
		__MMT_MPU_ERROR("::dumpMpu ******* UNABLE TO OPEN FILE %s", myFilePathName);
			return;
	}

	int blocks_written = fwrite(mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, 1, f);
	if(blocks_written != 1) {
		__MMT_MPU_WARN("Incomplete block written for %s", myFilePathName);
	}


	fclose(f);
}

//assumes in-order delivery
void mpu_dump_reconstitued(uint32_t dst_ip, uint16_t dst_port, mmtp_payload_fragments_union_t* mmtp_payload) {
	//sub_flow_vector is a global
	mpu_dump_header(mmtp_payload);

	__MMT_MPU_DEBUG("::dump_mpu_reconstitued ******* file dump file: %d.%d.%d.%d:%d-p:%d.s:%d.ft:%d",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number,

			mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);

	char *myFilePathName = calloc(64, sizeof(char*));
	snprintf(myFilePathName, 64, "mpu/%d.%d.%d.%d,%d-p.%d.s,%d.ft",
			(dst_ip>>24)&0xFF,(dst_ip>>16)&0xFF,(dst_ip>>8)&0xFF,(dst_ip)&0xFF,
			dst_port,
			mmtp_payload->mmtp_mpu_type_packet_header.mmtp_packet_id,
			mmtp_payload->mpu_data_unit_payload_fragments_timed.mpu_sequence_number);


	__MMT_MPU_DEBUG("::dumpMfu ******* file dump file: %s", myFilePathName);

		FILE *f = fopen(myFilePathName, "a");
	if(!f) {
		__MMT_MPU_ERROR("::dumpMpu ******* UNABLE TO OPEN FILE %s", myFilePathName);
			return;
	}

	int blocks_written = fwrite(mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, 1, f);
	if(blocks_written != 1) {
		__MMT_MPU_WARN("Incomplete block written for %s", myFilePathName);
	}

	fclose(f);
}

/*
 *
 * rules:
 *
 * mpu_sequnece number change ->
 * packet_seqeunce number gap
 *
 * for each sample_number increment from 1...60 map
 * 	when fragmentation_indication==1, fragmentation_counter = N fragments to process (e.g. 70, 69....)
 *
 *
 */

void mpu_push_to_output_buffer(pipe_ffplay_buffer_t* pipe_ffplay_buffer, mmtp_payload_fragments_union_t* mmtp_payload) {

	bool should_signal = false;
	if(mmtp_payload->mmtp_mpu_type_packet_header.mmtp_payload_type != 0x0) {
		__MMT_MPU_WARN("Incorrect payload type: 0x%x", mmtp_payload->mmtp_mpu_type_packet_header.mmtp_payload_type);
		goto cleanup;

	}

	if(!pipe_ffplay_buffer->has_written_init_box) {
		if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type != 0x0) {
			goto cleanup;
		}

		pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer);
		pipe_ffplay_buffer->last_mpu_sequence_number = mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number;
		pipe_ffplay_buffer->has_written_init_box = true;
		pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

		__MMT_MPU_DEBUG("pushing init fragment for %d fragment_type: 0", mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number);

		goto cleanup;

	} else {
		//ignore our init box after we've sent it the first time
		if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type == 0x0) {
			goto cleanup;
		}

		pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);
		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer);

		if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number != pipe_ffplay_buffer->last_mpu_sequence_number) {
			//signal here, we will have the first fragment of the next slice in the payload, but its simpler for now...
			__MMT_MPU_DEBUG("triggering signal because mpu_sequence changed from %u to %u",  pipe_ffplay_buffer->last_mpu_sequence_number, mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number );
			pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer);
		}
		pipe_ffplay_buffer->last_mpu_sequence_number = mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number;

		pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);

	}


cleanup:
	mmtp_payload_fragments_union_free(&mmtp_payload);

}

/*
 * only use this if you will manage the mutex externally
 *
 * callee is responsible for freeing mpu_data_unit_payload
 */

void mpu_push_to_output_buffer_no_locking(pipe_ffplay_buffer_t* pipe_ffplay_buffer, mmtp_payload_fragments_union_t* mmtp_payload) {

	__MMT_MPU_DEBUG("payload of size: %d payload type: 0x%x", mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);

	//discard if we are not a mpu payload
	if(mmtp_payload->mmtp_mpu_type_packet_header.mmtp_payload_type != 0x0) {
		__MMT_MPU_WARN("Incorrect payload type: 0x%x", mmtp_payload->mmtp_mpu_type_packet_header.mmtp_payload_type);
		goto cleanup;

	}

	//if we have not written our init box, mpu_fragment_type == 0x0, then discard until we see our first mpu_fragment_type == 0x0
	if(!pipe_ffplay_buffer->has_written_init_box) {
		if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type != 0x0) {
			goto cleanup;
		}

		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer);
		pipe_ffplay_buffer->last_mpu_sequence_number = mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number;
		pipe_ffplay_buffer->has_written_init_box = true;

		__MMT_MPU_DEBUG("pushing init fragment for %d fragment_type: 0", mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number);

		goto cleanup;

	} else {
		//ignore our init box after we've sent it the first time
		if(mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type == 0x0) {
			goto cleanup;
		}

		__MMT_MPU_DEBUG("pushing payload fragment for %d fragment_type: %d", mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number, mmtp_payload->mmtp_mpu_type_packet_header.mpu_fragment_type);

		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->p_buffer, mmtp_payload->mmtp_mpu_type_packet_header.mpu_data_unit_payload->i_buffer);

		pipe_ffplay_buffer->last_mpu_sequence_number = mmtp_payload->mmtp_mpu_type_packet_header.mpu_sequence_number;
	}

cleanup:
	return;
}

void mpu_fragments_vector_shrink_to_fit(mpu_fragments_t* mpu_fragments) {
	//atsc3_vector_shrink_to_fit(&mpu_fragments->all_mpu_fragments_vector);
	atsc3_vector_autoshrink(&mpu_fragments->all_mpu_fragments_vector);
}
