/*
 * atsc3_gzip.h
 *
 *  Created on: Mar 6, 2019
 *      Author: jjustman
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "zlib.h"

#ifndef ATSC3_GZIP_H_
#define ATSC3_GZIP_H_
/**
 * footnote 5
 * The maximum size of the IP datagram is 65,535 bytes.
 * The maximum UDP data payload is 65,535 minus 20 bytes for the IP header minus 8 bytes for the UDP header.
 */

#define GZIP_CHUNK_INPUT_SIZE_MAX 65507
#define GZIP_CHUNK_INPUT_READ_SIZE 1024
#define GZIP_CHUNK_OUTPUT_BUFFER_SIZE 1024*8

int32_t atsc3_unzip_gzip_payload(uint8_t* input_payload, uint32_t input_payload_size, uint8_t **decompressed_payload);


#endif /* ATSC3_GZIP_H_ */
