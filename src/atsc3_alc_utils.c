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

#include "atsc3_route_package_utils.h"

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
	if(!fp || st.st_size == 0) {
		__ALC_UTILS_ERROR("alc_get_payload_from_filename: size: 0 file: %s", file_name);
		return NULL;
	}

	fread(payload->p_buffer, st.st_size, 1, fp);
	payload->i_pos = st.st_size;
	fclose(fp);

	return payload;

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
 *
 *	Note: entity mode will re-write the delivery object body to extract relevant headers -
 *	TODO: push entity mode headers into atsc3_route_object as needed
 */

char* alc_packet_dump_to_object_get_s_tsid_filename(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {

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


					//jjustman-2020-07-14 - guard for  atsc3_route_s_tsid_RS_LS_SrcFlow_Payload = 0x0000000000000000
					//  * frame #0: 0x00000001000374fe atsc3_alc_listener_mde_writer`alc_packet_dump_to_object_get_s_tsid_filename(udp_flow=0x00000001015075b0, alc_packet=0x000000010150e8c0, lls_sls_alc_monitor=0x0000000100704ca0) at atsc3_alc_utils.c:298:13


					if(atsc3_route_s_tsid_RS_LS->tsi == alc_packet->def_lct_hdr->tsi && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload) {
					    //Process anything but entity mode (2), by default for $TOI$ replacement,
						//SrcFlow_Payload.format_id == 1 for file mode:

					    if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id != 2) {
                            //try to find our matching toi and content-location value
                            if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance->atsc3_fdt_file_v.count) {
                                atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance;
                                for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
                                    atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];

                                    //if not in entity mode, and toi matches, then use this mapping, otherwise, fallback to file_template
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
                            }
					    }

					    //check if we are formatId==2 for entity mode
						if(!content_location) {
						    if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload) {
                                __ALC_UTILS_DEBUG("received ALC as delivery object with formatId: %d", atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id);

                                if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id == 2) {
                                    __ALC_UTILS_DEBUG("processing ALC as Entity Mode (formatId: %d), tsi: %d, toi: %d",
                                    		atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id,
											alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);

                                    //extract out our content_location from the ALC payload headers here
                                    char* temp_content_location = alc_packet_dump_to_object_get_temporary_recovering_filename(udp_flow, alc_packet);

                                    struct stat st;
                                    stat(temp_content_location, &st);

                                    //jjustman-2020-01-16 - parse first block of rfc 7231 headers...
                                    char* temp_content_header = calloc(8193, sizeof(char)); //over-allocate for null pad at end
                                    FILE* temp_fp = fopen(temp_content_location, "r+");
                                    if(temp_fp) {
                                        //jjustman-2020-01-16 - refactor me to proper RFC 7231 header handling
                                        int read_len = fread(temp_content_header, 1, 8192, temp_fp); //try and get actual bytes read just in case
                                        //"content-location: "
                                        // 12345678901234567 = 18 bytes (w/ whitespace?)

                                        char* location_found = strcasestr(temp_content_header, "content-location: ");

                                        if(location_found) {
                                            location_found += 18; //move us forward 18 bytes for content-location: literal
                                            //find newline char (either 0x0d|0x0a)
                                            char* endofline = NULL;
                                            int pos = 0;
                                            while(true) {
                                                if(!location_found[pos]) {
                                                    __ALC_UTILS_DEBUG("ALC MDE: no content-location found after pos: %d", pos);

                                                    break;
                                                } else if(location_found[pos] == 0x0d || location_found[pos] == 0x0a) {
                                                    endofline = location_found + (pos-1);
                                                    content_location = strndup(location_found, pos);
                                                    __ALC_UTILS_DEBUG("ALC MDE: local entity mode filename is: %s", content_location);

                                                    bool has_additional_headers = false;
                                                    int newline_count = 1;
                                                    while(true) {
                                                        pos++;

                                                        //now we need to chomp off the remaining entity header(s) until we get to an empty newline
                                                        //happy path
                                                        if(location_found[pos] == 0x0d || location_found[pos] == 0x0a) {
                                                            newline_count++;
                                                            if(newline_count > 3) {
                                                                pos++; //and get rid of our last linebreak..
                                                                //break out and trim our file
                                                                int trim_size = (location_found + pos) - temp_content_header;
                                                                int new_mde_payload_size = st.st_size - trim_size;
                                                                __ALC_UTILS_INFO("ALC MDE: entity mode, original size: %lld, header cut is: %d bytes, new mde size is: %d", st.st_size, trim_size, new_mde_payload_size);

                                                                if(trim_size > 0 && new_mde_payload_size > 0) {
                                                                    uint8_t* to_trim_payload = calloc(new_mde_payload_size, sizeof(uint8_t));

                                                                    fseek(temp_fp, trim_size, SEEK_SET);
                                                                    fread(to_trim_payload, new_mde_payload_size, 1, temp_fp);
                                                                    int ret = ftruncate(fileno(temp_fp), new_mde_payload_size);
                                                                    //printf("ftruncate for fd: %d, ret is: %d", fileno(temp_fp), ret);
                                                                    fsync(fileno(temp_fp));
                                                                    fseek(temp_fp, 0, SEEK_SET);
                                                                    fwrite(to_trim_payload, new_mde_payload_size, 1, temp_fp);
                                                                   /* for(int i=0; i < 32; i++) {
                                                                        printf("to_trim_payload[%d]: 0x%02x (%c)", i, to_trim_payload[i], to_trim_payload[i]);
                                                                    }*/

                                                                    fsync(fileno(temp_fp));

                                                                    free(to_trim_payload);
                                                                    to_trim_payload = NULL;

                                                                    break; //done
                                                                }
                                                            }
                                                        } else {
                                                            has_additional_headers = true;
                                                            newline_count = 0;
                                                        }
                                                    }



                                                    break;
                                                }
                                                pos++;
                                            }
                                        }
                                    }

                                    if(temp_fp) {
                                        fclose(temp_fp);
                                        temp_fp = NULL;
                                    }
                                    if(temp_content_location) {
                                        free(temp_content_location);
                                        temp_content_location = NULL;
                                    }
                                    if(temp_content_header) {
                                        free(temp_content_header);
                                        temp_content_header = NULL;
                                    }
                                } else {
                                	__ALC_UTILS_DEBUG("deferring processing ALC with formatId: %d, tsi: %d, toi: %d",
                                			atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id,
											alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);

                                }

						    } else {
                                __ALC_UTILS_WARN("processing ALC MDE - but no atsc3_route_s_tsid_RS_LS_SrcFlow_Payload!");

                            }
						}
					}
				}
			}
		}
	}

	//fallback to ip_tsi_toi filename
	if(!content_location) {
		if(alc_packet->def_lct_hdr) {
            content_location = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
			__ALC_UTILS_INFO("alc_packet_dump_to_object_get_s_tsid_filename: no content_location to return for alc_packet: %p, falling back to %s", alc_packet, content_location);
		} else {
			__ALC_UTILS_ERROR("alc_packet_dump_to_object_get_s_tsid_filename: no content_location to return for alc_packet: %p, falling back to null string! THIS SHOULD NEVER HAPPEN", alc_packet);
		}
	}

	return content_location;
}


//jjustman-2020-07-07 - get <LS> element for matching flow and packet


atsc3_route_s_tsid_RS_LS_t* atsc3_alc_packet_get_RS_LS_element(udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	if(lls_sls_alc_monitor->atsc3_sls_metadata_fragments && lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid && lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count) {
		for(int i=0; i < lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.count; i++) {
 			atsc3_route_s_tsid_RS_t* atsc3_route_s_tsid_RS = lls_sls_alc_monitor->atsc3_sls_metadata_fragments->atsc3_route_s_tsid->atsc3_route_s_tsid_RS_v.data[i];

 			if(atsc3_route_s_tsid_RS->dest_ip_addr == udp_flow->dst_ip_addr && atsc3_route_s_tsid_RS->dest_port == udp_flow->dst_port && atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.count) {
 				for(int j=0; j < atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.count; j++) {
 					atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = atsc3_route_s_tsid_RS->atsc3_route_s_tsid_RS_LS_v.data[j];

 					if(atsc3_route_s_tsid_RS_LS->tsi == alc_packet->def_lct_hdr->tsi) {
 						return atsc3_route_s_tsid_RS_LS;
 					}
 				}
 			}
 		}
	 }

	return NULL;
}

atsc3_fdt_file_t* atsc3_alc_RS_LS_get_matching_toi_file_instance(atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS, uint32_t search_toi) {
	if(atsc3_route_s_tsid_RS_LS && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow) {
		if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload) {
			if(atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance && atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance->atsc3_fdt_file_v.count) {
				atsc3_fdt_instance_t* atsc3_fdt_instance = atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_fdt_instance;
                for(int k=0; k < atsc3_fdt_instance->atsc3_fdt_file_v.count; k++) {
                	atsc3_fdt_file_t* atsc3_fdt_file = atsc3_fdt_instance->atsc3_fdt_file_v.data[k];
                	if(atsc3_fdt_file->toi == search_toi) {
                		return atsc3_fdt_file;
                	}
                }
			}
		}
	}

	return NULL;
}

 //end jjustman-2020-07-07

//jjustman-2020-08-05 - todo: use atsc3_utils / FILE* atsc3_object_open(char* file_name) {

FILE* atsc3_alc_object_open(char* file_name) {
	if( access( file_name, F_OK ) != -1 ) {
		FILE* f = fopen(file_name, "r+");
		if(f) {
			return f;
		}
	}

	return NULL;
}


FILE* alc_object_open_or_pre_allocate(char* file_name, atsc3_alc_packet_t* alc_packet) {
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
FILE* alc_object_pre_allocate(char* file_name, atsc3_alc_packet_t* alc_packet) {
	if(!__TO_PREALLOC_ZERO_SLAB_PTR) {
		__TO_PREALLOC_ZERO_SLAB_PTR = (uint8_t*)malloc(__TO_PREALLOC_ZERO_SLAB_SIZE);
		memset(__TO_PREALLOC_ZERO_SLAB_PTR, 0, __TO_PREALLOC_ZERO_SLAB_SIZE);
	}
	struct stat st = {0};

	if(stat(__ALC_DUMP_OUTPUT_PATH__, &st) == -1) {
		 mkdir(__ALC_DUMP_OUTPUT_PATH__, 0777);
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
        __ALC_UTILS_IOTRACE("pre_allocate: file %s, transfer_len is 0, not pre allocating", file_name);
    }
    fclose(f);
    f = fopen(file_name, "r+");
   
    return f;
}

int alc_packet_write_fragment(FILE* f, char* file_name, uint32_t offset, atsc3_alc_packet_t* alc_packet) {
    
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


/* atsc3_alc_packet_persist_to_toi_resource_process_sls_mbms_and_emit_callback
 *
 * persist to disk, process sls mbms and/or emit ROUTE media_delivery_event complete to the application tier if
 * the full packet has been recovered (e.g. no missing data units in the forward transmission)
 * Notes:
 *
 *      TOI size:     uint32_t to_allocate_size = alc_packet->transfer_len;
 *
 *
 *      return values:
 *      	-1 if unable to open file pointer to object
 *      	-2 if lls_sls_alc_monitor or !lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled
 *
 *      	if/when we pre-allocate, objects will be placed in __ALC_DUMP_OUTPUT_PATH__
 *
 */

int atsc3_alc_packet_persist_to_toi_resource_process_sls_mbms_and_emit_callback(udp_flow_t *udp_flow, atsc3_alc_packet_t *alc_packet, lls_sls_alc_monitor_t *lls_sls_alc_monitor, atsc3_route_object_t* atsc3_route_object) {
    char* temporary_recovery_filename = NULL;
	char* s_tsid_content_location = NULL;

	int bytesWritten = 0;

	//jjustman-2020-08-05 - deprecate file_dump_enabled option
	if(!lls_sls_alc_monitor || (lls_sls_alc_monitor && !lls_sls_alc_monitor->lls_sls_monitor_output_buffer_mode.file_dump_enabled)) {
		return -2;
	}

	//get a local copy of our temporary object recovery filename for promoting to final payload objet filename
	if(!atsc3_route_object->temporary_object_recovery_filename) {
		temporary_recovery_filename = alc_packet_dump_to_object_get_temporary_recovering_filename(udp_flow, alc_packet);
		atsc3_route_object_set_temporary_object_recovery_filename_if_null(atsc3_route_object, temporary_recovery_filename);
	} else {
		temporary_recovery_filename = atsc3_route_object_get_temporary_object_recovery_filename_strdup(atsc3_route_object);
	}

	//prepare our temporary_recovery_filename fp handle with merged atsc3_route_object_lct_packet pending pending_alc_payload_to_persist
	//NOTE: mutallly exclusive impl's between recovery_block and atsc3_route_object_persist_recovery_all_pending_lct_packet_vector
	//atsc3_route_object_persist_recovery_block_from_lct_packet_vector(atsc3_route_object);

    //jjustman-2020-07-28 - TODO: don't redispatch repeadedly for carousels...
    //jjustman-2020-08-05 - allow is_toi_init to re-write our object on carousel emission
    if(atsc3_route_object_is_complete(atsc3_route_object) && (atsc3_route_object->is_toi_init || !atsc3_route_object->recovery_complete_timestamp)) {

    	//build our full recovery recovery_file_buffer block_t payload here...
    	atsc3_route_object_persist_recovery_buffer_all_pending_lct_packet_vector(atsc3_route_object);

        atsc3_route_object_recovery_file_handle_flush_and_close(atsc3_route_object);

        //update our sls here if we have a service we are listenting to
        if(lls_sls_alc_monitor && lls_sls_alc_monitor->atsc3_lls_slt_service && alc_packet->def_lct_hdr->tsi == 0) {

            char* final_mbms_toi_filename = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, 0, alc_packet->def_lct_hdr->toi);
            rename(temporary_recovery_filename, final_mbms_toi_filename);

            //jjustman-2020-07-28 - purge atsc3_route_object->atsc3_route_object_lct_packet_received_v as we are invalidated at this point
            //do not set final_object_recovery_filename here, as we don't want to purge this...
        	atsc3_route_object_clear_temporary_object_recovery_filename(atsc3_route_object);
            atsc3_route_object_reset_and_free_atsc3_route_object_lct_packet_received(atsc3_route_object);

            __ALC_UTILS_IOTRACE("ALC: service_id: %u, ------ TSI of 0, TOI: %d, transfer_len: %lld, final object name: %s, calling atsc3_route_sls_process_from_alc_packet_and_file",
            		lls_sls_alc_monitor->atsc3_lls_slt_service->service_id,
            		alc_packet->def_lct_hdr->toi,
            		alc_packet->transfer_len,
            		final_mbms_toi_filename);

            atsc3_route_sls_process_from_alc_packet_and_file(udp_flow, alc_packet, lls_sls_alc_monitor);
			
			atsc3_route_sls_process_from_sls_metadata_fragments_patch_mpd_availability_start_time_and_start_number(lls_sls_alc_monitor->atsc3_sls_metadata_fragments, lls_sls_alc_monitor);
					
            free(final_mbms_toi_filename);

        } else {
        	//jjustman-2020-07-28 - todo: use atsc3_route_object for fp handle reference

        	//jjustman-2020-08-05 - dirty hack - don't process ROUTE object completion if we don't have our SLS parsed yet,
        	// i.e. lls_sls_alc_monitor->atsc3_sls_metadata_fragments is null

        	if(!lls_sls_alc_monitor->atsc3_sls_metadata_fragments) {
                __ALC_UTILS_ERROR("lls_sls_alc_monitor->atsc3_sls_metadata_fragments is NULL, tsi: %u, toi: %u, bailing on object recovery complete!",
                        alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
                bytesWritten = -3;
                goto cleanup;
            }

            s_tsid_content_location = alc_packet_dump_to_object_get_s_tsid_filename(udp_flow, alc_packet, lls_sls_alc_monitor);
     
            if(strncmp(temporary_recovery_filename, s_tsid_content_location, __MIN(strlen(temporary_recovery_filename), strlen(s_tsid_content_location))) !=0) {
                char new_file_name_raw_buffer[1024] = { 0 };
                char* new_file_name = (char*)&new_file_name_raw_buffer; //hack
                snprintf(new_file_name_raw_buffer, 1024, __ALC_DUMP_OUTPUT_PATH__"%d/%s", lls_sls_alc_monitor->atsc3_lls_slt_service->service_id, s_tsid_content_location);
                
                //todo: jjustman-2019-11-15: sanatize path parameter for .. or other traversal attacks
                bool is_traversal = new_file_name[0] == '.';
                
                for(int i=0; i < strlen(new_file_name) && is_traversal; i++) {
                    new_file_name++;
                    is_traversal = new_file_name[0] == '.';
                }
                
                //jjustman-2020-08-04: TODO - refactor this to mkpath
                //iterate over occurances of '/' and create directory hierarchy
                char* path_slash_position = new_file_name;
                char* first_path_slash_position = new_file_name;
                while((path_slash_position = strstr(path_slash_position + 1, "/"))) {
                    if(path_slash_position - first_path_slash_position > 0) {
                        //hack
                        *path_slash_position = '\0';
                        mkdir(first_path_slash_position, 0777);
                        *path_slash_position = '/';
                    }
                }
                
                rename(temporary_recovery_filename, new_file_name);
                __ALC_UTILS_IOTRACE("tsi: %u, toi: %u, moving from to temporary_filename: %s to: %s, is complete: %d",
                		alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi,
						temporary_recovery_filename, new_file_name, alc_packet->close_object_flag);

            	atsc3_route_object_clear_temporary_object_recovery_filename(atsc3_route_object);
				atsc3_route_object_set_final_object_recovery_filename_for_logging(atsc3_route_object, new_file_name);
                atsc3_sls_alc_flow_t* matching_sls_alc_flow = NULL;

				//jjustman-2020-07-14 - global route dash representationId patching for s-tsid flows
				//atsc3_sls_alc_all_mediainfo_flow_v
				if((matching_sls_alc_flow = atsc3_sls_alc_flow_find_entry_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, alc_packet->def_lct_hdr->tsi))) {
					if(matching_sls_alc_flow->toi_init != alc_packet->def_lct_hdr->toi) {
						matching_sls_alc_flow->last_closed_toi = alc_packet->def_lct_hdr->toi;

						//keep track of our new file name path so we can purge/reap as needed for media segments
		            	atsc3_route_object_set_final_object_recovery_filename_for_eviction(atsc3_route_object, new_file_name);
					} else if(matching_sls_alc_flow->toi_init == alc_packet->def_lct_hdr->toi) {
						atsc3_route_object_set_is_toi_init_object(atsc3_route_object, true);
					}
				}

                //jjustman-2020-07-07 - package extraction - process any requirements for codePoint==3 || codePoint == 4 OR formatId==3 || formatId==4
                atsc3_route_s_tsid_RS_LS_t* atsc3_route_s_tsid_RS_LS = atsc3_alc_packet_get_RS_LS_element(udp_flow, alc_packet, lls_sls_alc_monitor);
                if(atsc3_route_s_tsid_RS_LS) {
                	//jjustman-2020-07-28 - TODO: push back for filesystem expiration and removal as needed

                	atsc3_fdt_file_t* atsc3_fdt_file = atsc3_alc_RS_LS_get_matching_toi_file_instance(atsc3_route_s_tsid_RS_LS, alc_packet->def_lct_hdr->toi);
                	if(atsc3_fdt_file) {
                        __ALC_UTILS_DEBUG("atsc3_fdt_file: object closed: tsi: %u, toi: %u, filename: %s, LCT: codepoint: %d, S-TSID: codepoint: %d, formatId: %d, frag: %d, order: %d, lct packets: %d",
                        		alc_packet->def_lct_hdr->tsi,
								alc_packet->def_lct_hdr->toi,
								new_file_name,
								alc_packet->def_lct_hdr->codepoint,
								atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->codepoint,
								atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id,
								atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->frag,
								atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->order,
								atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);

                        //perform package extraction for codepoint 3 || 4 or
                        int lct_codepoint_package_matching = (alc_packet->def_lct_hdr->codepoint == atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->codepoint) &&
                        								  (atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->codepoint == 3 || atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->codepoint == 4);
						int stsid_formatid_package_matching = (atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id == 3 || atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id == 4);

						if(lct_codepoint_package_matching || stsid_formatid_package_matching) {
							__ALC_UTILS_DEBUG("calling atsc3_route_package_extract_unsigned_payload with package: %s", new_file_name);
							//perform package extraction into shared appContextIdList path
							char* package_extract_path = atsc3_route_package_generate_path_from_appContextIdList(atsc3_fdt_file);

							atsc3_route_package_extracted_envelope_metadata_and_payload_t* atsc3_route_package_extracted_envelope_metadata_and_payload = atsc3_route_package_extract_unsigned_payload(new_file_name, package_extract_path);
							if(atsc3_route_package_extracted_envelope_metadata_and_payload) {
								atsc3_route_package_extracted_envelope_metadata_and_payload_set_alc_tsi_toi_from_alc_packet(atsc3_route_package_extracted_envelope_metadata_and_payload, alc_packet);
								atsc3_route_package_extracted_envelope_metadata_and_payload_set_fdt_attributes(atsc3_route_package_extracted_envelope_metadata_and_payload, atsc3_fdt_file);
								if(lls_sls_alc_monitor->atsc3_lls_sls_alc_on_package_extract_completed_callback) {
									lls_sls_alc_monitor->atsc3_lls_sls_alc_on_package_extract_completed_callback(atsc3_route_package_extracted_envelope_metadata_and_payload);
								}
							} else {
								__ALC_UTILS_WARN("Unable to extract package: %s to path: %s", new_file_name, package_extract_path);
							}
							freesafe(package_extract_path);

							atsc3_route_package_extracted_envelope_metadata_and_payload_free(&atsc3_route_package_extracted_envelope_metadata_and_payload);

                        }
                	} else {
                		__ALC_UTILS_DEBUG("route template: object closed: tsi: %u, toi: %u, filename: %s, LCT: codepoint: %d, S-TSID: codepoint: %d, formatId: %d, frag: %d, order: %d, lct packets: %d",
							alc_packet->def_lct_hdr->tsi,
							alc_packet->def_lct_hdr->toi,
							new_file_name,
							alc_packet->def_lct_hdr->codepoint,
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->codepoint,
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->format_id,
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->frag,
							atsc3_route_s_tsid_RS_LS->atsc3_route_s_tsid_RS_LS_SrcFlow->atsc3_route_s_tsid_RS_LS_SrcFlow_Payload->order,
							atsc3_route_object->atsc3_route_object_lct_packet_received_v.count);
                	}
                }

            }
			//emit lls alc context callback
			if(lls_sls_alc_monitor->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_callback) {
				lls_sls_alc_monitor->atsc3_lls_sls_alc_on_object_close_flag_s_tsid_content_location_callback(alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, s_tsid_content_location);

			}

			//jjustman-2020-07-28 - purge our lct_packet_received list as we are moved, and remove atsc3_route_object from flow
			//atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects will handle object unlinking for media fragment
			//purging of full atsc3_route_object occurs in atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows
		   atsc3_route_object_set_object_recovery_complete(atsc3_route_object);
        }
	} else {
		__ALC_UTILS_IOTRACE("dumping to file step: %s, is complete: %d", temporary_recovery_filename, alc_packet->close_object_flag);
	}

	__ALC_UTILS_IOTRACE("checking tsi: %u, toi: %u, close_object_flag: %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

cleanup:

	if(temporary_recovery_filename) {
		free(temporary_recovery_filename);
		temporary_recovery_filename = NULL;
	}

	if(s_tsid_content_location) {
		free(s_tsid_content_location);
		s_tsid_content_location = NULL;
	}

	return bytesWritten;
}


void __alc_prepend_fragment_with_init_box(char* file_name, atsc3_alc_packet_t* alc_packet) {

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

void __alc_recon_fragment_with_init_box(char* file_name, atsc3_alc_packet_t* alc_packet, uint32_t tsi, uint32_t toi_init, const char* to_write_filename) {


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
		__ALC_UTILS_TRACE("read bytes: %" PRIu64, read_bytes);

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

void alc_recon_file_ptr_fragment_with_init_box(FILE* output_file_ptr, udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet, uint32_t to_match_toi_init) {
	int flush_ret = 0;
	if(!__ALC_RECON_FILE_PTR_TSI || !__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_WARN("alc_recon_file_ptr_fragment_with_init_box - NULL: tsi: %p, toi: %p", __ALC_RECON_FILE_PTR_TSI, __ALC_RECON_FILE_PTR_TOI_INIT);
		return;
	}

	char* file_name = alc_packet_dump_to_object_get_temporary_recovering_filename(udp_flow,
                                                                                  alc_packet);
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
		__ALC_UTILS_TRACE("read bytes: %" PRIu64, read_bytes);

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
void alc_recon_file_buffer_struct_fragment_with_init_box(pipe_ffplay_buffer_t* pipe_ffplay_buffer, udp_flow_t* udp_flow, atsc3_alc_packet_t* alc_packet) {
	int flush_ret = 0;
	if(!__ALC_RECON_FILE_PTR_TSI || !__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_WARN("alc_recon_file_ptr_fragment_with_init_box - NULL: tsi: %p, toi: %p", __ALC_RECON_FILE_PTR_TSI, __ALC_RECON_FILE_PTR_TOI_INIT);
		return;
	}

	char* file_name = alc_packet_dump_to_object_get_temporary_recovering_filename(udp_flow,
                                                                                  alc_packet);
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
		__ALC_UTILS_TRACE("read bytes: %" PRIu64 ", bytes written: %" PRIu64 ", total filesize: %" PRIu64 ", has eof input: %d", read_bytes, total_bytes_written, fragment_input_stat.st_size, has_eof);

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

/*
 * atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows
 * keep track of our current TSI/TOI's start_offset attribute for ALC flows that only provide the EXT_FTI value at the first
 * packet of the TOI flow
 *
 * additionally, set the close_object flag if our current alc packet start_offset + transfer_len will be greater than the
 * persisted EXT_FTI object transfer len in lls_sls_alc_monitor->last_..._toi_length (...: video, audio, text)
 *
 * TODO: jjustman-2020-02-28:
 *      - remove tight coupling from video/audio media essences
 *      - add in support for generic TSI flows in the lls_sls_alc_monitor route attribute tracking model (e.g. collections-c map)
 *
 * - validate that alc_packet->use_start_offset is the correct attribute to key for EXT_FTI
 *
 * jjustman-2020-03-12 - NOTE - a more robust implementation is in atsc3_alc_rx.c
 * this code path will only handle alc->use_start_offset, as atsc3_alc_rx logic that handles both start_offset and sbn_esi
 *
 * ***NOTE***: atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity MUST BE CALLED BEFORE
 *          atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows IN FLOW,
 *          OTHERWISE lls_sls_alc_monitor->last_..._toi will be overwritten and the discontinuity WILL NOT BE DETECTED!
 *
 * 	NOTE: jjustman-2020-06-23: TODO: also parse A.3.10.2. Basic Delivery Object Recovery
 
	Upon receipt of the first ROUTE packet payload for an object, the ROUTE receiver uses the File@Transfer-Length
	attribute of the associated Extended FDT Instance, when present, to determine the length T of the object.
	When the File@Transfer- Length attribute is not present in the Extended FDT Instance, the receiver uses the
	@maxTransportSize attribute of the associated Extended FDT Instance to determine the maximum length T’ of the object.
 */


//#define __ATSC3_ALC_UTILS_CHECK_CLOSE_FLAG_ON_TOI_LENGTH_PERSIST__
//jjustman-2020-03-25 - workaround for digicap packager that is only emitting EXT_FTI on the very first packet of the TOI, and no close object flag

atsc3_route_object_t* atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows(atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
    atsc3_sls_alc_flow_t* matching_sls_alc_flow = NULL;
	atsc3_route_object_t* atsc3_route_object = NULL;

	if(lls_sls_alc_monitor) {
		//jjustman-2020-08-04 - hack-ish - todo: is this the best place to call this?
		//jjustman-2020-08-04 - NOTE: clear out our candidate purged route objects first, then search for our S_TSID flow lookup next
		atsc3_lls_sls_alc_monitor_check_all_s_tsid_flows_has_given_up_route_objects(lls_sls_alc_monitor);

		atsc3_lls_sls_alc_monitor_increment_lct_packet_received_count(lls_sls_alc_monitor);

		atsc3_route_object = atsc3_sls_alc_flow_route_object_add_unique_lct_packet_received(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, alc_packet);
	}

	__ALC_UTILS_DEBUG("atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows: complete, tsi: %d, toi: %d, lls_sls_alc_monitor is: %p, size: %d, atsc3_route_object: %p",
			alc_packet->def_lct_hdr->tsi,
			alc_packet->def_lct_hdr->toi,
			lls_sls_alc_monitor,
			lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v.count,
			atsc3_route_object);

	return atsc3_route_object;
}


/*
 * atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity:
 *
 * check our alc_packet TOI value to determine if we have a value less than our last closed TOI object,
 * signalling a wraparound or loop of our input source (e.g. STLTP or ALP replay or RFcapture replay) and
 * force a re-patch of the MPD on the next MBMS emission.
 *
 * the wrapaound check is limited to only TSI flows containing a/v/stpp media essense id's
 * that are monitored in the lls_sls_alc_monitor, and the TOI_init objects are ignored from this check.
 *
 * if detected, force a rebuild of the mpd with updated availabiltyStartTime and relevant startNumber values for each TSI flow/essence
 * will be checked at the next MBMS emission when the carouseled MPD is written to disk, and patched accordingly in
 * atsc3_route_sls_patch_mpd_availability_start_time_and_start_number *
 *
 * ***NOTE***: atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity MUST BE CALLED BEFORE
 *              atsc3_alc_persist_route_object_lct_packet_received_for_lls_sls_alc_monitor_all_flows IN FLOW,
 *              OTHERWISE lls_sls_alc_monitor->last_..._toi will be overwritten and the discontinuity WILL NOT BE DETECTED!
 *
 *
 *              jjustman-2020-07-28 - TODO: validate this functionality after refactoring...
 */
void atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity(atsc3_alc_packet_t* alc_packet, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
    if (lls_sls_alc_monitor) {

        uint32_t tsi = alc_packet->def_lct_hdr->tsi;
        uint32_t toi = alc_packet->def_lct_hdr->toi;

        //don't re-set double-set set our sls_alc_monitor flag for discontigious toi
        //jjustman-2020-03-11 - TODO: mutex lock this parameter during this check

        if(!lls_sls_alc_monitor->has_discontiguous_toi_flow) {

        	atsc3_sls_alc_flow_t* matching_sls_alc_flow = NULL;

			if((matching_sls_alc_flow = atsc3_sls_alc_flow_find_entry_tsi(&lls_sls_alc_monitor->atsc3_sls_alc_all_s_tsid_flow_v, alc_packet->def_lct_hdr->tsi))) {
				if(!lls_sls_alc_monitor->has_discontiguous_toi_flow && matching_sls_alc_flow->toi_init != toi && matching_sls_alc_flow->last_closed_toi > toi) {
					lls_sls_alc_monitor->has_discontiguous_toi_flow = true;
					if (lls_sls_alc_monitor->last_mpd_payload) {
						block_Destroy(&lls_sls_alc_monitor->last_mpd_payload);
					}
					
					//trigger a re-patch when we have matching toi flows
					if (lls_sls_alc_monitor->last_mpd_payload_patched) {
						block_Destroy(&lls_sls_alc_monitor->last_mpd_payload_patched);
					}
					
	                __ALC_UTILS_INFO("atsc3_alc_packet_check_monitor_flow_for_toi_wraparound_discontinuity: has discontigious re-wrap of TOI flow(s), "
	                                 "tsi: %d, toi: %d, matching_sls_alc_flow->last_closed_toi: %d",
	                                 tsi, toi, matching_sls_alc_flow->last_closed_toi);
					
					//clear out our last_closed_toi
					matching_sls_alc_flow->last_closed_toi = 0;

				}
			}
        }
    }
}


