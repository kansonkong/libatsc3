/*------------------------------------------------------------------------------
  Copyright 2014-2015 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/
/**
 @file  sony_cxd_common.h

 @brief Common definitions and functions used in all reference code.

        The user should modify several points depend on their software system.
        Please check "<PORTING>" comments.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_COMMON_H
#define sony_cxd_COMMON_H


#if 0
/* Type definitions. */
/* <PORTING> Please comment out if conflicted */
#ifdef __linux__
#include <stdint.h>
#else
typedef unsigned char uint8_t;      /**< Unsigned  8 bit integer. */
typedef unsigned short uint16_t;    /**< Unsigned 16 bit integer. */
//typedef unsigned int uint32_t;      /**< Unsigned 32 bit integer. */
//typedef signed char int8_t;         /**< Signed    8 bit integer. */
typedef signed short int16_t;       /**< Signed   16 bit integer. */
//typedef signed int int32_t;         /**< Signed   32 bit integer. */
#endif
#endif


#include <sys/time.h>

#define do_gettimeofday(tv) gettimeofday(tv, NULL);

//FFS!

#include <stdint.h>

//#include <stdint.h> //mark by Sean 201902
//#include <linux/types.h>



#ifndef NULL
#ifdef __cplusplus
/** Null pointer */
#define NULL 0
#else
/** Null pointer */
#define NULL ((void*)0)
#endif
#endif

#if 1
/* <PORTING> Sleep function define (Please modify as you like) */
#if defined(_WINDOWS)
#include "sony_cxd_windows_sleep.h"
#define sony_cxd_SLEEP(n) sony_cxd_windows_Sleep(n)
#elif defined(__linux__)
//#include <linux/unistd.h>
//#define sony_cxd_SLEEP(n) msleep(n)
#include "brUser.h"
#define sony_cxd_SLEEP(n) BrUser_delay(NULL,n)
#endif
#endif

#if 0
#include "sony_cxd_windows_sleep.h"
#define sony_cxd_SLEEP(n) sony_cxd_windows_Sleep(n)
#endif

#ifndef sony_cxd_SLEEP
#error sony_cxd_SLEEP(n) is not defined. This macro must be ported to your platform.
#endif

/*
  <PORTING> Sleep macro used in monitor functions.
  Monitor functions are used while TS is output.
  So normally, if SPI/SDIO is used, monitoring thread
  and TS handling thread are both run.
  The user should use mutual exclusion methods to use all APIs
  in this driver atomically if multiple threads call driver API.
  So, sleep in monitor thread can block TS handling thread.

  To avoid such situation,
  sony_cxd_SLEEP_IN_MONITOR is used instead of sony_cxd_SLEEP in monitor function codes.
  The user can modify sony_cxd_SLEEP_IN_MONITOR macro like as follows to avoid
  blocking TS handling thread.

  1. Leave critical section (unlock mutex, release semaphore).
  2. Sleep (TS handling thread can work in this timing.)
  3. Enter critical section (lock mutex, get semaphore).
*/
#ifndef sony_cxd_SLEEP_IN_MONITOR
/** Default sleep macro definition in monitor functions. */
#define sony_cxd_SLEEP_IN_MONITOR(n, obj) sony_cxd_SLEEP(n)
#endif

/* <PORTING> Trace function enable */
/* #define sony_cxd_TRACE_ENABLE */
/* <PORTING> Enable if I/O related function trace is necessary */
/* #define sony_cxd_TRACE_IO_ENABLE */

/**
 @brief *PORTING* Macro to specify unused argument and suppress compiler warnings.
*/
#define sony_cxd_ARG_UNUSED(arg) ((void)(arg))

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Return codes
*/
typedef enum {
    sony_cxd_RESULT_OK,              /**< Function was successfully actioned. */
    sony_cxd_RESULT_ERROR_ARG,       /**< Invalid argument (maybe software bug). */
    sony_cxd_RESULT_ERROR_IO,        /**< I/O error. */
    sony_cxd_RESULT_ERROR_SW_STATE,  /**< Invalid software state. */
    sony_cxd_RESULT_ERROR_HW_STATE,  /**< Invalid hardware state. */
    sony_cxd_RESULT_ERROR_TIMEOUT,   /**< Timeout occurred. */
    sony_cxd_RESULT_ERROR_UNLOCK,    /**< Failed to lock. */
    sony_cxd_RESULT_ERROR_RANGE,     /**< Out of range. */
    sony_cxd_RESULT_ERROR_NOSUPPORT, /**< Not supported for current device. */
    sony_cxd_RESULT_ERROR_CANCEL,    /**< The operation is cancelled. */
    sony_cxd_RESULT_ERROR_OTHER,     /**< Unspecified error. */
    sony_cxd_RESULT_ERROR_OVERFLOW,  /**< Memory overflow. */
    sony_cxd_RESULT_OK_CONFIRM       /**< Tune was successful, but confirm parameters. */
} sony_cxd_result_t;

/*------------------------------------------------------------------------------
  Common functions
------------------------------------------------------------------------------*/
/**
 @brief Convert unsigned integer to signed integer.

 @param value Unsigned integer value.
 @param bitlen The bit width of "value".

 @return The signed integer value.
*/
int32_t sony_cxd_Convert2SComplement(uint32_t value, uint32_t bitlen);

/**
 @brief Split bit from byte array data.

 @param pArray The byte array data.
 @param startBit The start bit position.
 @param bitNum The bit length.

 @return The data split from byte array data.
*/
uint32_t sony_cxd_BitSplitFromByteArray(uint8_t * pArray, uint32_t startBit, uint32_t bitNum);

/*------------------------------------------------------------------------------
  Trace
------------------------------------------------------------------------------*/
/*
 Disables MS compiler warning (__pragma(warning(disable:4127))
 with do { } while (0);
*/
/** Macro for beginning multiline macro. */
#define sony_cxd_MACRO_MULTILINE_BEGIN  do {
#if ((defined _MSC_VER) && (_MSC_VER >= 1300))
#define sony_cxd_MACRO_MULTILINE_END \
        __pragma(warning(push)) \
        __pragma(warning(disable:4127)) \
        } while(0) \
        __pragma(warning(pop))
#else
/** Macro for ending multiline macro. */
#define sony_cxd_MACRO_MULTILINE_END } while(0)
#endif


#ifdef sony_cxd_TRACE_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
void sony_cxd_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void sony_cxd_trace_log_return(sony_cxd_result_t result, const char* filename, unsigned int linenum);
#define sony_cxd_TRACE_ENTER(func) sony_cxd_trace_log_enter((func), __FILE__, __LINE__)
#define sony_cxd_TRACE_RETURN(result) \
    sony_cxd_MACRO_MULTILINE_BEGIN \
        sony_cxd_trace_log_return((result), __FILE__, __LINE__); \
        return (result); \
    sony_cxd_MACRO_MULTILINE_END
#else /* sony_cxd_TRACE_ENABLE */
/** Trace enter */
#define sony_cxd_TRACE_ENTER(func)
/** Trace return */
#define sony_cxd_TRACE_RETURN(result) return(result)
/** I/O trace enter */
#define sony_cxd_TRACE_IO_ENTER(func)
/** I/O trace return */
#define sony_cxd_TRACE_IO_RETURN(result) return(result)
#endif /* sony_cxd_TRACE_ENABLE */


#ifdef sony_cxd_TRACE_IO_ENABLE
/* <PORTING> This is only a sample of trace macro. Please modify is necessary. */
void sony_cxd_trace_io_log_enter(const char* funcname, const char* filename, unsigned int linenum);
void sony_cxd_trace_io_log_return(sony_cxd_result_t result, const char* filename, unsigned int linenum);
#define sony_cxd_TRACE_IO_ENTER(func) sony_cxd_trace_io_log_enter((func), __FILE__, __LINE__)
#define sony_cxd_TRACE_IO_RETURN(result) \
    sony_cxd_MACRO_MULTILINE_BEGIN \
        sony_cxd_trace_io_log_return((result), __FILE__, __LINE__); \
        return (result); \
    sony_cxd_MACRO_MULTILINE_END
#else /* sony_cxd_TRACE_IO_ENABLE */
#define sony_cxd_TRACE_IO_ENTER(func)
#define sony_cxd_TRACE_IO_RETURN(result) return(result)
#endif /* sony_cxd_TRACE_IO_ENABLE */


/*------------------------------------------------------------------------------
  Multi-threaded defines
 ------------------------------------------------------------------------------*/
/**
 @brief "<PORTING>" Defines for basic atomic operations for cancellation. 
*/
typedef struct sony_cxd_atomic_t {
    /**
     @brief Underlying counter.
    */
    volatile int counter;
} sony_cxd_atomic_t;
#define sony_cxd_atomic_set(a,i) ((a)->counter = i)   /**< Set counter atomically. */
#define sony_cxd_atomic_read(a) ((a)->counter)        /**< Get counter atomically. */

/*------------------------------------------------------------------------------
  Stopwatch struct and functions definitions
------------------------------------------------------------------------------*/
/**
 @brief "<PORTING>" Stopwatch structure to measure accurate time.
*/
typedef struct sony_cxd_stopwatch_t {
    /**
     @brief Underlying start time.
    */
    uint32_t startTime;

} sony_cxd_stopwatch_t;

/**
 @brief Start the stopwatch.

 @param pStopwatch The stopwatch instance.

 @return sony_cxd_RESULT_OK is successful.
*/
sony_cxd_result_t sony_cxd_stopwatch_start (sony_cxd_stopwatch_t * pStopwatch);

/**
 @brief Pause for a specified period of time.

 @param pStopwatch The stopwatch instance.
 @param ms         The time in milliseconds to sleep.

 @return sony_cxd_RESULT_OK is successful.
*/
sony_cxd_result_t sony_cxd_stopwatch_sleep (sony_cxd_stopwatch_t * pStopwatch, uint32_t ms);

/**
 @brief Returns the elapsed time (ms) since the stopwatch was started.

 @param pStopwatch The stopwatch instance.
 @param pElapsed   The elapsed time in milliseconds.

 @return sony_cxd_RESULT_OK is successful.
*/
sony_cxd_result_t sony_cxd_stopwatch_elapsed (sony_cxd_stopwatch_t * pStopwatch, uint32_t * pElapsed);

#endif /* sony_cxd_COMMON_H */
