//
// Created by Jason Justman on 12/1/20.
//
#include <stddef.h>
#include <stdlib.h>

#ifndef ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_H_
#define ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_H_

#include "atsc3_logging_externs.h"
#include "atsc3_vector_builder.h"
#include "atsc3_mmt_context_mfu_depacketizer.h"

#ifdef __cplusplus
extern "C" {
#endif

//wire up default JNI callbacks
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_callbacks_default_jni_new();

#ifdef __cplusplus
}
#endif

#endif //ATSC3_MMT_MFU_CONTEXT_CALLBACKS_DEFAULT_JNI_H_
