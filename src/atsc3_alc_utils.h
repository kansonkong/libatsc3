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

extern int _ALC_UTILS_DEBUG_ENABLED;
extern int _ALC_UTILS_TRACE_ENABLED;


//ALC dump object output path
#define __ALC_DUMP_OUTPUT_PATH__ "route/"

//prepend fragments with init box (ftyp, mmpu, moov, meta)
//#define __TESTING_PREPEND_TSI__ 			"003000"
//#define __TESTING_PREPEND_TOI_INIT__ 		"0000002"

//reconsitiute fragments into single file, init box, for pipe playback...
#define __TESTING_RECONSTITUTED_TSI__ 		"003000"
#define __TESTING_RECONSTITUTED_TOI_INIT__ 	"0000002"
#define __TESTING_RECONSITIUTED_FILE_NAME__ "3.m4v"


/**
 * deubg toi dump methods
 */
char* alc_packet_dump_to_object_get_filename(alc_packet_t* alc_packet);

int alc_packet_dump_to_object(alc_packet_t* alc_packet);
void __alc_prepend_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet);
void __alc_recon_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet);


#define __ALC_UTILS_ERROR(...)   printf("%s:%d:ERROR: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");
#define __ALC_UTILS_WARN(...)    printf("%s:%d:WARN: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");
#define __ALC_UTILS_INFO(...)    printf("%s:%d:INFO: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");

#define __ALC_UTILS_DEBUG(...)   if(_ALC_UTILS_DEBUG_ENABLED) { printf("%s:%d:DEBUG: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n"); }
#define __ALC_UTILS_TRACE(...)   if(_ALC_UTILS_TRACE_ENABLED) { printf("%s:%d:TRACE: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n"); }


#endif /* ATSC3_ALC_UTILS_H_ */
