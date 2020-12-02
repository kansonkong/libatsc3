//
// Created by Jason Justman on 12/1/20.
//

#ifndef ATSC3_ISOBMFF_BOX_PARSER_TOOLS_H
#define ATSC3_ISOBMFF_BOX_PARSER_TOOLS_H

#include <stddef.h>

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * mdhd: extends fullbox
 */
typedef struct atsc3_isobmff_mdhd_box {
    uint32_t                        box_size;               //preceeding isobmff box size
    uint32_t                        type;                   //ac-4 fourcc code
    uint8_t                         version;
    uint32_t                        flags:24;

    //version==1
    struct atsc3_isobmff_mdhd_box_v1_t {
        uint64_t        creation_time;
        uint64_t        modification_time;
        uint32_t        timescale;
        uint64_t        duration;
    } atsc3_isobmff_mdhd_box_v1;

    //version==0
    struct atsc3_isobmff_mdhd_box_v0_t {
        uint32_t        creation_time;
        uint32_t        modification_time;
        uint32_t        timescale;
        uint32_t        duration;
    } atsc3_isobmff_mdhd_box_v0;

    uint8_t         pad:1;
    uint8_t         language[3];
    uint16_t        pre_defined;

} atsc3_isobmff_mdhd_box_t;


/*
 * tfhd: extends fullbox
 *
 *    tf_flags:
 *          0x000001 base‐data‐offset‐present: indicates the presence of the base‐data‐offset field.
 *          0x000002 sample‐description‐index‐present: indicates the presence of this field, which over‐rides, in this fragment, the default set up in the Track Extends Box.
 *          0x000008 default‐sample‐duration‐present
 *          0x000010 default‐sample‐size‐present
 *          0x000020 default‐sample‐flags‐present
 *          0x010000 duration‐is‐empty
 *          0x020000 default‐base‐is‐moof
 */
typedef struct atsc3_isobmff_tfhd_box {
    uint32_t        box_size;               //preceeding isobmff box size
    uint32_t        type;                   //ac-4 fourcc code
    uint8_t         version;
    uint32_t        flags:24;

    bool            flag_default_sample_duration;
    bool            flag_duration_is_empty; //0x010000: this indicates that the duration provided in either default‐sample‐ duration, or by the default‐duration in the Track Extends Box, is empty, i.e. that there are no samples for this time interval.

    uint32_t        track_id;

    //the following are optional fields: tf_flags
    uint64_t        base_data_offset;
    uint32_t        sample_description_index;
    uint32_t        default_sample_duration;
    uint32_t        default_sample_size;
    uint32_t        default_sample_flags;
} atsc3_isobmff_tfhd_box_t;


/*
 * trun: extends fullbox
 *
 *    tf_flags:
 *          0x000001 data‐offset‐present.
            0x000004 first‐sample‐flags‐present; this over‐rides the default flags for the first sample only. This
                        makes it possible to record a group of frames where the first is a key and the rest are
                        difference frames, without supplying explicit flags for every sample.
                        If this flag and field are used, sample‐ flags shall not be present.
            0x000100 sample‐duration‐present: indicates that each sample has its own duration, otherwise the default is used.
            0x000200 sample‐size‐present: each sample has its own size, otherwise the default is used.
            0x000400 sample‐flags‐present; each sample has its own flags, otherwise the default is used.
            0x000800 sample‐composition‐time‐offsets‐present; each sample has a composition time offset
                        (e.g. as used for I/P/B video in MPEG).
 */
typedef struct atsc3_isobmff_trun_box {
    uint32_t        box_size;               //preceeding isobmff box size
    uint32_t        type;                   //ac-4 fourcc code
    uint8_t         version;
    uint32_t        flags:24;

    bool            flag_data_offset_present;                       //0x000001
    bool            flag_first_sample_flags_present;                //0x000004

    bool            flag_sample_duration_present;                   //0x000100
    bool            flag_sample_size_present;                       //0x000200
    bool            flag_sample_flags_present;                      //0x000400
    bool            flag_sample_composition_time_offset_present;    //0x000800

    uint32_t        sample_count;

    //the following are optional fields: tf_flags
    int32_t         data_offset;
    uint32_t        first_sample_flags;

    //jjustman-2020-12-01 - TODO: vector for multiple entries

    //all fields in the following array are optional
    uint32_t        sample_duration;
    uint32_t        sample_size;
    uint32_t        sample_flags;

    //if version==0, unsigned
        struct atsc3_isobmff_trun_box_v0_t {
            uint32_t         sample_composition_time_offset;
        } atsc3_isobmff_trun_box_v0;
    //else
        struct atsc3_isobmff_trun_box_v_not0_t {
            signed         sample_composition_time_offset;
        } atsc3_isobmff_trun_box_v_not0;

} atsc3_isobmff_trun_box_t;


atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box_new();
void atsc3_isobmff_mdhd_box_free(atsc3_isobmff_mdhd_box_t** atsc3_isobmff_mdhd_box_p);
atsc3_isobmff_mdhd_box_t* atsc3_isobmff_box_parser_tools_parse_mdhd_from_block_t(block_t* isobmff_fragment_block_t);

atsc3_isobmff_tfhd_box_t* atsc3_isobmff_tfhd_box_new();
void atsc3_isobmff_tfhd_box_free(atsc3_isobmff_tfhd_box_t** atsc3_isobmff_tfhd_box_p);
atsc3_isobmff_tfhd_box_t* atsc3_isobmff_box_parser_tools_parse_tfhd_from_block_t(block_t* isobmff_movie_fragment_metadata_block_t);



#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_INFO(...)  if(_ATSC3_ISOBMFF_BOX_PARSER_TOOLS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG(...) if(_ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE(...) if(_ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#ifdef __cplusplus
}
#endif


#endif //ATSC3_ISOBMFF_BOX_PARSER_TOOLS_H
