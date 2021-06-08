/*
 * atsc3_utils.h
 *
 *  Created on: Jan 6, 2019
 *      Author: jjustman
 */
#ifndef _WIN32
#pragma GCC diagnostic ignored "-Wformat-zero-length"
#endif

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
#include <time.h>

#ifndef _WIN32
#include <strings.h>
#include <sys/time.h>
#include <libgen.h>
#include <semaphore.h>
#else
#include <winsock2.h>
#include <Windows.h>
#include <semaphore.h>
#ifndef F_OK 
#define F_OK 0
#endif
#endif


#include <stdbool.h>
#include <sys/stat.h>

//jjustman-2021-02-16 - not needed? basename/dirname methods #include <libgen.h>
#include <limits.h>

#ifndef ATSC3_UTILS_H_
#define ATSC3_UTILS_H_

#include "fixups.h"

#ifndef _WIN32
#include "unistd.h"
#endif

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

#ifndef ntohq
#ifndef ntohll
#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )
#endif
#ifndef htonll
#define htonll(x) ntohll(x)
#endif
//jjustman-2020-11-24 - wrap this for android/bionic 64bit network to host
#define ntohq(x) ntohll(x)

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

	bool     _overflow_i_pos;

    uint8_t  _bitpos;
    uint8_t  _refcnt;
    uint8_t  _is_alloc;
    uint32_t _a_size;
} block_t;


#define block_Refcount(a) ({ if(a) { _block_Refcount(a);  \
    _ATSC3_UTILS_TRACE("UTRACE:INCR:%p:%s, block_Refcount: incrementing to: %d, block: %p (p_buffer: %p)", a, __FUNCTION__, a->_refcnt, a, a->p_buffer); } \
    a; })
    
    
#define ATSC3_UTILS_GET_ALIGNED_SIZE_FOR_CALLOC(size_requested)  size_requested ? size_requested + 16 + (8 - (size_requested %8))    :    8;


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
uint8_t  block_Read_uint8_bitlen(block_t* src, int bitlen);
uint16_t block_Read_uint16_bitlen(block_t* src, int bitlen);
uint32_t block_Read_uint32_bitlen(block_t* src, int bitlen);
uint64_t block_Read_uint64_bitlen(block_t* src, int bitlen);

//read from network to host aligned short/long/double long
uint8_t  block_Read_uint8(block_t* src);
uint8_t* block_Read_uint8_varlen(block_t* src, uint32_t varlen); //for var length uint8_t* blocks, callee will be owner of memory calloc
uint16_t block_Read_uint16_ntohs(block_t* src);
uint32_t block_Read_uint32_ntohl(block_t* src);
uint64_t block_Read_uint64_ntohll(block_t* src);

//read from filesystem into block_t
block_t* block_Read_from_filename(const char* file_name);
int	block_Write_to_filename(block_t* src, const char* file_name);

#define block_RefZero(a) ({ a->_refcnt = 0; })
#define block_Release(a) ({ _ATSC3_UTILS_TRACE("UTRACE:DECR:%p:%s, block_Refcount: decrementing to: %d, block: %p (p_buffer: %p)", *a, __FUNCTION__, (*a->_refcnt)-1, *a, *a->p_buffer);  _block_Release(a); })

void _block_Release(block_t** a); //_refcnt MUST == 0 for p_buffer to be freed, see block_Refcount
void _block_Refcount(block_t* a);

void block_Destroy(block_t** a); //hard destroy overriding GC
    
//alloc and copy - note limited to 16k
char* strlcopy(const char*);
char *_ltrim(char *str);
char* _rtrim(char *str);
char* __trim(char *str);
bool str_is_utf8(const char* str);

void freesafe(void* tofree);

void freeclean(void** tofree);
void freeclean_uint8_t(uint8_t** tofree);

uint32_t parseIpAddressIntoIntval(const char* dst_ip);

uint16_t parsePortIntoIntval(const char* dst_port);

int mkpath(char *dir, mode_t mode);

//see also atsc3_mmtp_ntp32_to_pts
uint64_t compute_seconds_microseconds_to_scalar64(uint32_t seconds, uint32_t microseconds);

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

//jjustman-2020-09-01 win32 fixups

#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#include <direct.h>
#include <Shlwapi.h>
#include <io.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Ws2_32.lib")


//hack for non exported method linkage
static void usleep(int waitTime) {
    __int64 time1 = 0, time2 = 0, freq = 0;

    QueryPerformanceCounter((LARGE_INTEGER*)&time1);
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

    do {
        QueryPerformanceCounter((LARGE_INTEGER*)&time2);
    } while ((time2 - time1) < waitTime);
}

static char* strcasestr(const char* first, const char* search) {
    return StrStrIA(first, search);
}

static char* strndup(const char* s, size_t n)
{

    char* x = NULL;

    if (n + 1 < n) {
        return NULL;
    }

    x = (char*) malloc(n + 1);
    if (x == NULL) {
        return NULL;
    }

    memcpy(x, s, n);
    x[n] = '\0';

    return x;
}

#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#define ftruncate _chsize



#include < time.h >
#include < windows.h >

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

#ifndef _TIMEZONE_DEFINED /* also in sys/time.h */
#define _TIMEZONE_DEFINED

struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

inline int gettimeofday(struct timeval* tv, struct timezone* tz)
{
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag = 0;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        tmpres /= 10;  /*convert into microseconds*/
        /*converting file time to unix epoch*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        long my_tz;
        _get_timezone(&my_tz);

        int my_daylight;
        _get_daylight(&my_daylight);

        tz->tz_minuteswest = my_tz / 60;
        tz->tz_dsttime = my_daylight;
    }

    return 0;
}
#endif

/* Windows sleep in 100ns units */
static BOOLEAN nanosleep(LONGLONG ns) {
    /* Declarations */
    HANDLE timer;	/* Timer handle */
    LARGE_INTEGER li;	/* Time defintion */
    /* Create timer */
    if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
        return FALSE;
    /* Set timer properties */
    li.QuadPart = -ns;
    if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)) {
        CloseHandle(timer);
        return FALSE;
    }
    /* Start & wait for timer */
    WaitForSingleObject(timer, INFINITE);
    /* Clean resources */
    CloseHandle(timer);
    /* Slept without problems */
    return TRUE;
}


//https://github.com/ivanrad/getline/blob/master/getline.c
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

static ssize_t getdelim(char** lineptr, size_t* n, int delim, FILE* stream) {
    char* cur_pos, * new_lineptr;
    size_t new_lineptr_len;
    int c;

    if (lineptr == NULL || n == NULL || stream == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (*lineptr == NULL) {
        *n = 128; /* init len */
        if ((*lineptr = (char*)malloc(*n)) == NULL) {
            errno = ENOMEM;
            return -1;
        }
    }

    cur_pos = *lineptr;
    for (;;) {
        c = getc(stream);

        if (ferror(stream) || (c == EOF && cur_pos == *lineptr))
            return -1;

        if (c == EOF)
            break;

        if ((*lineptr + *n - cur_pos) < 2) {
            if (MAXSSIZE_T / 2 < *n) {
#ifdef EOVERFLOW
                errno = EOVERFLOW;
#else
                errno = ERANGE; /* no EOVERFLOW defined */
#endif
                return -1;
            }
            new_lineptr_len = *n * 2;

            if ((new_lineptr = (char*)realloc(*lineptr, new_lineptr_len)) == NULL) {
                errno = ENOMEM;
                return -1;
            }
            cur_pos = new_lineptr + (cur_pos - *lineptr);
            *lineptr = new_lineptr;
            *n = new_lineptr_len;
        }

        *cur_pos++ = (char)c;

        if (c == delim)
            break;
    }

    *cur_pos = '\0';
    return (ssize_t)(cur_pos - *lineptr);
}

static ssize_t getline(char** lineptr, size_t* n, FILE* stream) {
    return getdelim(lineptr, n, '\n', stream);
}


#endif

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
