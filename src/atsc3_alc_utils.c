/*
 * atsc3_alc_utils.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 *
 *	< https://tools.ietf.org/html/rfc5775 >
 *      4.4.  Receiver Operation

   The receiver operation, when using ALC, includes all the points made
   about the receiver operation when using the LCT building block
   [RFC5651], the FEC building block [RFC5052], and the multiple rate
   congestion control building block.

   To be able to participate in a session, a receiver needs to obtain
   the required Session Description as listed in Section 2.4.  How
   receivers obtain a Session Description is outside the scope of this
   document.

   As described in Section 2.3, a receiver needs to obtain the required
   FEC Object Transmission Information for each object for which the
   receiver receives and processes packets.




Luby, et al.                 Standards Track                   [Page 15]

RFC 5775               ALC Protocol Instantiation             April 2010


   Upon receipt of each packet, the receiver proceeds with the following
   steps in the order listed.

   1.  The receiver MUST parse the packet header and verify that it is a
       valid header.  If it is not valid, then the packet MUST be
       discarded without further processing.

   2.  The receiver MUST verify that the sender IP address together with
       the TSI carried in the header matches one of the (sender IP
       address, TSI) pairs that was received in a Session Description
       and to which the receiver is currently joined.  If there is not a
       match, then the packet MUST be silently discarded without further
       processing.  The remaining steps are performed within the scope
       of the (sender IP address, TSI) session of the received packet.

   3.  The receiver MUST process and act on the CCI field in accordance
       with the multiple rate congestion control building block.

   4.  If more than one object is carried in the session, the receiver
       MUST verify that the TOI carried in the LCT header is valid.  If
       the TOI is not valid, the packet MUST be discarded without
       further processing.

   5.  The receiver SHOULD process the remainder of the packet,
       including interpreting the other header fields appropriately, and
       using the FEC Payload ID and the encoding symbol(s) in the
       payload to reconstruct the corresponding object.

   It is RECOMMENDED that packet authentication be used.  If packet
   authentication is used, then it is RECOMMENDED that the receiver
   immediately check the authenticity of a packet before proceeding with
   step (3) above.  If immediate checking is possible and if the packet
   fails the check, then the receiver MUST silently discard the packet.
 */

#include "atsc3_alc_utils.h"

#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
//shortcut hack
#include "atsc3_isobmff_tools.h"

int _ALC_UTILS_DEBUG_ENABLED=0;
int _ALC_UTILS_TRACE_ENABLED=0;
int _ALC_UTILS_IOTRACE_ENABLED=0;

bool __ALC_RECON_FILE_PTR_HAS_WRITTEN_INIT_BOX = false;

pipe_ffplay_buffer_t* __ALC_RECON_FILE_BUFFER_STRUCT = NULL;
uint32_t* __ALC_RECON_FILE_PTR_TSI = NULL;
uint32_t* __ALC_RECON_FILE_PTR_TOI_INIT = NULL;

FILE* __ALC_RECON_FILE_PTR = NULL; //deprecated

block_t* alc_get_payload_from_filename(char* file_name) {
	if( access(file_name, F_OK ) == -1 ) {
		__ALC_UTILS_ERROR("alc_get_payload_from_filename: unable to open file: %s", file_name);
		return NULL;
	}

	struct stat st;
	stat(file_name, &st);

	//uint8_t* payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
	block_t* payload = block_Alloc(st.st_size);

	FILE* fp = fopen(file_name, "r");
	if(st.st_size == 0) {
		__ALC_UTILS_ERROR("alc_get_payload_from_filename: size: 0 file: %s", file_name);
		return NULL;
	}

	fread(payload->p_buffer, st.st_size, 1, fp);
    payload->i_pos = st.st_size;
	fclose(fp);

	return payload;

}

/* jjustman-2019-09-17: TODO - free temporary filename when done */

char* alc_packet_dump_to_object_get_temporary_filename(udp_flow_t* udp_flow, alc_packet_t* alc_packet) {
	char* temporary_file_name = (char *)calloc(255, sizeof(char));
	if(alc_packet->def_lct_hdr) {
		snprintf(temporary_file_name, 255, "%s%u.%u.%u.%u.%u.%u-%u",
			__ALC_DUMP_OUTPUT_PATH__,
			__toipandportnonstruct(udp_flow->dst_ip_addr, udp_flow->dst_port),
			alc_packet->def_lct_hdr->tsi,
			alc_packet->def_lct_hdr->toi);
	}

	return temporary_file_name;
}


/* jjustman-2019-09-17: TODO - free temporary filename when done */
char* alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow_t* udp_flow, uint32_t tsi, uint32_t toi) {
    char* temporary_file_name = (char *)calloc(255, sizeof(char));
    snprintf(temporary_file_name, 255, "%s%u.%u.%u.%u.%u.%u-%u",
             __ALC_DUMP_OUTPUT_PATH__,
             __toipandportnonstruct(udp_flow->dst_ip_addr, udp_flow->dst_port),
             tsi,
             toi);
    
    return temporary_file_name;
}


/**
 * todo:
 * 	write out header metadata for future app-cache use cases
 *
 *				Content-Location="sgdd.xml"
 * 				Transfer-Length="21595"
                Content-Length="450384"
                Content-Type="application/vnd.oma.bcast.sgdd+xml"
                Content-Encoding="gzip"

and for ATSC A/344:2019 ATSC 3.0 Interactive Content use cases, extend this path mapping with:

		FDT-Instance@appContextIdList

...ESG files available through its HTTP server. The Broadcaster Application should make no assumptions regarding the URL path and simply use it to access the fragment data directly.

The referenced service guide files, in this example, Service.xml, Schedule.xml and Content.xml, shall contain the Service, Schedule and Content XML fragments as described in A/332 [2], respectively.
The Receiver shall extract each XML fragment from the binary SGDU structure before making it available to the Broadcaster Application.
To associate ESG files with Broadcaster Applications, the corresponding Application Context Identifiers shall be provided in the Extended FDT (EFDT) element,
< FDT-Instance@appContextIdList > defined when sending the ESG files in the LCT channel of the ESG Service ROUTE session. Descriptions of the FDT extensions and the ESG Service can be found in A/331 [1].

Application Context Identifiers need not be included in the EFDT if the ESG data is not needed by the Broadcaster Application.
 *
 *
 *
A.3.3.2.6. Extended FDT Instance Semantics
 * *
 *
 *The Extended FDT Instance shall conform to an FDT Instance according to RFC 6726 [30], with the following rules.
 • At least one File element must be present, with the following additional constraints:
 *
 o When exactly one File element is present in an Extended FDT Instance that is embedded in the S-TSID, and the Extended FDT Instance describes DASH Segments as the delivery objects carried by a source flow, that File element will strictly contain the metadata for the Initialization Segment. In other words, no File element instances
 are present for the purpose of describing Media Segments.
 *
 o When more than one File element is present in an Extended FDT Instance that is  embedded in the S-TSID, and the
 * Extended FDT Instance describes NRT content files as the delivery objects carried by a source flow, each of those
 * File elements will contain the metadata for an individual NRT file.
 *
 o When more than one File element is present in an Extended FDT Instance that is transported as TOI=0 in the same
 * LCT channel carrying the associated source flow, the delivery objects transported by the source flow are NRT content
 * files whereby each of those File elements will contain the metadata for an individual NRT file.
 *
 • The @Expires attribute must be present.
 
 When a @fileTemplate attribute is present, then the sender shall operate as follows:
 • The TOI field in the ROUTE packet header shall be set such that Content-Location can be derived according to Section A.3.3.2.7.
 • After sending the first packet with a given TOI value, none of the packets pertaining to this TOI shall be sent later
 * than the wall clock time as derived from @maxExpiresDelta.
 *
 * In addition, the EXT_TIME header with Expected Residual Time (ERT) may be used in order to convey more accurate expiry time,
 * if considered useful. When @maxExpiresDelta is not present, then the EXT_TIME header with Expected Residual Time (ERT)
 * shall be used to derive the value of FDT-Instance@Expires, according to the procedure described below in Section A.3.3.2.7.
 
 When a @fileTemplate attribute is present, an Extended FDT Instance is produced at the receiver as follows:
 • Any data that is contained in the EFDT may be used as is in generating an Extended FDT Instance.
 • The data in the @fileTemplate attribute is used to generate the file URI (equivalent to the File@Content-Location in the FDT)
 * as documented in Section A.3.3.2.7 with the reception of an LCT packet with a specific TOI value.
 *
 *
 * A.3.3.2.7. File Template
  When an LCT packet with a new TOI is received for this transport session, then an Extended FDT Instance is
 generated with a new File entry as follows:
 
 • The TOI is used to generate File@Content-Location using the mechanism defined in Section A.3.3.2.8.
 • All other attributes that are present in the EFDT.FDT-Instance element are applicable to the File.
 • Either the EXT_FTI header (per RFC 5775 [27]) or the EXT_TOL header (per Section A.3.8.1), when present,
    shall be used to signal the Transport Object Length (TOL) of the File.
 *
 * If the File@Transfer-Length parameter in the Extended FDT Instance is not present,
 * then the EXT_TOL header or the or EXT_FTI header shall be present.
 *
 * Note that a header containing the transport object length (EXT_TOL or EXT_FTI) need not be present in each packet header.
 *
 * If the broadcaster does not know the length of the transport object at the beginning of the transfer,
 * an EXT_TOL or EXT_FTI header shall be included in at least the last packet of the file and should be included in the last
 * few packets of the transfer.
 *
 • When present, the @maxExpiresDelta shall be used to generate the value of the FDT- Instance@Expires attribute.
 * The receiver is expected to add this value to its wall clock time when acquiring the first ROUTE packet carrying the
 * data of a given delivery object to obtain the value for @Expires.
 *
 * When @maxExpiresDelta is not present, the* EXT_TIME header with Expected Residual Time (ERT) shall be used to derive the
 * expiry time of the Extended FDT Instance.
 *
 * When both @maxExpiresDelta and the ERT of EXT_TIME are present, the smaller of the two values should be used as the
 * incremental time interval to be added to the receiver’s current time to generate the effective value for @Expires.
 *
 * When neither @maxExpiresDelta nor the ERT field of the EXT_TIME header is present, then the expiration time of the
 * Extended FDT Instance is given by its @Expires attribute.

 A.3.3.2.8. Substitution
 The @fileTemplate attribute, when present, shall include the “$TOI$” identifier.
 After parameter substitution using the TOI number in this transport session, the
 @fileTemplate shall be a valid URL corresponding to the Content-Location attribute of the associated file.
 Excluding the TOI values associated with any files listed in FDT-Instance.File elements, the
 @fileTemplate attribute generates a one-to-one mapping between the TOI and the Content-Location value.
 When the @fileTemplate is used to identify a sequence of DASH Media Segments, the Segment number is equal to the TOI value
 
 In each URI, the identifiers from Table A.3.5 shall be replaced by the substitution parameter defined in Table A.3.5.
 
 Identifier matching is case-sensitive. If the URI contains unescaped $ symbols which do not enclose a valid identifier,
 then the result of URI formation is undefined.
 
 The format of the identifier is also specified in Table A.3.5.
 
 Each identifier may be suffixed, within the enclosing ‘$’ characters following this prototype:
    %0[width]d
 
 The width parameter is an unsigned integer that provides the minimum number of characters to be printed.
 If the value to be printed is shorter than this number, the result shall be padded with leading zeroes.
 The value is not truncated even if the result is larger.
 *
 An example @fileTemplate using a width of 5 is: fileTemplate="myVideo$TOI%05d$.mps",
 resulting in file names with exactly five digits in the number portion.
 
 The Media Segment file name for TOI=33 using this template is myVideo00033.mps.
 
 The @fileTemplate shall be authored such that the application of the substitution process results in valid URIs.
 Strings outside identifiers shall only contain characters that are permitted within URIs according to RFC 3986 [19].
 *
 *
    Table A.3.5 Identifiers for File Templates
    ----------------------------------------------------------------------------------
    $<Identifier>$           Substitution Parameter                             Format
    --------------           ------------------------------------------------   ------
    $$                       Is an escape sequence, i.e. "$$" is non-           not applicable
                             recursively replaced with a single "$"

    $TOI$                    This identifier is substituted with the TOI.       The format tag may be present.
                                                                                When no format tag is present, a default format
                                                                                tag with width=1 shall be used.
    
 *
 *TODO: check codepoint if we are in entity mode...
 */

char* alc_packet_dump_to_object_get_s_tsid_filename(udp_flow_t* udp_flow, alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {

	//jjustman-2019-09-07: TODO: expand context for application cache and header attributes for object caching
	char* content_location = NULL;
	char* content_type = NULL;
	char* content_encoding = NULL;
	uint32_t content_length;
	uint32_t transfer_length;

	if(lls_sls_alc_monitor->atsc3_sls_metadata_fragments && lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid && lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count) {
		for(int i=0; i < lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count; i++) {
			atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS = lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.data[i];

			if(atsc3_route_s_tsid_RS->dest_ip_addr == udp_flow->dst_ip_addr && atsc3_route_s_tsid_RS->dest_port == udp_flow->dst_port && atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.count) {
				for(int j=0; j < atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.count; j++) {
					atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.data[j];

					if(atsc3_route_s_tsid_RS_LS->tsi == alc_packet->def_lct_hdr->tsi && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow) {

						//try to find our matching toi and content-location value
						if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance->atsc3_fdt_file_v.count) {
                            atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance;
                            for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
								atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];
                                //if toi matches, then use this mapping, otherwise, fallback to file_template
								if(atsc3_fdt_file->toi == alc_packet->def_lct_hdr->toi && atsc3_fdt_file->content_location && strlen(atsc3_fdt_file->content_location)) {
                                    size_t content_location_length = strlen(atsc3_fdt_file->content_location);
                                    content_location = calloc(content_location_length + 1, sizeof(char));
                                    strncpy(content_location, atsc3_fdt_file->content_location, content_location_length);
                                    
                                    //TODO: jjustman-2019-09-18 -  apply mappings from FLUTE to HTTP object caching here
//                                    atsc3_fdt_file->content_type;
//                                    atsc3_fdt_file->content_length;
//                                    atsc3_fdt_file->content_encoding;
//                                    atsc3_fdt_file->transfer_length;
                                    
                                }
							}
                            
                            if(!content_location) {
                                //fallback to instance template
                                if(atsc3_fdt_instance->file_template) {
                                    int file_template_strlen = strlen(atsc3_fdt_instance->file_template);
                                    char intermediate_file_name[1025] = { 0 }; //include null padding
                                    int intermediate_pos = 0;
                                    char* final_file_name = calloc(1025, sizeof(char));
                                    
                                    //replace $$ to $
                                    //replace $TOI$ (and width formatting, e.g. $TOI%05d$) with our TOI
                                    for(int i=0; i < file_template_strlen && i < 1024; i++) {
                                        if(atsc3_fdt_instance->file_template[i] == '$') {
                                            if(atsc3_fdt_instance->file_template[i+1] == '$') {
                                                //escape
                                                intermediate_file_name[intermediate_pos++] = '$';
                                                i++;
                                            } else if(i+4 < file_template_strlen &&
                                                      atsc3_fdt_instance->file_template[i+1] == 'T' &&
                                                      atsc3_fdt_instance->file_template[i+2] == 'O' &&
                                                      atsc3_fdt_instance->file_template[i+3] == 'I') { //next 3 chars should be TOI at least
                                                if(atsc3_fdt_instance->file_template[i+4] == '$') {
                                                    //close out with just a %d value
                                                    intermediate_file_name[intermediate_pos++] = '%';
                                                    intermediate_file_name[intermediate_pos++] = 'd';
                                                    i += 4;
                                                    __ALC_UTILS_DEBUG("intermediate file template name after TOI property substituion is: %s", intermediate_file_name);

                                                } else if(atsc3_fdt_instance->file_template[i+4] == '%') {
                                                    i += 4;
                                                    //copy over our formatting until we get to a $
                                                    //e.g. myVideo$TOI%05d$.mps
                                                    while(i < file_template_strlen && atsc3_fdt_instance->file_template[i] != '$') {
                                                        intermediate_file_name[intermediate_pos++] = atsc3_fdt_instance->file_template[i++];
                                                    }
                                                    __ALC_UTILS_DEBUG("intermediate file template name after TOI width substitution is: %s", intermediate_file_name);
                                                 
                                                } else {
                                                    __ALC_UTILS_WARN("file template name at pos: %d doesn't match template value of TOI: %s, ignoring...", i, atsc3_fdt_instance->file_template);
                                                }
                                            } else {
                                                __ALC_UTILS_WARN("file template name at pos: %d doesn't match template value of TOI: %s, ignoring...", i, atsc3_fdt_instance->file_template);
                                            }
                                        } else {
                                            intermediate_file_name[intermediate_pos++] = atsc3_fdt_instance->file_template[i];
                                        }
                                    }
                                
                                    //perform final replacement
                                    snprintf(final_file_name, 1024, intermediate_file_name, alc_packet->def_lct_hdr->toi);
                                    content_location = final_file_name;
                                    __ALC_UTILS_DEBUG("final file template name after TOI substitution is: %s", content_location);
                                }
                            }
                           
						} else {

							//alternative strategies for content-location here?
						}
					}
				}
			}
		}
	}

	if(!content_location) {

		if(alc_packet->def_lct_hdr) {
            content_location = alc_packet_dump_to_object_get_temporary_filename(udp_flow, alc_packet);

			__ALC_UTILS_INFO("alc_packet_dump_to_object_get_s_tsid_filename: no content_location to return for alc_packet: %p, falling back to %s", alc_packet, content_location);
		} else {
			__ALC_UTILS_ERROR("alc_packet_dump_to_object_get_s_tsid_filename: no content_location to return for alc_packet: %p, falling back to null string!", alc_packet);
		}
	}

	return content_location;
}

//todo - build this in memory first...

FILE* alc_object_open_or_pre_allocate(char* file_name, alc_packet_t* alc_packet) {
    if( access( file_name, F_OK ) != -1 ) {
        FILE* f = fopen(file_name, "r+");
        if(f) {
            return f;
        }
    }
    
    //otherwise, pre_allocate this object
    return alc_object_pre_allocate(file_name, alc_packet);
    
}

//nothing to see here...
uint8_t* __TO_PREALLOC_ZERO_SLAB_PTR = NULL;
FILE* alc_object_pre_allocate(char* file_name, alc_packet_t* alc_packet) {
	if(!__TO_PREALLOC_ZERO_SLAB_PTR) {
		__TO_PREALLOC_ZERO_SLAB_PTR = (uint8_t*)malloc(__TO_PREALLOC_ZERO_SLAB_SIZE);
		memset(__TO_PREALLOC_ZERO_SLAB_PTR, 0, __TO_PREALLOC_ZERO_SLAB_SIZE);
	}

    if( access( file_name, F_OK ) != -1 ) {
    	__ALC_UTILS_IOTRACE("pre_allocate: file %s exists, removing", file_name);
        //__ALC_UTILS_WARN("pre_allocate: file %s exists, removing", file_name);
        // file exists
        remove(file_name);
    }
    
    FILE* f = fopen(file_name, "w");
    if(!f) {
        __ALC_UTILS_WARN("pre_allocate: unable to open %s", file_name);
        return NULL;
    }
    
    uint32_t to_allocate_size = alc_packet->transfer_len;
    if(to_allocate_size) {
    	__ALC_UTILS_IOTRACE("pre_allocate: before: file %s to size: %d", file_name, to_allocate_size);
        uint32_t alloc_offset = 0;
        uint32_t blocksize;
        uint32_t loop_count = 0;
        while(alloc_offset < to_allocate_size) {
        	blocksize = __MIN(__TO_PREALLOC_ZERO_SLAB_SIZE, to_allocate_size - alloc_offset);
            fwrite(__TO_PREALLOC_ZERO_SLAB_PTR, blocksize, 1, f);
            alloc_offset += blocksize;
            loop_count++;
        }
        __ALC_UTILS_IOTRACE("pre_allocate: after: file %s to size: %d, wrote out: %u in %u fwrite", file_name, to_allocate_size, alloc_offset, loop_count);

    } else {
        __ALC_UTILS_WARN("pre_allocate: file %s, transfer_len is 0, not pre allocating", file_name);
    }
    fclose(f);
    f = fopen(file_name, "r+");
   
    return f;
}

int alc_packet_write_fragment(FILE* f, char* file_name, uint32_t offset, alc_packet_t* alc_packet) {
    
	__ALC_UTILS_IOTRACE("write fragment: tsi: %u, toi: %u, sbn: %x, esi: %x len: %d, complete: %d, file: %p, file name: %s, offset: %u, size: %u",  alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi,
        alc_packet->sbn, alc_packet->esi, alc_packet->alc_len, alc_packet->close_object_flag,
        f, file_name, offset, alc_packet->alc_len);

    fseek(f, offset, SEEK_SET);
    int blocks_written = fwrite(alc_packet->alc_payload, alc_packet->alc_len, 1, f);
   
    if(blocks_written != 1) {
        __ALC_UTILS_WARN("short packet write: blocks: %u", blocks_written);
        return 0;
    }
    
    return alc_packet->alc_len;
}

int alc_packet_dump_to_object(udp_flow_t* udp_flow, alc_packet_t** alc_packet_ptr, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {

	alc_packet_t* alc_packet = *alc_packet_ptr;
	int bytesWritten = 0;

    if(lls_sls_alc_monitor && !lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled) {
        return -1;
    }

    char* temporary_filename = alc_packet_dump_to_object_get_temporary_filename(udp_flow, alc_packet);
    char* s_tsid_content_location = NULL;
    
    mkdir("route", 0777);

    FILE *f = NULL;

    if(alc_packet->use_sbn_esi) {
        //raptor fec, use the esi to see if we should write out to a new file vs append
        if(!alc_packet->esi) {
            f = alc_object_pre_allocate(temporary_filename, alc_packet);
            __ALC_UTILS_IOTRACE("raptor_fec: done creating new pre-allocation for temporary_filename: %s, size: %llu", temporary_filename, alc_packet->transfer_len);
        } else {
            f = alc_object_open_or_pre_allocate(temporary_filename, alc_packet);
        }
        if(!f) {
            __ALC_UTILS_WARN("alc_packet_dump_to_object, unable to open temporary_filename: %s", temporary_filename);
            return -2;
        }
        alc_packet_write_fragment(f, temporary_filename, alc_packet->esi, alc_packet);
        __ALC_UTILS_IOTRACE("raptor_fec: done writing out fragment for %s", temporary_filename);

    } else if(alc_packet->use_start_offset) {
        if(!alc_packet->start_offset) {
            f = alc_object_pre_allocate(temporary_filename, alc_packet);
            __ALC_UTILS_IOTRACE("ALC: tsi: %u, toi: %u, done creating new pre-allocation temporary_filename %s, size: %llu",
            		alc_packet->def_lct_hdr->tsi,
					alc_packet->def_lct_hdr->toi,
					temporary_filename,
					alc_packet->transfer_len);

        } else {
            __ALC_UTILS_IOTRACE("ALC: tsi: %u, toi: %u, using existing pre-alloc temporary_filename %s, offset: %u, size: %llu",
            		alc_packet->def_lct_hdr->tsi,
					alc_packet->def_lct_hdr->toi,
					temporary_filename,
					alc_packet->start_offset,
					alc_packet->transfer_len);

            f = alc_object_open_or_pre_allocate(temporary_filename, alc_packet);
        }
        if(!f) {
            __ALC_UTILS_WARN("alc_packet_dump_to_object, unable to open file: %s", temporary_filename);
            return -2;
        }
        
        alc_packet_write_fragment(f, temporary_filename, alc_packet->start_offset, alc_packet);
        __ALC_UTILS_IOTRACE("done writing out temporary_filename for %s", temporary_filename);

    } else {
        __ALC_UTILS_WARN("alc_packet_dump_to_object, no alc offset strategy for temporary_filename: %s", temporary_filename);
    }
	
    if(f) {
        fclose(f);
        f = NULL;
    }
    
    //both codepoint=0 and codepoint=128 will set close_object_flag when we have finished delivery of the object
	if(alc_packet->close_object_flag) {
        s_tsid_content_location = alc_packet_dump_to_object_get_s_tsid_filename(udp_flow, alc_packet, lls_sls_alc_monitor);
 
        if(0 != strncmp(temporary_filename, s_tsid_content_location, __MIN(strlen(temporary_filename), strlen(s_tsid_content_location)))) {
            char new_file_name[1024] = { 0 };
            snprintf(new_file_name, 1024, "route/%d", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
            mkdir(new_file_name, 0777);
            snprintf(new_file_name, 1024, "%s/%s", new_file_name, s_tsid_content_location);
            
            rename(temporary_filename, new_file_name);
            __ALC_UTILS_IOTRACE("tsi: %u, toi: %u, moving from to temporary_filename: %s to: %s, is complete: %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi,  temporary_filename, new_file_name, alc_packet->close_object_flag);
        }
        //update our sls here if we have a service we are listenting to
		if(lls_sls_alc_monitor && lls_sls_alc_monitor->atsc3_lls_slt_service &&  alc_packet->def_lct_hdr->tsi == 0) {
			__ALC_UTILS_IOTRACE("ALC: service_id: %u, ------ TSI of 0, calling atsc3_route_sls_process_from_alc_packet_and_file", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id);
			atsc3_route_sls_process_from_alc_packet_and_file(udp_flow, alc_packet, lls_sls_alc_monitor);

		} else {
            //jjustman-2019-11-02: todo: remove this old code
			//only push to our output buffer video and audio flows
			if(lls_sls_alc_monitor && (alc_packet->def_lct_hdr->tsi == lls_sls_alc_monitor->audio_tsi || alc_packet->def_lct_hdr->tsi == lls_sls_alc_monitor->video_tsi)) {
				__ALC_UTILS_IOTRACE("------ TSI of %d, toi: %u, calling alc_recon_file_buffer_struct_monitor_fragment_with_init_box", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
				alc_recon_file_buffer_struct_monitor_fragment_with_init_box(udp_flow, alc_packet, lls_sls_alc_monitor);
			} else {
				s_tsid_content_location = alc_packet_dump_to_object_get_s_tsid_filename(udp_flow, alc_packet, lls_sls_alc_monitor);
				if(s_tsid_content_location && strlen(s_tsid_content_location)) {
					__ALC_UTILS_INFO("tsi: %u, toi: %u, not video or audio payload, using for NRT caching at s_tsid_content_location: %s", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, s_tsid_content_location);
					rename(temporary_filename, s_tsid_content_location);
				} else {
					__ALC_UTILS_ERROR("tsi: %u, toi: %u, not video or audio payload and no content_location target!", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);

				}
			}
		}
	} else {
		__ALC_UTILS_IOTRACE("dumping to file step: %s, is complete: %d", temporary_filename, alc_packet->close_object_flag);
	}

	__ALC_UTILS_IOTRACE("checking tsi: %u, toi: %u, close_object_flag: %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

cleanup:
	if(temporary_filename) {
		free(temporary_filename);
	}

	if(s_tsid_content_location) {
		free(s_tsid_content_location);
	}

	return bytesWritten;
}


void __alc_prepend_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet) {

#if defined(__TESTING_PREPEND_TSI__) && defined(__TESTING_PREPEND_TOI_INIT__)

	char* tsi_init = __TESTING_PREPEND_TSI__;
	char* toi_init = __TESTING_PREPEND_TOI_INIT__;

	char* init_file_name = calloc(255, sizeof(char));
	char* fm4v_file_name = calloc(255, sizeof(char)); //.m4v == 4

	__ALC_UTILS_DEBUG(" - concat %s, %s,  %d", alc_packet->tsi_c, alc_packet->toi_c, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%s-%s", __ALC_DUMP_OUTPUT_PATH__, tsi_init, toi_init);
	snprintf(fm4v_file_name, 255, "%s%s-%s.m4v", __ALC_DUMP_OUTPUT_PATH__, alc_packet->tsi_c, alc_packet->toi_c);

	if( access( init_file_name, F_OK ) == -1 ) {
		__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
		goto cleanup;
	}
	struct stat st;
	stat(init_file_name, &st);

	uint8_t* init_payload = calloc(st.st_size, sizeof(uint8_t));
	FILE* init_file = fopen(init_file_name, "r");
	if(!init_file) {
		__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
		goto cleanup;
	}

	fread(init_payload, st.st_size, 1, init_file);
	fclose(init_file);

	FILE* fm4v_output_file = fopen(fm4v_file_name, "w");
	if(!fm4v_output_file) {
		__ALC_UTILS_ERROR("unable to open fm4v output file: %s", fm4v_file_name);
		goto cleanup;
	}

	fwrite(init_payload, st.st_size, 1, fm4v_output_file);
	uint64_t block_size = 8192;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		goto cleanup;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count=0;
	bool has_eof = false;
	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		__ALC_UTILS_TRACE("read bytes: %llu", read_bytes);

		int write_size = fwrite(m4v_payload, read_bytes, 1, fm4v_output_file);
		if(has_eof) {
			__ALC_UTILS_TRACE("write bytes: %u", write_size);

			fclose(m4v_fragment_input_file);
			fclose(fm4v_output_file);
			break;
		}
		write_count++;
	}
cleanup:
	return;
#endif

}

bool __ALC_RECON_HAS_WRITTEN_INIT_BOX = false;

void __alc_recon_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet, uint32_t tsi, uint32_t toi_init, const char* to_write_filename) {


	char* init_file_name = (char*)calloc(255, sizeof(char));
	char* recon_file_name = (char*)calloc(255, sizeof(char)); //.m4v == 4
	FILE* recon_output_file = NULL;

	__ALC_UTILS_DEBUG(" alc_recon_fragment_with_init_box: %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, tsi, toi_init);
	snprintf(recon_file_name, 255, "%s%s", __ALC_DUMP_OUTPUT_PATH__, to_write_filename );

	if(!__ALC_RECON_HAS_WRITTEN_INIT_BOX) {


		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		fread(init_payload, st.st_size, 1, init_file);
		fclose(init_file);
		recon_output_file = fopen(recon_file_name, "w");
		if(!recon_output_file) {
			__ALC_UTILS_ERROR("unable to open recon_output_file for writing: %s", recon_file_name);
			return;
		}
		fwrite(init_payload, st.st_size, 1, recon_output_file);
		__ALC_RECON_HAS_WRITTEN_INIT_BOX = true;

	} else {
		recon_output_file = fopen(recon_file_name, "a");
		if(!recon_output_file) {
		__ALC_UTILS_ERROR("unable to open recon_output_file for append: %s", recon_file_name);
		return;
		}
	}

	uint64_t block_size = 8192;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = (uint8_t*)calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		return;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count=0;
	bool has_eof = false;
	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		__ALC_UTILS_TRACE("read bytes: %llu", read_bytes);

		int write_size = fwrite(m4v_payload, read_bytes, 1, recon_output_file);
		if(has_eof) {
			__ALC_UTILS_TRACE("write bytes: %u", write_size);

			fclose(m4v_fragment_input_file);
			fclose(recon_output_file);
			break;
		}
		write_count++;
	}
cleanup:
	return;
}

//watch out for leaks...
void alc_recon_file_ptr_set_tsi_toi(FILE* file_ptr, uint32_t tsi, uint32_t toi_init) {
	__ALC_RECON_FILE_PTR = file_ptr;
	if(!__ALC_RECON_FILE_PTR_TSI) {
		__ALC_RECON_FILE_PTR_TSI = (uint32_t*)calloc(1, sizeof(uint32_t));
	}
	*__ALC_RECON_FILE_PTR_TSI = tsi;


	if(!__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_RECON_FILE_PTR_TOI_INIT = (uint32_t*)calloc(1, sizeof(uint32_t));
		}
	*__ALC_RECON_FILE_PTR_TOI_INIT = toi_init;
}

void alc_recon_file_ptr_fragment_with_init_box(FILE* output_file_ptr, udp_flow_t* udp_flow, alc_packet_t* alc_packet, uint32_t to_match_toi_init) {
	int flush_ret = 0;
	if(!__ALC_RECON_FILE_PTR_TSI || !__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_WARN("alc_recon_file_ptr_fragment_with_init_box - NULL: tsi: %p, toi: %p", __ALC_RECON_FILE_PTR_TSI, __ALC_RECON_FILE_PTR_TOI_INIT);
		return;
	}

	char* file_name = alc_packet_dump_to_object_get_temporary_filename(udp_flow, alc_packet);
	uint32_t toi_init = to_match_toi_init;

	char* init_file_name = (char* )calloc(255, sizeof(char));

	__ALC_UTILS_DEBUG("recon %u, %u, %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, *__ALC_RECON_FILE_PTR_TSI, toi_init);

	if(!__ALC_RECON_FILE_PTR_HAS_WRITTEN_INIT_BOX) {
		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		fread(init_payload, st.st_size, 1, init_file);
		fclose(init_file);

		fwrite(init_payload, st.st_size, 1, output_file_ptr);
		__ALC_RECON_HAS_WRITTEN_INIT_BOX = true;

	} else {
		//noop here
	}

	uint64_t block_size = 8192;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = (uint8_t*)calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		return;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count=0;
	bool has_eof = false;
	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		__ALC_UTILS_TRACE("read bytes: %llu", read_bytes);

		if(feof(output_file_ptr)) {
			goto broken_pipe;
		}

		int write_size = fwrite(m4v_payload, read_bytes, 1, output_file_ptr);
		if(has_eof) {
			__ALC_UTILS_TRACE("write bytes: %u", write_size);

			fclose(m4v_fragment_input_file);
			flush_ret = fflush(output_file_ptr);
			if(flush_ret || feof(output_file_ptr)) {
				goto broken_pipe;
			}
			break;
		}
		write_count++;
	}
	goto cleanup;

broken_pipe:
	__ALC_UTILS_ERROR("flush returned: %d, closing pipe", flush_ret);
	fclose(__ALC_RECON_FILE_PTR);
	__ALC_RECON_FILE_PTR = NULL;

cleanup:
	if(m4v_payload) {
		free(m4v_payload);
		m4v_payload = NULL;
	}
	if(file_name) {
		free(file_name);
		file_name = NULL;
	}


	return;
}

/*
 * mutex buffer writer
 */

void alc_recon_file_buffer_struct_set_tsi_toi(pipe_ffplay_buffer_t* pipe_ffplay_buffer, uint32_t tsi, uint32_t toi_init) {
	__ALC_RECON_FILE_BUFFER_STRUCT = pipe_ffplay_buffer;

	if(!__ALC_RECON_FILE_PTR_TSI) {
		__ALC_RECON_FILE_PTR_TSI = (uint32_t*)calloc(1, sizeof(uint32_t));
	}

	if(!__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_RECON_FILE_PTR_TOI_INIT = (uint32_t*)calloc(1, sizeof(uint32_t));
	}

	*__ALC_RECON_FILE_PTR_TSI = tsi;
	*__ALC_RECON_FILE_PTR_TOI_INIT = toi_init;
}



/*** we take this off of disk for the reassembeled fragment metadta and mpu
 *
 *
 */
void alc_recon_file_buffer_struct_fragment_with_init_box(pipe_ffplay_buffer_t* pipe_ffplay_buffer, udp_flow_t* udp_flow, alc_packet_t* alc_packet) {
	int flush_ret = 0;
	if(!__ALC_RECON_FILE_PTR_TSI || !__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_WARN("alc_recon_file_ptr_fragment_with_init_box - NULL: tsi: %p, toi: %p", __ALC_RECON_FILE_PTR_TSI, __ALC_RECON_FILE_PTR_TOI_INIT);
		return;
	}

	char* file_name = alc_packet_dump_to_object_get_temporary_filename(udp_flow, alc_packet);
	uint32_t toi_init = *__ALC_RECON_FILE_PTR_TOI_INIT;
	char* init_file_name = (char*)calloc(255, sizeof(char));

	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - ENTER - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, *__ALC_RECON_FILE_PTR_TSI, toi_init);

	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);

	if(!pipe_ffplay_buffer->has_written_init_box) {
		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		fread(init_payload, st.st_size, 1, init_file);
		fclose(init_file);

		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, init_payload, st.st_size);
		pipe_ffplay_buffer->has_written_init_box = true;

	} else {
		//noop here
	}

	uint64_t block_size = __PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = (uint8_t*)calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		return;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count = 0;
	uint64_t total_bytes_written = 0;
	bool has_eof = false;

	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		total_bytes_written += read_bytes;
		__ALC_UTILS_TRACE("read bytes: %llu, bytes written: %llu, total filesize: %llu, has eof input: %d", read_bytes, total_bytes_written, fragment_input_stat.st_size, has_eof);

		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, m4v_payload, read_bytes);

		if(has_eof) {
			fclose(m4v_fragment_input_file);
			break;
		}
		write_count++;

	}

	//signal and then unlock, docs indicate the only way to ensure a signal is not lost is to send it while holding the lock
	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - SIGNALING - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer);

	//check to see if we have shutdown
	pipe_buffer_reader_check_if_shutdown(&pipe_ffplay_buffer);

	pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - RETURN - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	goto cleanup;

broken_pipe:
	__ALC_UTILS_ERROR("flush returned: %d, closing pipe", flush_ret);
	fclose(__ALC_RECON_FILE_PTR);
	__ALC_RECON_FILE_PTR = NULL;

cleanup:
	if(m4v_payload) {
		free(m4v_payload);
		m4v_payload = NULL;
	}
	if(file_name) {
		free(file_name);
		file_name = NULL;
	}


	return;
}


void alc_recon_file_buffer_struct_monitor_fragment_with_init_box(udp_flow_t* udp_flow, alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	int flush_ret = 0;
	char* audio_init_file_name = NULL;
	char* video_init_file_name = NULL;
	char* audio_fragment_file_name = NULL;
	char* video_fragment_file_name = NULL;
	block_t* audio_fragment_payload = NULL;
	block_t* video_fragment_payload = NULL;
	block_t* audio_init_payload = NULL;
	block_t* video_init_payload = NULL;

	//tsi matching for audio and video fragments
	if(alc_packet->def_lct_hdr->tsi == lls_sls_alc_monitor->audio_tsi) {
		//don't flush out init boxes here..
		if(alc_packet->def_lct_hdr->toi == lls_sls_alc_monitor->audio_toi_init) {
			__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box, got audio init box: tsi: %u, toi: %u, ignoring", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
			return;
		}

		lls_sls_alc_monitor->last_closed_audio_toi = alc_packet->def_lct_hdr->toi;
		if(alc_packet->ext_route_presentation_ntp_timestamp_set && !lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time_set) {
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time = alc_packet->ext_route_presentation_ntp_timestamp;
			compute_ntp64_to_seconds_microseconds(lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time, &lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time_s, &lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time_us);
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time_set = true;
		}
	}

	if(alc_packet->def_lct_hdr->tsi == lls_sls_alc_monitor->video_tsi) {
		if(alc_packet->def_lct_hdr->toi == lls_sls_alc_monitor->video_toi_init) {
			__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box, got video init box: tsi: %u, toi: %u, ignoring", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
			return;
		}

		lls_sls_alc_monitor->last_closed_video_toi = alc_packet->def_lct_hdr->toi;
		if(alc_packet->ext_route_presentation_ntp_timestamp_set && !lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time_set) {
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time = alc_packet->ext_route_presentation_ntp_timestamp;
			compute_ntp64_to_seconds_microseconds(lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time, &lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time_s, &lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time_us);
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time_set = true;

		}
	}

	//we may have short audio/video packets, so allow our buffer accumulate and then flush independently after we have written our initbox
    uint32_t audio_toi = lls_sls_alc_monitor->last_closed_audio_toi;
    uint32_t video_toi = lls_sls_alc_monitor->last_closed_video_toi;
    
	audio_fragment_file_name = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, lls_sls_alc_monitor->audio_tsi, audio_toi);
	video_fragment_file_name = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, lls_sls_alc_monitor->video_tsi, video_toi);

	if(!lls_sls_alc_monitor->lls_sls_monitor_output_buffer.has_written_init_box) {
		audio_init_file_name = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, lls_sls_alc_monitor->audio_tsi, lls_sls_alc_monitor->audio_toi_init);
		video_init_file_name = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, lls_sls_alc_monitor->video_tsi, lls_sls_alc_monitor->video_toi_init);

		audio_init_payload = alc_get_payload_from_filename(audio_init_file_name);
		video_init_payload = alc_get_payload_from_filename(video_init_file_name);

		audio_fragment_payload = alc_get_payload_from_filename(audio_fragment_file_name);
		video_fragment_payload = alc_get_payload_from_filename(video_fragment_file_name);

		if(audio_init_payload && video_init_payload &&
			audio_fragment_payload && video_fragment_payload) {

			__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - audio: %s, video: %s", audio_init_file_name, video_init_file_name);

			lls_sls_monitor_output_buffer_copy_audio_init_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer, audio_init_payload);
			lls_sls_monitor_output_buffer_copy_video_init_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer, video_init_payload);
			lls_sls_monitor_output_buffer_copy_audio_fragment_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer, audio_fragment_payload);
			lls_sls_monitor_output_buffer_copy_video_fragment_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer, video_fragment_payload);
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.has_written_init_box = true;
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer = true;
	        lls_sls_alc_monitor->last_closed_audio_toi = 0;
	        lls_sls_alc_monitor->last_closed_audio_toi = 0;

			lls_sls_alc_monitor->last_pending_flushed_audio_toi = 0;
			lls_sls_alc_monitor->last_pending_flushed_video_toi = 0;

	        lls_sls_alc_monitor->last_completed_flushed_audio_toi = audio_toi;
	        lls_sls_alc_monitor->last_completed_flushed_video_toi = video_toi;


		} else {
			__ALC_UTILS_ERROR("missing init/moof payloads, audio init: %s (%p), audio moof: %s (%p), video init: %s (%p), video moof: %s (%p)",
					audio_init_file_name,
					audio_init_payload,
					audio_fragment_file_name,
					audio_fragment_payload,
					video_init_file_name,
					video_init_payload,
					video_fragment_file_name,
					video_fragment_payload);

			goto cleanup;
		}
	} else {

		//TODO - determine if we should prepend the most recent init box?
		//append audio if we have an audio frame
		if(audio_toi && audio_fragment_file_name) {
			audio_fragment_payload = alc_get_payload_from_filename(audio_fragment_file_name);
			if(audio_fragment_payload) {
				lls_sls_monitor_output_buffer_merge_alc_fragment_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff, audio_fragment_payload);
				lls_sls_alc_monitor->last_closed_audio_toi = 0;
				lls_sls_alc_monitor->last_pending_flushed_audio_toi = audio_toi;
				__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - pushing audio fragment: %d, file: %s", audio_toi, audio_fragment_file_name);
			} else {
				__ALC_UTILS_ERROR("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - missing audio fragment: %d, file: %s", audio_toi, audio_fragment_file_name);
			}
		}

		//append video if we have a video frame
		if(video_toi && video_fragment_file_name) {
			video_fragment_payload = alc_get_payload_from_filename(video_fragment_file_name);
			if(video_fragment_payload) {
				lls_sls_monitor_output_buffer_merge_alc_fragment_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff, video_fragment_payload);

				lls_sls_alc_monitor->last_closed_video_toi = 0;
				lls_sls_alc_monitor->last_pending_flushed_video_toi = video_toi;

				__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - pushing video fragment: %d, file: %s", video_toi, video_fragment_file_name);
			} else {
				__ALC_UTILS_ERROR("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - missing video fragment: %d, file: %s", video_toi, video_fragment_file_name);
			}
		}

		if(lls_sls_alc_monitor->last_pending_flushed_audio_toi && lls_sls_alc_monitor->last_pending_flushed_video_toi) {
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer = true;
			__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - setting should_flush output buffer for: audio fragment: %d, video fragment: %d",
					lls_sls_alc_monitor->last_pending_flushed_audio_toi, lls_sls_alc_monitor->last_pending_flushed_video_toi);



			lls_sls_alc_monitor->last_completed_flushed_audio_toi = lls_sls_alc_monitor->last_pending_flushed_audio_toi;
			lls_sls_alc_monitor->last_completed_flushed_video_toi = lls_sls_alc_monitor->last_pending_flushed_video_toi;

			lls_sls_alc_monitor->last_closed_audio_toi = 0;
			lls_sls_alc_monitor->last_pending_flushed_audio_toi = 0;
			lls_sls_alc_monitor->last_closed_video_toi = 0;
			lls_sls_alc_monitor->last_pending_flushed_video_toi = 0;
		}
	}

cleanup:
	freesafe(audio_init_file_name);
	freesafe(video_init_file_name);
	freesafe(audio_fragment_file_name);
	freesafe(video_fragment_file_name);
	block_Destroy(&audio_fragment_payload);
	block_Destroy(&video_fragment_payload);
	block_Destroy(&audio_init_payload);
	block_Destroy(&video_init_payload);

	return;
}

