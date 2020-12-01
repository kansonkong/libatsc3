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
 * extends fullbox
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

atsc3_isobmff_mdhd_box_t* atsc3_isobmff_mdhd_box_new();
void atsc3_isobmff_mdhd_box_free(atsc3_isobmff_mdhd_box_t** atsc3_isobmff_mdhd_box_p);

atsc3_isobmff_mdhd_box_t* atsc3_isobmff_box_parser_tools_parse_mdhd_timescale_from_block_t(block_t* isobmff_fragment_block_t);


#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_INFO(...)  if(_ATSC3_ISOBMFF_BOX_PARSER_TOOLS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG(...) if(_ATSC3_ISOBMFF_BOX_PARSER_TOOLS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE(...) if(_ATSC3_ISOBMFF_BOX_PARSER_TOOLS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#ifdef __cplusplus
}
#endif


#endif //ATSC3_ISOBMFF_BOX_PARSER_TOOLS_H
