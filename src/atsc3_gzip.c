/*
 * atsc3_gzip.c
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */



#include "atsc3_gzip.h"

int32_t atsc3_unzip_gzip_payload(uint8_t* input_payload, uint32_t input_payload_size, uint8_t **decompressed_payload) {

	if(input_payload_size > GZIP_CHUNK_INPUT_SIZE_MAX) return -1;

	uint32_t input_payload_offset = 0;
	uint32_t output_payload_offset = 0;
	uint32_t output_payload_available = GZIP_CHUNK_OUTPUT_BUFFER_SIZE;
    unsigned char *output_payload = (unsigned char*)calloc(GZIP_CHUNK_OUTPUT_BUFFER_SIZE + 1, sizeof(unsigned char));

	if(!output_payload) {
		return -1;
	}

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
    int loop_count=0;
	//treat this input_payload as gzip not just delfate
	ret = inflateInit2(&strm, 16+MAX_WBITS);

	if (ret != Z_OK)
	   return ret;

	do {
		strm.next_in = &input_payload[input_payload_offset];

		uint32_t payload_chunk_size = input_payload_size - input_payload_offset > GZIP_CHUNK_INPUT_READ_SIZE ? GZIP_CHUNK_INPUT_READ_SIZE : input_payload_size - input_payload_offset;
		strm.avail_in = payload_chunk_size;

		if (strm.avail_in <= 0)
			break;

		do {
			strm.avail_out = output_payload_available;
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
                output_payload_available += GZIP_CHUNK_OUTPUT_BUFFER_SIZE;
				uint32_t output_payload_new_size = output_payload_offset + GZIP_CHUNK_OUTPUT_BUFFER_SIZE + 1;
				output_payload = (uint8_t*)realloc(output_payload, output_payload_new_size);
            } else {
                output_payload_available = strm.avail_out;
            }
            output_payload_offset = strm.total_out; //move our append pointer forward
		} while (strm.avail_out == 0); //loop until we are out of free output space, then loop input position

        input_payload_offset = strm.total_in; //move input pointer forward += GZIP_CHUNK_INPUT_READ_SIZE;

	} while (ret != Z_STREAM_END && input_payload_offset < input_payload_size);

    int paylod_len = strm.total_out; //(output_payload_offset + (GZIP_CHUNK_OUTPUT_BUFFER_SIZE - strm.avail_out));
	/* clean up and return */
	output_payload[paylod_len] = '\0';
	*decompressed_payload = output_payload;

	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ?  paylod_len : Z_DATA_ERROR;
}


uint32_t atsc3_compress_gzip_payload(uint8_t* input, uint32_t inputSize, uint8_t* output, uint32_t outputSize) {
    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = (uInt)inputSize;
    zs.next_in = (Bytef *)input;
    zs.avail_out = (uInt)outputSize;
    zs.next_out = (Bytef *)output;

    // hard to believe they don't have a macro for gzip encoding, "Add 16" is the best thing zlib can do:
    // "Add 16 to windowBits to write a simple gzip header and trailer around the compressed data instead of a zlib wrapper"
    deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 | 16, 8, Z_DEFAULT_STRATEGY);
    deflate(&zs, Z_FINISH);
    deflateEnd(&zs);
    return zs.total_out;
}
