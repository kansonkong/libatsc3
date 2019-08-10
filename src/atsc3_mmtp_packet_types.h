/*
 * atsc3_mmtp_packet_types.h
 *
 *  Created on: Jan 3, 2019
 *      Author: jjustman
 *
 *  Refactored on: Aug 10, 2019
 *
 */

#ifndef ATSC3_MMTP_PACKET_TYPES_H_
#define ATSC3_MMTP_PACKET_TYPES_H_

#if defined (__cplusplus)
extern "C" {
#endif


#include <assert.h>
#include <limits.h>

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"

#include "atsc3_mmtp_ntp32_to_pts.h"
#include "atsc3_mmt_signalling_message_types.h"
#include "atsc3_mmt_mpu_sample_format_type.h"

extern int _MMTP_DEBUG_ENABLED;
extern int _MMTP_TRACE_ENABLED;

#define MIN_MMTP_SIZE 32
#define MAX_MMTP_SIZE 1514

//packet type=v0/v1 have an upper bound of ~1432
#define UPPER_BOUND_MPU_FRAGMENT_SIZE 1432

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
	_MMTP_PACKET_HEADER_FIELDS;
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
	_MMTP_PACKET_HEADER_FIELDS;				            \
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


typedef struct mmtp_mpu_packet {
    _MMTP_MPU_PACKET_HEADER_FIELDS;
    uint32_t    movie_fragment_sequence_number;
    uint32_t    sample_number;
    uint32_t    offset;
    uint8_t     priority;
    uint8_t     dep_counter;
    
    //todo: other attributes for re-fragmentation should be under the
    //  mpu_sample,
    //      and then mpu_fragments based upon mpu_fragmentation_counter
    mmthsample_header_t* mmthsample_header;

} mmtp_mpu_packet_t;
    
typedef struct {
    uint32_t mpu_sequence_number;
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_mpu_packet);
} mpu_sequence_number_mmtp_mpu_packet_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mpu_sequence_number_mmtp_mpu_packet, mmtp_mpu_packet);

//forward declare, as this struct definition is in atsc3_mmt_signalling_packet_types.h
typedef struct mmt_signalling_message_header_and_payload mmt_signalling_message_header_and_payload_t;
typedef struct mmtp_signalling_packet {
	_MMTP_PACKET_HEADER_FIELDS;

    uint8_t		si_fragmentation_indiciator:2;  /* f_i */
    uint8_t     si_res:4;                       /* res */
    uint8_t		si_additional_length_header:1;  /* H */
    uint8_t		si_aggregation_flag:1; 		    /* A */
    uint8_t		si_fragmentation_counter:8;     /* frag_counter */
    uint16_t	si_aggregation_message_length;  /* if A==0, field is omitted, otherwise,
                                                    16 + 16*H bits is repeated after every MSG_payload */
   //mmt_signalling_message_vector_t mmt_signalling_message_vector;
    ATSC3_VECTOR_BUILDER_STRUCT(mmt_signalling_message_header_and_payload);
} mmtp_signalling_packet_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_signalling_packet, mmt_signalling_message_header_and_payload);

//atsc3 does not use nontimed mpu's, so...just for posterity's sake
typedef struct mmtp_mpu_nontimed_packet {
    _MMTP_MPU_PACKET_HEADER_FIELDS;
    uint32_t        non_timed_mfu_item_id;
} mmtp_mpu_nontimed_packet_t;

//atsc3 does not use generic_object, so...just for posterity's sake
typedef struct mmtp_generic_object_packet {
    _MMTP_PACKET_HEADER_FIELDS;
} mmtp_generic_object_packet_t;

//atsc3 does not use generic_object, so...just for posterity's sake
typedef struct mmtp_repair_symbol_packet {
	_MMTP_PACKET_HEADER_FIELDS;
} mmtp_repair_symbol_packet_t;

typedef struct mmtp_packet_id_packets_container {
    uint16_t            packet_id;
    
    ATSC3_VECTOR_BUILDER_STRUCT(mpu_sequence_number_mmtp_mpu_packet);
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_signalling_packet);
    
    //others not used in atsc3.0
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_mpu_nontimed_packet);
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_generic_object_packet);
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_repair_symbol_packet);
} mmtp_packet_id_packets_container_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mpu_sequence_number_mmtp_mpu_packet);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mmtp_signalling_packet);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mmtp_mpu_nontimed_packet);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mmtp_generic_object_packet);
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_packet_id_packets_container, mmtp_repair_symbol_packet);


//forward declare for child/parent relationship
typedef struct mmtp_asset_flow mmtp_asset_flow_t;
    
typedef struct mmtp_asset {
    uint16_t            mmtp_packet_id;
    mmtp_asset_flow_t*   parent_mmtp_asset_flow;
    
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_packet_id_packets_container);
    
} mmtp_asset_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_asset, mmtp_packet_id_packets_container);

typedef struct mmtp_asset_flow {
	uint32_t dst_ip;
	uint16_t dst_port;
	
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_asset);

} mmtp_asset_flow_t;

ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(mmtp_asset_flow, mmtp_asset);


#ifdef __cplusplus
}
#endif

#define __MMTP_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMTP_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMTP_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __MMTP_DEBUG(...)   if(_MMTP_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __MMTP_TRACE(...)   if(_MMTP_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#endif /* ATSC3_MMTP_PACKET_TYPES_H_ */
