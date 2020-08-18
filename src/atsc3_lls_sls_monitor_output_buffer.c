//
//  atsc3_lls_sls_monitor_output_buffer.c
//  cmd
//
//  Created by Jason Justman on 3/2/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//


#include "atsc3_lls_sls_monitor_output_buffer.h"

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(lls_sls_monitor_buffer_isobmff, trun_sample_entry);

void trun_sample_entry_free(trun_sample_entry_t** trun_sample_entry_p) {
	if(trun_sample_entry_p) {
		trun_sample_entry_t* trun_sample_entry = *trun_sample_entry_p;

		if(trun_sample_entry) {
			block_Destroy(&trun_sample_entry->sample);

			freesafe(trun_sample_entry);
			trun_sample_entry = NULL;
		}
		*trun_sample_entry_p = NULL;
	}
}

void lls_sls_monitor_buffer_isobmff_clear_trun_sample_entry_instances(lls_sls_monitor_buffer_isobmff_t* lls_sls_monitor_buffer_isobmff) {
    for(int i=0; i < lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.count; i++) {
        trun_sample_entry_t* trun_sample_entry = lls_sls_monitor_buffer_isobmff->trun_sample_entry_v.data[i];
        if(trun_sample_entry->sample) {
            block_Destroy(&trun_sample_entry->sample);
        }
    }
    
    lls_sls_monitor_buffer_isobmff_clear_trun_sample_entry(lls_sls_monitor_buffer_isobmff);
}

