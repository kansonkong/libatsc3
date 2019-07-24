/*
 * atsc3_alp_parser.h
 *
 *  Created on: May 1, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_ALP_PARSER_H_
#define ATSC3_ALP_PARSER_H_

#include <pcap.h>
#include <string.h>

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_alp_types.h"
#include "atsc3_stltp_types.h"

#if defined (__cplusplus)
extern "C" {
#endif

/*
 A/322 PLP - 5.2.2.1 Base Field
 Since ALP packets may be split across Baseband Packets, the start of the payload of a Baseband Packet does not necessarily signify the start of an ALP packet. The Base Field of a Baseband Packet shall provide the start position of the first ALP packet that begins in the Baseband Packet through a pointer.
 
 The value of the pointer shall be the offset (in bytes) from the beginning of the payload to the start of the first ALP packet that begins in that Baseband Packet.
 
 When an ALP packet begins at the start of the payload portion of a Baseband Packet, the value of the pointer shall be 0.
 
 When there is no ALP packet starting within that Baseband Packet, the value of the pointer shall be 8191 and a 2 byte Base Field shall be used.
 
 When there are no ALP packets and only padding is present, the value of the pointer shall also be 8191 and a 2 byte Base Field shall be used, together with any necessary Optional Fields and Extension Fields as signaled by the OFI (Optional Field Indicator) field.
 
 **/
 
typedef struct atsc3_baseband_packet_header {
    uint8_t     base_field_mode;    //1 bit
    uint16_t    base_field_pointer; //either 7 bits or 13 bits
    uint8_t     option_field_mode;  //                  2 bits
    uint8_t     ext_type;           //                        3 bits
    uint16_t    ext_len;            //
    uint8_t*    extension;          // 0-31 bytes, or 0-full BBP
} atsc3_baseband_packet_header_t;
    
void atsc3_alp_parse_stltp_baseband_packet(atsc3_stltp_baseband_packet_t* atsc3_stltp_baseband_packet);

//stltp reflection

extern pcap_t* descrInject;

    

#if defined (__cplusplus)
}
#endif

#define __ALP_PARSER_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ALP_PARSER_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ALP_PARSER_INFO(...)  if(_ALP_PARSER_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __ALP_PARSER_DEBUG(...) if(_ALP_PARSER_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }

#endif /* ATSC3_ALP_PARSER_H_ */
