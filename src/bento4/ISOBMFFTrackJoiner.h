/*
 * ISOBMFFTrackJoiner.h
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 */

//linux compat
//#include <_types/_uint32_t.h>
//#include <_types/_uint64_t.h>
//#include <_types/_uint8_t.h>


#include <list>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
using namespace std;

//#include "../atsc3_utils.h"
#include "../atsc3_isobmff_trun_box.h"
#include "../atsc3_lls_sls_monitor_output_buffer.h"

#include "Ap4.h"
#include "Ap4Atom.h"

#ifndef BENTO4_ISOBMFFTRACKJOINER_H_
#define BENTO4_ISOBMFFTRACKJOINER_H_

#include "ISOBMFFTrackJoiner_firewall_gpl.h"

#if defined (__cplusplus)
extern "C" {
#endif

extern int _ISOBMFFTRACKJOINER_DEBUG_ENABLED;
extern int _ISOBMFFTRACKJOINER_TRACE_ENABLED;

void ISOBMFF_track_joiner_monitor_output_buffer_parse_and_build_joined_boxes(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, AP4_MemoryByteStream** output_stream);

void parseAndBuildJoinedBoxes_from_lls_sls_monitor_output_buffer(lls_sls_monitor_output_buffer_t* lls_sls_monitor_output_buffer, AP4_MemoryByteStream** output_stream_p);
void parseAndBuildJoinedBoxes(ISOBMFFTrackJoinerFileResouces_t*, AP4_ByteStream* output_stream);
void parseAndBuildJoinedBoxesFromMemory(uint8_t* file1_payload, uint32_t file1_size, uint8_t* file2_payload, uint32_t file2_size, AP4_ByteStream* output_stream);

void dumpFullMetadata(list<AP4_Atom*> atomList);
void printBoxType(AP4_Atom* atom);

#define __ISOBMFF_JOINER_PRINTLN(...)
  //fprintf(stderr, __VA_ARGS__);fprintf(stderr, "\n")
#define __ISOBMFF_JOINER_INFO(...)
  //fprintf(stderr, "%s:%d:INFO :",__FILE__,__LINE__);__ISOBMFF_JOINER_PRINTLN(__VA_ARGS__);
#define __ISOBMFF_JOINER_DEBUG(...)
  //if(_ISOBMFFTRACKJOINER_DEBUG_ENABLED) { fprintf(stderr, "%s:%d:DEBUG :",__FILE__,__LINE__);__ISOBMFF_JOINER_PRINTLN(__VA_ARGS__); }

#if defined (__cplusplus)
}
#endif


//list<AP4_Atom*> ISOBMFFTrackParse(uint8_t* full_mpu_payload, uint32_t full_mpu_payload_size);
list<AP4_Atom*> ISOBMFFTrackParse(block_t* isobmff_track_block);

//ISOBMFFTrackJoinerFileResouces_t* loadFileResources(const char*, const char*);



#endif /* BENTO4_ISOBMFFTRACKJOINER_H_ */
