/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 1 API
   API properties strings functions definitions
   FILE: Si2168_L1_Properties.c
   Supported IC : Si2168
   Compiled for ROM 2 firmware 2_0_build_11
   Revision: REVISION_NAME
   Tag:  V2.6.0
   Date: September 24 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
/* Change log:
 As from V2.5.7:
  Commas removed outside functions, to avoid ISO-C compiler warnings

 As from V2.5.3:
  In Si2168_setupCOMMONDefaults: making SQI response as fast as possible
    api->prop->dd_ssi_sqi_param.sqi_average           =     1; (default     1)

 As from V2.4.6:
  Changing type of Si2168_featureText[] to const char *
  Adding Si2168_DD_SSI_SQI_PARAM property code
   api->prop->dd_ts_setup_ser.ts_data_strength       =     7;
   api->prop->dd_ts_setup_ser.ts_data_shape          =     1;
   api->prop->dd_ts_setup_ser.ts_clk_strength        =     7;
   api->prop->dd_ts_setup_ser.ts_clk_shape           =     1;

 As from V2.4.1:
  api->prop->dd_fer_resol.exp                       =     2; (default     3)

 As from V2.3.7:
  setting api->prop->dd_diseqc_freq.freq_hz = 0 to set DiSEqC pin in 'envelop' mode
  adding  api->prop->dd_ts_mode.ts_err_polarity to allow inverting the TS ERR pin


  In Si2168_setupSCANDefaults:
   scan_sat_config.analog_detect set to 'ENABLED' to speed up SAT scan (faster rejection of analog channels).
   TS strength and shape reduced to avoid perfomance issues. (Values for serial mode may need to be adjusted).
   api->prop->dd_ts_setup_par.ts_data_strength       =     3;
   api->prop->dd_ts_setup_par.ts_data_shape          =     2;
   api->prop->dd_ts_setup_par.ts_clk_strength        =     3;
   api->prop->dd_ts_setup_par.ts_clk_shape           =     2;

   api->prop->dd_ts_setup_ser.ts_data_strength       =     3;
   api->prop->dd_ts_setup_ser.ts_data_shape          =     1;
   api->prop->dd_ts_setup_ser.ts_clk_strength        =     3;
   api->prop->dd_ts_setup_ser.ts_clk_shape           =     1;

 As from 2.2.7:
  in Si2168_setupDDDefaults: increased ts pins shape in parallel mode to really allow TS output over USB
  api->prop->dd_ts_setup_par.ts_data_shape          =     7;
  api->prop->dd_ts_setup_par.ts_clk_shape           =     7;

 As from 2.2.0:
  in Si2168_setupDDDefaults: changed ts pins shape in parallel mode to allow TS output over USB
  api->prop->dd_ts_setup_par.ts_data_shape          =     3;
  api->prop->dd_ts_setup_par.ts_clk_shape           =     3;
****************************************************************************************/
#define   Si2168_COMMAND_PROTOTYPES

#include "Si2168_Platform_Definition.h"

#ifdef Si2168_COMMAND_LINE_APPLICATION

/* define a local PropObj variable to use for the Property Dictionary initializer */
Si2168_PropObj Si2168_prop;
char optionText[20];

/* _properties_features_text_insertion_start */
const char *Si2168_featureText[] =
{
  "COMMON",
  "DD",
  "DVBC",


  "DVBT",

  "DVBT2",

  "SCAN",
};
/* _properties_features_text_insertion_point */

/* _properties_strings_insertion_start */
Si2168_optionStruct    Si2168_DD_BER_RESOL_PROP_EXP[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     8}
};
Si2168_optionStruct    Si2168_DD_BER_RESOL_PROP_MANT[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     9}
};
Si2168_fieldDicoStruct Si2168_DD_BER_RESOL_PROP_FIELDS[] =
{
  {"EXP",  Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ber_resol.exp , sizeof(Si2168_DD_BER_RESOL_PROP_EXP )/sizeof(Si2168_optionStruct), Si2168_DD_BER_RESOL_PROP_EXP },
  {"MANT", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ber_resol.mant, sizeof(Si2168_DD_BER_RESOL_PROP_MANT)/sizeof(Si2168_optionStruct), Si2168_DD_BER_RESOL_PROP_MANT},
};

Si2168_optionStruct    Si2168_DD_CBER_RESOL_PROP_EXP[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     8}
};
Si2168_optionStruct    Si2168_DD_CBER_RESOL_PROP_MANT[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     9}
};
Si2168_fieldDicoStruct Si2168_DD_CBER_RESOL_PROP_FIELDS[] =
{
  {"EXP",  Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_cber_resol.exp , sizeof(Si2168_DD_CBER_RESOL_PROP_EXP )/sizeof(Si2168_optionStruct), Si2168_DD_CBER_RESOL_PROP_EXP },
  {"MANT", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_cber_resol.mant, sizeof(Si2168_DD_CBER_RESOL_PROP_MANT)/sizeof(Si2168_optionStruct), Si2168_DD_CBER_RESOL_PROP_MANT},
};


Si2168_optionStruct    Si2168_DD_FER_RESOL_PROP_EXP[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     4}
};
Si2168_optionStruct    Si2168_DD_FER_RESOL_PROP_MANT[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     9}
};
Si2168_fieldDicoStruct Si2168_DD_FER_RESOL_PROP_FIELDS[] =
{
  {"EXP",  Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_fer_resol.exp , sizeof(Si2168_DD_FER_RESOL_PROP_EXP )/sizeof(Si2168_optionStruct), Si2168_DD_FER_RESOL_PROP_EXP },
  {"MANT", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_fer_resol.mant, sizeof(Si2168_DD_FER_RESOL_PROP_MANT)/sizeof(Si2168_optionStruct), Si2168_DD_FER_RESOL_PROP_MANT},
};

Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT0[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT1[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT2[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT3[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT4[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT5[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT6[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT7[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_fieldDicoStruct Si2168_DD_IEN_PROP_FIELDS[] =
{
  {"IEN_BIT0", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ien.ien_bit0, sizeof(Si2168_DD_IEN_PROP_IEN_BIT0)/sizeof(Si2168_optionStruct), Si2168_DD_IEN_PROP_IEN_BIT0},
  {"IEN_BIT1", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ien.ien_bit1, sizeof(Si2168_DD_IEN_PROP_IEN_BIT1)/sizeof(Si2168_optionStruct), Si2168_DD_IEN_PROP_IEN_BIT1},
  {"IEN_BIT2", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ien.ien_bit2, sizeof(Si2168_DD_IEN_PROP_IEN_BIT2)/sizeof(Si2168_optionStruct), Si2168_DD_IEN_PROP_IEN_BIT2},
  {"IEN_BIT3", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ien.ien_bit3, sizeof(Si2168_DD_IEN_PROP_IEN_BIT3)/sizeof(Si2168_optionStruct), Si2168_DD_IEN_PROP_IEN_BIT3},
  {"IEN_BIT4", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ien.ien_bit4, sizeof(Si2168_DD_IEN_PROP_IEN_BIT4)/sizeof(Si2168_optionStruct), Si2168_DD_IEN_PROP_IEN_BIT4},
  {"IEN_BIT5", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ien.ien_bit5, sizeof(Si2168_DD_IEN_PROP_IEN_BIT5)/sizeof(Si2168_optionStruct), Si2168_DD_IEN_PROP_IEN_BIT5},
  {"IEN_BIT6", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ien.ien_bit6, sizeof(Si2168_DD_IEN_PROP_IEN_BIT6)/sizeof(Si2168_optionStruct), Si2168_DD_IEN_PROP_IEN_BIT6},
  {"IEN_BIT7", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ien.ien_bit7, sizeof(Si2168_DD_IEN_PROP_IEN_BIT7)/sizeof(Si2168_optionStruct), Si2168_DD_IEN_PROP_IEN_BIT7},
};

Si2168_optionStruct    Si2168_DD_IF_INPUT_FREQ_PROP_OFFSET[] =
{
  {"MIN_RANGE",     0},
  {"MAX_RANGE", 36000}
};
Si2168_fieldDicoStruct Si2168_DD_IF_INPUT_FREQ_PROP_FIELDS[] =
{
  {"OFFSET", Si2168_UNSIGNED_INT, &Si2168_prop.dd_if_input_freq.offset, sizeof(Si2168_DD_IF_INPUT_FREQ_PROP_OFFSET)/sizeof(Si2168_optionStruct), Si2168_DD_IF_INPUT_FREQ_PROP_OFFSET},
};

Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT0[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT1[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT2[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT3[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT4[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT5[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT6[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT7[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT0[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT1[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT2[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT3[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT4[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT5[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT6[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT7[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_fieldDicoStruct Si2168_DD_INT_SENSE_PROP_FIELDS[] =
{
  {"NEG_BIT0", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.neg_bit0, sizeof(Si2168_DD_INT_SENSE_PROP_NEG_BIT0)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_NEG_BIT0},
  {"NEG_BIT1", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.neg_bit1, sizeof(Si2168_DD_INT_SENSE_PROP_NEG_BIT1)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_NEG_BIT1},
  {"NEG_BIT2", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.neg_bit2, sizeof(Si2168_DD_INT_SENSE_PROP_NEG_BIT2)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_NEG_BIT2},
  {"NEG_BIT3", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.neg_bit3, sizeof(Si2168_DD_INT_SENSE_PROP_NEG_BIT3)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_NEG_BIT3},
  {"NEG_BIT4", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.neg_bit4, sizeof(Si2168_DD_INT_SENSE_PROP_NEG_BIT4)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_NEG_BIT4},
  {"NEG_BIT5", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.neg_bit5, sizeof(Si2168_DD_INT_SENSE_PROP_NEG_BIT5)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_NEG_BIT5},
  {"NEG_BIT6", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.neg_bit6, sizeof(Si2168_DD_INT_SENSE_PROP_NEG_BIT6)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_NEG_BIT6},
  {"NEG_BIT7", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.neg_bit7, sizeof(Si2168_DD_INT_SENSE_PROP_NEG_BIT7)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_NEG_BIT7},
  {"POS_BIT0", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.pos_bit0, sizeof(Si2168_DD_INT_SENSE_PROP_POS_BIT0)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_POS_BIT0},
  {"POS_BIT1", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.pos_bit1, sizeof(Si2168_DD_INT_SENSE_PROP_POS_BIT1)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_POS_BIT1},
  {"POS_BIT2", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.pos_bit2, sizeof(Si2168_DD_INT_SENSE_PROP_POS_BIT2)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_POS_BIT2},
  {"POS_BIT3", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.pos_bit3, sizeof(Si2168_DD_INT_SENSE_PROP_POS_BIT3)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_POS_BIT3},
  {"POS_BIT4", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.pos_bit4, sizeof(Si2168_DD_INT_SENSE_PROP_POS_BIT4)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_POS_BIT4},
  {"POS_BIT5", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.pos_bit5, sizeof(Si2168_DD_INT_SENSE_PROP_POS_BIT5)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_POS_BIT5},
  {"POS_BIT6", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.pos_bit6, sizeof(Si2168_DD_INT_SENSE_PROP_POS_BIT6)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_POS_BIT6},
  {"POS_BIT7", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_int_sense.pos_bit7, sizeof(Si2168_DD_INT_SENSE_PROP_POS_BIT7)/sizeof(Si2168_optionStruct), Si2168_DD_INT_SENSE_PROP_POS_BIT7},
};

Si2168_optionStruct    Si2168_DD_MODE_PROP_BW[] =
{
  {"BW_5MHZ",       5},
  {"BW_6MHZ",       6},
  {"BW_7MHZ",       7},
  {"BW_8MHZ",       8},
  {"BW_1D7MHZ",     2},
};
Si2168_optionStruct    Si2168_DD_MODE_PROP_MODULATION[] =
{
  {"DVBT",            2},
  {"DVBC",            3},
  {"DVBT2",           7},
  {"DVBS",            8},
  {"DVBS2",           9},
  {"DSS",            10},
  {"AUTO_DETECT",    15},
};
Si2168_optionStruct    Si2168_DD_MODE_PROP_INVERT_SPECTRUM[] =
{
  {"NORMAL",       0},
  {"INVERTED",     1},
};
Si2168_optionStruct    Si2168_DD_MODE_PROP_AUTO_DETECT[] =
{
  {"NONE",                  0},
  {"AUTO_DVB_T_T2",         1},
  {"AUTO_DVB_S_S2",         2},
  {"AUTO_DVB_S_S2_DSS",     3},
};
Si2168_fieldDicoStruct Si2168_DD_MODE_PROP_FIELDS[] =
{
  {"BW",              Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_mode.bw             , sizeof(Si2168_DD_MODE_PROP_BW             )/sizeof(Si2168_optionStruct), Si2168_DD_MODE_PROP_BW             },
  {"MODULATION",      Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_mode.modulation     , sizeof(Si2168_DD_MODE_PROP_MODULATION     )/sizeof(Si2168_optionStruct), Si2168_DD_MODE_PROP_MODULATION     },
  {"INVERT_SPECTRUM", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_mode.invert_spectrum, sizeof(Si2168_DD_MODE_PROP_INVERT_SPECTRUM)/sizeof(Si2168_optionStruct), Si2168_DD_MODE_PROP_INVERT_SPECTRUM},
  {"AUTO_DETECT",     Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_mode.auto_detect    , sizeof(Si2168_DD_MODE_PROP_AUTO_DETECT    )/sizeof(Si2168_optionStruct), Si2168_DD_MODE_PROP_AUTO_DETECT    },
};

Si2168_optionStruct    Si2168_DD_PER_RESOL_PROP_EXP[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     9}
};
Si2168_optionStruct    Si2168_DD_PER_RESOL_PROP_MANT[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     9}
};
Si2168_fieldDicoStruct Si2168_DD_PER_RESOL_PROP_FIELDS[] =
{
  {"EXP",  Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_per_resol.exp , sizeof(Si2168_DD_PER_RESOL_PROP_EXP )/sizeof(Si2168_optionStruct), Si2168_DD_PER_RESOL_PROP_EXP },
  {"MANT", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_per_resol.mant, sizeof(Si2168_DD_PER_RESOL_PROP_MANT)/sizeof(Si2168_optionStruct), Si2168_DD_PER_RESOL_PROP_MANT},
};

Si2168_optionStruct    Si2168_DD_RSQ_BER_THRESHOLD_PROP_EXP[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",     8}
};
Si2168_optionStruct    Si2168_DD_RSQ_BER_THRESHOLD_PROP_MANT[] =
{
  {"MIN_RANGE",     0},
  {"MAX_RANGE",    99}
};
Si2168_fieldDicoStruct Si2168_DD_RSQ_BER_THRESHOLD_PROP_FIELDS[] =
{
  {"EXP",  Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_rsq_ber_threshold.exp , sizeof(Si2168_DD_RSQ_BER_THRESHOLD_PROP_EXP )/sizeof(Si2168_optionStruct), Si2168_DD_RSQ_BER_THRESHOLD_PROP_EXP },
  {"MANT", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_rsq_ber_threshold.mant, sizeof(Si2168_DD_RSQ_BER_THRESHOLD_PROP_MANT)/sizeof(Si2168_optionStruct), Si2168_DD_RSQ_BER_THRESHOLD_PROP_MANT},
};

Si2168_optionStruct    Si2168_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE[] =
{
  {"MIN_RANGE",     1},
  {"MAX_RANGE",    30}
};
Si2168_fieldDicoStruct Si2168_DD_SSI_SQI_PARAM_PROP_FIELDS[] =
{
  {"SQI_AVERAGE", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ssi_sqi_param.sqi_average, sizeof(Si2168_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE)/sizeof(Si2168_optionStruct), Si2168_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE},
};

Si2168_optionStruct    Si2168_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ[] =
{
  {"MIN_RANGE",     0},
  {"MAX_RANGE", 14550}
};
Si2168_fieldDicoStruct Si2168_DD_TS_FREQ_PROP_FIELDS[] =
{
  {"REQ_FREQ_10KHZ", Si2168_UNSIGNED_INT, &Si2168_prop.dd_ts_freq.req_freq_10khz, sizeof(Si2168_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ)/sizeof(Si2168_optionStruct), Si2168_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ},
};

Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_MODE[] =
{
  {"TRISTATE",     0},
  {"OFF",          1},
  {"SERIAL",       3},
  {"PARALLEL",     6},
  {"GPIF",         7},
};
Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_CLOCK[] =
{
  {"AUTO_FIXED",     0},
  {"AUTO_ADAPT",     1},
  {"MANUAL",         2},
};
Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN[] =
{
  {"DISABLED",     0},
  {"ENABLED",      1},
};
Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY[] =
{
  {"NOT_INVERTED",     0},
  {"INVERTED",         1},
};
Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_SPECIAL[] =
{
  {"FULL_TS",            0},
  {"DATAS_TRISTATE",     1},
};
Si2168_fieldDicoStruct Si2168_DD_TS_MODE_PROP_FIELDS[] =
{
  {"MODE",          Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_mode.mode         , sizeof(Si2168_DD_TS_MODE_PROP_MODE         )/sizeof(Si2168_optionStruct), Si2168_DD_TS_MODE_PROP_MODE         },
  {"CLOCK",         Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_mode.clock        , sizeof(Si2168_DD_TS_MODE_PROP_CLOCK        )/sizeof(Si2168_optionStruct), Si2168_DD_TS_MODE_PROP_CLOCK        },
  {"CLK_GAPPED_EN", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_mode.clk_gapped_en, sizeof(Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN)/sizeof(Si2168_optionStruct), Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN},
  {"TS_ERR_POLARITY", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_mode.ts_err_polarity, sizeof(Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY)/sizeof(Si2168_optionStruct), Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY},
  {"SPECIAL",         Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_mode.special        , sizeof(Si2168_DD_TS_MODE_PROP_SPECIAL        )/sizeof(Si2168_optionStruct), Si2168_DD_TS_MODE_PROP_SPECIAL        },
};

Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH[] =
{
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE[] =
{
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH[] =
{
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE[] =
{
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT[] =
{
  {"NOT_INVERTED",     0},
  {"INVERTED",         1},
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT[] =
{
};
Si2168_fieldDicoStruct Si2168_DD_TS_SETUP_PAR_PROP_FIELDS[] =
{
  {"TS_DATA_STRENGTH", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_par.ts_data_strength, sizeof(Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH)/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH},
  {"TS_DATA_SHAPE",    Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_par.ts_data_shape   , sizeof(Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE   )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE   },
  {"TS_CLK_STRENGTH",  Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_par.ts_clk_strength , sizeof(Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH },
  {"TS_CLK_SHAPE",     Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_par.ts_clk_shape    , sizeof(Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE    )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE    },
  {"TS_CLK_INVERT",    Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_par.ts_clk_invert   , sizeof(Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT   )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT   },
  {"TS_CLK_SHIFT",     Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_par.ts_clk_shift    , sizeof(Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT    )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT    },
};

Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH[] =
{
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE[] =
{
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH[] =
{
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE[] =
{
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT[] =
{
  {"NOT_INVERTED",     0},
  {"INVERTED",         1},
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION[] =
{
  {"FIRST_BYTE",     0},
  {"FIRST_BIT",      1},
};
Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER[] =
{
  {"MSB_FIRST",     0},
  {"LSB_FIRST",     1},
};
Si2168_fieldDicoStruct Si2168_DD_TS_SETUP_SER_PROP_FIELDS[] =
{
  {"TS_DATA_STRENGTH", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_ser.ts_data_strength, sizeof(Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH)/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH},
  {"TS_DATA_SHAPE",    Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_ser.ts_data_shape   , sizeof(Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE   )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE   },
  {"TS_CLK_STRENGTH",  Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_ser.ts_clk_strength , sizeof(Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH },
  {"TS_CLK_SHAPE",     Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_ser.ts_clk_shape    , sizeof(Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE    )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE    },
  {"TS_CLK_INVERT",    Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_ser.ts_clk_invert   , sizeof(Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT   )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT   },
  {"TS_SYNC_DURATION", Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_ser.ts_sync_duration, sizeof(Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION)/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION},
  {"TS_BYTE_ORDER",    Si2168_UNSIGNED_CHAR, &Si2168_prop.dd_ts_setup_ser.ts_byte_order   , sizeof(Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER   )/sizeof(Si2168_optionStruct), Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER   },
};

Si2168_optionStruct    Si2168_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR[] =
{
};
Si2168_fieldDicoStruct Si2168_DVBC_ADC_CREST_FACTOR_PROP_FIELDS[] =
{
  {"CREST_FACTOR", Si2168_UNSIGNED_CHAR, &Si2168_prop.dvbc_adc_crest_factor.crest_factor, sizeof(Si2168_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR)/sizeof(Si2168_optionStruct), Si2168_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR},
};

Si2168_optionStruct    Si2168_DVBC_AFC_RANGE_PROP_RANGE_KHZ[] =
{
};
Si2168_fieldDicoStruct Si2168_DVBC_AFC_RANGE_PROP_FIELDS[] =
{
  {"RANGE_KHZ", Si2168_UNSIGNED_INT, &Si2168_prop.dvbc_afc_range.range_khz, sizeof(Si2168_DVBC_AFC_RANGE_PROP_RANGE_KHZ)/sizeof(Si2168_optionStruct), Si2168_DVBC_AFC_RANGE_PROP_RANGE_KHZ},
};

Si2168_optionStruct    Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION[] =
{
  {"AUTO",       0},
  {"QAM16",      7},
  {"QAM32",      8},
  {"QAM64",      9},
  {"QAM128",    10},
  {"QAM256",    11},
};
Si2168_fieldDicoStruct Si2168_DVBC_CONSTELLATION_PROP_FIELDS[] =
{
  {"CONSTELLATION", Si2168_UNSIGNED_CHAR, &Si2168_prop.dvbc_constellation.constellation, sizeof(Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION)/sizeof(Si2168_optionStruct), Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION},
};

Si2168_optionStruct    Si2168_DVBC_SYMBOL_RATE_PROP_RATE[] =
{
};
Si2168_fieldDicoStruct Si2168_DVBC_SYMBOL_RATE_PROP_FIELDS[] =
{
  {"RATE", Si2168_UNSIGNED_INT, &Si2168_prop.dvbc_symbol_rate.rate, sizeof(Si2168_DVBC_SYMBOL_RATE_PROP_RATE)/sizeof(Si2168_optionStruct), Si2168_DVBC_SYMBOL_RATE_PROP_RATE},
};



Si2168_optionStruct    Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR[] =
{
};
Si2168_fieldDicoStruct Si2168_DVBT2_ADC_CREST_FACTOR_PROP_FIELDS[] =
{
  {"CREST_FACTOR", Si2168_UNSIGNED_CHAR, &Si2168_prop.dvbt2_adc_crest_factor.crest_factor, sizeof(Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR)/sizeof(Si2168_optionStruct), Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR},
};

Si2168_optionStruct    Si2168_DVBT2_AFC_RANGE_PROP_RANGE_KHZ[] =
{
};
Si2168_fieldDicoStruct Si2168_DVBT2_AFC_RANGE_PROP_FIELDS[] =
{
  {"RANGE_KHZ", Si2168_UNSIGNED_INT, &Si2168_prop.dvbt2_afc_range.range_khz, sizeof(Si2168_DVBT2_AFC_RANGE_PROP_RANGE_KHZ)/sizeof(Si2168_optionStruct), Si2168_DVBT2_AFC_RANGE_PROP_RANGE_KHZ},
};

Si2168_optionStruct    Si2168_DVBT2_FEF_TUNER_PROP_TUNER_DELAY[] =
{
};
Si2168_optionStruct    Si2168_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME[] =
{
};
Si2168_optionStruct    Si2168_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME[] =
{
};
Si2168_fieldDicoStruct Si2168_DVBT2_FEF_TUNER_PROP_FIELDS[] =
{
  {"TUNER_DELAY",         Si2168_UNSIGNED_CHAR, &Si2168_prop.dvbt2_fef_tuner.tuner_delay        , sizeof(Si2168_DVBT2_FEF_TUNER_PROP_TUNER_DELAY        )/sizeof(Si2168_optionStruct), Si2168_DVBT2_FEF_TUNER_PROP_TUNER_DELAY        },
  {"TUNER_FREEZE_TIME",   Si2168_UNSIGNED_CHAR, &Si2168_prop.dvbt2_fef_tuner.tuner_freeze_time  , sizeof(Si2168_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME  )/sizeof(Si2168_optionStruct), Si2168_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME  },
  {"TUNER_UNFREEZE_TIME", Si2168_UNSIGNED_CHAR, &Si2168_prop.dvbt2_fef_tuner.tuner_unfreeze_time, sizeof(Si2168_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME)/sizeof(Si2168_optionStruct), Si2168_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME},
};


Si2168_optionStruct    Si2168_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR[] =
{
};
Si2168_fieldDicoStruct Si2168_DVBT_ADC_CREST_FACTOR_PROP_FIELDS[] =
{
  {"CREST_FACTOR", Si2168_UNSIGNED_CHAR, &Si2168_prop.dvbt_adc_crest_factor.crest_factor, sizeof(Si2168_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR)/sizeof(Si2168_optionStruct), Si2168_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR},
};

Si2168_optionStruct    Si2168_DVBT_AFC_RANGE_PROP_RANGE_KHZ[] =
{
};
Si2168_fieldDicoStruct Si2168_DVBT_AFC_RANGE_PROP_FIELDS[] =
{
  {"RANGE_KHZ", Si2168_UNSIGNED_INT, &Si2168_prop.dvbt_afc_range.range_khz, sizeof(Si2168_DVBT_AFC_RANGE_PROP_RANGE_KHZ)/sizeof(Si2168_optionStruct), Si2168_DVBT_AFC_RANGE_PROP_RANGE_KHZ},
};

Si2168_optionStruct    Si2168_DVBT_HIERARCHY_PROP_STREAM[] =
{
  {"HP",     0},
  {"LP",     1},
};
Si2168_fieldDicoStruct Si2168_DVBT_HIERARCHY_PROP_FIELDS[] =
{
  {"STREAM", Si2168_UNSIGNED_CHAR, &Si2168_prop.dvbt_hierarchy.stream, sizeof(Si2168_DVBT_HIERARCHY_PROP_STREAM)/sizeof(Si2168_optionStruct), Si2168_DVBT_HIERARCHY_PROP_STREAM},
};


Si2168_optionStruct    Si2168_MASTER_IEN_PROP_DDIEN[] =
{
  {"OFF",     0},
  {"ON",      1},
};
Si2168_optionStruct    Si2168_MASTER_IEN_PROP_SCANIEN[] =
{
  {"OFF",     0},
  {"ON",      1},
};
Si2168_optionStruct    Si2168_MASTER_IEN_PROP_ERRIEN[] =
{
  {"OFF",     0},
  {"ON",      1},
};
Si2168_optionStruct    Si2168_MASTER_IEN_PROP_CTSIEN[] =
{
  {"OFF",     0},
  {"ON",      1},
};
Si2168_fieldDicoStruct Si2168_MASTER_IEN_PROP_FIELDS[] =
{
  {"DDIEN",   Si2168_UNSIGNED_CHAR, &Si2168_prop.master_ien.ddien  , sizeof(Si2168_MASTER_IEN_PROP_DDIEN  )/sizeof(Si2168_optionStruct), Si2168_MASTER_IEN_PROP_DDIEN  },
  {"SCANIEN", Si2168_UNSIGNED_CHAR, &Si2168_prop.master_ien.scanien, sizeof(Si2168_MASTER_IEN_PROP_SCANIEN)/sizeof(Si2168_optionStruct), Si2168_MASTER_IEN_PROP_SCANIEN},
  {"ERRIEN",  Si2168_UNSIGNED_CHAR, &Si2168_prop.master_ien.errien , sizeof(Si2168_MASTER_IEN_PROP_ERRIEN )/sizeof(Si2168_optionStruct), Si2168_MASTER_IEN_PROP_ERRIEN },
  {"CTSIEN",  Si2168_UNSIGNED_CHAR, &Si2168_prop.master_ien.ctsien , sizeof(Si2168_MASTER_IEN_PROP_CTSIEN )/sizeof(Si2168_optionStruct), Si2168_MASTER_IEN_PROP_CTSIEN },
};

Si2168_optionStruct    Si2168_SCAN_FMAX_PROP_SCAN_FMAX[] =
{
};
Si2168_fieldDicoStruct Si2168_SCAN_FMAX_PROP_FIELDS[] =
{
  {"SCAN_FMAX", Si2168_UNSIGNED_INT, &Si2168_prop.scan_fmax.scan_fmax, sizeof(Si2168_SCAN_FMAX_PROP_SCAN_FMAX)/sizeof(Si2168_optionStruct), Si2168_SCAN_FMAX_PROP_SCAN_FMAX},
};

Si2168_optionStruct    Si2168_SCAN_FMIN_PROP_SCAN_FMIN[] =
{
};
Si2168_fieldDicoStruct Si2168_SCAN_FMIN_PROP_FIELDS[] =
{
  {"SCAN_FMIN", Si2168_UNSIGNED_INT, &Si2168_prop.scan_fmin.scan_fmin, sizeof(Si2168_SCAN_FMIN_PROP_SCAN_FMIN)/sizeof(Si2168_optionStruct), Si2168_SCAN_FMIN_PROP_SCAN_FMIN},
};

Si2168_optionStruct    Si2168_SCAN_IEN_PROP_BUZIEN[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_SCAN_IEN_PROP_REQIEN[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_fieldDicoStruct Si2168_SCAN_IEN_PROP_FIELDS[] =
{
  {"BUZIEN", Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_ien.buzien, sizeof(Si2168_SCAN_IEN_PROP_BUZIEN)/sizeof(Si2168_optionStruct), Si2168_SCAN_IEN_PROP_BUZIEN},
  {"REQIEN", Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_ien.reqien, sizeof(Si2168_SCAN_IEN_PROP_REQIEN)/sizeof(Si2168_optionStruct), Si2168_SCAN_IEN_PROP_REQIEN},
};

Si2168_optionStruct    Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_SCAN_INT_SENSE_PROP_REQNEGEN[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_optionStruct    Si2168_SCAN_INT_SENSE_PROP_REQPOSEN[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_fieldDicoStruct Si2168_SCAN_INT_SENSE_PROP_FIELDS[] =
{
  {"BUZNEGEN", Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_int_sense.buznegen, sizeof(Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN)/sizeof(Si2168_optionStruct), Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN},
  {"REQNEGEN", Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_int_sense.reqnegen, sizeof(Si2168_SCAN_INT_SENSE_PROP_REQNEGEN)/sizeof(Si2168_optionStruct), Si2168_SCAN_INT_SENSE_PROP_REQNEGEN},
  {"BUZPOSEN", Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_int_sense.buzposen, sizeof(Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN)/sizeof(Si2168_optionStruct), Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN},
  {"REQPOSEN", Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_int_sense.reqposen, sizeof(Si2168_SCAN_INT_SENSE_PROP_REQPOSEN)/sizeof(Si2168_optionStruct), Si2168_SCAN_INT_SENSE_PROP_REQPOSEN},
};


Si2168_optionStruct    Si2168_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX[] =
{
};
Si2168_fieldDicoStruct Si2168_SCAN_SYMB_RATE_MAX_PROP_FIELDS[] =
{
  {"SCAN_SYMB_RATE_MAX", Si2168_UNSIGNED_INT, &Si2168_prop.scan_symb_rate_max.scan_symb_rate_max, sizeof(Si2168_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX)/sizeof(Si2168_optionStruct), Si2168_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX},
};

Si2168_optionStruct    Si2168_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN[] =
{
};
Si2168_fieldDicoStruct Si2168_SCAN_SYMB_RATE_MIN_PROP_FIELDS[] =
{
  {"SCAN_SYMB_RATE_MIN", Si2168_UNSIGNED_INT, &Si2168_prop.scan_symb_rate_min.scan_symb_rate_min, sizeof(Si2168_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN)/sizeof(Si2168_optionStruct), Si2168_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN},
};

Si2168_optionStruct    Si2168_SCAN_TER_CONFIG_PROP_MODE[] =
{
  {"BLIND_SCAN",       0},
  {"MAPPING_SCAN",     1},
  {"BLIND_LOCK",       2},
};
Si2168_optionStruct    Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW[] =
{
  {"6MHZ",     1},
  {"7MHZ",     2},
  {"8MHZ",     3},
};
Si2168_optionStruct    Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG[] =
{
  {"DISABLE",     0},
  {"ENABLE",      1},
};
Si2168_fieldDicoStruct Si2168_SCAN_TER_CONFIG_PROP_FIELDS[] =
{
  {"MODE",          Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_ter_config.mode         , sizeof(Si2168_SCAN_TER_CONFIG_PROP_MODE         )/sizeof(Si2168_optionStruct), Si2168_SCAN_TER_CONFIG_PROP_MODE         },
  {"ANALOG_BW",     Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_ter_config.analog_bw    , sizeof(Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW    )/sizeof(Si2168_optionStruct), Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW    },
  {"SEARCH_ANALOG", Si2168_UNSIGNED_CHAR, &Si2168_prop.scan_ter_config.search_analog, sizeof(Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG)/sizeof(Si2168_optionStruct), Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG},
};


Si2168_propertyInfoStruct Si2168_propertyDictionary[] =
{
  {Si2168_COMMON, "MASTER_IEN"            , sizeof(Si2168_MASTER_IEN_PROP_FIELDS            )/sizeof(Si2168_fieldDicoStruct), Si2168_MASTER_IEN_PROP_FIELDS             , Si2168_MASTER_IEN_PROP             },
  {Si2168_DD    , "DD_BER_RESOL"          , sizeof(Si2168_DD_BER_RESOL_PROP_FIELDS          )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_BER_RESOL_PROP_FIELDS           , Si2168_DD_BER_RESOL_PROP           },
  {Si2168_DD    , "DD_CBER_RESOL"         , sizeof(Si2168_DD_CBER_RESOL_PROP_FIELDS         )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_CBER_RESOL_PROP_FIELDS          , Si2168_DD_CBER_RESOL_PROP          },

  {Si2168_DD    , "DD_FER_RESOL"          , sizeof(Si2168_DD_FER_RESOL_PROP_FIELDS          )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_FER_RESOL_PROP_FIELDS           , Si2168_DD_FER_RESOL_PROP           },
  {Si2168_DD    , "DD_IEN"                , sizeof(Si2168_DD_IEN_PROP_FIELDS                )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_IEN_PROP_FIELDS                 , Si2168_DD_IEN_PROP                 },
  {Si2168_DD    , "DD_IF_INPUT_FREQ"      , sizeof(Si2168_DD_IF_INPUT_FREQ_PROP_FIELDS      )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_IF_INPUT_FREQ_PROP_FIELDS       , Si2168_DD_IF_INPUT_FREQ_PROP       },
  {Si2168_DD    , "DD_INT_SENSE"          , sizeof(Si2168_DD_INT_SENSE_PROP_FIELDS          )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_INT_SENSE_PROP_FIELDS           , Si2168_DD_INT_SENSE_PROP           },
  {Si2168_DD    , "DD_MODE"               , sizeof(Si2168_DD_MODE_PROP_FIELDS               )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_MODE_PROP_FIELDS                , Si2168_DD_MODE_PROP                },
  {Si2168_DD    , "DD_PER_RESOL"          , sizeof(Si2168_DD_PER_RESOL_PROP_FIELDS          )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_PER_RESOL_PROP_FIELDS           , Si2168_DD_PER_RESOL_PROP           },
  {Si2168_DD    , "DD_RSQ_BER_THRESHOLD"  , sizeof(Si2168_DD_RSQ_BER_THRESHOLD_PROP_FIELDS  )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_RSQ_BER_THRESHOLD_PROP_FIELDS   , Si2168_DD_RSQ_BER_THRESHOLD_PROP   },
  {Si2168_DD    , "DD_SSI_SQI_PARAM"      , sizeof(Si2168_DD_SSI_SQI_PARAM_PROP_FIELDS      )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_SSI_SQI_PARAM_PROP_FIELDS       , Si2168_DD_SSI_SQI_PARAM_PROP       },
  {Si2168_DD    , "DD_TS_FREQ"            , sizeof(Si2168_DD_TS_FREQ_PROP_FIELDS            )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_TS_FREQ_PROP_FIELDS             , Si2168_DD_TS_FREQ_PROP             },
  {Si2168_DD    , "DD_TS_MODE"            , sizeof(Si2168_DD_TS_MODE_PROP_FIELDS            )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_TS_MODE_PROP_FIELDS             , Si2168_DD_TS_MODE_PROP             },
  {Si2168_DD    , "DD_TS_SETUP_PAR"       , sizeof(Si2168_DD_TS_SETUP_PAR_PROP_FIELDS       )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_TS_SETUP_PAR_PROP_FIELDS        , Si2168_DD_TS_SETUP_PAR_PROP        },
  {Si2168_DD    , "DD_TS_SETUP_SER"       , sizeof(Si2168_DD_TS_SETUP_SER_PROP_FIELDS       )/sizeof(Si2168_fieldDicoStruct), Si2168_DD_TS_SETUP_SER_PROP_FIELDS        , Si2168_DD_TS_SETUP_SER_PROP        },
  {Si2168_DVBC  , "DVBC_ADC_CREST_FACTOR" , sizeof(Si2168_DVBC_ADC_CREST_FACTOR_PROP_FIELDS )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBC_ADC_CREST_FACTOR_PROP_FIELDS  , Si2168_DVBC_ADC_CREST_FACTOR_PROP  },
  {Si2168_DVBC  , "DVBC_AFC_RANGE"        , sizeof(Si2168_DVBC_AFC_RANGE_PROP_FIELDS        )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBC_AFC_RANGE_PROP_FIELDS         , Si2168_DVBC_AFC_RANGE_PROP         },
  {Si2168_DVBC  , "DVBC_CONSTELLATION"    , sizeof(Si2168_DVBC_CONSTELLATION_PROP_FIELDS    )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBC_CONSTELLATION_PROP_FIELDS     , Si2168_DVBC_CONSTELLATION_PROP     },
  {Si2168_DVBC  , "DVBC_SYMBOL_RATE"      , sizeof(Si2168_DVBC_SYMBOL_RATE_PROP_FIELDS      )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBC_SYMBOL_RATE_PROP_FIELDS       , Si2168_DVBC_SYMBOL_RATE_PROP       },


  {Si2168_DVBT  , "DVBT_ADC_CREST_FACTOR" , sizeof(Si2168_DVBT_ADC_CREST_FACTOR_PROP_FIELDS )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBT_ADC_CREST_FACTOR_PROP_FIELDS  , Si2168_DVBT_ADC_CREST_FACTOR_PROP  },
  {Si2168_DVBT  , "DVBT_AFC_RANGE"        , sizeof(Si2168_DVBT_AFC_RANGE_PROP_FIELDS        )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBT_AFC_RANGE_PROP_FIELDS         , Si2168_DVBT_AFC_RANGE_PROP         },
  {Si2168_DVBT  , "DVBT_HIERARCHY"        , sizeof(Si2168_DVBT_HIERARCHY_PROP_FIELDS        )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBT_HIERARCHY_PROP_FIELDS         , Si2168_DVBT_HIERARCHY_PROP         },

  {Si2168_DVBT2 , "DVBT2_ADC_CREST_FACTOR", sizeof(Si2168_DVBT2_ADC_CREST_FACTOR_PROP_FIELDS)/sizeof(Si2168_fieldDicoStruct), Si2168_DVBT2_ADC_CREST_FACTOR_PROP_FIELDS , Si2168_DVBT2_ADC_CREST_FACTOR_PROP },
  {Si2168_DVBT2 , "DVBT2_AFC_RANGE"       , sizeof(Si2168_DVBT2_AFC_RANGE_PROP_FIELDS       )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBT2_AFC_RANGE_PROP_FIELDS        , Si2168_DVBT2_AFC_RANGE_PROP        },
  {Si2168_DVBT2 , "DVBT2_FEF_TUNER"       , sizeof(Si2168_DVBT2_FEF_TUNER_PROP_FIELDS       )/sizeof(Si2168_fieldDicoStruct), Si2168_DVBT2_FEF_TUNER_PROP_FIELDS        , Si2168_DVBT2_FEF_TUNER_PROP        },

  {Si2168_SCAN  , "SCAN_FMAX"             , sizeof(Si2168_SCAN_FMAX_PROP_FIELDS             )/sizeof(Si2168_fieldDicoStruct), Si2168_SCAN_FMAX_PROP_FIELDS              , Si2168_SCAN_FMAX_PROP              },
  {Si2168_SCAN  , "SCAN_FMIN"             , sizeof(Si2168_SCAN_FMIN_PROP_FIELDS             )/sizeof(Si2168_fieldDicoStruct), Si2168_SCAN_FMIN_PROP_FIELDS              , Si2168_SCAN_FMIN_PROP              },
  {Si2168_SCAN  , "SCAN_IEN"              , sizeof(Si2168_SCAN_IEN_PROP_FIELDS              )/sizeof(Si2168_fieldDicoStruct), Si2168_SCAN_IEN_PROP_FIELDS               , Si2168_SCAN_IEN_PROP               },
  {Si2168_SCAN  , "SCAN_INT_SENSE"        , sizeof(Si2168_SCAN_INT_SENSE_PROP_FIELDS        )/sizeof(Si2168_fieldDicoStruct), Si2168_SCAN_INT_SENSE_PROP_FIELDS         , Si2168_SCAN_INT_SENSE_PROP         },

  {Si2168_SCAN  , "SCAN_SYMB_RATE_MAX"    , sizeof(Si2168_SCAN_SYMB_RATE_MAX_PROP_FIELDS    )/sizeof(Si2168_fieldDicoStruct), Si2168_SCAN_SYMB_RATE_MAX_PROP_FIELDS     , Si2168_SCAN_SYMB_RATE_MAX_PROP     },
  {Si2168_SCAN  , "SCAN_SYMB_RATE_MIN"    , sizeof(Si2168_SCAN_SYMB_RATE_MIN_PROP_FIELDS    )/sizeof(Si2168_fieldDicoStruct), Si2168_SCAN_SYMB_RATE_MIN_PROP_FIELDS     , Si2168_SCAN_SYMB_RATE_MIN_PROP     },
  {Si2168_SCAN  , "SCAN_TER_CONFIG"       , sizeof(Si2168_SCAN_TER_CONFIG_PROP_FIELDS       )/sizeof(Si2168_fieldDicoStruct), Si2168_SCAN_TER_CONFIG_PROP_FIELDS        , Si2168_SCAN_TER_CONFIG_PROP        },

};
/* _properties_strings_insertion_point */

int   Si2168_PropertyNames        (L1_Si2168_Context *api, char *msg)
{
  int propIndex;
  int size;
  api = api; /* to avoid 'unused parameter' warning */
  size = sizeof(Si2168_propertyDictionary)/sizeof(Si2168_propertyInfoStruct);
  msg[0]='\0';
  for (propIndex=0; propIndex<size; propIndex++)
  {
    strcat(msg,Si2168_propertyDictionary[propIndex].name);
    if (propIndex< (size-1)) strcat(msg," ");
  }
  return propIndex;
};

int   Si2168_PropertyIndex        (L1_Si2168_Context *api, const char *property)
{
  int propIndex;
  int size;
  api = api; /* to avoid 'unused parameter' warning */
  size = sizeof(Si2168_propertyDictionary)/sizeof(Si2168_propertyInfoStruct);
  for (propIndex=0; propIndex <size; propIndex++)
  {
    if (strcmp_nocase(property, Si2168_propertyDictionary[propIndex].name)==0)
    {
      return propIndex;
    }
  }
  return -1;
};

int   Si2168_PropertyFields       (L1_Si2168_Context *api, int propIndex, char *msg)
{
  int fieldIndex;
  api = api; /* to avoid 'unused parameter' warning */
  msg[0]='\0';
  for (fieldIndex=0; fieldIndex < Si2168_propertyDictionary[propIndex].nbFields; fieldIndex++)
  {
    strcat(msg, Si2168_propertyDictionary[propIndex].field[fieldIndex].name);
    if (fieldIndex< (Si2168_propertyDictionary[propIndex].nbFields-1)) strcat(msg," ");
  }
  return -1;
};

int   Si2168_PropertyFieldIndex   (L1_Si2168_Context *api, int propIndex, char *field)
{
  int fieldIndex;
  api = api; /* to avoid 'unused parameter' warning */
  for (fieldIndex=0; fieldIndex < Si2168_propertyDictionary[propIndex].nbFields; fieldIndex++)
  {
    if (strcmp_nocase(field, Si2168_propertyDictionary[propIndex].field[fieldIndex].name)==0)
    {
      return fieldIndex;
    }
  }
  return -1;
};

int   Si2168_PropertyFieldEnums   (L1_Si2168_Context *api, int propIndex, int fieldIndex, char *msg)
{
  int enumIndex;
  api = api; /* to avoid 'unused parameter' warning */
  msg[0]='\0';
  for (enumIndex=0; enumIndex < Si2168_propertyDictionary[propIndex].field[fieldIndex].nbOptions; enumIndex++)
  {
    strcat(msg, Si2168_propertyDictionary[propIndex].field[fieldIndex].option[enumIndex].name);
    if (enumIndex < (Si2168_propertyDictionary[propIndex].field[fieldIndex].nbOptions-1)) strcat(msg," ");
  }
  return enumIndex;
};

int   Si2168_PropertyFieldRanges  (L1_Si2168_Context *api, int propIndex, int fieldIndex, char *msg)
{
  int index;
  char buf[10];
  api = api; /* to avoid 'unused parameter' warning */
  msg[0]='\0';
  for (index=0; index < 2; index++)
  {
    itoa(Si2168_propertyDictionary[propIndex].field[fieldIndex].option[index].value,buf,10);
    strcat(msg, buf);
    if (index < 1) strcat(msg," - ");
  }
  return index;
};

int   Si2168_GetValueFromEntry    (L1_Si2168_Context *api, Si2168_fieldDicoStruct field, char *entry, int *value)
{
  int optionIndex;
  api = api; /* to avoid 'unused parameter' warning */

  if (!strcmp_nocase(field.option[0].name,"MIN_RANGE"))
  {
    if (field.datatype== Si2168_UNSIGNED_INT || field.datatype== Si2168_UNSIGNED_CHAR)
    {
      *value = (unsigned int)atoi(entry);
      return 1;
    }
    else if (field.datatype == Si2168_SIGNED_INT || field.datatype == Si2168_SIGNED_CHAR)
    {
      *value = (int)atoi(entry);
      return 1;
    }
  }
  for (optionIndex=0; optionIndex < field.nbOptions; optionIndex++)
  {
    if (strcmp_nocase(entry, field.option[optionIndex].name     )==0)
    {
      *value = field.option[optionIndex].value;
      return 1;
    }
  }
  return 0;
};

char *Si2168_GetEnumFromValue     (L1_Si2168_Context *api, Si2168_fieldDicoStruct field, int iValue)
{
  int optionIndex;
  api = api; /* to avoid 'unused parameter' warning */
  if (field.nbOptions ==0 )
    {
    sprintf(optionText, "%d", iValue);
    return optionText;
  }
  for (optionIndex=0; optionIndex < field.nbOptions; optionIndex++)
  {
    if (iValue == field.option[optionIndex].value)
    {
      return (char*)field.option[optionIndex].name;
    }
  }
  return (char*)NULL;
};

int   Si2168_GetValueFromField    (L1_Si2168_Context *api, Si2168_fieldDicoStruct field)
{
  short intValue;
  short *fieldAsInt;
  short iTmp=0;
  fieldAsInt = &intValue;

  if (field.datatype == Si2168_SIGNED_INT   )
  {
      fieldAsInt= (short *) ((field.pField-(void *) &Si2168_prop)+ (void *)api->prop);
  }
  else if (field.datatype == Si2168_UNSIGNED_CHAR)
  {
      iTmp = *((unsigned char *) ((field.pField-(void *) &Si2168_prop)+ (void *)api->prop));
      fieldAsInt = &iTmp;
  }
  else if (field.datatype == Si2168_SIGNED_CHAR)
  {
      iTmp= *((char *) ((field.pField-(void *) &Si2168_prop)+ (void *)api->prop));
      fieldAsInt = &iTmp;
  }
  else /* if (field.datatype == Si2168_UNSIGNED_INT) */
  {
      fieldAsInt = (unsigned short *) ((field.pField-(void *) &Si2168_prop)+ (void *)api->prop);
  }

  return  *fieldAsInt;
};

char *Si2168_GetEnumFromField     (L1_Si2168_Context *api, Si2168_fieldDicoStruct field)
{
  int intValue;
  int *fieldAsInt;
  int iTmp=0;
  fieldAsInt = &intValue;

  if (field.datatype == Si2168_SIGNED_INT   )
  {
      fieldAsInt= (int *) ((field.pField-(void *) &Si2168_prop) + (void *)api->prop);
  }
  else if (field.datatype == Si2168_UNSIGNED_CHAR)
  {
      iTmp = *((unsigned char *) ((field.pField-(void *) &Si2168_prop) + (void *)api->prop));
      fieldAsInt = &iTmp;
  }
  else if (field.datatype == Si2168_SIGNED_CHAR)
  {
      iTmp= *((char *) ((field.pField-(void *) &Si2168_prop) + (void *)api->prop));
      fieldAsInt = &iTmp;
  }
  else /* if (field.datatype == Si2168_UNSIGNED_INT) */
  {
      fieldAsInt = (unsigned int *)((field.pField-(void *) &Si2168_prop) + (void *)api->prop);
  }

  return Si2168_GetEnumFromValue(api, field, *fieldAsInt);
};

int   Si2168_SetFieldFromEntry    (L1_Si2168_Context *api, Si2168_fieldDicoStruct field, char *entry)
{
  int *fieldAsInt;
  int iTmp=0;
  fieldAsInt = &iTmp;
  if (Si2168_GetValueFromEntry(api, field, entry, fieldAsInt) == 0) return 0;
  if (field.datatype == Si2168_SIGNED_INT )
  {
    *( (int *) ((field.pField-(void *) &Si2168_prop) + (void *)api->prop))=iTmp;
  }
  else if (field.datatype == Si2168_UNSIGNED_CHAR)
  {
    *( (unsigned char *) ((field.pField-(void *) &Si2168_prop) + (void *)api->prop))=iTmp;
  }
  else if (field.datatype == Si2168_SIGNED_CHAR)
  {
    *( (char *) ((field.pField-(void *) &Si2168_prop) + (void *)api->prop))=iTmp;
  }
  else /* if (field.datatype == Si2168_UNSIGNED_INT) */
  {
    *( (unsigned int *) ((field.pField-(void *) &Si2168_prop) + (void *)api->prop))=iTmp;
  }

  return 1;
};

void  Si2168_showProperty         (L1_Si2168_Context *api, int propIndex)
{
  int fieldIndex;
  for (fieldIndex=0; fieldIndex<Si2168_propertyDictionary[propIndex].nbFields; fieldIndex++)
  {
    printf("%s:%s:%s = ", Si2168_featureText[Si2168_propertyDictionary[propIndex].feature], Si2168_propertyDictionary[propIndex].name, Si2168_propertyDictionary[propIndex].field[fieldIndex].name);
    printf("<%s>\n", Si2168_GetEnumFromField(api, Si2168_propertyDictionary[propIndex].field[fieldIndex]));
  }

}

void  Si2168_showPropertyNamed    (L1_Si2168_Context *api, char *property)
{
  int propIndex;

  if ((propIndex = Si2168_PropertyIndex(api, property)) >= 0)
  {
    Si2168_showProperty(api, propIndex);
  }
  else
  {
    printf("Unknown property '%s'\n", property);
  }

}

int   Si2168_showProperties       (L1_Si2168_Context *api, Si2168_featureEnum feature)
{
  int propertyIndex;
  int dictSize;

  dictSize=sizeof(Si2168_propertyDictionary)/sizeof(Si2168_propertyInfoStruct);

  printf("\n%s current properties:\n\n", Si2168_featureText[feature]);

  for (propertyIndex=0; propertyIndex<dictSize; propertyIndex++)
  {
    if (Si2168_propertyDictionary[propertyIndex].feature != feature) continue;
    Si2168_showProperty(api, propertyIndex);
  }
  return 0;
}

void  Si2168_setupProperty        (L1_Si2168_Context *api, int propIndex)
{
  #define LMAX 20
  char entry[LMAX];
  char setup_msg[1000];
  int fieldIndex;
  char isRangeType = 1;

  for (fieldIndex=0; fieldIndex<Si2168_propertyDictionary[propIndex].nbFields; fieldIndex++)
  {
    isRangeType = (strcmp_nocase(Si2168_propertyDictionary[propIndex].field[fieldIndex].option->name,"MIN_RANGE") == 0);

    printf("%s:%s:%s = ", Si2168_featureText[Si2168_propertyDictionary[propIndex].feature], Si2168_propertyDictionary[propIndex].name, Si2168_propertyDictionary[propIndex].field[fieldIndex].name);

    if ( isRangeType )
    {
      printf("<%d>\n", Si2168_GetValueFromField(api, Si2168_propertyDictionary[propIndex].field[fieldIndex]));
      Si2168_PropertyFieldRanges(api, propIndex, fieldIndex, setup_msg);
      printf("Range: <%s>  ",setup_msg);
    }
    else
    {
      Si2168_PropertyFieldEnums(api, propIndex, fieldIndex, setup_msg);
      printf("<%s>\n Options: <%s>\n", Si2168_GetEnumFromField(api, Si2168_propertyDictionary[propIndex].field[fieldIndex]), setup_msg);
    }

    fgets(entry,LMAX,stdin);

    if (strlen(entry) > 1)
    {
      entry[strlen(entry)-1]='\0';
      if (!Si2168_SetFieldFromEntry(api, Si2168_propertyDictionary[propIndex].field[fieldIndex], entry))
      {
        printf("Invalid entry!\n");
      }
    }

    if (isRangeType)
      printf("<%d>\n\n", Si2168_GetValueFromField(api, Si2168_propertyDictionary[propIndex].field[fieldIndex]));
    else
      printf("<%s>\n\n", Si2168_GetEnumFromField(api, Si2168_propertyDictionary[propIndex].field[fieldIndex]));
  }
}

int   Si2168_setupProperties      (L1_Si2168_Context *api, Si2168_featureEnum feature)
{
  int propertyIndex;
  int dictSize;

  dictSize=sizeof(Si2168_propertyDictionary)/sizeof(Si2168_propertyInfoStruct);

  printf("\n%s properties:\n\n", Si2168_featureText[feature]);

  for (propertyIndex=0; propertyIndex<dictSize; propertyIndex++)
  {
    if (Si2168_propertyDictionary[propertyIndex].feature != feature) continue;
    Si2168_setupProperty(api, propertyIndex);
  }
  return 0;
}

#endif /* Si2168_COMMAND_LINE_APPLICATION */

/* _property_string_functions_insertion_start */
#ifdef    Si2168_COMMAND_LINE_APPLICATION
int Si2168_showCOMMONProperties    (L1_Si2168_Context *api)
{
  return Si2168_showProperties(api, Si2168_COMMON);
};

int Si2168_showDDProperties        (L1_Si2168_Context *api)
{
  return Si2168_showProperties(api, Si2168_DD);
};

int Si2168_showDVBCProperties      (L1_Si2168_Context *api)
{
  return Si2168_showProperties(api, Si2168_DVBC);
};



int Si2168_showDVBTProperties      (L1_Si2168_Context *api)
{
  return Si2168_showProperties(api, Si2168_DVBT);
};


int Si2168_showDVBT2Properties     (L1_Si2168_Context *api)
{
  return Si2168_showProperties(api, Si2168_DVBT2);
};


int Si2168_showSCANProperties      (L1_Si2168_Context *api)
{
  return Si2168_showProperties(api, Si2168_SCAN);
};

int Si2168_showAllProperties       (L1_Si2168_Context *api)
{
  Si2168_showCOMMONProperties    (api);
  Si2168_showDDProperties        (api);
  Si2168_showDVBCProperties      (api);


  Si2168_showDVBTProperties      (api);

  Si2168_showDVBT2Properties     (api);

  Si2168_showSCANProperties      (api);
  return 0;
};

#endif /* Si2168_COMMAND_LINE_APPLICATION */
void Si2168_setupCOMMONDefaults     (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupCOMMONDefaults    \n");
  api->prop->master_ien.ddien                       = Si2168_MASTER_IEN_PROP_DDIEN_OFF   ; /* (default 'OFF') */
  api->prop->master_ien.scanien                     = Si2168_MASTER_IEN_PROP_SCANIEN_OFF ; /* (default 'OFF') */
  api->prop->master_ien.errien                      = Si2168_MASTER_IEN_PROP_ERRIEN_OFF  ; /* (default 'OFF') */
  api->prop->master_ien.ctsien                      = Si2168_MASTER_IEN_PROP_CTSIEN_OFF  ; /* (default 'OFF') */

}

void Si2168_setupDDDefaults         (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupDDDefaults        \n");
  api->prop->dd_ber_resol.exp                       =     7; /* (default     7) */
  api->prop->dd_ber_resol.mant                      =     1; /* (default     1) */

  api->prop->dd_cber_resol.exp                      =     5; /* (default     5) */
  api->prop->dd_cber_resol.mant                     =     1; /* (default     1) */


  api->prop->dd_fer_resol.exp                       =     2; /* (default     3) */
  api->prop->dd_fer_resol.mant                      =     1; /* (default     1) */

  api->prop->dd_ien.ien_bit0                        = Si2168_DD_IEN_PROP_IEN_BIT0_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_ien.ien_bit1                        = Si2168_DD_IEN_PROP_IEN_BIT1_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_ien.ien_bit2                        = Si2168_DD_IEN_PROP_IEN_BIT2_ENABLE  ; /* (default 'DISABLE') Used for DVB-T/T2 'LOCK'       */
  api->prop->dd_ien.ien_bit3                        = Si2168_DD_IEN_PROP_IEN_BIT3_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_ien.ien_bit4                        = Si2168_DD_IEN_PROP_IEN_BIT4_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_ien.ien_bit5                        = Si2168_DD_IEN_PROP_IEN_BIT5_ENABLE  ; /* (default 'DISABLE') Used for DVB-T/T2 'NO_CHANNEL' */
  api->prop->dd_ien.ien_bit6                        = Si2168_DD_IEN_PROP_IEN_BIT6_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_ien.ien_bit7                        = Si2168_DD_IEN_PROP_IEN_BIT7_DISABLE ; /* (default 'DISABLE') */

  api->prop->dd_if_input_freq.offset                =  5000; /* (default  5000) */

  api->prop->dd_int_sense.neg_bit0                  = Si2168_DD_INT_SENSE_PROP_NEG_BIT0_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.neg_bit1                  = Si2168_DD_INT_SENSE_PROP_NEG_BIT1_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.neg_bit2                  = Si2168_DD_INT_SENSE_PROP_NEG_BIT2_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.neg_bit3                  = Si2168_DD_INT_SENSE_PROP_NEG_BIT3_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.neg_bit4                  = Si2168_DD_INT_SENSE_PROP_NEG_BIT4_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.neg_bit5                  = Si2168_DD_INT_SENSE_PROP_NEG_BIT5_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.neg_bit6                  = Si2168_DD_INT_SENSE_PROP_NEG_BIT6_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.neg_bit7                  = Si2168_DD_INT_SENSE_PROP_NEG_BIT7_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.pos_bit0                  = Si2168_DD_INT_SENSE_PROP_POS_BIT0_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.pos_bit1                  = Si2168_DD_INT_SENSE_PROP_POS_BIT1_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.pos_bit2                  = Si2168_DD_INT_SENSE_PROP_POS_BIT2_ENABLE  ; /* (default 'DISABLE') Sense DVB-T/T2 'LOCK'       changes from '0' to '1' */
  api->prop->dd_int_sense.pos_bit3                  = Si2168_DD_INT_SENSE_PROP_POS_BIT3_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.pos_bit4                  = Si2168_DD_INT_SENSE_PROP_POS_BIT4_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.pos_bit5                  = Si2168_DD_INT_SENSE_PROP_POS_BIT5_ENABLE  ; /* (default 'DISABLE') Sense DVB-T/T2 'NO_CHANNEL' changes from '0' to '1' */
  api->prop->dd_int_sense.pos_bit6                  = Si2168_DD_INT_SENSE_PROP_POS_BIT6_DISABLE ; /* (default 'DISABLE') */
  api->prop->dd_int_sense.pos_bit7                  = Si2168_DD_INT_SENSE_PROP_POS_BIT7_DISABLE ; /* (default 'DISABLE') */

  api->prop->dd_mode.bw                             = Si2168_DD_MODE_PROP_BW_BW_8MHZ              ; /* (default 'BW_8MHZ') */
  api->prop->dd_mode.modulation                     = Si2168_DD_MODE_PROP_MODULATION_DVBT         ; /* (default 'DVBT') */
  api->prop->dd_mode.invert_spectrum                = Si2168_DD_MODE_PROP_INVERT_SPECTRUM_NORMAL  ; /* (default 'NORMAL') */
  api->prop->dd_mode.auto_detect                    = Si2168_DD_MODE_PROP_AUTO_DETECT_NONE        ; /* (default 'NONE') */

  api->prop->dd_per_resol.exp                       =     5; /* (default     5) */
  api->prop->dd_per_resol.mant                      =     1; /* (default     1) */

  api->prop->dd_rsq_ber_threshold.exp               =     1; /* (default     1) */
  api->prop->dd_rsq_ber_threshold.mant              =    10; /* (default    10) */

  api->prop->dd_ssi_sqi_param.sqi_average           =     1; /* (default     1) */

  api->prop->dd_ts_freq.req_freq_10khz              =   720; /* (default   720) */

  api->prop->dd_ts_mode.mode                        = Si2168_DD_TS_MODE_PROP_MODE_TRISTATE                ; /* (default 'TRISTATE') */
  api->prop->dd_ts_mode.clock                       = Si2168_DD_TS_MODE_PROP_CLOCK_AUTO_FIXED             ; /* (default 'AUTO_FIXED') */
  api->prop->dd_ts_mode.clk_gapped_en               = Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN_DISABLED       ; /* (default 'DISABLED') */
  api->prop->dd_ts_mode.ts_err_polarity             = Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY_NOT_INVERTED ; /* (default 'NOT_INVERTED') */
  api->prop->dd_ts_mode.special                     = Si2168_DD_TS_MODE_PROP_SPECIAL_FULL_TS              ; /* (default 'FULL_TS') */

  api->prop->dd_ts_setup_par.ts_data_strength       =     3; /* (default     3) */
  api->prop->dd_ts_setup_par.ts_data_shape          =     2; /* (default     1) */
  api->prop->dd_ts_setup_par.ts_clk_strength        =     3; /* (default     3) */
  api->prop->dd_ts_setup_par.ts_clk_shape           =     2; /* (default     1) */
  api->prop->dd_ts_setup_par.ts_clk_invert          = Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT_INVERTED    ; /* (default 'INVERTED') */
  api->prop->dd_ts_setup_par.ts_clk_shift           =     0; /* (default     0) */

  api->prop->dd_ts_setup_ser.ts_data_strength       =     7; /* (default    15) */
  api->prop->dd_ts_setup_ser.ts_data_shape          =     1; /* (default     3) */
  api->prop->dd_ts_setup_ser.ts_clk_strength        =     7; /* (default    15) */
  api->prop->dd_ts_setup_ser.ts_clk_shape           =     1; /* (default     3) */
  api->prop->dd_ts_setup_ser.ts_clk_invert          = Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT_INVERTED      ; /* (default 'INVERTED') */
  api->prop->dd_ts_setup_ser.ts_sync_duration       = Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION_FIRST_BYTE ; /* (default 'FIRST_BYTE') */
  api->prop->dd_ts_setup_ser.ts_byte_order          = Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER_MSB_FIRST     ; /* (default 'MSB_FIRST') */

}

void Si2168_setupDVBCDefaults       (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupDVBCDefaults      \n");
  api->prop->dvbc_adc_crest_factor.crest_factor     =   112; /* (default   112) */

  api->prop->dvbc_afc_range.range_khz               =   100; /* (default   100) */

  api->prop->dvbc_constellation.constellation       = Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION_AUTO ; /* (default 'AUTO') */

  api->prop->dvbc_symbol_rate.rate                  =  6900; /* (default  6900) */

}



void Si2168_setupDVBTDefaults       (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupDVBTDefaults      \n");
  api->prop->dvbt_adc_crest_factor.crest_factor     =   130; /* (default   130) */

  api->prop->dvbt_afc_range.range_khz               =   550; /* (default   550) */

  api->prop->dvbt_hierarchy.stream                  = Si2168_DVBT_HIERARCHY_PROP_STREAM_HP ; /* (default 'HP') */

}


void Si2168_setupDVBT2Defaults      (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupDVBT2Defaults     \n");
  api->prop->dvbt2_adc_crest_factor.crest_factor    =   130; /* (default   130) */

  api->prop->dvbt2_afc_range.range_khz              =   550; /* (default   550) */

  api->prop->dvbt2_fef_tuner.tuner_delay            =     1; /* (default     1) */
  api->prop->dvbt2_fef_tuner.tuner_freeze_time      =     1; /* (default     1) */
  api->prop->dvbt2_fef_tuner.tuner_unfreeze_time    =     1; /* (default     1) */

}


void Si2168_setupSCANDefaults       (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupSCANDefaults      \n");
  api->prop->scan_fmax.scan_fmax                    =     0; /* (default     0) */

  api->prop->scan_fmin.scan_fmin                    =     0; /* (default     0) */

  api->prop->scan_ien.buzien                        = Si2168_SCAN_IEN_PROP_BUZIEN_DISABLE ; /* (default 'DISABLE') */
  api->prop->scan_ien.reqien                        = Si2168_SCAN_IEN_PROP_REQIEN_DISABLE ; /* (default 'DISABLE') */

  api->prop->scan_int_sense.buznegen                = Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN_ENABLE  ; /* (default 'ENABLE') */
  api->prop->scan_int_sense.reqnegen                = Si2168_SCAN_INT_SENSE_PROP_REQNEGEN_DISABLE ; /* (default 'DISABLE') */
  api->prop->scan_int_sense.buzposen                = Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN_DISABLE ; /* (default 'DISABLE') */
  api->prop->scan_int_sense.reqposen                = Si2168_SCAN_INT_SENSE_PROP_REQPOSEN_ENABLE  ; /* (default 'ENABLE') */


  api->prop->scan_symb_rate_max.scan_symb_rate_max  =     0; /* (default     0) */

  api->prop->scan_symb_rate_min.scan_symb_rate_min  =     0; /* (default     0) */

  api->prop->scan_ter_config.mode                   = Si2168_SCAN_TER_CONFIG_PROP_MODE_BLIND_SCAN          ; /* (default 'BLIND_SCAN') */
  api->prop->scan_ter_config.analog_bw              = Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW_8MHZ           ; /* (default '8MHZ') */
  api->prop->scan_ter_config.search_analog          = Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG_DISABLE    ; /* (default 'DISABLE') */
}

void Si2168_setupAllDefaults       (L1_Si2168_Context *api)
{
  Si2168_setupCOMMONDefaults    (api);
  Si2168_setupDDDefaults        (api);
  Si2168_setupDVBCDefaults      (api);


  Si2168_setupDVBTDefaults      (api);

  Si2168_setupDVBT2Defaults     (api);

  Si2168_setupSCANDefaults      (api);
}

int Si2168_setupCOMMONProperties    (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupCOMMONProperties    \n");
#ifdef    Si2168_COMMAND_LINE_APPLICATION
  Si2168_setupProperties(api, Si2168_COMMON);
#else  /* Si2168_COMMAND_LINE_APPLICATION */
  Si2168_setupCOMMONDefaults(api);
#endif /* Si2168_COMMAND_LINE_APPLICATION */
  return 0;
}

int Si2168_setupDDProperties        (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupDDProperties        \n");
#ifdef    Si2168_COMMAND_LINE_APPLICATION
  Si2168_setupProperties(api, Si2168_DD);
#else  /* Si2168_COMMAND_LINE_APPLICATION */
  Si2168_setupDDDefaults(api);
#endif /* Si2168_COMMAND_LINE_APPLICATION */
  return 0;
}

int Si2168_setupDVBCProperties      (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupDVBCProperties      \n");
#ifdef    Si2168_COMMAND_LINE_APPLICATION
  Si2168_setupProperties(api, Si2168_DVBC);
#else  /* Si2168_COMMAND_LINE_APPLICATION */
  Si2168_setupDVBCDefaults(api);
#endif /* Si2168_COMMAND_LINE_APPLICATION */
  return 0;
}



int Si2168_setupDVBTProperties      (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupDVBTProperties      \n");
#ifdef    Si2168_COMMAND_LINE_APPLICATION
  Si2168_setupProperties(api, Si2168_DVBT);
#else  /* Si2168_COMMAND_LINE_APPLICATION */
  Si2168_setupDVBTDefaults(api);
#endif /* Si2168_COMMAND_LINE_APPLICATION */
  return 0;
}


int Si2168_setupDVBT2Properties     (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupDVBT2Properties     \n");
#ifdef    Si2168_COMMAND_LINE_APPLICATION
  Si2168_setupProperties(api, Si2168_DVBT2);
#else  /* Si2168_COMMAND_LINE_APPLICATION */
  Si2168_setupDVBT2Defaults(api);
#endif /* Si2168_COMMAND_LINE_APPLICATION */
  return 0;
}


int Si2168_setupSCANProperties      (L1_Si2168_Context *api)
{
  SiTRACE("Si2168_setupSCANProperties      \n");
#ifdef    Si2168_COMMAND_LINE_APPLICATION
  Si2168_setupProperties(api, Si2168_SCAN);
#else  /* Si2168_COMMAND_LINE_APPLICATION */
  Si2168_setupSCANDefaults(api);
#endif /* Si2168_COMMAND_LINE_APPLICATION */
  return 0;
}

int Si2168_setupAllProperties  (L1_Si2168_Context *api)
{
  Si2168_setupCOMMONProperties    (api);
  Si2168_setupDDProperties        (api);
  Si2168_setupDVBCProperties      (api);


  Si2168_setupDVBTProperties      (api);

  Si2168_setupDVBT2Properties     (api);

  Si2168_setupSCANProperties      (api);
  return 0;
}


 /*****************************************************************************************
 NAME: Si2168_downloadCOMMONProperties
  DESCRIPTION: Setup Si2168 COMMON properties configuration
  This function will download all the COMMON configuration properties.
  The function SetupCOMMONDefaults() should be called before the first call to this function.
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
  Programming Guide Reference:    COMMON setup flowchart
******************************************************************************************/
int  Si2168_downloadCOMMONProperties(L1_Si2168_Context *api)
{
  SiTRACE("Si2168_downloadCOMMONProperties      \n");
#ifdef    Si2168_MASTER_IEN_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_MASTER_IEN_PROP_CODE            ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_MASTER_IEN_PROP */
return NO_Si2168_ERROR;
}

 /*****************************************************************************************
 NAME: Si2168_downloadDDProperties
  DESCRIPTION: Setup Si2168 DD properties configuration
  This function will download all the DD configuration properties.
  The function SetupDDDefaults() should be called before the first call to this function.
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
  Programming Guide Reference:    DD setup flowchart
******************************************************************************************/
int  Si2168_downloadDDProperties(L1_Si2168_Context *api)
{
  SiTRACE("Si2168_downloadDDProperties            \n");
#ifdef    Si2168_DD_BER_RESOL_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_BER_RESOL_PROP_CODE          ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_BER_RESOL_PROP */
#ifdef    Si2168_DD_CBER_RESOL_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_CBER_RESOL_PROP_CODE         ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_CBER_RESOL_PROP */

#ifdef    Si2168_DD_FER_RESOL_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_FER_RESOL_PROP_CODE          ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_FER_RESOL_PROP */
#ifdef    Si2168_DD_IEN_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_IEN_PROP_CODE                ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_IEN_PROP */
#ifdef    Si2168_DD_IF_INPUT_FREQ_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_IF_INPUT_FREQ_PROP_CODE      ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_IF_INPUT_FREQ_PROP */
#ifdef    Si2168_DD_INT_SENSE_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_INT_SENSE_PROP_CODE          ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_INT_SENSE_PROP */
#ifdef    Si2168_DD_MODE_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_MODE_PROP_CODE               ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_MODE_PROP */
#ifdef    Si2168_DD_PER_RESOL_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_PER_RESOL_PROP_CODE          ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_PER_RESOL_PROP */
#ifdef    Si2168_DD_RSQ_BER_THRESHOLD_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_RSQ_BER_THRESHOLD_PROP_CODE  ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_RSQ_BER_THRESHOLD_PROP */
#ifdef    Si2168_DD_SSI_SQI_PARAM_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_SSI_SQI_PARAM_PROP_CODE      ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_SSI_SQI_PARAM_PROP */
#ifdef    Si2168_DD_TS_FREQ_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_TS_FREQ_PROP_CODE            ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_TS_FREQ_PROP */
#ifdef    Si2168_DD_TS_MODE_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_TS_MODE_PROP_CODE            ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_TS_MODE_PROP */
#ifdef    Si2168_DD_TS_SETUP_PAR_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_TS_SETUP_PAR_PROP_CODE       ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_TS_SETUP_PAR_PROP */
#ifdef    Si2168_DD_TS_SETUP_SER_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DD_TS_SETUP_SER_PROP_CODE       ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DD_TS_SETUP_SER_PROP */
return NO_Si2168_ERROR;
}

 /*****************************************************************************************
 NAME: Si2168_downloadDVBCProperties
  DESCRIPTION: Setup Si2168 DVBC properties configuration
  This function will download all the DVBC configuration properties.
  The function SetupDVBCDefaults() should be called before the first call to this function.
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
  Programming Guide Reference:    DVBC setup flowchart
******************************************************************************************/
int  Si2168_downloadDVBCProperties(L1_Si2168_Context *api)
{
  SiTRACE("Si2168_downloadDVBCProperties       \n");
#ifdef    Si2168_DVBC_ADC_CREST_FACTOR_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBC_ADC_CREST_FACTOR_PROP_CODE ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBC_ADC_CREST_FACTOR_PROP */
#ifdef    Si2168_DVBC_AFC_RANGE_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBC_AFC_RANGE_PROP_CODE        ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBC_AFC_RANGE_PROP */
#ifdef    Si2168_DVBC_CONSTELLATION_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBC_CONSTELLATION_PROP_CODE    ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBC_CONSTELLATION_PROP */
#ifdef    Si2168_DVBC_SYMBOL_RATE_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBC_SYMBOL_RATE_PROP_CODE      ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBC_SYMBOL_RATE_PROP */
return NO_Si2168_ERROR;
}



 /*****************************************************************************************
 NAME: Si2168_downloadDVBTProperties
  DESCRIPTION: Setup Si2168 DVBT properties configuration
  This function will download all the DVBT configuration properties.
  The function SetupDVBTDefaults() should be called before the first call to this function.
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
  Programming Guide Reference:    DVBT setup flowchart
******************************************************************************************/
int  Si2168_downloadDVBTProperties(L1_Si2168_Context *api)
{
  SiTRACE("Si2168_downloadDVBTProperties     \n");
#ifdef    Si2168_DVBT_ADC_CREST_FACTOR_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBT_ADC_CREST_FACTOR_PROP_CODE ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBT_ADC_CREST_FACTOR_PROP */
#ifdef    Si2168_DVBT_AFC_RANGE_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBT_AFC_RANGE_PROP_CODE        ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBT_AFC_RANGE_PROP */
#ifdef    Si2168_DVBT_HIERARCHY_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBT_HIERARCHY_PROP_CODE        ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBT_HIERARCHY_PROP */
return NO_Si2168_ERROR;
}


 /*****************************************************************************************
 NAME: Si2168_downloadDVBT2Properties
  DESCRIPTION: Setup Si2168 DVBT2 properties configuration
  This function will download all the DVBT2 configuration properties.
  The function SetupDVBT2Defaults() should be called before the first call to this function.
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
  Programming Guide Reference:    DVBT2 setup flowchart
******************************************************************************************/
int  Si2168_downloadDVBT2Properties(L1_Si2168_Context *api)
{
  SiTRACE("Si2168_downloadDVBT2Properties        \n");
#ifdef    Si2168_DVBT2_ADC_CREST_FACTOR_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CODE) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBT2_ADC_CREST_FACTOR_PROP */
#ifdef    Si2168_DVBT2_AFC_RANGE_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBT2_AFC_RANGE_PROP_CODE       ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBT2_AFC_RANGE_PROP */
#ifdef    Si2168_DVBT2_FEF_TUNER_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_DVBT2_FEF_TUNER_PROP_CODE       ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_DVBT2_FEF_TUNER_PROP */
return NO_Si2168_ERROR;
}

 /*****************************************************************************************
 NAME: Si2168_downloadSCANProperties
  DESCRIPTION: Setup Si2168 SCAN properties configuration
  This function will download all the SCAN configuration properties.
  The function SetupSCANDefaults() should be called before the first call to this function.
  Parameter:  Pointer to Si2168 Context
  Returns:    I2C transaction error code, NO_Si2168_ERROR if successful
  Programming Guide Reference:    SCAN setup flowchart
******************************************************************************************/
int  Si2168_downloadSCANProperties(L1_Si2168_Context *api)
{
  SiTRACE("Si2168_downloadSCANProperties       \n");
#ifdef    Si2168_SCAN_FMAX_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_SCAN_FMAX_PROP_CODE             ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_SCAN_FMAX_PROP */
#ifdef    Si2168_SCAN_FMIN_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_SCAN_FMIN_PROP_CODE             ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_SCAN_FMIN_PROP */
#ifdef    Si2168_SCAN_IEN_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_SCAN_IEN_PROP_CODE              ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_SCAN_IEN_PROP */
#ifdef    Si2168_SCAN_INT_SENSE_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_SCAN_INT_SENSE_PROP_CODE        ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_SCAN_INT_SENSE_PROP */

#ifdef    Si2168_SCAN_SYMB_RATE_MAX_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_SCAN_SYMB_RATE_MAX_PROP_CODE    ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_SCAN_SYMB_RATE_MAX_PROP */
#ifdef    Si2168_SCAN_SYMB_RATE_MIN_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_SCAN_SYMB_RATE_MIN_PROP_CODE    ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_SCAN_SYMB_RATE_MIN_PROP */
#ifdef    Si2168_SCAN_TER_CONFIG_PROP
  if (Si2168_L1_SetProperty2(api, Si2168_SCAN_TER_CONFIG_PROP_CODE       ) != NO_Si2168_ERROR) {return ERROR_Si2168_SENDING_COMMAND;}
#endif /* Si2168_SCAN_TER_CONFIG_PROP */
return NO_Si2168_ERROR;
}

int Si2168_downloadAllProperties       (L1_Si2168_Context *api)
{
  Si2168_downloadCOMMONProperties    (api);
  Si2168_downloadDDProperties        (api);
  if (api->media == Si2168_TERRESTRIAL) {
  Si2168_downloadDVBCProperties      (api);
  }


  if (api->media == Si2168_TERRESTRIAL) {
  Si2168_downloadDVBTProperties      (api);
  }

  if (api->media == Si2168_TERRESTRIAL) {
  Si2168_downloadDVBT2Properties     (api);
  }

  Si2168_downloadSCANProperties      (api);
  return 0;
}

/* _property_string_functions_insertion_point */





