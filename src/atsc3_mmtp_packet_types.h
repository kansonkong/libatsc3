/*
 * atsc3_mmtp_packet_types.h
 *
 *  Created on: Jan 3, 2019
 *      Author: jjustman
 *
 *  Refactored on: Aug 10, 2019
 *
 */

#include <assert.h>
#include <limits.h>

#ifndef ATSC3_MMTP_PACKET_TYPES_H_
#define ATSC3_MMTP_PACKET_TYPES_H_

#if defined (__cplusplus)
extern "C" {
#endif

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"

#include "atsc3_mmtp_ntp32_to_pts.h"
#include "atsc3_mmt_signalling_message_types.h"
#include "atsc3_mmt_mpu_sample_format_type.h"
    
#include "atsc3_listener_udp.h"

#include "atsc3_video_decoder_configuration_record.h"
#include "atsc3_audio_decoder_configuration_record.h"
#include "atsc3_stpp_decoder_configuration_record.h"

extern int _MMTP_DEBUG_ENABLED;
extern int _MMTP_TRACE_ENABLED;

#define MIN_MMTP_SIZE 32
#define MAX_MMTP_SIZE MAX_ATSC3_PHY_ALP_DATA_PAYLOAD_SIZE

#define MPU_REASSEMBLE_MAX_BUFFER 8192000

/**
 * base mmtp_packet_header fields
 *
 * clang doesn't know how to inherit from structs, e.g. -fms-extensions, so use a define instead
 * see https://stackoverflow.com/questions/1114349/struct-inheritance-in-c
 *
 *
 * todo, ptr back to chain to mmtp_packet_id
 */



//base header fields for separate mmt data type packets
//TODO: jjustman-2019-08-10 - re-factor mmtp_sub_flow_t* to a more sane re-fragmentation hierarchical model
//	mmtp_sub_flow_t*	mmtp_sub_flow;					                    \

    
#define _MMTP_PACKET_HEADER_FIELDS 						                    \
                                                                            \
	block_t*			raw_packet;						                    \
                                                                            \
    uint8_t 		    mmtp_packet_version:2; 			                    \
    uint8_t 		    packet_counter_flag:1; /* C */                      \
    uint8_t 		    fec_type:2;                                         \
    uint8_t             __reserved_r:1;                                     \
                                                                            \
    uint8_t             mmtp_header_extension_flag:1;                       \
    uint8_t             mmtp_rap_flag:1;                                    \
    uint8_t             __reserved_res:2;                                   \
                                                                            \
    uint8_t             mmtp_header_compression:1;                          \
    uint8_t             mmtp_indicator_ref_header_flag:1;                   \
    uint8_t 		    mmtp_payload_type:6; /*for v=0 compat*/	            \
    uint16_t            mmtp_packet_id;                                     \
    uint32_t            packet_sequence_number;                             \
    uint32_t            mmtp_timestamp;                                     \
                                                                            \
    uint16_t            mmtp_timestamp_s; /*ntp-mapped into s*/             \
    uint16_t            mmtp_timestamp_us; /*ntp-frac.into ms*/             \
                                                                            \
    uint32_t            packet_counter; /* opt, reqires C abv*/             \
    uint32_t            source_fec_payload_id; /* opt, req fec_type=1 */    \
                                                                            \
    uint16_t            mmtp_header_extension_type; /* opt hdr_ex block*/   \
    uint16_t            mmtp_header_extension_length;                       \
    block_t*            mmtp_header_extension;                       		\
                                                                            \
    uint8_t 		    mmtp_qos_flag:1;            /* Q */		            \
    uint8_t 		    mmtp_flow_identifer_flag:1; /* F */		            \
    uint8_t 		    mmtp_flow_extension_flag:1;	/* E */     	        \
    uint8_t             mmtp_reliability_flag:1;    /* r */                 \
    uint8_t             mmtp_type_of_bitrate:2;     /* TB */                \
    uint8_t             mmtp_delay_sensitivity:3;   /* DS */                \
                                                                            \
    uint8_t             mmtp_transmission_priority:3; /* TP */              \
    uint8_t 		    flow_label:7;
    
typedef struct mmtp_packet_header {
	_MMTP_PACKET_HEADER_FIELDS
} mmtp_packet_header_t;

//todo, streamline this
mmtp_packet_header_t* mmtp_packet_header_new();
void mmtp_packet_header_free(mmtp_packet_header_t** mmtp_packet_header_p);

//define for mpu type common header fields for struct inheritance


/**

 MMTP packet vs. payload philosophy:

    raw structs are fragment packets
    re-fragmented logical units are 'payload'

    For payload_type = 0x0 - MPU, the structure is as follows:

 An Asset (asset_id) {
    which contains a collection of:

    mmtp_packet(s) (packet_id) {
        which contains a collection of:

        mpu_sequence(s) (mpu_sequence_number) {
            which contains a collection of:

            mfu_sample(s) (samplenumber) {
                which contains a collection of:

                mfu_fragment(s) (fragment_counter) {
                    which contains a collection of:

                    block_t bytes;


 Defer worrying about FT=0, FT=2....FT=1, as we want to process these emissions in flow,
 rather than re-constituion of the full MPU

 **/

#define _MMTP_MPU_PACKET_HEADER_FIELDS                  \
                                                        \
	_MMTP_PACKET_HEADER_FIELDS				            \
                                                        \
	uint16_t    mpu_packet_length;			            \
    uint8_t     mpu_fragment_type:4;	                \
                                                        \
    /* note, iso23008-1 is NOT NORMATIVE when    */     \
    /* defining FT ordering, it SHOULD be:       */     \
    /*                                           */     \
    /* FT=0     MPU Metadata                     */     \
    /* FT=2     MFU Emission (sample/frame)      */     \
    /* FT=2     MFU Emission (sample/frame)      */     \
    /* ...      ....                             */     \
    /* FT=1     Movie Fragment Metadata          */     \
    /* ----     -----------------------          */     \
    /* NOTE     FT=1 fragment type can be built  */     \
    /*          from the MFU mmthsample hint     */     \
    /*          header payload that prefixes     */     \
    /*          every MFU sample payload (fi=0)  */     \
    /*          and FT=1 IS NOT NEEDED FOR       */     \
    /*          MMT OOO MODE!                    */     \
                                                        \
    uint8_t     mpu_timed_flag:1;	           /* T  */ \
    uint8_t     mpu_fragmentation_indicator:2; /* fi */	\
                                                        \
    uint8_t     mpu_aggregation_flag:1;        /* A */  \
	uint8_t     mpu_fragment_counter;		            \
	uint32_t    mpu_sequence_number;			        \
	uint16_t    data_unit_length;                       \
														\
    block_t*    du_mpu_metadata_block;	 /* 0x0 */		\
    block_t*	du_movie_fragment_block; /* (OOO)0x1 */ \
    block_t*	du_mfu_block;			 /* 0x2 */

/* See 9.3.2.2 MMTP payload header for MPU mode
 *
 * Sane min header length must be at least:
 *
        32 + 32 + 32 + 8 + 8 bits = 14 bytes
 *
*/

#define ATSC3_MMTP_MPU_PACKET_PARSE_FROM_BLOCK_T_MINIMUM_SANE_LENGTH_BYTES 14

typedef struct mmtp_mpu_packet {
    _MMTP_MPU_PACKET_HEADER_FIELDS;

    //if mpu_timed_flag == 1
        uint32_t    movie_fragment_sequence_number;
        uint32_t    sample_number;
        uint32_t    offset;
        uint8_t     priority;
        uint8_t     dep_counter;

        //todo: other attributes for re-fragmentation should be under the
        //  mpu_sample,
        //      and then mpu_fragments based upon mpu_fragmentation_counter
        mmthsample_header_t* mmthsample_header;

    // else (non-timed MFU) - see 9.3.2.2 Fig 13
        uint32_t    item_id;

    //jjustman-2019-10-23: hack-ish for MFU re-assembly
    bool mfu_reassembly_performed;

    //jjustman-2021-01-19: limit warn logging for missing re-assembly
    bool mmtp_mpu_init_packet_missing_du_movie_fragment_block_warning_logged;

} mmtp_mpu_packet_t;
    
typedef struct {
    uint32_t mpu_sequence_number;
    uint16_t packet_id;

    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_mpu_packet);
} mpu_sequence_number_mmtp_mpu_packet_collection_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mpu_sequence_number_mmtp_mpu_packet_collection, mmtp_mpu_packet)

//forward declare, as this struct definition is in atsc3_mmt_signalling_packet_types.h
typedef struct mmt_signalling_message_header_and_payload mmt_signalling_message_header_and_payload_t;
typedef struct mmtp_signalling_packet {
	_MMTP_PACKET_HEADER_FIELDS

    uint8_t		si_fragmentation_indicator:2;  /* f_i */
    uint8_t     si_res:4;                       /* res */
    uint8_t		si_additional_length_header:1;  /* H */
    uint8_t		si_aggregation_flag:1; 		    /* A */
    uint8_t		si_fragmentation_counter:8;     /* frag_counter */
    /*
     * jjustman-2020-11-12 - do not add in MSG_length, as it is only used for si aggregation messages, handled in: mmt_signalling_message_parse_packet
     *      si_msg_length uint32_t	si_msg_length;
     *
     *  if A==0, field is omitted, otherwise,
        for each si_message aggregated,
            if(si_additional_length_header == 0)
               si_msg_length is 16 bits long  - e.g.    mmtp_si_msg_length = block_Read_uint16_ntohs(udp_packet);
        else
            si_msg_length is 32 bits long  - e.g.     mmtp_si_msg_length = block_Read_uint32_ntohl(udp_packet);

        after each MSG_payload
        */

    //used for de-fragmentation if si_fragmentation_indicator != 0x00
    block_t*   udp_packet_inner_msg_payload;

    ATSC3_VECTOR_BUILDER_STRUCT(mmt_signalling_message_header_and_payload);
} mmtp_signalling_packet_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_signalling_packet, mmt_signalling_message_header_and_payload)

//atsc3 does not use nontimed mpu's, so...just for posterity's sake
typedef struct mmtp_mpu_nontimed_packet {
    _MMTP_MPU_PACKET_HEADER_FIELDS
    uint32_t        non_timed_mfu_item_id;
} mmtp_mpu_nontimed_packet_t;

//atsc3 does not use generic_object, so...just for posterity's sake
typedef struct mmtp_generic_object_packet {
    _MMTP_PACKET_HEADER_FIELDS
} mmtp_generic_object_packet_t;

//atsc3 does not use generic_object, so...just for posterity's sake
typedef struct mmtp_repair_symbol_packet {
	_MMTP_PACKET_HEADER_FIELDS
} mmtp_repair_symbol_packet_t;

//forward declare as defn is in atsc3_mmt_signalling_message_types.h
typedef struct mp_table_asset_row mp_table_asset_row_t;

typedef struct mmtp_packet_id_packets_container {
    uint16_t                packet_id;

    //populated from mmt SI mp_table event callbacks,
    //  invoked from: atsc3_mmt_signalling_information_on_*_essence_packet_id_internal callback,
    //      invoker: invoked from mmt_signalling_message_dispatch_context_notification_callbacks

    char                    asset_type[4];
    mp_table_asset_row_t*   mp_table_asset_row;

    atsc3_video_decoder_configuration_record_t*     atsc3_video_decoder_configuration_record;
    atsc3_audio_decoder_configuration_record_t*     atsc3_audio_decoder_configuration_record;
    atsc3_stpp_decoder_configuration_record_t*      atsc3_stpp_decoder_configuration_record;

    ATSC3_VECTOR_BUILDER_STRUCT(mpu_sequence_number_mmtp_mpu_packet_collection);
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_signalling_packet);
    
    //others not used in atsc3.0
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_mpu_nontimed_packet);
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_generic_object_packet);
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_repair_symbol_packet);
} mmtp_packet_id_packets_container_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet_collection)
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mmtp_signalling_packet)
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mmtp_mpu_nontimed_packet)
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mmtp_generic_object_packet)
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mmtp_repair_symbol_packet)


//forward declare for child/parent relationship
typedef struct mmtp_asset_flow mmtp_asset_flow_t;
    
typedef struct mmtp_asset {
    /* TODO: jjustman-2019-08-31
     
     map this to proper iso23008-1:

     uint32_t   asset_id_scheme
     uint32_t   asset_id_length
     uint8_t*   asset_id_value
     
     in the interim, use ATSC A/331 service_id as our "interim" key
     */
    uint16_t			atsc3_service_id;
    mmtp_asset_flow_t*  parent_mmtp_asset_flow;
    
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_packet_id_packets_container);
    
} mmtp_asset_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_asset, mmtp_packet_id_packets_container);

typedef struct mmtp_asset_flow {
	uint32_t dst_ip_addr;
	uint16_t dst_port;
	
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_asset);

} mmtp_asset_flow_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_asset_flow, mmtp_asset);

typedef struct mmtp_flow {
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_asset_flow);
} mmtp_flow_t;
    
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_flow, mmtp_asset_flow);
    
    
#ifdef __cplusplus
}
#endif

#define __MMTP_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMTP_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMTP_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __MMTP_DEBUG(...)   if(_MMTP_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __MMTP_TRACE(...)   if(_MMTP_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#endif /* ATSC3_MMTP_PACKET_TYPES_H_ */
