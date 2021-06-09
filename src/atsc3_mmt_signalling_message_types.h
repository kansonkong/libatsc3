/*
 * atsc3_mmt_signalling_message_types.h
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_SIGNALLING_MESSAGE_TYPES_H_
#define ATSC3_MMT_SIGNALLING_MESSAGE_TYPES_H_

#if defined (__cplusplus)
extern "C" {
#endif


#include "atsc3_vector_builder.h"
#include "atsc3_mmtp_packet_types.h"

//signaling message - message id values:

#define PA_message 				0x0000

#define MPI_message				0x0001
#define MPI_message_start 		0x0001
#define MPI_message_end	 		0x0010

#define MPT_message				0x0011
#define MPT_message_start		0x0011
#define MPT_message_end			0x0020
//		RESERVED				0x0021 ~ 0x01FF

#define	CRI_message				0x0200
#define	DCI_message				0x0201
#define	SSWR_message			0x0202
#define	AL_FEC_message			0x0203
#define	HRBM_message			0x0204
#define	MC_message				0x0205
#define	AC_message				0x0206
#define	AF_message				0x0207
#define	RQF_message				0x0208
#define	ADC_message				0x0209
#define	HRB_removal_message		0x020A
#define	LS_message				0x020B
#define	LR_message				0x020C
#define	NAMF_message			0x020D
#define	LDC_message				0x020E
//Reserved for private use 		0x8000 ~ 0xFFFF


#define	MMT_ATSC3_MESSAGE_ID		0x8100
#define	SIGNED_MMT_ATSC3_MESSAGE_ID	0x8101

//From A/331:2020 - Table 7.8 Code Values for atsc3_message_content_type

#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_RESERVED			                    0x0000

#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_UserServiceDescription               0x0001

//redundant, but...as needed...
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_MPD_FROM_DASHIF                      0x0002

//HELD trigger is in the MMT SLS (SI message), not as part of the fdt-instance as in ROUTE
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_HELD                                 0x0003

//see atsc3_mmt_signalling_message.c: mmt_atsc3_message_payload_parse
// NOTE: this should be a first class citizen from the signaller direct api invocation for creating this emission,
// and will be wrapped as an  with relevant ntp_timestamp, see MMT design proposal for this use case in libatsc3
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION_A337   0x0004

#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR   0x0005
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_ATSC_STAGGERCAST_DESCRIPTOR          0x0006

//re-wrapping of upstream emsg box "translated" into an inband_event_descriptor, see A/337:2019 table 4.3 for more details
//remember the emsg box is present in the movie fragment metadata (e.g. mpu_fragment_type = 0x01), so if you are using OOO MMT, this will most likely be delivered "late",
// as the MOOF atom will come at the close of the mpu sequence/GOP, so use 0x0004 instead as a real-time SI message creation in the signaller
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR_A337         0x0007

#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR             0x0008
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES_DESCRIPTOR   0x0009
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_DWD                                  0x000A
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_RSAT_A200                            0x000B

#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR       0x000C

//jjustman-2021-06-03: TODO - implement additional SI for LA_url support
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL	0x000D

//reserved to 0x000E ~ 0xFFFF
#define MMT_ATSC3_MESSAGE_CONTENT_TYPE_RESERVED_FUTURE                      0x000E


#define MMT_SCTE35_Signal_Message		0xF337	// SCTE35_Signal_Message Type
#define MMT_SCTE35_Signal_Descriptor	0xF33F	// SCTE35_Signal_Descriptor tag


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
	ATSC3_MESSAGE_CONTENT_TYPE_ATSC_RESERVED=MMT_ATSC3_MESSAGE_CONTENT_TYPE_RESERVED,
	ATSC3_MESSAGE_CONTENT_TYPE_USERSERVICEDESCRIPTION=MMT_ATSC3_MESSAGE_CONTENT_TYPE_UserServiceDescription,
	ATSC3_MESSAGE_CONTENT_TYPE_MPD=MMT_ATSC3_MESSAGE_CONTENT_TYPE_MPD_FROM_DASHIF,
	ATSC3_MESSAGE_CONTENT_TYPE_HELD=MMT_ATSC3_MESSAGE_CONTENT_TYPE_HELD,
	ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION=MMT_ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION_A337,
	ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR=MMT_ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR,
	ATSC3_MESSAGE_CONTENT_TYPE_STAGGERCAST_DESCRIPTOR=MMT_ATSC3_MESSAGE_CONTENT_TYPE_ATSC_STAGGERCAST_DESCRIPTOR,
	ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR=MMT_ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR_A337,
	ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR=MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR,
	ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES=MMT_ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES_DESCRIPTOR,
	ATSC3_MESSAGE_CONTENT_TYPE_DWD=MMT_ATSC3_MESSAGE_CONTENT_TYPE_DWD,
	ATSC3_MESSAGE_CONTENT_TYPE_RSAT=MMT_ATSC3_MESSAGE_CONTENT_TYPE_RSAT_A200,
	ATSC3_MESSAGE_CONTENT_TYPE_SI_DESCRIPTOR=MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR,
	ATSC3_MESSAGE_CONTENT_TYPE_SI_DESCRIPTOR_LAURL=MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL,
	ATSC3_MESSAGE_CONTENT_TYPE_ATSC_RESERVED_FUTURE=MMT_ATSC3_MESSAGE_CONTENT_TYPE_RESERVED_FUTURE
};

enum ATSC3_MESSAGE_CONTENT_COMPRESSION {
	ATSC3_MESSAGE_CONTENT_COMPRESSION_RESERVED=0,
	ATSC3_MESSAGE_CONTENT_COMPRESSION_NONE=1,
	ATSC3_MESSAGE_CONTENT_COMPRESSION_GZIP=2,
	ATSC3_MESSAGE_CONTENT_COMPRESSION_TEMPLATE=3,
	ATSC3_MESSAGE_CONTENT_COMPRESSION_RESERVED_OTHER=-1
};


typedef struct mmt_signalling_information_message_descriptor_header {
	uint16_t	descriptor_tag;
	uint16_t	descriptor_length;
} mmt_signalling_information_message_descriptor_header_t;

typedef struct mmt_atsc3_message_content_type_descriptor_number_of_assets_header {
	uint16_t	descriptor_tag;
	uint16_t	descriptor_length;
	uint8_t		number_of_assets;

} mmt_atsc3_message_content_type_descriptor_number_of_assets_header_t;

typedef struct mmt_atsc3_message_content_type_asset_heaader {
	uint32_t		asset_id_length;
	uint8_t*		asset_id;
} mmt_atsc3_message_content_type_asset_heaader_t;

typedef struct mmt_atsc3_message_content_type_language_heaader {
	uint8_t		language_length;
	char*		language;
} mmt_atsc3_message_content_type_language_heaader_t;


//Used in MMT_ATSC3_MESSAGE_CONTENT_TYPE_UserServiceDescription
typedef struct mmt_atsc3_route_component {
    uint8_t*    stsid_uri_s;

    uint8_t*    stsid_destination_ip_address_s;
    uint32_t    stsid_destination_ip_address;

    uint16_t    stsid_destination_udp_port;
    uint8_t*    stsid_source_ip_address_s;
    uint32_t    stsid_source_ip_address;

    //jjustman-2020-12-08 - TODO - fixme so we don't free this pinned instance
    bool        __is_pinned_to_context;

} mmt_atsc3_route_component_t;

//Used in MMT_ATSC3_MESSAGE_CONTENT_TYPE_HELD
typedef struct mmt_atsc3_held_message {
    block_t*    held_message;
} mmt_atsc3_held_message_t;

//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_APPLICATION_EVENT_INFORMATION_A337
typedef struct mmt_atsc3_message_content_type_application_event_information_a337 {
	void* TODO;
} mmt_atsc3_message_content_type_application_event_information_a337_t;



typedef struct mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset {
	uint8_t		codec_codec[4];
	uint8_t		temporal_scalability_present:1;
	uint8_t		scalability_info_present:1;
	uint8_t		multiview_info_present:1;
	uint8_t		res_cf_bd_info_present:1;
	uint8_t		pr_info_present:1;
	uint8_t		br_info_present:1;
	uint8_t		color_info_present:1;
	uint8_t		reserved_1:1;

	//if (temporal_scalability_present) {
	//
		struct  {
			uint8_t	max_sub_layers_instream:6;
			uint8_t	sub_layer_profile_tier_level_info_present:1;
			uint8_t	temporal_filter_present:1;
			uint8_t	tid_max:3;
			uint8_t	tid_min:3;

			//if (temporal_filter_present) {
				uint8_t	tfweight:2;
			//} else {
				uint8_t	reserved_2:2;
			//}
		} temporal_scalability_info;
	//}

	//if(scalability_info_present) {
	//	mmt_atsc3_message_content_type_video_stream_properties_descriptor_scalability_info_t	scalability_info;
		struct {
			uint8_t		asset_layer_id:6;
			uint8_t		reserved:2;

		} scalability_info;
	//}

	//if(multiview_info_present) {
	//	mmt_atsc3_message_content_type_video_stream_properties_descriptor_multview_info_t		multiview_info;
		struct {
			uint8_t		view_nuh_layer_id:6;
			uint8_t		view_pos:6;
			uint8_t		reserved_4:4;
			uint16_t	min_disp_with_offset:11;
			uint16_t	max_disp_range:11;
			uint8_t		reserved_2:2;
		} multiview_info;
	//}

	//if(res_cf_bd_info_present) {
		//mmt_atsc3_message_content_type_video_stream_properties_descriptor_res_cf_bd_info_t		res_cf_bd_info;
		struct {
			uint16_t 		pic_width_in_luma_samples;
			uint16_t		pic_height_in_luma_samples;
			uint8_t			chroma_format_idc:3;
			//if(chroma_format_idc == 3) {
				uint8_t		separate_colour_plane_flag:1;
				uint8_t		reserved_3:3;
			//} else {
				uint8_t		reserved_4:4;
			//}

			uint8_t			video_still_present:1;
			uint8_t			video_24hr_pic_present:1;
			uint8_t			bit_depth_luma_minus8:4;
			uint8_t			bit_depth_chroma_minus8:4;

		} res_cf_bd_info;
	//}

	//if(pr_info_present) {
		//if(sub_layer_profile_tier_level_info_present) {
			//pr_info(max_sub_layers_instream-1)
		//} else {
			//pr_info(0)
		//}
	//}

	//pr_info() {
		//for(i=0; i <= maxSubLayersMinus1; i++) {
			uint8_t			picture_rate_code[255];
			uint16_t		average_picture_rate[255]; //only if picture_rate_code[i] ==
		//}
	//}


	//if(br_info_present) {
		//if(sub_layer_profile_tier_level_info_present) {
			//br_info(max_sub_layers_instream-1)
		//} else {
			//br_info(0)
		//}
	//}
	//br_info() {
		//for(i=0; i < maxSubLayersMinus1; i++) {
			uint16_t		average_bitrate[255];
			uint16_t		maximum_bitrate[255];
		//}
	//}

	//if(color_info_present) {
		struct {
				uint8_t		colour_primaries;
				uint8_t		transfer_characteristics;
				uint8_t		matrix_coeffs;

				//if(colour_primaries>=9) {
					uint8_t 	cg_compatibility:1;
					uint8_t		reserved_7_cp:7;
				//}

				//if(transfer_characteristics>=16) {
					uint8_t		eotf_info_present:1;
					//if(eotf_info_present) {
						uint16_t	eotf_info_len_minus1:15;
						struct {
							uint8_t		num_SEIs_minus1;
							uint16_t	SEI_NUT_length_minus1[255];
							uint8_t*	SEI_NUT_data[255];  //alloc to uint8_t, len: 8*(SEI_NUT_length_minus1[ i ]+1)
							///eotf_info()
						} eotf_info;
					//}
				//} else {
					uint8_t		reserved_7_tf;
				//}
			};
	//}

	//if(sub_layer_profile_tier_level_info_present) {
		//profile_tier_level(1, max_sub_layers_instream-1)
	//} else {
		//profile_tier_level(1, 0)
	//}
	//A/331:2021 Table 7.1 lists this as var length in "H.265" format?
	uint8_t*	profile_tier_level[255];  //up to max_sub_layers_instream-1 of var length

} mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset_t;

//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_VIDEO_STREAM_PROPERTIES_DESCRIPTOR
typedef struct mmt_atsc3_message_content_type_video_stream_properties_descriptor {
	mmt_atsc3_message_content_type_asset_heaader_t										asset_header;
	ATSC3_VECTOR_BUILDER_STRUCT(mmt_atsc3_message_content_type_video_stream_properties_descriptor_asset);

} mmt_atsc3_message_content_type_video_stream_properties_descriptor_t;

//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_ATSC_STAGGERCAST_DESCRIPTOR
typedef struct mmt_atsc3_message_content_type_atsc_staggercast_descriptor {
	void* TODO;
} mmt_atsc3_message_content_type_atsc_staggercast_descriptor_t;

//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_INBAND_EVENT_DESCRIPTOR_A337
typedef struct mmt_atsc3_message_content_type_inband_event_descriptor_a337 {
	void* TODO;
} mmt_atsc3_message_content_type_inband_event_descriptor_a337_t;

enum MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ROLE {
	MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ROLE_MAIN =				0x0,
	MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ROLE_ALTERNATE =			0x1,
	MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ROLE_COMMENTARY =			0x2,
	MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ROLE_RESERVED =			0x3
};

enum MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ASPECT_RATIO {
	MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ASPECT_RATIO_16X9 =		0x0,
	MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ASPECT_RATIO_4X3 =		0x1,
	MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ASPECT_RATIO_21X9 =		0x2,
	MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ASPECT_RATIO_RESERVED =	0x3
};

typedef struct mmt_atsc3_message_content_type_caption_asset_descriptor_asset {
	mmt_atsc3_message_content_type_asset_heaader_t										asset_header;
	mmt_atsc3_message_content_type_language_heaader_t									language_header;

	enum MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ROLE 			role;
	enum MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR_ASSET_ASPECT_RATIO		aspect_ratio;
	uint8_t																				easy_reader:1;
	uint8_t																				profile:2;
	uint8_t																				flag_3d_support:1;
	uint8_t																				reserved:4;

} mmt_atsc3_message_content_type_caption_asset_descriptor_asset_t;

//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_CAPTION_ASSET_DESCRIPTOR
typedef struct mmt_atsc3_message_content_type_caption_asset_descriptor {
	mmt_atsc3_message_content_type_descriptor_number_of_assets_header_t		descriptor_header;

	ATSC3_VECTOR_BUILDER_STRUCT(mmt_atsc3_message_content_type_caption_asset_descriptor_asset);

	uint8_t													reserved:8;

} mmt_atsc3_message_content_type_caption_asset_descriptor_t;


//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_AUDIO_STREAM_PROPERTIES_DESCRIPTOR
typedef struct mmt_atsc3_message_content_type_audio_stream_properties_descriptor {
	mmt_atsc3_message_content_type_descriptor_number_of_assets_header_t		descriptor_header;

	void* TODO;
} mmt_atsc3_message_content_type_audio_stream_properties_descriptor_t;

//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_DWD
typedef struct mmt_atsc3_message_content_type_dwd {
	mmt_atsc3_message_content_type_descriptor_number_of_assets_header_t		descriptor_header;

	void* TODO;
} mmt_atsc3_message_content_type_dwd_t;

//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_RSAT_A200
typedef struct mmt_atsc3_message_content_type_rsat_a200 {
	mmt_atsc3_message_content_type_descriptor_number_of_assets_header_t		descriptor_header;

	void* TODO;
} mmt_atsc3_message_content_type_rsat_a200_t;

//jjustman-2021-06-08 - todo: refactor out into MMT SI types?
/**
 *
 23008-1:2017 Section 10.5.5.2: SI descriptor syntax

 Syntax										Value		No. of Bits		Mnemonic
 ------										-----		-----------		--------
 SI_descriptor {
 	 descriptor_tag										16
 	 descriptor_length									16
 	 security_system_count					N1			8
 	 reserved								"000 0000"	7
 	 system_provider_url_flag				1

 	 if(system_provider_url_flag) {
 	 	 system_provider_url_length 		N2			8
 	 	 for(i=0; i < N2; i++) {
 	 	 	 system_provider_url_byte					8
 	 	 }
 	 }

 	 for(i=0; i < N1; i++) {
		 system_id										16*8
		 kid_count							N3			16
		 for(j=0; j < N3; j++) {
			 KID										16*8 		(note per KID)
		 }
		 data_size										32
		 for(j=0; j < N4; j++) {
			 data										8
		 }
	}
 }
 */
typedef struct mmt_si_security_properties_descriptor_kid {
	uint8_t			kid[16]; //8*16
} mmt_si_security_properties_descriptor_kid_t;

typedef struct mmt_si_security_properties_descriptor_system {

	uint8_t	 													system_id[16];
	uint16_t													kid_count;
		 //for(j=0; j < N3; j++) {

	ATSC3_VECTOR_BUILDER_STRUCT(mmt_si_security_properties_descriptor_kid);
		 //}

	uint32_t													data_size;

		 //for(j=0; j < N4; j++) {
			 uint8_t*											data;
		 //}
} mmt_si_security_properties_descriptor_system_t;
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmt_si_security_properties_descriptor_system, mmt_si_security_properties_descriptor_kid);

typedef struct mmt_si_security_properties_descriptor {
	mmt_signalling_information_message_descriptor_header_t		descriptor_header;
	uint8_t														security_system_count;		//N1
	uint8_t														reserved_7_0:7; 			//000 0000
	uint8_t														system_provider_url_flag:1;

	//if(system_provider_url_flag) {
	uint8_t														system_provider_url_length;
	 	 //for(i=0; i < N2; i++) {
	uint8_t*							 						system_provider_url;
	 	 //}
	 //}

	 //for(i=0; i < N1; i++) {
	ATSC3_VECTOR_BUILDER_STRUCT(mmt_si_security_properties_descriptor_system);

	//}
} mmt_si_security_properties_descriptor_t;
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmt_si_security_properties_descriptor, mmt_si_security_properties_descriptor_system);


//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR
typedef struct mmt_atsc3_message_content_type_security_properties_descriptor {
	mmt_atsc3_message_content_type_descriptor_number_of_assets_header_t				descriptor_header;

	void* TODO;
} mmt_atsc3_message_content_type_security_properties_descriptor_t;


/*
 * DRAFT: 2021-06-08 RP MMT DRM
 *
  Syntax																Bits		Format
  -----------------------------------------								-------     --------
  security_properties_descriptor_LAURL() {
	descriptor_tag
	descriptor_length
	number_of_assets
	for (i=0; i<number_of_assets; i++) {
		asset_id_length
		for (j=0; j<asset_id_length; j++) {
			asset_id_byte
		}
		scheme_code_present												1
		default_KID_present												1
		license_info_present											1
        reserved														5			00000
		if (schme_code_present) {
			scheme_code													4*8			uimsbf
		}
		if (default_KID_present) {
		    default_KID_length											8
	 	    for (j=0; j<default_KID_length; j++) {
			default_KID_byte											8
		    }
		}
		if (license_info_present) {
        	number_of_license_info										8
            for (i=0; i<number_of_license_info; i++) {
		        license_type											8
		        LA_URL_length											8
		        for (j=0; j<URL_length; j++) {
			    	LA_URL_byte											8
		       }
        	}
		}

		SI_descriptor()													var			Subclause 10.5.5 of 23008-1
	}
}
 *
 */

typedef struct mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info {
	uint8_t		license_type;
	uint8_t		LA_URL_length;
	uint8_t*	LA_URL;

} mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info_t;


typedef struct mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset {
	mmt_atsc3_message_content_type_asset_heaader_t						asset_header;

	uint8_t																scheme_code_present:1;
	uint8_t																default_KID_present:1;
	uint8_t																license_info_present:1;
	uint8_t																reserved_5_0:5;		 //5 bits: 00000

	//if(scheme_code_present) {
	uint8_t																scheme_code[4]; 	//8*4
	//}

	//if(default_KID_present) {
	uint8_t																default_KID_length;
	uint8_t*															default_KID;		//varchar
	//}

	//if(license_info_present) {
	uint8_t																number_of_license_info;
	ATSC3_VECTOR_BUILDER_STRUCT(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info);
	//}

	//si_descriptor_23008-1-10.5.5
	mmt_si_security_properties_descriptor_t								mmt_si_security_properties_descriptor;

} mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset_license_info);

//TODO: jjustman-2021-06-03: MMT_ATSC3_MESSAGE_CONTENT_TYPE_SECURITY_PROPERTIES_DESCRIPTOR_LAURL
typedef struct mmt_atsc3_message_content_type_security_properties_descriptor_LAURL {
	mmt_atsc3_message_content_type_descriptor_number_of_assets_header_t		descriptor_header;
	ATSC3_VECTOR_BUILDER_STRUCT(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset);

} mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_t;
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmt_atsc3_message_content_type_security_properties_descriptor_LAURL, mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_asset);



typedef struct mmt_atsc3_message_payload {
	uint16_t 	service_id;
	uint16_t 	atsc3_message_content_type;
	uint8_t		atsc3_message_content_version;
	uint8_t		atsc3_message_content_compression;

	uint8_t 	URI_length;
	uint8_t* 	URI_payload;

	//jjustman-2021-06-08 - todo: map this into block_t*
	uint32_t	atsc3_message_content_length;
	uint8_t*	atsc3_message_content;

	block_t*	atsc3_message_content_blockt;

	uint32_t	atsc3_message_content_length_compressed;
	uint8_t*	atsc3_message_content_compressed;

	//reserved:	8 bits to pad out length
	//i < length - 11 - URI_length - atsc3_message_content_length

	//<ROUTEComponent sTSIDUri="stsid.sls" sTSIDDestinationIpAddress="239.255.70.1" sTSIDDestinationUdpPort="5009" sTSIDSourceIpAddress="172.16.200.1"></ROUTEComponent>
	mmt_atsc3_route_component_t*    mmt_atsc3_route_component;

	mmt_atsc3_held_message_t*       mmt_atsc3_held_message;


	//other content_types as needed
	mmt_atsc3_message_content_type_security_properties_descriptor_LAURL_t* mmt_atsc3_message_content_type_security_properties_descriptor_LAURL;

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
	//jjustman-2020-12-02 - TODO - refactor this to vector
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
	uint8_t*		mp_table_descriptors_byte;
} mp_table_descriptors_t;

typedef struct mmt_package_id {
	uint8_t		mmt_package_id_length;
	uint8_t*	mmt_package_id;
} mmt_package_id_t;

/*
 
 From ISO 23008-1:2017 10.6.1.3 Semantics for Table 56 — MMT_general_location_info syntax
 
  Value Description
  ----- -----------
  0x01  MMTP packet flow over UDP/IP (version 4)
  0x02  MMTP packet flow over UDP/IP (version 6)
  0x03  An elementary stream within an MPEG-2 TS in a broadcast network.
  0x04  An elementary stream (ES) in an MPEG-2 TS over the IP broadcast network
  0x05  URL
  0x06  Reserved for private location information
  0x07  The same signalling message as the one that carries the data structure to which this MMT_ general_location_info() belongs
  0x08  A signalling message delivered in the same data path as the one that carries the data structure to which this MMT_general_location_info() belongs
  0x09  A signalling message delivered in a data path in the same UDP/IP flow as the one that carries the data structure to which this MMT_general_location_info() belongs
  0x0A  A signalling message delivered in a data path in a UDP/IP (version 4) flow
  0x0B  A signalling message delivered in a data path in a UDP/IP (version 6) flow
  0x0C  An elementary stream (ES) in an MPEG-2 TS over the IP v4 broadcast network
 
 */

#define MMT_GENERAL_LOCATION_INFO_LOCATION_TYPE_MMTP_PACKET_FLOW_SAME_AS_SI 0x00
#define MMT_GENERAL_LOCATION_INFO_LOCATION_TYPE_MMTP_PACKET_FLOW_UDP_IP_V4  0x01

typedef struct mmt_general_location_info {
	uint8_t location_type;
	
	//for _most_ location_type values (e.g. 0x00, 0x01, !0x03, !0x04, !0x05, ...etc)
	uint16_t packet_id;
	
	//for location_type == 0x01
	uint32_t ipv4_src_addr;
	uint32_t ipv4_dst_addr;
	uint16_t ipv4_dst_port;
	
	//ignoring ipv6
	uint16_t message_id;

} mmt_general_location_info_t;

typedef struct mmt_signaling_message_mpu_tuple {
    uint32_t mpu_sequence_number;
    uint64_t mpu_presentation_time; //jjustman-2021-01-19 - for _debug, use  "...%" PRIu64
} mmt_signalling_message_mpu_tuple_t;

//mpu_timestamp_descriptor_tag is 0x0001

typedef struct mmt_signaling_message_mpu_timestamp_descriptor {
    uint16_t                               descriptor_tag;
    uint8_t                                descriptor_length;
    uint8_t                                mpu_tuple_n; //mpu_tuple_n = descriptor_length/12 = (32+64)/8
    mmt_signalling_message_mpu_tuple_t*    mpu_tuple;
} mmt_signalling_message_mpu_timestamp_descriptor_t;


#define MP_TABLE_ASSET_ROW_ASSET_TYPE_LENGTH 4

/*
 * Video: HEVC
 *
 * A/331:2021 pp. 99:
 *
 * codec_code – This field shall specify a 4-character code for a codec.
 *
 *      The value of these four characters shall be one of 'hev1', 'hev2', 'hvc1', 'hvc2', 'lhv1' or 'lhe1' with semantic meaning for these codes as specified in ISO/IEC 14496-15 [35] as amended.
*/

#define ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_HEV1 "hev1"
#define ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_HEV2 "hev2"
#define ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_HVC1 "hvc1"
#define ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_HVC2 "hvc2"
#define ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_LVH1 "lvh1"
#define ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_LHE1 "lhe1"

#define ATSC3_MP_TABLE_IS_VIDEO_ASSET_TYPE_HEVC(asset_type) \
    (\
        strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_HEV1, asset_type, 4) == 0 || \
        strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_HEV2, asset_type, 4) == 0 || \
        strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_HVC1, asset_type, 4) == 0 || \
        strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_HVC2, asset_type, 4) == 0 || \
        strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_LVH1, asset_type, 4) == 0 || \
        strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_HEVC_ID_LHE1, asset_type, 4) == 0 \
    )

//Video: h264
#define ATSC3_MP_TABLE_ASSET_ROW_H264_ID "avc1"

#define ATSC3_MP_TABLE_IS_VIDEO_ASSET_TYPE_H264(asset_type) \
    (\
        strncasecmp(ATSC3_MP_TABLE_ASSET_ROW_H264_ID, asset_type, 4) == 0 \
    )

#define ATSC3_MP_TABLE_IS_VIDEO_ASSET_TYPE_ANY(asset_type) \
    ( ATSC3_MP_TABLE_IS_VIDEO_ASSET_TYPE_HEVC(asset_type) || ATSC3_MP_TABLE_IS_VIDEO_ASSET_TYPE_H264(asset_type) )

//Audio: AC-4
#define ATSC3_MP_TABLE_ASSET_ROW_AC_4_ID "ac-4"

//jjustman-2020-01-08 - adding in MPEG-H mime type support - Thanks Stefan!
/*
 * ATSC A/342-3:2017
 *
 * 5.2.2.1 MPEG-H Audio Sample Entry
    The sample entry “mhm1” shall be used for encapsulation of MHAS packets into ISOBMFF files,
    according to ISO/IEC 23008-3 Amendment 2, Clause 20.6 [3].
    The sample entry “mhm2” shall be used in cases of multi-stream or hybrid delivery, i.e., when
    the MPEG-H Audio Program is split into two or more streams for delivery as described in ISO/IEC
    23008-3, Clause 14.6 [2].

    If the MHAConfigurationBox() is present, the MPEG-H Profile-Level Indicator
    mpegh3daProfileLevelIndication in the MHADecoderConfigurationRecord() shall be set to
    “0x0B,” “0x0C,” or “0x0D” for MPEG-H Audio Low Complexity Profile Level 1, Level 2, or
    Level 3, respectively. The Profile-LLevel Indicator in the MHAS PACTYP_MPEGH3DACFG
    packet shall be set accordingly
 */

//Audio: MPEG-H
#define ATSC3_MP_TABLE_ASSET_ROW_MHM1_ID "mhm1"
#define ATSC3_MP_TABLE_ASSET_ROW_MHM2_ID "mhm2"

//Audio: mp4 (latm?)
#define ATSC3_MP_TABLE_ASSET_ROW_MP4A_ID "mp4a"


//Captions: captions support
#define ATSC3_MP_TABLE_ASSET_ROW_IMSC1_ID "stpp" //MPEG- 4 Part 30 (ISO/IEC 14496-30) defines a way to carry IMSC1-conformat TTM XML in MP4 tracks. Those tracks have a codec 4-character code of stpp.

typedef struct mp_table_asset_row {
	identifier_mapping_t            identifier_mapping;

	//identifer_mapping()
	char		                    asset_type[5];          //asset_type – provides the type of Asset. This is described in a four character code (“4CC”) type registered in MP4REG (http://www.mp4ra.org).
	                                                        // leave null pad
	//6 bits reserved
	uint8_t		                    default_asset_flag;

	uint8_t		                    asset_clock_relation_flag;
	uint8_t		                    asset_clock_relation_id;
	//7bits reserved
	uint8_t		                    asset_timescale_flag;
	uint32_t	                    asset_timescale;

	//asset_location (
	uint8_t		                    location_count;
	mmt_general_location_info_t     mmt_general_location_info;
    
	//asset_descriptors (
	uint16_t	                    asset_descriptors_length;
	uint8_t*	                    asset_descriptors_payload;
    
    mmt_signalling_message_mpu_timestamp_descriptor_t*      mmt_signalling_message_mpu_timestamp_descriptor;
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

/**
 *
0xF337 : SCTE35_Signal message

SCTE35_Signal_Message {
	message_id 								16
	version 								8
	length  		 						32

	number_of_SCTE35_Signal_Descriptor 	N1 	8
	SCTE35_Signal_Descriptor() 				var	(per length)
}

SCTE35 Signal Descriptor:

0xF33F: SCTE35_Signal_Descriptor tag

SCTE35_Signal_Descriptor {
	descriptor_tag 							16
	descriptor_length						16
	NTP_timestamp 							64
	reserved								7 	'1111111'
	PTS_timestamp							33
	signal_length 						N1	16
	  for(i=0; i < N1; i++) {
		signal_byte[i];						8
	 }
}
 *
 */

typedef struct mmt_scte35_signal_descriptor {
	uint16_t	descriptor_tag;
	uint16_t	descriptor_length;
	uint64_t	ntp_timestamp;
	uint64_t	pts_timestamp;
	uint16_t	signal_length;
	uint8_t*    signal_byte;

} mmt_scte35_signal_descriptor_t;

typedef struct mmt_scte35_message_payload {
	uint16_t	message_id;
	uint8_t		version;
	uint32_t	length;

	ATSC3_VECTOR_BUILDER_STRUCT(mmt_scte35_signal_descriptor);
} mmt_scte35_message_payload_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmt_scte35_message_payload, mmt_scte35_signal_descriptor);

/*..fix me..*/

typedef union mmt_signalling_message_payload {
	mmt_atsc3_message_payload_t			mmt_atsc3_message_payload;
	mp_table_t							mp_table;
	mmt_scte35_message_payload_t		mmt_scte35_message_payload;
} mmt_signalling_message_payload_u;

typedef struct mmt_signalling_message_header_and_payload {
	mmt_signalling_message_header_t 	message_header;
	mmt_signalling_message_payload_u 	message_payload;
} mmt_signalling_message_header_and_payload_t;


//jjustman-2020-09-01 - confirm this is being chained for cleanup
void mmt_signalling_message_header_and_payload_free(mmt_signalling_message_header_and_payload_t** mmt_signalling_message_header_and_payload_p);

mp_table_asset_row_t* atsc3_mmt_mp_table_asset_row_duplicate(const mp_table_asset_row_t* mp_table_asset_row);
void atsc3_mmt_mp_table_asset_row_free_inner(mp_table_asset_row_t* mp_table_asset_row);

mmt_atsc3_route_component_t* mmt_atsc3_message_payload_add_mmt_atsc3_route_component(mmt_atsc3_message_payload_t* mmt_atsc3_message_payload);
mmt_atsc3_held_message_t*    mmt_atsc3_message_payload_add_mmt_atsc3_held_message(mmt_atsc3_message_payload_t* mmt_atsc3_message_payload);

void mmt_atsc3_route_component_free(mmt_atsc3_route_component_t** mmt_atsc3_route_component_p);
void mmt_atsc3_held_message_free(mmt_atsc3_held_message_t** mmt_atsc3_held_message_p);



#if defined (__cplusplus)
}
#endif

#endif /* ATSC3_MMT_SIGNALLING_MESSAGE_TYPES_H_ */
