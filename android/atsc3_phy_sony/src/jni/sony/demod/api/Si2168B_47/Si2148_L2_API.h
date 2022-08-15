/***************************************************************************************
                  Silicon Laboratories Broadcast Si2148 Layer 2 API
   L2 API header for commands and properties
   FILE: Si2148_L2_API.h
   Supported IC : Si2148
   Compiled for ROM 33 firmware 2_1_build_6
   Revision: 0.1
   Date: November 12 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/

#ifndef   Si2148_L2_API_H
#define   Si2148_L2_API_H

#include "Si2148_L1_API.h"
#include "Si2148_Properties_Strings.h"

int  Si2148_Init                      (L1_Si2148_Context *api);
int  Si2148_Configure                 (L1_Si2148_Context *api);
int  Si2148_PowerUpWithPatch          (L1_Si2148_Context *api);
int  Si2148_LoadFirmware              (L1_Si2148_Context *api, unsigned char *fw_table, int lines);
int  Si2148_LoadFirmware_16           (L1_Si2148_Context *api, firmware_struct fw_table[], int nbLines);
int  Si2148_StartFirmware             (L1_Si2148_Context *api);

/* _specific_code_insertion_start */
int Si2148_UpdateChannelScanFrequency(int freq,int channelsFound);
int Si2148_GetRF                 (L1_Si2148_Context *api);
int Si2148_Tune                  (L1_Si2148_Context *api, unsigned char mode, unsigned long freq);
int Si2148_DTVTune               (L1_Si2148_Context *api, unsigned long freq, unsigned char bw, unsigned char modulation, unsigned char invert_spectrum);
int Si2148_XoutOn                (L1_Si2148_Context *api);
int Si2148_XoutOff               (L1_Si2148_Context *api);
int Si2148_Standby               (L1_Si2148_Context *api);
int Si2148_Powerdown             (L1_Si2148_Context *api);
#define Si2148_FORCE_NORMAL_AGC       0
#define Si2148_FORCE_MAX_AGC          1
#define Si2148_FORCE_TOP_AGC          2
int Si2148_AGC_Override    (L1_Si2148_Context *Si2148, unsigned char mode );
/* _specific_code_insertion_point */

#endif /* Si2148_L2_API_H */





