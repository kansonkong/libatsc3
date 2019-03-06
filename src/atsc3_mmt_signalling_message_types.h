/*
 * atsc3_mmt_signalling_message_types.h
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_SIGNALLING_MESSAGE_TYPES_H_
#define ATSC3_MMT_SIGNALLING_MESSAGE_TYPES_H_


//signaling message - message id values:

#define PA_message 			0x0000

#define MPI_message			0x0001
#define MPI_message_start 	0x0001
#define MPI_message_end	 	0x0010

#define MPT_message			0x0011
#define MPT_message_start	0x0011
#define MPT_message_end		0x0020
//		RESERVED			0x0021 ~ 0x01FF

#define	CRI_message			0x0200
#define	DCI_message			0x0201
#define	SSWR_message		0x0202
#define	AL_FEC_message		0x0203
#define	HRBM_message		0x0204
#define	MC_message			0x0205
#define	AC_message			0x0206
#define	AF_message			0x0207
#define	RQF_message			0x0208
#define	ADC_message			0x0209
#define	HRB_removal_message	0x020A
#define	LS_message			0x020B
#define	LR_message			0x020C
#define	NAMF_message		0x020D
#define	LDC_message			0x020E
//Reserved for private use 0x8000 ~ 0xFFFF
#define	MMT_ATSC3_MESSAGE_ID	0x8100


//10.2.2 signalling message header

typedef struct mmt_signalling_message_header {
	uint16_t 	message_id;
	uint8_t 	version;
	uint32_t 	length;
	uint16_t	MESSAGE_id_type; //see #defines above
} mmt_signalling_message_header_t;


/**
 * from A/331 2017 - page 64
 * table 7.6 code vaules for atsc3_message_content_type:
 *
 	atsc3_message_content_type 	Meaning
 	--------------------------	-------------
 	0x0000						ATSC Reserved
 	0x0001						UserServiceDescription as given in Table 7.4.
	0x0002						MPD as given in DASH-IF [12].
	0x0003						HELD as given in A/337, Application Signaling [7].
	0x0004						Application Event Information as given in A/337, Application Signaling [7].
	0x0005						Video Stream Properties Descriptor (Sec. 7.2.3.2)
	0x0006						ATSC Staggercast Descriptor (Sec. 7.2.3.3)
	0x0007						Inband Event Descriptor as given in A/337, Application Signaling [7].
	0x0008						Caption Asset Descriptor (Sec. 7.2.3.5)
	0x0009						Audio Stream Properties Descriptor (Sec. 7.2.3.4)
	0x000A						DWD as given in A/337, Application Signaling [7].
	0x000B~0xFFFF				ATSC Reserved

 */

enum ATSC3_MESSAGE_CONTENT_TYPE {
	ATSC3_MESSAGE_CONTENT_TYPE_ATSC_RESERVED=0,
	ATSC3_MESSAGE_CONTENT_TYPE_USERSERVICEDESCRIPTION=1,
	ATSC3_MESSAGE_CONTENT_TYPE_MPD=2,
	ATSC3_MESSAGE_CONTENT_TYPE_HELD=3,
	ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION=4,
	ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR=5,
	ATSC3_MESSAGE_CONTENT_TYPE_STAGGERCAST_DESCRIPTOR=6,
	ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR=7,
	ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR=8,
	ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES=9,
	ATSC3_MESSAGE_CONTENT_TYPE_DWD=0xA,
	ATSC3_MESSAGE_CONTENT_TYPE_ATSC_RESERVED_OTHER=-1
};

enum ATSC3_MESSAGE_CONTENT_COMPRESSION {
	ATSC3_MESSAGE_CONTENT_COMPRESSION_RESERVED=0,
	ATSC3_MESSAGE_CONTENT_COMPRESSION_NONE=1,
	ATSC3_MESSAGE_CONTENT_COMPRESSION_GZIP=2,
	ATSC3_MESSAGE_CONTENT_COMPRESSION_TEMPLATE=3,
	ATSC3_MESSAGE_CONTENT_COMPRESSION_RESERVED_OTHER=-1
};

typedef struct mmt_atsc3_message_payload {
	uint16_t 	service_id;
	uint16_t 	atsc3_message_content_type;
	uint8_t		atsc3_message_content_version;
	uint8_t		atsc3_message_content_compression;

	uint8_t 	URI_length;
	uint8_t* 	URI_payload;

	uint32_t	atsc3_message_content_length;
	uint8_t*	atsc3_message_content;

	uint32_t	atsc3_message_content_length_compressed;
	uint8_t*	atsc3_message_content_compressed;

	//reserved:	8 bits to pad out length
	//i < length - 11 - URI_length - atsc3_message_content_length
} mmt_atsc3_message_payload_t;



//table 58 - asset id descriptor
typedef struct asset_id {
	uint32_t	asset_id_scheme;
	uint32_t	asset_id_length;
	uint8_t*	asset_id;
} asset_id_t;



//from table 59 - identifier mapping

typedef struct url_length {
	uint16_t	length;
	uint8_t* 	byte;
} url_length_t;

/**
 * identifer mapping:  table 59
 *
 * identifier_type values:
 *
 * 	0x00	identifier of the content is provided as an asset_id
 * 	0x01	a list of URL's that are related togther and share the same packet_id mapping
 * 	0x02	identifier is provided with a regex string used to match urls
 * 	0x03 	provided as a DASH representation@id
 * 	0x04	reserved for private identifiers
 */

typedef struct identifier_mapping {
	uint8_t		identifier_type;

	//if(identifier_type == 0x00)
	asset_id_t 		asset_id;
	//else if type == 0x01

	uint16_t		url_count;
	url_length_t*	url_length_list;

	//else if type == 0x02
	uint16_t		regex_length;
	uint8_t*		regex_byte;

	//else if identifier_type == 0x03
	uint16_t		representation_id_length;
	uint8_t*		representation_id_byte;

	//else
	uint16_t		private_length;
	uint8_t*		private_byte;

} identifier_mapping_t;

typedef struct mp_table_descriptors {
	uint16_t	mp_table_descriptors_length;
	uint8_t*	mp_table_descriptors_byte;
} mp_table_descriptors_t;

typedef struct mmt_package_id {
	uint8_t		mmt_package_id_length;
	uint8_t*	mmt_package_id;
} mmt_package_id_t;


typedef struct mmt_general_location_info {
	uint8_t location_type;
	uint16_t packet_id;
	uint32_t ipv4_src_addr;
	uint32_t ipv4_dst_addr;
	uint16_t dst_port;
	//ignoring ipv6
	uint16_t message_id;

} mmt_general_location_info_t;

typedef struct mp_table_asset_row {
	identifier_mapping_t identifier_mapping;

	//identifer_mapping()
	uint32_t	asset_type;
	//6 bits reserved
	uint8_t		default_asset_flag;

	uint8_t		asset_clock_relation_flag;
	uint8_t		asset_clock_relation_id;
	//7bits reserved
	uint8_t		asset_timescale_flag;
	uint32_t	asset_timescale;

	//asset_location (
	uint8_t		location_count;
	mmt_general_location_info_t mmt_general_location_info;
	//asset_descriptors (
	uint16_t	asset_descriptors_length;
	uint8_t*	asset_descriptors;

} mp_table_asset_row_t;

typedef struct mp_table {
	uint8_t					table_id;
	uint8_t					version;
	uint16_t				length;
	//6 bits are reserved
	uint8_t					mp_table_mode;

	//table_id==0x20 || table_id==0x11 - mmt_package_id
	mmt_package_id_t 		mmt_package_id;
	//mp_table_descriptors
	mp_table_descriptors_t 	mp_table_descriptors;

	uint8_t					number_of_assets;
	mp_table_asset_row_t* 	mp_table_asset_row;

} mp_table_t;

typedef struct mpt_message {
	uint16_t	message_id;
	uint8_t		version;
	uint16_t	length;
	mp_table_t  mp_table;

} mpt_message_t;

typedef struct mmt_signaling_message_mpu_tuple {
	uint32_t mpu_sequence_number;
	uint64_t mpu_presentation_time;
} mmt_signaling_message_mpu_tuple_t;

typedef struct mmt_signaling_message_mpu_timestamp_descriptor {
	uint16_t							descriptor_tag;
	uint8_t								descriptor_length;
	uint8_t								mpu_tuple_n; //mpu_tuple_n = descriptor_length/12 = (32+64)/8
	mmt_signaling_message_mpu_tuple_t*	mpu_tuple;
} mmt_signaling_message_mpu_timestamp_descriptor_t;


typedef union mmt_signalling_message_type {
	mmt_atsc3_message_payload_t			mmt_atsc3_message_payload;
	mp_table_t							mp_table;
} mmt_signalling_message_payload_u;


typedef struct mmt_signalling_message_id_type {
	mmt_signalling_message_header_t 	message_header;
	mmt_signalling_message_payload_u 	message_payload;
} mmt_signalling_message_header_and_payload_t;

typedef struct mmt_signalling_message_vector {
	int 											messages_n;
	mmt_signalling_message_header_and_payload_t** 	messages;
} mmt_signalling_message_vector_t;

#endif /* ATSC3_MMT_SIGNALLING_MESSAGE_TYPES_H_ */
