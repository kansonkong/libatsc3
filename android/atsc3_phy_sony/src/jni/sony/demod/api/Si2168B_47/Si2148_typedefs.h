/*************************************************************************************
                  Silicon Laboratories Broadcast Si2148 Layer 1 API
   API types used by commands and properties
   FILE: Si2148_typedefs.h
   Supported IC : Si2148
   Compiled for ROM 33 firmware 2_1_build_6
   Revision: 0.1
   Date: November 12 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef   Si2148_TYPEDEFS_H
#define   Si2148_TYPEDEFS_H

#define BUF_SIZE 1000
/* define the maximum possible channels (1002MHz - 43MHz) / 6MHz  (required for the channelscan array)*/
#define MAX_POSSIBLE_CHANNELS 160
#define MAX_LENGTH             80

typedef struct L1_Si2148_Context {
  L0_Context                 *i2c;
  L0_Context                  i2cObj;
  Si2148_CmdObj              *cmd;
  Si2148_CmdObj               cmdObj;
  Si2148_CmdReplyObj         *rsp;
  Si2148_CmdReplyObj          rspObj;
  Si2148_PropObj             *prop;
  Si2148_PropObj              propObj;
  Si2148_COMMON_REPLY_struct *status;
  Si2148_COMMON_REPLY_struct  statusObj;
  /*chip rev constants for integrity checking */
  unsigned char chiprev;
  unsigned char part;
  /* Last 2 digits of part number */
  unsigned char partMajorVersion;
  unsigned char partMinorVersion;
  unsigned char partRomid;
/* Channel Scan Globals */
  /* Global array to store the list of found channels */
  unsigned long ChannelList[MAX_POSSIBLE_CHANNELS];
  /* ChannelScanPal needs to store the PAL type also so allocate 4 chars each for that */
  char ChannelType[MAX_POSSIBLE_CHANNELS][4];
  /* Number of found channels from a channel scan */
  int ChannelListSize;
} L1_Si2148_Context;

/* _additional_definitions_start */
 #ifndef __VID_FILT_STRUCT__
#define __VID_FILT_STRUCT__
/* struct to read video filter file with varying line lengths */
typedef struct  {
  char vid_filt_len;
  unsigned char vid_filt_table [16];
} vid_filt_struct;
#endif /* __VID_FILT_STRUCT__ */
/* _additional_definitions_point */
#ifndef __FIRMWARE_STRUCT__
#define __FIRMWARE_STRUCT__
typedef struct  {
	unsigned char firmware_len;
	unsigned char firmware_table[16];
} firmware_struct;
#endif /* __FIRMWARE_STRUCT__ */
#endif /* Si2148_TYPEDEFS_H */





