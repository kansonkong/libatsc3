/*
 * atsc3_mmt_signaling_message.h
 *
 *  Created on: Jan 21, 2019
 *      Author: jjustman
 */


/**
 *
 * Citiation from from A/331 Section 7.2.3
 *
 * ATSC A/331:2017 Signaling, Delivery, Synchronization, and Error Protection 6 December 2017
 *
 * MMTP-Specific Signaling Message
 *
 * When MMTP sessions are used to carry an ATSC 3.0 streaming service, MMTP-specific signaling messages specified in Clause 10
 * of ISO/IEC 23008-1 [37] are delivered in binary format by MMTP packets according to Signaling Message Mode specified in
 * subclause 9.3.4 of ISO/IEC 23008-1 [37].
 *
 * The value of the packet_id field of MMTP packets carrying Service Layer Signaling shall be set to 0x0000 except for
 * MMTP packets carrying MMTP-specific signaling messages specific to an Asset, which shall
 * be set to 0x0000 or the same packet_id value as the MMTP packets carrying the Asset.
 *
 * Identifiers referencing the appropriate Package for each ATSC 3.0 Service are signaled by the USBD fragment as
 * described in Table 7.4.
 *
 * MMT Package Table (MPT) messages with matching MMT_package_id shall be delivered on the MMTP session signaled in the SLT.
 *
 * Each MMTP session carries MMTP-specific signaling messages specific to its session or each asset delivered by the MMTP session.
 *
 * The following MMTP messages shall be delivered by the MMTP session signaled in the SLT:
 *
 *    MMT Package Table (MPT) message: This message carries an MP (MMT Package) table which contains the list of all Assets and
 * their location information as specified in subclause 10.3.4 of ISO/IEC 23008-1) [37].
 *    MMT ATSC3 (MA3) message mmt_atsc3_message(): This message carries system metadata specific for ATSC 3.0 services including
 *
 * Service Layer Signaling as specified in Section 7.2.3.1.
 *
 * The following MMTP messages shall be delivered by the MMTP session signaled in the SLT, if required:
 *
 *   Media Presentation Information (MPI) message: This message carries an MPI table which contains the whole document or a
 * subset of a document of presentation information. An MP table associated with the MPI table also can be delivered by this
 * message (see subclause 10.3.3 of ISO/IEC 23008-1) [37];
 *
 * The following MMTP messages shall be delivered by the MMTP session carrying an associated Asset and the value of the
 * packet_id field of MMTP packets carrying them shall be set to the same as the MMTP packets carrying the Asset::
 *
 *   Hypothetical Receiver Buffer Model message: This message carries information required by the receiver to manage its
 * buffer (see subclause 10.4.2 of ISO/IEC 23008-1 [37]);
 *
 *   Hypothetical Receiver Buffer Model Removal message: This message carries information required by the receiver to
 * manage its MMT de-capsulation buffer (see subclause 10.4.9 of ISO/IEC 23008-1) [37];
 *
 */


#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <arpa/inet.h>
#endif

#ifndef ATSC3_MMT_SIGNALLING_MESSAGE_H
#define ATSC3_MMT_SIGNALLING_MESSAGE_H


#ifdef __cplusplus
extern "C" {
#endif


#include "atsc3_utils.h"
#include "atsc3_gzip.h"
#include "atsc3_logging_externs.h"
#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmtp_parser.h"
#include "atsc3_lls_types.h"

//#include "atsc3_mmt_context_signalling_information_depacketizer.h"
#include "atsc3_mmt_context_mfu_depacketizer.h"


/**
 *
 * MPU_timestamp_descriptor message example
 *
0000   62 02 00 23 af b9 00 00 00 2b 4f 2f 00 35 10 58   b..#¯¹...+O/.5.X
0010   a4 00 00 00 00 12 ce 00 3f 12 ce 00 3b 04 01 00   ¤.....Î.?.Î.;...
0020   00 00 00 00 00 00 00 10 11 11 11 11 11 11 11 11   ................
0030   11 11 11 11 11 11 11 11 68 65 76 31 fd 00 ff 00   ........hev1ý.ÿ.
0040   01 5f 90 01 00 00 23 00 0f 00 01 0c 00 00 16 ce   ._....#........Î
0050   df c2 af b8 d6 45 9f ff                           ßÂ¯¸ÖE.ÿ

raw base64 payload:

62020023afb90000002b4f2f00351058a40000000012ce003f12ce003b04010000000000000000101111111111111111111111111111111168657631fd00ff00015f9001000023000f00010c000016cedfc2afb8d6459fff
 *
 */

//parse mmtp_packet_header for signalling_message extraction, freeing packet_header with concrete object
mmtp_signalling_packet_t* mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(mmtp_packet_header_t** mmtp_packet_header_p, block_t* udp_packet);

mmtp_signalling_packet_t* mmtp_signalling_packet_parse_from_block_t(mmtp_packet_header_t* mmtp_packet_header, block_t* udp_packet);
//parse mmtp_signalling_packet_t, calls mmt_signalling_message_parse_id_type
uint8_t mmt_signalling_message_parse_packet(mmtp_signalling_packet_t* mmtp_signalling_packet, block_t* udp_packet_block);
    
//context callbacks
void mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet_t* mmtp_signalling_packet, lls_sls_mmt_session_t* matching_lls_sls_mmt_session);
    

void mmt_signalling_message_dump(mmtp_signalling_packet_t* mmtp_signalling_packet);
void pa_message_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload);
void mpi_message_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload);
void mpt_message_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload);
void mmt_atsc3_message_payload_dump(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload);


/**
 * Internal MMT-SI processing methods
 */
uint8_t mmt_signalling_message_parse_id_type(mmtp_signalling_packet_t* mmtp_signalling_packet, block_t* udp_packet_block);

mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload_create(uint16_t message_id, uint8_t version);

uint8_t* pa_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet_block);
uint8_t* mpi_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet_block);
uint8_t* mpt_message_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet_block);
uint8_t* mmt_atsc3_message_payload_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet_block);
uint8_t* mmt_scte35_message_payload_parse(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet_block);

uint8_t* si_message_not_supported(mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload, block_t* udp_packet_block);

#ifdef __cplusplus
}
#endif

#define __MMSM_ERROR(...)   		__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMSM_ERROR_23008_1(...)  	if(_MMT_SIGNALLING_MESSAGE_ERROR_23008_1_ENABLED) { __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);}

#define __MMSM_WARN(...)   			__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMSM_INFO(...)   			__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
    
#define __MMSM_DEBUG(...)   		if(_MMT_SIGNALLING_MESSAGE_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __MMSM_TRACE(...)   		if(_MMT_SIGNALLING_MESSAGE_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#endif /* ATSC3_MMT_SIGNALLING_MESSAGE_H */
