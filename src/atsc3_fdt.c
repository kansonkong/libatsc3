
/*
 * atsc3_fdt.c
 *
 *  Created on: Mar 17, 2019
 *      Author: jjustman
 */
#include "atsc3_fdt.h"

ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_fdt_instance, atsc3_fdt_file);

/*
 char*                         content_location;
 uint32_t                     toi;
 uint32_t                     content_length;
 uint32_t                     transfer_length;
 char*                        content_type;
 char*                        content_encoding;
 char*                        content_md5;


 atsc3_fdt_fec_attributes_t    atsc3_fdt_fec_attributes;
 */
void atsc3_fdt_file_free(atsc3_fdt_file_t** atsc3_fdt_file_p) {
    if(atsc3_fdt_file_p) {
        atsc3_fdt_file_t* atsc3_fdt_file = *atsc3_fdt_file_p;
        if(atsc3_fdt_file) {
            freeclean((void**)&atsc3_fdt_file->content_location);
            freeclean((void**)&atsc3_fdt_file->content_type);
            freeclean((void**)&atsc3_fdt_file->content_encoding);
            freeclean((void**)&atsc3_fdt_file->content_md5);


            freeclean((void**)&atsc3_fdt_file->app_context_id_list);
            freeclean((void**)&atsc3_fdt_file->filter_codes);

            //atsc3_fdt_fec_attributes
            freeclean((void**)&atsc3_fdt_file->atsc3_fdt_fec_attributes.fec_oti_sceheme_specific_info);

            free(atsc3_fdt_file);
            atsc3_fdt_file = NULL;
        }
        *atsc3_fdt_file_p = NULL;
    }
}



//todo: move this to atsc3_fdt.c
void atsc3_fdt_instance_free(atsc3_fdt_instance_t** atsc3_fdt_instance_p) {
    if(atsc3_fdt_instance_p) {
        atsc3_fdt_instance_t* atsc3_fdt_instance = *atsc3_fdt_instance_p;
        if(atsc3_fdt_instance) {
            freeclean((void**)&atsc3_fdt_instance->content_type);
            freeclean((void**)&atsc3_fdt_instance->content_encoding);
            freeclean((void**)&atsc3_fdt_instance->atsc3_fdt_fec_attributes.fec_oti_sceheme_specific_info);
            freeclean((void**)&atsc3_fdt_instance->app_context_id_list);
            freeclean((void**)&atsc3_fdt_instance->file_template);
            freeclean((void**)&atsc3_fdt_instance->filter_codes);

            atsc3_fdt_instance_free_atsc3_fdt_file(atsc3_fdt_instance);

            free(atsc3_fdt_instance);
            atsc3_fdt_instance = NULL;
        }
        atsc3_fdt_instance_p = NULL;
    }
}

