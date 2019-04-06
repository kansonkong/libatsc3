/*
 * atsc3_isobmff_trun_box.h
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_ISOBMFF_TRUN_BOX_H_
#define ATSC3_ISOBMFF_TRUN_BOX_H_

#include "atsc3_utils.h"

//for bento4 support
typedef struct trun_sample_entry {

	//sample length and offset will be unknown if our mmth_box is missing, so be prepared to recompute
	bool 	mmth_box_missing;

	block_t* sample;

	uint32_t 	sequence_number;				//same as mpu_sequence_number
	uint8_t 	trackrefindex; 					//track id in mpu
	uint32_t 	movie_fragment_sequence_number;	//fragment id of mpu in isobmff
	uint32_t 	samplenumber;					//indvidual sample (e.g. frame) for recon
	uint8_t  	priority;
	uint8_t  	dependency_counter;				//e.g. b-frame

	uint32_t 	sample_length;					//how many bytes this sampe comprises for pre-alloc
	uint32_t 	sample_offset;					//position in mdat for sample
	uint32_t 	sample_duration;

	uint32_t sample_flags;
	uint32_t sample_composition_time_offset;

	bool to_remove_sample_entry;
	bool has_matching_sample;

} trun_sample_entry_t;

/**
 * deprecated
typedef struct trun_sample_entry_vector {
	uint32_t size;
	trun_sample_entry_t** data;
} trun_sample_entry_vector_t;

**/


#endif /* ATSC3_ISOBMFF_TRUN_BOX_H_ */
