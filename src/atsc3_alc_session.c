/*
 * atsc3_alc_session.c
 *
 *  Created on: Feb 28, 2019
 *      Author: jjustman
 */

#include "atsc3_alc_session.h"

atsc3_alc_session_t* atsc3_open_alc_session(atsc3_alc_arguments_t* atsc3_alc_arguments) {

    atsc3_alc_session_t *atsc3_alc_session = NULL;

    if(!atsc3_alc_arguments) {
        goto error;
    }

    atsc3_alc_session = calloc(1, sizeof(atsc3_alc_session_t));
    atsc3_alc_session->tsi = atsc3_alc_arguments->tsi;

    return atsc3_alc_session;

error:

    return NULL;
}

