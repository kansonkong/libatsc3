/***************************************************************************************
                  Silicon Laboratories Broadcast Si2157  API
   FILE: Si2147_firmware_3_0_build_x.h
   Supported IC : Si2157
   Date: August 22 2013
   (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.

   This is a dummy firmware patch for Si2147-A30 devices.
   When a new patch becomes available,  replace this file and all calls to it with the new patch.
   This patch is formatted in the new 16 byte download format.  The future patches will match this structure.
****************************************************************************************/
#ifndef _Si2147_FIRMWARE_3_0_BUILD_X_H_
#define _Si2147_FIRMWARE_3_0_BUILD_X_H_

#ifndef __FIRMWARE_STRUCT__
#define __FIRMWARE_STRUCT__
typedef struct firmware_struct {
	unsigned char firmware_len;
	unsigned char firmware_table[16];
} firmware_struct;
#endif /* __FIRMWARE_STRUCT__ */

firmware_struct Si2147_FW_3_0bx[] = {
{ 0 , { 0x00} }
};

#define FIRMWARE_LINES_3_0bx (sizeof(Si2147_FW_3_0bx)/(sizeof(firmware_struct)))

#endif /* _Si2147_FIRMWARE_3_0_BUILD_X_H_ */
