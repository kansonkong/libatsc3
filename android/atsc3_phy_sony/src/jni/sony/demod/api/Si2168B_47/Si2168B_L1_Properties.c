/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168B Layer 1 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   API properties definitions
   FILE: Si2168B_L1_Properties.c
   Supported IC : Si2168B
   Compiled for ROM 0 firmware 3_A_build_3
   Revision: REVISION_NAME
   Tag:  V0.3.8_FW_4_4b17
   Date: May 22 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
/* Change log:

 As from V0.3.7:
   <improvement> [TS_spurious/DUAL] In Si2168B_storeUserProperties:
      prop->dd_ts_setup_par.ts_data_strength                                 =     4;
      prop->dd_ts_setup_par.ts_data_shape                                    =     0;
      prop->dd_ts_setup_par.ts_clk_strength                                  =     4;
      prop->dd_ts_setup_par.ts_clk_shape                                     =     0;

      prop->dd_ts_setup_ser.ts_data_strength                                 =     7;
      prop->dd_ts_setup_ser.ts_data_shape                                    =     0;
      prop->dd_ts_setup_ser.ts_clk_strength                                  =     7;
      prop->dd_ts_setup_ser.ts_clk_shape                                     =     0;

   <improvement> [Teracom/BEST_MUX] In Si2168B_storeUserProperties: Changing BER resolution to allow BEST_MUX detection
     prop->dd_ber_resol.exp  = 5;
     prop->dd_ber_resol.mant = 8;

 As from V0.3.3:
   <new_feature> {SAT scan DEBUG] In Si2168B_PackProperty: adding scan_debug field, to enable drawing the SAT spectrum

 As from V0.3.2:
   <improvement> [SQI/BestMux] In Si2168B_storeUserProperties:
   prop->dd_ssi_sqi_param.sqi_average = 16;
   This allows the SQI to be as accurate as required and fast for the Best Mux selection.

As from V0.3.1: 
    <improvement> [code_checkers] In Si2168B_L1_PropertyText:   using snprintf with a fixed size of 1000.
                  Declaring msg with a length of 1000.
                  Updating function comments in string-related functions

 As from V0.2.7:
  Enabling   prop->dd_int_sense.pos_bit5 (DVBx_STATUS 'no_signal' interrupt) to use rsqint_bit5 as expected

 As from V0.1.0:
  Adding new field in property SCAN_TER_CONFIG: scan_debug
   This is useful to allow spectrum display during blindscan, for debug purposes.

 As from V0.0.1:
  Adding new field in property DD_TS_MODE_PROP: ts_freq_resolution
  Adding new property DD_TS_SERIAL_DIFF

 As from V0.0.0:
  Initial version (based on Si2169 code V2.6.1)
****************************************************************************************/
#define   Si2168B_COMMAND_PROTOTYPES

#include "Si2168B_Platform_Definition.h"


//Edit by ITE
//snprintf -> sprintf
//strncat -> strcat

/***********************************************************************************************************************
  Si2168B_storeUserProperties function
  Use:        property preparation function
              Used to fill the prop structure with user values.
  Parameter: *prop    a property structure to be filled

  Returns:    void
 ***********************************************************************************************************************/
void          Si2168B_storeUserProperties    (Si2168B_PropObj   *prop) {
  SiTRACE("Si2168B_storeUserProperties\n");
#ifdef    Si2168B_MASTER_IEN_PROP
  prop->master_ien.ddien                                                 = Si2168B_MASTER_IEN_PROP_DDIEN_OFF   ; /* (default 'OFF') */
  prop->master_ien.scanien                                               = Si2168B_MASTER_IEN_PROP_SCANIEN_OFF ; /* (default 'OFF') */
  prop->master_ien.errien                                                = Si2168B_MASTER_IEN_PROP_ERRIEN_OFF  ; /* (default 'OFF') */
  prop->master_ien.ctsien                                                = Si2168B_MASTER_IEN_PROP_CTSIEN_OFF  ; /* (default 'OFF') */
#endif /* Si2168B_MASTER_IEN_PROP */

#ifdef    Si2168B_DD_BER_RESOL_PROP
  prop->dd_ber_resol.exp                                                 =     5; /* (default     7) */
  prop->dd_ber_resol.mant                                                =     8; /* (default     1) */
#endif /* Si2168B_DD_BER_RESOL_PROP */

#ifdef    Si2168B_DD_CBER_RESOL_PROP
  prop->dd_cber_resol.exp                                                =     5; /* (default     5) */
  prop->dd_cber_resol.mant                                               =     1; /* (default     1) */
#endif /* Si2168B_DD_CBER_RESOL_PROP */

#ifdef    Si2168B_DD_DISEQC_FREQ_PROP
  prop->dd_diseqc_freq.freq_hz                                           = 22000; /* (default 22000) */
#endif /* Si2168B_DD_DISEQC_FREQ_PROP */

#ifdef    Si2168B_DD_DISEQC_PARAM_PROP
  prop->dd_diseqc_param.sequence_mode                                    = Si2168B_DD_DISEQC_PARAM_PROP_SEQUENCE_MODE_GAP ; /* (default 'GAP') */
#endif /* Si2168B_DD_DISEQC_PARAM_PROP */

#ifdef    Si2168B_DD_FER_RESOL_PROP
  prop->dd_fer_resol.exp                                                 =     3; /* (default     3) */
  prop->dd_fer_resol.mant                                                =     1; /* (default     1) */
#endif /* Si2168B_DD_FER_RESOL_PROP */

#ifdef    Si2168B_DD_IEN_PROP
  prop->dd_ien.ien_bit0                                                  = Si2168B_DD_IEN_PROP_IEN_BIT0_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit1                                                  = Si2168B_DD_IEN_PROP_IEN_BIT1_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit2                                                  = Si2168B_DD_IEN_PROP_IEN_BIT2_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit3                                                  = Si2168B_DD_IEN_PROP_IEN_BIT3_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit4                                                  = Si2168B_DD_IEN_PROP_IEN_BIT4_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit5                                                  = Si2168B_DD_IEN_PROP_IEN_BIT5_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit6                                                  = Si2168B_DD_IEN_PROP_IEN_BIT6_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit7                                                  = Si2168B_DD_IEN_PROP_IEN_BIT7_DISABLE ; /* (default 'DISABLE') */
#endif /* Si2168B_DD_IEN_PROP */

#ifdef    Si2168B_DD_IF_INPUT_FREQ_PROP
  prop->dd_if_input_freq.offset                                          =  5000; /* (default  5000) */
#endif /* Si2168B_DD_IF_INPUT_FREQ_PROP */

#ifdef    Si2168B_DD_INT_SENSE_PROP
  prop->dd_int_sense.neg_bit0                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT0_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit1                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT1_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit2                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT2_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit3                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT3_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit4                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT4_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit5                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT5_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit6                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT6_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit7                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT7_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit0                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT0_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit1                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT1_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit2                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT2_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit3                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT3_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit4                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT4_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit5                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT5_ENABLE  ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit6                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT6_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit7                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT7_DISABLE ; /* (default 'DISABLE') */
#endif /* Si2168B_DD_INT_SENSE_PROP */

#ifdef    Si2168B_DD_MODE_PROP
  prop->dd_mode.bw                                                       = Si2168B_DD_MODE_PROP_BW_BW_8MHZ              ; /* (default 'BW_8MHZ') */
  prop->dd_mode.modulation                                               = Si2168B_DD_MODE_PROP_MODULATION_DVBT         ; /* (default 'DVBT') */
  prop->dd_mode.invert_spectrum                                          = Si2168B_DD_MODE_PROP_INVERT_SPECTRUM_NORMAL  ; /* (default 'NORMAL') */
  prop->dd_mode.auto_detect                                              = Si2168B_DD_MODE_PROP_AUTO_DETECT_NONE        ; /* (default 'NONE') */
#endif /* Si2168B_DD_MODE_PROP */

#ifdef    Si2168B_DD_PER_RESOL_PROP
  prop->dd_per_resol.exp                                                 =     5; /* (default     5) */
  prop->dd_per_resol.mant                                                =     1; /* (default     1) */
#endif /* Si2168B_DD_PER_RESOL_PROP */

#ifdef    Si2168B_DD_RSQ_BER_THRESHOLD_PROP
  prop->dd_rsq_ber_threshold.exp                                         =     1; /* (default     1) */
  prop->dd_rsq_ber_threshold.mant                                        =    10; /* (default    10) */
#endif /* Si2168B_DD_RSQ_BER_THRESHOLD_PROP */

#ifdef    Si2168B_DD_SSI_SQI_PARAM_PROP
  prop->dd_ssi_sqi_param.sqi_average                                     =    30; /* (default     1) */
#endif /* Si2168B_DD_SSI_SQI_PARAM_PROP */

#ifdef    Si2168B_DD_TS_FREQ_PROP
  prop->dd_ts_freq.req_freq_10khz                                        =   720; /* (default   720) */
#endif /* Si2168B_DD_TS_FREQ_PROP */

#ifdef    Si2168B_DD_TS_MODE_PROP
  prop->dd_ts_mode.mode                                                  = Si2168B_DD_TS_MODE_PROP_MODE_TRISTATE                ; /* (default 'TRISTATE') */
  prop->dd_ts_mode.clock                                                 = Si2168B_DD_TS_MODE_PROP_CLOCK_AUTO_FIXED             ; /* (default 'AUTO_FIXED') */
  prop->dd_ts_mode.clk_gapped_en                                         = Si2168B_DD_TS_MODE_PROP_CLK_GAPPED_EN_DISABLED       ; /* (default 'DISABLED') */
  prop->dd_ts_mode.ts_err_polarity                                       = Si2168B_DD_TS_MODE_PROP_TS_ERR_POLARITY_NOT_INVERTED ; /* (default 'NOT_INVERTED') */
  prop->dd_ts_mode.special                                               = Si2168B_DD_TS_MODE_PROP_SPECIAL_FULL_TS              ; /* (default 'FULL_TS') */
  prop->dd_ts_mode.ts_freq_resolution                                    = Si2168B_DD_TS_MODE_PROP_TS_FREQ_RESOLUTION_NORMAL       ; /* (default 'NORMAL') */
#endif /* Si2168B_DD_TS_MODE_PROP */

#ifdef    Si2168B_DD_TS_SERIAL_DIFF_PROP
  prop->dd_ts_serial_diff.ts_data1_strength                              =    15; /* (default    15) */
  prop->dd_ts_serial_diff.ts_data1_shape                                 =     3; /* (default     3) */
  prop->dd_ts_serial_diff.ts_data2_strength                              =    15; /* (default    15) */
  prop->dd_ts_serial_diff.ts_data2_shape                                 =     3; /* (default     3) */
  prop->dd_ts_serial_diff.ts_clkb_on_data1                               = Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_CLKB_ON_DATA1_DISABLE   ; /* (default 'DISABLE') */
  prop->dd_ts_serial_diff.ts_data0b_on_data2                             = Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA0B_ON_DATA2_DISABLE ; /* (default 'DISABLE') */
#endif /* Si2168B_DD_TS_SERIAL_DIFF_PROP */

#ifdef    Si2168B_DD_TS_SETUP_PAR_PROP
  prop->dd_ts_setup_par.ts_data_strength                                 =     4; /* (default     3) */
  prop->dd_ts_setup_par.ts_data_shape                                    =     0; /* (default     1) */
  prop->dd_ts_setup_par.ts_clk_strength                                  =     4; /* (default     3) */
  prop->dd_ts_setup_par.ts_clk_shape                                     =     0; /* (default     1) */
  prop->dd_ts_setup_par.ts_clk_invert                                    = Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_INVERTED    ; /* (default 'INVERTED') */
  prop->dd_ts_setup_par.ts_clk_shift                                     =     0; /* (default     0) */
#endif /* Si2168B_DD_TS_SETUP_PAR_PROP */

#ifdef    Si2168B_DD_TS_SETUP_SER_PROP
  prop->dd_ts_setup_ser.ts_data_strength                                 =     7; /* (default    15) */
  prop->dd_ts_setup_ser.ts_data_shape                                    =     0; /* (default     3) */
  prop->dd_ts_setup_ser.ts_clk_strength                                  =     7; /* (default    15) */
  prop->dd_ts_setup_ser.ts_clk_shape                                     =     0; /* (default     3) */
  prop->dd_ts_setup_ser.ts_clk_invert                                    = Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_INVERTED      ; /* (default 'INVERTED') */
  prop->dd_ts_setup_ser.ts_sync_duration                                 = Si2168B_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_FIRST_BYTE ; /* (default 'FIRST_BYTE') */
  prop->dd_ts_setup_ser.ts_byte_order                                    = Si2168B_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_MSB_FIRST     ; /* (default 'MSB_FIRST') */
#endif /* Si2168B_DD_TS_SETUP_SER_PROP */

#ifdef    Si2168B_DVBC_ADC_CREST_FACTOR_PROP
  prop->dvbc_adc_crest_factor.crest_factor                               =   112; /* (default   112) */
#endif /* Si2168B_DVBC_ADC_CREST_FACTOR_PROP */

#ifdef    Si2168B_DVBC_AFC_RANGE_PROP
  prop->dvbc_afc_range.range_khz                                         =   100; /* (default   100) */
#endif /* Si2168B_DVBC_AFC_RANGE_PROP */

#ifdef    Si2168B_DVBC_CONSTELLATION_PROP
  prop->dvbc_constellation.constellation                                 = Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_AUTO ; /* (default 'AUTO') */
#endif /* Si2168B_DVBC_CONSTELLATION_PROP */

#ifdef    Si2168B_DVBC_SYMBOL_RATE_PROP
  prop->dvbc_symbol_rate.rate                                            =  6900; /* (default  6900) */
#endif /* Si2168B_DVBC_SYMBOL_RATE_PROP */




#ifdef    Si2168B_DVBT_ADC_CREST_FACTOR_PROP
  prop->dvbt_adc_crest_factor.crest_factor                               =   130; /* (default   130) */
#endif /* Si2168B_DVBT_ADC_CREST_FACTOR_PROP */

#ifdef    Si2168B_DVBT_AFC_RANGE_PROP
  prop->dvbt_afc_range.range_khz                                         =   550; /* (default   550) */
#endif /* Si2168B_DVBT_AFC_RANGE_PROP */

#ifdef    Si2168B_DVBT_HIERARCHY_PROP
  prop->dvbt_hierarchy.stream                                            = Si2168B_DVBT_HIERARCHY_PROP_STREAM_HP ; /* (default 'HP') */
#endif /* Si2168B_DVBT_HIERARCHY_PROP */


#ifdef    Si2168B_DVBT2_ADC_CREST_FACTOR_PROP
  prop->dvbt2_adc_crest_factor.crest_factor                              =   130; /* (default   130) */
#endif /* Si2168B_DVBT2_ADC_CREST_FACTOR_PROP */

#ifdef    Si2168B_DVBT2_AFC_RANGE_PROP
  prop->dvbt2_afc_range.range_khz                                        =   550; /* (default   550) */
#endif /* Si2168B_DVBT2_AFC_RANGE_PROP */

#ifdef    Si2168B_DVBT2_FEF_TUNER_PROP
  prop->dvbt2_fef_tuner.tuner_delay                                      =     1; /* (default     1) */
  prop->dvbt2_fef_tuner.tuner_freeze_time                                =     1; /* (default     1) */
  prop->dvbt2_fef_tuner.tuner_unfreeze_time                              =     1; /* (default     1) */
#endif /* Si2168B_DVBT2_FEF_TUNER_PROP */

#ifdef    Si2168B_DVBT2_MODE_PROP
  prop->dvbt2_mode.lock_mode                                             = Si2168B_DVBT2_MODE_PROP_LOCK_MODE_ANY ; /* (default 'ANY') */
#endif /* Si2168B_DVBT2_MODE_PROP */


#ifdef    Si2168B_MCNS_ADC_CREST_FACTOR_PROP
  prop->mcns_adc_crest_factor.crest_factor                               =   112; /* (default   112) */
#endif /* Si2168B_MCNS_ADC_CREST_FACTOR_PROP */

#ifdef    Si2168B_MCNS_AFC_RANGE_PROP
  prop->mcns_afc_range.range_khz                                         =   100; /* (default   100) */
#endif /* Si2168B_MCNS_AFC_RANGE_PROP */

#ifdef    Si2168B_MCNS_CONSTELLATION_PROP
  prop->mcns_constellation.constellation                                 = Si2168B_MCNS_CONSTELLATION_PROP_CONSTELLATION_AUTO ; /* (default 'AUTO') */
#endif /* Si2168B_MCNS_CONSTELLATION_PROP */

#ifdef    Si2168B_MCNS_SYMBOL_RATE_PROP
  prop->mcns_symbol_rate.rate                                            =  6900; /* (default  6900) */
#endif /* Si2168B_MCNS_SYMBOL_RATE_PROP */


#ifdef    Si2168B_SCAN_FMAX_PROP
  prop->scan_fmax.scan_fmax                                              =     0; /* (default     0) */
#endif /* Si2168B_SCAN_FMAX_PROP */

#ifdef    Si2168B_SCAN_FMIN_PROP
  prop->scan_fmin.scan_fmin                                              =     0; /* (default     0) */
#endif /* Si2168B_SCAN_FMIN_PROP */

#ifdef    Si2168B_SCAN_IEN_PROP
  prop->scan_ien.buzien                                                  = Si2168B_SCAN_IEN_PROP_BUZIEN_DISABLE ; /* (default 'DISABLE') */
  prop->scan_ien.reqien                                                  = Si2168B_SCAN_IEN_PROP_REQIEN_DISABLE ; /* (default 'DISABLE') */
#endif /* Si2168B_SCAN_IEN_PROP */

#ifdef    Si2168B_SCAN_INT_SENSE_PROP
  prop->scan_int_sense.buznegen                                          = Si2168B_SCAN_INT_SENSE_PROP_BUZNEGEN_ENABLE  ; /* (default 'ENABLE') */
  prop->scan_int_sense.reqnegen                                          = Si2168B_SCAN_INT_SENSE_PROP_REQNEGEN_DISABLE ; /* (default 'DISABLE') */
  prop->scan_int_sense.buzposen                                          = Si2168B_SCAN_INT_SENSE_PROP_BUZPOSEN_DISABLE ; /* (default 'DISABLE') */
  prop->scan_int_sense.reqposen                                          = Si2168B_SCAN_INT_SENSE_PROP_REQPOSEN_ENABLE  ; /* (default 'ENABLE') */
#endif /* Si2168B_SCAN_INT_SENSE_PROP */

#ifdef    Si2168B_SCAN_SAT_CONFIG_PROP
  prop->scan_sat_config.analog_detect                                    = Si2168B_SCAN_SAT_CONFIG_PROP_ANALOG_DETECT_DISABLED ; /* (default 'DISABLED') */
  prop->scan_sat_config.reserved1                                        =     0; /* (default     0) */
  prop->scan_sat_config.reserved2                                        =    12; /* (default    12) */
  prop->scan_sat_config.scan_debug                                       =     0; /* (default     0) */
#endif /* Si2168B_SCAN_SAT_CONFIG_PROP */

#ifdef    Si2168B_SCAN_SAT_UNICABLE_BW_PROP
  prop->scan_sat_unicable_bw.scan_sat_unicable_bw                        =     0; /* (default     0) */
#endif /* Si2168B_SCAN_SAT_UNICABLE_BW_PROP */

#ifdef    Si2168B_SCAN_SAT_UNICABLE_MIN_TUNE_STEP_PROP
  prop->scan_sat_unicable_min_tune_step.scan_sat_unicable_min_tune_step  =    50; /* (default    50) */
#endif /* Si2168B_SCAN_SAT_UNICABLE_MIN_TUNE_STEP_PROP */

#ifdef    Si2168B_SCAN_SYMB_RATE_MAX_PROP
  prop->scan_symb_rate_max.scan_symb_rate_max                            =     0; /* (default     0) */
#endif /* Si2168B_SCAN_SYMB_RATE_MAX_PROP */

#ifdef    Si2168B_SCAN_SYMB_RATE_MIN_PROP
  prop->scan_symb_rate_min.scan_symb_rate_min                            =     0; /* (default     0) */
#endif /* Si2168B_SCAN_SYMB_RATE_MIN_PROP */

#ifdef    Si2168B_SCAN_TER_CONFIG_PROP
  prop->scan_ter_config.mode                                             = Si2168B_SCAN_TER_CONFIG_PROP_MODE_BLIND_SCAN          ; /* (default 'BLIND_SCAN') */
  prop->scan_ter_config.analog_bw                                        = Si2168B_SCAN_TER_CONFIG_PROP_ANALOG_BW_8MHZ           ; /* (default '8MHZ') */
  prop->scan_ter_config.search_analog                                    = Si2168B_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_DISABLE    ; /* (default 'DISABLE') */
#endif /* Si2168B_SCAN_TER_CONFIG_PROP */

}
/***********************************************************************************************************************
  Si2168B_L1_SetProperty function
  Use:        property set function
              Used to call L1_SET_PROPERTY with the property Id and data provided.
  Parameter: *api     the Si2168B context
  Parameter: prop     the property Id
  Parameter: data     the property bytes
  Behavior:  This function will only download the property if required.
               Conditions to download the property are:
                - The property changes
                - The propertyWriteMode is set to Si2168B_DOWNLOAD_ALWAYS
                - The property is unknown to Si2168B_PackProperty (this may be useful for debug purpose)
  Returns:    0 if no error, an error code otherwise
 ***********************************************************************************************************************/
unsigned char Si2168B_L1_SetProperty         (L1_Si2168B_Context *api, unsigned int prop_code, int  data) {
#ifdef    Si2168B_GET_PROPERTY_STRING
    char msg[1000];
#endif /* Si2168B_GET_PROPERTY_STRING */
    int            shadowData;
    unsigned char  res;
    unsigned char  reserved;

    reserved          = 0;

    res = Si2168B_PackProperty(api->propShadow, prop_code, &shadowData);

    /* -- Download property only if required --     */
    if ( ((data != shadowData)  || (api->propertyWriteMode == Si2168B_DOWNLOAD_ALWAYS)) ) {
      res = Si2168B_L1_SET_PROPERTY (api, reserved, prop_code, data);
      SiTRACE("\nSi2168B_L1_SetProperty: Setting Property 0x%04x to 0x%04x(%d)\n", prop_code,data,data);
      if (res != NO_Si2168B_ERROR) {
        SiTRACE("\nERROR: Si2168B_L1_SetProperty: %s 0x%04x!\n\n", Si2168B_L1_API_ERROR_TEXT(res), prop_code);
      } else {
        /* Update propShadow with latest data if correctly set */
        Si2168B_UnpackProperty   (api->propShadow, prop_code, data);
        #ifdef    Si2168B_GET_PROPERTY_STRING
        Si2168B_L1_PropertyText(api->propShadow, prop_code, (char*)" ", msg);
        SiTRACE("%s\n",msg);
        #endif /* Si2168B_GET_PROPERTY_STRING */
      }
    }
  return res;
}
/***********************************************************************************************************************
  Si2168B_L1_GetProperty function
  Use:        property get function
              Used to call L1_GET_PROPERTY with the property Id provided.
  Parameter: *api     the Si2168B context
  Parameter: prop     the property Id
  Parameter: *data    a buffer to store the property bytes into
  Returns:    0 if no error, an error code otherwise
 ***********************************************************************************************************************/
unsigned char Si2168B_L1_GetProperty         (L1_Si2168B_Context *api, unsigned int prop_code, int *data) {
    unsigned char  reserved          = 0;
    unsigned char res;
    res = Si2168B_L1_GET_PROPERTY (api, reserved, prop_code);
    *data = api->rsp->get_property.data;
    return res;
}
/***********************************************************************************************************************
  Si2168B_L1_SetProperty2 function
  Use:        Sets the property given the property code.
  Parameter: *api     the Si2168B context
  Parameter: prop     the property Id

  Returns:    NO_Si2168B_ERROR if successful.
 ***********************************************************************************************************************/
unsigned char Si2168B_L1_SetProperty2        (L1_Si2168B_Context *api, unsigned int prop_code) {
    int data, res;
    char msg[1000];
    res = Si2168B_PackProperty(api->prop, prop_code, &data);
    if (res != NO_Si2168B_ERROR) {
      sprintf(msg, 1000, "\nSi2168B_L1_SetProperty2: %s 0x%04x!\n\n", Si2168B_L1_API_ERROR_TEXT(res), prop_code);
      SiTRACE(msg);
      SiERROR(msg);
      return res;
    }
    return Si2168B_L1_SetProperty (api, prop_code & 0xffff, data);
  }
/***********************************************************************************************************************
  Si2168B_L1_GetProperty2 function
  Use:        property get function
              Used to call L1_GET_PROPERTY with the property Id provided.
  Parameter: *api     the Si2168B context
  Parameter: prop     the property Id

  Returns:    NO_Si2168B_ERROR if successful.
 ***********************************************************************************************************************/
unsigned char Si2168B_L1_GetProperty2        (L1_Si2168B_Context *api, unsigned int prop_code) {
  int data, res;
  res = Si2168B_L1_GetProperty(api, prop_code & 0xffff, &data);
  if (res != NO_Si2168B_ERROR) {
    SiTRACE("Si2168B_L1_GetProperty2: %s 0x%04x\n", Si2168B_L1_API_ERROR_TEXT(res), prop_code);
    SiERROR(Si2168B_L1_API_ERROR_TEXT(res));
    return res;
  }
  return Si2168B_UnpackProperty(api->prop, prop_code, data);
}

/*****************************************************************************************
 NAME: Si2168B_downloadCOMMONProperties
  DESCRIPTION: Setup Si2168B COMMON properties configuration
  This function will download all the COMMON configuration properties.
  The function Si2168B_storeUserProperties should be called before the first call to this function.
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
  Programming Guide Reference:    COMMON setup flowchart
******************************************************************************************/
int  Si2168B_downloadCOMMONProperties        (L1_Si2168B_Context *api) {
  SiTRACE("Si2168B_downloadCOMMONProperties\n");
#ifdef    Si2168B_MASTER_IEN_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_MASTER_IEN_PROP_CODE                     ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_MASTER_IEN_PROP */
return NO_Si2168B_ERROR;
}
/*****************************************************************************************
 NAME: Si2168B_downloadDDProperties
  DESCRIPTION: Setup Si2168B DD properties configuration
  This function will download all the DD configuration properties.
  The function Si2168B_storeUserProperties should be called before the first call to this function.
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
  Programming Guide Reference:    DD setup flowchart
******************************************************************************************/
int  Si2168B_downloadDDProperties            (L1_Si2168B_Context *api) {
  SiTRACE("Si2168B_downloadDDProperties\n");
#ifdef    Si2168B_DD_BER_RESOL_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_BER_RESOL_PROP_CODE                   ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_BER_RESOL_PROP */
#ifdef    Si2168B_DD_CBER_RESOL_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_CBER_RESOL_PROP_CODE                  ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_CBER_RESOL_PROP */

#ifdef    Si2168B_DD_FER_RESOL_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_FER_RESOL_PROP_CODE                   ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_FER_RESOL_PROP */
#ifdef    Si2168B_DD_IEN_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_IEN_PROP_CODE                         ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_IEN_PROP */
#ifdef    Si2168B_DD_IF_INPUT_FREQ_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_IF_INPUT_FREQ_PROP_CODE               ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_IF_INPUT_FREQ_PROP */
#ifdef    Si2168B_DD_INT_SENSE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_INT_SENSE_PROP_CODE                   ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_INT_SENSE_PROP */
#ifdef    Si2168B_DD_MODE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_MODE_PROP_CODE                        ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_MODE_PROP */
#ifdef    Si2168B_DD_PER_RESOL_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_PER_RESOL_PROP_CODE                   ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_PER_RESOL_PROP */
#ifdef    Si2168B_DD_RSQ_BER_THRESHOLD_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_RSQ_BER_THRESHOLD_PROP_CODE           ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_RSQ_BER_THRESHOLD_PROP */
#ifdef    Si2168B_DD_SSI_SQI_PARAM_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_SSI_SQI_PARAM_PROP_CODE               ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_SSI_SQI_PARAM_PROP */
#ifdef    Si2168B_DD_TS_FREQ_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_TS_FREQ_PROP_CODE                     ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_TS_FREQ_PROP */
#ifdef    Si2168B_DD_TS_MODE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_TS_MODE_PROP_CODE                     ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_TS_MODE_PROP */
#ifdef    Si2168B_DD_TS_SERIAL_DIFF_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_TS_SERIAL_DIFF_PROP_CODE              ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_TS_SERIAL_DIFF_PROP */
#ifdef    Si2168B_DD_TS_SETUP_PAR_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_TS_SETUP_PAR_PROP_CODE                ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_TS_SETUP_PAR_PROP */
#ifdef    Si2168B_DD_TS_SETUP_SER_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DD_TS_SETUP_SER_PROP_CODE                ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DD_TS_SETUP_SER_PROP */
return NO_Si2168B_ERROR;
}
/*****************************************************************************************
 NAME: Si2168B_downloadDVBCProperties
  DESCRIPTION: Setup Si2168B DVBC properties configuration
  This function will download all the DVBC configuration properties.
  The function Si2168B_storeUserProperties should be called before the first call to this function.
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
  Programming Guide Reference:    DVBC setup flowchart
******************************************************************************************/
int  Si2168B_downloadDVBCProperties          (L1_Si2168B_Context *api) {
  SiTRACE("Si2168B_downloadDVBCProperties\n");
#ifdef    Si2168B_DVBC_ADC_CREST_FACTOR_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBC_ADC_CREST_FACTOR_PROP_CODE          ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBC_ADC_CREST_FACTOR_PROP */
#ifdef    Si2168B_DVBC_AFC_RANGE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBC_AFC_RANGE_PROP_CODE                 ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBC_AFC_RANGE_PROP */
#ifdef    Si2168B_DVBC_CONSTELLATION_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBC_CONSTELLATION_PROP_CODE             ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBC_CONSTELLATION_PROP */
#ifdef    Si2168B_DVBC_SYMBOL_RATE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBC_SYMBOL_RATE_PROP_CODE               ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBC_SYMBOL_RATE_PROP */
return NO_Si2168B_ERROR;
}



/*****************************************************************************************
 NAME: Si2168B_downloadDVBTProperties
  DESCRIPTION: Setup Si2168B DVBT properties configuration
  This function will download all the DVBT configuration properties.
  The function Si2168B_storeUserProperties should be called before the first call to this function.
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
  Programming Guide Reference:    DVBT setup flowchart
******************************************************************************************/
int  Si2168B_downloadDVBTProperties          (L1_Si2168B_Context *api) {
  SiTRACE("Si2168B_downloadDVBTProperties\n");
#ifdef    Si2168B_DVBT_ADC_CREST_FACTOR_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBT_ADC_CREST_FACTOR_PROP_CODE          ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBT_ADC_CREST_FACTOR_PROP */
#ifdef    Si2168B_DVBT_AFC_RANGE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBT_AFC_RANGE_PROP_CODE                 ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBT_AFC_RANGE_PROP */
#ifdef    Si2168B_DVBT_HIERARCHY_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBT_HIERARCHY_PROP_CODE                 ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBT_HIERARCHY_PROP */
return NO_Si2168B_ERROR;
}

/*****************************************************************************************
 NAME: Si2168B_downloadDVBT2Properties
  DESCRIPTION: Setup Si2168B DVBT2 properties configuration
  This function will download all the DVBT2 configuration properties.
  The function Si2168B_storeUserProperties should be called before the first call to this function.
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
  Programming Guide Reference:    DVBT2 setup flowchart
******************************************************************************************/
int  Si2168B_downloadDVBT2Properties         (L1_Si2168B_Context *api) {
  SiTRACE("Si2168B_downloadDVBT2Properties\n");
#ifdef    Si2168B_DVBT2_ADC_CREST_FACTOR_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBT2_ADC_CREST_FACTOR_PROP_CODE         ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBT2_ADC_CREST_FACTOR_PROP */
#ifdef    Si2168B_DVBT2_AFC_RANGE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBT2_AFC_RANGE_PROP_CODE                ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBT2_AFC_RANGE_PROP */
#ifdef    Si2168B_DVBT2_FEF_TUNER_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBT2_FEF_TUNER_PROP_CODE                ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBT2_FEF_TUNER_PROP */
#ifdef    Si2168B_DVBT2_MODE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_DVBT2_MODE_PROP_CODE                     ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_DVBT2_MODE_PROP */
return NO_Si2168B_ERROR;
}

/*****************************************************************************************
 NAME: Si2168B_downloadMCNSProperties
  DESCRIPTION: Setup Si2168B MCNS properties configuration
  This function will download all the MCNS configuration properties.
  The function Si2168B_storeUserProperties should be called before the first call to this function.
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
  Programming Guide Reference:    MCNS setup flowchart
******************************************************************************************/
int  Si2168B_downloadMCNSProperties          (L1_Si2168B_Context *api) {
  SiTRACE("Si2168B_downloadMCNSProperties\n");
#ifdef    Si2168B_MCNS_ADC_CREST_FACTOR_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_MCNS_ADC_CREST_FACTOR_PROP_CODE          ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_MCNS_ADC_CREST_FACTOR_PROP */
#ifdef    Si2168B_MCNS_AFC_RANGE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_MCNS_AFC_RANGE_PROP_CODE                 ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_MCNS_AFC_RANGE_PROP */
#ifdef    Si2168B_MCNS_CONSTELLATION_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_MCNS_CONSTELLATION_PROP_CODE             ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_MCNS_CONSTELLATION_PROP */
#ifdef    Si2168B_MCNS_SYMBOL_RATE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_MCNS_SYMBOL_RATE_PROP_CODE               ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_MCNS_SYMBOL_RATE_PROP */
return NO_Si2168B_ERROR;
}

/*****************************************************************************************
 NAME: Si2168B_downloadSCANProperties
  DESCRIPTION: Setup Si2168B SCAN properties configuration
  This function will download all the SCAN configuration properties.
  The function Si2168B_storeUserProperties should be called before the first call to this function.
  Parameter:  Pointer to Si2168B Context
  Returns:    I2C transaction error code, NO_Si2168B_ERROR if successful
  Programming Guide Reference:    SCAN setup flowchart
******************************************************************************************/
int  Si2168B_downloadSCANProperties          (L1_Si2168B_Context *api) {
  SiTRACE("Si2168B_downloadSCANProperties\n");
#ifdef    Si2168B_SCAN_FMAX_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_SCAN_FMAX_PROP_CODE                      ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_SCAN_FMAX_PROP */
#ifdef    Si2168B_SCAN_FMIN_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_SCAN_FMIN_PROP_CODE                      ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_SCAN_FMIN_PROP */
#ifdef    Si2168B_SCAN_IEN_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_SCAN_IEN_PROP_CODE                       ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_SCAN_IEN_PROP */
#ifdef    Si2168B_SCAN_INT_SENSE_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_SCAN_INT_SENSE_PROP_CODE                 ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_SCAN_INT_SENSE_PROP */

#ifdef    Si2168B_SCAN_SYMB_RATE_MAX_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_SCAN_SYMB_RATE_MAX_PROP_CODE             ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_SCAN_SYMB_RATE_MAX_PROP */
#ifdef    Si2168B_SCAN_SYMB_RATE_MIN_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_SCAN_SYMB_RATE_MIN_PROP_CODE             ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_SCAN_SYMB_RATE_MIN_PROP */
#ifdef    Si2168B_SCAN_TER_CONFIG_PROP
  if (Si2168B_L1_SetProperty2(api, Si2168B_SCAN_TER_CONFIG_PROP_CODE                ) != NO_Si2168B_ERROR) {return ERROR_Si2168B_SENDING_COMMAND;}
#endif /* Si2168B_SCAN_TER_CONFIG_PROP */
return NO_Si2168B_ERROR;
}

int  Si2168B_downloadAllProperties           (L1_Si2168B_Context *api) {
  Si2168B_downloadCOMMONProperties      (api);
  Si2168B_downloadDDProperties          (api);
  Si2168B_downloadDVBCProperties        (api);



  Si2168B_downloadDVBTProperties        (api);

  Si2168B_downloadDVBT2Properties       (api);

  Si2168B_downloadMCNSProperties        (api);

  Si2168B_downloadSCANProperties        (api);
  return 0;
}

/***********************************************************************************************************************
  Si2168B_PackProperty function
  Use:        This function will pack all the members of a property into an integer for the SetProperty function.

  Parameter: *prop          the Si2168B property context
  Parameter:  prop_code     the property Id
  Parameter:  *data         an int to store the property data

  Returns:    NO_Si2168B_ERROR if the property exists.
 ***********************************************************************************************************************/
unsigned char Si2168B_PackProperty           (Si2168B_PropObj   *prop, unsigned int prop_code, int *data) {
    switch (prop_code) {
    #ifdef        Si2168B_DD_BER_RESOL_PROP
     case         Si2168B_DD_BER_RESOL_PROP_CODE:
      *data = (prop->dd_ber_resol.exp  & Si2168B_DD_BER_RESOL_PROP_EXP_MASK ) << Si2168B_DD_BER_RESOL_PROP_EXP_LSB  |
              (prop->dd_ber_resol.mant & Si2168B_DD_BER_RESOL_PROP_MANT_MASK) << Si2168B_DD_BER_RESOL_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168B_DD_BER_RESOL_PROP */
    #ifdef        Si2168B_DD_CBER_RESOL_PROP
     case         Si2168B_DD_CBER_RESOL_PROP_CODE:
      *data = (prop->dd_cber_resol.exp  & Si2168B_DD_CBER_RESOL_PROP_EXP_MASK ) << Si2168B_DD_CBER_RESOL_PROP_EXP_LSB  |
              (prop->dd_cber_resol.mant & Si2168B_DD_CBER_RESOL_PROP_MANT_MASK) << Si2168B_DD_CBER_RESOL_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168B_DD_CBER_RESOL_PROP */

    #ifdef        Si2168B_DD_FER_RESOL_PROP
     case         Si2168B_DD_FER_RESOL_PROP_CODE:
      *data = (prop->dd_fer_resol.exp  & Si2168B_DD_FER_RESOL_PROP_EXP_MASK ) << Si2168B_DD_FER_RESOL_PROP_EXP_LSB  |
              (prop->dd_fer_resol.mant & Si2168B_DD_FER_RESOL_PROP_MANT_MASK) << Si2168B_DD_FER_RESOL_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168B_DD_FER_RESOL_PROP */
    #ifdef        Si2168B_DD_IEN_PROP
     case         Si2168B_DD_IEN_PROP_CODE:
      *data = (prop->dd_ien.ien_bit0 & Si2168B_DD_IEN_PROP_IEN_BIT0_MASK) << Si2168B_DD_IEN_PROP_IEN_BIT0_LSB  |
              (prop->dd_ien.ien_bit1 & Si2168B_DD_IEN_PROP_IEN_BIT1_MASK) << Si2168B_DD_IEN_PROP_IEN_BIT1_LSB  |
              (prop->dd_ien.ien_bit2 & Si2168B_DD_IEN_PROP_IEN_BIT2_MASK) << Si2168B_DD_IEN_PROP_IEN_BIT2_LSB  |
              (prop->dd_ien.ien_bit3 & Si2168B_DD_IEN_PROP_IEN_BIT3_MASK) << Si2168B_DD_IEN_PROP_IEN_BIT3_LSB  |
              (prop->dd_ien.ien_bit4 & Si2168B_DD_IEN_PROP_IEN_BIT4_MASK) << Si2168B_DD_IEN_PROP_IEN_BIT4_LSB  |
              (prop->dd_ien.ien_bit5 & Si2168B_DD_IEN_PROP_IEN_BIT5_MASK) << Si2168B_DD_IEN_PROP_IEN_BIT5_LSB  |
              (prop->dd_ien.ien_bit6 & Si2168B_DD_IEN_PROP_IEN_BIT6_MASK) << Si2168B_DD_IEN_PROP_IEN_BIT6_LSB  |
              (prop->dd_ien.ien_bit7 & Si2168B_DD_IEN_PROP_IEN_BIT7_MASK) << Si2168B_DD_IEN_PROP_IEN_BIT7_LSB ;
     break;
    #endif /*     Si2168B_DD_IEN_PROP */
    #ifdef        Si2168B_DD_IF_INPUT_FREQ_PROP
     case         Si2168B_DD_IF_INPUT_FREQ_PROP_CODE:
      *data = (prop->dd_if_input_freq.offset & Si2168B_DD_IF_INPUT_FREQ_PROP_OFFSET_MASK) << Si2168B_DD_IF_INPUT_FREQ_PROP_OFFSET_LSB ;
     break;
    #endif /*     Si2168B_DD_IF_INPUT_FREQ_PROP */
    #ifdef        Si2168B_DD_INT_SENSE_PROP
     case         Si2168B_DD_INT_SENSE_PROP_CODE:
      *data = (prop->dd_int_sense.neg_bit0 & Si2168B_DD_INT_SENSE_PROP_NEG_BIT0_MASK) << Si2168B_DD_INT_SENSE_PROP_NEG_BIT0_LSB  |
              (prop->dd_int_sense.neg_bit1 & Si2168B_DD_INT_SENSE_PROP_NEG_BIT1_MASK) << Si2168B_DD_INT_SENSE_PROP_NEG_BIT1_LSB  |
              (prop->dd_int_sense.neg_bit2 & Si2168B_DD_INT_SENSE_PROP_NEG_BIT2_MASK) << Si2168B_DD_INT_SENSE_PROP_NEG_BIT2_LSB  |
              (prop->dd_int_sense.neg_bit3 & Si2168B_DD_INT_SENSE_PROP_NEG_BIT3_MASK) << Si2168B_DD_INT_SENSE_PROP_NEG_BIT3_LSB  |
              (prop->dd_int_sense.neg_bit4 & Si2168B_DD_INT_SENSE_PROP_NEG_BIT4_MASK) << Si2168B_DD_INT_SENSE_PROP_NEG_BIT4_LSB  |
              (prop->dd_int_sense.neg_bit5 & Si2168B_DD_INT_SENSE_PROP_NEG_BIT5_MASK) << Si2168B_DD_INT_SENSE_PROP_NEG_BIT5_LSB  |
              (prop->dd_int_sense.neg_bit6 & Si2168B_DD_INT_SENSE_PROP_NEG_BIT6_MASK) << Si2168B_DD_INT_SENSE_PROP_NEG_BIT6_LSB  |
              (prop->dd_int_sense.neg_bit7 & Si2168B_DD_INT_SENSE_PROP_NEG_BIT7_MASK) << Si2168B_DD_INT_SENSE_PROP_NEG_BIT7_LSB  |
              (prop->dd_int_sense.pos_bit0 & Si2168B_DD_INT_SENSE_PROP_POS_BIT0_MASK) << Si2168B_DD_INT_SENSE_PROP_POS_BIT0_LSB  |
              (prop->dd_int_sense.pos_bit1 & Si2168B_DD_INT_SENSE_PROP_POS_BIT1_MASK) << Si2168B_DD_INT_SENSE_PROP_POS_BIT1_LSB  |
              (prop->dd_int_sense.pos_bit2 & Si2168B_DD_INT_SENSE_PROP_POS_BIT2_MASK) << Si2168B_DD_INT_SENSE_PROP_POS_BIT2_LSB  |
              (prop->dd_int_sense.pos_bit3 & Si2168B_DD_INT_SENSE_PROP_POS_BIT3_MASK) << Si2168B_DD_INT_SENSE_PROP_POS_BIT3_LSB  |
              (prop->dd_int_sense.pos_bit4 & Si2168B_DD_INT_SENSE_PROP_POS_BIT4_MASK) << Si2168B_DD_INT_SENSE_PROP_POS_BIT4_LSB  |
              (prop->dd_int_sense.pos_bit5 & Si2168B_DD_INT_SENSE_PROP_POS_BIT5_MASK) << Si2168B_DD_INT_SENSE_PROP_POS_BIT5_LSB  |
              (prop->dd_int_sense.pos_bit6 & Si2168B_DD_INT_SENSE_PROP_POS_BIT6_MASK) << Si2168B_DD_INT_SENSE_PROP_POS_BIT6_LSB  |
              (prop->dd_int_sense.pos_bit7 & Si2168B_DD_INT_SENSE_PROP_POS_BIT7_MASK) << Si2168B_DD_INT_SENSE_PROP_POS_BIT7_LSB ;
     break;
    #endif /*     Si2168B_DD_INT_SENSE_PROP */
    #ifdef        Si2168B_DD_MODE_PROP
     case         Si2168B_DD_MODE_PROP_CODE:
      *data = (prop->dd_mode.bw              & Si2168B_DD_MODE_PROP_BW_MASK             ) << Si2168B_DD_MODE_PROP_BW_LSB  |
              (prop->dd_mode.modulation      & Si2168B_DD_MODE_PROP_MODULATION_MASK     ) << Si2168B_DD_MODE_PROP_MODULATION_LSB  |
              (prop->dd_mode.invert_spectrum & Si2168B_DD_MODE_PROP_INVERT_SPECTRUM_MASK) << Si2168B_DD_MODE_PROP_INVERT_SPECTRUM_LSB  |
              (prop->dd_mode.auto_detect     & Si2168B_DD_MODE_PROP_AUTO_DETECT_MASK    ) << Si2168B_DD_MODE_PROP_AUTO_DETECT_LSB ;
     break;
    #endif /*     Si2168B_DD_MODE_PROP */
    #ifdef        Si2168B_DD_PER_RESOL_PROP
     case         Si2168B_DD_PER_RESOL_PROP_CODE:
      *data = (prop->dd_per_resol.exp  & Si2168B_DD_PER_RESOL_PROP_EXP_MASK ) << Si2168B_DD_PER_RESOL_PROP_EXP_LSB  |
              (prop->dd_per_resol.mant & Si2168B_DD_PER_RESOL_PROP_MANT_MASK) << Si2168B_DD_PER_RESOL_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168B_DD_PER_RESOL_PROP */
    #ifdef        Si2168B_DD_RSQ_BER_THRESHOLD_PROP
     case         Si2168B_DD_RSQ_BER_THRESHOLD_PROP_CODE:
      *data = (prop->dd_rsq_ber_threshold.exp  & Si2168B_DD_RSQ_BER_THRESHOLD_PROP_EXP_MASK ) << Si2168B_DD_RSQ_BER_THRESHOLD_PROP_EXP_LSB  |
              (prop->dd_rsq_ber_threshold.mant & Si2168B_DD_RSQ_BER_THRESHOLD_PROP_MANT_MASK) << Si2168B_DD_RSQ_BER_THRESHOLD_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168B_DD_RSQ_BER_THRESHOLD_PROP */
    #ifdef        Si2168B_DD_SSI_SQI_PARAM_PROP
     case         Si2168B_DD_SSI_SQI_PARAM_PROP_CODE:
      *data = (prop->dd_ssi_sqi_param.sqi_average & Si2168B_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE_MASK) << Si2168B_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE_LSB ;
     break;
    #endif /*     Si2168B_DD_SSI_SQI_PARAM_PROP */
    #ifdef        Si2168B_DD_TS_FREQ_PROP
     case         Si2168B_DD_TS_FREQ_PROP_CODE:
      *data = (prop->dd_ts_freq.req_freq_10khz & Si2168B_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ_MASK) << Si2168B_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ_LSB ;
     break;
    #endif /*     Si2168B_DD_TS_FREQ_PROP */
    #ifdef        Si2168B_DD_TS_MODE_PROP
     case         Si2168B_DD_TS_MODE_PROP_CODE:
      *data = (prop->dd_ts_mode.mode            & Si2168B_DD_TS_MODE_PROP_MODE_MASK           ) << Si2168B_DD_TS_MODE_PROP_MODE_LSB  |
              (prop->dd_ts_mode.clock           & Si2168B_DD_TS_MODE_PROP_CLOCK_MASK          ) << Si2168B_DD_TS_MODE_PROP_CLOCK_LSB  |
              (prop->dd_ts_mode.clk_gapped_en   & Si2168B_DD_TS_MODE_PROP_CLK_GAPPED_EN_MASK  ) << Si2168B_DD_TS_MODE_PROP_CLK_GAPPED_EN_LSB  |
              (prop->dd_ts_mode.ts_err_polarity & Si2168B_DD_TS_MODE_PROP_TS_ERR_POLARITY_MASK) << Si2168B_DD_TS_MODE_PROP_TS_ERR_POLARITY_LSB  |
              (prop->dd_ts_mode.special            & Si2168B_DD_TS_MODE_PROP_SPECIAL_MASK           ) << Si2168B_DD_TS_MODE_PROP_SPECIAL_LSB  |
              (prop->dd_ts_mode.ts_freq_resolution & Si2168B_DD_TS_MODE_PROP_TS_FREQ_RESOLUTION_MASK) << Si2168B_DD_TS_MODE_PROP_TS_FREQ_RESOLUTION_LSB ;
     break;
    #endif /*     Si2168B_DD_TS_MODE_PROP */
    #ifdef        Si2168B_DD_TS_SERIAL_DIFF_PROP
     case         Si2168B_DD_TS_SERIAL_DIFF_PROP_CODE:
      *data = (prop->dd_ts_serial_diff.ts_data1_strength  & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA1_STRENGTH_MASK ) << Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA1_STRENGTH_LSB  |
              (prop->dd_ts_serial_diff.ts_data1_shape     & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA1_SHAPE_MASK    ) << Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA1_SHAPE_LSB  |
              (prop->dd_ts_serial_diff.ts_data2_strength  & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA2_STRENGTH_MASK ) << Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA2_STRENGTH_LSB  |
              (prop->dd_ts_serial_diff.ts_data2_shape     & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA2_SHAPE_MASK    ) << Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA2_SHAPE_LSB  |
              (prop->dd_ts_serial_diff.ts_clkb_on_data1   & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_CLKB_ON_DATA1_MASK  ) << Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_CLKB_ON_DATA1_LSB  |
              (prop->dd_ts_serial_diff.ts_data0b_on_data2 & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA0B_ON_DATA2_MASK) << Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA0B_ON_DATA2_LSB ;
     break;
    #endif /*     Si2168B_DD_TS_SERIAL_DIFF_PROP */
    #ifdef        Si2168B_DD_TS_SETUP_PAR_PROP
     case         Si2168B_DD_TS_SETUP_PAR_PROP_CODE:
      *data = (prop->dd_ts_setup_par.ts_data_strength & Si2168B_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH_MASK) << Si2168B_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH_LSB  |
              (prop->dd_ts_setup_par.ts_data_shape    & Si2168B_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE_MASK   ) << Si2168B_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE_LSB  |
              (prop->dd_ts_setup_par.ts_clk_strength  & Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH_MASK ) << Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH_LSB  |
              (prop->dd_ts_setup_par.ts_clk_shape     & Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE_MASK    ) << Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE_LSB  |
              (prop->dd_ts_setup_par.ts_clk_invert    & Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_MASK   ) << Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_LSB  |
              (prop->dd_ts_setup_par.ts_clk_shift     & Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT_MASK    ) << Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT_LSB ;
     break;
    #endif /*     Si2168B_DD_TS_SETUP_PAR_PROP */
    #ifdef        Si2168B_DD_TS_SETUP_SER_PROP
     case         Si2168B_DD_TS_SETUP_SER_PROP_CODE:
      *data = (prop->dd_ts_setup_ser.ts_data_strength & Si2168B_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH_MASK) << Si2168B_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH_LSB  |
              (prop->dd_ts_setup_ser.ts_data_shape    & Si2168B_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE_MASK   ) << Si2168B_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE_LSB  |
              (prop->dd_ts_setup_ser.ts_clk_strength  & Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH_MASK ) << Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH_LSB  |
              (prop->dd_ts_setup_ser.ts_clk_shape     & Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE_MASK    ) << Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE_LSB  |
              (prop->dd_ts_setup_ser.ts_clk_invert    & Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_MASK   ) << Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_LSB  |
              (prop->dd_ts_setup_ser.ts_sync_duration & Si2168B_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_MASK) << Si2168B_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_LSB  |
              (prop->dd_ts_setup_ser.ts_byte_order    & Si2168B_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_MASK   ) << Si2168B_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_LSB ;
     break;
    #endif /*     Si2168B_DD_TS_SETUP_SER_PROP */

    #ifdef        Si2168B_DVBC_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBC_ADC_CREST_FACTOR_PROP_CODE:
      *data = (prop->dvbc_adc_crest_factor.crest_factor & Si2168B_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK) << Si2168B_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB ;
     break;
    #endif /*     Si2168B_DVBC_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBC_AFC_RANGE_PROP
     case         Si2168B_DVBC_AFC_RANGE_PROP_CODE:
      *data = (prop->dvbc_afc_range.range_khz & Si2168B_DVBC_AFC_RANGE_PROP_RANGE_KHZ_MASK) << Si2168B_DVBC_AFC_RANGE_PROP_RANGE_KHZ_LSB ;
     break;
    #endif /*     Si2168B_DVBC_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBC_CONSTELLATION_PROP
     case         Si2168B_DVBC_CONSTELLATION_PROP_CODE:
      *data = (prop->dvbc_constellation.constellation & Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_MASK) << Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_LSB ;
     break;
    #endif /*     Si2168B_DVBC_CONSTELLATION_PROP */
    #ifdef        Si2168B_DVBC_SYMBOL_RATE_PROP
     case         Si2168B_DVBC_SYMBOL_RATE_PROP_CODE:
      *data = (prop->dvbc_symbol_rate.rate & Si2168B_DVBC_SYMBOL_RATE_PROP_RATE_MASK) << Si2168B_DVBC_SYMBOL_RATE_PROP_RATE_LSB ;
     break;
    #endif /*     Si2168B_DVBC_SYMBOL_RATE_PROP */


    #ifdef        Si2168B_DVBT2_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBT2_ADC_CREST_FACTOR_PROP_CODE:
      *data = (prop->dvbt2_adc_crest_factor.crest_factor & Si2168B_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK) << Si2168B_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB ;
     break;
    #endif /*     Si2168B_DVBT2_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBT2_AFC_RANGE_PROP
     case         Si2168B_DVBT2_AFC_RANGE_PROP_CODE:
      *data = (prop->dvbt2_afc_range.range_khz & Si2168B_DVBT2_AFC_RANGE_PROP_RANGE_KHZ_MASK) << Si2168B_DVBT2_AFC_RANGE_PROP_RANGE_KHZ_LSB ;
     break;
    #endif /*     Si2168B_DVBT2_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBT2_FEF_TUNER_PROP
     case         Si2168B_DVBT2_FEF_TUNER_PROP_CODE:
      *data = (prop->dvbt2_fef_tuner.tuner_delay         & Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_DELAY_MASK        ) << Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_DELAY_LSB  |
              (prop->dvbt2_fef_tuner.tuner_freeze_time   & Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME_MASK  ) << Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME_LSB  |
              (prop->dvbt2_fef_tuner.tuner_unfreeze_time & Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME_MASK) << Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME_LSB ;
     break;
    #endif /*     Si2168B_DVBT2_FEF_TUNER_PROP */
    #ifdef        Si2168B_DVBT2_MODE_PROP
     case         Si2168B_DVBT2_MODE_PROP_CODE:
      *data = (prop->dvbt2_mode.lock_mode & Si2168B_DVBT2_MODE_PROP_LOCK_MODE_MASK) << Si2168B_DVBT2_MODE_PROP_LOCK_MODE_LSB ;
     break;
    #endif /*     Si2168B_DVBT2_MODE_PROP */

    #ifdef        Si2168B_DVBT_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBT_ADC_CREST_FACTOR_PROP_CODE:
      *data = (prop->dvbt_adc_crest_factor.crest_factor & Si2168B_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK) << Si2168B_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB ;
     break;
    #endif /*     Si2168B_DVBT_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBT_AFC_RANGE_PROP
     case         Si2168B_DVBT_AFC_RANGE_PROP_CODE:
      *data = (prop->dvbt_afc_range.range_khz & Si2168B_DVBT_AFC_RANGE_PROP_RANGE_KHZ_MASK) << Si2168B_DVBT_AFC_RANGE_PROP_RANGE_KHZ_LSB ;
     break;
    #endif /*     Si2168B_DVBT_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBT_HIERARCHY_PROP
     case         Si2168B_DVBT_HIERARCHY_PROP_CODE:
      *data = (prop->dvbt_hierarchy.stream & Si2168B_DVBT_HIERARCHY_PROP_STREAM_MASK) << Si2168B_DVBT_HIERARCHY_PROP_STREAM_LSB ;
     break;
    #endif /*     Si2168B_DVBT_HIERARCHY_PROP */

    #ifdef        Si2168B_MASTER_IEN_PROP
     case         Si2168B_MASTER_IEN_PROP_CODE:
      *data = (prop->master_ien.ddien   & Si2168B_MASTER_IEN_PROP_DDIEN_MASK  ) << Si2168B_MASTER_IEN_PROP_DDIEN_LSB  |
              (prop->master_ien.scanien & Si2168B_MASTER_IEN_PROP_SCANIEN_MASK) << Si2168B_MASTER_IEN_PROP_SCANIEN_LSB  |
              (prop->master_ien.errien  & Si2168B_MASTER_IEN_PROP_ERRIEN_MASK ) << Si2168B_MASTER_IEN_PROP_ERRIEN_LSB  |
              (prop->master_ien.ctsien  & Si2168B_MASTER_IEN_PROP_CTSIEN_MASK ) << Si2168B_MASTER_IEN_PROP_CTSIEN_LSB ;
     break;
    #endif /*     Si2168B_MASTER_IEN_PROP */
    #ifdef        Si2168B_MCNS_ADC_CREST_FACTOR_PROP
     case         Si2168B_MCNS_ADC_CREST_FACTOR_PROP_CODE:
      *data = (prop->mcns_adc_crest_factor.crest_factor & Si2168B_MCNS_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK) << Si2168B_MCNS_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB ;
     break;
    #endif /*     Si2168B_MCNS_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_MCNS_AFC_RANGE_PROP
     case         Si2168B_MCNS_AFC_RANGE_PROP_CODE:
      *data = (prop->mcns_afc_range.range_khz & Si2168B_MCNS_AFC_RANGE_PROP_RANGE_KHZ_MASK) << Si2168B_MCNS_AFC_RANGE_PROP_RANGE_KHZ_LSB ;
     break;
    #endif /*     Si2168B_MCNS_AFC_RANGE_PROP */
    #ifdef        Si2168B_MCNS_CONSTELLATION_PROP
     case         Si2168B_MCNS_CONSTELLATION_PROP_CODE:
      *data = (prop->mcns_constellation.constellation & Si2168B_MCNS_CONSTELLATION_PROP_CONSTELLATION_MASK) << Si2168B_MCNS_CONSTELLATION_PROP_CONSTELLATION_LSB ;
     break;
    #endif /*     Si2168B_MCNS_CONSTELLATION_PROP */
    #ifdef        Si2168B_MCNS_SYMBOL_RATE_PROP
     case         Si2168B_MCNS_SYMBOL_RATE_PROP_CODE:
      *data = (prop->mcns_symbol_rate.rate & Si2168B_MCNS_SYMBOL_RATE_PROP_RATE_MASK) << Si2168B_MCNS_SYMBOL_RATE_PROP_RATE_LSB ;
     break;
    #endif /*     Si2168B_MCNS_SYMBOL_RATE_PROP */

    #ifdef        Si2168B_SCAN_FMAX_PROP
     case         Si2168B_SCAN_FMAX_PROP_CODE:
      *data = (prop->scan_fmax.scan_fmax & Si2168B_SCAN_FMAX_PROP_SCAN_FMAX_MASK) << Si2168B_SCAN_FMAX_PROP_SCAN_FMAX_LSB ;
     break;
    #endif /*     Si2168B_SCAN_FMAX_PROP */
    #ifdef        Si2168B_SCAN_FMIN_PROP
     case         Si2168B_SCAN_FMIN_PROP_CODE:
      *data = (prop->scan_fmin.scan_fmin & Si2168B_SCAN_FMIN_PROP_SCAN_FMIN_MASK) << Si2168B_SCAN_FMIN_PROP_SCAN_FMIN_LSB ;
     break;
    #endif /*     Si2168B_SCAN_FMIN_PROP */
    #ifdef        Si2168B_SCAN_IEN_PROP
     case         Si2168B_SCAN_IEN_PROP_CODE:
      *data = (prop->scan_ien.buzien & Si2168B_SCAN_IEN_PROP_BUZIEN_MASK) << Si2168B_SCAN_IEN_PROP_BUZIEN_LSB  |
              (prop->scan_ien.reqien & Si2168B_SCAN_IEN_PROP_REQIEN_MASK) << Si2168B_SCAN_IEN_PROP_REQIEN_LSB ;
     break;
    #endif /*     Si2168B_SCAN_IEN_PROP */
    #ifdef        Si2168B_SCAN_INT_SENSE_PROP
     case         Si2168B_SCAN_INT_SENSE_PROP_CODE:
      *data = (prop->scan_int_sense.buznegen & Si2168B_SCAN_INT_SENSE_PROP_BUZNEGEN_MASK) << Si2168B_SCAN_INT_SENSE_PROP_BUZNEGEN_LSB  |
              (prop->scan_int_sense.reqnegen & Si2168B_SCAN_INT_SENSE_PROP_REQNEGEN_MASK) << Si2168B_SCAN_INT_SENSE_PROP_REQNEGEN_LSB  |
              (prop->scan_int_sense.buzposen & Si2168B_SCAN_INT_SENSE_PROP_BUZPOSEN_MASK) << Si2168B_SCAN_INT_SENSE_PROP_BUZPOSEN_LSB  |
              (prop->scan_int_sense.reqposen & Si2168B_SCAN_INT_SENSE_PROP_REQPOSEN_MASK) << Si2168B_SCAN_INT_SENSE_PROP_REQPOSEN_LSB ;
     break;
    #endif /*     Si2168B_SCAN_INT_SENSE_PROP */

    #ifdef        Si2168B_SCAN_SYMB_RATE_MAX_PROP
     case         Si2168B_SCAN_SYMB_RATE_MAX_PROP_CODE:
      *data = (prop->scan_symb_rate_max.scan_symb_rate_max & Si2168B_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX_MASK) << Si2168B_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX_LSB ;
     break;
    #endif /*     Si2168B_SCAN_SYMB_RATE_MAX_PROP */
    #ifdef        Si2168B_SCAN_SYMB_RATE_MIN_PROP
     case         Si2168B_SCAN_SYMB_RATE_MIN_PROP_CODE:
      *data = (prop->scan_symb_rate_min.scan_symb_rate_min & Si2168B_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN_MASK) << Si2168B_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN_LSB ;
     break;
    #endif /*     Si2168B_SCAN_SYMB_RATE_MIN_PROP */
    #ifdef        Si2168B_SCAN_TER_CONFIG_PROP
     case         Si2168B_SCAN_TER_CONFIG_PROP_CODE:
      *data = (prop->scan_ter_config.mode          & Si2168B_SCAN_TER_CONFIG_PROP_MODE_MASK         ) << Si2168B_SCAN_TER_CONFIG_PROP_MODE_LSB  |
              (prop->scan_ter_config.analog_bw     & Si2168B_SCAN_TER_CONFIG_PROP_ANALOG_BW_MASK    ) << Si2168B_SCAN_TER_CONFIG_PROP_ANALOG_BW_LSB  |
              (prop->scan_ter_config.search_analog & Si2168B_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_MASK) << Si2168B_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_LSB |
              (prop->scan_ter_config.scan_debug    & Si2168B_SCAN_TER_CONFIG_PROP_SCAN_DEBUG_MASK   ) << Si2168B_SCAN_TER_CONFIG_PROP_SCAN_DEBUG_LSB ;
              ;
     break;
    #endif /*     Si2168B_SCAN_TER_CONFIG_PROP */

     default : return ERROR_Si2168B_UNKNOWN_PROPERTY; break;
    }
    return NO_Si2168B_ERROR;
  }


/***********************************************************************************************************************
  Si2168B_UnpackProperty function
  Use:        This function will unpack all the members of a property from an integer from the GetProperty function.

  Parameter: *prop          the Si2168B property context
  Parameter:  prop_code     the property Id
  Parameter:  data          the property data

  Returns:    NO_Si2168B_ERROR if the property exists.
 ***********************************************************************************************************************/
unsigned char Si2168B_UnpackProperty         (Si2168B_PropObj   *prop, unsigned int prop_code, int  data) {
    switch (prop_code) {
    #ifdef        Si2168B_DD_BER_RESOL_PROP
     case         Si2168B_DD_BER_RESOL_PROP_CODE:
               prop->dd_ber_resol.exp  = (data >> Si2168B_DD_BER_RESOL_PROP_EXP_LSB ) & Si2168B_DD_BER_RESOL_PROP_EXP_MASK;
               prop->dd_ber_resol.mant = (data >> Si2168B_DD_BER_RESOL_PROP_MANT_LSB) & Si2168B_DD_BER_RESOL_PROP_MANT_MASK;
     break;
    #endif /*     Si2168B_DD_BER_RESOL_PROP */
    #ifdef        Si2168B_DD_CBER_RESOL_PROP
     case         Si2168B_DD_CBER_RESOL_PROP_CODE:
               prop->dd_cber_resol.exp  = (data >> Si2168B_DD_CBER_RESOL_PROP_EXP_LSB ) & Si2168B_DD_CBER_RESOL_PROP_EXP_MASK;
               prop->dd_cber_resol.mant = (data >> Si2168B_DD_CBER_RESOL_PROP_MANT_LSB) & Si2168B_DD_CBER_RESOL_PROP_MANT_MASK;
     break;
    #endif /*     Si2168B_DD_CBER_RESOL_PROP */

    #ifdef        Si2168B_DD_FER_RESOL_PROP
     case         Si2168B_DD_FER_RESOL_PROP_CODE:
               prop->dd_fer_resol.exp  = (data >> Si2168B_DD_FER_RESOL_PROP_EXP_LSB ) & Si2168B_DD_FER_RESOL_PROP_EXP_MASK;
               prop->dd_fer_resol.mant = (data >> Si2168B_DD_FER_RESOL_PROP_MANT_LSB) & Si2168B_DD_FER_RESOL_PROP_MANT_MASK;
     break;
    #endif /*     Si2168B_DD_FER_RESOL_PROP */
    #ifdef        Si2168B_DD_IEN_PROP
     case         Si2168B_DD_IEN_PROP_CODE:
               prop->dd_ien.ien_bit0 = (data >> Si2168B_DD_IEN_PROP_IEN_BIT0_LSB) & Si2168B_DD_IEN_PROP_IEN_BIT0_MASK;
               prop->dd_ien.ien_bit1 = (data >> Si2168B_DD_IEN_PROP_IEN_BIT1_LSB) & Si2168B_DD_IEN_PROP_IEN_BIT1_MASK;
               prop->dd_ien.ien_bit2 = (data >> Si2168B_DD_IEN_PROP_IEN_BIT2_LSB) & Si2168B_DD_IEN_PROP_IEN_BIT2_MASK;
               prop->dd_ien.ien_bit3 = (data >> Si2168B_DD_IEN_PROP_IEN_BIT3_LSB) & Si2168B_DD_IEN_PROP_IEN_BIT3_MASK;
               prop->dd_ien.ien_bit4 = (data >> Si2168B_DD_IEN_PROP_IEN_BIT4_LSB) & Si2168B_DD_IEN_PROP_IEN_BIT4_MASK;
               prop->dd_ien.ien_bit5 = (data >> Si2168B_DD_IEN_PROP_IEN_BIT5_LSB) & Si2168B_DD_IEN_PROP_IEN_BIT5_MASK;
               prop->dd_ien.ien_bit6 = (data >> Si2168B_DD_IEN_PROP_IEN_BIT6_LSB) & Si2168B_DD_IEN_PROP_IEN_BIT6_MASK;
               prop->dd_ien.ien_bit7 = (data >> Si2168B_DD_IEN_PROP_IEN_BIT7_LSB) & Si2168B_DD_IEN_PROP_IEN_BIT7_MASK;
     break;
    #endif /*     Si2168B_DD_IEN_PROP */
    #ifdef        Si2168B_DD_IF_INPUT_FREQ_PROP
     case         Si2168B_DD_IF_INPUT_FREQ_PROP_CODE:
               prop->dd_if_input_freq.offset = (data >> Si2168B_DD_IF_INPUT_FREQ_PROP_OFFSET_LSB) & Si2168B_DD_IF_INPUT_FREQ_PROP_OFFSET_MASK;
     break;
    #endif /*     Si2168B_DD_IF_INPUT_FREQ_PROP */
    #ifdef        Si2168B_DD_INT_SENSE_PROP
     case         Si2168B_DD_INT_SENSE_PROP_CODE:
               prop->dd_int_sense.neg_bit0 = (data >> Si2168B_DD_INT_SENSE_PROP_NEG_BIT0_LSB) & Si2168B_DD_INT_SENSE_PROP_NEG_BIT0_MASK;
               prop->dd_int_sense.neg_bit1 = (data >> Si2168B_DD_INT_SENSE_PROP_NEG_BIT1_LSB) & Si2168B_DD_INT_SENSE_PROP_NEG_BIT1_MASK;
               prop->dd_int_sense.neg_bit2 = (data >> Si2168B_DD_INT_SENSE_PROP_NEG_BIT2_LSB) & Si2168B_DD_INT_SENSE_PROP_NEG_BIT2_MASK;
               prop->dd_int_sense.neg_bit3 = (data >> Si2168B_DD_INT_SENSE_PROP_NEG_BIT3_LSB) & Si2168B_DD_INT_SENSE_PROP_NEG_BIT3_MASK;
               prop->dd_int_sense.neg_bit4 = (data >> Si2168B_DD_INT_SENSE_PROP_NEG_BIT4_LSB) & Si2168B_DD_INT_SENSE_PROP_NEG_BIT4_MASK;
               prop->dd_int_sense.neg_bit5 = (data >> Si2168B_DD_INT_SENSE_PROP_NEG_BIT5_LSB) & Si2168B_DD_INT_SENSE_PROP_NEG_BIT5_MASK;
               prop->dd_int_sense.neg_bit6 = (data >> Si2168B_DD_INT_SENSE_PROP_NEG_BIT6_LSB) & Si2168B_DD_INT_SENSE_PROP_NEG_BIT6_MASK;
               prop->dd_int_sense.neg_bit7 = (data >> Si2168B_DD_INT_SENSE_PROP_NEG_BIT7_LSB) & Si2168B_DD_INT_SENSE_PROP_NEG_BIT7_MASK;
               prop->dd_int_sense.pos_bit0 = (data >> Si2168B_DD_INT_SENSE_PROP_POS_BIT0_LSB) & Si2168B_DD_INT_SENSE_PROP_POS_BIT0_MASK;
               prop->dd_int_sense.pos_bit1 = (data >> Si2168B_DD_INT_SENSE_PROP_POS_BIT1_LSB) & Si2168B_DD_INT_SENSE_PROP_POS_BIT1_MASK;
               prop->dd_int_sense.pos_bit2 = (data >> Si2168B_DD_INT_SENSE_PROP_POS_BIT2_LSB) & Si2168B_DD_INT_SENSE_PROP_POS_BIT2_MASK;
               prop->dd_int_sense.pos_bit3 = (data >> Si2168B_DD_INT_SENSE_PROP_POS_BIT3_LSB) & Si2168B_DD_INT_SENSE_PROP_POS_BIT3_MASK;
               prop->dd_int_sense.pos_bit4 = (data >> Si2168B_DD_INT_SENSE_PROP_POS_BIT4_LSB) & Si2168B_DD_INT_SENSE_PROP_POS_BIT4_MASK;
               prop->dd_int_sense.pos_bit5 = (data >> Si2168B_DD_INT_SENSE_PROP_POS_BIT5_LSB) & Si2168B_DD_INT_SENSE_PROP_POS_BIT5_MASK;
               prop->dd_int_sense.pos_bit6 = (data >> Si2168B_DD_INT_SENSE_PROP_POS_BIT6_LSB) & Si2168B_DD_INT_SENSE_PROP_POS_BIT6_MASK;
               prop->dd_int_sense.pos_bit7 = (data >> Si2168B_DD_INT_SENSE_PROP_POS_BIT7_LSB) & Si2168B_DD_INT_SENSE_PROP_POS_BIT7_MASK;
     break;
    #endif /*     Si2168B_DD_INT_SENSE_PROP */
    #ifdef        Si2168B_DD_MODE_PROP
     case         Si2168B_DD_MODE_PROP_CODE:
               prop->dd_mode.bw              = (data >> Si2168B_DD_MODE_PROP_BW_LSB             ) & Si2168B_DD_MODE_PROP_BW_MASK;
               prop->dd_mode.modulation      = (data >> Si2168B_DD_MODE_PROP_MODULATION_LSB     ) & Si2168B_DD_MODE_PROP_MODULATION_MASK;
               prop->dd_mode.invert_spectrum = (data >> Si2168B_DD_MODE_PROP_INVERT_SPECTRUM_LSB) & Si2168B_DD_MODE_PROP_INVERT_SPECTRUM_MASK;
               prop->dd_mode.auto_detect     = (data >> Si2168B_DD_MODE_PROP_AUTO_DETECT_LSB    ) & Si2168B_DD_MODE_PROP_AUTO_DETECT_MASK;
     break;
    #endif /*     Si2168B_DD_MODE_PROP */
    #ifdef        Si2168B_DD_PER_RESOL_PROP
     case         Si2168B_DD_PER_RESOL_PROP_CODE:
               prop->dd_per_resol.exp  = (data >> Si2168B_DD_PER_RESOL_PROP_EXP_LSB ) & Si2168B_DD_PER_RESOL_PROP_EXP_MASK;
               prop->dd_per_resol.mant = (data >> Si2168B_DD_PER_RESOL_PROP_MANT_LSB) & Si2168B_DD_PER_RESOL_PROP_MANT_MASK;
     break;
    #endif /*     Si2168B_DD_PER_RESOL_PROP */
    #ifdef        Si2168B_DD_RSQ_BER_THRESHOLD_PROP
     case         Si2168B_DD_RSQ_BER_THRESHOLD_PROP_CODE:
               prop->dd_rsq_ber_threshold.exp  = (data >> Si2168B_DD_RSQ_BER_THRESHOLD_PROP_EXP_LSB ) & Si2168B_DD_RSQ_BER_THRESHOLD_PROP_EXP_MASK;
               prop->dd_rsq_ber_threshold.mant = (data >> Si2168B_DD_RSQ_BER_THRESHOLD_PROP_MANT_LSB) & Si2168B_DD_RSQ_BER_THRESHOLD_PROP_MANT_MASK;
     break;
    #endif /*     Si2168B_DD_RSQ_BER_THRESHOLD_PROP */
    #ifdef        Si2168B_DD_SSI_SQI_PARAM_PROP
     case         Si2168B_DD_SSI_SQI_PARAM_PROP_CODE:
               prop->dd_ssi_sqi_param.sqi_average = (data >> Si2168B_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE_LSB) & Si2168B_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE_MASK;
     break;
    #endif /*     Si2168B_DD_SSI_SQI_PARAM_PROP */
    #ifdef        Si2168B_DD_TS_FREQ_PROP
     case         Si2168B_DD_TS_FREQ_PROP_CODE:
               prop->dd_ts_freq.req_freq_10khz = (data >> Si2168B_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ_LSB) & Si2168B_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ_MASK;
     break;
    #endif /*     Si2168B_DD_TS_FREQ_PROP */
    #ifdef        Si2168B_DD_TS_MODE_PROP
     case         Si2168B_DD_TS_MODE_PROP_CODE:
               prop->dd_ts_mode.mode            = (data >> Si2168B_DD_TS_MODE_PROP_MODE_LSB           ) & Si2168B_DD_TS_MODE_PROP_MODE_MASK;
               prop->dd_ts_mode.clock           = (data >> Si2168B_DD_TS_MODE_PROP_CLOCK_LSB          ) & Si2168B_DD_TS_MODE_PROP_CLOCK_MASK;
               prop->dd_ts_mode.clk_gapped_en   = (data >> Si2168B_DD_TS_MODE_PROP_CLK_GAPPED_EN_LSB  ) & Si2168B_DD_TS_MODE_PROP_CLK_GAPPED_EN_MASK;
               prop->dd_ts_mode.ts_err_polarity = (data >> Si2168B_DD_TS_MODE_PROP_TS_ERR_POLARITY_LSB) & Si2168B_DD_TS_MODE_PROP_TS_ERR_POLARITY_MASK;
               prop->dd_ts_mode.special         = (data >> Si2168B_DD_TS_MODE_PROP_SPECIAL_LSB        ) & Si2168B_DD_TS_MODE_PROP_SPECIAL_MASK;
               prop->dd_ts_mode.ts_freq_resolution = (data >> Si2168B_DD_TS_MODE_PROP_TS_FREQ_RESOLUTION_LSB) & Si2168B_DD_TS_MODE_PROP_TS_FREQ_RESOLUTION_MASK;
     break;
    #endif /*     Si2168B_DD_TS_MODE_PROP */
    #ifdef        Si2168B_DD_TS_SERIAL_DIFF_PROP
     case         Si2168B_DD_TS_SERIAL_DIFF_PROP_CODE:
               prop->dd_ts_serial_diff.ts_data1_strength  = (data >> Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA1_STRENGTH_LSB ) & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA1_STRENGTH_MASK;
               prop->dd_ts_serial_diff.ts_data1_shape     = (data >> Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA1_SHAPE_LSB    ) & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA1_SHAPE_MASK;
               prop->dd_ts_serial_diff.ts_data2_strength  = (data >> Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA2_STRENGTH_LSB ) & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA2_STRENGTH_MASK;
               prop->dd_ts_serial_diff.ts_data2_shape     = (data >> Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA2_SHAPE_LSB    ) & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA2_SHAPE_MASK;
               prop->dd_ts_serial_diff.ts_clkb_on_data1   = (data >> Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_CLKB_ON_DATA1_LSB  ) & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_CLKB_ON_DATA1_MASK;
               prop->dd_ts_serial_diff.ts_data0b_on_data2 = (data >> Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA0B_ON_DATA2_LSB) & Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA0B_ON_DATA2_MASK;
     break;
    #endif /*     Si2168B_DD_TS_SERIAL_DIFF_PROP */
    #ifdef        Si2168B_DD_TS_SETUP_PAR_PROP
     case         Si2168B_DD_TS_SETUP_PAR_PROP_CODE:
               prop->dd_ts_setup_par.ts_data_strength = (data >> Si2168B_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH_LSB) & Si2168B_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH_MASK;
               prop->dd_ts_setup_par.ts_data_shape    = (data >> Si2168B_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE_LSB   ) & Si2168B_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE_MASK;
               prop->dd_ts_setup_par.ts_clk_strength  = (data >> Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH_LSB ) & Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH_MASK;
               prop->dd_ts_setup_par.ts_clk_shape     = (data >> Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE_LSB    ) & Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE_MASK;
               prop->dd_ts_setup_par.ts_clk_invert    = (data >> Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_LSB   ) & Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_MASK;
               prop->dd_ts_setup_par.ts_clk_shift     = (data >> Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT_LSB    ) & Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT_MASK;
     break;
    #endif /*     Si2168B_DD_TS_SETUP_PAR_PROP */
    #ifdef        Si2168B_DD_TS_SETUP_SER_PROP
     case         Si2168B_DD_TS_SETUP_SER_PROP_CODE:
               prop->dd_ts_setup_ser.ts_data_strength = (data >> Si2168B_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH_LSB) & Si2168B_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH_MASK;
               prop->dd_ts_setup_ser.ts_data_shape    = (data >> Si2168B_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE_LSB   ) & Si2168B_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE_MASK;
               prop->dd_ts_setup_ser.ts_clk_strength  = (data >> Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH_LSB ) & Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH_MASK;
               prop->dd_ts_setup_ser.ts_clk_shape     = (data >> Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE_LSB    ) & Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE_MASK;
               prop->dd_ts_setup_ser.ts_clk_invert    = (data >> Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_LSB   ) & Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_MASK;
               prop->dd_ts_setup_ser.ts_sync_duration = (data >> Si2168B_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_LSB) & Si2168B_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_MASK;
               prop->dd_ts_setup_ser.ts_byte_order    = (data >> Si2168B_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_LSB   ) & Si2168B_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_MASK;
     break;
    #endif /*     Si2168B_DD_TS_SETUP_SER_PROP */

    #ifdef        Si2168B_DVBC_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBC_ADC_CREST_FACTOR_PROP_CODE:
               prop->dvbc_adc_crest_factor.crest_factor = (data >> Si2168B_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB) & Si2168B_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK;
     break;
    #endif /*     Si2168B_DVBC_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBC_AFC_RANGE_PROP
     case         Si2168B_DVBC_AFC_RANGE_PROP_CODE:
               prop->dvbc_afc_range.range_khz = (data >> Si2168B_DVBC_AFC_RANGE_PROP_RANGE_KHZ_LSB) & Si2168B_DVBC_AFC_RANGE_PROP_RANGE_KHZ_MASK;
     break;
    #endif /*     Si2168B_DVBC_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBC_CONSTELLATION_PROP
     case         Si2168B_DVBC_CONSTELLATION_PROP_CODE:
               prop->dvbc_constellation.constellation = (data >> Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_LSB) & Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_MASK;
     break;
    #endif /*     Si2168B_DVBC_CONSTELLATION_PROP */
    #ifdef        Si2168B_DVBC_SYMBOL_RATE_PROP
     case         Si2168B_DVBC_SYMBOL_RATE_PROP_CODE:
               prop->dvbc_symbol_rate.rate = (data >> Si2168B_DVBC_SYMBOL_RATE_PROP_RATE_LSB) & Si2168B_DVBC_SYMBOL_RATE_PROP_RATE_MASK;
     break;
    #endif /*     Si2168B_DVBC_SYMBOL_RATE_PROP */


    #ifdef        Si2168B_DVBT2_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBT2_ADC_CREST_FACTOR_PROP_CODE:
               prop->dvbt2_adc_crest_factor.crest_factor = (data >> Si2168B_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB) & Si2168B_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK;
     break;
    #endif /*     Si2168B_DVBT2_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBT2_AFC_RANGE_PROP
     case         Si2168B_DVBT2_AFC_RANGE_PROP_CODE:
               prop->dvbt2_afc_range.range_khz = (data >> Si2168B_DVBT2_AFC_RANGE_PROP_RANGE_KHZ_LSB) & Si2168B_DVBT2_AFC_RANGE_PROP_RANGE_KHZ_MASK;
     break;
    #endif /*     Si2168B_DVBT2_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBT2_FEF_TUNER_PROP
     case         Si2168B_DVBT2_FEF_TUNER_PROP_CODE:
               prop->dvbt2_fef_tuner.tuner_delay         = (data >> Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_DELAY_LSB        ) & Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_DELAY_MASK;
               prop->dvbt2_fef_tuner.tuner_freeze_time   = (data >> Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME_LSB  ) & Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME_MASK;
               prop->dvbt2_fef_tuner.tuner_unfreeze_time = (data >> Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME_LSB) & Si2168B_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME_MASK;
     break;
    #endif /*     Si2168B_DVBT2_FEF_TUNER_PROP */
    #ifdef        Si2168B_DVBT2_MODE_PROP
     case         Si2168B_DVBT2_MODE_PROP_CODE:
               prop->dvbt2_mode.lock_mode = (data >> Si2168B_DVBT2_MODE_PROP_LOCK_MODE_LSB) & Si2168B_DVBT2_MODE_PROP_LOCK_MODE_MASK;
     break;
    #endif /*     Si2168B_DVBT2_MODE_PROP */

    #ifdef        Si2168B_DVBT_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBT_ADC_CREST_FACTOR_PROP_CODE:
               prop->dvbt_adc_crest_factor.crest_factor = (data >> Si2168B_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB) & Si2168B_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK;
     break;
    #endif /*     Si2168B_DVBT_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBT_AFC_RANGE_PROP
     case         Si2168B_DVBT_AFC_RANGE_PROP_CODE:
               prop->dvbt_afc_range.range_khz = (data >> Si2168B_DVBT_AFC_RANGE_PROP_RANGE_KHZ_LSB) & Si2168B_DVBT_AFC_RANGE_PROP_RANGE_KHZ_MASK;
     break;
    #endif /*     Si2168B_DVBT_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBT_HIERARCHY_PROP
     case         Si2168B_DVBT_HIERARCHY_PROP_CODE:
               prop->dvbt_hierarchy.stream = (data >> Si2168B_DVBT_HIERARCHY_PROP_STREAM_LSB) & Si2168B_DVBT_HIERARCHY_PROP_STREAM_MASK;
     break;
    #endif /*     Si2168B_DVBT_HIERARCHY_PROP */

    #ifdef        Si2168B_MASTER_IEN_PROP
     case         Si2168B_MASTER_IEN_PROP_CODE:
               prop->master_ien.ddien   = (data >> Si2168B_MASTER_IEN_PROP_DDIEN_LSB  ) & Si2168B_MASTER_IEN_PROP_DDIEN_MASK;
               prop->master_ien.scanien = (data >> Si2168B_MASTER_IEN_PROP_SCANIEN_LSB) & Si2168B_MASTER_IEN_PROP_SCANIEN_MASK;
               prop->master_ien.errien  = (data >> Si2168B_MASTER_IEN_PROP_ERRIEN_LSB ) & Si2168B_MASTER_IEN_PROP_ERRIEN_MASK;
               prop->master_ien.ctsien  = (data >> Si2168B_MASTER_IEN_PROP_CTSIEN_LSB ) & Si2168B_MASTER_IEN_PROP_CTSIEN_MASK;
     break;
    #endif /*     Si2168B_MASTER_IEN_PROP */
    #ifdef        Si2168B_MCNS_ADC_CREST_FACTOR_PROP
     case         Si2168B_MCNS_ADC_CREST_FACTOR_PROP_CODE:
               prop->mcns_adc_crest_factor.crest_factor = (data >> Si2168B_MCNS_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB) & Si2168B_MCNS_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK;
     break;
    #endif /*     Si2168B_MCNS_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_MCNS_AFC_RANGE_PROP
     case         Si2168B_MCNS_AFC_RANGE_PROP_CODE:
               prop->mcns_afc_range.range_khz = (data >> Si2168B_MCNS_AFC_RANGE_PROP_RANGE_KHZ_LSB) & Si2168B_MCNS_AFC_RANGE_PROP_RANGE_KHZ_MASK;
     break;
    #endif /*     Si2168B_MCNS_AFC_RANGE_PROP */
    #ifdef        Si2168B_MCNS_CONSTELLATION_PROP
     case         Si2168B_MCNS_CONSTELLATION_PROP_CODE:
               prop->mcns_constellation.constellation = (data >> Si2168B_MCNS_CONSTELLATION_PROP_CONSTELLATION_LSB) & Si2168B_MCNS_CONSTELLATION_PROP_CONSTELLATION_MASK;
     break;
    #endif /*     Si2168B_MCNS_CONSTELLATION_PROP */
    #ifdef        Si2168B_MCNS_SYMBOL_RATE_PROP
     case         Si2168B_MCNS_SYMBOL_RATE_PROP_CODE:
               prop->mcns_symbol_rate.rate = (data >> Si2168B_MCNS_SYMBOL_RATE_PROP_RATE_LSB) & Si2168B_MCNS_SYMBOL_RATE_PROP_RATE_MASK;
     break;
    #endif /*     Si2168B_MCNS_SYMBOL_RATE_PROP */

    #ifdef        Si2168B_SCAN_FMAX_PROP
     case         Si2168B_SCAN_FMAX_PROP_CODE:
               prop->scan_fmax.scan_fmax = (data >> Si2168B_SCAN_FMAX_PROP_SCAN_FMAX_LSB) & Si2168B_SCAN_FMAX_PROP_SCAN_FMAX_MASK;
     break;
    #endif /*     Si2168B_SCAN_FMAX_PROP */
    #ifdef        Si2168B_SCAN_FMIN_PROP
     case         Si2168B_SCAN_FMIN_PROP_CODE:
               prop->scan_fmin.scan_fmin = (data >> Si2168B_SCAN_FMIN_PROP_SCAN_FMIN_LSB) & Si2168B_SCAN_FMIN_PROP_SCAN_FMIN_MASK;
     break;
    #endif /*     Si2168B_SCAN_FMIN_PROP */
    #ifdef        Si2168B_SCAN_IEN_PROP
     case         Si2168B_SCAN_IEN_PROP_CODE:
               prop->scan_ien.buzien = (data >> Si2168B_SCAN_IEN_PROP_BUZIEN_LSB) & Si2168B_SCAN_IEN_PROP_BUZIEN_MASK;
               prop->scan_ien.reqien = (data >> Si2168B_SCAN_IEN_PROP_REQIEN_LSB) & Si2168B_SCAN_IEN_PROP_REQIEN_MASK;
     break;
    #endif /*     Si2168B_SCAN_IEN_PROP */
    #ifdef        Si2168B_SCAN_INT_SENSE_PROP
     case         Si2168B_SCAN_INT_SENSE_PROP_CODE:
               prop->scan_int_sense.buznegen = (data >> Si2168B_SCAN_INT_SENSE_PROP_BUZNEGEN_LSB) & Si2168B_SCAN_INT_SENSE_PROP_BUZNEGEN_MASK;
               prop->scan_int_sense.reqnegen = (data >> Si2168B_SCAN_INT_SENSE_PROP_REQNEGEN_LSB) & Si2168B_SCAN_INT_SENSE_PROP_REQNEGEN_MASK;
               prop->scan_int_sense.buzposen = (data >> Si2168B_SCAN_INT_SENSE_PROP_BUZPOSEN_LSB) & Si2168B_SCAN_INT_SENSE_PROP_BUZPOSEN_MASK;
               prop->scan_int_sense.reqposen = (data >> Si2168B_SCAN_INT_SENSE_PROP_REQPOSEN_LSB) & Si2168B_SCAN_INT_SENSE_PROP_REQPOSEN_MASK;
     break;
    #endif /*     Si2168B_SCAN_INT_SENSE_PROP */

    #ifdef        Si2168B_SCAN_SYMB_RATE_MAX_PROP
     case         Si2168B_SCAN_SYMB_RATE_MAX_PROP_CODE:
               prop->scan_symb_rate_max.scan_symb_rate_max = (data >> Si2168B_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX_LSB) & Si2168B_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX_MASK;
     break;
    #endif /*     Si2168B_SCAN_SYMB_RATE_MAX_PROP */
    #ifdef        Si2168B_SCAN_SYMB_RATE_MIN_PROP
     case         Si2168B_SCAN_SYMB_RATE_MIN_PROP_CODE:
               prop->scan_symb_rate_min.scan_symb_rate_min = (data >> Si2168B_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN_LSB) & Si2168B_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN_MASK;
     break;
    #endif /*     Si2168B_SCAN_SYMB_RATE_MIN_PROP */
    #ifdef        Si2168B_SCAN_TER_CONFIG_PROP
     case         Si2168B_SCAN_TER_CONFIG_PROP_CODE:
               prop->scan_ter_config.mode          = (data >> Si2168B_SCAN_TER_CONFIG_PROP_MODE_LSB         ) & Si2168B_SCAN_TER_CONFIG_PROP_MODE_MASK;
               prop->scan_ter_config.analog_bw     = (data >> Si2168B_SCAN_TER_CONFIG_PROP_ANALOG_BW_LSB    ) & Si2168B_SCAN_TER_CONFIG_PROP_ANALOG_BW_MASK;
               prop->scan_ter_config.search_analog = (data >> Si2168B_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_LSB) & Si2168B_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_MASK;
               prop->scan_ter_config.scan_debug    = (data >> Si2168B_SCAN_TER_CONFIG_PROP_SCAN_DEBUG_LSB   ) & Si2168B_SCAN_TER_CONFIG_PROP_SCAN_DEBUG_MASK;
     break;
    #endif /*     Si2168B_SCAN_TER_CONFIG_PROP */

     default : return ERROR_Si2168B_UNKNOWN_PROPERTY; break;
    }
    return NO_Si2168B_ERROR;
  }
/***********************************************************************************************************************
  Si2168B_storePropertiesDefaults function
  Use:        property defaults function
              Used to fill the propShadow structure with startup values.
  Parameter: *prop     the Si2168B_PropObject structure

 |---------------------------------------------------------------------------------------------------------------------|
 | Do NOT change this code unless you really know what you're doing!                                                   |
 | It should reflect the part internal property settings after firmware download                                       |
 |---------------------------------------------------------------------------------------------------------------------|

 Returns:    void
 ***********************************************************************************************************************/
void          Si2168B_storePropertiesDefaults(Si2168B_PropObj   *prop) {
  SiTRACE("Si2168B_storePropertiesDefaults\n");
#ifdef    Si2168B_MASTER_IEN_PROP
  prop->master_ien.ddien                                                 = Si2168B_MASTER_IEN_PROP_DDIEN_OFF   ; /* (default 'OFF') */
  prop->master_ien.scanien                                               = Si2168B_MASTER_IEN_PROP_SCANIEN_OFF ; /* (default 'OFF') */
  prop->master_ien.errien                                                = Si2168B_MASTER_IEN_PROP_ERRIEN_OFF  ; /* (default 'OFF') */
  prop->master_ien.ctsien                                                = Si2168B_MASTER_IEN_PROP_CTSIEN_OFF  ; /* (default 'OFF') */
#endif /* Si2168B_MASTER_IEN_PROP */

#ifdef    Si2168B_DD_BER_RESOL_PROP
  prop->dd_ber_resol.exp                                                 =     7; /* (default     7) */
  prop->dd_ber_resol.mant                                                =     1; /* (default     1) */
#endif /* Si2168B_DD_BER_RESOL_PROP */

#ifdef    Si2168B_DD_CBER_RESOL_PROP
  prop->dd_cber_resol.exp                                                =     5; /* (default     5) */
  prop->dd_cber_resol.mant                                               =     1; /* (default     1) */
#endif /* Si2168B_DD_CBER_RESOL_PROP */

#ifdef    Si2168B_DD_DISEQC_FREQ_PROP
  prop->dd_diseqc_freq.freq_hz                                           = 22000; /* (default 22000) */
#endif /* Si2168B_DD_DISEQC_FREQ_PROP */

#ifdef    Si2168B_DD_DISEQC_PARAM_PROP
  prop->dd_diseqc_param.sequence_mode                                    = Si2168B_DD_DISEQC_PARAM_PROP_SEQUENCE_MODE_GAP ; /* (default 'GAP') */
#endif /* Si2168B_DD_DISEQC_PARAM_PROP */

#ifdef    Si2168B_DD_FER_RESOL_PROP
  prop->dd_fer_resol.exp                                                 =     3; /* (default     3) */
  prop->dd_fer_resol.mant                                                =     1; /* (default     1) */
#endif /* Si2168B_DD_FER_RESOL_PROP */

#ifdef    Si2168B_DD_IEN_PROP
  prop->dd_ien.ien_bit0                                                  = Si2168B_DD_IEN_PROP_IEN_BIT0_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit1                                                  = Si2168B_DD_IEN_PROP_IEN_BIT1_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit2                                                  = Si2168B_DD_IEN_PROP_IEN_BIT2_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit3                                                  = Si2168B_DD_IEN_PROP_IEN_BIT3_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit4                                                  = Si2168B_DD_IEN_PROP_IEN_BIT4_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit5                                                  = Si2168B_DD_IEN_PROP_IEN_BIT5_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit6                                                  = Si2168B_DD_IEN_PROP_IEN_BIT6_DISABLE ; /* (default 'DISABLE') */
  prop->dd_ien.ien_bit7                                                  = Si2168B_DD_IEN_PROP_IEN_BIT7_DISABLE ; /* (default 'DISABLE') */
#endif /* Si2168B_DD_IEN_PROP */

#ifdef    Si2168B_DD_IF_INPUT_FREQ_PROP
  prop->dd_if_input_freq.offset                                          =  5000; /* (default  5000) */
#endif /* Si2168B_DD_IF_INPUT_FREQ_PROP */

#ifdef    Si2168B_DD_INT_SENSE_PROP
  prop->dd_int_sense.neg_bit0                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT0_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit1                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT1_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit2                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT2_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit3                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT3_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit4                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT4_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit5                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT5_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit6                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT6_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.neg_bit7                                            = Si2168B_DD_INT_SENSE_PROP_NEG_BIT7_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit0                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT0_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit1                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT1_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit2                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT2_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit3                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT3_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit4                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT4_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit5                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT5_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit6                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT6_DISABLE ; /* (default 'DISABLE') */
  prop->dd_int_sense.pos_bit7                                            = Si2168B_DD_INT_SENSE_PROP_POS_BIT7_DISABLE ; /* (default 'DISABLE') */
#endif /* Si2168B_DD_INT_SENSE_PROP */

#ifdef    Si2168B_DD_MODE_PROP
  prop->dd_mode.bw                                                       = Si2168B_DD_MODE_PROP_BW_BW_8MHZ              ; /* (default 'BW_8MHZ') */
  prop->dd_mode.modulation                                               = Si2168B_DD_MODE_PROP_MODULATION_DVBT         ; /* (default 'DVBT') */
  prop->dd_mode.invert_spectrum                                          = Si2168B_DD_MODE_PROP_INVERT_SPECTRUM_NORMAL  ; /* (default 'NORMAL') */
  prop->dd_mode.auto_detect                                              = Si2168B_DD_MODE_PROP_AUTO_DETECT_NONE        ; /* (default 'NONE') */
#endif /* Si2168B_DD_MODE_PROP */

#ifdef    Si2168B_DD_PER_RESOL_PROP
  prop->dd_per_resol.exp                                                 =     5; /* (default     5) */
  prop->dd_per_resol.mant                                                =     1; /* (default     1) */
#endif /* Si2168B_DD_PER_RESOL_PROP */

#ifdef    Si2168B_DD_RSQ_BER_THRESHOLD_PROP
  prop->dd_rsq_ber_threshold.exp                                         =     1; /* (default     1) */
  prop->dd_rsq_ber_threshold.mant                                        =    10; /* (default    10) */
#endif /* Si2168B_DD_RSQ_BER_THRESHOLD_PROP */

#ifdef    Si2168B_DD_SSI_SQI_PARAM_PROP
  prop->dd_ssi_sqi_param.sqi_average                                     =     1; /* (default     1) */
#endif /* Si2168B_DD_SSI_SQI_PARAM_PROP */

#ifdef    Si2168B_DD_TS_FREQ_PROP
  prop->dd_ts_freq.req_freq_10khz                                        =   720; /* (default   720) */
#endif /* Si2168B_DD_TS_FREQ_PROP */

#ifdef    Si2168B_DD_TS_MODE_PROP
  prop->dd_ts_mode.mode                                                  = Si2168B_DD_TS_MODE_PROP_MODE_TRISTATE                ; /* (default 'TRISTATE') */
  prop->dd_ts_mode.clock                                                 = Si2168B_DD_TS_MODE_PROP_CLOCK_AUTO_FIXED             ; /* (default 'AUTO_FIXED') */
  prop->dd_ts_mode.clk_gapped_en                                         = Si2168B_DD_TS_MODE_PROP_CLK_GAPPED_EN_DISABLED       ; /* (default 'DISABLED') */
  prop->dd_ts_mode.ts_err_polarity                                       = Si2168B_DD_TS_MODE_PROP_TS_ERR_POLARITY_NOT_INVERTED ; /* (default 'NOT_INVERTED') */
  prop->dd_ts_mode.special                                               = Si2168B_DD_TS_MODE_PROP_SPECIAL_FULL_TS              ; /* (default 'FULL_TS') */
  prop->dd_ts_mode.ts_freq_resolution                                    = Si2168B_DD_TS_MODE_PROP_TS_FREQ_RESOLUTION_NORMAL       ; /* (default 'NORMAL') */
#endif /* Si2168B_DD_TS_MODE_PROP */

#ifdef    Si2168B_DD_TS_SERIAL_DIFF_PROP
  prop->dd_ts_serial_diff.ts_data1_strength                              =    15; /* (default    15) */
  prop->dd_ts_serial_diff.ts_data1_shape                                 =     3; /* (default     3) */
  prop->dd_ts_serial_diff.ts_data2_strength                              =    15; /* (default    15) */
  prop->dd_ts_serial_diff.ts_data2_shape                                 =     3; /* (default     3) */
  prop->dd_ts_serial_diff.ts_clkb_on_data1                               = Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_CLKB_ON_DATA1_DISABLE   ; /* (default 'DISABLE') */
  prop->dd_ts_serial_diff.ts_data0b_on_data2                             = Si2168B_DD_TS_SERIAL_DIFF_PROP_TS_DATA0B_ON_DATA2_DISABLE ; /* (default 'DISABLE') */
#endif /* Si2168B_DD_TS_SERIAL_DIFF_PROP */

#ifdef    Si2168B_DD_TS_SETUP_PAR_PROP
  prop->dd_ts_setup_par.ts_data_strength                                 =     3; /* (default     3) */
  prop->dd_ts_setup_par.ts_data_shape                                    =     1; /* (default     1) */
  prop->dd_ts_setup_par.ts_clk_strength                                  =     3; /* (default     3) */
  prop->dd_ts_setup_par.ts_clk_shape                                     =     1; /* (default     1) */
  prop->dd_ts_setup_par.ts_clk_invert                                    = Si2168B_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_INVERTED    ; /* (default 'INVERTED') */
  prop->dd_ts_setup_par.ts_clk_shift                                     =     0; /* (default     0) */
#endif /* Si2168B_DD_TS_SETUP_PAR_PROP */

#ifdef    Si2168B_DD_TS_SETUP_SER_PROP
  prop->dd_ts_setup_ser.ts_data_strength                                 =    15; /* (default    15) */
  prop->dd_ts_setup_ser.ts_data_shape                                    =     3; /* (default     3) */
  prop->dd_ts_setup_ser.ts_clk_strength                                  =    15; /* (default    15) */
  prop->dd_ts_setup_ser.ts_clk_shape                                     =     3; /* (default     3) */
  prop->dd_ts_setup_ser.ts_clk_invert                                    = Si2168B_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_INVERTED      ; /* (default 'INVERTED') */
  prop->dd_ts_setup_ser.ts_sync_duration                                 = Si2168B_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_FIRST_BYTE ; /* (default 'FIRST_BYTE') */
  prop->dd_ts_setup_ser.ts_byte_order                                    = Si2168B_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_MSB_FIRST     ; /* (default 'MSB_FIRST') */
#endif /* Si2168B_DD_TS_SETUP_SER_PROP */

#ifdef    Si2168B_DVBC_ADC_CREST_FACTOR_PROP
  prop->dvbc_adc_crest_factor.crest_factor                               =   112; /* (default   112) */
#endif /* Si2168B_DVBC_ADC_CREST_FACTOR_PROP */

#ifdef    Si2168B_DVBC_AFC_RANGE_PROP
  prop->dvbc_afc_range.range_khz                                         =   100; /* (default   100) */
#endif /* Si2168B_DVBC_AFC_RANGE_PROP */

#ifdef    Si2168B_DVBC_CONSTELLATION_PROP
  prop->dvbc_constellation.constellation                                 = Si2168B_DVBC_CONSTELLATION_PROP_CONSTELLATION_AUTO ; /* (default 'AUTO') */
#endif /* Si2168B_DVBC_CONSTELLATION_PROP */

#ifdef    Si2168B_DVBC_SYMBOL_RATE_PROP
  prop->dvbc_symbol_rate.rate                                            =  6900; /* (default  6900) */
#endif /* Si2168B_DVBC_SYMBOL_RATE_PROP */




#ifdef    Si2168B_DVBT_ADC_CREST_FACTOR_PROP
  prop->dvbt_adc_crest_factor.crest_factor                               =   130; /* (default   130) */
#endif /* Si2168B_DVBT_ADC_CREST_FACTOR_PROP */

#ifdef    Si2168B_DVBT_AFC_RANGE_PROP
  prop->dvbt_afc_range.range_khz                                         =   550; /* (default   550) */
#endif /* Si2168B_DVBT_AFC_RANGE_PROP */

#ifdef    Si2168B_DVBT_HIERARCHY_PROP
  prop->dvbt_hierarchy.stream                                            = Si2168B_DVBT_HIERARCHY_PROP_STREAM_HP ; /* (default 'HP') */
#endif /* Si2168B_DVBT_HIERARCHY_PROP */


#ifdef    Si2168B_DVBT2_ADC_CREST_FACTOR_PROP
  prop->dvbt2_adc_crest_factor.crest_factor                              =   130; /* (default   130) */
#endif /* Si2168B_DVBT2_ADC_CREST_FACTOR_PROP */

#ifdef    Si2168B_DVBT2_AFC_RANGE_PROP
  prop->dvbt2_afc_range.range_khz                                        =   550; /* (default   550) */
#endif /* Si2168B_DVBT2_AFC_RANGE_PROP */

#ifdef    Si2168B_DVBT2_FEF_TUNER_PROP
  prop->dvbt2_fef_tuner.tuner_delay                                      =     1; /* (default     1) */
  prop->dvbt2_fef_tuner.tuner_freeze_time                                =     1; /* (default     1) */
  prop->dvbt2_fef_tuner.tuner_unfreeze_time                              =     1; /* (default     1) */
#endif /* Si2168B_DVBT2_FEF_TUNER_PROP */

#ifdef    Si2168B_DVBT2_MODE_PROP
  prop->dvbt2_mode.lock_mode                                             = Si2168B_DVBT2_MODE_PROP_LOCK_MODE_ANY ; /* (default 'ANY') */
#endif /* Si2168B_DVBT2_MODE_PROP */


#ifdef    Si2168B_MCNS_ADC_CREST_FACTOR_PROP
  prop->mcns_adc_crest_factor.crest_factor                               =   112; /* (default   112) */
#endif /* Si2168B_MCNS_ADC_CREST_FACTOR_PROP */

#ifdef    Si2168B_MCNS_AFC_RANGE_PROP
  prop->mcns_afc_range.range_khz                                         =   100; /* (default   100) */
#endif /* Si2168B_MCNS_AFC_RANGE_PROP */

#ifdef    Si2168B_MCNS_CONSTELLATION_PROP
  prop->mcns_constellation.constellation                                 = Si2168B_MCNS_CONSTELLATION_PROP_CONSTELLATION_AUTO ; /* (default 'AUTO') */
#endif /* Si2168B_MCNS_CONSTELLATION_PROP */

#ifdef    Si2168B_MCNS_SYMBOL_RATE_PROP
  prop->mcns_symbol_rate.rate                                            =  6900; /* (default  6900) */
#endif /* Si2168B_MCNS_SYMBOL_RATE_PROP */


#ifdef    Si2168B_SCAN_FMAX_PROP
  prop->scan_fmax.scan_fmax                                              =     0; /* (default     0) */
#endif /* Si2168B_SCAN_FMAX_PROP */

#ifdef    Si2168B_SCAN_FMIN_PROP
  prop->scan_fmin.scan_fmin                                              =     0; /* (default     0) */
#endif /* Si2168B_SCAN_FMIN_PROP */

#ifdef    Si2168B_SCAN_IEN_PROP
  prop->scan_ien.buzien                                                  = Si2168B_SCAN_IEN_PROP_BUZIEN_DISABLE ; /* (default 'DISABLE') */
  prop->scan_ien.reqien                                                  = Si2168B_SCAN_IEN_PROP_REQIEN_DISABLE ; /* (default 'DISABLE') */
#endif /* Si2168B_SCAN_IEN_PROP */

#ifdef    Si2168B_SCAN_INT_SENSE_PROP
  prop->scan_int_sense.buznegen                                          = Si2168B_SCAN_INT_SENSE_PROP_BUZNEGEN_ENABLE  ; /* (default 'ENABLE') */
  prop->scan_int_sense.reqnegen                                          = Si2168B_SCAN_INT_SENSE_PROP_REQNEGEN_DISABLE ; /* (default 'DISABLE') */
  prop->scan_int_sense.buzposen                                          = Si2168B_SCAN_INT_SENSE_PROP_BUZPOSEN_DISABLE ; /* (default 'DISABLE') */
  prop->scan_int_sense.reqposen                                          = Si2168B_SCAN_INT_SENSE_PROP_REQPOSEN_ENABLE  ; /* (default 'ENABLE') */
#endif /* Si2168B_SCAN_INT_SENSE_PROP */

#ifdef    Si2168B_SCAN_SAT_CONFIG_PROP
  prop->scan_sat_config.analog_detect                                    = Si2168B_SCAN_SAT_CONFIG_PROP_ANALOG_DETECT_DISABLED ; /* (default 'DISABLED') */
  prop->scan_sat_config.reserved1                                        =     0; /* (default     0) */
  prop->scan_sat_config.reserved2                                        =    12; /* (default    12) */
  prop->scan_sat_config.scan_debug                                       =     0; /* (default     0s) */
#endif /* Si2168B_SCAN_SAT_CONFIG_PROP */

#ifdef    Si2168B_SCAN_SAT_UNICABLE_BW_PROP
  prop->scan_sat_unicable_bw.scan_sat_unicable_bw                        =     0; /* (default     0) */
#endif /* Si2168B_SCAN_SAT_UNICABLE_BW_PROP */

#ifdef    Si2168B_SCAN_SAT_UNICABLE_MIN_TUNE_STEP_PROP
  prop->scan_sat_unicable_min_tune_step.scan_sat_unicable_min_tune_step  =    50; /* (default    50) */
#endif /* Si2168B_SCAN_SAT_UNICABLE_MIN_TUNE_STEP_PROP */

#ifdef    Si2168B_SCAN_SYMB_RATE_MAX_PROP
  prop->scan_symb_rate_max.scan_symb_rate_max                            =     0; /* (default     0) */
#endif /* Si2168B_SCAN_SYMB_RATE_MAX_PROP */

#ifdef    Si2168B_SCAN_SYMB_RATE_MIN_PROP
  prop->scan_symb_rate_min.scan_symb_rate_min                            =     0; /* (default     0) */
#endif /* Si2168B_SCAN_SYMB_RATE_MIN_PROP */

#ifdef    Si2168B_SCAN_TER_CONFIG_PROP
  prop->scan_ter_config.mode                                             = Si2168B_SCAN_TER_CONFIG_PROP_MODE_MAPPING_SCAN        ; /* (default 'BLIND_SCAN') */
  prop->scan_ter_config.analog_bw                                        = Si2168B_SCAN_TER_CONFIG_PROP_ANALOG_BW_8MHZ           ; /* (default '8MHZ') */
  prop->scan_ter_config.search_analog                                    = Si2168B_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_DISABLE    ; /* (default 'DISABLE') */
#endif /* Si2168B_SCAN_TER_CONFIG_PROP */

}
#ifdef    Si2168B_GET_PROPERTY_STRING
/***********************************************************************************************************************
  Si2168B_L1_PropertyText function
  Use:        property text function
              Used to turn the property data into clear text.
  Parameter: *prop     the Si2168B property structure (containing all properties)
  Parameter: prop_code the property Id (used to know which property to use)
  Parameter: separator the string to use between fields (often either a blank or a newline character)
  Parameter: msg       the string used to store the resulting string
                      It must be declared by the caller with a size of 1000 bytes
  Returns:    NO_Si2168B_ERROR if successful.
 ***********************************************************************************************************************/
unsigned char Si2168B_L1_PropertyText          (Si2168B_PropObj   *prop, unsigned int prop_code, const char *separator, char *msg) {
    switch (prop_code) {
    #ifdef        Si2168B_DD_BER_RESOL_PROP
     case         Si2168B_DD_BER_RESOL_PROP_CODE:
      sprintf(msg,1000,"DD_BER_RESOL");
       strcat(msg,separator,1000); strcat(msg,"-EXP ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ber_resol.exp);
       strcat(msg,separator,1000); strcat(msg,"-MANT ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ber_resol.mant);
     break;
    #endif /*     Si2168B_DD_BER_RESOL_PROP */
    #ifdef        Si2168B_DD_CBER_RESOL_PROP
     case         Si2168B_DD_CBER_RESOL_PROP_CODE:
      sprintf(msg,1000,"DD_CBER_RESOL");
       strcat(msg,separator,1000); strcat(msg,"-EXP ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_cber_resol.exp);
       strcat(msg,separator,1000); strcat(msg,"-MANT ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_cber_resol.mant);
     break;
    #endif /*     Si2168B_DD_CBER_RESOL_PROP */

    #ifdef        Si2168B_DD_FER_RESOL_PROP
     case         Si2168B_DD_FER_RESOL_PROP_CODE:
      sprintf(msg,1000,"DD_FER_RESOL");
       strcat(msg,separator,1000); strcat(msg,"-EXP ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_fer_resol.exp);
       strcat(msg,separator,1000); strcat(msg,"-MANT ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_fer_resol.mant);
     break;
    #endif /*     Si2168B_DD_FER_RESOL_PROP */
    #ifdef        Si2168B_DD_IEN_PROP
     case         Si2168B_DD_IEN_PROP_CODE:
      sprintf(msg,1000,"DD_IEN");
       strcat(msg,separator,1000); strcat(msg,"-IEN_BIT0 ",1000);
           if  (prop->dd_ien.ien_bit0 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ien.ien_bit0 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->dd_ien.ien_bit0);
       strcat(msg,separator,1000); strcat(msg,"-IEN_BIT1 ",1000);
           if  (prop->dd_ien.ien_bit1 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ien.ien_bit1 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->dd_ien.ien_bit1);
       strcat(msg,separator,1000); strcat(msg,"-IEN_BIT2 ",1000);
           if  (prop->dd_ien.ien_bit2 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ien.ien_bit2 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->dd_ien.ien_bit2);
       strcat(msg,separator,1000); strcat(msg,"-IEN_BIT3 ",1000);
           if  (prop->dd_ien.ien_bit3 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ien.ien_bit3 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->dd_ien.ien_bit3);
       strcat(msg,separator,1000); strcat(msg,"-IEN_BIT4 ",1000);
           if  (prop->dd_ien.ien_bit4 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ien.ien_bit4 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->dd_ien.ien_bit4);
       strcat(msg,separator,1000); strcat(msg,"-IEN_BIT5 ",1000);
           if  (prop->dd_ien.ien_bit5 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ien.ien_bit5 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->dd_ien.ien_bit5);
       strcat(msg,separator,1000); strcat(msg,"-IEN_BIT6 ",1000);
           if  (prop->dd_ien.ien_bit6 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ien.ien_bit6 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->dd_ien.ien_bit6);
       strcat(msg,separator,1000); strcat(msg,"-IEN_BIT7 ",1000);
           if  (prop->dd_ien.ien_bit7 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ien.ien_bit7 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->dd_ien.ien_bit7);
     break;
    #endif /*     Si2168B_DD_IEN_PROP */
    #ifdef        Si2168B_DD_IF_INPUT_FREQ_PROP
     case         Si2168B_DD_IF_INPUT_FREQ_PROP_CODE:
      sprintf(msg,1000,"DD_IF_INPUT_FREQ");
       strcat(msg,separator,1000); strcat(msg,"-OFFSET ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_if_input_freq.offset);
     break;
    #endif /*     Si2168B_DD_IF_INPUT_FREQ_PROP */
    #ifdef        Si2168B_DD_INT_SENSE_PROP
     case         Si2168B_DD_INT_SENSE_PROP_CODE:
      sprintf(msg,1000,"DD_INT_SENSE");
       strcat(msg,separator,1000); strcat(msg,"-NEG_BIT0 ",1000);
           if  (prop->dd_int_sense.neg_bit0 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.neg_bit0 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.neg_bit0);
       strcat(msg,separator,1000); strcat(msg,"-NEG_BIT1 ",1000);
           if  (prop->dd_int_sense.neg_bit1 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.neg_bit1 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.neg_bit1);
       strcat(msg,separator,1000); strcat(msg,"-NEG_BIT2 ",1000);
           if  (prop->dd_int_sense.neg_bit2 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.neg_bit2 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.neg_bit2);
       strcat(msg,separator,1000); strcat(msg,"-NEG_BIT3 ",1000);
           if  (prop->dd_int_sense.neg_bit3 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.neg_bit3 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.neg_bit3);
       strcat(msg,separator,1000); strcat(msg,"-NEG_BIT4 ",1000);
           if  (prop->dd_int_sense.neg_bit4 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.neg_bit4 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.neg_bit4);
       strcat(msg,separator,1000); strcat(msg,"-NEG_BIT5 ",1000);
           if  (prop->dd_int_sense.neg_bit5 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.neg_bit5 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.neg_bit5);
       strcat(msg,separator,1000); strcat(msg,"-NEG_BIT6 ",1000);
           if  (prop->dd_int_sense.neg_bit6 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.neg_bit6 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.neg_bit6);
       strcat(msg,separator,1000); strcat(msg,"-NEG_BIT7 ",1000);
           if  (prop->dd_int_sense.neg_bit7 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.neg_bit7 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.neg_bit7);
       strcat(msg,separator,1000); strcat(msg,"-POS_BIT0 ",1000);
           if  (prop->dd_int_sense.pos_bit0 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.pos_bit0 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.pos_bit0);
       strcat(msg,separator,1000); strcat(msg,"-POS_BIT1 ",1000);
           if  (prop->dd_int_sense.pos_bit1 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.pos_bit1 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.pos_bit1);
       strcat(msg,separator,1000); strcat(msg,"-POS_BIT2 ",1000);
           if  (prop->dd_int_sense.pos_bit2 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.pos_bit2 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.pos_bit2);
       strcat(msg,separator,1000); strcat(msg,"-POS_BIT3 ",1000);
           if  (prop->dd_int_sense.pos_bit3 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.pos_bit3 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.pos_bit3);
       strcat(msg,separator,1000); strcat(msg,"-POS_BIT4 ",1000);
           if  (prop->dd_int_sense.pos_bit4 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.pos_bit4 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.pos_bit4);
       strcat(msg,separator,1000); strcat(msg,"-POS_BIT5 ",1000);
           if  (prop->dd_int_sense.pos_bit5 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.pos_bit5 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.pos_bit5);
       strcat(msg,separator,1000); strcat(msg,"-POS_BIT6 ",1000);
           if  (prop->dd_int_sense.pos_bit6 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.pos_bit6 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.pos_bit6);
       strcat(msg,separator,1000); strcat(msg,"-POS_BIT7 ",1000);
           if  (prop->dd_int_sense.pos_bit7 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_int_sense.pos_bit7 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dd_int_sense.pos_bit7);
     break;
    #endif /*     Si2168B_DD_INT_SENSE_PROP */
    #ifdef        Si2168B_DD_MODE_PROP
     case         Si2168B_DD_MODE_PROP_CODE:
      sprintf(msg,1000,"DD_MODE");
       strcat(msg,separator,1000); strcat(msg,"-BW ",1000);
           if  (prop->dd_mode.bw              ==     5) strcat(msg,"BW_5MHZ   ",1000);
      else if  (prop->dd_mode.bw              ==     6) strcat(msg,"BW_6MHZ   ",1000);
      else if  (prop->dd_mode.bw              ==     7) strcat(msg,"BW_7MHZ   ",1000);
      else if  (prop->dd_mode.bw              ==     8) strcat(msg,"BW_8MHZ   ",1000);
      else if  (prop->dd_mode.bw              ==     2) strcat(msg,"BW_1D7MHZ ",1000);
      else                                             sprintf(msg,1000,"%s%d", msg, prop->dd_mode.bw);
       strcat(msg,separator,1000); strcat(msg,"-MODULATION ",1000);
           if  (prop->dd_mode.modulation      ==     1) strcat(msg,"MCNS        ",1000);
      else if  (prop->dd_mode.modulation      ==     2) strcat(msg,"DVBT        ",1000);
      else if  (prop->dd_mode.modulation      ==     3) strcat(msg,"DVBC        ",1000);
      else if  (prop->dd_mode.modulation      ==     7) strcat(msg,"DVBT2       ",1000);
      else if  (prop->dd_mode.modulation      ==     8) strcat(msg,"DVBS        ",1000);
      else if  (prop->dd_mode.modulation      ==     9) strcat(msg,"DVBS2       ",1000);
      else if  (prop->dd_mode.modulation      ==    10) strcat(msg,"DSS         ",1000);
      else if  (prop->dd_mode.modulation      ==    11) strcat(msg,"DVBC2       ",1000);
      else if  (prop->dd_mode.modulation      ==    15) strcat(msg,"AUTO_DETECT ",1000);
      else                                             sprintf(msg,1000,"%s%d", msg, prop->dd_mode.modulation);
       strcat(msg,separator,1000); strcat(msg,"-INVERT_SPECTRUM ",1000);
           if  (prop->dd_mode.invert_spectrum ==     0) strcat(msg,"NORMAL   ",1000);
      else if  (prop->dd_mode.invert_spectrum ==     1) strcat(msg,"INVERTED ",1000);
      else                                             sprintf(msg,1000,"%s%d", msg, prop->dd_mode.invert_spectrum);
       strcat(msg,separator,1000); strcat(msg,"-AUTO_DETECT ",1000);
           if  (prop->dd_mode.auto_detect     ==     0) strcat(msg,"NONE              ",1000);
      else if  (prop->dd_mode.auto_detect     ==     1) strcat(msg,"AUTO_DVB_T_T2     ",1000);
      else if  (prop->dd_mode.auto_detect     ==     2) strcat(msg,"AUTO_DVB_S_S2     ",1000);
      else if  (prop->dd_mode.auto_detect     ==     3) strcat(msg,"AUTO_DVB_S_S2_DSS ",1000);
      else                                             sprintf(msg,1000,"%s%d", msg, prop->dd_mode.auto_detect);
     break;
    #endif /*     Si2168B_DD_MODE_PROP */
    #ifdef        Si2168B_DD_PER_RESOL_PROP
     case         Si2168B_DD_PER_RESOL_PROP_CODE:
      sprintf(msg,1000,"DD_PER_RESOL");
       strcat(msg,separator,1000); strcat(msg,"-EXP ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_per_resol.exp);
       strcat(msg,separator,1000); strcat(msg,"-MANT ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_per_resol.mant);
     break;
    #endif /*     Si2168B_DD_PER_RESOL_PROP */
    #ifdef        Si2168B_DD_RSQ_BER_THRESHOLD_PROP
     case         Si2168B_DD_RSQ_BER_THRESHOLD_PROP_CODE:
      sprintf(msg,1000,"DD_RSQ_BER_THRESHOLD");
       strcat(msg,separator,1000); strcat(msg,"-EXP ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_rsq_ber_threshold.exp);
       strcat(msg,separator,1000); strcat(msg,"-MANT ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_rsq_ber_threshold.mant);
     break;
    #endif /*     Si2168B_DD_RSQ_BER_THRESHOLD_PROP */
    #ifdef        Si2168B_DD_SSI_SQI_PARAM_PROP
     case         Si2168B_DD_SSI_SQI_PARAM_PROP_CODE:
      sprintf(msg,1000,"DD_SSI_SQI_PARAM");
       strcat(msg,separator,1000); strcat(msg,"-SQI_AVERAGE ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ssi_sqi_param.sqi_average);
     break;
    #endif /*     Si2168B_DD_SSI_SQI_PARAM_PROP */
    #ifdef        Si2168B_DD_TS_FREQ_PROP
     case         Si2168B_DD_TS_FREQ_PROP_CODE:
      sprintf(msg,1000,"DD_TS_FREQ");
       strcat(msg,separator,1000); strcat(msg,"-REQ_FREQ_10KHZ ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_freq.req_freq_10khz);
     break;
    #endif /*     Si2168B_DD_TS_FREQ_PROP */
    #ifdef        Si2168B_DD_TS_MODE_PROP
     case         Si2168B_DD_TS_MODE_PROP_CODE:
      sprintf(msg,1000,"DD_TS_MODE");
       strcat(msg,separator,1000); strcat(msg,"-MODE ",1000);
           if  (prop->dd_ts_mode.mode               ==     0) strcat(msg,"TRISTATE ",1000);
      else if  (prop->dd_ts_mode.mode               ==     1) strcat(msg,"OFF      ",1000);
      else if  (prop->dd_ts_mode.mode               ==     3) strcat(msg,"SERIAL   ",1000);
      else if  (prop->dd_ts_mode.mode               ==     6) strcat(msg,"PARALLEL ",1000);
      else if  (prop->dd_ts_mode.mode               ==     7) strcat(msg,"GPIF     ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->dd_ts_mode.mode);
       strcat(msg,separator,1000); strcat(msg,"-CLOCK ",1000);
           if  (prop->dd_ts_mode.clock              ==     0) strcat(msg,"AUTO_FIXED ",1000);
      else if  (prop->dd_ts_mode.clock              ==     1) strcat(msg,"AUTO_ADAPT ",1000);
      else if  (prop->dd_ts_mode.clock              ==     2) strcat(msg,"MANUAL     ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->dd_ts_mode.clock);
       strcat(msg,separator,1000); strcat(msg,"-CLK_GAPPED_EN ",1000);
           if  (prop->dd_ts_mode.clk_gapped_en      ==     0) strcat(msg,"DISABLED ",1000);
      else if  (prop->dd_ts_mode.clk_gapped_en      ==     1) strcat(msg,"ENABLED  ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->dd_ts_mode.clk_gapped_en);
       strcat(msg,separator,1000); strcat(msg,"-TS_ERR_POLARITY ",1000);
           if  (prop->dd_ts_mode.ts_err_polarity    ==     0) strcat(msg,"NOT_INVERTED ",1000);
      else if  (prop->dd_ts_mode.ts_err_polarity    ==     1) strcat(msg,"INVERTED     ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->dd_ts_mode.ts_err_polarity);
       strcat(msg,separator,1000); strcat(msg,"-SPECIAL ",1000);
           if  (prop->dd_ts_mode.special            ==     0) strcat(msg,"FULL_TS        ",1000);
      else if  (prop->dd_ts_mode.special            ==     1) strcat(msg,"DATAS_TRISTATE ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->dd_ts_mode.special);
       strcat(msg,separator,1000); strcat(msg,"-TS_FREQ_RESOLUTION ",1000);
           if  (prop->dd_ts_mode.ts_freq_resolution ==     0) strcat(msg,"NORMAL ",1000);
      else if  (prop->dd_ts_mode.ts_freq_resolution ==     1) strcat(msg,"FINE   ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->dd_ts_mode.ts_freq_resolution);
     break;
    #endif /*     Si2168B_DD_TS_MODE_PROP */
    #ifdef        Si2168B_DD_TS_SERIAL_DIFF_PROP
     case         Si2168B_DD_TS_SERIAL_DIFF_PROP_CODE:
      sprintf(msg,1000,"DD_TS_SERIAL_DIFF");
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA1_STRENGTH ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_serial_diff.ts_data1_strength);
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA1_SHAPE ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_serial_diff.ts_data1_shape);
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA2_STRENGTH ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_serial_diff.ts_data2_strength);
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA2_SHAPE ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_serial_diff.ts_data2_shape);
       strcat(msg,separator,1000); strcat(msg,"-TS_CLKB_ON_DATA1 ",1000);
           if  (prop->dd_ts_serial_diff.ts_clkb_on_data1   ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ts_serial_diff.ts_clkb_on_data1   ==     1) strcat(msg,"ENABLE  ",1000);
      else                                                          sprintf(msg,1000,"%s%d", msg, prop->dd_ts_serial_diff.ts_clkb_on_data1);
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA0B_ON_DATA2 ",1000);
           if  (prop->dd_ts_serial_diff.ts_data0b_on_data2 ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->dd_ts_serial_diff.ts_data0b_on_data2 ==     1) strcat(msg,"ENABLE  ",1000);
      else                                                          sprintf(msg,1000,"%s%d", msg, prop->dd_ts_serial_diff.ts_data0b_on_data2);
     break;
    #endif /*     Si2168B_DD_TS_SERIAL_DIFF_PROP */
    #ifdef        Si2168B_DD_TS_SETUP_PAR_PROP
     case         Si2168B_DD_TS_SETUP_PAR_PROP_CODE:
      sprintf(msg,1000,"DD_TS_SETUP_PAR");
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA_STRENGTH ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_par.ts_data_strength);
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA_SHAPE ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_par.ts_data_shape);
       strcat(msg,separator,1000); strcat(msg,"-TS_CLK_STRENGTH ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_par.ts_clk_strength);
       strcat(msg,separator,1000); strcat(msg,"-TS_CLK_SHAPE ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_par.ts_clk_shape);
       strcat(msg,separator,1000); strcat(msg,"-TS_CLK_INVERT ",1000);
           if  (prop->dd_ts_setup_par.ts_clk_invert    ==     0) strcat(msg,"NOT_INVERTED ",1000);
      else if  (prop->dd_ts_setup_par.ts_clk_invert    ==     1) strcat(msg,"INVERTED     ",1000);
      else                                                      sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_par.ts_clk_invert);
       strcat(msg,separator,1000); strcat(msg,"-TS_CLK_SHIFT ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_par.ts_clk_shift);
     break;
    #endif /*     Si2168B_DD_TS_SETUP_PAR_PROP */
    #ifdef        Si2168B_DD_TS_SETUP_SER_PROP
     case         Si2168B_DD_TS_SETUP_SER_PROP_CODE:
      sprintf(msg,1000,"DD_TS_SETUP_SER");
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA_STRENGTH ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_ser.ts_data_strength);
       strcat(msg,separator,1000); strcat(msg,"-TS_DATA_SHAPE ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_ser.ts_data_shape);
       strcat(msg,separator,1000); strcat(msg,"-TS_CLK_STRENGTH ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_ser.ts_clk_strength);
       strcat(msg,separator,1000); strcat(msg,"-TS_CLK_SHAPE ",1000); sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_ser.ts_clk_shape);
       strcat(msg,separator,1000); strcat(msg,"-TS_CLK_INVERT ",1000);
           if  (prop->dd_ts_setup_ser.ts_clk_invert    ==     0) strcat(msg,"NOT_INVERTED ",1000);
      else if  (prop->dd_ts_setup_ser.ts_clk_invert    ==     1) strcat(msg,"INVERTED     ",1000);
      else                                                      sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_ser.ts_clk_invert);
       strcat(msg,separator,1000); strcat(msg,"-TS_SYNC_DURATION ",1000);
           if  (prop->dd_ts_setup_ser.ts_sync_duration ==     0) strcat(msg,"FIRST_BYTE ",1000);
      else if  (prop->dd_ts_setup_ser.ts_sync_duration ==     1) strcat(msg,"FIRST_BIT  ",1000);
      else                                                      sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_ser.ts_sync_duration);
       strcat(msg,separator,1000); strcat(msg,"-TS_BYTE_ORDER ",1000);
           if  (prop->dd_ts_setup_ser.ts_byte_order    ==     0) strcat(msg,"MSB_FIRST ",1000);
      else if  (prop->dd_ts_setup_ser.ts_byte_order    ==     1) strcat(msg,"LSB_FIRST ",1000);
      else                                                      sprintf(msg,1000,"%s%d", msg, prop->dd_ts_setup_ser.ts_byte_order);
     break;
    #endif /*     Si2168B_DD_TS_SETUP_SER_PROP */

    #ifdef        Si2168B_DVBC_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBC_ADC_CREST_FACTOR_PROP_CODE:
      sprintf(msg,1000,"DVBC_ADC_CREST_FACTOR");
       strcat(msg,separator,1000); strcat(msg,"-CREST_FACTOR ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbc_adc_crest_factor.crest_factor);
     break;
    #endif /*     Si2168B_DVBC_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBC_AFC_RANGE_PROP
     case         Si2168B_DVBC_AFC_RANGE_PROP_CODE:
      sprintf(msg,1000,"DVBC_AFC_RANGE");
       strcat(msg,separator,1000); strcat(msg,"-RANGE_KHZ ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbc_afc_range.range_khz);
     break;
    #endif /*     Si2168B_DVBC_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBC_CONSTELLATION_PROP
     case         Si2168B_DVBC_CONSTELLATION_PROP_CODE:
      sprintf(msg,1000,"DVBC_CONSTELLATION");
       strcat(msg,separator,1000); strcat(msg,"-CONSTELLATION ",1000);
           if  (prop->dvbc_constellation.constellation ==     0) strcat(msg,"AUTO   ",1000);
      else if  (prop->dvbc_constellation.constellation ==     7) strcat(msg,"QAM16  ",1000);
      else if  (prop->dvbc_constellation.constellation ==     8) strcat(msg,"QAM32  ",1000);
      else if  (prop->dvbc_constellation.constellation ==     9) strcat(msg,"QAM64  ",1000);
      else if  (prop->dvbc_constellation.constellation ==    10) strcat(msg,"QAM128 ",1000);
      else if  (prop->dvbc_constellation.constellation ==    11) strcat(msg,"QAM256 ",1000);
      else                                                      sprintf(msg,1000,"%s%d", msg, prop->dvbc_constellation.constellation);
     break;
    #endif /*     Si2168B_DVBC_CONSTELLATION_PROP */
    #ifdef        Si2168B_DVBC_SYMBOL_RATE_PROP
     case         Si2168B_DVBC_SYMBOL_RATE_PROP_CODE:
      sprintf(msg,1000,"DVBC_SYMBOL_RATE");
       strcat(msg,separator,1000); strcat(msg,"-RATE ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbc_symbol_rate.rate);
     break;
    #endif /*     Si2168B_DVBC_SYMBOL_RATE_PROP */


    #ifdef        Si2168B_DVBT2_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBT2_ADC_CREST_FACTOR_PROP_CODE:
      sprintf(msg,1000,"DVBT2_ADC_CREST_FACTOR");
       strcat(msg,separator,1000); strcat(msg,"-CREST_FACTOR ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbt2_adc_crest_factor.crest_factor);
     break;
    #endif /*     Si2168B_DVBT2_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBT2_AFC_RANGE_PROP
     case         Si2168B_DVBT2_AFC_RANGE_PROP_CODE:
      sprintf(msg,1000,"DVBT2_AFC_RANGE");
       strcat(msg,separator,1000); strcat(msg,"-RANGE_KHZ ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbt2_afc_range.range_khz);
     break;
    #endif /*     Si2168B_DVBT2_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBT2_FEF_TUNER_PROP
     case         Si2168B_DVBT2_FEF_TUNER_PROP_CODE:
      sprintf(msg,1000,"DVBT2_FEF_TUNER");
       strcat(msg,separator,1000); strcat(msg,"-TUNER_DELAY ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbt2_fef_tuner.tuner_delay);
       strcat(msg,separator,1000); strcat(msg,"-TUNER_FREEZE_TIME ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbt2_fef_tuner.tuner_freeze_time);
       strcat(msg,separator,1000); strcat(msg,"-TUNER_UNFREEZE_TIME ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbt2_fef_tuner.tuner_unfreeze_time);
     break;
    #endif /*     Si2168B_DVBT2_FEF_TUNER_PROP */
    #ifdef        Si2168B_DVBT2_MODE_PROP
     case         Si2168B_DVBT2_MODE_PROP_CODE:
      sprintf(msg,1000,"DVBT2_MODE");
       strcat(msg,separator,1000); strcat(msg,"-LOCK_MODE ",1000);
           if  (prop->dvbt2_mode.lock_mode ==     0) strcat(msg,"ANY       ",1000);
      else if  (prop->dvbt2_mode.lock_mode ==     1) strcat(msg,"BASE_ONLY ",1000);
      else if  (prop->dvbt2_mode.lock_mode ==     2) strcat(msg,"LITE_ONLY ",1000);
      else if  (prop->dvbt2_mode.lock_mode ==     3) strcat(msg,"RESERVED  ",1000);
      else                                          sprintf(msg,1000,"%s%d", msg, prop->dvbt2_mode.lock_mode);
     break;
    #endif /*     Si2168B_DVBT2_MODE_PROP */

    #ifdef        Si2168B_DVBT_ADC_CREST_FACTOR_PROP
     case         Si2168B_DVBT_ADC_CREST_FACTOR_PROP_CODE:
      sprintf(msg,1000,"DVBT_ADC_CREST_FACTOR");
       strcat(msg,separator,1000); strcat(msg,"-CREST_FACTOR ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbt_adc_crest_factor.crest_factor);
     break;
    #endif /*     Si2168B_DVBT_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_DVBT_AFC_RANGE_PROP
     case         Si2168B_DVBT_AFC_RANGE_PROP_CODE:
      sprintf(msg,1000,"DVBT_AFC_RANGE");
       strcat(msg,separator,1000); strcat(msg,"-RANGE_KHZ ",1000); sprintf(msg,1000,"%s%d", msg, prop->dvbt_afc_range.range_khz);
     break;
    #endif /*     Si2168B_DVBT_AFC_RANGE_PROP */
    #ifdef        Si2168B_DVBT_HIERARCHY_PROP
     case         Si2168B_DVBT_HIERARCHY_PROP_CODE:
      sprintf(msg,1000,"DVBT_HIERARCHY");
       strcat(msg,separator,1000); strcat(msg,"-STREAM ",1000);
           if  (prop->dvbt_hierarchy.stream ==     0) strcat(msg,"HP ",1000);
      else if  (prop->dvbt_hierarchy.stream ==     1) strcat(msg,"LP ",1000);
      else                                           sprintf(msg,1000,"%s%d", msg, prop->dvbt_hierarchy.stream);
     break;
    #endif /*     Si2168B_DVBT_HIERARCHY_PROP */

    #ifdef        Si2168B_MASTER_IEN_PROP
     case         Si2168B_MASTER_IEN_PROP_CODE:
      sprintf(msg,1000,"MASTER_IEN");
       strcat(msg,separator,1000); strcat(msg,"-DDIEN ",1000);
           if  (prop->master_ien.ddien   ==     0) strcat(msg,"OFF ",1000);
      else if  (prop->master_ien.ddien   ==     1) strcat(msg,"ON  ",1000);
      else                                        sprintf(msg,1000,"%s%d", msg, prop->master_ien.ddien);
       strcat(msg,separator,1000); strcat(msg,"-SCANIEN ",1000);
           if  (prop->master_ien.scanien ==     0) strcat(msg,"OFF ",1000);
      else if  (prop->master_ien.scanien ==     1) strcat(msg,"ON  ",1000);
      else                                        sprintf(msg,1000,"%s%d", msg, prop->master_ien.scanien);
       strcat(msg,separator,1000); strcat(msg,"-ERRIEN ",1000);
           if  (prop->master_ien.errien  ==     0) strcat(msg,"OFF ",1000);
      else if  (prop->master_ien.errien  ==     1) strcat(msg,"ON  ",1000);
      else                                        sprintf(msg,1000,"%s%d", msg, prop->master_ien.errien);
       strcat(msg,separator,1000); strcat(msg,"-CTSIEN ",1000);
           if  (prop->master_ien.ctsien  ==     0) strcat(msg,"OFF ",1000);
      else if  (prop->master_ien.ctsien  ==     1) strcat(msg,"ON  ",1000);
      else                                        sprintf(msg,1000,"%s%d", msg, prop->master_ien.ctsien);
     break;
    #endif /*     Si2168B_MASTER_IEN_PROP */
    #ifdef        Si2168B_MCNS_ADC_CREST_FACTOR_PROP
     case         Si2168B_MCNS_ADC_CREST_FACTOR_PROP_CODE:
      sprintf(msg,1000,"MCNS_ADC_CREST_FACTOR");
       strcat(msg,separator,1000); strcat(msg,"-CREST_FACTOR ",1000); sprintf(msg,1000,"%s%d", msg, prop->mcns_adc_crest_factor.crest_factor);
     break;
    #endif /*     Si2168B_MCNS_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168B_MCNS_AFC_RANGE_PROP
     case         Si2168B_MCNS_AFC_RANGE_PROP_CODE:
      sprintf(msg,1000,"MCNS_AFC_RANGE");
       strcat(msg,separator,1000); strcat(msg,"-RANGE_KHZ ",1000); sprintf(msg,1000,"%s%d", msg, prop->mcns_afc_range.range_khz);
     break;
    #endif /*     Si2168B_MCNS_AFC_RANGE_PROP */
    #ifdef        Si2168B_MCNS_CONSTELLATION_PROP
     case         Si2168B_MCNS_CONSTELLATION_PROP_CODE:
      sprintf(msg,1000,"MCNS_CONSTELLATION");
       strcat(msg,separator,1000); strcat(msg,"-CONSTELLATION ",1000);
           if  (prop->mcns_constellation.constellation ==     0) strcat(msg,"AUTO   ",1000);
      else if  (prop->mcns_constellation.constellation ==     9) strcat(msg,"QAM64  ",1000);
      else if  (prop->mcns_constellation.constellation ==    11) strcat(msg,"QAM256 ",1000);
      else                                                      sprintf(msg,1000,"%s%d", msg, prop->mcns_constellation.constellation);
     break;
    #endif /*     Si2168B_MCNS_CONSTELLATION_PROP */
    #ifdef        Si2168B_MCNS_SYMBOL_RATE_PROP
     case         Si2168B_MCNS_SYMBOL_RATE_PROP_CODE:
      sprintf(msg,1000,"MCNS_SYMBOL_RATE");
       strcat(msg,separator,1000); strcat(msg,"-RATE ",1000); sprintf(msg,1000,"%s%d", msg, prop->mcns_symbol_rate.rate);
     break;
    #endif /*     Si2168B_MCNS_SYMBOL_RATE_PROP */

    #ifdef        Si2168B_SCAN_FMAX_PROP
     case         Si2168B_SCAN_FMAX_PROP_CODE:
      sprintf(msg,1000,"SCAN_FMAX");
       strcat(msg,separator,1000); strcat(msg,"-SCAN_FMAX ",1000); sprintf(msg,1000,"%s%d", msg, prop->scan_fmax.scan_fmax);
     break;
    #endif /*     Si2168B_SCAN_FMAX_PROP */
    #ifdef        Si2168B_SCAN_FMIN_PROP
     case         Si2168B_SCAN_FMIN_PROP_CODE:
      sprintf(msg,1000,"SCAN_FMIN");
       strcat(msg,separator,1000); strcat(msg,"-SCAN_FMIN ",1000); sprintf(msg,1000,"%s%d", msg, prop->scan_fmin.scan_fmin);
     break;
    #endif /*     Si2168B_SCAN_FMIN_PROP */
    #ifdef        Si2168B_SCAN_IEN_PROP
     case         Si2168B_SCAN_IEN_PROP_CODE:
      sprintf(msg,1000,"SCAN_IEN");
       strcat(msg,separator,1000); strcat(msg,"-BUZIEN ",1000);
           if  (prop->scan_ien.buzien ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->scan_ien.buzien ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->scan_ien.buzien);
       strcat(msg,separator,1000); strcat(msg,"-REQIEN ",1000);
           if  (prop->scan_ien.reqien ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->scan_ien.reqien ==     1) strcat(msg,"ENABLE  ",1000);
      else                                     sprintf(msg,1000,"%s%d", msg, prop->scan_ien.reqien);
     break;
    #endif /*     Si2168B_SCAN_IEN_PROP */
    #ifdef        Si2168B_SCAN_INT_SENSE_PROP
     case         Si2168B_SCAN_INT_SENSE_PROP_CODE:
      sprintf(msg,1000,"SCAN_INT_SENSE");
       strcat(msg,separator,1000); strcat(msg,"-BUZNEGEN ",1000);
           if  (prop->scan_int_sense.buznegen ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->scan_int_sense.buznegen ==     1) strcat(msg,"ENABLE  ",1000);
      else                                             sprintf(msg,1000,"%s%d", msg, prop->scan_int_sense.buznegen);
       strcat(msg,separator,1000); strcat(msg,"-REQNEGEN ",1000);
           if  (prop->scan_int_sense.reqnegen ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->scan_int_sense.reqnegen ==     1) strcat(msg,"ENABLE  ",1000);
      else                                             sprintf(msg,1000,"%s%d", msg, prop->scan_int_sense.reqnegen);
       strcat(msg,separator,1000); strcat(msg,"-BUZPOSEN ",1000);
           if  (prop->scan_int_sense.buzposen ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->scan_int_sense.buzposen ==     1) strcat(msg,"ENABLE  ",1000);
      else                                             sprintf(msg,1000,"%s%d", msg, prop->scan_int_sense.buzposen);
       strcat(msg,separator,1000); strcat(msg,"-REQPOSEN ",1000);
           if  (prop->scan_int_sense.reqposen ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->scan_int_sense.reqposen ==     1) strcat(msg,"ENABLE  ",1000);
      else                                             sprintf(msg,1000,"%s%d", msg, prop->scan_int_sense.reqposen);
     break;
    #endif /*     Si2168B_SCAN_INT_SENSE_PROP */

    #ifdef        Si2168B_SCAN_SYMB_RATE_MAX_PROP
     case         Si2168B_SCAN_SYMB_RATE_MAX_PROP_CODE:
      sprintf(msg,1000,"SCAN_SYMB_RATE_MAX");
       strcat(msg,separator,1000); strcat(msg,"-SCAN_SYMB_RATE_MAX ",1000); sprintf(msg,1000,"%s%d", msg, prop->scan_symb_rate_max.scan_symb_rate_max);
     break;
    #endif /*     Si2168B_SCAN_SYMB_RATE_MAX_PROP */
    #ifdef        Si2168B_SCAN_SYMB_RATE_MIN_PROP
     case         Si2168B_SCAN_SYMB_RATE_MIN_PROP_CODE:
      sprintf(msg,1000,"SCAN_SYMB_RATE_MIN");
       strcat(msg,separator,1000); strcat(msg,"-SCAN_SYMB_RATE_MIN ",1000); sprintf(msg,1000,"%s%d", msg, prop->scan_symb_rate_min.scan_symb_rate_min);
     break;
    #endif /*     Si2168B_SCAN_SYMB_RATE_MIN_PROP */
    #ifdef        Si2168B_SCAN_TER_CONFIG_PROP
     case         Si2168B_SCAN_TER_CONFIG_PROP_CODE:
      sprintf(msg,1000,"SCAN_TER_CONFIG");
       strcat(msg,separator,1000); strcat(msg,"-MODE ",1000);
           if  (prop->scan_ter_config.mode          ==     0) strcat(msg,"BLIND_SCAN   ",1000);
      else if  (prop->scan_ter_config.mode          ==     1) strcat(msg,"MAPPING_SCAN ",1000);
      else if  (prop->scan_ter_config.mode          ==     2) strcat(msg,"BLIND_LOCK   ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->scan_ter_config.mode);
       strcat(msg,separator,1000); strcat(msg,"-ANALOG_BW ",1000);
           if  (prop->scan_ter_config.analog_bw     ==     1) strcat(msg,"6MHZ ",1000);
      else if  (prop->scan_ter_config.analog_bw     ==     2) strcat(msg,"7MHZ ",1000);
      else if  (prop->scan_ter_config.analog_bw     ==     3) strcat(msg,"8MHZ ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->scan_ter_config.analog_bw);
       strcat(msg,separator,1000); strcat(msg,"-SEARCH_ANALOG ",1000);
           if  (prop->scan_ter_config.search_analog ==     0) strcat(msg,"DISABLE ",1000);
      else if  (prop->scan_ter_config.search_analog ==     1) strcat(msg,"ENABLE  ",1000);
      else                                                   sprintf(msg,1000,"%s%d", msg, prop->scan_ter_config.search_analog);
     break;
    #endif /*     Si2168B_SCAN_TER_CONFIG_PROP */

     default : sprintf(msg,1000,"Unknown property code '0x%06x'\n", prop_code); return ERROR_Si2168B_UNKNOWN_PROPERTY; break;
    }

  return NO_Si2168B_ERROR;
}
/***********************************************************************************************************************
  Si2168B_L1_FillPropertyStringText function
  Use:        property text retrieval function
              Used to retrieve the property text for a selected property.
  Parameter: *api      the Si2168B context
  Parameter: prop_code the property Id (used to know which property to use)
  Parameter: separator the string to use between fields (often either a blank or a newline character)
  Parameter: msg       the string used to store the resulting string
                       It must be declared by the caller with a size of 1000 bytes
  Returns:    NO_Si2168B_ERROR if successful.
 ***********************************************************************************************************************/
void          Si2168B_L1_FillPropertyStringText(L1_Si2168B_Context *api, unsigned int prop_code, const char *separator, char *msg) {
  Si2168B_L1_PropertyText (api->prop, prop_code, separator, msg);
}
/***********************************************************************************************************************
  Si2168B_L1_GetPropertyString function
  Use:        current property text retrieval function
              Used to retrieve the property value from the hardware then retrieve the corresponding property text.
  Parameter: *api      the Si2168B context
  Parameter: prop_code the property Id (used to know which property to use)
  Parameter: separator the string to use between fields (often either a blank or a newline character)
  Parameter: msg       the string used to store the resulting string
                       It must be declared by the caller with a size of 1000 bytes
  Returns:    NO_Si2168B_ERROR if successful.
 ***********************************************************************************************************************/
unsigned char Si2168B_L1_GetPropertyString     (L1_Si2168B_Context *api, unsigned int prop_code, const char *separator, char *msg) {
    int res;
    res = Si2168B_L1_GetProperty2(api,prop_code);
    if (res!=NO_Si2168B_ERROR) { sprintf(msg,1000, "%s",Si2168B_L1_API_ERROR_TEXT(res)); return res; }
    Si2168B_L1_PropertyText(api->prop, prop_code, separator, msg);
    return NO_Si2168B_ERROR;
  }
#endif /* Si2168B_GET_PROPERTY_STRING */

