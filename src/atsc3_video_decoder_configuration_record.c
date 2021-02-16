//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_video_decoder_configuration_record.h"

//default atsc3_vector_builder collection for hevc/avc1 nal types

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_vps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_sps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_pps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_prefix_sei);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(hevc_decoder_configuration_record, atsc3_nal_unit_suffix_sei);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_sps);

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(avc1_decoder_configuration_record, atsc3_avc1_nal_unit_pps);

atsc3_video_decoder_configuration_record_t* atsc3_video_decoder_configuration_record_new() {
    atsc3_video_decoder_configuration_record_t* atsc3_video_decoder_configuration_record = calloc(1, sizeof(atsc3_video_decoder_configuration_record_t));
    return atsc3_video_decoder_configuration_record;
}

void atsc3_video_decoder_configuration_record_free(atsc3_video_decoder_configuration_record_t** atsc3_video_decoder_configuration_record_p) {
    if(atsc3_video_decoder_configuration_record_p) {
        atsc3_video_decoder_configuration_record_t* atsc3_video_decoder_configuration_record = *atsc3_video_decoder_configuration_record_p;
        if(atsc3_video_decoder_configuration_record) {
            if(atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record) {
                atsc3_hevc_decoder_configuration_record_free(&atsc3_video_decoder_configuration_record->hevc_decoder_configuration_record);
            }
            if(atsc3_video_decoder_configuration_record->avc1_decoder_configuration_record) {
                atsc3_avc1_decoder_configuration_record_free(&atsc3_video_decoder_configuration_record->avc1_decoder_configuration_record);
            }
            free(atsc3_video_decoder_configuration_record);
            atsc3_video_decoder_configuration_record = NULL;
        }
        *atsc3_video_decoder_configuration_record_p = NULL;
    }
}


/*
* create a new hevc_decoder_configuration_record
* set our configuration_version to 1,
* 	todo: set MSB bitmasks to 1 to match ISO 14496-15:2019 spec
*/
hevc_decoder_configuration_record_t *hevc_decoder_configuration_record_new() {
    hevc_decoder_configuration_record_t *hevc_decoder_configuration_record = calloc(1, sizeof(hevc_decoder_configuration_record_t));
    hevc_decoder_configuration_record->configuration_version = 1;

    return hevc_decoder_configuration_record;
}

void atsc3_hevc_decoder_configuration_record_free(hevc_decoder_configuration_record_t** hevc_decoder_configuration_record_p) {
    if(hevc_decoder_configuration_record_p) {
        hevc_decoder_configuration_record_t* hevc_decoder_configuration_record = *hevc_decoder_configuration_record_p;
        if(hevc_decoder_configuration_record) {
            if(hevc_decoder_configuration_record->box_data_original) {
                block_Destroy(&hevc_decoder_configuration_record->box_data_original);
            }

            hevc_decoder_configuration_record_free_atsc3_nal_unit_vps(hevc_decoder_configuration_record);
            hevc_decoder_configuration_record_free_atsc3_nal_unit_sps(hevc_decoder_configuration_record);
            hevc_decoder_configuration_record_free_atsc3_nal_unit_pps(hevc_decoder_configuration_record);

            hevc_decoder_configuration_record_free_atsc3_nal_unit_prefix_sei(hevc_decoder_configuration_record);
            hevc_decoder_configuration_record_free_atsc3_nal_unit_suffix_sei(hevc_decoder_configuration_record);

            if(hevc_decoder_configuration_record->hevc_nals_combined) {
                block_Destroy(&hevc_decoder_configuration_record->hevc_nals_combined);
            }

            free(hevc_decoder_configuration_record);
            hevc_decoder_configuration_record = NULL;
        }
        *hevc_decoder_configuration_record_p = NULL;
    }
}

/*
 * legacy (read: non ATSC/3.0) avcC SPS/PPS support
 */

avc1_decoder_configuration_record_t *avc1_decoder_configuration_record_new() {
    avc1_decoder_configuration_record_t *avc1_decoder_configuration_record = calloc(1, sizeof(avc1_decoder_configuration_record_t));
    return avc1_decoder_configuration_record;
}

void atsc3_avc1_decoder_configuration_record_free(avc1_decoder_configuration_record_t** avc1_decoder_configuration_record_p) {
    if(avc1_decoder_configuration_record_p) {
        avc1_decoder_configuration_record_t* avc1_decoder_configuration_record = *avc1_decoder_configuration_record_p;
        if(avc1_decoder_configuration_record) {
            avc1_decoder_configuration_record_free_atsc3_avc1_nal_unit_sps(avc1_decoder_configuration_record);
            avc1_decoder_configuration_record_free_atsc3_avc1_nal_unit_pps(avc1_decoder_configuration_record);

            free(avc1_decoder_configuration_record);
            avc1_decoder_configuration_record = NULL;
        }
        *avc1_decoder_configuration_record_p = NULL;
    }
}




/**
 * add default _free methods for each one of these to block_Destroy nal_unit, e.g.:
 *
 * void atsc3_nal_unit_vps_free(atsc3_nal_unit_vps_t** atsc3_nal_unit_vps_p) {
	if(atsc3_nal_unit_vps_p) {
		atsc3_nal_unit_vps_t* atsc3_nal_unit_vps = *atsc3_nal_unit_vps_p;
		if(atsc3_nal_unit_vps) {
			//todo: more impl's as needed
			block_Destroy(&atsc3_nal_unit_vps->nal_unit);

			free(atsc3_nal_unit_vps);
		}
		atsc3_nal_unit_vps = NULL;
		*atsc3_nal_unit_vps_p = NULL;
	}
}

s/nal_unit_vps/nal_unit_sps ...etc
*/

//atsc3_nal_unit_vps_free
void atsc3_nal_unit_vps_free(atsc3_nal_unit_vps_t **atsc3_nal_unit_vps_p) {
    if (atsc3_nal_unit_vps_p) {
        atsc3_nal_unit_vps_t *atsc3_nal_unit_vps = *atsc3_nal_unit_vps_p;
        if (atsc3_nal_unit_vps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_vps->nal_unit);

            free(atsc3_nal_unit_vps);
        }
        atsc3_nal_unit_vps = NULL;
        *atsc3_nal_unit_vps_p = NULL;
    }
}

//atsc3_nal_unit_sps
void atsc3_nal_unit_sps_free(atsc3_nal_unit_sps_t **atsc3_nal_unit_sps_p) {
    if (atsc3_nal_unit_sps_p) {
        atsc3_nal_unit_sps_t *atsc3_nal_unit_sps = *atsc3_nal_unit_sps_p;
        if (atsc3_nal_unit_sps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_sps->nal_unit);

            free(atsc3_nal_unit_sps);
        }
        atsc3_nal_unit_sps = NULL;
        *atsc3_nal_unit_sps_p = NULL;
    }
}

//atsc3_nal_unit_pps
void atsc3_nal_unit_pps_free(atsc3_nal_unit_pps_t **atsc3_nal_unit_pps_p) {
    if (atsc3_nal_unit_pps_p) {
        atsc3_nal_unit_pps_t *atsc3_nal_unit_pps = *atsc3_nal_unit_pps_p;
        if (atsc3_nal_unit_pps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_pps->nal_unit);

            free(atsc3_nal_unit_pps);
        }
        atsc3_nal_unit_pps = NULL;
        *atsc3_nal_unit_pps_p = NULL;
    }
}

//atsc3_nal_unit_prefix_sei
void atsc3_nal_unit_prefix_sei_free(atsc3_nal_unit_prefix_sei_t **atsc3_nal_unit_prefix_sei_p) {
    if (atsc3_nal_unit_prefix_sei_p) {
        atsc3_nal_unit_prefix_sei_t *atsc3_nal_unit_prefix_sei = *atsc3_nal_unit_prefix_sei_p;
        if (atsc3_nal_unit_prefix_sei) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_prefix_sei->nal_unit);

            free(atsc3_nal_unit_prefix_sei);
        }
        atsc3_nal_unit_prefix_sei = NULL;
        *atsc3_nal_unit_prefix_sei_p = NULL;
    }
}

//atsc3_nal_unit_suffix_sei
void atsc3_nal_unit_suffix_sei_free(atsc3_nal_unit_suffix_sei_t **atsc3_nal_unit_suffix_sei_p) {
    if (atsc3_nal_unit_suffix_sei_p) {
        atsc3_nal_unit_suffix_sei_t *atsc3_nal_unit_suffix_sei = *atsc3_nal_unit_suffix_sei_p;
        if (atsc3_nal_unit_suffix_sei) {
//todo: more impl's as needed
            block_Destroy(&atsc3_nal_unit_suffix_sei->nal_unit);

            free(atsc3_nal_unit_suffix_sei);
        }
        atsc3_nal_unit_suffix_sei = NULL;
        *atsc3_nal_unit_suffix_sei_p = NULL;
    }
}


//atsc3_avc1_nal_unit_sps
void atsc3_avc1_nal_unit_sps_free(atsc3_avc1_nal_unit_sps_t **atsc3_avc1_nal_unit_sps_p) {
    if (atsc3_avc1_nal_unit_sps_p) {
        atsc3_avc1_nal_unit_sps_t *atsc3_avc1_nal_unit_sps = *atsc3_avc1_nal_unit_sps_p;
        if (atsc3_avc1_nal_unit_sps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_avc1_nal_unit_sps->nal_unit);

            free(atsc3_avc1_nal_unit_sps);
        }
        atsc3_avc1_nal_unit_sps = NULL;
        *atsc3_avc1_nal_unit_sps_p = NULL;
    }
}

//atsc3_avc1_nal_unit_pps
void atsc3_avc1_nal_unit_pps_free(atsc3_avc1_nal_unit_pps_t **atsc3_avc1_nal_unit_pps_p) {
    if (atsc3_avc1_nal_unit_pps_p) {
        atsc3_avc1_nal_unit_pps_t *atsc3_avc1_nal_unit_pps = *atsc3_avc1_nal_unit_pps_p;
        if (atsc3_avc1_nal_unit_pps) {
//todo: more impl's as needed
            block_Destroy(&atsc3_avc1_nal_unit_pps->nal_unit);

            free(atsc3_avc1_nal_unit_pps);
        }
        atsc3_avc1_nal_unit_pps = NULL;
        *atsc3_avc1_nal_unit_pps_p = NULL;
    }
}
