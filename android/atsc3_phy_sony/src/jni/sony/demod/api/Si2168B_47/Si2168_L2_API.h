/***************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 2 API
   L2 API header for commands and properties
   FILE: Si2168_L2_API.h
   Supported IC : Si2168
   Compiled for ROM 2 firmware 2_0_build_7
   Revision: REVISION_NAME
   Date: August 22 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/
/* Change log:
 As from V2.5.3:
  In Si2168_L2_Context:
    Adding context members:
     unsigned char      satellite_spectrum_inversion;
     int                handshakeUsed;
     int                handshakeStart_ms;
     int                handshakePeriod_ms;
     unsigned char      handshakeOn;
    Adding unsigned char Si2168_L2_Set_Invert_Spectrum(Si2168_L2_Context *front_end);

****************************************************************************************/

#ifndef   Si2168_L2_API_H
#define   Si2168_L2_API_H

#define   Si2168_COMMAND_PROTOTYPES
#include "Si2168_Platform_Definition.h"
#include "Si2168_Properties_Strings.h"



#ifndef    Si2168_0B_COMPATIBLE
 #ifndef    Si2168_10_COMPATIBLE
  #ifndef    Si2168_20_COMPATIBLE
   #ifndef    Si2168_30_COMPATIBLE
    "If you get a compilation error on these lines, it means that no Si2168 version has been selected.";
    "Please define Si2168_0B_COMPATIBLE or Si2168_10_COMPATIBLE or Si2168_20_COMPATIBLE or Si2168_30_COMPATIBLE at project level!";
    "Once the flags will be defined, this code will not be visible to the compiler anymore";
    "Do NOT comment these lines, they are here to help, showing if there are missing project flags";
    #endif /* Si2168_30_COMPATIBLE */
  #endif /* Si2168_20_COMPATIBLE */
 #endif /* Si2168_10_COMPATIBLE */
#endif /* Si2168_0B_COMPATIBLE */

 #ifndef   TERRESTRIAL_FRONT_END
  #define   TERRESTRIAL_FRONT_END
 #endif /* TERRESTRIAL_FRONT_END */


typedef struct Si2168_L2_Context {
   L1_Si2168_Context *demod;
   TER_TUNER_CONTEXT *tuner_ter;
   L1_Si2168_Context  demodObj;
   TER_TUNER_CONTEXT  tuner_terObj;
   int                first_init_done;
   int                Si2168_init_done;
   int                TER_init_done;
   int                TER_tuner_init_done;
   unsigned char      auto_detect_TER;
   int                standard;
   int                detected_rf;
   int                previous_standard;
   int                tuneUnitHz;
   int                rangeMin;
   int                rangeMax;
   int                seekBWHz;
   int                seekStepHz;
   int                minSRbps;
   int                maxSRbps;
   int                minRSSIdBm;
   int                maxRSSIdBm;
   int                minSNRHalfdB;
   int                maxSNRHalfdB;
   int                seekAbort;
   int                lockAbort;
   unsigned char      seekRunning;
   int                handshakeUsed;
   int                handshakeStart_ms;
   int                handshakePeriod_ms;
   unsigned char      handshakeOn;
   int                center_rf;
} Si2168_L2_Context;

int   Si2168_Init                      (L1_Si2168_Context *api);
int   Si2168_Media                     (L1_Si2168_Context *api, int modulation);
int   Si2168_Configure                 (L1_Si2168_Context *api);
int   Si2168_PowerUpWithPatch          (L1_Si2168_Context *api);
int   Si2168_LoadFirmware              (L1_Si2168_Context *api, unsigned char *fw_table, int lines);
int   Si2168_StartFirmware             (L1_Si2168_Context *api);
int   Si2168_STANDBY                   (L1_Si2168_Context *api);
char *Si2168_standardName              (int standard);

/*****************************************************************************************/
/*               SiLabs demodulator API functions (demod and tuner)                      */
/*****************************************************************************************/
    /*  Software info functions */
int   Si2168_L2_Infos                     (Si2168_L2_Context *front_end, char *infoString);

    /*  Software init functions */
char  Si2168_L2_SW_Init                  (Si2168_L2_Context *front_end
                                   , int demodAdd
                                   , int tunerAdd_Ter
								   , Endeavour* endeavour
								   , unsigned char endeavourChipidx
								   , unsigned char i2cbus
                                   );

void  Si2168_L2_HW_Connect               (Si2168_L2_Context   *front_end);
    /*  Locking and status functions */
int   Si2168_L2_switch_to_standard       (Si2168_L2_Context   *front_end, int new_standard, unsigned char force_full_init);
int   Si2168_L2_lock_to_carrier          (Si2168_L2_Context   *front_end, int standard, int freq, int dvb_t_bandwidth_hz, int dvb_t_stream, unsigned int symbol_rate, unsigned char dvb_c_constellation, unsigned char dvb_t2_plp_id);
int   Si2168_L2_Channel_Lock_Abort       (Si2168_L2_Context   *front_end);
int   Si2168_L2_communication_check      (Si2168_L2_Context   *front_end);
int   Si2168_L2_Part_Check               (Si2168_L2_Context   *front_end);
unsigned char Si2168_L2_Set_Invert_Spectrum(Si2168_L2_Context *front_end);
    /*  TS functions */
int   Si2168_L2_TS_mode                  (Si2168_L2_Context   *front_end, int ts_mode);

    /*  Tuner wrapping functions */
int   Si2168_L2_Tune                     (Si2168_L2_Context   *front_end, int rf);
int   Si2168_L2_Get_RF                   (Si2168_L2_Context   *front_end);


    /*  Tuner i2c bus control functions */
unsigned char Si2168_L2_Tuner_I2C_Enable (Si2168_L2_Context   *front_end);
unsigned char Si2168_L2_Tuner_I2C_Disable(Si2168_L2_Context   *front_end);

    /*  Scanning functions */
const char *Si2168_L2_Trace_Scan_Status  (int scan_status);
int   Si2168_L2_Channel_Seek_Init        (Si2168_L2_Context   *front_end,
                                          int rangeMinHz,   int rangeMaxHz,
                                          int seekBWHz,     int seekStepHz,
                                          int minSRbps,     int maxSRbps,
                                          int minRSSIdBm,   int maxRSSIdBm,
                                          int minSNRHalfdB, int maxSNRHalfdB);
int   Si2168_L2_Channel_Seek_Next        (Si2168_L2_Context   *front_end, int *standard, int *freq, int *bandwidth_Hz, int *stream, unsigned int *symbol_rate_bps, int *constellation, int *dvb_t2_num_plp);
int   Si2168_L2_Channel_Seek_End         (Si2168_L2_Context   *front_end);
int   Si2168_L2_Channel_Seek_Abort       (Si2168_L2_Context   *front_end);

int   Si2168_L2_TER_FEF                  (Si2168_L2_Context   *front_end, int fef);
int   Si2168_L2_TER_FEF_SETUP            (Si2168_L2_Context   *front_end, int fef);
int   Si2168_TerAutoDetect               (Si2168_L2_Context   *front_end);
int   Si2168_TerAutoDetectOff            (Si2168_L2_Context   *front_end);

#ifdef    SILABS_API_TEST_PIPE
int   Si2168_L2_Test                     (Si2168_L2_Context *front_end, char *target, char *cmd, char *sub_cmd, double dval, double *retdval, char **rettxt);
#endif /* SILABS_API_TEST_PIPE */

#endif /* Si2168_L2_API_H */

