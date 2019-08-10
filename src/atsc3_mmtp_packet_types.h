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

#include <assert.h>
#include <limits.h>

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"

#include "atsc3_mmtp_ntp32_to_pts.h"
#include "atsc3_mmt_signalling_message_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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


typedef struct mmtp_sub_flow mmtp_sub_flow_t;

//base header fields for separate mmt data type packets
//TODO: jjustman-2019-08-10 - re-factor mmtp_sub_flow_t* to a more sane re-fragmentation hierarchical model
    
#define _MMTP_PACKET_HEADER_FIELDS 						                    \
                                                                            \
	block_t*			raw_packet;						                    \
	mmtp_sub_flow_t*	mmtp_sub_flow;					                    \
                                                                            \
    uint8_t 		    mmtp_packet_version:2; 			                    \
    uint8_t 		    packet_counter_flag:1; /* C */                      \
    uint8_t 		    fec_type:2;                                         \
    uint8_t             __reserved_r:1;                                      \
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
    uint8_t*            mmtp_header_extension_value;                        \
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

//define for mpu type common header fields for struct inheritance

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
    block_t*    mpu_data_unit_packet;


/* only use temporarly */
typedef struct mmtp_mpu_packet_header {
	_MMTP_MPU_PACKET_HEADER_FIELDS;
} mmtp_mpu_packet_header_t;

/**
TODO: move this into atsc3_mmtp_mpu_sample_types.h
 does not belong as __mpu_data_unit_payload_fragments_timed_t
 **/
    
    /* ISO23008-1:2017, Section 8.3 - Sample Format */
    
typedef struct atsc3_mmt_multiLayerInfoBox {
    uint8_t multilayer_flag:1;
    uint8_t reserved0:7;
/* if (multilayer_flag == 1) { */
        uint8_t     dependency_id:3;
        uint8_t     depth_flag:1;
        uint8_t     reserved1:4;
        uint8_t     temporal_id:3;
        uint8_t     reserved2:1;
        uint8_t     quality_id:4;
        uint8_t     priority_id:6;
        uint16_t    view_id:10;
/* } else { */
        uint8_t     layer_id:6;
        //            duplicated above
        //uint8_t     temporal_id:3;
        uint8_t     reserved3:7;
/* } */
} atsc3_mmt_multiLayerInfoBox_t;
    
typedef struct mmthsample_header {
    uint32_t    sequence_number;
/* if timed { */
        int8_t                          trackrefindex;
        uint32_t                        movie_fragment_sequence_number;
        uint32_t                        samplenumber;
        uint8_t                         priority;
        uint8_t                         dependency_counter;
        uint32_t                        offset;
        uint32_t                        length;
        atsc3_mmt_multiLayerInfoBox_t   atsc3_mmt_multiLayerInfoBox;
/* } else { */
        uint16_t                        item_id;
/* } */
} mmthsample_header_t;
    
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
} mmtp_mpu_packet_t;
    
typedef struct {
    uint32_t mpu_sequence_number;
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_mpu_packet);
} mpu_sequence_number_mmtp_mpu_packet_t;

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


//forward declare for child/parent relationship
typedef struct mmtp_asset_flow mmtp_asset_flow_t;
    
typedef struct mmtp_asset {
    uint16_t            mmtp_packet_id;
    mmtp_asset_flow_t*   parent_mmtp_asset_flow;
    
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_packet_id_packets_container);
    
} mmtp_asset_t;

typedef struct mmtp_asset_flow {
	uint32_t dst_ip;
	uint16_t dst_port;
	
    ATSC3_VECTOR_BUILDER_STRUCT(mmtp_asset);

} mmtp_asset_flow_t;

#ifdef __cplusplus
}
#endif


#define __MMTP_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMTP_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMTP_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __MMTP_DEBUG(...)   if(_MMTP_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __MMTP_TRACE(...)   if(_MMTP_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }



#endif /* ATSC3_MMTP_PACKET_TYPES_H_ */
