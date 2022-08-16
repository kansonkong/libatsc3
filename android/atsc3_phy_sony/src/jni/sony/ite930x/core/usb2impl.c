// all for Linux

/*
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/usb.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/firmware.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
*/

#include "usb2impl.h"
#include "it930x-core.h"

#ifdef UNDER_CE

Dword Usb2_getDriver (
	IN  Bridge*	bridge,
	OUT Handle*			handle
) {
	return (Error_NO_ERROR);
}


Dword Usb2_exitDriver (
    IN  Bridge*    bridge
) {
    return (Error_NO_ERROR);
}


Dword Usb2_writeControlBus (
	IN  Bridge*	bridge,
	IN  Dword			bufferLength,
	IN  Byte*			buffer
) {
	return (Error_NO_ERROR);
}


Dword Usb2_readControlBus (
	IN  Bridge*	bridge,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
) {
	return (Error_NO_ERROR);
}


Dword Usb2_readDataBus (
	IN  Bridge*	bridge,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
) {
	return (Error_NO_ERROR);
}

#else

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

/*
bool (__cdecl *Usb2_initialize) (
);
void (__cdecl *Usb2_finalize) (
);
bool (__cdecl *Usb2_writeControl) (
	Byte*		poutBuf,
	unsigned long		WriteLen,
	unsigned long*		pnBytesWrite
); 
bool (__cdecl *Usb2_readControl) (
	Byte*		pinBuf,
	unsigned long		ReadLen,
	unsigned long*		pnBytesRead
);
bool (__cdecl *Usb2_readData) (
	BYTE*		pinBuf,
	ULONG		ReadLen
);
*/

Dword Usb2_getDriver (
	IN  Bridge*	bridge,
	OUT Handle*			handle
) {
	Dword error = Error_NO_ERROR;
/*
	HINSTANCE instance = NULL;

    instance = LoadLibrary ("AF15BDAEX.dll");
    Usb2_initialize = (bool (__cdecl *) (
			)) GetProcAddress (instance, "af15_init");
    Usb2_finalize = (void (__cdecl *) (
			)) GetProcAddress (instance, "af15_exit");
	Usb2_writeControl = (bool (__cdecl *) (
					BYTE*		poutBuf,
					ULONG		WriteLen,
					ULONG*		pnBytesWrite
			)) GetProcAddress (instance, "af15_WriteBulkData");
    Usb2_readControl = (bool (__cdecl *) (
					BYTE*		pinBuf,
					ULONG		ReadLen,
					ULONG*		pnBytesRead
			)) GetProcAddress (instance, "af15_ReadBulkData");
    Usb2_readData = (bool (__cdecl *) (
					BYTE*		pinBuf,
					ULONG		ReadLen
			)) GetProcAddress (instance, "af15_GetTsData");

	if (!Usb2_initialize ())
		error = Error_DRIVER_INVALID;
			
	*handle = (Handle) instance;
*/
	return (error);
}

Dword Usb2_exitDriver (
    IN  Bridge*    bridge
) {

    Dword error = Error_NO_ERROR;

    return (error);
}

Dword Usb2_writeControlBus (
	IN  Bridge*	bridge,
	IN  Dword			bufferLength,
	IN  Byte*			buffer
) {
	Endeavour* it9300;		
	Device_Context*	PDC;
	Dword ret = Error_NO_ERROR;
    int act_len;
	
	it9300 = (Endeavour*) bridge;
	PDC = (Device_Context *)it9300->userData;
	
	if (PDC->disconnect)
		return -1;
	
	memcpy(PDC->temp_write_buffer, buffer, bufferLength);
    
//   	ret = usb_bulk_msg(usb_get_dev(PDC->usb_dev),
//			usb_sndbulkpipe(usb_get_dev(PDC->usb_dev), 0x02),
//			PDC->temp_write_buffer,
//			bufferLength,
//			&act_len,
//			bridge->usbbus_timeout);
//
//	if (ret)
//		deb_data(" Usb2_writeControlBus fail : %ld!\n", ret);


	return ret;
}


Dword Usb2_readControlBus (
	IN  Bridge*	bridge,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
) {
	Dword ret = Error_NO_ERROR;
	int nBytesRead;
	
	Device_Context *PDC = (Device_Context *)bridge->userData;	
	
	if (PDC->disconnect)
		return -1;
//
//	ret = usb_bulk_msg(usb_get_dev(PDC->usb_dev),
//					usb_rcvbulkpipe(usb_get_dev(PDC->usb_dev), 0x81),
//					PDC->temp_read_buffer,
//					bufferLength,
//					&nBytesRead,
//					bridge->usbbus_timeout);
//
//	memcpy(buffer, PDC->temp_read_buffer, bufferLength);
//
//	if (ret)
//		deb_data(" Usb2_readControlBus fail : %ld!\n", ret);
//
	return ret;
}


Dword Usb2_readDataBus (
	IN  Bridge*	bridge,
	IN  Dword			bufferLength,
	OUT Byte*			buffer
) {
	return (Error_NO_ERROR);
}
#endif
