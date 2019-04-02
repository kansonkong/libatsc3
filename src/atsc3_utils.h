/*
 * atsc3_utils.h
 *
 *  Created on: Jan 6, 2019
 *      Author: jjustman
 */
#pragma GCC diagnostic ignored "-Wformat-zero-length"

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <strings.h>

#include <time.h>
#include <sys/time.h>

#ifndef ATSC3_UTILS_H_
#define ATSC3_UTILS_H_

#include "fixups.h"
#include "unistd.h"

#define uS 1000000ULL

//ATSC3/331 Section 6.1 - drop non mulitcast ip ranges - e.g not in  239.255.0.0 to 239.255.255.255
#define MIN_ATSC3_MULTICAST_BLOCK (239 << 24 | 255 << 16)
#define MAX_ATSC3_MULTICAST_BLOCK (239 << 24 | 255 << 16 | 255 << 8 | 255)

//mDNS destination addr and port - filter this noise out
#define UDP_FILTER_MDNS_IP_ADDRESS 3758096635
#define UDP_FILTER_MDNS_PORT 5353

#if defined (__cplusplus)
extern "C" {
#endif


int is_big_endian(void);

long long timediff(struct timeval t1, struct timeval t0);
double gt();

//convert struct { uint32_t ip, uint16_t port} to bitsift representation
//printf format: //%u.%u.%u.%u:%u
#define __toip(packet) (packet->ip >> 24) & 0xFF, (packet->ip >> 16) & 0xFF, (packet->ip >> 8) & 0xFF,  (packet->ip) & 0xFF,  packet->port
#define __toipnonstruct(ip) (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, (ip) & 0xFF
#define __toipandportnonstruct(ip, port) (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, (ip) & 0xFF, port

#ifndef __MAX
#   define __MAX(a, b)   ( ((a) > (b)) ? (a) : (b) )
#endif
#ifndef __MIN
#   define __MIN(a, b)   ( ((a) < (b)) ? (a) : (b) )
#endif

/* clip v in [min, max] */
#define __CLIP(v, min, max)    __MIN(__MAX((v), (min)), (max))

void* extract(uint8_t *bufPosPtr, uint8_t *dest, int size);

//key=value or key="value" attribute par collection parsing and searching
typedef struct kvp {
	char* key;
	char* val;
} kvp_t;

typedef struct kvp_collection {
	kvp_t **kvp_collection;
	int 	size_n;
} kvp_collection_t;

kvp_collection_t* kvp_collection_parse(uint8_t* input_string);
//return the cloned value from the collection for datamodel construction
char* kvp_collection_get(kvp_collection_t *collection, char* key);
//return the reference pointer to the value
char* kvp_collection_get_reference_p(kvp_collection_t *collection, char* key);
void kvp_collection_free(kvp_collection_t* collection);

//or block_t as in VLC?
typedef struct atsc3_block {
	uint8_t* p_buffer;
	uint32_t p_size;
	uint32_t i_pos;
} block_t;

block_t* block_Alloc(int len);
block_t* block_Promote(char*);
block_t* block_Write(block_t* dest, uint8_t* buf, uint32_t size);
uint32_t block_Append(block_t* dest, block_t* src);
uint32_t block_Seek(block_t* block, int32_t seek_pos);
block_t* block_Rewind(block_t* dest);
block_t* block_Resize(block_t* dest, uint32_t dest_size_required);
block_t* block_Duplicate(block_t* a);
block_t* block_Duplicate_from_position(block_t* a);
void block_Release(block_t** a);

//alloc and copy - note limited to 16k
char* strlcopy(char*);
char *_ltrim(char *str);
char* _rtrim(char *str);
char* __trim(char *str);

void freesafe(void* tofree);
void freeclean(void** tofree);

uint32_t parseIpAddressIntoIntval(char* dst_ip);

uint16_t parsePortIntoIntval(char* dst_port);

/*
 * Concatenate preprocessor tokens A and B without expanding macro definitions
 * (however, if invoked from a macro, macro arguments are expanded).
 */
#define PPCAT_NX(A, B) A ## B

/*
 * Concatenate preprocessor tokens A and B after macro-expanding them.
 * e.g. e.g., PPCAT(s, 1) produces the identifier s1.
 *
 */
#define PPCAT(A, B) PPCAT_NX(A, B)
/*
 * Turn A into a string literal without expanding macro definitions
 * (however, if invoked from a macro, macro arguments are expanded).
 *
 * e..g.
 * #define T1 s
 * #define T2 1
 * STRINGIZE(PPCAT(T1, T2)) // produces "s1"
 *
 */
#define STRINGIZE_NX(A) #A

/*
 * Turn A into a string literal after macro-expanding it.
 */
#define STRINGIZE(A) STRINGIZE_NX(A)

#if defined (__cplusplus)
}
#endif


#define println(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __PRINTF(...)  printf(__VA_ARGS__);

#define _ATSC3_UTILS_PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define _ATSC3_UTILS_PRINTF(...)  printf(__VA_ARGS__);

#define _ATSC3_UTILS_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_UTILS_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_UTILS_INFO(...)    if(_ATSC3_UTILS_INFO_ENABLED) { printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__); }
#define _ATSC3_UTILS_DEBUG(...)   if(_ATSC3_UTILS_DEBUG_ENABLED) { printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__); }

#define _ATSC3_UTILS_TRACE(...)   if(_ATSC3_UTILS_TRACE_ENABLED) { printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__); }
#define _ATSC3_UTILS_TRACEF(...)  if(_ATSC3_UTILS_TRACE_ENABLED) { printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTF(__VA_ARGS__); }
#define _ATSC3_UTILS_TRACEA(...)  if(_ATSC3_UTILS_TRACE_ENABLED) { _ATSC3_UTILS_PRINTF(__VA_ARGS__); }
#define _ATSC3_UTILS_TRACEN(...)  if(_ATSC3_UTILS_TRACE_ENABLED) { _ATSC3_UTILS_PRINTLN(__VA_ARGS__); }

#endif /* ATSC3_UTILS_H_ */
