/*
 * atsc3_alc_rx.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_alc_rx.h"

/**
 *
 *
 *
 *                          Note for ATSC3 compatability:
 *
 *                          A.3.6 LCT Building Block
The LCT packet header fields shall be used as defined by the LCT building block in RFC 5651 [26]. The semantics and usage of the following LCT header fields shall be further constrained in ROUTE as follows:
123
ATSC A/331:2017 A/331, Annex A 6 December 2017
 Version number (V) – This 4-bit field indicates the protocol version number. The version number for this specification is ‘0001’.

Congestion Control flag (C) field – This 2-bit field, as defined in RFC 5651 [26], shall be set to ‘00’.
Protocol-Specific Indication (PSI) – This 2-bit field indicates whether the current packet is a source packet or an FEC repair packet. As the ROUTE source protocol only delivers source packets, this field shall be set to ‘10’.

Transport Session Identifier flag (S) – This 1-bit field shall be set to ‘1’ to indicate a 32-bit word in the TSI field.
Transport Object Identifier flag (O) – This 2-bit field shall be set to ‘01’ to indicate the number of full 32-bit words in the TOI field.

Half-word flag (H) – This 1-bit field shall be set to ‘0’ to indicate that no half-word field sizes are used.

Codepoint (CP) – This 8-bit field is used to indicate the type of the payload that is carried by this packet, and for ROUTE, is defined as shown below in Table A.3.6 to indicate the type of delivery object carried in the payload of the associated ROUTE packet. Depending on the type of the payload, additional payload header(s) may be added to prefix the payload data.

Transport Session Identifier (TSI) – This 32-bit field shall identify the Transport Session in ROUTE. The context of the Transport Session is provided by signaling metadata. The TSI field is constrained to a length of 32 bits because the Transport Session Identifier flag (S) must be set to ‘1’ and the Half-word flag (H) must be set to ‘0’.

Transport Object Identifier (TOI) – This 32-bit field shall identify the object within this session to which the payload of the current packet belongs. The mapping of the TOI field to the object is provided by the Extended FDT. The TOI field is constrained to a length of 32 bits because the Transport Object Identifier flag (O) must be set to ‘01’ and the Half-word flag (H) must be set to ‘0’.


The main changes that ROUTE introduces to the usage of the LCT building block are the following:
• ROUTE limits the usage of the LCT building block to a single channel per session. Congestion control is thus sender-driven in ROUTE.
The functionality of receiver-driven layered multicast may still be offered by the application, allowing the receiver application to select the appropriate delivery session based on the bandwidth requirement of that session.
 *
 *
 *
 *
 *The MSB of the PSI shall be set to 1 to indicate a source packet.

o In accordance to ALC, a source FEC Payload ID header is used to identify, for FEC
purposes, the encoding symbols of the delivery object,
or a portion thereof, carried by the associated ROUTE packet.

This information may be sent in several ways:
 As a simple new null FEC scheme with the following usage:

• The value of the source FEC Payload ID header shall be set to 0,
in case the ROUTE packet contains the entire delivery object, or

• The value of the source FEC Payload ID header shall be set as a
direct address (start offset) corresponding to the starting byte
position of the portion of the object carried in this packet using a 32-bit field.

 In a compatible manner to RFC 6330 [28] where the SBN and ESI defines the
start offset together with the symbol size T.
 The signaling metadata provides the appropriate parameters to
indicate any of the above modes using the @srcFecPayloadId attribute.


 * Notes:
 *
 * Asynchronous Layered Coding (ALC) Protocol Instantiation
 * 	https://tools.ietf.org/html/rfc5775
 *
 * Layered Coding Transport (LCT) Building Block
 *  https://tools.ietf.org/html/rfc5651
 *
 * RaptorQ
 * 	https://tools.ietf.org/html/rfc6330
 *
 *
 *
 *
 *
 *
 */

int _ALC_RX_DEBUG_ENABLED = 0;
int _ALC_RX_TRACE_ENABLED = 0;
int _ALC_RX_TRACE_TAB_ENABLED = 0;

typedef struct route_fragment {
	unsigned long long tsi;
	unsigned long long toi;

	uint8_t *fragment;
	unsigned long long pos;
	long size;

} route_fragment_t;

void alc_packet_free(atsc3_alc_packet_t** alc_packet_ptr) {
	atsc3_alc_packet_t* alc_packet = *alc_packet_ptr;
	if(alc_packet) {
		if(alc_packet->def_lct_hdr) {
			free(alc_packet->def_lct_hdr);
			alc_packet->def_lct_hdr = NULL;
		}

		if(alc_packet->alc_payload) {
			free(alc_packet->alc_payload);
			alc_packet->alc_payload = NULL;
		}

		free(alc_packet);
		*alc_packet_ptr = NULL;
	}
}


int alc_rx_analyze_packet_a331_compliant(char *data, int len, atsc3_alc_packet_t** alc_packet_ptr) {

	int retval = -1;
	int header_pos = 0;			//keep track of where we are in the header parsing data[]

	int het = 0;
	int hel = 0;
	int exthdrlen = 0;
	unsigned int word = 0;
    
	short fec_enc_id = 0; 
	unsigned long long ull = 0;
	unsigned long long block_len = 0;
	unsigned long long pos = 0;

	/* LCT header upto CCI */

	atsc3_def_lct_hdr_t *def_lct_hdr = NULL;

	/* remaining LCT header fields*/

	/* EXT_FDT */

	unsigned short flute_version = 0; /* V */
	int fdt_instance_id = 0; /* FDT Instance ID */

	/* EXT_CENC */

	unsigned char content_enc_algo = 0; /* CENC */
	unsigned short reserved = 0; /* Reserved */ 

	/* EXT_FTI */

	unsigned long long transfer_len = 0; /* L */
	uint64_t	transfer_len_scratch_64t;

	unsigned char finite_field = 0; /* m */
	unsigned char nb_of_es_per_group = 0; /* G */
	unsigned short es_len = 0; /* E */
	unsigned short sb_len = 0;
	unsigned int max_sb_len = 0; /* B */
	unsigned short max_nb_of_es = 0; /* max_n */
    
	/* EXT_ROUTE_PRESENTATION_TIME	 */
	bool 	 ext_route_presentation_ntp_timestamp_set = false;
	uint64_t ext_route_presentation_ntp_timestamp = 0;
	uint64_t ext_route_scratch_64t;

	int fec_inst_id = 0; /* FEC Instance ID */

	char *buf = NULL;


	unsigned short j = 0;
	unsigned short nb_of_symbols = 0;
	
	if(len < (int)(sizeof(atsc3_def_lct_hdr_t))) {
		ALC_RX_ERROR("analyze_packet: packet too short %d", len);
		retval = -1;
		goto error;

	}

	ALC_RX_TRACE("alc_rx.c: first 2 bytes are: 0x%02X 0x%02X", data[0]&0xFF, data[1]&0xFF);

	//fix for endianness.
	//byte 1
	def_lct_hdr = calloc(1, sizeof(atsc3_def_lct_hdr_t));
	def_lct_hdr->version = (data[0] >> 4) & 0xF;
	def_lct_hdr->flag_c = (data[0] >> 2) & 0x3;
	def_lct_hdr->psi = (data[0]) & 0x3;

	//atsc3 A.3.6 LCT specifiation checks
	if(def_lct_hdr->version != 1) {
		ALC_RX_ERROR("LCT Header error: Version (v) must be 1, value is: %hu", def_lct_hdr->version);
	}
	if(def_lct_hdr->flag_c != 0) {
		ALC_RX_ERROR("LCT Header error: Congestion Control (C) must be 0, value is: %hu", def_lct_hdr->flag_c);
	}
	if(def_lct_hdr->psi != 2) {
		//e.g. bit 10 pattern
		ALC_RX_ERROR("LCT Header error: Protocol-Specific Indication (PSI) must be 2, value is: %hu", def_lct_hdr->psi);
	}

	//byte 2, header_pos=1
	header_pos++;
	def_lct_hdr->flag_s = (data[header_pos]>>7) & 0x1;
	if(def_lct_hdr->flag_s != 1) {
		ALC_RX_ERROR("LCT Header error: Transport Session Identifier flag (S) must be 1, value is: %hu", def_lct_hdr->flag_s);
	}

	def_lct_hdr->flag_o = (data[header_pos]>>5) & 0x3;

	if(def_lct_hdr->flag_o != 1) {
		ALC_RX_ERROR("LCT Header error: Transport Object Identifier flag (O) must be 1, value is: %hu", def_lct_hdr->flag_o);
	}

	def_lct_hdr->flag_h = (data[header_pos]>>4) & 0x1;
	if(def_lct_hdr->flag_h != 0) {
		ALC_RX_ERROR("LCT Header error: Half-word flag (H) must be 0, value is: %hu", def_lct_hdr->flag_h);
	}

	def_lct_hdr->reserved = (data[header_pos]>>2) & 0x3;

	if(def_lct_hdr->reserved != 0) {
		ALC_RX_ERROR("Reserved field not zero - 0x%x", def_lct_hdr->reserved);
		retval = -1;
		goto error;
	}

	def_lct_hdr->flag_a = (data[header_pos]>>1) & 0x1; //close session flag
	def_lct_hdr->flag_b = (data[header_pos]) & 0x1; //close object flag

	//byte3
	header_pos++;

	def_lct_hdr->hdr_len_raw = data[header_pos];
	def_lct_hdr->hdr_len = data[header_pos] * 4;

	//byte4
	header_pos++;

	def_lct_hdr->codepoint = data[header_pos];

	//byte 5
	header_pos++;

	def_lct_hdr->cci = __readuint32(data, header_pos);
	ALC_RX_TRACE("ALC: tsi: %u, toi: %u, def_lct_hdr->flag_c: %d, header_len is: %d, cci is: %u",
			def_lct_hdr->tsi,
			def_lct_hdr->toi,
			def_lct_hdr->flag_c, header_pos, def_lct_hdr->cci);
	header_pos += 4;

	def_lct_hdr->tsi = __readuint32(data, header_pos);
	header_pos += 4;

	def_lct_hdr->toi = __readuint32(data, header_pos);
	header_pos += 4;

	ALC_RX_TRACE("ALC: tsi: %u, toi: %u, codepoint: %u", def_lct_hdr->tsi , def_lct_hdr->toi, def_lct_hdr->codepoint);


	if(def_lct_hdr->flag_a == 1) {
		ALC_RX_TRACE("flag_a, close session flag: 1 ");
	}

	//jjustman-2020-07-27 - TODO: confirm this is 'correct'
	fec_enc_id = def_lct_hdr->codepoint;

	//for any codepoint <=128...
	if(!(fec_enc_id == COM_NO_C_FEC_ENC_ID || fec_enc_id == RS_FEC_ENC_ID ||
		fec_enc_id == SB_SYS_FEC_ENC_ID || fec_enc_id == SIMPLE_XOR_FEC_ENC_ID)) {
			ALC_RX_TRACE("ALC: tsi: %u, toi: %u, FEC Encoding ID: %i is not supported, ignoring!",
					def_lct_hdr->tsi,
					def_lct_hdr->toi,
					fec_enc_id);
	}

	//if we have extra data in the header we haven't read yet, process it as an extension
	if(def_lct_hdr->hdr_len > header_pos) {

		/* LCT header extensions(EXT_FDT, EXT_CENC, EXT_FTI, EXT_AUTH, EXT_NOP)
		go through all possible EH */

		exthdrlen = def_lct_hdr->hdr_len - header_pos;
		ALC_RX_TRACE("ALC: tsi: %u, toi: %u, ext header loop, def_lct_hdr->hdr_len: %d, exthdrlen: %d, header_pos:%d", def_lct_hdr->tsi, def_lct_hdr->toi, def_lct_hdr->hdr_len, exthdrlen, header_pos);

		while(exthdrlen > 0) {
			word = 0x00000000;

			//process any header extensions here
			het = data[header_pos++] & 0xFF;

			if(het < 128) {
				hel = data[header_pos++] & 0xFF;
				word |= (data[header_pos++] << 8) & 0xFF00;
				word |= (data[header_pos++]) & 0xFF;
			} else {
				word  = (data[header_pos++] << 16) & 0xFF0000;
				word |= (data[header_pos++] << 8) & 0xFF00;
				word |= (data[header_pos++]) & 0xFF;
			}
			exthdrlen-=4;

			ALC_RX_TRACE("ALC: tsi: %u, toi: %u, def_lct_hdr->hdr_len: %d, exthdrlen: %d, hdrlen:%d, het: %d, hel: %d",  def_lct_hdr->tsi, def_lct_hdr->toi, def_lct_hdr->hdr_len, exthdrlen, header_pos, het, hel);

			switch(het) {

			  case EXT_FDT:
				  ALC_RX_DEBUG("ALC: tsi: %u, toi: %u, EXT_FDT: def_lct_hdr->hdr_len: %d, exthdrlen: %d, hdrlen:%d, het: %d, hel: %d", def_lct_hdr->tsi, def_lct_hdr->toi, def_lct_hdr->hdr_len, exthdrlen, header_pos, het, hel);

				  flute_version = (word & 0x00F00000) >> 20;
				  fdt_instance_id = (word & 0x000FFFFF);

				  if(flute_version != FLUTE_VERSION_1 && flute_version != FLUTE_VERSION_2) {
					  ALC_RX_WARN("ALC: tsi: %u, toi: %u, FLUTE version: %i is not supported",  def_lct_hdr->tsi, def_lct_hdr->toi, flute_version);
					  retval = -1;
					  goto error;
				  }
				  break;

			  case EXT_CENC:
				  ALC_RX_DEBUG("EXT_CENC: tsi: %u, toi: %u, def_lct_hdr->hdr_len: %d, exthdrlen: %d, hdrlen:%d, het: %d, hel: %d",  def_lct_hdr->tsi, def_lct_hdr->toi, def_lct_hdr->hdr_len, exthdrlen, header_pos, het, hel);

				  content_enc_algo = (word & 0x00FF0000) >> 16;
				  reserved = (word & 0x0000FFFF);

				  if(reserved != 0) {
					  ALC_RX_WARN("Bad CENC header extension!");
					  retval = -1;
					  goto error;
				  }

				#ifdef USE_ZLIB
					  if((content_enc_algo != 0) && (content_enc_algo != ZLIB)) {
						  ALC_RX_WARN("Only NULL or ZLIB content encoding supported with FDT Instance!");
						  retval = -1;
						  goto error;
					  }
				#else
					  if(content_enc_algo != 0) {
						  ALC_RX_WARN("Only NULL content encoding supported with FDT Instance!");
						  retval = -1;
						  goto error;
					  }
				#endif

				break;

			  case EXT_FTI:
				  /**
				   * https://tools.ietf.org/html/rfc3926 - FLUTE
				   */

				  ALC_RX_TRACE("ALC: tsi: %u, toi: %u, EXT_FTI: %i, def_lct_hdr->hdr_len: %d, exthdrlen: %d, header_pos:%d, het: %d, hel: %d",  def_lct_hdr->tsi, def_lct_hdr->toi, hel, def_lct_hdr->hdr_len, exthdrlen, header_pos, het, hel);

				  if(hel != 4) {
					  ALC_RX_WARN("Bad FTI header extension, length: %i", hel);
					  retval = -1;
					  goto error;
				  }

				  //6 bytes for transfer len
				  transfer_len = ((word & 0x0000FFFF) << 16);

				  transfer_len |= __readuint32(data, header_pos);
				  header_pos+=4;
				  exthdrlen-=4;
				  ALC_RX_TRACE("Reading FTI TSI: transfer len: %llu", transfer_len);
				  ALC_RX_TRACE("ALC: tsi: %u, toi: %u, transfer_len: %llu, def_lct_hdr->hdr_len: %d, exthdrlen: %d, header_pos:%d, het: %d, hel: %d", def_lct_hdr->tsi, def_lct_hdr->toi, transfer_len, def_lct_hdr->hdr_len, exthdrlen, header_pos, het, hel);

				  word = __readuint32(data, header_pos);
				  header_pos+=4;
				  exthdrlen-=4;
				  ALC_RX_TRACE("def_lct_hdr->hdr_len: %d, exthdrlen: %d, header_pos:%d, het: %d, hel: %d", def_lct_hdr->hdr_len, exthdrlen, header_pos, het, hel);
				  //jdj-2019-05-29 - replace our FEC instance id with the EXT_FTI value rather than the codepoint definition
				  fec_enc_id = (word >> 16) & 0xFF;


				  if(fec_enc_id == RS_FEC_ENC_ID) {
					  finite_field = (word & 0xFF000000) >> 24;
					  nb_of_es_per_group = (word & 0x00FF0000) >> 16;

					  /*if(finite_field < 2 || finite_field >16) {
						  printf("Finite Field parameter: %i not supported!", finite_field);
						  return HDR_ERROR;
					  }*/
				  }	  else {
					  fec_inst_id = ((word & 0xFFFF0000) >> 16);

					  if((fec_enc_id == COM_NO_C_FEC_ENC_ID || fec_enc_id == SIMPLE_XOR_FEC_ENC_ID)
						  && fec_inst_id != 0) {
						  	  ALC_RX_ERROR("Bad FTI header extension.");
							  retval = -1;
							  goto error;
					  }
					  else if(fec_enc_id == SB_SYS_FEC_ENC_ID && fec_inst_id != REED_SOL_FEC_INST_ID) {
						  ALC_RX_ERROR("FEC Encoding %i/%i is not supported!", fec_enc_id, fec_inst_id);
						  retval = -1;
						  goto error;
					  }
				  }

				  if(((fec_enc_id == COM_NO_C_FEC_ENC_ID) || (fec_enc_id == SIMPLE_XOR_FEC_ENC_ID)
					  ||(fec_enc_id == SB_LB_E_FEC_ENC_ID) || (fec_enc_id == COM_FEC_ENC_ID))){

					  	  es_len = (word & 0x0000FFFF);

						  max_sb_len = __readuint32(data, header_pos);
						  ALC_RX_TRACE("ALC: tsi: %u, toi: %u, doing max_sb_len %d", def_lct_hdr->tsi, def_lct_hdr->toi, max_sb_len);

						  header_pos += 4;
						  exthdrlen -=4;
				  }  else if(((fec_enc_id == RS_FEC_ENC_ID) || (fec_enc_id == SB_SYS_FEC_ENC_ID))) {

					  es_len = (word & 0x0000FFFF);

					  word = __readuint32(data, header_pos);

					  max_sb_len = ((word & 0xFFFF0000) >> 16);
					  max_nb_of_es = (word & 0x0000FFFF);
					  ALC_RX_TRACE("ALC: tsi: %u, toi: %u, doing RS_FEC_ENC_ID/SB_SYS_FEC_ENC_ID, max_sb_len: %d, max_nb_of_es: %d",  def_lct_hdr->tsi, def_lct_hdr->toi, max_sb_len, max_nb_of_es);

					  header_pos += 4;
					  exthdrlen-=4;
				  }
				  break;

			  case EXT_AUTH:
				  /* ignore */
				  ALC_RX_DEBUG("ignoring EXT_AUTH: tsi: %u, toi: %u",  def_lct_hdr->tsi, def_lct_hdr->toi);

				  //magic?
				  header_pos += (hel-1) << 2;
				  exthdrlen -= (hel-1) << 2;
				  break;

			  case EXT_NOP:
				  /* ignore */
				  ALC_RX_DEBUG("ignoring EXT_NOP: tsi: %u, toi: %u",  def_lct_hdr->tsi, def_lct_hdr->toi);
				  header_pos += (hel-1) << 2;
				  exthdrlen -= (hel-1) << 2;
				  break;

			  case EXT_TIME:
				  /* ignore */
				  ALC_RX_DEBUG("ignoring EXT_TIME: tsi: %u, toi: %u",  def_lct_hdr->tsi, def_lct_hdr->toi);

				  header_pos += (hel-1) << 2;
				  exthdrlen -= (hel-1)  << 2;
				  break;

			  case EXT_ROUTE_PRESENTATION_TIME:
				  /* from atsc a/331-2017:
				   *
				   *
					 0                   1                   2                     3
					 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					| HET = 66    | HEL           | reserved                        |
					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					| NTP timestamp, most significant word                          |
					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					| NTP timestamp, least significant word                         |
					+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					*/


				  ext_route_scratch_64t = __readuint32(data, header_pos);
				  header_pos += 4;
				  exthdrlen-=4;
				  ext_route_presentation_ntp_timestamp |= (ext_route_scratch_64t << 32);

				  word = __readuint32(data, header_pos);
				  header_pos += 4;
				  exthdrlen-=4;
				  ext_route_presentation_ntp_timestamp |= word;

				  ext_route_presentation_ntp_timestamp_set = true;

				  ALC_RX_DEBUG("ALC: tsi: %u, toi: %u, EXT_ROUTE_PRESENTATION_TIME, value is: %" PRIu64 ,  def_lct_hdr->tsi, def_lct_hdr->toi, ext_route_presentation_ntp_timestamp);

				  break;

			  case EXT_TOL_24:
				  /* jjustman-2019-02-19 - parse out transfer len as there is no close object flag here
				   *
					  0                   1                   2                   3
					  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
					 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					 | HET = 194     | Transfer Length                               |
					 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
				   */

  				  transfer_len = (word & 0x00FFFFFF);

                  ALC_RX_DEBUG("EXT_TOL (24 bit), tsi: %u, toi: %u,  het is: %d, hel is: %d, exthdrlen: %d, toi transfer len: %llu", def_lct_hdr->tsi, def_lct_hdr->toi, het,  hel, exthdrlen, transfer_len);

                  //no additional read performed here, continue the loop
                  break;

			  case EXT_TOL_48:
				  /*
				   * jjustman-2019-03-30 - supporting 48 bit tol
				   *
				   *
	 			      0                   1                   2                   3
			 	 	  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			 	 	 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					 | HET = 67      | HEL           |                               |
					 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
					 | Transfer Length                                               |
					 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
					 Figure A.4.3 EXT_TOL Header 48-bit version
				   */

				  transfer_len_scratch_64t = (word & 0x0000FFFF);

				  transfer_len = (transfer_len_scratch_64t<< 32) ;
				  word = __readuint32(data, header_pos);
				  transfer_len |= word;

				  header_pos += 4;
				  exthdrlen-=4;
				  ALC_RX_DEBUG("EXT_TOL (48 bit), tsi: %u, toi: %u,  het is: %d, hel is: %d, exthdrlen: %d, toi transfer len: %llu", def_lct_hdr->tsi, def_lct_hdr->toi, het,  hel, exthdrlen, transfer_len);

				  break;

			  default:
				  ALC_RX_ERROR("Unknown LCT Extension header, het: %i", het);
				  return -1;
				  break;
			}
		}
	}

	if(header_pos != def_lct_hdr->hdr_len) {
		/* Wrong header length */
		ALC_RX_WARN("ALC: analyze_packet: tsi: %u, toi: %u, packet header length %d, should be %d",  def_lct_hdr->tsi, def_lct_hdr->toi, header_pos, def_lct_hdr->hdr_len);
		  retval = -1;
		  goto error;
	}

	/* Check if we have an empty packet without FEC Payload ID */
	if(header_pos == len) {
		retval = -2;
		ALC_RX_WARN("ALC: analyze_packet: tsi: %u, toi: %u, empty packet!",  def_lct_hdr->tsi, def_lct_hdr->toi);

		goto error;
	}

	/***
     *
     A.3.5.1 FEC Payload ID for Source Flows
     
     The syntax of the FEC Payload ID for the Compact No-Code FEC Scheme
     used in ROUTE source flows shall be a 32-bit unsigned integer
     value that expresses the start_offset of the fragment.
     
     Figure A.3.3 diagrams the 32-bit start_offset field.
     
      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                         start_offset                          |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     Figure A.3.3 FEC Payload ID for Source Flows
     
     
     A.3.5.2 FEC Payload ID for Repair Flows
    
     In accordance with RFC 6330 [28] Section 3.2, the FEC Payload ID
     for the RaptorQ FEC Scheme used for repair flows is composed of a
     Source Block Number (SBN) and an Encoding Symbol ID,
     formatted as shown in Figure A.3.4.
     
      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |     SBN     |                Encoding Symbol ID               |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     Figure A.3.4 FEC Payload ID for Repair Flows
    
	 *
	 */

    atsc3_alc_packet_t* alc_packet = calloc(1, sizeof(atsc3_alc_packet_t));
    *alc_packet_ptr = alc_packet;

    alc_packet->def_lct_hdr = def_lct_hdr;
    alc_packet->fec_encoding_id = fec_enc_id;

    uint32_t fec_payload_id_to_parse;
	fec_payload_id_to_parse = __readuint32(data, header_pos);
	header_pos += 4;
    
	alc_packet->alc_len = len - header_pos;
    alc_packet->transfer_len = transfer_len;
	alc_packet->close_object_flag = def_lct_hdr->flag_b;
	alc_packet->close_session_flag = def_lct_hdr->flag_a;
	alc_packet->ext_route_presentation_ntp_timestamp_set = ext_route_presentation_ntp_timestamp_set;
	alc_packet->ext_route_presentation_ntp_timestamp = ext_route_presentation_ntp_timestamp;


    if(alc_packet->fec_encoding_id == SB_LB_E_FEC_ENC_ID) {
        alc_packet->use_sbn_esi = true;
        alc_packet->sbn = (fec_payload_id_to_parse >> 24) & 0xFF; //upper 8 bits for sbn
        alc_packet->esi = (fec_payload_id_to_parse) & 0x00FFFFFF; //lower 24 bits
        //final check to see if we should "force" this object closed, raptorq fec doesn't send a close_object flag...
        //transfer len should be set on the alc session for this toi, not just on the lct packet...
        if(alc_packet->transfer_len  > 0 && alc_packet->transfer_len  == (alc_packet->alc_len + alc_packet->esi)) {
            ALC_RX_TRACE("ALC: tsi: %u, toi: %u, FLUTE: setting close_object_flag because transfer len: (%llu) == alc_packet->alc_len (%u) + alc_packet->esi (%u)",
                         alc_packet->def_lct_hdr->tsi,
                         alc_packet->def_lct_hdr->toi,
                         alc_packet->transfer_len,
                         alc_packet->alc_len,
                         alc_packet->esi);

        	alc_packet->close_object_flag = true;
        }
        ALC_RX_TRACE("ALC: tsi: %u, toi: %u, FEC Encoding ID: %i, sbn: %u, esi: %u, transfer_len: %llu, alc_len+esi: %u",
         		alc_packet->def_lct_hdr->tsi,
   				alc_packet->def_lct_hdr->toi,
        		alc_packet->fec_encoding_id,
				alc_packet->sbn,
				alc_packet->esi,
				transfer_len, alc_packet->alc_len + alc_packet->esi);
    } else {
        alc_packet->use_start_offset = true;
        alc_packet->start_offset = fec_payload_id_to_parse;
        ALC_RX_TRACE("ALC: tsi: %u, toi: %u, start offset: %u",
        		alc_packet->def_lct_hdr->tsi,
				alc_packet->def_lct_hdr->toi,
				alc_packet->start_offset);

        //jdj-2019-05-29 - hack for missing close_object flag on a single ALC payload toi
        if(alc_packet->transfer_len > 0 && alc_packet->transfer_len == (alc_packet->alc_len + alc_packet->start_offset)) {
        	ALC_RX_TRACE("ALC: tsi: %u, toi: %u, FLUTE: setting close_object_flag because transfer len: (%llu) == alc_packet->alc_len (%u) + alc_packet->start_offset (%u)",
            		alc_packet->def_lct_hdr->tsi,
					alc_packet->def_lct_hdr->toi,
					alc_packet->transfer_len,
					alc_packet->alc_len,
					alc_packet->start_offset);
            alc_packet->close_object_flag = true;
        } 
    }

	alc_packet->alc_payload = calloc(alc_packet->alc_len, sizeof(uint8_t));

	memcpy(alc_packet->alc_payload, &data[header_pos], alc_packet->alc_len);

	//header_pos: %u,
	ALC_RX_DEBUG("ALC: tsi: %u, toi: %u, fec_encoding_id: %u, SBN: %u, esi: %u, packet length: %u, start_offset: %u, transfer_len: %llu, codepoint: %u, close_session: %u, close_object: %u, ext_route_presentation_ntp_timestamp: %llu",
			alc_packet->def_lct_hdr->tsi,
			alc_packet->def_lct_hdr->toi,
			alc_packet->fec_encoding_id,
			alc_packet->sbn,
			alc_packet->esi,
			alc_packet->alc_len,
			alc_packet->start_offset,
			alc_packet->transfer_len,
			alc_packet->def_lct_hdr->codepoint,
			alc_packet->close_session_flag,
			alc_packet->close_object_flag,
			alc_packet->ext_route_presentation_ntp_timestamp);

	ALC_RX_TRACE_TAB("ALC\ttsi\t%u\ttoi\t%u\tfec_encoding_id\t%u\tSBN\t%u\tesi\t%u\tpacket_length\t%u\tstart_offset\t%u\ttransfer_len\t%llu\tcodepoint\t%u\tclose_session\t%u\tclose_object\t%u\text_route_presentation_ntp_timestamp\t%llu",
				alc_packet->def_lct_hdr->tsi,
				alc_packet->def_lct_hdr->toi,
				alc_packet->fec_encoding_id,
				alc_packet->sbn,
				alc_packet->esi,
				alc_packet->alc_len,
				alc_packet->start_offset,
				alc_packet->transfer_len,
				alc_packet->def_lct_hdr->codepoint,
				alc_packet->close_session_flag,
				alc_packet->close_object_flag,
				alc_packet->ext_route_presentation_ntp_timestamp);


	return 0;

error:
	if(def_lct_hdr) {
		free(def_lct_hdr);
		def_lct_hdr = NULL;
	}

	return retval;

}
