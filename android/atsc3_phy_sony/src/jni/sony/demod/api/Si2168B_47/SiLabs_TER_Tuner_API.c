/***************************************************************************************
                  Silicon Laboratories Broadcast SiLabs_TER_Tuner API
              for easy control of Silicon Laboratories Terrestrial tuners.

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   FILE: SiLabs_TER_Tuner_API.c

  Basically, this is a 'tuner hub' layer, allowing the user to control any supported tuner from a single API.

  To use this layer, add the code to the project and define TER_TUNER_SILABS
  To include one supported tuner in the application, define TER_TUNER_Si21xx
  To use several different tuners, define several TER_TUNER_Sixx
  To use several identical tuners, use Si21xx_TUNER_COUNT

  The current tuner in use is selectable using
    SiLabs_TER_Tuner_Select_Tuner (SILABS_TER_TUNER_Context *silabs_tuner, int ter_tuner_code, int tuner_index);
    Where:
     - ter_tuner_code can be 0x2147, ox2157, 0x2177 in the initial version.
       (This syntax will allow 0x21xxa to 0x21xxf later on).
     - tuner_index is the index when using several identical tuners of the same type

  __________________________________________________________________________________________________________________________________________

  Example use case for a dual front-end with each front-end supporting a Si2147 providing its clock to a Si2177, which also outputs a clock:
  __________________________________________________________________________________________________________________________________________

   Compilation flags:

     TER_TUNER_SILABS
     TER_TUNER_Si2147
     Si2147_TUNER_COUNT 2
     TER_TUNER_Si2177
     Si2177_TUNER_COUNT 2

   SW init:

      SILABS_TER_TUNER_Context  *ter_tuner;
      SILABS_TER_TUNER_Context   ter_tunerObj;
      ter_tuner = &ter_tunerObj;

      SiLabs_TER_Tuner_SW_Init (ter_tuner, 0xc0);

      ter_tuner->Si2147_Tuner[0]->i2c->address = 0xc0;
      ter_tuner->Si2147_Tuner[0]->cmd->power_up.clock_mode      = Si2147_POWER_UP_CMD_CLOCK_MODE_XTAL;
      ter_tuner->Si2147_Tuner[0]->cmd->power_up.en_xout         = Si2147_POWER_UP_CMD_EN_XOUT_EN_XOUT;
      ter_tuner->Si2147_Tuner[0]->cmd->config_clocks.clock_mode = Si2147_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;

      ter_tuner->Si2177_Tuner[0]->i2c->address = 0xc2;
      ter_tuner->Si2177_Tuner[0]->cmd->power_up.clock_mode      = Si2147_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
      ter_tuner->Si2177_Tuner[0]->cmd->power_up.en_xout         = Si2147_POWER_UP_CMD_EN_XOUT_EN_XOUT;
      ter_tuner->Si2177_Tuner[0]->cmd->config_clocks.clock_mode = Si2147_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK;

      ter_tuner->Si2147_Tuner[1]->i2c->address = 0xc4;
      ter_tuner->Si2147_Tuner[1]->cmd->power_up.clock_mode      = Si2147_POWER_UP_CMD_CLOCK_MODE_XTAL;
      ter_tuner->Si2147_Tuner[1]->cmd->power_up.en_xout         = Si2147_POWER_UP_CMD_EN_XOUT_EN_XOUT;
      ter_tuner->Si2147_Tuner[1]->cmd->config_clocks.clock_mode = Si2147_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;

      ter_tuner->Si2177_Tuner[1]->i2c->address = 0xc6;
      ter_tuner->Si2177_Tuner[1]->cmd->power_up.clock_mode      = Si2147_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
      ter_tuner->Si2177_Tuner[1]->cmd->power_up.en_xout         = Si2147_POWER_UP_CMD_EN_XOUT_EN_XOUT;
      ter_tuner->Si2177_Tuner[1]->cmd->config_clocks.clock_mode = Si2147_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK;

   HW init:

      SiLabs_TER_Tuner_Select_Tuner (ter_tuner, 0x2147, 0);
      SiLabs_TER_Tuner_HW_Connect   (ter_tuner, mode);
      SiLabs_TER_Tuner_HW_Init      (ter_tuner);

      SiLabs_TER_Tuner_Select_Tuner (ter_tuner, 0x2177, 0);
      SiLabs_TER_Tuner_HW_Connect   (ter_tuner, mode);
      SiLabs_TER_Tuner_HW_Init      (ter_tuner);

      SiLabs_TER_Tuner_Select_Tuner (ter_tuner, 0x2147, 1);
      SiLabs_TER_Tuner_HW_Connect   (ter_tuner, mode);
      SiLabs_TER_Tuner_HW_Init      (ter_tuner);

      SiLabs_TER_Tuner_Select_Tuner (ter_tuner, 0x2177, 1);
      SiLabs_TER_Tuner_HW_Connect   (ter_tuner, mode);
      SiLabs_TER_Tuner_HW_Init      (ter_tuner);

   HINT: When using several different tuners, after SiLabs_TER_Tuner_SW_Init the current tuner is the first one in the arrays

   Tag:  V0.2.6
   Date: May 15 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/
/* Change log: */
/* Last changes:

 As from V0.3.6:
  Adding Si2141 and Si2151 TER tuners

 As from V0.3.5:
  Adding CUSTOMTER, Si2148B, Si2158B and Si2191B TER tuners

 As from V0.3.4:
  Adding fef_level in SILABS_TER_TUNER_Context
  In SiLabs_TER_Tuner_FEF_FREEZE_PIN: correctly taking into account active fef level according to value of silabs_tuner->fef_level
  In SiLabs_TER_Tuner_FEF_FREEZE_PIN_SETUP: disabling GPIO1 (driving LOW by default) to prevent conflict when GPIO1 is used as FEF input signal
                                            storing the fef_level used on the demodulator side, to properly
                                             set dtv_agc_freeze_input.level in SiLabs_TER_Tuner_FEF_FREEZE_PIN

 As from V0.3.3:
  In SiLabs_TER_Tuner_DTV_AGC_AUTO_FREEZE: adding code to avoid compilation warnings when the selected tuners don't support this feature.

 As from V0.3.2:
  <new_feature> Adding SiLabs_TER_Tuner_DTV_AGC_AUTO_FREEZE (only available for Si21x8X TER tuners)
   This helps when locking on DVBT2_Lite signals with FEF
  <improvement> [code_checkers] In text-based functions using sprintf, replacing sprintf by sprintf with a size of 1000.
  <improvement> [warnings] In SiLabs_ATV_Channel_Scan_M and SiLabs_ATV_Channel_Scan_PAL: adding lines to avoid compilation warning when some parameters are not used.

 As from V0.3.1:
  <new feature> Adding SiLabs_TER_Tuner_ClockConfig, to easily configure the TER tuner clock:
    int   SiLabs_API_TER_Tuner_ClockConfig    (SILABS_FE_Context *front_end, int xtal, int xout);
     xtal = 1: a Xtal is connected to and driven by the TER tuner.
     xtal = 0: a clock signal is connected to the TER tuner, which doesn't drive a Xtal.
     xout = 1: the clock is going out of the TER tuner.
     xout = 1: no  clock is going out of the TER tuner.

 As from V0.3.0:
  <new feature> Adding SiLabs_TER_Tuner_Broadcast_I2C (only for Si2178B in this first version)

 As from V0.2.9:
  In SiLabs_TER_Tuner_DTV_LIF_OUT: correctly taking into account offset for Si2196

 As from V0.2.8:
  In SiLabs_TER_Tuner_Fine_Tune/SiLabs_TER_Tuner_Block_VCO_Code: modifying SiTRACE to match %ld type for freq values.

 As from V0.2.7:
  In SiLabs_TER_Tuner_DTV_LIF_OUT_amp:
   Correction of amp values for Si21x6 tuners when used with recent demods

 As from V0.2.6:
 .Compatibility with Si2146/47/48, Si2156/57/58, Si2173/76/77/78, Si2190/91/96 Si2178B all over the TER tuner wrapper

 As from V0.2.5:
 .Compatibility with Si2146/47/48, Si2156/57/58, Si2173/76/77/78, Si2190/91/96 all over the TER tuner wrapper

 As from V0.2.4:
 .Compatibility with Si2146/47/48, Si2156/57/58, Si2173/76/77/78, Si2190/91/96

 As from V0.2.3:
  Adding SiLabs_TER_Tuner_Fine_Tune
  Adding SiLabs_TER_Tuner_Text_STATUS
  Adding SiLabs_TER_Tuner_ATV_Text_STATUS
  Adding SiLabs_TER_Tuner_DTV_Text_STATUS

 As from V0.2.2:
  Adding SiLabs_TER_Tuner_DTV_LIF_OUT_amp, to set DTV LIF OUT AMP according to the tuner and demod used

 As from V0.2.1:
  SiLabs tuners supported:
    Si2146  Si2147  Si2148  Si2156  Si2157  Si2158  Si2176  Si2177  Si2178  Si2190  Si2191  Si2196
  Adding SiLabs_TER_Tuner_DTV_LIF_OUT function, required to set LIF amp differently between 'cable' and 'terrestrial'
   int   SiLabs_TER_Tuner_DTV_LIF_OUT          (SILABS_TER_TUNER_Context *silabs_tuner, int amp, int offset);

 As from V0.2.0:
  Adding several tuner functions:
    int   SiLabs_TER_Tuner_DTV_LOInjection      (SILABS_TER_TUNER_Context *silabs_tuner);
    int   SiLabs_TER_Tuner_ATV_LOInjection      (SILABS_TER_TUNER_Context *silabs_tuner);
    int   SiLabs_TER_Tuner_FEF_FREEZE_PIN_SETUP (SILABS_TER_TUNER_Context *silabs_tuner);
    int   SiLabs_TER_Tuner_FEF_FREEZE_PIN       (SILABS_TER_TUNER_Context *silabs_tuner, int fef);
    int   SiLabs_TER_Tuner_SLOW_INITIAL_AGC     (SILABS_TER_TUNER_Context *silabs_tuner, int fef);
    int   SiLabs_TER_Tuner_SLOW_NORMAL_AGC_SETUP(SILABS_TER_TUNER_Context *silabs_tuner, int fef);
    int   SiLabs_TER_Tuner_SLOW_NORMAL_AGC      (SILABS_TER_TUNER_Context *silabs_tuner, int fef);

 As from V0.0.2:
  SiLabs tuners supported in initial version:
    Si2147     Si2157     Si2177

*/

#include "SiLabs_TER_Tuner_API.h"

int   SiLabs_TER_Tuner_SW_Init              (SILABS_TER_TUNER_Context *silabs_tuner, int add, Endeavour* endeavour, unsigned char chip, unsigned char i2cbus) {
  int i;
  int ter_tuner_code;
  #ifdef    SiTRACES
    char possible[1000];
  #endif /* SiTRACES */
  ter_tuner_code = 0;
#ifdef    TER_TUNER_CUSTOMTER
  if (ter_tuner_code ==0) {ter_tuner_code = CUSTOMTER_CODE;}
  for (i=0; i< CUSTOMTER_TUNER_COUNT; i++) {
    silabs_tuner->CUSTOMTER_Tuner[i] = &(silabs_tuner->CUSTOMTER_TunerObj[i]);
    L1_RF_CUSTOMTER_Init                (silabs_tuner->CUSTOMTER_Tuner[i], add);
  }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2141;}
  for (i=0; i< Si2141_TUNER_COUNT; i++) {
    silabs_tuner->Si2141_Tuner[i] = &(silabs_tuner->Si2141_TunerObj[i]);
    Si2141_L1_API_Init               (silabs_tuner->Si2141_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (ter_tuner_code ==0) {ter_tuner_code = 0x2146;}
  for (i=0; i< Si2146_TUNER_COUNT; i++) {
    silabs_tuner->Si2146_Tuner[i] = &(silabs_tuner->Si2146_TunerObj[i]);
    Si2146_L1_API_Init               (silabs_tuner->Si2146_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (ter_tuner_code ==0) {ter_tuner_code = 0x2147;}
  for (i=0; i< Si2147_TUNER_COUNT; i++) {
    silabs_tuner->Si2147_Tuner[i] = &(silabs_tuner->Si2147_TunerObj[i]);
    Si2147_L1_API_Init               (silabs_tuner->Si2147_Tuner[i], add, endeavour, chip, i2cbus);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2148;}
  for (i=0; i< Si2148_TUNER_COUNT; i++) {
    silabs_tuner->Si2148_Tuner[i] = &(silabs_tuner->Si2148_TunerObj[i]);
    Si2148_L1_API_Init               (silabs_tuner->Si2148_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2148B;}
  for (i=0; i< Si2148B_TUNER_COUNT; i++) {
    silabs_tuner->Si2148B_Tuner[i] = &(silabs_tuner->Si2148B_TunerObj[i]);
    Si2148B_L1_API_Init               (silabs_tuner->Si2148B_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2151;}
  for (i=0; i< Si2151_TUNER_COUNT; i++) {
    silabs_tuner->Si2151_Tuner[i] = &(silabs_tuner->Si2151_TunerObj[i]);
    Si2151_L1_API_Init               (silabs_tuner->Si2151_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (ter_tuner_code ==0) {ter_tuner_code = 0x2156;}
  for (i=0; i< Si2156_TUNER_COUNT; i++) {
    silabs_tuner->Si2156_Tuner[i] = &(silabs_tuner->Si2156_TunerObj[i]);
    Si2156_L1_API_Init               (silabs_tuner->Si2156_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2157;}
  for (i=0; i< Si2157_TUNER_COUNT; i++) {
    silabs_tuner->Si2157_Tuner[i] = &(silabs_tuner->Si2157_TunerObj[i]);
    Si2157_L1_API_Init               (silabs_tuner->Si2157_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2158;}
  for (i=0; i< Si2158_TUNER_COUNT; i++) {
    silabs_tuner->Si2158_Tuner[i] = &(silabs_tuner->Si2158_TunerObj[i]);
    Si2158_L1_API_Init               (silabs_tuner->Si2158_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2158B;}
  for (i=0; i< Si2158B_TUNER_COUNT; i++) {
    silabs_tuner->Si2158B_Tuner[i] = &(silabs_tuner->Si2158B_TunerObj[i]);
    Si2158B_L1_API_Init               (silabs_tuner->Si2158B_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (ter_tuner_code ==0) {ter_tuner_code = 0x2173;}
  for (i=0; i< Si2173_TUNER_COUNT; i++) {
    silabs_tuner->Si2173_Tuner[i] = &(silabs_tuner->Si2173_TunerObj[i]);
    Si2173_L1_API_Init               (silabs_tuner->Si2173_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (ter_tuner_code ==0) {ter_tuner_code = 0x2176;}
  for (i=0; i< Si2176_TUNER_COUNT; i++) {
    silabs_tuner->Si2176_Tuner[i] = &(silabs_tuner->Si2176_TunerObj[i]);
    Si2176_L1_API_Init               (silabs_tuner->Si2176_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2177;}
  for (i=0; i< Si2177_TUNER_COUNT; i++) {
    silabs_tuner->Si2177_Tuner[i] = &(silabs_tuner->Si2177_TunerObj[i]);
    Si2177_L1_API_Init               (silabs_tuner->Si2177_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2178;}
  for (i=0; i< Si2178_TUNER_COUNT; i++) {
    silabs_tuner->Si2178_Tuner[i] = &(silabs_tuner->Si2178_TunerObj[i]);
    Si2178_L1_API_Init               (silabs_tuner->Si2178_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2178B;}
  for (i=0; i< Si2178B_TUNER_COUNT; i++) {
    silabs_tuner->Si2178B_Tuner[i] = &(silabs_tuner->Si2178B_TunerObj[i]);
    Si2178B_L1_API_Init               (silabs_tuner->Si2178B_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2190;}
  for (i=0; i< Si2190_TUNER_COUNT; i++) {
    silabs_tuner->Si2190_Tuner[i] = &(silabs_tuner->Si2190_TunerObj[i]);
    Si2190_L1_API_Init               (silabs_tuner->Si2190_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2191;}
  for (i=0; i< Si2191_TUNER_COUNT; i++) {
    silabs_tuner->Si2191_Tuner[i] = &(silabs_tuner->Si2191_TunerObj[i]);
    Si2191_L1_API_Init               (silabs_tuner->Si2191_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (ter_tuner_code == 0) {ter_tuner_code = 0x2191B;}
  for (i=0; i< Si2191B_TUNER_COUNT; i++) {
    silabs_tuner->Si2191B_Tuner[i] = &(silabs_tuner->Si2191B_TunerObj[i]);
    Si2191B_L1_API_Init               (silabs_tuner->Si2191B_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (ter_tuner_code ==0) {ter_tuner_code = 0x2196;}
  for (i=0; i< Si2196_TUNER_COUNT; i++) {
    silabs_tuner->Si2196_Tuner[i] = &(silabs_tuner->Si2196_TunerObj[i]);
    Si2196_L1_API_Init               (silabs_tuner->Si2196_Tuner[i], add);
  }
#endif /* TER_TUNER_Si2196 */
  #ifdef    SiTRACES
    i = SiLabs_TER_Tuner_Possible_Tuners(silabs_tuner, possible);
    SiTRACE("%d possible TER tuners via SiLabs_TER_Tuner: %s\n", i, possible);
  #endif /* SiTRACES */
  if (ter_tuner_code != 0) { SiLabs_TER_Tuner_Select_Tuner (silabs_tuner, ter_tuner_code, 0); }
  return 0;
}
int   SiLabs_TER_Tuner_Select_Tuner         (SILABS_TER_TUNER_Context *silabs_tuner, int ter_tuner_code, int tuner_index) {
  SiTRACE("Select TER Tuner selecting Si%04x_Tuner[%d]\n", ter_tuner_code, tuner_index);
#ifdef    TER_TUNER_CUSTOMTER
  if (ter_tuner_code               == CUSTOMTER_CODE ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > CUSTOMTER_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (ter_tuner_code               == 0x2141 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2141_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (ter_tuner_code               == 0x2146 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2146_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (ter_tuner_code               == 0x2147 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2147_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (ter_tuner_code               == 0x2148 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2148_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (ter_tuner_code               == 0x2148B ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2148B_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (ter_tuner_code               == 0x2151 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2151_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (ter_tuner_code               == 0x2156 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2156_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (ter_tuner_code               == 0x2157 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2157_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (ter_tuner_code               == 0x2158 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2158_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (ter_tuner_code               == 0x2158B ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2158B_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (ter_tuner_code               == 0x2173 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2173_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (ter_tuner_code               == 0x2176 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2176_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (ter_tuner_code               == 0x2177 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2177_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (ter_tuner_code               == 0x2178 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2178_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (ter_tuner_code               == 0x2178B ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2178B_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (ter_tuner_code               == 0x2190 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2190_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (ter_tuner_code               == 0x2191 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2191_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (ter_tuner_code               == 0x2191B ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2191B_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (ter_tuner_code               == 0x2196 ) {
    silabs_tuner->ter_tuner_code = ter_tuner_code;
    silabs_tuner->tuner_index    = tuner_index;
    if (tuner_index > Si2196_TUNER_COUNT) {silabs_tuner->tuner_index = 0;}
    silabs_tuner->i2c            = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->i2c;
  }
#endif /* TER_TUNER_Si2196 */
  SiTRACE("Select_Tuner selected  Si%04x_Tuner[%d] (i2c address 0x%02x)\n", silabs_tuner->ter_tuner_code, silabs_tuner->tuner_index, silabs_tuner->i2c->address);
  return (silabs_tuner->ter_tuner_code<<8)+silabs_tuner->tuner_index;
}
int   SiLabs_TER_Tuner_Set_Address          (SILABS_TER_TUNER_Context *silabs_tuner, int add) {
  silabs_tuner->i2c->address = add;
  SiTRACE("Si%04x_Tuner[%d] (i2c address 0x%02x)\n", silabs_tuner->ter_tuner_code, silabs_tuner->tuner_index, silabs_tuner->i2c->address);
  return 1;
}

int   SiLabs_TER_Tuner_Broadcast_I2C        (SILABS_TER_TUNER_Context *silabs_tuners[], int tuner_count) {
  int i;
  int return_code;
  silabs_tuners = silabs_tuners; /* To avoid compilation warnings */
  tuner_count    = tuner_count;  /* To avoid compilation warnings */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuners[0]->ter_tuner_code == 0x2178B) {
    L1_Si2178B_Context *Si2178B_tuners[tuner_count];
    for (i = 0; i < tuner_count; i++) {
      Si2178B_tuners[i] = silabs_tuners[i]->Si2178B_Tuner[silabs_tuners[i]->tuner_index];
    }
    return_code = Si2178B_PowerUpUsingBroadcastI2C(Si2178B_tuners, tuner_count);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuners[0]->ter_tuner_code == 0x2191B) {
    L1_Si2191B_Context *Si2191B_tuners[tuner_count];
    for (i = 0; i < tuner_count; i++) {
      Si2191B_tuners[i] = silabs_tuners[i]->Si2191B_Tuner[silabs_tuners[i]->tuner_index];
    }
    return_code = Si2191B_PowerUpUsingBroadcastI2C(Si2191B_tuners, tuner_count);
  }
#endif /* TER_TUNER_Si2191B */
  SiTRACE("SiLabs_TER_Tuner_Broadcast_I2C (%d tuners)\n", tuner_count);
  return_code = i = 0;
  return -1;
}
int   SiLabs_TER_Tuner_HW_Init              (SILABS_TER_TUNER_Context *silabs_tuner) {
  int return_code;
  return_code = -1;
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for CUSTOMTER at i2c 0x%02x\n", silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = L1_RF_CUSTOMTER_InitAfterReset(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2141 at i2c 0x%02x\n", silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2141_Init(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2146 at i2c 0x%02x\n", silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2146_Init(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2147 at i2c 0x%02x\n", silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2147_Init(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2148 at i2c 0x%02x\n", silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2148_Init(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2148B at i2c 0x%02x\n", silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2148B_Init(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2151 at i2c 0x%02x\n", silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2151_Init(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2156 at i2c 0x%02x\n", silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2156_Init(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2157 at i2c 0x%02x\n", silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2157_Init(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2158 at i2c 0x%02x\n", silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2158_Init(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2158B at i2c 0x%02x\n", silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2158B_Init(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2173 at i2c 0x%02x\n", silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2173_Init(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2176 at i2c 0x%02x\n", silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2176_Init(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2177 at i2c 0x%02x\n", silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2177_Init(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2178 at i2c 0x%02x\n", silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2178_Init(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2178B at i2c 0x%02x\n", silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2178B_Init(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2190 at i2c 0x%02x\n", silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2190_Init(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2191 at i2c 0x%02x\n", silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2191_Init(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2191B at i2c 0x%02x\n", silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2191B_Init(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    SiTRACE("SiLabs_TER_Tuner_HW_Init for Si2196 at i2c 0x%02x\n", silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->i2c->address);
    return_code = Si2196_Init(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_Si2196 */
  return return_code;
}
#if 0
int   SiLabs_TER_Tuner_HW_Connect           (SILABS_TER_TUNER_Context *silabs_tuner) {
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) { return L0_Connect(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return L0_Connect(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return L0_Connect(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return L0_Connect(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->i2c); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return L0_Connect(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return L0_Connect(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return L0_Connect(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return L0_Connect(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return L0_Connect(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return L0_Connect(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return L0_Connect(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return L0_Connect(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return L0_Connect(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return L0_Connect(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return L0_Connect(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return L0_Connect(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return L0_Connect(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return L0_Connect(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return L0_Connect(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return L0_Connect(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->i2c, connection_mode); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}

int   SiLabs_TER_Tuner_bytes_trace          (SILABS_TER_TUNER_Context *silabs_tuner, unsigned char track_mode) {
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) { return silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->i2c->trackWrite = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->i2c->trackRead = track_mode; }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
#endif

int   SiLabs_TER_Tuner_Part_Info            (SILABS_TER_TUNER_Context *silabs_tuner) {
  int return_code;
  return_code = -1;
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    silabs_tuner->part              = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    silabs_tuner->part              = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    silabs_tuner->part              = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    silabs_tuner->part              = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    silabs_tuner->part              = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    silabs_tuner->part              = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    silabs_tuner->part              = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    silabs_tuner->part              = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    silabs_tuner->part              = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    silabs_tuner->part              = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    silabs_tuner->part              = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    silabs_tuner->part              = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    silabs_tuner->part              = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    silabs_tuner->part              = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    silabs_tuner->part              = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    silabs_tuner->part              = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    silabs_tuner->part              = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    silabs_tuner->part              = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    silabs_tuner->part              = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->part_info.part;
    silabs_tuner->romid             = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->part_info.romid;
    silabs_tuner->part_info.chiprev = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->part_info.chiprev;
    silabs_tuner->part_info.major   = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pmajor;
    silabs_tuner->part_info.minor   = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pminor;
    silabs_tuner->part_info.build   = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->part_info.pbuild;
  }
#endif /* TER_TUNER_Si2196 */
  return return_code;
}
int   SiLabs_TER_Tuner_Get_Rev              (SILABS_TER_TUNER_Context *silabs_tuner) {
  int return_code;
  return_code = -1;
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    Si2141_L1_GET_REV(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    Si2146_L1_GET_REV(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    return_code = Si2147_L1_GET_REV(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    Si2148_L1_GET_REV(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    Si2148B_L1_GET_REV(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    Si2151_L1_GET_REV(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    Si2156_L1_GET_REV(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    Si2157_L1_GET_REV(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    Si2158_L1_GET_REV(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    Si2158B_L1_GET_REV(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    Si2173_L1_GET_REV(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    Si2176_L1_GET_REV(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    Si2177_L1_GET_REV(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    Si2178_L1_GET_REV(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    Si2178B_L1_GET_REV(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    Si2190_L1_GET_REV(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    Si2191_L1_GET_REV(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    Si2191B_L1_GET_REV(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    Si2196_L1_GET_REV(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->get_rev.chiprev = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.chiprev;
    silabs_tuner->get_rev.major   = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpmajor;
    silabs_tuner->get_rev.minor   = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpminor;
    silabs_tuner->get_rev.build   = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->get_rev.cmpbuild;
  }
#endif /* TER_TUNER_Si2196 */
  return return_code;
}
int   SiLabs_TER_Tuner_Wakeup               (SILABS_TER_TUNER_Context *silabs_tuner) {
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) { return L1_RF_CUSTOMTER_Wakeup(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_pollForCTS(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_pollForCTS(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_pollForCTS(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return Si2148_pollForCTS(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_pollForCTS(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_pollForCTS(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return Si2156_pollForCTS(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_pollForCTS(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_pollForCTS(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_pollForCTS(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_pollForCTS(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_pollForCTS(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_pollForCTS(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_pollForCTS(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_pollForCTS(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_pollForCTS(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_pollForCTS(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_pollForCTS(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_pollForCTS(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_Standby              (SILABS_TER_TUNER_Context *silabs_tuner) {
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) { return L1_RF_CUSTOMTER_Standby(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_Standby(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_L1_STANDBY(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_Standby(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return Si2148_Standby(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_Standby(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_Standby(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return Si2156_L1_STANDBY(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_STANDBY_CMD_TYPE_NORMAL); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_Standby(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_Standby(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_Standby(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { SiERROR("SiLabs_TER_Tuner_Standby not supported by Si2173!\n"); return 0; }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_Standby(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_Standby(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_Standby(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_Standby(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_Standby(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_Standby(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_Standby(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_Standby(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_ClockConfig          (SILABS_TER_TUNER_Context *silabs_tuner, int xtal, int xout) {
  SiTRACE("SiLabs_TER_Tuner_ClockConfig 0x%x  xtal %d xout %d\n", silabs_tuner->ter_tuner_code, xtal, xout);
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    if (xtal == 1) { silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2141_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2141_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2141_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2141_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2141_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2141_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    if (xtal == 1) { silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2146_POWER_UP_CMD_CLOCK_MODE_XTAL;      }
    else           { silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2146_POWER_UP_CMD_CLOCK_MODE_EXTCLK;    }
    if (xout == 1) { silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2146_CONFIG_PINS_CMD_XOUT_MODE_XOUT;    }
    else           { silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2146_CONFIG_PINS_CMD_XOUT_MODE_DISABLE; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    if (xtal == 1) { silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2147_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2147_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2147_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2147_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2147_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2147_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    if (xtal == 1) { silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2148_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2148_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2148_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2148_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2148_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2148_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    if (xtal == 1) { silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2148B_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2148B_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2148B_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2148B_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2148B_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2148B_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    if (xtal == 1) { silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2151_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2151_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2151_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2151_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2151_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2151_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    if (xtal == 1) { silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2156_POWER_UP_CMD_CLOCK_MODE_XTAL;      }
    else           { silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2156_POWER_UP_CMD_CLOCK_MODE_EXTCLK;    }
    if (xout == 1) { silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2156_CONFIG_PINS_CMD_XOUT_MODE_XOUT;    }
    else           { silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2156_CONFIG_PINS_CMD_XOUT_MODE_DISABLE; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    if (xtal == 1) { silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2157_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2157_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2157_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2157_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2157_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2157_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    if (xtal == 1) { silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2158_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2158_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2158_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2158_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2158_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2158_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    if (xtal == 1) { silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2158B_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2158B_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2158B_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2158B_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2158B_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2158B_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    if (xtal == 1) { silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2173_POWER_UP_CMD_CLOCK_MODE_XTAL;      }
    else           { silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2173_POWER_UP_CMD_CLOCK_MODE_EXTCLK;    }
    if (xout == 1) { silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2173_CONFIG_PINS_CMD_XOUT_MODE_XOUT;    }
    else           { silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2173_CONFIG_PINS_CMD_XOUT_MODE_DISABLE; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    if (xtal == 1) { silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2176_POWER_UP_CMD_CLOCK_MODE_XTAL;      }
    else           { silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2176_POWER_UP_CMD_CLOCK_MODE_EXTCLK;    }
    if (xout == 1) { silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2176_CONFIG_PINS_CMD_XOUT_MODE_XOUT;    }
    else           { silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2176_CONFIG_PINS_CMD_XOUT_MODE_DISABLE; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    if (xtal == 1) { silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2177_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2177_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2177_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2177_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2177_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2177_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    if (xtal == 1) { silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2178_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2178_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2178_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2178_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2178_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2178_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    if (xtal == 1) { silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2178B_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2178B_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2178B_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2178B_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2178B_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2178B_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    if (xtal == 1) { silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2190_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2190_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2190_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2190_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2190_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2190_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    if (xtal == 1) { silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2191_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2191_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2191_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2191_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2191_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2191_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    if (xtal == 1) { silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2191B_POWER_UP_CMD_CLOCK_MODE_XTAL;
                     silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2191B_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;   }
    else           { silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2191B_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
                     silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->cmd->config_clocks.clock_mode = Si2191B_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK; }
    if (xout == 1) { silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2191B_POWER_UP_CMD_EN_XOUT_EN_XOUT;  }
    else           { silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->cmd->power_up.en_xout         = Si2191B_POWER_UP_CMD_EN_XOUT_DIS_XOUT; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    if (xtal == 1) { silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2196_POWER_UP_CMD_CLOCK_MODE_XTAL;      }
    else           { silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->cmd->power_up.clock_mode      = Si2196_POWER_UP_CMD_CLOCK_MODE_EXTCLK;    }
    if (xout == 1) { silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2196_CONFIG_PINS_CMD_XOUT_MODE_XOUT;    }
    else           { silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.xout_mode    = Si2196_CONFIG_PINS_CMD_XOUT_MODE_DISABLE; }
    return silabs_tuner->ter_tuner_code; }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_ClockOff             (SILABS_TER_TUNER_Context *silabs_tuner) {
  SiTRACE("SiLabs_TER_Tuner_ClockOff\n");
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) { return L1_RF_CUSTOMTER_ClockOff(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_XoutOff(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_ClockOff(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_XoutOff(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return Si2148_XoutOff(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_XoutOff(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_XoutOff(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return Si2156_ClockOff(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_XoutOff(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_XoutOff(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_XoutOff(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_ClockOff(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_ClockOff(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_XoutOff(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_XoutOff(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_XoutOff(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_XoutOff(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_XoutOff(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_XoutOff(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_XoutOff(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_ClockOn              (SILABS_TER_TUNER_Context *silabs_tuner) {
  SiTRACE("SiLabs_TER_Tuner_ClockOn\n");
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) { return L1_RF_CUSTOMTER_ClockOn(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_XoutOn(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_ClockOn(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_XoutOn(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return Si2148_XoutOn(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_XoutOn(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_XoutOn(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return Si2156_ClockOn(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_XoutOn(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_XoutOn(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_XoutOn(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_ClockOn(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_ClockOn(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_XoutOn(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_XoutOn(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_XoutOn(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_XoutOn(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_XoutOn(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_XoutOn(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_XoutOn(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_PowerDown            (SILABS_TER_TUNER_Context *silabs_tuner) {
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) { return L1_RF_CUSTOMTER_Standby(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_Powerdown(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_L1_POWER_DOWN(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_Powerdown(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return Si2148_Powerdown(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_Powerdown(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_Powerdown(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return Si2156_L1_POWER_DOWN(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_Powerdown(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_Powerdown(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_Powerdown(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { SiERROR("SiLabs_TER_Tuner_PowerDown not sopported by Si2173!\n");return 0; }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_Powerdown(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_Powerdown(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_Powerdown(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_Powerdown(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_Powerdown(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_Powerdown(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_Powerdown(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_Powerdown(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_Possible_Tuners      (SILABS_TER_TUNER_Context *silabs_tuner, char* tunerList) {
  int i;
  silabs_tuner = silabs_tuner; /* To avoid compiler warning if not used */
  i = 0;
  sprintf(tunerList, 1000, "%s", "");
#ifdef    TER_TUNER_CUSTOMTER
  sprintf(tunerList, 1000, "%s CUSTOMTER[%d]", tunerList, CUSTOMTER_TUNER_COUNT); i++;
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  sprintf(tunerList, 1000, "%s Si2141[%d]", tunerList, Si2141_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  sprintf(tunerList, 1000, "%s Si2146[%d]", tunerList, Si2146_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  sprintf(tunerList, 1000, "%s Si2147[%d]", tunerList, Si2147_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  sprintf(tunerList, 1000, "%s Si2148[%d]", tunerList, Si2148_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  sprintf(tunerList, 1000, "%s Si2148B[%d]", tunerList, Si2148B_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  sprintf(tunerList, 1000, "%s Si2151[%d]", tunerList, Si2151_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  sprintf(tunerList, 1000, "%s Si2156[%d]", tunerList, Si2156_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  sprintf(tunerList, 1000, "%s Si2157[%d]", tunerList, Si2157_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  sprintf(tunerList, 1000, "%s Si2158[%d]", tunerList, Si2158_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  sprintf(tunerList, 1000, "%s Si2158B[%d]", tunerList, Si2158B_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  sprintf(tunerList, 1000, "%s Si2173[%d]", tunerList, Si2173_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  sprintf(tunerList, 1000, "%s Si2176[%d]", tunerList, Si2176_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  sprintf(tunerList, 1000, "%s Si2177[%d]", tunerList, Si2177_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  sprintf(tunerList, 1000, "%s Si2178[%d]", tunerList, Si2178_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  sprintf(tunerList, 1000, "%s Si2178B[%d]", tunerList, Si2178B_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  sprintf(tunerList, 1000, "%s Si2190[%d]", tunerList, Si2190_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  sprintf(tunerList, 1000, "%s Si2191[%d]", tunerList, Si2191_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  sprintf(tunerList, 1000, "%s Si2191B[%d]", tunerList, Si2191B_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  sprintf(tunerList, 1000, "%s Si2196[%d]", tunerList, Si2196_TUNER_COUNT); i++;
#endif /* TER_TUNER_Si2196 */
  return i;
}
char* SiLabs_TER_Tuner_ERROR_TEXT           (SILABS_TER_TUNER_Context *silabs_tuner, unsigned char  error_code) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return Si2148_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return Si2156_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_L1_API_ERROR_TEXT(error_code); }
#endif /* TER_TUNER_Si2196 */
  return "Unknown tuner code 0x%04x";
}
int   SiLabs_TER_Tuner_ATVTune              (SILABS_TER_TUNER_Context *silabs_tuner, unsigned long freq, unsigned char video_sys, unsigned char trans, unsigned char color, unsigned char invert_signal) {
  freq          = freq          ; /* To avoid compiler warnings when not used */
  video_sys     = video_sys     ; /* To avoid compiler warnings when not used */
  trans         = trans         ; /* To avoid compiler warnings when not used */
  color         = color         ; /* To avoid compiler warnings when not used */
  invert_signal = invert_signal ; /* To avoid compiler warnings when not used */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_Tune(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_Tune(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_Tune(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return Si2148_Tune(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_Tune(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_Tune(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return Si2156_Tune(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_Tune(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_Tune(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_Tune(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_Tune(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_Tune(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_Tune(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_Tune(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_Tune(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_Tune(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_Tune(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq, 1); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_Tune(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_TUNER_TUNE_FREQ_CMD_MODE_ATV, freq); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_ATVTune(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], freq, video_sys, trans, color, invert_signal); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_DTVTune              (SILABS_TER_TUNER_Context *silabs_tuner, unsigned long freq, unsigned char bw, unsigned char modulation) {
  int return_code;
  return_code = 0;
  SiTRACE("SiLabs_TER_Tuner_DTVTune %ld Hz, bw %d, modulation %d (ter_tuner_code %d/0x%x)\n", freq, bw, modulation, silabs_tuner->ter_tuner_code, silabs_tuner->ter_tuner_code);
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    return_code = Si2141_DTVTune(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2141_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    return_code = Si2146_DTVTune(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2146_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(139);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    return_code = Si2147_DTVTune(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2147_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    return_code = Si2148_DTVTune(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2148_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    return_code = Si2148B_DTVTune(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2148B_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    return_code = Si2151_DTVTune(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2151_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    return_code = Si2156_DTVTune(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2156_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(169);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    return_code = Si2157_DTVTune(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2157_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    return_code = Si2158_DTVTune(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2158_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    return_code = Si2158B_DTVTune(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2158B_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    return_code = Si2173_DTVTune(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2173_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(169);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    return_code = Si2176_DTVTune(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2176_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(169);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    return_code = Si2177_DTVTune(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2177_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    return_code = Si2178_DTVTune(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2178_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    return_code = Si2178B_DTVTune(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2178B_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    return_code = Si2190_DTVTune(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2190_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    return_code = Si2191_DTVTune(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2191_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    return_code = Si2191B_DTVTune(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2191B_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(85);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    return_code = Si2196_DTVTune(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], freq, bw, modulation, Si2196_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL);
    system_wait(149);
  }
#endif /* TER_TUNER_Si2196 */
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    L1_RF_CUSTOMTER_Saw(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index], bw);
    return_code = L1_RF_CUSTOMTER_Tune(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index], freq);
    /* Wait the required time to get the RF level within +/- 1 dB of its final value */
    system_wait(0);
  }
#endif /* TER_TUNER_CUSTOMTER */
  return return_code;
}
int   SiLabs_TER_Tuner_Fine_Tune            (SILABS_TER_TUNER_Context *silabs_tuner, unsigned char persistence, unsigned char apply_to_lif,int offset_500hz) {
  int return_code;
  return_code = 0;
  SiTRACE("SiLabs_TER_Tuner_Fine_Tune offset %d Hz (ter_tuner_code %d/0x%x)\n", offset_500hz, silabs_tuner->ter_tuner_code, silabs_tuner->ter_tuner_code);
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return_code = Si2141_L1_FINE_TUNE(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], persistence, offset_500hz); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return_code = Si2146_L1_FINE_TUNE(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], persistence, offset_500hz); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
	if (silabs_tuner->ter_tuner_code == 0x2147) { return_code = Si2147_L1_FINE_TUNE(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return_code = Si2148_L1_FINE_TUNE(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return_code = Si2148B_L1_FINE_TUNE(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return_code = Si2151_L1_FINE_TUNE(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], persistence, offset_500hz); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return_code = Si2156_L1_FINE_TUNE(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], persistence, offset_500hz); }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return_code = Si2157_L1_FINE_TUNE(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return_code = Si2158_L1_FINE_TUNE(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return_code = Si2158B_L1_FINE_TUNE(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return_code = Si2173_L1_FINE_TUNE(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], persistence, offset_500hz); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return_code = Si2176_L1_FINE_TUNE(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], persistence, offset_500hz); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return_code = Si2177_L1_FINE_TUNE(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return_code = Si2178_L1_FINE_TUNE(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return_code = Si2178B_L1_FINE_TUNE(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return_code = Si2190_L1_FINE_TUNE(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return_code = Si2191_L1_FINE_TUNE(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return_code = Si2191B_L1_FINE_TUNE(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], persistence, apply_to_lif, offset_500hz); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return_code = Si2196_L1_FINE_TUNE(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], persistence, offset_500hz); }
#endif /* TER_TUNER_Si2196 */
  return return_code;
}
int   SiLabs_TER_Tuner_Block_VCO_Code       (SILABS_TER_TUNER_Context *silabs_tuner, int vco_code) {
  int return_code;
  return_code = 0;
  SiTRACE("SiLabs_TER_Tuner_Block_VCO_Code %d (ter_tuner_code %d/0x%x)\n", vco_code, silabs_tuner->ter_tuner_code, silabs_tuner->ter_tuner_code);
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2173_L1_SetProperty2(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_blocked_vco.vco_code = vco_code;
    return_code = Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_TUNER_BLOCKED_VCO_PROP);
  }
#endif /* TER_TUNER_Si2196 */
  return return_code;
}
int   SiLabs_TER_Tuner_Get_RF               (SILABS_TER_TUNER_Context *silabs_tuner) {
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) { return L1_RF_CUSTOMTER_Get_RF(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_GetRF(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_GetRF(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_GetRF(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    Si2148_L1_TUNER_STATUS (silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_TUNER_STATUS_CMD_INTACK_OK);
    return silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_GetRF(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_GetRF(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    Si2156_L1_TUNER_STATUS (silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_TUNER_STATUS_CMD_INTACK_OK);
    return silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_GetRF(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_GetRF(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_GetRF(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_GetRF(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_GetRF(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_GetRF(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_GetRF(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_GetRF(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_GetRF(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_GetRF(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_GetRF(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_GetRF(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_Status               (SILABS_TER_TUNER_Context *silabs_tuner) {
  int return_code;
  SiTRACE("SiLabs_TER_Tuner_Status\n");
  return_code = -1;
#ifdef    TER_TUNER_CUSTOMTER
  if (silabs_tuner->ter_tuner_code == CUSTOMTER_CODE) {
    silabs_tuner->rssi     = L1_RF_CUSTOMTER_Get_RSSI(silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->freq     = L1_RF_CUSTOMTER_Get_RF  (silabs_tuner->CUSTOMTER_Tuner[silabs_tuner->tuner_index]);
  }
#endif /* TER_TUNER_CUSTOMTER */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    return_code = Si2141_L1_TUNER_STATUS(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->vco_code = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->rssi     = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    return_code = Si2146_L1_TUNER_STATUS(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    return_code = Si2147_L1_TUNER_STATUS(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    return_code = Si2148_L1_TUNER_STATUS(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    return_code = Si2148B_L1_TUNER_STATUS(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    return_code = Si2151_L1_TUNER_STATUS(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]);
    silabs_tuner->vco_code = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->rssi     = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    return_code = Si2156_L1_TUNER_STATUS(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    return_code = Si2157_L1_TUNER_STATUS(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    return_code = Si2158_L1_TUNER_STATUS(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    return_code = Si2158B_L1_TUNER_STATUS(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    return_code = Si2173_L1_TUNER_STATUS(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    return_code = Si2176_L1_TUNER_STATUS(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    return_code = Si2177_L1_TUNER_STATUS(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    return_code = Si2178_L1_TUNER_STATUS(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    return_code = Si2178B_L1_TUNER_STATUS(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    return_code = Si2190_L1_TUNER_STATUS(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    return_code = Si2191_L1_TUNER_STATUS(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    return_code = Si2191B_L1_TUNER_STATUS(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    return_code = Si2196_L1_TUNER_STATUS(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_TUNER_STATUS_CMD_INTACK_OK);
    silabs_tuner->vco_code = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.vco_code;
    silabs_tuner->tc       = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.tc;
    silabs_tuner->rssi     = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.rssi;
    silabs_tuner->freq     = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.freq;
    silabs_tuner->mode     = silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->rsp->tuner_status.mode;
  }
#endif /* TER_TUNER_Si2196 */
  if (return_code == -1) {
    SiTRACE("SiLabs_TER_Tuner_Status: unknown tuner_code 0x%04x\n", silabs_tuner->ter_tuner_code);
    SiERROR("SiLabs_TER_Tuner_Status: unknown tuner_code\n");
  }
  return return_code;
}
int   SiLabs_TER_Tuner_Text_STATUS          (SILABS_TER_TUNER_Context *silabs_tuner, char *separator, char *msg) {
  int return_code = 0;	//steven
  sprintf(msg, 1000, "SiLabs_TER_Tuner_Text_STATUS: unknown tuner_code 0x%04x\n", silabs_tuner->ter_tuner_code);
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    return_code = Si2141_L1_TUNER_STATUS(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]);
    Si2141_L1_GetCommandResponseString  (silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    return_code = Si2146_L1_TUNER_STATUS(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_TUNER_STATUS_CMD_INTACK_OK);
    Si2146_L1_GetCommandResponseString  (silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    return_code = Si2147_L1_TUNER_STATUS(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_TUNER_STATUS_CMD_INTACK_OK);
//    Si2147_L1_GetCommandResponseString  (silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    return_code = Si2148_L1_TUNER_STATUS(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_TUNER_STATUS_CMD_INTACK_OK);
    Si2148_L1_GetCommandResponseString  (silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    return_code = Si2148B_L1_TUNER_STATUS(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_TUNER_STATUS_CMD_INTACK_OK);
    Si2148B_L1_GetCommandResponseString  (silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    return_code = Si2151_L1_TUNER_STATUS(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]);
    Si2151_L1_GetCommandResponseString  (silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    return_code = Si2156_L1_TUNER_STATUS(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_TUNER_STATUS_CMD_INTACK_OK);
    Si2156_L1_GetCommandResponseString  (silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    return_code = Si2157_L1_TUNER_STATUS(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_TUNER_STATUS_CMD_INTACK_OK);
    Si2157_L1_GetCommandResponseString  (silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    return_code = Si2158_L1_TUNER_STATUS(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_TUNER_STATUS_CMD_INTACK_OK);
    Si2158_L1_GetCommandResponseString  (silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    return_code = Si2158B_L1_TUNER_STATUS(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_TUNER_STATUS_CMD_INTACK_OK);
    Si2158B_L1_GetCommandResponseString  (silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    return_code = Si2173_L1_TUNER_STATUS(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_TUNER_STATUS_CMD_INTACK_OK);
    Si2173_L1_GetCommandResponseString  (silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    return_code = Si2176_L1_TUNER_STATUS(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_TUNER_STATUS_CMD_INTACK_OK);
    Si2176_L1_GetCommandResponseString  (silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    return_code = Si2177_L1_TUNER_STATUS(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_TUNER_STATUS_CMD_INTACK_OK);
    Si2177_L1_GetCommandResponseString  (silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    return_code = Si2178_L1_TUNER_STATUS(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_TUNER_STATUS_CMD_INTACK_OK);
    Si2178_L1_GetCommandResponseString  (silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    return_code = Si2178B_L1_TUNER_STATUS(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_TUNER_STATUS_CMD_INTACK_OK);
    Si2178B_L1_GetCommandResponseString  (silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    return_code = Si2190_L1_TUNER_STATUS(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_TUNER_STATUS_CMD_INTACK_OK);
    Si2190_L1_GetCommandResponseString  (silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    return_code = Si2191_L1_TUNER_STATUS(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_TUNER_STATUS_CMD_INTACK_OK);
    Si2191_L1_GetCommandResponseString  (silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    return_code = Si2191B_L1_TUNER_STATUS(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_TUNER_STATUS_CMD_INTACK_OK);
    Si2191B_L1_GetCommandResponseString  (silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    return_code = Si2196_L1_TUNER_STATUS(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_TUNER_STATUS_CMD_INTACK_OK);
    Si2196_L1_GetCommandResponseString  (silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_TUNER_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2196 */
  return return_code;
}
int   SiLabs_TER_Tuner_ATV_Text_STATUS      (SILABS_TER_TUNER_Context *silabs_tuner, char *separator, char *msg) {
  int return_code;
  SiTRACE("SiLabs_TER_Tuner_ATV_Text_STATUS\n");
  return_code = -1;
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    return_code = Si2146_L1_ATV_STATUS(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_ATV_STATUS_CMD_INTACK_OK);
    Si2146_L1_GetCommandResponseString(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {

#if 0
    return_code = Si2147_L1_ATV_STATUS(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_ATV_STATUS_CMD_INTACK_OK);
    Si2147_L1_GetCommandResponseString(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_ATV_STATUS_CMD_CODE, separator, msg);
#endif
	return 0;

  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    return_code = Si2156_L1_ATV_STATUS(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_ATV_STATUS_CMD_INTACK_OK);
    Si2156_L1_GetCommandResponseString(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    return_code = Si2157_L1_ATV_STATUS(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_ATV_STATUS_CMD_INTACK_OK);
    Si2157_L1_GetCommandResponseString(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    return_code = Si2158_L1_ATV_STATUS(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_ATV_STATUS_CMD_INTACK_OK);
    Si2158_L1_GetCommandResponseString(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    return_code = Si2158B_L1_ATV_STATUS(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_ATV_STATUS_CMD_INTACK_OK);
    Si2158B_L1_GetCommandResponseString(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    return_code = Si2173_L1_ATV_STATUS(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_ATV_STATUS_CMD_INTACK_OK);
    Si2173_L1_GetCommandResponseString(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    return_code = Si2176_L1_ATV_STATUS(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_ATV_STATUS_CMD_INTACK_OK);
    Si2176_L1_GetCommandResponseString(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    return_code = Si2177_L1_ATV_STATUS(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_ATV_STATUS_CMD_INTACK_OK);
    Si2177_L1_GetCommandResponseString(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    return_code = Si2178_L1_ATV_STATUS(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_ATV_STATUS_CMD_INTACK_OK);
    Si2178_L1_GetCommandResponseString(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    return_code = Si2178B_L1_ATV_STATUS(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_ATV_STATUS_CMD_INTACK_OK);
    Si2178B_L1_GetCommandResponseString(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    return_code = Si2190_L1_ATV_STATUS(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_ATV_STATUS_CMD_INTACK_OK);
    Si2190_L1_GetCommandResponseString(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    return_code = Si2191_L1_ATV_STATUS(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_ATV_STATUS_CMD_INTACK_OK);
    Si2191_L1_GetCommandResponseString(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    return_code = Si2191B_L1_ATV_STATUS(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_ATV_STATUS_CMD_INTACK_OK);
    Si2191B_L1_GetCommandResponseString(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    return_code = Si2196_L1_ATV_STATUS(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_ATV_STATUS_CMD_INTACK_OK);
    Si2196_L1_GetCommandResponseString(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_ATV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2196 */
  if (return_code == -1) {
    SiTRACE("SiLabs_TER_Tuner_ATV_Text_STATUS: unknown tuner_code 0x%04x\n", silabs_tuner->ter_tuner_code);
    SiERROR("SiLabs_TER_Tuner_ATV_Text_STATUS: unknown tuner_code\n");
  }
  return return_code;
}
int   SiLabs_TER_Tuner_DTV_Text_STATUS      (SILABS_TER_TUNER_Context *silabs_tuner) {
  int return_code;
  SiTRACE("SiLabs_TER_Tuner_DTV_Text_STATUS\n");
  return_code = -1;
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    return_code = Si2146_L1_DTV_STATUS(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_STATUS_CMD_INTACK_OK);
    Si2146_L1_GetCommandResponseString(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    return_code = Si2147_L1_DTV_STATUS(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_STATUS_CMD_INTACK_OK);
//    Si2147_L1_GetCommandResponseString(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    return_code = Si2148_L1_DTV_STATUS(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_STATUS_CMD_INTACK_OK);
    Si2148_L1_GetCommandResponseString(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    return_code = Si2148B_L1_DTV_STATUS(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_STATUS_CMD_INTACK_OK);
    Si2148B_L1_GetCommandResponseString(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    return_code = Si2156_L1_DTV_STATUS(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_STATUS_CMD_INTACK_OK);
    Si2156_L1_GetCommandResponseString(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    return_code = Si2157_L1_DTV_STATUS(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_STATUS_CMD_INTACK_OK);
    Si2157_L1_GetCommandResponseString(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    return_code = Si2158_L1_DTV_STATUS(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_STATUS_CMD_INTACK_OK);
    Si2158_L1_GetCommandResponseString(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    return_code = Si2158B_L1_DTV_STATUS(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_STATUS_CMD_INTACK_OK);
    Si2158B_L1_GetCommandResponseString(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    return_code = Si2173_L1_DTV_STATUS(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_DTV_STATUS_CMD_INTACK_OK);
    Si2173_L1_GetCommandResponseString(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    return_code = Si2176_L1_DTV_STATUS(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_STATUS_CMD_INTACK_OK);
    Si2176_L1_GetCommandResponseString(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    return_code = Si2177_L1_DTV_STATUS(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_STATUS_CMD_INTACK_OK);
    Si2177_L1_GetCommandResponseString(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    return_code = Si2178_L1_DTV_STATUS(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_STATUS_CMD_INTACK_OK);
    Si2178_L1_GetCommandResponseString(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    return_code = Si2178B_L1_DTV_STATUS(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_STATUS_CMD_INTACK_OK);
    Si2178B_L1_GetCommandResponseString(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    return_code = Si2190_L1_DTV_STATUS(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_STATUS_CMD_INTACK_OK);
    Si2190_L1_GetCommandResponseString(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    return_code = Si2191_L1_DTV_STATUS(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_STATUS_CMD_INTACK_OK);
    Si2191_L1_GetCommandResponseString(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    return_code = Si2191B_L1_DTV_STATUS(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_STATUS_CMD_INTACK_OK);
    Si2191B_L1_GetCommandResponseString(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    return_code = Si2196_L1_DTV_STATUS(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_STATUS_CMD_INTACK_OK);
    Si2196_L1_GetCommandResponseString(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_STATUS_CMD_CODE, separator, msg);
  }
#endif /* TER_TUNER_Si2196 */
  if (return_code == -1) {
    SiTRACE("SiLabs_TER_Tuner_DTV_Text_STATUS: unknown tuner_code 0x%04x\n", silabs_tuner->ter_tuner_code);
    SiERROR("SiLabs_TER_Tuner_DTV_Text_STATUS: unknown tuner_code\n");
  }
  return return_code;
}
int   SiLabs_TER_Tuner_AGC_External         (SILABS_TER_TUNER_Context *silabs_tuner) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2141_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF;
    Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_CONFIG_IF_PORT_PROP_CODE);
    return Si2141_L1_CONFIG_PINS (silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]
                          ,Si2141_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE
                          ,Si2141_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ
                          ,Si2141_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE
                          ,Si2141_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ
                          ,Si2141_CONFIG_PINS_CMD_AGC1_MODE_DTV_AGC
                          ,Si2141_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ
                          ,Si2141_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE
                          ,Si2141_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ
                          ,Si2141_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
      silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2146_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
      silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2146_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_DLIF_AGC_3DB;
    return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2147_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF2;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2147_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2148_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF2;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2148_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2148B_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2148B_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2151_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF;
    Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_CONFIG_IF_PORT_PROP_CODE);
    return Si2151_L1_CONFIG_PINS (silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]
                          ,Si2151_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE
                          ,Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ
                          ,Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE
                          ,Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ
                          ,Si2151_CONFIG_PINS_CMD_AGC1_MODE_DTV_AGC
                          ,Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ
                          ,Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE
                          ,Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ
                          ,Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
      silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2156_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
      silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2156_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_DLIF_AGC_3DB;
    return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2157_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF2;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2157_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2158_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF2;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2158_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2158B_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2158B_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2173_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2173_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_DLIF_AGC_3DB;
    return Si2173_L1_SetProperty2(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2176_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2176_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_DLIF_AGC_3DB;
    return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2177_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2177_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2178_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2178_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2178B_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2178B_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2190_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF2;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2190_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2191_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF2;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2191_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2191B_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF1;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2191B_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_AGC1_3DB;
    return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_out_type    = Si2196_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE_LIF_IF2;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_config_if_port.dtv_agc_source  = Si2196_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE_DLIF_AGC_3DB;
    return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_CONFIG_IF_PORT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_DTV_LIF_OUT          (SILABS_TER_TUNER_Context *silabs_tuner, int amp, int offset) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2173_L1_SetProperty2(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.amp = amp;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_lif_out.offset = offset;
    return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_LIF_OUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_DTV_LIF_OUT_amp      (SILABS_TER_TUNER_Context *silabs_tuner, unsigned int amp_index) {
/*
  For older families of tuner (Si21x3, Si21x6, Si2196) and Si2165-D/Si2167-A, recommended AMP values are:
  -	DVBT standard: DTV_LIF_OUT:AMP=24
  -	DVBC standard: DTV_LIF_OUT:AMP=34
  For Si2190/Si2191/Si21x8/Si21x7/Si21x8B families of tuner and Si2165-D/Si2167-A, recommended AMP values are:
  -	DVBT standard: DTV_LIF_OUT:AMP=25
  -	DVBC standard: DTV_LIF_OUT:AMP=37
  For all families of tuner and newer demods , AMP values are:
  -	DVBT/DVBT2/DVBC2 (OFDM standards): DTV_LIF_OUT:AMP=32
  -	DVBC/MCNS        (QAM  standards): DTV_LIF_OUT:AMP=43
*/
  #define DTV_LIF_OUT_TABLES 4
  #define NEW_DEMOD_OFDM 0
  #define NEW_DEMOD_QAM  1
  #define OLD_DEMOD_OFDM 2
  #define OLD_DEMOD_QAM  3
  int amp[DTV_LIF_OUT_TABLES];
  if (amp_index > DTV_LIF_OUT_TABLES - 1) {amp_index = 0;}
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 24; amp[OLD_DEMOD_QAM] = 34; }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 24; amp[OLD_DEMOD_QAM] = 34; }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { amp[NEW_DEMOD_OFDM] = 23; amp[NEW_DEMOD_QAM] = 32; amp[OLD_DEMOD_OFDM] = 24; amp[OLD_DEMOD_QAM] = 34; }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 24; amp[OLD_DEMOD_QAM] = 34; }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 25; amp[OLD_DEMOD_QAM] = 37; }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { amp[NEW_DEMOD_OFDM] = 32; amp[NEW_DEMOD_QAM] = 43; amp[OLD_DEMOD_OFDM] = 24; amp[OLD_DEMOD_QAM] = 34; }
#endif /* TER_TUNER_Si2196 */
  return SiLabs_TER_Tuner_DTV_LIF_OUT(silabs_tuner, amp[amp_index], 148);
}
int   SiLabs_TER_Tuner_DTV_LOInjection      (SILABS_TER_TUNER_Context *silabs_tuner) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2141_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2141_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2141_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2146_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2146_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2146_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2147_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2147_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2147_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2148_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2148_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2148_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2148B_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2148B_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2148B_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2151_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2151_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2151_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2156_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2156_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2156_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2156_TUNER_LO_INJECTION_PROP_BAND_4_LOW_SIDE;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2156_TUNER_LO_INJECTION_PROP_BAND_5_LOW_SIDE;
    return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2157_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2157_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2157_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2158_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2158_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2158_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2158B_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2158B_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2158B_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2173_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2173_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2173_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2173_TUNER_LO_INJECTION_PROP_BAND_4_LOW_SIDE;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2173_TUNER_LO_INJECTION_PROP_BAND_5_LOW_SIDE;
    return Si2173_L1_SetProperty2(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2176_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2176_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2176_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2176_TUNER_LO_INJECTION_PROP_BAND_4_LOW_SIDE;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2176_TUNER_LO_INJECTION_PROP_BAND_5_LOW_SIDE;
    return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2177_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2177_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2177_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2178_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2178_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2178_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2178B_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2178B_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2178B_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2190_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2190_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2190_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2191_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2191_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2191_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2191B_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2191B_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2191B_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2196_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2196_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2196_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2196_TUNER_LO_INJECTION_PROP_BAND_4_LOW_SIDE;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2196_TUNER_LO_INJECTION_PROP_BAND_5_LOW_SIDE;
    return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_ATV_LOInjection      (SILABS_TER_TUNER_Context *silabs_tuner) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2141_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2141_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2141_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2146_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2146_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2146_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2146_TUNER_LO_INJECTION_PROP_BAND_4_HIGH_SIDE;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2146_TUNER_LO_INJECTION_PROP_BAND_5_HIGH_SIDE;
    return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2147_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2147_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2147_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2148_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2148_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2148_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2148B_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2148B_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2148B_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2151_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2151_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2151_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2156_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2156_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2156_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2156_TUNER_LO_INJECTION_PROP_BAND_4_HIGH_SIDE;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2156_TUNER_LO_INJECTION_PROP_BAND_5_HIGH_SIDE;
    return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2157_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2157_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2157_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2158_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2158_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2158_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2158B_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2158B_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2158B_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2173_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2173_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2173_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2173_TUNER_LO_INJECTION_PROP_BAND_4_HIGH_SIDE;
    silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2173_TUNER_LO_INJECTION_PROP_BAND_5_HIGH_SIDE;
    return Si2173_L1_SetProperty2(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2176_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2176_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2176_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2176_TUNER_LO_INJECTION_PROP_BAND_4_HIGH_SIDE;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2176_TUNER_LO_INJECTION_PROP_BAND_5_HIGH_SIDE;
    return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2177_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2177_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2177_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2178_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2178_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2178_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2178B_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2178B_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2178B_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2190_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2190_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2190_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2191_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2191_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2191_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2191B_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2191B_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2191B_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_1 = Si2196_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_2 = Si2196_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_3 = Si2196_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_4 = Si2196_TUNER_LO_INJECTION_PROP_BAND_4_HIGH_SIDE;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->tuner_lo_injection.band_5 = Si2196_TUNER_LO_INJECTION_PROP_BAND_5_HIGH_SIDE;
    return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_TUNER_LO_INJECTION_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_FEF_FREEZE_PIN_SETUP (SILABS_TER_TUNER_Context *silabs_tuner, int fef_level) {
  SiTRACE("SiLabs_TER_Tuner_FEF_FREEZE_PIN_SETUP silabs_tuner 0x%08x fef_level %d\n" , (int)silabs_tuner, fef_level);
  SiTRACE("SiLabs_TER_Tuner_FEF_FREEZE_PIN_SETUP silabs_tuner->ter_tuner_code 0x%x\n", (int)silabs_tuner->ter_tuner_code);
  /* fef_level will be used during SiLabs_TER_Tuner_FEF_FREEZE_PIN to set the dtv_agc_freeze_input.level */
  if (fef_level) {silabs_tuner->fef_level = 1;} else {silabs_tuner->fef_level = 0;}
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2141_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2141_L1_SendCommand2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index],Si2141_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2141_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2141_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2141_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2141_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2147_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2147_L1_SendCommand2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index],Si2147_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2147_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2147_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2147_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2147_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2148_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2148_L1_SendCommand2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index],Si2148_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2148_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2148B_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2148B_L1_SendCommand2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index],Si2148B_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2148B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2151_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2151_L1_SendCommand2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index],Si2151_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2151_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2151_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2151_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2151_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2157_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2157_L1_SendCommand2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index],Si2157_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2157_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2157_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2157_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2157_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2158_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2158_L1_SendCommand2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index],Si2158_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2158_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2158_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2158B_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2158B_L1_SendCommand2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index],Si2158B_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2158B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2158B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    SiERROR("SiLabs_TER_Tuner_FEF_FREEZE_PIN_SETUP not supported by Si2173!\n");
    return 0;
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2176_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2176_L1_SendCommand2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index],Si2176_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2176_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2176_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2176_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2176_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2177_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2177_L1_SendCommand2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index],Si2177_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2177_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2177_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2177_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2177_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2178_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2178_L1_SendCommand2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index],Si2178_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2178_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2178_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2178B_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2178B_L1_SendCommand2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index],Si2178B_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2178B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2178B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2190_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2190_L1_SendCommand2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index],Si2190_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2190_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2190_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2190_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2190_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2191_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2191_L1_SendCommand2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index],Si2191_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2191_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2191_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2191B_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2191B_L1_SendCommand2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index],Si2191B_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2191B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2191B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2196_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2196_L1_SendCommand2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index],Si2196_CONFIG_PINS_CMD_CODE);
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2196_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2196_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2196_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2196_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
  #ifdef    Si2146_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2146_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2146_L1_SendCommand2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index],Si2146_CONFIG_PINS_CMD_CODE);
  #endif /* Si2146_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH */
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2146_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2146_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2146_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2146_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
  #ifdef    Si2156_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->cmd->config_pins.gpio1_mode               = Si2156_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;
    Si2156_L1_SendCommand2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index],Si2156_CONFIG_PINS_CMD_CODE);
  #endif /* Si2156_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH */
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2156_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2156_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2156_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
    silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2156_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
    Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
    return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2156 */
  return -1;
}
int   SiLabs_TER_Tuner_FEF_FREEZE_PIN       (SILABS_TER_TUNER_Context *silabs_tuner, int fef) {
  SiTRACE("SiLabs_TER_Tuner_FEF_FREEZE_PIN       silabs_tuner->ter_tuner_code 0x%x, fef %d\n", (int)silabs_tuner->ter_tuner_code, fef);
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
    if (fef == 0) {
      Si2141_L1_CONFIG_PINS (silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]
                            ,Si2141_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE
                            ,Si2141_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ
                            ,Si2141_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE
                            ,Si2141_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ
                            ,Si2141_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE
                            ,Si2141_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ
                            ,Si2141_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE
                            ,Si2141_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ
                            ,Si2141_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE);
    } else {
      if (silabs_tuner->fef_level == 0) {
        Si2141_L1_CONFIG_PINS (silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]
                              ,Si2141_CONFIG_PINS_CMD_GPIO1_MODE_DTV_FREEZE_LOW
                              ,Si2141_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ
                              ,Si2141_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE
                              ,Si2141_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ
                              ,Si2141_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE
                              ,Si2141_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ
                              ,Si2141_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE
                              ,Si2141_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ
                              ,Si2141_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE);
      } else {
        Si2141_L1_CONFIG_PINS (silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]
                              ,Si2141_CONFIG_PINS_CMD_GPIO1_MODE_DTV_FREEZE_HIGH
                              ,Si2141_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ
                              ,Si2141_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE
                              ,Si2141_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ
                              ,Si2141_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE
                              ,Si2141_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ
                              ,Si2141_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE
                              ,Si2141_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ
                              ,Si2141_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE);
      }
    }
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2147_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2147_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2147_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2147_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2148_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2148_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2148_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2148_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2148B_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2148B_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2148B_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2148B_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
    if (fef == 0) {
      Si2151_L1_CONFIG_PINS (silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]
                            ,Si2151_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE
                            ,Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ
                            ,Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE
                            ,Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ
                            ,Si2151_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE
                            ,Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ
                            ,Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE
                            ,Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ
                            ,Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE);
    } else {
      if (silabs_tuner->fef_level == 0) {
        Si2151_L1_CONFIG_PINS (silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]
                              ,Si2151_CONFIG_PINS_CMD_GPIO1_MODE_DTV_FREEZE_LOW
                              ,Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ
                              ,Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE
                              ,Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ
                              ,Si2151_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE
                              ,Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ
                              ,Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE
                              ,Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ
                              ,Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE);
      } else {
        Si2151_L1_CONFIG_PINS (silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]
                              ,Si2151_CONFIG_PINS_CMD_GPIO1_MODE_DTV_FREEZE_HIGH
                              ,Si2151_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ
                              ,Si2151_CONFIG_PINS_CMD_GPIO2_MODE_NO_CHANGE
                              ,Si2151_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ
                              ,Si2151_CONFIG_PINS_CMD_AGC1_MODE_NO_CHANGE
                              ,Si2151_CONFIG_PINS_CMD_AGC1_READ_DO_NOT_READ
                              ,Si2151_CONFIG_PINS_CMD_AGC2_MODE_NO_CHANGE
                              ,Si2151_CONFIG_PINS_CMD_AGC2_READ_DO_NOT_READ
                              ,Si2151_CONFIG_PINS_CMD_XOUT_MODE_NO_CHANGE);
      }
    }
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2157_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2157_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2157_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2157_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2158_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2158_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2158_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2158_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2158B_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2158B_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2158B_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2158B_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    SiERROR("SiLabs_TER_Tuner_FEF_FREEZE_PIN not supported by Si2173!\n");
    return 0;
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level        = Si2176_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level        = Si2176_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2176_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2176_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2177_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2177_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2177_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2177_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2178_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2178_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2178_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2178_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2178B_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2178B_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2178B_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2178B_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2190_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2190_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2190_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2190_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2191_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2191_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2191_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2191_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2191B_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2191B_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2191B_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2191B_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2196_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2196_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2196_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2196_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
  #ifdef    Si2146_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2146_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2146_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2146_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2146_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  #endif /* Si2146_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH */
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
  #ifdef    Si2156_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH
    if (silabs_tuner->fef_level == 0) {
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2156_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_LOW;
    } else {
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.level      = Si2156_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;
    }
    if (fef == 0) {
      silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2156_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;
    } else {
      silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_freeze_input.pin          = Si2156_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;
    }
    return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_AGC_FREEZE_INPUT_PROP_CODE);
  #endif /* Si2156_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH */
  }
#endif /* TER_TUNER_Si2156 */
  return -1;
}
int   SiLabs_TER_Tuner_SLOW_INITIAL_AGC     (SILABS_TER_TUNER_Context *silabs_tuner, int fef) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
      if (fef == 0) {
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2141_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2141_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2141_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2141_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2141_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2141_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2141_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2141_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
      if (fef == 0) {
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2146_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2146_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2146_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2146_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2146_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2146_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2146_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2146_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
      if (fef == 0) {
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2147_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2147_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2147_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2147_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2147_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2147_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2147_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2147_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
      if (fef == 0) {
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2148_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2148_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
      if (fef == 0) {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2148B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2148B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
      if (fef == 0) {
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2151_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2151_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2151_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2151_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2151_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2151_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2151_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2151_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
      if (fef == 0) {
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2156_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2156_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2156_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2156_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2156_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2156_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2156_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2156_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
      if (fef == 0) {
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2157_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2157_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2157_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2157_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2157_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2157_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2157_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2157_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
      if (fef == 0) {
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2158_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2158_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2158_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2158_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
      if (fef == 0) {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2158B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2158B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2158B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2158B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
      SiERROR("SiLabs_TER_Tuner_SLOW_INITIAL_AGC not supported by Si2173!\n");
      return 0;
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
      if (fef == 0) {
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2176_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2176_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2176_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2176_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2176_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2176_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2176_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2176_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
      if (fef == 0) {
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2177_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2177_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2177_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2177_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2177_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2177_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2177_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2177_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
      if (fef == 0) {
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2178_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2178_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2178_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2178_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
      if (fef == 0) {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2178B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2178B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2178B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2178B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
      if (fef == 0) {
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2190_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2190_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2190_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2190_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2190_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2190_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2190_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2190_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
      if (fef == 0) {
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2191_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2191_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2191_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2191_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
      if (fef == 0) {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2191B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2191B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2191B_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2191B_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
      if (fef == 0) {
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2196_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2196_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2196_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2196_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.agc_decim     = Si2196_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed.if_agc_speed  = Si2196_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 240;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2196_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2196_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
      }
      Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_SLOW_NORMAL_AGC_SETUP(SILABS_TER_TUNER_Context *silabs_tuner, int fef) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
      silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2141_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2141_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2141_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2141_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
      silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2146_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2146_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2146_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2146_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
      silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2147_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2147_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2147_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2147_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
      silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
      silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
      silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2151_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2151_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2151_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2151_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
      silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2156_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2156_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2156_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2156_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
      silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2157_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2157_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2157_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2157_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
      silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
      silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
      if (fef == 0) {
        silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2173_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2173_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2173_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2173_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2173_L1_SetProperty2(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
      silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2176_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2176_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2176_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2176_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
      silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2177_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2177_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2177_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2177_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
      silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
      silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
      silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2190_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2190_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2190_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2190_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
      silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
      silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
      silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_initial_agc_speed_period.period = 0;
      if (fef == 0) {
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2196_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2196_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2196_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2196_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);
      return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_SLOW_NORMAL_AGC      (SILABS_TER_TUNER_Context *silabs_tuner, int fef) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
      if (fef == 0) {
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2141_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2141_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2141_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2141_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) {
      if (fef == 0) {
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2146_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2146_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2146_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2146_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2146_L1_SetProperty2(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], Si2146_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) {
      if (fef == 0) {
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2147_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2147_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2147_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2147_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2147_L1_SetProperty2(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], Si2147_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) {
      if (fef == 0) {
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], Si2148_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
      if (fef == 0) {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2148B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2148B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
      if (fef == 0) {
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2151_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2151_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2151_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2151_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) {
      if (fef == 0) {
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2156_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2156_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2156_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2156_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2156_L1_SetProperty2(silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) {
      if (fef == 0) {
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2157_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2157_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2157_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2157_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2157_L1_SetProperty2(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], Si2157_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) {
      if (fef == 0) {
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2158_L1_SetProperty2(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], Si2158_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
      if (fef == 0) {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2158B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2158B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) {
      if (fef == 0) {
        silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2173_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2173_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2173_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2173_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2173_L1_SetProperty2(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) {
      if (fef == 0) {
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2176_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2176_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2176_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2176_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2176_L1_SetProperty2(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) {
      if (fef == 0) {
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2177_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2177_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2177_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2177_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2177_L1_SetProperty2(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], Si2177_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) {
      if (fef == 0) {
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2178_L1_SetProperty2(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], Si2178_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
      if (fef == 0) {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2178B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2178B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) {
      if (fef == 0) {
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2190_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2190_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2190_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2190_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2190_L1_SetProperty2(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], Si2190_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) {
      if (fef == 0) {
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2191_L1_SetProperty2(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], Si2191_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
      if (fef == 0) {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191B_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2191B_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2191B_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) {
      if (fef == 0) {
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2196_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2196_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;
      } else {
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.if_agc_speed          = Si2196_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;
        silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_speed.agc_decim             = Si2196_DTV_AGC_SPEED_PROP_AGC_DECIM_4;
      }
      return Si2196_L1_SetProperty2(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_DTV_AGC_SPEED_PROP_CODE);
  }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_TER_Tuner_DTV_AGC_AUTO_FREEZE  (SILABS_TER_TUNER_Context *silabs_tuner, int fef) {
  silabs_tuner = silabs_tuner;
  fef          = fef;
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) {
      if (fef == 0) {
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 255;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 1;
      } else {
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 9;
        silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 63;
      }
      return Si2141_L1_SetProperty2(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], Si2141_DTV_AGC_AUTO_FREEZE_PROP_CODE);
  }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) {
      if (fef == 0) {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 255;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 1;
      } else {
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 9;
        silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 63;
      }
      return Si2148B_L1_SetProperty2(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], Si2148B_DTV_AGC_AUTO_FREEZE_PROP_CODE);
  }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) {
      if (fef == 0) {
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 255;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 1;
      } else {
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 9;
        silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 63;
      }
      return Si2151_L1_SetProperty2(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], Si2151_DTV_AGC_AUTO_FREEZE_PROP_CODE);
  }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) {
      if (fef == 0) {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 255;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 1;
      } else {
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 9;
        silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 63;
      }
      return Si2158B_L1_SetProperty2(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], Si2158B_DTV_AGC_AUTO_FREEZE_PROP_CODE);
  }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) {
      if (fef == 0) {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 255;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 1;
      } else {
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 9;
        silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 63;
      }
      return Si2178B_L1_SetProperty2(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], Si2178B_DTV_AGC_AUTO_FREEZE_PROP_CODE);
  }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) {
      if (fef == 0) {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 255;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 1;
      } else {
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.thld          = 9;
        silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index]->prop->dtv_agc_auto_freeze.timeout       = 63;
      }
      return Si2191B_L1_SetProperty2(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], Si2191B_DTV_AGC_AUTO_FREEZE_PROP_CODE);
  }
#endif /* TER_TUNER_Si2191B */
  return -1;
}
int   SiLabs_LoadVideofilter                (SILABS_TER_TUNER_Context *silabs_tuner, vid_filt_struct *vidFiltTable, int lines) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_LoadVideofilter(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_LoadVideofilter(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], vidFiltTable->vid_filt_table, lines); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_LoadVideofilter(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return -1; /* Si2148_LoadVideofilter is not declared in Si2148_L2_API.h */ }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_LoadVideofilter(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_LoadVideofilter(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return 0; }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_LoadVideofilter(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_LoadVideofilter(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_LoadVideofilter(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_LoadVideofilter(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], vidFiltTable->vid_filt_table, lines); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_LoadVideofilter(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], vidFiltTable->vid_filt_table, lines); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_LoadVideofilter(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_LoadVideofilter(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_LoadVideofilter(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_LoadVideofilter(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_LoadVideofilter(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_LoadVideofilter(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], vidFiltTable, lines); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_LoadVideofilter(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], vidFiltTable->vid_filt_table, lines); }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
int   SiLabs_ATV_Channel_Scan_M             (SILABS_TER_TUNER_Context *silabs_tuner, unsigned long rangeMinHz, unsigned long rangeMaxHz, int minRSSIdBm, int maxRSSIdBm, int minSNRHalfdB, int maxSNRHalfdB) {

	return 0;
#if 0
  minSNRHalfdB = minSNRHalfdB; /* To avoid compilation warnings */
  maxSNRHalfdB = maxSNRHalfdB; /* To avoid compilation warnings */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return 0; }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return 0; }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_ATV_Channel_Scan_M(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return 0; }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return 0; }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return 0; }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return 0; }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_ATV_Channel_Scan_M(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_ATV_Channel_Scan_M(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return 0; }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_ATV_Channel_Scan_M(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_ATV_Channel_Scan_M(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_ATV_Channel_Scan_M(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_ATV_Channel_Scan_M(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_ATV_Channel_Scan_M(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_ATV_Channel_Scan_M(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_ATV_Channel_Scan_M(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_ATV_Channel_Scan_M(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_ATV_Channel_Scan_M(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2196 */
  return -1;

#endif
}
int   SiLabs_ATV_Channel_Scan_PAL           (SILABS_TER_TUNER_Context *silabs_tuner, unsigned long rangeMinHz, unsigned long rangeMaxHz, int minRSSIdBm, int maxRSSIdBm, int minSNRHalfdB, int maxSNRHalfdB) {

return 0;

#if 0
  minSNRHalfdB = minSNRHalfdB; /* To avoid compilation warnings */
  maxSNRHalfdB = maxSNRHalfdB; /* To avoid compilation warnings */
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return 0; }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return 0; }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_ATV_Channel_Scan_PAL(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return 0; }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return 0; }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return 0; }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  if (silabs_tuner->ter_tuner_code == 0x2156) { return 0; }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_ATV_Channel_Scan_PAL(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_ATV_Channel_Scan_PAL(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return 0; }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  if (silabs_tuner->ter_tuner_code == 0x2173) { return Si2173_ATV_Channel_Scan_PAL(silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  if (silabs_tuner->ter_tuner_code == 0x2176) { return Si2176_ATV_Channel_Scan_PAL(silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_ATV_Channel_Scan_PAL(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_ATV_Channel_Scan_PAL(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_ATV_Channel_Scan_PAL(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_ATV_Channel_Scan_PAL(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_ATV_Channel_Scan_PAL(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_ATV_Channel_Scan_PAL(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  if (silabs_tuner->ter_tuner_code == 0x2196) { return Si2196_ATV_Channel_Scan_PAL(silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], rangeMinHz, rangeMaxHz, minRSSIdBm, maxRSSIdBm, minSNRHalfdB, maxSNRHalfdB); }
#endif /* TER_TUNER_Si2196 */
  return -1;

#endif
}
int   SiLabs_AGC_Override                   (SILABS_TER_TUNER_Context *silabs_tuner, unsigned char mode ) {
#ifdef    TER_TUNER_Si2141
  if (silabs_tuner->ter_tuner_code == 0x2141) { return Si2141_AGC_Override(silabs_tuner->Si2141_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2141 */
#ifdef    TER_TUNER_Si2146
  if (silabs_tuner->ter_tuner_code == 0x2146) { return Si2146_AGC_Override(silabs_tuner->Si2146_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2146 */
#ifdef    TER_TUNER_Si2147
  if (silabs_tuner->ter_tuner_code == 0x2147) { return Si2147_AGC_Override(silabs_tuner->Si2147_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2147 */
#ifdef    TER_TUNER_Si2148
  if (silabs_tuner->ter_tuner_code == 0x2148) { return Si2148_AGC_Override(silabs_tuner->Si2148_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2148 */
#ifdef    TER_TUNER_Si2148B
  if (silabs_tuner->ter_tuner_code == 0x2148B) { return Si2148B_AGC_Override(silabs_tuner->Si2148B_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2148B */
#ifdef    TER_TUNER_Si2151
  if (silabs_tuner->ter_tuner_code == 0x2151) { return Si2151_AGC_Override(silabs_tuner->Si2151_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2151 */
#ifdef    TER_TUNER_Si2156
  #define Si2156_FORCE_NORMAL_AGC       0
  #define Si2156_FORCE_MAX_AGC          1
  #define Si2156_FORCE_TOP_AGC          2
  if (silabs_tuner->ter_tuner_code == 0x2156) {
    switch (mode) {
        case  Si2156_FORCE_NORMAL_AGC: return Si2156_L1_AGC_OVERRIDE (silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2156_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE); break;
        case  Si2156_FORCE_MAX_AGC   : return Si2156_L1_AGC_OVERRIDE (silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_ENABLE ,Si2156_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE); break;
        case  Si2156_FORCE_TOP_AGC   : return Si2156_L1_AGC_OVERRIDE (silabs_tuner->Si2156_Tuner[silabs_tuner->tuner_index], Si2156_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2156_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_ENABLE ); break;
        default: break;
    }
  }
#endif /* TER_TUNER_Si2156 */
#ifdef    TER_TUNER_Si2157
  if (silabs_tuner->ter_tuner_code == 0x2157) { return Si2157_AGC_Override(silabs_tuner->Si2157_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2157 */
#ifdef    TER_TUNER_Si2158
  if (silabs_tuner->ter_tuner_code == 0x2158) { return Si2158_AGC_Override(silabs_tuner->Si2158_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2158 */
#ifdef    TER_TUNER_Si2158B
  if (silabs_tuner->ter_tuner_code == 0x2158B) { return Si2158B_AGC_Override(silabs_tuner->Si2158B_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2158B */
#ifdef    TER_TUNER_Si2173
  #define Si2173_FORCE_NORMAL_AGC       0
  #define Si2173_FORCE_MAX_AGC          1
  #define Si2173_FORCE_TOP_AGC          2
  if (silabs_tuner->ter_tuner_code == 0x2173) {
    switch (mode) {
        case  Si2173_FORCE_NORMAL_AGC: return Si2173_L1_AGC_OVERRIDE (silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2173_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE); break;
        case  Si2173_FORCE_MAX_AGC   : return Si2173_L1_AGC_OVERRIDE (silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_ENABLE ,Si2173_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE); break;
        case  Si2173_FORCE_TOP_AGC   : return Si2173_L1_AGC_OVERRIDE (silabs_tuner->Si2173_Tuner[silabs_tuner->tuner_index], Si2173_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2173_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_ENABLE ); break;
        default: break;
    }
  }
#endif /* TER_TUNER_Si2173 */
#ifdef    TER_TUNER_Si2176
  #define Si2176_FORCE_NORMAL_AGC       0
  #define Si2176_FORCE_MAX_AGC          1
  #define Si2176_FORCE_TOP_AGC          2
  if (silabs_tuner->ter_tuner_code == 0x2176) {
    switch (mode) {
        case  Si2176_FORCE_NORMAL_AGC: return Si2176_L1_AGC_OVERRIDE (silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2176_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE); break;
        case  Si2176_FORCE_MAX_AGC   : return Si2176_L1_AGC_OVERRIDE (silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_ENABLE ,Si2176_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE); break;
        case  Si2176_FORCE_TOP_AGC   : return Si2176_L1_AGC_OVERRIDE (silabs_tuner->Si2176_Tuner[silabs_tuner->tuner_index], Si2176_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2176_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_ENABLE ); break;
        default: break;
    }
  }
#endif /* TER_TUNER_Si2176 */
#ifdef    TER_TUNER_Si2177
  if (silabs_tuner->ter_tuner_code == 0x2177) { return Si2177_AGC_Override(silabs_tuner->Si2177_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2177 */
#ifdef    TER_TUNER_Si2178
  if (silabs_tuner->ter_tuner_code == 0x2178) { return Si2178_AGC_Override(silabs_tuner->Si2178_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2178 */
#ifdef    TER_TUNER_Si2178B
  if (silabs_tuner->ter_tuner_code == 0x2178B) { return Si2178B_AGC_Override(silabs_tuner->Si2178B_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2178B */
#ifdef    TER_TUNER_Si2190
  if (silabs_tuner->ter_tuner_code == 0x2190) { return Si2190_AGC_Override(silabs_tuner->Si2190_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2190 */
#ifdef    TER_TUNER_Si2191
  if (silabs_tuner->ter_tuner_code == 0x2191) { return Si2191_AGC_Override(silabs_tuner->Si2191_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2191 */
#ifdef    TER_TUNER_Si2191B
  if (silabs_tuner->ter_tuner_code == 0x2191B) { return Si2191B_AGC_Override(silabs_tuner->Si2191B_Tuner[silabs_tuner->tuner_index], mode); }
#endif /* TER_TUNER_Si2191B */
#ifdef    TER_TUNER_Si2196
  #define Si2196_FORCE_NORMAL_AGC       0
  #define Si2196_FORCE_MAX_AGC          1
  #define Si2196_FORCE_TOP_AGC          2
  if (silabs_tuner->ter_tuner_code == 0x2196) {
    switch (mode) {
        case  Si2196_FORCE_NORMAL_AGC: return Si2196_L1_AGC_OVERRIDE (silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2196_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE); break;
        case  Si2196_FORCE_MAX_AGC   : return Si2196_L1_AGC_OVERRIDE (silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_ENABLE ,Si2196_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE); break;
        case  Si2196_FORCE_TOP_AGC   : return Si2196_L1_AGC_OVERRIDE (silabs_tuner->Si2196_Tuner[silabs_tuner->tuner_index], Si2196_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2196_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_ENABLE ); break;
        default: break;
    }
  }
#endif /* TER_TUNER_Si2196 */
  return -1;
}
#ifdef    SILABS_API_TEST_PIPE
/************************************************************************************************************************
  SiLabs_TER_Tuner_Test function
  Use:        Generic test pipe function
              Used to send a generic command to the selected tuner.
  Returns:    0 if the command is unknow, 1 otherwise
  Porting:    Mostly used for debug during sw development.
************************************************************************************************************************/
int   SiLabs_TER_Tuner_Test                 (SILABS_TER_TUNER_Context *silabs_tuner, char *target, char *cmd, char *sub_cmd, double dval, double *retdval, char **rettxt)
{
  CONNECTION_TYPE mode;
  int track_mode;
  silabs_tuner = silabs_tuner; /* To avoid compiler warning if not used */
  *retdval   = 0;
  mode       = USB;
  track_mode = 0;
  SiTRACE("\nSiLabs_TER_Tuner_Test %s %s %s %f...\n", target, cmd, sub_cmd, dval);
       if (strcmp_nocase(target,"help"      ) == 0) {
    sprintf(*rettxt, 1000, "\n Possible SiLabs_TER_Tuner est commands:\n\
tuner get_rev                          : display part and fw info\n\
tuner part_info                        : display part info\n\
");
 return 1;
  }
  else if (strcmp_nocase(target,"ter_tuner" ) == 0) {
    if (strcmp_nocase(cmd,"address"     ) == 0) {
      if ( (dval >= 0xc0) & (dval <= 0xc6) ) {silabs_tuner->i2c->address = (int)dval; }
       *retdval = (double)silabs_tuner->i2c->address; sprintf(*rettxt, 1000, "0x%02x", (int)*retdval ); return 1;
    }
    if (strcmp_nocase(cmd,"dtv_rf_top"  ) == 0) {
      #ifdef    TER_TUNER_Si2148
      silabs_tuner->Si2148_Tuner[0]->prop->dtv_rf_top.dtv_rf_top = (int)dval;
      Si2148_L1_SetProperty2(silabs_tuner->Si2148_Tuner[0], Si2148_DTV_RF_TOP_PROP_CODE);
      Si2148_L1_DTV_RESTART (silabs_tuner->Si2148_Tuner[0]);
      sprintf(*rettxt, 1000, "Si2148 TOP set to %d\n", silabs_tuner->Si2148_Tuner[0]->prop->dtv_rf_top.dtv_rf_top );
      #endif /* TER_TUNER_Si2148 */
      return 1;
    }
    if (strcmp_nocase(cmd,"get_rev"     ) == 0) {
      SiLabs_TER_Tuner_Part_Info(silabs_tuner); /* Also using 'part', which comes from PART_INFO */
      SiLabs_TER_Tuner_Get_Rev(silabs_tuner);
      sprintf(*rettxt, 1000, "Si21%2d Part running 'FW_%c_%cb%d'\n", silabs_tuner->part, silabs_tuner->part_info.major, silabs_tuner->part_info.minor, silabs_tuner->part_info.build );
      return 1;
    }
    if (strcmp_nocase(cmd,"part_info"   ) == 0) {
      SiLabs_TER_Tuner_Part_Info(silabs_tuner);
      sprintf(*rettxt, 1000, "Full Info 'Si21%02d-%c%c%c ROM%x NVM%c_%cb%d'\n\n\n",
                silabs_tuner->part,
                silabs_tuner->part_info.chiprev + 0x40,
                silabs_tuner->part_info.major,
                silabs_tuner->part_info.minor,
                silabs_tuner->romid,
                silabs_tuner->part_info.major,
                silabs_tuner->part_info.minor,
                silabs_tuner->part_info.build );
      return 1;
    }
    if (strcmp_nocase(cmd,"agc_external") == 0) {
      SiLabs_TER_Tuner_AGC_External(silabs_tuner);
      sprintf(*rettxt, 1000, "agc_external done\n");
      return 1;
    }
    if (strcmp_nocase(cmd,"hw_init"     ) == 0) {
      SiLabs_TER_Tuner_Part_Info(silabs_tuner);
      sprintf(*rettxt, 1000, "sw_init done\n");
      return 1;
    }
    if (strcmp_nocase(cmd,"init_example") == 0) {
      SILABS_TER_TUNER_Context  *ter_tuner;
      SILABS_TER_TUNER_Context   ter_tunerObj;
      ter_tuner = &ter_tunerObj;
      SiLabs_TER_Tuner_SW_Init (ter_tuner, 0xc0);
      #ifdef    TER_TUNER_Si2147
      ter_tuner->Si2147_Tuner[0]->i2c->address = 0xc0;
      ter_tuner->Si2147_Tuner[0]->cmd->power_up.clock_mode      = Si2147_POWER_UP_CMD_CLOCK_MODE_XTAL;
      ter_tuner->Si2147_Tuner[0]->cmd->power_up.en_xout         = Si2147_POWER_UP_CMD_EN_XOUT_EN_XOUT;
      ter_tuner->Si2147_Tuner[0]->cmd->config_clocks.clock_mode = Si2147_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;
      #endif /* TER_TUNER_Si2147 */
      #ifdef    TER_TUNER_Si2177
      ter_tuner->Si2177_Tuner[0]->i2c->address = 0xc2;
      ter_tuner->Si2177_Tuner[0]->cmd->power_up.clock_mode      = Si2177_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
      ter_tuner->Si2177_Tuner[0]->cmd->power_up.en_xout         = Si2177_POWER_UP_CMD_EN_XOUT_EN_XOUT;
      ter_tuner->Si2177_Tuner[0]->cmd->config_clocks.clock_mode = Si2177_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK;
      #endif /* TER_TUNER_Si2177 */

      #ifdef    TER_TUNER_Si2147
      ter_tuner->Si2147_Tuner[1]->i2c->address = 0xc4;
      ter_tuner->Si2147_Tuner[1]->cmd->power_up.clock_mode      = Si2147_POWER_UP_CMD_CLOCK_MODE_XTAL;
      ter_tuner->Si2147_Tuner[1]->cmd->power_up.en_xout         = Si2147_POWER_UP_CMD_EN_XOUT_EN_XOUT;
      ter_tuner->Si2147_Tuner[1]->cmd->config_clocks.clock_mode = Si2147_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL;
      #endif /* TER_TUNER_Si2147 */
      #ifdef    TER_TUNER_Si2177
      ter_tuner->Si2177_Tuner[1]->i2c->address = 0xc6;
      ter_tuner->Si2177_Tuner[1]->cmd->power_up.clock_mode      = Si2177_POWER_UP_CMD_CLOCK_MODE_EXTCLK;
      ter_tuner->Si2177_Tuner[1]->cmd->power_up.en_xout         = Si2177_POWER_UP_CMD_EN_XOUT_EN_XOUT;
      ter_tuner->Si2177_Tuner[1]->cmd->config_clocks.clock_mode = Si2177_CONFIG_CLOCKS_CMD_CLOCK_MODE_EXTCLK;
      #endif /* TER_TUNER_Si2177 */

      #ifdef    TER_TUNER_Si2147
      SiLabs_TER_Tuner_Select_Tuner (ter_tuner, 0x2147, 0);
      SiLabs_TER_Tuner_HW_Connect   (ter_tuner, mode);
      SiLabs_TER_Tuner_bytes_trace  (ter_tuner, track_mode);
      SiLabs_TER_Tuner_HW_Init      (ter_tuner);
      #endif /* TER_TUNER_Si2147 */
      #ifdef    TER_TUNER_Si2177
      SiLabs_TER_Tuner_Select_Tuner (ter_tuner, 0x2177, 0);
      SiLabs_TER_Tuner_HW_Connect   (ter_tuner, mode);
      SiLabs_TER_Tuner_bytes_trace  (ter_tuner, track_mode);
      SiLabs_TER_Tuner_HW_Init      (ter_tuner);
      #endif /* TER_TUNER_Si2177 */
      #ifdef    TER_TUNER_Si2147
      SiLabs_TER_Tuner_Select_Tuner (ter_tuner, 0x2147, 1);
      SiLabs_TER_Tuner_HW_Connect   (ter_tuner, mode);
      SiLabs_TER_Tuner_bytes_trace  (ter_tuner, track_mode);
      SiLabs_TER_Tuner_HW_Init      (ter_tuner);
      #endif /* TER_TUNER_Si2147 */
      #ifdef    TER_TUNER_Si2177
      SiLabs_TER_Tuner_Select_Tuner (ter_tuner, 0x2177, 1);
      SiLabs_TER_Tuner_HW_Connect   (ter_tuner, mode);
      SiLabs_TER_Tuner_bytes_trace  (ter_tuner, track_mode);
      SiLabs_TER_Tuner_HW_Init      (ter_tuner);
      #endif /* TER_TUNER_Si2177 */
      sprintf(*rettxt, 1000, "sw_init and hw_init done\n");
      return 1;
    }
  }
  else {
    sprintf(*rettxt, 1000, "unknown '%s' command for silabs_tuner, can not process '%s %s %s %f'\n", cmd, target, cmd, sub_cmd, dval);
    SiTRACE(*rettxt);
    SiERROR(*rettxt);
    return 0;
  }
  return 0;
}
#endif /* SILABS_API_TEST_PIPE */
/* End of SiLabs_TER_Tuner_API.c */

