/*
 * atsc3_alc_utils.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>


#include "alc_rx.h"
#include "alc_channel.h"

#ifndef ATSC3_ALC_UTILS_H_
#define ATSC3_ALC_UTILS_H_

//ALC dump object output path
#define __ALC_DUMP_OUTPUT_PATH__ "route/"

//prepend fragments with init box (ftyp, mmpu, moov, meta)
//#define __TESTING_PREPEND_TSI__ 			"003000"
//#define __TESTING_PREPEND_TOI_INIT__ 		"0000002"

//reconsitiute fragments into single file, init box, for pipe playback...
#define __TESTING_RECONSTITUTED_TSI__ 		"003000"
#define __TESTING_RECONSTITUTED_TOI_INIT__ 	"0000002"
#define __TESTING_RECONSITIUTED_FILE_NAME__ "3.m4v"


#define println(...) printf(__VA_ARGS__);printf("\n")

#define __PRINTLN(...) printf(__VA_ARGS__);printf("\n")
#define __PRINTF(...)  printf(__VA_ARGS__);

#define __ALC_UTILS_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);
#define __ALC_UTILS_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);
#define __ALC_UTILS_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);

#ifdef _ENABLE_DEBUG
#define __ALC_UTILS_DEBUG(...)   __PRINTLN("%s:%d:DEBUG:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);
#define __ALC_UTILS_TRACE(...)   __PRINTLN("%s:%d:DEBUG:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);

#else
#define __ALC_UTILS_DEBUG(...)
#define __ALC_UTILS_TRACE(...) __PRINTLN("%s:%d:DEBUG:",__FILE__,__LINE__);__PRINTLN(__VA_ARGS__);
#endif

/**
 * deubg toi dump methods
 */
char* alc_packet_dump_to_object_get_filename(alc_packet_t* alc_packet);

int alc_packet_dump_to_object(alc_packet_t* alc_packet);
void __alc_prepend_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet);
void __alc_recon_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet);

#endif /* ATSC3_ALC_UTILS_H_ */
