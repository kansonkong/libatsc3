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

#define __ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT(label, ptr) \
	if(!atsc3_mmt_mfu_context) { \
		__MMT_CONTEXT_MPU_WARN("%s: atsc3_mmt_mfu_context is NULL for %s, ptr: %p", __FUNCTION__, label, ptr); return; \
	} else { \
		__MMT_CONTEXT_MPU_DEBUG("callback_noop: %s: atsc3_mmt_mfu_context: %p, data: %s, ptr: %p", __FUNCTION__, atsc3_mmt_mfu_context, label, ptr); \
	}

#define __ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_GUARD_AND_LOG_ATSC3_MMT_MFU_CONTEXT_RETURN_FALSE(label, ptr) \
	if(!atsc3_mmt_mfu_context) { \
		__MMT_CONTEXT_MPU_WARN("%s: atsc3_mmt_mfu_context is NULL for %s, ptr: %p", __FUNCTION__, label, ptr); return false; \
	} else { \
		__MMT_CONTEXT_MPU_DEBUG("callback_noop: %s: atsc3_mmt_mfu_context: %p, data: %s, ptr: %p", __FUNCTION__, atsc3_mmt_mfu_context, label, ptr); \
	}


atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context_callbacks_noop_new();


#ifdef __cplusplus
}
#endif

#endif //ATSC3_MMT_CONTEXT_MFU_DEPACKETIZER_CALLBACKS_NOOP_H_
