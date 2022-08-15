/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168B Layer 1 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   API types used by commands and properties
   FILE: Si2168B_typedefs.h
   Supported IC : Si2168B
   Compiled for ROM 0 firmware 3_A_build_3
   Revision: REVISION_NAME
   Date: May 22 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef   Si2168B_TYPEDEFS_H
#define   Si2168B_TYPEDEFS_H

#define Si2168B_DOWNLOAD_ON_CHANGE 1
#define Si2168B_DOWNLOAD_ALWAYS    0

  #define Si2168B_TERRESTRIAL 1

  #define Si2168B_DVBT_MIN_LOCK_TIME    100
  #define Si2168B_DVBT_MAX_LOCK_TIME   2000

#define Si2168B_CLOCK_ALWAYS_OFF 0
#define Si2168B_CLOCK_ALWAYS_ON  1
#define Si2168B_CLOCK_MANAGED    2

  #define Si2168B_DVBT2_MIN_LOCK_TIME       100
  #define Si2168B_DVBT2_MAX_LOCK_TIME      2000

  #define Si2168B_DVBC_MIN_LOCK_TIME     20
  #define Si2168B_DVBC_MAX_SEARCH_TIME 5000


typedef struct L1_Si2168B_Context {
  L0_Context                 *i2c;
  L0_Context                  i2cObj;
  Si2168B_CmdObj              *cmd;
  Si2168B_CmdObj               cmdObj;
  Si2168B_CmdReplyObj         *rsp;
  Si2168B_CmdReplyObj          rspObj;
  Si2168B_PropObj             *prop;
  Si2168B_PropObj              propObj;
  Si2168B_PropObj             *propShadow;
  Si2168B_PropObj              propShadowObj;
  Si2168B_COMMON_REPLY_struct *status;
  Si2168B_COMMON_REPLY_struct  statusObj;
  int                         standard;
  int                         media;
  /* global variable for property settings mode
     Possible values:
         Si2168B_DOWNLOAD_ON_CHANGE (recommended)
      or Si2168B_DOWNLOAD_ALWAYS.                 */
  int propertyWriteMode;
  /* _additional_struct_members_point */
  unsigned int                tuner_ter_clock_source;
  unsigned int                tuner_ter_clock_freq;
  unsigned int                tuner_ter_clock_input;
  unsigned int                tuner_ter_clock_control;
  unsigned int                tuner_ter_agc_control;
  unsigned int                fef_selection;
  unsigned int                fef_mode;
  unsigned int                fef_pin;   /* FEF pin connected to TER tuner AGC freeze input      */
  unsigned int                fef_level; /* GPIO state on FEF_pin when used (during FEF periods) */
  unsigned int  tuner_rssi;
  unsigned int  spi_download;
  unsigned int  spi_send_option;
  unsigned int  spi_clk_pin;
  unsigned int  spi_clk_pola;
  unsigned int  spi_data_pin;
  unsigned int  spi_data_order;
} L1_Si2168B_Context;

#ifndef __FIRMWARE_STRUCT__
#define __FIRMWARE_STRUCT__
typedef struct  {
  unsigned char firmware_len;
  unsigned char firmware_table[16];
} firmware_struct;
#endif /* __FIRMWARE_STRUCT__ */

#endif /* Si2168B_TYPEDEFS_H */

