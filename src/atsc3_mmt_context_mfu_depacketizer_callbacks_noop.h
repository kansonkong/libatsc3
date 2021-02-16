//
// Created by Jason Justman on 12/1/20.
//
#include <stddef.h>
#include <stdlib.h>

#ifndef ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_H_
#define ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_H_

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"

#include "atsc3_mmt_context_mfu_depacketizer.h"

#ifdef __cplusplus
extern "C" {
#endif

atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_callbacks_noop_new();


#ifdef __cplusplus
}
#endif

#endif //ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_H_
