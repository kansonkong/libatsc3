/*
 * atsc3_utils.h
 *
 *  Created on: Jan 6, 2019
 *      Author: jjustman
 */
#pragma GCC diagnostic ignored "-Wformat-zero-length"

#ifdef __LIBATSC3_ANDROID__
#include <sys/endian.h>
#endif

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
#include <stdbool.h>
#include <sys/stat.h>

#include <libgen.h>


#ifndef ATSC3_UTILS_H_
#define ATSC3_UTILS_H_

#include "fixups.h"
#include "unistd.h"
#include "endianess.c"

#include "atsc3_logging_externs.h"

#define uS 1000000ULL

//ATSC3/331 Section 6.1 - drop non mulitcast ip ranges - e.g not in  239.255.0.0 to 239.255.255.255
#define MIN_ATSC3_MULTICAST_BLOCK (239 << 24 | 255 << 16)
#define MAX_ATSC3_MULTICAST_BLOCK (239 << 24 | 255 << 16 | 255 << 8 | 255)

/*
	A331/2020 Section 6.2 defines this value as 65,507, from the following calculation in footnote 5:
 
	The maximum size of the IP datagram is 65,535 bytes. The maximum UDP data payload is 65,535 minus 20 bytes for the IP header
	minus 8 bytes for the UDP header.
*/
#define MAX_ATSC3_PHY_IP_DATAGRAM_SIZE 65535
#define MAX_ATSC3_PHY_ALP_DATA_PAYLOAD_SIZE 65507

#define MAX_ATSC3_ETHERNET_PHY_FRAME_LENGTH 1518

//mDNS destination addr and port - filter this noise out
#define UDP_FILTER_MDNS_IP_ADDRESS 3758096635
#define UDP_FILTER_MDNS_PORT 5353

#if defined (__cplusplus)
extern "C" {
#endif


int is_big_endian(void);

long long timediff(struct timeval t1, struct timeval t0);
double gt();
long gtl();

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

#ifndef ABS
#define ABS(x)           (((x) < 0) ? -(x) : (x))
#endif

/* clip v in [min, max] */
#define __CLIP(v, min, max)    __MIN(__MAX((v), (min)), (max))

#define __readuint32(data, hdrlen) (data[hdrlen] & 0xFF) << 24 | (data[hdrlen+1] & 0xFF) << 16 | (data[hdrlen+2] & 0xFF) << 8 | (data[hdrlen+3] & 0xFF);


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

typedef struct atsc3_block {
	uint8_t* p_buffer;
	uint32_t p_size;
	uint32_t i_pos;
    uint8_t  _bitpos;
    uint8_t  _refcnt;
    uint8_t  _is_alloc;
    uint32_t _a_size;
} block_t;


#define block_Refcount(a) ({ if(a) { _block_Refcount(a);  \
    _ATSC3_UTILS_TRACE("UTRACE:INCR:%p:%s, block_Refcount: incrementing to: %d, block: %p (p_buffer: %p)", a, __FUNCTION__, a->_refcnt, a, a->p_buffer); } \
    a; })
    
    
//block_t* _block_Refcount(block_t*); //used for sharing pointers between ref's
block_t* block_Alloc(int len);
bool block_IsAlloc(block_t*);
block_t* block_Promote(const char*);
block_t* block_Write(block_t* dest, const uint8_t* buf, uint32_t size); //write starting at i_pos
uint32_t block_Append(block_t* dest, block_t* src); //combine two blocks at i_pos, len: src->i_pos, return end position
block_t* block_AppendFromSrciPos(block_t* dest, block_t* src); //combine two blocks at dest->i_pos, block_Get(src), len: src->p_size - src->i_pos
block_t* block_AppendFromBuf(block_t* dest, const uint8_t* src_buf, uint32_t src_size); //write buffer to block starting a tp_size
uint32_t block_AppendFull(block_t* dest, block_t* src); //combine two blocks, dest at i_pos and full size of src p_size
uint32_t block_Merge(block_t* dest, block_t* src); //combine two blocks from p_size, p_size, return new merged p_size,
uint32_t block_MergeNoRewind(block_t* dest, block_t* src);
uint32_t block_Seek(block_t* block, int32_t seek_pos);
uint32_t block_Seek_Relative(block_t* block, int32_t relative_pos);
block_t* block_Rewind(block_t* dest);
block_t* block_Resize(block_t* dest, uint32_t dest_size_required);
block_t* block_Resize_Soft(block_t* dest, uint32_t dest_size_min_required); //perform a soft allocation to src->p_size * 2  where p_size < 2M
block_t* block_Duplicate(block_t* a);
block_t* block_Duplicate_from_position(block_t* a);
block_t* block_Duplicate_to_size(block_t* src, uint32_t target_len);
block_t* block_Duplicate_from_ptr(uint8_t* data, uint32_t size); //src
uint32_t block_Remaining_size(block_t* src);
bool block_Valid(block_t* src);
uint8_t* block_Get(block_t* src);
uint32_t block_Len(block_t* src);
bool block_Tail_Truncate(block_t* src, uint32_t len);

//bit-unpacking functions for parsing A/322 variable length L1(b/d) structs
uint8_t block_Read_uint8_bitlen(block_t* src, int bitlen);
uint16_t block_Read_uint16_bitlen(block_t* src, int bitlen);
uint32_t block_Read_uint32_bitlen(block_t* src, int bitlen);
uint64_t block_Read_uint64_bitlen(block_t* src, int bitlen);

//read from network to host aligned short/long/double long
uint16_t block_Read_uint16_ntohs(block_t* src);
uint32_t block_Read_uint32_ntohl(block_t* src);
uint64_t block_Read_uint64_ntohul(block_t* src);

//read from filesystem into block_t
block_t* block_Read_from_filename(char* file_name);


#define block_RefZero(a) ({ a->_refcnt = 0; })
#define block_Release(a) ({ _ATSC3_UTILS_TRACE("UTRACE:DECR:%p:%s, block_Refcount: decrementing to: %d, block: %p (p_buffer: %p)", *a, __FUNCTION__, (*a->_refcnt)-1, *a, *a->p_buffer);  _block_Release(a); })

void _block_Release(block_t** a); //_refcnt MUST == 0 for p_buffer to be freed, see block_Refcount
void _block_Refcount(block_t* a);

void block_Destroy(block_t** a); //hard destroy overriding GC
    
//alloc and copy - note limited to 16k
char* strlcopy(char*);
char *_ltrim(char *str);
char* _rtrim(char *str);
char* __trim(char *str);

void freesafe(void* tofree);

void freeclean(void** tofree);
void freeclean_uint8_t(uint8_t** tofree);

uint32_t parseIpAddressIntoIntval(const char* dst_ip);

uint16_t parsePortIntoIntval(const char* dst_port);

int mkpath(char *dir, mode_t mode);


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

//check to see if file exists on disk, return FILE* or NULL
FILE* atsc3_object_open(char* file_name);

#if defined (__cplusplus)
}
#endif

//global logging hooks
#define __ATSC3_ERROR(...)	__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __ATSC3_WARN(...) 	__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __ATSC3_INFO(...) 	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);
#define __ATSC3_DEBUG(...) 	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__);
#define __ATSC3_TRACE(...) 	__LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__);

//local logging hooks
#define _ATSC3_UTILS_ERROR(...)  									__LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_UTILS_WARN(...)    									__LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_UTILS_INFO(...)    if(_ATSC3_UTILS_INFO_ENABLED)  { 	__LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define _ATSC3_UTILS_DEBUG(...)   if(_ATSC3_UTILS_DEBUG_ENABLED) { 	__LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_UTILS_TRACE(...)   if(_ATSC3_UTILS_TRACE_ENABLED) {	__LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }


//todo: flatten
#define println(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define __PRINTF(...)  printf(__VA_ARGS__);
#define _ATSC3_UTILS_PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define _ATSC3_UTILS_PRINTF(...)  printf(__VA_ARGS__);
#define _ATSC3_UTILS_TRACEF(...)  if(_ATSC3_UTILS_TRACE_ENABLED) { printf("%s:%d:TRACE:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTF(__VA_ARGS__); }
#define _ATSC3_UTILS_TRACEA(...)  if(_ATSC3_UTILS_TRACE_ENABLED) { _ATSC3_UTILS_PRINTF(__VA_ARGS__); }
#define _ATSC3_UTILS_TRACEN(...)  if(_ATSC3_UTILS_TRACE_ENABLED) { _ATSC3_UTILS_PRINTLN(__VA_ARGS__); }

#endif /* ATSC3_UTILS_H_ */
