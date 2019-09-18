/*
 * atsc3_aeat_types.c
 *
 *  Created on: Sep 18, 2019
 *      Author: jjustman
 */

#include "atsc3_aeat_types.h"

int _AEAT_TYPES_INFO_ENABLED = 1;
int _AEAT_TYPES_DEBUG_ENABLED = 0;
int _AEAT_TYPES_TRACE_ENABLED = 0;


atsc3_aeat_table_t* atsc3_aeat_table_new() {
	atsc3_aeat_table_t* atsc3_aeat_table = calloc(1, sizeof(atsc3_aeat_table_t));

	return atsc3_aeat_table;
}

void atsc3_aeat_table_free(atsc3_aeat_table_t** atsc3_aeat_table_p) {
    if(atsc3_aeat_table_p) {
        atsc3_aeat_table_t* atsc3_aeat_table = *atsc3_aeat_table_p;
        if(atsc3_aeat_table) {
            if(atsc3_aeat_table->aeat_xml_fragment_latest) {
                free(atsc3_aeat_table->aeat_xml_fragment_latest);
                atsc3_aeat_table->aeat_xml_fragment_latest = NULL;
            }
            
            //clear atsc3_aeat_table->atsc3_aea_table
            
            free(atsc3_aeat_table);
            atsc3_aeat_table = NULL;
        }
        
        *atsc3_aeat_table_p = NULL;
    }
}


