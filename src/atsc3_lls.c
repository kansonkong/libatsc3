/*
 *
 * atsc3_llt.c:  driver for ATSC 3.0 LLS listener over udp
 *
 *
 * jjustman@ngbp.org
 *
 *
 * Borrowed from A/331 6.3 Service List Table (SLT)
 *
 *
 * 6.3 Service List Table (SLT)
 *  The Service List Table (SLT) is one of the instance types of LLS information.
 *  The function of the SLT is similar to that of the Program Association Table (PAT) in MPEG-2 Systems [33],
 *  and the Fast Information Channel (FIC) found in ATSC A/153, Part 3 [44]. For a receiver first encountering the
 *  broadcast emission, this is the place to start. It supports a rapid channel scan which allows a receiver to
 *  build a list of all the services it can receive, with their channel name, channel number, etc., and it provides
 *  bootstrap information that allows a receiver to discover the SLS for each service. For ROUTE/DASH-delivered services,
 *  the bootstrap information includes the source IP address, the destination IP address and the destination port of the
 *  LCT channel that carries the ROUTE-specific SLS.
 *
 *  For MMTP/MPU-delivered services, the bootstrap information includes the destination IP address and destination
 *  port of the MMTP session carrying the MMTP- specific SLS.
 *
 *
 *
 *  2019-02-19 TODO:
 *
 *  find this leak:
 *
 *  ==42184== 35,776 (1,224 direct, 34,552 indirect) bytes in 51 blocks are definitely lost in loss record 74 of 75
==42184==    at 0x1000E36EA: calloc (in /usr/local/Cellar/valgrind/3.14.0/lib/valgrind/vgpreload_memcheck-amd64-darwin.so)
==42184==    by 0x100001A11: xml_parse_document (xml.c:828)
==42184==    by 0x10000389B: xml_payload_document_parse (atsc3_lls.c:300)
==42184==    by 0x100003685: lls_table_create (atsc3_lls.c:190)
==42184==    by 0x1000011F6: process_packet (atsc3_lls_listener_test.c:212)
==42184==    by 0x100119F60: pcap_read_bpf (in /usr/lib/libpcap.A.dylib)
==42184==    by 0x10011DF82: pcap_loop (in /usr/lib/libpcap.A.dylib)
==42184==    by 0x1000015C0: main (atsc3_lls_listener_test.c:276)
==42184==
 *
 */

#include "atsc3_utils.h"
#include "atsc3_lls.h"
#include "atsc3_lls_slt_parser.h"
#include "xml.h"

#include "atsc3_aeat_parser.h"
#include "atsc3_lls_types.h"
#include "atsc3_cms_utils.h"

int _LLS_INFO_ENABLED  = 0;
int _LLS_DEBUG_ENABLED = 0;
int _LLS_TRACE_ENABLED = 0;

int __DISABLE_LLS_SIGNEDMULTITABLE_VALIDATION__ = 0;

char* LLS_SERVICE_CATEGORY_VALUES[] = {"atsc reserved", "linear av", "linear audio", "app based svc.", "esg service", "eas service", "certificateData", "atsc other" };
//jjustman-2020-03-10: note: 0xFE=>"SignedMultiTable", 0xFF=>"UserDefined"

char* LLS_PROTOCOL_VALUES[] = {"atsc reserved", "ROUTE", "MMTP", "atsc other" };

static atsc3_lls_table_t* __lls_create_base_table_raw(block_t* lls_packet_block) {

    //make sure we have enough remaining data
    if(block_Remaining_size(lls_packet_block) < 4) {
        _LLS_ERROR("__lls_create_base_table_raw: preamble remaining bytes is: %d, need at least 4, returning NULL!", block_Remaining_size(lls_packet_block));
        return NULL;
    }
	//zero out full struct
	atsc3_lls_table_t* base_table = (atsc3_lls_table_t*)calloc(1, sizeof(atsc3_lls_table_t));
	uint8_t lls[4];
	memcpy(&lls, block_Get(lls_packet_block), 4);
	block_Seek_Relative(lls_packet_block, 4);

	//read first 32 bytes in
	base_table->lls_table_id = lls[0];
	base_table->lls_group_id = lls[1];
	base_table->group_count_minus1 = lls[2];
	base_table->lls_table_version = lls[3];

	int remaining_payload_size = __MIN(65536, block_Remaining_size(lls_packet_block));

	if(!remaining_payload_size || remaining_payload_size == 65536) {
	    _LLS_ERROR("__lls_create_base_table_raw: remaining payload size is: %d, invalid, returning NULL!", remaining_payload_size);
	    free(base_table);
	    return NULL;
	}

	//peek and re-assign as necessary for SignedMultiTable
	if(base_table->lls_table_id == SignedMultiTable) {
	    block_t* signed_multi_table_block = block_Duplicate(lls_packet_block);
        uint8_t* signed_multi_table_block_start_lls_payload_count = block_Get(signed_multi_table_block);

        base_table->signed_multi_table.lls_payload_count = block_Read_uint8(signed_multi_table_block);

        _LLS_TRACE("__lls_create_base_table_raw: SignedMultiTable: setting raw_signed_mutli_table block_t p: %p, len: %d, to first 4 hex: 0x%x 0x%x 0x%x 0x%x, payload_count: %d",
                   signed_multi_table_block,
                   signed_multi_table_block->p_size,
                   signed_multi_table_block->p_buffer[0],
                   signed_multi_table_block->p_buffer[1],
                   signed_multi_table_block->p_buffer[2],
                   signed_multi_table_block->p_buffer[3],
                   base_table->signed_multi_table.lls_payload_count);

        _LLS_TRACE("__lls_create_base_table_raw: SignedMultiTable: lls_payload_count is: %d", base_table->signed_multi_table.lls_payload_count);
        for(int i=0; i < base_table->signed_multi_table.lls_payload_count && base_table; i++) {
            //read out lls_payload_id, lls_payload_version and lls_payload_length, and re-cast a block_T buffer for lls_payload() parsing
            atsc3_signed_multi_table_lls_payload_t* lls_payload = atsc3_signed_multi_table_lls_payload_new();
            lls_payload->lls_payload_id = block_Read_uint8(signed_multi_table_block);
            lls_payload->lls_payload_version = block_Read_uint8(signed_multi_table_block);
            lls_payload->lls_payload_length = block_Read_uint16_ntohs(signed_multi_table_block);

            _LLS_DEBUG("__lls_create_base_table_raw: parsed lls_payload[%d], lls_payload_id: %d, lls_payload_version: %d, lls_payload_length: %d",
                       i,
                       lls_payload->lls_payload_id,
                       lls_payload->lls_payload_version,
                       lls_payload->lls_payload_length);

            if(lls_payload->lls_payload_length < block_Remaining_size(signed_multi_table_block)) {
                lls_payload->lls_payload = block_Duplicate_from_position_and_size(signed_multi_table_block, lls_payload->lls_payload_length); //block_Duplicate_from_ptr(block_Get(signed_multi_table_block), lls_payload->lls_payload_length);
				block_Seek_Relative(signed_multi_table_block, lls_payload->lls_payload_length);
                block_Rewind(lls_payload->lls_payload);

				lls_payload->lls_table = atsc3_lls_table_new();
				//jjustman-2022-05-27 - reamp from A/331-2020 and extract our lls payload
				//hack warning
				lls_payload->lls_table->lls_table_id = lls_payload->lls_payload_id; //remapping from a/331 section 6.7
				lls_payload->lls_table->lls_group_id = base_table->lls_group_id; //hack...
				lls_payload->lls_table->group_count_minus1 = base_table->group_count_minus1;
				lls_payload->lls_table->lls_table_version = lls_payload->lls_payload_version;

				int smt_remaining_payload_size = __MIN(65536, block_Remaining_size(lls_payload->lls_payload));

				//copy/paste warning

				uint8_t* temp_gzip_payload = (uint8_t*)calloc(smt_remaining_payload_size, sizeof(uint8_t));
				memcpy(temp_gzip_payload, block_Get(lls_payload->lls_payload), smt_remaining_payload_size);

				lls_payload->lls_table->raw_xml.xml_payload_compressed = temp_gzip_payload;
				lls_payload->lls_table->raw_xml.xml_payload_compressed_size = smt_remaining_payload_size;

				uint8_t *decompressed_payload;
				int32_t ret = atsc3_unzip_gzip_payload(lls_payload->lls_table->raw_xml.xml_payload_compressed, lls_payload->lls_table->raw_xml.xml_payload_compressed_size, &decompressed_payload);

				if(ret > 0) {
					lls_payload->lls_table->raw_xml.xml_payload = decompressed_payload;
					lls_payload->lls_table->raw_xml.xml_payload_size = ret;
					atsc3_signed_multi_table_add_atsc3_signed_multi_table_lls_payload(&base_table->signed_multi_table, lls_payload);
					_LLS_DEBUG("__lls_create_base_table_raw: adding lls_payload[%d], lls_payload_id: %d, lls_payload_version: %d, lls_payload_length: %d",
							   i,
							   lls_payload->lls_payload_id,
							   lls_payload->lls_payload_version,
							   lls_payload->lls_payload_length);
				} else {
                    _LLS_ERROR("__lls_create_base_table_raw: lls_payload[%d], unable to extract raw_xml.xml_payload", i);
					atsc3_lls_table_free(&lls_payload->lls_table);
                }
            } else {
                _LLS_ERROR("__lls_create_base_table_raw: unable to add table[%d], payload_length: %d is too long for remaining: %d, clearing out base_table to NULL",
                           i,
						   lls_payload->lls_payload_length,
                           block_Remaining_size(signed_multi_table_block));

                freeclean((void**)&lls_payload);
                atsc3_signed_multi_table_free_atsc3_signed_multi_table_lls_payload(&base_table->signed_multi_table);
                freeclean((void**)&base_table);
            }
        }

        if(base_table && block_Remaining_size((signed_multi_table_block))) {
            uint8_t* signed_multi_table_block_end_before_signature_length_field = block_Get(signed_multi_table_block);

            base_table->signed_multi_table.signature_length = block_Read_uint16_ntohs(signed_multi_table_block);
            _LLS_TRACE("__lls_create_base_table_raw: SignedMultiTable: signature_length is: %d, remaining bytes: %d",
                       base_table->signed_multi_table.signature_length,
                       block_Remaining_size(signed_multi_table_block));

            if(block_Remaining_size(signed_multi_table_block) != base_table->signed_multi_table.signature_length) {
                _LLS_ERROR("_lls_ceate_base_table_raw: SignedMultiTable: remaining signature length too short! signature_length is: %d, remaining bytes: %d",
                                                  base_table->signed_multi_table.signature_length,
                                                  block_Remaining_size(signed_multi_table_block));
                freeclean((void**)&base_table);
                _LLS_ERROR("returning base_table as: %p", base_table);
            } else {

                //copy from our interior signedMultiTable from lls_payload_count to before signature_length;
                base_table->signed_multi_table.raw_signed_multi_table_for_signature = block_Duplicate_from_ptr(signed_multi_table_block_start_lls_payload_count, (signed_multi_table_block_end_before_signature_length_field - signed_multi_table_block_start_lls_payload_count));
                //this should be our CMS message for verification
                base_table->signed_multi_table.signature = block_Duplicate_from_position(signed_multi_table_block);
                _LLS_TRACE("__lls_create_base_table_raw: SignedMultiTable: signature_length is: %d, signature: %s",
                           base_table->signed_multi_table.signature_length,
                           base_table->signed_multi_table.signature->p_buffer);


			}
        } else {
            _LLS_ERROR("_lls_create_base_table_raw: SignedMultiTable: error finalizing signedMultiTable, base_table: %p, remaining bytes for signature: %d",
                    base_table,
                    block_Remaining_size(signed_multi_table_block));
            freeclean((void**)&base_table);
			_LLS_ERROR("before block_Destroy(signed_multi_table_block) base_table as: %p", base_table);
        }

		if(signed_multi_table_block) {
			block_Destroy(&signed_multi_table_block);
		}
        _LLS_DEBUG("returning base_table as: %p", base_table);
	} else {
        uint8_t *temp_gzip_payload = (uint8_t*)calloc(remaining_payload_size, sizeof(uint8_t));
        memcpy(temp_gzip_payload, block_Get(lls_packet_block), remaining_payload_size);
        block_Seek_Relative(lls_packet_block, remaining_payload_size);

        base_table->raw_xml.xml_payload_compressed = temp_gzip_payload;
        base_table->raw_xml.xml_payload_compressed_size = remaining_payload_size;

        __LLS_SLT_PARSER_TRACE("__lls_create_base_table_raw: XML: first 4 hex: 0x%x 0x%x 0x%x 0x%x", temp_gzip_payload[0], temp_gzip_payload[1], temp_gzip_payload[2], temp_gzip_payload[3]);
	}

	return base_table;
}


bool atsc3_lls_SignedMultiTable_verify_cms_message(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_table_t* lls_table) {
	bool is_signature_valid = false;

	block_Rewind(lls_table->signed_multi_table.raw_signed_multi_table_for_signature);
	//block_Write_to_filename(lls_table->signed_multi_table.raw_signed_multi_table_for_signature, "lls.data");

	block_Rewind(lls_table->signed_multi_table.signature);
	//block_Write_to_filename(lls_table->signed_multi_table.signature, "lls.signature");

	atsc3_cms_entity_t* atsc3_cms_entity = atsc3_cms_entity_new();
	atsc3_cms_entity->signature = block_Duplicate(lls_table->signed_multi_table.signature);
	atsc3_cms_entity->raw_binary_payload = block_Duplicate(lls_table->signed_multi_table.raw_signed_multi_table_for_signature);

	atsc3_cms_validation_context_t* atsc3_cms_validation_context = atsc3_cms_validation_context_new(atsc3_cms_entity);

	if(lls_slt_monitor->lls_latest_certification_data_table) {
		atsc3_cms_validation_context->transients.atsc3_certification_data = &lls_slt_monitor->lls_latest_certification_data_table->certification_data;
	}

	atsc3_cms_validation_context_t* atsc3_cms_validation_context_ret = atsc3_cms_validate_from_context(atsc3_cms_validation_context);

	if(!atsc3_cms_validation_context_ret) {
		_LLS_WARN("!atsc3_cms_validation_context_ret");
		is_signature_valid = false;
	} else {
		_LLS_WARN("atsc3_cms_validation_context_ret is: %p", atsc3_cms_validation_context_ret);

		is_signature_valid = true;
	}
	atsc3_cms_validation_context_free(&atsc3_cms_validation_context);

	return is_signature_valid;
}

atsc3_lls_table_t* lls_create_xml_table(block_t* lls_packet_block) {
	atsc3_lls_table_t *lls_table = __lls_create_base_table_raw(lls_packet_block);
	if(!lls_table) {
        _LLS_ERROR("lls_create_xml_table - error creating instance of LLS XML table, lls_table* is NULL!");
        return NULL;
	}

	//jjustman-2020-03-10 - adding in support for lls_payload_id of 0xFE for SignedMultiTable from our base_table_raw creation

	if(lls_table->lls_table_id == SignedMultiTable) {

	    //special handling here, our child nodes are already gunzip'd
        return lls_table;
	}

	uint8_t *decompressed_payload;
	int32_t ret = atsc3_unzip_gzip_payload(lls_table->raw_xml.xml_payload_compressed, lls_table->raw_xml.xml_payload_compressed_size, &decompressed_payload);

	//make sure we won't overwrite an already extracted xml_payload, e.g. from signedMultiTable interior children..
	if(lls_table->raw_xml.xml_payload) {
		freeclean((void**)&lls_table->raw_xml.xml_payload);
		lls_table->raw_xml.xml_payload_size = 0;
	}

	if(ret > 0) {
		lls_table->raw_xml.xml_payload = decompressed_payload;
		lls_table->raw_xml.xml_payload_size = ret;
		return lls_table;
	}
    
    _LLS_ERROR("lls_create_xml_table - error creating instance of LLS XML table,  lls_table_id: %d, length: %d, lls_group_id: %d, group_count_minus1: %d, lls_table_version: %d",
               lls_table->lls_table_id,
               lls_table->raw_xml.xml_payload_compressed_size,
               lls_table->lls_group_id,
               lls_table->group_count_minus1,
               lls_table->lls_table_version);

    if(lls_table) {
        free(lls_table);
        lls_table = NULL;
    }

	return NULL;
}

/*
 * jjustman-2022-01-19 - NOTE: this method will call lls_slt_table_perform_update, so no need to call it in LLS ip/port detection flow logic
 */
atsc3_lls_table_t* lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor_t* lls_slt_monitor, block_t* lls_packet_block) {
	uint32_t parsed;
	uint32_t parsed_update;
	uint32_t parsed_error;

	atsc3_lls_table_t* lls_table = lls_table_create_or_update_from_lls_slt_monitor_with_metrics(lls_slt_monitor, lls_packet_block, &parsed, &parsed_update, &parsed_error);

	if(lls_table) {
	    if(lls_table->lls_table_id == SignedMultiTable) {
	        for(int i=0; i < lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count; i++) {
	            atsc3_signed_multi_table_lls_payload_t* atsc3_signed_multi_table_lls_payload = lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.data[i];
                atsc3_lls_table_create_or_update_from_lls_slt_monitor_dispatcher(lls_slt_monitor, atsc3_signed_multi_table_lls_payload->lls_table);
	        }

			if(lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count == 0) {
				lls_table_free(&lls_table);

			}
	    } else {
            atsc3_lls_table_create_or_update_from_lls_slt_monitor_dispatcher(lls_slt_monitor, lls_table);
	    }

    }

	return lls_table;
}


atsc3_lls_table_t* atsc3_lls_table_find_type_if_signedMultiTable(atsc3_lls_table_t* atsc3_lls_table_to_check, atsc3_lls_table_type_t atsc3_lls_table_type) {
    atsc3_lls_table_t* atsc3_matching_table_to_return = NULL;
    
    if(!atsc3_lls_table_to_check) {
        return NULL;
    }
    
    if(atsc3_lls_table_to_check->lls_table_id  == atsc3_lls_table_type) {
        return atsc3_lls_table_to_check;
    }
    
    //jjustman-2022-07-12 - hack-ish workaround for non-dispatcher wired up with SignedMultiTable
    if(atsc3_lls_table_to_check->lls_table_id == SignedMultiTable) {
      atsc3_lls_table_t* my_smt_lls_table = NULL;
      for(int i=0; i < atsc3_lls_table_to_check->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count && !atsc3_matching_table_to_return; i++) {
        atsc3_signed_multi_table_lls_payload_t* atsc3_signed_multi_table_lls_payload = atsc3_lls_table_to_check->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.data[i];
          if(atsc3_signed_multi_table_lls_payload->lls_table->lls_table_id == atsc3_lls_table_type) {
              atsc3_matching_table_to_return = atsc3_signed_multi_table_lls_payload->lls_table;
        }
      }
    }
    
    return atsc3_matching_table_to_return;
}

atsc3_lls_table_t* atsc3_lls_table_find_slt_if_signedMultiTable(atsc3_lls_table_t* atsc3_lls_table_to_check) {
    return atsc3_lls_table_find_type_if_signedMultiTable(atsc3_lls_table_to_check, SLT);
}


atsc3_lls_table_t* atsc3_lls_table_create_or_update_from_lls_slt_monitor_dispatcher(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_table_t* lls_table) {
    if(lls_table) {
        switch (lls_table->lls_table_id) {

            case SLT:
                //todo: jjustman-2019-10-12: only re-dispatch for updates?
                if (lls_slt_monitor->atsc3_lls_on_sls_table_present_callback) {
                    lls_slt_monitor->atsc3_lls_on_sls_table_present_callback(lls_table);
                }
                break;

            case RRT:
            	 if (lls_slt_monitor->atsc3_lls_on_rrt_table_present_callback) {
            	     lls_slt_monitor->atsc3_lls_on_rrt_table_present_callback(lls_table);
            	 }
            	 break;

            case SystemTime:
				 if (lls_slt_monitor->atsc3_lls_on_systemtime_table_present_callback) {
					 lls_slt_monitor->atsc3_lls_on_systemtime_table_present_callback(lls_table);
				 }
				 break;

            case AEAT:
				 if (lls_slt_monitor->atsc3_lls_on_aeat_table_present_callback) {
					 lls_slt_monitor->atsc3_lls_on_aeat_table_present_callback(lls_table);
				 }
				 break;

            case OnscreenMessageNotification:
				 if (lls_slt_monitor->atsc3_lls_on_onscreenmessagenotification_table_present_callback) {
					 lls_slt_monitor->atsc3_lls_on_onscreenmessagenotification_table_present_callback(lls_table);
				 }
				 break;

            case CertificationData:
           		 if (lls_slt_monitor->atsc3_lls_on_certificationdata_table_present_callback) {
					 lls_slt_monitor->atsc3_lls_on_certificationdata_table_present_callback(lls_table);
				 }
				 break;

            case SignedMultiTable:
				 if (lls_slt_monitor->atsc3_lls_on_signedmultitable_table_present_callback) {
					 lls_slt_monitor->atsc3_lls_on_signedmultitable_table_present_callback(lls_table);
				 }
				 break;

            case UserDefined:
				 if (lls_slt_monitor->atsc3_lls_on_userdefined_table_present_callback) {
					 lls_slt_monitor->atsc3_lls_on_userdefined_table_present_callback(lls_table);
				 }
				 break;

            default:
                //noop
                break;
        }
    }

    return lls_table;
}

//only return back if lls_table_version has changed

atsc3_lls_table_t* lls_table_create_or_update_from_lls_slt_monitor_with_metrics(lls_slt_monitor_t* lls_slt_monitor, block_t* lls_packet_block, uint32_t* parsed, uint32_t* parsed_update, uint32_t* parsed_error) {

	atsc3_lls_table_t* lls_table_new = __lls_table_create(lls_packet_block);
	if(!lls_table_new) {
		(*parsed_error)++;
		_LLS_ERROR("lls_table_create_or_update_from_lls_slt_monitor_with_metrics: failed to create lls_table_new!")
		return NULL; //parse error or not supported
	}

    if(lls_table_new->lls_table_id != SignedMultiTable) {
        return atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table(lls_slt_monitor, lls_table_new, parsed, parsed_update, parsed_error);
    } else {
		//validate that our SignedMultiTable signature is valid

		if(!__DISABLE_LLS_SIGNEDMULTITABLE_VALIDATION__ &&  !atsc3_lls_SignedMultiTable_verify_cms_message(lls_slt_monitor, lls_table_new)) {
			_LLS_ERROR("lls_table_create_or_update_from_lls_slt_monitor_with_metrics: atsc3_lls_SignedMultiTable_verify_cms_message invalid!");
			lls_table_free(&lls_table_new);
			return NULL;
		}

        _LLS_DEBUG("lls_table_create_or_update_from_lls_slt_monitor_with_metrics: iterating over %d entries", lls_table_new->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count);
        //iterate over our interior tables...
        for(int i=0; i < lls_table_new->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count; i++) {
            atsc3_signed_multi_table_lls_payload_t* lls_table_payload = lls_table_new->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.data[i];
            atsc3_lls_table_t* processed_table = atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table(lls_slt_monitor, lls_table_payload->lls_table, parsed, parsed_update, parsed_error);
			if(!processed_table) {
				//jjustman-2022-05-30 - remove this element if we aren't processed via table update
				atsc3_signed_multi_table_remove_atsc3_signed_multi_table_lls_payload(&lls_table_new->signed_multi_table, lls_table_payload);
				//jjustman-2022-08-24 - hack - set our lls_table_payload->lls_table to null as we are already freed
				lls_table_payload->lls_table = NULL;
				atsc3_signed_multi_table_lls_payload_free(&lls_table_payload);
				i--; //decrement our i position to process the next element (if applicable)

			}
        }

		if(!lls_table_new->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count) {
			lls_table_free(&lls_table_new);
			return NULL;
		} else {
			return lls_table_new;
		}
    }
}

atsc3_lls_table_t* atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_table_t* lls_table_new, uint32_t* parsed, uint32_t* parsed_update, uint32_t* parsed_error) {
	if(!lls_slt_monitor) {
		_LLS_ERROR("atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table: lls_slt_monitor is NULL!");
		return NULL;
	}

    _LLS_DEBUG("atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table: lls_slt_monitor: %p, lls_table_new: %p, lls_table_id: %d", lls_slt_monitor, lls_table_new, lls_table_new->lls_table_id);

    //jjustman-2019-09-18 - TODO: refactor this out from union to *
    if(lls_table_new->lls_table_id == AEAT) {
        if(!lls_slt_monitor->lls_latest_aeat_table) {
            _LLS_INFO_AEAT("Adding new AEAT table reference: lls_table_new->lls_table_version: %d, xml fragment:\n%s", lls_table_new->lls_table_id, lls_table_new->aeat_table.aeat_xml_fragment_latest);
            lls_slt_monitor->lls_latest_aeat_table = lls_table_new;
        } else if(lls_slt_monitor->lls_latest_aeat_table->lls_group_id == lls_table_new->lls_group_id &&
                  lls_slt_monitor->lls_latest_aeat_table->lls_table_version != lls_table_new->lls_table_version) {
            _LLS_INFO_AEAT("Updating new AEAT table reference, lls_slt_monitor->lls_latest_aeat_table->lls_table_version: %d, lls_table_new->lls_table_version: %d, xml fragment:\n%s", lls_slt_monitor->lls_latest_aeat_table->lls_table_version, lls_table_new->lls_table_version, lls_table_new->aeat_table.aeat_xml_fragment_latest);
            lls_table_free(&lls_slt_monitor->lls_latest_aeat_table);
            lls_slt_monitor->lls_latest_aeat_table = lls_table_new;
        } else {
            lls_table_free(&lls_table_new);
        }
        return lls_table_new;
    }
    
    //jjustman-2019-09-18 - TODO: refactor this out from union to *
    if(lls_table_new->lls_table_id == OnscreenMessageNotification) {
        if(!lls_slt_monitor->lls_latest_on_screen_message_notification_table) {
            _LLS_INFO("Adding new OnscreenMessageNotification table reference: %s", lls_table_new->on_screen_message_notification.on_screen_message_notification_xml_fragment_latest);
            lls_slt_monitor->lls_latest_on_screen_message_notification_table = lls_table_new;
        } else if(lls_slt_monitor->lls_latest_on_screen_message_notification_table->lls_group_id == lls_table_new->lls_group_id &&
                  lls_slt_monitor->lls_latest_on_screen_message_notification_table->lls_table_version != lls_table_new->lls_table_version) {
            _LLS_INFO("Updating new OnscreenMessageNotification table reference: %s", lls_table_new->on_screen_message_notification.on_screen_message_notification_xml_fragment_latest);
            
            lls_table_free(&lls_slt_monitor->lls_latest_on_screen_message_notification_table);
            lls_slt_monitor->lls_latest_on_screen_message_notification_table = lls_table_new;
        } else {
            lls_table_free(&lls_table_new);
        }
        return lls_table_new;
    }

	if(lls_table_new->lls_table_id == CertificationData) {
		//persist this CDT information as needed
		if(!lls_slt_monitor->lls_latest_certification_data_table) {
			_LLS_INFO("Adding new CertificationData table reference: %s", lls_table_new->certification_data.raw_certification_data_xml_fragment->p_buffer);
            
            atsc3_lls_slt_monitor_update_latest_certification_data_table_from_lls_table(lls_slt_monitor, lls_table_new);

		} else if(lls_slt_monitor->lls_latest_certification_data_table->lls_group_id == lls_table_new->lls_group_id &&
				  lls_slt_monitor->lls_latest_certification_data_table->lls_table_version != lls_table_new->lls_table_version) {
			_LLS_INFO("Updating new CertificationData table reference: %s", lls_table_new->certification_data.raw_certification_data_xml_fragment->p_buffer);

            atsc3_lls_slt_monitor_update_latest_certification_data_table_from_lls_table(lls_slt_monitor, lls_table_new);

			lls_table_free(&lls_slt_monitor->lls_latest_certification_data_table);
			lls_slt_monitor->lls_latest_certification_data_table = lls_table_new;

		} else {
			lls_table_free(&lls_table_new);
		}
        //jjustman-2022-08-16 - always update our monitors for cert data table, as they are transients
        atsc3_lls_slt_monitor_update_monitors_from_latest_certification_data_table(lls_slt_monitor);
        
		return lls_table_new;
	}
   
    //unhandled lls_table_id
    if(lls_table_new->lls_table_id != SLT) {
        _LLS_INFO("lls_table_create_or_update_from_lls_slt_monitor_with_metrics, ignoring lls_table_id: %d", lls_table_new->lls_table_id);
        
        //jjustman-2022-05-31 - don't free this table early...
		lls_table_free(&lls_table_new);
        return NULL;
    }

	(*parsed)++;
	//check if we should rebuild our signaling, note lls_table_version will roll over at FF
	//TODO: refactor me for event dispatching logic

	/*
	 * jjustman-2020-09-30 - todo:
	 * f(lls_slt_monitor->lls_latest_slt_table->lls_group_id == lls_table_new->lls_group_id &&
                  lls_slt_monitor->lls_latest_aeat_table->lls_table_version != lls_table_new->lls_table_version)
	 */
	if(lls_slt_monitor) {
		if(lls_slt_monitor->lls_latest_slt_table) {
            _LLS_DEBUG("atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table: checking lls_latest_slt_table: %p against %p ", lls_slt_monitor->lls_latest_slt_table, lls_table_new);

            /*
             * jjustman-2020-09-30 remove str comparison of table payload and use the lls_table_version instead
             *      == strncmp((const char*)lls_slt_monitor->lls_latest_slt_table->raw_xml.xml_payload, (const char*)lls_table_new->raw_xml.xml_payload, strlen((const char*)lls_table_new->raw_xml.xml_payload)) != 0
             */
            if(lls_slt_monitor->lls_latest_slt_table->lls_group_id == lls_table_new->lls_group_id &&
                (lls_table_new->lls_table_version > lls_slt_monitor->lls_latest_slt_table->lls_table_version ||
                (lls_table_new->lls_table_version == 0x00 && lls_slt_monitor->lls_latest_slt_table->lls_table_version == 0xFF))) {

                atsc3_lls_table_t* lls_slt_table_to_free = lls_slt_monitor->lls_latest_slt_table;

                //replace our lls_latest_slt_table by freeing it and assigning it new
                _LLS_WARN("atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table: lls_slt_table_to_free: %p (lls_table_version: %d), replacing with latest: %p (lls_table_version: %d)",
                          lls_slt_table_to_free,
                          lls_slt_table_to_free->lls_table_version,
                          lls_table_new,
                          lls_table_new->lls_table_version);


                //jjustman-2020-09-30 - clear our lls_slt_service_id_group_id_cache based upon the lls table group_id
                // from our monitor and force a rebuild
                lls_slt_monitor_free_lls_slt_service_id_group_id_cache_from_lls_group_id(lls_slt_monitor, lls_slt_table_to_free->lls_group_id);

                //perform our lls update first, then free the "old" table once any transient references are nulled out
                lls_slt_table_perform_update(lls_table_new, lls_slt_monitor);
                lls_slt_monitor->lls_latest_slt_table = lls_table_new;

                //free our old table, as all references _SHOULD_ (must have) been replaced or null'd out with lls_slt_table_perform_update
                _LLS_DEBUG("atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table: freeing with lls_slt_table_to_free: %p", lls_slt_table_to_free);
              	lls_table_free(&lls_slt_table_to_free);

            } else {
				//no change - free our new one and keep the old one
				lls_table_free(&lls_table_new);

				return NULL;
			}
		} else {
            _LLS_DEBUG("atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table: assigning new lls_slt_monitor->lls_latest_slt_table: %p", lls_table_new);

            lls_slt_monitor->lls_latest_slt_table = lls_table_new;
            lls_slt_table_perform_update(lls_table_new, lls_slt_monitor);
        }

		(*parsed_update)++;
		return lls_slt_monitor->lls_latest_slt_table;
	} else {
		_LLS_ERROR("lls_slt_monitor is null, can't propagate LLS update!");
	}

    _LLS_ERROR("atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table: returning NULL");

    return NULL;

}
//jjustman-2020-03-10 - todo: refactor me for signedMultiTable support

atsc3_lls_table_t* __lls_table_create(block_t* lls_packet_block) {

    atsc3_lls_table_t *lls_table = lls_create_xml_table(lls_packet_block);
    if(!lls_table) {
        _LLS_ERROR("lls_create_table - error creating instance of LLS table and subclass, return from lls_create_xml_table was null");
        return NULL;
    }

    if(lls_table->lls_table_id != SignedMultiTable) {
        return atsc3_lls_table_parse_raw_xml(lls_table);
    } else {
        //iterate over our interior tables...
        for(int i=0; i < lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count; i++) {
            atsc3_signed_multi_table_lls_payload_t* lls_table_payload = lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.data[i];
			atsc3_lls_table_t* processed_table = atsc3_lls_table_parse_raw_xml(lls_table_payload->lls_table);
			if(!processed_table) {
				//remove this entry as we were not able to parse as a LLS table...
				//jjustman-2022-05-30 - remove this element if we aren't processed via table update
				atsc3_signed_multi_table_remove_atsc3_signed_multi_table_lls_payload(&lls_table->signed_multi_table, lls_table_payload);
				//jjustman-2022-08-24 - hack - set our lls_table_payload->lls_table to null as we are already freed
				lls_table_payload->lls_table = NULL;
				atsc3_signed_multi_table_lls_payload_free(&lls_table_payload);
				i--; //decrement our i position to process the next element (if applicable)
			}
        }

        return lls_table;
    }
}

atsc3_lls_table_t* atsc3_lls_table_parse_raw_xml(atsc3_lls_table_t* lls_table) {
    int res = 0;
    xml_node_t *xml_root_node = NULL;

	if(!lls_table) {
		_LLS_ERROR("lls_create_table - error creating instance of LLS table and subclass, return from lls_create_xml_table was null");
		return NULL;
	}

	//create the xml document payload
	_LLS_DEBUGN("lls_create_table, raw xml payload is: \n%s", lls_table->raw_xml.xml_payload);

	lls_table->xml_document = xml_payload_document_parse(lls_table->raw_xml.xml_payload, lls_table->raw_xml.xml_payload_size);

	if(!lls_table->xml_document) {
		_LLS_ERROR("lls_create_table - unable to parse xml document!  raw xml payload is: size: %u, value:\n%s", lls_table->raw_xml.xml_payload_size, lls_table->raw_xml.xml_payload);
		goto error;
	}

	//extract the root node
	xml_root_node = xml_payload_document_extract_root_node(lls_table->xml_document);
	if(!xml_root_node) {
		_LLS_ERROR("lls_create_table - unable to build xml root nde,  raw xml payload is: size: %u, value:\n%s", lls_table->raw_xml.xml_payload_size, lls_table->raw_xml.xml_payload);

		goto error;
	}
	_LLS_TRACE("lls_create_table: calling lls_create_table_type_instance with xml children count: %zu\n", xml_node_children(xml_root_node));

	res = lls_create_table_type_instance(lls_table, xml_root_node);

	if(res) {
		//unable to instantiate lls_table, go to error
		_LLS_ERROR("lls_table_create: Unable to instantiate lls_table!");
		goto error;
	}

	return lls_table;


error:

	//if we have an xml document, lets force node cleanup here

	lls_table_free(&lls_table);
	lls_table = NULL;

	return NULL;
}

void lls_table_free(atsc3_lls_table_t** lls_table_p) {
	atsc3_lls_table_t* lls_table = *lls_table_p;
	if(!lls_table) {
		_LLS_DEBUG("lls_table_free: lls_table == NULL");
		return;
	}

	//free any instance specific mallocs

	if(lls_table->lls_table_id == SLT) {

        atsc3_lls_slt_table_free_atsc3_slt_ineturl(&lls_table->slt_table);
        atsc3_lls_slt_table_free_atsc3_lls_slt_service(&lls_table->slt_table);
        atsc3_lls_slt_table_free_atsc3_slt_capabilities(&lls_table->slt_table);

        if(lls_table->slt_table.bsid)
			free(lls_table->slt_table.bsid);
        lls_table->slt_table.bsid = NULL;
	} else if(lls_table->lls_table_id == RRT) {
        _LLS_TRACE("free: lls_create_table_type_instance: LLS table RRT not supported yet");
	} else if(lls_table->lls_table_id == SystemTime) {
		freesafe(lls_table->system_time_table.utc_local_offset);
	} else if(lls_table->lls_table_id == AEAT) {
        _LLS_TRACE("free: lls_create_table_type_instance: LLS table AEAT not supported yet");
        //jjustman-2019-09-18 - TODO - move this out of a union into *
        //atsc3_aeat_table_free(atsc3_aeat_table_t **atsc3_aeat_table_p)
		//jjustman-2022-07-19 - TODO: fixme
		if(lls_table->aeat_table.aeat_xml_fragment_latest) {
			free(lls_table->aeat_table.aeat_xml_fragment_latest);
		}
	} else if(lls_table->lls_table_id == OnscreenMessageNotification) {
        _LLS_TRACE("free: lls_create_table_type_instance: LLS table OnscreenMessageNotification not supported yet");
	} else if(lls_table->lls_table_id == CertificationData) {

		for(int i=0; i < lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.count; i++) {
			atsc3_certification_data_to_be_signed_data_certificates_t* atsc3_certification_data_to_be_signed_data_certificates_to_free = lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.data[i];
			if(atsc3_certification_data_to_be_signed_data_certificates_to_free) {
				block_Destroy(&atsc3_certification_data_to_be_signed_data_certificates_to_free->base64_payload);
				freeclean((void**)&lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.data[i]);
			}
		}

		freeclean((void**)&lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.data);
		lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.count = 0;
		lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.size = 0;

		block_Destroy(&lls_table->certification_data.atsc3_certification_data_to_be_signed_data.oscp_refresh_daytimeduration);
		block_Destroy(&lls_table->certification_data.atsc3_certification_data_to_be_signed_data.current_cert_subject_key_identifer);

		if(lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_cert_replacement) {
			block_Destroy(&lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_cert_replacement->next_cert_from_datetime);
			block_Destroy(&lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_cert_replacement->current_cert_until_datetime);
			block_Destroy(&lls_table->certification_data.atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_cert_replacement->next_cert_subject_key_identifier);
		}

		for(int i=0; i < lls_table->certification_data.atsc3_certification_data_oscp_response_v.count; i++) {
			atsc3_certification_data_oscp_response_t* atsc3_certification_data_oscp_response_to_free = lls_table->certification_data.atsc3_certification_data_oscp_response_v.data[i];
			if(atsc3_certification_data_oscp_response_to_free) {
				block_Destroy(&atsc3_certification_data_oscp_response_to_free->base64_payload);
				freeclean((void**)&lls_table->certification_data.atsc3_certification_data_oscp_response_v.data[i]);
			}
		}

		freeclean((void**)&lls_table->certification_data.atsc3_certification_data_oscp_response_v.data);
		lls_table->certification_data.atsc3_certification_data_oscp_response_v.count = 0;
		lls_table->certification_data.atsc3_certification_data_oscp_response_v.size = 0;

		block_Destroy(&lls_table->certification_data.raw_certification_data_xml_fragment);
		block_Destroy(&lls_table->certification_data.cms_signed_data);

	} else if(lls_table->lls_table_id == SignedMultiTable) {
			//free our inner tables as needed
		for(int i=0; i < lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count; i++) {
			atsc3_signed_multi_table_lls_payload_t* atsc3_signed_multi_table_lls_payload = lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.data[i];
			if(atsc3_signed_multi_table_lls_payload) {
				atsc3_signed_multi_table_lls_payload_free(&lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.data[i]);
				freeclean((void**)&lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.data[i]);

			}
		}

		freeclean((void**)&lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.data);
		lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.count = 0;
		lls_table->signed_multi_table.atsc3_signed_multi_table_lls_payload_v.size = 0;

		block_Destroy(&lls_table->signed_multi_table.signature);
		block_Destroy(&lls_table->signed_multi_table.raw_signed_multi_table_for_signature);
	}

    if(lls_table->raw_xml.xml_payload_compressed) {
		free(lls_table->raw_xml.xml_payload_compressed);
		lls_table->raw_xml.xml_payload_compressed = NULL;
	}
	if(lls_table->raw_xml.xml_payload) {
		free(lls_table->raw_xml.xml_payload);
		lls_table->raw_xml.xml_payload = NULL;
	}
	if(lls_table->xml_document) {
		xml_document_free(lls_table->xml_document, false);
		lls_table->xml_document = NULL;
	}

	free(lls_table);
	*lls_table_p = NULL;
}

/**
 * note, caller is responsible for freeing xml_document_type with xml_document_free
 *
 */
xml_document_t* xml_payload_document_parse(uint8_t *xml, int xml_size) {
	xml_document_t* document = xml_parse_document(xml, xml_size);
	if (!document) {
		_LLS_ERROR("xml_payload_document_parse: Could not parse document");
		return NULL;
	}

	return document;
}

//chomp past root xml document declaration
xml_node_t* xml_payload_document_extract_root_node(xml_document_t* document) {

	xml_node_t* root = xml_document_root(document);
	xml_string_t* root_node_name = xml_node_name(root); //root

	if(xml_string_equals_ignore_case(root_node_name, "xml")) {
		root = xml_node_child(root, 0);
		root_node_name = xml_node_name(root); //root
		dump_xml_string(root_node_name);
	} else {
		//_LLS_ERROR("xml_payload_document_extract_root_node: unable to parse out ?xml preamble");
		return NULL;
	}

	_LLS_TRACE("atsc3_lls.c:parse_xml_payload, returning document: %p", root);
	dump_xml_string(root_node_name);
	return root;
}

//caller must free xml_root
int lls_create_table_type_instance(atsc3_lls_table_t* lls_table, xml_node_t* xml_root) {

	xml_string_t* root_node_name = xml_node_name(xml_root); //root

	uint8_t* node_name = xml_string_clone(root_node_name);
	_LLS_TRACE("lls_create_table_type_instance: lls_table_id: %d, node ptr: %p, name is: %s", lls_table->lls_table_id, root_node_name, node_name);

	int ret = -1;
	if(lls_table->lls_table_id == SLT) {
		//build SLT table
		ret = lls_slt_table_build(lls_table, xml_root);
	} else if(lls_table->lls_table_id == RRT) {
        ret = build_rrt_table(lls_table, xml_root);
	} else if(lls_table->lls_table_id == SystemTime) {
		ret = build_system_time_table(lls_table, xml_root);
	} else if(lls_table->lls_table_id == AEAT) {
        ret = atsc3_aeat_table_populate_from_xml(lls_table, xml_root);
	} else if(lls_table->lls_table_id == OnscreenMessageNotification) {
        ret = build_onscreen_message_notification_table(lls_table, xml_root);
	} else if(lls_table->lls_table_id == CertificationData) {
		ret = atsc3_lls_build_certificationdata_table(lls_table, xml_root);
	} else {
		_LLS_ERROR("lls_create_table_type_instance: Unknown LLS table type: %d",  lls_table->lls_table_id);
	}
	_LLS_TRACE("lls_create_table_type_instance: returning ret: %d, lls_table_id: %d, node ptr: %p, name is: %s", ret, lls_table->lls_table_id, root_node_name, node_name);

	freesafe(node_name);

	return ret;
}


/** payload looks like:
 *
 * <SystemTime xmlns="http://www.atsc.org/XMLSchemas/ATSC3/Delivery/SYSTIME/1.0/" currentUtcOffset="37" utcLocalOffset="-PT5H" dsStatus="false"/>
 */
int build_system_time_table(atsc3_lls_table_t* lls_table, xml_node_t* xml_root) {

	int ret = 0;

	xml_string_t* root_node_name = xml_node_name(xml_root); //root
	dump_xml_string(root_node_name);

	uint8_t* SystemTime_attributes = xml_attributes_clone(root_node_name);
	kvp_collection_t* SystemTime_attributes_collecton = kvp_collection_parse(SystemTime_attributes);

	int scratch_i = 0;

	char* currentUtcOffset =	kvp_collection_get(SystemTime_attributes_collecton, "currentUtcOffset");
	char* ptpPrepend = 			kvp_collection_get(SystemTime_attributes_collecton, "ptpPrepend");
	char* leap59 =				kvp_collection_get(SystemTime_attributes_collecton, "leap59");
	char* leap61 = 				kvp_collection_get(SystemTime_attributes_collecton, "leap61");
	char* utcLocalOffset = 		kvp_collection_get(SystemTime_attributes_collecton, "utcLocalOffset");
	char* dsStatus = 			kvp_collection_get(SystemTime_attributes_collecton, "dsStatus");
	char* dsDayOfMonth = 		kvp_collection_get(SystemTime_attributes_collecton, "dsDayOfMonth");
	char* dsHour = 				kvp_collection_get(SystemTime_attributes_collecton, "dsHour");

	if(!currentUtcOffset || !utcLocalOffset) {
		_LLS_ERROR("build_SystemTime_table, required elements missing: currentUtcOffset: %p, utcLocalOffset: %p", currentUtcOffset, utcLocalOffset);
		ret = -1;
		goto cleanup;
	}

	scratch_i = atoi(currentUtcOffset);
	freesafe(currentUtcOffset);

	//munge negative sign
	if(scratch_i < 0) {
		lls_table->system_time_table.current_utc_offset = (1 << 15) | (scratch_i & 0x7FFF);
	} else {
		lls_table->system_time_table.current_utc_offset = scratch_i & 0x7FFF;
	}

	lls_table->system_time_table.utc_local_offset = utcLocalOffset;

	if(ptpPrepend) {
		scratch_i = atoi(ptpPrepend);
		lls_table->system_time_table.ptp_prepend = scratch_i & 0xFFFF;
	}

	if(leap59) {
		lls_table->system_time_table.leap59 = strcasecmp(leap59, "t") == 0;
	}

	if(leap61) {
		lls_table->system_time_table.leap61 = strcasecmp(leap61, "t") == 0;
	}

	if(dsStatus) {
		lls_table->system_time_table.ds_status = strcasecmp(dsStatus, "t") == 0;
		freesafe(dsStatus);
	}

	if(dsDayOfMonth) {
		scratch_i = atoi(dsDayOfMonth);
		lls_table->system_time_table.ds_status = scratch_i & 0xFF;
		freesafe(dsDayOfMonth);
	}

	if(dsHour) {
		scratch_i = atoi(dsHour);
		lls_table->system_time_table.ds_status = scratch_i & 0xFF;
		freesafe(dsHour);
	}

cleanup:
	if(SystemTime_attributes_collecton) {
		kvp_collection_free(SystemTime_attributes_collecton);
	}

	if(SystemTime_attributes) {
		free(SystemTime_attributes);
	}

	return ret;
}

char* lls_get_service_category_value(uint8_t service_category) {
	int lls_service_category_count = sizeof(LLS_SERVICE_CATEGORY_VALUES) / sizeof(char*);
	if(service_category < lls_service_category_count-1) {
		return LLS_SERVICE_CATEGORY_VALUES[service_category];
	} else {
		return LLS_SERVICE_CATEGORY_VALUES[lls_service_category_count-1];
	}
}

char* lls_get_sls_protocol_value(uint8_t protocol) {
	int lls_protocol_count = sizeof(LLS_PROTOCOL_VALUES) / sizeof(char*);
	if(protocol < lls_protocol_count-1) {
		return LLS_PROTOCOL_VALUES[protocol];
	} else {
		return LLS_PROTOCOL_VALUES[lls_protocol_count-1];
	}
}

int build_rrt_table(atsc3_lls_table_t* lls_table, xml_node_t* xml_root) {
    int ret = 0;
    _LLS_WARN("build_rrt_table: NOT IMPLEMENTED");
    return ret;
}

int build_aeat_table(atsc3_lls_table_t* lls_table, xml_node_t* xml_root) {
    int ret = 0;
    _LLS_WARN("build_aeat_table: NOT IMPLEMENTED");
    return ret;
}

int build_onscreen_message_notification_table(atsc3_lls_table_t* lls_table, xml_node_t* xml_root) {
    int ret = 0;
    _LLS_WARN("build_onscreen_message_notification_table: NOT IMPLEMENTED");
    return ret;
}


/*
 * see atsc3lls_types.h for atsc3_certification_data_t
 */
int atsc3_lls_build_certificationdata_table(atsc3_lls_table_t* lls_table, xml_node_t* xml_root) {

    int ret = 0;

	atsc3_certification_data_t*  atsc3_certification_data = &lls_table->certification_data;
    
    atsc3_certification_data->raw_certification_data_xml_fragment = block_Duplicate_from_ptr(lls_table->raw_xml.xml_payload, lls_table->raw_xml.xml_payload_size);
	xml_string_t* root_node_name = xml_node_name(xml_root); //CertificationData
	dump_xml_string(root_node_name);


	int certificationDataNode_count = xml_node_children(xml_root); //ToBeSignedData, CMSSignedData, OSCPResponse
	for(int i=0; i < certificationDataNode_count; i++) {

		xml_node_t* certificationData_child_node = xml_node_child(xml_root, i);
		xml_string_t* certificationData_child_node_xml_string = xml_node_name(certificationData_child_node);

		if(xml_string_equals_ignore_case(certificationData_child_node_xml_string, "ToBeSignedData")) {
			dump_xml_string(certificationData_child_node_xml_string);

			//walk thru Certificates

			int toBeSignedData_child_node_count = xml_node_children(certificationData_child_node);

			for(int j=0; j < toBeSignedData_child_node_count; j++) {
				xml_node_t* child_row_node = xml_node_child(certificationData_child_node, j);
				xml_string_t* child_row_node_xml_string = xml_node_name(child_row_node);
				if(xml_string_equals_ignore_case(child_row_node_xml_string, "Certificates")) {
					//append
					xml_string_t* child_certificate_data = xml_node_content(child_row_node);
					uint8_t* child_certificate_data_string = xml_string_clone(child_certificate_data);

					atsc3_certification_data_to_be_signed_data_certificates_t* atsc3_certification_data_to_be_signed_data_certificates = atsc3_certification_data_to_be_signed_data_certificates_new();
					atsc3_certification_data_to_be_signed_data_certificates->base64_payload = block_Promote((const char *)child_certificate_data_string);

					atsc3_certification_data_to_be_signed_data_add_atsc3_certification_data_to_be_signed_data_certificates(&atsc3_certification_data->atsc3_certification_data_to_be_signed_data, atsc3_certification_data_to_be_signed_data_certificates);

					freesafe(child_certificate_data_string);
                }
			}

		} else if(xml_string_equals_ignore_case(certificationData_child_node_xml_string, "CMSSignedData")) {
			//jjustman-2022-06-06 - TODO

		} else if(xml_string_equals_ignore_case(certificationData_child_node_xml_string, "OCSPResponse")) {
			//jjustman-2022-06-06 - TODO

		} else {
			//unknown
		}
	}
//
//				uint8_t* CertificationData_attributes = xml_attributes_clone(root_node_name);
//	kvp_collection_t* CertificationData_attributes_kvp = kvp_collection_parse(CertificationData_attributes);
//
//	int scratch_i = 0;
//
//	char* currentUtcOffset =	kvp_collection_get(SystemTime_attributes_collecton, "currentUtcOffset");
//	char* ptpPrepend = 			kvp_collection_get(SystemTime_attributes_collecton, "ptpPrepend");
//	char* leap59 =				kvp_collection_get(SystemTime_attributes_collecton, "leap59");
//	char* leap61 = 				kvp_collection_get(SystemTime_attributes_collecton, "leap61");
//	char* utcLocalOffset = 		kvp_collection_get(SystemTime_attributes_collecton, "utcLocalOffset");
//	char* dsStatus = 			kvp_collection_get(SystemTime_attributes_collecton, "dsStatus");
//	char* dsDayOfMonth = 		kvp_collection_get(SystemTime_attributes_collecton, "dsDayOfMonth");
//	char* dsHour = 				kvp_collection_get(SystemTime_attributes_collecton, "dsHour");
//
//	if(!currentUtcOffset || !utcLocalOffset) {
//		_LLS_ERROR("build_SystemTime_table, required elements missing: currentUtcOffset: %p, utcLocalOffset: %p", currentUtcOffset, utcLocalOffset);
//		ret = -1;
//		goto cleanup;
//	}
//
//	scratch_i = atoi(currentUtcOffset);
//	freesafe(currentUtcOffset);
//
//	//munge negative sign
//	if(scratch_i < 0) {
//		lls_table->system_time_table.current_utc_offset = (1 << 15) | (scratch_i & 0x7FFF);
//	} else {
//		lls_table->system_time_table.current_utc_offset = scratch_i & 0x7FFF;
//	}
//
//	lls_table->system_time_table.utc_local_offset = utcLocalOffset;
//
//	if(ptpPrepend) {
//		scratch_i = atoi(ptpPrepend);
//		lls_table->system_time_table.ptp_prepend = scratch_i & 0xFFFF;
//	}
//
//	if(leap59) {
//		lls_table->system_time_table.leap59 = strcasecmp(leap59, "t") == 0;
//	}
//
//	if(leap61) {
//		lls_table->system_time_table.leap61 = strcasecmp(leap61, "t") == 0;
//	}
//
//	if(dsStatus) {
//		lls_table->system_time_table.ds_status = strcasecmp(dsStatus, "t") == 0;
//		freesafe(dsStatus);
//	}
//
//	if(dsDayOfMonth) {
//		scratch_i = atoi(dsDayOfMonth);
//		lls_table->system_time_table.ds_status = scratch_i & 0xFF;
//		freesafe(dsDayOfMonth);
//	}
//
//	if(dsHour) {
//		scratch_i = atoi(dsHour);
//		lls_table->system_time_table.ds_status = scratch_i & 0xFF;
//		freesafe(dsHour);
//	}
//
//	cleanup:
//	if(SystemTime_attributes_collecton) {
//		kvp_collection_free(SystemTime_attributes_collecton);
//	}
//
//	if(SystemTime_attributes) {
//		free(SystemTime_attributes);
//	}

	return ret;
}



void lls_dump_instance_table(atsc3_lls_table_t* base_table) {
	_LLS_TRACE("dump_instance_table: base_table address: %p", base_table);

	_LLS_INFO("");
	_LLS_INFO_I("--------------------------");
	_LLS_INFO_I(" LLS Base Table:");
	_LLS_INFO_I("--------------------------");
	_LLS_INFO_I(" lls_table_id             : %d (0x%x)", base_table->lls_table_id, base_table->lls_table_id);
	_LLS_INFO_I(" lls_group_id             : %d (0x%x)", base_table->lls_group_id, base_table->lls_group_id);
	_LLS_INFO_I(" group_count_minus1       : %d (0x%x)", base_table->group_count_minus1, base_table->group_count_minus1);
	_LLS_INFO_I(" lls_table_version        : %d (0x%x)", base_table->lls_table_version, base_table->lls_table_version);
	_LLS_INFO_I(" xml decoded payload size : %d", 	base_table->raw_xml.xml_payload_size);
	_LLS_INFO_I(" --------------------------");

	if(base_table->raw_xml.xml_payload) {
		_LLS_INFO_I("\t%s", base_table->raw_xml.xml_payload);
	}

	_LLS_INFO_I(" --------------------------");

	if(base_table->lls_table_id == SLT) {

		_LLS_INFO_I("SLT: Service contains %d entries:", base_table->slt_table.atsc3_lls_slt_service_v.count);

		for(int i=0; i < base_table->slt_table.atsc3_lls_slt_service_v.count; i++) {

			atsc3_lls_slt_service_t* service = base_table->slt_table.atsc3_lls_slt_service_v.data[i];

			_LLS_INFO_I(" -----------------------------");
			_LLS_INFO_I("  service_id                  : %d", service->service_id);
			_LLS_INFO_I("  global_service_id           : %s", service->global_service_id);
			_LLS_INFO_I("  major_channel_no            : %d", service->major_channel_no);
			_LLS_INFO_I("  minor_channel_no            : %d", service->minor_channel_no);
			_LLS_INFO_I("  service_category            : %d", service->service_category);
			_LLS_INFO_I("  short_service_name          : %s", service->short_service_name);
			_LLS_INFO_I("  slt_svc_seq_num             : %d", service->slt_svc_seq_num);
			_LLS_INFO_I(" -----------------------------");
			if(service->atsc3_slt_broadcast_svc_signalling_v.count) {

				_LLS_INFO_I("  broadcast_svc_signaling: entry: %d", service->atsc3_slt_broadcast_svc_signalling_v.count);
				atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = service->atsc3_slt_broadcast_svc_signalling_v.data[0];
				_LLS_INFO_I("  -----------------------------");
				_LLS_INFO_I("    sls_protocol              : %d", atsc3_slt_broadcast_svc_signalling->sls_protocol);
				_LLS_INFO_I("    sls_major_protocol_version: %u", atsc3_slt_broadcast_svc_signalling->sls_major_protocol_version);
				_LLS_INFO_I("    sls_minor_protocol_version: %u", atsc3_slt_broadcast_svc_signalling->sls_minor_protocol_version);

				_LLS_INFO_I("    sls_destination_ip_address: %s", atsc3_slt_broadcast_svc_signalling->sls_destination_ip_address);
				_LLS_INFO_I("    sls_destination_udp_port  : %s", atsc3_slt_broadcast_svc_signalling->sls_destination_udp_port);
				_LLS_INFO_I("    sls_source_ip_address     : %s", atsc3_slt_broadcast_svc_signalling->sls_source_ip_address);
			} else {
				_LLS_INFO_I("  broadcast_svc_signaling *NOT PRESENT*");
				_LLS_INFO_I("  -------------------------------------");
			}

		}
		_LLS_DEBUGN("--------------------------");
	}

	//decorate with instance types: hd = int16_t, hu = uint_16t, hhu = uint8_t
	if(base_table->lls_table_id == SystemTime) {
		_LLS_INFO_I(" SystemTime:");
		_LLS_INFO_I(" --------------------------");
		_LLS_INFO_I("  current_utc_offset       : %hd", base_table->system_time_table.current_utc_offset);
		_LLS_INFO_I("  ptp_prepend              : %hu", base_table->system_time_table.ptp_prepend);
		_LLS_INFO_I("  leap59                   : %d",  base_table->system_time_table.leap59);
		_LLS_INFO_I("  leap61                   : %d",  base_table->system_time_table.leap61);
		_LLS_INFO_I("  utc_local_offset         : %s",  base_table->system_time_table.utc_local_offset);

		_LLS_INFO_I("  ds_status                : %d",  base_table->system_time_table.ds_status);
		_LLS_INFO_I("  ds_day_of_month          : %hhu", base_table->system_time_table.ds_day_of_month);
		_LLS_INFO_I("  ds_hour                  : %hhu", base_table->system_time_table.ds_hour);
		_LLS_DEBUGN("--------------------------");

	}
	_LLS_DEBUGN("");

}

void atsc3_lls_slt_monitor_update_latest_certification_data_table_from_lls_table(atsc3_lls_slt_monitor_t* atsc3_lls_slt_monitor, atsc3_lls_table_t* lls_table_new) {
    
    if(atsc3_lls_slt_monitor->lls_latest_certification_data_table) {
        lls_table_free(&atsc3_lls_slt_monitor->lls_latest_certification_data_table);
    }
    
    atsc3_lls_slt_monitor->lls_latest_certification_data_table = lls_table_new;
}

void atsc3_lls_slt_monitor_update_monitors_from_latest_certification_data_table(atsc3_lls_slt_monitor_t* atsc3_lls_slt_monitor) {
    
    for(int i=0; i < atsc3_lls_slt_monitor->lls_sls_alc_monitor_v.count; i++) {
        lls_sls_alc_monitor_t* lls_sls_alc_monitor = atsc3_lls_slt_monitor->lls_sls_alc_monitor_v.data[i];
        lls_sls_alc_monitor->transients.atsc3_certification_data = &atsc3_lls_slt_monitor->lls_latest_certification_data_table->certification_data;
    }
    
    for(int i=0; i < atsc3_lls_slt_monitor->lls_sls_mmt_monitor_v.count; i++) {
        lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = atsc3_lls_slt_monitor->lls_sls_mmt_monitor_v.data[i];
        lls_sls_mmt_monitor->transients.atsc3_certification_data = &atsc3_lls_slt_monitor->lls_latest_certification_data_table->certification_data;
    }
}

