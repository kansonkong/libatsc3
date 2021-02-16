//
// Created by Jason Justman on 12/1/20.
//

#include "atsc3_stpp_decoder_configuration_record.h"

atsc3_stpp_decoder_configuration_record_t* atsc3_stpp_decoder_configuration_record_new() {
    atsc3_stpp_decoder_configuration_record_t* atsc3_stpp_decoder_configuration_record = calloc(1, sizeof(atsc3_stpp_decoder_configuration_record_t));
    return atsc3_stpp_decoder_configuration_record;
}

void atsc3_stpp_decoder_configuration_record_free(atsc3_stpp_decoder_configuration_record_t** atsc3_stpp_decoder_configuration_record_p) {
    if(atsc3_stpp_decoder_configuration_record_p) {
        atsc3_stpp_decoder_configuration_record_t* atsc3_stpp_decoder_configuration_record = *atsc3_stpp_decoder_configuration_record_p;
        if(atsc3_stpp_decoder_configuration_record) {

            free(atsc3_stpp_decoder_configuration_record);
            atsc3_stpp_decoder_configuration_record = NULL;
        }

        *atsc3_stpp_decoder_configuration_record_p = NULL;
    }
}
