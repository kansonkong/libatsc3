/*
 * atsc3_baseband_packet_types.h
 *
 *  Created on: Aug 1, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_BASEBAND_PACKET_TYPES_H_
#define ATSC3_BASEBAND_PACKET_TYPES_H_

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"

#include "atsc3_ip_udp_rtp_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

/*
 A/322 PLP -

 5.2.1 Mapping ALP Packets to Baseband Packets

 Since ALP packets may be split across Baseband Packets, the start of the payload of a Baseband Packet does not necessarily signify the start of an ALP packet. The Base Field of a Baseband Packet shall provide the start position of the first ALP packet that begins in the Baseband Packet through a pointer.

 The value of the pointer shall be the offset (in bytes) from the beginning of the payload to the start of the first ALP packet that begins in that Baseband Packet.

 When an ALP packet begins at the start of the payload portion of a Baseband Packet, the value of the pointer shall be 0.

 When there is no ALP packet starting within that Baseband Packet, the value of the pointer shall be 8191 and a 2 byte Base Field shall be used.

 When there are no ALP packets and only padding is present, the value of the pointer shall also be 8191 and a 2 byte Base Field shall be used, together with any necessary Optional Fields and Extension Fields as signaled by the OFI (Optional Field Indicator) field.
 **/

typedef struct atsc3_baseband_packet {
	uint8_t		plp_num;

	atsc3_rtp_header_timestamp_t	bootstrap_timing_data_timestamp_short_reference;

    uint8_t     base_field_mode;    //1 bit
    uint16_t    base_field_pointer; //either 7 bits or 13 bits
    uint8_t     option_field_mode;  //                  2 bits
    uint8_t     ext_type;           //                        3 bits
    uint16_t    ext_len;            //
    uint8_t*    extension;          // 0-31 bytes, or 0-full BBP

    block_t*    alp_payload_pre_pointer; //TODO: jjustman-2019-08-09 - is this the right place for the pre/post ALP pointers?
    block_t*    alp_payload_post_pointer;
} atsc3_baseband_packet_t;

void atsc3_baseband_packet_free_v(atsc3_baseband_packet_t* atsc3_baseband_packet);
void atsc3_baseband_packet_free(atsc3_baseband_packet_t** atsc3_baseband_packet);
void atsc3_baseband_packet_free_no_alp_pointer_release(atsc3_baseband_packet_t** atsc3_baseband_packet);

#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_BASEBAND_PACKET_TYPES_H_ */
