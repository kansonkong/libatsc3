/*
 * ISOBMFFTrackJoiner.cpp
 *
 *  Created on: Feb 20, 2019
 *      Author: jjustman
 *
 *      ISOBMFF ftyp/moov/moof/mdat track joiner
 */


#include "ISOBMFFTrackJoiner_firewall_gpl.h"

int _ISOBMFFTRACKJOINER_DEBUG_ENABLED = 0;
int _ISOBMFFTRACKJOINER_TRACE_ENABLED = 0;


ISOBMFFTrackJoinerFileResouces_t* loadFileResources(const char* file1, const char* file2) {
	return NULL;
}

trun_sample_entry_vector_t* parseMoofBoxForTrunSampleEntries(block_t* moof_box) {
	return NULL;
}
