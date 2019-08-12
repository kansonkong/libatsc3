/*
 * atsc3_fdt_test.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


#include "../atsc3_utils.h"
#include "../atsc3_mmt_signalling_message_types.h"
#include "../atsc3_mmt_signalling_message.h"


block_t* get_payload_from_filename(const char* file_name) {
	if( access(file_name, F_OK ) == -1 ) {
		__MMSM_ERROR("alc_get_payload_from_filename: unable to open file: %s", file_name);
		return NULL;
	}

	struct stat st;
	stat(file_name, &st);

	//uint8_t* payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
	block_t* payload = block_Alloc(st.st_size);

	FILE* fp = fopen(file_name, "r");
	if(st.st_size == 0) {
		__MMSM_ERROR("alc_get_payload_from_filename: size: 0 file: %s", file_name);
		return NULL;
	}

	fread(payload->p_buffer, st.st_size, 1, fp);
    payload->i_pos = st.st_size;
	fclose(fp);

	return payload;

}



int test_parse_mp_item() {

	return 0;
}

int test_parse_mp_table() {


	return 0;
}
/*
 * 239.155.1.1.49152.6425.9816.33024.atsc3_mmt_message.bin
 */
int test_parse_atsc3_mmt_message() {
	const char* TEST_ATSC3_MMT_MESSAGE_FILENAME = "testdata/mmt/signalling_info/239.155.1.1.49152.6425.9816.33024.atsc3_mmt_message.bin";

	block_t* test_atsc3_mmt_message = get_payload_from_filename(TEST_ATSC3_MMT_MESSAGE_FILENAME);




	return 0;
}
/*
 * todo add HRBM testing
 */


int main(int argc, char* argv[] ) {



}
