#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

#include "dibconfig.h" 


/* include specific parts */
#ifdef _CVI_

 #include <platform/platform_cvi.h>
 #ifdef CONFIG_STANDARD_ANALOG
  #include "analog.h"
 #endif

#elif defined _WIN32

 #ifdef DIB_KERNEL_MODE

  #include "dibplatform_windows_kernel.h"
  #ifdef CONFIG_STANDARD_ANALOG
   #include "analog_windows_kernel.h"
  #endif

 #else

  #include "dibplatform_windows_user.h"
  #ifdef CONFIG_STANDARD_ANALOG
   #include "analog_windows_user.h"
  #endif

 #endif

#elif defined(__sparc__) || defined(__sparc)

 #include <platform/platform_leon.h>

#elif defined __unix__

 #ifndef __KERNEL__
  #include "dibplatform_linux.h"
 #else
  #include "dibplatform_linux_kernel.h"
 #endif

 #ifdef CONFIG_STANDARD_ANALOG
  #include "analog.h"
 #endif

#endif

/* define a INFO_TEXT in your platform.h and get rid of the strings */
#ifndef INFO_TEXT
#define INFO_TEXT(x) x
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if defined(__BYTE_ORDER)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define from_big_endian16(v) bswap_16(v)
#define from_big_endian32(v) bswap_32(v)
#define from_little_endian16(v) (v)
#define from_little_endian32(v) (v)
#else
#define from_big_endian16(v) (v)
#define from_big_endian32(v) (v)
#define from_little_endian16(v) bswap_16(v)
#define from_little_endian32(v) bswap_32(v)
#endif
#else
static inline uint16_t from_big_endian16(uint16_t v)
{
	uint16_t w=0x1;
	/* little endian */
	if (*(uint8_t *)&w == 1)
		return (((v>>8)&0xff) | ((v&0xff)<<8));
	/* big endian */
	return v;
}

static inline uint32_t from_big_endian32(uint32_t v)
{
	uint16_t w=0x1;
	/* little endian */
	if (*(uint8_t *)&w == 1)
		return ((v&0xff000000)>>24) | ((v&0x00ff0000)>>8) | ((v&0x0000ff00)<<8) | ((v&0x000000ff)<<24);
	/* big endian */
	return v;
}
#endif

#define REQUEST_FIRMWARE_OPTION_MASK         0xF0000000

/*
extern
int platform_request_firmware(const char *name,
                              int options,
                              int size,
                              int * off,
                              int (*platform_firmware_indication)(void * ctx, uint8_t * buf, uint32_t len, uint32_t off, uint32_t done),
                              void *ctx);
*/


#ifdef __cplusplus
}
#endif


#endif
