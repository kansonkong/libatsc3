/** \file alc_rx.c \brief ALC level receiving
 *
 *  $Author: peltotal $ $Date: 2007/02/28 08:58:00 $ $Revision: 1.146 $
 *
 *  MAD-ALCLIB: Implementation of ALC/LCT protocols, Compact No-Code FEC,
 *  Simple XOR FEC, Reed-Solomon FEC, and RLC Congestion Control protocol.
 *  Copyright (c) 2003-2007 TUT - Tampere University of Technology
 *  main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  In addition, as a special exception, TUT - Tampere University of Technology
 *  gives permission to link the code of this program with the OpenSSL library (or
 *  with modified versions of OpenSSL that use the same license as OpenSSL), and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify this file, you may extend this exception to your version
 *  of the file, but you are not obligated to do so. If you do not wish to do so,
 *  delete this exception statement from your version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <assert.h>

#include <stdint.h>

#ifdef _MSC_VER
#include <winsock2.h>
#include <process.h>
#include <io.h>
#else
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#endif

#include "defines.h"
#include "atsc3_alc_rx.h"
#include "alc_channel.h"
#include "mad_rlc.h"
#include "null_fec.h"
#include "xor_fec.h"
#include "rs_fec.h"
#include "utils.h"
#include "transport.h"
#include "alc_list.h"

/**
 * This is a private function which parses and analyzes an ALC packet.
 *
 * @param data pointer to the ALC packet
 * @param len length of packet
 * @param ch pointer to the channel
 *
 * @return status of packet [WAITING_FDT = 5, OK = 4, EMPTY_PACKET = 3, HDR_ERROR = 2,
 *                          MEM_ERROR = 1, DUP_PACKET = 0]
 *
 *
 *
 *
 *                          Note for ATSC3 compat:
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

typedef struct route_fragment {
	unsigned long long tsi;
	unsigned long long toi;

	uint8_t *fragment;
	unsigned long long pos;
	long size;

} route_fragment_t;

void alc_packet_free(alc_packet_t** alc_packet_ptr) {
	alc_packet_t* alc_packet = *alc_packet_ptr;
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


int alc_rx_analyze_packet_a331_compliant(char *data, int len, alc_channel_t *ch, alc_packet_t** alc_packet_ptr) {

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
//
//    trans_obj_t *trans_obj = NULL;
//    trans_block_t *trans_block = NULL;
//    trans_unit_t *trans_unit = NULL;
//    trans_unit_t *tu = NULL;
//    trans_unit_t *next_tu = NULL;
//    wanted_obj_t *wanted_obj = NULL;

	char *buf = NULL;

	char filename[MAX_PATH_LENGTH];
	double rx_percent = 0;
	
	unsigned short j = 0;
	unsigned short nb_of_symbols = 0;
	
	if(len < (int)(sizeof(atsc3_def_lct_hdr_t))) {
		ALC_RX_ERROR("analyze_packet: packet too short %d", len);
		retval = HDR_ERROR;
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
		retval = HDR_ERROR;
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
		ch->s->state = SAFlagReceived;
		ALC_RX_TRACE("flag_a, close session flag: 1 ");
	}

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

				  if(flute_version != FLUTE_VERSION) {
					  ALC_RX_WARN("ALC: tsi: %u, toi: %u, FLUTE version: %i is not supported",  def_lct_hdr->tsi, def_lct_hdr->toi, flute_version);
					  retval = HDR_ERROR;
					  goto error;
				  }
				  break;

			  case EXT_CENC:
				  ALC_RX_DEBUG("EXT_CENC: tsi: %u, toi: %u, def_lct_hdr->hdr_len: %d, exthdrlen: %d, hdrlen:%d, het: %d, hel: %d",  def_lct_hdr->tsi, def_lct_hdr->toi, def_lct_hdr->hdr_len, exthdrlen, header_pos, het, hel);

				  content_enc_algo = (word & 0x00FF0000) >> 16;
				  reserved = (word & 0x0000FFFF);

				  if(reserved != 0) {
					  ALC_RX_WARN("Bad CENC header extension!");
					  retval = HDR_ERROR;
					  goto error;
				  }

				#ifdef USE_ZLIB
					  if((content_enc_algo != 0) && (content_enc_algo != ZLIB)) {
						  ALC_RX_WARN("Only NULL or ZLIB content encoding supported with FDT Instance!");
						  retval = HDR_ERROR;
						  goto error;
					  }
				#else
					  if(content_enc_algo != 0) {
						  ALC_RX_WARN("Only NULL content encoding supported with FDT Instance!");
						  retval = HDR_ERROR;
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
					  retval = HDR_ERROR;
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
							  retval = HDR_ERROR;
							  goto error;
					  }
					  else if(fec_enc_id == SB_SYS_FEC_ENC_ID && fec_inst_id != REED_SOL_FEC_INST_ID) {
						  ALC_RX_ERROR("FEC Encoding %i/%i is not supported!", fec_enc_id, fec_inst_id);
						  retval = HDR_ERROR;
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
				  exthdrlen -= (hel-1);
				  break;

			  case EXT_NOP:
				  /* ignore */
				  ALC_RX_DEBUG("ignoring EXT_NOP: tsi: %u, toi: %u",  def_lct_hdr->tsi, def_lct_hdr->toi);
				  header_pos += (hel-1) << 2;
				  exthdrlen -= (hel-1);
				  break;

			  case EXT_TIME:
				  /* ignore */
				  ALC_RX_DEBUG("ignoring EXT_TIME: tsi: %u, toi: %u",  def_lct_hdr->tsi, def_lct_hdr->toi);

				  header_pos += (hel-1) << 2;
				  exthdrlen -= (hel-1);
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

				  ALC_RX_DEBUG("ALC: tsi: %u, toi: %u, EXT_ROUTE_PRESENTATION_TIME, value is: %llu",  def_lct_hdr->tsi, def_lct_hdr->toi, ext_route_presentation_ntp_timestamp);

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
				  return HDR_ERROR;
				  break;
			}
		}
	}

	if(header_pos != def_lct_hdr->hdr_len) {
		/* Wrong header length */
		ALC_RX_WARN("ALC: analyze_packet: tsi: %u, toi: %u, packet header length %d, should be %d",  def_lct_hdr->tsi, def_lct_hdr->toi, header_pos, def_lct_hdr->hdr_len);
		  retval = HDR_ERROR;
		  goto error;
	}

	/* Check if we have an empty packet without FEC Payload ID */
	if(header_pos == len) {
		retval = EMPTY_PACKET;
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

    alc_packet_t* alc_packet = calloc(1, sizeof(alc_packet_t));
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
        alc_packet->sbn = (fec_payload_id_to_parse >> 24) & 0xFF;
        alc_packet->esi = (fec_payload_id_to_parse) & 0x00FFFFFF;
        //final check to see if we should "force" this object closed, raptorq fec doesn't send a close_object flag...
        //transfer len should be set on the alc session for this toi, not just on the lct packet...
        if(transfer_len > 0 && transfer_len == (alc_packet->alc_len + alc_packet->esi)) {
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
        if(alc_packet->transfer_len && alc_packet->transfer_len <= alc_packet->alc_len) {
        	ALC_RX_TRACE("ALC: tsi: %u, toi: %u, hack: setting close_object_flag because transfer len: %llu is <= alc_len: %u (start_offset: %u)",
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

	return ALC_OK;

error:
	if(def_lct_hdr) {
		free(def_lct_hdr);
		def_lct_hdr = NULL;
	}

	return retval;

}
//jdj-2019-01-31 - anything after this line needs cleanup...

////toi
//	if(len - hdrlen != 0) {
//
//		/* check if we have enough information */
//
//		//jdj-2019-01-07
////		if(((transfer_len == 0) || (fec_enc_id == -1) || ((fec_enc_id > 127) && (fec_inst_id == -1)) ||
////			(es_len == 0) || (max_sb_len == 0))) {
//
//		if(((transfer_len == 0) || (fec_enc_id == -1) || ((fec_enc_id > 127) && (fec_inst_id == -1)))) {
//#ifdef _MSC_VER
//				printf("Not enough information to create Transport Object, TOI: %s", toi);
//#else
//				printf("Not enough information to create Transport Object, TOI: %s", toi);
//#endif
//				fflush(stdout);
//				return HDR_ERROR;
//		}
//
//		if(fec_enc_id == RS_FEC_ENC_ID) {
//			nb_of_symbols = nb_of_es_per_group;
//		}
//		else {
//			/* Let's check how many symbols are in the packet */
//			/* Encoding Symbol group length = len - hdrlen */
//
//			nb_of_symbols = (unsigned short)ceil((double)(len - hdrlen) / es_len);
//		}
//
//		/* Now we have to go through each symbol */
//		printf("%d:alc_rx - num symbols: %d", __LINE__, nb_of_symbols);
//		if(!nb_of_symbols) nb_of_symbols++;
//		es_len = len - hdrlen - 4;
//
//		for(j = 0; j < nb_of_symbols; j++) {
//
//#ifdef USE_RETRIEVE_UNIT
//			/* Retrieve a transport unit from the session pool  */
//			printf("%d:alc_rx - creating trans_unit ", __LINE__);
//
//			trans_unit = retrieve_unit(ch->s, es_len);
//#else
//			/* Create transport unit */
//			trans_unit = create_units(1);
//#endif
//
//			if(trans_unit == NULL) {
//				return MEM_ERROR;
//			}
//
//			trans_unit->esi = esi + j;
//			trans_unit->len = es_len;
//
//#ifndef USE_RETRIEVE_UNIT
//			/* Alloc memory for incoming TU data */
//			if(!(trans_unit->data = (char*)calloc(es_len, sizeof(char)))) {
//				printf("Could not alloc memory for transport unit's data!");
//				return MEM_ERROR;
//			}
//#endif
//
//			memcpy(trans_unit->data, (data + hdrlen + j*es_len), trans_unit->len);
//
//			/* Check if object already exist */
//			if(toi == FDT_TOI) {
//				trans_obj = object_exist(fdt_instance_id, ch->s, 0);
//			}
//			else {
//				trans_obj = object_exist(toi, ch->s, 1);
//			}
//
//			if(trans_obj == NULL) {
//
//				printf("%d:alc_rx - creating trans_obj ", __LINE__);
//
//				trans_obj = create_object();
//
//				if(trans_obj == NULL) {
//					return MEM_ERROR;
//				}
//
//				if(toi == FDT_TOI) {
//					trans_obj->toi = fdt_instance_id;
//					trans_obj->content_enc_algo = content_enc_algo;
//				}
//				else {
//					trans_obj->toi = toi;
//
//
//					if(ch->s->rx_memory_mode == 1 || ch->s->rx_memory_mode == 2) {
//
//						memset(filename, 0, MAX_PATH_LENGTH);
//
//						if(content_enc_algo == 0) {
//							sprintf(filename, "%s/%s", ch->s->base_dir, "object_XXXXXX");
//							mktemp(filename);
//						}
//#ifdef USE_ZLIB
//						else if(content_enc_algo == GZIP) {
//							sprintf(filename, "%s/%s", ch->s->base_dir, "object_XXXXXX");
//							mktemp(filename);
//							strcat(filename, GZ_SUFFIX);
//						}
//#endif
//						else if(content_enc_algo == PAD) {
//							sprintf(filename, "%s/%s", ch->s->base_dir, "object_XXXXXX");
//							mktemp(filename);
//							strcat(filename, PAD_SUFFIX);
//						}
//
//						/* Alloc memory for tmp_filename */
//						if(!(trans_obj->tmp_filename = (char*)calloc(strlen(filename)+1, sizeof(char)))) {
//							printf("Could not alloc memory for tmp_filename!");
//							return MEM_ERROR;
//						}
//
//						memcpy(trans_obj->tmp_filename, filename, strlen(filename));
//
//#ifdef _MSC_VER
//						if((trans_obj->fd = open((const char*)trans_obj->tmp_filename,
//							_O_WRONLY | _O_CREAT | _O_BINARY | _O_TRUNC , _S_IWRITE)) < 0) {
//#else
//						if((trans_obj->fd = open(trans_obj->tmp_filename,
//							O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU)) < 0) {
//#endif
//								printf("Error: unable to open file %s", trans_obj->tmp_filename);
//								fflush(stdout);
//								return MEM_ERROR;
//						}
//					}
//
//
//					if(ch->s->rx_memory_mode == 2) {
//
//						/* when receiver is in large file mode a tmp file is used to store the data symbols */
//
//						memset(filename, 0, MAX_PATH_LENGTH);
//						sprintf(filename, "%s/%s", ch->s->base_dir, "st_XXXXXX");
//						mktemp(filename);
//
//						/* Alloc memory for tmp_st_filename */
//						if(!(trans_obj->tmp_st_filename = (char*)calloc(strlen(filename)+1, sizeof(char)))) {
//							printf("Could not alloc memory for tmp_st_filename!");
//							return MEM_ERROR;
//						}
//
//						memcpy(trans_obj->tmp_st_filename, filename, strlen(filename));
//
//#ifdef _MSC_VER
//						if((trans_obj->fd_st = open((const char*)trans_obj->tmp_st_filename,
//							_O_RDWR | _O_CREAT | _O_BINARY | _O_TRUNC , _S_IREAD | _S_IWRITE)) < 0) {
//#else
//						if((trans_obj->fd_st = open(trans_obj->tmp_st_filename,
//							O_RDWR | O_CREAT | O_TRUNC , S_IRWXU)) < 0) {
//#endif
//								printf("Error: unable to open file %s", trans_obj->tmp_st_filename);
//								fflush(stdout);
//								return MEM_ERROR;
//						}
//					}
//				}
//
//				trans_obj->len = transfer_len;
//				trans_obj->fec_enc_id = (unsigned char)fec_enc_id;
//				trans_obj->fec_inst_id = (unsigned short)fec_inst_id;
//				trans_obj->es_len = es_len;
//				trans_obj->max_sb_len = max_sb_len;
//
//				/* Let's calculate the blocking structure for this object */
//
//				trans_obj->bs = compute_blocking_structure(transfer_len, max_sb_len, es_len);
//
//				if(!(trans_obj->block_list = (trans_block_t*)calloc(trans_obj->bs->N, sizeof(trans_block_t)))) {
//					printf("Could not alloc memory for transport block list!");
//					return MEM_ERROR;
//				}
//
//				if(toi == FDT_TOI) {
//					insert_object(trans_obj, ch->s, 0);
//				}
//				else {
//					insert_object(trans_obj, ch->s, 1);
//				}
//			}
//
//			trans_block = trans_obj->block_list+sbn;
//
//			if(trans_block->nb_of_rx_units == 0) {
//				trans_block->sbn = sbn;
//
//				printf("%d:alc_rx - using fec_enc_id: %d", __LINE__, fec_enc_id);
//
//				if(fec_enc_id == COM_NO_C_FEC_ENC_ID) {
//
//					if(sbn < trans_obj->bs->I) {
//						trans_block->k = trans_obj->bs->A_large;
//
//					}
//					else {
//						trans_block->k = trans_obj->bs->A_small;
//					}
//					printf("%d:alc_rx - trans block->k %d", __LINE__, trans_block->k);
//
//				}
//				else if(fec_enc_id == SB_SYS_FEC_ENC_ID) {
//
//					trans_block->k = sb_len;
//					trans_block->max_k = max_sb_len;
//					trans_block->max_n = max_nb_of_es;
//				}
//				else if(fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
//
//					if(sbn < trans_obj->bs->I) {
//						trans_block->k = trans_obj->bs->A_large;
//					}
//					else {
//						trans_block->k = trans_obj->bs->A_small;
//					}
//
//					trans_block->max_k = max_sb_len;
//				}
//				else if(fec_enc_id == RS_FEC_ENC_ID) {
//
//					if(sbn < trans_obj->bs->I) {
//						trans_block->k = trans_obj->bs->A_large;
//					}
//					else {
//						trans_block->k = trans_obj->bs->A_small;
//					}
//
//					trans_block->max_k = max_sb_len;
//					trans_block->max_n = max_nb_of_es;
//
//					/*trans_block->finite_field = finite_field;*/
//				}
//			}
//
//			printf("%d:alc_rx - line ", __LINE__);
//
//			if(!block_ready_to_decode(trans_block)) {
//
//				if(insert_unit(trans_unit, trans_block, trans_obj) != 1) {
//
//					if(toi == FDT_TOI || ch->s->rx_memory_mode == 0) {
//
//						if(block_ready_to_decode(trans_block)) {
//							trans_obj->nb_of_ready_blocks++;
//						} else {
//							printf("%d:alc_rx - !block_ready_to_decode ", __LINE__);
//						}
//					}
//
//					/* if large file mode data symbol is stored in the tmp file */
//					if(toi != FDT_TOI && ch->s->rx_memory_mode == 2) {
//
//#ifdef _MSC_VER
//						trans_unit->offset = _lseeki64(trans_obj->fd_st, 0, SEEK_END);
//#else
//						trans_unit->offset = lseek(trans_obj->fd_st, 0, SEEK_END);
//#endif
//						if(trans_unit->offset == -1) {
//#ifdef _MSC_VER
//							printf("lseek error, toi: %I64u", toi);
//#else
//							printf("lseek error, toi: %llu", toi);
//#endif
//							fflush(stdout);
//							set_session_state(ch->s->s_id, SExiting);
//							return MEM_ERROR;
//						}
//
//						if(write(trans_obj->fd_st, trans_unit->data, (unsigned int)trans_unit->len) == -1) {
//#ifdef _MSC_VER
//							printf("write error, toi: %I64u, sbn: %i", toi, sbn);
//#else
//							printf("write error, toi: %llu, sbn: %i", toi, sbn);
//#endif
//							fflush(stdout);
//							set_session_state(ch->s->s_id, SExiting);
//							return MEM_ERROR;
//						}
//
//#ifndef USE_RETRIEVE_UNIT
//						free(trans_unit->data);
//						trans_unit->data = NULL;
//#endif
//					}
//
//					if(((toi == FDT_TOI && ch->s->verbosity == 4) || (toi != FDT_TOI && ch->s->verbosity > 1))) {
//
//						rx_percent = (double)((double)100 *
//							((double)(long long)trans_obj->rx_bytes/(double)(long long)trans_obj->len));
//
//						if(((rx_percent >= (trans_obj->last_print_rx_percent + 1)) || (rx_percent == 100))) {
//							trans_obj->last_print_rx_percent = rx_percent;
//							printf("%.2f%% of object received (TOI=%llu LAYERS=%i)", rx_percent,
//								toi, ch->s->nb_channel);
//							fflush(stdout);
//						}
//					}
//				}
//				else {
//
//#ifdef USE_RETRIEVE_UNIT
//					trans_unit->used = 0;
//#else
//					free(trans_unit->data);
//					free(trans_unit);
//#endif
//
//					printf("%d:alc_rx - duplicate packet", __LINE__);
//
//					return DUP_PACKET;
//				}
//			}
//			else {
//
//#ifdef USE_RETRIEVE_UNIT
//				trans_unit->used = 0;
//#else
//				free(trans_unit->data);
//				free(trans_unit);
//#endif
//				return DUP_PACKET;
//			}
//
//			if(toi != FDT_TOI) {
//
//				if(ch->s->rx_memory_mode == 1 || ch->s->rx_memory_mode == 2) {
//
//					if(!block_ready_to_decode(trans_block)) {
//						printf("%d:alc_rx - block not ready to decode!", __LINE__);
//					}
//
//					if(block_ready_to_decode(trans_block)) {
//
//						if(ch->s->rx_memory_mode == 2){
//
//							/* We have to restore the data symbols to trans_units from the symbol store tmp file */
//
//							next_tu = trans_block->unit_list;
//
//							while(next_tu != NULL) {
//
//								tu = next_tu;
//
//#ifdef _MSC_VER
//								if(_lseeki64(trans_obj->fd_st, tu->offset, SEEK_SET) == -1) {
//#else
//								if(lseek(trans_obj->fd_st, tu->offset, SEEK_SET) == -1) {
//#endif
//
//#ifdef _MSC_VER
//									printf("lseek error, toi: %I64u", toi);
//#else
//									printf("alc_rx.c line 1035 lseek error, toi: %llu", toi);
//#endif
//									fflush(stdout);
//									set_session_state(ch->s->s_id, SExiting);
//									return MEM_ERROR;
//								}
//
//								/* let's copy the data symbols from the tmp file to the memory */
//
//								/* Alloc memory for restoring data symbol */
//
//								if(!(tu->data = (char*)calloc(tu->len, sizeof(char)))) {
//									printf("Could not alloc memory for transport unit's data!");
//									return MEM_ERROR;
//								}
//
//								if(read(trans_obj->fd_st, tu->data, tu->len) == -1) {
//#ifdef _MSC_VER
//									printf("read error, toi: %I64u, sbn: %i", toi, sbn);
//#else
//									printf("read error, toi: %llu, sbn: %i", toi, sbn);
//#endif
//									fflush(stdout);
//									set_session_state(ch->s->s_id, SExiting);
//
//									return MEM_ERROR;
//								}
//
//								next_tu = tu->next;
//							}
//						}
//
//						/* decode the block and save data to the tmp file */
//
//						if(fec_enc_id == COM_NO_C_FEC_ENC_ID) {
//							buf = null_fec_decode_src_block(trans_block, &block_len, es_len);
//						}
//						else if(fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
//							buf = xor_fec_decode_src_block(trans_block, &block_len, es_len);
//						}
//						else if(fec_enc_id == RS_FEC_ENC_ID) {
//							buf = rs_fec_decode_src_block(trans_block, &block_len, es_len);
//						}
//						else if(fec_enc_id == SB_SYS_FEC_ENC_ID && fec_inst_id == REED_SOL_FEC_INST_ID) {
//							buf = rs_fec_decode_src_block(trans_block, &block_len, es_len);
//						}
//
//						if(buf == NULL) {
//							printf("%d:alc_rx - error!", __LINE__);
//							return MEM_ERROR;
//						}
//
//						/* We have to check if there is padding in the last source symbol of the last source block */
//
//						if(trans_block->sbn == ((trans_obj->bs->N) - 1)) {
//							block_len = (trans_obj->len - (es_len * (trans_obj->bs->I * trans_obj->bs->A_large +
//								(trans_obj->bs->N - trans_obj->bs->I -1) * trans_obj->bs->A_small)));
//						}
//
//						if(trans_block->sbn < trans_obj->bs->I) {
//							pos = ( (unsigned long long)trans_block->sbn * (unsigned long long)trans_obj->bs->A_large * (unsigned long long)es_len );
//						}
//						else {
//							pos = ( ( ( (unsigned long long)trans_obj->bs->I * (unsigned long long)trans_obj->bs->A_large ) +
//								( (unsigned long long)trans_block->sbn - (unsigned long long)trans_obj->bs->I )  *
//								(unsigned long long)trans_obj->bs->A_small ) * (unsigned long long)es_len );
//						}
//
//						/* set correct position */
//
//#ifdef _MSC_VER
//						if(_lseeki64(trans_obj->fd, pos, SEEK_SET) == -1) {
//#else
//						if(lseek(trans_obj->fd, pos, SEEK_SET) == -1) {
//#endif
//
//#ifdef _MSC_VER
//							printf("lseek error, toi: %I64u", toi);
//#else
//							printf("alc_rx.c line 1111 lseek error, toi: %llu", toi);
//#endif
//							fflush(stdout);
//							free(buf);
//							set_session_state(ch->s->s_id, SExiting);
//							return MEM_ERROR;
//						}
//
//						if(write(trans_obj->fd, buf, (unsigned int)block_len) == -1) {
//#ifdef _MSC_VER
//							printf("write error, toi: %I64u, sbn: %i", toi, sbn);
//#else
//							printf("write error, toi: %llu, sbn: %i", toi, sbn);
//#endif
//							fflush(stdout);
//							free(buf);
//							set_session_state(ch->s->s_id, SExiting);
//							return MEM_ERROR;
//						}
//
//						trans_obj->nb_of_ready_blocks++;
//
//						free(buf);
//
//#ifdef USE_RETRIEVE_UNIT
//						free_units2(trans_block);
//#else
//						free_units(trans_block);
//#endif
//
//						if(ch->s->verbosity > 2) {
//#ifdef _MSC_VER
//							printf("%u/%u Source Blocks decoded (TOI=%I64u SBN=%u)", trans_obj->nb_of_ready_blocks, trans_obj->bs->N, toi, sbn);
//							fflush(stdout);
//#else
//							printf("%u/%u Source Blocks decoded (TOI=%llu SBN=%u)", trans_obj->nb_of_ready_blocks, trans_obj->bs->N, toi, sbn);
//							fflush(stdout);
//#endif
//						}
//					}
//				}
//			}
//		} /* End of "for(j = 0; j < nb_of_symbols; j++) {" */
//	}
//	else { /* We have an empty packet with FEC Payload ID */
//		return EMPTY_PACKET;
//	}
//
//	return OK;
//}
//
///**
// * This is a private function which receives unit(s) from the session's channels.
// *
// * @param s pointer to the session
// *
// * @return number of correct packets received from ALC session, or 0 when state is SClosed or no packets,
// * or -1 in error cases, or -2 when state is SExiting
// *
// */
//
//int recv_packet(alc_session_t *s) {
//
//  char recvbuf[MAX_PACKET_LENGTH];
//  int recvlen;
//  int i;
//  int retval;
//  int recv_pkts = 0;
//  alc_channel_t *ch;
//  struct sockaddr_storage from;
//
//  double loss_prob;
//
//  alc_rcv_container_t *container;
//  int my_list_not_empty = 0;
//
//#ifdef _MSC_VER
//  int fromlen;
//#else
//  socklen_t fromlen;
//#endif
//
//  time_t systime;
//  unsigned long long curr_time;
//
//  memset(recvbuf, 0, MAX_PACKET_LENGTH);
//
//  for(i = 0; i < s->nb_channel; i++) {
//    ch = s->ch_list[i];
//
//    if(ch->receiving_list != NULL) {
//      if(!is_empty(ch->receiving_list)) {
//	++my_list_not_empty;
//	break;
//      }
//    }
//  }
//
//  if(my_list_not_empty == 0) {
//
//    if(s->stoptime != 0) {
//      time(&systime);
//      curr_time = systime + 2208988800U;
//
//      if(curr_time >= s->stoptime) {
//		s->state = SExiting;
//		return -2;
//      }
//    }
//
//#ifdef _MSC_VER
//    Sleep(500);
//#else
//    usleep(500000);
//#endif
//
//    if(s->state == SAFlagReceived) {
//      s->state = STxStopped;
//    }
//
//    return 0;
//  }
//
//  for(i = 0; i < s->nb_channel; i++) {
//    ch = s->ch_list[i];
//
//    if(!is_empty(ch->receiving_list)) {
//      assert(ch->rx_socket_thread_id != 0);
//
//      container = (alc_rcv_container_t*)pop_front(ch->receiving_list);
//
//      assert(container != NULL);
//
//      recvlen = container->recvlen;
//      from = container->from;
//      fromlen = container->fromlen;
//      memcpy(recvbuf, container->recvbuf, MAX_PACKET_LENGTH);
////      printf("%d:recv_packet: first 8 bytes are:", __LINE__);
////      for(int i=0; i < 8; i++) {
////    	  printf("0x%02X ", recvbuf[i]);
////      }
////      printf("---");
//
//      if(recvlen < 0) {
//
//	free(container);
//	container = NULL;
//
//	if(s->state == SExiting) {
//	  printf("recv_packet() SExiting");
//	  fflush(stdout);
//	  return -2;
//	}
//	else if(s->state == SClosed) {
//	  printf("recv_packet() SClosed");
//	  fflush(stdout);
//	  return 0;
//	}
//	else {
//#ifdef _MSC_VER
//	  printf("recvfrom failed: %d", WSAGetLastError());
//	  fflush(stdout);
//#else
//	  printf("recvfrom failed: %d", errno);
//#endif
//	  return -1;
//	}
//      }
//
//      loss_prob = 0;
//
//      if(ch->s->simul_losses) {
//	if(ch->previous_lost == TRUE) {
//	  loss_prob = ch->s->loss_ratio2;
//	}
//	else {
//	  loss_prob = ch->s->loss_ratio1;
//	}
//      }
//
//  //    if(!randomloss(loss_prob)) {
//    alc_packet_t* alc_packet;
//	retval = analyze_packet(recvbuf, recvlen, ch, alc_packet);
//
////	printf("%d:alc_rx: retval is: %d", __LINE__, retval);
//	if(ch->s->cc_id == RLC) {
//
//	  if(((ch->s->rlc->drop_highest_layer) && (ch->s->nb_channel != 1))) {
//
//	    ch->s->rlc->drop_highest_layer = FALSE;
//	    close_alc_channel(ch->s->ch_list[ch->s->nb_channel - 1], ch->s);
//	  }
////	}
//
//	if(retval == WAITING_FDT) {
//	  push_front(ch->receiving_list, (void*)container);
//	}
//	else {
//	  free(container);
//	  container = NULL;
//
//	  if(retval == HDR_ERROR) {
//	    continue;
//	  }
//	  else if(retval == DUP_PACKET) {
//	    continue;
//	  }
//	  else if(retval == MEM_ERROR) {
//	    return -1;
//	  }
//
//	  recv_pkts++;
//
//	  ch->previous_lost = FALSE;
//	}
//      }
//      else {
//	ch->previous_lost = TRUE;
//      }
//    }
//  }
//  return recv_pkts;
//}
////
////void* rx_socket_thread(void *ch) {
////
////  alc_channel_t *channel;
////  alc_rcv_container_t *container;
////  fd_set read_set;
////  struct timeval time_out;
////  char hostname[100];
////  int retval;
////
////  channel = (alc_channel_t *)ch;
////
////  while(channel->s->state == SActive) {
////
////    time_out.tv_sec = 1;
////    time_out.tv_usec = 0;
////
////    FD_ZERO(&read_set);
////    FD_SET(channel->rx_sock, &read_set);
////
////    retval = select((int)channel->rx_sock + 1, &read_set, 0, 0, &time_out);
////
////    if(retval > 0) {
////      if(!(container = (alc_rcv_container_t*)calloc(1, sizeof(alc_rcv_container_t)))) {
////	printf("Could not alloc memory for container!");
////	continue;
////      }
////
////      if(channel->s->addr_family == PF_INET) {
////	container->fromlen = sizeof(struct sockaddr_in);
////      }
////      else if(channel->s->addr_family == PF_INET6) {
////	container->fromlen = sizeof(struct sockaddr_in6);
////      }
////
////      container->recvlen = recvfrom(channel->rx_sock, container->recvbuf, MAX_PACKET_LENGTH,
////				    0, (struct sockaddr*)&(container->from), &(container->fromlen));
////
////#ifdef _MSC_VER
////      if(container->recvlen == -1) {
////	/* Some times when you quit program very quick after starting in Windows, select returns
////	   1, but there is nothing to be stored to the queue. Continue is for avoiding error */
////	continue;
////      }
////#endif
////
////      getnameinfo((struct sockaddr*)&(container->from), container->fromlen,
////		  hostname, sizeof(hostname), NULL, 0, NI_NUMERICHOST);
////
////      if(strcmp(channel->s->src_addr, "") != 0) {
////	if(strcmp(hostname, channel->s->src_addr) != 0) {
////	  printf("Packet to wrong session: wrong source: %s", hostname);
////	  fflush(stdout);
////	  continue;
////	}
////      }
////
////      push_back(channel->receiving_list, (void*)container);
////
////      if(strcmp(channel->s->src_addr, "") == 0) {
////	if(channel->s->verbosity > 0) {
////	  printf("Locked to source: %s", hostname);
////	  fflush(stdout);
////	}
////
////	memcpy(channel->s->src_addr, hostname, strlen(hostname));
////      }
////    }
////    else {
////      continue;
////    }
////  }
////
////#ifdef _MSC_VER
////  _endthread();
////#else
////  pthread_exit(0);
////#endif
////
////  return NULL;
////}
//
////
////void join_rx_socket_thread(alc_channel_t *ch) {
////
////#ifndef _MSC_VER
////  int join_retval;
////#endif
////
////  if(ch != NULL) {
////#ifdef _MSC_VER
////    WaitForSingleObject(ch->handle_rx_socket_thread, INFINITE);
////    CloseHandle(ch->handle_rx_socket_thread);
////#else
////    join_retval = pthread_join(ch->rx_socket_thread_id, NULL);
////    assert(join_retval == 0);
////    pthread_detach(ch->rx_socket_thread_id);
////#endif
////  }
////}
////
////void* rx_thread(void *s) {
////
////  alc_session_t *session;
////  int retval = 0;
////
////  srand((unsigned)time(NULL));
////
////  session = (alc_session_t *)s;
////
////  while(session->state == SActive || session->state == SAFlagReceived) {
////
////    if(session->nb_channel != 0) {
////      retval = recv_packet(session);
////    }
////    else {
////#ifdef _MSC_VER
////      Sleep(1);
////#else
////      usleep(1000);
////#endif
////    }
////  }
////
////#ifdef _MSC_VER
////  _endthread();
////#else
////  pthread_exit(0);
////#endif
////
////  return NULL;
////}
//
//char* alc_recv(int s_id, unsigned long long toi, unsigned long long *data_len, int *retval) {
//
//	BOOL obj_completed = FALSE;
//	alc_session_t *s;
//	char *buf = NULL; /* Buffer where to construct the object from data units */
//	trans_obj_t *to;
//	int object_exists = 0;
//
//	s = get_alc_session(s_id);
//
//	while(!obj_completed) {
//
//		if(s->state == SExiting) {
//			/*printf("alc_recv() SExiting");
//			fflush(stdout);*/
//			*retval = -2;
//			return NULL;
//		}
//		else if(s->state == SClosed) {
//			/*printf("alc_recv() SClosed");
//			fflush(stdout);*/
//			*retval = 0;
//			return NULL;
//		}
//
//		to = s->obj_list;
//
//		if(!object_exists) {
//
//			while(to != NULL) {
//				if(to->toi == toi) {
//					object_exists = 1;
//					break;
//				}
//				to = to->next;
//			}
//
//			if(to == NULL) {
//				continue;
//			}
//		}
//
//		obj_completed = object_completed(to);
//
//		if(((s->state == STxStopped) && (!obj_completed))) {
//			/*printf("alc_recv() STxStopped, toi: %i", toi);
//			fflush(stdout);*/
//			*retval = -3;
//			return NULL;
//		}
//
//#ifdef _MSC_VER
//		Sleep(1);
//#else
//		usleep(1000);
//#endif
//	}
//	printf("");
//
//	remove_wanted_object(s_id, toi);
//
//	/* Parse data from object to data buffer, return buffer and buffer length */
//
//	to = object_exist(toi, s, 1);
//
//	if(to->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
//		buf = null_fec_decode_object(to, data_len, s);
//	}
//	else if(to->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
//		buf = xor_fec_decode_object(to, data_len, s);
//	}
//	else if(to->fec_enc_id == RS_FEC_ENC_ID) {
//		buf = rs_fec_decode_object(to, data_len, s);
//	}
//	else if(to->fec_enc_id == SB_SYS_FEC_ENC_ID && to->fec_inst_id == REED_SOL_FEC_INST_ID) {
//		buf = rs_fec_decode_object(to, data_len, s);
//	}
//
//	if(buf == NULL) {
//		*retval = -1;
//	}
//
//	free_object(to, s, 1);
//	return buf;
//}
//
//char* alc_recv2(int s_id, unsigned long long *toi, unsigned long long *data_len, int *retval) {
//
//	BOOL obj_completed = FALSE;
//	alc_session_t *s;
//
//	unsigned long long tmp_toi = 0;
//
//	char *buf = NULL; /* Buffer where to construct the object from data units */
//	trans_obj_t *to;
//
//	s = get_alc_session(s_id);
//
//	while(1) {
//
//		to = s->obj_list;
//
//		if(s->state == SExiting) {
//			/*printf("alc_recv2() SExiting");
//			fflush(stdout);*/
//			*retval = -2;
//			return NULL;
//		}
//		else if(s->state == SClosed) {
//			/*printf("alc_recv2() SClosed");
//			fflush(stdout);*/
//			*retval = 0;
//			return NULL;
//		}
//		else if(((s->state == STxStopped) && (to == NULL))) {
//			/*printf("alc_recv2() STxStopped");
//			fflush(stdout);*/
//			*retval = -3;
//			return NULL;
//		}
//
//		while(to != NULL) {
//
//			if(s->state == SExiting) {
//				/*printf("alc_recv2() SExiting");
//				fflush(stdout);*/
//				*retval = -2;
//				return NULL;
//			}
//			else if(s->state == SClosed) {
//				/*printf("alc_recv2() SClosed");
//				fflush(stdout);*/
//				*retval = 0;
//				return NULL;
//			}
//
//			obj_completed = object_completed(to);
//
//			if(obj_completed) {
//				tmp_toi = to->toi;
//				break;
//			}
//
//			if(((s->state == STxStopped) && (!obj_completed))) {
//				/*printf("alc_recv2() STxStopped");
//				fflush(stdout);*/
//				*retval = -3;
//				return NULL;
//			}
//
//			to = to->next;
//		}
//
//		if(obj_completed) {
//			break;
//		}
//
//#ifdef _MSC_VER
//		Sleep(1);
//#else
//		usleep(1000);
//#endif
//	}
//
//	printf("");
//
//	remove_wanted_object(s_id, tmp_toi);
//
//	/* Parse data from object to data buffer, return buffer length */
//
//	to = object_exist(tmp_toi, s, 1);
//
//	if(to->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
//		buf = null_fec_decode_object(to, data_len, s);
//	}
//	else if(to->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
//		buf = xor_fec_decode_object(to, data_len, s);
//	}
//	else if(to->fec_enc_id == RS_FEC_ENC_ID) {
//		buf = rs_fec_decode_object(to, data_len, s);
//	}
//	else if(to->fec_enc_id == SB_SYS_FEC_ENC_ID && to->fec_inst_id == REED_SOL_FEC_INST_ID) {
//		buf = rs_fec_decode_object(to, data_len, s);
//	}
//
//	if(buf == NULL) {
//		*retval = -1;
//	}
//	else {
//		*toi = tmp_toi;
//	}
//
//	free_object(to, s, 1);
//	return buf;
//}
//
//char* alc_recv3(int s_id, unsigned long long *toi, int *retval) {
//
//	BOOL obj_completed = FALSE;
//	alc_session_t *s;
//
//	unsigned long long tmp_toi = 0;
//
//	trans_obj_t *to;
//	char *tmp_filename = NULL;
//
//	s = get_alc_session(s_id);
//
//	while(1) {
//
//		to = s->obj_list;
//
//		if(s->state == SExiting) {
//			/*printf("alc_recv3() SExiting");
//			fflush(stdout);*/
//			*retval = -2;
//			return NULL;
//		}
//		else if(s->state == SClosed) {
//			/*printf("alc_recv3() SClosed");
//			fflush(stdout);*/
//			*retval = 0;
//			return NULL;
//		}
//		else if(((s->state == STxStopped) && (to == NULL))) {
//			/*printf("alc_recv3() STxStopped, to == NULL");
//			fflush(stdout);*/
//			*retval = -3;
//			return NULL;
//		}
//
//		while(to != NULL) {
//
//			obj_completed = FALSE;
//
//			if(s->state == SExiting) {
//				/*printf("alc_recv3() SExiting");
//				fflush(stdout);*/
//				*retval = -2;
//				return NULL;
//			}
//			else if(s->state == SClosed) {
//				/*printf("alc_recv3() SClosed");
//				fflush(stdout);*/
//				*retval = 0;
//				return NULL;
//			}
//			else if(s->state == STxStopped) {
//				break;
//			}
//
//			obj_completed = object_completed(to);
//
//			if(obj_completed) {
//				tmp_toi = to->toi;
//				break;
//			}
//
//			to = to->next;
//		}
//
//		if(obj_completed) {
//			break;
//		}
//		else if(s->state == STxStopped) {
//
//			/* Check if there is completed object after A-flag is received */
//
//			to = s->obj_list;
//
//			while(to != NULL) {
//
//				obj_completed = object_completed(to);
//
//				if(obj_completed) {
//					tmp_toi = to->toi;
//					break;
//				}
//
//				to = to->next;
//			}
//
//			if(obj_completed) {
//				break;
//			}
//			else {
//				/*printf("alc_recv3() STxStopped, any object not completed");
//				fflush(stdout);*/
//				*retval = -3;
//				return NULL;
//			}
//		}
//
//#ifdef _MSC_VER
//		Sleep(1);
//#else
//		usleep(1000);
//#endif
//	}
//
//	remove_wanted_object(s_id, tmp_toi);
//
//	if(!(tmp_filename = (char*)calloc((strlen(to->tmp_filename) + 1), sizeof(char)))) {
//		printf("Could not alloc memory for tmp_filename!");
//		*retval = -1;
//		return NULL;
//	}
//
//	memcpy(tmp_filename, to->tmp_filename, strlen(to->tmp_filename));
//
//	free_object(to, s, 1);
//	*toi = tmp_toi;
//
//	return tmp_filename;
//}
//
//char* fdt_recv(int s_id, unsigned long long *data_len, int *retval,
//			   unsigned char *content_enc_algo, int* fdt_instance_id) {
//
//   alc_session_t *s;
//   char *buf = NULL; /* Buffer where to construct the object from data units */
//   trans_obj_t *to;
//
//   s = get_alc_session(s_id);
//
//   while(1) {
//	   to = s->fdt_list;
//
//	   if(s->state == SExiting) {
//		   /*printf("fdt_recv() SExiting");
//		   fflush(stdout);*/
//		   *retval = -2;
//		   return NULL;
//	   }
//	   else if(s->state == SClosed) {
//		   /*printf("fdt_recv() SClosed");
//		   fflush(stdout);*/
//		   *retval = 0;
//		   return NULL;
//	   }
//	   else if(s->state == STxStopped) {
//		   /*printf("fdt_recv() STxStopped");
//		   fflush(stdout);*/
//		   *retval = -3;
//		   return NULL;
//	   }
//
//	   if(to == NULL) {
//
//#ifdef _MSC_VER
//		   Sleep(1);
//#else
//		   usleep(1000);
//#endif
//		   continue;
//	   }
//
//	   do {
//		   if(object_completed(to)) {
//			   set_received_instance(s, (unsigned int)to->toi);
//
//			   *content_enc_algo = to->content_enc_algo;
//			   *fdt_instance_id = (int)to->toi;
//
//			   if(to->fec_enc_id == COM_NO_C_FEC_ENC_ID) {
//				   buf = null_fec_decode_object(to, data_len, s);
//			   }
//			   else if(to->fec_enc_id == SIMPLE_XOR_FEC_ENC_ID) {
//				   buf = xor_fec_decode_object(to, data_len, s);
//			   }
//			   else if(to->fec_enc_id == RS_FEC_ENC_ID) {
//				   buf = rs_fec_decode_object(to, data_len, s);
//			   }
//			   else if(to->fec_enc_id == SB_SYS_FEC_ENC_ID && to->fec_inst_id == REED_SOL_FEC_INST_ID) {
//				   buf = rs_fec_decode_object(to, data_len, s);
//			   }
//
//			   if(buf == NULL) {
//				   *retval = -1;
//			   }
//
//			   free_object(to, s, 0);
//			   return buf;
//		   }
//		   to = to->next;
//	   } while(to != NULL);
//
//#ifdef _MSC_VER
//	   Sleep(1);
//#else
//	   usleep(1000);
//#endif
//   }
//
//   return buf;
//}
//
//trans_obj_t* object_exist(unsigned long long toi, alc_session_t *s, int type) {
//
//  trans_obj_t *trans_obj = NULL;
//
//  if(type == 0) {
//	  trans_obj = s->fdt_list;
//  }
//  else if(type == 1) {
//	  trans_obj = s->obj_list;
//  }
//
//  if(trans_obj != NULL) {
//	  for(;;) {
//		  if(trans_obj->toi == toi) {
//			  break;
//		  }
//		  if(trans_obj->next == NULL) {
//			  trans_obj = NULL;
//			  break;
//		  }
//		  trans_obj = trans_obj->next;
//	  }
//  }
//
//  return trans_obj;
//}
//
//BOOL object_completed(trans_obj_t *to) {
//
//	BOOL ready = FALSE;
//
//	if(to->nb_of_ready_blocks == to->bs->N) {
//		ready = TRUE;
//	}
//
//	return ready;
//}
//
//BOOL block_ready_to_decode(trans_block_t *tb) {
//
//	BOOL ready = FALSE;
//
//	if(tb->nb_of_rx_units >= tb->k) {
//		ready = TRUE;
//	}
//
//	return ready;
//}
