/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 1 API
   API properties definitions
   FILE: Si2168_L1_Properties.c
   Supported IC : Si2168
   Compiled for ROM 2 firmware 2_0_build_11
   Revision: REVISION_NAME
   Tag:  V2.6.0
   Date: September 24 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
/* Change log:

 As from V2.5.0:
   Adding Si2168_SCAN_SAT_UNICABLE_BW_PROP property
    (allows controlling the chunk size during SAT blindscan, to be compatible with all Unicable equipments)

 As from V2.4.6:
    Adding Si2168_DD_SSI_SQI_PARAM_PROP property
    (allows controlling the number of samples for BER sliding window computing)

 As from V2.4.2:
    separator parameter type changed to 'const char *'
****************************************************************************************/
#define   Si2168_COMMAND_PROTOTYPES

#include "Si2168_Platform_Definition.h"

/***********************************************************************************************************************
  Si2168_L1_SetProperty function
  Use:        property set function
              Used to call L1_SET_PROPERTY with the property Id and data provided.
  Parameter: *api     the Si2168 context
  Parameter: prop     the property Id
  Parameter: data     the property bytes
  Returns:    0 if no error, an error code otherwise
 ***********************************************************************************************************************/
unsigned char Si2168_L1_SetProperty(L1_Si2168_Context *api, unsigned int prop, int  data)  {
    unsigned char  reserved          = 0;
    return Si2168_L1_SET_PROPERTY (api, reserved, prop, data);
}

/***********************************************************************************************************************
  Si2168_L1_GetProperty function
  Use:        property get function
              Used to call L1_GET_PROPERTY with the property Id provided.
  Parameter: *api     the Si2168 context
  Parameter: prop     the property Id
  Parameter: *data    a buffer to store the property bytes into
  Returns:    0 if no error, an error code otherwise
 ***********************************************************************************************************************/
unsigned char    Si2168_L1_GetProperty(L1_Si2168_Context *api, unsigned int prop, int *data) {
    unsigned char  reserved          = 0;
    unsigned char res;
    res = Si2168_L1_GET_PROPERTY (api, reserved, prop);
    *data = api->rsp->get_property.data;
    return res;
}

/* _set_property2_insertion_start */

  /* --------------------------------------------*/
  /* SET_PROPERTY2 FUNCTION                      */
  /* --------------------------------------------*/
unsigned char Si2168_L1_SetProperty2(L1_Si2168_Context *api, unsigned int prop_code) {
    int data;
#ifdef    Si2168_GET_PROPERTY_STRING
    char msg[1000];
#endif /* Si2168_GET_PROPERTY_STRING */
    switch (prop_code) {
    #ifdef        Si2168_DD_BER_RESOL_PROP
     case         Si2168_DD_BER_RESOL_PROP_CODE:
       data = (api->prop->dd_ber_resol.exp  & Si2168_DD_BER_RESOL_PROP_EXP_MASK ) << Si2168_DD_BER_RESOL_PROP_EXP_LSB  |
              (api->prop->dd_ber_resol.mant & Si2168_DD_BER_RESOL_PROP_MANT_MASK) << Si2168_DD_BER_RESOL_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168_DD_BER_RESOL_PROP */
    #ifdef        Si2168_DD_CBER_RESOL_PROP
     case         Si2168_DD_CBER_RESOL_PROP_CODE:
       data = (api->prop->dd_cber_resol.exp  & Si2168_DD_CBER_RESOL_PROP_EXP_MASK ) << Si2168_DD_CBER_RESOL_PROP_EXP_LSB  |
              (api->prop->dd_cber_resol.mant & Si2168_DD_CBER_RESOL_PROP_MANT_MASK) << Si2168_DD_CBER_RESOL_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168_DD_CBER_RESOL_PROP */

    #ifdef        Si2168_DD_FER_RESOL_PROP
     case         Si2168_DD_FER_RESOL_PROP_CODE:
       data = (api->prop->dd_fer_resol.exp  & Si2168_DD_FER_RESOL_PROP_EXP_MASK ) << Si2168_DD_FER_RESOL_PROP_EXP_LSB  |
              (api->prop->dd_fer_resol.mant & Si2168_DD_FER_RESOL_PROP_MANT_MASK) << Si2168_DD_FER_RESOL_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168_DD_FER_RESOL_PROP */
    #ifdef        Si2168_DD_IEN_PROP
     case         Si2168_DD_IEN_PROP_CODE:
       data = (api->prop->dd_ien.ien_bit0 & Si2168_DD_IEN_PROP_IEN_BIT0_MASK) << Si2168_DD_IEN_PROP_IEN_BIT0_LSB  |
              (api->prop->dd_ien.ien_bit1 & Si2168_DD_IEN_PROP_IEN_BIT1_MASK) << Si2168_DD_IEN_PROP_IEN_BIT1_LSB  |
              (api->prop->dd_ien.ien_bit2 & Si2168_DD_IEN_PROP_IEN_BIT2_MASK) << Si2168_DD_IEN_PROP_IEN_BIT2_LSB  |
              (api->prop->dd_ien.ien_bit3 & Si2168_DD_IEN_PROP_IEN_BIT3_MASK) << Si2168_DD_IEN_PROP_IEN_BIT3_LSB  |
              (api->prop->dd_ien.ien_bit4 & Si2168_DD_IEN_PROP_IEN_BIT4_MASK) << Si2168_DD_IEN_PROP_IEN_BIT4_LSB  |
              (api->prop->dd_ien.ien_bit5 & Si2168_DD_IEN_PROP_IEN_BIT5_MASK) << Si2168_DD_IEN_PROP_IEN_BIT5_LSB  |
              (api->prop->dd_ien.ien_bit6 & Si2168_DD_IEN_PROP_IEN_BIT6_MASK) << Si2168_DD_IEN_PROP_IEN_BIT6_LSB  |
              (api->prop->dd_ien.ien_bit7 & Si2168_DD_IEN_PROP_IEN_BIT7_MASK) << Si2168_DD_IEN_PROP_IEN_BIT7_LSB ;
     break;
    #endif /*     Si2168_DD_IEN_PROP */
    #ifdef        Si2168_DD_IF_INPUT_FREQ_PROP
     case         Si2168_DD_IF_INPUT_FREQ_PROP_CODE:
       data = (api->prop->dd_if_input_freq.offset & Si2168_DD_IF_INPUT_FREQ_PROP_OFFSET_MASK) << Si2168_DD_IF_INPUT_FREQ_PROP_OFFSET_LSB ;
     break;
    #endif /*     Si2168_DD_IF_INPUT_FREQ_PROP */
    #ifdef        Si2168_DD_INT_SENSE_PROP
     case         Si2168_DD_INT_SENSE_PROP_CODE:
       data = (api->prop->dd_int_sense.neg_bit0 & Si2168_DD_INT_SENSE_PROP_NEG_BIT0_MASK) << Si2168_DD_INT_SENSE_PROP_NEG_BIT0_LSB  |
              (api->prop->dd_int_sense.neg_bit1 & Si2168_DD_INT_SENSE_PROP_NEG_BIT1_MASK) << Si2168_DD_INT_SENSE_PROP_NEG_BIT1_LSB  |
              (api->prop->dd_int_sense.neg_bit2 & Si2168_DD_INT_SENSE_PROP_NEG_BIT2_MASK) << Si2168_DD_INT_SENSE_PROP_NEG_BIT2_LSB  |
              (api->prop->dd_int_sense.neg_bit3 & Si2168_DD_INT_SENSE_PROP_NEG_BIT3_MASK) << Si2168_DD_INT_SENSE_PROP_NEG_BIT3_LSB  |
              (api->prop->dd_int_sense.neg_bit4 & Si2168_DD_INT_SENSE_PROP_NEG_BIT4_MASK) << Si2168_DD_INT_SENSE_PROP_NEG_BIT4_LSB  |
              (api->prop->dd_int_sense.neg_bit5 & Si2168_DD_INT_SENSE_PROP_NEG_BIT5_MASK) << Si2168_DD_INT_SENSE_PROP_NEG_BIT5_LSB  |
              (api->prop->dd_int_sense.neg_bit6 & Si2168_DD_INT_SENSE_PROP_NEG_BIT6_MASK) << Si2168_DD_INT_SENSE_PROP_NEG_BIT6_LSB  |
              (api->prop->dd_int_sense.neg_bit7 & Si2168_DD_INT_SENSE_PROP_NEG_BIT7_MASK) << Si2168_DD_INT_SENSE_PROP_NEG_BIT7_LSB  |
              (api->prop->dd_int_sense.pos_bit0 & Si2168_DD_INT_SENSE_PROP_POS_BIT0_MASK) << Si2168_DD_INT_SENSE_PROP_POS_BIT0_LSB  |
              (api->prop->dd_int_sense.pos_bit1 & Si2168_DD_INT_SENSE_PROP_POS_BIT1_MASK) << Si2168_DD_INT_SENSE_PROP_POS_BIT1_LSB  |
              (api->prop->dd_int_sense.pos_bit2 & Si2168_DD_INT_SENSE_PROP_POS_BIT2_MASK) << Si2168_DD_INT_SENSE_PROP_POS_BIT2_LSB  |
              (api->prop->dd_int_sense.pos_bit3 & Si2168_DD_INT_SENSE_PROP_POS_BIT3_MASK) << Si2168_DD_INT_SENSE_PROP_POS_BIT3_LSB  |
              (api->prop->dd_int_sense.pos_bit4 & Si2168_DD_INT_SENSE_PROP_POS_BIT4_MASK) << Si2168_DD_INT_SENSE_PROP_POS_BIT4_LSB  |
              (api->prop->dd_int_sense.pos_bit5 & Si2168_DD_INT_SENSE_PROP_POS_BIT5_MASK) << Si2168_DD_INT_SENSE_PROP_POS_BIT5_LSB  |
              (api->prop->dd_int_sense.pos_bit6 & Si2168_DD_INT_SENSE_PROP_POS_BIT6_MASK) << Si2168_DD_INT_SENSE_PROP_POS_BIT6_LSB  |
              (api->prop->dd_int_sense.pos_bit7 & Si2168_DD_INT_SENSE_PROP_POS_BIT7_MASK) << Si2168_DD_INT_SENSE_PROP_POS_BIT7_LSB ;
     break;
    #endif /*     Si2168_DD_INT_SENSE_PROP */
    #ifdef        Si2168_DD_MODE_PROP
     case         Si2168_DD_MODE_PROP_CODE:
       data = (api->prop->dd_mode.bw              & Si2168_DD_MODE_PROP_BW_MASK             ) << Si2168_DD_MODE_PROP_BW_LSB  |
              (api->prop->dd_mode.modulation      & Si2168_DD_MODE_PROP_MODULATION_MASK     ) << Si2168_DD_MODE_PROP_MODULATION_LSB  |
              (api->prop->dd_mode.invert_spectrum & Si2168_DD_MODE_PROP_INVERT_SPECTRUM_MASK) << Si2168_DD_MODE_PROP_INVERT_SPECTRUM_LSB  |
              (api->prop->dd_mode.auto_detect     & Si2168_DD_MODE_PROP_AUTO_DETECT_MASK    ) << Si2168_DD_MODE_PROP_AUTO_DETECT_LSB ;
     break;
    #endif /*     Si2168_DD_MODE_PROP */
    #ifdef        Si2168_DD_PER_RESOL_PROP
     case         Si2168_DD_PER_RESOL_PROP_CODE:
       data = (api->prop->dd_per_resol.exp  & Si2168_DD_PER_RESOL_PROP_EXP_MASK ) << Si2168_DD_PER_RESOL_PROP_EXP_LSB  |
              (api->prop->dd_per_resol.mant & Si2168_DD_PER_RESOL_PROP_MANT_MASK) << Si2168_DD_PER_RESOL_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168_DD_PER_RESOL_PROP */
    #ifdef        Si2168_DD_RSQ_BER_THRESHOLD_PROP
     case         Si2168_DD_RSQ_BER_THRESHOLD_PROP_CODE:
       data = (api->prop->dd_rsq_ber_threshold.exp  & Si2168_DD_RSQ_BER_THRESHOLD_PROP_EXP_MASK ) << Si2168_DD_RSQ_BER_THRESHOLD_PROP_EXP_LSB  |
              (api->prop->dd_rsq_ber_threshold.mant & Si2168_DD_RSQ_BER_THRESHOLD_PROP_MANT_MASK) << Si2168_DD_RSQ_BER_THRESHOLD_PROP_MANT_LSB ;
     break;
    #endif /*     Si2168_DD_RSQ_BER_THRESHOLD_PROP */
    #ifdef        Si2168_DD_SSI_SQI_PARAM_PROP
     case         Si2168_DD_SSI_SQI_PARAM_PROP_CODE:
       data = (api->prop->dd_ssi_sqi_param.sqi_average & Si2168_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE_MASK) << Si2168_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE_LSB ;
     break;
    #endif /*     Si2168_DD_SSI_SQI_PARAM_PROP */
    #ifdef        Si2168_DD_TS_FREQ_PROP
     case         Si2168_DD_TS_FREQ_PROP_CODE:
       data = (api->prop->dd_ts_freq.req_freq_10khz & Si2168_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ_MASK) << Si2168_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ_LSB ;
     break;
    #endif /*     Si2168_DD_TS_FREQ_PROP */
    #ifdef        Si2168_DD_TS_MODE_PROP
     case         Si2168_DD_TS_MODE_PROP_CODE:
       data = (api->prop->dd_ts_mode.mode          & Si2168_DD_TS_MODE_PROP_MODE_MASK         ) << Si2168_DD_TS_MODE_PROP_MODE_LSB  |
              (api->prop->dd_ts_mode.clock         & Si2168_DD_TS_MODE_PROP_CLOCK_MASK        ) << Si2168_DD_TS_MODE_PROP_CLOCK_LSB  |
              (api->prop->dd_ts_mode.clk_gapped_en   & Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN_MASK  ) << Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN_LSB  |
              (api->prop->dd_ts_mode.ts_err_polarity & Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY_MASK) << Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY_LSB  |
              (api->prop->dd_ts_mode.special         & Si2168_DD_TS_MODE_PROP_SPECIAL_MASK        ) << Si2168_DD_TS_MODE_PROP_SPECIAL_LSB ;
     break;
    #endif /*     Si2168_DD_TS_MODE_PROP */
    #ifdef        Si2168_DD_TS_SETUP_PAR_PROP
     case         Si2168_DD_TS_SETUP_PAR_PROP_CODE:
       data = (api->prop->dd_ts_setup_par.ts_data_strength & Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH_MASK) << Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH_LSB  |
              (api->prop->dd_ts_setup_par.ts_data_shape    & Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE_MASK   ) << Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE_LSB  |
              (api->prop->dd_ts_setup_par.ts_clk_strength  & Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH_MASK ) << Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH_LSB  |
              (api->prop->dd_ts_setup_par.ts_clk_shape     & Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE_MASK    ) << Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE_LSB  |
              (api->prop->dd_ts_setup_par.ts_clk_invert    & Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_MASK   ) << Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_LSB  |
              (api->prop->dd_ts_setup_par.ts_clk_shift     & Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT_MASK    ) << Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT_LSB ;
     break;
    #endif /*     Si2168_DD_TS_SETUP_PAR_PROP */
    #ifdef        Si2168_DD_TS_SETUP_SER_PROP
     case         Si2168_DD_TS_SETUP_SER_PROP_CODE:
       data = (api->prop->dd_ts_setup_ser.ts_data_strength & Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH_MASK) << Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH_LSB  |
              (api->prop->dd_ts_setup_ser.ts_data_shape    & Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE_MASK   ) << Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE_LSB  |
              (api->prop->dd_ts_setup_ser.ts_clk_strength  & Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH_MASK ) << Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH_LSB  |
              (api->prop->dd_ts_setup_ser.ts_clk_shape     & Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE_MASK    ) << Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE_LSB  |
              (api->prop->dd_ts_setup_ser.ts_clk_invert    & Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_MASK   ) << Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_LSB  |
              (api->prop->dd_ts_setup_ser.ts_sync_duration & Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_MASK) << Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_LSB  |
              (api->prop->dd_ts_setup_ser.ts_byte_order    & Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_MASK   ) << Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_LSB ;
     break;
    #endif /*     Si2168_DD_TS_SETUP_SER_PROP */
    #ifdef        Si2168_DVBC_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBC_ADC_CREST_FACTOR_PROP_CODE:
       data = (api->prop->dvbc_adc_crest_factor.crest_factor & Si2168_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK) << Si2168_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB ;
     break;
    #endif /*     Si2168_DVBC_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBC_AFC_RANGE_PROP
     case         Si2168_DVBC_AFC_RANGE_PROP_CODE:
       data = (api->prop->dvbc_afc_range.range_khz & Si2168_DVBC_AFC_RANGE_PROP_RANGE_KHZ_MASK) << Si2168_DVBC_AFC_RANGE_PROP_RANGE_KHZ_LSB ;
     break;
    #endif /*     Si2168_DVBC_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBC_CONSTELLATION_PROP
     case         Si2168_DVBC_CONSTELLATION_PROP_CODE:
       data = (api->prop->dvbc_constellation.constellation & Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION_MASK) << Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION_LSB ;
     break;
    #endif /*     Si2168_DVBC_CONSTELLATION_PROP */
    #ifdef        Si2168_DVBC_SYMBOL_RATE_PROP
     case         Si2168_DVBC_SYMBOL_RATE_PROP_CODE:
       data = (api->prop->dvbc_symbol_rate.rate & Si2168_DVBC_SYMBOL_RATE_PROP_RATE_MASK) << Si2168_DVBC_SYMBOL_RATE_PROP_RATE_LSB ;
     break;
    #endif /*     Si2168_DVBC_SYMBOL_RATE_PROP */


    #ifdef        Si2168_DVBT2_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CODE:
       data = (api->prop->dvbt2_adc_crest_factor.crest_factor & Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK) << Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB ;
     break;
    #endif /*     Si2168_DVBT2_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBT2_AFC_RANGE_PROP
     case         Si2168_DVBT2_AFC_RANGE_PROP_CODE:
       data = (api->prop->dvbt2_afc_range.range_khz & Si2168_DVBT2_AFC_RANGE_PROP_RANGE_KHZ_MASK) << Si2168_DVBT2_AFC_RANGE_PROP_RANGE_KHZ_LSB ;
     break;
    #endif /*     Si2168_DVBT2_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBT2_FEF_TUNER_PROP
     case         Si2168_DVBT2_FEF_TUNER_PROP_CODE:
       data = (api->prop->dvbt2_fef_tuner.tuner_delay         & Si2168_DVBT2_FEF_TUNER_PROP_TUNER_DELAY_MASK        ) << Si2168_DVBT2_FEF_TUNER_PROP_TUNER_DELAY_LSB  |
              (api->prop->dvbt2_fef_tuner.tuner_freeze_time   & Si2168_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME_MASK  ) << Si2168_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME_LSB  |
              (api->prop->dvbt2_fef_tuner.tuner_unfreeze_time & Si2168_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME_MASK) << Si2168_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME_LSB ;
     break;
    #endif /*     Si2168_DVBT2_FEF_TUNER_PROP */

    #ifdef        Si2168_DVBT_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBT_ADC_CREST_FACTOR_PROP_CODE:
       data = (api->prop->dvbt_adc_crest_factor.crest_factor & Si2168_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK) << Si2168_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB ;
     break;
    #endif /*     Si2168_DVBT_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBT_AFC_RANGE_PROP
     case         Si2168_DVBT_AFC_RANGE_PROP_CODE:
       data = (api->prop->dvbt_afc_range.range_khz & Si2168_DVBT_AFC_RANGE_PROP_RANGE_KHZ_MASK) << Si2168_DVBT_AFC_RANGE_PROP_RANGE_KHZ_LSB ;
     break;
    #endif /*     Si2168_DVBT_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBT_HIERARCHY_PROP
     case         Si2168_DVBT_HIERARCHY_PROP_CODE:
       data = (api->prop->dvbt_hierarchy.stream & Si2168_DVBT_HIERARCHY_PROP_STREAM_MASK) << Si2168_DVBT_HIERARCHY_PROP_STREAM_LSB ;
     break;
    #endif /*     Si2168_DVBT_HIERARCHY_PROP */

    #ifdef        Si2168_MASTER_IEN_PROP
     case         Si2168_MASTER_IEN_PROP_CODE:
       data = (api->prop->master_ien.ddien   & Si2168_MASTER_IEN_PROP_DDIEN_MASK  ) << Si2168_MASTER_IEN_PROP_DDIEN_LSB  |
              (api->prop->master_ien.scanien & Si2168_MASTER_IEN_PROP_SCANIEN_MASK) << Si2168_MASTER_IEN_PROP_SCANIEN_LSB  |
              (api->prop->master_ien.errien  & Si2168_MASTER_IEN_PROP_ERRIEN_MASK ) << Si2168_MASTER_IEN_PROP_ERRIEN_LSB  |
              (api->prop->master_ien.ctsien  & Si2168_MASTER_IEN_PROP_CTSIEN_MASK ) << Si2168_MASTER_IEN_PROP_CTSIEN_LSB ;
     break;
    #endif /*     Si2168_MASTER_IEN_PROP */
    #ifdef        Si2168_SCAN_FMAX_PROP
     case         Si2168_SCAN_FMAX_PROP_CODE:
       data = (api->prop->scan_fmax.scan_fmax & Si2168_SCAN_FMAX_PROP_SCAN_FMAX_MASK) << Si2168_SCAN_FMAX_PROP_SCAN_FMAX_LSB ;
     break;
    #endif /*     Si2168_SCAN_FMAX_PROP */
    #ifdef        Si2168_SCAN_FMIN_PROP
     case         Si2168_SCAN_FMIN_PROP_CODE:
       data = (api->prop->scan_fmin.scan_fmin & Si2168_SCAN_FMIN_PROP_SCAN_FMIN_MASK) << Si2168_SCAN_FMIN_PROP_SCAN_FMIN_LSB ;
     break;
    #endif /*     Si2168_SCAN_FMIN_PROP */
    #ifdef        Si2168_SCAN_IEN_PROP
     case         Si2168_SCAN_IEN_PROP_CODE:
       data = (api->prop->scan_ien.buzien & Si2168_SCAN_IEN_PROP_BUZIEN_MASK) << Si2168_SCAN_IEN_PROP_BUZIEN_LSB  |
              (api->prop->scan_ien.reqien & Si2168_SCAN_IEN_PROP_REQIEN_MASK) << Si2168_SCAN_IEN_PROP_REQIEN_LSB ;
     break;
    #endif /*     Si2168_SCAN_IEN_PROP */
    #ifdef        Si2168_SCAN_INT_SENSE_PROP
     case         Si2168_SCAN_INT_SENSE_PROP_CODE:
       data = (api->prop->scan_int_sense.buznegen & Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN_MASK) << Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN_LSB  |
              (api->prop->scan_int_sense.reqnegen & Si2168_SCAN_INT_SENSE_PROP_REQNEGEN_MASK) << Si2168_SCAN_INT_SENSE_PROP_REQNEGEN_LSB  |
              (api->prop->scan_int_sense.buzposen & Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN_MASK) << Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN_LSB  |
              (api->prop->scan_int_sense.reqposen & Si2168_SCAN_INT_SENSE_PROP_REQPOSEN_MASK) << Si2168_SCAN_INT_SENSE_PROP_REQPOSEN_LSB ;
     break;
    #endif /*     Si2168_SCAN_INT_SENSE_PROP */

    #ifdef        Si2168_SCAN_SYMB_RATE_MAX_PROP
     case         Si2168_SCAN_SYMB_RATE_MAX_PROP_CODE:
       data = (api->prop->scan_symb_rate_max.scan_symb_rate_max & Si2168_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX_MASK) << Si2168_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX_LSB ;
     break;
    #endif /*     Si2168_SCAN_SYMB_RATE_MAX_PROP */
    #ifdef        Si2168_SCAN_SYMB_RATE_MIN_PROP
     case         Si2168_SCAN_SYMB_RATE_MIN_PROP_CODE:
       data = (api->prop->scan_symb_rate_min.scan_symb_rate_min & Si2168_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN_MASK) << Si2168_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN_LSB ;
     break;
    #endif /*     Si2168_SCAN_SYMB_RATE_MIN_PROP */
    #ifdef        Si2168_SCAN_TER_CONFIG_PROP
     case         Si2168_SCAN_TER_CONFIG_PROP_CODE:
       data = (api->prop->scan_ter_config.mode          & Si2168_SCAN_TER_CONFIG_PROP_MODE_MASK         ) << Si2168_SCAN_TER_CONFIG_PROP_MODE_LSB  |
              (api->prop->scan_ter_config.analog_bw     & Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW_MASK    ) << Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW_LSB  |
              (api->prop->scan_ter_config.search_analog & Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_MASK) << Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_LSB ;
     break;
    #endif /*     Si2168_SCAN_TER_CONFIG_PROP */

   default : break;
    }
#ifdef    Si2168_GET_PROPERTY_STRING
    Si2168_L1_FillPropertyStringText(api, prop_code, (char*)" ", msg);
    SiTRACE("%s\n",msg);
#endif /* Si2168_GET_PROPERTY_STRING */
    return Si2168_L1_SetProperty(api, prop_code & 0xffff , data);
  }

/* _set_property2_insertion_point */

/* _get_property2_insertion_start */

  /* --------------------------------------------*/
  /* GET_PROPERTY2 FUNCTION                       */
  /* --------------------------------------------*/
unsigned char Si2168_L1_GetProperty2(L1_Si2168_Context *api, unsigned int prop_code) {
    unsigned char res;
	int data;
    res = Si2168_L1_GetProperty(api,prop_code & 0xffff,&data);
    if (res!=NO_Si2168_ERROR) return res;
    switch (prop_code) {
    #ifdef        Si2168_DD_BER_RESOL_PROP
     case         Si2168_DD_BER_RESOL_PROP_CODE:
               api->prop->dd_ber_resol.exp  = (data >> Si2168_DD_BER_RESOL_PROP_EXP_LSB ) & Si2168_DD_BER_RESOL_PROP_EXP_MASK;
               api->prop->dd_ber_resol.mant = (data >> Si2168_DD_BER_RESOL_PROP_MANT_LSB) & Si2168_DD_BER_RESOL_PROP_MANT_MASK;
     break;
    #endif /*     Si2168_DD_BER_RESOL_PROP */
    #ifdef        Si2168_DD_CBER_RESOL_PROP
     case         Si2168_DD_CBER_RESOL_PROP_CODE:
               api->prop->dd_cber_resol.exp  = (data >> Si2168_DD_CBER_RESOL_PROP_EXP_LSB ) & Si2168_DD_CBER_RESOL_PROP_EXP_MASK;
               api->prop->dd_cber_resol.mant = (data >> Si2168_DD_CBER_RESOL_PROP_MANT_LSB) & Si2168_DD_CBER_RESOL_PROP_MANT_MASK;
     break;
    #endif /*     Si2168_DD_CBER_RESOL_PROP */

    #ifdef        Si2168_DD_FER_RESOL_PROP
     case         Si2168_DD_FER_RESOL_PROP_CODE:
               api->prop->dd_fer_resol.exp  = (data >> Si2168_DD_FER_RESOL_PROP_EXP_LSB ) & Si2168_DD_FER_RESOL_PROP_EXP_MASK;
               api->prop->dd_fer_resol.mant = (data >> Si2168_DD_FER_RESOL_PROP_MANT_LSB) & Si2168_DD_FER_RESOL_PROP_MANT_MASK;
     break;
    #endif /*     Si2168_DD_FER_RESOL_PROP */
    #ifdef        Si2168_DD_IEN_PROP
     case         Si2168_DD_IEN_PROP_CODE:
               api->prop->dd_ien.ien_bit0 = (data >> Si2168_DD_IEN_PROP_IEN_BIT0_LSB) & Si2168_DD_IEN_PROP_IEN_BIT0_MASK;
               api->prop->dd_ien.ien_bit1 = (data >> Si2168_DD_IEN_PROP_IEN_BIT1_LSB) & Si2168_DD_IEN_PROP_IEN_BIT1_MASK;
               api->prop->dd_ien.ien_bit2 = (data >> Si2168_DD_IEN_PROP_IEN_BIT2_LSB) & Si2168_DD_IEN_PROP_IEN_BIT2_MASK;
               api->prop->dd_ien.ien_bit3 = (data >> Si2168_DD_IEN_PROP_IEN_BIT3_LSB) & Si2168_DD_IEN_PROP_IEN_BIT3_MASK;
               api->prop->dd_ien.ien_bit4 = (data >> Si2168_DD_IEN_PROP_IEN_BIT4_LSB) & Si2168_DD_IEN_PROP_IEN_BIT4_MASK;
               api->prop->dd_ien.ien_bit5 = (data >> Si2168_DD_IEN_PROP_IEN_BIT5_LSB) & Si2168_DD_IEN_PROP_IEN_BIT5_MASK;
               api->prop->dd_ien.ien_bit6 = (data >> Si2168_DD_IEN_PROP_IEN_BIT6_LSB) & Si2168_DD_IEN_PROP_IEN_BIT6_MASK;
               api->prop->dd_ien.ien_bit7 = (data >> Si2168_DD_IEN_PROP_IEN_BIT7_LSB) & Si2168_DD_IEN_PROP_IEN_BIT7_MASK;
     break;
    #endif /*     Si2168_DD_IEN_PROP */
    #ifdef        Si2168_DD_IF_INPUT_FREQ_PROP
     case         Si2168_DD_IF_INPUT_FREQ_PROP_CODE:
               api->prop->dd_if_input_freq.offset = (data >> Si2168_DD_IF_INPUT_FREQ_PROP_OFFSET_LSB) & Si2168_DD_IF_INPUT_FREQ_PROP_OFFSET_MASK;
     break;
    #endif /*     Si2168_DD_IF_INPUT_FREQ_PROP */
    #ifdef        Si2168_DD_INT_SENSE_PROP
     case         Si2168_DD_INT_SENSE_PROP_CODE:
               api->prop->dd_int_sense.neg_bit0 = (data >> Si2168_DD_INT_SENSE_PROP_NEG_BIT0_LSB) & Si2168_DD_INT_SENSE_PROP_NEG_BIT0_MASK;
               api->prop->dd_int_sense.neg_bit1 = (data >> Si2168_DD_INT_SENSE_PROP_NEG_BIT1_LSB) & Si2168_DD_INT_SENSE_PROP_NEG_BIT1_MASK;
               api->prop->dd_int_sense.neg_bit2 = (data >> Si2168_DD_INT_SENSE_PROP_NEG_BIT2_LSB) & Si2168_DD_INT_SENSE_PROP_NEG_BIT2_MASK;
               api->prop->dd_int_sense.neg_bit3 = (data >> Si2168_DD_INT_SENSE_PROP_NEG_BIT3_LSB) & Si2168_DD_INT_SENSE_PROP_NEG_BIT3_MASK;
               api->prop->dd_int_sense.neg_bit4 = (data >> Si2168_DD_INT_SENSE_PROP_NEG_BIT4_LSB) & Si2168_DD_INT_SENSE_PROP_NEG_BIT4_MASK;
               api->prop->dd_int_sense.neg_bit5 = (data >> Si2168_DD_INT_SENSE_PROP_NEG_BIT5_LSB) & Si2168_DD_INT_SENSE_PROP_NEG_BIT5_MASK;
               api->prop->dd_int_sense.neg_bit6 = (data >> Si2168_DD_INT_SENSE_PROP_NEG_BIT6_LSB) & Si2168_DD_INT_SENSE_PROP_NEG_BIT6_MASK;
               api->prop->dd_int_sense.neg_bit7 = (data >> Si2168_DD_INT_SENSE_PROP_NEG_BIT7_LSB) & Si2168_DD_INT_SENSE_PROP_NEG_BIT7_MASK;
               api->prop->dd_int_sense.pos_bit0 = (data >> Si2168_DD_INT_SENSE_PROP_POS_BIT0_LSB) & Si2168_DD_INT_SENSE_PROP_POS_BIT0_MASK;
               api->prop->dd_int_sense.pos_bit1 = (data >> Si2168_DD_INT_SENSE_PROP_POS_BIT1_LSB) & Si2168_DD_INT_SENSE_PROP_POS_BIT1_MASK;
               api->prop->dd_int_sense.pos_bit2 = (data >> Si2168_DD_INT_SENSE_PROP_POS_BIT2_LSB) & Si2168_DD_INT_SENSE_PROP_POS_BIT2_MASK;
               api->prop->dd_int_sense.pos_bit3 = (data >> Si2168_DD_INT_SENSE_PROP_POS_BIT3_LSB) & Si2168_DD_INT_SENSE_PROP_POS_BIT3_MASK;
               api->prop->dd_int_sense.pos_bit4 = (data >> Si2168_DD_INT_SENSE_PROP_POS_BIT4_LSB) & Si2168_DD_INT_SENSE_PROP_POS_BIT4_MASK;
               api->prop->dd_int_sense.pos_bit5 = (data >> Si2168_DD_INT_SENSE_PROP_POS_BIT5_LSB) & Si2168_DD_INT_SENSE_PROP_POS_BIT5_MASK;
               api->prop->dd_int_sense.pos_bit6 = (data >> Si2168_DD_INT_SENSE_PROP_POS_BIT6_LSB) & Si2168_DD_INT_SENSE_PROP_POS_BIT6_MASK;
               api->prop->dd_int_sense.pos_bit7 = (data >> Si2168_DD_INT_SENSE_PROP_POS_BIT7_LSB) & Si2168_DD_INT_SENSE_PROP_POS_BIT7_MASK;
     break;
    #endif /*     Si2168_DD_INT_SENSE_PROP */
    #ifdef        Si2168_DD_MODE_PROP
     case         Si2168_DD_MODE_PROP_CODE:
               api->prop->dd_mode.bw              = (data >> Si2168_DD_MODE_PROP_BW_LSB             ) & Si2168_DD_MODE_PROP_BW_MASK;
               api->prop->dd_mode.modulation      = (data >> Si2168_DD_MODE_PROP_MODULATION_LSB     ) & Si2168_DD_MODE_PROP_MODULATION_MASK;
               api->prop->dd_mode.invert_spectrum = (data >> Si2168_DD_MODE_PROP_INVERT_SPECTRUM_LSB) & Si2168_DD_MODE_PROP_INVERT_SPECTRUM_MASK;
               api->prop->dd_mode.auto_detect     = (data >> Si2168_DD_MODE_PROP_AUTO_DETECT_LSB    ) & Si2168_DD_MODE_PROP_AUTO_DETECT_MASK;
     break;
    #endif /*     Si2168_DD_MODE_PROP */
    #ifdef        Si2168_DD_PER_RESOL_PROP
     case         Si2168_DD_PER_RESOL_PROP_CODE:
               api->prop->dd_per_resol.exp  = (data >> Si2168_DD_PER_RESOL_PROP_EXP_LSB ) & Si2168_DD_PER_RESOL_PROP_EXP_MASK;
               api->prop->dd_per_resol.mant = (data >> Si2168_DD_PER_RESOL_PROP_MANT_LSB) & Si2168_DD_PER_RESOL_PROP_MANT_MASK;
     break;
    #endif /*     Si2168_DD_PER_RESOL_PROP */
    #ifdef        Si2168_DD_RSQ_BER_THRESHOLD_PROP
     case         Si2168_DD_RSQ_BER_THRESHOLD_PROP_CODE:
               api->prop->dd_rsq_ber_threshold.exp  = (data >> Si2168_DD_RSQ_BER_THRESHOLD_PROP_EXP_LSB ) & Si2168_DD_RSQ_BER_THRESHOLD_PROP_EXP_MASK;
               api->prop->dd_rsq_ber_threshold.mant = (data >> Si2168_DD_RSQ_BER_THRESHOLD_PROP_MANT_LSB) & Si2168_DD_RSQ_BER_THRESHOLD_PROP_MANT_MASK;
     break;
    #endif /*     Si2168_DD_RSQ_BER_THRESHOLD_PROP */
    #ifdef        Si2168_DD_SSI_SQI_PARAM_PROP
     case         Si2168_DD_SSI_SQI_PARAM_PROP_CODE:
               api->prop->dd_ssi_sqi_param.sqi_average = (data >> Si2168_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE_LSB) & Si2168_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE_MASK;
     break;
    #endif /*     Si2168_DD_SSI_SQI_PARAM_PROP */
    #ifdef        Si2168_DD_TS_FREQ_PROP
     case         Si2168_DD_TS_FREQ_PROP_CODE:
               api->prop->dd_ts_freq.req_freq_10khz = (data >> Si2168_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ_LSB) & Si2168_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ_MASK;
     break;
    #endif /*     Si2168_DD_TS_FREQ_PROP */
    #ifdef        Si2168_DD_TS_MODE_PROP
     case         Si2168_DD_TS_MODE_PROP_CODE:
               api->prop->dd_ts_mode.mode          = (data >> Si2168_DD_TS_MODE_PROP_MODE_LSB         ) & Si2168_DD_TS_MODE_PROP_MODE_MASK;
               api->prop->dd_ts_mode.clock         = (data >> Si2168_DD_TS_MODE_PROP_CLOCK_LSB        ) & Si2168_DD_TS_MODE_PROP_CLOCK_MASK;
               api->prop->dd_ts_mode.clk_gapped_en = (data >> Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN_LSB) & Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN_MASK;
               api->prop->dd_ts_mode.ts_err_polarity = (data >> Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY_LSB) & Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY_MASK;
               api->prop->dd_ts_mode.special         = (data >> Si2168_DD_TS_MODE_PROP_SPECIAL_LSB        ) & Si2168_DD_TS_MODE_PROP_SPECIAL_MASK;
     break;
    #endif /*     Si2168_DD_TS_MODE_PROP */
    #ifdef        Si2168_DD_TS_SETUP_PAR_PROP
     case         Si2168_DD_TS_SETUP_PAR_PROP_CODE:
               api->prop->dd_ts_setup_par.ts_data_strength = (data >> Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH_LSB) & Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH_MASK;
               api->prop->dd_ts_setup_par.ts_data_shape    = (data >> Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE_LSB   ) & Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE_MASK;
               api->prop->dd_ts_setup_par.ts_clk_strength  = (data >> Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH_LSB ) & Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH_MASK;
               api->prop->dd_ts_setup_par.ts_clk_shape     = (data >> Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE_LSB    ) & Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE_MASK;
               api->prop->dd_ts_setup_par.ts_clk_invert    = (data >> Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_LSB   ) & Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_MASK;
               api->prop->dd_ts_setup_par.ts_clk_shift     = (data >> Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT_LSB    ) & Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT_MASK;
     break;
    #endif /*     Si2168_DD_TS_SETUP_PAR_PROP */
    #ifdef        Si2168_DD_TS_SETUP_SER_PROP
     case         Si2168_DD_TS_SETUP_SER_PROP_CODE:
               api->prop->dd_ts_setup_ser.ts_data_strength = (data >> Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH_LSB) & Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH_MASK;
               api->prop->dd_ts_setup_ser.ts_data_shape    = (data >> Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE_LSB   ) & Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE_MASK;
               api->prop->dd_ts_setup_ser.ts_clk_strength  = (data >> Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH_LSB ) & Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH_MASK;
               api->prop->dd_ts_setup_ser.ts_clk_shape     = (data >> Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE_LSB    ) & Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE_MASK;
               api->prop->dd_ts_setup_ser.ts_clk_invert    = (data >> Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_LSB   ) & Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_MASK;
               api->prop->dd_ts_setup_ser.ts_sync_duration = (data >> Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_LSB) & Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_MASK;
               api->prop->dd_ts_setup_ser.ts_byte_order    = (data >> Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_LSB   ) & Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_MASK;
     break;
    #endif /*     Si2168_DD_TS_SETUP_SER_PROP */
    #ifdef        Si2168_DVBC_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBC_ADC_CREST_FACTOR_PROP_CODE:
               api->prop->dvbc_adc_crest_factor.crest_factor = (data >> Si2168_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB) & Si2168_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK;
     break;
    #endif /*     Si2168_DVBC_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBC_AFC_RANGE_PROP
     case         Si2168_DVBC_AFC_RANGE_PROP_CODE:
               api->prop->dvbc_afc_range.range_khz = (data >> Si2168_DVBC_AFC_RANGE_PROP_RANGE_KHZ_LSB) & Si2168_DVBC_AFC_RANGE_PROP_RANGE_KHZ_MASK;
     break;
    #endif /*     Si2168_DVBC_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBC_CONSTELLATION_PROP
     case         Si2168_DVBC_CONSTELLATION_PROP_CODE:
               api->prop->dvbc_constellation.constellation = (data >> Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION_LSB) & Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION_MASK;
     break;
    #endif /*     Si2168_DVBC_CONSTELLATION_PROP */
    #ifdef        Si2168_DVBC_SYMBOL_RATE_PROP
     case         Si2168_DVBC_SYMBOL_RATE_PROP_CODE:
               api->prop->dvbc_symbol_rate.rate = (data >> Si2168_DVBC_SYMBOL_RATE_PROP_RATE_LSB) & Si2168_DVBC_SYMBOL_RATE_PROP_RATE_MASK;
     break;
    #endif /*     Si2168_DVBC_SYMBOL_RATE_PROP */


    #ifdef        Si2168_DVBT2_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CODE:
               api->prop->dvbt2_adc_crest_factor.crest_factor = (data >> Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB) & Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK;
     break;
    #endif /*     Si2168_DVBT2_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBT2_AFC_RANGE_PROP
     case         Si2168_DVBT2_AFC_RANGE_PROP_CODE:
               api->prop->dvbt2_afc_range.range_khz = (data >> Si2168_DVBT2_AFC_RANGE_PROP_RANGE_KHZ_LSB) & Si2168_DVBT2_AFC_RANGE_PROP_RANGE_KHZ_MASK;
     break;
    #endif /*     Si2168_DVBT2_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBT2_FEF_TUNER_PROP
     case         Si2168_DVBT2_FEF_TUNER_PROP_CODE:
               api->prop->dvbt2_fef_tuner.tuner_delay         = (data >> Si2168_DVBT2_FEF_TUNER_PROP_TUNER_DELAY_LSB        ) & Si2168_DVBT2_FEF_TUNER_PROP_TUNER_DELAY_MASK;
               api->prop->dvbt2_fef_tuner.tuner_freeze_time   = (data >> Si2168_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME_LSB  ) & Si2168_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME_MASK;
               api->prop->dvbt2_fef_tuner.tuner_unfreeze_time = (data >> Si2168_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME_LSB) & Si2168_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME_MASK;
     break;
    #endif /*     Si2168_DVBT2_FEF_TUNER_PROP */

    #ifdef        Si2168_DVBT_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBT_ADC_CREST_FACTOR_PROP_CODE:
               api->prop->dvbt_adc_crest_factor.crest_factor = (data >> Si2168_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR_LSB) & Si2168_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR_MASK;
     break;
    #endif /*     Si2168_DVBT_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBT_AFC_RANGE_PROP
     case         Si2168_DVBT_AFC_RANGE_PROP_CODE:
               api->prop->dvbt_afc_range.range_khz = (data >> Si2168_DVBT_AFC_RANGE_PROP_RANGE_KHZ_LSB) & Si2168_DVBT_AFC_RANGE_PROP_RANGE_KHZ_MASK;
     break;
    #endif /*     Si2168_DVBT_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBT_HIERARCHY_PROP
     case         Si2168_DVBT_HIERARCHY_PROP_CODE:
               api->prop->dvbt_hierarchy.stream = (data >> Si2168_DVBT_HIERARCHY_PROP_STREAM_LSB) & Si2168_DVBT_HIERARCHY_PROP_STREAM_MASK;
     break;
    #endif /*     Si2168_DVBT_HIERARCHY_PROP */

    #ifdef        Si2168_MASTER_IEN_PROP
     case         Si2168_MASTER_IEN_PROP_CODE:
               api->prop->master_ien.ddien   = (data >> Si2168_MASTER_IEN_PROP_DDIEN_LSB  ) & Si2168_MASTER_IEN_PROP_DDIEN_MASK;
               api->prop->master_ien.scanien = (data >> Si2168_MASTER_IEN_PROP_SCANIEN_LSB) & Si2168_MASTER_IEN_PROP_SCANIEN_MASK;
               api->prop->master_ien.errien  = (data >> Si2168_MASTER_IEN_PROP_ERRIEN_LSB ) & Si2168_MASTER_IEN_PROP_ERRIEN_MASK;
               api->prop->master_ien.ctsien  = (data >> Si2168_MASTER_IEN_PROP_CTSIEN_LSB ) & Si2168_MASTER_IEN_PROP_CTSIEN_MASK;
     break;
    #endif /*     Si2168_MASTER_IEN_PROP */
    #ifdef        Si2168_SCAN_FMAX_PROP
     case         Si2168_SCAN_FMAX_PROP_CODE:
               api->prop->scan_fmax.scan_fmax = (data >> Si2168_SCAN_FMAX_PROP_SCAN_FMAX_LSB) & Si2168_SCAN_FMAX_PROP_SCAN_FMAX_MASK;
     break;
    #endif /*     Si2168_SCAN_FMAX_PROP */
    #ifdef        Si2168_SCAN_FMIN_PROP
     case         Si2168_SCAN_FMIN_PROP_CODE:
               api->prop->scan_fmin.scan_fmin = (data >> Si2168_SCAN_FMIN_PROP_SCAN_FMIN_LSB) & Si2168_SCAN_FMIN_PROP_SCAN_FMIN_MASK;
     break;
    #endif /*     Si2168_SCAN_FMIN_PROP */
    #ifdef        Si2168_SCAN_IEN_PROP
     case         Si2168_SCAN_IEN_PROP_CODE:
               api->prop->scan_ien.buzien = (data >> Si2168_SCAN_IEN_PROP_BUZIEN_LSB) & Si2168_SCAN_IEN_PROP_BUZIEN_MASK;
               api->prop->scan_ien.reqien = (data >> Si2168_SCAN_IEN_PROP_REQIEN_LSB) & Si2168_SCAN_IEN_PROP_REQIEN_MASK;
     break;
    #endif /*     Si2168_SCAN_IEN_PROP */
    #ifdef        Si2168_SCAN_INT_SENSE_PROP
     case         Si2168_SCAN_INT_SENSE_PROP_CODE:
               api->prop->scan_int_sense.buznegen = (data >> Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN_LSB) & Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN_MASK;
               api->prop->scan_int_sense.reqnegen = (data >> Si2168_SCAN_INT_SENSE_PROP_REQNEGEN_LSB) & Si2168_SCAN_INT_SENSE_PROP_REQNEGEN_MASK;
               api->prop->scan_int_sense.buzposen = (data >> Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN_LSB) & Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN_MASK;
               api->prop->scan_int_sense.reqposen = (data >> Si2168_SCAN_INT_SENSE_PROP_REQPOSEN_LSB) & Si2168_SCAN_INT_SENSE_PROP_REQPOSEN_MASK;
     break;
    #endif /*     Si2168_SCAN_INT_SENSE_PROP */

    #ifdef        Si2168_SCAN_SYMB_RATE_MAX_PROP
     case         Si2168_SCAN_SYMB_RATE_MAX_PROP_CODE:
               api->prop->scan_symb_rate_max.scan_symb_rate_max = (data >> Si2168_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX_LSB) & Si2168_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX_MASK;
     break;
    #endif /*     Si2168_SCAN_SYMB_RATE_MAX_PROP */
    #ifdef        Si2168_SCAN_SYMB_RATE_MIN_PROP
     case         Si2168_SCAN_SYMB_RATE_MIN_PROP_CODE:
               api->prop->scan_symb_rate_min.scan_symb_rate_min = (data >> Si2168_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN_LSB) & Si2168_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN_MASK;
     break;
    #endif /*     Si2168_SCAN_SYMB_RATE_MIN_PROP */
    #ifdef        Si2168_SCAN_TER_CONFIG_PROP
     case         Si2168_SCAN_TER_CONFIG_PROP_CODE:
               api->prop->scan_ter_config.mode          = (data >> Si2168_SCAN_TER_CONFIG_PROP_MODE_LSB         ) & Si2168_SCAN_TER_CONFIG_PROP_MODE_MASK;
               api->prop->scan_ter_config.analog_bw     = (data >> Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW_LSB    ) & Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW_MASK;
               api->prop->scan_ter_config.search_analog = (data >> Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_LSB) & Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_MASK;
     break;
    #endif /*     Si2168_SCAN_TER_CONFIG_PROP */

   default : break;
    }
    return res;
  }
  /* _get_property2_insertion_point */

#ifdef    Si2168_GET_PROPERTY_STRING
/* _get_property_string_insertion_start */

  /* --------------------------------------------*/
  /* GET_PROPERTY_STRING FUNCTION                */
  /* --------------------------------------------*/
unsigned char Si2168_L1_GetPropertyString(L1_Si2168_Context *api, unsigned int prop_code, const char *separator, char *msg) {
    unsigned char res;
    res = Si2168_L1_GetProperty2(api,prop_code);
    if (res!=NO_Si2168_ERROR) { sprintf(msg, "%s",Si2168_L1_API_ERROR_TEXT(res)); return res; }
    Si2168_L1_FillPropertyStringText(api, prop_code, separator, msg);
    return NO_Si2168_ERROR;
}
  /* --------------------------------------------*/
  /* FillPropertyStringText FUNCTION             */
  /* --------------------------------------------*/
void          Si2168_L1_FillPropertyStringText(L1_Si2168_Context *api, unsigned int prop_code, const char *separator, char *msg) {
    switch (prop_code) {
    #ifdef        Si2168_DD_BER_RESOL_PROP
     case         Si2168_DD_BER_RESOL_PROP_CODE:
      sprintf(msg,"DD_BER_RESOL");
       strcat(msg,separator); strcat(msg,"-EXP "); sprintf(msg,"%s%d", msg, api->prop->dd_ber_resol.exp);
       strcat(msg,separator); strcat(msg,"-MANT "); sprintf(msg,"%s%d", msg, api->prop->dd_ber_resol.mant);
     break;
    #endif /*     Si2168_DD_BER_RESOL_PROP */
    #ifdef        Si2168_DD_CBER_RESOL_PROP
     case         Si2168_DD_CBER_RESOL_PROP_CODE:
      sprintf(msg,"DD_CBER_RESOL");
       strcat(msg,separator); strcat(msg,"-EXP "); sprintf(msg,"%s%d", msg, api->prop->dd_cber_resol.exp);
       strcat(msg,separator); strcat(msg,"-MANT "); sprintf(msg,"%s%d", msg, api->prop->dd_cber_resol.mant);
     break;
    #endif /*     Si2168_DD_CBER_RESOL_PROP */

    #ifdef        Si2168_DD_FER_RESOL_PROP
     case         Si2168_DD_FER_RESOL_PROP_CODE:
      sprintf(msg,"DD_FER_RESOL");
       strcat(msg,separator); strcat(msg,"-EXP "); sprintf(msg,"%s%d", msg, api->prop->dd_fer_resol.exp);
       strcat(msg,separator); strcat(msg,"-MANT "); sprintf(msg,"%s%d", msg, api->prop->dd_fer_resol.mant);
     break;
    #endif /*     Si2168_DD_FER_RESOL_PROP */
    #ifdef        Si2168_DD_IEN_PROP
     case         Si2168_DD_IEN_PROP_CODE:
      sprintf(msg,"DD_IEN");
       strcat(msg,separator); strcat(msg,"-IEN_BIT0 ");
           if  (api->prop->dd_ien.ien_bit0 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_ien.ien_bit0 ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->dd_ien.ien_bit0);
       strcat(msg,separator); strcat(msg,"-IEN_BIT1 ");
           if  (api->prop->dd_ien.ien_bit1 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_ien.ien_bit1 ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->dd_ien.ien_bit1);
       strcat(msg,separator); strcat(msg,"-IEN_BIT2 ");
           if  (api->prop->dd_ien.ien_bit2 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_ien.ien_bit2 ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->dd_ien.ien_bit2);
       strcat(msg,separator); strcat(msg,"-IEN_BIT3 ");
           if  (api->prop->dd_ien.ien_bit3 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_ien.ien_bit3 ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->dd_ien.ien_bit3);
       strcat(msg,separator); strcat(msg,"-IEN_BIT4 ");
           if  (api->prop->dd_ien.ien_bit4 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_ien.ien_bit4 ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->dd_ien.ien_bit4);
       strcat(msg,separator); strcat(msg,"-IEN_BIT5 ");
           if  (api->prop->dd_ien.ien_bit5 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_ien.ien_bit5 ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->dd_ien.ien_bit5);
       strcat(msg,separator); strcat(msg,"-IEN_BIT6 ");
           if  (api->prop->dd_ien.ien_bit6 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_ien.ien_bit6 ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->dd_ien.ien_bit6);
       strcat(msg,separator); strcat(msg,"-IEN_BIT7 ");
           if  (api->prop->dd_ien.ien_bit7 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_ien.ien_bit7 ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->dd_ien.ien_bit7);
     break;
    #endif /*     Si2168_DD_IEN_PROP */
    #ifdef        Si2168_DD_IF_INPUT_FREQ_PROP
     case         Si2168_DD_IF_INPUT_FREQ_PROP_CODE:
      sprintf(msg,"DD_IF_INPUT_FREQ");
       strcat(msg,separator); strcat(msg,"-OFFSET "); sprintf(msg,"%s%d", msg, api->prop->dd_if_input_freq.offset);
     break;
    #endif /*     Si2168_DD_IF_INPUT_FREQ_PROP */
    #ifdef        Si2168_DD_INT_SENSE_PROP
     case         Si2168_DD_INT_SENSE_PROP_CODE:
      sprintf(msg,"DD_INT_SENSE");
       strcat(msg,separator); strcat(msg,"-NEG_BIT0 ");
           if  (api->prop->dd_int_sense.neg_bit0 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.neg_bit0 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.neg_bit0);
       strcat(msg,separator); strcat(msg,"-NEG_BIT1 ");
           if  (api->prop->dd_int_sense.neg_bit1 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.neg_bit1 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.neg_bit1);
       strcat(msg,separator); strcat(msg,"-NEG_BIT2 ");
           if  (api->prop->dd_int_sense.neg_bit2 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.neg_bit2 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.neg_bit2);
       strcat(msg,separator); strcat(msg,"-NEG_BIT3 ");
           if  (api->prop->dd_int_sense.neg_bit3 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.neg_bit3 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.neg_bit3);
       strcat(msg,separator); strcat(msg,"-NEG_BIT4 ");
           if  (api->prop->dd_int_sense.neg_bit4 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.neg_bit4 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.neg_bit4);
       strcat(msg,separator); strcat(msg,"-NEG_BIT5 ");
           if  (api->prop->dd_int_sense.neg_bit5 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.neg_bit5 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.neg_bit5);
       strcat(msg,separator); strcat(msg,"-NEG_BIT6 ");
           if  (api->prop->dd_int_sense.neg_bit6 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.neg_bit6 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.neg_bit6);
       strcat(msg,separator); strcat(msg,"-NEG_BIT7 ");
           if  (api->prop->dd_int_sense.neg_bit7 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.neg_bit7 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.neg_bit7);
       strcat(msg,separator); strcat(msg,"-POS_BIT0 ");
           if  (api->prop->dd_int_sense.pos_bit0 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.pos_bit0 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.pos_bit0);
       strcat(msg,separator); strcat(msg,"-POS_BIT1 ");
           if  (api->prop->dd_int_sense.pos_bit1 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.pos_bit1 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.pos_bit1);
       strcat(msg,separator); strcat(msg,"-POS_BIT2 ");
           if  (api->prop->dd_int_sense.pos_bit2 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.pos_bit2 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.pos_bit2);
       strcat(msg,separator); strcat(msg,"-POS_BIT3 ");
           if  (api->prop->dd_int_sense.pos_bit3 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.pos_bit3 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.pos_bit3);
       strcat(msg,separator); strcat(msg,"-POS_BIT4 ");
           if  (api->prop->dd_int_sense.pos_bit4 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.pos_bit4 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.pos_bit4);
       strcat(msg,separator); strcat(msg,"-POS_BIT5 ");
           if  (api->prop->dd_int_sense.pos_bit5 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.pos_bit5 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.pos_bit5);
       strcat(msg,separator); strcat(msg,"-POS_BIT6 ");
           if  (api->prop->dd_int_sense.pos_bit6 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.pos_bit6 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.pos_bit6);
       strcat(msg,separator); strcat(msg,"-POS_BIT7 ");
           if  (api->prop->dd_int_sense.pos_bit7 ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->dd_int_sense.pos_bit7 ==     1) strcat(msg,"ENABLE  ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dd_int_sense.pos_bit7);
     break;
    #endif /*     Si2168_DD_INT_SENSE_PROP */
    #ifdef        Si2168_DD_MODE_PROP
     case         Si2168_DD_MODE_PROP_CODE:
      sprintf(msg,"DD_MODE");
       strcat(msg,separator); strcat(msg,"-BW ");
           if  (api->prop->dd_mode.bw              ==     5) strcat(msg,"BW_5MHZ   ");
      else if  (api->prop->dd_mode.bw              ==     6) strcat(msg,"BW_6MHZ   ");
      else if  (api->prop->dd_mode.bw              ==     7) strcat(msg,"BW_7MHZ   ");
      else if  (api->prop->dd_mode.bw              ==     8) strcat(msg,"BW_8MHZ   ");
      else if  (api->prop->dd_mode.bw              ==     2) strcat(msg,"BW_1D7MHZ ");
      else                                                  sprintf(msg,"%s%d", msg, api->prop->dd_mode.bw);
       strcat(msg,separator); strcat(msg,"-MODULATION ");
           if  (api->prop->dd_mode.modulation      ==     2) strcat(msg,"DVBT        ");
      else if  (api->prop->dd_mode.modulation      ==     3) strcat(msg,"DVBC        ");
      else if  (api->prop->dd_mode.modulation      ==     7) strcat(msg,"DVBT2       ");
      else if  (api->prop->dd_mode.modulation      ==     8) strcat(msg,"DVBS        ");
      else if  (api->prop->dd_mode.modulation      ==     9) strcat(msg,"DVBS2       ");
      else if  (api->prop->dd_mode.modulation      ==    10) strcat(msg,"DSS         ");
      else if  (api->prop->dd_mode.modulation      ==    15) strcat(msg,"AUTO_DETECT ");
      else                                                  sprintf(msg,"%s%d", msg, api->prop->dd_mode.modulation);
       strcat(msg,separator); strcat(msg,"-INVERT_SPECTRUM ");
           if  (api->prop->dd_mode.invert_spectrum ==     0) strcat(msg,"NORMAL   ");
      else if  (api->prop->dd_mode.invert_spectrum ==     1) strcat(msg,"INVERTED ");
      else                                                  sprintf(msg,"%s%d", msg, api->prop->dd_mode.invert_spectrum);
       strcat(msg,separator); strcat(msg,"-AUTO_DETECT ");
           if  (api->prop->dd_mode.auto_detect     ==     0) strcat(msg,"NONE              ");
      else if  (api->prop->dd_mode.auto_detect     ==     1) strcat(msg,"AUTO_DVB_T_T2     ");
      else if  (api->prop->dd_mode.auto_detect     ==     2) strcat(msg,"AUTO_DVB_S_S2     ");
      else if  (api->prop->dd_mode.auto_detect     ==     3) strcat(msg,"AUTO_DVB_S_S2_DSS ");
      else                                                  sprintf(msg,"%s%d", msg, api->prop->dd_mode.auto_detect);
     break;
    #endif /*     Si2168_DD_MODE_PROP */
    #ifdef        Si2168_DD_PER_RESOL_PROP
     case         Si2168_DD_PER_RESOL_PROP_CODE:
      sprintf(msg,"DD_PER_RESOL");
       strcat(msg,separator); strcat(msg,"-EXP "); sprintf(msg,"%s%d", msg, api->prop->dd_per_resol.exp);
       strcat(msg,separator); strcat(msg,"-MANT "); sprintf(msg,"%s%d", msg, api->prop->dd_per_resol.mant);
     break;
    #endif /*     Si2168_DD_PER_RESOL_PROP */
    #ifdef        Si2168_DD_RSQ_BER_THRESHOLD_PROP
     case         Si2168_DD_RSQ_BER_THRESHOLD_PROP_CODE:
      sprintf(msg,"DD_RSQ_BER_THRESHOLD");
       strcat(msg,separator); strcat(msg,"-EXP "); sprintf(msg,"%s%d", msg, api->prop->dd_rsq_ber_threshold.exp);
       strcat(msg,separator); strcat(msg,"-MANT "); sprintf(msg,"%s%d", msg, api->prop->dd_rsq_ber_threshold.mant);
     break;
    #endif /*     Si2168_DD_RSQ_BER_THRESHOLD_PROP */
    #ifdef        Si2168_DD_SSI_SQI_PARAM_PROP
     case         Si2168_DD_SSI_SQI_PARAM_PROP_CODE:
      sprintf(msg,"DD_SSI_SQI_PARAM");
       strcat(msg,separator); strcat(msg,"-SQI_AVERAGE "); sprintf(msg,"%s%d", msg, api->prop->dd_ssi_sqi_param.sqi_average);
     break;
    #endif /*     Si2168_DD_SSI_SQI_PARAM_PROP */
    #ifdef        Si2168_DD_TS_FREQ_PROP
     case         Si2168_DD_TS_FREQ_PROP_CODE:
      sprintf(msg,"DD_TS_FREQ");
       strcat(msg,separator); strcat(msg,"-REQ_FREQ_10KHZ "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_freq.req_freq_10khz);
     break;
    #endif /*     Si2168_DD_TS_FREQ_PROP */
    #ifdef        Si2168_DD_TS_MODE_PROP
     case         Si2168_DD_TS_MODE_PROP_CODE:
      sprintf(msg,"DD_TS_MODE");
       strcat(msg,separator); strcat(msg,"-MODE ");
           if  (api->prop->dd_ts_mode.mode          ==     0) strcat(msg,"TRISTATE ");
      else if  (api->prop->dd_ts_mode.mode          ==     1) strcat(msg,"OFF      ");
      else if  (api->prop->dd_ts_mode.mode          ==     3) strcat(msg,"SERIAL   ");
      else if  (api->prop->dd_ts_mode.mode          ==     6) strcat(msg,"PARALLEL ");
      else if  (api->prop->dd_ts_mode.mode          ==     7) strcat(msg,"GPIF     ");
      else                                                   sprintf(msg,"%s%d", msg, api->prop->dd_ts_mode.mode);
       strcat(msg,separator); strcat(msg,"-CLOCK ");
           if  (api->prop->dd_ts_mode.clock         ==     0) strcat(msg,"AUTO_FIXED ");
      else if  (api->prop->dd_ts_mode.clock         ==     1) strcat(msg,"AUTO_ADAPT ");
      else if  (api->prop->dd_ts_mode.clock         ==     2) strcat(msg,"MANUAL     ");
      else                                                   sprintf(msg,"%s%d", msg, api->prop->dd_ts_mode.clock);
       strcat(msg,separator); strcat(msg,"-CLK_GAPPED_EN ");
           if  (api->prop->dd_ts_mode.clk_gapped_en ==     0) strcat(msg,"DISABLED ");
      else if  (api->prop->dd_ts_mode.clk_gapped_en ==     1) strcat(msg,"ENABLED  ");
      else                                                   sprintf(msg,"%s%d", msg, api->prop->dd_ts_mode.clk_gapped_en);
       strcat(msg,separator); strcat(msg,"-TS_ERR_POLARITY ");
           if  (api->prop->dd_ts_mode.ts_err_polarity ==     0) strcat(msg,"NOT_INVERTED ");
      else if  (api->prop->dd_ts_mode.ts_err_polarity ==     1) strcat(msg,"INVERTED     ");
      else                                                     sprintf(msg,"%s%d", msg, api->prop->dd_ts_mode.ts_err_polarity);
       strcat(msg,separator); strcat(msg,"-SPECIAL ");
           if  (api->prop->dd_ts_mode.special         ==     0) strcat(msg,"FULL_TS        ");
      else if  (api->prop->dd_ts_mode.special         ==     1) strcat(msg,"DATAS_TRISTATE ");
      else                                                     sprintf(msg,"%s%d", msg, api->prop->dd_ts_mode.special);
     break;
    #endif /*     Si2168_DD_TS_MODE_PROP */
    #ifdef        Si2168_DD_TS_SETUP_PAR_PROP
     case         Si2168_DD_TS_SETUP_PAR_PROP_CODE:
      sprintf(msg,"DD_TS_SETUP_PAR");
       strcat(msg,separator); strcat(msg,"-TS_DATA_STRENGTH "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_par.ts_data_strength);
       strcat(msg,separator); strcat(msg,"-TS_DATA_SHAPE "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_par.ts_data_shape);
       strcat(msg,separator); strcat(msg,"-TS_CLK_STRENGTH "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_par.ts_clk_strength);
       strcat(msg,separator); strcat(msg,"-TS_CLK_SHAPE "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_par.ts_clk_shape);
       strcat(msg,separator); strcat(msg,"-TS_CLK_INVERT ");
           if  (api->prop->dd_ts_setup_par.ts_clk_invert    ==     0) strcat(msg,"NOT_INVERTED ");
      else if  (api->prop->dd_ts_setup_par.ts_clk_invert    ==     1) strcat(msg,"INVERTED     ");
      else                                                           sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_par.ts_clk_invert);
       strcat(msg,separator); strcat(msg,"-TS_CLK_SHIFT "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_par.ts_clk_shift);
     break;
    #endif /*     Si2168_DD_TS_SETUP_PAR_PROP */
    #ifdef        Si2168_DD_TS_SETUP_SER_PROP
     case         Si2168_DD_TS_SETUP_SER_PROP_CODE:
      sprintf(msg,"DD_TS_SETUP_SER");
       strcat(msg,separator); strcat(msg,"-TS_DATA_STRENGTH "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_ser.ts_data_strength);
       strcat(msg,separator); strcat(msg,"-TS_DATA_SHAPE "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_ser.ts_data_shape);
       strcat(msg,separator); strcat(msg,"-TS_CLK_STRENGTH "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_ser.ts_clk_strength);
       strcat(msg,separator); strcat(msg,"-TS_CLK_SHAPE "); sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_ser.ts_clk_shape);
       strcat(msg,separator); strcat(msg,"-TS_CLK_INVERT ");
           if  (api->prop->dd_ts_setup_ser.ts_clk_invert    ==     0) strcat(msg,"NOT_INVERTED ");
      else if  (api->prop->dd_ts_setup_ser.ts_clk_invert    ==     1) strcat(msg,"INVERTED     ");
      else                                                           sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_ser.ts_clk_invert);
       strcat(msg,separator); strcat(msg,"-TS_SYNC_DURATION ");
           if  (api->prop->dd_ts_setup_ser.ts_sync_duration ==     0) strcat(msg,"FIRST_BYTE ");
      else if  (api->prop->dd_ts_setup_ser.ts_sync_duration ==     1) strcat(msg,"FIRST_BIT  ");
      else                                                           sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_ser.ts_sync_duration);
       strcat(msg,separator); strcat(msg,"-TS_BYTE_ORDER ");
           if  (api->prop->dd_ts_setup_ser.ts_byte_order    ==     0) strcat(msg,"MSB_FIRST ");
      else if  (api->prop->dd_ts_setup_ser.ts_byte_order    ==     1) strcat(msg,"LSB_FIRST ");
      else                                                           sprintf(msg,"%s%d", msg, api->prop->dd_ts_setup_ser.ts_byte_order);
     break;
    #endif /*     Si2168_DD_TS_SETUP_SER_PROP */
    #ifdef        Si2168_DVBC_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBC_ADC_CREST_FACTOR_PROP_CODE:
      sprintf(msg,"DVBC_ADC_CREST_FACTOR");
       strcat(msg,separator); strcat(msg,"-CREST_FACTOR "); sprintf(msg,"%s%d", msg, api->prop->dvbc_adc_crest_factor.crest_factor);
     break;
    #endif /*     Si2168_DVBC_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBC_AFC_RANGE_PROP
     case         Si2168_DVBC_AFC_RANGE_PROP_CODE:
      sprintf(msg,"DVBC_AFC_RANGE");
       strcat(msg,separator); strcat(msg,"-RANGE_KHZ "); sprintf(msg,"%s%d", msg, api->prop->dvbc_afc_range.range_khz);
     break;
    #endif /*     Si2168_DVBC_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBC_CONSTELLATION_PROP
     case         Si2168_DVBC_CONSTELLATION_PROP_CODE:
      sprintf(msg,"DVBC_CONSTELLATION");
       strcat(msg,separator); strcat(msg,"-CONSTELLATION ");
           if  (api->prop->dvbc_constellation.constellation ==     0) strcat(msg,"AUTO   ");
      else if  (api->prop->dvbc_constellation.constellation ==     7) strcat(msg,"QAM16  ");
      else if  (api->prop->dvbc_constellation.constellation ==     8) strcat(msg,"QAM32  ");
      else if  (api->prop->dvbc_constellation.constellation ==     9) strcat(msg,"QAM64  ");
      else if  (api->prop->dvbc_constellation.constellation ==    10) strcat(msg,"QAM128 ");
      else if  (api->prop->dvbc_constellation.constellation ==    11) strcat(msg,"QAM256 ");
      else                                                           sprintf(msg,"%s%d", msg, api->prop->dvbc_constellation.constellation);
     break;
    #endif /*     Si2168_DVBC_CONSTELLATION_PROP */
    #ifdef        Si2168_DVBC_SYMBOL_RATE_PROP
     case         Si2168_DVBC_SYMBOL_RATE_PROP_CODE:
      sprintf(msg,"DVBC_SYMBOL_RATE");
       strcat(msg,separator); strcat(msg,"-RATE "); sprintf(msg,"%s%d", msg, api->prop->dvbc_symbol_rate.rate);
     break;
    #endif /*     Si2168_DVBC_SYMBOL_RATE_PROP */


    #ifdef        Si2168_DVBT2_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CODE:
      sprintf(msg,"DVBT2_ADC_CREST_FACTOR");
       strcat(msg,separator); strcat(msg,"-CREST_FACTOR "); sprintf(msg,"%s%d", msg, api->prop->dvbt2_adc_crest_factor.crest_factor);
     break;
    #endif /*     Si2168_DVBT2_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBT2_AFC_RANGE_PROP
     case         Si2168_DVBT2_AFC_RANGE_PROP_CODE:
      sprintf(msg,"DVBT2_AFC_RANGE");
       strcat(msg,separator); strcat(msg,"-RANGE_KHZ "); sprintf(msg,"%s%d", msg, api->prop->dvbt2_afc_range.range_khz);
     break;
    #endif /*     Si2168_DVBT2_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBT2_FEF_TUNER_PROP
     case         Si2168_DVBT2_FEF_TUNER_PROP_CODE:
      sprintf(msg,"DVBT2_FEF_TUNER");
       strcat(msg,separator); strcat(msg,"-TUNER_DELAY "); sprintf(msg,"%s%d", msg, api->prop->dvbt2_fef_tuner.tuner_delay);
       strcat(msg,separator); strcat(msg,"-TUNER_FREEZE_TIME "); sprintf(msg,"%s%d", msg, api->prop->dvbt2_fef_tuner.tuner_freeze_time);
       strcat(msg,separator); strcat(msg,"-TUNER_UNFREEZE_TIME "); sprintf(msg,"%s%d", msg, api->prop->dvbt2_fef_tuner.tuner_unfreeze_time);
     break;
    #endif /*     Si2168_DVBT2_FEF_TUNER_PROP */

    #ifdef        Si2168_DVBT_ADC_CREST_FACTOR_PROP
     case         Si2168_DVBT_ADC_CREST_FACTOR_PROP_CODE:
      sprintf(msg,"DVBT_ADC_CREST_FACTOR");
       strcat(msg,separator); strcat(msg,"-CREST_FACTOR "); sprintf(msg,"%s%d", msg, api->prop->dvbt_adc_crest_factor.crest_factor);
     break;
    #endif /*     Si2168_DVBT_ADC_CREST_FACTOR_PROP */
    #ifdef        Si2168_DVBT_AFC_RANGE_PROP
     case         Si2168_DVBT_AFC_RANGE_PROP_CODE:
      sprintf(msg,"DVBT_AFC_RANGE");
       strcat(msg,separator); strcat(msg,"-RANGE_KHZ "); sprintf(msg,"%s%d", msg, api->prop->dvbt_afc_range.range_khz);
     break;
    #endif /*     Si2168_DVBT_AFC_RANGE_PROP */
    #ifdef        Si2168_DVBT_HIERARCHY_PROP
     case         Si2168_DVBT_HIERARCHY_PROP_CODE:
      sprintf(msg,"DVBT_HIERARCHY");
       strcat(msg,separator); strcat(msg,"-STREAM ");
           if  (api->prop->dvbt_hierarchy.stream ==     0) strcat(msg,"HP ");
      else if  (api->prop->dvbt_hierarchy.stream ==     1) strcat(msg,"LP ");
      else                                                sprintf(msg,"%s%d", msg, api->prop->dvbt_hierarchy.stream);
     break;
    #endif /*     Si2168_DVBT_HIERARCHY_PROP */

    #ifdef        Si2168_MASTER_IEN_PROP
     case         Si2168_MASTER_IEN_PROP_CODE:
      sprintf(msg,"MASTER_IEN");
       strcat(msg,separator); strcat(msg,"-DDIEN ");
           if  (api->prop->master_ien.ddien   ==     0) strcat(msg,"OFF ");
      else if  (api->prop->master_ien.ddien   ==     1) strcat(msg,"ON  ");
      else                                             sprintf(msg,"%s%d", msg, api->prop->master_ien.ddien);
       strcat(msg,separator); strcat(msg,"-SCANIEN ");
           if  (api->prop->master_ien.scanien ==     0) strcat(msg,"OFF ");
      else if  (api->prop->master_ien.scanien ==     1) strcat(msg,"ON  ");
      else                                             sprintf(msg,"%s%d", msg, api->prop->master_ien.scanien);
       strcat(msg,separator); strcat(msg,"-ERRIEN ");
           if  (api->prop->master_ien.errien  ==     0) strcat(msg,"OFF ");
      else if  (api->prop->master_ien.errien  ==     1) strcat(msg,"ON  ");
      else                                             sprintf(msg,"%s%d", msg, api->prop->master_ien.errien);
       strcat(msg,separator); strcat(msg,"-CTSIEN ");
           if  (api->prop->master_ien.ctsien  ==     0) strcat(msg,"OFF ");
      else if  (api->prop->master_ien.ctsien  ==     1) strcat(msg,"ON  ");
      else                                             sprintf(msg,"%s%d", msg, api->prop->master_ien.ctsien);
     break;
    #endif /*     Si2168_MASTER_IEN_PROP */
    #ifdef        Si2168_SCAN_FMAX_PROP
     case         Si2168_SCAN_FMAX_PROP_CODE:
      sprintf(msg,"SCAN_FMAX");
       strcat(msg,separator); strcat(msg,"-SCAN_FMAX "); sprintf(msg,"%s%d", msg, api->prop->scan_fmax.scan_fmax);
     break;
    #endif /*     Si2168_SCAN_FMAX_PROP */
    #ifdef        Si2168_SCAN_FMIN_PROP
     case         Si2168_SCAN_FMIN_PROP_CODE:
      sprintf(msg,"SCAN_FMIN");
       strcat(msg,separator); strcat(msg,"-SCAN_FMIN "); sprintf(msg,"%s%d", msg, api->prop->scan_fmin.scan_fmin);
     break;
    #endif /*     Si2168_SCAN_FMIN_PROP */
    #ifdef        Si2168_SCAN_IEN_PROP
     case         Si2168_SCAN_IEN_PROP_CODE:
      sprintf(msg,"SCAN_IEN");
       strcat(msg,separator); strcat(msg,"-BUZIEN ");
           if  (api->prop->scan_ien.buzien ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->scan_ien.buzien ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->scan_ien.buzien);
       strcat(msg,separator); strcat(msg,"-REQIEN ");
           if  (api->prop->scan_ien.reqien ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->scan_ien.reqien ==     1) strcat(msg,"ENABLE  ");
      else                                          sprintf(msg,"%s%d", msg, api->prop->scan_ien.reqien);
     break;
    #endif /*     Si2168_SCAN_IEN_PROP */
    #ifdef        Si2168_SCAN_INT_SENSE_PROP
     case         Si2168_SCAN_INT_SENSE_PROP_CODE:
      sprintf(msg,"SCAN_INT_SENSE");
       strcat(msg,separator); strcat(msg,"-BUZNEGEN ");
           if  (api->prop->scan_int_sense.buznegen ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->scan_int_sense.buznegen ==     1) strcat(msg,"ENABLE  ");
      else                                                  sprintf(msg,"%s%d", msg, api->prop->scan_int_sense.buznegen);
       strcat(msg,separator); strcat(msg,"-REQNEGEN ");
           if  (api->prop->scan_int_sense.reqnegen ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->scan_int_sense.reqnegen ==     1) strcat(msg,"ENABLE  ");
      else                                                  sprintf(msg,"%s%d", msg, api->prop->scan_int_sense.reqnegen);
       strcat(msg,separator); strcat(msg,"-BUZPOSEN ");
           if  (api->prop->scan_int_sense.buzposen ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->scan_int_sense.buzposen ==     1) strcat(msg,"ENABLE  ");
      else                                                  sprintf(msg,"%s%d", msg, api->prop->scan_int_sense.buzposen);
       strcat(msg,separator); strcat(msg,"-REQPOSEN ");
           if  (api->prop->scan_int_sense.reqposen ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->scan_int_sense.reqposen ==     1) strcat(msg,"ENABLE  ");
      else                                                  sprintf(msg,"%s%d", msg, api->prop->scan_int_sense.reqposen);
     break;
    #endif /*     Si2168_SCAN_INT_SENSE_PROP */

    #ifdef        Si2168_SCAN_SYMB_RATE_MAX_PROP
     case         Si2168_SCAN_SYMB_RATE_MAX_PROP_CODE:
      sprintf(msg,"SCAN_SYMB_RATE_MAX");
       strcat(msg,separator); strcat(msg,"-SCAN_SYMB_RATE_MAX "); sprintf(msg,"%s%d", msg, api->prop->scan_symb_rate_max.scan_symb_rate_max);
     break;
    #endif /*     Si2168_SCAN_SYMB_RATE_MAX_PROP */
    #ifdef        Si2168_SCAN_SYMB_RATE_MIN_PROP
     case         Si2168_SCAN_SYMB_RATE_MIN_PROP_CODE:
      sprintf(msg,"SCAN_SYMB_RATE_MIN");
       strcat(msg,separator); strcat(msg,"-SCAN_SYMB_RATE_MIN "); sprintf(msg,"%s%d", msg, api->prop->scan_symb_rate_min.scan_symb_rate_min);
     break;
    #endif /*     Si2168_SCAN_SYMB_RATE_MIN_PROP */
    #ifdef        Si2168_SCAN_TER_CONFIG_PROP
     case         Si2168_SCAN_TER_CONFIG_PROP_CODE:
      sprintf(msg,"SCAN_TER_CONFIG");
       strcat(msg,separator); strcat(msg,"-MODE ");
           if  (api->prop->scan_ter_config.mode          ==     0) strcat(msg,"BLIND_SCAN   ");
      else if  (api->prop->scan_ter_config.mode          ==     1) strcat(msg,"MAPPING_SCAN ");
      else if  (api->prop->scan_ter_config.mode          ==     2) strcat(msg,"BLIND_LOCK   ");
      else                                                        sprintf(msg,"%s%d", msg, api->prop->scan_ter_config.mode);
       strcat(msg,separator); strcat(msg,"-ANALOG_BW ");
           if  (api->prop->scan_ter_config.analog_bw     ==     1) strcat(msg,"6MHZ ");
      else if  (api->prop->scan_ter_config.analog_bw     ==     2) strcat(msg,"7MHZ ");
      else if  (api->prop->scan_ter_config.analog_bw     ==     3) strcat(msg,"8MHZ ");
      else                                                        sprintf(msg,"%s%d", msg, api->prop->scan_ter_config.analog_bw);
       strcat(msg,separator); strcat(msg,"-SEARCH_ANALOG ");
           if  (api->prop->scan_ter_config.search_analog ==     0) strcat(msg,"DISABLE ");
      else if  (api->prop->scan_ter_config.search_analog ==     1) strcat(msg,"ENABLE  ");
      else                                                        sprintf(msg,"%s%d", msg, api->prop->scan_ter_config.search_analog);
     break;
    #endif /*     Si2168_SCAN_TER_CONFIG_PROP */

   default : sprintf(msg,"Unknown property code '0x%06x'\n", prop_code); break;
    }
  }
  /* _get_property_string_insertion_point */
#endif /* Si2168_GET_PROPERTY_STRING */






