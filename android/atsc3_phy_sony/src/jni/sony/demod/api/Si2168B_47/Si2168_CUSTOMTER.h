#ifndef  TUNER_CUSTOMTER_SPECIFICS
    #define TUNER_CUSTOMTER_SPECIFICS

#include "SiLabs_L1_RF_CUSTOMTER_API.h"

#ifdef    SILABS_EVB
    #define Si2168_CLOCK_MODE_TER              Si2168_START_CLK_CMD_CLK_MODE_CLK_CLKIO
    #define Si2168_REF_FREQUENCY_TER           24
#else  /* SILABS_EVB */
    #define Si2168_CLOCK_MODE_TER              Si2168_START_CLK_CMD_CLK_MODE_CLK_CLKIO
    #define Si2168_REF_FREQUENCY_TER           24
#endif /* SILABS_EVB */

    #define TER_TUNER_CONTEXT            CUSTOMTER_Context
    #define TUNER_ADDRESS_TER            0xc6

    /* Satellite tuner function names */
    #define L1_RF_TER_TUNER_Process             L1_RF_CUSTOMTER_Process
    #define L1_RF_TER_TUNER_Init                L1_RF_CUSTOMTER_Init
    #define L1_RF_TER_TUNER_InitAfterReset      L1_RF_CUSTOMTER_InitAfterReset
    #define TER_TUNER_WAKEUP                    L1_RF_CUSTOMTER_Wakeup
    #define TER_TUNER_STANDBY                   L1_RF_CUSTOMTER_Standby
    #define TER_TUNER_CLOCK_OFF                 L1_RF_CUSTOMTER_ClockOff
    #define TER_TUNER_CLOCK_ON                  L1_RF_CUSTOMTER_ClockOn

    #define L1_RF_TER_TUNER_Tune                L1_RF_CUSTOMTER_Tune
    #define L1_RF_TER_TUNER_Saw                 L1_RF_CUSTOMTER_Saw
    #define L1_RF_TER_TUNER_Get_Saw             L1_RF_CUSTOMTER_Saw
    #define L1_RF_TER_TUNER_Get_RF              L1_RF_CUSTOMTER_Get_RF

    #define TER_TUNER_TAG_TEXT                  L1_RF_CUSTOMTER_TAG_TEXT
    #define TER_TUNER_ERROR_TEXT                L1_RF_CUSTOMTER_Error_TEXT

#endif /* TUNER_CUSTOMTER_SPECIFICS */
