/*
 * atsc3_mmt_signalling_messages_types.c
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */

#include "atsc3_mmt_signalling_message_types.h"

//todo, we will probably need to iterate over each one of these entries
void mmt_signalling_message_vector_free(mmt_signalling_message_vector_t** mmt_signalling_message_vector_p) {
	mmt_signalling_message_vector_t* mmt_signalling_message_vector = *mmt_signalling_message_vector_p;
	if(mmt_signalling_message_vector) {
		for(int i=0; i < mmt_signalling_message_vector->messages_n; mmt_signalling_message_vector++) {
			mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = mmt_signalling_message_vector->messages[i];
			mmt_signalling_message_header_and_payload_free(&mmt_signalling_message_header_and_payload);
		}

		free(mmt_signalling_message_vector);
		mmt_signalling_message_vector = NULL;
		*mmt_signalling_message_vector_p = NULL;
	}
}

void mmt_signalling_message_header_and_payload_free(mmt_signalling_message_header_and_payload_t** mmt_signalling_message_header_and_payload_p) {
	mmt_signalling_message_header_and_payload_t* mmt_signalling_message_header_and_payload = *mmt_signalling_message_header_and_payload_p;
	if(mmt_signalling_message_header_and_payload) {

		//determine if we have any internal mallocs to clear
		if(mmt_signalling_message_header_and_payload->message_header.message_id == MMT_ATSC3_MESSAGE_ID) {
			if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.URI_payload = NULL;
			}
			if(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content) {
				free(mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content);
				mmt_signalling_message_header_and_payload->message_payload.mmt_atsc3_message_payload.atsc3_message_content = NULL;
			}
		}

finally:
	free(mmt_signalling_message_header_and_payload);
	*mmt_signalling_message_header_and_payload_p = NULL;

	}
}
