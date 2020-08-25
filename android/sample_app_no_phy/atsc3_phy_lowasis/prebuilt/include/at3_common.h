/*
	at3_common.h

	for Project Atlas

	Define common tools

	Digital STREAM Labs, Inc. 2017
	Copyright © 2017, 2018, 2019 LowaSIS, Inc.
*/

#ifndef __AT3_COMMON_H__
#define __AT3_COMMON_H__

//============================================================================

#define __STDC_FORMAT_MACROS
	// this should be defined before inttypes.h inclusion for use PRIu64, PRIx64, ...


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <memory.h>
#include <inttypes.h>
#if defined(_WIN32)
/*
	그냥 #include <Windows.h> 하면 내부에서 winsock.h 를 포함시키므로
	그보다 더 앞서서 winsock2.h 를 포함시키는 것이 좋다.

	ws2def.h 에 있는 문구 참고.
	Do not include winsock.h and ws2def.h in the same module. Instead include only winsock2.h.

	헤더를 잘못 포함하는 경우 보통 다음과 같은 충돌이 나게 됨.
	...\Windows Kits\10\Include\10.0.16299.0\shared\ws2def.h(103): warning C4005: 'AF_IPX': macro redefinition
	...\Windows Kits\10\Include\10.0.16299.0\um\winsock.h(457): note: see previous definition of 'AF_IPX'
*/
#include <winsock2.h>
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __cplusplus
  #include <string>
  #include <vector>
#endif

#include "at3_types.h"
// #include "at3drv_version.h"


//============================================================================
// top-level configs


/*
	enable AT3_MODULE and dprint macro.
	without this, dprint only print simple messages.
*/
#define AT3CFG_ENABLE_DBGLOG 1





//============================================================================

/*
	linkage type of public api
*/

#if defined(_WIN32)
#ifdef __cplusplus
	#define AT3STDAPI extern "C" __declspec(dllexport)
#else
	#define AT3STDAPI __declspec(dllexport)
#endif
#else
#ifdef __cplusplus
	#define AT3STDAPI extern "C"
#else
	#define AT3STDAPI 
#endif
#endif


//============================================================================
// for platform-specific codes..
// 
// refer
// https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
//
// here, i only shows how you can use the macros.
//
#ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif

//#elif __APPLE__ // not supported yet

#elif __linux__
	// linux
	// warning! android platform also defines __linux__ macro.
	// so you should check ANDROID first.
	typedef int HANDLE;
	#define INVALID_HANDLE_VALUE ((HANDLE)(long)-1)

  #if defined(ANDROID)
	// android ndk-build automatically defines ANDROID
	// __ANDROID__ is defined by gcc and clang compiler in android toolchain
  #endif

#else
	#error unknown platform!
#endif


//============================================================================
// 

#if defined(__GNUC__)
  #define __AT3_ATTRIB(arch, str, first) __attribute__ ((format (arch, str, first)))
#else
  #define __AT3_ATTRIB(arch, str, first)
#endif



//============================================================================
// 
// ref: https://stackoverflow.com/questions/3378560/how-to-disable-gcc-warnings-for-a-few-lines-of-code

#define DIAG_STR(s) #s
#define DIAG_JOINSTR(x,y) DIAG_STR(x ## y)

#ifdef _MSC_VER
  #define DIAG_DO_PRAGMA(x) __pragma (x)
  #define DIAG_PRAGMA(compiler,x) DIAG_DO_PRAGMA(warning(x))
#else
  #define DIAG_DO_PRAGMA(x) _Pragma (#x)
  #define DIAG_PRAGMA(compiler,x) DIAG_DO_PRAGMA(compiler diagnostic x)
#endif

#if defined(__clang__)
  #define __AT3_HideWarning(gccx,clang_option,msvcx) DIAG_PRAGMA(clang,push) DIAG_PRAGMA(clang,ignored DIAG_JOINSTR(-W,clang_option))
  #define __AT3_ShowWarning(gccx,clang_option,msvcx) DIAG_PRAGMA(clang,pop)
  // _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wname_of_warning\"")
  // _Pragma("clang diagnostic pop")
#elif defined(_MSC_VER)
  #define __AT3_HideWarning(gccx,clangx,msvc_errorcode) DIAG_PRAGMA(msvc,push) DIAG_DO_PRAGMA(warning(disable:##msvc_errorcode))
  #define __AT3_ShowWarning(gccx,clangx,msvc_errorcode) DIAG_PRAGMA(msvc,pop)
  // __pragma(warning(push)) __pragma(warning(disable:1234))
  // __pragma(warning(pop))
#elif defined(__GNUC__)
  #if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
    #define __AT3_HideWarning(gcc_option,clangx,msvcx) DIAG_PRAGMA(GCC,push) DIAG_PRAGMA(GCC,ignored DIAG_JOINSTR(-W,gcc_option))
    #define __AT3_ShowWarning(gcc_option,clangx,msvcx) DIAG_PRAGMA(GCC,pop)
    // _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wname_of_warning\"")
    // _Pragma("GCC diagnostic pop")
  #else
    #define __AT3_HideWarning(gcc_option,clangx,msvcx) DIAG_PRAGMA(GCC,ignored DIAG_JOINSTR(-W,gcc_option))
    #define __AT3_ShowWarning(gcc_option,clang_option,msvcx) DIAG_PRAGMA(GCC,warning DIAG_JOINSTR(-W,gcc_option))
    // _Pragma("GCC diagnostic ignored \"-Wname_of_warning\"")
    // _Pragma("GCC diagnostic warning \"-Wname_of_warning\"")
  #endif
#endif


//============================================================================
// Notes about integer data types

/*
	64-bit x64 gcc (linux):
		sizeof(int)==4, sizeof(long)==8, sizeof(long long)==8, sizeof(void *)==8

	64-bit x64 msvc (windows):
		sizeof(int)==4, sizeof(long)==4, sizeof(long long)==8, sizeof(void *)==8
		https://msdn.microsoft.com/ko-kr/library/s3f49ktz.aspx

	32-bit arm7 clang: TODO
	64-bit arm64 clang: TODO

	32-bit mipsel gcc:
		sizeof(int)==4, sizeof(long)==4, sizeof(long long)==8, sizeof(void *)==4
*/

//============================================================================

#define AT3_MIN(a,b) ((a)<(b)?(a):(b))
#define AT3_MAX(a,b) ((a)>(b)?(a):(b))
#define AT3_ABS(a)   ((a)<0?-(a):(a))

// return true if x is in range (mn, mx) inclusive
#define AT3_INRANGE(x,mn,mx) ( (x)>=(mn) && (x)<=(mx) )

// number elemement in array
#define AT3_NELM(a) (sizeof(a) / sizeof((a)[0]))

// swap
#define AT3_SWAP(a,b,tmp) do { (tmp)=(a); (a)=(b); (b)=(tmp); } while(0)

// macro to delete object in safe and easy manner.
// 'obj' should be pointer of some class object.
#define AT3_SAFE_DELETE(obj) \
	do { if (obj) { delete (obj); (obj) = NULL; } } while(0)

#define AT3_SAFE_DELETE_MSG(obj, msg) \
	do { if (obj) { \
			if (msg) dprint(2, "delete %s..\n", msg); \
			delete (obj); (obj) = NULL; \
	} } while(0)

#define AT3_SAFE_DELETE_MSGF(obj, fmt, ...) \
	do { if (obj) { \
			dprint(2, fmt, ## __VA_ARGS__); \
			delete (obj); (obj) = NULL; \
	} } while(0)

#define AT3_SAFE_FREE(ptr) \
	do { if (ptr) { free(ptr); (ptr) = NULL; } } while(0)

// bugfix! free -> delete
#define AT3_SAFE_FREE_MSG(ptr, msg) \
	do { if (ptr) { \
			if (msg) dprint(2, "free %s..\n", msg); \
			free (ptr); (ptr) = NULL; \
	} } while(0)

#define AT3_SAFE_FREE_MSGF(ptr, fmt, ...) \
	do { if (ptr) { \
			dprint(2, fmt, ## __VA_ARGS__); \
			free (ptr); (ptr) = NULL; \
	} } while(0)

#define _1M 0x100000  // bugfix!
#define _1K 0x400


#if defined(_WIN32)
	// msvc cmd 도스창 에서 color code 가 제대로 표시 안되므로 사용하지 않도록 함.
	// powershell 의 경우는 TODO..
	#define KRED  ""
	#define KGRN  ""
	#define KYEL  ""
	#define KMAG  ""
	#define KCYN  ""
	#define KNRM  ""
#else
	// for printing colored-text, refer below:
	// https://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
	#define KRED  "\x1B[31;1m"
	#define KGRN  "\x1B[32;1m"
	#define KYEL  "\x1B[33;1m"
	#define KMAG  "\x1B[35;1m"
	#define KCYN  "\x1B[36;1m"
	#define KNRM  "\x1B[0m"
#endif // color code
	

#endif // __AT3_COMMON_H__
