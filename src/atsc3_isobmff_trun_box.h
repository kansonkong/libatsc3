/*
 * atsc3_isobmff_trun_box.h
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_ISOBMFF_TRUN_BOX_H_
#define ATSC3_ISOBMFF_TRUN_BOX_H_

//for bento4 support
typedef struct trun_sample_entry {
   uint32_t sample_duration;
   uint32_t sample_size;
   uint32_t sample_flags;
   uint32_t sample_composition_time_offset;
   bool to_remove_sample_entry;
   bool has_matching_sample;
} trun_sample_entry_t;

typedef struct trun_sample_entry_vector {
	uint32_t size;
	trun_sample_entry_t** data;
} trun_sample_entry_vector_t;



#endif /* ATSC3_ISOBMFF_TRUN_BOX_H_ */
