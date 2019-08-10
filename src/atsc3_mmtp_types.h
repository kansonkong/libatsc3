/*
 * mmtp_types.h
 *
 *  Created on: Jan 3, 2019
 *      Author: jjustman
 */
#include <assert.h>
#include <limits.h>

#ifndef MODULES_DEMUX_MMT_MMTP_TYPES_H_
#define MODULES_DEMUX_MMT_MMTP_TYPES_H_

//#include "atsc3_vector.h"

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
 * typedef struct mmtp_packet_header {
 *
 * todo, ptr back to chain to mmtp_packet_id
 */


typedef struct mmtp_sub_flow mmtp_sub_flow_t;

//base header fields for separate mmt data type payloads
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
	uint16_t    mpu_payload_length;			            \
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
    block_t*    mpu_data_unit_payload;


/* only use temporarly */
typedef struct mmtp_mpu_packet_header {
	_MMTP_MPU_TYPE_PACKET_HEADER_FIELDS;
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
        uint8_t     temporal_id:3;
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
    
    
//
//typedef struct {
//
//    uint32_t     mmth_sequence_number;
//    uint8_t     mmth_trackrefindex;
//    uint32_t     mmth_movie_fragment_sequence_number;
//    uint32_t     mmth_samplenumber;
//    uint8_t      mmth_priority;
//    uint8_t      mmth_dependency_counter;
//    uint32_t     mmth_offset;
//    uint32_t     mmth_length;
//    mmthsample_muli_box_t mmth_muli;
//
//    uint64_t pts;
//    uint64_t last_pts;
//} __mpu_data_unit_payload_fragments_timed_t;

/**
 
 MMTP payload vs. packet philosophy:
 
    raw structs are fragment payloads
    re-fragmented logical units are 'packets'
 
    For payload_type = 0x0 - MPU, the structure is as follows:
 
 An Asset (asset_id) {
    which contains a collection of:
 
    mmtp_packet (packet_id) {
        which contains a collection of:
 
        mpu_sequence (mpu_sequence_number) {
            which contains a collection of:
 
            mfu_sample (samplenumber) {
                which contains a collection of:
 
                mfu_fragment (fragment_counter) {
                    which contains a collection of:
 
                    block_t bytes;
 
 **/
    
typedef struct mmtp_mpu_payload {
    _MMTP_MPU_TYPE_PACKET_HEADER_FIELDS;
    uint32_t    movie_fragment_sequence_number;
    uint32_t    sample_number;
    uint32_t    offset;
    uint8_t     priority;
    uint8_t     dep_counter;
    
    //todo: other attributes for re-fragmentation should be under the
    //  mpu_sample,
    //      and then mpu_fragments based upon mpu_fragmentation_counter
} mmtp_mpu_payload_t;

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
typedef struct mmtp_mpu_packet_nontimed {
    _MMTP_MPU_TYPE_PACKET_HEADER_FIELDS;
    uint32_t        non_timed_mfu_item_id;
} mmtp_mpu_packet_nontimed_t;

//atsc3 does not use generic_object, so...just for posterity's sake
typedef struct mmtp_generic_object_packet {
    _MMTP_PACKET_HEADER_FIELDS;
} mmtp_generic_object_packet_t;

//atsc3 does not use generic_object, so...just for posterity's sake
typedef struct mmtp_repair_symbol {
	_MMTP_PACKET_HEADER_FIELDS;
} mmtp_repair_symbol_packet_t;

typedef union mmtp_packet_id_payload_container {
    uint16_t    packet_id;
    
    
    //mmtp_packet_header_t		        mmtp_packet_header;
	//mmtp_mpu_packet_header_t			mmtp_mpu_type_packet_header;

	__mpu_data_unit_payload_fragments_timed_t 		mpu_data_unit_payload_fragments_timed;
	__mpu_data_unit_payload_fragments_nontimed_t	mpu_data_unit_payload_fragments_nontimed;

	//add in the other mmtp types here
	__generic_object_fragments_t 					mmtp_generic_object_fragments;
	__signalling_message_fragments_t				mmtp_signalling_message_fragments;
	__repair_symbol_t								mmtp_repair_symbol;
} mmtp_payload_fragments_union_t;

typedef struct ATSC3_VECTOR(mmtp_payload_fragments_union_t *) 	mpu_type_packet_header_fields_vector_t;
typedef struct ATSC3_VECTOR(mmtp_payload_fragments_union_t *) 	mpu_data_unit_payload_fragments_timed_vector_t;
typedef struct ATSC3_VECTOR(mmtp_payload_fragments_union_t *)	mpu_data_unit_payload_fragments_nontimed_vector_t;
typedef struct ATSC3_VECTOR(mmtp_payload_fragments_union_t *) 	mmtp_generic_object_fragments_vector_t;
typedef struct ATSC3_VECTOR(mmtp_payload_fragments_union_t *) 	mmtp_signalling_message_fragments_vector_t;
typedef struct ATSC3_VECTOR(mmtp_payload_fragments_union_t *) 	mmtp_repair_symbol_vector_t;

//todo, make this union
typedef struct {
	uint32_t mpu_sequence_number;
	mpu_data_unit_payload_fragments_timed_vector_t 		timed_fragments_vector;
	mpu_data_unit_payload_fragments_nontimed_vector_t 	nontimed_fragments_vector;

} mpu_data_unit_payload_fragments_t;

typedef struct ATSC3_VECTOR(mpu_data_unit_payload_fragments_t *) mpu_data_unit_payload_fragments_vector_t;


//partial refactoring from vlc to libatsc3
#ifndef LIBATSC3_MPU_ISOBMFF_FRAGMENT_PARAMETERS_T_
#define LIBATSC3_MPU_ISOBMFF_FRAGMENT_PARAMETERS_T_

typedef struct {
	void*			mpu_demux_track;
	block_t*		p_mpu_block;
	uint32_t     	i_timescale;          /* movie time scale */
	uint64_t     	i_moov_duration;
	uint64_t     	i_cumulated_duration; /* Same as above, but not from probing, (movie time scale) */
	uint64_t     	i_duration;           /* Declared fragmented duration (movie time scale) */
	unsigned int 	i_tracks;       /* number of tracks */
	void*		  	*track;         /* array of track */
	bool        	b_fragmented;   /* fMP4 */
	bool         	b_seekable;

	/**
	 * declared in vlc_libatsc3_types.h for impl

	block_t* 		tmp_mpu_fragment_block_t;
	block_t* 		mpu_fragment_block_t;  //capture our MPU Metadat box

	MP4_Box_t*		mpu_fragments_p_root_box;
	MP4_Box_t*		mpu_fragments_p_moov;

	//reconstitue per movie fragment as needed
	block_t* 		mp4_movie_fragment_block_t;
	MP4_Box_t*		mpu_fragments_p_moof;


	struct
	{
		 uint32_t        i_current_box_type;
		 MP4_Box_t      *p_fragment_atom;
		 uint64_t        i_post_mdat_offset;
		 uint32_t        i_lastseqnumber;
	} context;
	*/
} mpu_isobmff_fragment_parameters_t;
#endif

typedef struct {
	mmtp_sub_flow_t *mmtp_sub_flow;
	uint16_t mmtp_packet_id;

	mpu_type_packet_header_fields_vector_t 		all_mpu_fragments_vector;

	//MPU Fragment type collections for reconstruction/recovery of fragments

	//MPU metadata, 							mpu_fragment_type==0x00
	mpu_data_unit_payload_fragments_vector_t 	mpu_metadata_fragments_vector;

	//Movie fragment metadata, 					mpu_fragment_type==0x01
	mpu_data_unit_payload_fragments_vector_t	movie_fragment_metadata_vector;

	//MPU (media fragment_unit),				mpu_fragment_type==0x02
	mpu_data_unit_payload_fragments_vector_t	media_fragment_unit_vector;

	mpu_isobmff_fragment_parameters_t			mpu_isobmff_fragment_parameters;

} mpu_fragments_t;

/**
 * todo:  impl's
 */


typedef struct mmtp_sub_flow {
	uint32_t dst_ip;
	uint16_t dst_port;
	uint16_t mmtp_packet_id;

	//mmtp payload type collections for reconstruction/recovery of payload types

	//mpu (media_processing_unit):				paylod_type==0x00
	//mpu_fragments_vector_t 					mpu_fragments_vector;
	mpu_fragments_t								*mpu_fragments;

	//generic object:							payload_type==0x01
    mmtp_generic_object_fragments_vector_t 		mmtp_generic_object_fragments_vector;

	//signalling message: 						payload_type=0x02
	mmtp_signalling_message_fragments_vector_t 	mmtp_signalling_message_fragements_vector;

	//repair symbol:							payload_type==0x03
	mmtp_repair_symbol_vector_t 				mmtp_repair_symbol_vector;

} mmtp_sub_flow_t;


//todo - refactor mpu_fragments to vector, create a new tuple class for mmtp_sub_flow_sequence


typedef struct ATSC3_VECTOR(mmtp_sub_flow_t*) mmtp_sub_flow_vector_t;


#define _MMTP_PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define _MMTP_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_MMTP_PRINTLN(__VA_ARGS__);
#define _MMTP_WARN(...)    printf("%s:%d:WARN :",__FILE__,__LINE__);_MMTP_PRINTLN(__VA_ARGS__);
#define _MMTP_INFO(...)    printf("%s:%d:INFO :",__FILE__,__LINE__);_MMTP_PRINTLN(__VA_ARGS__);

#define _MMTP_DEBUG(...)   if(_MMTP_DEBUG_ENABLED) { printf("%s:%d:DEBUG :",__FILE__,__LINE__);_MMTP_PRINTLN(__VA_ARGS__); }
#define _MMTP_TRACE(...)   if(_MMTP_TRACE_ENABLED) { printf("%s:%d:TRACE :",__FILE__,__LINE__);_MMTP_PRINTLN(__VA_ARGS__); }
#define __LOG_MPU_REASSEMBLY(...) printf(__VA_ARGS__)

//TODO - jdj-2019-05-29 REMOVE ME!
#define __LOG_DEBUG(...) printf(__VA_ARGS__)
//(msg_Info(__VA_ARGS__))
#define __LOG_TRACE(...)
#define __PRINTF_DEBUG(...)
//printf(__VA_ARGS__)
//(
#define __PRINTF_TRACE(...)
//printf(__VA_ARGS__)
#ifdef __cplusplus
}
#endif

#endif /* MODULES_DEMUX_MMT_MMTP_TYPES_H_ */
