/*
 * atsc3_route_sls_processor.c
 *
 *  Created on: Apr 5, 2019
 *      Author: jjustman
 */


#include "atsc3_route_sls_processor.h"


void atsc3_route_sls_process_from_alc_packet_and_file(alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {

	//check if our toi == 0, if so, reprocess our sls fdt in preperation for an upcoming actual mbms emission

	if(alc_packet->def_lct_hdr->toi == 0) {

	    char* file_name = alc_packet_dump_to_object_get_filename(alc_packet);

		FILE *fp = fopen(file_name, "r");
		if(!fp) {
			_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/0 filename: %s is null!", file_name);
			return;
		}

		xml_document_t* fdt_xml = xml_open_document(fp);
		if(fdt_xml) {
			atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_fdt_instance_parse_from_xml_document(fdt_xml);
			if(!atsc3_fdt_instance) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/0 instance is null!");
				return;
			}
			atsc3_fdt_instance_dump(atsc3_fdt_instance);

		}
	} else {
		//			//keep a reference for our mbms toi

		if(lls_sls_alc_monitor->atsc3_fdt_instance) {
			uint32_t* mbms_toi = atsc3_mbms_envelope_find_toi_from_fdt(lls_sls_alc_monitor->atsc3_fdt_instance);
			if(!mbms_toi) {
				printf("Unable to find MBMS TOI");
				return;
			}

		    char* mbms_toi_filename = alc_packet_dump_to_object_get_filename_tsi_toi(0, *mbms_toi);

			FILE *fp = fopen(mbms_toi_filename, "r");
			if(!fp) {
				_ATSC3_ROUTE_SLS_PROCESSOR_ERROR("alc_packet tsi/toi:0/%u filename: %s is null!", *mbms_toi, mbms_toi_filename);
				return;
			}

			FILE *fp_mbms = fopen(mbms_toi_filename, "r");

			atsc3_sls_metadata_fragments_t* atsc3_sls_metadata_fragments = atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(fp_mbms);

			//update our audio and video tsi and init
			if(atsc3_sls_metadata_fragments) {
				//
			}


		}
	}


}
