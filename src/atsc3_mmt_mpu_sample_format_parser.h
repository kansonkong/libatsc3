/*
 * atsc3_mmt_mpu_sample_format_parser.h
 *
 *  Created on: Aug 31, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_MPU_SAMPLE_FORMAT_PARSER_H_
#define ATSC3_MMT_MPU_SAMPLE_FORMAT_PARSER_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "atsc3_utils.h"
#include "atsc3_logging_externs.h"
#include "atsc3_mmtp_packet_types.h"
#include "atsc3_mmt_mpu_parser.h"

extern int _MMTP_MPU_SAMPLE_FORMAT_DEBUG_ENABLED;
extern int _MMTP_MPU_SAMPLE_FORMAT_TRACE_ENABLED;

mmtp_mpu_packet_t* atsc3_mmt_mpu_sample_format_parse(mmtp_mpu_packet_t* mmtp_mpu_packet, block_t* raw_packet);

#define __MMTP_MPU_SAMPLE_FORMAT_ERROR(...)   __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __MMTP_MPU_SAMPLE_FORMAT_WARN(...)    __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __MMTP_MPU_SAMPLE_FORMAT_INFO(...)    __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);

#define __MMTP_MPU_SAMPLE_FORMAT_DEBUG(...)   if(_MMTP_MPU_SAMPLE_FORMAT_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __MMTP_MPU_SAMPLE_FORMAT_TRACE(...)   if(_MMTP_MPU_SAMPLE_FORMAT_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


#ifdef __cplusplus
}
#endif
#endif /* ATSC3_MMT_MPU_SAMPLE_FORMAT_PARSER_H_ */
