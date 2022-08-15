/***************************************************************************************
                  Silicon Laboratories Broadcast Si2168B Layer 2 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   L2 API for commands and properties
   FILE: Si2168B_L2_API.c
   Supported IC : Si2168B
   Compiled for ROM 0 firmware 3_A_build_3
   Revision: REVISION_NAME
   Tag:  V0.3.8_FW_4_4b17
   Date: May 22 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/
/* Change log: */
/* Last  changes:

  As from V0.3.8:
    <wrapper> Wrapper V2.4.4
    <correction> In Wrapper code, using dvbt2_cnr when in T2 (was inadvertently using dvbt_cnr as from wrapper V2.3.6)

  As from V0.3.7:
    <wrapper> Wrapper V2.4.1
    <firmware> With FW 4_4b17 for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b19 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download).
    <TER_Tuner_API>[V0.3.9] Using TER-Tuner API V0.3.9, to benefit from the 1.7 MHz filtering feature (not available with all TER tuners).
    <improvement>[TER_BW/1.7MHz] In Si2168B_L2_Tune: now using SILABS_BW enum as defined in SiLABS_TER_TUNER API V0.3.9, to
      use the 1.7 MHz filtering feature in SiLabs TER tuners whenever possible.
    <correction> [Switch_to_standard/same_clock_source] In Si2168B_L2_switch_to_standard: condition to pass through
      the demod init simplified to a standard change. The previous version worked fine when changing the clock source, but not
      when using the same clock source for TER and SAT. Adding SiTRACES for flags used in the function.
    <improvement>[Src_code_GUIs] In Si2168B_L2_Test: more complete testpipe 'demod help'
    <correction>[Tuner_i2c] In Si2183_L2_Tune: Moving 'ifdef    UNICABLE_COMPATIBLE' line around the closing
      bracket after disabling the SAT tuner i2c.
      The previous version didn't disable the tuner i2c with the following compilation flags:
      UNICABLE_COMPATIBLE      NOT defined
      INDIRECT_I2C_CONNECTION  defined
    <correction>[SAT_Blindscan/Spectrum_Display/Unicable] In Si2168B_plot: correction of displayed frequencies when plotting the
      SAT spectrum in Unicable mode.
    <improvement> [Unicable/Multi-Treading/Multiple frontends] In Si2168B_L2_TER_FEF_SETUP: removing I2C pass-through enable/disable.
    This is only called from switch_to_standard, and the i2c pass-through is already enabled when calling this function.
    The change removes nested i2c pass-through enable/disable calls.
    These had generally no consequences, except for duals when several tuners use the same i2c address.

  As from V0.3.6:
    <correction> [SILENT/SLEEP/ANALOG] In Si2168B_L2_switch_to_standard: Adding dtv_demod_sleeping flag to
        more easily handle the 'sleep' mode, which can occur upon a clock source change in DTV, or when
        going to 'ANALOG' or 'ATV'. The WAKEUP sequence is required in the first case, not in the second case.

*/
/* Older changes:

  As from V0.3.5:
    <correction> [SILENT/SLEEP/ANALOG] Changing Si2168B_SILENT to use L2 content.
        This provides access to the TER_init_done and SAT_init_done flags
        These need to be set to 0 when calling Si2168B_SILENT(front_end,1), to trigger a call to Si2168B_Configure when re-starting DTV reception.

  As from V0.3.4:
    <wrapper>  Wrapper V2.3.6
    <firmware> With FW 4_4b16 for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b18 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download).
    <improvement/DVB-T HP/LP> Setting dvbt_hierarchy.stream as Si2168B_DVBT_HIERARCHY_PROP_STREAM_HP.
      This allows locking on DVB-T hierarchical channels with low C/N allowing only HP reception
       (i.e. when the C/N is too low for a good lock on the LP stream).
    <improvement> [DUAL/TRIPLE/QUAD] In Si2168B_L2_switch_to_standard: over-riding clock flags if the clocks need to be always kept on or off
    <new feature> [SILENT] Adding Si2168B_SILENT, to allow settings all possible pins in tristate.
    <new feature> [SILENT] In Si2168B_L2_switch_to_standard: calling Si2168B_SILENT when called with either 'ANALOG' or 'SLEEP'
                           NB: when going to 'SLEEP', the TS pins are tristated. It is required to re-enable TS after the next lock.

  As from V0.3.3:
    <firmware> With FW 4_4b13 for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b15 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download).

  As from V0.3.2:
    <wrapper>  Wrapper V2.3.6
    <improvement> [no_float] In Si2168B_L2_Channel_Seek_Init: Computing scan_fmin.scan_fmin and scan_fmax.scan_fmax using a method compatible with no float use.
                             In Si2168B_L2_Channel_Seek_Next: Tracing frequency values using a method compatible with no float use.
    <improvement> [no_float] In Si2168B_L2_lock_to_carrier:   Computing lpf using a method compatible with no float use.
    <improvement> [TRACES]   In Si2168B_L2_lock_to_carrier:   Tracing DVB-S and S2 afc ranges and lpf
    <improvement> [FEF] In Si2168B_L2_TER_FEF_SETUP: adding fef_level in call to SiLabs_TER_Tuner_FEF_FREEZE_PIN_SETUP.
              This function has been redefined to allow making the FEF level consistent between demodulator and tuner
    <correction>[TRACES] in Si2168B_L2_Channel_Seek_Init: tracing afc ranges. The previous version didn't trace these values correctly

    <firmware> With FW 4_4b11 for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b13 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download).
    <new_feature> [TestPipe] Adding Si2168B_L2_Health_Check

  As from V0.3.1:
    <new_feature> [SPI] Adding SPI code:
      In Si2168B_PowerUpWithPatch: calling Si2168B_LoadFirmwareSPI if api->spi_download is set
      Adding Si2168B_LoadFirmwareSPI function
    <improvement> [code_checkers] In Si2168B_L2_switch_to_standard: using snprintf with a fixed size of 1000.
                                  Declaring sequence with a length of 1000.
    <improvement> [code_checkers] In Si2168B_L2_Infos:  using snprintf with a fixed size of 1000.
                                  CAUTION: The message strings must be declared by the caller with a length of 1000.
  As from V0.3.0:
    <wrapper>  Wrapper V2.3.4
    <correction> [DVBT2/FEF] In Si2168B_L2_Tune: Activate FEF management in all cases where the signal can be DVB-T2
    <new_feature>[DVBT2/FEF] In Si2168B_L2_TER_FEF: Adding the 'DTV_AGC_AUTO_FREEZE' call, to help when receiving DVB-T2 with FEF.
            NB: This feature is only available for the most recent (as from Si21x8B) TER tuners.

  As from V0.2.9:
    <wrapper>  Wrapper V2.3.4
    <firmware> With FW 4_4b9  for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b11 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download).
               With FW 3_Ab13 for ROM0 engineering samples, now without C2. C2 can't be fully supported with ROM0 parts.
    <correction> [DVBT2/FEF] In Si2168B_L2_TER_FEF_SETUP: connect tuner I2C before the FEF setup macros, as these macros need access to the tuner.
      As Si2168B_L2_TER_FEF_SETUP is only called from Si2168B_L2_switch_to_standard while the tuner i2c is connected there is no issue when using the
       SiLabs API wrapper as in the example console code. However, because the related macros used in Si2168B_L2_TER_FEF_SETUP issue tuner properties,
        it is better to enable I2C when entering the function, in case this function is called directly.

  As from V0.2.8:
    <wrapper>  Wrapper V2.3.4
    <firmware> With FW 4_4b8  for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b10 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download).
               With FW 3_Ab13 for ROM0 engineering samples, now without C2. C2 can't be fully supported with ROM0 parts.

    <code_checkers> In Si2168B_L2_Channel_Seek_Next:
      Added start_resume = 0, to avoid code checker warning. start_resume is always set by construction, as front_end->handshakeUsed
        can only be '0' or '1', but code checkers don't see this.
    <code_checkers> In Si2168B_L2_lock_to_carrier:
      Added new_lock = 1, to avoid code checker warning. start_resume is always set by construction, as front_end->handshakeUsed
        can only be '0' or '1', but code checkers don't see this.
    <code_checkers> In Si2168B_Media: adding missing break in switch.
    <improvement> [Si2160] In Si2168B_L2_Channel_Seek_Init: tracing DVB-T/T2 AFC ranges separately. (Si2160 doesn't support T2)

  As from V0.2.7:
  <correction>[BLINDSCAN/HANDSHAKE] In Si2168B_L2_Channel_Seek_Next:
    Correct timeoutStartTime management.
      When 'blind_mode = 1' It worked if handshakeUsed = '0', but was improperly overwritten when handshakeUsed = '1'.
      If 'handshakeUsed = 1', force min_lock_time_ms to 0, to avoid waiting when using the handshake feature.
  <improvement>[BLINDSCAN/TRACES] In Si2168B_L2_Channel_Seek_Next:
    Tracing the initial scan_ctrl.action only if 'blind_mode = 1'. (it's not relevant otherwise)
  <improvement>[DVB-T2-Lite] In Si2168B_L2_Channel_Seek_Init: removing dvbt2_mode.lock_mode (not forced to 'ANY'), to allow
        T/T2 scanning in 'T2-Base' mode. This flag is controlled via the default value set in Si2168B_storeUserProperties, and
      later on using the new wrapper function SiLabs_API_TER_T2_lock_mode
  <improvement>[DVB-T2-Lite] In Si2168B_L2_Channel_Seek_Next: returning dvbt2_status.t2_mode + 1 in *T2_base_lite, to match the
    values used in SiLabs_API_TER_T2_lock_mode (0='ANY', 1='T2-Base', 2='T2-Lite')

  As from V0.2.6:
    <firmware> With FW 4_4b7  for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b9  for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download).
               With FW 3_Ab12 for ROM0 engineering samples, now without C2. C2 can't be fully supported with ROM0 parts.
    <improvement> [BLINDSCAN/DVB-C] In Si2168B_L2_Channel_Seek_Init: forcing dvbc_constellation.constellation to 'AUTO'.
                    Otherwise only the current constellation is detected.
    <new feature> [MCNS] In Si2168B_L2_Channel_Seek_Init, Si2168B_L2_Channel_Seek_Next : adding MCNS support.
    <new feature> [TESTPIPE] Adding DVB-T2 signaling support
    <improvement> [TESTPIPE] In Si2168B_L2_Test: tracing additional properties.
    <improvement> [TER AGC]  In Si2168B_L2_TER_AGC: checking AGC TER settings to set front_end->demod->tuner_ter_agc_control at '0'
                              when the TER AGC is not controlled from the demodulator
                             In Si2168B_L2_switch_to_standard:
                              Setting TER AGC to 'EXTERNAL' only if front_end->demod->tuner_ter_agc_control is '1'
    <traces> [BLINDSCAN] In Si2168B_L2_Channel_Seek_Init: tracing AFC ranges
    <traces> [BLINDSCAN DEBUG] In Si2168B_plot: spectrum traces now OK for DVB-C blindscan
    <traces> [DVB-T2] In Si2168B_lock_to_carrier: tracing T2 lock mode
    <traces> [BLINDSCAN] In Si2168B_L2_Channel_Seek_Next: tracing max_lock_time_ms

  As from V0.2.5:
    <firmware> With FW 4_4b4 for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b6 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download).
               With FW 3_Ab11 for ROM0 engineering samples, now without C2. C2 can't be fully supported with ROM0 parts.
    <correction> In FW: Having scan_status.buz flag set to 'buzy' at API level before being set to 'buzy' in the MCU code.
                         Before this, SCAN_CTRL errors could occur if SCAN_CTRL/START was issued less than (about) 10 ms after SCAN_CTRL/ABORT.
                         This occurred on platforms with 'fast' i2c, when calling Seek_Init/Seek_Next very fast, when it was 'buzy'
                          in MCU but wasn't reported as 'buzy' at API level..
                 In SW:  Before issuing SCAN_CTRL/START, the SW now checks the scan_status.buz field, making sure it's not 'buzy'.
    <improvement> [BLINDSCAN] In Si2168B_L2_Channel_Seek_Next: tracing and waiting for 'buz=0' before issuing SCAN_CTRL START/RESUME
    <improvement> [BLINDSCAN] In Si2168B_L2_Channel_Seek_Init  enabling BUZINT interrupt, to get the scan_status.buz status visible
                               in status.scanint
    <improvement> [ATV] In Si2168B_L2_switch_to_standard:
      When switching to ATV, forcing the demodulator for DVB-T/non auto detect reception before POWER_DOWN
      This is to make sure that the FEF pin will be at the level defined by DD_MP_DEFAULTS
    <new feature>  In Si2168B_L2_Test (if SILABS_API_TEST_PIPE):
      Adding DVB-T2 signalling code (used by 'l1_pre'/'l1_post'/'l1_misc' options. NOT FULLY FUNCTIONAL YET

  As from V0.2.4:
    <wrapper>  Wrapper V2.3.1
    <firmware> With FW 4_4b3 for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
               With FW 4_0b5 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download)
               With FW 3_Ab9 for ROM0 engineering samples, now without C2. C2 can't be fully supported with ROM0 parts.
    <correction>  [T/T2] In Si2168B_L2_Channel_Seek_Init:
     Moving DD_MODE after all DD_MODE property fields are set (previous version had issues with 7MHz/8MHz TER scan transition)
     Moving DD_RESTART after all properties are set (not really required, as there is another DD_RESTART in Seek_Next, but better for clarity)
    <correction>  [S/S2/DSS] In Si2168B_L2_SAT_LPF: (if INDIRECT_I2C_CONNECTION) calling the proper callback function to connect the SAT tuner.
      This probably never created any issue, as most applications generally connect both tuners via the same demodulator
    <correction>  [MCNS] In Si2168B_L2_Channel_Seek_End: adding the MCNS case.
    <improvement> [T]    In Si2168B_L2_Channel_Seek_Init: forcing dvbt_hierarchy.stream to keep track of the current stream selection
    <improvement> [T]    In Si2168B_L2_Channel_Seek_Next: storing dvbt_hierarchy.stream in *stream
    <improvement> [MCNS] In Si2168B_L2_lock_to_carrier:
      Using  Si2168B_DVB_C_max_lock_ms for MCNS, as for DVB-C. (Only impacting MCNS time out)
      Removing duplicate timeout settings in MCNS
    <new feature>  clock_control:
      Adding 'int clock_control' to Si2168B_L2_TER_Clock and Si2168B_L2_SAT_Clock functions.
      This is used for multi-frontends applications when a tuner's clock is forwarded to another frontend.
        In this case it needs to be 'ALWAYS_ON'.
        To keep the previous behavior, use '2' (i.e. the 'MANAGED' mode)
      In Si2168B_L2_switch_to_standard: managing tuner clocks depending on the corresponding clock_control values.
    <new feature>  In Si2168B_L2_Test (if SILABS_API_TEST_PIPE):
      Adding 'download' option, to allow controlling the propertyWriteMode from the SILABS_API_TEST_PIPE
      Adding 'dump' option, useful for debugging difficult cases
      Adding 'init_done' / 'clock_on' / 'clock_off' / 'init_after_reset' / 'standby' / 'wakeup' options for TER and SAT
    <traces>      In Si2168B_DVB_C_max_lock_ms: tracing the resulting value
    <cleanup>     In Si2168B_typedefs.h: removing unused declarations
    <cleanup>     In Si2168B_L2_lock_to_carrier: Unwanted info text removed from function documentation
    <cleanup>     Removing seekRunning from Si2168B_L2_Context (unused)

  As from V0.2.3:
    With wrapper V2.3.0.
    This corrects an improper type change in SiLabs_API_lock_to_carrier, which broke the DVB-C AUTO QAM capability.
    Changes are limited to the wrapper code.

  As from V0.2.2:
    Adding fef_pin and fef_level in L1_Si2168B_Context
    Adding Si2168B_L2_TER_FEF_CONFIG, to make it easy to configure the FEF for DVB-T2
     NB: The FEF configuration can't be identical for both dies in a dual demod,
          where DEMOD A can only control MP_A/MP_C, DEMOD B can only control MP_B/MP_D.
         The typical use case is:
             | DEMOD A (Die 1) | DEMOD B (Die 2) |
     TER AGC | MP_A (agc2)     | MP_B (agc2)     |
     SAT AGC | MP_C (agc1)     | MP_D (agc1)     |
     TER FEF | MP_C (active 1) | MP_D (active 1) |
     This is obtained after calling:
     For DEMOD A:
      Si2168B_L2_TER_AGC    ('front_end[A]', 0x0, 0, 0xa, 0);
      Si2168B_L2_SAT_AGC    ('front_end[A]', 0xc, 1, 0x0, 0);
      Si2168B_L2_TER_FEF_CONFIG('front_end[A]', 1, 0xc, 1);
     For DEMOD B:
      Si2168B_L2_TER_AGC    ('front_end[B]', 0x0, 0, 0xb, 0);
      Si2168B_L2_SAT_AGC    ('front_end[B]', 0xd, 1, 0x0, 0);
      Si2168B_L2_TER_FEF_CONFIG('front_end[B]', 1, 0xd, 1);
    In Si2168B_Configure:
     Configure DD_MP_DEFAULTS based on fef_pin/fef_level values, including limitations for dual demodulators.
     Remove dvbt2_fef fields setup, as it's now in Si2168B_L2_TER_FEF_CONFIG.
     Apply DVBT2_FEF command when in TER.
      To be configured like with the previous version, call Si2168B_L2_TER_FEF_CONFIG(front_end, 0xb, 1);
     Trace Die if dual part
    In Si2168B_L2_Part_Check:
      Correcting typo (',' instead of ';' at the end of one line)
    Changing Si2168B_L2_switch_to_standard and Si2168B_L2_set_standard function definitions to use 'unsigned char' instead of 'int' for standard.
    Changing Si2168B_L2_lock_to_carrier function definitions to use 'unsigned char' instead of 'int' for standard/stream/constellation/polarization/band/T2_lock_Mode.
     This avoids casting the related values to (unsigned int) within the functions when compiled with '-pedantic'.
      data_slice_id and plp_id are kept as 'int', as they may take a value of '-1' at wrapper level to select the corresponding 'auto' modes.
    In Si2168B_L2_lock_to_carrier:
      Clean up of calls to DVBT2_PLP_SELECT:
        Removed one redundant call.
        Now using similar calls in both places (AUTO_T_T2 and T2-only).
      Casting plp_id and dataslice_id to 'unsigned char' when stored in command fields.
        This is required because those can be set to '-1' to select the 'auto' mode.
      Clean up of some SiTRACE: using %ld for 'long' types
    In Si2168B_L2_Test:
      Limiting string lengths below 500 in sprintf and printf, to avoid warnings when compiled with '-pedantic'

  As from V0.2.1:
   With Wrapper V2.2.8
   With FW 4_4b2 for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2).
   With FW 4_0b4 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download)
   With FW 3_Ab7 for ROM0 engineering samples, now without C2. C2 can't be fully supported with ROM0 parts.
   In Si2168B_L2_lock_to_carrier:
     Adding an option to allow testing the relock time upon a reset (if freq<0)
     Reducing lock check wait from 10 to 5 ms

  As from V0.2.0:
   Corrected tag used by the export tool
   With FW 3_Ab6 for ROM0 engineering samples, now without C2. C2 can't be fully supported with ROM0 parts.
   Updating Si2168B_plot function to get the proper spectrum display during blindscan.

  As from V0.1.9:
   Moving FW files inclusion after including Si2168B_L2_API.h (useful for export tools).
   With FW 4_4b1 for Si2168B-A40, Si2162-A40 and Si2160-A40 (patch with DVB-C2)
   With FW 4_0b3 for Si2169-B40 and Si2168B-40 (patch without DVB-C2, for smaller size and faster download)
   With FW 3_Ab5 for ROM0 engineering samples, now without C2. C2 can't be fully supported with ROM0 parts.

   Adding Si2168B_LoadFirmware_16 function, to allow faster FW download (in 16 bytes mode)

   In Si2168B_PowerUpWithPatch:
    Adding code to:
     Load a patch with DVB-C2 in Si2168B/62/60 ROM1 parts (checking all PART_INFO fields)
     Load a smaller patch     in Si2169B/68B  ROM1 parts (checking all PART_INFO fields)

   ------------------------------------------------------------------------------------------------------------
   WARNING 1:
    The compilation flags have been changed to match the final part marking:
      The code is now using Si2168B_40_COMPATIBLE (instead of Si2168B_40_COMPATIBLE) and Si2168B_ES_COMPATIBLE
      It is required to change the compilation flags at project level.
   ------------------------------------------------------------------------------------------------------------
   WARNING 2:
    ROM0 (engineering samples) parts will not support DVB-C2 anymore, as there is not enough memory left to
      support the DVB-C2 patches together with the other standards.
    To experiment with DVB-C2, use ROM1 parts (with Si216x-y40 marking).
   ------------------------------------------------------------------------------------------------------------

   In Si2168B_L2_Test:
     Adding MCM infos in 'get_rev' option

  As from V0.1.8:
   With FW 3_Ab5
   In Si2168B_L2_lock_to_carrier:
     Adding T2_lock_mode flag (selects whether to lock on the T2-Base or T2-Lite signal (o='any'))
     WARNING: data_slice_id parameter now before plp_id, to match the wrapper code and the C2 system logic
               (In a C2 system, the data slice is selected first, then the PLP is selected inside the data slice).
   In Si2168B_L2_switch_to_standard:
    Using TER_TUNER_DTV_LIF_OUT_AMP to set DTV_LIF_OUT differently for 'qam' and 'ofdm' TER reception
   In Si2168B_L2_lock_to_carrier:
    C2 locking capability added, with dual-tune if not on the final frequency for the selected dataslice.
   In Si2168B_L2_Channel_Seek_Init:
    Setting T2 lock mode to 'any'
    Setting C2 select mode to 'AUTO'
   In Si2168B_L2_Channel_Seek_Next:
    Adding T2_base_lite flag  (indicates whether the locked signal is T2-Base or T2-Lite)
    C2 scan capability added.

  As from V0.1.7:
   In Si2168B_L2_switch_to_standard:
    Using TER_TUNER_DTV_LIF_OUT to set DTV_LIF_OUT differently for 'cable' and 'terrestrial' reception
   In wrapper code: adding DVB-C SSI/SQI in SiLabs_API_SSI_SQI

  As from V0.1.6:
   In Si2168B_L2_Channel_Seek_Next: moving timeoutStartTime setup right before DD_RESTART.

  As from V0.1.5:

    In Si2168B_L2_Channel_Seek_Next:
     In DVB-T: using front_end->seekStepHz as the step when not locked.
               using front_end->seekBWHz   as the step when locked.

  As from V0.1.4:

    API changes:
      Adding new field in property DD_TS_MODE_PROP: ts_freq_resolution
      Adding new property DD_TS_SERIAL_DIFF

  As from V0.1.3:

    Compatibility with the SILABS_SAT_TUNER_API, the new method used to control satellite tuners,
     with no change in the demodulator code.
     (this requires compiling with the SiLabs_SAT_Tuner code and defining SAT_TUNER_SILABS at project level).
    Adding Si2168B_L2_TER_AGC and Si2168B_L2_SAT_AGC functions, to make it easy to configure the AGCs

  As from V0.1.2:

    With FW 3_Ab3

    API change: adding mcm_die field in GET_REV reply (0 = SINGLE, 1 = DIE_A, 2 = DIE_B)

    Compatibility with the SILABS_TER_TUNER_API, the new method used to control SiLabs terrestrial tuners,
     with no change in the demodulator code.
     (this requires compiling with the SiLabs_TER_Tuner code and defining TER_TUNER_SILABS at project level).

    Compatibility with INDIRECT_I2C_CONNECTION compilation, a mode allowing tuner access from other demoulators,
     used for multi-frontend applications

    Adding Si2168B_L2_TER_Clock and Si2168B_L2_SAT_Clock functions, to make it easy to configure the clock paths

    In Si2168B_L2_switch_to_standard: adding C2 case when setting flags

    In Si2168B_L2_lock_to_carrier and Si2168B_L2_Channel_Seek_Next:
     Rework of the handshake feature with different flags used to check several items:
      A- The time spent in the function (to return if in handshake mode and longer than handshakePeriod_ms)
      B- The total time spent trying to lock/detect a channel (compared to the max allowed time)
      C- The time between 2 interactions with the FW (making sure the FW is running as expected).

  As from V0.1.1.3Ab2:   With FW 3_Ab2
  As from V0.1.1.3Ab1:   With FW 3_Ab1

  As from V0.1.1:
   Adding DD_DISEQC_PARAM property, to select the sequence mode between 'Unicable' and 'Normal' SAT modes

  As from V0.1.0:
   First version after receiving the silicon.

  As from V0.0.3 (in preparation):
   Adding Si2168B_L2_prepare_diseqc_sequence and Si2168B_L2_trigger_diseqc_sequence to allow preparing
    the DiSEqC message and sending it in two steps. This is required for Unicable with Si2168B, as otherwise the
    preparation takes too much time to stay within the Unicable Td specification (4 to 22 ms)

 As from V0.0.2:
  Compiled for ROM 0 firmware 0_A_build_1
  Si2168B_L2_Test_MPLP renamed Si2168B_L2_Test_T2_MPLP
  In Test pipe:
   Retrieving C2 system info

 As from V0.0.1:
  In sync with Si2169 V2.6.4 code.
  Compatibility with Si2169 (to allow development on a Si2169 EVB)

 As from V0.0.0:
  Initial version (based on Si2169 code V2.6.1)

****************************************************************************************/

/* Si2168B API Specific Includes */
#define   SILABS_API_LAYER 2
#include "Si2168B_L2_API.h"               /* Include file for this code */
#ifdef    Si2168B_40_COMPATIBLE
 /* FW without DVB-C2 (4_0b3 is the minimal patch for Si2169B-A40/Si2169B-40 NIM production testing) */
 #include "Si2168B_ROM1_Patch16_4_0b19.h"     /* 16 bytes mode patch compatible with Si2168B-A40, Si2169-B40 and Si268-B40 (smaller patch with no DVB-C2) */
/*#include "Si2168B_ROM1_SPI_4_0b13.h"*/      /* SPI      mode patch compatible with Si2168B-A40, Si2169-B40 and Si268-B40 (smaller patch with no DVB-C2) */
#endif /* Si2168B_40_COMPATIBLE */
#ifdef    Si2168B_ES_COMPATIBLE
 #include "Si2168B_ROM0_Firmware_3_Ab14.h"     /* firmware compatible with Si2168B-A3A (ROM0) */
 #include "Si2168B_ROM0_SPI_3_Ab14.h"          /* SPI      mode patch compatible with Si2168B-A3A, Si2169-B3A and Si268-B3A (smaller patch with no DVB-C2) */
#endif /* Si2168B_ES_COMPATIBLE */
#ifdef    Si2169_30_COMPATIBLE
/* #include "Si2169_30_ROM3_Patch_3_0b18.h" */         /* patch    compatible with Si2169-30 ROM 3 */
#endif /* Si2169_30_COMPATIBLE */
#ifdef    Si2167B_20_COMPATIBLE
 #include "Si2167B_ROM0_Patch_2_0b22.h"          /* firmware compatible with Si2167B_20 */
#endif /* Si2167B_20_COMPATIBLE */
#ifdef    Si2183_ES_COMPATIBLE
 #include "Si2183_ROM0_Firmware_4_Ab1.h"     /* firmware compatible with Si2183-A3A (ROM0) */
#endif /* Si2183_ES_COMPATIBLE */

#define Si2168B_BYTES_PER_LINE 8
#ifdef    ALLOW_Si2168B_BLINDSCAN_DEBUG
 #ifdef    SiTRACES
  #define Si2168B_FORCED_TRACE(...)             SiTraceConfiguration("traces resume"); SiTraceFunction(2,"plot ",__FILE__, __LINE__, __func__,__VA_ARGS__); SiTraceConfiguration("traces suspend");
 #else  /* SiTRACES */
  #define Si2168B_FORCED_TRACE                  printf
 #endif /* SiTRACES */
int  inter_carrier_space;
/************************************************************************************************************************
  Si2168B_plot function
  Use:        Si2168B plot function for DVB-C and SAT blindscan
              Used to  print trace messages in the console to allow drawing the scan spectrum
  Porting:    Useful to validate the blindscan sw porting. Can be removed once working
  Parameter:  source,  a string to display, also used to select the Si2168B_plot mode
************************************************************************************************************************/
void Si2168B_plot                             (Si2168B_L2_Context   *front_end, const char* source, char signed_val, long currentRF) {
  #ifdef     PLOT_WITH_BACKOFF_INFO
  int nb_channel;
  #endif /* PLOT_WITH_BACKOFF_INFO */
  int nb_word;
  int count;
  int w;
  int blanks;
  int read_data;
  int msb, lsb;
  int fw_nb_try_lock;
  int trylock_center;
  int trylock_symbol_rate;
  int trylock_offset;
  int tune_freq;
  int spectrum_start_freq;
  int spectrum_stop_freq;
  int analysis_bw;
  int analyzis_start_freq;
  int analyzis_stop_freq;
  int lsa_ch_offset;
  int lsa_symb_rate;
  int standard_specific_spectrum_offset;
  int standard_specific_spectrum_scaling;
  int standard_specific_freq_unit;
  int nb_skip_first_words;

#ifdef   Si2168B_READ
  int gp_reg8_3;
  int gp_reg16_0;
  int gp_reg16_1;
  int gp_reg16_2;
  int gp_reg16_3;
  int gp_reg32_0;
  int gp_reg32_1;
  int gp_reg32_2;
  int dcom_read;
  int if_freq_shift;
#define   Si2168B_REGISTERS
#ifdef    Si2168B_REGISTERS
#define Si2168B_en_rst_error_LSB    0
#define Si2168B_en_rst_error_MID  244
#define Si2168B_en_rst_error_MSB    9

#define Si2168B_dcom_control_byte_LSB   31
#define Si2168B_dcom_control_byte_MID  252
#define Si2168B_dcom_control_byte_MSB    9

#define Si2168B_dcom_addr_LSB   31
#define Si2168B_dcom_addr_MID    0
#define Si2168B_dcom_addr_MSB   10

#define Si2168B_dcom_data_LSB   31
#define Si2168B_dcom_data_MID    4
#define Si2168B_dcom_data_MSB   10

#define Si2168B_dcom_read_LSB   31
#define Si2168B_dcom_read_MID   44
#define Si2168B_dcom_read_MSB   10

#define Si2168B_dcom_read_toggle_LSB    0
#define Si2168B_dcom_read_toggle_MID   43
#define Si2168B_dcom_read_toggle_MSB   10

#define Si2168B_gp_reg16_0_LSB   15
#define Si2168B_gp_reg16_0_MID   60
#define Si2168B_gp_reg16_0_MSB   10

#define Si2168B_gp_reg16_1_LSB   15
#define Si2168B_gp_reg16_1_MID   62
#define Si2168B_gp_reg16_1_MSB   10

#define Si2168B_gp_reg16_2_LSB   15
#define Si2168B_gp_reg16_2_MID   64
#define Si2168B_gp_reg16_2_MSB   10

#define Si2168B_gp_reg16_3_LSB   15
#define Si2168B_gp_reg16_3_MID   66
#define Si2168B_gp_reg16_3_MSB   10

#define Si2168B_gp_reg32_0_LSB   31
#define Si2168B_gp_reg32_0_MID   48
#define Si2168B_gp_reg32_0_MSB   10

#define Si2168B_gp_reg32_1_LSB   31
#define Si2168B_gp_reg32_1_MID   52
#define Si2168B_gp_reg32_1_MSB   10

#define Si2168B_gp_reg32_2_LSB   31
#define Si2168B_gp_reg32_2_MID   56
#define Si2168B_gp_reg32_2_MSB   10

#define Si2168B_gp_reg8_3_LSB    7
#define Si2168B_gp_reg8_3_MID   71
#define Si2168B_gp_reg8_3_MSB   10

#define Si2168B_if_freq_shift_LSB   28
#define Si2168B_if_freq_shift_MID   92
#define Si2168B_if_freq_shift_MSB    4
#endif /* Si2168B_REGISTERS */

#endif /* Si2168B_READ */
  if (front_end->demod->media == Si2168B_TERRESTRIAL) {
    inter_carrier_space = 1000000.0*64/(7*1024);
    standard_specific_spectrum_offset  =   0;
    standard_specific_spectrum_scaling = 400;
    standard_specific_freq_unit = 1;
    SiTRACE("DVB-C spectrum ");
  } else {
    inter_carrier_space = 106600000/2048;
    standard_specific_spectrum_offset  = 650;
    standard_specific_spectrum_scaling =  75;
    standard_specific_freq_unit = 1024;
    SiTRACE("SAT spectrum ");
  }
  SiTRACE("inter_carrier_space %d\n", inter_carrier_space);
  count = 0;
  SiTraceConfiguration("traces suspend");
  /* Stop Si2168B DSP */
  Si2168B_WRITE(front_end->demod, en_rst_error,      0);
  Si2168B_WRITE(front_end->demod, dcom_control_byte, 0xc0000000);
  Si2168B_WRITE(front_end->demod, dcom_addr,         0x90000000);
  Si2168B_WRITE(front_end->demod, dcom_data,         0x000004f0);

  Si2168B_READ (front_end->demod, gp_reg16_0);
  nb_word = gp_reg16_0;

  Si2168B_READ  (front_end->demod, gp_reg32_0);
  Si2168B_READ  (front_end->demod, gp_reg32_1);
  Si2168B_READ  (front_end->demod, gp_reg32_2);
  Si2168B_READ  (front_end->demod, gp_reg16_1);
  Si2168B_READ  (front_end->demod, gp_reg16_2);
  Si2168B_READ  (front_end->demod, gp_reg16_3);
  Si2168B_READ  (front_end->demod, gp_reg8_3);
  Si2168B_READ  (front_end->demod, if_freq_shift);

  if (front_end->demod->prop->scan_sat_unicable_bw.scan_sat_unicable_bw == 0) {
    analysis_bw = 2*nb_word*inter_carrier_space;
    nb_skip_first_words=0;
    tune_freq           = gp_reg32_2*standard_specific_freq_unit;
    analyzis_stop_freq  = tune_freq + analysis_bw/2;
    analyzis_start_freq = tune_freq - analysis_bw/2;
    spectrum_stop_freq  = analyzis_stop_freq;
    spectrum_start_freq = spectrum_stop_freq - inter_carrier_space*2*nb_word;
  } else {
    analysis_bw = front_end->demod->prop->scan_sat_unicable_bw.scan_sat_unicable_bw*100000;
    nb_skip_first_words=(1353 - front_end->demod->prop->scan_sat_unicable_bw.scan_sat_unicable_bw*2048/1066)/2; /* scan_sat_unicable_bw  property is in 100kHz unit*/
    tune_freq           = gp_reg32_2*standard_specific_freq_unit;
    analyzis_stop_freq  = tune_freq + analysis_bw/2;
    analyzis_start_freq = analyzis_stop_freq - analysis_bw/2;
    spectrum_stop_freq  = tune_freq + analysis_bw/2;
    spectrum_start_freq = tune_freq - analysis_bw/2;
  }

/*  tune_freq           = gp_reg32_2*standard_specific_freq_unit;
  analyzis_stop_freq  = tune_freq + analysis_bw/2;
  analyzis_start_freq = tune_freq - analysis_bw/2;
  spectrum_stop_freq  = analyzis_stop_freq;
  spectrum_start_freq = spectrum_stop_freq - inter_carrier_space*2*nb_word;
*/
  Si2168B_FORCED_TRACE("tune_freq %8d, spectrum_start_freq %8d, spectrum_stop_freq %8d, analyzis_start_freq %8d, analyzis_stop_freq %8d, analysis_bw %8d, inter_carrier_space %8d\n", tune_freq, spectrum_start_freq, spectrum_stop_freq, analyzis_start_freq, analyzis_stop_freq, analysis_bw, inter_carrier_space);

  currentRF = spectrum_start_freq;

  if (strcmp(source,"spectrum")==0) {
  #ifdef    PLOT_ON_LIMITED_RANGE
    if (spectrum_stop_freq >= LIMITED_RANGE_MIN_RF) {
      if (spectrum_start_freq <= LIMITED_RANGE_MAX_RF) {
  #endif /* PLOT_ON_LIMITED_RANGE */
        Si2168B_FORCED_TRACE("spectrum center %10.6f (from %10.6f to %10.6f) nb_word %d nb_skip_first_words %d    {\n", gp_reg32_2/1000000.0, (currentRF)/1000000.0, (gp_reg32_2 + (inter_carrier_space*nb_word/1))/1000000.0, nb_word,nb_skip_first_words );
        Si2168B_FORCED_TRACE("if_freq_shift %10d \n", if_freq_shift );
  #ifdef    PLOT_ON_LIMITED_RANGE
      } else {
        Si2168B_FORCED_TRACE("not traced because above LIMITED_RANGE_MAX_RF...\n");
  }
    } else {
        Si2168B_FORCED_TRACE("not traced because below LIMITED_RANGE_MIN_RF...\n");
    }
  #endif /* PLOT_ON_LIMITED_RANGE */
  }
  if (strcmp(source,"trylock" )==0) {
    fw_nb_try_lock = (gp_reg16_2&0x0F00)>>8;
    if (gp_reg16_1 >> 15) {  gp_reg16_1 = gp_reg16_1 - 0xFFFF -1; }
    lsa_symb_rate              = gp_reg16_0;
    lsa_ch_offset              = gp_reg16_1;
    if (front_end->demod->media == Si2168B_TERRESTRIAL) {
    trylock_center             = gp_reg32_2;
    trylock_symbol_rate        = gp_reg16_0*4096;
    } else {
    trylock_center             = gp_reg32_2*1024/1000;
    trylock_symbol_rate        = gp_reg32_0*1024;
    }
    trylock_offset             = lsa_ch_offset*inter_carrier_space;
  #ifdef    PLOT_ON_LIMITED_RANGE
          if (trylock_center >= LIMITED_RANGE_MIN_RF) {
            if (trylock_center <= LIMITED_RANGE_MAX_RF) {
  #endif /* PLOT_ON_LIMITED_RANGE */
              Si2168B_FORCED_TRACE("blindscan_interaction trylock    center %7.3f / SR %10.3f\n"
           ,  trylock_center/1000.0
           ,  trylock_symbol_rate/1000000.0
           );
        Si2168B_FORCED_TRACE("if_freq_shift %10d \n", if_freq_shift );
        Si2168B_FORCED_TRACE("freq_corr_t   %10d \n", if_freq_shift );
#if 0
    for (nb_try_lock = 0; nb_try_lock< fw_nb_try_lock; nb_try_lock++) {
      lsa_symb_rate_i = lsa_symb_rate;
       /** for try 0 and 1 keep the freq lsa estimation   */
       /** for try 2 take the freq lsa estimation - 12.5% */
       /** for try 3 take the freq lsa estimation + 12.5% */
       lsa_ch_offset_i=lsa_ch_offset;
       if (nb_try_lock==2) {
               lsa_ch_offset_i=lsa_ch_offset-(lsa_ch_offset>>3);
  }
       if (nb_try_lock==3) {
               lsa_ch_offset_i=lsa_ch_offset+(lsa_ch_offset>>3);
       }
       /** for try 0 and 1 keep the symb rate lsa estimation*/
       /** for try 2 and 3 take the symb rate lsa estimation + 25% */
       if (nb_try_lock>1) {
               lsa_symb_rate_i=lsa_symb_rate+(lsa_symb_rate>>2);
       }
       trylock_offset             = lsa_ch_offset_i*inter_carrier_space;
       trylock_center_with_offset = trylock_center + trylock_offset;
       trylock_symbol_rate        = lsa_symb_rate_i*inter_carrier_space;

       Si2168B_FORCED_TRACE("nb_try_lock %d: RF %7.3f / SR %10.3f ( limits %7.3f / %7.3f )\n"
              , nb_try_lock
              , trylock_center_with_offset/1000000.0
              , trylock_symbol_rate/1000000.0
              , (trylock_center_with_offset - (trylock_symbol_rate/(2/1.0)))/1000000.0
              , (trylock_center_with_offset + (trylock_symbol_rate/(2/1.0)))/1000000.0
              );
    }
#endif /* 0 */
    Si2168B_FORCED_TRACE("\n");
  #ifdef    PLOT_ON_LIMITED_RANGE
            }
          }
  #endif /* PLOT_ON_LIMITED_RANGE */
  }


  w = 0;
  SiTraceConfiguration("traces suspend");
  if (strcmp(source,"spectrum")==0) {
    /* read spectrum data */
    Si2168B_WRITE (front_end->demod, dcom_control_byte, (0x80000000 | (nb_word - 1)) );
    Si2168B_WRITE (front_end->demod, dcom_addr        ,  gp_reg32_0);
    while ( w < nb_word ) {
        SiTraceConfiguration("traces suspend");
        Si2168B_READ (front_end->demod , dcom_read);
        Si2168B_WRITE(front_end->demod , dcom_read_toggle, 1);
        read_data = dcom_read;
        msb = (read_data >> 16) & 0xffff;
        lsb = (read_data >>  0) & 0xffff;
        if (signed_val) {
          if (msb >> 15) { msb = msb - (1<<16); }
          if (lsb >> 15) { lsb = lsb - (1<<16); }
        }
        if ((w < nb_word-0) && (w > nb_skip_first_words)) {
          /* Console spectrum mode */
  #ifdef    PLOT_ON_LIMITED_RANGE
          if (currentRF >= LIMITED_RANGE_MIN_RF) {
            if (currentRF <= LIMITED_RANGE_MAX_RF) {
  #endif /* PLOT_ON_LIMITED_RANGE */
            if ((count%5) ==0 ) {
              blanks = (int)((msb/standard_specific_spectrum_scaling) -standard_specific_spectrum_offset);
              Si2168B_FORCED_TRACE("%10.3f %8d (%3d) [%4d] |%*s\n", currentRF/1000000.0, (int)(msb), w, blanks, blanks,"*");
/*              if ((currentRF > tune_freq           - 1000000) && (currentRF < tune_freq           + 1000000) ) {Si2168B_FORCED_TRACE(" ====== T %4d, correction %6d \n", front_end->unicable->T, front_end->unicable->correction); }*/
/*              if ((currentRF > analyzis_start_freq - 1000000) && (currentRF < analyzis_start_freq + 1000000) ) {Si2168B_FORCED_TRACE(" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");}*/
/*              if ((currentRF > analyzis_stop_freq  - 1000000) && (currentRF < analyzis_stop_freq  + 1000000) ) {Si2168B_FORCED_TRACE(" <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");}*/
/*              if ((currentRF > 1610000000          - 1000000) && (currentRF < 1610000000          + 1000000) ) {Si2168B_FORCED_TRACE(" --------- 1610 peak ------------ %d %d\n", currentRF + front_end->unicable->correction*1000, currentRF - front_end->unicable->correction*1000 );}*/
            }
  #ifdef    PLOT_ON_LIMITED_RANGE
            }
          }
  #endif /* PLOT_ON_LIMITED_RANGE */
/*          if (0 ==0 ) {printf("%10ld %8d |%*s\n", currentRF, (int)(msb), (int)((msb-50000)/100),"*"); }*/
          currentRF = currentRF + inter_carrier_space;
          currentRF = currentRF + inter_carrier_space;
          count++;
        }
        w++;
    }
  }
  if (strcmp(source,"spectrum")==0) {
  #ifdef    PLOT_ON_LIMITED_RANGE
    if (spectrum_stop_freq >= LIMITED_RANGE_MIN_RF) {
      if (spectrum_start_freq <= LIMITED_RANGE_MAX_RF) {
  #endif /* PLOT_ON_LIMITED_RANGE */
    Si2168B_FORCED_TRACE("}\n");
  #ifdef    PLOT_ON_LIMITED_RANGE
  }
    }
  #endif /* PLOT_ON_LIMITED_RANGE */
  }
  SiTraceConfiguration("traces suspend");
  /* for (i=0; i<12; i++) { Si2168B_READ(front_end->demod, dcom_read);}*/ /* To (possibly) compensate a dcom issue seen with FPGA boards */

  if (strcmp(source,"spectrum")==0) {
  /*#define   PLOT_WITH_BACKOFF_INFO */ /* DO NOT USE !!! */
  #ifdef    PLOT_WITH_BACKOFF_INFO
    Si2168B_FORCED_TRACE("rf_backoff 0: %10d %7.3f (center %7.6f)\n", gp_reg16_1, gp_reg16_1*inter_carrier_space/1000000.0, gp_reg32_2/1000000.0);
    Si2168B_FORCED_TRACE("rf_backoff 1: %10d %7.3f (center %7.6f)\n", gp_reg16_2, gp_reg16_2*inter_carrier_space/1000000.0, gp_reg32_2/1000000.0);
    Si2168B_FORCED_TRACE("rf_backoff 2: %10d %7.3f (center %7.6f)\n", gp_reg16_3, gp_reg16_3*inter_carrier_space/1000000.0, gp_reg32_2/1000000.0);
    Si2168B_FORCED_TRACE("backoff limited to %3d%% (%7.3f MHz) BW %7.3f MHz\n", gp_reg8_3*100/255, (gp_reg8_3*1.0/255)*(front_end->demod->prop->scan_sat_unicable_bw.scan_sat_unicable_bw/10.0), front_end->demod->prop->scan_sat_unicable_bw.scan_sat_unicable_bw/10.0);
    /* read spectrum additional data tables */
    for (i=0; i< 3; i++) {
      Si2168B_WRITE (front_end->demod, dcom_control_byte, (0x80000000 | (48 - 1)) );
      Si2168B_WRITE (front_end->demod, dcom_addr        ,  gp_reg32_0 + i*48*4);
      nb_channel = (gp_reg32_1 >> (8*i)) & 0xFF;
      Si2168B_FORCED_TRACE("run %d, %3d channels\n", i, nb_channel);
      w = 0;
      while ( w < nb_channel ) {
        Si2168B_READ (front_end->demod , dcom_read);
        Si2168B_FORCED_TRACE("channel %d, %2d : 0x%08x tune_freq %8d ", i, w, dcom_read, dcom_read);

        Si2168B_READ (front_end->demod , dcom_read);
        read_data = dcom_read;
        msb = (read_data >> 16) & 0xffff;
        lsb = (read_data >>  0) & 0xffff;
        Si2168B_FORCED_TRACE(" 0x%08x offset %8d sr %8d ", dcom_read, msb, lsb);

        Si2168B_READ (front_end->demod , dcom_read);
        Si2168B_FORCED_TRACE(" 0x%08x variance %8d \n", dcom_read, dcom_read);

        w++;
      }
      Si2168B_FORCED_TRACE("\n");
    }
  #endif /* PLOT_WITH_BACKOFF_INFO */
  }

  SiTraceConfiguration("traces suspend");
  /* Start Si2168B DSP */
  Si2168B_WRITE (front_end->demod, dcom_control_byte, 0xc0000000);
  Si2168B_WRITE (front_end->demod, dcom_addr,         0x90000000);
  Si2168B_WRITE (front_end->demod, dcom_data,         0x00000000);
  #ifdef    PAUSE_FREQ
  if (strcmp(source,"spectrum")==0) {
    if (tune_freq >= PAUSE_FREQ - 50000000) {
      if (tune_freq <= PAUSE_FREQ + 50000000) {
        Si2168B_FORCED_TRACE ("please check the spectrum on the spectrum analyser (tuner_freq %8d) and press a key when done...\n", tune_freq);
        system("pause");
      }
    }
  }
  #endif /* PAUSE_FREQ */
  SiTraceConfiguration("traces resume");
}
#endif /* ALLOW_Si2168B_BLINDSCAN_DEBUG */
/************************************************************************************************************************
  NAME: Si2168B_Configure
  DESCRIPTION: Setup TER and SAT AGCs, Common Properties startup
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_Configure           (L1_Si2168B_Context *api)
{
    int return_code;
    return_code = NO_Si2168B_ERROR;

    SiTRACE("media %d\n",api->media);

    /* AGC settings when not used */
    if        ( api->rsp->get_rev.mcm_die == Si2168B_GET_REV_RESPONSE_MCM_DIE_DIE_A) {
      api->cmd->dd_mp_defaults.mp_a_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_DISABLE;
      api->cmd->dd_mp_defaults.mp_b_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_NO_CHANGE;
      api->cmd->dd_mp_defaults.mp_c_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_DISABLE;
      api->cmd->dd_mp_defaults.mp_d_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_NO_CHANGE;
    } else if ( api->rsp->get_rev.mcm_die == Si2168B_GET_REV_RESPONSE_MCM_DIE_DIE_B) {
      api->cmd->dd_mp_defaults.mp_a_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_NO_CHANGE;
      api->cmd->dd_mp_defaults.mp_b_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_DISABLE;
      api->cmd->dd_mp_defaults.mp_c_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_NO_CHANGE;
      api->cmd->dd_mp_defaults.mp_d_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_DISABLE;
    } else {
      api->cmd->dd_mp_defaults.mp_a_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_DISABLE;
      api->cmd->dd_mp_defaults.mp_b_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_DISABLE;
      api->cmd->dd_mp_defaults.mp_c_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_DISABLE;
      api->cmd->dd_mp_defaults.mp_d_mode   = Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_DISABLE;
    }
    /*  For DVB_T2, if the TER tuner has a FEF freeze input pin, drive this pin to 0 or 1 when NOT in T2 */
    /* if FEF is active high, set the pin to 0 when NOT in T2 */
    /* if FEF is active low,  set the pin to 1 when NOT in T2 */
    if (api->fef_mode == Si2168B_FEF_MODE_FREEZE_PIN) {
      switch (api->fef_pin) {
        case Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_A: {
          api->cmd->dvbt2_fef.fef_tuner_flag      = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_A;
          if (api->fef_level == 1) { api->cmd->dd_mp_defaults.mp_a_mode = Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_DRIVE_0; }
          else                     { api->cmd->dd_mp_defaults.mp_a_mode = Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_DRIVE_1; }
          break;
        }
        case Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_B: {
          api->cmd->dvbt2_fef.fef_tuner_flag      = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_B;
          if (api->fef_level == 1) { api->cmd->dd_mp_defaults.mp_b_mode = Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_DRIVE_0; }
          else                     { api->cmd->dd_mp_defaults.mp_b_mode = Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_DRIVE_1; }
          break;
        }
        case Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_C: {
          api->cmd->dvbt2_fef.fef_tuner_flag      = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_C;
          if (api->fef_level == 1) { api->cmd->dd_mp_defaults.mp_c_mode = Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_DRIVE_0; }
          else                     { api->cmd->dd_mp_defaults.mp_c_mode = Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_DRIVE_1; }
          break;
        }
        case Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_D: {
          api->cmd->dvbt2_fef.fef_tuner_flag      = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_D;
          if (api->fef_level == 1) { api->cmd->dd_mp_defaults.mp_d_mode = Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_DRIVE_0; }
          else                     { api->cmd->dd_mp_defaults.mp_d_mode = Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_DRIVE_1; }
          break;
        }
        default: break;
      }
    }
    Si2168B_L1_SendCommand2(api, Si2168B_DVBT2_FEF_CMD_CODE);
    Si2168B_L1_SendCommand2(api, Si2168B_DD_MP_DEFAULTS_CMD_CODE);

    if (api->media == Si2168B_TERRESTRIAL) {
      /* TER AGC pins and inversion are previously selected using Si2168B_L2_TER_AGC */
      api->cmd->dd_ext_agc_ter.agc_1_kloop = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_MIN;
      api->cmd->dd_ext_agc_ter.agc_1_min   = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MIN_MIN;

      api->cmd->dd_ext_agc_ter.agc_2_kloop = 18;
      api->cmd->dd_ext_agc_ter.agc_2_min   = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MIN_MIN;
      Si2168B_L1_SendCommand2(api, Si2168B_DD_EXT_AGC_TER_CMD_CODE);
    }


    /* LEDS MANAGEMENT */
    /* set hardware lock on LED */
    if        ( api->rsp->get_rev.mcm_die == Si2168B_GET_REV_RESPONSE_MCM_DIE_DIE_A) {
      api->cmd->config_pins.gpio0_mode     = Si2168B_CONFIG_PINS_CMD_GPIO0_MODE_NO_CHANGE;
      api->cmd->config_pins.gpio0_read     = Si2168B_CONFIG_PINS_CMD_GPIO0_READ_DO_NOT_READ;
      api->cmd->config_pins.gpio1_mode     = Si2168B_CONFIG_PINS_CMD_GPIO1_MODE_HW_LOCK;
      api->cmd->config_pins.gpio1_read     = Si2168B_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ;
    } else if ( api->rsp->get_rev.mcm_die == Si2168B_GET_REV_RESPONSE_MCM_DIE_DIE_B) {
      api->cmd->config_pins.gpio0_mode     = Si2168B_CONFIG_PINS_CMD_GPIO0_MODE_HW_LOCK;
      api->cmd->config_pins.gpio0_read     = Si2168B_CONFIG_PINS_CMD_GPIO0_READ_DO_NOT_READ;
      api->cmd->config_pins.gpio1_mode     = Si2168B_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE;
      api->cmd->config_pins.gpio1_read     = Si2168B_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ;
    } else {
      api->cmd->config_pins.gpio0_mode     = Si2168B_CONFIG_PINS_CMD_GPIO0_MODE_HW_LOCK;
      api->cmd->config_pins.gpio0_read     = Si2168B_CONFIG_PINS_CMD_GPIO0_READ_DO_NOT_READ;
      api->cmd->config_pins.gpio1_mode     = Si2168B_CONFIG_PINS_CMD_GPIO1_MODE_TS_ERR;
      api->cmd->config_pins.gpio1_read     = Si2168B_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ;
    }
    Si2168B_L1_SendCommand2(api, Si2168B_CONFIG_PINS_CMD_CODE);

    /* Edit the procedure below if you have any properties settings different from the standard defaults */
    Si2168B_storeUserProperties     (api->prop);
    /* Download properties different from 'default' */
    Si2168B_downloadAllProperties(api);

//Edit by ITE
	api->prop->dd_ts_mode.mode = Si2168B_DD_TS_MODE_PROP_MODE_SERIAL;		
	Si2168B_L1_SetProperty2(api, Si2168B_DD_TS_MODE_PROP_CODE);

#ifdef    USB_Capability
    if        ( api->rsp->get_rev.mcm_die == Si2168B_GET_REV_RESPONSE_MCM_DIE_SINGLE) {
      /* Setting GPIF clock to not_inverted to allow TS over USB transfer */
      Si2168B_L1_DD_SET_REG  (api, 0 , 35, 1, 0);
    }
#endif /* USB_Capability */
#ifdef    Si2167B_20_COMPATIBLE
    /* This needs to be done for Si2167B (part 67, ROM 0) with the following FW: */
    /*  FW 2_0b23 and above                                     */
    /*  FW 2_2b2  and above                                     */
    if (  (api->rsp->part_info.part     == 67 )
        & (api->rsp->part_info.romid    ==  0 ) ) {
      if ( (
            (api->rsp->get_rev.cmpmajor == '2')
          & (api->rsp->get_rev.cmpminor == '0')
          & (api->rsp->get_rev.cmpbuild >= 23)
         ) | (
            (api->rsp->get_rev.cmpmajor == '2')
          & (api->rsp->get_rev.cmpminor == '2')
          & (api->rsp->get_rev.cmpbuild >=  2)
         ) ) {
		    api->cmd->dd_set_reg.reg_code_lsb = 132;
		    api->cmd->dd_set_reg.reg_code_mid = 50;
		    api->cmd->dd_set_reg.reg_code_msb = 10;
		    api->cmd->dd_set_reg.value        = 1;
		     /* 0x8e for DVB-C with Si2167B 2_0b23: fw 2.0b23 is able to behave as 2.0b18 for 32QAM lock time with this code. */
        SiTRACE("[SiTRACE]Si2168B_Configure: DD_SET_REG  (api, 132, 50, 10, 1) for 32QAM lock time with Si2167B\n");
        Si2168B_L1_SendCommand2 (api, Si2168B_DD_SET_REG_CMD_CODE);
      }
    }
#endif /* Si2167B_20_COMPATIBLE */

    return return_code;
}
/************************************************************************************************************************
  NAME: Si2168B_SILENT
  DESCRIPTION: Turning Demod pins tristate, to allow using another demod
  Parameter:  Pointer to Si2168B Context
  Parameter:  silent, a flag used to select the result: '1' -> go silent, '0' -> return to active mode
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_SILENT              (Si2168B_L2_Context *front_end, int silent)
{
  int mp_a_odrv;
  int start_time_ms;
  SiTRACE (" switch Si2168B_SILENT: silent %d\n", silent);
  if (silent) {
    /* turn all possible I/Os to tristate, to allow using another demod */
    Si2168B_L1_DD_MP_DEFAULTS(front_end->demod, Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_DISABLE, Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_DISABLE
                                             , Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_DISABLE, Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_DISABLE);
    Si2168B_L1_DD_EXT_AGC_TER(front_end->demod, Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_NOT_USED,     front_end->demod->cmd->dd_ext_agc_ter.agc_1_inv
                                             , Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_NOT_USED,     front_end->demod->cmd->dd_ext_agc_ter.agc_2_inv
                                             , front_end->demod->cmd->dd_ext_agc_ter.agc_1_kloop, front_end->demod->cmd->dd_ext_agc_ter.agc_2_kloop
                                             , front_end->demod->cmd->dd_ext_agc_ter.agc_1_min  , front_end->demod->cmd->dd_ext_agc_ter.agc_2_min);
    front_end->TER_init_done = 0;
    Si2168B_L1_CONFIG_PINS   (front_end->demod, Si2168B_CONFIG_PINS_CMD_GPIO0_MODE_DISABLE, Si2168B_CONFIG_PINS_CMD_GPIO0_READ_DO_NOT_READ
                                             , Si2168B_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE, Si2168B_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ);
    /* turn TS pins to tristate */
    front_end->demod->prop->dd_ts_mode.mode =  Si2168B_DD_TS_MODE_PROP_MODE_TRISTATE;
    Si2168B_L1_SetProperty2  (front_end->demod, Si2168B_DD_TS_MODE_PROP_CODE);
    Si2168B_L1_DD_RESTART    (front_end->demod);
    system_wait(8); /* Wait at least 8 ms before issuing POWER_DOWN, to make sure that the AGCs are in tristate. */
    start_time_ms = system_time();
    while (system_time() - start_time_ms < 1000 ) {
      if (Si2168B_L1_DD_GET_REG  (front_end->demod, 4, 104, 1) != 0) {
      } else {
        mp_a_odrv = ( front_end->demod->rsp->dd_get_reg.data4<<24)
                    +(front_end->demod->rsp->dd_get_reg.data3<<16)
                    +(front_end->demod->rsp->dd_get_reg.data2<< 8)
                    +(front_end->demod->rsp->dd_get_reg.data1<< 0);
        if (mp_a_odrv != 0) {
          SiERROR("wrong mp_a_odrv value after TRISTATING & DD_RESTART\n");
        } else {
          break;
        }
      }
    }
  } else {
    /* return to 'active mode' (it will be necessary to turn TS on again after locking) */
    Si2168B_WAKEUP        (front_end->demod);
  }
  return silent;
}
/************************************************************************************************************************
  NAME: Si2168B_STANDBY
  DESCRIPTION:
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_STANDBY             (L1_Si2168B_Context *api)
{
    return Si2168B_L1_POWER_DOWN (api);
}
/************************************************************************************************************************
  NAME: Si2168B_WAKEUP
  DESCRIPTION:
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_WAKEUP              (L1_Si2168B_Context *api)
{
    int return_code;
    int media;

    return_code = NO_Si2168B_ERROR;
    media       = Si2168B_Media(api, api->standard);
    SiTRACE ("Si2168B_WAKEUP: media %d\n", media);

    /* Clock source selection */
    switch (media) {
      default                                  :
      case Si2168B_TERRESTRIAL : {
        api->cmd->start_clk.clk_mode = api->tuner_ter_clock_input;
        break;
      }
    }
    if (api->cmd->start_clk.clk_mode == Si2168B_START_CLK_CMD_CLK_MODE_XTAL) {
      api->cmd->start_clk.tune_cap = Si2168B_START_CLK_CMD_TUNE_CAP_15P6;
    } else {
      api->cmd->start_clk.tune_cap = Si2168B_START_CLK_CMD_TUNE_CAP_EXT_CLK;
    }
    Si2168B_L1_START_CLK (api,
                            Si2168B_START_CLK_CMD_SUBCODE_CODE,
                            Si2168B_START_CLK_CMD_RESERVED1_RESERVED,
                            api->cmd->start_clk.tune_cap,
                            Si2168B_START_CLK_CMD_RESERVED2_RESERVED,
                            api->cmd->start_clk.clk_mode,
                            Si2168B_START_CLK_CMD_RESERVED3_RESERVED,
                            Si2168B_START_CLK_CMD_RESERVED4_RESERVED,
                            Si2168B_START_CLK_CMD_START_CLK_START_CLK);
    /* Reference frequency selection */
    switch (media) {
      default                 :
      case Si2168B_TERRESTRIAL : {
        if (api->tuner_ter_clock_freq == 16) {
          SiTRACE("Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_16MHZ\n");
          api->cmd->power_up.clock_freq = Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_16MHZ;
        } else if (api->tuner_ter_clock_freq == 24) {
          SiTRACE("Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ\n");
          api->cmd->power_up.clock_freq = Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ;
        } else {
          SiTRACE("Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_27MHZ\n");
          api->cmd->power_up.clock_freq = Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_27MHZ;
        }
        break;
      }
    }

    return_code = Si2168B_L1_POWER_UP (api,
                            Si2168B_POWER_UP_CMD_SUBCODE_CODE,
                            api->cmd->power_up.reset,
                            Si2168B_POWER_UP_CMD_RESERVED2_RESERVED,
                            Si2168B_POWER_UP_CMD_RESERVED4_RESERVED,
                            Si2168B_POWER_UP_CMD_RESERVED1_RESERVED,
                            Si2168B_POWER_UP_CMD_ADDR_MODE_CURRENT,
                            Si2168B_POWER_UP_CMD_RESERVED5_RESERVED,
                            api->cmd->power_up.func,
                            api->cmd->power_up.clock_freq,
                            Si2168B_POWER_UP_CMD_CTSIEN_DISABLE,
                            Si2168B_POWER_UP_CMD_WAKE_UP_WAKE_UP);

         if (api->cmd->start_clk.clk_mode == Si2168B_START_CLK_CMD_CLK_MODE_CLK_CLKIO  ) { SiTRACE ("Si2168B_START_CLK_CMD_CLK_MODE_CLK_CLKIO\n"  );}
    else if (api->cmd->start_clk.clk_mode == Si2168B_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN) { SiTRACE ("Si2168B_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN\n");}
    else if (api->cmd->start_clk.clk_mode == Si2168B_START_CLK_CMD_CLK_MODE_XTAL       ) { SiTRACE ("Si2168B_START_CLK_CMD_CLK_MODE_XTAL\n"       );}

         if (api->cmd->power_up.reset == Si2168B_POWER_UP_CMD_RESET_RESET  ) { SiTRACE ("Si2168B_POWER_UP_CMD_RESET_RESET\n"  );}
    else if (api->cmd->power_up.reset == Si2168B_POWER_UP_CMD_RESET_RESUME ) { SiTRACE ("Si2168B_POWER_UP_CMD_RESET_RESUME\n");}

    if (return_code != NO_Si2168B_ERROR ) {
      SiTRACE("Si2168B_WAKEUP: POWER_UP ERROR!\n");
      SiERROR("Si2168B_WAKEUP: POWER_UP ERROR!\n");
      return return_code;
    }
    /* After a successful POWER_UP, set values for 'resume' only */
    api->cmd->power_up.reset = Si2168B_POWER_UP_CMD_RESET_RESUME;
    api->cmd->power_up.func  = Si2168B_POWER_UP_CMD_FUNC_NORMAL;

    return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168B_PowerUpWithPatch
  DESCRIPTION: Send Si2168B API PowerUp Command with PowerUp to bootloader,
  Check the Chip rev and part, and ROMID are compared to expected values.
  Load the Firmware Patch then Start the Firmware.
  Programming Guide Reference:    Flowchart A.2 (POWER_UP with patch flowchart)

  Parameter:  pointer to Si2168B Context
  Returns:    Si2168B/I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_PowerUpWithPatch    (L1_Si2168B_Context *api)
{
    int return_code;
    int fw_loaded;
    return_code = NO_Si2168B_ERROR;
    fw_loaded   = 0;

    /* Before patching, set POWER_UP values for 'RESET' and 'BOOTLOADER' */
    api->cmd->power_up.reset = Si2168B_POWER_UP_CMD_RESET_RESET;
    api->cmd->power_up.func  = Si2168B_POWER_UP_CMD_FUNC_BOOTLOADER;

    return_code = Si2168B_WAKEUP(api);

    if (return_code != NO_Si2168B_ERROR) {
      SiERROR("Si2168B_PowerUpWithPatch: WAKEUP error!\n");
      return return_code;
    }

    /* Get the Part Info from the chip.   This command is only valid in Bootloader mode */
    if ((return_code = Si2168B_L1_PART_INFO(api)) != NO_Si2168B_ERROR) {
      SiTRACE ("Si2168B_L1_PART_INFO error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
      return return_code;
    }
    SiTRACE("part    Si21%02d",   api->rsp->part_info.part   );
           if (api->rsp->part_info.chiprev == Si2168B_PART_INFO_RESPONSE_CHIPREV_A) {
    SiTRACE("A\n");
    } else if (api->rsp->part_info.chiprev == Si2168B_PART_INFO_RESPONSE_CHIPREV_B) {
    SiTRACE("B\n");
    } else {
    SiTRACE("\nchiprev %d\n",        api->rsp->part_info.chiprev);
    }
    SiTRACE("romid   %d\n",        api->rsp->part_info.romid  );
    SiTRACE("pmajor  0x%02x\n",    api->rsp->part_info.pmajor );
    SiTRACE("pminor  0x%02x\n",    api->rsp->part_info.pminor );
    SiTRACE("pbuild  %d\n",        api->rsp->part_info.pbuild );
    if ((api->rsp->part_info.pmajor >= 0x30) & (api->rsp->part_info.pminor >= 0x30)) {
    SiTRACE("Full Info       'Si21%02d-%c%c%c ROM%x NVM%c_%cb%d'\n\n\n", api->rsp->part_info.part, api->rsp->part_info.chiprev + 0x40, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.romid, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
    }


    /* Check part info values and load the proper firmware */
#ifdef    Si2183_ES_COMPATIBLE
    #ifdef    Si2183_FIRMWARE_ES_FW_LINES
    if (!fw_loaded) {
      SiTRACE  ("Si2183_FIRMWARE_ES_FW: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2183_FIRMWARE_ES_FW_PART, Si2183_FIRMWARE_ES_FW_ROM, Si2183_FIRMWARE_ES_FW_PMAJOR, Si2183_FIRMWARE_ES_FW_PMINOR, Si2183_FIRMWARE_ES_FW_PBUILD );
      if  (((api->rsp->part_info.romid  == Si2183_FIRMWARE_ES_FW_ROM   )
        &(
            ((api->rsp->part_info.part == 83 ) & (api->rsp->part_info.chiprev + 0x40 == 'A') )
         || ((api->rsp->part_info.part == 82 ) & (api->rsp->part_info.chiprev + 0x40 == 'A') )
         || ((api->rsp->part_info.part == 81 ) & (api->rsp->part_info.chiprev + 0x40 == 'A') )
         || ((api->rsp->part_info.part == 80 ) & (api->rsp->part_info.chiprev + 0x40 == 'A') )
         || ((api->rsp->part_info.part == 69 ) & (api->rsp->part_info.chiprev + 0x40 == 'C') )
         || ((api->rsp->part_info.part == 68 ) & (api->rsp->part_info.chiprev + 0x40 == 'C') )
         || ((api->rsp->part_info.part == 64 ) & (api->rsp->part_info.chiprev + 0x40 == 'B') )
         || ((api->rsp->part_info.part == 62 ) & (api->rsp->part_info.chiprev + 0x40 == 'B') )
         || ((api->rsp->part_info.part == 60 ) & (api->rsp->part_info.chiprev + 0x40 == 'B') )
        ) )
        /*  One line for compatibility with early Si2183 and derivatives samples with NO NVM content */
        || ((api->rsp->part_info.romid  == 0   ) & ( api->rsp->part_info.part == 0  ) )
        ) {
        SiTRACE("Updating FW for 'Si21%2d NVM%c_%cb%d' (full download)\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        #ifdef    Si2183_FIRMWARE_ES_FW_INFOS
          SiTRACE(Si2183_FIRMWARE_ES_FW_INFOS);
          SiERROR(Si2183_FIRMWARE_ES_FW_INFOS);
        #endif /* Si2183_FIRMWARE_ES_FW_INFOS */
        if ((return_code = Si2168B_LoadFirmware(api, Si2183_FIRMWARE_ES_FW, Si2183_FIRMWARE_ES_FW_LINES)) != NO_Si2168B_ERROR) {
           SiTRACE ("Si2168B_LoadFirmware error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
           return return_code;
         }
        fw_loaded++;
      }
    }
    #endif /* Si2183_FIRMWARE_ES_FW_LINES */
#endif /* Si2183_ES_COMPATIBLE */
#ifdef    Si2168B_40_COMPATIBLE
    #ifdef    Si2168B_SPI_4_4b17_LINES
    if ( (api->spi_download) & (!fw_loaded) ) {
      SiTRACE  ("Si2168B_SPI_4_4b17_LINES: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168B_SPI_4_4b17_PART, Si2168B_SPI_4_4b17_ROM, Si2168B_SPI_4_4b17_PMAJOR, Si2168B_SPI_4_4b17_PMINOR, Si2168B_SPI_4_4b17_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168B_SPI_4_4b17_ROM   )
        &(  (api->rsp->part_info.part == 64 )
         || (api->rsp->part_info.part == 62 )
         || (api->rsp->part_info.part == 60 )
        )
        & (api->rsp->part_info.pmajor == Si2168B_SPI_4_4b17_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168B_SPI_4_4b17_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168B_SPI_4_4b17_PBUILD)
          ) {
        SiTRACE("Updating FW for 'Si21%2d NVM%c_%cb%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2168B_LoadFirmwareSPI(api, Si2168B_SPI_4_4b17, Si2168B_SPI_4_4b17_LINES, Si2168B_SPI_4_4b17_KEY, Si2168B_SPI_4_4b17_NUM )) != NO_Si2168B_ERROR) {
          SiTRACE ("Si2168B_LoadFirmwareSPI  error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        #ifdef    Si2168B_SPI_4_4b17_INFOS
          SiTRACE(Si2168B_SPI_4_4b17_INFOS);
          SiERROR(Si2168B_SPI_4_4b17_INFOS);
        #endif /* Si2168B_SPI_4_4b17_INFOS */
        fw_loaded++;
      }
    }
    #endif /* Si2168B_SPI_4_4b17_LINES */
    #ifdef    Si2168B_PATCH16_4_4b17_LINES
    if (!fw_loaded) {
      SiTRACE  ("Si2168B_PATCH16_4_4b17: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168B_PATCH16_4_4b17_PART, Si2168B_PATCH16_4_4b17_ROM, Si2168B_PATCH16_4_4b17_PMAJOR, Si2168B_PATCH16_4_4b17_PMINOR, Si2168B_PATCH16_4_4b17_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168B_PATCH16_4_4b17_ROM   )
        &(  (api->rsp->part_info.part == 64 )
         || (api->rsp->part_info.part == 62 )
         || (api->rsp->part_info.part == 60 )
        )
        & (api->rsp->part_info.pmajor == Si2168B_PATCH16_4_4b17_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168B_PATCH16_4_4b17_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168B_PATCH16_4_4b17_PBUILD)
          ) {
        SiTRACE("Updating FW for 'Si21%2d NVM%c_%cb%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        #ifdef    Si2168B_PATCH16_4_4b17_INFOS
          SiTRACE(Si2168B_PATCH16_4_4b17_INFOS);
          SiERROR(Si2168B_PATCH16_4_4b17_INFOS);
        #endif /* Si2168B_PATCH16_4_4b17_INFOS */
        if ((return_code = Si2168B_LoadFirmware_16(api, Si2168B_PATCH16_4_4b17, Si2168B_PATCH16_4_4b17_LINES)) != NO_Si2168B_ERROR) {
          SiTRACE ("Si2168B_LoadFirmware_16 error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        fw_loaded++;
      }
    }
    #endif /* Si2168B_PATCH16_4_4b17_LINES */
    #ifdef    Si2168B_SPI_4_0b19_LINES
    if ( (api->spi_download) & (!fw_loaded) ) {
      SiTRACE  ("Si2168B_SPI_4_0b19_LINES: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168B_SPI_4_0b19_PART, Si2168B_SPI_4_0b19_ROM, Si2168B_SPI_4_0b19_PMAJOR, Si2168B_SPI_4_0b19_PMINOR, Si2168B_SPI_4_0b19_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168B_SPI_4_0b19_ROM   )
        &(  (api->rsp->part_info.part == 64 )
         || (api->rsp->part_info.part == 62 )
         || (api->rsp->part_info.part == 60 )
        )
        & (api->rsp->part_info.pmajor == Si2168B_SPI_4_0b19_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168B_SPI_4_0b19_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168B_SPI_4_0b19_PBUILD)
          ) {
        SiTRACE("Updating FW for 'Si21%2d NVM%c_%cb%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2168B_LoadFirmwareSPI(api, Si2168B_SPI_4_0b19, Si2168B_SPI_4_0b19_LINES, Si2168B_SPI_4_0b19_KEY, Si2168B_SPI_4_0b19_NUM )) != NO_Si2168B_ERROR) {
          SiTRACE ("Si2168B_LoadFirmwareSPI  error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        #ifdef    Si2168B_SPI_4_0b19_INFOS
          SiTRACE(Si2168B_SPI_4_0b19_INFOS);
          SiERROR(Si2168B_SPI_4_0b19_INFOS);
        #endif /* Si2168B_SPI_4_0b19_INFOS */
        fw_loaded++;
      }
    }
    #endif /* Si2168B_SPI_4_0b19_LINES */
    #ifdef    Si2168B_PATCH16_4_0b19_LINES
    if (!fw_loaded) {
      SiTRACE  ("Si2168B_PATCH16_4_0b19: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168B_PATCH16_4_0b19_PART, Si2168B_PATCH16_4_0b19_ROM, Si2168B_PATCH16_4_0b19_PMAJOR, Si2168B_PATCH16_4_0b19_PMINOR, Si2168B_PATCH16_4_0b19_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168B_PATCH16_4_0b19_ROM   )
        &(  (api->rsp->part_info.part == 69 )
         || (api->rsp->part_info.part == 68 )
        )
        & (api->rsp->part_info.pmajor == Si2168B_PATCH16_4_0b19_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168B_PATCH16_4_0b19_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168B_PATCH16_4_0b19_PBUILD)
          ) {
        SiTRACE("Updating FW for 'Si21%2d NVM%c_%cb%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        #ifdef    Si2168B_PATCH16_4_0b19_INFOS
          SiTRACE(Si2168B_PATCH16_4_0b19_INFOS);
          SiERROR(Si2168B_PATCH16_4_0b19_INFOS);
        #endif /* Si2168B_PATCH16_4_0b19_INFOS */
        if ((return_code = Si2168B_LoadFirmware_16(api, Si2168B_PATCH16_4_0b19, Si2168B_PATCH16_4_0b19_LINES)) != NO_Si2168B_ERROR) {
          SiTRACE ("Si2168B_LoadFirmware_16 error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        fw_loaded++;
      }
    }
    #endif /* Si2168B_PATCH16_4_0b19_LINES */
#endif /* Si2168B_40_COMPATIBLE */
#ifdef    Si2168B_ES_COMPATIBLE
    #ifdef    Si2168B_SPI_3_Ab14_LINES
    if ( (api->spi_download) & (!fw_loaded) ) {
      SiTRACE  ("Si2168B_SPI_3_Ab14_LINES: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168B_SPI_3_Ab14_PART, Si2168B_SPI_3_Ab14_ROM, Si2168B_SPI_3_Ab14_PMAJOR, Si2168B_SPI_3_Ab14_PMINOR, Si2168B_SPI_3_Ab14_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168B_SPI_3_Ab14_ROM   )
/*
        &(  (api->rsp->part_info.part == 64 )
         || (api->rsp->part_info.part == 62 )
         || (api->rsp->part_info.part == 60 )
        )
        & (api->rsp->part_info.pmajor == Si2168B_SPI_3_Ab14_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168B_SPI_3_Ab14_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168B_SPI_3_Ab14_PBUILD)
*/
          ) {
        SiTRACE("Updating FW for 'Si21%2d NVM%c_%cb%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        if ((return_code = Si2168B_LoadFirmwareSPI(api, Si2168B_SPI_3_Ab14, Si2168B_SPI_3_Ab14_LINES, Si2168B_SPI_3_Ab14_KEY, Si2168B_SPI_3_Ab14_NUM )) != NO_Si2168B_ERROR) {
          SiTRACE ("Si2168B_LoadFirmwareSPI  error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        #ifdef    Si2168B_SPI_3_Ab14_INFOS
          SiTRACE(Si2168B_SPI_3_Ab14_INFOS);
          SiERROR(Si2168B_SPI_3_Ab14_INFOS);
        #endif /* Si2168B_SPI_3_Ab14_INFOS */
        fw_loaded++;
      }
    }
    #endif /* Si2168B_SPI_3_Ab14_LINES */
    #ifdef    Si2168B_FIRMWARE_3_Ab14_LINES
    if (!fw_loaded) {
      SiTRACE  ("Si2168B_FIRMWARE_3_Ab14: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2168B_FIRMWARE_3_Ab14_PART, Si2168B_FIRMWARE_3_Ab14_ROM, Si2168B_FIRMWARE_3_Ab14_PMAJOR, Si2168B_FIRMWARE_3_Ab14_PMINOR, Si2168B_FIRMWARE_3_Ab14_PBUILD );
      if ((api->rsp->part_info.romid  == Si2168B_FIRMWARE_3_Ab14_ROM   )
        &(  (api->rsp->part_info.part == 69 )
         || (api->rsp->part_info.part == 68 )
         || (api->rsp->part_info.part == 64 )
         || (api->rsp->part_info.part == 62 )
         || (api->rsp->part_info.part == 60 )
         || (api->rsp->part_info.part == 0  )
        )
/*
        & (api->rsp->part_info.pmajor == Si2168B_FIRMWARE_3_Ab14_PMAJOR)
        & (api->rsp->part_info.pminor == Si2168B_FIRMWARE_3_Ab14_PMINOR)
        & (api->rsp->part_info.pbuild == Si2168B_FIRMWARE_3_Ab14_PBUILD)
*/
          ) {
        SiTRACE("Updating FW for 'Si21%2d NVM%c_%cb%d' (full download)\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        #ifdef    Si2168B_FIRMWARE_3_Ab14_INFOS
          SiTRACE(Si2168B_FIRMWARE_3_Ab14_INFOS);
          SiERROR(Si2168B_FIRMWARE_3_Ab14_INFOS);
        #endif /* Si2168B_FIRMWARE_3_Ab14_INFOS */
        if ((return_code = Si2168B_LoadFirmware(api, Si2168B_FIRMWARE_3_Ab14, Si2168B_FIRMWARE_3_Ab14_LINES)) != NO_Si2168B_ERROR) {
          SiTRACE ("Si2168B_LoadFirmware error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        fw_loaded++;
      }
    }
    #endif /* Si2168B_FIRMWARE_3_Ab14_LINES */
#endif /* Si2168B_ES_COMPATIBLE */
#ifdef    Si2169_30_COMPATIBLE
    #ifdef    Si2169_PATCH_3_0b18_LINES
    if (!fw_loaded) {
      SiTRACE  ("Si2169_PATCH_3_0b18: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2169_PATCH_3_0b18_PART, Si2169_PATCH_3_0b18_ROM, Si2169_PATCH_3_0b18_PMAJOR, Si2169_PATCH_3_0b18_PMINOR, Si2169_PATCH_3_0b18_PBUILD );
      if ((api->rsp->part_info.romid  == Si2169_PATCH_3_0b18_ROM   )
        &((api->rsp->part_info.part   == 69 ) || (api->rsp->part_info.part == 68 ))
        & (api->rsp->part_info.pmajor == Si2169_PATCH_3_0b18_PMAJOR)
        & (api->rsp->part_info.pminor == Si2169_PATCH_3_0b18_PMINOR)
        & (api->rsp->part_info.pbuild == Si2169_PATCH_3_0b18_PBUILD)) {
          SiTRACE("Updating FW for 'Si21%2d_ROM%x %c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.romid, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
          #ifdef    Si2169_PATCH_3_0b18_INFOS
          SiTRACE(Si2169_PATCH_3_0b18_INFOS);
          SiERROR(Si2169_PATCH_3_0b18_INFOS);
          #endif /* Si2169_PATCH_3_0b18_INFOS */
          if ((return_code = Si2168B_LoadFirmware(api, Si2169_PATCH_3_0b18, Si2169_PATCH_3_0b18_LINES)) != NO_Si2168B_ERROR) {
              SiTRACE ("Si2169_LoadFirmware error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
              return return_code;
          }
          fw_loaded++;
        }
    }
    #endif /* Si2169_PATCH_3_0b18_LINES */
#endif /* Si2169_30_COMPATIBLE */
#ifdef    Si2167B_20_COMPATIBLE
    #ifdef    Si2167B_PATCH_2_0b5_LINES
    if (!fw_loaded) {
      SiTRACE  ("Si2167B_PATCH_2_0b5: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2167B_PATCH_2_0b5_PART, Si2167B_PATCH_2_0b5_ROM, Si2167B_PATCH_2_0b5_PMAJOR, Si2167B_PATCH_2_0b5_PMINOR, Si2167B_PATCH_2_0b5_PBUILD );
      if ((api->rsp->part_info.romid  == Si2167B_PATCH_2_0b5_ROM   )
        &((api->rsp->part_info.part   == 67 ) || (api->rsp->part_info.part == 66 ))
        & (api->rsp->part_info.pmajor == Si2167B_PATCH_2_0b5_PMAJOR)
        & (api->rsp->part_info.pminor == Si2167B_PATCH_2_0b5_PMINOR)
        & (api->rsp->part_info.pbuild == Si2167B_PATCH_2_0b5_PBUILD)) {
        SiTRACE("Updating FW for 'Si21%2d_FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        #ifdef    Si2167B_PATCH_2_0b5_INFOS
        SiTRACE("%s\n", Si2167B_PATCH_2_0b5_INFOS);
        #endif /* Si2167B_PATCH_2_0b5_INFOS */
        if ((return_code = Si2167B_LoadFirmware(api, Si2167B_PATCH_2_0b5, Si2167B_PATCH_2_0b5_LINES)) != NO_Si2167B_ERROR) {
          SiTRACE ("Si2167B_LoadPatch error 0x%02x: %s\n", return_code, Si2167B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        fw_loaded++;
      }
    }
    #endif /* Si2167B_PATCH_2_0b5_LINES */
    #ifdef    Si2167B_20_PATCH_CUSTOMER_LINES
    if (!fw_loaded) {
      SiTRACE  ("Si2167B_20_PATCH_CUSTOMER: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2167B_20_PATCH_CUSTOMER_PART, Si2167B_20_PATCH_CUSTOMER_ROM, Si2167B_20_PATCH_CUSTOMER_PMAJOR, Si2167B_20_PATCH_CUSTOMER_PMINOR, Si2167B_20_PATCH_CUSTOMER_PBUILD );
      if ((api->rsp->part_info.romid  == Si2167B_20_PATCH_CUSTOMER_ROM   )
        &((api->rsp->part_info.part   == 67 ) || (api->rsp->part_info.part == 66 ))
        & (api->rsp->part_info.pmajor == Si2167B_20_PATCH_CUSTOMER_PMAJOR)
        & (api->rsp->part_info.pminor == Si2167B_20_PATCH_CUSTOMER_PMINOR)
        & (api->rsp->part_info.pbuild == Si2167B_20_PATCH_CUSTOMER_PBUILD)) {
          SiTRACE("Updating FW for 'Si21%2d_ROM%x %c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.romid, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
          #ifdef    Si2167B_20_PATCH_CUSTOMER_INFOS
          SiTRACE(Si2167B_20_PATCH_CUSTOMER_INFOS);
          SiERROR(Si2167B_20_PATCH_CUSTOMER_INFOS);
          #endif /* Si2167B_20_PATCH_CUSTOMER_INFOS */
          if ((return_code = Si2167B_LoadFirmware(api, Si2167B_20_PATCH_CUSTOMER, Si2167B_20_PATCH_CUSTOMER_LINES)) != NO_Si2167B_ERROR) {
            SiTRACE ("Si2167B_LoadFirmware error 0x%02x: %s\n", return_code, Si2167B_L1_API_ERROR_TEXT(return_code) );
            return return_code;
          }
          fw_loaded++;
        }
    }
    #endif /* Si2167B_20_PATCH_CUSTOMER_LINES */
    #ifdef    Si2167B_PATCH_2_0b22_LINES
    if (!fw_loaded) {
      SiTRACE  ("Si2167B_PATCH_2_0b22: Is this part a  'Si21%2d_ROM%x_%c_%c_b%d'?\n", Si2167B_PATCH_2_0b22_PART, Si2167B_PATCH_2_0b22_ROM, Si2167B_PATCH_2_0b22_PMAJOR, Si2167B_PATCH_2_0b22_PMINOR, Si2167B_PATCH_2_0b22_PBUILD );
      if ((api->rsp->part_info.romid  == Si2167B_PATCH_2_0b22_ROM   )
        &((api->rsp->part_info.part   == 67 ) || (api->rsp->part_info.part == 66 ))
        & (api->rsp->part_info.pmajor == Si2167B_PATCH_2_0b22_PMAJOR)
        & (api->rsp->part_info.pminor == Si2167B_PATCH_2_0b22_PMINOR)
        & (api->rsp->part_info.pbuild == Si2167B_PATCH_2_0b22_PBUILD)) {
        SiTRACE("Updating FW for 'Si21%2d_FW_%c_%c_b%d'\n", api->rsp->part_info.part, api->rsp->part_info.pmajor, api->rsp->part_info.pminor, api->rsp->part_info.pbuild );
        #ifdef    Si2167B_PATCH_2_0b22_INFOS
        SiTRACE("%s\n", Si2167B_PATCH_2_0b22_INFOS);
        #endif /* Si2167B_PATCH_2_0b22_INFOS */
        if ((return_code = Si2168B_LoadFirmware(api, Si2167B_PATCH_2_0b22, Si2167B_PATCH_2_0b22_LINES)) != NO_Si2168B_ERROR) {
          SiTRACE ("Si2167B_LoadPatch error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
        fw_loaded++;
      }
    }
    #endif /* Si2167B_PATCH_2_0b22_LINES */
#endif /* Si2167B_20_COMPATIBLE */

    if (!fw_loaded) {
      SiTRACE ("Si2168B_LoadFirmware error: NO Firmware Loaded! Possible part/code incompatibility !\n");
      SiERROR ("Si2168B_LoadFirmware error: NO Firmware Loaded! Possible part/code incompatibility !\n");
      return ERROR_Si2168B_LOADING_FIRMWARE;
    }

    /*Start the Firmware */
    if ((return_code = Si2168B_StartFirmware(api)) != NO_Si2168B_ERROR) {
       /* Start firmware */
        SiTRACE ("Si2168B_StartFirmware error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }

    Si2168B_L1_GET_REV (api);
    if ((api->rsp->get_rev.mcm_die) != Si2168B_GET_REV_RESPONSE_MCM_DIE_SINGLE) {
      SiTRACE("Si21%2d%d-%c%c%c Die %c Part running 'FW_%c_%cb%d'\n", api->rsp->part_info.part
                                                                    , 2
                                                                    , api->rsp->part_info.chiprev + 0x40
                                                                    , api->rsp->part_info.pmajor
                                                                    , api->rsp->part_info.pminor
                                                                    , api->rsp->get_rev.mcm_die   + 0x40
                                                                    , api->rsp->get_rev.cmpmajor
                                                                    , api->rsp->get_rev.cmpminor
                                                                    , api->rsp->get_rev.cmpbuild );
    } else {
      SiTRACE("Si21%2d-%c%c%c Part running 'FW_%c_%cb%d'\n", api->rsp->part_info.part
                                                                    , api->rsp->part_info.chiprev + 0x40
                                                                    , api->rsp->part_info.pmajor
                                                                    , api->rsp->part_info.pminor
                                                                    , api->rsp->get_rev.cmpmajor
                                                                    , api->rsp->get_rev.cmpminor
                                                                    , api->rsp->get_rev.cmpbuild );
    }

    return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168B_LoadFirmware
  DESCRIPTON: Load firmware from FIRMWARE_TABLE array in Si2168B_Firmware_x_y_build_z.h file into Si2168B
              Requires Si2168B to be in bootloader mode after PowerUp
  Programming Guide Reference:    Flowchart A.3 (Download FW PATCH flowchart)

  Parameter:  Si2168B Context (I2C address)
  Parameter:  pointer to firmware table array
  Parameter:  number of lines in firmware table array (size in bytes / BYTES_PER_LINE)
  Returns:    Si2168B/I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_LoadFirmware        (L1_Si2168B_Context *api, unsigned char fw_table[], int nbLines)
{
    int return_code;
    int line;
    int load_start_ms;
    return_code = NO_Si2168B_ERROR;

    SiTRACE ("Si2168B_LoadFirmware starting...\n");
    SiTRACE ("Si2168B_LoadFirmware nbLines %d\n", nbLines);
    load_start_ms = system_time();
    /* for each line in fw_table */
    for (line = 0; line < nbLines; line++)
    {
        /* send Si2168B_BYTES_PER_LINE fw bytes to Si2168B */
        if ((return_code = Si2168B_L1_API_Patch(api, Si2168B_BYTES_PER_LINE, fw_table + Si2168B_BYTES_PER_LINE*line)) != NO_Si2168B_ERROR)
        {
          SiTraceConfiguration((char*)"traces resume");
          SiTRACE("Si2168B_LoadFirmware error 0x%02x patching line %d: %s\n", return_code, line, Si2168B_L1_API_ERROR_TEXT(return_code) );
          if (line == 0) {
          SiTRACE("The firmware is incompatible with the part!\n");
          }
          SiTraceConfiguration((char*)"traces resume");
          return ERROR_Si2168B_LOADING_FIRMWARE;
        }
        if (line==0) {
          if (system_time() - load_start_ms > 200) {
            SiERROR ("Si2168B_LoadFirmware line 1 took too much time!\n");
          }
          SiTRACE ("Si2168B_LoadFirmware line 1 took %4d ms\n", system_time() - load_start_ms);
        }
        if (line==3) {SiTraceConfiguration((char*)"traces suspend");}
    }
    SiTraceConfiguration((char*)"traces resume");
    SiTRACE ("Si2168B_LoadFirmware took %4d ms\n", system_time() - load_start_ms);
    /* Storing Properties startup configuration in propShadow                              */
    /* !! Do NOT change the content of Si2168B_storePropertiesDefaults                   !! */
    /* !! It should reflect the part internal property settings after firmware download !! */
    Si2168B_storePropertiesDefaults (api->propShadow);

    SiTRACE ("Si2168B_LoadFirmware complete...\n");
    return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168B_LoadFirmware_16
  DESCRIPTON: Load firmware from firmware_struct array in Si2168B_Firmware_x_y_build_z.h file into Si2168B
              Requires Si2168B to be in bootloader mode after PowerUp

  Parameter:  Si2168B Context (I2C address)
  Parameter:  pointer to firmware_struct array
  Parameter:  number of lines in firmware table array (size in bytes / firmware_struct)
  Returns:    Si2168B/I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_LoadFirmware_16     (L1_Si2168B_Context *api, firmware_struct fw_table[], int nbLines)
{
    int return_code;
    int line;
    int load_start_ms;
    return_code = NO_Si2168B_ERROR;

    SiTRACE ("Si2168B_LoadFirmware_16 starting...\n");
    SiTRACE ("Si2168B_LoadFirmware_16 nbLines %d\n", nbLines);
    load_start_ms = system_time();
    /* for each line in fw_table */
    for (line = 0; line < nbLines; line++) {
      if (fw_table[line].firmware_len > 0)  /* don't download if length is 0 , e.g. dummy firmware */
      {
        /* send firmware_len bytes (up to 16) to Si2168B */
        if ((return_code = Si2168B_L1_API_Patch(api, fw_table[line].firmware_len, fw_table[line].firmware_table)) != NO_Si2168B_ERROR)
        {
          SiTRACE("Si2168B_LoadFirmware_16 error 0x%02x patching line %d: %s\n", return_code, line, Si2168B_L1_API_ERROR_TEXT(return_code) );
          if (line == 0) {
          SiTRACE("The firmware is incompatible with the part!\n");
          }
          return ERROR_Si2168B_LOADING_FIRMWARE;
        }
        if (line==3) {SiTraceConfiguration("traces suspend");}
      }
    }
    SiTraceConfiguration("traces resume");
    SiTRACE ("Si2168B_LoadFirmware_16 took %4d ms\n", system_time() - load_start_ms);
    /* Storing Properties startup configuration in propShadow                              */
    /* !! Do NOT change the content of Si2168B_storePropertiesDefaults                   !! */
    /* !! It should reflect the part internal property settings after firmware download !! */
    Si2168B_storePropertiesDefaults (api->propShadow);

    SiTRACE ("Si2168B_LoadFirmware_16 complete...\n");
    return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168B_LoadFirmwareSPI
  DESCRIPTON: Load firmware from FIRMWARE_TABLE array in Si2168B_Firmware_x_y_build_z.h file into Si2168B
              Requires Si2168B to be in bootloader mode after PowerUp
  Programming Guide Reference:    Flowchart A.3 (Download FW PATCH flowchart)

  Parameter:  Si2168B Context (I2C address)
  Parameter:  pointer to firmware table array
  Parameter:  number of lines in firmware table array (size in bytes / BYTES_PER_LINE)
  Returns:    Si2168B/I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_LoadFirmwareSPI     (L1_Si2168B_Context *api, firmware_struct fw_table[], int nbLines, unsigned char pbl_key,  unsigned char pbl_num )
{
//Edit by ITE
#if 0
    #define SPI_MAX_BUFFER_SIZE 64
    int return_code;
    int line;
    int load_start_ms;
    unsigned char spi_bytes[SPI_MAX_BUFFER_SIZE];
    char spi_index;

/*
  #define   SPI_DEBUGGING
*/
  #ifdef    SPI_DEBUGGING
    int  spi_state;
    int  spi_log;
  #endif /* SPI_DEBUGGING */
    unsigned char PATCH_IMAGE_SPI[] = {0x04,0x01,0x80,0x00,0x00,0x00,0x00,0x00};

    return_code = NO_Si2168B_ERROR;

    /* <porting> For SiLabs EVB, the SPI_config code is 0x07, to allow spi_data and spi_clk signals
                  to be sent using several configuration on PA2/PA1/PA0 pins of the Cypress part
                  This needs to be adapted to each SPI setup                                          */
    if ( L0_EnableSPI(0x03) == 0) {
      SiERROR ("SPI can't be enabled. It's not supported by the L0!\n");
      SiTRACE ("SPI can't be enabled. It's not supported by the L0!\n");
      return_code = ERROR_Si2168B_LOADING_FIRMWARE;
      goto exit_spi;
    }

    SiTRACE ("Si2168B_LoadFirmwareSPI starting...nbLines %d spi_send_option %d\n", nbLines, api->spi_send_option);

    L0_WriteCommandBytes  (api->i2c, 8, PATCH_IMAGE_SPI);
    Si2168B_L1_CheckStatus (api);

    /* Set all spi__link fields to allow SPI download */
    api->cmd->spi_link.subcode       = Si2168B_SPI_LINK_CMD_SUBCODE_CODE;
    api->cmd->spi_link.spi_pbl_key   = pbl_key;
    api->cmd->spi_link.spi_pbl_num   = pbl_num;
    api->cmd->spi_link.spi_conf_clk  = api->spi_clk_pin;
    api->cmd->spi_link.spi_clk_pola  = api->spi_clk_pola;
    api->cmd->spi_link.spi_conf_data = api->spi_data_pin;
    api->cmd->spi_link.spi_data_dir  = api->spi_data_order;
    api->cmd->spi_link.spi_enable    = Si2168B_SPI_LINK_CMD_SPI_ENABLE_ENABLE;

    Si2168B_L1_SendCommand2(api, Si2168B_SPI_LINK_CMD_CODE);

    /* Set spi_link fields to prepare disabling SPI download */
    api->cmd->spi_link.spi_pbl_key   =   0;
    api->cmd->spi_link.spi_conf_clk  = Si2168B_SPI_LINK_CMD_SPI_CONF_CLK_DISABLE;
    api->cmd->spi_link.spi_conf_data = Si2168B_SPI_LINK_CMD_SPI_CONF_DATA_DISABLE;
    api->cmd->spi_link.spi_enable    = Si2168B_SPI_LINK_CMD_SPI_ENABLE_DISABLE;

    load_start_ms = system_time();
    spi_index = 0;

    /* for each line in fw_table */
    for (line = 0; line < nbLines; line++) {
  #ifdef    SPI_DEBUGGING
      if (line == 0) {
        spi_log   = spi_state = -1;
        spi_bytes[0] = 0xff; spi_bytes[1] = 0x00; L0_WriteBytes(api->i2c, 0x00, 2, spi_bytes);
        api->i2c->indexSize  = 2; api->i2c->trackWrite = api->i2c->trackRead = 1; api->i2c->mustReadWithoutStop = 1;
      }
  #endif /* SPI_DEBUGGING */
      if (spi_index + fw_table[line].firmware_len <= SPI_MAX_BUFFER_SIZE) {
       /* pile up bytes in the spi_buffer, if the entire line fits in */
        memcpy(spi_bytes+spi_index, fw_table[line].firmware_table,  fw_table[line].firmware_len );
        spi_index = spi_index + fw_table[line].firmware_len;
      } else {
       /* No room in the buffer for the entire line, send bytes over SPI */
        return_code = L0_LoadSPI(spi_bytes, spi_index, api->spi_send_option);
       /* Set value to re-process the current line with an empty buffer  */
        spi_index = 0;
  #ifdef    SPI_DEBUGGING
        if (spi_log != (spi_state = L0_ReadRegister(api->i2c, 0x57, 0, 3, 0) ) ) {
          spi_log = spi_state;
          SiTRACE("\nline %6d : spi_state %d ", line, spi_state);
        }
  #endif /* SPI_DEBUGGING */
        line--;
        if ( return_code != NO_Si2168B_ERROR)
        {
          SiTraceConfiguration((char*)"traces resume");
          Si2168B_L1_SendCommand2(api, Si2168B_SPI_LINK_CMD_CODE);
          SiTRACE("Si2168B_LoadFirmwareSPI  error 0x%02x patching line %d: %s\n", return_code, line, Si2168B_L1_API_ERROR_TEXT(return_code) );
          SiERROR("Si2168B_LoadFirmwareSPI  error\n");
  #ifdef    SPI_DEBUGGING
          api->i2c->indexSize  = 0; api->i2c->trackWrite = api->i2c->trackRead = 0;
          spi_bytes[0] = 0xfe; spi_bytes[1] = 0x00; L0_WriteBytes(api->i2c, 0x00, 2, spi_bytes);
  #endif /* SPI_DEBUGGING */
          return_code = ERROR_Si2168B_LOADING_FIRMWARE;
          goto exit_spi;
        }
      }
      if (line==4) {SiTraceConfiguration("traces suspend"); }
    }
    if (spi_index) {
      /* If the buffer is not empty, send the last lines, even though the buffer is not full */
      L0_LoadSPI(spi_bytes, spi_index, api->spi_send_option);
    }

    SiTraceConfiguration((char*)"traces resume");
  #ifdef    SPI_DEBUGGING
    api->i2c->indexSize  = 0;
    spi_bytes[0] = 0xfe; spi_bytes[1] = 0x00; L0_WriteBytes(api->i2c, 0x00, 2, spi_bytes);
    api->i2c->trackWrite = api->i2c->trackRead = 0;
  #endif /* SPI_DEBUGGING */
    Si2168B_L1_SendCommand2(api, Si2168B_SPI_LINK_CMD_CODE);
    SiTRACE ("Si2168B_LoadFirmwareSPI took %4d ms\n", system_time() - load_start_ms);
    /* Storing Properties startup configuration in propShadow                              */
    /* !! Do NOT change the content of Si2168B_storePropertiesDefaults                   !! */
    /* !! It should reflect the part internal property settings after firmware download !! */
    Si2168B_storePropertiesDefaults (api->propShadow);

    SiTRACE ("Si2168B_LoadFirmware complete...\n");
    return_code = NO_Si2168B_ERROR;

    exit_spi:

    L0_DisableSPI();
    return return_code;
    
#endif 
	return 0;    
}
/************************************************************************************************************************
  NAME: Si2168B_StartFirmware
  DESCRIPTION: Start Si2168B firmware (put the Si2168B into run mode)
  Parameter:   Si2168B Context (I2C address)
  Parameter (passed by Reference):   ExitBootloadeer Response Status byte : tunint, atvint, dtvint, err, cts
  Returns:     I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_StartFirmware       (L1_Si2168B_Context *api)
{
    if (Si2168B_L1_EXIT_BOOTLOADER(api, Si2168B_EXIT_BOOTLOADER_CMD_FUNC_NORMAL, Si2168B_EXIT_BOOTLOADER_CMD_CTSIEN_OFF) != NO_Si2168B_ERROR)
    {
        return ERROR_Si2168B_STARTING_FIRMWARE;
    }

    return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  NAME: Si2168B_Init
  DESCRIPTION:Reset and Initialize Si2168B
  Parameter:  Si2168B Context (I2C address)
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
************************************************************************************************************************/
int Si2168B_Init                (L1_Si2168B_Context *api)
{
    int return_code;
    SiTRACE("Si2168B_Init starting...\n");

    if ((return_code = Si2168B_PowerUpWithPatch(api)) != NO_Si2168B_ERROR) {   /* PowerUp into bootloader */
        SiTRACE ("Si2168B_PowerUpWithPatch error 0x%02x: %s\n", return_code, Si2168B_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }
    /* At this point, FW is loaded and started.  */
    Si2168B_Configure(api);
    SiTRACE("Si2168B_Init complete...\n");
    return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  Si2168B_Media function
  Use:        media retrieval function
              Used to retrieve the media used by the Si2168B
************************************************************************************************************************/
int Si2168B_Media               (L1_Si2168B_Context *api, int modulation)
{
  switch (modulation) {
    default             :
    case Si2168B_DD_MODE_PROP_MODULATION_AUTO_DETECT : {
      switch (api->prop->dd_mode.auto_detect) {
        default             : break;
        case Si2168B_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2     : return Si2168B_TERRESTRIAL; break;
      }
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT : return Si2168B_TERRESTRIAL; break;
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2: return Si2168B_TERRESTRIAL; break;
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC :
    case Si2168B_DD_MODE_PROP_MODULATION_MCNS : return Si2168B_TERRESTRIAL; break;

  }
  SiERROR("UNKNOWN media!\n");
  return 0;
}
/************************************************************************************************************************
  Si2168B_DVB_C_max_lock_ms function
  Use:        DVB-C lock time retrieval function
              Used to know how much time DVB-C lock will take in the worst case
************************************************************************************************************************/
int Si2168B_DVB_C_max_lock_ms   (L1_Si2168B_Context *api, int constellation, int symbol_rate_baud)
{
  int   swt;
  int   afc_khz;
  
//Edit by ITE  
  //float swt_coeff;
  int	swt_coeff_multi2;
  
  
  
  /* To avoid division by 0, return 5000 if SR is 0 */
  if (symbol_rate_baud == 0) return 5000;
  afc_khz = api->prop->dvbc_afc_range.range_khz;
  if (afc_khz*1000 > symbol_rate_baud*11/100 ) { afc_khz = symbol_rate_baud*11/100000;}
  
//Edit by ITE
/*  
  swt      = (int)(0.5 + (2.0/3.0)*afc_khz*16777216/((symbol_rate_baud/1000.0)*(symbol_rate_baud/1000.0)));
  switch (constellation) {
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_QAM64  :
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_QAM16  :
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_QAM256 : { swt_coeff =  3.0; break; }
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_QAM128 : { swt_coeff =  4.5; break; }
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_AUTO   :
    default                                                     : { swt_coeff = 11.0; break; }
  }
  SiTRACE("afc_khz %3d, swt %6d, swt_coeff %.1f DVB_C_max_lock_ms %d\n", afc_khz, swt, swt_coeff, (int)(720000000/symbol_rate_baud + swt*swt_coeff)+ 100 );
  return (int)(720000000/symbol_rate_baud + swt*swt_coeff)+ 100;
*/
   swt      = (int)(1 + (2/3) * afc_khz * 16777216 / ((symbol_rate_baud / 1000) * (symbol_rate_baud / 1000)));
  switch (constellation) {
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_QAM64  :
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_QAM16  :
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_QAM256 : { swt_coeff_multi2 =  6; break; }
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_QAM128 : { swt_coeff_multi2 =  9; break; }
    case    Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_AUTO   :
    default                                                     : { swt_coeff_multi2 = 22; break; }
  }

	return (int)(720000000/symbol_rate_baud + swt*swt_coeff_multi2/2)+ 100;  
}
/*****************************************************************************************/
/*               SiLabs demodulator API functions (demod and tuner)                      */
/*****************************************************************************************/
/* Allow profiling information during Si2168B_switch_to_standard */
#define PROFILING
/************************************************************************************************************************
  Si2168B_standardName function
  Use:        standard text retrieval function
              Used to retrieve the standard text used by the Si2168B
  Parameter:  standard, the value of the standard
************************************************************************************************************************/
char *Si2168B_standardName (int standard)
{
  switch (standard)
  {
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT    : {return (char*)"DVB-T"  ;}
    case Si2168B_DD_MODE_PROP_MODULATION_MCNS    : {return (char*)"MCNS"   ;}
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC    : {return (char*)"DVB-C"  ;}
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2   : {return (char*)"DVB-T2" ;}
    default                                     : {return (char*)"UNKNOWN";}
  }
}
/************************************************************************************************************************
  Si2168B_L2_Infos function
  Use:        software information function
              Used to retrieve information about the compilation
  Parameter:  front_end, a pointer to the Si2168B_L2_Context context to be initialized
  Parameter:  infoString, a text buffer to be filled with the information.
              It must be initialized by the caller with a size of 1000.
  Return:     the length of the information string
************************************************************************************************************************/
int  Si2168B_L2_Infos              (Si2168B_L2_Context *front_end, char *infoString)
{
//Edit by ITE
#if 0
    if (infoString == NULL) return 0;
    if (front_end  == NULL) {
      snprintf(infoString, 1000, "Si2168B front-end not initialized yet. Call Si2168B_L2_SW_Init first!\n");
      return strlen(infoString);
    }

    snprintf(infoString, 1000, "\n");
    snprintf(infoString, 1000, "%s--------------------------------------\n", infoString);
    snprintf(infoString, 1000, "%sSi_I2C               Source code %s\n"   , infoString, Si_I2C_TAG_TEXT() );
    snprintf(infoString, 1000, "%sDemod                Si2168B  at 0x%02x\n", infoString , front_end->demod->i2c->address);
    snprintf(infoString, 1000, "%sDemod                Source code %s\n"   , infoString , Si2168B_L1_API_TAG_TEXT() );
#ifdef    TERRESTRIAL_FRONT_END
#ifdef    TER_TUNER_SILABS
    snprintf(infoString, 1000, "%sTerrestrial tuner    SiLabs    at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_SILABS */
#ifdef    TER_TUNER_CUSTOMTER
    snprintf(infoString, 1000, "%sTerrestrial tuner    CUSTOMTER  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_NO_TER
    snprintf(infoString, 1000, "%sTerrestrial tuner    NO_TER  at 0x%02x\n", infoString , front_end->tuner_ter->i2c->address);
#endif /* TER_TUNER_NO_TER */
#ifdef    TER_TUNER_TAG_TEXT
    snprintf(infoString, 1000, "%sTerrestrial tuner    Source code %s\n"   , infoString , TER_TUNER_TAG_TEXT() );
#endif /* TER_TUNER_TAG_TEXT */
#endif /* TERRESTRIAL_FRONT_END */


#ifdef    TERRESTRIAL_FRONT_END
  if ( front_end->demod->tuner_ter_clock_source == Si2168B_TER_Tuner_clock) snprintf(infoString, 1000, "%sTER clock from  TER Tuner ", infoString);
  if ( front_end->demod->tuner_ter_clock_source == Si2168B_SAT_Tuner_clock) snprintf(infoString, 1000, "%sTER clock from  SAT Tuner ", infoString);
  if ( front_end->demod->tuner_ter_clock_source == Si2168B_Xtal_clock     ) snprintf(infoString, 1000, "%sTER clock from  Xtal      ", infoString);
                                                                           snprintf(infoString, 1000, "%s(%d MHz)\n", infoString , front_end->demod->tuner_ter_clock_freq);
  if ( front_end->demod->tuner_ter_clock_input  == Si2168B_START_CLK_CMD_CLK_MODE_CLK_CLKIO  ) snprintf(infoString, 1000, "%sTER clock input CLKIO\n"  , infoString);
  if ( front_end->demod->tuner_ter_clock_input  == Si2168B_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN) snprintf(infoString, 1000, "%sTER clock input XTAL_IN\n", infoString);
  if ( front_end->demod->tuner_ter_clock_input  == Si2168B_START_CLK_CMD_CLK_MODE_XTAL       ) snprintf(infoString, 1000, "%sTER clock input XTAL\n"   , infoString);

  if (front_end->demod->fef_mode                 == Si2168B_FEF_MODE_SLOW_NORMAL_AGC          ) snprintf(infoString, 1000, "%sFEF mode 'SLOW NORMAL AGC'" , infoString);
  if (front_end->demod->fef_mode                 == Si2168B_FEF_MODE_SLOW_INITIAL_AGC         ) snprintf(infoString, 1000, "%sFEF mode 'SLOW INITIAL AGC'", infoString);
  if (front_end->demod->fef_mode                 == Si2168B_FEF_MODE_FREEZE_PIN               ) snprintf(infoString, 1000, "%sFEF mode 'FREEZE PIN'"      , infoString);
  if (front_end->demod->fef_mode                 != front_end->demod->fef_selection          ) snprintf(infoString, 1000, "%s(CHANGED!)"                 , infoString);
  snprintf(infoString, 1000, "%s\n", infoString);
#endif /* TERRESTRIAL_FRONT_END */


  snprintf(infoString, 1000, "%s--------------------------------------\n", infoString);
  return strlen(infoString);
  
#endif
  return 0;  
}
/************************************************************************************************************************
  Si2168B_L2_SW_Init function
  Use:        software initialization function
              Used to initialize the Si2168B and tuner structures
  Behavior:   This function performs all the steps necessary to initialize the Si2168B and tuner instances
  Parameter:  front_end, a pointer to the Si2168B_L2_Context context to be initialized
  Parameter:  demodAdd, the I2C address of the demod
  Parameter:  tunerAdd, the I2C address of the tuner
  Comments:     It MUST be called first and once before using any other function.
                It can be used to build a multi-demod/multi-tuner application, if called several times from the upper layer with different pointers and addresses
                After execution, all demod and tuner functions are accessible.
************************************************************************************************************************/
//Edit by ITE
char Si2168B_L2_SW_Init            (Si2168B_L2_Context *front_end
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
                                   )
{
    char infoStringBuffer[1000];
    char *infoString;
    infoString = &(infoStringBuffer[0]);

    SiTRACE("Si2168B_L2_EVB_SW_Init starting...\n");

    /* Pointers initialization */
    front_end->demod     = &(front_end->demodObj    );
    front_end->Si2168B_init_done    = 0;
    front_end->first_init_done     = 0;
    front_end->handshakeUsed       = 0; /* set to '0' by default for compatibility with previous versions */
    front_end->handshakeOn         = 0;
    front_end->handshakePeriod_ms  = 1000;
    front_end->tuner_ter = &(front_end->tuner_terObj);
    front_end->TER_init_done       = 0;
    front_end->TER_tuner_init_done = 0;
    front_end->auto_detect_TER     = 1;
 #ifdef    INDIRECT_I2C_CONNECTION
    front_end->f_TER_tuner_enable  = TER_tuner_enable_func;
    front_end->f_TER_tuner_disable = TER_tuner_disable_func;
 #endif /* INDIRECT_I2C_CONNECTION */
    /* Calling underlying SW initialization functions */
    
//Edit by ITE    
//    Si2168B_L1_API_Init      (front_end->demod,     demodAdd);
//    L1_RF_TER_TUNER_Init    (front_end->tuner_ter, tunerAdd_Ter);
    Si2168B_L1_API_Init      (front_end->demod, demodAdd, endeavour, endeavourChipidx, i2cbus);
    L1_RF_TER_TUNER_Init    (front_end->tuner_ter, tunerAdd_Ter, endeavour, endeavourChipidx, i2cbus);
    
    
    
    front_end->demod->fef_mode      = Si2168B_FEF_MODE_SLOW_NORMAL_AGC;
    front_end->demod->fef_selection = Si2168B_FEF_MODE;
    front_end->demod->fef_pin       = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_B;
 #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP
    /* If the TER tuner has initial AGC speed control and it's the selected mode, activate it */
    if (front_end->demod->fef_selection == Si2168B_FEF_MODE_SLOW_INITIAL_AGC) {
      SiTRACE("TER tuner FEF set to 'SLOW_INITIAL_AGC' mode\n");
      front_end->demod->fef_mode = Si2168B_FEF_MODE_SLOW_INITIAL_AGC;
    }
  #ifdef    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP
    /* If the TER tuner has an AGC freeze pin and it's the selected mode, activate it */
    if (front_end->demod->fef_selection == Si2168B_FEF_MODE_FREEZE_PIN      ) {
      SiTRACE("TER tuner FEF set to 'FREEZE_PIN' mode\n");
      front_end->demod->fef_mode = Si2168B_FEF_MODE_FREEZE_PIN;
    }
  #else  /* L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP */
    if (front_end->demod->fef_selection == Si2168B_FEF_MODE_FREEZE_PIN      ) {
      SiTRACE("TER tuner FEF can not use 'FREEZE_PIN' mode, using 'SLOW_INITIAL_AGC' mode instead\n");
      front_end->demod->fef_mode = Si2168B_FEF_MODE_SLOW_INITIAL_AGC;
    }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP */
 #else  /* L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP */
      SiTRACE("TER tuner FEF set to 'SLOW_NORMAL_AGC' mode\n");
 #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP */
    front_end->callback        = p_context;
#ifdef    SiTRACE
    if (Si2168B_L2_Infos(front_end, infoString))  {SiTRACE("%s\n", infoString);}
#endif /* SiTRACE */
    SiTRACE("Si2168B_L2_EVB_SW_Init complete\n");
    return 1;
}
/************************************************************************************************************************
  Si2168B_L2_HW_Connect function
  Use:        Front-End connection function
              Specific to SiLabs USB connection!
  Porting:    Remove or replace by the final application corresponding calls
  Behavior:   This function connects the Si2168B, and the tuners via the Cypress chip
  Parameter:  *front_end, the front-end handle
  Parameter   mode, the required connection mode
************************************************************************************************************************/
//Edit by ITE
void Si2168B_L2_HW_Connect         (Si2168B_L2_Context *front_end)
{
}
/************************************************************************************************************************
  NAME: Si2168B_L2_Part_Check
  DESCRIPTION:startup and part checking Si2168B
  Parameter:  Si2168B Context
  Returns:    the part_info 'part' field value
************************************************************************************************************************/
int Si2168B_L2_Part_Check          (Si2168B_L2_Context   *front_end)
{
  int start_time_ms;
  front_end->demod->rsp->part_info.part = 0;
  start_time_ms = system_time();
  front_end->demod->cmd->power_up.reset = Si2168B_POWER_UP_CMD_RESET_RESET;
  front_end->demod->cmd->power_up.func  = Si2168B_POWER_UP_CMD_FUNC_BOOTLOADER;
#ifdef    SiTRACES
    SiTraceConfiguration("traces suspend");
#endif /* SiTRACES */
  Si2168B_WAKEUP      (front_end->demod);
  Si2168B_L1_PART_INFO(front_end->demod);
#ifdef    SiTRACES
  SiTraceConfiguration("traces resume");
#endif /* SiTRACES */
  SiTRACE("Si2168B_Part_Check took %3d ms. Part is Si21%2d\n", system_time() - start_time_ms, front_end->demod->rsp->part_info.part );
  return front_end->demod->rsp->part_info.part;
}
/************************************************************************************************************************
  Si2168B_L2_Tuner_I2C_Enable function
  Use:        Tuner i2c bus connection
              Used to allow communication with the tuners
  Parameter:  *front_end, the front-end handle
************************************************************************************************************************/
unsigned char Si2168B_L2_Tuner_I2C_Enable (Si2168B_L2_Context *front_end)
{
    return Si2168B_L1_I2C_PASSTHROUGH(front_end->demod, Si2168B_I2C_PASSTHROUGH_CMD_SUBCODE_CODE, Si2168B_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_CLOSE, Si2168B_I2C_PASSTHROUGH_CMD_RESERVED_RESERVED);
}
/************************************************************************************************************************
  Si2168B_L2_Tuner_I2C_Disable function
  Use:        Tuner i2c bus connection
              Used to disconnect i2c communication with the tuners
  Parameter:  *front_end, the front-end handle
************************************************************************************************************************/
unsigned char Si2168B_L2_Tuner_I2C_Disable(Si2168B_L2_Context *front_end)
{
    return Si2168B_L1_I2C_PASSTHROUGH(front_end->demod, Si2168B_I2C_PASSTHROUGH_CMD_SUBCODE_CODE, Si2168B_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_OPEN, Si2168B_I2C_PASSTHROUGH_CMD_RESERVED_RESERVED);
}
/************************************************************************************************************************
  Si2168B_L2_communication_check function
  Use:        Si2168B front i2c bus connection check
              Used to check i2c communication with the demod and the tuners
  Parameter:  *front_end, the front-end handle
************************************************************************************************************************/
int  Si2168B_L2_communication_check(Si2168B_L2_Context *front_end)
{
  int comm_errors;
  comm_errors=0;
  /* Close i2c Passthru       */
  Si2168B_L2_Tuner_I2C_Enable(front_end);
  /* Check i2c Passthru value */
  if ((int)Si2168B_L1_CheckStatus(front_end->demod) != NO_Si2168B_ERROR) {
    SiTRACE ("DEMOD Communication error ! \n");
    comm_errors++;
  } else {
    SiTRACE ("DEMOD Communication OK\n");
  }
 #ifdef    INDIRECT_I2C_CONNECTION
  front_end->f_TER_tuner_enable(front_end->callback);
 #endif /* INDIRECT_I2C_CONNECTION */
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
 #ifdef    INDIRECT_I2C_CONNECTION
  front_end->f_TER_tuner_disable(front_end->callback);
  front_end->f_SAT_tuner_disable(front_end->callback);
 #endif /* INDIRECT_I2C_CONNECTION */
  Si2168B_L2_Tuner_I2C_Disable(front_end);

  if (comm_errors) return 0;

  return 1;
}
/************************************************************************************************************************
  Si2168B_L2_switch_to_standard function
  Use:      Standard switching function selection
            Used to switch nicely to the wanted standard, taking into account the previous state
  Parameter: new_standard the wanted standard to switch to
  Behavior: This function positions a set of flags to easily decide what needs to be done to
              switch between standards.
************************************************************************************************************************/
int  Si2168B_L2_switch_to_standard (Si2168B_L2_Context *front_end, unsigned char new_standard, unsigned char force_full_init)
{
  /* previous state flags */
  int dtv_demod_already_used = 0;
  #ifdef    INDIRECT_I2C_CONNECTION
  int ter_i2c_connected      = 0;
  #else  /* INDIRECT_I2C_CONNECTION */
  int i2c_connected          = 0;
  #endif /* INDIRECT_I2C_CONNECTION */
  int ter_tuner_already_used = 0;
  int ter_clock_already_used = 0;
  /* new state flags      */
  int dtv_demod_needed       = 0;
  int ter_tuner_needed       = 0;
  int ter_clock_needed       = 0;
  int dtv_demod_sleep_request= 0;
  int dtv_demod_sleeping     = 0;
  int res;

#ifdef    PROFILING
  int start;
  int ter_tuner_delay   = 0;
  int dtv_demod_delay   = 0;
  int switch_start;
  char sequence[1000];
  #define TER_DELAY  ter_tuner_delay=ter_tuner_delay+system_time()-start;start=system_time();
  #define DTV_DELAY  dtv_demod_delay=dtv_demod_delay+system_time()-start;start=system_time();
#else
  #define TER_DELAY
  #define DTV_DELAY
#endif /* PROFILING */

#ifdef    PROFILING
  start = switch_start = system_time();
  SiTRACE("%s->%s\n", Si2168B_standardName(front_end->previous_standard), Si2168B_standardName(new_standard) );
#endif /* PROFILING */

  SiTRACE("Si2168B_switch_to_standard starting...\n");
  SiTRACE(" %s-->%s switch starting with Si2168B_init_done %d, first_init_done %d\n", Si2168B_standardName(front_end->previous_standard), Si2168B_standardName(new_standard), front_end->Si2168B_init_done, front_end->first_init_done);
  SiTRACE("TER flags:    TER_init_done %d, TER_tuner_init_done %d\n", front_end->TER_init_done, front_end->TER_tuner_init_done);

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
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT :
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2:
    case Si2168B_DD_MODE_PROP_MODULATION_MCNS :
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC : {
      dtv_demod_already_used = 1;
      ter_tuner_already_used = 1;
      if ( front_end->demod->tuner_ter_clock_source == Si2168B_TER_Tuner_clock) {
        ter_clock_already_used = 1;
      }
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_ANALOG: {
      ter_tuner_already_used = 1;
      dtv_demod_sleeping = 1;
      break;
    }
    default : /* SLEEP */   {
      ter_tuner_already_used = 0;
      dtv_demod_sleeping = 1;
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
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT :
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2:
    case Si2168B_DD_MODE_PROP_MODULATION_MCNS :
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC : {
      dtv_demod_needed = 1;
      ter_tuner_needed = 1;
      if ( front_end->demod->tuner_ter_clock_source == Si2168B_TER_Tuner_clock) {
        ter_clock_needed = 1;
      }
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_ANALOG: {
      ter_tuner_needed = 1;
      break;
    }
    default : /* SLEEP */   {
      ter_tuner_needed = 0;
      break;
    }
  }

  /* ------------------------------------------------------------ */
  /* For multiple front-ends: override clock_needed flags         */
  /*  to avoid switching shared clocks                            */
  /* ------------------------------------------------------------ */
  /* For multiple front-ends: override clock_needed flags to avoid switching shared clocks */
  if (ter_clock_already_used == 0) {
    if (front_end->TER_tuner_init_done == 1) {
      if ( front_end->demod->tuner_ter_clock_control == Si2168B_CLOCK_ALWAYS_ON ) { SiTRACE("forcing ter_clock_already_used = 1\n"); ter_clock_already_used = 1; }
    }
  } else {
    if ( front_end->demod->tuner_ter_clock_control == Si2168B_CLOCK_ALWAYS_OFF  ) { SiTRACE("forcing ter_clock_already_used = 0\n"); ter_clock_already_used = 0; }
  }
  if (ter_clock_needed == 0) {
    if ( front_end->demod->tuner_ter_clock_control == Si2168B_CLOCK_ALWAYS_ON   ) { SiTRACE("forcing ter_clock_needed = 1\n"); ter_clock_needed = 1; }
  } else {
    if ( front_end->demod->tuner_ter_clock_control == Si2168B_CLOCK_ALWAYS_OFF  ) { SiTRACE("forcing ter_clock_needed = 0\n"); ter_clock_needed = 0; }
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
    front_end->Si2168B_init_done    = 0;
    front_end->TER_init_done       = 0;
    front_end->TER_tuner_init_done = 0;
    /* set 'already used' flags to force full init  */
    ter_tuner_already_used = 0;
    dtv_demod_already_used = 0;
    /* set 'needed' flags to force tuner init if required by the clock control flags */
    if ( front_end->demod->tuner_ter_clock_control == Si2168B_CLOCK_ALWAYS_ON ) { SiTRACE("forcing ter_tuner_needed = 1\n"); ter_tuner_needed = 1; }
  }

  /* ------------------------------------------------------------ */
  /* Request demodulator sleep if its clock will be stopped       */
  /* ------------------------------------------------------------ */
  SiTRACE("ter_tuner_already_used %d, ter_tuner_needed %d\n", ter_tuner_already_used,ter_tuner_needed);
  SiTRACE("ter_clock_already_used %d, ter_clock_needed %d\n", ter_clock_already_used,ter_clock_needed);
  if ((ter_tuner_already_used == 1) & (ter_tuner_needed == 0) ) { SiTRACE("TER tuner 1->0 "); }
  if ((ter_tuner_already_used == 0) & (ter_tuner_needed == 1) ) { SiTRACE("TER tuner 0->1 "); }
  if ((ter_clock_already_used == 1) & (ter_clock_needed == 0) ) { SiTRACE("TER clock 1->0 "); dtv_demod_sleep_request = 1; }
  if ((ter_clock_already_used == 0) & (ter_clock_needed == 1) ) { SiTRACE("TER clock 0->1 "); dtv_demod_sleep_request = 1; }
  SiTRACE("\n");
  /* ------------------------------------------------------------ */
  /* Request demodulator sleep if transition from '1' to '0'      */
  /* ------------------------------------------------------------ */
  if ((dtv_demod_already_used == 1) & (dtv_demod_needed == 0) ) { dtv_demod_sleep_request = 1; }
  SiTRACE(" %s-->%s switch flags    dtv_demod_already_used %d, dtv_demod_needed %d, dtv_demod_sleep_request %d, dtv_demod_sleeping %d\n", Si2168B_standardName(front_end->previous_standard), Si2168B_standardName(new_standard),
                                 dtv_demod_already_used,    dtv_demod_needed,    dtv_demod_sleep_request,    dtv_demod_sleeping   );
  /* ------------------------------------------------------------ */
  /* Sleep dtv demodulator if requested                           */
  /* ------------------------------------------------------------ */
  if (dtv_demod_sleep_request == 1) {
    SiTRACE("Sleep DTV demod\n");
    /* To avoid issues with the FEF pin when switching from T2 to ANALOG, set the demodulator for DVB-T/non auto detect reception before POWER_DOWN */
    if (new_standard                       == Si2168B_DD_MODE_PROP_MODULATION_ANALOG) {
      if ( ( (front_end->previous_standard == Si2168B_DD_MODE_PROP_MODULATION_DVBT  )
           & (front_end->demod->prop->dd_mode.auto_detect == Si2168B_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2) )
           | (front_end->previous_standard == Si2168B_DD_MODE_PROP_MODULATION_DVBT2 ) ) {
        front_end->demod->prop->dd_mode.modulation  = Si2168B_DD_MODE_PROP_MODULATION_DVBT;
        front_end->demod->prop->dd_mode.auto_detect = Si2168B_DD_MODE_PROP_AUTO_DETECT_NONE;
        Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_MODE_PROP_CODE);
        Si2168B_L1_DD_RESTART  (front_end->demod);
      }
    }
    /* If the demod is not needed, it means it's either going to ANALOG or SLEEP   */
    /*  In this case, set the demod silent, putting all possible pins to tristate  */
    /* To nicely recover from this, Si2168B_L2_Configure will need to be re-applied */
    /*  so set the flags to allow this                                             */
    if (dtv_demod_needed == 0) {
      Si2168B_SILENT(front_end, 1);
    }
    Si2168B_STANDBY (front_end->demod);
    dtv_demod_sleeping = 1;
    SiTRACE(" %s-->%s switch now   dtv_demod_sleeping %d\n", Si2168B_standardName(front_end->previous_standard), Si2168B_standardName(new_standard), dtv_demod_sleeping   );
    DTV_DELAY
  }

  /* ------------------------------------------------------------ */
  /* Set media for new standard                                   */
  /* ------------------------------------------------------------ */
  if (dtv_demod_needed) {
    front_end->demod->prop->dd_mode.modulation = new_standard;
    front_end->demod->media = Si2168B_Media(front_end->demod, front_end->demod->prop->dd_mode.modulation);
  }

  /* ------------------------------------------------------------ */
  /* Allow i2c traffic to reach the tuners                        */
  /* ------------------------------------------------------------ */
   #ifdef    INDIRECT_I2C_CONNECTION
   /* Connection will be done later on, depending on TER/SAT */
   #else  /* INDIRECT_I2C_CONNECTION */
    SiTRACE("Connect tuners i2c\n");
    Si2168B_L2_Tuner_I2C_Enable(front_end);
    DTV_DELAY
    i2c_connected = 1;
   #endif /* INDIRECT_I2C_CONNECTION */

  /* ------------------------------------------------------------ */
  /* Sleep Ter Tuner                                              */
  /* Sleep terrestrial tuner  if transition from '1' to '0'       */
  /* ------------------------------------------------------------ */
  if ((ter_tuner_already_used == 1) & (ter_tuner_needed == 0) ) {
   #ifdef    INDIRECT_I2C_CONNECTION
    if (ter_i2c_connected==0) {
      SiTRACE("-- I2C -- Connect TER tuner i2c to sleep it\n");
      front_end->f_TER_tuner_enable(front_end->callback);
      ter_i2c_connected++;
    }
    DTV_DELAY
   #endif /* INDIRECT_I2C_CONNECTION */
    SiTRACE("Sleep terrestrial tuner\n");
    #ifdef    TER_TUNER_CLOCK_OFF
    if (front_end->demod->tuner_ter_clock_control != Si2168B_CLOCK_ALWAYS_ON) {
      SiTRACE("Terrestrial tuner clock OFF\n");
      if ((res= TER_TUNER_CLOCK_OFF(front_end->tuner_ter)) !=0 ) {
        SiTRACE("Terrestrial tuner CLOCK OFF error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
        SiERROR("Terrestrial tuner CLOCK OFF error!\n");
        return 0;
      }
    }
    #endif /* TER_TUNER_CLOCK_OFF */
    #ifdef    TER_TUNER_STANDBY
    SiTRACE("Terrestrial tuner STANDBY\n");
    if ((res= TER_TUNER_STANDBY(front_end->tuner_ter)) !=0 ) {
      SiTRACE("Terrestrial tuner Standby error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
      SiERROR("Terrestrial tuner Standby error!\n");
      return 0;
    }
    #endif /* TER_TUNER_STANDBY */
    TER_DELAY
  }


  /* ------------------------------------------------------------ */
  /* Wakeup Ter Tuner                                             */
  /* Wake up terrestrial tuner if transition from '0' to '1'      */
  /* ------------------------------------------------------------ */
  if ((ter_tuner_already_used == 0) & (ter_tuner_needed == 1)) {
   #ifdef    INDIRECT_I2C_CONNECTION
    if (ter_i2c_connected==0) {
      SiTRACE("-- I2C -- Connect TER tuner i2c to init/wakeup it\n");
      front_end->f_TER_tuner_enable(front_end->callback);
      ter_i2c_connected++;
    }
    DTV_DELAY
   #endif /* INDIRECT_I2C_CONNECTION */
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
      }
      #ifdef    TER_TUNER_AGC_EXTERNAL
      if (front_end->demod->tuner_ter_agc_control) {
        TER_TUNER_AGC_EXTERNAL(front_end->tuner_ter);
      }
      #endif /* TER_TUNER_AGC_EXTERNAL */
      #endif /* TER_TUNER_INIT */
      front_end->TER_tuner_init_done =1;
    } else {
      SiTRACE("Wakeup terrestrial tuner\n");
      #ifdef    TER_TUNER_WAKEUP
      if ((res= TER_TUNER_WAKEUP(front_end->tuner_ter)) !=0) {
        SiTRACE("Terrestrial tuner wake up error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
        SiERROR("Terrestrial tuner wake up error!\n");
        return 0;
      }
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
     #ifdef    INDIRECT_I2C_CONNECTION
      if (ter_i2c_connected==0) {
        SiTRACE("-- I2C -- Connect TER tuner i2c to start its clock\n");
        front_end->f_TER_tuner_enable(front_end->callback);
        ter_i2c_connected++;
      }
      DTV_DELAY
     #endif /* INDIRECT_I2C_CONNECTION */
    if (front_end->demod->tuner_ter_clock_control != Si2168B_CLOCK_ALWAYS_OFF) {
      SiTRACE("Terrestrial tuner CLOCK ON\n");
      if ((res= TER_TUNER_CLOCK_ON(front_end->tuner_ter) ) !=0) {
        SiTRACE("Terrestrial tuner CLOCK ON error: 0x%02x : %s\n",res, TER_TUNER_ERROR_TEXT(res) );
        SiERROR("Terrestrial tuner CLOCK ON error!\n");
        return 0;
      }
    }
    #endif /* TER_TUNER_CLOCK_ON */
    TER_DELAY
  }
  if ((front_end->previous_standard != new_standard) & (dtv_demod_needed == 0) & (front_end->demod->media == Si2168B_TERRESTRIAL)) {
   if (front_end->demod->media == Si2168B_TERRESTRIAL) {
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
    if (front_end->demod->media == Si2168B_TERRESTRIAL) {
    #ifdef    TER_TUNER_DTV_LO_INJECTION
     TER_TUNER_DTV_LO_INJECTION(front_end->tuner_ter);
    #endif /* TER_TUNER_DTV_LO_INJECTION */
    #ifdef    TER_TUNER_DTV_LIF_OUT_AMP
    /* Adjusting LIF signal for cable or terrestrial reception */
      switch (new_standard) {
        case Si2168B_DD_MODE_PROP_MODULATION_DVBT :
        case Si2168B_DD_MODE_PROP_MODULATION_DVBT2:
        {
          TER_TUNER_DTV_LIF_OUT_AMP(front_end->tuner_ter, 0);
          break;
        }
       case Si2168B_DD_MODE_PROP_MODULATION_MCNS :
       case Si2168B_DD_MODE_PROP_MODULATION_DVBC : {
          TER_TUNER_DTV_LIF_OUT_AMP(front_end->tuner_ter, 1);
         break;
       }
       default: break;
      }
    #endif /* TER_TUNER_DTV_LIF_OUT_AMP */
    }
  }
  /* ------------------------------------------------------------ */
  /* Wakeup Dtv Demod                                             */
  /*  if it has been put in 'standby mode' and is needed          */
  /* ------------------------------------------------------------ */
  if (front_end->Si2168B_init_done) {
    SiTRACE("dtv_demod_sleeping %d\n", dtv_demod_sleeping);
    if ((dtv_demod_sleeping == 1) & (dtv_demod_needed == 1) ) {
      if (dtv_demod_already_used == 0) {
        SiTRACE("Take DTV demod out of SILENT mode\n");
        Si2168B_SILENT(front_end, 0);
      } else {
        SiTRACE("Wake UP DTV demod\n");
        if (Si2168B_WAKEUP (front_end->demod) == NO_Si2168B_ERROR) {
          SiTRACE("Wake UP DTV demod OK\n");
        } else {
          SiERROR("Wake UP DTV demod failed!\n");
          SiTRACE("Wake UP DTV demod failed!\n");
          return 0;
        }
      }
    }
  }
  /* ------------------------------------------------------------ */
  /* Setup Dtv Demod                                              */
  /* Setup dtv demodulator if transition from '0' to '1'          */
  /* ------------------------------------------------------------ */
  if (dtv_demod_needed == 1) {
    /* Do the 'first init' only the first time, plus if requested  */
    /* (when 'force' flag is 1, Si2168B_init_done is set to '0')   */
    if (!front_end->Si2168B_init_done) {
      SiTRACE("Init demod\n");
      if (Si2168B_Init(front_end->demod) == NO_Si2168B_ERROR) {
        front_end->Si2168B_init_done = 1;
        SiTRACE("Demod init OK\n");
      } else {
        SiTRACE("Demod init failed!\n");
        SiERROR("Demod init failed!\n");
        return 0;
      }
    }
    if (front_end->demod->media == Si2168B_TERRESTRIAL) {
      SiTRACE("front_end->demod->media Si2168B_TERRESTRIAL\n");
      if (front_end->TER_init_done == 0) {
        SiTRACE("Configure demod for TER\n");
        if (Si2168B_Configure(front_end->demod) == NO_Si2168B_ERROR) {
          /* set dd_mode.modulation again, as it is overwritten by Si2168B_Configure */
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
      if (new_standard == Si2168B_DD_MODE_PROP_MODULATION_DVBT2) {
        Si2168B_L2_TER_FEF_SETUP (front_end, 1);
      } else {
        Si2168B_L2_TER_FEF_SETUP (front_end, 0);
      }
      TER_DELAY
    }
    front_end->demod->prop->dd_mode.invert_spectrum = Si2168B_L2_Set_Invert_Spectrum(front_end);
    if (Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_MODE_PROP_CODE)==0) {
      Si2168B_L1_DD_RESTART(front_end->demod);
    } else {
      SiTRACE("Demod restart failed !\n");
      return 0;
    }
    DTV_DELAY
  }

  /* ------------------------------------------------------------ */
  /* Forbid i2c traffic to reach the tuners                       */
  /* ------------------------------------------------------------ */
  #ifdef    INDIRECT_I2C_CONNECTION
  if (ter_i2c_connected) {
    SiTRACE("-- I2C -- Disconnect TER tuner i2c\n");
    front_end->f_TER_tuner_disable(front_end->callback);
    DTV_DELAY
  }
  #else  /* INDIRECT_I2C_CONNECTION */
  if (i2c_connected) {
    SiTRACE("Disconnect tuners i2c\n");
    Si2168B_L2_Tuner_I2C_Disable(front_end);
    DTV_DELAY
  }
  #endif /* INDIRECT_I2C_CONNECTION */
  /* ------------------------------------------------------------ */
  /* update value of previous_standard to prepare next call       */
  /* ------------------------------------------------------------ */
  front_end->previous_standard = new_standard;
  front_end->demod->standard   = new_standard;

  front_end->first_init_done = 1;
#ifdef    PROFILING

//Edit by ITE
/*
  snprintf(sequence, 1000, "%s","");
  snprintf(sequence, 1000, "%s| TER: %4d ms ", sequence, ter_tuner_delay);
  snprintf(sequence, 1000, "%s| DTV: %4d ms ", sequence, dtv_demod_delay);
  snprintf(sequence, 1000, "%s| (%5d ms) ",    sequence, system_time()-switch_start);
*/
  sprintf(sequence, 1000, "%s","");
  sprintf(sequence, 1000, "%s| TER: %4d ms ", sequence, ter_tuner_delay);
  sprintf(sequence, 1000, "%s| DTV: %4d ms ", sequence, dtv_demod_delay);
  sprintf(sequence, 1000, "%s| (%5d ms) ",    sequence, system_time()-switch_start);
  
  SiTRACE("%s\n", sequence);
#endif /* PROFILING */

  SiTRACE("Si2168B_switch_to_standard complete\n");
  return 1;
}
/************************************************************************************************************************
  Si2168B_lock_to_carrier function
  Use:      relocking function
            Used to relock on a channel for the current standard
            options:
              if freq = 0, do not tune. This is useful to test the lock time without the tuner delays.
              if freq < 0, do not tune and don't change settings. Just do a DD_RESTART. This is useful to test the relock time upom a reset.
  Parameter: standard the standard to lock to
  Parameter: freq                the frequency to lock to    (in Hz for TER, in kHz for SAT)
  Parameter: dvb_t_bandwidth_hz  the channel bandwidth in Hz (only for DVB-T and DVB-T2)
  Parameter: dvb_t_stream        the HP/LP stream            (only for DVB-T)
  Parameter: symbol_rate_bps     the symbol rate             (for DVB-C, MCNS and SAT)
  Parameter: dvb_c_constellation the DVB-C constellation     (only for DVB-C)
  Parameter: data_slice_id       the DVB-C2 data slice Id    (only for DVB-C2)
  Parameter: plp_id              the PLP Id                  (only for DVB-T2 and DVB-C2 when num_dslice  > 1)
  Parameter: T2_lock_mode        the DVB-T2 lock mode        (0='ANY', 1='T2-Base', 2='T2-Lite')
  Return:    1 if locked, 0 otherwise
************************************************************************************************************************/
int   Si2168B_L2_lock_to_carrier   (Si2168B_L2_Context *front_end
                                 ,  int standard
                                 ,  int freq
                                  , int dvb_t_bandwidth_hz
                                  , unsigned char dvb_t_stream
                                  , unsigned int symbol_rate_bps
                                  , unsigned char dvb_c_constellation
                                  , int plp_id
                                  , unsigned char T2_lock_mode
                                  )
{
  int return_code;
  int lockStartTime;       /* lockStartTime is used to trace the time spent in Si2168B_L2_lock_to_carrier and is only set at when entering the function                       */
  int startTime;           /* startTime is used to measure internal durations. It is set in various places, whenever required                                                */
  int searchDelay;
  int handshakeDelay;
  int lock;
  int new_lock;
  int max_lock_time_ms = 0; 
  int min_lock_time_ms = 0; 
  plp_id = plp_id; /* to avoid compiler warning when not used */
  lockStartTime = system_time(); /* lockStartTime is used to trace the time spent in Si2168B_L2_lock_to_carrier and is only set here */
  lock     = 0;
  new_lock = 1; /* To avoid code checker warning */

  front_end->lockAbort = 0;
  SiTRACE ("relock to %s at %d\n", Si2168B_standardName(standard), freq);

  if (front_end->handshakeUsed == 0) {
    new_lock = 1;
    front_end->searchStartTime = lockStartTime;
  }
  if (front_end->handshakeUsed == 1) {
    if (front_end->handshakeOn == 1) {
      new_lock = 0;
      SiTRACE("lock_to_carrier_handshake : recalled after   handshake.\n");
    }
    if (front_end->handshakeOn == 0) {
      new_lock = 1;
      front_end->handshakeStart_ms = lockStartTime;
    }
    front_end->searchStartTime = front_end->handshakeStart_ms;
    SiTRACE("lock_to_carrier_handshake : handshake start %d\n", front_end->handshakeStart_ms);
  }

  /* Setting max_lock_time_ms and min_lock_time_ms for locking on required standard */
  switch (standard)
  {
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT : {
      max_lock_time_ms = Si2168B_DVBT_MAX_LOCK_TIME;
      min_lock_time_ms = Si2168B_DVBT_MIN_LOCK_TIME;
      if (front_end->auto_detect_TER) {
        max_lock_time_ms = Si2168B_DVBT2_MAX_LOCK_TIME;
      }
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2: {
      max_lock_time_ms = Si2168B_DVBT2_MAX_LOCK_TIME;
      min_lock_time_ms = Si2168B_DVBT2_MIN_LOCK_TIME;
      if (front_end->auto_detect_TER) {
        min_lock_time_ms = Si2168B_DVBT_MIN_LOCK_TIME;
      }
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC : {
      max_lock_time_ms = Si2168B_DVB_C_max_lock_ms(front_end->demod, dvb_c_constellation, symbol_rate_bps);
      min_lock_time_ms = Si2168B_DVBC_MIN_LOCK_TIME;
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_MCNS : {
      max_lock_time_ms = Si2168B_DVB_C_max_lock_ms(front_end->demod, dvb_c_constellation, symbol_rate_bps);
      min_lock_time_ms = Si2168B_DVBC_MIN_LOCK_TIME;
      break;
    }
    default : /* ATV */   {
      break;
    }
  }

  /* change settings only if not testing the relock time upon a reset (activated if freq<0) */
  if ( (freq >= 0 ) && (new_lock == 1) ) {
  /* Setting demod for locking on required standard */
    switch (standard)
    {
      case Si2168B_DD_MODE_PROP_MODULATION_DVBT2:
      case Si2168B_DD_MODE_PROP_MODULATION_DVBT : {
        front_end->demod->prop->dd_mode.bw                = dvb_t_bandwidth_hz/1000000;
        front_end->demod->prop->dvbt_hierarchy.stream = dvb_t_stream;
        return_code = Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DVBT_HIERARCHY_PROP_CODE);
        if (dvb_t_bandwidth_hz == 1700000) {
          front_end->demod->prop->dd_mode.bw              = Si2168B_DD_MODE_PROP_BW_BW_1D7MHZ;
        }
        if (front_end->auto_detect_TER) {
          SiTRACE("DVB-T/T2 auto detect\n");
          if (plp_id != -1) {
            front_end->demod->cmd->dvbt2_plp_select.plp_id_sel_mode = Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MANUAL;
            front_end->demod->cmd->dvbt2_plp_select.plp_id = (unsigned char)plp_id;
          } else {
            front_end->demod->cmd->dvbt2_plp_select.plp_id_sel_mode = Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_AUTO;
            front_end->demod->cmd->dvbt2_plp_select.plp_id = 0;
          }
          Si2168B_L1_DVBT2_PLP_SELECT    (front_end->demod, front_end->demod->cmd->dvbt2_plp_select.plp_id , front_end->demod->cmd->dvbt2_plp_select.plp_id_sel_mode);
          front_end->demod->prop->dd_mode.modulation  = Si2168B_DD_MODE_PROP_MODULATION_AUTO_DETECT;
          front_end->demod->prop->dd_mode.auto_detect = Si2168B_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2;
          front_end->demod->prop->dvbt2_mode.lock_mode= T2_lock_mode;
          Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DVBT2_MODE_PROP_CODE);
          SiTRACE ("T2_lock_mode %d\n",T2_lock_mode);
        } else {
          if (standard == Si2168B_DD_MODE_PROP_MODULATION_DVBT ) {
            front_end->demod->prop->dvbt_hierarchy.stream     = dvb_t_stream;
            return_code = Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DVBT_HIERARCHY_PROP_CODE);
          }
          if (standard == Si2168B_DD_MODE_PROP_MODULATION_DVBT2) {
            if (plp_id != -1) {
              front_end->demod->cmd->dvbt2_plp_select.plp_id_sel_mode = Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MANUAL;
              front_end->demod->cmd->dvbt2_plp_select.plp_id = (unsigned char)plp_id;
            } else {
              front_end->demod->cmd->dvbt2_plp_select.plp_id_sel_mode = Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_AUTO;
              front_end->demod->cmd->dvbt2_plp_select.plp_id = 0;
            }
            Si2168B_L1_DVBT2_PLP_SELECT    (front_end->demod, front_end->demod->cmd->dvbt2_plp_select.plp_id , front_end->demod->cmd->dvbt2_plp_select.plp_id_sel_mode);
            front_end->demod->prop->dvbt2_mode.lock_mode= T2_lock_mode;
            Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DVBT2_MODE_PROP_CODE);
            SiTRACE ("T2_lock_mode %d\n",T2_lock_mode);
          }
        }
        Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_MODE_PROP_CODE);
        SiTRACE("bw %d Hz\n", dvb_t_bandwidth_hz);
        break;
      }
      case Si2168B_DD_MODE_PROP_MODULATION_DVBC : {
        front_end->demod->prop->dd_mode.bw                       = 8;
        front_end->demod->prop->dvbc_symbol_rate.rate            = symbol_rate_bps/1000;
        front_end->demod->prop->dvbc_constellation.constellation = dvb_c_constellation;
        Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_MODE_PROP_CODE);
        Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DVBC_SYMBOL_RATE_PROP_CODE);
        Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DVBC_CONSTELLATION_PROP_CODE);
        SiTRACE("sr %d bps, constel %d\n", symbol_rate_bps, dvb_c_constellation);
        break;
      }
    case Si2168B_DD_MODE_PROP_MODULATION_MCNS : {
      front_end->demod->prop->dd_mode.bw                       = 8;
      front_end->demod->prop->mcns_symbol_rate.rate            = symbol_rate_bps/1000;
      front_end->demod->prop->mcns_constellation.constellation = dvb_c_constellation;
      Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_MODE_PROP_CODE);
      Si2168B_L1_SetProperty2(front_end->demod, Si2168B_MCNS_SYMBOL_RATE_PROP_CODE);
      Si2168B_L1_SetProperty2(front_end->demod, Si2168B_MCNS_CONSTELLATION_PROP_CODE);
      SiTRACE("sr %d bps, constel %d\n", symbol_rate_bps, dvb_c_constellation);
      break;
    }
      default : /* ATV */   {
        SiTRACE("'%d' standard (%s) is not managed by Si2168B_lock_to_carrier\n", standard, Si2168B_standardName(standard));
        return 0;
        break;
      }
    }

    if (front_end->lockAbort) {
      SiTRACE("Si2168B_L2_lock_to_carrier : lock aborted before tuning, after %d ms.\n", system_time() - lockStartTime );
      return 0;
    }

    /* ALlow using this function without tuning */
    if (freq != 0) {
      startTime = system_time();
      Si2168B_L2_Tune                  (front_end,       freq);
      SiTRACE ("Si2168B_lock_to_carrier 'tune'  took %3d ms\n", system_time() - startTime);
    }

    startTime = system_time();
      Si2168B_L1_DD_RESTART            (front_end->demod);
      SiTRACE   ("Si2168B_lock_to_carrier 'reset' took %3d ms\n", system_time() - startTime);

    /* as we will not lock in less than min_lock_time_ms, wait a while..., but check for a possible 'abort' from the application */
    startTime = system_time();
    while (system_time() - startTime < min_lock_time_ms) {
      if (front_end->lockAbort) {
        SiTRACE("Si2168B_L2_lock_to_carrier : lock aborted before checking lock status, after %d ms.\n", system_time() - lockStartTime );
        return 0;
      }
      /* Adapt here the minimal 'reaction time' of the application*/
      system_wait(20);
    }
  }
  /* testing the relock time upon a reset (activated if freq<0) */
  if (freq < 0) {
    SiTRACE   ("Si2168B_lock_to_carrier 'only_reset'\n");
    Si2168B_L1_DD_RESTART (front_end->demod);
  }

  /* The actual lock check loop */
  while (1) {

    searchDelay = system_time() - front_end->searchStartTime;

    /* Check the status for the current modulation */

    switch (standard) {
      default                                   :
      case Si2168B_DD_MODE_PROP_MODULATION_DVBT  :
      case Si2168B_DD_MODE_PROP_MODULATION_DVBT2 : {
        /* DVB-T/T2 auto detect seek loop, using Si2168B_L1_DD_STATUS                                          */
        /* if DL LOCKED                            : demod is locked on a dd_status->modulation signal        */
        /* if DL NO_LOCK and rsqint_bit5 NO_CHANGE : demod is searching for a DVB-T/T2 signal                 */
        /* if DL NO_LOCK and rsqint_bit5 CHANGE    : demod says this is not a DVB-T/T2 signal (= 'neverlock') */
        return_code = Si2168B_L1_DD_STATUS(front_end->demod, Si2168B_DD_STATUS_CMD_INTACK_CLEAR);
        if (return_code != NO_Si2168B_ERROR) {
          SiTRACE("Si2168B_lock_to_carrier: Si2168B_L1_DD_STATUS error\n");
          SiERROR("Si2168B_lock_to_carrier: Si2168B_L1_DD_STATUS error\n");
          goto exit_lock;
          break;
        }

        if (  front_end->demod->rsp->dd_status.dl  == Si2168B_DD_STATUS_RESPONSE_DL_LOCKED   ) {
          /* Return 1 to signal that the Si2168B is locked on a valid DVB-T/T2 channel */
          SiTRACE("Si2168B_lock_to_carrier: locked on a %s signal\n", Si2168B_standardName(front_end->demod->rsp->dd_status.modulation) );
          lock = 1;
          /* Make sure FEF mode is ON when locked on a T2 channel */
          if   (front_end->demod->rsp->dd_status.modulation == Si2168B_DD_MODE_PROP_MODULATION_DVBT2) {
           #ifdef    INDIRECT_I2C_CONNECTION
            front_end->f_TER_tuner_enable(front_end->callback);
           #else  /* INDIRECT_I2C_CONNECTION */
            Si2168B_L2_Tuner_I2C_Enable(front_end);
           #endif /* INDIRECT_I2C_CONNECTION */
            Si2168B_L2_TER_FEF(front_end, 1);
           #ifdef    INDIRECT_I2C_CONNECTION
            front_end->f_TER_tuner_disable(front_end->callback);
           #else  /* INDIRECT_I2C_CONNECTION */
            Si2168B_L2_Tuner_I2C_Disable(front_end);
           #endif /* INDIRECT_I2C_CONNECTION */
          }
          goto exit_lock;
        } else {
          if (  front_end->demod->rsp->dd_status.rsqint_bit5 == Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT5_CHANGED ) {
          /* Return 0 if firmware signals 'no DVB-T/T2 channel' */
          SiTRACE ("'no DVB-T/T2 channel': not locked after %3d ms\n", searchDelay);
          goto exit_lock;
          }
        }
        break;
      }
      case Si2168B_DD_MODE_PROP_MODULATION_DVBC  : {
        return_code = Si2168B_L1_DD_STATUS(front_end->demod, Si2168B_DD_STATUS_CMD_INTACK_CLEAR);

        if (return_code != NO_Si2168B_ERROR) {
          SiTRACE("Si2168B_lock_to_carrier: Si2168B_L1_DD_STATUS error\n");
          SiERROR("Si2168B_lock_to_carrier: Si2168B_L1_DD_STATUS error\n");
          goto exit_lock;
          break;
        }

        if ( (front_end->demod->rsp->dd_status.dl    == Si2168B_DD_STATUS_RESPONSE_DL_LOCKED     ) ) {
          /* Return 1 to signal that the Si2168B is locked on a valid SAT channel */
          SiTRACE("%s lock\n", Si2168B_standardName(front_end->demod->rsp->dd_status.modulation));
          lock = 1;
          goto exit_lock;
        }
        break;
      }
      case Si2168B_DD_MODE_PROP_MODULATION_MCNS  : {
        return_code = Si2168B_L1_DD_STATUS(front_end->demod, Si2168B_DD_STATUS_CMD_INTACK_CLEAR);

        if (return_code != NO_Si2168B_ERROR) {
          SiTRACE("Si2168B_lock_to_carrier: Si2168B_L1_DD_STATUS error\n");
          SiERROR("Si2168B_lock_to_carrier: Si2168B_L1_DD_STATUS error\n");
          goto exit_lock;
          break;
        }

        if ( (front_end->demod->rsp->dd_status.dl    == Si2168B_DD_STATUS_RESPONSE_DL_LOCKED     ) ) {
          /* Return 1 to signal that the Si2168B is locked on a valid SAT channel */
          SiTRACE("%s lock\n", Si2168B_standardName(front_end->demod->rsp->dd_status.modulation));
          lock = 1;
          goto exit_lock;
        }
        break;
      }
    }

    /* timeout management (this should never happen if timeout values are correctly set) */
    searchDelay = system_time() - front_end->searchStartTime;
    if (searchDelay >= max_lock_time_ms) {
      SiTRACE ("Si2168B_lock_to_carrier timeout(%d) after %d ms\n", max_lock_time_ms, searchDelay);
      goto exit_lock;
      break;
    }

    if (front_end->handshakeUsed == 1) {
      handshakeDelay = system_time() - lockStartTime;
      if (handshakeDelay >= front_end->handshakePeriod_ms) {
        SiTRACE ("lock_to_carrier_handshake : handshake after %5d ms (at %10d). (search delay %6d ms)\n\n", handshakeDelay, freq, searchDelay);
        front_end->handshakeOn = 1;
        /* The application will check handshakeStart_ms to know whether the lock is complete or not */
        return searchDelay;
      } else {
        SiTRACE ("lock_to_carrier_handshake : no handshake yet. (handshake delay %6d ms, search delay %6d ms)\n", handshakeDelay, searchDelay);
      }
    }

    if (front_end->lockAbort) {
      SiTRACE("Si2168B_L2_lock_to_carrier : lock aborted after %d ms.\n", system_time() - lockStartTime);
      goto exit_lock;
    }

    /* Check status every 10 ms */
    system_wait(5);
  }

  exit_lock:

  front_end->handshakeOn = 0;
  searchDelay = system_time() - front_end->searchStartTime;

  if (lock) {
    Si2168B_L1_DD_BER(front_end->demod, Si2168B_DD_BER_CMD_RST_CLEAR);
    SiTRACE ("Si2168B_lock_to_carrier 'lock'  took %3d ms\n"        , searchDelay);
  } else {
    SiTRACE ("Si2168B_lock_to_carrier at %10d (%s) failed after %d ms\n",freq, Si2168B_standardName(front_end->demod->rsp->dd_status.modulation), searchDelay);
  }

  return lock;
}
/************************************************************************************************************************
  Si2168B_L2_Tune function
  Use:        tuner current frequency retrieval function
              Used to retrieve the current RF from the tuner's driver.
  Porting:    Replace the internal TUNER function calls by the final tuner's corresponding calls
  Comments:   If the tuner is connected via the demodulator's I2C switch, enabling/disabling the i2c_passthru is required before/after tuning.
  Behavior:   This function closes the Si2168B's I2C switch then tunes and finally reopens the I2C switch
  Parameter:  *front_end, the front-end handle
  Parameter:  rf, the frequency to tune at
  Returns:    rf
************************************************************************************************************************/
int  Si2168B_L2_Tune               (Si2168B_L2_Context *front_end, int rf)
{
#ifdef    TUNERTER_API
  #define Si2168B_USING_SILABS_TER_TUNER
#else  /* TUNERTER_API */
  #ifdef    SILABS_TER_TUNER_API
    #define Si2168B_USING_SILABS_TER_TUNER
  #endif /* SILABS_TER_TUNER_API */
#endif /* TUNERTER_API */

#ifdef    Si2168B_USING_SILABS_TER_TUNER
    char bw;
    char modulation;
#endif /* Si2168B_USING_SILABS_TER_TUNER */

    SiTRACE("Si2168B_L2_Tune at %d\n",rf);

 #ifdef    INDIRECT_I2C_CONNECTION
  /*  I2C connection will be done later on, depending on the media */
 #else  /* INDIRECT_I2C_CONNECTION */
    Si2168B_L2_Tuner_I2C_Enable(front_end);
 #endif /* INDIRECT_I2C_CONNECTION */

    if (front_end->demod->media == Si2168B_TERRESTRIAL) {
 #ifdef    INDIRECT_I2C_CONNECTION
      front_end->f_TER_tuner_enable(front_end->callback);
 #endif /* INDIRECT_I2C_CONNECTION */
      Si2168B_L2_TER_FEF (front_end, 0);
#ifdef    Si2168B_USING_SILABS_TER_TUNER
      if (front_end->demod->prop->dd_mode.modulation == Si2168B_DD_MODE_PROP_MODULATION_DVBC ) {
        modulation = L1_RF_TER_TUNER_MODULATION_DVBC;
        bw         = 8;
      } else {
        modulation = L1_RF_TER_TUNER_MODULATION_DVBT;
        switch (front_end->demod->prop->dd_mode.bw) {

//Edit by ITE
/*
          case Si2168B_DD_MODE_PROP_BW_BW_1D7MHZ   :  bw = BW_1P7MHZ; break;
          case Si2168B_DD_MODE_PROP_BW_BW_5MHZ     :  bw = BW_6MHZ  ; break;
          case Si2168B_DD_MODE_PROP_BW_BW_6MHZ     :  bw = BW_6MHZ  ; break;
          case Si2168B_DD_MODE_PROP_BW_BW_7MHZ     :  bw = BW_7MHZ  ; break;
          case Si2168B_DD_MODE_PROP_BW_BW_8MHZ     :  bw = BW_8MHZ  ; break;
*/
          case Si2168B_DD_MODE_PROP_BW_BW_1D7MHZ   :  bw = 9; break;
          case Si2168B_DD_MODE_PROP_BW_BW_5MHZ     :  bw = 6  ; break;
          case Si2168B_DD_MODE_PROP_BW_BW_6MHZ     :  bw = 6  ; break;
          case Si2168B_DD_MODE_PROP_BW_BW_7MHZ     :  bw = 7  ; break;
          case Si2168B_DD_MODE_PROP_BW_BW_8MHZ     :  bw = 8  ; break;
          default: {
            SiTRACE("Si2168B_L2_Tune: Invalid dd_mode.bw (%d)\n", front_end->demod->prop->dd_mode.bw);
            SiERROR("Si2168B_L2_Tune: Invalid dd_mode.bw\n");
            bw = 8; break;
          }
        }
      }
#endif /* Si2168B_USING_SILABS_TER_TUNER */
      L1_RF_TER_TUNER_Tune       (front_end->tuner_ter , rf);
      /* Activate FEF management in all cases where the signal can be DVB-T2 */
      if (   (front_end->demod->prop->dd_mode.modulation  == Si2168B_DD_MODE_PROP_MODULATION_DVBT2)
          | ((front_end->demod->prop->dd_mode.modulation  == Si2168B_DD_MODE_PROP_MODULATION_AUTO_DETECT)
            &(front_end->demod->prop->dd_mode.auto_detect == Si2168B_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2))
         ) { Si2168B_L2_TER_FEF (front_end, 1); }
 #ifdef    INDIRECT_I2C_CONNECTION
      front_end->f_TER_tuner_disable(front_end->callback);
 #endif /* INDIRECT_I2C_CONNECTION */
    }


   #ifdef    INDIRECT_I2C_CONNECTION
   #else  /* INDIRECT_I2C_CONNECTION */
    Si2168B_L2_Tuner_I2C_Disable(front_end);
   #endif /* INDIRECT_I2C_CONNECTION */

    return rf;
}
/************************************************************************************************************************
  Si2168B_L2_Get_RF function
  Use:        tuner current frequency retrieval function
              Used to retrieve the current RF from the tuner's driver.
  Porting:    Replace the internal TUNER function calls by the final tuner's corresponding calls
  Behavior:   This function does not need to activate the Si2168B's I2C switch, as the required value is part of the tuner's structure
  Parameter:  *front_end, the front-end handle
************************************************************************************************************************/
int  Si2168B_L2_Get_RF             (Si2168B_L2_Context *front_end) {

  if (front_end->demod->media == Si2168B_TERRESTRIAL) {
#ifdef    TUNERTER_API
  #ifdef   TUNERTER_SILABS_API
    return (int)L1_RF_TER_TUNER_Get_RF (front_end->tuner_ter);
  #else  /* TUNERTER_SILABS_API */
    return front_end->tuner_ter->cmd->tuner_tune_freq.freq;
  #endif /* TUNERTER_SILABS_API */
#else  /* TUNERTER_API */
    return (int)L1_RF_TER_TUNER_Get_RF (front_end->tuner_ter);
#endif /* TUNERTER_API */
  }


  return 0;
}
/************************************************************************************************************************
  Si2168B_L2_TER_Clock function
  Use:        Terrestrial clock configuration function
              Used to set the terrestrial clock source, input pin and frequency
  Parameter:  clock_source:  where the clock signal comes from.
              possible sources:
                Si2168B_Xtal_clock,
                Si2168B_TER_Tuner_clock,
                Si2168B_SAT_Tuner_clock
  Parameter:  clock_input:   on which pin is the clock received.
              possible inputs:
                44 for Si2168B_START_CLK_CMD_CLK_MODE_CLK_CLKIO    (pin 44 for 'single' parts)
                33 for Si2168B_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN  (pin 33 for 'single' parts)
                32 for Si2168B_START_CLK_CMD_CLK_MODE_XTAL         (Xtal connected on pins 32/33 for 'single' parts, driven by the Si264)
  Parameter:  clock_freq:   clock frequency
              possible frequencies:
                Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_16MHZ
                Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ
                Si2168B_POWER_UP_CMD_CLOCK_FREQ_CLK_27MHZ
  Parameter:  clock_control:   how the TER clock must be controlled
              possible modes:
                Si2168B_CLOCK_ALWAYS_ON                             turn clock ON then never switch it off, used when the clock is provided to another part
                Si2168B_CLOCK_ALWAYS_OFF                            never switch it on, used when the clock is not going anywhere
                Si2168B_CLOCK_MANAGED                               control clock state as before
 ***********************************************************************************************************************/
int  Si2168B_L2_TER_Clock          (Si2168B_L2_Context *front_end, int clock_source, int clock_input, int clock_freq, int clock_control)
{
  front_end->demod->tuner_ter_clock_source = clock_source;
  switch (clock_input) {
    case 44: {
      front_end->demod->tuner_ter_clock_input  = Si2168B_START_CLK_CMD_CLK_MODE_CLK_CLKIO;
      break;
    }
    case 33: {
      front_end->demod->tuner_ter_clock_input  = Si2168B_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN;
      break;
    }
    default:
    case 32: {
      front_end->demod->tuner_ter_clock_input  = Si2168B_START_CLK_CMD_CLK_MODE_XTAL;
      break;
    }
  }
  front_end->demod->tuner_ter_clock_freq   = clock_freq;
  front_end->demod->tuner_ter_clock_control= clock_control;
  return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  Si2168B_L2_TER_AGC function
  Use:        Terrestrial AGC configuration function
              Used to set the terrestrial AGC source, input pin and frequency
  Parameter:  agc1_mode:  where the AGC 1 PWM comes from.
              possible modes:
                0x0: Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_NOT_USED,
                0xA: Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_A,
                0xB: Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_B,
                0xC: Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_C,
                0xD: Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_D,
  Parameter:  agc1_inversion: 0/1 to indicate if AGC 1 is inverted or not (depends on the TER tuner and HW design)
  Parameter:  agc2_mode:  where the AGC 2 PWM comes from.
              possible modes:
                0x0: Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_NOT_USED,
                0xA: Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_A,
                0xB: Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_B,
                0xC: Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_C,
                0xD: Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_D,
  Parameter:  agc2_inversion: 0/1 to indicate if AGC 2 is inverted or not (depends on the TER tuner and HW design)
  Returns:    0 if no error
 ***********************************************************************************************************************/
int  Si2168B_L2_TER_AGC            (Si2168B_L2_Context *front_end, int agc1_mode, int agc1_inversion, int agc2_mode, int agc2_inversion)
{
  switch (agc1_mode) {
    default:
    case 0x0: { front_end->demod->cmd->dd_ext_agc_ter.agc_1_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_NOT_USED; break; }
    case 0xA: { front_end->demod->cmd->dd_ext_agc_ter.agc_1_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_A    ; break; }
    case 0xB: { front_end->demod->cmd->dd_ext_agc_ter.agc_1_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_B    ; break; }
    case 0xC: { front_end->demod->cmd->dd_ext_agc_ter.agc_1_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_C    ; break; }
    case 0xD: { front_end->demod->cmd->dd_ext_agc_ter.agc_1_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_D    ; break; }
  }

  if (agc1_inversion == 0) {
                front_end->demod->cmd->dd_ext_agc_ter.agc_1_inv   = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_INV_NOT_INVERTED;}
   else {       front_end->demod->cmd->dd_ext_agc_ter.agc_1_inv   = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_INV_INVERTED;    }

  switch (agc2_mode) {
    default:
    case 0x0: { front_end->demod->cmd->dd_ext_agc_ter.agc_2_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_NOT_USED; break; }
    case 0xA: { front_end->demod->cmd->dd_ext_agc_ter.agc_2_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_A    ; break; }
    case 0xB: { front_end->demod->cmd->dd_ext_agc_ter.agc_2_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_B    ; break; }
    case 0xC: { front_end->demod->cmd->dd_ext_agc_ter.agc_2_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_C    ; break; }
    case 0xD: { front_end->demod->cmd->dd_ext_agc_ter.agc_2_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_D    ; break; }
  }
  if (agc2_inversion == 0) {
                front_end->demod->cmd->dd_ext_agc_ter.agc_2_inv   = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_INV_NOT_INVERTED;}
  else {        front_end->demod->cmd->dd_ext_agc_ter.agc_2_inv   = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_INV_INVERTED;}

  if ( (agc1_mode == 0) & (agc2_mode == 0) ) {
    front_end->demod->tuner_ter_agc_control = 0;
  } else {
    front_end->demod->tuner_ter_agc_control = 1;
  }

  return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  Si2168B_L2_TER_FEF_CONFIG function
  Use:        TER tuner FEF pin selection function
              Used to select the FEF pin connected to the terrestrial tuner
  Parameter:  *front_end, the front-end handle
  Parameter:  fef_mode, a flag controlling the FEF mode between SLOW_NORMAL_AGC(0), FREEZE_PIN(1)' and SLOW_INITIAL_AGC(2)
  Parameter:  fef_pin: where the FEF signal comes from.
              possible values:
                0x0: Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_NOT_USED
                0xA: Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_A
                0xB: Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_B
                0xC: Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_C
                0xD: Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_D
  Parameter:  fef_level, a flag controlling the FEF signal level when active between 'low'(0) and 'high'(1)
  Returns:    1
************************************************************************************************************************/
int   Si2168B_L2_TER_FEF_CONFIG    (Si2168B_L2_Context   *front_end, int fef_mode, int fef_pin, int fef_level)
{
  switch (fef_mode) {
    default :
    case   0: { front_end->demod->fef_mode = Si2168B_FEF_MODE_SLOW_NORMAL_AGC ; front_end->demod->fef_pin = 0x0    ; front_end->demod->fef_level = 0        ; break; }
    case   1: { front_end->demod->fef_mode = Si2168B_FEF_MODE_FREEZE_PIN      ; front_end->demod->fef_pin = fef_pin; front_end->demod->fef_level = fef_level; break; }
    case   2: { front_end->demod->fef_mode = Si2168B_FEF_MODE_SLOW_INITIAL_AGC; front_end->demod->fef_pin = 0x0    ; front_end->demod->fef_level = 0        ; break; }
  }
  switch (front_end->demod->fef_pin) {
    default  :
    case 0x0: { front_end->demod->cmd->dvbt2_fef.fef_tuner_flag  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_NOT_USED; break; }
    case 0xA: { front_end->demod->cmd->dvbt2_fef.fef_tuner_flag  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_A    ; break; }
    case 0xB: { front_end->demod->cmd->dvbt2_fef.fef_tuner_flag  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_B    ; break; }
    case 0xC: { front_end->demod->cmd->dvbt2_fef.fef_tuner_flag  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_C    ; break; }
    case 0xD: { front_end->demod->cmd->dvbt2_fef.fef_tuner_flag  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_D    ; break; }
  }
  if (front_end->demod->fef_level == 0) {
                front_end->demod->cmd->dvbt2_fef.fef_tuner_flag_inv  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_FEF_LOW ; }
   else {       front_end->demod->cmd->dvbt2_fef.fef_tuner_flag_inv  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_FEF_HIGH; }

  return NO_Si2168B_ERROR;
}
/************************************************************************************************************************
  Si2168B_L2_TER_FEF function
  Use:        TER tuner FEF activation function
              Used to enable/disable the FEF mode in the terrestrial tuner
  Comments:   If the tuner is connected via the demodulator's I2C switch, enabling/disabling the i2c_passthru is required before/after tuning.
  Parameter:  *front_end, the front-end handle
  Parameter:  fef, a flag controlling the selection between FEF 'off'(0) and FEF 'on'(1)
  Returns:    1
************************************************************************************************************************/
int  Si2168B_L2_TER_FEF            (Si2168B_L2_Context *front_end, int fef)
{
  front_end = front_end; /* To avoid compiler warning if not used */
  SiTRACE("Si2168B_L2_TER_FEF %d\n",fef);

  #ifdef    L1_RF_TER_TUNER_DTV_AGC_AUTO_FREEZE
    SiTRACE("TER tuner: AUTO_AGC_FREEZE\n");
    L1_RF_TER_TUNER_DTV_AGC_AUTO_FREEZE(front_end->tuner_ter,fef);
  #endif /* L1_RF_TER_TUNER_DTV_AGC_AUTO_FREEZE */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN
  if (front_end->demod->fef_mode == Si2168B_FEF_MODE_FREEZE_PIN      ) {
    SiTRACE("FEF mode Si2168B_FEF_MODE_FREEZE_PIN\n");
    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN(front_end->tuner_ter, fef);
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP
  if (front_end->demod->fef_mode == Si2168B_FEF_MODE_SLOW_INITIAL_AGC) {
    SiTRACE("FEF mode Si2168B_FEF_MODE_SLOW_INITIAL_AGC (AGC slowed down after tuning)\n");
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP
  if (front_end->demod->fef_mode == Si2168B_FEF_MODE_SLOW_NORMAL_AGC ) {
    SiTRACE("FEF mode Si2168B_FEF_MODE_SLOW_NORMAL_AGC: AGC slowed down\n");
    L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC(front_end->tuner_ter, fef);
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP */
  SiTRACE("Si2168B_L2_TER_FEF done\n");
  return 1;
}
/************************************************************************************************************************
  Si2168B_L2_TER_FEF_SETUP function
  Use:        TER tuner LPF setting function
              Used to configure the FEF mode in the terrestrial tuner
  Comments:   If the tuner is connected via the demodulator's I2C switch, enabling/disabling the i2c_passthru is required before/after tuning.
  Behavior:   This function closes the Si2168B's I2C switch then sets the TER FEF mode and finally reopens the I2C switch
  Parameter:  *front_end, the front-end handle
  Parameter:  fef, a flag controlling the selection between FEF 'off'(0) and FEF 'on'(1)
  Returns:    1
************************************************************************************************************************/
int  Si2168B_L2_TER_FEF_SETUP      (Si2168B_L2_Context *front_end, int fef)
{
  SiTRACE("Si2168B_L2_TER_FEF_SETUP %d\n",fef);
#if  0 /* Only called from switch_to_standard, while tuners are alreay connected */
 #ifdef    INDIRECT_I2C_CONNECTION
  front_end->f_TER_tuner_enable(front_end->callback);
 #else  /* INDIRECT_I2C_CONNECTION */
  Si2168B_L2_Tuner_I2C_Enable(front_end);
 #endif /* INDIRECT_I2C_CONNECTION */
#endif /* Only called from switch_to_standard, while tuners are alreay connected */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP
  if (front_end->demod->fef_mode == Si2168B_FEF_MODE_FREEZE_PIN      ) {
    SiTRACE("FEF mode Si2168B_FEF_MODE_FREEZE_PIN\n");  
          
//Edit by ITE          
    //SiLabs_TER_Tuner_FEF_FREEZE_PIN_SETUP(front_end->tuner_ter, front_end->demod->fef_level);
    L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP(front_end->tuner_ter, fef);
}
  #endif /* L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP
  if (front_end->demod->fef_mode == Si2168B_FEF_MODE_SLOW_INITIAL_AGC) {
    SiTRACE("FEF mode Si2168B_FEF_MODE_SLOW_INITIAL_AGC (AGC slowed down after tuning)\n");
    L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP(front_end->tuner_ter, fef);
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP */

  #ifdef    L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP
  if (front_end->demod->fef_mode == Si2168B_FEF_MODE_SLOW_NORMAL_AGC ) {
    SiTRACE("FEF mode Si2168B_FEF_MODE_SLOW_NORMAL_AGC: AGC slowed down\n");
    L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP(front_end->tuner_ter, fef);
  }
  #endif /* L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC */

  Si2168B_L2_TER_FEF(front_end, fef);
#if  0 /* Only called from switch_to_standard, while tuners are alreay connected */
 #ifdef    INDIRECT_I2C_CONNECTION
  front_end->f_TER_tuner_disable(front_end->callback);
 #else  /* INDIRECT_I2C_CONNECTION */
  Si2168B_L2_Tuner_I2C_Disable(front_end);
 #endif /* INDIRECT_I2C_CONNECTION */
#endif /* Only called from switch_to_standard, while tuners are alreay connected */

  SiTRACE("Si2168B_L2_TER_FEF_SETUP done\n");
  return 1;
}
/************************************************************************************************************************
  NAME: Si2168B_L2_Trace_Scan_Status
  DESCRIPTION: traces the scan_status
  Parameter:  Pointer to Si2168B Context
  Returns:    void
************************************************************************************************************************/
const char *Si2168B_L2_Trace_Scan_Status  (int scan_status)
{
    switch (scan_status) {
      case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_ANALOG_CHANNEL_FOUND  : { return "ANALOG  CHANNEL_FOUND"; break; }
      case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_DIGITAL_CHANNEL_FOUND : { return "DIGITAL CHANNEL_FOUND"; break; }
      case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_DEBUG                 : { return "DEBUG"                ; break; }
      case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_ERROR                 : { return "ERROR"                ; break; }
      case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_ENDED                 : { return "ENDED"                ; break; }
      case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_IDLE                  : { return "IDLE"                 ; break; }
      case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_SEARCHING             : { return "SEARCHING"            ; break; }
      case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_TUNE_REQUEST          : { return "TUNE_REQUEST"         ; break; }
      default                                                             : { return "Unknown!"             ; break; }
    }
}
/************************************************************************************************************************
  NAME: Si2168B_L2_Set_Invert_Spectrum
  DESCRIPTION: return the required invert_spectrum value depending on the settings:
              front_end->demod->media
              front_end->satellite_spectrum_inversion
              front_end->lnb_type
              front_end->unicable_spectrum_inversion

  Parameter:  Pointer to Si2168B Context
  Returns:    the required invert_spectrum value
************************************************************************************************************************/
unsigned char Si2168B_L2_Set_Invert_Spectrum (Si2168B_L2_Context *front_end)
{
  unsigned char inversion;
  if (front_end->demod->media == Si2168B_TERRESTRIAL) {
    inversion = Si2168B_DD_MODE_PROP_INVERT_SPECTRUM_NORMAL;
  }
  return inversion;
}
/************************************************************************************************************************
  NAME: Si2168B_L2_Channel_Seek_Init
  DESCRIPTION: logs the seek parameters in the context structure
  Programming Guide Reference:    Flowchart TBD (Channel Scan flowchart)
              standards where the parameter is used:     T   T2    C MCNS   C2    S   S2   DSS  ATV
  Parameter:  front_end Pointer to Si2168B Context        x    x    x    x    x    x    x     x    x
  Parameter:  rangeMin     starting Frequency Hz         x    x    x    x    x    x    x     x    x
  Parameter:  rangeMax     ending Frequency Hz           x    x    x    x    x    x    x     x    x
  Parameter:  seekBWHz     Signal Bandwidth (for T/T2)   x    x
  Parameter:  seekStepHz   Frequency step in Hz          x    x
  Parameter:  minSRbps     minimum symbol rate in Baud             x    x         x    x     x
  Parameter:  maxSRbps     maximum symbol rate in Baud             x    x         x    x     x
  Parameter:  minRSSIdBm   min RSSI dBm                  x    x                                   x
  Parameter:  maxRSSIdBm   max RSSI dBm                  x    x                                   x
  Parameter:  minSNRHalfdB min SNR 1/2 dB                x    x                                   x
  Parameter:  maxSNRHalfdB max SNR 1/2 dB                x    x                                   x
  Returns:    0 if successful, otherwise an error.
************************************************************************************************************************/
int  Si2168B_L2_Channel_Seek_Init  (Si2168B_L2_Context *front_end, int rangeMin, int rangeMax, int seekBWHz, int seekStepHz, int minSRbps, int maxSRbps, int minRSSIdBm, int maxRSSIdBm, int minSNRHalfdB, int maxSNRHalfdB)
{
  int modulation = 0;
  if (front_end->demod->media == Si2168B_TERRESTRIAL) {
    SiTRACE("media TERRESTRIAL\n");
    front_end->tuneUnitHz   =    1;
  }
  SiTRACE ("blindscan_interaction >> (init  ) Si2168B_L1_SCAN_CTRL( front_end->demod, Si2168B_SCAN_CTRL_CMD_ACTION_ABORT)\n");
  Si2168B_L1_SCAN_CTRL (front_end->demod, Si2168B_SCAN_CTRL_CMD_ACTION_ABORT , 0);
  /* Check detection standard based on dd_mode.modulation and dd_mode.auto_detect */
  SiTRACE("standard %d, dd_mode.modulation %d, dd_mode.auto_detect %d\n",  front_end->standard, front_end->demod->prop->dd_mode.modulation, front_end->demod->prop->dd_mode.auto_detect);
  switch (front_end->demod->prop->dd_mode.modulation) {
    case Si2168B_DD_MODE_PROP_MODULATION_AUTO_DETECT : {
      switch (front_end->demod->prop->dd_mode.auto_detect) {
        case Si2168B_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2     : {
          modulation = Si2168B_DD_MODE_PROP_MODULATION_DVBT2;
          break;
        }
        default : {
          SiTRACE("AUTO DETECT '%d' is not managed by Si2168B_L2_Channel_Seek_Init\n", front_end->demod->prop->dd_mode.auto_detect);
          break;
        }
      }
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC :
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2:
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT : {
      modulation = front_end->demod->prop->dd_mode.modulation;
      break;
    }
    default : {
      SiTRACE("'%d' modulation (%s) is not managed by Si2168B_L2_Channel_Seek_Init\n", front_end->demod->prop->dd_mode.modulation, Si2168B_standardName(front_end->demod->prop->dd_mode.modulation));
      break;
    }
  }
  SiTRACE("Si2168B_L2_Channel_Seek_Init for %s (%d)\n", Si2168B_standardName(modulation), modulation );
  switch (modulation) {
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC :
     /* Forcing BW to 8 MHz for DVB-C */
      seekBWHz = 8000000;
      front_end->demod->prop->dd_mode.modulation  = Si2168B_DD_MODE_PROP_MODULATION_DVBC;
      front_end->demod->prop->dd_mode.auto_detect = Si2168B_DD_MODE_PROP_AUTO_DETECT_NONE;
      front_end->demod->prop->dvbc_constellation.constellation = Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_AUTO;
      Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DVBC_CONSTELLATION_PROP_CODE);
      SiTRACE("DVB-C AFC range %d\n", front_end->demod->prop->dvbc_afc_range.range_khz);
      break;
    case Si2168B_DD_MODE_PROP_MODULATION_MCNS :
     /* Forcing BW to 8 MHz for MCNS */
      seekBWHz = 8000000;
      front_end->demod->prop->dd_mode.modulation  = Si2168B_DD_MODE_PROP_MODULATION_MCNS;
      front_end->demod->prop->dd_mode.auto_detect = Si2168B_DD_MODE_PROP_AUTO_DETECT_NONE;
      front_end->demod->prop->mcns_constellation.constellation = Si2168B_MCNS_CONSTELLATION_PROP_CONSTELLATION_AUTO;
      Si2168B_L1_SetProperty2(front_end->demod, Si2168B_MCNS_CONSTELLATION_PROP_CODE);
      SiTRACE("MCNS AFC range %d\n", front_end->demod->prop->mcns_afc_range.range_khz);
      break;
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2:
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT :
      front_end->demod->prop->dvbt_hierarchy.stream = Si2168B_DVBT_HIERARCHY_PROP_STREAM_HP;
      Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DVBT_HIERARCHY_PROP_CODE);
      SiTRACE("DVB-T AFC range %d\n", front_end->demod->prop->dvbt_afc_range.range_khz);
      front_end->demod->prop->dd_mode.modulation  = Si2168B_DD_MODE_PROP_MODULATION_AUTO_DETECT;
      front_end->demod->prop->dd_mode.auto_detect = Si2168B_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2;
      SiTRACE("DVB-T2 AFC range %d\n", front_end->demod->prop->dvbt2_afc_range.range_khz);
      Si2168B_L1_DVBT2_PLP_SELECT    (front_end->demod, 0, Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_AUTO);
      break;
    default : {
      SiTRACE("'%d' modulation (%s) is not managed by Si2168B_L2_Channel_Seek_Init\n", modulation, Si2168B_standardName(modulation));
      break;
    }
  }

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

  SiTRACE("Si2168B_L2_Channel_Seek_Init with %d to  %d, sawBW %d, minSR %d, maxSR %d\n", front_end->rangeMin, front_end->rangeMax, front_end->seekBWHz, front_end->minSRbps, front_end->maxSRbps);
  SiTRACE("spectrum inversion %d\n",front_end->demod->prop->dd_mode.invert_spectrum );
  
//Edit by ITE  
  //front_end->demod->prop->scan_fmin.scan_fmin                   = (int)((front_end->rangeMin*front_end->tuneUnitHz)/65536);
  //front_end->demod->prop->scan_fmax.scan_fmax                   = (int)((front_end->rangeMax*front_end->tuneUnitHz)/65536);
  front_end->demod->prop->scan_fmin.scan_fmin                   = (int)(front_end->rangeMin * (front_end->tuneUnitHz / 65536));
  front_end->demod->prop->scan_fmax.scan_fmax                   = (int)(front_end->rangeMax * (front_end->tuneUnitHz / 65536));  

  front_end->demod->prop->scan_symb_rate_min.scan_symb_rate_min = front_end->minSRbps/1000;
  front_end->demod->prop->scan_symb_rate_max.scan_symb_rate_max = front_end->maxSRbps/1000;

  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_SCAN_FMIN_PROP_CODE);
  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_SCAN_FMAX_PROP_CODE);
  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_SCAN_SYMB_RATE_MIN_PROP_CODE);
  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_SCAN_SYMB_RATE_MAX_PROP_CODE);



  front_end->demod->prop->scan_ien.buzien           = Si2168B_SCAN_IEN_PROP_BUZIEN_DISABLE ; /* (default 'DISABLE') */
  front_end->demod->prop->scan_ien.reqien           = Si2168B_SCAN_IEN_PROP_REQIEN_ENABLE  ; /* (default 'DISABLE') */
  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_SCAN_IEN_PROP_CODE);

  front_end->demod->prop->scan_int_sense.reqnegen   = Si2168B_SCAN_INT_SENSE_PROP_REQNEGEN_DISABLE ; /* (default 'DISABLE') */
  front_end->demod->prop->scan_int_sense.reqposen   = Si2168B_SCAN_INT_SENSE_PROP_REQPOSEN_ENABLE  ; /* (default 'ENABLE') */
  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_SCAN_INT_SENSE_PROP_CODE);


#ifdef    ALLOW_Si2168B_BLINDSCAN_DEBUG
  front_end->demod->prop->scan_ter_config.scan_debug = 0x0f;
#endif /* ALLOW_Si2168B_BLINDSCAN_DEBUG */
  if (front_end->demod->media == Si2168B_TERRESTRIAL) {
    front_end->demod->prop->scan_ter_config.analog_bw     = Si2168B_SCAN_TER_CONFIG_PROP_ANALOG_BW_8MHZ;
    if ( front_end->rangeMin == front_end->rangeMax ) {
      front_end->demod->prop->scan_ter_config.mode        = Si2168B_SCAN_TER_CONFIG_PROP_MODE_BLIND_LOCK;
      SiTRACE("Blindlock < %8d %8d > < %8d %8d >\n", front_end->demod->prop->scan_fmin.scan_fmin, front_end->demod->prop->scan_fmax.scan_fmax, front_end->demod->prop->scan_symb_rate_min.scan_symb_rate_min, front_end->demod->prop->scan_symb_rate_max.scan_symb_rate_max);
    } else {
      front_end->demod->prop->scan_ter_config.mode        = Si2168B_SCAN_TER_CONFIG_PROP_MODE_BLIND_SCAN;
      SiTRACE("Blindscan < %8d %8d > < %8d %8d >\n", front_end->demod->prop->scan_fmin.scan_fmin, front_end->demod->prop->scan_fmax.scan_fmax, front_end->demod->prop->scan_symb_rate_min.scan_symb_rate_min, front_end->demod->prop->scan_symb_rate_max.scan_symb_rate_max);
    }
    front_end->demod->prop->scan_ter_config.search_analog = Si2168B_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_DISABLE;
    Si2168B_L1_SetProperty2(front_end->demod, Si2168B_SCAN_TER_CONFIG_PROP_CODE);
    if (seekBWHz == 1700000) {
      front_end->demod->prop->dd_mode.bw = Si2168B_DD_MODE_PROP_BW_BW_1D7MHZ;
    } else {
      front_end->demod->prop->dd_mode.bw = seekBWHz/1000000;
    }
  }
  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_MODE_PROP_CODE);

  Si2168B_L1_DD_RESTART  (front_end->demod);

  Si2168B_L1_SCAN_STATUS (front_end->demod, Si2168B_SCAN_STATUS_CMD_INTACK_OK);
  SiTRACE("blindscan_status leaving Seek_Init %s\n", Si2168B_L2_Trace_Scan_Status(front_end->demod->rsp->scan_status.scan_status) );
  /* Preparing the next call to Si2168B_L1_SCAN_CTRL which needs to be a 'START'*/
  front_end->demod->cmd->scan_ctrl.action = Si2168B_SCAN_CTRL_CMD_ACTION_START;
  front_end->handshakeOn       = 0;
  SiTRACE("blindscan_handshake : Seek_Next will return every ~%d ms\n", front_end->handshakePeriod_ms );
  return 0;
}
/************************************************************************************************************************
  NAME: Si2168B_L2_Channel_Seek_Next
  DESCRIPTION: Looks for the next channel, starting from the last detected channel
  Programming Guide Reference:    Flowchart TBD (Channel Scan flowchart)

  Parameter:  Pointer to Si2168B Context
  Returns:    1 if channel is found, 0 otherwise (either abort or end of range)
              Any other value represents the time spent searching (if front_end->handshakeUsed == 1)
************************************************************************************************************************/
int  Si2168B_L2_Channel_Seek_Next  (Si2168B_L2_Context *front_end
                                           , int *standard
                                           , int *freq
                                           , int *bandwidth_Hz
                                           , int *stream
                                           , unsigned int *symbol_rate_bps
                                           , int *constellation
                                           , int *num_plp
                                           , int *T2_base_lite
                                           )
{
    int           return_code;
    int           seek_freq;
    int           seek_freq_kHz;
    int           channelIncrement;
    int           seekStartTime;    /* seekStartTime    is used to trace the time spent in Si2168B_L2_Channel_Seek_Next and is only set when entering the function                            */
    int           buzyStartTime;    /* buzyStartTime    is used to trace the time spent waiting for scan_status.buz to be different from 'BUZY'                                              */
    int           timeoutDelay;
    int           handshakeDelay;
    int           searchDelay = 0; 
    int           max_lock_time_ms;
    int           min_lock_time_ms;
    int           max_decision_time_ms;
    int           blind_mode;
    int           skip_resume;
    int           start_resume;
    int           previous_scan_status;
    unsigned char jump_to_next_channel;
    L1_Si2168B_Context *api;

    api = front_end->demod;

    blind_mode   = 0;
    start_resume = 0; /* To avoid code checker warning */

    /* Clear all return values which may not be used depending on the standard */
    *bandwidth_Hz    = 0;
    *stream          = 0;
    *symbol_rate_bps = 0;
    *constellation   = 0;
     *num_plp        = 0;
     *T2_base_lite   = 0;

    if (front_end->seekAbort) {
      SiTRACE("Si2168B_L2_Channel_Seek_Next : previous run aborted. Please Si2168B_L2_Channel_Seek_Init to perform a new search.\n");
      return 0;
    }

    SiTRACE("front_end->demod->standard %d (%s)\n",front_end->demod->standard, Si2168B_standardName(front_end->demod->standard) );

    /* Setting max and max lock times and blind_mode flag */
    switch ( front_end->demod->standard )
    {
      /* For T/T2 detection, use the max value between Si2168B_DVBT_MAX_LOCK_TIME and Si2168B_DVBT2_MAX_LOCK_TIME */
      /* With Si2168B-A, it's Si2168B_DVBT2_MAX_LOCK_TIME                                                         */
      /* This value will be refined as soon as the standard is known, i.e. when PCL = 1                         */
      case Si2168B_DD_MODE_PROP_MODULATION_DVBT2:
      case Si2168B_DD_MODE_PROP_MODULATION_DVBT: {
        blind_mode = 0;
        max_lock_time_ms = Si2168B_DVBT_MAX_LOCK_TIME;
          max_lock_time_ms = Si2168B_DVBT2_MAX_LOCK_TIME;
        min_lock_time_ms = Si2168B_DVBT_MIN_LOCK_TIME;
        break;
      }
      case Si2168B_DD_MODE_PROP_MODULATION_DVBC : {
        blind_mode = 1;
        max_lock_time_ms = Si2168B_DVBC_MAX_SEARCH_TIME;
        min_lock_time_ms = Si2168B_DVBC_MIN_LOCK_TIME;
        break;
      }
      default                                  : {
        SiTRACE("'%d' standard (%s) is not managed by Si2168B_L2_Channel_Seek_Next\n", front_end->demod->prop->dd_mode.modulation, Si2168B_standardName(front_end->demod->prop->dd_mode.modulation));
        front_end->seekAbort = 1;
        return 0;
        break;
      }
    }
    SiTRACE("blindscan : max_lock_time_ms %d\n", max_lock_time_ms);

    seekStartTime = system_time();

    if (front_end->handshakeUsed == 0) {
      start_resume = 1;
      front_end->searchStartTime = seekStartTime;
    }
    if (front_end->handshakeUsed == 1) {
      min_lock_time_ms = 0;
      /* Recalled after handshaking    */
      if (front_end->handshakeOn == 1) {
        /* Skip tuner and demod settings */
        start_resume = 0;
        SiTRACE("blindscan_handshake : recalled after   handshake. Skipping tuner and demod settings\n");
      }
      /* When recalled after a lock */
      if (front_end->handshakeOn == 0) {
        /* Allow tuner and demod settings */
        start_resume = 1;
        if (blind_mode == 1) { /* DVB-C / DVB-S / DVB-S2 / MCNS */
          if (front_end->demod->cmd->scan_ctrl.action == Si2168B_SCAN_CTRL_CMD_ACTION_START) {
            SiTRACE("blindscan_handshake : no handshake : starting.\n");
          } else {
            SiTRACE("blindscan_handshake : no handshake : resuming.\n");
          }
        }
        front_end->searchStartTime = seekStartTime;
      }
    }
    front_end->handshakeStart_ms = seekStartTime;

    if (start_resume == 1) {
      /* Enabling FEF control for T/T2 */
      switch ( front_end->demod->standard )
      {
        case Si2168B_DD_MODE_PROP_MODULATION_DVBT2 :
        case Si2168B_DD_MODE_PROP_MODULATION_DVBT: {
         #ifdef    INDIRECT_I2C_CONNECTION
          front_end->f_TER_tuner_enable(front_end->callback);
         #else  /* INDIRECT_I2C_CONNECTION */
          Si2168B_L2_Tuner_I2C_Enable(front_end);
         #endif /* INDIRECT_I2C_CONNECTION */
          Si2168B_L2_TER_FEF (front_end,1);
        #ifdef    INDIRECT_I2C_CONNECTION
          front_end->f_TER_tuner_disable(front_end->callback);
        #else  /* INDIRECT_I2C_CONNECTION */
          Si2168B_L2_Tuner_I2C_Disable(front_end);
        #endif /* INDIRECT_I2C_CONNECTION */
          break;
        }
        default                                  : {
          break;
        }
      }
    }

    max_decision_time_ms = max_lock_time_ms;

    /* Select TER channel increment (this value will only be used for 'TER' scanning) */
    channelIncrement = front_end->seekStepHz;

    /* Start Seeking */
    SiTRACE("Si2168B_L2_Channel_Seek_Next front_end->rangeMin %10d,front_end->rangeMax %10d blind_mode %d\n", front_end->rangeMin,front_end->rangeMax, blind_mode);

    seek_freq = front_end->rangeMin;

  if (blind_mode == 0) { /* DVB-T / DVB-T2 */
    while ( seek_freq <= front_end->rangeMax )
    {
      if (start_resume) {
        /* Call the Si2168B_L2_Tune command to tune the frequency */
        if (Si2168B_L2_Tune (front_end, seek_freq )!= seek_freq) {
          /* Manage possible tune error */
          SiTRACE("Si2168B_L2_Channel_Seek_Next Tune error at %d, aborting (skipped)\n", seek_freq);
          front_end->seekAbort = 1;
          return 0;
        }

        Si2168B_L1_DD_RESTART        (api);
        /* In non-blind mode, the timeout reference is the last DD_RESTART */
        front_end->timeoutStartTime = system_time();
        /* as we will not lock in less than min_lock_time_ms, wait a while... */
        system_wait(min_lock_time_ms);
      }

      jump_to_next_channel = 0;

      while (!jump_to_next_channel) {

        if ((front_end->demod->standard == Si2168B_DD_MODE_PROP_MODULATION_DVBT) | (front_end->demod->standard == Si2168B_DD_MODE_PROP_MODULATION_DVBT2) ) {

          return_code = Si2168B_L1_DD_STATUS(api, Si2168B_DD_STATUS_CMD_INTACK_CLEAR);
          if (return_code != NO_Si2168B_ERROR) {
            SiTRACE("Si2168B_L2_Channel_Seek_Next: Si2168B_L1_DD_STATUS error at %d, aborting\n", seek_freq);
            front_end->handshakeOn = 0;
            front_end->seekAbort   = 1;
            return 0;
          }

          searchDelay = system_time() - front_end->searchStartTime;

          if ( (front_end->demod->rsp->dd_status.dl  == Si2168B_DD_STATUS_RESPONSE_DL_NO_LOCK  ) & ( front_end->demod->rsp->dd_status.rsqstat_bit5   == Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT5_NO_CHANGE ) ) {
            /* Check PCL to refine the max_lock_time_ms value if the standard has been detected */
            if   (front_end->demod->rsp->dd_status.pcl        == Si2168B_DD_STATUS_RESPONSE_PCL_LOCKED) {
              if (front_end->demod->rsp->dd_status.modulation == Si2168B_DD_STATUS_RESPONSE_MODULATION_DVBT ) { max_lock_time_ms = Si2168B_DVBT_MAX_LOCK_TIME ;}
            }
          }
          if ( (front_end->demod->rsp->dd_status.dl  == Si2168B_DD_STATUS_RESPONSE_DL_NO_LOCK  ) & ( front_end->demod->rsp->dd_status.rsqstat_bit5   == Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT5_CHANGED   ) ) {
            SiTRACE ("NO DVBT/T2. Jumping from  %d after %3d ms\n", seek_freq, searchDelay);
            if(seek_freq==front_end->rangeMax) {front_end->rangeMin=seek_freq;}
            seek_freq = seek_freq + channelIncrement;
            jump_to_next_channel = 1;
            start_resume         = 1;
            break;
          }
          if ( (front_end->demod->rsp->dd_status.dl  == Si2168B_DD_STATUS_RESPONSE_DL_LOCKED   ) & ( front_end->demod->rsp->dd_status.rsqstat_bit5   == Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT5_NO_CHANGE ) ) {
            *standard     = front_end->demod->rsp->dd_status.modulation;
            if (*standard == Si2168B_DD_STATUS_RESPONSE_MODULATION_DVBT ) {
              Si2168B_L1_DVBT_STATUS (api, Si2168B_DVBT_STATUS_CMD_INTACK_CLEAR);
              front_end->detected_rf = seek_freq + front_end->demod->rsp->dvbt_status.afc_freq*1000;
              if (front_end->demod->rsp->dvbt_status.hierarchy == Si2168B_DVBT_STATUS_RESPONSE_HIERARCHY_NONE) {
                *stream       = Si2168B_DVBT_HIERARCHY_PROP_STREAM_HP;
              } else {
                *stream       = front_end->demod->prop->dvbt_hierarchy.stream;
              }
              *bandwidth_Hz = front_end->demod->prop->dd_mode.bw*1000000;
              *freq         = front_end->detected_rf;
              SiTRACE ("DVB-T  lock at %d after %3d ms\n", (front_end->detected_rf)/1000, searchDelay);
            }
            if (*standard == Si2168B_DD_STATUS_RESPONSE_MODULATION_DVBT2) {
           #ifdef    INDIRECT_I2C_CONNECTION
             front_end->f_TER_tuner_enable(front_end->callback);
           #else  /* INDIRECT_I2C_CONNECTION */
             Si2168B_L2_Tuner_I2C_Enable(front_end);
           #endif /* INDIRECT_I2C_CONNECTION */
              Si2168B_L2_TER_FEF (front_end,1);
           #ifdef    INDIRECT_I2C_CONNECTION
             front_end->f_TER_tuner_disable(front_end->callback);
           #else  /* INDIRECT_I2C_CONNECTION */
             Si2168B_L2_Tuner_I2C_Disable(front_end);
           #endif /* INDIRECT_I2C_CONNECTION */
              Si2168B_L1_DVBT2_STATUS (api, Si2168B_DVBT2_STATUS_CMD_INTACK_CLEAR);
              *num_plp = api->rsp->dvbt2_status.num_plp;
              front_end->detected_rf = seek_freq + front_end->demod->rsp->dvbt2_status.afc_freq*1000;
              SiTRACE ("DVB-T2 lock at %d after %3d ms\n", (front_end->detected_rf)/1000, searchDelay);
              switch (front_end->demod->prop->dd_mode.bw) {
                case Si2168B_DD_MODE_PROP_BW_BW_1D7MHZ : { *bandwidth_Hz =                                    1700000; break; }
                default                               : { *bandwidth_Hz = front_end->demod->prop->dd_mode.bw*1000000; break; }
              }
              *T2_base_lite = api->rsp->dvbt2_status.t2_mode + 1; /* Adding 1 to match dvbt2_mode.lock_mode values (0='ANY', 1='T2-Base', 2='T2-Lite')*/
              *freq         = front_end->detected_rf;
            }
            /* Set min seek_freq for next seek */
            front_end->rangeMin = seek_freq + front_end->seekBWHz;
            /* Return 1 to signal that the Si2168B is locked on a valid channel */
            front_end->handshakeOn = 0;
            return 1;
          }
        }

        /* timeout management (this should only trigger if the channel is very difficult, i.e. when pcl = 1 and dl = 0 until the timeout) */
        timeoutDelay = system_time() - front_end->timeoutStartTime;
        if (timeoutDelay >= max_lock_time_ms) {
          SiTRACE ("Timeout from  %d after %3d ms\n", seek_freq/1000, timeoutDelay);
          SiERROR ("Timeout (blind_mode = 0)\n");
          seek_freq = seek_freq + channelIncrement;
          jump_to_next_channel = 1;
          start_resume         = 1;
          break;
        }

        if (front_end->handshakeUsed) {
          handshakeDelay = system_time() - front_end->handshakeStart_ms;
          if (handshakeDelay >= front_end->handshakePeriod_ms) {
            SiTRACE ("blindscan_handshake : handshake after %5d ms (at %10d). (search delay %6d ms) %*s\n", handshakeDelay, front_end->rangeMin, searchDelay, (searchDelay)/1000,"*");
           *freq                   = seek_freq;
            front_end->rangeMin    = seek_freq;
            front_end->handshakeOn = 1;
            /* The application will check handshakeStart_ms to know whether the blindscan is ended or not */
            return searchDelay;
          } else {
            SiTRACE ("blindscan_handshake : no handshake yet. (handshake delay %6d ms, search delay %6d ms)\n", handshakeDelay, searchDelay);
          }
        }
        /* Check status every n ms */
        system_wait(10);
      }
    }
  }

  if (blind_mode == 1) { /* DVB-C / DVB-S / DVB-S2 / MCNS */

    if (front_end->tuneUnitHz == 1) {
      seek_freq_kHz = seek_freq/1000;
    } else {
      seek_freq_kHz = seek_freq;
    }

    previous_scan_status = front_end->demod->rsp->scan_status.scan_status;

    /* Checking blindscan status before issuing a 'start' or 'resume' */
    Si2168B_L1_SCAN_STATUS (front_end->demod, Si2168B_SCAN_STATUS_CMD_INTACK_OK);
    SiTRACE("blindscan_status      %s buz %d\n", Si2168B_L2_Trace_Scan_Status(front_end->demod->rsp->scan_status.scan_status),front_end->demod->rsp->scan_status.buz );

    if (front_end->demod->rsp->scan_status.scan_status != previous_scan_status) {
      SiTRACE ("scan_status changed from %s to %s\n", Si2168B_L2_Trace_Scan_Status(previous_scan_status), Si2168B_L2_Trace_Scan_Status(front_end->demod->rsp->scan_status.scan_status));
    }

    if (start_resume) {
      /* Wait for scan_status.buz to be '0' before issuing SCAN_CTRL */
      buzyStartTime = system_time();
      while (front_end->demod->rsp->scan_status.buz == Si2168B_SCAN_STATUS_RESPONSE_BUZ_BUSY) {
        Si2168B_L1_SCAN_STATUS (front_end->demod, Si2168B_SCAN_STATUS_CMD_INTACK_OK);
        SiTRACE ("blindscan_interaction ?? (buzy)   Si2168B_L1_SCAN_STATUS scan_status.buz %d after %d ms\n", front_end->demod->rsp->scan_status.buz, system_time() - buzyStartTime);
        if (system_time() - buzyStartTime > 100) {
        SiTRACE ("blindscan_interaction -- (error)  Si2168B_L1_SCAN_STATUS is always 'BUZY'\n");
          return 0;
        }
      }
      if (front_end->demod->cmd->scan_ctrl.action == Si2168B_SCAN_CTRL_CMD_ACTION_START) {
        SiTRACE ("blindscan_interaction >> (start ) Si2168B_L1_SCAN_CTRL( front_end->demod, %d, %8d) \n", front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
      } else {
        SiTRACE ("blindscan_interaction >> (resume) Si2168B_L1_SCAN_CTRL( front_end->demod, %d, %8d) \n", front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
      }
      return_code = Si2168B_L1_SCAN_CTRL (front_end->demod,               front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
      front_end->demod->cmd->scan_ctrl.action = Si2168B_SCAN_CTRL_CMD_ACTION_RESUME;
      if (return_code != NO_Si2168B_ERROR) {
        SiTRACE ("blindscan_interaction -- (error1) Si2168B_L1_SCAN_CTRL %d      ERROR at %10d (%d)\n!!!!!!!!!!!!!!!!!!!!!!!\n", front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz, front_end->demod->rsp->scan_status.scan_status);
        SiTRACE("scan_status.buz %d\n", front_end->demod->rsp->scan_status.buz);
        return 0;
      }
      /* In blind mode, the timeout reference is the SCAN_CTRL */
      front_end->timeoutStartTime = system_time();
    }
    front_end->demod->cmd->scan_ctrl.action = Si2168B_SCAN_CTRL_CMD_ACTION_RESUME;

    /* The actual search loop... */
    while ( 1 ) {

      Si2168B_L1_CheckStatus (front_end->demod);

      searchDelay = system_time() - front_end->searchStartTime;

      if ( (front_end->demod->status->scanint == Si2168B_STATUS_SCANINT_TRIGGERED) ) {

        /* There is an interaction with the FW, refresh the timeoutStartTime */
        front_end->timeoutStartTime = system_time();

        Si2168B_L1_SCAN_STATUS (front_end->demod, Si2168B_SCAN_STATUS_CMD_INTACK_CLEAR);
        SiTRACE("blindscan_status      %s\n", Si2168B_L2_Trace_Scan_Status(front_end->demod->rsp->scan_status.scan_status) );
        skip_resume = 0;

        switch (front_end->demod->rsp->scan_status.scan_status) {
          case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_TUNE_REQUEST          : {
            SiTRACE ("blindscan_interaction -- (tune  ) SCAN TUNE_REQUEST at %8ld kHz\n", front_end->demod->rsp->scan_status.rf_freq);
            if (front_end->tuneUnitHz == 1) {
              seek_freq = Si2168B_L2_Tune (front_end, front_end->demod->rsp->scan_status.rf_freq*1000 );
              seek_freq_kHz = seek_freq/1000;
            } else {
              seek_freq = Si2168B_L2_Tune (front_end, front_end->demod->rsp->scan_status.rf_freq );
              seek_freq_kHz = seek_freq;
            }
            *freq = front_end->rangeMin = seek_freq;
            /* as we will not lock in less than min_lock_time_ms, wait a while... */
            system_wait(min_lock_time_ms);
            break;
          }
          case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_DIGITAL_CHANNEL_FOUND : {
            *standard        = front_end->demod->rsp->scan_status.modulation;
            switch (front_end->demod->rsp->scan_status.modulation) {
              case Si2168B_SCAN_STATUS_RESPONSE_MODULATION_DVBC : {
                *freq            = front_end->demod->rsp->scan_status.rf_freq*1000;
                *symbol_rate_bps = front_end->demod->rsp->scan_status.symb_rate*1000;
                Si2168B_L1_DVBC_STATUS(front_end->demod, Si2168B_DVBC_STATUS_CMD_INTACK_OK);
                front_end->demod->prop->dvbc_symbol_rate.rate = front_end->demod->rsp->scan_status.symb_rate;
                *constellation   = front_end->demod->rsp->dvbc_status.constellation;
                break;
              }
              case Si2168B_SCAN_STATUS_RESPONSE_MODULATION_MCNS : {
                *freq            = front_end->demod->rsp->scan_status.rf_freq*1000;
                *symbol_rate_bps = front_end->demod->rsp->scan_status.symb_rate*1000;
                Si2168B_L1_MCNS_STATUS(front_end->demod, Si2168B_MCNS_STATUS_CMD_INTACK_OK);
                front_end->demod->prop->mcns_symbol_rate.rate = front_end->demod->rsp->scan_status.symb_rate;
                *constellation   = front_end->demod->rsp->mcns_status.constellation;
                break;
              }
              default : {
                SiTRACE("Si2168B_L2_Channel_Seek_Next DIGITAL_CHANNEL_FOUND error at %d: un-handled modulation (%d), aborting (skipped)\n", seek_freq, front_end->demod->rsp->scan_status.modulation);
                front_end->seekAbort = 1;
                return 0;
                break;
              }
            }
            /* When locked, clear scanint before returning from SeekNext, to avoid seeing it again on the 'RESUME', with fast i2c platforms */
            Si2168B_L1_SCAN_STATUS (front_end->demod, Si2168B_SCAN_STATUS_CMD_INTACK_CLEAR);
            SiTRACE ("blindscan_interaction -- (locked) SCAN DIGITAL lock at %d kHz after %3d ms. modulation %3d (%s)\n", *freq, searchDelay, *standard, Si2168B_standardName(*standard) );
            front_end->handshakeOn = 0;
            return 1;
            break;
          }
          case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_ERROR                 : {
            SiTRACE ("blindscan_interaction -- (error2) SCAN error at %d after %4d ms\n", seek_freq/1000, searchDelay);
            SiERROR ("SCAN status returns 'ERROR'\n");
            front_end->handshakeOn = 0;
            return 0;
            break;
          }
          case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_SEARCHING             : {
            SiTRACE("SCAN Searching...\n");
            skip_resume = 1;
            break;
          }
          case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_ENDED                 : {
            SiTRACE ("blindscan_interaction -- (ended ) SCAN ENDED\n");
            Si2168B_L1_SCAN_CTRL (front_end->demod, Si2168B_SCAN_CTRL_CMD_ACTION_ABORT , 0);
            front_end->handshakeOn = 0;
            return 0;
            break;
          }
#ifdef    ALLOW_Si2168B_BLINDSCAN_DEBUG
           case  Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_DEBUG                 : {
            SiTRACE ("blindscan_interaction -- (debug) SCAN DEBUG code %d\n", front_end->demod->rsp->scan_status.symb_rate);
            switch (front_end->demod->rsp->scan_status.symb_rate) {
              case 4: { /* SPECTRUM */
              #ifndef   DO_NOT_DRAW_SPECTRUM
                Si2168B_plot(front_end, "spectrum", 0, seek_freq);
              #endif /* DO_NOT_DRAW_SPECTRUM */
                break;
              }
              case 9: { /* TRYLOCK */
              #ifndef   DO_NOT_TRACK_TRYLOCKS
                Si2168B_plot(front_end, "trylock", 0, seek_freq);
              #endif /* DO_NOT_DRAW_SPECTRUM */
                break;
              }
              default: {}
            }
            /* There has been a debug request by the FW, refresh the timeoutStartTime */
            front_end->timeoutStartTime = system_time();
            break;
          }
#else  /* ALLOW_Si2168B_BLINDSCAN_DEBUG */
           case  63                 : {
             SiERROR("blindscan_interaction -- (warning) You probably run a DEBUG fw, so you need to define ALLOW_Si2168B_BLINDSCAN_DEBUG at project level\n");
             break;
           }
#endif /* ALLOW_Si2168B_BLINDSCAN_DEBUG */
          default : {
            SiTRACE("unknown scan_status %d\n", front_end->demod->rsp->scan_status.scan_status);
            skip_resume = 1;
            break;
          }
        }

        if (skip_resume == 0) {
          SiTRACE ("blindscan_interaction >> (resume) Si2168B_L1_SCAN_CTRL( front_end->demod, %d, %8d)\n", front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
          return_code = Si2168B_L1_SCAN_CTRL (front_end->demod,              front_end->demod->cmd->scan_ctrl.action, seek_freq_kHz);
          if (return_code != NO_Si2168B_ERROR) {
            SiTRACE ( "Si2168B_L1_SCAN_CTRL ERROR at %d (%d)\n!!!!!!!!!!!!!!!!!!!!!!!\n", seek_freq_kHz, front_end->demod->rsp->scan_status.scan_status);
            SiERROR ( "Si2168B_L1_SCAN_CTRL 'RESUME' ERROR during seek loop\n");
          }
        }
      }

      /* timeout management (this should never happen if timeout values are correctly set) */
      timeoutDelay = system_time() - front_end->timeoutStartTime;
      if (timeoutDelay >= max_decision_time_ms) {
        SiTRACE ("Scan decision timeout from  %d after %d ms. Check your timeout limits!\n", seek_freq_kHz, timeoutDelay);
        SiERROR ("Scan decision timeout (blind_mode = 1)\n");
        front_end->seekAbort   = 1;
        front_end->handshakeOn = 0;
        break;
      }

        if (front_end->handshakeUsed) {
        handshakeDelay = system_time() - front_end->handshakeStart_ms;
        if (handshakeDelay >= front_end->handshakePeriod_ms) {
          SiTRACE ("blindscan_handshake : handshake after %5d ms (at %10d). (search delay %6d ms) %*s\n", handshakeDelay, front_end->rangeMin, searchDelay, (searchDelay)/1000,"*");
         *freq                    = seek_freq;
          front_end->handshakeOn = 1;
          /* The application will check handshakeStart_ms to know whether the blindscan is ended or not */
          return searchDelay;
        } else {
          SiTRACE ("blindscan_handshake : no handshake yet. (handshake delay %6d ms, search delay %6d ms)\n", handshakeDelay, searchDelay);
        }
      }

      /* Check seekAbort flag (set in case of timeout or by the top-level application) */
      if (front_end->seekAbort) {
        /* Abort the SCAN loop to allow it to restart with the new rangeMin frequency */
        SiTRACE ("blindscan_interaction >> (abort!) Si2168B_L1_SCAN_CTRL( front_end->demod, Si2168B_SCAN_CTRL_CMD_ACTION_ABORT)\n");
                  Si2168B_L1_SCAN_CTRL (front_end->demod, Si2168B_SCAN_CTRL_CMD_ACTION_ABORT , 0);
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
  NAME: Si2168B_L2_Channel_Seek_Abort
  DESCRIPTION: aborts the seek
  Programming Guide Reference:    Flowchart TBD (Channel Scan flowchart)

  Parameter:  Pointer to Si2168B Context
  Returns:    0 if successful, otherwise an error.
************************************************************************************************************************/
int  Si2168B_L2_Channel_Seek_Abort (Si2168B_L2_Context *front_end)
{
  front_end->seekAbort   = 1;
  front_end->handshakeOn = 0;
  return 0;
}
/************************************************************************************************************************
  NAME: Si2168B_L2_Channel_Lock_Abort
  DESCRIPTION: aborts the lock_to_carrier
  Programming Guide Reference:    Flowchart TBD (Channel Lock flowchart)

  Parameter:  Pointer to Si2168B Context
  Returns:    0 if successful, otherwise an error.
************************************************************************************************************************/
int  Si2168B_L2_Channel_Lock_Abort (Si2168B_L2_Context *front_end)
{
  front_end->lockAbort = 1;
  front_end->handshakeOn = 0;
  return 0;
}
/************************************************************************************************************************
  NAME: Si2168B_L2_Channel_Seek_End
  DESCRIPTION: returns the chip back to normal use following a seek sequence
  Programming Guide Reference:    Flowchart TBD (Channel Scan flowchart)

  Parameter:  Pointer to Si2168B Context
  Returns:    0 if successful, otherwise an error.
************************************************************************************************************************/
int  Si2168B_L2_Channel_Seek_End   (Si2168B_L2_Context *front_end)
{
  front_end = front_end; /* To avoid compiler warning */

  front_end->demod->prop->scan_ien.buzien           = Si2168B_SCAN_IEN_PROP_BUZIEN_DISABLE ; /* (default 'DISABLE') */
  front_end->demod->prop->scan_ien.reqien           = Si2168B_SCAN_IEN_PROP_REQIEN_DISABLE ; /* (default 'DISABLE') */
  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_SCAN_IEN_PROP_CODE);

  switch (front_end->demod->standard)
  {
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT    : { front_end->demod->prop->dd_mode.modulation = Si2168B_DD_MODE_PROP_MODULATION_DVBT ; break; }
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC    : { front_end->demod->prop->dd_mode.modulation = Si2168B_DD_MODE_PROP_MODULATION_DVBC ; break; }
    case Si2168B_DD_MODE_PROP_MODULATION_MCNS    : { front_end->demod->prop->dd_mode.modulation = Si2168B_DD_MODE_PROP_MODULATION_MCNS ; break; }
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2   : { front_end->demod->prop->dd_mode.modulation = Si2168B_DD_MODE_PROP_MODULATION_DVBT2; break; }
    default                                     : { SiTRACE("UNKNOWN standard %d\n", front_end->demod->standard); break;}
  }

  SiTRACE("auto_detect_TER %d\n",front_end->auto_detect_TER);
  if (front_end->auto_detect_TER) {
    switch (front_end->demod->standard)
    {
      case Si2168B_DD_MODE_PROP_MODULATION_DVBT    :
      case Si2168B_DD_MODE_PROP_MODULATION_DVBT2   : {
        front_end->demod->prop->dd_mode.modulation  = Si2168B_DD_MODE_PROP_MODULATION_AUTO_DETECT;
        front_end->demod->prop->dd_mode.auto_detect = Si2168B_DD_MODE_PROP_AUTO_DETECT_AUTO_DVB_T_T2;
      }
      default                                     : { break;}
    }
  }
  Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_MODE_PROP_CODE);

  return 0;
}
/************************************************************************************************************************
  NAME: Si2168B_TerAutoDetect
  DESCRIPTION: Set the Si2168B in Ter Auto Detect mode

  Parameter:  Pointer to Si2168B Context
  Returns:    front_end->auto_detect_TER
************************************************************************************************************************/
int  Si2168B_TerAutoDetect         (Si2168B_L2_Context *front_end)
{
  front_end->auto_detect_TER = 1;
  return front_end->auto_detect_TER;
}
/************************************************************************************************************************
  NAME: Si2168B_TerAutoDetectOff
  DESCRIPTION: Set the Si2168B in Ter Auto Detect 'off' mode

  Parameter:  Pointer to Si2168B Context
  Returns:    front_end->auto_detect_TER
************************************************************************************************************************/
int  Si2168B_TerAutoDetectOff      (Si2168B_L2_Context *front_end)
{
  front_end->auto_detect_TER = 0;
  return front_end->auto_detect_TER;
}
#ifdef    SILABS_API_TEST_PIPE
int   Si2168B_L2_GET_REG           (Si2168B_L2_Context *front_end, unsigned char   reg_code_lsb, unsigned char   reg_code_mid, unsigned char   reg_code_msb) {
  int res;
  SiTraceConfiguration("traces suspend");
  if (Si2168B_L1_DD_GET_REG  (front_end->demod, reg_code_lsb, reg_code_mid, reg_code_msb) != NO_Si2168B_ERROR) {
    SiERROR("Error calling Si2168B_L1_DD_GET_REG\n");
    SiTraceConfiguration("traces resume");
    return 0;
  }
  SiTraceConfiguration("traces resume");
  res =  (front_end->demod->rsp->dd_get_reg.data4<<24)
        +(front_end->demod->rsp->dd_get_reg.data3<<16)
        +(front_end->demod->rsp->dd_get_reg.data2<<8 )
        +(front_end->demod->rsp->dd_get_reg.data1<<0 );
  return res;
}
#define get_reg(ptr, register)               SiTRACE("%s %3d  ", #register, Si2168B_L2_GET_REG (ptr, Si2168B_##register##_LSB, Si2168B_##register##_MID, Si2168B_##register##_MSB));
#define get_reg_from_code(ptr,register)      SiTRACE("%s %3d  ", #register, Si2168B_L2_GET_REG (ptr, Si2168B_##register##_CODE));
#if 1  /* Codes for GET_REG */
  #define Si2168B_auto_relock_CODE          0,78,10
  #define Si2168B_en_rst_error_CODE         0,244,9
  #define Si2168B_demod_lock_t_CODE         0,0,8
  #define Si2168B_fec_lock_t2_CODE          0,140,18
  #define Si2168B_tbd_a_CODE               15,88,10
  #define Si2168B_trap_type_CODE            7,246,9
  #define Si2168B_firmware_sm_CODE          7,76,10
  #define Si2168B_pre_crc_ok_CODE           0,82,12
  #define Si2168B_pre_crc_nok_tog_CODE      0,84,12
  #define Si2168B_l1_post_crc_ok_CODE       0,14,12
  #define Si2168B_l1_post_crc_nok_tog_CODE  0,15,12
  #define Si2168B_td_alarms_CODE            8,148,14
  #define Si2168B_td_alarms_first_CODE      8,144,14
  #define Si2168B_fd_alarms_CODE           31,56,14
  #define Si2168B_fd_alarms_first_CODE     31,60,14
  #define Si2168B_mp_a_mux_CODE             3,106,1
  #define Si2168B_mp_b_mux_CODE             3,109,1
  #define Si2168B_mp_c_mux_CODE             3,112,1
  #define Si2168B_mp_d_mux_CODE             3,115,1
  #define Si2168B_agc1_cmd_CODE             7,38,4
  #define Si2168B_agc2_cmd_CODE             7,50,4
  #define Si2168B_agc1_pola_CODE           33,36,4
  #define Si2168B_agc2_pola_CODE           33,48,4
  #define Si2168B_agc1_min_CODE             7,34,4
  #define Si2168B_agc1_max_CODE             7,35,4
  #define Si2168B_agc1_freeze_CODE          0,36,4
  #define Si2168B_agc2_min_CODE             7,46,4
  #define Si2168B_agc2_max_CODE             7,47,4
  #define Si2168B_agc2_freeze_CODE          0,48,4
  #define Si2168B_standard_CODE             5,116,4
  #define Si2168B_mailbox61_CODE            7,165,0
  #define Si2168B_wdog_error_CODE          66,245,9
#endif /* Codes for GET_REG */
int   Si2168B_L2_CheckLoop         (Si2168B_L2_Context *front_end) {

  get_reg_from_code(front_end,mp_a_mux);
  get_reg_from_code(front_end,mp_b_mux);
  get_reg_from_code(front_end,mp_c_mux);
  get_reg_from_code(front_end,mp_d_mux);

  get_reg_from_code(front_end,firmware_sm);
  get_reg_from_code(front_end,agc1_min);
  get_reg_from_code(front_end,agc2_min);
  get_reg_from_code(front_end,agc1_max);
  get_reg_from_code(front_end,agc2_max);
  get_reg_from_code(front_end,agc1_pola);
  get_reg_from_code(front_end,agc2_pola);
  get_reg_from_code(front_end,agc1_freeze);
  get_reg_from_code(front_end,agc2_freeze);
  get_reg_from_code(front_end,agc1_cmd);
  get_reg_from_code(front_end,agc2_cmd);

  printf("\n");

  printf("\n");
  return 1;
}
int   Si2168B_L2_Health_Check      (Si2168B_L2_Context *front_end) {
  int standard;
  int firmware_sm;
  int mailbox61;
  int tbd_a;
  int wdog_error;
  int trap_type;

  standard    = Si2168B_L2_GET_REG (front_end, Si2168B_standard_CODE);
  firmware_sm = Si2168B_L2_GET_REG (front_end, Si2168B_firmware_sm_CODE);
  tbd_a       = Si2168B_L2_GET_REG (front_end, Si2168B_tbd_a_CODE);
  mailbox61   = Si2168B_L2_GET_REG (front_end, Si2168B_mailbox61_CODE);
  wdog_error  = Si2168B_L2_GET_REG (front_end, Si2168B_wdog_error_CODE);
  trap_type   = Si2168B_L2_GET_REG (front_end, Si2168B_trap_type_CODE);

  SiTRACE("std %2d, fw_sm %4d / tbd_a %4d / mb61 %4d / wdog %4d / trap_type %4d\n", standard, firmware_sm, tbd_a, mailbox61, wdog_error, trap_type);

  return 1;
}
/* #include "Si_2164_dump.c"   */
/* #include "Si_2164_Get_Echoes.c" */
/* #include "SiLabs_DVB_T2_Signalling.h"*/
#ifdef    _DVB_T2_SIGNALLING_H_
#if 1

#define Si2168B_get_reg(ptr,register)       Si2168B_L2_GET_REG            (ptr, Si2168B_##register##_LSB, Si2168B_##register##_MID, Si2168B_##register##_MSB)

#define Si2168B_pre_type_LSB 7
#define Si2168B_pre_type_MID 44
#define Si2168B_pre_type_MSB 12

#define Si2168B_pre_bwt_ext_LSB 0
#define Si2168B_pre_bwt_ext_MID 240
#define Si2168B_pre_bwt_ext_MSB 4

#define Si2168B_pre_s1_LSB 2
#define Si2168B_pre_s1_MID 46
#define Si2168B_pre_s1_MSB 12

#define Si2168B_pre_s2_LSB 3
#define Si2168B_pre_s2_MID 47
#define Si2168B_pre_s2_MSB 12

#define Si2168B_pre_l1_repetition_flag_LSB 0
#define Si2168B_pre_l1_repetition_flag_MID 48
#define Si2168B_pre_l1_repetition_flag_MSB 12

#define Si2168B_pre_guard_interval_LSB 2
#define Si2168B_pre_guard_interval_MID 49
#define Si2168B_pre_guard_interval_MSB 12

#define Si2168B_pre_papr_LSB 3
#define Si2168B_pre_papr_MID 50
#define Si2168B_pre_papr_MSB 12

#define Si2168B_pre_l1_mod_LSB 3
#define Si2168B_pre_l1_mod_MID 51
#define Si2168B_pre_l1_mod_MSB 12

#define Si2168B_pre_l1_cod_LSB 1
#define Si2168B_pre_l1_cod_MID 52
#define Si2168B_pre_l1_cod_MSB 12

#define Si2168B_pre_l1_fec_type_LSB 1
#define Si2168B_pre_l1_fec_type_MID 53
#define Si2168B_pre_l1_fec_type_MSB 12

#define Si2168B_pre_l1_post_size_LSB 17
#define Si2168B_pre_l1_post_size_MID 56
#define Si2168B_pre_l1_post_size_MSB 12

#define Si2168B_pre_l1_post_info_size_LSB 17
#define Si2168B_pre_l1_post_info_size_MID 60
#define Si2168B_pre_l1_post_info_size_MSB 12

#define Si2168B_pre_pilot_pattern_LSB 3
#define Si2168B_pre_pilot_pattern_MID 64
#define Si2168B_pre_pilot_pattern_MSB 12

#define Si2168B_pre_tx_id_availability_LSB 7
#define Si2168B_pre_tx_id_availability_MID 65
#define Si2168B_pre_tx_id_availability_MSB 12

#define Si2168B_pre_cell_id_LSB 15
#define Si2168B_pre_cell_id_MID 66
#define Si2168B_pre_cell_id_MSB 12

#define Si2168B_pre_network_id_LSB 15
#define Si2168B_pre_network_id_MID 68
#define Si2168B_pre_network_id_MSB 12

#define Si2168B_pre_t2_system_id_LSB 15
#define Si2168B_pre_t2_system_id_MID 70
#define Si2168B_pre_t2_system_id_MSB 12

#define Si2168B_pre_num_t2_frames_LSB 7
#define Si2168B_pre_num_t2_frames_MID 72
#define Si2168B_pre_num_t2_frames_MSB 12

#define Si2168B_pre_num_data_symbols_LSB 11
#define Si2168B_pre_num_data_symbols_MID 74
#define Si2168B_pre_num_data_symbols_MSB 12

#define Si2168B_pre_regen_flag_LSB 2
#define Si2168B_pre_regen_flag_MID 76
#define Si2168B_pre_regen_flag_MSB 12

#define Si2168B_pre_l1_post_extension_LSB 0
#define Si2168B_pre_l1_post_extension_MID 77
#define Si2168B_pre_l1_post_extension_MSB 12

#define Si2168B_pre_num_rf_LSB    2
#define Si2168B_pre_num_rf_MID   78
#define Si2168B_pre_num_rf_MSB   12

#define Si2168B_pre_reserved_LSB    5
#define Si2168B_pre_reserved_MID   81
#define Si2168B_pre_reserved_MSB   12

#define Si2168B_num_rf_LSB    2
#define Si2168B_num_rf_MID  230
#define Si2168B_num_rf_MSB   12

#define Si2168B_pre_current_rf_idx_LSB 2
#define Si2168B_pre_current_rf_idx_MID 79
#define Si2168B_pre_current_rf_idx_MSB 12

#define Si2168B_pre_t2_version_LSB 3
#define Si2168B_pre_t2_version_MID 80
#define Si2168B_pre_t2_version_MSB 12

#define Si2168B_bbdf_data_issyi_status_LSB    1
#define Si2168B_bbdf_data_issyi_status_MID   10
#define Si2168B_bbdf_data_issyi_status_MSB   18

#define Si2168B_bbdf_comm_issyi_status_LSB    1
#define Si2168B_bbdf_comm_issyi_status_MID   11
#define Si2168B_bbdf_comm_issyi_status_MSB   18

#define Si2168B_bbdf_data_npd_active_LSB 0
#define Si2168B_bbdf_data_npd_active_MID 8
#define Si2168B_bbdf_data_npd_active_MSB 18

#define Si2168B_bbdf_comm_npd_active_LSB 0
#define Si2168B_bbdf_comm_npd_active_MID 9
#define Si2168B_bbdf_comm_npd_active_MSB 18

#define Si2168B_djb_alarm_data_LSB 10
#define Si2168B_djb_alarm_data_MID 92
#define Si2168B_djb_alarm_data_MSB 19

#define Si2168B_djb_alarm_comm_LSB 10
#define Si2168B_djb_alarm_comm_MID 94
#define Si2168B_djb_alarm_comm_MSB 19

#define Si2168B_ts_rate_in_LSB   26
#define Si2168B_ts_rate_in_MID  116
#define Si2168B_ts_rate_in_MSB   19

#define Si2168B_storing_enable_LSB 0
#define Si2168B_storing_enable_MID 92
#define Si2168B_storing_enable_MSB 13

#define Si2168B_stored_l1_post_nb_LSB 12
#define Si2168B_stored_l1_post_nb_MID 94
#define Si2168B_stored_l1_post_nb_MSB 13

#define Si2168B_start_ibs_storing_data_LSB 10
#define Si2168B_start_ibs_storing_data_MID 96
#define Si2168B_start_ibs_storing_data_MSB 13

#define Si2168B_store_l1_post_en_LSB 33
#define Si2168B_store_l1_post_en_MID 92
#define Si2168B_store_l1_post_en_MSB 13

#define Si2168B_base_read_address_LSB 10
#define Si2168B_base_read_address_MID 104
#define Si2168B_base_read_address_MSB 13

#define Si2168B_read_data_LSB 31
#define Si2168B_read_data_MID 108
#define Si2168B_read_data_MSB 13

#define Si2168B_read_data_latch_LSB   33
#define Si2168B_read_data_latch_MID  134
#define Si2168B_read_data_latch_MSB   11

#endif

unsigned int   Si2168B_n_bits             (unsigned int *wordTable, int n, unsigned int *bitIndex) {
  int startByteIndex;
  int stopByteIndex;
  int leftBitShift;
  int bitShift;
  int wordShift;
  int intSize;
  int b;
  unsigned int res;
  SiTRACE("Si2168B_n_bits(wordTable, %2d, %4d) ", n, *bitIndex);
  intSize        = sizeof(int)*8;
  res            = 0x00000000;
  wordShift      = 0;
  startByteIndex =  *bitIndex/intSize;
  stopByteIndex  = (*bitIndex+n-1)/intSize;
  leftBitShift   = (*bitIndex) - (startByteIndex*intSize);
  bitShift       = ((stopByteIndex+1)*intSize) - (*bitIndex+n);
  *bitIndex      = *bitIndex + n;
  for (b = startByteIndex; b <= stopByteIndex; b++) {
    res = res + (wordTable[b]<<wordShift);
    if (b == startByteIndex) {
      res = res << leftBitShift;
      res = res >> leftBitShift;
    }
    wordShift = wordShift+intSize;
  }
  res = res >> bitShift;
  SiTRACE("0x%08x (%12d)\n", res, res);
  return res;
}
int   Si2168B_L2_Read_L1_Misc_Data        (Si2168B_L2_Context *front_end, SiLabs_T2_Misc_Signalling    *misc) {
  int issyi_status;
  misc->bw                 = front_end->demod->prop->dd_mode.bw;
  issyi_status             = Si2168B_get_reg (front_end, bbdf_data_issyi_status);
       if (issyi_status == 0) {misc->data_issy = 0; misc->data_issy_ts_long  = 0; }
  else if (issyi_status == 1) {misc->data_issy = 1; misc->data_issy_ts_long  = 1; }
  else if (issyi_status == 2) {misc->data_issy = 1; misc->data_issy_ts_long  = 0; }
  misc->data_npd           = Si2168B_get_reg (front_end, bbdf_data_npd_active);
  misc->data_ts_rate       = Si2168B_get_reg (front_end, ts_rate_in);
  misc->data_djb_alarm     = Si2168B_get_reg (front_end, djb_alarm_data);
  issyi_status             = Si2168B_get_reg (front_end, bbdf_comm_issyi_status);
       if (issyi_status == 0) {misc->comm_issy = 0; misc->comm_issy_ts_long  = 0; }
  else if (issyi_status == 1) {misc->comm_issy = 1; misc->comm_issy_ts_long  = 1; }
  else if (issyi_status == 2) {misc->comm_issy = 1; misc->comm_issy_ts_long  = 0; }
  misc->comm_npd           = Si2168B_get_reg (front_end, bbdf_comm_npd_active);
  misc->comm_ts_rate       = 0;
  misc->comm_npd           = Si2168B_get_reg (front_end, djb_alarm_comm);
  return 1;
}
int   Si2168B_L2_Read_L1_Pre_Data         (Si2168B_L2_Context *front_end, SiLabs_T2_L1_Pre_Signalling  *pre ) {
  pre->type               = Si2168B_get_reg (front_end, pre_type);
  pre->bwt_ext            = Si2168B_get_reg (front_end, pre_bwt_ext);
  pre->s1                 = Si2168B_get_reg (front_end, pre_s1);
  pre->s2                 = Si2168B_get_reg (front_end, pre_s2);
  pre->l1_repetition_flag = Si2168B_get_reg (front_end, pre_l1_repetition_flag);
  pre->guard_interval     = Si2168B_get_reg (front_end, pre_guard_interval);
  pre->papr               = Si2168B_get_reg (front_end, pre_papr);
  pre->l1_mod             = Si2168B_get_reg (front_end, pre_l1_mod);
  pre->l1_cod             = Si2168B_get_reg (front_end, pre_l1_cod);
  pre->l1_fec_type        = Si2168B_get_reg (front_end, pre_l1_fec_type);
  pre->l1_post_size       = Si2168B_get_reg (front_end, pre_l1_post_size);
  pre->l1_post_info_size  = Si2168B_get_reg (front_end, pre_l1_post_info_size);
  pre->pilot_pattern      = Si2168B_get_reg (front_end, pre_pilot_pattern);
  pre->tx_id_availability = Si2168B_get_reg (front_end, pre_tx_id_availability);
  pre->cell_id            = Si2168B_get_reg (front_end, pre_cell_id);
  pre->network_id         = Si2168B_get_reg (front_end, pre_network_id);
  pre->t2_system_id       = Si2168B_get_reg (front_end, pre_t2_system_id);
  pre->num_t2_frames      = Si2168B_get_reg (front_end, pre_num_t2_frames);
  pre->num_data_symbols   = Si2168B_get_reg (front_end, pre_num_data_symbols);
  pre->regen_flag         = Si2168B_get_reg (front_end, pre_regen_flag);
  pre->l1_post_extension  = Si2168B_get_reg (front_end, pre_l1_post_extension);
  pre->num_rf             = Si2168B_get_reg (front_end, pre_num_rf);
  pre->current_rf_idx     = Si2168B_get_reg (front_end, pre_current_rf_idx);
  pre->t2_version         = Si2168B_get_reg (front_end, pre_t2_version);
  return 1;
}
int   Si2168B_L2_Read_L1_Post_Data        (Si2168B_L2_Context *front_end, SiLabs_T2_L1_Post_Signalling *post) {
  int stored_l1_post_nb;
  int start_ibs_storing_data;
  int read_data;
  int max_l1_storing;
  int word_index;
  unsigned int i;
  unsigned int bitIndex;
  int storing_enable;
  unsigned int  intTable[50000];
  unsigned int *wordTable;

  wordTable  = &intTable[0];

  /* Read L1 Post table data */
  word_index = 0;
  Si2168B_READ(front_end->demod, storing_enable);
  if (storing_enable==0) {
    Si2168B_WRITE(front_end->demod, storing_enable, 1);
    system_wait(5000);
  }

  Si2168B_READ (front_end->demod, stored_l1_post_nb);
  Si2168B_READ (front_end->demod, start_ibs_storing_data);
  max_l1_storing = start_ibs_storing_data;
  Si2168B_WRITE(front_end->demod, base_read_address, 0);
  Si2168B_WRITE(front_end->demod, read_data_latch, 1);
  SiTRACE("Load L1 signalling (%s) from the storing memory (%d x 32 bits words).\n", Si2168B_standardName(front_end->demod->rsp->dd_status.modulation), stored_l1_post_nb);

  SiTRACE("reading until %d or %d\n", stored_l1_post_nb-1, max_l1_storing);

  while ( (word_index < stored_l1_post_nb-1) && (word_index < max_l1_storing) ) {
    Si2168B_READ (front_end->demod, read_data);
    Si2168B_WRITE(front_end->demod, read_data_latch, 1);
    SiTRACE("word %3d = 0x%08x\n", word_index, read_data);
    wordTable[word_index++] = read_data;
  }
  SiTRACE("\n");
  Si2168B_WRITE(front_end->demod, store_l1_post_en, 1);

  word_index = 0;
  while ( (word_index < stored_l1_post_nb-1) && (word_index < max_l1_storing) ) {
    SiTRACE("%08x ", wordTable[word_index++]);
  }
  SiTRACE("\n");

  /* Parse L1 Post table data */
  bitIndex = 0;

  post->configurable.num_rf               = Si2168B_get_reg (front_end, num_rf);
  post->configurable.s2                   = Si2168B_get_reg (front_end, pre_s2);
  post->configurable.sub_slices_per_frame = Si2168B_n_bits(wordTable, 15, &bitIndex);
  post->configurable.num_plp              = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->configurable.num_aux              = Si2168B_n_bits(wordTable,  4, &bitIndex);
  post->configurable.aux_config_rfu       = Si2168B_n_bits(wordTable,  8, &bitIndex);
  for (i=0; i < post->configurable.num_rf; i++) {
  post->configurable.rf[i].rf_idx         = Si2168B_n_bits(wordTable,  3, &bitIndex);
  post->configurable.rf[i].frequency      = Si2168B_n_bits(wordTable, 32, &bitIndex);
  }
  if ( (post->configurable.s2&0x0f) == 0x01) {
  post->configurable.fef_type             = Si2168B_n_bits(wordTable,  4, &bitIndex);
  post->configurable.fef_length           = Si2168B_n_bits(wordTable, 22, &bitIndex);
  post->configurable.fef_interval         = Si2168B_n_bits(wordTable,  8, &bitIndex);
 }
 for (i = 0; i < post->configurable.num_plp; i++) {
  post->configurable.plp[i].plp_id               = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->configurable.plp[i].plp_type             = Si2168B_n_bits(wordTable,  3, &bitIndex);
  post->configurable.plp[i].plp_payload_type     = Si2168B_n_bits(wordTable,  5, &bitIndex);
  post->configurable.plp[i].ff_flag              = Si2168B_n_bits(wordTable,  1, &bitIndex);
  post->configurable.plp[i].first_rf_idx         = Si2168B_n_bits(wordTable,  3, &bitIndex);
  post->configurable.plp[i].first_frame_idx      = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->configurable.plp[i].plp_group_id         = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->configurable.plp[i].plp_cod              = Si2168B_n_bits(wordTable,  3, &bitIndex);
  post->configurable.plp[i].plp_mod              = Si2168B_n_bits(wordTable,  3, &bitIndex);
  post->configurable.plp[i].plp_rotation         = Si2168B_n_bits(wordTable,  1, &bitIndex);
  post->configurable.plp[i].plp_fec_type         = Si2168B_n_bits(wordTable,  2, &bitIndex);
  post->configurable.plp[i].plp_num_blocks_max   = Si2168B_n_bits(wordTable, 10, &bitIndex);
  post->configurable.plp[i].frame_interval       = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->configurable.plp[i].time_il_length       = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->configurable.plp[i].time_il_type         = Si2168B_n_bits(wordTable,  1, &bitIndex);
  post->configurable.plp[i].in_band_a_flag       = Si2168B_n_bits(wordTable,  1, &bitIndex);
  post->configurable.plp[i].in_band_b_flag       = Si2168B_n_bits(wordTable,  1, &bitIndex);
  post->configurable.plp[i].reserved_1           = Si2168B_n_bits(wordTable, 11, &bitIndex);
  post->configurable.plp[i].plp_mode             = Si2168B_n_bits(wordTable,  2, &bitIndex);
  post->configurable.plp[i].static_flag          = Si2168B_n_bits(wordTable,  1, &bitIndex);
  post->configurable.plp[i].static_padding_flag  = Si2168B_n_bits(wordTable,  1, &bitIndex);
  }
  post->configurable.fef_length_msb              = Si2168B_n_bits(wordTable,  2, &bitIndex);
  post->configurable.reserved_2                  = Si2168B_n_bits(wordTable, 30, &bitIndex);
  for (i=0; i<post->configurable.num_aux; i++) {
  post->configurable.aux[i].aux_stream_type      = Si2168B_n_bits(wordTable,  4, &bitIndex);
  post->configurable.aux[i].aux_private_conf     = Si2168B_n_bits(wordTable, 28, &bitIndex);
  }
  post->dynamic.frame_idx            = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->dynamic.sub_slice_interval   = Si2168B_n_bits(wordTable, 22, &bitIndex);
  post->dynamic.type_2_start         = Si2168B_n_bits(wordTable, 22, &bitIndex);
  post->dynamic.l1_change_counter    = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->dynamic.start_rf_idx         = Si2168B_n_bits(wordTable,  3, &bitIndex);
  post->dynamic.reserved_1           = Si2168B_n_bits(wordTable,  8, &bitIndex);
  for (i=0; i< post->dynamic.num_plp; i++) {
  post->dynamic.plp[i].plp_id               = Si2168B_n_bits(wordTable,  8, &bitIndex);
  post->dynamic.plp[i].plp_start            = Si2168B_n_bits(wordTable, 22, &bitIndex);
  post->dynamic.plp[i].plp_num_blocks       = Si2168B_n_bits(wordTable, 10, &bitIndex);
  post->dynamic.plp[i].reserved_2           = Si2168B_n_bits(wordTable,  8, &bitIndex);
  }
  post->dynamic.reserved_3           = Si2168B_n_bits(wordTable,  8, &bitIndex);
  for (i=0; i <post->configurable.num_aux; i++) {
  post->dynamic.aux_private_dyn[i].msb  = Si2168B_n_bits(wordTable, 24, &bitIndex);
  post->dynamic.aux_private_dyn[i].lsb  = Si2168B_n_bits(wordTable, 24, &bitIndex);
  }
  return 1;
}
#endif /* _DVB_T2_SIGNALLING_H_ */
int   Si2168B_L2_Test_MPLP                (Si2168B_L2_Context *front_end, double freq) {
  int data_plp_count;
  int num_plp;
  int plp_index;
  int plp_id;
  int plp_type;
  int lock_wait_start_ms;
  front_end = front_end;
  freq = freq;
  data_plp_count = 0;
  lock_wait_start_ms = system_time();
  Si2168B_L2_lock_to_carrier (front_end, Si2168B_DD_MODE_PROP_MODULATION_DVBT2, freq, 8000000, 0, 0, 0
                             , -1
                             , 0
                             );
  if (front_end->demod->rsp->dd_status.dl == 0) {
    SiTRACE("Demod timeout after %4d ms\n", system_time() -  lock_wait_start_ms);
    return 0;
  }
  /* Locked; Now check DVBT2 status */
  if (Si2168B_L1_DVBT2_STATUS    (front_end->demod, Si2168B_DVBT2_STATUS_CMD_INTACK_OK) != NO_Si2168B_ERROR) {
    SiTRACE("Si2168B_L1_DVBT2_STATUS error\n");
    return 0;
  }
  SiTRACE("dvbt2_status.pcl     %d\n", front_end->demod->rsp->dvbt2_status.pcl);
  SiTRACE("dvbt2_status.num_plp %d\n", front_end->demod->rsp->dvbt2_status.num_plp);
  SiTRACE("dvbt2_status.plp_id  %d\n", front_end->demod->rsp->dvbt2_status.plp_id);
  num_plp = front_end->demod->rsp->dvbt2_status.num_plp;
  SiTRACE("There are %d PLPs in this stream\n", num_plp);
  data_plp_count = 0;
  for (plp_index=0; plp_index<num_plp; plp_index++) {
    Si2168B_L1_DVBT2_PLP_INFO (front_end->demod, plp_index);
    plp_id   = front_end->demod->rsp->dvbt2_plp_info.plp_id;
    plp_type = front_end->demod->rsp->dvbt2_plp_info.plp_type;
    SiTRACE("PLP index %3d: PLP ID %3d, PLP TYPE %d : ", plp_index, plp_id, plp_type);
    if (plp_type == Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_COMMON) {
      SiTRACE("COMMON PLP at index %3d: PLP ID %3d, PLP TYPE %d\n", plp_index, plp_id, plp_type);
    } else {
      SiTRACE("DATA   PLP at index %3d: PLP ID %3d, PLP TYPE %d\n", plp_index, plp_id, plp_type);
      data_plp_count++;
    }
  }
  return data_plp_count;
}
unsigned char Si2168B_iq_demap = 0;
int   Si2168B_L2_Get_Constellation_IQ_rx_type (Si2168B_L2_Context   *front_end, unsigned char rx_type) {
  #define Si2168B_rx_type_CODE       2,245,13
  #define Si2168B_rx_type_l1_pre      0
  #define Si2168B_rx_type_l1_post     1
  #define Si2168B_rx_type_data_plp    2
  #define Si2168B_rx_type_common_plp  3
  #define Si2168B_rx_type_all         4
  #define Si2168B_rx_type_off         5

  if (rx_type  < 6) {
    Si2168B_iq_demap = 1;
  } else {
    Si2168B_iq_demap = 0;
  }

  return Si2168B_L1_DD_SET_REG (front_end->demod, Si2168B_rx_type_CODE, rx_type);
}
int   Si2168B_L2_Get_Constellation_IQ         (Si2168B_L2_Context   *front_end, int *i, int *q) {
  int           return_code;
  unsigned int  imask;
  unsigned int  qmask;
  unsigned int  ishift;
  unsigned int  iq_coef;
  unsigned int  limit;
  unsigned int  value;
  unsigned int  sign_bit;
  int  ival;
  int  qval;

  *i = *q = 0;

  #define Si2168B_DVBT_IQ_CODE 19,104,6
  #define Si2168B_DVBC_IQ_CODE 31,184,7
  #define Si2168B_DVBS_IQ_CODE 15,184,5
  #define Si2168B_rx_iq_CODE   31,240,13

  switch (front_end->demod->standard)
  {
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC2:
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT :
    case Si2168B_DD_MODE_PROP_MODULATION_DVBT2: {
      if (Si2168B_iq_demap != 0) {
        return_code = Si2168B_L1_DD_GET_REG (front_end->demod, Si2168B_rx_iq_CODE);
      } else {
        return_code = Si2168B_L1_DD_GET_REG (front_end->demod, Si2168B_DVBT_IQ_CODE);
      }
      imask = 0x000ffc00 ; qmask = 0x000003ff ; ishift = 10 ; iq_coef = 4; limit = qmask+1;
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_DVBC : {
      return_code = Si2168B_L1_DD_GET_REG (front_end->demod, Si2168B_DVBC_IQ_CODE);
      imask = 0xffff0000 ; qmask = 0x0000ffff ; ishift = 16 ; iq_coef = 2; limit = qmask+1;
      break;
    }
    case Si2168B_DD_MODE_PROP_MODULATION_DSS  :
    case Si2168B_DD_MODE_PROP_MODULATION_DVBS :
    case Si2168B_DD_MODE_PROP_MODULATION_DVBS2: {
      return_code = Si2168B_L1_DD_GET_REG (front_end->demod, Si2168B_DVBS_IQ_CODE);
      imask = 0x0000ff00 ; qmask = 0x000000ff ; ishift =  8 ; iq_coef = 4; limit = qmask+1;
      break;
    }
    default : {
      SiTRACE("IQ not possible with standard %d\n", front_end->demod->standard);
      SiERROR("IQ not possible with the current standard\n");
      return 0;
      break;
   }
  }

  if (return_code != NO_Si2168B_ERROR) return 0;

  value = (front_end->demod->rsp->dd_get_reg.data1<< 0)
        + (front_end->demod->rsp->dd_get_reg.data2<< 8)
        + (front_end->demod->rsp->dd_get_reg.data3<<16)
        + (front_end->demod->rsp->dd_get_reg.data4<<24);

  sign_bit = ishift -1;

  ival = (value & imask) >> ishift;
  if (ival >> sign_bit) { ival = ival - limit; }

  qval = (value & qmask);
  if (qval >> sign_bit) { qval = qval - limit; }

  *i = ival*iq_coef;
  *q = qval*iq_coef;

  return 1;
}
/************************************************************************************************************************
  Si2168B_L2_Test function
  Use:        Generic test pipe function
              Used to send a generic command to the selected target.
  Returns:    0 if the command is unknow, 1 otherwise
  Porting:    Mostly used for debug during sw development.
************************************************************************************************************************/
int   Si2168B_L2_Test                     (Si2168B_L2_Context *front_end, char *target, char *cmd, char *sub_cmd, double dval, double *retdval, char **rettxt)
{
  int i,c,p;
  int passthru_enable;
  char c2InfoText[1000];
  unsigned char testBufferBytes[100];
  char *c2Text;
  unsigned char *testBuffer;
  int start_time_ms;
#ifdef   _DVB_T2_SIGNALLING_H_
  SiLabs_T2_Misc_Signalling    misc;
  SiLabs_T2_L1_Post_Signalling post;
  SiLabs_T2_L1_Pre_Signalling  pre;
#endif /* _DVB_T2_SIGNALLING_H_ */
  testBuffer = &testBufferBytes[0];
  c2Text     = &c2InfoText[0];
  front_end = front_end; /* To avoid compiler warning if not used */
  *retdval = 0;
  start_time_ms = system_time();
  SiTRACE("\nSi2168B_L2_Test %s %s %s %f...\n", target, cmd, sub_cmd, dval);
  sprintf(*rettxt, "%s", "");
       if (strcmp_nocase(target,"help"      ) == 0) {
    sprintf(*rettxt, "\n Possible Si2168B test commands:\n\
demod properties                       : displays ALL current property fields\n\
demod getProperty <prop_code>          : displays current property fields\n\
demod download    <always/on_change>   : selects the property download mode\n\
demod address     <address>            : changes or display i2c address\n\
demod fef         <0-1>                : set FEF mode\n");
    sprintf(*rettxt, "%s\
demod passthru    <enable/disable/''>  : changes or display i2c passthrough state\n\
demod ber         <mant/exp> <dval/''> : changes ber mant or exp and display their values\n\
demod ber         <loop>               : BER reset followed by BER convergence test\n", *rettxt);
    sprintf(*rettxt, "%s\
demod get_reg     <reg_code>           : call DD_GET_REG and display value\n\
demod set_reg     <reg_code> <dval>    : call DD_SET_REG and display value\n\
demod get_rev                          : display part and fw info\n\
demod part_info                        : displays part info\n\
demod code_version                     : displays code version\n", *rettxt);
    sprintf(*rettxt, "%s\
demod iq          <loop>     <dval>    : read (int)dval constellation points\n\
demod mer                              : read MER\n\
demod mplp        <freq>               : lock on 'freq' and trace PLP info\n\
demod spi_download <enable/disable> <api_send_option> : enable/disable SPI download (if available)\n\
demod spi_regs                         : displays SPI registers\n\
demod health_check                     : displays registers interesting for health checking\n\
demod handshake  infos                 : displays current handshake settings\n\
demod handshake  used   <0/1>          : controls front_end->handshakeUsed\n\
demod handshake  period <period_ms>    : controls front_end->handshakePeriod\n\
demod clock_mode ter <clock_mode>      : controls front_end->demod->tuner_ter_clock_input\n\
demod clock_mode ter <clock_freq>      : controls front_end->demod->tuner_ter_clock_freq\n\
demod clock_mode ter <clock_mode>      : controls front_end->demod->tuner_sat_clock_input\n\
demod clock_mode sat <clock_freq>      : controls front_end->demod->tuner_sat_clock_freq\n\
", *rettxt);
#ifdef    _Si2168B_L2_Get_Echoes_Info_
    sprintf(*rettxt, "%s\
demod echoes                           : displays the x and y echo values\n", *rettxt);
#endif /* _Si2168B_L2_Get_Echoes_Info_ */
#ifdef    Si2168B_L2_DUMP_CODE
    sprintf(*rettxt, "%s\
demod dump                              : dumps all register values\n", *rettxt);
#endif /* Si2168B_L2_DUMP_CODE */
#ifdef    _DVB_T2_SIGNALLING_H_
    sprintf(*rettxt, "%s\
demod l1_misc                          : fills the T2 signaling 'L1 misc' structure and displays it\n\
demod l1_pre                           : fills the T2 signaling 'L1 Pre'  structure and displays it\n\
demod l1_post                          : fills the T2 signaling 'L1 Post' structure and displays it\n\
demod dektec                           : uses  the T2 signaling structures to prepare a Dektec configuration string and displays it\n", *rettxt);
#endif /* _DVB_T2_SIGNALLING_H_ */
 return 1;
  }
  else if (strcmp_nocase(target,"demod"     ) == 0) {
#ifdef    Si2168B_SET_PROPERTY_FIELDS
          Si2168B_SET_PROPERTY_FIELDS
#endif /* Si2168B_SET_PROPERTY_FIELDS */
  #ifdef    Si2168B_GET_PROPERTY_STRING
    if (strcmp_nocase(cmd,"properties"  ) == 0) {
      //SiTraceConfiguration("traces suspend");
      c = 0;
      for (i=0x0301; i<=0x030a; i++) { if (Si2168B_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168B_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x0401; i<=0x0401; i++) { if (Si2168B_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168B_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1001; i<=0x1010; i++) { if (Si2168B_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168B_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1101; i<=0x1104; i++) { if (Si2168B_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168B_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1201; i<=0x1203; i++) { if (Si2168B_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168B_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1301; i<=0x1304; i++) { if (Si2168B_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168B_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1401; i<=0x1403; i++) { if (Si2168B_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168B_ERROR) {printf("%s\n",*rettxt); c++;} }
      for (i=0x1501; i<=0x1503; i++) { if (Si2168B_L1_GetPropertyString(front_end->demod, i, (char *)" ", *rettxt) ==NO_Si2168B_ERROR) {printf("%s\n",*rettxt); c++;} }
      //SiTraceConfiguration("traces resume");
      sprintf(*rettxt, "%d properties checked\n", c);
      return 1;
    }
    if (strcmp_nocase(cmd,"getProperty" ) == 0) {
       *retdval = (double)Si2168B_L1_GetPropertyString(front_end->demod, dval, (char *)" ", *rettxt); return 1;
    }
  #endif /* Si2168B_GET_PROPERTY_STRING */
    if (strcmp_nocase(cmd,"address"     ) == 0) {
      if ( (dval >= 0xc8) & (dval <= 0xce) ) {front_end->demod->i2c->address = (int)dval; }
       *retdval = (double)front_end->demod->i2c->address; sprintf(*rettxt, "0x%02x", (int)*retdval ); return 1;
    }
    if (strcmp_nocase(cmd,"fef"         ) == 0) {
     #ifdef    INDIRECT_I2C_CONNECTION
       front_end->f_TER_tuner_enable(front_end->callback);
     #else  /* INDIRECT_I2C_CONNECTION */
       Si2168B_L2_Tuner_I2C_Enable(front_end);
     #endif /* INDIRECT_I2C_CONNECTION */
       *retdval = (double)Si2168B_L2_TER_FEF(front_end, dval);
     #ifdef    INDIRECT_I2C_CONNECTION
       front_end->f_TER_tuner_disable(front_end->callback);
     #else  /* INDIRECT_I2C_CONNECTION */
       Si2168B_L2_Tuner_I2C_Disable(front_end);
     #endif /* INDIRECT_I2C_CONNECTION */
       sprintf(*rettxt, "FEF %.0f\n", dval);
        return 1;
    }
#ifdef    _Si2168B_L2_Get_Echoes_Info_
    int echoes_x[6];
    int echoes_y[6];
    if (strcmp_nocase(cmd,"echoes"    ) == 0) {
      *retdval = (double)Si2168B_L2_Get_Echoes_Info(front_end, echoes_x, echoes_y);
      if ((int)*retdval != 0) {
        sprintf(*rettxt, " x_echoes ");
        for (i=0; i<(int)*retdval; i++) { sprintf (*rettxt, "%s%8d ", *rettxt, echoes_x[i]); }
        strcat(*rettxt, "\n y_echoes ");
        for (i=0; i<(int)*retdval; i++) { sprintf (*rettxt, "%s%8d ", *rettxt, echoes_y[i]); }
      }
      return 1;
    }
#endif /* _Si2168B_L2_Get_Echoes_Info_ */
    if (strcmp_nocase(cmd,"passthru"    ) == 0) {
      if (strcmp_nocase(sub_cmd,"enable" ) == 0) { Si2168B_L2_Tuner_I2C_Enable (front_end); }
      if (strcmp_nocase(sub_cmd,"disable") == 0) { Si2168B_L2_Tuner_I2C_Disable(front_end); }
      testBuffer[0] = 0xff;
      testBuffer[1] = 0x00;
      L0_WriteCommandBytes(front_end->demod->i2c, 2, testBuffer);
      testBuffer[0] = 0xc0;
      testBuffer[1] = 0x0d;
      L0_WriteCommandBytes(front_end->demod->i2c, 2, testBuffer);
      L0_ReadBytes        (front_end->demod->i2c, 0xc00d, 1, testBuffer);
      passthru_enable = testBuffer[0]&0x01;
      testBuffer[0] = 0xfe;
      testBuffer[1] = 0x00;
      L0_WriteCommandBytes(front_end->demod->i2c, 2, testBuffer);
      *retdval = (double)passthru_enable;
      sprintf(*rettxt, "passthru enable %d\n", passthru_enable);
      return 1;
    }
    if (strcmp_nocase(cmd,"download"    ) == 0) {
      if (strcmp_nocase(sub_cmd,"always"   ) == 0) { front_end->demod->propertyWriteMode = Si2168B_DOWNLOAD_ALWAYS;    }
      if (strcmp_nocase(sub_cmd,"on_change") == 0) { front_end->demod->propertyWriteMode = Si2168B_DOWNLOAD_ON_CHANGE; }
      *retdval = (double)front_end->demod->propertyWriteMode;
      if (front_end->demod->propertyWriteMode == Si2168B_DOWNLOAD_ALWAYS   ) {sprintf(*rettxt, "download always"   );}
      if (front_end->demod->propertyWriteMode == Si2168B_DOWNLOAD_ON_CHANGE) {sprintf(*rettxt, "download on_change");}
      return 1;
    }
    if (strcmp_nocase(cmd,"ber"         ) == 0) {
     if (strcmp_nocase(sub_cmd,"mant" ) == 0) {
      i = (int)dval;
      if (i !=0) {
       front_end->demod->prop->dd_ber_resol.mant                      =     (int) dval;
       Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_BER_RESOL_PROP_CODE);
      }
      *retdval = (double)front_end->demod->prop->dd_ber_resol.mant;
      sprintf(*rettxt, "BER mant %.0f\n", *retdval);
     }
     if (strcmp_nocase(sub_cmd,"exp"  ) == 0) {
      i = (int)dval;
      if (i !=0) {
       front_end->demod->prop->dd_ber_resol.exp                       =     (int) dval;
       Si2168B_L1_SetProperty2(front_end->demod, Si2168B_DD_BER_RESOL_PROP_CODE);
      }
      *retdval = (double)front_end->demod->prop->dd_ber_resol.exp;
      sprintf(*rettxt, "BER exp  %.0f\n", *retdval);
     }
     if (strcmp_nocase(sub_cmd,"loop" ) == 0) {
       Si2168B_L1_DD_BER(front_end->demod, Si2168B_DD_BER_CMD_RST_CLEAR);
       printf("BER CLEAR\n");
       for (i=0; i< (int)dval; i++) {
        Si2168B_L1_DD_BER(front_end->demod, Si2168B_DD_BER_CMD_RST_RUN);
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
       sscanf(sub_cmd, "%d",&c);
       Si2168B_L1_DD_GET_REG(front_end->demod, (c>>16)&0xff, (c>>8)&0xff, (c>>0)&0xff );
       sprintf(*rettxt, "%d\n",
                (front_end->demod->rsp->dd_get_reg.data4<<24)
               +(front_end->demod->rsp->dd_get_reg.data3<<16)
               +(front_end->demod->rsp->dd_get_reg.data2<<8 )
               +(front_end->demod->rsp->dd_get_reg.data1<<0 )
        );
        return 1;
    }
    if (strcmp_nocase(cmd,"set_reg"     ) == 0) {
       i = (int)dval;
       sscanf(sub_cmd, "%d",&c);
       Si2168B_L1_DD_SET_REG(front_end->demod, (c>>16)&0xff, (c>>8)&0xff, (c>>0)&0xff, i );
       Si2168B_L1_DD_GET_REG(front_end->demod, (c>>16)&0xff, (c>>8)&0xff, (c>>0)&0xff );
       sprintf(*rettxt, "%d\n",
                (front_end->demod->rsp->dd_get_reg.data4<<24)
               +(front_end->demod->rsp->dd_get_reg.data3<<16)
               +(front_end->demod->rsp->dd_get_reg.data2<<8 )
               +(front_end->demod->rsp->dd_get_reg.data1<<0 )
        );
        return 1;
    }
    if (strcmp_nocase(cmd,"get_rev"     ) == 0) {
       Si2168B_L1_GET_REV(front_end->demod);
       sprintf(*rettxt,"Si21%02d", front_end->demod->rsp->get_rev.pn);
              if ((front_end->demod->rsp->get_rev.mcm_die) != Si2168B_GET_REV_RESPONSE_MCM_DIE_SINGLE) {
       sprintf(*rettxt,"%s2", *rettxt);
       }
              if (front_end->demod->rsp->get_rev.chiprev == Si2168B_PART_INFO_RESPONSE_CHIPREV_A) {
       sprintf(*rettxt,"%s A", *rettxt);
       } else if (front_end->demod->rsp->get_rev.chiprev == Si2168B_PART_INFO_RESPONSE_CHIPREV_B) {
       sprintf(*rettxt,"%s B", *rettxt);
       } else {
       sprintf(*rettxt,"%s chiprev %d",*rettxt, front_end->demod->rsp->get_rev.chiprev);
       }
       sprintf(*rettxt,"%s ROM %d NVM %c_%cb%d ", *rettxt
              , front_end->demod->rsp->part_info.romid
              , front_end->demod->rsp->part_info.pmajor
              , front_end->demod->rsp->part_info.pminor
              , front_end->demod->rsp->part_info.pbuild
               );
              if (front_end->demod->rsp->get_rev.mcm_die == Si2168B_GET_REV_RESPONSE_MCM_DIE_DIE_A) {
       sprintf(*rettxt,"%s die A", *rettxt);
       } else if (front_end->demod->rsp->get_rev.mcm_die == Si2168B_GET_REV_RESPONSE_MCM_DIE_DIE_B) {
       sprintf(*rettxt,"%s die B", *rettxt);
       }
       sprintf(*rettxt,"%s Running FW %c_%cb%d \n", *rettxt
              , front_end->demod->rsp->get_rev.cmpmajor
              , front_end->demod->rsp->get_rev.cmpminor
              , front_end->demod->rsp->get_rev.cmpbuild
               );
       return 1;
    }
    if (strcmp_nocase(cmd,"part_info"   ) == 0) {
     if ((front_end->demod->rsp->get_rev.mcm_die) != Si2168B_GET_REV_RESPONSE_MCM_DIE_SINGLE) {
       sprintf(*rettxt, "Full Info 'Si21%02d%d-%c%c%c ROM%x NVM%c_%cb%d'\n", front_end->demod->rsp->part_info.part, 2, front_end->demod->rsp->part_info.chiprev + 0x40, front_end->demod->rsp->part_info.pmajor, front_end->demod->rsp->part_info.pminor, front_end->demod->rsp->part_info.romid, front_end->demod->rsp->part_info.pmajor, front_end->demod->rsp->part_info.pminor, front_end->demod->rsp->part_info.pbuild );
     } else {
       sprintf(*rettxt, "Full Info 'Si21%02d-%c%c%c ROM%x NVM%c_%cb%d'\n", front_end->demod->rsp->part_info.part, front_end->demod->rsp->part_info.chiprev + 0x40, front_end->demod->rsp->part_info.pmajor, front_end->demod->rsp->part_info.pminor, front_end->demod->rsp->part_info.romid, front_end->demod->rsp->part_info.pmajor, front_end->demod->rsp->part_info.pminor, front_end->demod->rsp->part_info.pbuild );
     }
     return 1;
    }
    if (strcmp_nocase(cmd,"iq"          ) == 0) {
      sprintf(*rettxt, "  I    Q \n");
      if (strcmp_nocase(sub_cmd,"loop"  ) == 0) {
        for (i=0; i< (int)dval; i++) {
          Si2168B_L2_Get_Constellation_IQ(front_end, &c, &p);
          sprintf(*rettxt, "%s %4d %4d\n", *rettxt, c, p );
        }
      } else {
          Si2168B_L2_Get_Constellation_IQ(front_end, &c, &p);
          sprintf(*rettxt, "%s %4d %4d\n", *rettxt, c, p );
      }
       return 1;
    }
    if (strcmp_nocase(cmd,"rx_type"     ) == 0) {
      Si2168B_L2_Get_Constellation_IQ_rx_type ( front_end, (unsigned char)dval);
      *retdval = (double)Si2168B_iq_demap;
      sprintf(*rettxt, "iq_demap %d\n", Si2168B_iq_demap);
      return 1;
    }
    if (strcmp_nocase(cmd,"code_version") == 0) {
     sprintf(*rettxt, "Code %s\n", Si2168B_L1_API_TAG_TEXT() );
     return 1;
    }
    if (strcmp_nocase(cmd,"mplp"        ) == 0) {
     i = (int)dval;
     sprintf(*rettxt, "%d DATA streams\n", Si2168B_L2_Test_MPLP(front_end, i) );
     return 1;
    }
    if (strcmp_nocase(cmd,"spi_download") == 0) {
      if (strcmp_nocase(sub_cmd,"enable" ) == 0) { front_end->demod->spi_download = 1; front_end->demod->spi_send_option = (int) dval; }
      if (strcmp_nocase(sub_cmd,"disable") == 0) { front_end->demod->spi_download = 0; }
      *retdval = (double)front_end->demod->spi_download;
      sprintf(*rettxt, "spi_download %d\n", front_end->demod->spi_download );
      return 1;
    }
    if (strcmp_nocase(cmd,"spi_regs"    ) == 0) {
      testBuffer[0]=0xff; testBuffer[1]=0x00; L0_WriteCommandBytes(front_end->demod->i2c, 2, testBuffer);
      front_end->demod->i2c->indexSize  = 2;
      sprintf(*rettxt, "spi_crc_status %ld spi_state %ld ", L0_ReadRegister(front_end->demod->i2c, 0x55, 0, 1, 0) , L0_ReadRegister(front_end->demod->i2c, 0x57, 0, 3, 0));
      front_end->demod->i2c->indexSize  = 0;
      testBuffer[0]=0xfe; testBuffer[1]=0x00; L0_WriteCommandBytes(front_end->demod->i2c, 2, testBuffer);
      return 1;
    }
    if (strcmp_nocase(cmd,"health_check") == 0) {
      Si2168B_L2_Health_Check(front_end);
      return 1;
    }
#ifdef    Si2168B_L2_DUMP_CODE
    if (strcmp_nocase(cmd,"dump"        ) == 0) {
      Si2168B_L2_DUMP(front_end);
      return 1;
    }
#endif /* Si2168B_L2_DUMP_CODE */
    if (strcmp_nocase(cmd,"handshake"   ) == 0) {
      i = (int)dval;
      if (strcmp_nocase(sub_cmd,"infos" ) != 0) {
        if (strcmp_nocase(sub_cmd,"used"   ) == 0) {
          if (i!=0) {
            front_end->handshakeUsed = 1;
          } else {
            front_end->handshakeUsed = 0;
          }
        }
        if (strcmp_nocase(sub_cmd,"period" ) == 0) {
          if (i!=0) {
            front_end->handshakePeriod_ms = i;
          }
        }
      }
      sprintf(*rettxt, "handshakeUsed %d, handshakePeriod %d \n", front_end->handshakeUsed, front_end->handshakePeriod_ms );
      return 1;
    }
    if (strcmp_nocase(cmd,"clock_mode"  ) == 0) {
      i = (int)dval;
      if (strcmp_nocase(sub_cmd,"ter"   ) == 0) {
        if (i != 0) { front_end->demod->tuner_ter_clock_input = i;}
        sprintf(*rettxt, "front_end->demod->tuner_ter_clock_input %d\n", front_end->demod->tuner_ter_clock_input );
      }
      return 1;
    }
    if (strcmp_nocase(cmd,"clock_freq"  ) == 0) {
      i = (int)dval;
      if (strcmp_nocase(sub_cmd,"ter"   ) == 0) {
        if (i != 0) { front_end->demod->tuner_ter_clock_freq = i; }
        sprintf(*rettxt, "front_end->demod->tuner_ter_clock_freq %d\n", front_end->demod->tuner_ter_clock_freq );
      }
      return 1;
    }
    if (strcmp_nocase(cmd,"check_loop"  ) == 0) {
      Si2168B_L2_CheckLoop (front_end);
      sprintf(*rettxt, "check_loop complete\n");
      return 1;
    }
#ifdef   _DVB_T2_SIGNALLING_H_
    if (strcmp_nocase(cmd,"mplp"        ) == 0) {
       i = (int)dval;
       sprintf(*rettxt, "%d DATA streams\n", Si2168B_L2_Test_MPLP(front_end, i) );
       return 1;
    }
    if (strcmp_nocase(cmd,"l1_misc"     ) == 0) {
       Si2168B_L2_Read_L1_Misc_Data (front_end, &misc);
       SiLabs_L1_Misc_Text         (&misc, *rettxt);
       return 1;
    }
    if (strcmp_nocase(cmd,"l1_pre"      ) == 0) {
       Si2168B_L2_Read_L1_Pre_Data (front_end, &pre);
       SiLabs_L1_Pre_Text         (&pre, *rettxt);
       return 1;
    }
    if (strcmp_nocase(cmd,"l1_post"     ) == 0) {
       Si2168B_L2_Read_L1_Post_Data (front_end, &post);
       SiLabs_L1_Post_Text         (&post, *rettxt);
       return 1;
    }
    if (strcmp_nocase(cmd,"dektec"      ) == 0) {
      if (strcmp_nocase(sub_cmd,"full") == 0) {
        Si2168B_L2_Read_L1_Misc_Data (front_end, &misc);
        Si2168B_L2_Read_L1_Pre_Data  (front_end, &pre);
        Si2168B_L2_Read_L1_Post_Data (front_end, &post);
      }
      if (strcmp_nocase(sub_cmd,"loop") == 0) {
        while (1) {
          Si2168B_L2_Read_L1_Misc_Data (front_end, &misc);
          Si2168B_L2_Read_L1_Pre_Data  (front_end, &pre);
          Si2168B_L2_Read_L1_Post_Data (front_end, &post);
          SiLabs_DekTec_T2_Configuration (&pre, &post, &misc, *rettxt);
          system("cls");
          printf("%s\n", *rettxt);
          system_wait((int)dval);
        }
      }
      SiLabs_DekTec_T2_Configuration (&pre, &post, &misc, *rettxt);
      return 1;
    }
#endif /* _DVB_T2_SIGNALLING_H_ */
    sprintf(*rettxt, "unknown '%s' demod command for Si2168B, can not process '%s %s %s %f'\n", cmd, target, cmd, sub_cmd, dval);
    return 0;
  }
  else if (strcmp_nocase(target,"ter_tuner" ) == 0) {
   #ifdef    INDIRECT_I2C_CONNECTION
    front_end->f_TER_tuner_enable(front_end->callback);
   #else  /* INDIRECT_I2C_CONNECTION */
    Si2168B_L2_Tuner_I2C_Enable(front_end);
   #endif /* INDIRECT_I2C_CONNECTION */
    i = 0;
    if (strcmp_nocase(cmd,"init_done"       ) == 0) { i++;
      if ((int)dval >= 1) {front_end->TER_tuner_init_done = 1;}
      if ((int)dval == 0) {front_end->TER_tuner_init_done = 0;}
      sprintf(*rettxt, "front_end->TER_tuner_init_done %d\n", front_end->TER_tuner_init_done);
    }
    if (strcmp_nocase(cmd,"clock_on"        ) == 0) { i++;
      #ifdef    TER_TUNER_CLOCK_ON
      sprintf(*rettxt, "TER_TUNER_CLOCK_ON %d\n", TER_TUNER_CLOCK_ON(front_end->tuner_ter));
      #endif /* TER_TUNER_CLOCK_ON */
    }
    if (strcmp_nocase(cmd,"clock_off"       ) == 0) { i++;
      #ifdef    TER_TUNER_CLOCK_OFF
      sprintf(*rettxt, "TER_TUNER_CLOCK_OFF %d\n", TER_TUNER_CLOCK_OFF(front_end->tuner_ter));
      #endif /* TER_TUNER_CLOCK_OFF */
    }
    if (strcmp_nocase(cmd,"init_after_reset") == 0) { i++;
      #ifdef    TER_TUNER_INIT
      sprintf(*rettxt, "TER_TUNER_INIT %d\n", TER_TUNER_INIT(front_end->tuner_ter));
      #endif /* TER_TUNER_INIT */
    }
    if (strcmp_nocase(cmd,"standby"         ) == 0) { i++;
      #ifdef    TER_TUNER_STANDBY
      sprintf(*rettxt, "TER_TUNER_STANDBY %d\n", TER_TUNER_STANDBY(front_end->tuner_ter));
      #endif /* TER_TUNER_STANDBY */
    }
    if (strcmp_nocase(cmd,"wakeup"          ) == 0) { i++;
      #ifdef    TER_TUNER_WAKEUP
      sprintf(*rettxt, "TER_TUNER_WAKEUP %d\n", TER_TUNER_WAKEUP(front_end->tuner_ter));
      #endif /* TER_TUNER_WAKEUP */
    }
  #ifdef    TER_TUNER_SILABS
    if (i==0) {
      SiLabs_TER_Tuner_Test(front_end->tuner_ter, target, cmd, sub_cmd, dval, retdval, rettxt);
    }
  #endif /* TER_TUNER_SILABS */
   #ifdef    INDIRECT_I2C_CONNECTION
    front_end->f_TER_tuner_disable(front_end->callback);
   #else  /* INDIRECT_I2C_CONNECTION */
    Si2168B_L2_Tuner_I2C_Disable(front_end);
   #endif /* INDIRECT_I2C_CONNECTION */
    if (strlen(*rettxt)!=0) { return 1; }
    sprintf(*rettxt, "no ter_tuner command implemented so far for Si2168B, can not process '%s %s %s %f'\n", target, cmd, sub_cmd, dval);
    return 0;
  }
  else if (strcmp_nocase(target,"sat_tuner" ) == 0) {
    sprintf(*rettxt, "Can not process '%s %s %s %f'\n", target, cmd, sub_cmd, dval);
    return 0;
  }
  else if (strcmp_nocase(target,"lnb_supply") == 0) {
    sprintf(*rettxt, "no lnb_supply command implemented so far for Si2168B, can not process '%s %s %s %f'\n", target, cmd, sub_cmd, dval);
    return 0;
  }
  else {
    sprintf(*rettxt, "unknown '%s' command for Si2168B, can not process '%s %s %s %f'\n", cmd, target, cmd, sub_cmd, dval);
    SiTRACE(*rettxt);
    SiERROR(*rettxt);
    return 0;
  }
  return 0;
}
#endif /* SILABS_API_TEST_PIPE */

/**/



