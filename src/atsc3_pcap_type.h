/*
 * atsc3_pcap_type.h
 *
 *  Created on: Oct 9, 2019
 *      Author: jjustman
 *
 *      from: https://github.com/hokiespurs/velodyne-copter/wiki/PCAP-format#targetText=The%20pcap%20file%20format%20is,data%20each%20with%20individual%20headers.
 *
 *
 *

Global Header (24 Bytes)

bytes	type	Name			Description
-----	------	------------	------------------------------------------------------
4		uint32	magic_number	'A1B2C3D4' means the endianness is correct
2		uint16	version_major	major number of the file format
2		uint16	version_minor	minor number of the file format
4		int32	thiszone		correction time in seconds from UTC to local time (0)
4		uint32	sigfigs			accuracy of time stamps in the capture (0)
4		uint32	snaplen			max length of captured packed (65535)
4		uint32	network			type of data link (1 = ethernet)

Packet Header

PCAP Packet Header (16 Bytes)

bytes	type	Name		Description
-----	------	------		-----------------
4		uint32	ts_sec		timestamp seconds
4		uint32	ts_usec		timestamp microseconds
4		uint32	incl_len	number of octets of packet saved in file
4		uint32	orig_len	actual length of packet

Ethernet Header (14 Bytes)

bytes	Name
-----	----
6		Destination MAC address
6		Source MAC address
2		Ethernet Type

 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#ifndef ATSC3_PCAP_TYPE_H_
#define ATSC3_PCAP_TYPE_H_

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"


#define ATSC3_PCAP_MIN_GLOBAL_AND_PACKET_HEADER_LENGTH 24+16
#define ATSC3_PCAP_MIN_GLOBAL_AND_PACKET_AND_ETH_HEADER_LENGTH 24+16+14

//global header length: 24 bytes
typedef struct atsc3_pcap_global_header {
	uint32_t	magic_number;		//0xa1b2c3d4 (identical byte order), or 0xd4c3b2a1 (swapped)
	uint16_t	version_major;		//2.4
	uint16_t	version_minor;
	int32_t		thiszone;
	uint32_t	sigfigs;
	uint32_t	snaplen;
	uint32_t	network;			//type of datalink, 1 for ethernet
} atsc3_pcap_global_header_t;

//pcap packet header length: 16 bytes
typedef struct atsc3_pcap_packet_header {
	uint32_t	ts_sec;				//timestamp seconds
	uint32_t	ts_usec;			//uSec of when this packet was captured
	uint32_t	incl_len;			//incl_len: the number of bytes of packet data actually captured and saved in the file. This value should never become larger than orig_len or the snaplen value of the global header.
	uint32_t	orig_len;			//orig_len: the length of the packet as it appeared on the network when it was captured. If incl_len and orig_len differ, the actually saved packet size was limited by snaplen.

} atsc3_pcap_packet_header_t;

//The actual packet data will immediately follow the packet header as a data blob of incl_len bytes without a specific byte alignment.

typedef struct uint48 {
	unsigned long long v:48;
} uint48_t __attribute__((packed));

//pcap ethernet header: 14 bytes
typedef struct atsc3_pcap_packet_ethernet_header {
	uint48_t 	destination_mac_addr;
	uint48_t 	source_mac_addr;
	uint16_t	ethernet_type;

} atsc3_pcap_ethernet_header_t;

typedef struct atsc3_pcap_packet_instance {
	atsc3_pcap_packet_header_t		atsc3_pcap_packet_header;

	block_t*						current_pcap_packet;
} atsc3_pcap_packet_instance_t;

typedef struct atsc3_pcap_replay_context {
	char* 							pcap_file_name;

	FILE* 							pcap_fp;
	uint32_t						pcap_file_len;
	uint32_t						pcap_file_pos;

	bool							has_read_atsc3_pcap_global_header;
	atsc3_pcap_global_header_t		atsc3_pcap_global_header;

	uint32_t						pcap_read_packet_count;

	atsc3_pcap_packet_instance_t	atsc3_pcap_packet_instance;
	uint32_t						current_ts_sec;
	uint32_t						current_ts_usec;

} atsc3_pcap_replay_context_t;


atsc3_pcap_replay_context_t* atsc3_pcap_replay_open_filename(const char* pcap_filename);
atsc3_pcap_replay_context_t* atsc3_pcap_replay_iterate_packet(atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_to_iterate);
atsc3_pcap_replay_context_t* atsc3_pcap_replay_usleep_packet(atsc3_pcap_replay_context_t* atsc3_pcap_replay_context_to_iterate);




#define _ATSC3_PCAP_TYPE_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_TYPE_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_TYPE_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_PCAP_TYPE_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);





#endif /* ATSC3_PCAP_TYPE_H_ */
