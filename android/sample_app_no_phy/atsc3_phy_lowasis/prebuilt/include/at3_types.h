/*
	at3_types.h

	for Project Atlas

	Define general types used in AT3DRV driver.

	Digital STREAM Labs, Inc. 2017
	Copyright © 2017, 2018, 2019 LowaSIS, Inc.
*/

#ifndef __AT3_TYPES_H__
#define __AT3_TYPES_H__

//============================================================================

#include <inttypes.h>


#ifdef __cplusplus
extern "C" {
#endif


//============================================================================


typedef enum
{
	AT3RES_OK = 0,

	// all "FAIL"s has negative value.
	AT3RES_FAIL = -1, // general failure. no specific reason.

	// common error in interface api usage
	AT3RES_BAD_HANDLE = -100,  // handle is invalid
	AT3RES_BAD_PARAM = -101,   // paramter is invalid
	// AT3RES_RANGE = -102,       // parameter is out-of-range -> use BAD_PARAM instead
	AT3RES_NOT_IMPL = -103,    // api is not implemented
	AT3RES_API_LOCK = -104,    // api is locked by another call

	// common error occurred during api execution
	AT3RES_TIMEOUT = -200,    // timeout error
	AT3RES_BUSY = -201,       // device/resource busy
	AT3RES_BAD_STATE = -202,  // invalid internal state
	AT3RES_MEMORY = -203,     // out of memory
	AT3RES_RESOURCE = -204,   // out of resource
	AT3RES_NOT_FOUND = -205,  // device (or anything) not found
	// AT3RES_BAD_DEVICE = -206,  // device is malfunction -> not used
	AT3RES_BUFFER = -207,      // full/overflow or not-ready/underflow, short buffer, ..
	AT3RES_NO_DATA = -208,     // data not availale.
	AT3RES_CANCEL = -209,      // operation cancelled.
	AT3RES_PERMISSION = -210,  // permission not allowed
	AT3RES_BAD_DATA = -211,    // 
	AT3RES_AUTH = -212,        // authentication fail
	AT3RES_BAD_FWVER = -213,   // device fw version is not valid

	// task specific errors
	AT3RES_USB_TOP = -290, // 
		// LIBUSB_TRANSFER_OVERFLOW = +6
		// ...
		// LIBUSB_TRANSFER_ERROR = +1
	AT3RES_USB_BASE = -300,
	AT3RES_LIBUSB = AT3RES_USB_BASE, // general libusb error.
		// refer libusb.h: enum libusb_error.
		// there will be total LIBUSB_ERROR_COUNT libusb errors.
		// LIBUSB_ERROR_IO = -1
		// ...
		// LIBUSB_ERROR_OTHER = -99
	AT3RES_USB_BOTTOM = -399,

	AT3RES_FE_FAIL = -400,
	AT3RES_FE_BAD_FWVER = -401, // user requested wrong fw version

	// posix
	AT3RES_ERRNO_TOP = -1000,
		// EPERM = 1 -> -1001,
		// ENOENT = 2 -> -1002, 
		// ...
		// EHWPOISON = 133 -> -1133     
	AT3RES_ERRNO_BOTTOM = -1200,

	AT3RES_MIN = -1200, // smallest errcode

	//--------------------
	// below codes are actually not error, but some informations to caller.
	// 
	AT3RES_OK_BUSY = 100, // operations are succeeded, but callee says that he is almost busy.
	AT3RES_OK_IGNORED = 101, // callee says that this operation is silently ignored.
	AT3RES_OK_MORE = 102, // more data exist, so caller should do additional job.


} AT3RESULT;

#define USBR_AT3R(ur) (AT3RESULT)((int)AT3RES_USB_BASE + (ur))
	// convert libusb result to at3result
#define IS_USBERR(ar) ( (int)(ar)>=(int)AT3RES_USB_BOTTOM && (int)(ar)<=(int)AT3RES_USB_TOP )
	// check if ar belongs to usb error range.
#define AT3R_USBR(ar) ( IS_USBERR(ar) ? (int)((ar)-(int)AT3RES_USB_BASE) : 0 )
	// convert at3result to libusb result

#define ERRNO_AT3R(erno) (AT3RESULT)((int)AT3RES_ERRNO_TOP - (erno))
#define IS_ERRNOR(ar)  ( (int)(ar)>=(int)AT3RES_ERRNO_BOTTOM && (int)(ar)<=(int)AT3RES_ERRNO_TOP )
#define AT3R_ERRNO(ar) ( IS_ERRNOR(ar) ? (int)((int)AT3RES_ERRNO_TOP - (ar)) : 0 )


#if defined(_DEFINE_AT3RESULT_STR_)
#define AT3RESULT_STR(r) ( \
	(r) == AT3RES_OK ? "ok" : \
	(r) == AT3RES_FAIL ? "fail" : \
	(r) == AT3RES_BAD_HANDLE ? "bad handle" : \
	(r) == AT3RES_BAD_PARAM ? "bad param" : \
	(r) == AT3RES_NOT_IMPL ? "not impl" : \
	(r) == AT3RES_TIMEOUT ? "timeout" : \
	(r) == AT3RES_BUSY ? "busy" : \
	(r) == AT3RES_BAD_STATE ? "bad state" : \
	(r) == AT3RES_MEMORY ? "memory" : \
	(r) == AT3RES_RESOURCE ? "resource" : \
	(r) == AT3RES_NOT_FOUND ? "not found" : \
	(r) == AT3RES_BUFFER ? "buffer" : \
	(r) == AT3RES_NO_DATA ? "no data" : \
	(r) == AT3RES_CANCEL ? "cancel" : \
	(r) == AT3RES_PERMISSION ? "permission" : \
	(r) == AT3RES_BAD_DATA ? "bad data" : \
	(r) == AT3RES_AUTH ? "auth fail" : \
	(r) == AT3RES_BAD_FWVER ? "badfw" : \
	(r) == AT3RES_FE_FAIL ? "FE fail" : \
	(r) == AT3RES_FE_BAD_FWVER ? "FE badfw" : \
	"??" \
)
#endif //defined(_DEFINE_AT3RESULT_STR_)


#define AT3ERR(r)  !!((r) < 0)
#define AT3FAILED(r)  AT3ERR(r)
#define AT3OK(r) ((r) >= 0)


//============================================================================


#ifdef __cplusplus
};
#endif

#endif // __AT3_TYPES_H__

