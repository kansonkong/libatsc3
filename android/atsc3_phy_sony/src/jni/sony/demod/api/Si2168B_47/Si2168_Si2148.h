#ifndef  TUNER_Si2148_SPECIFICS
#define  TUNER_Si2148_SPECIFICS

    #include "Si2148_L2_API.h"

#ifdef    SILABS_EVB
    #define Si2168_CLOCK_MODE_TER              Si2168_START_CLK_CMD_CLK_MODE_CLK_CLKIO
    #define Si2168_REF_FREQUENCY_TER           24
#else  /* SILABS_EVB */
    #define Si2168_CLOCK_MODE_TER              Si2168_START_CLK_CMD_CLK_MODE_CLK_CLKIO
    #define Si2168_REF_FREQUENCY_TER           24
#endif /* SILABS_EVB */

    #define TUNERTER_API
    #define TER_TUNER_CONTEXT            L1_Si2148_Context
    #define TUNER_ADDRESS_TER            0xc6

    #define TER_TUNER_INIT(api)           Si2148_Init(api)
    #define TER_TUNER_WAKEUP(api)         Si2148_pollForCTS(api)
    /* Compatibility with Si2148 ROM12 & ROM13 APIs */
   #ifdef    Si2148_STANDBY_CMD_TYPE_MIN
     #define TER_TUNER_STANDBY(api)        Si2148_L1_STANDBY(api, Si2148_STANDBY_CMD_TYPE_MIN)
   #else  /* Si2148_STANDBY_CMD_TYPE_MIN */
    #define TER_TUNER_STANDBY(api)        Si2148_L1_STANDBY(api)
   #endif /* Si2148_STANDBY_CMD_TYPE_MIN */
    #define TER_TUNER_CLOCK_OFF(api)      Si2148_XoutOff(api)
    #define TER_TUNER_CLOCK_ON(api)       Si2148_XoutOn(api)
    #define TER_TUNER_ERROR_TEXT(res)     Si2148_L1_API_ERROR_TEXT(res)
#ifdef    Si2168_COMMAND_LINE_APPLICATION
    #define TER_TUNER_MENU(full)          Si2148_menu(full)
    #define TER_TUNER_LOOP(api,choice)    Si2148_demoLoop(api,choice)
#endif /* Si2168_COMMAND_LINE_APPLICATION */

    #define TER_TUNER_MODULATION_DVBT     Si2148_DTV_MODE_PROP_MODULATION_DVBT
    #define TER_TUNER_MODULATION_DVBC     Si2148_DTV_MODE_PROP_MODULATION_DVBC

    #define L1_RF_TER_TUNER_Init(api,add,endeavour,endeavourChipidx, i2cbus) Si2148_L1_API_Init(api, add, endeavour, endeavourChipidx, i2cbus);
    #define L1_RF_TER_TUNER_Tune(api,rf)  Si2148_DTVTune(api, rf, bw, modulation, Si2148_DTV_MODE_PROP_INVERT_SPECTRUM_NORMAL)

    #define L1_RF_TER_TUNER_Get_RF

    #define L1_RF_TER_TUNER_MODULATION_DVBC  Si2148_DTV_MODE_PROP_MODULATION_DVBC
    #define L1_RF_TER_TUNER_MODULATION_DVBT  Si2148_DTV_MODE_PROP_MODULATION_DVBT
    #define L1_RF_TER_TUNER_MODULATION_DVBT2 Si2148_DTV_MODE_PROP_MODULATION_DVBT

    #define   L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN_SETUP(api) \
    api->cmd->config_pins.gpio1_mode               = Si2148_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE;\
    api->cmd->config_pins.gpio1_read               = Si2148_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ;\
    api->cmd->config_pins.gpio2_mode               = Si2148_CONFIG_PINS_CMD_GPIO2_MODE_DISABLE;\
    api->cmd->config_pins.gpio2_read               = Si2148_CONFIG_PINS_CMD_GPIO2_READ_DO_NOT_READ;\
    api->cmd->config_pins.reserved1                = Si2148_CONFIG_PINS_CMD_RESERVED1_RESERVED;\
    api->cmd->config_pins.reserved2                = Si2148_CONFIG_PINS_CMD_RESERVED2_RESERVED;\
    api->cmd->config_pins.reserved3                = Si2148_CONFIG_PINS_CMD_RESERVED3_RESERVED;\
    api->prop->dtv_initial_agc_speed.agc_decim     = Si2148_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;\
    api->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;\
    api->prop->dtv_initial_agc_speed_period.period = 0;\
    api->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;\
    api->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;\
    Si2148_L1_SendCommand2(api, Si2148_CONFIG_PINS_CMD_CODE);\
    Si2148_L1_SetProperty2(api, Si2148_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);\
    Si2148_L1_SetProperty2(api, Si2148_DTV_AGC_SPEED_PROP_CODE);

    #define   L1_RF_TER_TUNER_FEF_MODE_FREEZE_PIN(api,fef) \
    api->prop->dtv_agc_freeze_input.level          = Si2148_DTV_AGC_FREEZE_INPUT_PROP_LEVEL_HIGH;\
    if (fef == 0) {\
      api->prop->dtv_agc_freeze_input.pin          = Si2148_DTV_AGC_FREEZE_INPUT_PROP_PIN_NONE;\
    } else {\
      api->prop->dtv_agc_freeze_input.pin          = Si2148_DTV_AGC_FREEZE_INPUT_PROP_PIN_GPIO1;\
    }\
    Si2148_L1_SetProperty2(api, Si2148_DTV_AGC_FREEZE_INPUT_PROP_CODE);

    #define   L1_RF_TER_TUNER_FEF_MODE_SLOW_INITIAL_AGC_SETUP(api,fef) \
      if (fef == 0) {\
        api->prop->dtv_initial_agc_speed.agc_decim     = Si2148_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;\
        api->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;\
        api->prop->dtv_initial_agc_speed_period.period = 0;\
        api->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;\
        api->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;\
      } else {\
        api->prop->dtv_initial_agc_speed.agc_decim     = Si2148_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM_OFF;\
        api->prop->dtv_initial_agc_speed.if_agc_speed  = Si2148_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;\
        api->prop->dtv_initial_agc_speed_period.period = 310;\
        api->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_4;\
        api->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;\
      }\
      Si2148_L1_SetProperty2(api, Si2148_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);\
      Si2148_L1_SetProperty2(api, Si2148_DTV_AGC_SPEED_PROP_CODE);


    #define   L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC_SETUP(api,fef) \
      api->prop->dtv_initial_agc_speed_period.period = 0;\
      if (fef == 0) {\
        api->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;\
        api->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;\
      } else {\
        api->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;\
        api->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_4;\
      }\
      Si2148_L1_SetProperty2(api, Si2148_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_CODE);\
      Si2148_L1_SetProperty2(api, Si2148_DTV_AGC_SPEED_PROP_CODE);

    #define   L1_RF_TER_TUNER_FEF_MODE_SLOW_NORMAL_AGC(api,fef) \
      if (fef == 0) {\
        api->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_OFF;\
        api->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_AUTO;\
      } else {\
        api->prop->dtv_agc_speed.if_agc_speed          = Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED_39;\
        api->prop->dtv_agc_speed.agc_decim             = Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM_4;\
      }\
      Si2148_L1_SetProperty2(api, Si2148_DTV_AGC_SPEED_PROP_CODE);

    #define TER_TUNER_ATV_LO_INJECTION(api) \
      api->prop->tuner_lo_injection.band_1 = Si2148_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;\
      api->prop->tuner_lo_injection.band_2 = Si2148_TUNER_LO_INJECTION_PROP_BAND_2_HIGH_SIDE;\
      api->prop->tuner_lo_injection.band_3 = Si2148_TUNER_LO_INJECTION_PROP_BAND_3_HIGH_SIDE;\
      Si2148_L1_SetProperty2(api, Si2148_TUNER_LO_INJECTION_PROP_CODE);

    #define TER_TUNER_DTV_LO_INJECTION(api) \
      api->prop->tuner_lo_injection.band_1 = Si2148_TUNER_LO_INJECTION_PROP_BAND_1_HIGH_SIDE;\
      api->prop->tuner_lo_injection.band_2 = Si2148_TUNER_LO_INJECTION_PROP_BAND_2_LOW_SIDE;\
      api->prop->tuner_lo_injection.band_3 = Si2148_TUNER_LO_INJECTION_PROP_BAND_3_LOW_SIDE;\
      Si2148_L1_SetProperty2(api, Si2148_TUNER_LO_INJECTION_PROP_CODE);

#endif /* TUNER_Si2148_SPECIFICS */

