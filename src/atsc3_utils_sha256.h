/*
 * atsc3_utils_sha256.h
 *
 *  Created on: Jul 27, 2020
 *      Author: jjustman
 *
 *      borrowed from: https://raw.githubusercontent.com/B-Con/crypto-algorithms/master/sha256.h
 *
 *		2021-03-01: prefixed with atsc3_ to prevent typedef and method collisions with openssl linkage for CMS verification
 */


#ifndef ATSC3_UTILS_SHA256_H_
#define ATSC3_UTILS_SHA256_H_

/*********************************************************************
* Filename:   sha256.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding SHA1 implementation.
*********************************************************************/

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include <stdint.h>

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
#ifdef _WIN32
typedef uint16_t WORD;
#else
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines
#endif

typedef struct {
	BYTE data[64];
	WORD datalen;
	unsigned long long bitlen;
	WORD state[8];
} atsc3_SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void atsc3_sha256_init(atsc3_SHA256_CTX *ctx);
void atsc3_sha256_update(atsc3_SHA256_CTX *ctx, const BYTE data[], size_t len);
void atsc3_sha256_final(atsc3_SHA256_CTX *ctx, BYTE hash[]);

#endif /* ATSC3_UTILS_SHA256_H_ */
