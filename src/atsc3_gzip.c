/*
 * atsc3_gzip.c
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */



#include "atsc3_gzip.h"

int32_t atsc3_unzip_gzip_payload(uint8_t* input_payload, uint32_t input_payload_size, uint8_t **decompressed_payload) {

	if(input_payload_size > GZIP_CHUNK_INPUT_SIZE_MAX) return -1;

	uint input_payload_offset = 0;
	uint output_payload_offset = 0;
    unsigned char *output_payload = NULL;

    int ret;
    unsigned have;
    z_stream strm;

    uint8_t *decompressed;

    strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	strm.data_type = Z_TEXT;

	//treat this input_payload as gzip not just delfate
	ret = inflateInit2(&strm, 16+MAX_WBITS);

	if (ret != Z_OK)
	   return ret;

	do {

		strm.next_in = &input_payload[input_payload_offset];

		uint payload_chunk_size = input_payload_size - input_payload_offset > GZIP_CHUNK_INPUT_READ_SIZE ? GZIP_CHUNK_INPUT_READ_SIZE : input_payload_size - input_payload_offset;
		strm.avail_in = payload_chunk_size;

		if (strm.avail_in <= 0)
			break;

		do {
			if(!output_payload) {
				output_payload = (uint8_t*)calloc(GZIP_CHUNK_OUTPUT_BUFFER_SIZE + 1, sizeof(uint8_t));
			}

			if(!output_payload)
				return -1;

			strm.avail_out = GZIP_CHUNK_OUTPUT_BUFFER_SIZE;
			strm.next_out = &output_payload[output_payload_offset];

			ret = inflate(&strm, Z_NO_FLUSH);

			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
				return ret;
			}

			if(strm.avail_out == 0) {
				output_payload_offset += GZIP_CHUNK_OUTPUT_BUFFER_SIZE;
				output_payload = (uint8_t*)realloc(output_payload, output_payload_offset + GZIP_CHUNK_OUTPUT_BUFFER_SIZE + 1);

			}
		} while (strm.avail_out == 0);

		input_payload_offset += GZIP_CHUNK_INPUT_READ_SIZE;

	} while (ret != Z_STREAM_END && input_payload_offset < input_payload_size);


	int paylod_len = (output_payload_offset + (GZIP_CHUNK_OUTPUT_BUFFER_SIZE - strm.avail_out));
	/* clean up and return */
	output_payload[paylod_len] = '\0';
	*decompressed_payload = output_payload;

	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ?  paylod_len : Z_DATA_ERROR;

}
