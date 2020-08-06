/*
 * atsc3_sls_alc_flow.c
 *
 *  Created on: Jul 28, 2020
 *      Author: jjustman
 */

#include "atsc3_sls_alc_flow.h"

int _SLS_ALC_FLOW_INFO_ENABLED = 0;
int _SLS_ALC_FLOW_DEBUG_ENABLED = 0;
int _SLS_ALC_FLOW_TRACE_ENABLED = 0;


ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT_METHODS_IMPLEMENTATION(atsc3_sls_alc_flow);

void atsc3_sls_alc_flow_typedef_free(atsc3_sls_alc_flow_t** atsc3_sls_alc_flow_p) {
	if(atsc3_sls_alc_flow_p) {
		atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = *atsc3_sls_alc_flow_p;

		//jjustman-2020-07-28 - TODO - make sure we're not doublefree'ing
		if(atsc3_sls_alc_flow) {
			if(atsc3_sls_alc_flow->atsc3_route_s_tsid_RS_LS) {
				atsc3_route_s_tsid_RS_LS_free(&atsc3_sls_alc_flow->atsc3_route_s_tsid_RS_LS);
			} else {
				if(atsc3_sls_alc_flow->media_info) {
					atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_free(&atsc3_sls_alc_flow->media_info);
				}
			}
			atsc3_sls_alc_flow_free_atsc3_route_object(atsc3_sls_alc_flow);

			free(atsc3_sls_alc_flow);
			atsc3_sls_alc_flow = NULL;
		}
		*atsc3_sls_alc_flow_p = NULL;
	}
}

/*
 * atsc3_sls_alc_flow_route_object_add_unique_lct_packet_received
 *
 * 	step 1. find our matching tsi/toi flow here
 */
atsc3_route_object_t* atsc3_sls_alc_flow_route_object_add_unique_lct_packet_received(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow_vector, atsc3_alc_packet_t* atsc3_alc_packet) {
	atsc3_route_object_t* atsc3_route_object = NULL;

	//find or create our alc_flow
	atsc3_sls_alc_flow_t* atsc3_sls_alc_flow = atsc3_sls_alc_flow_find_or_create_entry_from_alc_packet(atsc3_sls_alc_flow_vector, atsc3_alc_packet);

	_ATSC3_SLS_ALC_FLOW_TRACE("atsc3_sls_alc_flow_route_object_add_unique_lct_packet_received before find_or_create_route_object, atsc3_sls_alc_flow_vector.count: %d, tsi: %d, toi: %d, atsc3_sls_alc_flow: %p, atsc3_route_object_v: %d ",
			atsc3_sls_alc_flow_vector->count,
			atsc3_alc_packet->def_lct_hdr->tsi, atsc3_alc_packet->def_lct_hdr->toi,
			atsc3_sls_alc_flow,
			atsc3_sls_alc_flow->atsc3_route_object_v.count);

	//find or create our route_object
	atsc3_route_object = atsc3_sls_alc_flow_find_or_create_route_object_from_alc_packet(atsc3_sls_alc_flow, atsc3_alc_packet);

	_ATSC3_SLS_ALC_FLOW_TRACE("atsc3_sls_alc_flow_route_object_add_unique_lct_packet_received after find_or_create_route_object, obj: %p, tsi: %d, toi: %d, atsc3_sls_alc_flow: %p, count: %d ",
			atsc3_route_object,
			atsc3_alc_packet->def_lct_hdr->tsi, atsc3_alc_packet->def_lct_hdr->toi,
			atsc3_sls_alc_flow, atsc3_sls_alc_flow->atsc3_route_object_v.count);

	atsc3_route_object_add_or_update_lct_packet_received(atsc3_route_object, atsc3_alc_packet);

	return atsc3_route_object;
}


atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_or_create_entry_from_alc_packet(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow_vector, atsc3_alc_packet_t* atsc3_alc_packet) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_find_entry_from_alc_packet(atsc3_sls_alc_flow_vector, atsc3_alc_packet);
	if(matching_atsc3_sls_alc_flow == NULL) {
		//create a new entry
		matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow->tsi = atsc3_alc_packet->def_lct_hdr->tsi;
		atsc3_sls_alc_flow_add(atsc3_sls_alc_flow_vector, matching_atsc3_sls_alc_flow);
	}

	return matching_atsc3_sls_alc_flow;
}


//&& to_check_atsc3_sls_alc_flow_nrt->toi == alc_packet->def_lct_hdr->toi
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_from_alc_packet(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow_vector, atsc3_alc_packet_t* alc_packet) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = NULL;
	atsc3_sls_alc_flow_t* to_check_atsc3_sls_alc_flow = NULL;

	for(int i=0; i < atsc3_sls_alc_flow_vector->count && !matching_atsc3_sls_alc_flow; i++) {
		to_check_atsc3_sls_alc_flow = atsc3_sls_alc_flow_vector->data[i];
		if(to_check_atsc3_sls_alc_flow->tsi == alc_packet->def_lct_hdr->tsi) {
			matching_atsc3_sls_alc_flow = to_check_atsc3_sls_alc_flow;
		}
	}

	return matching_atsc3_sls_alc_flow;
}

atsc3_route_object_t* atsc3_sls_alc_flow_find_or_create_route_object_from_alc_packet(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, atsc3_alc_packet_t* alc_packet) {
	atsc3_route_object_t* matching_atsc3_route_object = atsc3_sls_alc_flow_find_route_object_entry_from_alc_packet(atsc3_sls_alc_flow, alc_packet);
	if(matching_atsc3_route_object == NULL) {
		//create a new entry
		matching_atsc3_route_object = atsc3_route_object_new();
		atsc3_route_object_set_alc_flow_and_tsi_toi(matching_atsc3_route_object, atsc3_sls_alc_flow, alc_packet);
		atsc3_sls_alc_flow_add_atsc3_route_object(atsc3_sls_alc_flow, matching_atsc3_route_object);
	}

	return matching_atsc3_route_object;
}

atsc3_route_object_t* atsc3_sls_alc_flow_find_route_object_entry_from_alc_packet(atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, atsc3_alc_packet_t* alc_packet) {
	atsc3_route_object_t* matching_atsc3_route_object = NULL;
	atsc3_route_object_t* to_check_atsc3_route_object = NULL;

	for(int i=0; i < atsc3_sls_alc_flow->atsc3_route_object_v.count && !matching_atsc3_route_object; i++) {
		to_check_atsc3_route_object = atsc3_sls_alc_flow->atsc3_route_object_v.data[i];
		if(to_check_atsc3_route_object->tsi == alc_packet->def_lct_hdr->tsi && to_check_atsc3_route_object->toi == alc_packet->def_lct_hdr->toi) {
			matching_atsc3_route_object = to_check_atsc3_route_object;
		}
	}

	return matching_atsc3_route_object;
}

//still needed!
//jjustman-2020-07-28
//for matching contentInfo.mediaInfo@repId
atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_find_entry_tsi(atsc3_sls_alc_flow, tsi);
	if(matching_atsc3_sls_alc_flow == NULL) {
		_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi: adding new entry to %p, tsi: %d\n", &atsc3_sls_alc_flow, tsi);
		//create a new entry
		matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow->tsi = tsi;

		if(media_info) {
			matching_atsc3_sls_alc_flow->media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone(media_info);
			_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: cloned media_info is now: %p (stsid: %p), content_type: %s, lang: %s, rep_id: %s",
					matching_atsc3_sls_alc_flow->media_info,
					media_info,
					matching_atsc3_sls_alc_flow->media_info->content_type,
					matching_atsc3_sls_alc_flow->media_info->lang,
					matching_atsc3_sls_alc_flow->media_info->rep_id);

		} else {
			_ATSC3_SLS_ALC_FLOW_WARN("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: media_info is NULL for new entry to %p, tsi: %d", &atsc3_sls_alc_flow, tsi);
		}

		atsc3_sls_alc_flow_add(atsc3_sls_alc_flow, matching_atsc3_sls_alc_flow);
	} else {
		//make sure we have our media_info
		if(media_info && !matching_atsc3_sls_alc_flow->media_info) {
			matching_atsc3_sls_alc_flow->media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone(media_info);
			_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: cloned media_info is now: %p (stsid: %p), content_type: %s, lang: %s, rep_id: %s",
					matching_atsc3_sls_alc_flow->media_info,
					media_info,
					matching_atsc3_sls_alc_flow->media_info->content_type,
					matching_atsc3_sls_alc_flow->media_info->lang,
					matching_atsc3_sls_alc_flow->media_info->rep_id);

		}
	}

	return matching_atsc3_sls_alc_flow;
}


atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init, atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_t* media_info) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_find_entry_tsi_toi_init(atsc3_sls_alc_flow, tsi, toi_init);
	if(matching_atsc3_sls_alc_flow == NULL) {
		_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: adding new entry to %p, tsi: %d, toi_init: %d\n", &atsc3_sls_alc_flow, tsi, toi_init);
		//create a new entry
		matching_atsc3_sls_alc_flow = atsc3_sls_alc_flow_new();
		matching_atsc3_sls_alc_flow->tsi = tsi;
		matching_atsc3_sls_alc_flow->toi_init = toi_init;

		if(media_info) {
			matching_atsc3_sls_alc_flow->media_info = atsc3_route_s_tsid_RS_LS_SrcFlow_ContentInfo_MediaInfo_clone(media_info);
			_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: cloned media_info is now: %p (stsid: %p), content_type: %s, lang: %s, rep_id: %s",
					matching_atsc3_sls_alc_flow->media_info,
					media_info,
					matching_atsc3_sls_alc_flow->media_info->content_type,
					matching_atsc3_sls_alc_flow->media_info->lang,
					matching_atsc3_sls_alc_flow->media_info->rep_id);

		} else {
			_ATSC3_SLS_ALC_FLOW_WARN("atsc3_sls_alc_flow_add_entry_unique_tsi_toi_init: media_info is NULL for new entry to %p, tsi: %d, toi_init: %d\n", &atsc3_sls_alc_flow, tsi, toi_init);
		}

		atsc3_sls_alc_flow_add(atsc3_sls_alc_flow, matching_atsc3_sls_alc_flow);
	}

	return matching_atsc3_sls_alc_flow;
}

atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi, uint32_t toi_init) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = NULL;
	atsc3_sls_alc_flow_t* to_check_atsc3_sls_alc_flow = NULL;

	for(int i=0; i < atsc3_sls_alc_flow->count && !matching_atsc3_sls_alc_flow; i++) {
		to_check_atsc3_sls_alc_flow = atsc3_sls_alc_flow->data[i];
		if(to_check_atsc3_sls_alc_flow->tsi == tsi && to_check_atsc3_sls_alc_flow->toi_init == toi_init) {
			matching_atsc3_sls_alc_flow = to_check_atsc3_sls_alc_flow;
		}
	}

	return matching_atsc3_sls_alc_flow;
}


atsc3_sls_alc_flow_t* atsc3_sls_alc_flow_find_entry_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow, uint32_t tsi) {
	atsc3_sls_alc_flow_t* matching_atsc3_sls_alc_flow = NULL;
	atsc3_sls_alc_flow_t* to_check_atsc3_sls_alc_flow = NULL;

	for(int i=0; i < atsc3_sls_alc_flow->count && !matching_atsc3_sls_alc_flow; i++) {
		to_check_atsc3_sls_alc_flow = atsc3_sls_alc_flow->data[i];
		if(to_check_atsc3_sls_alc_flow->tsi == tsi) {
			matching_atsc3_sls_alc_flow = to_check_atsc3_sls_alc_flow;
		}
	}
	if(!matching_atsc3_sls_alc_flow) {
		_ATSC3_SLS_ALC_FLOW_TRACE("atsc3_sls_alc_flow_find_entry_tsi: couldn't find flow in %p, count: %d, tsi: %d\n", atsc3_sls_alc_flow, atsc3_sls_alc_flow->count, tsi);
	}
	return matching_atsc3_sls_alc_flow;
}




void atsc3_route_object_set_alc_flow_and_tsi_toi(atsc3_route_object_t* atsc3_route_object, atsc3_sls_alc_flow_t* atsc3_sls_alc_flow, atsc3_alc_packet_t* atsc3_alc_packet) {
	atsc3_route_object->atsc3_sls_alc_flow = atsc3_sls_alc_flow;

	atsc3_route_object->tsi = atsc3_alc_packet->def_lct_hdr->tsi;
	atsc3_route_object->toi = atsc3_alc_packet->def_lct_hdr->toi;

	//defer any other attributes to the atsc3_route_object_lct_packet_received impl
}

atsc3_route_object_lct_packet_received_t* atsc3_route_object_add_or_update_lct_packet_received(atsc3_route_object_t* atsc3_route_object, atsc3_alc_packet_t* atsc3_alc_packet) {
	atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received = atsc3_route_object_find_lct_packet_received(atsc3_route_object, atsc3_alc_packet);
	if(!atsc3_route_object_lct_packet_received) {
		atsc3_route_object_lct_packet_received = atsc3_route_object_lct_packet_received_new();

		atsc3_route_object_lct_packet_received_set_attributes_from_alc_packet(atsc3_route_object_lct_packet_received, atsc3_alc_packet);
		atsc3_route_object_add_atsc3_route_object_lct_packet_len(atsc3_route_object, atsc3_route_object_lct_packet_received);
		//add this to our atsc3_route_object->atsc3_route_object_lct_packet_received_tree_root_p

		atsc3_route_object_lct_packet_received_t* inserted_atsc3_route_object_lct_packet_received = NULL;

		atsc3_route_object_lct_packet_received_node_t* atsc3_route_object_lct_packet_received_node_to_insert = calloc(1, sizeof(atsc3_route_object_lct_packet_received_node_t));
		if(atsc3_route_object_lct_packet_received->use_sbn_esi) {
			atsc3_route_object_lct_packet_received_node_to_insert->key = atsc3_route_object_lct_packet_received->sbn_esi_merged;
		} else if(atsc3_route_object_lct_packet_received->use_start_offset) {
			atsc3_route_object_lct_packet_received_node_to_insert->key = atsc3_route_object_lct_packet_received->start_offset;
		} else {
			return NULL;
		}

		atsc3_route_object_lct_packet_received_node_to_insert->atsc3_route_object_lct_packet_received = atsc3_route_object_lct_packet_received;

		avltree_insert(&atsc3_route_object_lct_packet_received_node_to_insert->node, &atsc3_route_object->atsc3_route_object_lct_packet_received_tree);


#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
		printf("new atsc3_route_object_lct_packet_received: atsc3_route_object: %p, lct_packet_recv: %p, tsi: %d, toi: %d, start_offset: %d\n",atsc3_route_object, atsc3_route_object_lct_packet_received, atsc3_alc_packet->def_lct_hdr->tsi, atsc3_alc_packet->def_lct_hdr->toi, atsc3_alc_packet->start_offset);
#endif
		atsc3_route_object_add_atsc3_route_object_lct_packet_received(atsc3_route_object, atsc3_route_object_lct_packet_received);

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__
		_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_route_object_add_or_update_lct_packet_received: ADDED, tsi: %d, toi: %d, alc_packet->start_offset: %d, to_check->start_offset: %d",
								atsc3_route_object->tsi, atsc3_route_object->toi,
								atsc3_alc_packet->start_offset,
								atsc3_route_object_lct_packet_received->start_offset);
#endif

	} else {
		//increment our packet carousel count
		atsc3_route_object_lct_packet_received_update_carousel_count(atsc3_route_object_lct_packet_received, atsc3_alc_packet);
		_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_route_object_add_or_update_lct_packet_received: UPDATED, tsi: %d, toi: %d, alc_packet->start_offset: %d, to_check->start_offset: %d, carousel_count: %d",
								atsc3_route_object->tsi, atsc3_route_object->toi,
								atsc3_alc_packet->start_offset,
								atsc3_route_object_lct_packet_received->start_offset,
								atsc3_route_object_lct_packet_received->carousel_count);

	}

	atsc3_route_object_lct_packet_received_update_atsc3_route_object(atsc3_route_object, atsc3_route_object_lct_packet_received);

	return atsc3_route_object_lct_packet_received;
}

/*
 * jjustman-2020-07-28: two ways to solve this o(N^2) operation:
 *
 * 	1.) implement avltree (binary search tree) for our matching atsc3_alc_packet->start_offset and ptr* ref,
 * 		-OR-
 * 	2.)	enforce qsort, and walk the tree backwards...
 *
 */


atsc3_route_object_lct_packet_received_t* atsc3_route_object_find_lct_packet_received(atsc3_route_object_t* atsc3_route_object, atsc3_alc_packet_t* atsc3_alc_packet) {
	atsc3_route_object_lct_packet_received_t* matching_atsc3_route_object_lct_packet_received = NULL;

	//hack-ish...
	//jjustman-2020-08-04 - TODO - fix me...
	atsc3_route_object_lct_packet_received_t* to_find_atsc3_route_object_lct_packet_received = atsc3_route_object_lct_packet_received_new();
	atsc3_route_object_lct_packet_received_set_source_attributes_from_alc_packet(to_find_atsc3_route_object_lct_packet_received, atsc3_alc_packet);

	atsc3_route_object_lct_packet_received_node_t* atsc3_route_object_lct_packet_received_node_to_lookup = calloc(1, sizeof(atsc3_route_object_lct_packet_received_node_t));

	if(to_find_atsc3_route_object_lct_packet_received->use_sbn_esi) {
		atsc3_route_object_lct_packet_received_node_to_lookup->key = to_find_atsc3_route_object_lct_packet_received->sbn_esi_merged;
	} else if(to_find_atsc3_route_object_lct_packet_received->use_start_offset) {
		atsc3_route_object_lct_packet_received_node_to_lookup->key = to_find_atsc3_route_object_lct_packet_received->start_offset;
	} else {
		return NULL;
	}

	struct avltree_node* avltree_node = avltree_lookup(&atsc3_route_object_lct_packet_received_node_to_lookup->node, &atsc3_route_object->atsc3_route_object_lct_packet_received_tree);
	if(avltree_node) {
        atsc3_route_object_lct_packet_received_node_t *p = avltree_container_of(avltree_node, atsc3_route_object_lct_packet_received_node_t, node);
        if(p->atsc3_route_object_lct_packet_received) {
        	matching_atsc3_route_object_lct_packet_received = p->atsc3_route_object_lct_packet_received;
        	_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_route_object_find_lct_packet_received: FOUND MATCHING atsc3_route_object_lct_packet_received?! ptr: %p, tsi: %d, toi: %d, alc_packet->start_offset: %d",
        									matching_atsc3_route_object_lct_packet_received,
        									atsc3_route_object->tsi, atsc3_route_object->toi,
        									atsc3_alc_packet->start_offset);

        }
	}

	freeclean((void**)&to_find_atsc3_route_object_lct_packet_received);
	freeclean((void**)&atsc3_route_object_lct_packet_received_node_to_lookup);

	return matching_atsc3_route_object_lct_packet_received;
}
//	atsc3_route_object_lct_packet_received_t* matching_atsc3_route_object_lct_packet_received = NULL;
//	atsc3_route_object_lct_packet_received_t* to_check_atsc3_route_object_lct_packet_received = NULL;
//
//	//jjustman-2020-07-28 - todo: refactor out this matching logic to use tfind/tsearch
//
//	for(int i=0; i < atsc3_route_object->atsc3_route_object_lct_packet_received_v.count && !matching_atsc3_route_object_lct_packet_received; i++) {
//		to_check_atsc3_route_object_lct_packet_received = atsc3_route_object->atsc3_route_object_lct_packet_received_v.data[i];
//
//		if(atsc3_alc_packet->use_sbn_esi && to_check_atsc3_route_object_lct_packet_received->use_sbn_esi) {
//			if(atsc3_alc_packet->sbn == to_check_atsc3_route_object_lct_packet_received->sbn && atsc3_alc_packet->esi == to_check_atsc3_route_object_lct_packet_received->esi) {
//				matching_atsc3_route_object_lct_packet_received = to_check_atsc3_route_object_lct_packet_received;
//			}
//		} else if(atsc3_alc_packet->use_start_offset && to_check_atsc3_route_object_lct_packet_received->use_start_offset) {
//			if(atsc3_alc_packet->start_offset == to_check_atsc3_route_object_lct_packet_received->start_offset) {
//
//				_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_route_object_find_lct_packet_received: found matching for tsi: %d, toi: %d, alc_packet->start_offset: %d, to_check->start_offset: %d",
//						atsc3_route_object->tsi, atsc3_route_object->toi,
//						atsc3_alc_packet->start_offset,
//						to_check_atsc3_route_object_lct_packet_received->start_offset);
//
//				matching_atsc3_route_object_lct_packet_received = to_check_atsc3_route_object_lct_packet_received;
//			}
//		}
//	}
//
//	return matching_atsc3_route_object_lct_packet_received;


void atsc3_route_object_lct_packet_received_set_source_attributes_from_alc_packet(atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received, atsc3_alc_packet_t* atsc3_alc_packet) {

	atsc3_route_object_lct_packet_received->use_sbn_esi = atsc3_alc_packet->use_sbn_esi;
	atsc3_route_object_lct_packet_received->sbn = atsc3_alc_packet->sbn;
	atsc3_route_object_lct_packet_received->esi = atsc3_alc_packet->esi;
	uint32_t sbn_esi_merged = (atsc3_alc_packet->sbn & 0xFF) << 24 || (atsc3_alc_packet->esi & 0xFFFFFF);

	atsc3_route_object_lct_packet_received->sbn_esi_merged = sbn_esi_merged;

	atsc3_route_object_lct_packet_received->use_start_offset = atsc3_alc_packet->use_start_offset;
	atsc3_route_object_lct_packet_received->start_offset = atsc3_alc_packet->start_offset;
}

void atsc3_route_object_lct_packet_received_set_attributes_from_alc_packet(atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received, atsc3_alc_packet_t* atsc3_alc_packet) {
	atsc3_route_object_lct_packet_received->first_received_timestamp = gtl();
	atsc3_route_object_lct_packet_received->most_recent_received_timestamp = atsc3_route_object_lct_packet_received->first_received_timestamp;
	atsc3_route_object_lct_packet_received->codepoint = atsc3_alc_packet->def_lct_hdr->codepoint;

	atsc3_route_object_lct_packet_received_set_source_attributes_from_alc_packet(atsc3_route_object_lct_packet_received, atsc3_alc_packet);
	//jjustman-2020-07-28 - todo - fixme
	atsc3_route_object_lct_packet_received->fec_encoding_id = atsc3_alc_packet->def_lct_hdr->codepoint;

	atsc3_route_object_lct_packet_received->close_object_flag = atsc3_alc_packet->close_object_flag;
	atsc3_route_object_lct_packet_received->close_session_flag = atsc3_alc_packet->close_session_flag;

	atsc3_route_object_lct_packet_received->ext_route_presentation_ntp_timestamp_set = atsc3_alc_packet->ext_route_presentation_ntp_timestamp_set;
	atsc3_route_object_lct_packet_received->ext_route_presentation_ntp_timestamp = atsc3_alc_packet->ext_route_presentation_ntp_timestamp;

	atsc3_route_object_lct_packet_received->packet_len = atsc3_alc_packet->alc_len;
	atsc3_route_object_lct_packet_received->object_len = atsc3_alc_packet->transfer_len;

#ifdef __ATSC3_ROUTE_OBJECT_PENDANTIC__

	_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_route_object_lct_packet_received_set_attributes_from_alc_packet: atsc3_alc_packet: tsi: %d, toi: %d, use_sbn_esi: %d, use_start_offset: %d, atsc3_route_object_lct_packet_received: use_sbn_esi: %d, use_start_offset: %d",
			atsc3_alc_packet->def_lct_hdr->tsi, atsc3_alc_packet->def_lct_hdr->toi,
			atsc3_alc_packet->use_sbn_esi, atsc3_alc_packet->use_start_offset,
			atsc3_route_object_lct_packet_received->use_sbn_esi, atsc3_route_object_lct_packet_received->use_start_offset);

#endif

}

void atsc3_route_object_lct_packet_received_update_carousel_count(atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received, atsc3_alc_packet_t* atsc3_alc_packet) {
	atsc3_route_object_lct_packet_received->carousel_count++;
	atsc3_route_object_lct_packet_received->most_recent_received_timestamp = gtl();

}

//jjustman-2020-08-04 - TSI=0 is allowed to update object length as it might be a SLS change (e.g.  afdt:efdtVersion="9" changes to  afdt:efdtVersion="10", resulting in being 1 byte longer..)

void atsc3_route_object_lct_packet_received_update_atsc3_route_object(atsc3_route_object_t* atsc3_route_object, atsc3_route_object_lct_packet_received_t* atsc3_route_object_lct_packet_received) {
	if((!atsc3_route_object->object_length || atsc3_route_object->tsi == 0) && atsc3_route_object_lct_packet_received->object_len) {
		atsc3_route_object->object_length = atsc3_route_object_lct_packet_received->object_len;

		//pre_allocate our expected atsc3_route_object->
		//atsc3_route_object->atsc3_route_object_lct_packet_received_v size

		atsc3_route_object_calculate_expected_route_object_lct_packet_count(atsc3_route_object, atsc3_route_object_lct_packet_received);

		atsc3_route_object_prealloc_atsc3_route_object_lct_packet_received(atsc3_route_object, atsc3_route_object->expected_route_object_lct_packet_count);

		_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_route_object_lct_packet_received_update_atsc3_route_object: assigning object_length, route_object: %p, atsc3_alc_packet: tsi: %d, toi: %d, use_sbn_esi: %d, use_start_offset: %d, expected_lct_packet_len_for_count: %d, pre_allocating and setting expected_route_object_lct_packet_count to %d",
				atsc3_route_object,
					atsc3_route_object->tsi, atsc3_route_object->toi,
					atsc3_route_object_lct_packet_received->use_sbn_esi, atsc3_route_object_lct_packet_received->use_start_offset,
					atsc3_route_object->expected_route_object_lct_packet_len_for_count,
					atsc3_route_object->expected_route_object_lct_packet_count);

	} else if(atsc3_route_object->object_length) {
		//over/under by 40 bytes for header...
		if(ABS(atsc3_route_object->expected_route_object_lct_packet_len_for_count - atsc3_route_object_lct_packet_received->packet_len) > 40) {
			//recalc
			atsc3_route_object_calculate_expected_route_object_lct_packet_count(atsc3_route_object, atsc3_route_object_lct_packet_received);

			_ATSC3_SLS_ALC_FLOW_DEBUG("atsc3_route_object_lct_packet_received_update_atsc3_route_object: recalculating, route_object: %p, atsc3_alc_packet: tsi: %d, toi: %d, use_sbn_esi: %d, use_start_offset: %d, expected_lct_packet_len_for_count: %d, updating setting expected_route_object_lct_packet_count to %d",
					atsc3_route_object,
					atsc3_route_object->tsi, atsc3_route_object->toi,
					atsc3_route_object_lct_packet_received->use_sbn_esi, atsc3_route_object_lct_packet_received->use_start_offset,
					atsc3_route_object->expected_route_object_lct_packet_len_for_count,
					atsc3_route_object->expected_route_object_lct_packet_count);
		}
	}

	atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received = atsc3_route_object_lct_packet_received;
}

/*
 *
 * jjustman-2020-08-05 -
 *
 * TODO: buffer up to __ATSC3_ROUTE_OBJECT_PERSIST_BLOCK_SIZE_BYTES__  of block_t before flushing, avoiding most small pre_allocation for small objects
 *
 * object is then flushed and closed to disk with atsc3_route_object_recovery_file_handle_flush_and_close
 *
 *
 */

int atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow(atsc3_route_object_t* atsc3_route_object, atsc3_alc_packet_t* alc_packet, udp_flow_t* udp_flow) {
	int bytesWritten = 0;
	char* temporary_recovery_filename = NULL;
	bool is_source_symbol = false;

	if(!atsc3_route_object || !atsc3_route_object->most_recent_atsc3_route_object_lct_packet_received) {
		_ATSC3_SLS_ALC_FLOW_WARN("atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow: atsc3_route_object: %p, most_recent_atsc3_route_object_lct_packet_received is NULL, discarding!", atsc3_route_object);
		return -2;
	}

	if(!atsc3_route_object->temporary_object_recovery_filename) {
		temporary_recovery_filename = alc_packet_dump_to_object_get_temporary_recovering_filename(udp_flow, alc_packet);
		atsc3_route_object_set_temporary_object_recovery_filename_if_null(atsc3_route_object, temporary_recovery_filename);
		freeclean((void*)&temporary_recovery_filename);
	}

	is_source_symbol = atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position(atsc3_route_object, alc_packet);

	//if we are a repair symbol, skip source symbol recovery and defer processing for raptorQ
	if(!is_source_symbol) {
		return 0;
	}

	int32_t block_remaining_size = block_Remaining_size(atsc3_route_object->recovery_file_buffer);

	//compute gap between atsc3_route_object->recovery_file_buffer_position and our start_offset
	int32_t relative_buffer_write_position = alc_packet->start_offset - atsc3_route_object->recovery_file_buffer_position;
	int32_t updated_file_buffer_position_after_write = alc_packet->start_offset + alc_packet->alc_len;
	int32_t required_buffer_size = updated_file_buffer_position_after_write - atsc3_route_object->recovery_file_buffer_position;

	if(relative_buffer_write_position >= 0) { //this is an "append"
		
		if(block_remaining_size >= required_buffer_size) {		//if we have enough space, write to our block_t
			block_Seek(atsc3_route_object->recovery_file_buffer, relative_buffer_write_position);
			block_Write(atsc3_route_object->recovery_file_buffer, alc_packet->alc_payload, alc_packet->alc_len);
			//note: only set atsc3_route_object->recovery_file_buffer_position) after we have flushed out to disk

			_ATSC3_SLS_ALC_FLOW_INFO("atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow: calling block_Write with alc_packet, tsi: %d, toi: %d, start_offset: %d, len: %d, remaining recovery_file_buffer size: %d, atsc3_route_object->recovery_file_buffer_position: %d",
					alc_packet->def_lct_hdr->tsi,
					alc_packet->def_lct_hdr->toi,
					alc_packet->start_offset,
					alc_packet->alc_len,
					block_Remaining_size(atsc3_route_object->recovery_file_buffer),
					atsc3_route_object->recovery_file_buffer_position);


		} else {
			//otherwise flush out disk and create new recovery_file_buffer and write our alc_packet
			_ATSC3_SLS_ALC_FLOW_INFO("atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow: flushing and reallocing recovery_file_buffer, tsi: %d, toi: %d, start_offset: %d, len: %d, remaining recovery_file_buffer size: %d, required_buffer_size size: %d, alc_len: %d",
								alc_packet->def_lct_hdr->tsi,
								alc_packet->def_lct_hdr->toi,
								alc_packet->start_offset,
								alc_packet->alc_len,
								block_Remaining_size(atsc3_route_object->recovery_file_buffer),
								required_buffer_size,
								alc_packet->alc_len);

			bytesWritten = atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object);
			atsc3_route_object_recovery_file_buffer_ensure_alloc_and_position(atsc3_route_object, alc_packet);
			block_Write(atsc3_route_object->recovery_file_buffer, alc_packet->alc_payload, alc_packet->alc_len);
			atsc3_route_object->recovery_file_buffer_position = updated_file_buffer_position_after_write;

		}
	} else if(relative_buffer_write_position < 0) {
		//we might have a carousel packet, but cheat for our flush to disk
		_ATSC3_SLS_ALC_FLOW_INFO("atsc3_route_object_persist_atsc3_alc_packet_from_udp_flow: buffer_gap_size < 0! flushing and reallocing recovery_file_buffer, tsi: %d, toi: %d, start_offset: %d, len: %d, remaining recovery_file_buffer size: %d, relative_buffer_write_position: %d, alc_len: %d",
										alc_packet->def_lct_hdr->tsi,
										alc_packet->def_lct_hdr->toi,
										alc_packet->start_offset,
										alc_packet->alc_len,
										block_Remaining_size(atsc3_route_object->recovery_file_buffer),
										relative_buffer_write_position,
										alc_packet->alc_len);

		bytesWritten = atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object);
		//hack...!  let fseek do the hard work
		atsc3_route_object->recovery_file_buffer = block_Duplicate_from_ptr(alc_packet->alc_payload, alc_packet->alc_len);
		atsc3_route_object->recovery_file_buffer_position = alc_packet->start_offset;
		bytesWritten = atsc3_route_object_recovery_file_buffer_flush_block_to_temporary_object_recovery_filename(atsc3_route_object);
	}

	return bytesWritten;
}


uint32_t atsc3_sls_alc_flow_get_first_tsi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow) {
	uint32_t matching_tsi = 0;
	if(atsc3_sls_alc_flow->count && atsc3_sls_alc_flow->data[0]) {
		matching_tsi = atsc3_sls_alc_flow->data[0]->tsi;
	}

	return matching_tsi;
}


uint32_t atsc3_sls_alc_flow_get_last_closed_toi(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow) {
	uint32_t matching_last_closed_toi = 0;
	if(atsc3_sls_alc_flow->count && atsc3_sls_alc_flow->data[0]) {
		matching_last_closed_toi = atsc3_sls_alc_flow->data[0]->last_closed_toi;
	}

	return matching_last_closed_toi;
}



uint32_t atsc3_sls_alc_flow_get_first_toi_init(atsc3_sls_alc_flow_v* atsc3_sls_alc_flow) {
	uint32_t matching_toi_init = 0;
	if(atsc3_sls_alc_flow->count && atsc3_sls_alc_flow->data[0]) {
		matching_toi_init = atsc3_sls_alc_flow->data[0]->toi_init;
	}

	return matching_toi_init;
}
