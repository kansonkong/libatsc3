/*
 * atsc3_route_mpd.c
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 */


#include "atsc3_route_mpd.h"

int _ROUTE_MPD_PARSER_INFO_ENABLED  = 0;
int _ROUTE_MPD_PARSER_DEBUG_ENABLED = 0;


ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_route_mpd);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_route_period, atsc3_route_adaptation_set);
ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_route_mpd, atsc3_route_period);

//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_route_adaptation_set);
/**
	 char*                                       content_type;
	 char*                                       id;
	 char*                                       max_frame_rate_str;
	 atsc3_frame_rate_t                          max_frame_rate;
	 uint32_t                                    max_height;
	 uint32_t                                    max_width;
	 char*                                       mime_type;
	 char*                                       min_frame_str;

	 atsc3_frame_rate_t                          min_frame_rate;
	 uint32_t                                    min_height;
	 uint32_t                                    min_width;
	 char*                                       par;
	 bool                                        segment_alignment;
	 bool                                        start_with_sap;
	 atsc3_route_role_t                           atsc3_route_role;
	 atsc3_route_representation_t                 atsc3_route_representation;

 */
void atsc3_route_adaptation_set_free(atsc3_route_adaptation_set_t** atsc3_route_adaptation_set_p) {
    if(atsc3_route_adaptation_set_p) {
        atsc3_route_adaptation_set_t* atsc3_route_adaptation_set = *atsc3_route_adaptation_set_p;
        if(atsc3_route_adaptation_set) {
            
            freeclean((void**)&atsc3_route_adaptation_set->content_type);
            freeclean((void**)&atsc3_route_adaptation_set->id);
            freeclean((void**)&atsc3_route_adaptation_set->max_frame_rate_str);

            freeclean((void**)&atsc3_route_adaptation_set->mime_type);
            freeclean((void**)&atsc3_route_adaptation_set->min_frame_str);
            freeclean((void**)&atsc3_route_adaptation_set->par);
            
            //todo: fix me
            freeclean((void**)&atsc3_route_adaptation_set->atsc3_route_role.scheme_id_uri);
            freeclean((void**)&atsc3_route_adaptation_set->atsc3_route_role.value);

            freeclean((void**)&atsc3_route_adaptation_set->atsc3_route_representation.codecs);
            freeclean((void**)&atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_audio_channel_configuration.scheme_id_uri);
            freeclean((void**)&atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_segment_template.initialization);
            freeclean((void**)&atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_segment_template.media);

            free(atsc3_route_adaptation_set);
            atsc3_route_adaptation_set = NULL;
            
        }
        *atsc3_route_adaptation_set_p = NULL;
    }
}

//ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_route_period);
void atsc3_route_period_free(atsc3_route_period_t** atsc3_route_period_p) {
    if(atsc3_route_period_p) {
        atsc3_route_period_t* atsc3_route_period = *atsc3_route_period_p;
        if(atsc3_route_period) {
            freeclean((void**)&atsc3_route_period->id);
            freeclean((void**)&atsc3_route_period->start);
            
            //todo: clean this up
            if(atsc3_route_period->atsc3_xlink) {
                freeclean((void**)&atsc3_route_period->atsc3_xlink->actuate);
                freeclean((void**)&atsc3_route_period->atsc3_xlink->href);
                freeclean((void**)&atsc3_route_period->atsc3_xlink->xlink);
            }
            atsc3_route_period_free_atsc3_route_adaptation_set(atsc3_route_period);
            free(atsc3_route_period);
            atsc3_route_period = NULL;
        }
        *atsc3_route_period_p = NULL;
    }
}

//jjustman-2020-07-27 - todo: change this char* payload to block_t*
atsc3_route_mpd_t* atsc3_route_mpd_parse_from_payload(char* payload, char* content_location) {

	block_t* mpd_fragment_block = block_Promote(payload);
	xml_document_t* xml_document = xml_parse_document(mpd_fragment_block->p_buffer, mpd_fragment_block->i_pos);
	if(!xml_document) {
		return NULL;

	}
	xml_node_t* xml_document_root_node = xml_document_root(xml_document);
	xml_string_t* xml_document_root_node_name = xml_node_name(xml_document_root_node);

	//opening header should be xml
	dump_xml_string(xml_document_root_node_name);
	if(!xml_string_equals_ignore_case(xml_document_root_node_name, "xml")) {
		_ATSC3_ROUTE_MPD_PARSER_ERROR("atsc3_route_mpd_parse_from_payload: opening tag missing xml preamble");
		return NULL;
	}

	atsc3_route_mpd_t* atsc3_route_mpd;

	size_t num_root_children = xml_node_children(xml_document_root_node);
	for(int i=0; i < num_root_children; i++) {
		xml_node_t* root_child = xml_node_child(xml_document_root_node, i);
		xml_string_t* root_child_name = xml_node_name(root_child);

		_ATSC3_ROUTE_MPD_PARSER_INFO("checking root_child tag at: %i, val", i);
		dump_xml_string(root_child_name);

		if(xml_node_equals_ignore_case(root_child, "MPD")) {
			atsc3_route_mpd = atsc3_route_mpd_new();

			//assign any of our attributes here
			uint8_t* xml_attributes = xml_attributes_clone_node(root_child);
			_ATSC3_ROUTE_MPD_PARSER_INFO("mpd.attributes: %s", xml_attributes);
			kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);

			char* matching_attribute = NULL;

			if((matching_attribute = kvp_collection_get(kvp_collection,  "availabilityStartTime"))) {
				atsc3_route_mpd->availability_start_time = matching_attribute;
			}

			//maxSegmentDuration
			if((matching_attribute = kvp_collection_get(kvp_collection,  "maxSegmentDuration"))) {
				atsc3_route_mpd->max_segment_duration = matching_attribute;
			}

			//minBufferTime
			if((matching_attribute = kvp_collection_get(kvp_collection,  "minBufferTime"))) {
				atsc3_route_mpd->min_buffer_time = matching_attribute;
			}

			//minimumUpdatePeriod
			if((matching_attribute = kvp_collection_get(kvp_collection,  "minimumUpdatePeriod"))) {
				atsc3_route_mpd->minimum_update_period = matching_attribute;
			}

			//profiles
			if((matching_attribute = kvp_collection_get(kvp_collection,  "profiles"))) {
				atsc3_route_mpd->profiles = matching_attribute;
			}

			//publishTime
			if((matching_attribute = kvp_collection_get(kvp_collection,  "publishTime"))) {
				atsc3_route_mpd->publish_time = matching_attribute;
			}

			//publishTime
			if((matching_attribute = kvp_collection_get(kvp_collection,  "timeShiftBufferDepth"))) {
				atsc3_route_mpd->time_shift_buffer_depth = matching_attribute;
			}

			//publishTime
			if((matching_attribute = kvp_collection_get(kvp_collection,  "type"))) {
				atsc3_route_mpd->type = matching_attribute;
			}

			size_t num_mpd_entry_row_children = xml_node_children(root_child);
			for(int j=0; j < num_mpd_entry_row_children; j++) {
				xml_node_t* mpd_entry_row_children = xml_node_child(root_child, j);
				if(xml_node_equals_ignore_case(mpd_entry_row_children, "Period")) {
					atsc3_route_mpd_parse_period(mpd_entry_row_children, atsc3_route_mpd);

				}
			}
            free(xml_attributes);
            kvp_collection_free(kvp_collection);
		}
	}
    xml_document_free(xml_document, false);
    block_Destroy(&mpd_fragment_block);
	return atsc3_route_mpd;
}

atsc3_route_mpd_t* atsc3_route_mpd_parse_period(xml_node_t* xml_node, atsc3_route_mpd_t* atsc3_route_mpd) {

	atsc3_route_period_t* atsc3_route_period = atsc3_route_period_new();

	//assign any of our attributes here
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_MPD_PARSER_INFO("period.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "id"))) {
		atsc3_route_period->id = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "start"))) {
		atsc3_route_period->start = matching_attribute;
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "actuate"))) {
		if(!atsc3_route_period->atsc3_xlink) {
			atsc3_route_period->atsc3_xlink = calloc(1, sizeof(atsc3_xlink_t));
		}
		atsc3_route_period->start = matching_attribute;
	}

	atsc3_route_mpd_add_atsc3_route_period(atsc3_route_mpd, atsc3_route_period);

	size_t num_mpd_entry_row_children = xml_node_children(xml_node);
	for(int j=0; j < num_mpd_entry_row_children; j++) {
		xml_node_t* mpd_entry_row_children = xml_node_child(xml_node, j);
		if(xml_node_equals_ignore_case(mpd_entry_row_children, "AdaptationSet")) {
			atsc3_route_mpd_parse_adaption_set(mpd_entry_row_children, atsc3_route_period);
		}
	}
    
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
    
	return atsc3_route_mpd;
}



atsc3_route_period_t* atsc3_route_mpd_parse_adaption_set(xml_node_t* xml_node, atsc3_route_period_t* atsc3_route_period) {

	atsc3_route_adaptation_set_t* atsc3_route_adaptation_set = atsc3_route_adaptation_set_new();
	atsc3_route_period_add_atsc3_route_adaptation_set(atsc3_route_period, atsc3_route_adaptation_set);

	//assign any of our attributes here
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_MPD_PARSER_INFO("adaptationSet.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "contentType"))) {
		atsc3_route_adaptation_set->content_type = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "id"))) {
		atsc3_route_adaptation_set->id = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "maxFrameRate"))) {
		atsc3_route_adaptation_set->max_frame_rate_str = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "maxHeight"))) {
		atsc3_route_adaptation_set->max_height = atoi(matching_attribute);
        free(matching_attribute);
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "maxWidth"))) {
		atsc3_route_adaptation_set->max_width = atoi(matching_attribute);
        free(matching_attribute);
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "mimeType"))) {
		atsc3_route_adaptation_set->mime_type = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "min_frame_str"))) {
		atsc3_route_adaptation_set->min_frame_str = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "min_height"))) {
		atsc3_route_adaptation_set->min_height = atoi(matching_attribute);
        free(matching_attribute);
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "min_width"))) {
		atsc3_route_adaptation_set->min_width = atoi(matching_attribute);
        free(matching_attribute);
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "par"))) {
		atsc3_route_adaptation_set->par = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "segmentAlignment"))) {
		atsc3_route_adaptation_set->segment_alignment = strncmp("true", matching_attribute, 4) == 0;
        free(matching_attribute);
	}
	if((matching_attribute = kvp_collection_get(kvp_collection,  "startWithSAP"))) {
		atsc3_route_adaptation_set->start_with_sap = strncmp("true", matching_attribute, 4) == 0;
        free(matching_attribute);
	}

	_ATSC3_ROUTE_MPD_PARSER_DEBUG("doing Role and Representation startNumber (start_number) to: %p",xml_node );

	size_t num_mpd_entry_row_children = xml_node_children(xml_node);
	for(int j=0; j < num_mpd_entry_row_children; j++) {
		xml_node_t* mpd_entry_row_children = xml_node_child(xml_node, j);
		if(xml_node_equals_ignore_case(mpd_entry_row_children, "Role")) {
			atsc3_route_mpd_parse_role_set(mpd_entry_row_children, atsc3_route_adaptation_set);
		} else if(xml_node_equals_ignore_case(mpd_entry_row_children, "Representation")) {
			atsc3_route_mpd_parse_representation_set(mpd_entry_row_children, atsc3_route_adaptation_set);
		}
	}
    
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_period;
}

atsc3_route_adaptation_set_t* atsc3_route_mpd_parse_role_set(xml_node_t* xml_node, atsc3_route_adaptation_set_t* atsc3_route_adaptation_set) {

	//assign any of our attributes here
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_MPD_PARSER_INFO("role.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "value"))) {
		atsc3_route_adaptation_set->atsc3_route_role.value = matching_attribute;
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "schemeIdUri"))) {
		atsc3_route_adaptation_set->atsc3_route_role.scheme_id_uri = matching_attribute;
	}
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_adaptation_set;
}

atsc3_route_adaptation_set_t* atsc3_route_mpd_parse_representation_set(xml_node_t* xml_node,  atsc3_route_adaptation_set_t* atsc3_route_adaptation_set) {

	//assign any of our attributes here
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_MPD_PARSER_INFO("representationSet.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "audioSamplingRate"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.audio_sampling_rate = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "bandwidth"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.bandwidth = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "codecs"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.codecs = matching_attribute;
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "id"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.id = atoi(matching_attribute);
        free(matching_attribute);
	}

	size_t num_mpd_entry_row_children = xml_node_children(xml_node);
	for(int j=0; j < num_mpd_entry_row_children; j++) {
		xml_node_t* mpd_entry_row_children = xml_node_child(xml_node, j);
		if(xml_node_equals_ignore_case(mpd_entry_row_children, "AudioChannelConfiguration")) {
			atsc3_route_mpd_parse_audio_channel_configuration_set(mpd_entry_row_children, atsc3_route_adaptation_set);
		} else if(xml_node_equals_ignore_case(mpd_entry_row_children, "SegmentTemplate")) {
			atsc3_route_mpd_parse_segment_template_set(mpd_entry_row_children, atsc3_route_adaptation_set);
		}
	}
    
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_adaptation_set;
}

atsc3_route_adaptation_set_t* atsc3_route_mpd_parse_audio_channel_configuration_set(xml_node_t*  xml_node, atsc3_route_adaptation_set_t* atsc3_route_adaptation_set) {
	//assign any of our attributes here via atsc3_route_audio_channel_configuration_t
	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);
	_ATSC3_ROUTE_MPD_PARSER_INFO("AudioChannelConfiguration.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;
	if((matching_attribute = kvp_collection_get(kvp_collection, "schemeIdUri"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_audio_channel_configuration.scheme_id_uri = matching_attribute;
	}
	if((matching_attribute = kvp_collection_get(kvp_collection, "value"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_audio_channel_configuration.value = atoi(matching_attribute);
        free(matching_attribute);
	}
    
    free(xml_attributes);
    kvp_collection_free(kvp_collection);
	return atsc3_route_adaptation_set;
}

/**
 *
 *  <SegmentTemplate
 *  duration="2002000"
 *  initialization="test-$RepresentationID$-init.mp4a"
 *  media="test-$RepresentationID$-$Number$.mp4a"
 *  presentationTimeOffset="2215112000"
 *  startNumber="1119"
 *  timescale="1000000"/>
 */
atsc3_route_adaptation_set_t* atsc3_route_mpd_parse_segment_template_set(xml_node_t* xml_node,  atsc3_route_adaptation_set_t* atsc3_route_adaptation_set) {

	uint8_t* xml_attributes = xml_attributes_clone_node(xml_node);

	_ATSC3_ROUTE_MPD_PARSER_INFO("SegmentTemplate.attributes: %s", xml_attributes);
	kvp_collection_t* kvp_collection = kvp_collection_parse(xml_attributes);
	char* matching_attribute = NULL;

	if((matching_attribute = kvp_collection_get(kvp_collection,  "duration"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_segment_template.duration = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "initialization"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_segment_template.initialization = matching_attribute;
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "media"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_segment_template.media = matching_attribute;
		_ATSC3_ROUTE_MPD_PARSER_DEBUG("setting media (media) to: %s", matching_attribute );
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "presentationTimeOffset"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_segment_template.presentation_time_offset = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "startNumber"))) {
        atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_segment_template.start_number = atoi(matching_attribute);
        free(matching_attribute);
	}

	if((matching_attribute = kvp_collection_get(kvp_collection,  "timescale"))) {
		atsc3_route_adaptation_set->atsc3_route_representation.atsc3_route_segment_template.timescale = atoi(matching_attribute);
        free(matching_attribute);
	}

    free(xml_attributes);
    kvp_collection_free(kvp_collection);
    
	return atsc3_route_adaptation_set;
}


void atsc3_route_mpd_dump(atsc3_route_mpd_t* atsc3_route_mpd) {
	if(!atsc3_route_mpd) {
		_ATSC3_ROUTE_MPD_PARSER_DEBUG("atsc3_route_mpd is null!");
		return;
	}

	_ATSC3_ROUTE_MPD_PARSER_DEBUG("---dumping route dash, size is: %u", atsc3_route_mpd->atsc3_route_period_v.count);
	for(int i=0; i < atsc3_route_mpd->atsc3_route_period_v.count; i++) {
		atsc3_route_period_t* atsc3_route_period = atsc3_route_mpd->atsc3_route_period_v.data[i];

		_ATSC3_ROUTE_MPD_PARSER_DEBUG("---dumping route dash i: %u, id: %s, start: %s", i, atsc3_route_period->id, atsc3_route_period->start);
		//val: %s", i, atsc3_route_period->atsc3_xlink->xlink);

		for(int j=0; j < atsc3_route_period->atsc3_route_adaptation_set_v.count; j++) {
			_ATSC3_ROUTE_MPD_PARSER_DEBUG("Dumping route: %i, init: %s, media: %s",
				j,
				atsc3_route_period->atsc3_route_adaptation_set_v.data[j]->atsc3_route_representation.atsc3_route_segment_template.initialization,
				atsc3_route_period->atsc3_route_adaptation_set_v.data[j]->atsc3_route_representation.atsc3_route_segment_template.media);

		}
	}

}



