//
// Created by Jason Justman on 12/1/20.
//

#include <stddef.h>
#include <stdlib.h>

#ifndef ATSC3_STPP_DECODER_CONFIGURATION_RECORD_H_
#define ATSC3_STPP_DECODER_CONFIGURATION_RECORD_H_

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"
#include "atsc3_isobmff_box_parser_tools.h"

#ifdef __cplusplus
extern "C" {
#endif

//jjustman-2020-12-01 - TODO - refactor this out
typedef struct atsc3_stpp_decoder_configuration_record {
    uint32_t        timebase;
} atsc3_stpp_decoder_configuration_record_t;

atsc3_stpp_decoder_configuration_record_t* atsc3_stpp_decoder_configuration_record_new();
void atsc3_stpp_decoder_configuration_record_free(atsc3_stpp_decoder_configuration_record_t** atsc3_stpp_decoder_configuration_record_p);


#ifdef __cplusplus
}
#endif

#endif //ATSC3_STPP_DECODER_CONFIGURATION_RECORD_H_