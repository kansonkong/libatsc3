/***************************************************************************************
                  Silicon Laboratories Broadcast Si2168B Layer 2 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   L2 API header for commands and properties
   FILE: Si2168B_L2_API.h
   Supported IC : Si2168B
   Compiled for ROM 0 firmware 3_A_build_3
   Revision: REVISION_NAME
   Date: May 22 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/
/* Change log:

 As from V0.3.7:
   <improvement>[UNICABLE] redefinition of the return type of the Unicable tune function (from void to int),
    To allow proving the final Unicable tune freq to Seek_Next.

 As from V0.3.4:
   <correction> [handshake in DVB-T/T2 Seek] Moving searchStartTime and timeoutStartTime in Si2168B_L2_Context to allow correct
                Seek handshake management. This has very few impact, as no customer has been using this so far. It's worth
                correcting it anyway. The handshake variables are also better managed with these changes, making it easier to
                manage the handshake and timeout.

 As from V0.3.2:
   <new_feature> [config_macros] Using SiLabs_API_Frontend_Chip / SiLabs_API_TER_tuner_I2C_connection / SiLabs_API_SAT_tuner_I2C_connection
                 functions now that they are available in the wrapper code.

 As from V0.3.1:
   Replacing Si2168B_3A_COMPATIBLE by Si2168B_ES_COMPATIBLE, to be consistent with C file

 As from V0.2.8:
   Adding SW_INIT_Dual_Si2191_Si216x2_Si2168B macro (for QUAD EVB, using new SiLabs_API_TER_Tuner_ClockConfig function)

 As from V0.2.2:
  In Si2168B_L2_Context:
    Moving context member 'callback' to have it defined for all parts.
     This is now required as the callback pointer (a pointer to the context itself)
      is used for Unicable and INDIRECT_I2C_CONNECTION, 2 totally independent features.
    Removing 'legacy' comments.

****************************************************************************************/

#ifndef   Si2168B_L2_API_H
#define   Si2168B_L2_API_H

#define   Si2168B_COMMAND_PROTOTYPES
#include "Si2168B_Platform_Definition.h"
#include "Si2168B_Properties_Functions.h"

#ifndef    Si2168B_40_COMPATIBLE
 #ifndef    Si2168B_ES_COMPATIBLE
    "If you get a compilation error on these lines, it means that no Si2168B version has been selected.";
    "Please define Si2168B_40_COMPATIBLE or Si2168B_ES_COMPATIBLE at project level!";
    "Once the flags will be defined, this code will not be visible to the compiler anymore";
    "Do NOT comment these lines, they are here to help, showing if there are missing project flags";
 #endif /* Si2168B_ES_COMPATIBLE */
#endif /* Si2168B_40_COMPATIBLE */

 #ifndef   TERRESTRIAL_FRONT_END
  #define   TERRESTRIAL_FRONT_END
 #endif /* TERRESTRIAL_FRONT_END */



#ifdef    INDIRECT_I2C_CONNECTION
  typedef int  (*Si2168B_INDIRECT_I2C_FUNC)  (void*);
#endif /* INDIRECT_I2C_CONNECTION */

typedef struct _Si2168B_L2_Context {
   L1_Si2168B_Context *demod;
   TER_TUNER_CONTEXT *tuner_ter;
 #ifdef    INDIRECT_I2C_CONNECTION
   Si2168B_INDIRECT_I2C_FUNC  f_TER_tuner_enable;
   Si2168B_INDIRECT_I2C_FUNC  f_TER_tuner_disable;
 #endif /* INDIRECT_I2C_CONNECTION */
   L1_Si2168B_Context  demodObj;
   TER_TUNER_CONTEXT  tuner_terObj;
   int                first_init_done;
   int                Si2168B_init_done;
   int                TER_init_done;
   int                TER_tuner_init_done;
   unsigned char      auto_detect_TER;
   void              *callback;
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
   int                searchStartTime;  /* searchStartTime  is used to trace the time spent trying to detect a channel.                      */
                                        /* It is set differently from seekStartTime when returning from a handshake                          */
   int                timeoutStartTime; /* timeoutStartTime is used in blind mode to make sure the FW is correctly responding.               */
                                        /* timeoutStartTime is used in non-blind mode to manage the timeout when trying to lock on a channel */
   int                handshakeUsed;
   int                handshakeStart_ms;
   int                handshakePeriod_ms;
   unsigned char      handshakeOn;
   int                center_rf;
} Si2168B_L2_Context;

/* firmware_struct needs to be declared to allow loading the FW in 16 bytes mode */
#ifndef __FIRMWARE_STRUCT__
#define __FIRMWARE_STRUCT__
typedef struct firmware_struct {
  unsigned char firmware_len;
  unsigned char firmware_table[16];
} firmware_struct;
#endif /* __FIRMWARE_STRUCT__ */

int   Si2168B_Init                      (L1_Si2168B_Context *api);
int   Si2168B_Media                     (L1_Si2168B_Context *api, int modulation);
int   Si2168B_Configure                 (L1_Si2168B_Context *api);
int   Si2168B_PowerUpWithPatch          (L1_Si2168B_Context *api);
int   Si2168B_LoadFirmware              (L1_Si2168B_Context *api, unsigned char *fw_table, int lines);
int   Si2168B_LoadFirmware_16           (L1_Si2168B_Context *api, firmware_struct fw_table[], int nbLines);
int   Si2168B_LoadFirmwareSPI           (L1_Si2168B_Context *api, firmware_struct fw_table[], int nbLines, unsigned char pbl_key,  unsigned char pbl_num);
int   Si2168B_StartFirmware             (L1_Si2168B_Context *api);
int   Si2168B_STANDBY                   (L1_Si2168B_Context *api);
int   Si2168B_WAKEUP                    (L1_Si2168B_Context *api);
char *Si2168B_standardName              (int standard);
int   Si2168B_SILENT                    (Si2168B_L2_Context *front_end, int silent);

/*****************************************************************************************/
/*               SiLabs demodulator API functions (demod and tuner)                      */
/*****************************************************************************************/
    /*  Software info functions */
int   Si2168B_L2_Infos                     (Si2168B_L2_Context *front_end, char *infoString);

    /*  Software init functions */
//Edit by ITE    
char  Si2168B_L2_SW_Init                  (Si2168B_L2_Context *front_end
                                   , int demodAdd
                                   , int tunerAdd_Ter
								   , Endeavour* endeavour
								   , unsigned char endeavourChipidx
								   , unsigned char i2cbus                                    
 #ifdef    INDIRECT_I2C_CONNECTION
                                   , Si2168B_INDIRECT_I2C_FUNC           TER_tuner_enable_func
                                   , Si2168B_INDIRECT_I2C_FUNC           TER_tuner_disable_func
 #endif /* INDIRECT_I2C_CONNECTION */
                                   , void *p_context
                                   );
                                   
//Edit by ITE                                    
void  Si2168B_L2_HW_Connect               (Si2168B_L2_Context   *front_end);
    /*  Locking and status functions */
int   Si2168B_L2_switch_to_standard       (Si2168B_L2_Context   *front_end, unsigned char new_standard, unsigned char force_full_init);
int   Si2168B_L2_lock_to_carrier   (Si2168B_L2_Context *front_end
                                 ,  int standard
                                 ,  int freq
                                  , int dvb_t_bandwidth_hz
                                  , unsigned char dvb_t_stream
                                  , unsigned int symbol_rate_bps
                                  , unsigned char dvb_c_constellation
                                  , int plp_id
                                  , unsigned char T2_lock_mode
                                    );
int   Si2168B_L2_Channel_Lock_Abort       (Si2168B_L2_Context   *front_end);
int   Si2168B_L2_communication_check      (Si2168B_L2_Context   *front_end);
int   Si2168B_L2_Part_Check               (Si2168B_L2_Context   *front_end);
unsigned char Si2168B_L2_Set_Invert_Spectrum(Si2168B_L2_Context *front_end);
    /*  TS functions */
int   Si2168B_L2_TS_mode                  (Si2168B_L2_Context   *front_end, int ts_mode);

    /*  Tuner wrapping functions */
int   Si2168B_L2_Tune                     (Si2168B_L2_Context   *front_end, int rf);
int   Si2168B_L2_Get_RF                   (Si2168B_L2_Context   *front_end);


    /*  Tuner i2c bus control functions */
unsigned char Si2168B_L2_Tuner_I2C_Enable (Si2168B_L2_Context   *front_end);
unsigned char Si2168B_L2_Tuner_I2C_Disable(Si2168B_L2_Context   *front_end);

    /*  Scanning functions */
const char *Si2168B_L2_Trace_Scan_Status  (int scan_status);
int   Si2168B_L2_Channel_Seek_Init        (Si2168B_L2_Context   *front_end,
                                          int rangeMinHz,   int rangeMaxHz,
                                          int seekBWHz,     int seekStepHz,
                                          int minSRbps,     int maxSRbps,
                                          int minRSSIdBm,   int maxRSSIdBm,
                                          int minSNRHalfdB, int maxSNRHalfdB);
int  Si2168B_L2_Channel_Seek_Next  (Si2168B_L2_Context *front_end
                                           , int *standard
                                           , int *freq
                                           , int *bandwidth_Hz
                                           , int *stream
                                           , unsigned int *symbol_rate_bps
                                           , int *constellation
                                           , int *num_plp
                                           , int *T2_base_lite
                                           );
int   Si2168B_L2_Channel_Seek_End         (Si2168B_L2_Context   *front_end);
int   Si2168B_L2_Channel_Seek_Abort       (Si2168B_L2_Context   *front_end);
#define   AGC_FUNCTIONS
int   Si2168B_L2_TER_Clock                (Si2168B_L2_Context   *front_end, int clock_source, int clock_input, int clock_freq, int clock_control);
int   Si2168B_L2_TER_AGC                  (Si2168B_L2_Context   *front_end, int agc1_mode, int agc1_inversion, int agc2_mode, int agc2_inversion);
int   Si2168B_L2_TER_FEF_CONFIG           (Si2168B_L2_Context   *front_end, int fef_mode, int fef_pin, int fef_level);
int   Si2168B_L2_TER_FEF                  (Si2168B_L2_Context   *front_end, int fef);
int   Si2168B_L2_TER_FEF_SETUP            (Si2168B_L2_Context   *front_end, int fef);
int   Si2168B_TerAutoDetect               (Si2168B_L2_Context   *front_end);
int   Si2168B_TerAutoDetectOff            (Si2168B_L2_Context   *front_end);

#ifdef    SILABS_API_TEST_PIPE
int   Si2168B_L2_Test                     (Si2168B_L2_Context *front_end, char *target, char *cmd, char *sub_cmd, double dval, double *retdval, char **rettxt);
#endif /* SILABS_API_TEST_PIPE */

#define Si2168B_READ(ptr, register)         Si2168B_L1_DD_GET_REG (ptr, Si2168B_##register##_LSB, Si2168B_##register##_MID, Si2168B_##register##_MSB); register = (ptr->rsp->dd_get_reg.data1<<0) + (ptr->rsp->dd_get_reg.data2<<8) + (ptr->rsp->dd_get_reg.data3<<16) + (ptr->rsp->dd_get_reg.data4<<24)
#define Si2168B_WRITE(ptr, register, value) Si2168B_L1_DD_SET_REG (ptr, Si2168B_##register##_LSB, Si2168B_##register##_MID, Si2168B_##register##_MSB, value);

#define   MACROS_FOR_SINGLE_USING_Si2168B
#ifdef    MACROS_FOR_SINGLE_USING_Si2168B

#define SW_INIT_Si2169_67_76_EVB_Rev1_1_Si2168B\
  /* SW Init for front end 0 */\
  front_end                   = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x18);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2176, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 2);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xa, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5812, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 21, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 0, 32, 16, 2);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 0, 0x0, 0);\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si216x_EVB_Rev3_0_Si2168B_V2018\
  /* SW Init for front end 0 */\
  front_end  = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0xC6);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 2);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xa, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0xA2018, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 21, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 2);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 1);\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si216x_EVB_Rev3_0_Si2168B\
  /* SW Init for front end 0 */\
  front_end  = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x18);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 2);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xa, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5815, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 25, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 1, 44, 24, 2);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 1);\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si216x_SOC_EVB_Rev1_0_Si2178_Si2168B\
  /* SW Init for front end 0 */\
  front_end                   = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x18);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 2);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xa, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 2);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 1);\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si216x_SOC_EVB_Rev1_0_Si2178B_Si2168B\
  /* SW Init for front end 0 */\
  front_end                   = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x18);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178B, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 2);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xa, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 2);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 1);\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si216x_SOC_EVB_Rev1_0_Si2178_Si2168B_iroha\
  /* SW Init for front end 0 */\
  front_end                   = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0xC6);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2176, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xa, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0xA2018, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 1);\
  SiLabs_API_HW_Connect               (front_end, 1);

#endif /* MACROS_FOR_SINGLE_USING_Si2168B */

#define   MACROS_FOR_DUAL_USING_Si2168B
#ifdef    MACROS_FOR_DUAL_USING_Si2168B

#define SW_INIT_Si216x2_EVB_Rev1_x_Si2168B\
  /* SW Init for front end 0 */\
  front_end                   = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x14);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2148, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xa, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xc, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 1);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xc, 1, 0x0, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[0]");\
  SiLabs_API_HW_Connect               (front_end, 1);\
  /* SW Init for front end 1 */\
  front_end                    = &(FrontEnd_Table[1]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xce, 0xc6, 0x16);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2148, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 0, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 0);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xd, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 0);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 1, "fe[1]");\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si216x2_EVB_Rev2_0_Si2168B\
  /* SW Init for front end 0 */\
  front_end                   = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x14);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2177, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xa, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xc, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 1);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xc, 1, 0x0, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[0]");\
  SiLabs_API_HW_Connect               (front_end, 1);\
  /* SW Init for front end 1 */\
  front_end                    = &(FrontEnd_Table[1]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xce, 0xc6, 0x16);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2157, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 0, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 2);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xd, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 0);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[1]");\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si216x2_EVB_Rev1_x_Si2178_Si2168B\
  /* SW Init for front end 0 */\
  front_end                   = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x14);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xa, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xc, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 1);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xc, 1, 0x0, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[0]");\
  SiLabs_API_HW_Connect               (front_end, 1);\
  /* SW Init for front end 1 */\
  front_end                    = &(FrontEnd_Table[1]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xce, 0xc6, 0x16);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 0, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 0);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xd, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 0);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 1, "fe[1]");\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si216x2_EVB_Rev1_x_Si2178B_Si2168B\
  /* SW Init for front end 0 */\
  front_end                   = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x14);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178B, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xa, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xc, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 1);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xc, 1, 0x0, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[0]");\
  SiLabs_API_HW_Connect               (front_end, 1);\
  /* SW Init for front end 1 */\
  front_end                    = &(FrontEnd_Table[1]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xce, 0xc6, 0x16);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178B, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 0, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xd, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 2, 33, 27, 1);\
  SiLabs_API_SAT_Spectrum             (front_end, 0);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 1, "fe[1]");\
  SiLabs_API_HW_Connect               (front_end, 1);

#define SW_INIT_Si21682_EVB_Rev1_0_Si2168B\
  /* SW Init for front end 0 (TER-ONLY) */\
  front_end                    = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2141, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xa, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xc, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[0]");\
  SiLabs_API_HW_Connect               (front_end, 1);\
  /* SW Init for front end 1 (TER-ONLY) */\
  front_end                    = &(FrontEnd_Table[1]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xce, 0xc6, 0);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2141, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 0, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_FEF_Config           (front_end, 1, 0xb, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xd, 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[1]");\
  SiLabs_API_HW_Connect               (front_end, 1);

#endif /* MACROS_FOR_DUAL_USING_Si2168B */

#define   MACROS_FOR_QUAD_USING_Si2168B
#ifdef    MACROS_FOR_QUAD_USING_Si2168B

  #define SW_INIT_Dual_Si2191_Si216x2_Si2168B\
  \
  front_end                    = &(FrontEnd_Table[0]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xc8, 0xc0, 0x14);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178B, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xc, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x10);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0xc, 1, 0x0 , 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[0]");\
  SiLabs_API_HW_Connect               (front_end, 1);\
  \
  front_end                   = &(FrontEnd_Table[1]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xca, 0xc6, 0x16);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178B, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 0, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 0);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xd, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x12);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 0);\
  SiLabs_API_SAT_Clock                (front_end, 1, 44, 24, 0);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0xd, 1, 0x0 , 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[1]");\
  SiLabs_API_HW_Connect               (front_end, 1);\
  \
  front_end                   = &(FrontEnd_Table[2]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xce, 0xc2, 0x00);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178B, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 2);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 1, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xc, 0);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x14);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 2);\
  SiLabs_API_SAT_Clock                (front_end, 1, 44, 24, 1);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0x0, 0, 0x0 , 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[2]");\
  SiLabs_API_HW_Connect               (front_end, 1);\
  \
  front_end                   = &(FrontEnd_Table[3]);\
  SiLabs_API_Frontend_Chip            (front_end, 0x2164);\
  SiLabs_API_SW_Init                  (front_end, 0xcc, 0xc4, 0x00);\
  SiLabs_API_Select_TER_Tuner         (front_end, 0x2178B, 0);\
  SiLabs_API_TER_tuner_I2C_connection (front_end, 2);\
  SiLabs_API_TER_Tuner_ClockConfig    (front_end, 0, 1);\
  SiLabs_API_TER_Clock                (front_end, 1, 44, 24, 0);\
  SiLabs_API_TER_AGC                  (front_end, 0x0, 0, 0xd, 0);\
  SiLabs_API_SAT_Select_LNB_Chip      (front_end, 26, 0x16);\
  SiLabs_API_Select_SAT_Tuner         (front_end, 0x5816, 0);\
  SiLabs_API_SAT_tuner_I2C_connection (front_end, 2);\
  SiLabs_API_SAT_Clock                (front_end, 1, 44, 24, 0);\
  SiLabs_API_SAT_Spectrum             (front_end, 1);\
  SiLabs_API_SAT_AGC                  (front_end, 0x0, 0, 0x0 , 0);\
  SiLabs_API_Set_Index_and_Tag        (front_end, 0, "fe[3]");\
  SiLabs_API_HW_Connect               (front_end, 1);

#endif /* MACROS_FOR_QUAD_USING_Si2168B */

#endif /* Si2168B_L2_API_H */

