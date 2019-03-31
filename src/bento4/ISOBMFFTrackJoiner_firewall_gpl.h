/*
 * ISOBMFFTrackJoiner_methods.h
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include "../atsc3_utils.h"
#include "../atsc3_lls_types.h"

#include "../atsc3_isobmff_trun_box.h"
#include "../atsc3_lls_sls_monitor_output_buffer.h"

#ifndef BENTO4_ISOBMFFTRACKJOINER_METHODS_H_
#define BENTO4_ISOBMFFTRACKJOINER_METHODS_H_

#if defined (__cplusplus)
extern "C" {
#endif


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


//extern trun_sample_entry_vector_t* parseMoofBoxForTrunSampleEntries(block_t* moof_box);
block_t* lls_sls_monitor_output_buffer_copy_alc_full_isobmff_box(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff);


#if defined (__cplusplus)
}
#endif


#endif /* BENTO4_ISOBMFFTRACKJOINER_METHODS_H_ */
