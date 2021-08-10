/*
 * atsc3_gzip.c
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */



#include "atsc3_gzip.h"

/*
 jjustman-2021-08-09
 
 NOTE: if ret <=0, decompressed_payload will be set to NULL as we don't have a valid decompressed datastream

 */
int32_t atsc3_unzip_gzip_payload(uint8_t* input_payload, uint32_t input_payload_size, uint8_t **decompressed_payload) {

	*decompressed_payload = NULL;
	
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

	if (ret != Z_OK) {
		//jjustman-2021-08-09 - hack-ish - don't leak our malloc output_payload if we've failed deflate...
		if(output_payload) {
			freesafe((void*)output_payload);
		}
	   return ret;
	}
	
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
					
				//jjustman-2021-08-09 - hack-ish - don't leak our malloc output_payload if we've failed deflate...
				if(output_payload) {
					freesafe((void*)output_payload);
				}

				return ret;
			}

			if(strm.avail_in > 0 && strm.avail_out == 0) {
                output_payload_available += GZIP_CHUNK_OUTPUT_BUFFER_SIZE;
				uint32_t output_payload_new_size = output_payload_offset + GZIP_CHUNK_OUTPUT_BUFFER_SIZE + 1;
				output_payload = (uint8_t*)realloc(output_payload, output_payload_new_size);
            } else {
                output_payload_available = strm.avail_out;
            }
            output_payload_offset = strm.total_out; //move our append pointer forward
			
		} while (ret != Z_STREAM_END && strm.avail_out == 0); //loop until we are out of free output space, then loop input position

        input_payload_offset = strm.total_in; //move input pointer forward += GZIP_CHUNK_INPUT_READ_SIZE;

	} while (ret != Z_STREAM_END && input_payload_offset < input_payload_size);

    int payload_len = strm.total_out; //(output_payload_offset + (GZIP_CHUNK_OUTPUT_BUFFER_SIZE - strm.avail_out));
	/* clean up and return */
	output_payload[payload_len] = '\0';
	*decompressed_payload = output_payload;

	(void)inflateEnd(&strm);
	
	if(ret == Z_STREAM_END) {
		return payload_len;
	} else {
		//jjustman-2021-08-09 - hack-ish - don't leak our malloc output_payload if we've failed deflate...
		if(output_payload) {
			freesafe((void*)output_payload);
		}
		return Z_DATA_ERROR;
	}
}

/*
 gunzip w/ pre-allocated dest, e.g.
 
	 block_t* atsc3_decompressed_payload = block_Alloc(atsc3_fdt_file_matching->content_length);
							 
	 int32_t unzipped_size = atsc3_unzip_gzip_payload_block_t(atsc3_fdt_file_gzip_contents, atsc3_decompressed_payload);
 */

int32_t atsc3_unzip_gzip_payload_block_t(block_t* src, block_t* dest) {
	block_Rewind(src);
	
	uint8_t* input_payload = block_Get(src);
	uint32_t input_payload_size = block_Remaining_size(src);
	
	uint8_t* output_payload = block_Get(dest);
    uint32_t output_payload_allocated = block_Remaining_size(dest);
    uint32_t output_payload_available = block_Remaining_size(dest);
	
	if(input_payload_size > output_payload_available) return -1;

	unsigned int input_payload_offset = 0;
	unsigned int output_payload_offset = 0;

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

	int loop_counter_outer = 0;
	int loop_counter_inner = 0;
	do {
		strm.next_in = &input_payload[input_payload_offset];

		unsigned int payload_chunk_size = input_payload_size - input_payload_offset > GZIP_CHUNK_INPUT_READ_SIZE ? GZIP_CHUNK_INPUT_READ_SIZE : input_payload_size - input_payload_offset;
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
                case Z_BUF_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);

				return ret;
			}

            output_payload_offset = strm.total_out; //move our append pointer forward
            loop_counter_inner++;

            //jjustman-2020-12-09 - hack, if we are within a few bytes of chunk size, we may fill the buffer, so check against strm.total_out != output_payload_allocated

        } while (ret != Z_STREAM_END && strm.avail_out == 0 && strm.total_out != output_payload_allocated); //loop until we are out of free output space, then loop input position

        input_payload_offset = strm.total_in; //move input pointer forward += GZIP_CHUNK_INPUT_READ_SIZE;

        loop_counter_outer++;
	} while (ret != Z_STREAM_END && input_payload_offset < input_payload_size);

    int payload_len = strm.total_out;

	(void)inflateEnd(&strm);
	
	return ret == Z_STREAM_END ?  payload_len : Z_DATA_ERROR;
}


int32_t atsc3_unzip_gzip_payload_block_t_with_dynamic_realloc(block_t* src, block_t** dest_p) {
	if(!dest_p) {
		return -1;
	}
	block_Rewind(src);
	
	uint8_t* input_payload = block_Get(src);
	uint32_t input_payload_size = block_Remaining_size(src);
	
	*dest_p = block_Alloc(0);
	block_t* dest = *dest_p;
	
    unsigned int output_payload_available = GZIP_CHUNK_OUTPUT_BUFFER_SIZE;
    unsigned char *output_payload = (unsigned char*)calloc(GZIP_CHUNK_OUTPUT_BUFFER_SIZE + 1, sizeof(unsigned char));

	unsigned int input_payload_offset = 0;
	unsigned int output_payload_offset = 0;

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

		unsigned int payload_chunk_size = input_payload_size - input_payload_offset > GZIP_CHUNK_INPUT_READ_SIZE ? GZIP_CHUNK_INPUT_READ_SIZE : input_payload_size - input_payload_offset;
		strm.avail_in = payload_chunk_size;

		if (strm.avail_in <= 0)
			break;

		do {
			strm.avail_out = output_payload_available;
			strm.next_out = output_payload;

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
			
			block_Write(dest, output_payload, output_payload_available - strm.avail_out);
		} while (ret != Z_STREAM_END && strm.avail_out == 0); //loop until we are out of free output space, then loop input position

		input_payload_offset = strm.total_in; //move input pointer forward += GZIP_CHUNK_INPUT_READ_SIZE;

	} while (ret != Z_STREAM_END && input_payload_offset < input_payload_size);

	int payload_len = strm.total_out; //(output_payload_offset + (GZIP_CHUNK_OUTPUT_BUFFER_SIZE - strm.avail_out));
	/* clean up and return */
	//jjustman-2020-11-23 - don't put null terminating character, assumed that block_t will have pre-allocated if needed...
	//output_payload[payload_len] = '\0';

	(void)inflateEnd(&strm);
	
	return ret == Z_STREAM_END ?  payload_len : Z_DATA_ERROR;
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
