/***************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 2 API
   L2 API for commands and properties
   FILE: Si2168_L2_API.c
   Supported IC : Si2168
   Compiled for ROM 2 firmware 2_0_build_11
   Revision: REVISION_NAME
   Tag:  V2.6.0
   Date: September 24 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/
/* Change log: */
/* Last  changes:

 As from V2.6.0:
   With fw 2_0b19  (for ROM2)
   With fw 3_0b2   (for ROM3)

*/
/* Older changes:

 As from V2.5.9:
  Si2168_10_ROM1_Patch_0_Cb20.h corrected to load on Si2168-20 parts
  In Si2168_L2_Channel_Lock_Abort:
   front_end->handshakeOn = 0; (to have the next 'lock' restart handshaking from 0)

 As from V2.5.8:
  In Si2168_L2_lock_to_carrier:
   Re-inserting stream selection lines (removed by mistake with HP/LP workaround code), to allow stream selection when in AUTO_T_T2

 As from V2.5.7:
  Compatibility with Si2168-30 (FW version still TBD)
   With fw 2_0b18  (for ROM2)
   With fw 3_0bx   (for ROM3)
   In Si2168_L2_SW_Init:
    front_end->Si2168_init_done    = 0; (may not be set to '0' by all compilers)
   In Si2168_L2_lock_to_carrier:
    Handshake lines modified to allow easier-to-read folding (no functional change)
    Some handshake-related traces only traced if handshake feature is used
    Moving 1.7 MHz BW setting in a more 'T/T2 general place, just in case somebody tries calling the function in AUTO_T_T2 with 1.7 MHz BW
   In Si2168_L2_Channel_Seek_Init:
    Adding workaround for 'consecutive T/T2 seek followed by C seek'.
     (will probably be corrected by fw in a coming release)
   In Si2168_L2_Test: Adding 'get_rev' option.

 As from V2.5.6:
   With fw 2_0b17  (for ROM2)
  In Si2168_Configure:
   Calling Si2168_downloadAllProperties after setting DD_TS_MODE, to same some time (otherwise this property is set twice)
  In Si2168_L2_SW_Init:
   Moving front_end->satellite_spectrum_inversion and front_end->unicable_spectrum_inversion setup lines for compatibility with/without SAT and UNICABLE compilations
   front_end->auto_detect_TER     = 1; by default
  In Si2168_L2_lock_to_carrier:
   Removing HP/LP workaround, as this is now corrected as from fw 2_0b17.
   Enabling i2c passthru when turning FEF on (when locked in T2 using AUTO_DETECT/AUTO_T_T2)
  SILABS_API_TEST_PIPE:
   Adding Si2168_L2_Test_MPLP function, demonstrating how plp_ids are retrieved when locked on a T2 MPLP channel

 As from V2.5.5:
   In AUTO_T_T2 mode, it is mandatory to set Si2168_DD_MODE_PROP_CODE in DVB-T to set the wanted HP/LP stream.
   This work around must be used with firmware version lower than 2_0b17

 As from V2.5.4:
   With fw 2_0b16  (for ROM2)

 As from V2.5.3:
  In Si2168_LoadFirmware:
   Surrounding SiTRACE message with brackets to allow compiling with no SiTRACES
  In Si2168_L2_Part_Check:
   Adding '#ifdef SiTRACES' to allow compiling with no SiTRACES
  Adding Si2168_L2_Set_Invert_Spectrum function, to control invert_spectrum in a single location
  Adding the 'handshake' feature in Seek_Next and lock_to_carrier:
   No matter what happens, Seek_Next and lock_to_carrier will return after (roughly) front_end->handshakePeriod_ms
   (front_end->handshakePeriod_ms = 1000 by default, set in Si2168_L2_SW_Init)
   This allows the application to decide what to do in case seeking takes a long time:
   For Seek_Next:
    1- Call Seek_Next again to continue
    2- Set front_end->SeekAbort to '1' and call Seek_Next to abort the search
    3- Call Seek_End to cancel the search
   The currently tuned frequency is stored in *freq, for monitoring purposes
   The normal use is to check the value of 'freq' to update a progress indicator and call Seek_Next to continue
   Note: during the handshake, the fw is still searching for channels in the current spectrum 'chunk'
  _______
  CAUTION: if front_end->handshakeUsed is '1': this requires a change in the way the Seek_Next return value is managed!
  -------
  The new definition is:
    '0': scan ENDED
    '1': scan locked
    any other value: the amount of time with no fw interaction. This value represents
    the amount of time spent in the 'searching' state.

   For lock_to_carrier:
    1- Call the lock_to_carrier again to continue
    2- Set front_end->LockAbort to '1' and call lock_to_carrier to abort the lock
    3- Call lock_to_carrier with other value to start another lock
    4- Do whatever is requested by the application
  _______
  CAUTION: if front_end->handshakeUsed is '1': this requires a change in the way the lock_to_carrier return value is managed!
  -------
  The new definition is:
    '0': 'never lock'
    '1': 'locked'
    any other value: the amount of time spent searching.

   Handshake controls:
   front_end->handshakeUsed:      a flag to activate the handshake feature
   front_end->handshakePeriod_ms: the period after which the function will return if front_end->handshakeUsed == 1

  In Si2168_L2_SW_Init:
    front_end->handshakeUsed       = 0; ( set to '0' by default for compatibility with previous versions )
    front_end->handshakePeriod_ms  = 1000;
    front_end->satellite_spectrum_inversion = Si2168_SAT_IQ_SPECTRUM_INVERSION;
    front_end->unicable_spectrum_inversion  = 1;
  In Si2168_L2_lock_to_carrier:
   Using Si2168_L2_Set_Invert_Spectrum(front_end); to set dd_mode.invert_spectrum
  In Si2168_L2_Channel_Seek_Init:
   Using Si2168_L2_Set_Invert_Spectrum(front_end); to set dd_mode.invert_spectrum
  In Si2168_L2_Test:
   Moving some lines to avoid compilation errors with Visual Studio
   (all variables must be defined before any valuer is set)

 As from V2.5.2:
  Adding Si2168_L2_Part_Check function, to allow checking part_info.part without FW download
  In Si2168_PowerUpWithPatch:
   Changing traces for Si2168_0B and Si2168_10, as these have some PART_INFO fields at 0.

 As from V2.5.1:
  Adding Si2168_L2_Trace_Scan_Status to more easily check blindscan implementation
  In Si2168_L2_Channel_Seek_Init:
   Tracing scan_status when leaving
  In Si2168_L2_Channel_Seek_Next:
   Tracing scan_status when entering the function as well as during the blindscan loop
  In Si2168_L2_send_diseqc_sequence:
    Checking DiSEqC bus readiness before sending a DiSEqC sequence

 As from V2.5.0:
  In Si2168_L2_Channel_Seek_Init:
   When using a Unicable LNB, setting scan_sat_unicable_bw to 48 MHz to allow working with all Unicable equipments.

 As from V2.4.9:
  Added Si2168_L2_Channel_Lock_Abort function
   With fw 0_Cb20  (for ROM1)
   With fw 2_0b11  (for ROM2)

 As from V2.4.8:
   With fw 0_Cb19  (for ROM1)
   With fw 2_0b10  (for ROM2)
   In Si2168_L2_Channel_Seek_Init:
     Properly setting dd_mode.bw according to seekBWHz. (This is required for proper lock on T/T2 signals less than 8MHz wide)
   In Si2168_L2_lock_to_carrier:
    Removing the PLP selection when in AUTO_T_T2 with standard = T and the channel is T2, as Si2168_L1_DVBT2_PLP_SELECT, now works in T/T2/AUTO_T_T2
    Checking the (new) front_end->lockAbort flag to exit if required by the application (allows fast P+/P+/P+... zapping).
     Si2168_L2_lock_to_carrier can be aborted
      - before tuning
      - while waiting for min_lock_time_ms
      - while checking the lock status

 As from V2.4.7:
   With fw 0_Cb17  (for ROM1)
   With fw 2_0b8   (for ROM2)
   Easier compatibility with Si2167B: copy/pasting the related portions of Si2167B_PowerUpWithPatch into Si2168_PowerUpWithPatch

 As from V2.4.6:
   With fw 0_Cb16  (for ROM1)
   With fw 2_0b7   (for ROM2)
   API change:
    Adding Si2168_DD_SSI_SQI_PARAM_PROP property
   In Si2168_L2_lock_to_carrier:
    Removing temporary workaround for Si2168_L1_DVBT2_PLP_SELECT, which now works in T/T2/AUTO_T_T2
    Only waiting 40 ms after selecting the PLP_ID in AUTO_T_T2

 As from V2.4.5:
   Adding NO_SAT tags to allow using Si2168 code without SAT features
   With fw 0_Cb15  (for ROM1)
   With fw 2_0b6   (for ROM2)
   In Si2168_L2_lock_to_carrier:
    moving lpf_khz to avoid compilation errors with some compilers
   In Si2168_L2_read_diseqc_reply: correction check on Si2168_L1_DD_DISEQC_STATUS return value

 As from V2.4.4:
   With fw 2_0b6  (for ROM2)

 As from V2.4.3:
  Compatibility with TER_TUNER_Si2190
  Compatibility with TER_TUNER_CUSTOMTER
  In Si2168_L2_lock_to_carrier:
   Make sure FEF mode is ON when locked on a T2 channel
  In Si2168_L2_switch_to_standard:
   Adding missing breaks when setting flags (only impacting ANALOG mode: tuner_already_used and tuner_needed were improperly set at '0' in 'default' case)
  Compatibility with SAT_TUNER_RDA5816S

 As from V2.4.2:
   With fw 0_Cb14 (for ROM1)
   With fw 2_0b5  (for ROM2)
   Removing compilation flags Si2168_SCAN_DVB_C and Si2168_SCAN_SAT (always available)

 As from V2.4.1:
   In Si2168_standardName:
    casting returned strings as 'char*'
   In Si2168_L2_Channel_Seek_Next:
    setting value of dvb_t2_num_plp to 0 by default

 As from V2.4.0:
   Added unicable_spectrum_inversion in front-end structure, to handle a possible inversion done by the Unicable ODU
   Using DVB-T2 lock times for T/T2 locks
   Adding Si2168_DD_MODE_PROP_MODULATION_ANALOG enum, to separate SLEEP mode and ANALOG mode in Si2168_L2_switch_to_standard
   In Si2168_L2_switch_to_standard:
     ter_clock_already_used/ter_tune_needed = 1 for ANALOG mode
     ter_clock_already_used/ter_tune_needed = 0 for SLEEP  mode
   Removing unused part information code

 As from V2.3.9:
   With fw 0_Cb13 (for ROM1)
   With fw 2_0b4  (for ROM2)

 As from V2.3.7:
  Adding Test Pipe feature (only if SILABS_API_TEST_PIPE is defined at project level), using new Si2168_L2_Test function
  Adding PLP management (for DVB_T2 only).
  When scanning: returning dvb_t2_num_plp if locked in DVB-T2, to allow storing one 'channel' per PLP ID
  When locking in DVB-T2: selecting the required dvb_t2_plp_id
  In Si2168_L2_lock_to_carrier:
   Selecting T2 PLP in 2 steps:
    Before tuning when requested standard is DVB-T2
    After locking if
     1- requested standard is DVB-T
     2- AutoTER is active
     3- locked signal is DVB-T2
  In Si2168_L2_Channel_Seek_Next:
   Closing I2C passthrough when selecting FEF mode
   Managing weak signals in a more explicit way.
  In Si2168_L2_Channel_Seek_End:
   Managing auto detect modes
  In Si2168_L2_TER_FEF_SETUP:
   Calling Si2168_L2_TER_FEF when done
  In Si2168_L2_lock_to_carrier:
   Storing plp_id for later statusing (This should become part of DVBT2_STATUS's reply)
  Allowing RDA5815 RSSI (relies on the latest RDA5815 driver)

 As from V2.3.6:
  In Si2168_L2_Channel_Seek_Next:
    removed analog detection part from blind_mode section (not used/not validated)

 As from V2.3.5:
   With fw 0_Cb12 (for ROM1)
   With fw 2_0b3  (for ROM2)
   API change: Added dd_ts_mode.special property field to control TS clock separately from other signals
   Adding L2 context member 'unicable_spectrum_inversion', to manage possible spectrum inversion in Unicable LNB
    (this 'reverts' the SAT spectrum during blindscan and needs to be compensated)
  In Si2168_L2_Channel_Seek_Init:
    When using a Unicable LNB, check unicable_spectrum_inversion to set the dd_mode.invert_spectrum field
  In Si2168_L2_switch_to_standard:
    Adding ATV mode: switching SAT and TER clocks off, sleeping DTV demod.

 As from V2.3.4:
   Compatibility with Si2146/Si2158/Si2178

   In Si2168_L2_Channel_Seek_Init, setting DVB-C, DVB-S and DVB-S2 symbol rates based on scan_status, for proper SR statusing.
   With fw 0_Cb9

  In Si2168_Configure:
   DiSEqC 'DD_SET_REG' removed, as this is now controlled by api->prop->dd_diseqc_freq.freq_hz = 0
    (default 22000, '0' means 'envelop mode'), in Si2168_setupDDDefaults
   ts_err 'DD_SET_REG' removed, as this is now controlled by api->prop->dd_ts_mode.ts_err_polarity, in Si2168_setupDDDefaults

  Added lnb_type in Si2168_L2_Context, to store Unicable status (if compiled with UNICABLE_COMPATIBLE)
    lnb_type can be 'UNICABLE_LNB_TYPE_NORMAL'(default) or 'UNICABLE_LNB_TYPE_UNICABLE'
    (lnb_type becomes 'UNICABLE_LNB_TYPE_UNICABLE' when SiLabs_API_SAT_Unicable_Install succeeds)
    if lnb_type == 'UNICABLE_LNB_TYPE_UNICABLE':
     Tuning   uses both the Unicable LNB and the SAT tuner
     Scanning uses both the Unicable LNB and the SAT tuner
     Locking  uses both the Unicable LNB and the SAT tuner
    Otherwise (lnb_type == 'UNICABLE_LNB_TYPE_NORMAL') the behavior is as before

  Added auto_detect_TER and auto_detect_SAT flags in Si2168_L2_Context, to manage AUTO_DETECT
  Changed Si2168_TerAutoDetect    and Si2168_SatAutoDetect    functions to set   the above flags
  Added   Si2168_TerAutoDetectOff and Si2168_SatAutoDetectOff functions to clear the above flags
  In Si2168_L2_lock_to_carrier:
   Using the above flags to lock with or without AUTO_DETECT, depending on the flags.
  In Si2168_L2_SW_Init:
   auto_detect_TER set by default at '0'
   auto_detect_SAT set by default at '1'
   <porting> Change this if required
  In Si2168_L2_switch_to_standard:
   Removed SAT_AUTO_DETECT setup, as this is now controlled by the auto_detect_SAT flag.

  As from V2.3.3:
   Si2178_FEF_MODE_FREEZE_PIN_SETUP macro changed (in Si2168_Si2178.h) to send CONFIG_PINS command to the Si2178, to complete FEF setup.

  As from V2.3.2:
   with fw 0_Cb9
   In Si2168_Configure:
    Initializing value of api->cmd->dd_ext_agc_sat.agc_2_inv, even when not in use.
     (Some compilers may init this value to a non-0 value, and then the command fails if not explicitely set to a valid value)
    Modified FEF management to match new documentation
   In Si2168_PowerUpWithPatch:
    Calling Si2168_L1_GET_REV to display the final revision informatio following fw download.
   In Si2168_L2_switch_to_standard:
    Setting media earlier , as it is used to select tuner LO parameters
   In Si2168_L2_lock_to_carrier:Si2168_L2_Channel_Seek_Init
    Using DD_STATUS for SAT standards, to allow AUTO_DETECT.
   In Si2168_L2_Channel_Seek_Init:
    scan_sat_config.analog_detect set to 'ENABLED' to speed up SAT scan (faster rejection of analog channels).
   In Si2168_L2_Channel_Seek_Next:
    more explicit trace message in case the timemout limits are hit.
   In Si2168_L2_TER_FEF:
    Revised FEF management to match new documentation.
   Si2168_SatAutoDetect and Si2168_TertAutoDetect corrected (dd_mode.modulation  = Si2168_DD_MODE_PROP_MODULATION_AUTO_DETECT)

 As from 2.3.1:
   In Si2168_L2_Channel_Seek_Next:
    Tracing tune request frequency as read in scan_status.rf_freq, in kHz unit
    front_end->rangeMin properly set for DVB-C blindscan
    Properly storing DVB-C blindscan values when locked

 As from 2.3.0:
   In Si2168_L2_switch_to_standard:
    Tuner LO settings optimized (DTV values different from ATV values). This requires new macros in TER tuner files:
     TER_TUNER_ATV_LO_INJECTION
     TER_TUNER_DTV_LO_INJECTION
    Setting IQ spectrum inversion depending on SAT tuner and HW design, based on a new macro in SAT tuner files:
     Si2168_SAT_IQ_SPECTRUM_INVERSION
   In Si2168_L2_TER_FEF:
    Optimized settings for FEF management, following tests. This requires new macros in TER tuner files:
     L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC
     L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC
     L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN
   In Si2168_L2_Infos: always tracing FEF control method
   In Si2168_L2_SW_Init: FEF management working for all possible cases. DVB-T2 lock OK.
   In Si2168_L2_Channel_Seek_Next: properly storing freq when locked on DVB-T2
   In Si2168_L2_lock_to_carrier:
     Added traces to track tune/reset/lock durations
     Loop cycle time reduced to 10 ms
     Not displaying the DD_STATUS anymore, to save time.

 As from 2.2.9:
   Full Info in Si2168_PowerUpWithPatch compatible with ROM above 0x0f (for consistency with Tuner code)
   Corrected SAT clock transition trace in Si2168_L2_switch_to_standard

 As from 2.2.8:
   Downloading FW for '0b' parts, patches for '10' parts
   patch and firmware files modified to allow checking consistency between part and patch/fw before loading
   68 parts using 1_0_b3, 69 parts using 1_1b1
   Compatibility with 'NO_TER' tuner (for lab testing)
   SAT & DVB-C blindscan improved.

 As from 2.2.7:
   With fw 0_Cb7
   Si2168_L2_Channel_Seek_Next modified to allow auto detect in TER and SAT to work no matter which initial standard is selected.
   (With earlier releases, auto detection worked only for DVB-T and DVB-S)
   In Si2168_L2_Infos: TERRESTRIAL_FRONT_END tag used to surround TER lines

 As from 2.2.6:
   corrected RDA58xx AGC inversion (#define Si2168_SAT_AGC_INVERTED in Si2168_RDA58xx.h)
   api->cmd->dd_ext_agc_ter.agc_2_kloop = 18;

 As from 2.2.5:
   corrected FEF flag management
   corrected DVB-T2 statusing in Si2168_L2_Channel_Seek_Next

 As from 2.2.4:
   Using 0_C_b6 firmware:
    adding Si2168_CONFIG_CLKIO & Si2168_DVBT2_TX_ID commands
    adding prop->dd_ts_mode.clk_gapped_en property field

 As from 2.2.2:
   Adding Si2168_L2_TER_FEF function
   in Si2168_L2_Channel_Seek_Next:
     FEF set ON in TER, then set to OFF if locked on DVB-T
     Correcting offset in TER (only valid after retrieving dvbt_status or dvbt2_status)
   in Si2168_L2_switch_to_standard:
     Using Si2168_L2_TER_FEF function to set FEF ('on' only in DVB_T2)

 As from 2.2.1:
  Adding FEF control in flags.
   in Si2168_L2_SW_Init: setting fef_mode variable based on user choice (set in Si2168_Platform_Definition.h) and tuner capabilities.
   in Si2168_L2_switch_to_standard: setting FEF mode depending on the current standard.

 As from 2.2.0:
  in Si2168_Configure: removed LED control (unused)

 As from 2.1.9:
  in Si2168_Configure: Setting all MPs by default at 'disable' when not used.

  As from 2.1.5:
  With fw 0_Cb5

 As from 2.1.4:
  Si2168_0B differentiated from Si2168_10 (Si2168_10 has a much smaller fw to download)
  ADC_CREST_FACTOR properties added
  SCAN_CONFIG replaced by SCAN_SAT_CONFIG and SCAN_TER_CONFIG

 As from 2.1.2:
 Si2168_WAKEUP: compatible with Si2168 export

 As from 2.1.2:
 Si2168_WAKEUP:
  Using api->media to select clock source and freq
 Si2168_PowerUpWithPatch:
  Using Si2168_WAKEUP and returning an error if Si2168_WAKEUP fails
  If Si2168_10_COMPATIBLE: api->partMajorVersion = '1';
 Si2168_LoadFirmware:
  Using "traces suspend" & "traces resume" to avoid too many traces during FW download

 Adding more flags to manage the init phases separately (some TER commands may not execute when in SAT, and vice-versa)
  Si2168_L2_SW_Init:
   front_end->first_init_done     = 0;
   front_end->TER_tuner_init_done = 0;
   front_end->SAT_tuner_init_done = 0;
  Si2168_L2_switch_to_standard:
   Tracing the flags statuses.
   Now performing the TER and SAT init only when using the corresponding media.
   Added more traces and error messages.
   Proper management of Si2168_WAKEUP

 As from 2.1.1:
 (only for ROM1 parts)
 DVB-T scan working (with AUTO_T_T2)
 DVB-C blindscan working
 SAT   blindscan working (with AUTO_S_S2)

 In Si2168_L2_switch_to_standard:
  Handling TER and SAT configuration separately, because some commands require the corresponding media's clock to be active

 In Si2168_L2_lock_to_carrier:
  calling Si2168_L2_Tune after setting dd_mode.bw, as Si2168_L2_Tune uses this variable to set the tuner bw
  forcing bw to 8 MHz for DVB-C

 As from 2.1.0:
  compatibility with Si2168_A10.
   There is now a need to define Si2168_0A_COMPATIBLE or Si2168_10_COMPATIBLE (or both) to select which version(s) can be used
  In Si2168_L2_switch_to_standard:
   removing error message in case of error during Si2168_L2_Tuner_I2C_Enable,
    as this is expected, depending if the function is called after a HW reset or not.
  In Si2168_L2_Tune:
   Limiting bw to 6 MHz min
   Disabling FEF input in TER tuner (if any) in all 'TER' cases
   Enabling  FEF input in TER tuner (if any) only for DVB-T2

****************************************************************************************/

/* Si2168 API Specific Includes */
#include "Si2168_L2_API.h"               /* Include file for this code */


#ifdef    Si2167B_1A_COMPATIBLE
 #include "Si2167B_ROM0_Firmware_2_0b9.h"       /* firmware compatible with Si2167B_1A */
 #ifndef   Si2167B_LoadFirmware
  #define  Si2167B_LoadFirmware       Si2168_LoadFirmware
  #define  NO_Si2167B_ERROR           NO_Si2168_ERROR
  #define  Si2167B_L1_API_ERROR_TEXT  Si2168_L1_API_ERROR_TEXT
 #endif /* Si2167B_LoadFirmware */
#endif /* Si2167B_1A_COMPATIBLE */
#ifdef    Si2167B_20_COMPATIBLE
 #include "Si2167B_ROM0_Patch_2_0b9.h"          /* firmware compatible with Si2167B_20 */
 #ifndef   Si2167B_LoadFirmware
  #define  Si2167B_LoadFirmware       Si2168_LoadFirmware
  #define  NO_Si2167B_ERROR           NO_Si2168_ERROR
  #define  Si2167B_L1_API_ERROR_TEXT  Si2168_L1_API_ERROR_TEXT
 #endif /* Si2167B_LoadFirmware */
#endif /* Si2167B_20_COMPATIBLE */
#ifdef    Si2168_0B_COMPATIBLE
 #include "Si2168_0B_ROM1_Firmware_0_Cb20.h"    /* firmware compatible with Si2168-0B ROM 1 */
#endif /* Si2168_0B_COMPATIBLE */
#ifdef    Si2168_10_COMPATIBLE
 #include "Si2168_10_ROM1_Patch_0_Cb20.h"       /* patch    compatible with Si2168-10 ROM 1 */
#endif /* Si2168_10_COMPATIBLE */
#ifdef    Si2168_20_COMPATIBLE
 #include "Si2168_20_ROM2_Patch_2_0b19.h"       /* patch    compatible with Si2168-20 ROM 2 */
#endif /* Si2168_20_COMPATIBLE */
#ifdef    Si2168_30_COMPATIBLE
 #include "Si2168_30_ROM3_Patch_3_0b2.h"        /* patch    compatible with Si2168-30 ROM 3 */
#endif /* Si2168_30_COMPATIBLE */



#define Si2168_BYTES_PER_LINE 8

  int fef_selection;
  int fef_mode;

/************************************************************************************************************************
  NAME: Si2168_Configure
  DESCRIPTION: Setup TER and SAT AGCs, Common Properties startup
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
************************************************************************************************************************/
int Si2168_Configure           (L1_Si2168_Context *api)
{
    int return_code;
    return_code = NO_Si2168_ERROR;

    SiTRACE("media %d\n",api->media);

    /* AGC settings when not used */
    api->cmd->dd_mp_defaults.mp_a_mode   = Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_DISABLE;
    if (fef_mode == Si2168_FEF_MODE_FREEZE_PIN) {
      api->cmd->dd_mp_defaults.mp_b_mode   = Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_DRIVE_0;
    } else {
    api->cmd->dd_mp_defaults.mp_b_mode   = Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_DISABLE;
    }
    api->cmd->dd_mp_defaults.mp_c_mode   = Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_DISABLE;
    api->cmd->dd_mp_defaults.mp_d_mode   = Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_DISABLE;
    Si2168_L1_SendCommand2(api, Si2168_DD_MP_DEFAULTS_CMD_CODE);

    /* LEDS MANAGEMENT */
    /* set hardware lock on green LED */
    api->cmd->config_pins.gpio0_mode     = Si2168_CONFIG_PINS_CMD_GPIO0_MODE_HW_LOCK;
    api->cmd->config_pins.gpio0_read     = Si2168_CONFIG_PINS_CMD_GPIO0_READ_DO_NOT_READ;

    if (api->media == Si2168_TERRESTRIAL) {
      /* Settings for TER AGC. These settings need to match the HW design for TER AGCs */
      api->cmd->dd_ext_agc_ter.agc_1_mode  = Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_NOT_USED;
      #ifdef    Si2168_TER_AGC_INVERTED
      api->cmd->dd_ext_agc_ter.agc_1_inv   = Si2168_DD_EXT_AGC_TER_CMD_AGC_1_INV_INVERTED;
      #else  /* Si2168_TER_AGC_INVERTED */
      api->cmd->dd_ext_agc_ter.agc_1_inv   = Si2168_DD_EXT_AGC_TER_CMD_AGC_1_INV_NOT_INVERTED;
      #endif /* Si2168_TER_AGC_INVERTED */
      api->cmd->dd_ext_agc_ter.agc_1_kloop = Si2168_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_MIN;
      api->cmd->dd_ext_agc_ter.agc_1_min   = Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MIN_MIN;

      api->cmd->dd_ext_agc_ter.agc_2_mode  = Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_A;
      #ifdef    Si2168_TER_AGC_INVERTED
      api->cmd->dd_ext_agc_ter.agc_2_inv   = Si2168_DD_EXT_AGC_TER_CMD_AGC_2_INV_INVERTED;
      #else  /* Si2168_TER_AGC_INVERTED */
      api->cmd->dd_ext_agc_ter.agc_2_inv   = Si2168_DD_EXT_AGC_TER_CMD_AGC_2_INV_NOT_INVERTED;
      #endif /* Si2168_TER_AGC_INVERTED */
      api->cmd->dd_ext_agc_ter.agc_2_kloop = 18;
      api->cmd->dd_ext_agc_ter.agc_2_min   = Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MIN_MIN;
      Si2168_L1_SendCommand2(api, Si2168_DD_EXT_AGC_TER_CMD_CODE);

      if (fef_mode == Si2168_FEF_MODE_FREEZE_PIN) {
      /* SET FEF capability ON MP_B pins and Active HIGH*/
      api->cmd->dvbt2_fef.fef_tuner_flag      = Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_B;
      api->cmd->dvbt2_fef.fef_tuner_flag_inv  = Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_FEF_HIGH;
      } else {
        api->cmd->dvbt2_fef.fef_tuner_flag      = Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_NOT_USED;
      }
      Si2168_L1_SendCommand2(api, Si2168_DVBT2_FEF_CMD_CODE);
    }


    api->cmd->config_pins.gpio1_mode     = Si2168_CONFIG_PINS_CMD_GPIO1_MODE_TS_ERR;
    api->cmd->config_pins.gpio1_read     = Si2168_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ;
    Si2168_L1_SendCommand2(api, Si2168_CONFIG_PINS_CMD_CODE);

    /* Set All Properties startup configuration */
    Si2168_setupAllDefaults     (api);

    /* TS settings in parallel */
    api->prop->dd_ts_mode.mode           = Si2168_DD_TS_MODE_PROP_MODE_SERIAL;
    api->prop->dd_ts_mode.clock          = Si2168_DD_TS_MODE_PROP_CLOCK_AUTO_ADAPT;

    Si2168_downloadAllProperties(api);


#ifdef    USB_Capability
    /* Setting GPIF clock to not_inverted to allow TS over USB transfer */
    Si2168_L1_DD_SET_REG  (api, 0 , 35, 1, 0);
#endif /* USB_Capability */

    return return_code;
}
/************************************************************************************************************************
  NAME: Si2168_STANDBY
  DESCRIPTION:
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
************************************************************************************************************************/
int Si2168_STANDBY             (L1_Si2168_Context *api)
{
    return Si2168_L1_POWER_DOWN (api);
}
/************************************************************************************************************************
  NAME: Si2168_WAKEUP
  DESCRIPTION:
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
************************************************************************************************************************/
int Si2168_WAKEUP              (L1_Si2168_Context *api)
{
    int return_code;
    int media;

    return_code = NO_Si2168_ERROR;
    media       = Si2168_Media(api, api->standard);
    SiTRACE ("Si2168_WAKEUP: media %d\n", media);

    /* Clock source selection */
    switch (media) {
      default                                  :
      case Si2168_TERRESTRIAL : {
        api->cmd->start_clk.clk_mode = api->tuner_ter_clock_input;
        break;
      }
    }

    Si2168_L1_START_CLK (api,
                            Si2168_START_CLK_CMD_SUBCODE_CODE,
                            Si2168_START_CLK_CMD_RESERVED1_RESERVED,
                            Si2168_START_CLK_CMD_TUNE_CAP_15P6,
                            Si2168_START_CLK_CMD_RESERVED2_RESERVED,
                            api->cmd->start_clk.clk_mode,
                            Si2168_START_CLK_CMD_RESERVED3_RESERVED,
                            Si2168_START_CLK_CMD_RESERVED4_RESERVED,
                            Si2168_START_CLK_CMD_START_CLK_START_CLK);
    /* Reference frequency selection */
    switch (media) {
      default                 :
      case Si2168_TERRESTRIAL : {
        if (api->tuner_ter_clock_freq == 16) {
          SiTRACE("Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_16MHZ\n");
          api->cmd->power_up.clock_freq = Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_16MHZ;
        } else if (api->tuner_ter_clock_freq == 24) {
          SiTRACE("Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ\n");
          api->cmd->power_up.clock_freq = Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ;
        } else {
          SiTRACE("Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_27MHZ\n");
          api->cmd->power_up.clock_freq = Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_27MHZ;
        }
        break;
      }
    }

    return_code = Si2168_L1_POWER_UP (api,
                            Si2168_POWER_UP_CMD_SUBCODE_CODE,
                            api->cmd->power_up.reset,
                            Si2168_POWER_UP_CMD_RESERVED2_RESERVED,
                            Si2168_POWER_UP_CMD_RESERVED4_RESERVED,
                            Si2168_POWER_UP_CMD_RESERVED1_RESERVED,
                            Si2168_POWER_UP_CMD_ADDR_MODE_CURRENT,
                            Si2168_POWER_UP_CMD_RESERVED5_RESERVED,
                            api->cmd->power_up.func,
                            api->cmd->power_up.clock_freq,
                            Si2168_POWER_UP_CMD_CTSIEN_DISABLE,
                            Si2168_POWER_UP_CMD_WAKE_UP_WAKE_UP);

         if (api->cmd->start_clk.clk_mode == Si2168_START_CLK_CMD_CLK_MODE_CLK_CLKIO  ) { SiTRACE ("Si2168_START_CLK_CMD_CLK_MODE_CLK_CLKIO\n"  );}
    else if (api->cmd->start_clk.clk_mode == Si2168_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN) { SiTRACE ("Si2168_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN\n");}
    else if (api->cmd->start_clk.clk_mode == Si2168_START_CLK_CMD_CLK_MODE_XTAL       ) { SiTRACE ("Si2168_START_CLK_CMD_CLK_MODE_XTAL\n"       );}

         if (api->cmd->power_up.reset == Si2168_POWER_UP_CMD_RESET_RESET  ) { SiTRACE ("Si2168_POWER_UP_CMD_RESET_RESET\n"  );}
    else if (api->cmd->power_up.reset == Si2168_POWER_UP_CMD_RESET_RESUME ) { SiTRACE ("Si2168_POWER_UP_CMD_RESET_RESUME\n");}

    if (return_code != NO_Si2168_ERROR ) {
      SiTRACE("Si2168_WAKEUP: POWER_UP ERROR!\n");
      SiERROR("Si2168_WAKEUP: POWER_UP ERROR!\n");
      return return_code;
    }
    /* After a successful POWER_UP, set values for 'resume' only */
    api->cmd->power_up.reset = Si2168_POWER_UP_CMD_RESET_RESUME;
    api->cmd->power_up.func  = Si2168_POWER_UP_CMD_FUNC_NORMAL;

    return NO_Si2168_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168_PowerUpWithPatch
  DESCRIPTION: Send Si2168 API PowerUp Command with PowerUp to bootloader,
  Check the Chip rev and part, and ROMID are compared to expected values.
  Load the Firmware Patch then Start the Firmware.
  Programming Guide Reference:    Flowchart A.2 (POWER_UP with patch flowchart)

  Parameter:  pointer to Si2168 Context
  Returns:    Si2168/I2C transaction error code, NO_Si2168_ERROR if successful
************************************************************************************************************************/
int Si2168_PowerUpWithPatch    (L1_Si2168_Context *api)
{
    int return_code;
    int fw_loaded;
    return_code = NO_Si2168_ERROR;
    fw_loaded   = 0;

    /* Before patching, set POWER_UP values for 'RESET' and 'BOOTLOADER' */
    api->cmd->power_up.reset = Si2168_POWER_UP_CMD_RESET_RESET;
    api->cmd->power_up.func  = Si2168_POWER_UP_CMD_FUNC_BOOTLOADER,

    return_code = Si2168_WAKEUP(api);

    if (return_code != NO_Si2168_ERROR) {
      SiERROR("Si2168_PowerUpWithPatch: WAKEUP error!\n");
        return return_code;
    }

    /* Get the Part Info from the chip.   This command is only valid in Bootloader mode */
    if ((return_code = Si2168_L1_PART_INFO(api)) != NO_Si2168_ERROR) {
        SiTRACE ("Si2168_L1_PART_INFO error 0x%02x: %s\n", return_code, Si2168_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }
    SiTRACE("chiprev %d\n",        api->rsp->part_info.chiprev);
    SiTRACE("part    Si21%d\n",    api->rsp->part_info.part   );
    SiTRACE("romid   %d\n",        api->rsp->part_info.romid  );
    SiTRACE("pmajor  0x%02x\n",    api->rsp->part_info.pmajor );
    SiTRACE("pminor  0x%02x\n",    api->rsp->part_info.pminor );
    SiTRACE("pbuild  %d\n",        api->rsp->part_info.pbuild );
    if ((api->rsp->part_info.pmajor >= 0x30) & (api->rsp->part_info.pminor >= 0x30)) {
    SiTRACE("pmajor  '%c'\n",    api->rsp->part_info.pmajor );
    SiTRACE("pminor  '%c'\n",    api->rsp->part_info.pminor );
    SiTRACE("Full Info       'Si21%2d_ROM%x_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.romid, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
    }

    /* Check part info values and load the proper firmware */
#ifdef    Si2167B_1A_COMPATIBLE
    #ifdef    Si2167B_FIRMWARE_2_0b9_LINES
    if (!fw_loaded) {
      SiTRACE  ("Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2167B_FIRMWARE_2_0b9_PART, Si2167B_FIRMWARE_2_0b9_ROM, Si2167B_FIRMWARE_2_0b9_PMAJOR, Si2167B_FIRMWARE_2_0b9_PMINOR, Si2167B_FIRMWARE_2_0b9_PBUILD );
      if ((api->rsp->part_info.romid  == Si2167B_FIRMWARE_2_0b9_ROM   )
        & (api->rsp->part_info.part   == Si2167B_FIRMWARE_2_0b9_PART  )
        & (api->rsp->part_info.pmajor == Si2167B_FIRMWARE_2_0b9_PMAJOR)
        & (api->rsp->part_info.pminor == Si2167B_FIRMWARE_2_0b9_PMINOR)
        & (api->rsp->part_info.pbuild == Si2167B_FIRMWARE_2_0b9_PBUILD)) {
        printf ("Downloading 'Si21%2d_FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        SiTRACE("Downloading 'Si21%2d_FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2167B_LoadFirmware(api, Si2167B_FIRMWARE_2_0b9, Si2167B_FIRMWARE_2_0b9_LINES)) != NO_Si2167B_ERROR) {
          SiTRACE ("Si2167B_LoadFirmware error 0x%02x: %s\n", return_code, Si2167B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        fw_loaded++;
      }
    }
    #endif /* Si2167B_FIRMWARE_2_0b9_LINES */
#endif /* Si2167B_1A_COMPATIBLE */
#ifdef    Si2167B_20_COMPATIBLE
    #ifdef    Si2167B_PATCH_2_0b9_LINES
    if (!fw_loaded) {
      SiTRACE  ("Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2167B_PATCH_2_0b9_PART, Si2167B_PATCH_2_0b9_ROM, Si2167B_PATCH_2_0b9_PMAJOR, Si2167B_PATCH_2_0b9_PMINOR, Si2167B_PATCH_2_0b9_PBUILD );
      if ((api->rsp->part_info.romid  == Si2167B_PATCH_2_0b9_ROM   )
        & (api->rsp->part_info.part   == Si2167B_PATCH_2_0b9_PART  )
        & (api->rsp->part_info.pmajor == Si2167B_PATCH_2_0b9_PMAJOR)
        & (api->rsp->part_info.pminor == Si2167B_PATCH_2_0b9_PMINOR)
        & (api->rsp->part_info.pbuild == Si2167B_PATCH_2_0b9_PBUILD)) {
        printf ("Downloading 'Si21%2d_FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        SiTRACE("Downloading 'Si21%2d_FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2167B_LoadFirmware(api, Si2167B_PATCH_2_0b9, Si2167B_PATCH_2_0b9_LINES)) != NO_Si2167B_ERROR) {
          SiTRACE ("Si2167B_LoadPatch error 0x%02x: %s\n", return_code, Si2167B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        fw_loaded++;
      }
    }
    #endif /* Si2167B_PATCH_2_0b9_LINES */
#endif /* Si2167B_20_COMPATIBLE */
#ifdef    Si2168_0B_COMPATIBLE
    #ifdef    Si2168_FIRMWARE_0_Cb20_LINES
    if (!fw_loaded) {
      SiTRACE  ("Is this part a  'Si21%2d_ROM%x_%d_%d_b%d'?\n", Si2168_FIRMWARE_0_Cb20_PART, Si2168_FIRMWARE_0_Cb20_ROM, Si2168_FIRMWARE_0_Cb20_PMAJOR, Si2168_FIRMWARE_0_Cb20_PMINOR, Si2168_FIRMWARE_0_Cb20_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168_FIRMWARE_0_Cb20_ROM   )
        & (api->rsp->part_info.part   == Si2168_FIRMWARE_0_Cb20_PART  )
        & (api->rsp->part_info.pmajor == Si2168_FIRMWARE_0_Cb20_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168_FIRMWARE_0_Cb20_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168_FIRMWARE_0_Cb20_PBUILD)) {
        SiTRACE("Updating FW for 'Si21%2d_FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2168_LoadFirmware(api, Si2168_FIRMWARE_0_Cb20, Si2168_FIRMWARE_0_Cb20_LINES)) != NO_Si2168_ERROR) {
          SiTRACE ("Si2168_LoadFirmware error 0x%02x: %s\n", return_code, Si2168_L1_API_ERROR_TEXT(return_code) );
          return return_code;
      }
        fw_loaded++;
      }
    }
    #endif /* Si2168_FIRMWARE_0_Cb20_LINES */
#endif /* Si2168_0B_COMPATIBLE */
#ifdef    Si2168_10_COMPATIBLE
    #ifdef    Si2168_PATCH_0_Cb20_LINES
    if (!fw_loaded) {
      SiTRACE  ("Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168_PATCH_0_Cb20_PART, Si2168_PATCH_0_Cb20_ROM, Si2168_PATCH_0_Cb20_PMAJOR, Si2168_PATCH_0_Cb20_PMINOR, Si2168_PATCH_0_Cb20_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168_PATCH_0_Cb20_ROM   )
        & (api->rsp->part_info.part   == Si2168_PATCH_0_Cb20_PART  )
        & (api->rsp->part_info.pmajor == Si2168_PATCH_0_Cb20_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168_PATCH_0_Cb20_PMINOR)) {
        SiTRACE("Updating FW for 'Si21%2d_FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2168_LoadFirmware(api, Si2168_PATCH_0_Cb20, Si2168_PATCH_0_Cb20_LINES)) != NO_Si2168_ERROR) {
            SiTRACE ("Si2168_LoadFirmware error 0x%02x: %s\n", return_code, Si2168_L1_API_ERROR_TEXT(return_code) );
            return return_code;
          }
        fw_loaded++;
      }
    }
    #endif /* Si2168_PATCH_0_Cb20_LINES */
#endif /* Si2168_10_COMPATIBLE */
#ifdef    Si2168_20_COMPATIBLE
    #ifdef    Si2168_PATCH_2_0b19_LINES
    if (!fw_loaded) {
      SiTRACE  ("Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168_PATCH_2_0b19_PART, Si2168_PATCH_2_0b19_ROM, Si2168_PATCH_2_0b19_PMAJOR, Si2168_PATCH_2_0b19_PMINOR, Si2168_PATCH_2_0b19_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168_PATCH_2_0b19_ROM   )
        &((api->rsp->part_info.part   == 69  ) || (api->rsp->part_info.part   == 68  ))
        & (api->rsp->part_info.pmajor == Si2168_PATCH_2_0b19_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168_PATCH_2_0b19_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168_PATCH_2_0b19_PBUILD)) {
        SiTRACE("Updating FW for 'Si21%2d_ROM%x %c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.romid, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2168_LoadFirmware(api, Si2168_PATCH_2_0b19, Si2168_PATCH_2_0b19_LINES)) != NO_Si2168_ERROR) {
            SiTRACE ("Si2168_LoadFirmware error 0x%02x: %s\n", return_code, Si2168_L1_API_ERROR_TEXT(return_code) );
            return return_code;
          }
        fw_loaded++;
        }
    }
    #endif /* Si2168_PATCH_2_0b19_LINES */
#endif /* Si2168_20_COMPATIBLE */
#ifdef    Si2168_30_COMPATIBLE
    #ifdef    Si2168_PATCH_3_0b2_LINES
    if (!fw_loaded) {
      SiTRACE  ("Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168_PATCH_3_0b2_PART, Si2168_PATCH_3_0b2_ROM, Si2168_PATCH_3_0b2_PMAJOR, Si2168_PATCH_3_0b2_PMINOR, Si2168_PATCH_3_0b2_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168_PATCH_3_0b2_ROM   )
        & (api->rsp->part_info.part   == Si2168_PATCH_3_0b2_PART  )
        & (api->rsp->part_info.pmajor == Si2168_PATCH_3_0b2_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168_PATCH_3_0b2_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168_PATCH_3_0b2_PBUILD)) {
        SiTRACE("Updating FW for 'Si21%2d_ROM%x %c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.romid, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2168_LoadFirmware(api, Si2168_PATCH_3_0b2, Si2168_PATCH_3_0b2_LINES)) != NO_Si2168_ERROR) {
            SiTRACE ("Si2168_LoadFirmware error 0x%02x: %s\n", return_code, Si2168_L1_API_ERROR_TEXT(return_code) );
            return return_code;
          }
        fw_loaded++;
        }
    }
    #endif /* Si2168_PATCH_3_0b2_LINES */
#endif /* Si2168_0_COMPATIBLE */

    if (!fw_loaded) {
      SiTRACE ("Si2168_LoadFirmware error: NO Firmware Loaded! Possible part/code incompatibility !\n");
      SiERROR ("Si2168_LoadFirmware error: NO Firmware Loaded! Possible part/code incompatibility !\n");
      return ERROR_Si2168_LOADING_FIRMWARE;
    }

    /*Start the Firmware */
    if ((return_code = Si2168_StartFirmware(api)) != NO_Si2168_ERROR) {
       /* Start firmware */
        SiTRACE ("Si2168_StartFirmware error 0x%02x: %s\n", return_code, Si2168_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }

    Si2168_L1_GET_REV (api);
    SiTRACE("Si21%2d Part running 'FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->get_rev.cmpmajor, api->rsp->get_rev.cmpminor, api->rsp->get_rev.cmpbuild );

    return NO_Si2168_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168_LoadFirmware
  DESCRIPTON: Load firmware from FIRMWARE_TABLE array in Si2168_Firmware_x_y_build_z.h file into Si2168
              Requires Si2168 to be in bootloader mode after PowerUp
  Programming Guide Reference:    Flowchart A.3 (Download FW PATCH flowchart)

  Parameter:  Si2168 Context (I2C address)
  Parameter:  pointer to firmware table array
  Parameter:  number of lines in firmware table array (size in bytes / BYTES_PER_LINE)
  Returns:    Si2168/I2C transaction error code, NO_Si2168_ERROR if successful
************************************************************************************************************************/
int Si2168_LoadFirmware        (L1_Si2168_Context *api, unsigned char fw_table[], int nbLines)
{
    int return_code;
    int line;
    return_code = NO_Si2168_ERROR;

    SiTRACE ("Si2168_LoadFirmware starting...\n");
    SiTRACE ("Si2168_LoadFirmware nbLines %d\n", nbLines);

    /* for each line in fw_table */
    for (line = 0; line < nbLines; line++)
    {
        /* send Si2168_BYTES_PER_LINE fw bytes to Si2168 */
        if ((return_code = Si2168_L1_API_Patch(api, Si2168_BYTES_PER_LINE, fw_table + Si2168_BYTES_PER_LINE*line)) != NO_Si2168_ERROR)
        {
          SiTRACE("Si2168_LoadFirmware error 0x%02x patching line %d: %s\n", return_code, line, Si2168_L1_API_ERROR_TEXT(return_code) );
          if (line == 0) {
          SiTRACE("The firmware is incompatible with the part!\n");
          }
          return ERROR_Si2168_LOADING_FIRMWARE;
        }
        if (line==3) {SiTraceConfiguration("traces suspend");}
    }
    SiTraceConfiguration("traces resume");
    SiTRACE ("Si2168_LoadFirmware complete...\n");
    return NO_Si2168_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168_StartFirmware
  DESCRIPTION: Start Si2168 firmware (put the Si2168 into run mode)
  Parameter:   Si2168 Context (I2C address)
  Parameter (passed by Reference):   ExitBootloadeer Response Status byte : tunint, atvint, dtvint, err, cts
  Returns:     I2C transaction error code, NO_Si2168_ERROR if successful
************************************************************************************************************************/
int Si2168_StartFirmware       (L1_Si2168_Context *api)
{

    if (Si2168_L1_EXIT_BOOTLOADER(api, Si2168_EXIT_BOOTLOADER_CMD_FUNC_NORMAL, Si2168_EXIT_BOOTLOADER_CMD_CTSIEN_OFF) != NO_Si2168_ERROR)
    {
        return ERROR_Si2168_STARTING_FIRMWARE;
    }

    return NO_Si2168_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168_Init
  DESCRIPTION:Reset and Initialize Si2168
  Parameter:  Si2168 Context (I2C address)
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
************************************************************************************************************************/
int Si2168_Init                (L1_Si2168_Context *api)
{
    int return_code;
    SiTRACE("Si2168_Init starting...\n");

    if ((return_code = Si2168_PowerUpWithPatch(api)) != NO_Si2168_ERROR) {   /* PowerUp into bootloader */
        SiTRACE ("Si2168_PowerUpWithPatch error 0x%02x: %s\n", return_code, Si2168_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }
    SiTRACE("Si2168_Init complete...\n");
    return NO_Si2168_ERROR;
}
/************************************************************************************************************************
  Si2168_Media function
  Use:        media retrieval function
              Used to retrieve the media used by the Si2168
************************************************************************************************************************/
int  Si2168_Media              (L1_Si2168_Context *api, int modulation)
{
  switch (modulation) {
    default             :
    case Si2168_DD_MODE_PROP_MODULATION_AUTO_DETECT : {
      switch (api->prop->dd_mode.auto_detect) {
        default             :
        case Si2168_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2     : return Si2168_TERRESTRIAL; break;
      }
    }
    case Si2168_DD_MODE_PROP_MODULATION_DVBT :
    case Si2168_DD_MODE_PROP_MODULATION_DVBT2:
    case Si2168_DD_MODE_PROP_MODULATION_DVBC : return Si2168_TERRESTRIAL; break;
  }
  SiERROR("UNKNOWW media!\n");
  return 0;
}
/*****************************************************************************************/
/*               SiLabs demodulator API functions (demod and tuner)                      */
/*****************************************************************************************/
/* Allow profiling information during Si2168_switch_to_standard */
#define PROFILING
/************************************************************************************************************************
  Si2168_standardName function
  Use:        standard text retrieval function
              Used to retrieve the standard text used by the Si2168
  Parameter:  standard, the value of the standard
************************************************************************************************************************/
char *Si2168_standardName (int standard)
{
  switch (standard)
  {
    case Si2168_DD_MODE_PROP_MODULATION_DVBT    : {return (char*)"DVB-T"  ;}
    case Si2168_DD_MODE_PROP_MODULATION_DVBC    : {return (char*)"DVB-C"  ;}
    case Si2168_DD_MODE_PROP_MODULATION_DVBT2   : {return (char*)"DVB-T2" ;}
    default                                     : {return (char*)"UNKNOWN";}
  }
}
/************************************************************************************************************************
  Si2168_L2_Infos function
  Use:        software information function
              Used to retrieve information about the compilation
  Parameter:  front_end, a pointer to the Si2168_L2_Context context to be initialized
  Parameter:  infoString, a text buffer to be filled with teh information. It must be initialized by the caller.
  Return:     the length of the information string
************************************************************************************************************************/
int  Si2168_L2_Infos              (Si2168_L2_Context *front_end, char *infoString)
{
#if 0
    if (infoString == NULL) return 0;
    if (front_end  == NULL) {
      sprintf(infoString, "Si2168 front-end not initialized yet. Call Si2168_L2_SW_Init first!\n");
      return strlen(infoString);
    }

    sprintf(infoString, "\n");
    sprintf(infoString, "%s--------------------------------------\n", infoString);
    sprintf(infoString, "%sSi_I2C               Source code %s\n"   , infoString, Si_I2C_TAG_TEXT() );
    sprintf(infoString, "%sDemod                Si2168  at 0x%02x\n", infoString , front_end->demod->i2c->address);
    sprintf(infoString, "%sDemod                Source code %s\n"   , infoString , Si2168_L1_API_TAG_TEXT() );
#ifdef    TERRESTRIAL_FRONT_END
#ifdef    TER_TUNER_Si2146
    sprintf(infoString, "%sTerrestrial tuner    Si2146  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2148
    sprintf(infoString, "%sTerrestrial tuner    Si2148  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2156
    sprintf(infoString, "%sTerrestrial tuner    Si2156  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2158
    sprintf(infoString, "%sTerrestrial tuner    Si2158  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2173
    sprintf(infoString, "%sTerrestrial tuner    Si2173  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
    sprintf(infoString, "%sTerrestrial tuner    Si2176  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2178
    sprintf(infoString, "%sTerrestrial tuner    Si2178  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2190
    sprintf(infoString, "%sTerrestrial tuner    Si2190  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2196
    sprintf(infoString, "%sTerrestrial tuner    Si2196  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_Si2196 */
#ifdef    TER_TUNER_NO_TER
    sprintf(infoString, "%sTerrestrial tuner    NO_TER  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_NO_TER */
#ifdef    TER_TUNER_TAG_TEXT
    sprintf(infoString, "%sTerrestrial tuner    Source code %s\n"   , infoString , TER_TUNER_TAG_TEXT() );
#endif /* TER_TUNER_TAG_TEXT */
#endif /* TERRESTRIAL_FRONT_END */


#ifdef    TERRESTRIAL_FRONT_END
  if ( Si2168_TER_CLOCK_SOURCE == Si2168_TER_Tuner_clock) sprintf(infoString, "%sTER clock from  TER Tuner (%d MHz)\n", infoString , Si2168_REF_FREQUENCY_TER);
  if ( Si2168_TER_CLOCK_SOURCE == Si2168_SAT_Tuner_clock) sprintf(infoString, "%sTER clock from  SAT Tuner (%d MHz)\n", infoString , Si2168_REF_FREQUENCY_TER);
  if ( Si2168_TER_CLOCK_SOURCE == Si2168_Xtal_clock     ) sprintf(infoString, "%sTER clock from  Xtal      (%d MHz)\n", infoString , Si2168_REF_FREQUENCY_TER);
  if ( Si2168_CLOCK_MODE_TER   == Si2168_START_CLK_CMD_CLK_MODE_CLK_CLKIO  ) sprintf(infoString, "%sTER clock input CLKIO\n"  , infoString);
  if ( Si2168_CLOCK_MODE_TER   == Si2168_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN) sprintf(infoString, "%sTER clock input XTAL_IN\n", infoString);
  if ( Si2168_CLOCK_MODE_TER   == Si2168_START_CLK_CMD_CLK_MODE_XTAL       ) sprintf(infoString, "%sTER clock input XTAL\n"   , infoString);

  if (fef_mode                 == Si2168_FEF_MODE_SLOW_NORMAL_AGC          ) sprintf(infoString, "%sFEF mode 'SLOW NORMAL AGC'" , infoString);
  if (fef_mode                 == Si2168_FEF_MODE_SLOW_INITIAL_AGC         ) sprintf(infoString, "%sFEF mode 'SLOW INITIAL AGC'", infoString);
  if (fef_mode                 == Si2168_FEF_MODE_FREEZE_PIN               ) sprintf(infoString, "%sFEF mode 'FREEZE PIN'"      , infoString);
  if (fef_mode                 != fef_selection                            ) sprintf(infoString, "%s(CHANGED!)"                 , infoString);
  sprintf(infoString, "%s\n", infoString);
#endif /* TERRESTRIAL_FRONT_END */


  sprintf(infoString, "%s--------------------------------------\n", infoString);
  return strlen(infoString);

#endif
  return 0;
}
/************************************************************************************************************************
  Si2168_L2_SW_Init function
  Use:        software initialization function
              Used to initialize the Si2168 and tuner structures
  Behavior:   This function performs all the steps necessary to initialize the Si2168 and tuner instances
  Parameter:  front_end, a pointer to the Si2168_L2_Context context to be initialized
  Parameter:  demodAdd, the I2C address of the demod
  Parameter:  tunerAdd, the I2C address of the tuner
  Comments:     It MUST be called first and once before using any other function.
                It can be used to build a multi-demod/multi-tuner application, if called several times from the upper layer with different pointers and addresses
                After execution, all demod and tuner functions are accessible.
************************************************************************************************************************/
char Si2168_L2_SW_Init            (Si2168_L2_Context *front_end
                                   , int demodAdd
                                   , int tunerAdd_Ter
								   , Endeavour* endeavour
								   , unsigned char endeavourChipidx
								   , unsigned char i2cbus
                                   )
{
    char infoStringBuffer[1000];
    char *infoString;
    infoString = &(infoStringBuffer[0]);

    SiTRACE("Si2168_L2_EVB_SW_Init starting...\n");

    /* Pointers initialization */
    front_end->demod     = &(front_end->demodObj    );
    front_end->Si2168_init_done    = 0;
    front_end->first_init_done     = 0;
    front_end->handshakeUsed       = 0; /* set to '0' by default for compatibility with previous versions */
    front_end->handshakePeriod_ms  = 1000;
    front_end->tuner_ter = &(front_end->tuner_terObj);
    front_end->TER_init_done       = 0;
    front_end->TER_tuner_init_done = 0;
    front_end->auto_detect_TER     = 1;
    /* Calling underlying SW initialization functions */
    Si2168_L1_API_Init      (front_end->demod, demodAdd, endeavour, endeavourChipidx, i2cbus);
    L1_RF_TER_TUNER_Init    (front_end->tuner_ter, tunerAdd_Ter, endeavour, endeavourChipidx, i2cbus);
    fef_mode      = Si2168_FEF_MODE_SLOW_NORMAL_AGC;
    fef_selection = Si2168_FEF_MODE;
 #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP
    /* If the TER tuner has initial AGC speed control and it's the selected mode, activate it */
    if (fef_selection == Si2168_FEF_MODE_SLOW_INITIAL_AGC) {
      SiTRACE("TER tuner FEF set to 'SLOW_INITIAL_AGC' mode\n");
      fef_mode = Si2168_FEF_MODE_SLOW_INITIAL_AGC;
    }
  #ifdef    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP
    /* If the TER tuner has an AGC freeze pin and it's the selected mode, activate it */
    if (fef_selection == Si2168_FEF_MODE_FREEZE_PIN      ) {
      SiTRACE("TER tuner FEF set to 'FREEZE_PIN' mode\n");
      fef_mode = Si2168_FEF_MODE_FREEZE_PIN;
    }
  #else  /* L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP */
    if (fef_selection == Si2168_FEF_MODE_FREEZE_PIN      ) {
      SiTRACE("TER tuner FEF can not use 'FREEZE_PIN' mode, using 'SLOW_INITIAL_AGC' mode instead\n");
      fef_mode = Si2168_FEF_MODE_SLOW_INITIAL_AGC;
    }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP */
 #else  /* L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP */
      SiTRACE("TER tuner FEF set to 'SLOW_NORMAL_AGC' mode\n");
 #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP */

#ifdef    SiTRACE
    if (Si2168_L2_Infos(front_end, infoString))  {SiTRACE("%s\n", infoString);}
#endif /* SiTRACE */
    SiTRACE("Si2168_L2_EVB_SW_Init complete\n");
    return 1;
}
/************************************************************************************************************************
  Si2168_L2_HW_Connect function
  Use:        Front-End connection function
              Specific to SiLabs USB connection!
  Porting:    Remove or replace by the final application corresponding calls
  Behavior:   This function connects the Si2168, and the tuners via the Cypress chip
  Parameter:  *front_end, the front-end handle
  Parameter   mode, the required connection mode
************************************************************************************************************************/
void Si2168_L2_HW_Connect         (Si2168_L2_Context *front_end)
{
}
/************************************************************************************************************************
  NAME: Si2168_L2_Part_Check
  DESCRIPTION:startup and part checking Si2168
  Parameter:  Si2168 Context
  Returns:    the part_info 'part' field value
************************************************************************************************************************/
int Si2168_L2_Part_Check          (Si2168_L2_Context   *front_end)
{
  int start_time_ms;
  front_end->demod->rsp->part_info.part = 0;
  start_time_ms = system_time();
  front_end->demod->cmd->power_up.reset = Si2168_POWER_UP_CMD_RESET_RESET;
  front_end->demod->cmd->power_up.func  = Si2168_POWER_UP_CMD_FUNC_BOOTLOADER,
#ifdef    SiTRACES
    SiTraceConfiguration("traces suspend");
#endif /* SiTRACES */
  Si2168_WAKEUP      (front_end->demod);
  Si2168_L1_PART_INFO(front_end->demod);
#ifdef    SiTRACES
  SiTraceConfiguration("traces resume");
#endif /* SiTRACES */
  SiTRACE("Si2168_Part_Check took %3d ms. Part is Si21%2d\n", system_time() - start_time_ms, front_end->demod->rsp->part_info.part );
  return front_end->demod->rsp->part_info.part;
}
/************************************************************************************************************************
  Si2168_L2_Tuner_I2C_Enable function
  Use:        Tuner i2c bus connection
              Used to allow communication with the tuners
  Parameter:  *front_end, the front-end handle
************************************************************************************************************************/
unsigned char Si2168_L2_Tuner_I2C_Enable (Si2168_L2_Context *front_end)
{
    return Si2168_L1_I2C_PASSTHROUGH(front_end->demod, Si2168_I2C_PASSTHROUGH_CMD_SUBCODE_CODE, Si2168_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_CLOSE, Si2168_I2C_PASSTHROUGH_CMD_RESERVED_RESERVED);
}
/************************************************************************************************************************
  Si2168_L2_Tuner_I2C_Disable function
  Use:        Tuner i2c bus connection
              Used to disconnect i2c communication with the tuners
  Parameter:  *front_end, the front-end handle
************************************************************************************************************************/
unsigned char Si2168_L2_Tuner_I2C_Disable(Si2168_L2_Context *front_end)
{
    return Si2168_L1_I2C_PASSTHROUGH(front_end->demod, Si2168_I2C_PASSTHROUGH_CMD_SUBCODE_CODE, Si2168_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_OPEN, Si2168_I2C_PASSTHROUGH_CMD_RESERVED_RESERVED);
}
/************************************************************************************************************************
  Si2168_L2_communication_check function
  Use:        Si2168 front i2c bus connection check
              Used to check i2c communication with the demod and the tuners
  Parameter:  *front_end, the front-end handle
************************************************************************************************************************/
int  Si2168_L2_communication_check(Si2168_L2_Context *front_end)
{
  int comm_errors;
  comm_errors=0;
  /* Close i2c Passthru       */
  Si2168_L2_Tuner_I2C_Enable(front_end);
  /* Check i2c Passthru value */
  if ((int)Si2168_L1_CheckStatus(front_end->demod) != NO_Si2168_ERROR) {
    SiTRACE ("DEMOD Communication error ! \n");
    comm_errors++;
  } else {
    SiTRACE ("DEMOD Communication OK\n");
  }
  /* Check TER tuner read     */
    #ifdef    TER_TUNER_COMM_CHECK
  if (TER_TUNER_COMM_CHECK(front_end->tuner_ter) !=1) {
    SiTRACE ("TER tuner Communication error ! \n");
    comm_errors++;
  } else {
    SiTRACE ("TER tuner Communication OK\n");
  }
    #endif /* TER_TUNER_COMM_CHECK */


  /* Open  i2c Passthru       */
  Si2168_L2_Tuner_I2C_Disable(front_end);

  if (comm_errors) return 0;

  return 1;
}
/************************************************************************************************************************
  Si2168_L2_switch_to_standard function
  Use:      Standard switching function selection
            Used to switch nicely to the wanted standard, taking into account the previous state
  Parameter: new_standard the wanted standard to switch to
  Behavior: This function positions a set of flags to easily decide what needs to be done to
              switch between standards.
************************************************************************************************************************/
int  Si2168_L2_switch_to_standard (Si2168_L2_Context *front_end, int new_standard, unsigned char force_full_init)
{
  /* previous state flags */
  int dtv_demod_already_used = 0;
  int ter_tuner_already_used = 0;
  int ter_clock_already_used = 0;
  /* new state flags      */
  int dtv_demod_needed       = 0;
  int ter_tuner_needed       = 0;
  int ter_clock_needed       = 0;
  int dtv_demod_sleep_request= 0;
  int res;

#ifdef    PROFILING
  int start;
  int ter_tuner_delay   = 0;
  int dtv_demod_delay   = 0;
  int switch_start;
  char sequence[100];
  #define TER_DELAY  ter_tuner_delay=ter_tuner_delay+system_time()-start;start=system_time();
  #define DTV_DELAY  dtv_demod_delay=dtv_demod_delay+system_time()-start;start=system_time();
#else
  #define TER_DELAY
  #define DTV_DELAY
#endif /* PROFILING */

#ifdef    PROFILING
  start = switch_start = system_time();
  SiTRACE("%s->%s\n", Si2168_standardName(front_end->previous_standard), Si2168_standardName(new_standard) );
#endif /* PROFILING */

  SiTRACE("Si2168_switch_to_standard starting...\n");
  SiTRACE("starting with Si2168_init_done %d, first_init_done     %d ", front_end->Si2168_init_done, front_end->first_init_done);
  SiTRACE("TER flags:    TER_init_done    %d, TER_tuner_init_done %d ", front_end->TER_init_done, front_end->TER_tuner_init_done);

  /* In this function is called for the first time, force a full init */
  if (front_end->first_init_done == 0) {force_full_init = 1;}
  /* ------------------------------------------------------------ */
  /* Set Previous Flags                                           */
  /* Setting flags representing the previous state                */
  /* NB: Any value not matching a known standard will init as ATV */
  /* Logic applied:                                               */
  /*  dtv demod was used for TERRESTRIAL and SATELLITE reception  */
  /*  ter tuner was used for TERRESTRIAL reception                */
  /*   and for SATELLITE reception if it is the SAT clock source  */
  /*  sat tuner was used for SATELLITE reception                  */
  /*   and for TERRESTRIAL reception if it is the TER clock source*/
  /* ------------------------------------------------------------ */
  switch (front_end->previous_standard) {
    case Si2168_DD_MODE_PROP_MODULATION_DVBT :
    case Si2168_DD_MODE_PROP_MODULATION_DVBT2:
    case Si2168_DD_MODE_PROP_MODULATION_DVBC : {
      dtv_demod_already_used = 1;
      ter_tuner_already_used = 1;
      if ( Si2168_TER_CLOCK_SOURCE == Si2168_TER_Tuner_clock) {
        ter_clock_already_used = 1;
      }
      break;
    }
    case Si2168_DD_MODE_PROP_MODULATION_ANALOG: {
      ter_tuner_already_used = 1;
      break;
    }
    default : /* SLEEP */   {
      ter_tuner_already_used = 0;
      break;
    }
  }

  /* ------------------------------------------------------------ */
  /* Set Needed Flags                                             */
  /* Setting flags representing the new state                     */
  /* Logic applied:                                               */
  /*  dtv demod is needed for TERRESTRIAL and SATELLITE reception */
  /*  ter tuner is needed for TERRESTRIAL reception               */
  /*   and for SATELLITE reception if it is the SAT clock source  */
  /*  sat tuner is needed for SATELLITE reception                 */
  /*   and for TERRESTRIAL reception if it is the TER clock source*/
  /* ------------------------------------------------------------ */
  switch (new_standard) {
    case Si2168_DD_MODE_PROP_MODULATION_DVBT :
    case Si2168_DD_MODE_PROP_MODULATION_DVBT2:
    case Si2168_DD_MODE_PROP_MODULATION_DVBC : {
      dtv_demod_needed = 1;
      ter_tuner_needed = 1;
      if ( Si2168_TER_CLOCK_SOURCE == Si2168_TER_Tuner_clock) {
        ter_clock_needed = 1;
      }
      break;
    }
    case Si2168_DD_MODE_PROP_MODULATION_ANALOG: {
      ter_tuner_needed = 1;
      break;
    }
    default : /* SLEEP */   {
      ter_tuner_needed = 0;
      break;
    }
  }

  /* ------------------------------------------------------------ */
  /* if 'force' flag is set, set flags to trigger a full init     */
  /* This can be used to re-init the NIM after a power cycle      */
  /*  or a HW reset                                               */
  /* ------------------------------------------------------------ */
  if (force_full_init) {
    SiTRACE("Forcing full init\n");
    /* set 'init_done' flags to force full init     */
    front_end->first_init_done     = 0;
    front_end->Si2168_init_done    = 0;
    front_end->TER_init_done       = 0;
    front_end->TER_tuner_init_done = 0;
    /* set 'already used' flags to force full init  */
    ter_tuner_already_used = 0;
    dtv_demod_already_used = 0;
  }

  /* ------------------------------------------------------------ */
  /* Request demodulator sleep if its clock will be stopped       */
  /* ------------------------------------------------------------ */
  if ((ter_clock_already_used == 1) & (ter_clock_needed == 0) ) { SiTRACE("TER clock 1->0 "); dtv_demod_sleep_request = 1; }
  if ((ter_clock_already_used == 0) & (ter_clock_needed == 1) ) { SiTRACE("TER clock 0->1 "); dtv_demod_sleep_request = 1; }
  /* ------------------------------------------------------------ */
  /* Request demodulator sleep if transition from '1' to '0'      */
  /* ------------------------------------------------------------ */
  if ((dtv_demod_already_used == 1) & (dtv_demod_needed == 0) ) { dtv_demod_sleep_request = 1; }
  SiTRACE("dtv_demod_already_used %d, dtv_demod_needed %d, dtv_demod_sleep_request %d\n", dtv_demod_already_used , dtv_demod_needed, dtv_demod_sleep_request);
  /* ------------------------------------------------------------ */
  /* Sleep dtv demodulator if requested                           */
  /* ------------------------------------------------------------ */
  if (dtv_demod_sleep_request == 1) {
    SiTRACE("Sleep DTV demod\n");
    Si2168_STANDBY (front_end->demod);
    DTV_DELAY
  }

  /* ------------------------------------------------------------ */
  /* Set media for new standard                                   */
  /* ------------------------------------------------------------ */
  front_end->demod->prop->dd_mode.modulation = new_standard;
  front_end->demod->media = Si2168_Media(front_end->demod, front_end->demod->prop->dd_mode.modulation);

  /* ------------------------------------------------------------ */
  /* Allow i2c traffic to reach the tuners                        */
  /* ------------------------------------------------------------ */
  SiTRACE("Connect    tuners i2c\n");
  Si2168_L2_Tuner_I2C_Enable(front_end);
  DTV_DELAY

  /* ------------------------------------------------------------ */
  /* Sleep Ter Tuner                                              */
  /* Sleep terrestrial tuner  if transition from '1' to '0'       */
  /* ------------------------------------------------------------ */
  if ((ter_tuner_already_used == 1) & (ter_tuner_needed == 0) ) {
    SiTRACE("Sleep terrestrial tuner\n");
    #ifdef    TER_TUNER_CLOCK_OFF
    SiTRACE("Terrestrial tuner clock OFF\n");
    if ((res= TER_TUNER_CLOCK_OFF(front_end->tuner_ter)) !=0 ) {
      SiTRACE("Terrestrial tuner CLOCK OFF error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
      SiERROR("Terrestrial tuner CLOCK OFF error!\n");
      return 0;
    };
    #endif /* TER_TUNER_CLOCK_OFF */
    #ifdef    TER_TUNER_STANDBY
    SiTRACE("Terrestrial tuner STANDBY\n");
    if ((res= TER_TUNER_STANDBY(front_end->tuner_ter)) !=0 ) {
      SiTRACE("Terrestrial tuner Standby error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
      SiERROR("Terrestrial tuner Standby error!\n");
      return 0;
    };
    #endif /* TER_TUNER_STANDBY */
    TER_DELAY
  }


  /* ------------------------------------------------------------ */
  /* Wakeup Ter Tuner                                             */
  /* Wake up terrestrial tuner if transition from '0' to '1'      */
  /* ------------------------------------------------------------ */
  if ((ter_tuner_already_used == 0) & (ter_tuner_needed == 1)) {
    /* Do a full init of the Ter Tuner only if it has not been already done */
    if (front_end->TER_tuner_init_done==0) {
      SiTRACE("Init terrestrial tuner\n");
      #ifdef    TER_TUNER_INIT
      if ((res= TER_TUNER_INIT(front_end->tuner_ter)) !=0) {
        #ifdef    TER_TUNER_ERROR_TEXT
        SiTRACE("Terrestrial tuner HW init error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
        #endif /* TER_TUNER_ERROR_TEXT */
        SiERROR("Terrestrial tuner HW init error!\n");
        return 0;
      };
      #endif /* TER_TUNER_INIT */
      front_end->TER_tuner_init_done++;
    } else {
      SiTRACE("Wakeup terrestrial tuner\n");
      #ifdef    TER_TUNER_WAKEUP
      if ((res= TER_TUNER_WAKEUP(front_end->tuner_ter)) !=0) {
        SiTRACE("Terrestrial tuner wake up error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
        SiERROR("Terrestrial tuner wake up error!\n");
        return 0;
      };
      #endif /* TER_TUNER_WAKEUP */
    }
    TER_DELAY
  }
    /* ------------------------------------------------------------ */
    /* If the terrestrial tuner's clock is required, activate it    */
    /* ------------------------------------------------------------ */
  SiTRACE("ter_clock_needed %d\n",ter_clock_needed);
  if (ter_clock_needed) {
    SiTRACE("Turn terrestrial tuner clock on\n");
    #ifdef    TER_TUNER_CLOCK_ON
    SiTRACE("Terrestrial tuner CLOCK ON\n");
    if ((res= TER_TUNER_CLOCK_ON(front_end->tuner_ter) ) !=0) {
      SiTRACE("Terrestrial tuner CLOCK ON error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
      SiERROR("Terrestrial tuner CLOCK ON error!\n");
      return 0;
    };
    #endif /* TER_TUNER_CLOCK_ON */
    TER_DELAY
  }
  if ((front_end->previous_standard != new_standard) & (dtv_demod_needed == 0) & (front_end->demod->media == Si2168_TERRESTRIAL)) {
   if (front_end->demod->media == Si2168_TERRESTRIAL) {
    #ifdef    TER_TUNER_ATV_LO_INJECTION
     TER_TUNER_ATV_LO_INJECTION(front_end->tuner_ter);
    #endif /* TER_TUNER_ATV_LO_INJECTION */

  }
 }

  /* ------------------------------------------------------------ */
  /* Change Dtv Demod standard if required                        */
  /* ------------------------------------------------------------ */
  if ((front_end->previous_standard != new_standard) & (dtv_demod_needed == 1)) {
    SiTRACE("Store demod standard (%d)\n", new_standard);
    front_end->demod->standard = new_standard;
    DTV_DELAY
    /* Set flag to trigger Si2168 init or re_init, to complete    */
    /*  the standard change                                       */
    dtv_demod_already_used = 0;
    if (front_end->demod->media == Si2168_TERRESTRIAL) {
    #ifdef    TER_TUNER_DTV_LO_INJECTION
     TER_TUNER_DTV_LO_INJECTION(front_end->tuner_ter);
    #endif /* TER_TUNER_DTV_LO_INJECTION */
    }
  }

  /* ------------------------------------------------------------ */
  /* Wakeup Dtv Demod                                             */
  /*  if it has been put in 'standby mode' and is needed          */
  /* ------------------------------------------------------------ */
  if (front_end->Si2168_init_done) {
    SiTRACE("dtv_demod_sleep_request %d\n",dtv_demod_sleep_request);
    if ((dtv_demod_sleep_request == 1) & (dtv_demod_needed == 1) ) {
      SiTRACE("Wake UP DTV demod\n");
      if (Si2168_WAKEUP (front_end->demod) == NO_Si2168_ERROR) {
        SiTRACE("Wake UP DTV demod OK\n");
      } else {
        SiERROR("Wake UP DTV demod failed!\n");
        SiTRACE("Wake UP DTV demod failed!\n");
        return 0;
      }
    }
  }
  /* ------------------------------------------------------------ */
  /* Setup Dtv Demod                                              */
  /* Setup dtv demodulator if transition from '0' to '1'          */
  /* ------------------------------------------------------------ */
  if ((dtv_demod_already_used == 0)       & (dtv_demod_needed == 1)) {
    /* Do the 'first init' only the first time, plus if requested  */
    /* (when 'force' flag is 1, Si2168_init_done is set to '0')   */
    if (!front_end->Si2168_init_done) {
      SiTRACE("Init demod\n");
      if (Si2168_Init(front_end->demod) == NO_Si2168_ERROR) {
        front_end->Si2168_init_done = 1;
        SiTRACE("Demod init OK\n");
      } else {
        SiTRACE("Demod init failed!\n");
        SiERROR("Demod init failed!\n");
        return 0;
      }
    }
    if (front_end->demod->media == Si2168_TERRESTRIAL) {
      SiTRACE("front_end->demod->media Si2168_TERRESTRIAL\n");
      if (front_end->TER_init_done == 0) {
        SiTRACE("Configure demod for TER\n");
        if (Si2168_Configure(front_end->demod) == NO_Si2168_ERROR) {
          /* set dd_mode.modulation again, as it is overwritten by Si2168_Configure */
          front_end->demod->prop->dd_mode.modulation = new_standard;
          front_end->TER_init_done = 1;
        } else {
          SiTRACE("Demod TER configuration failed !\n");
          SiERROR("Demod TER configuration failed !\n");
          return 0;
        }
      }
      DTV_DELAY
      /* ------------------------------------------------------------ */
      /* Manage FEF mode in TER tuner                                 */
      /* ------------------------------------------------------------ */
      if (new_standard == Si2168_DD_MODE_PROP_MODULATION_DVBT2) {
        Si2168_L2_TER_FEF_SETUP (front_end, 1);
      } else {
        Si2168_L2_TER_FEF_SETUP (front_end, 0);
      }
      TER_DELAY
    }
    front_end->demod->prop->dd_mode.invert_spectrum = Si2168_L2_Set_Invert_Spectrum(front_end);
    if (Si2168_L1_SetProperty2(front_end->demod, Si2168_DD_MODE_PROP_CODE)==0) {
      Si2168_L1_DD_RESTART(front_end->demod);
    } else {
      SiTRACE("Demod restart failed !\n");
      return 0;
    }
    DTV_DELAY
  }

  /* ------------------------------------------------------------ */
  /* Forbid i2c traffic to reach the tuners                       */
  /* ------------------------------------------------------------ */
  SiTRACE("Disconnect tuners i2c\n");
  Si2168_L2_Tuner_I2C_Disable(front_end);
  DTV_DELAY

  /* ------------------------------------------------------------ */
  /* update value of previous_standard to prepare next call       */
  /* ------------------------------------------------------------ */
  front_end->previous_standard = new_standard;
  front_end->demod->standard   = new_standard;

  front_end->first_init_done = 1;
#ifdef    PROFILING
  sprintf(sequence,"%s","");
  sprintf(sequence,"%s| TER: %4d ms ", sequence, ter_tuner_delay);
  sprintf(sequence,"%s| DTV: %4d ms ", sequence, dtv_demod_delay);
  sprintf(sequence,"%s| (%5d ms) ",    sequence, system_time()-switch_start);
  SiTRACE("%s\n", sequence);
#endif /* PROFILING */

  SiTRACE("Si2168_switch_to_standard complete\n");
  return 1;
}
/************************************************************************************************************************
  Si2168_lock_to_carrier function
  Use:      relocking function
            Used to relock on a channel for the current standard
  Parameter: standard the standard to lock to
  Parameter: freq                the frequency to lock to (in Hz for TER, in kHz for SAT)
  Parameter: dvb_t_bandwidth_hz  the channel bandwidth in Hz (only for DVB-T)
  Parameter: dvb_t_stream        the HP/LP stream  (only for DVB-T)
  Parameter: symbol_rate_bps     the symbol rate   (for DVB-S and SAT)
  Parameter: dvb_c_constellation the DVB-C constellation (only for DVB-C)
  Return:    1 if locked, 0 otherwise
************************************************************************************************************************/
int   Si2168_L2_lock_to_carrier   (Si2168_L2_Context *front_end, int standard, int freq, int dvb_t_bandwidth_hz, int dvb_t_stream, unsigned int symbol_rate_bps, unsigned char dvb_c_constellation, unsigned char dvb_t2_plp_id)
{
  int return_code;
  int lockStartTime;
  int searchStartTime;
  int searchDelay;
  int lock;
  int max_lock_time_ms;
  unsigned int min_lock_time_ms;
  int abort_wait_start_ms;
  dvb_t2_plp_id = dvb_t2_plp_id; /* to avoid compiler warning when not used */

  lockStartTime = system_time();
  front_end->lockAbort = 0;
  SiTRACE ("relock to %s at %d\n", Si2168_standardName(standard), freq);

  SiTRACE("handshake state %d\n", front_end->handshakeOn);
  if (front_end->handshakeOn == 1) {
    if (front_end->handshakeUsed) { SiTRACE("lock_to_carrier_handshake : recalled after   handshake.\n"); }
    searchStartTime = front_end->handshakeStart_ms;
  }
  /* Setting demod for locking on required standard */
  if (front_end->handshakeOn == 0) {
    front_end->handshakeStart_ms = system_time();
    if (front_end->handshakeUsed) { SiTRACE("lock_to_carrier_handshake : handshake start %d\n", front_end->handshakeStart_ms); }
    switch (standard)
    {
      case Si2168_DD_MODE_PROP_MODULATION_DVBT :
      case Si2168_DD_MODE_PROP_MODULATION_DVBT2: {
        front_end->demod->prop->dd_mode.bw                = dvb_t_bandwidth_hz/1000000;
        if (dvb_t_bandwidth_hz == 1700000) {
          front_end->demod->prop->dd_mode.bw              = Si2168_DD_MODE_PROP_BW_BW_1D7MHZ;
        }
        if (front_end->auto_detect_TER) {
          SiTRACE("DVB-T/T2 auto detect\n");
          max_lock_time_ms = Si2168_DVBT2_MAX_LOCK_TIME;
          min_lock_time_ms = Si2168_DVBT_MIN_LOCK_TIME;
          front_end->demod->prop->dvbt_hierarchy.stream = dvb_t_stream;
          return_code = Si2168_L1_SetProperty2(front_end->demod, Si2168_DVBT_HIERARCHY_PROP_CODE);
//          printf("return_code = %d\n", return_code);
          if (dvb_t2_plp_id != -1) {
            Si2168_L1_DVBT2_PLP_SELECT    (front_end->demod, dvb_t2_plp_id, Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MANUAL);
          } else {
            Si2168_L1_DVBT2_PLP_SELECT    (front_end->demod, 0            , Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_AUTO);
          }
          front_end->demod->prop->dd_mode.modulation  = Si2168_DD_MODE_PROP_MODULATION_AUTO_DETECT;
          front_end->demod->prop->dd_mode.auto_detect = Si2168_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2;
        } else {
          if (standard == Si2168_DD_MODE_PROP_MODULATION_DVBT ) {
            max_lock_time_ms = Si2168_DVBT_MAX_LOCK_TIME;
            min_lock_time_ms = Si2168_DVBT_MIN_LOCK_TIME;
            front_end->demod->prop->dvbt_hierarchy.stream     = dvb_t_stream;
            return_code = Si2168_L1_SetProperty2(front_end->demod, Si2168_DVBT_HIERARCHY_PROP_CODE);
//            printf("return_code = %d\n", return_code);
          }
          if (standard == Si2168_DD_MODE_PROP_MODULATION_DVBT2) {
            max_lock_time_ms = Si2168_DVBT2_MAX_LOCK_TIME;
            min_lock_time_ms = Si2168_DVBT2_MIN_LOCK_TIME;
            if (dvb_t2_plp_id != -1) {
              Si2168_L1_DVBT2_PLP_SELECT    (front_end->demod, dvb_t2_plp_id, Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MANUAL);
            } else {
              Si2168_L1_DVBT2_PLP_SELECT    (front_end->demod, 0            , Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_AUTO);
            }
          }
        }
        Si2168_L1_SetProperty2(front_end->demod, Si2168_DD_MODE_PROP_CODE);
        SiTRACE("bw %d Hz\n", dvb_t_bandwidth_hz);
        break;
      }
      case Si2168_DD_MODE_PROP_MODULATION_DVBC : {
        max_lock_time_ms = Si2168_DVBC_MAX_LOCK_TIME;
        min_lock_time_ms = Si2168_DVBC_MIN_LOCK_TIME;
        front_end->demod->prop->dd_mode.bw                       = 8;
        front_end->demod->prop->dvbc_symbol_rate.rate            = symbol_rate_bps/1000;
        front_end->demod->prop->dvbc_constellation.constellation = dvb_c_constellation;
        Si2168_L1_SetProperty2(front_end->demod, Si2168_DD_MODE_PROP_CODE);
        Si2168_L1_SetProperty2(front_end->demod, Si2168_DVBC_SYMBOL_RATE_PROP_CODE);
        Si2168_L1_SetProperty2(front_end->demod, Si2168_DVBC_CONSTELLATION_PROP_CODE);
        SiTRACE("sr %d bps, constel %d\n", symbol_rate_bps, dvb_c_constellation);
        break;
      }
      default : /* ATV */   {
        SiTRACE("'%d' standard (%s) is not managed by Si2168_lock_to_carrier\n", standard, Si2168_standardName(standard));
        return 0;
        break;
      }
    }

    if (front_end->lockAbort) {
      SiTRACE("Si2168_L2_lock_to_carrier : lock aborted before tuning, after %d ms.\n", system_time() - lockStartTime );
      return 0;
    }

    searchStartTime = system_time();

    Si2168_L2_Tune                  (front_end,       freq);
    SiTRACE ("Si2168_lock_to_carrier 'tune'  took %3d ms\n"        , system_time() - searchStartTime);


    Si2168_L1_DD_RESTART            (front_end->demod);
    SiTRACE ("Si2168_lock_to_carrier 'reset' took %3d ms\n"        , system_time() - searchStartTime);

    lock = 0;
    searchStartTime = system_time();

    /* as we will not lock in less than min_lock_time_ms, wait a while..., but check for a possible 'abort' from the application */
    abort_wait_start_ms = system_time();
    while (system_time()-abort_wait_start_ms < min_lock_time_ms) {
      if (front_end->lockAbort) {
        SiTRACE("Si2168_L2_lock_to_carrier : lock aborted before checking lock status, after %d ms.\n", system_time() - lockStartTime );
        return 0;
      }
      /* Adapt here the minimal 'reaction time' of the application*/
      system_wait(20);
    }
  }
  /* The actual lock check loop */
  while (1) {

    searchDelay = system_time() - searchStartTime;

    /* Check the status for the current modulation */

    switch (standard) {
      default                                   :
      case Si2168_DD_MODE_PROP_MODULATION_DVBT  :
      case Si2168_DD_MODE_PROP_MODULATION_DVBT2 : {
        /* DVB-T/T2 auto detect seek loop, using Si2168_L1_DD_STATUS                                          */
        /* if DL LOCKED                            : demod is locked on a dd_status->modulation signal        */
        /* if DL NO_LOCK and rsqint_bit5 NO_CHANGE : demod is searching for a DVB-T/T2 signal                 */
        /* if DL NO_LOCK and rsqint_bit5 CHANGE    : demod says this is not a DVB-T/T2 signal (= 'neverlock') */
        return_code = Si2168_L1_DD_STATUS(front_end->demod, Si2168_DD_STATUS_CMD_INTACK_CLEAR);

        if (return_code != NO_Si2168_ERROR) {
          SiTRACE("Si2168_lock_to_carrier: Si2168_L1_DD_STATUS error\n");
          SiERROR("Si2168_lock_to_carrier: Si2168_L1_DD_STATUS error\n");
          goto exit_lock;
          break;
        }

        if (  front_end->demod->rsp->dd_status.dl  == Si2168_DD_STATUS_RESPONSE_DL_LOCKED   ) {
          /* Return 1 to signal that the Si2168 is locked on a valid DVB-T/T2 channel */
          SiTRACE("Si2168_lock_to_carrier: locked on a %s signal\n", Si2168_standardName(front_end->demod->rsp->dd_status.modulation) );
          lock = 1;
          /* Make sure FEF mode is ON when locked on a T2 channel */
          if   (front_end->demod->rsp->dd_status.modulation == Si2168_DD_MODE_PROP_MODULATION_DVBT2) {
            Si2168_L2_Tuner_I2C_Enable(front_end);
            Si2168_L2_TER_FEF(front_end, 1);
            Si2168_L2_Tuner_I2C_Disable(front_end);
          }
          goto exit_lock;
        } else {
          if (  front_end->demod->rsp->dd_status.rsqint_bit5 == Si2168_DD_STATUS_RESPONSE_RSQINT_BIT5_CHANGED ) {
          /* Return 0 if firmware signals 'no DVB-T/T2 channel' */
          SiTRACE ("'no DVB-T/T2 channel': not locked after %3d ms\n", searchDelay);
          goto exit_lock;
          }
        }
        break;
      }
      case Si2168_DD_MODE_PROP_MODULATION_DVBC  : {
        return_code = Si2168_L1_DD_STATUS(front_end->demod, Si2168_DD_STATUS_CMD_INTACK_CLEAR);

        if (return_code != NO_Si2168_ERROR) {
          SiTRACE("Si2168_lock_to_carrier: Si2168_L1_DD_STATUS error\n");
          SiERROR("Si2168_lock_to_carrier: Si2168_L1_DD_STATUS error\n");
          return return_code;
          break;
        }

        if ( (front_end->demod->rsp->dd_status.dl    == Si2168_DD_STATUS_RESPONSE_DL_LOCKED     ) ) {
          /* Return 1 to signal that the Si2168 is locked on a valid SAT channel */
          SiTRACE("%s lock\n", Si2168_standardName(front_end->demod->rsp->dd_status.modulation));
          lock = 1;
          goto exit_lock;
        }
        break;
      }
    }

    /* timeout management (this should never happen if timeout values are correctly set) */
    if (searchDelay >= max_lock_time_ms) {
      SiTRACE ("Si2168_lock_to_carrier timeout(%d) after %d ms\n", max_lock_time_ms, searchDelay);
      break;
    }
    if (searchDelay >= front_end->handshakePeriod_ms) {
      if (front_end->handshakeUsed) {
        SiTRACE ("\nlock_to_carrier_handshake : handshake after %5d ms (at %10d). (handshake period %6d ms) %*s\n\n", searchDelay, freq, system_time() - front_end->handshakeStart_ms, (system_time() - front_end->handshakeStart_ms)/1000,"*");
        SiTRACE ("%c", 0x07);
        front_end->handshakeOn = 1;
        /* The application will check handshakeStart_ms to know whether the lock is complete or not */
        return system_time() - front_end->handshakeStart_ms;
      } else {
/*        SiTRACE ("\nlock_to_carrier_handshake : handshake not in use after %5d ms (at %10d). Staying in lock_to_carrier. (handshake period %6d ms) %*s\n\n", searchDelay, freq, system_time() - front_end->handshakeStart_ms, (system_time() - front_end->handshakeStart_ms)/1000,"*");*/
      }
    }

    if (front_end->lockAbort) {
      SiTRACE("Si2168_L2_lock_to_carrier : lock aborted after %d ms.\n", system_time() - lockStartTime);
      return 0;
    }

    /* Check status every 10 ms */
    system_wait(10);
  }

  exit_lock:

  searchDelay = system_time() - searchStartTime;

  if (lock) {
    SiTRACE ("Si2168_lock_to_carrier 'lock'  took %3d ms\n"        , searchDelay);
  } else {
    SiTRACE ("Si2168_lock_to_carrier at %10d (%s) failed after %d ms\n",freq, Si2168_standardName(front_end->demod->rsp->dd_status.modulation), searchDelay);
  }

  return lock;
}
/************************************************************************************************************************
  Si2168_L2_Tune function
  Use:        tuner current frequency retrieval function
              Used to retrieve the current RF from the tuner's driver.
  Porting:    Replace the internal TUNER function calls by the final tuner's corresponding calls
  Comments:   If the tuner is connected via the demodulator's I2C switch, enabling/disabling the i2c_passthru is required before/after tuning.
  Behavior:   This function closes the Si2168's I2C switch then tunes and finally reopens the I2C switch
  Parameter:  *front_end, the front-end handle
  Parameter:  rf, the frequency to tune at
  Returns:    rf
************************************************************************************************************************/
int  Si2168_L2_Tune               (Si2168_L2_Context *front_end, int rf)
{
#ifdef    TUNERTER_API
    char bw;
    char modulation;
#endif /* TUNERTER_API */

    SiTRACE("Si2168_L2_Tune at %d\n",rf);

    Si2168_L2_Tuner_I2C_Enable(front_end);

    if (front_end->demod->media == Si2168_TERRESTRIAL) {
      Si2168_L2_TER_FEF (front_end, 0);
#ifdef    TUNERTER_API
      if (front_end->demod->prop->dd_mode.modulation == Si2168_DD_MODE_PROP_MODULATION_DVBC ) {
        modulation = L1_RF_TER_TUNER_MODULATION_DVBC;
        bw         = 8;
      } else {
        modulation = L1_RF_TER_TUNER_MODULATION_DVBT;
        switch (front_end->demod->prop->dd_mode.bw) {
          case Si2168_DD_MODE_PROP_BW_BW_1D7MHZ   : bw = 6; break;
          case Si2168_DD_MODE_PROP_BW_BW_5MHZ     : bw = 6; break;
          case Si2168_DD_MODE_PROP_BW_BW_6MHZ     : bw = 6; break;
          case Si2168_DD_MODE_PROP_BW_BW_7MHZ     : bw = 7; break;
          case Si2168_DD_MODE_PROP_BW_BW_8MHZ     : bw = 8; break;
          default: SiERROR("Invalid dd_mode.bw\n"); bw = 8; break;
        }
      }
#endif /* TUNERTER_API */
      L1_RF_TER_TUNER_Tune       (front_end->tuner_ter , rf);
#ifdef    TUNERTER_API
      if (front_end->demod->prop->dd_mode.modulation == Si2168_DD_MODE_PROP_MODULATION_DVBT2) { Si2168_L2_TER_FEF (front_end, 1); }
#endif /* TUNERTER_API */
    }


    Si2168_L2_Tuner_I2C_Disable(front_end);

    return rf;
}
/************************************************************************************************************************
  Si2168_L2_Get_RF function
  Use:        tuner current frequency retrieval function
              Used to retrieve the current RF from the tuner's driver.
  Porting:    Replace the internal TUNER function calls by the final tuner's corresponding calls
  Behavior:   This function does not need to activate the Si2168's I2C switch, as the required value is part of the tuner's structure
  Parameter:  *front_end, the front-end handle
************************************************************************************************************************/
int  Si2168_L2_Get_RF             (Si2168_L2_Context *front_end) {

  if (front_end->demod->media == Si2168_TERRESTRIAL) {
#ifdef    TUNERTER_API
    return front_end->tuner_ter->cmd->tuner_tune_freq.freq;
#else  /* TUNERTER_API */
    return (int)L1_RF_TER_TUNER_Get_RF (front_end->tuner_ter);
#endif /* TUNERTER_API */
  }


  return 0;
}
/************************************************************************************************************************
  Si2168_L2_TER_FEF function
  Use:        TER tuner FEF activation function
              Used to enable/disable the FEF mode in the terrestrial tuner
  Comments:   If the tuner is connected via the demodulator's I2C switch, enabling/disabling the i2c_passthru is required before/after tuning.
  Parameter:  *front_end, the front-end handle
  Parameter:  fef, a flag controlling the selection between FEF 'off'(0) and FEF 'on'(1)
  Returns:    1
************************************************************************************************************************/
int  Si2168_L2_TER_FEF            (Si2168_L2_Context *front_end, int fef)
{
  front_end = front_end; /* To avoid compiler warning if not used */
  SiTRACE("Si2168_L2_TER_FEF %d\n",fef);

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN
  if (fef_mode == Si2168_FEF_MODE_FREEZE_PIN      ) {
    SiTRACE("FEF mode Si2168_FEF_MODE_FREEZE_PIN\n");
    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN(front_end->tuner_ter, fef);
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP
  if (fef_mode == Si2168_FEF_MODE_SLOW_INITIAL_AGC) {
    SiTRACE("FEF mode Si2168_FEF_MODE_SLOW_INITIAL_AGC (AGC slowed down after tuning)\n");
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP
  if (fef_mode == Si2168_FEF_MODE_SLOW_NORMAL_AGC ) {
    SiTRACE("FEF mode Si2168_FEF_MODE_SLOW_NORMAL_AGC: AGC slowed down\n");
    L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC(front_end->tuner_ter, fef);
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP */
  SiTRACE("Si2168_L2_TER_FEF done\n");
  return 1;
}
/************************************************************************************************************************
  Si2168_L2_TER_FEF_SETUP function
  Use:        TER tuner LPF setting function
              Used to configure the FEF mode in the terrestrial tuner
  Comments:   If the tuner is connected via the demodulator's I2C switch, enabling/disabling the i2c_passthru is required before/after tuning.
  Behavior:   This function closes the Si2168's I2C switch then sets the TER FEF mode and finally reopens the I2C switch
  Parameter:  *front_end, the front-end handle
  Parameter:  fef, a flag controlling the selection between FEF 'off'(0) and FEF 'on'(1)
  Returns:    1
************************************************************************************************************************/
int  Si2168_L2_TER_FEF_SETUP      (Si2168_L2_Context *front_end, int fef)
{
  SiTRACE("Si2168_L2_TER_FEF_SETUP %d\n",fef);
  #ifdef    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP
  if (fef_mode == Si2168_FEF_MODE_FREEZE_PIN      ) {
    SiTRACE("FEF mode Si2168_FEF_MODE_FREEZE_PIN\n");
    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP(front_end->tuner_ter);
}
  #endif /* L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP
  if (fef_mode == Si2168_FEF_MODE_SLOW_INITIAL_AGC) {
    SiTRACE("FEF mode Si2168_FEF_MODE_SLOW_INITIAL_AGC (AGC slowed down after tuning)\n");
    L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP(front_end->tuner_ter, fef);
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP
  if (fef_mode == Si2168_FEF_MODE_SLOW_NORMAL_AGC ) {
    SiTRACE("FEF mode Si2168_FEF_MODE_SLOW_NORMAL_AGC: AGC slowed down\n");
    L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP(front_end->tuner_ter, fef);
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC */

  Si2168_L2_Tuner_I2C_Enable(front_end);

  Si2168_L2_TER_FEF(front_end, fef);

  Si2168_L2_Tuner_I2C_Disable(front_end);

  SiTRACE("Si2168_L2_TER_FEF_SETUP done\n");
  return 1;
}
/************************************************************************************************************************
  NAME: Si2168_L2_Trace_Scan_Status
  DESCRIPTION: traces the scan_status
  Parameter:  Pointer to Si2168 Context
  Returns:    void
************************************************************************************************************************/
const char *Si2168_L2_Trace_Scan_Status  (int scan_status) {
    switch (scan_status) {
      case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_ANALOG_CHANNEL_FOUND  : { return "ANALOG  CHANNEL_FOUND"; break; }
      case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_DIGITAL_CHANNEL_FOUND : { return "DIGITAL CHANNEL_FOUND"; break; }
      case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_DEBUG                 : { return "DEBUG"                ; break; }
      case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_ERROR                 : { return "ERROR"                ; break; }
      case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_ENDED                 : { return "ENDED"                ; break; }
      case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_IDLE                  : { return "IDLE"                 ; break; }
      case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_SEARCHING             : { return "SEARCHING"            ; break; }
      case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_TUNE_REQUEST          : { return "TUNE_REQUEST"         ; break; }
      default                                                             : { return "Unknown!"             ; break; }
    }
}
/************************************************************************************************************************
  NAME: Si2168_L2_Set_Invert_Spectrum
  DESCRIPTION: return the required invert_spectrum value depending on the settings:
              front_end->demod->media
              front_end->satellite_spectrum_inversion
              front_end->lnb_type
              front_end->unicable_spectrum_inversion

  Parameter:  Pointer to Si2168 Context
  Returns:    the required invert_spectrum value
************************************************************************************************************************/
unsigned char Si2168_L2_Set_Invert_Spectrum (Si2168_L2_Context *front_end) {
  unsigned char inversion;
  if (front_end->demod->media == Si2168_TERRESTRIAL) {
    inversion = Si2168_DD_MODE_PROP_INVERT_SPECTRUM_NORMAL;
  }
  return inversion;
}
/************************************************************************************************************************
  NAME: Si2168_L2_Channel_Seek_Init
  DESCRIPTION: logs the seek parameters in the context structure
  Programming Guide Reference:    Flowchart TBD (Channel Scan flowchart)

  Parameter:  Pointer to Si2168 Context
  Parameter:  starting Frequency Hz
  Parameter:  ending Frequency Hz
  Parameter:  min RSSI dBm
  Parameter:  max RSSI dBm
  Parameter:  min SNR 1/2 dB
  Parameter:  max SNR 1/2 dB
  Returns:    0 if successful, otherwise an error.
************************************************************************************************************************/
int  Si2168_L2_Channel_Seek_Init  (Si2168_L2_Context *front_end, int rangeMin, int rangeMax, int seekBWHz, int seekStepHz, int minSRbps, int maxSRbps, int minRSSIdBm, int maxRSSIdBm, int minSNRHalfdB, int maxSNRHalfdB)
{
  if (front_end->demod->media == Si2168_TERRESTRIAL) {
    SiTRACE("media TERRESTRIAL\n");
    front_end->tuneUnitHz   =    1;
  }

  switch (front_end->demod->prop->dd_mode.modulation) {
    case Si2168_DD_MODE_PROP_MODULATION_DVBC :
    /* Start of Workaround for consecutive T/T2 seek followed by C seek (VDAPPS78) */
    /*  Behavior: For DVBC, making sure that AUTO_T_T2 is terminated by switching
        momentarily in DVBT/AUTO_DETECT_NONE                                       */
/*      front_end->demod->prop->dd_mode.modulation  = Si2168_DD_MODE_PROP_MODULATION_DVBT;
      front_end->demod->prop->dd_mode.auto_detect = Si2168_DD_MODE_PROP_AUTO_DETECT_NONE;
      Si2168_L1_SetProperty2(front_end->demod, Si2168_DD_MODE_PROP_CODE);
      Si2168_L1_DD_RESTART  (front_end->demod);
  */  /* End   of Workaround for consecutive T/T2 seek followed by C seek (VDAPPS78) */
      front_end->demod->prop->dd_mode.modulation  = Si2168_DD_MODE_PROP_MODULATION_DVBC;
      front_end->demod->prop->dd_mode.auto_detect = Si2168_DD_MODE_PROP_AUTO_DETECT_NONE;
      break;
    case Si2168_DD_MODE_PROP_MODULATION_DVBT :
    case Si2168_DD_MODE_PROP_MODULATION_DVBT2:
      front_end->demod->prop->dd_mode.modulation  = Si2168_DD_MODE_PROP_MODULATION_AUTO_DETECT;
      front_end->demod->prop->dd_mode.auto_detect = Si2168_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2;
      break;
    case Si2168_DD_MODE_PROP_MODULATION_AUTO_DETECT : break;
    default : {
      SiTRACE("'%d' modulation (%s) is not managed by Si2168_L2_Channel_Seek_Init\n", front_end->demod->prop->dd_mode.modulation, Si2168_standardName(front_end->demod->prop->dd_mode.modulation));
      break;
    }
  }

  Si2168_L1_SetProperty2(front_end->demod, Si2168_DD_MODE_PROP_CODE);
  Si2168_L1_DD_RESTART  (front_end->demod);

  front_end->seekBWHz     = seekBWHz;
  front_end->seekStepHz   = seekStepHz;
  front_end->minSRbps     = minSRbps;
  front_end->maxSRbps     = maxSRbps;
  front_end->rangeMin     = rangeMin;
  front_end->rangeMax     = rangeMax;
  front_end->minRSSIdBm   = minRSSIdBm;
  front_end->maxRSSIdBm   = maxRSSIdBm;
  front_end->minSNRHalfdB = minSNRHalfdB;
  front_end->maxSNRHalfdB = maxSNRHalfdB;
  front_end->seekAbort    = 0;
  front_end->seekRunning  = 0;

  SiTRACE("Si2168_L2_Channel_Seek_Init with %d to  %d, sawBW %d, minSR %d, maxSR %d\n", front_end->rangeMin, front_end->rangeMax, front_end->seekBWHz, front_end->minSRbps, front_end->maxSRbps);
  SiTRACE("spectrum inversion %d\n",front_end->demod->prop->dd_mode.invert_spectrum );

  front_end->demod->prop->scan_fmin.scan_fmin                   = (int)(front_end->rangeMin*(front_end->tuneUnitHz/65536.0));
  front_end->demod->prop->scan_fmax.scan_fmax                   = (int)(front_end->rangeMax*(front_end->tuneUnitHz/65536.0));

  front_end->demod->prop->scan_symb_rate_min.scan_symb_rate_min = front_end->minSRbps/1000;
  front_end->demod->prop->scan_symb_rate_max.scan_symb_rate_max = front_end->maxSRbps/1000;

  Si2168_L1_SetProperty2(front_end->demod, Si2168_SCAN_FMIN_PROP_CODE);
  Si2168_L1_SetProperty2(front_end->demod, Si2168_SCAN_FMAX_PROP_CODE);
  Si2168_L1_SetProperty2(front_end->demod, Si2168_SCAN_SYMB_RATE_MIN_PROP_CODE);
  Si2168_L1_SetProperty2(front_end->demod, Si2168_SCAN_SYMB_RATE_MAX_PROP_CODE);



  front_end->demod->prop->scan_ien.buzien           = Si2168_SCAN_IEN_PROP_BUZIEN_DISABLE ; /* (default 'DISABLE') */
  front_end->demod->prop->scan_ien.reqien           = Si2168_SCAN_IEN_PROP_REQIEN_ENABLE  ; /* (default 'DISABLE') */
  Si2168_L1_SetProperty2(front_end->demod, Si2168_SCAN_IEN_PROP_CODE);

  front_end->demod->prop->scan_int_sense.reqnegen   = Si2168_SCAN_INT_SENSE_PROP_REQNEGEN_DISABLE ; /* (default 'DISABLE') */
  front_end->demod->prop->scan_int_sense.reqposen   = Si2168_SCAN_INT_SENSE_PROP_REQPOSEN_ENABLE  ; /* (default 'ENABLE') */
  Si2168_L1_SetProperty2(front_end->demod, Si2168_SCAN_INT_SENSE_PROP_CODE);


  if (front_end->demod->media == Si2168_TERRESTRIAL) {
    front_end->demod->prop->scan_ter_config.analog_bw     = Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW_8MHZ;
    if ( front_end->rangeMin == front_end->rangeMax ) {
      front_end->demod->prop->scan_ter_config.mode        = Si2168_SCAN_TER_CONFIG_PROP_MODE_BLIND_LOCK;
      SiTRACE("Blindlock < %8d %8d > < %8d %8d >\n", front_end->demod->prop->scan_fmin.scan_fmin, front_end->demod->prop->scan_fmax.scan_fmax, front_end->demod->prop->scan_symb_rate_min.scan_symb_rate_min, front_end->demod->prop->scan_symb_rate_max.scan_symb_rate_max);
    } else {
      front_end->demod->prop->scan_ter_config.mode        = Si2168_SCAN_TER_CONFIG_PROP_MODE_BLIND_SCAN;
      SiTRACE("Blindscan < %8d %8d > < %8d %8d >\n", front_end->demod->prop->scan_fmin.scan_fmin, front_end->demod->prop->scan_fmax.scan_fmax, front_end->demod->prop->scan_symb_rate_min.scan_symb_rate_min, front_end->demod->prop->scan_symb_rate_max.scan_symb_rate_max);
    }
    front_end->demod->prop->scan_ter_config.search_analog = Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_DISABLE;
    Si2168_L1_SetProperty2(front_end->demod, Si2168_SCAN_TER_CONFIG_PROP_CODE);
    if (seekBWHz == 1700000) {
      front_end->demod->prop->dd_mode.bw = Si2168_DD_MODE_PROP_BW_BW_1D7MHZ;
    } else {
      front_end->demod->prop->dd_mode.bw = seekBWHz/1000000;
    }
  }
  SiTRACE ("blindscan_interaction >> (init  ) Si2168_L1_SCAN_CTRL( front_end->demod, Si2168_SCAN_CTRL_CMD_ACTION_ABORT)\n");
                      Si2168_L1_SCAN_CTRL (front_end->demod, Si2168_SCAN_CTRL_CMD_ACTION_ABORT , 0);
  Si2168_L1_SCAN_STATUS (front_end->demod, Si2168_SCAN_STATUS_CMD_INTACK_OK);
  SiTRACE("blindscan_status leaving Seek_Init %s\n", Si2168_L2_Trace_Scan_Status(front_end->demod->rsp->scan_status.scan_status) );
  /* Preparing the next call to Si2168_L1_SCAN_CTRL which needs to be a 'START'*/
  front_end->demod->cmd->scan_ctrl.action = Si2168_SCAN_CTRL_CMD_ACTION_START;
  front_end->handshakeOn       = 0;
  SiTRACE("blindscan_handshake : Seek_Next will return every ~%d ms\n", front_end->handshakePeriod_ms );
  return 0;
}
/************************************************************************************************************************
  NAME: Si2168_L2_Channel_Seek_Next
  DESCRIPTION: Looks for the next channel, starting from the last detected channel
  Programming Guide Reference:    Flowchart TBD (Channel Scan flowchart)

  Parameter:  Pointer to Si2168 Context
  Returns:    1 if channel is found, 0 otherwise (either abort or end of range)
************************************************************************************************************************/
int  Si2168_L2_Channel_Seek_Next  (Si2168_L2_Context *front_end, int *standard, int *freq, int *bandwidth_Hz, int *stream, unsigned int *symbol_rate_bps, int *constellation, int *dvb_t2_num_plp)
{
    int           return_code;
    int           seek_freq;
    int           seek_freq_kHz;
    int           channelIncrement;
    int           startTime;
    unsigned int  searchStartTime;
    int           searchDelay;
    int           max_lock_time_ms;
    unsigned int  min_lock_time_ms;
    int           max_decision_time_ms;
    int           blind_mode;
    int           skip_resume;
    int           start_resume;
    int           previous_scan_status;
    unsigned char jump_to_next_channel;
    L1_Si2168_Context *api;

    api = front_end->demod;

    blind_mode   = 0;
    startTime    = system_time();

    /* Clear all return values which may not be used depending on the standard */
    *bandwidth_Hz    = 0;
    *stream          = 0;
    *symbol_rate_bps = 0;
    *constellation   = 0;
    *dvb_t2_num_plp  = 0;

    if (front_end->seekAbort) {
      SiTRACE("Si2168_L2_Channel_Seek_Next : previous run aborted. Please Si2168_L2_Channel_Seek_Init to perform a new search.\n");
      return 0;
    }


    SiTRACE("front_end->demod->standard %d (%s)\n",front_end->demod->standard, Si2168_standardName(front_end->demod->standard) );

    /* Setting max and max lock times and blind_mode flag */
    switch ( front_end->demod->standard )
    {
      case Si2168_DD_MODE_PROP_MODULATION_DVBT :
      case Si2168_DD_MODE_PROP_MODULATION_DVBT2: {
        blind_mode = 0;
        max_lock_time_ms = Si2168_DVBT2_MAX_LOCK_TIME;
        min_lock_time_ms = Si2168_DVBT_MIN_LOCK_TIME;
        break;
      }
      case Si2168_DD_MODE_PROP_MODULATION_DVBC : {
        blind_mode = 1;
        max_lock_time_ms = Si2168_DVBC_MAX_SEARCH_TIME;
        min_lock_time_ms = Si2168_DVBC_MIN_LOCK_TIME;
        break;
      }
      default                                  : {
        SiTRACE("'%d' standard (%s) is not managed by Si2168_L2_Channel_Seek_Next\n", front_end->demod->prop->dd_mode.modulation, Si2168_standardName(front_end->demod->prop->dd_mode.modulation));
        front_end->seekAbort = 1;
        return 0;
        break;
      }
    }

    SiTRACE("handshake state %d\n", front_end->handshakeOn);
    if (front_end->handshakeOn) {
      SiTRACE("blindscan_handshake : recalled after   handshake.\n");
      start_resume = 0;
    } else {
      if (front_end->demod->cmd->scan_ctrl.action == Si2168_SCAN_CTRL_CMD_ACTION_START) {
        SiTRACE("blindscan_handshake : no handshake : starting.\n");
      } else {
        SiTRACE("blindscan_handshake : no handshake : resuming.\n");
      }
      front_end->handshakeStart_ms = system_time();
      SiTRACE("blindscan_handshake : handshake start %d\n", front_end->handshakeStart_ms);
      start_resume = 1;
      /* Disabling FEF control for T/T2 */
      switch ( front_end->demod->standard )
      {
        case Si2168_DD_MODE_PROP_MODULATION_DVBT :
        case Si2168_DD_MODE_PROP_MODULATION_DVBT2: {
          Si2168_L2_Tuner_I2C_Enable(front_end);
          Si2168_L2_TER_FEF (front_end,1);
          Si2168_L2_Tuner_I2C_Disable(front_end);
          break;
        }
        default                                  : {
          break;
        }
      }
    }

    max_decision_time_ms = max_lock_time_ms;

    /* Select TER channel increment based on BW (this value will only be used for 'TER' scanning) */
    channelIncrement = front_end->seekBWHz;

    /* Start Seeking */
    SiTRACE("Si2168_L2_Channel_Seek_Next front_end->rangeMin %10d,front_end->rangeMax %10d blind_mode %d\n", front_end->rangeMin,front_end->rangeMax, blind_mode);

    seek_freq = front_end->rangeMin;

  if (blind_mode == 0) { /* DVB-T / DVB-T2 */
    while ( seek_freq <= front_end->rangeMax )
    {
      searchStartTime = system_time();

      /* Call the Si2168_L2_Tune command to tune the frequency */
      if (Si2168_L2_Tune (front_end, seek_freq )!= seek_freq) {
        /* Manage possible tune error */
        SiTRACE("Si2168_L2_Channel_Seek_Next Tune error at %d, aborting (skipped)\n", seek_freq);
        front_end->seekAbort = 1;
        return 0;
      }

      Si2168_L1_DD_RESTART        (api);

      /* as we will not lock in less than min_lock_time_ms, wait a while... */
      system_wait(min_lock_time_ms);

      jump_to_next_channel = 0;

      while (!jump_to_next_channel) {

        searchDelay    = system_time() - searchStartTime;

        if ((front_end->demod->standard == Si2168_DD_MODE_PROP_MODULATION_DVBT) | (front_end->demod->standard == Si2168_DD_MODE_PROP_MODULATION_DVBT2) ) {

          return_code = Si2168_L1_DD_STATUS(api, Si2168_DD_STATUS_CMD_INTACK_CLEAR);
          if (return_code != NO_Si2168_ERROR) {
            SiTRACE("Si2168_L2_Channel_Seek_Next: Si2168_L1_DD_STATUS error at %d, aborting\n", seek_freq);
            front_end->seekAbort = 1; return 0;
          }

          if ( (front_end->demod->rsp->dd_status.dl  == Si2168_DD_STATUS_RESPONSE_DL_NO_LOCK  ) & ( front_end->demod->rsp->dd_status.rsqstat_bit5   == Si2168_DD_STATUS_RESPONSE_RSQINT_BIT5_NO_CHANGE ) ) {
            /* Check PCL to handle weak signals */
            if ( (front_end->demod->rsp->dd_status.pcl == Si2168_DD_STATUS_RESPONSE_PCL_LOCKED) & (searchDelay > 2000) ) {
              SiTRACE ("Weak DVBT/T2 at %7.3f after %3d ms (%s)\n", seek_freq/1000000.0, searchDelay, Si2168_standardName(front_end->demod->rsp->dd_status.modulation) );
              if(seek_freq==front_end->rangeMax) {front_end->rangeMin=seek_freq;}
              seek_freq = seek_freq + channelIncrement;
              jump_to_next_channel = 1;
              break;
            }
          }
          if ( (front_end->demod->rsp->dd_status.dl  == Si2168_DD_STATUS_RESPONSE_DL_NO_LOCK  ) & ( front_end->demod->rsp->dd_status.rsqstat_bit5   == Si2168_DD_STATUS_RESPONSE_RSQINT_BIT5_CHANGED   ) ) {
            SiTRACE ("NO DVBT/T2. Jumping from  %7.3f after %3d ms\n", seek_freq/1000000.0, searchDelay);
            if(seek_freq==front_end->rangeMax) {front_end->rangeMin=seek_freq;}
            seek_freq = seek_freq + channelIncrement;
            jump_to_next_channel = 1;
            break;
          }
          if ( (front_end->demod->rsp->dd_status.dl  == Si2168_DD_STATUS_RESPONSE_DL_LOCKED   ) & ( front_end->demod->rsp->dd_status.rsqstat_bit5   == Si2168_DD_STATUS_RESPONSE_RSQINT_BIT5_NO_CHANGE ) ) {
            *standard     = front_end->demod->rsp->dd_status.modulation;
            if (*standard == Si2168_DD_STATUS_RESPONSE_MODULATION_DVBT ) {
              Si2168_L1_DVBT_STATUS (api, Si2168_DVBT_STATUS_CMD_INTACK_CLEAR);
              front_end->detected_rf = seek_freq + front_end->demod->rsp->dvbt_status.afc_freq*1000;
              if (front_end->demod->rsp->dvbt_status.hierarchy == Si2168_DVBT_STATUS_RESPONSE_HIERARCHY_NONE) {
                *stream       = Si2168_DVBT_HIERARCHY_PROP_STREAM_HP;
              } else {
                *stream       = Si2168_DVBT_HIERARCHY_PROP_STREAM_LP;
              }
              *bandwidth_Hz = front_end->demod->prop->dd_mode.bw*1000000;
              *freq         = front_end->detected_rf;
              SiTRACE ("DVB-T  lock at %7.3f after %3d ms\n", (front_end->detected_rf)/1000000.0, searchDelay);
            }
            if (*standard == Si2168_DD_STATUS_RESPONSE_MODULATION_DVBT2) {
              Si2168_L2_Tuner_I2C_Enable(front_end);
              Si2168_L2_TER_FEF (front_end,1);
              Si2168_L2_Tuner_I2C_Disable(front_end);
              Si2168_L1_DVBT2_STATUS (api, Si2168_DVBT_STATUS_CMD_INTACK_CLEAR);
              *dvb_t2_num_plp = api->rsp->dvbt2_status.num_plp;
              front_end->detected_rf = seek_freq + front_end->demod->rsp->dvbt2_status.afc_freq*1000;
              SiTRACE ("DVB-T2 lock at %7.3f after %3d ms\n", (front_end->detected_rf)/1000000.0, searchDelay);
              switch (front_end->demod->prop->dd_mode.bw) {
                case Si2168_DD_MODE_PROP_BW_BW_1D7MHZ : { *bandwidth_Hz =                                    1700000; break; }
                default                               : { *bandwidth_Hz = front_end->demod->prop->dd_mode.bw*1000000; break; }
              }
              *freq         = front_end->detected_rf;
            }
            /* Set min seek_freq for next seek */
            front_end->rangeMin = seek_freq + channelIncrement;
            /* Return 1 to signal that the Si2168 is locked on a valid channel */
            return 1;
          }
        }

        /* timeout management (this should never happen if timeout values are correctly set) */
        if (searchDelay >= max_lock_time_ms) {
          SiTRACE ("Timeout from  %7.3f after %3d ms\n", seek_freq/1000000.0, searchDelay);
          SiERROR ("Timeout (blind_mode = 0)\n");
          seek_freq = seek_freq + channelIncrement;
          jump_to_next_channel = 1;
          break;
        }
        /* Check status every n ms */
        system_wait(50);
      }
    }
  }

  if (blind_mode == 1) { /* DVB-C / DVB-S / DVB-S2 */

    if (front_end->tuneUnitHz == 1) {
      seek_freq_kHz = seek_freq/1000;
    } else {
      seek_freq_kHz = seek_freq;
    }

    previous_scan_status = front_end->demod->rsp->scan_status.scan_status;
    /* Checking blindscan status before issuing a 'start' or 'resume' */
    Si2168_L1_SCAN_STATUS (front_end->demod, Si2168_SCAN_STATUS_CMD_INTACK_OK);
    SiTRACE("blindscan_status      %s\n", Si2168_L2_Trace_Scan_Status(front_end->demod->rsp->scan_status.scan_status) );
    if (front_end->demod->rsp->scan_status.scan_status != previous_scan_status) {
      SiTRACE ("scan_status changed from %s to %s\n", Si2168_L2_Trace_Scan_Status(previous_scan_status), Si2168_L2_Trace_Scan_Status(front_end->demod->rsp->scan_status.scan_status));
    }

    if (start_resume) {
      if (front_end->demod->cmd->scan_ctrl.action == Si2168_SCAN_CTRL_CMD_ACTION_START) {
        SiTRACE ("blindscan_interaction >> (start ) Si2168_L1_SCAN_CTRL( front_end->demod, %d, %8d) \n", front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
      } else {
        SiTRACE ("blindscan_interaction >> (resume) Si2168_L1_SCAN_CTRL( front_end->demod, %d, %8d) \n", front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
      }
      return_code = Si2168_L1_SCAN_CTRL (front_end->demod,               front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
      if (return_code != NO_Si2168_ERROR) {
        SiTRACE ("blindscan_interaction -- (error1) Si2168_L1_SCAN_CTRL %d      ERROR at %10d (%d)\n!!!!!!!!!!!!!!!!!!!!!!!\n", front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz, front_end->demod->rsp->scan_status.scan_status);
        return 0;
      }
    }
    front_end->demod->cmd->scan_ctrl.action = Si2168_SCAN_CTRL_CMD_ACTION_RESUME;

    searchStartTime = system_time();

    /* as we will not lock in less than min_lock_time_ms, wait a while... */
    while (system_time()-searchStartTime < min_lock_time_ms) {
      if (front_end->seekAbort) { break; }
      /* Adapt here the minimal 'reaction time' of the application*/
      system_wait(20);
    }

    /* The actual search loop... */
    while ( 1 ) {

      Si2168_L1_CheckStatus (front_end->demod);

      if ( (front_end->demod->status->scanint == Si2168_STATUS_SCANINT_TRIGGERED) ) {

        Si2168_L1_SCAN_STATUS (front_end->demod, Si2168_SCAN_STATUS_CMD_INTACK_CLEAR);
        SiTRACE("blindscan_status      %s\n", Si2168_L2_Trace_Scan_Status(front_end->demod->rsp->scan_status.scan_status) );
        skip_resume = 0;

        switch (front_end->demod->rsp->scan_status.scan_status) {
          case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_TUNE_REQUEST          : {
            SiTRACE ("blindscan_interaction -- (tune  ) SCAN TUNE_REQUEST at %d kHz\n", front_end->demod->rsp->scan_status.rf_freq);
            if (front_end->tuneUnitHz == 1) {
              seek_freq = Si2168_L2_Tune (front_end, front_end->demod->rsp->scan_status.rf_freq*1000 );
              seek_freq_kHz = seek_freq/1000;
            } else {
              seek_freq = Si2168_L2_Tune (front_end, front_end->demod->rsp->scan_status.rf_freq );
              seek_freq_kHz = seek_freq;
            }
            *freq = front_end->rangeMin = seek_freq;
            front_end->handshakeStart_ms = system_time();
            SiTRACE("blindscan_handshake : new handshake start %d (tuned)\n", front_end->handshakeStart_ms);
            /* as we will not lock in less than min_lock_time_ms, wait a while... */
            system_wait(min_lock_time_ms);
            break;
          }
          case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_DIGITAL_CHANNEL_FOUND : {
            *standard        = front_end->demod->rsp->scan_status.modulation;
            switch (front_end->demod->rsp->scan_status.modulation) {
              case Si2168_SCAN_STATUS_RESPONSE_MODULATION_DVBC : {
                *freq            = front_end->demod->rsp->scan_status.rf_freq*1000;
                *symbol_rate_bps = front_end->demod->rsp->scan_status.symb_rate*1000;
                Si2168_L1_DVBC_STATUS(front_end->demod, Si2168_DVBC_STATUS_CMD_INTACK_OK);
                front_end->demod->prop->dvbc_symbol_rate.rate = front_end->demod->rsp->scan_status.symb_rate;
                *constellation   = front_end->demod->rsp->dvbc_status.constellation;
                break;
              }
              default : {
                SiTRACE("Si2168_L2_Channel_Seek_Next DIGITAL_CHANNEL_FOUND error at %d: un-handled modulation (%d), aborting (skipped)\n", seek_freq, front_end->demod->rsp->scan_status.modulation);
                front_end->seekAbort = 1;
                return 0;
                break;
              }
            }
            SiTRACE ("blindscan_interaction -- (locked) SCAN DIGITAL lock at %7.3f MHz after %3d ms. modulation %3d (%s)\n", *freq/1000.0, searchDelay, *standard, Si2168_standardName(*standard) );
            front_end->handshakeOn = 0;
            front_end->handshakeStart_ms = system_time();
            SiTRACE("blindscan_handshake : new handshake start %d (found)\n", front_end->handshakeStart_ms);
            return 1;
            break;
          }
          case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_ERROR                 : {
            SiTRACE ("blindscan_interaction -- (error2) SCAN error at %7.3f after %3f ms\n", seek_freq, searchDelay);
            SiERROR ("SCAN status returns 'ERROR'\n");
            front_end->handshakeOn = 0;
            front_end->handshakeStart_ms = system_time();
            SiTRACE("blindscan_handshake : new handshake start %d (error)\n", front_end->handshakeStart_ms);
            return 0;
            break;
          }
          case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_SEARCHING             : {
            SiTRACE("SCAN Searching...\n");
            skip_resume = 1;
            break;
          }
          case  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_ENDED                 : {
            SiTRACE ("blindscan_interaction -- (ended ) SCAN ENDED\n");
            Si2168_L1_SCAN_CTRL (front_end->demod, Si2168_SCAN_CTRL_CMD_ACTION_ABORT , 0);
            front_end->handshakeOn = 0;
            return 0;
            break;
          }
          default : {
            SiTRACE("unknown scan_status %d\n", front_end->demod->rsp->scan_status.scan_status);
            skip_resume = 1;
            break;
          }
        }

        searchStartTime = system_time();

        if (skip_resume == 0) {
          SiTRACE ("blindscan_interaction >> (resume) Si2168_L1_SCAN_CTRL( front_end->demod, %d, %8d)\n", front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
          return_code = Si2168_L1_SCAN_CTRL (front_end->demod,              front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
          if (return_code != NO_Si2168_ERROR) {
            SiTRACE ( "Si2168_L1_SCAN_CTRL ERROR at %d (%d)\n!!!!!!!!!!!!!!!!!!!!!!!\n", seek_freq_kHz, front_end->demod->rsp->scan_status.scan_status);
            SiERROR ( "Si2168_L1_SCAN_CTRL 'RESUME' ERROR during seek loop\n");
          }
        }
      }

      /* timeout management (this should never happen if timeout values are correctly set) */
      searchDelay      = system_time() - searchStartTime;

      if (searchDelay >= max_decision_time_ms) {
        SiTRACE ("Scan decision timeout from  %7.3f after %d ms. Check your timeout limits!\n", seek_freq_kHz/1000.0, searchDelay);
        SiERROR ("Scan decision timeout (blind_mode = 1)\n");
        front_end->handshakeStart_ms = searchDelay;
        front_end->seekAbort   = 1;
        front_end->handshakeOn = 0;
        break;
      }

      if (searchDelay >= front_end->handshakePeriod_ms) {
        SiTRACE ("\nblindscan_handshake : handshake after %5d ms (at %10d). (handshake period %6d ms) %*s\n\n", searchDelay, front_end->rangeMin, system_time() - front_end->handshakeStart_ms, (system_time() - front_end->handshakeStart_ms)/1000,"*");
        SiTRACE ("%c", 0x07);
        if (front_end->handshakeUsed) {
         *freq                    = seek_freq;
          front_end->handshakeOn = 1;
          /* The application will check handshakeStart_ms to know whether the blindscan is ended or not */
          return system_time() - front_end->handshakeStart_ms;
        } else {
          SiTRACE ("\nblindscan_handshake : handshake not in use after %5d ms (at %10d). Staying in Seek_Next. (handshake period %6d ms) %*s\n\n", searchDelay, front_end->rangeMin, system_time() - front_end->handshakeStart_ms, (system_time() - front_end->handshakeStart_ms)/1000,"*");
        }
      }

      /* Check seekAbort flag (set in case of timeout or by the top-level application) */
      if (front_end->seekAbort) {
        /* Abort the SCAN loop to allow it to restart with the new rangeMin frequency */
        SiTRACE ("blindscan_interaction >> (abort!) Si2168_L1_SCAN_CTRL( front_end->demod, Si2168_SCAN_CTRL_CMD_ACTION_ABORT)\n");
                  Si2168_L1_SCAN_CTRL (front_end->demod, Si2168_SCAN_CTRL_CMD_ACTION_ABORT , 0);
        front_end->handshakeOn = 0;
        return 0;
        break;
      }

      /* Check status every 100 ms */
      system_wait(100);

    }
  }
  front_end->handshakeOn = 0;
  return 0;
}
/************************************************************************************************************************
  NAME: Si2168_L2_Channel_Seek_Abort
  DESCRIPTION: aborts the seek
  Programming Guide Reference:    Flowchart TBD (Channel Scan flowchart)

  Parameter:  Pointer to Si2168 Context
  Returns:    0 if successful, otherwise an error.
************************************************************************************************************************/
int  Si2168_L2_Channel_Seek_Abort (Si2168_L2_Context *front_end)
{
  front_end->seekAbort   = 1;
  front_end->handshakeOn = 0;
  return 0;
}
/************************************************************************************************************************
  NAME: Si2168_L2_Channel_Lock_Abort
  DESCRIPTION: aborts the lock_to_carrier
  Programming Guide Reference:    Flowchart TBD (Channel Lock flowchart)

  Parameter:  Pointer to Si2168 Context
  Returns:    0 if successful, otherwise an error.
************************************************************************************************************************/
int  Si2168_L2_Channel_Lock_Abort (Si2168_L2_Context *front_end)
{
  front_end->lockAbort = 1;
  front_end->handshakeOn = 0;
  return 0;
}
/************************************************************************************************************************
  NAME: Si2168_L2_Channel_Seek_End
  DESCRIPTION: returns the chip back to normal use following a seek sequence
  Programming Guide Reference:    Flowchart TBD (Channel Scan flowchart)

  Parameter:  Pointer to Si2168 Context
  Returns:    0 if successful, otherwise an error.
************************************************************************************************************************/
int  Si2168_L2_Channel_Seek_End   (Si2168_L2_Context *front_end)
{
  front_end = front_end; /* To avoid compiler warning */

  front_end->demod->prop->scan_ien.buzien           = Si2168_SCAN_IEN_PROP_BUZIEN_DISABLE ; /* (default 'DISABLE') */
  front_end->demod->prop->scan_ien.reqien           = Si2168_SCAN_IEN_PROP_REQIEN_DISABLE ; /* (default 'DISABLE') */
  Si2168_L1_SetProperty2(front_end->demod, Si2168_SCAN_IEN_PROP_CODE);

  front_end->seekRunning  = 0;

  switch (front_end->demod->standard)
  {
    case Si2168_DD_MODE_PROP_MODULATION_DVBT    : { front_end->demod->prop->dd_mode.modulation = Si2168_DD_MODE_PROP_MODULATION_DVBT ; break; }
    case Si2168_DD_MODE_PROP_MODULATION_DVBC    : { front_end->demod->prop->dd_mode.modulation = Si2168_DD_MODE_PROP_MODULATION_DVBC ; break; }
    case Si2168_DD_MODE_PROP_MODULATION_DVBT2   : { front_end->demod->prop->dd_mode.modulation = Si2168_DD_MODE_PROP_MODULATION_DVBT2; break; }
    default                                     : { SiTRACE("UNKNOWN standard %d\n", front_end->demod->standard); break;}
  }

  SiTRACE("auto_detect_TER %d\n",front_end->auto_detect_TER);
  if (front_end->auto_detect_TER) {
    switch (front_end->demod->standard)
    {
      case Si2168_DD_MODE_PROP_MODULATION_DVBT    :
      case Si2168_DD_MODE_PROP_MODULATION_DVBT2   : {
        front_end->demod->prop->dd_mode.modulation  = Si2168_DD_MODE_PROP_MODULATION_AUTO_DETECT;
        front_end->demod->prop->dd_mode.auto_detect = Si2168_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2;
      }
      default                                     : { break;}
    }
  }
  Si2168_L1_SetProperty2(front_end->demod, Si2168_DD_MODE_PROP_CODE);

  return 0;
}
/************************************************************************************************************************
  NAME: Si2168_TerAutoDetect
  DESCRIPTION: Set the Si2168 in Ter Auto Detect mode

  Parameter:  Pointer to Si2168 Context
  Returns:    front_end->auto_detect_TER
************************************************************************************************************************/
int  Si2168_TerAutoDetect         (Si2168_L2_Context *front_end)
{
  front_end->auto_detect_TER = 1;
  return front_end->auto_detect_TER;
}
/************************************************************************************************************************
  NAME: Si2168_TerAutoDetectOff
  DESCRIPTION: Set the Si2168 in Ter Auto Detect 'off' mode

  Parameter:  Pointer to Si2168 Context
  Returns:    front_end->auto_detect_TER
************************************************************************************************************************/
int  Si2168_TerAutoDetectOff      (Si2168_L2_Context *front_end)
{
  front_end->auto_detect_TER = 0;
  return front_end->auto_detect_TER;
}
#ifdef    SILABS_API_TEST_PIPE
int   Si2168_L2_Test_MPLP                (Si2168_L2_Context *front_end, double freq) {
  int num_plp;
  int plp_index;
  int plp_id;
  int plp_type;
  int data_plp_count;
  int lock_wait_start_ms;
  Si2168_L2_lock_to_carrier (front_end, Si2168_DD_MODE_PROP_MODULATION_DVBT2, freq, 8000000, 0, 0, 0, -1);
  if (front_end->demod->rsp->dd_status.dl == 0) {
    SiTRACE("Demod timeout after %4d ms\n", system_time() -  lock_wait_start_ms);
    return 0;
  }
  /* Locked; Now check DVBT2 status */
  if (Si2168_L1_DVBT2_STATUS    (front_end->demod, Si2168_DVBT2_STATUS_CMD_INTACK_OK) != NO_Si2168_ERROR) {
    SiTRACE("Si2168_L1_DVBT2_STATUS error\n");
    return 0;
  }
  SiTRACE("dvbt2_status.pcl     %d\n", front_end->demod->rsp->dvbt2_status.pcl);
  SiTRACE("dvbt2_status.num_plp %d\n", front_end->demod->rsp->dvbt2_status.num_plp);
  SiTRACE("dvbt2_status.plp_id  %d\n", front_end->demod->rsp->dvbt2_status.plp_id);
  num_plp = front_end->demod->rsp->dvbt2_status.num_plp;
  SiTRACE("There are %d PLPs in this stream\n", num_plp);
  data_plp_count = 0;
  for (plp_index=0; plp_index<num_plp; plp_index++) {
    Si2168_L1_DVBT2_PLP_INFO (front_end->demod, plp_index);
    plp_id   = front_end->demod->rsp->dvbt2_plp_info.plp_id;
    plp_type = front_end->demod->rsp->dvbt2_plp_info.plp_type;
    SiTRACE("PLP index %3d: PLP ID %3d, PLP TYPE %d : ", plp_index, plp_id, plp_type);
    if (plp_type == Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_COMMON) {
      SiTRACE("COMMON PLP at index %3d: PLP ID %3d, PLP TYPE %d\n", plp_index, plp_id, plp_type);
    } else {
      SiTRACE("DATA   PLP at index %3d: PLP ID %3d, PLP TYPE %d\n", plp_index, plp_id, plp_type);
      data_plp_count++;
    }
  }
  return data_plp_count;
}
/************************************************************************************************************************
  Si2168_L2_Test function
  Use:        Generic test pipe function
              Used to send a generic command to the selected target.
  Returns:    0 if the command is unknow, 1 otherwise
  Porting:    Mostly used for debug during sw development.
************************************************************************************************************************/
int   Si2168_L2_Test                     (Si2168_L2_Context *front_end, char *target, char *cmd, char *sub_cmd, double dval, double *retdval, char **rettxt)
{
  int i,c,p;
  int start_time_ms;
  front_end = front_end; /* To avoid compiler warning if not used */
  *retdval = 0;
  start_time_ms = system_time();
  SiTRACE("\nSi2168_L2_Test %s %s %s %f...\n", target, cmd, sub_cmd, dval);
       if (strcmp_nocase(target,"help"      ) == 0) {
    sprintf(*rettxt, "\n Possible Si2168 test commands:\n\
demod properties              : displays ALL current property fields\n\
demod getProperty <prop_code> : displays current property fields\n\
demod fef     <0-1>           : set FEF mode\n\
demod get_reg <reg_code>      : displays reg value\n\
"); return 1;
  }
  else if (strcmp_nocase(target,"demod"     ) == 0) {
  #ifdef    Si2168_GET_PROPERTY_STRING
    if (strcmp_nocase(cmd,"properties"  ) == 0) {
      SiTraceConfiguration("traces suspend");
      c = 0;
      for (i=0x0301; i<=0x0308; i++) { if (Si2168_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x0401; i<=0x0401; i++) { if (Si2168_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1001; i<=0x100f; i++) { if (Si2168_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1101; i<=0x1104; i++) { if (Si2168_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1201; i<=0x1203; i++) { if (Si2168_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1301; i<=0x1303; i++) { if (Si2168_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1401; i<=0x1403; i++) { if (Si2168_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1501; i<=0x1503; i++) { if (Si2168_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168_ERROR) {printf("%s\n",*rettxt); c++;} }
      SiTraceConfiguration("traces resume");
      sprintf(*rettxt, "%d properties checked\n", c);
      return 1;
    }
    if (strcmp_nocase(cmd,"getProperty" ) == 0) {
       *retdval = (double)Si2168_L1_GetPropertyString(front_end->demod, dval, (char *)" ", *rettxt); return 1;
    }
  #endif /* Si2168_GET_PROPERTY_STRING */
    if (strcmp_nocase(cmd,"fef"         ) == 0) {
       Si2168_L2_Tuner_I2C_Enable(front_end);
       *retdval = (double)Si2168_L2_TER_FEF(front_end, dval);
       Si2168_L2_Tuner_I2C_Disable(front_end);
       sprintf(*rettxt, "FEF %.0f\n", dval);
        return 1;
    }
    if (strcmp_nocase(cmd,"ber"         ) == 0) {
     if (strcmp_nocase(sub_cmd,"mant" ) == 0) {
      i = (int)dval;
      if (i !=0) {
       front_end->demod->prop->dd_ber_resol.mant                      =     (int) dval;
       Si2168_L1_SetProperty2(front_end->demod, Si2168_DD_BER_RESOL_PROP_CODE);
      }
      *retdval = (double)front_end->demod->prop->dd_ber_resol.mant;
      sprintf(*rettxt, "BER mant %.0f\n", *retdval);
     }
     if (strcmp_nocase(sub_cmd,"exp"  ) == 0) {
      i = (int)dval;
      if (i !=0) {
       front_end->demod->prop->dd_ber_resol.exp                       =     (int) dval;
       Si2168_L1_SetProperty2(front_end->demod, Si2168_DD_BER_RESOL_PROP_CODE);
      }
      *retdval = (double)front_end->demod->prop->dd_ber_resol.exp;
      sprintf(*rettxt, "BER exp  %.0f\n", *retdval);
     }
     if (strcmp_nocase(sub_cmd,"loop" ) == 0) {
       Si2168_L1_DD_BER(front_end->demod, Si2168_DD_BER_CMD_RST_CLEAR);
       printf("BER CLEAR\n");
       for (i=0; i< (int)dval; i++) {
        Si2168_L1_DD_BER(front_end->demod, Si2168_DD_BER_CMD_RST_RUN);
        if (front_end->demod->rsp->dd_ber.exp!=0) {
         p = 1;
         for (c = 1; c<=front_end->demod->rsp->dd_ber.exp; c++) {p = p*10;}
         *retdval = (front_end->demod->rsp->dd_ber.mant/10.0) / p;
         printf("%4d: (%6d ms) BER %8.2e", i+1, system_time() - start_time_ms, *retdval);
        } else {
         *retdval = 1.0;
         printf("%4d: (%6d ms) BER unavailable", i+1, system_time() - start_time_ms);
        }
        printf("\n");
        system_wait(20);
       }
       sprintf(*rettxt, "final BER %8.2e\n", *retdval);
     }
     return 1;
    }
    if (strcmp_nocase(cmd,"get_reg"     ) == 0) {
       i = (int)dval;
       Si2168_L1_DD_GET_REG(front_end->demod, (i>>16)&0xff, (i>>8)&0xff, (i>>0)&0xff );
       sprintf(*rettxt, "%d\n",
                (front_end->demod->rsp->dd_get_reg.data4<<24)
               +(front_end->demod->rsp->dd_get_reg.data3<<16)
               +(front_end->demod->rsp->dd_get_reg.data2<<8 )
               +(front_end->demod->rsp->dd_get_reg.data1<<0 )
        );
        return 1;
    }
    if (strcmp_nocase(cmd,"get_rev"     ) == 0) {
       Si2168_L1_GET_REV(front_end->demod);
       printf("Si21%2d Part running 'FW_%c_%c_b%d'\n", front_end->demod->rsp->part_info.part, front_end->demod->rsp->get_rev.cmpmajor, front_end->demod->rsp->get_rev.cmpminor, front_end->demod->rsp->get_rev.cmpbuild );
       return 1;
    }
    if (strcmp_nocase(cmd,"mplp"     ) == 0) {
       i = (int)dval;
       sprintf(*rettxt, "%d DATA streams\n", Si2168_L2_Test_MPLP(front_end, i) );
       return 1;
    }
    sprintf(*rettxt, "unknown '%s' demod command for Si2168, can not process '%s %s %s %f'\n", cmd, target, cmd, sub_cmd, dval);
    return 0;
  }
  else if (strcmp_nocase(target,"ter_tuner" ) == 0) {
    sprintf(*rettxt, "no ter_tuner command implemented so far for Si2168, can not process '%s %s %s %f'\n", target, cmd, sub_cmd, dval);
    return 0;
  }
  else if (strcmp_nocase(target,"sat_tuner" ) == 0) {
    sprintf(*rettxt, "no sat_tuner command implemented so far for Si2168, can not process '%s %s %s %f'\n", target, cmd, sub_cmd, dval);
    return 0;
  }
  else if (strcmp_nocase(target,"lnb_supply") == 0) {
    sprintf(*rettxt, "no lnb_supply command implemented so far for Si2168, can not process '%s %s %s %f'\n", target, cmd, sub_cmd, dval);
    return 0;
  }
  else {
    sprintf(*rettxt, "unknown '%s' command for Si2168, can not process '%s %s %s %f'\n", cmd, target, cmd, sub_cmd, dval);
    SiTRACE(*rettxt);
    SiERROR(*rettxt);
    return 0;
  }
  return 0;
}
#endif /* SILABS_API_TEST_PIPE */

/**/



