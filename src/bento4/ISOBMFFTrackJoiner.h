/*
 * ISOBMFFTrackJoiner.h
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */

//linux compat
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#include <_types/_uint8_t.h>
#include <list>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Ap4.h"

#include "../../bento/include/Ap4Atom.h"
#include "../atsc3_utils.h"

#include "../atsc3_lls_sls_monitor_output_buffer.h"


using namespace std;

#ifndef BENTO4_ISOBMFFTRACKJOINER_H_
#define BENTO4_ISOBMFFTRACKJOINER_H_

extern int _ISOBMFFTRACKJOINER_DEBUG_ENABLED;
extern int _ISOBMFFTRACKJOINER_TRACE_ENABLED;

typedef struct ISOBMFFTrackJoinerFileResouces {
	char*		file1_name;
	uint8_t* 	file1_payload;
	uint64_t 	file1_size;
	uint8_t		file1_target_track_num;

	char* 		file2_name;
	uint8_t* 	file2_payload;
	uint64_t 	file2_size;
	uint8_t		file2_target_track_num;

} ISOBMFFTrackJoinerFileResouces_t;

list<AP4_Atom*> ISOBMFFTrackParse(uint8_t* full_mpu_payload, uint32_t full_mpu_payload_size);

ISOBMFFTrackJoinerFileResouces_t* loadFileResources(const char*, const char*);

void ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, AP4_ByteStream* output_stream);

void parseAndBuildJoinedBoxes(ISOBMFFTrackJoinerFileResouces_t*, AP4_ByteStream* output_stream);
void parseAndBuildJoinedBoxesFromMemory(uint8_t* file1_payload, uint32_t file1_size, uint8_t* file2_payload, uint32_t file2_size, AP4_ByteStream* output_stream);

void dumpFullMetadata(list<AP4_Atom*> atomList);
void printBoxType(AP4_Atom* atom);

#define __ISOBMFF_JOINER_PRINTLN(...) printf(__VA_ARGS__);printf("\n")
#define __ISOBMFF_JOINER_INFO(...)    printf("%s:%d:INFO :",__FILE__,__LINE__);__ISOBMFF_JOINER_PRINTLN(__VA_ARGS__);
#define __ISOBMFF_JOINER_DEBUG(...) if(_ISOBMFFTRACKJOINER_DEBUG_ENABLED) { printf("%s:%d:DEBUG :",__FILE__,__LINE__);__ISOBMFF_JOINER_PRINTLN(__VA_ARGS__); }


#endif /* BENTO4_ISOBMFFTRACKJOINER_H_ */
