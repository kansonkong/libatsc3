/*------------------------------------------------------------------------------
  Copyright 2014-2016 Sony Semiconductor Solutions Corporation

  Last Updated  : 2016/07/01
  File Revision : 1.1.1.0
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_helene.h

          This file provides the HELENE tuner control interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_HELENE_H
#define sony_cxd_HELENE_H

#include "sony_cxd_common.h"
#include "sony_cxd_i2c.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
/**
 @brief Version of this driver.
*/
#define sony_cxd_HELENE_VERSION       "1.1.1.0"

/**
 @brief Default I2C slave address of the HELENE tuner.
*/
#define sony_cxd_HELENE_ADDRESS       0xC0

/*------------------------------------------------------------------------------
  Enums
------------------------------------------------------------------------------*/
/**
 @brief HELENE tuner state.
*/
typedef enum {
    sony_cxd_HELENE_STATE_UNKNOWN,    /**< HELENE state is Unknown */
    sony_cxd_HELENE_STATE_SLEEP,      /**< HELENE state is Sleep */
    sony_cxd_HELENE_STATE_ACTIVE_T,   /**< HELENE state is Active (terrestrial) */
    sony_cxd_HELENE_STATE_ACTIVE_S    /**< HELENE state is Active (satellite) */
} sony_cxd_helene_state_t;

/**
 @brief Xtal frequencies supported by HELENE.
*/
typedef enum {
    sony_cxd_HELENE_XTAL_16000KHz,    /**< 16 MHz */
    sony_cxd_HELENE_XTAL_20500KHz,    /**< 20.5 MHz */
    sony_cxd_HELENE_XTAL_24000KHz,    /**< 24 MHz */
    sony_cxd_HELENE_XTAL_41000KHz     /**< 41 MHz */
} sony_cxd_helene_xtal_t;

/**
 @brief Analog/Digital terrestrial, satellite broadcasting system definitions supported by HELENE.

        Fc : Center Frequency, Fp : Picture Frequency
*/
typedef enum {
    sony_cxd_HELENE_TV_SYSTEM_UNKNOWN,
    /* Terrestrial Analog */
    sony_cxd_HELENE_ATV_MN_EIAJ,  /**< System-M (Japan) (IF: Fp=5.75MHz in default) */
    sony_cxd_HELENE_ATV_MN_SAP,   /**< System-M (US)    (IF: Fp=5.75MHz in default) */
    sony_cxd_HELENE_ATV_MN_A2,    /**< System-M (Korea) (IF: Fp=5.9MHz in default) */
    sony_cxd_HELENE_ATV_BG,       /**< System-B/G       (IF: Fp=7.3MHz in default) */
    sony_cxd_HELENE_ATV_I,        /**< System-I         (IF: Fp=7.85MHz in default) */
    sony_cxd_HELENE_ATV_DK,       /**< System-D/K       (IF: Fp=7.85MHz in default) */
    sony_cxd_HELENE_ATV_L,        /**< System-L         (IF: Fp=7.85MHz in default) */
    sony_cxd_HELENE_ATV_L_DASH,   /**< System-L DASH    (IF: Fp=2.2MHz in default) */
    /* Terrestrial/Cable Digital */
    sony_cxd_HELENE_DTV_8VSB,     /**< ATSC 8VSB        (IF: Fc=3.7MHz in default) */
    sony_cxd_HELENE_DTV_QAM,      /**< US QAM           (IF: Fc=3.7MHz in default) */
    sony_cxd_HELENE_DTV_ISDBT_6,  /**< ISDB-T 6MHzBW    (IF: Fc=3.55MHz in default) */
    sony_cxd_HELENE_DTV_ISDBT_7,  /**< ISDB-T 7MHzBW    (IF: Fc=4.15MHz in default) */
    sony_cxd_HELENE_DTV_ISDBT_8,  /**< ISDB-T 8MHzBW    (IF: Fc=4.75MHz in default) */
    sony_cxd_HELENE_DTV_DVBT_5,   /**< DVB-T 5MHzBW     (IF: Fc=3.6MHz in default) */
    sony_cxd_HELENE_DTV_DVBT_6,   /**< DVB-T 6MHzBW     (IF: Fc=3.6MHz in default) */
    sony_cxd_HELENE_DTV_DVBT_7,   /**< DVB-T 7MHzBW     (IF: Fc=4.2MHz in default) */
    sony_cxd_HELENE_DTV_DVBT_8,   /**< DVB-T 8MHzBW     (IF: Fc=4.8MHz in default) */
    sony_cxd_HELENE_DTV_DVBT2_1_7,/**< DVB-T2 1.7MHzBW  (IF: Fc=3.5MHz in default) */
    sony_cxd_HELENE_DTV_DVBT2_5,  /**< DVB-T2 5MHzBW    (IF: Fc=3.6MHz in default) */
    sony_cxd_HELENE_DTV_DVBT2_6,  /**< DVB-T2 6MHzBW    (IF: Fc=3.6MHz in default) */
    sony_cxd_HELENE_DTV_DVBT2_7,  /**< DVB-T2 7MHzBW    (IF: Fc=4.2MHz in default) */
    sony_cxd_HELENE_DTV_DVBT2_8,  /**< DVB-T2 8MHzBW    (IF: Fc=4.8MHz in default) */
    sony_cxd_HELENE_DTV_DVBC_6,   /**< DVB-C 6MHzBW     (IF: Fc=3.7MHz in default) */
    sony_cxd_HELENE_DTV_DVBC_8,   /**< DVB-C 8MHzBW     (IF: Fc=4.9MHz in default) */
    sony_cxd_HELENE_DTV_DVBC2_6,  /**< DVB-C2 6MHzBW    (IF: Fc=3.7MHz in default) */
    sony_cxd_HELENE_DTV_DVBC2_8,  /**< DVB-C2 8MHzBW    (IF: Fc=4.9MHz in default) */
    sony_cxd_HELENE_DTV_J83B_5_6, /**< J.83B 5.6Msps    (IF: Fc=3.75MHz in default) */
    sony_cxd_HELENE_DTV_DTMB,     /**< DTMB             (IF: Fc=5.1MHz in default) */
    /* Satellite */
    sony_cxd_HELENE_STV_ISDBS,    /**< ISDB-S */
    sony_cxd_HELENE_STV_DVBS,     /**< DVB-S */
    sony_cxd_HELENE_STV_DVBS2,    /**< DVB-S2 */

    sony_cxd_HELENE_ATV_MIN = sony_cxd_HELENE_ATV_MN_EIAJ, /**< Minimum analog terrestrial system */
    sony_cxd_HELENE_ATV_MAX = sony_cxd_HELENE_ATV_L_DASH,  /**< Maximum analog terrestrial system */
    sony_cxd_HELENE_DTV_MIN = sony_cxd_HELENE_DTV_8VSB,    /**< Minimum digital terrestrial system */
    sony_cxd_HELENE_DTV_MAX = sony_cxd_HELENE_DTV_DTMB,    /**< Maximum digital terrestrial system */
    sony_cxd_HELENE_TERR_TV_SYSTEM_NUM,                /**< Number of supported terrestrial broadcasting system */
    sony_cxd_HELENE_STV_MIN = sony_cxd_HELENE_STV_ISDBS,   /**< Minimum satellite system */
    sony_cxd_HELENE_STV_MAX = sony_cxd_HELENE_STV_DVBS2    /**< Maximum satellite system */
} sony_cxd_helene_tv_system_t;

/**
 @brief Macro to check that the system is analog terrestrial or not.
*/
#define sony_cxd_HELENE_IS_ATV(tvSystem) (((tvSystem) >= sony_cxd_HELENE_ATV_MIN) && ((tvSystem) <= sony_cxd_HELENE_ATV_MAX))

/**
 @brief Macro to check that the system is digital terrestrial or not.
*/
#define sony_cxd_HELENE_IS_DTV(tvSystem) (((tvSystem) >= sony_cxd_HELENE_DTV_MIN) && ((tvSystem) <= sony_cxd_HELENE_DTV_MAX))

/**
 @brief Macro to check that the system is satellite or not.
*/
#define sony_cxd_HELENE_IS_STV(tvSystem) (((tvSystem) >= sony_cxd_HELENE_STV_MIN) && ((tvSystem) <= sony_cxd_HELENE_STV_MAX))

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/**
 @brief HELENE settings that may need to change depend on customer's system.
*/
typedef struct sony_cxd_helene_terr_adjust_param_t {
    uint8_t RF_GAIN;           /**< Addr:0x69 Bit[6:4] : RFVGA gain. 0xFF means Auto. (RF_GAIN_SEL = 1) */
    uint8_t IF_BPF_GC;         /**< Addr:0x69 Bit[3:0] : IF_BPF gain. */
    uint8_t RFOVLD_DET_LV1_VL; /**< Addr:0x6B Bit[3:0] : RF overload RF input detect level. (FRF <= 172MHz) */
    uint8_t RFOVLD_DET_LV1_VH; /**< Addr:0x6B Bit[3:0] : RF overload RF input detect level. (172MHz < FRF <= 464MHz) */
    uint8_t RFOVLD_DET_LV1_U;  /**< Addr:0x6B Bit[3:0] : RF overload RF input detect level. (FRF > 464MHz) */
    uint8_t IFOVLD_DET_LV_VL;  /**< Addr:0x6C Bit[2:0] : Internal RFAGC detect level. (FRF <= 172MHz) */
    uint8_t IFOVLD_DET_LV_VH;  /**< Addr:0x6C Bit[2:0] : Internal RFAGC detect level. (172MHz < FRF <= 464MHz) */
    uint8_t IFOVLD_DET_LV_U;   /**< Addr:0x6C Bit[2:0] : Internal RFAGC detect level. (FRF > 464MHz) */
    uint8_t IF_BPF_F0;         /**< Addr:0x6D Bit[5:4] : IF filter center offset. */
    uint8_t BW;                /**< Addr:0x6D Bit[1:0] : 6MHzBW(0x00) or 7MHzBW(0x01) or 8MHzBW(0x02) or 1.7MHzBW(0x03) */
    uint8_t FIF_OFFSET;        /**< Addr:0x6E Bit[4:0] : 5bit signed. IF offset (kHz) = FIF_OFFSET x 50 */
    uint8_t BW_OFFSET;         /**< Addr:0x6F Bit[4:0] : 5bit signed. BW offset (kHz) = BW_OFFSET x 50 (BW_OFFSET x 10 in 1.7MHzBW) */
    uint8_t IS_LOWERLOCAL;     /**< Addr:0x9C Bit[0]   : Local polarity. (0: Upper Local, 1: Lower Local) */
} sony_cxd_helene_terr_adjust_param_t;

/**
 @brief The HELENE tuner definition which allows control of the HELENE tuner device
        through the defined set of functions.
*/
typedef struct sony_cxd_helene_t {
    sony_cxd_helene_xtal_t        xtalFreq;       /**< Xtal frequency for HELENE. */
    uint8_t                   i2cAddress;     /**< I2C slave address of the HELENE tuner (8-bit form - 8'bxxxxxxx0) */
    sony_cxd_i2c_t*               pI2c;           /**< I2C API instance. */
    uint32_t                  flags;          /**< ORed value of sony_cxd_HELENE_CONFIG_XXXX */

    /* For saving current setting */
    sony_cxd_helene_state_t       state;          /**< The driver operating state. */
    uint32_t                  frequencykHz;   /**< Currently RF frequency(kHz) tuned. */
    sony_cxd_helene_tv_system_t   tvSystem;       /**< Current broadcasting system tuned. */
    uint32_t                  symbolRateksps; /**< Current symbol rate(ksym/s) tuned. */

    uint8_t                   isFreesatMode;  /**< UK Freesat mode or not. */

    /**
     @brief Adjustment parameter table (sony_cxd_HELENE_TV_SYSTEM_NUM size)
    */
    const sony_cxd_helene_terr_adjust_param_t *pTerrParamTable;

    /* Following Xtal related parameters can be changed if optimization is necessary. */
    uint8_t                   xosc_sel;       /**< Driver current setting for crystal oscillator. (Addr:0x82 Bit[4:0]) */
    uint8_t                   xosc_cap_set;   /**< Driver current setting for crystal oscillator. (Addr:0x83 Bit[6:0]) */

    void*                     user;           /**< User defined data. */
} sony_cxd_helene_t;

/*
 Config flag definitions. (ORed value should be set to flags argument of Create API.)
*/
/**
 @brief Use external Xtal

        Should be used for slave HELENE in Xtal shared system.
*/
#define sony_cxd_HELENE_CONFIG_EXT_REF                0x80000000

/**
 @brief Disable Xtal in Sleep state.

        Should NOT be used for Xtal shared system.
        Not only for the master HELENE which has Xtal, but also slave HELENE which receive the clock signal.
        Cannot be used with sony_cxd_HELENE_CONFIG_EXT_REF.
*/
#define sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL      0x40000000

/**
 @brief Internal loop filter setting.

        If this is used, internal loop filter is used for
        digital broadcasting system except for DVB-C/C2.
        For analog and DVB-C/C2, external loop filter will be used.
*/
#define sony_cxd_HELENE_CONFIG_LOOPFILTER_INTERNAL    0x10000000

/**
 @brief Satellite LNA OFF (path through) setting.
*/
#define sony_cxd_HELENE_CONFIG_SAT_LNA_OFF            0x08000000

/**
 @name  Power Save setting for terrestrial.

        These settings are for avoiding steep input impedance change in power save state.
*/
/**@{*/
#define sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_NORMAL          0x00000000 /**< Normal (default) */
#define sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RFIN_MATCHING   0x00400000 /**< RFIN matching enable */
#define sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RF_ACTIVE       0x00800000 /**< Keep RF part active */
#define sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_MASK            0x00C00000 /**< Do not use this value */
/**@}*/

/**
 @name  Power Save setting for satellite.

        These settings are for avoiding steep input impedance change in power save state.
*/
/**@{*/
#define sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_NORMAL          0x00000000 /**< Normal (default) */
#define sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_RFIN_MATCHING   0x00100000 /**< RFIN matching enable */
#define sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_RF_ACTIVE       0x00200000 /**< Keep RF part active */
#define sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_MASK            0x00300000 /**< Do not use this value */
/**@}*/

/**
 @name  Maximum IF and IQ output setting. (for terrestrial analog)
*/
/**@{*/
#define sony_cxd_HELENE_CONFIG_OUTLMT_ATV_1_5Vpp      0x00000000  /**< 1.5Vpp (default) */
#define sony_cxd_HELENE_CONFIG_OUTLMT_ATV_1_2Vpp      0x00080000  /**< 1.2Vpp */
/**@}*/

/**
 @name  Maximum IF and IQ output setting. (for terrestrial digital)
*/
/**@{*/
#define sony_cxd_HELENE_CONFIG_OUTLMT_DTV_1_5Vpp      0x00000000  /**< 1.5Vpp (default) */
#define sony_cxd_HELENE_CONFIG_OUTLMT_DTV_1_2Vpp      0x00040000  /**< 1.2Vpp */
/**@}*/

/**
 @name  Maximum IF and IQ output setting. (for satellite)
*/
/**@{*/
#define sony_cxd_HELENE_CONFIG_OUTLMT_STV_0_75Vpp     0x00000000  /**< 0.75Vpp (default) */
#define sony_cxd_HELENE_CONFIG_OUTLMT_STV_0_6Vpp      0x00020000  /**< 0.6Vpp */
/**@}*/

/**
 @name  REFOUT setting related macros.

        If following values are not used, REFOUT output is disabled.
*/
/**@{*/
/**
 @brief REFOUT enable, output level is 500mVp-p.
*/
#define sony_cxd_HELENE_CONFIG_REFOUT_500mVpp         0x00001000
/**
 @brief REFOUT enable, output level is 400mVp-p.
*/
#define sony_cxd_HELENE_CONFIG_REFOUT_400mVpp         0x00002000
/**
 @brief REFOUT enable, output level is 600mVp-p.
*/
#define sony_cxd_HELENE_CONFIG_REFOUT_600mVpp         0x00003000
/**
 @brief REFOUT enable, output level is 800mVp-p.
*/
#define sony_cxd_HELENE_CONFIG_REFOUT_800mVpp         0x00004000
/**
 @brief Internal use only. Do not use this value.
*/
#define sony_cxd_HELENE_CONFIG_REFOUT_MASK            0x00007000
/**@}*/

/*------------------------------------------------------------------------------
  APIs
------------------------------------------------------------------------------*/
/**
 @brief Set up the HELENE tuner driver.

        This MUST be called before calling sony_cxd_helene_Initialize.

 @param pTuner      Reference to memory allocated for the HELENE instance.
                    The create function will setup this HELENE instance.
 @param xtalFreq    The frequency of the HELENE crystal.
 @param i2cAddress  The HELENE tuner I2C slave address in 8-bit form.
 @param pI2c        The I2C APIs that the HELENE driver will use as the I2C interface.
 @param flags       Configuration flags. It should be ORed value of sony_cxd_HELENE_CONFIG_XXXX.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_Create(sony_cxd_helene_t *pTuner, sony_cxd_helene_xtal_t xtalFreq,
    uint8_t i2cAddress, sony_cxd_i2c_t *pI2c, uint32_t flags);

/**
 @brief Initialize the HELENE tuner.

        This MUST be called before calling sony_cxd_helene_terr_Tune and sony_cxd_helene_sat_Tune.

 @param pTuner       The HELENE tuner instance.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_Initialize(sony_cxd_helene_t *pTuner);

/**
 @brief Tune to a given RF frequency with terrestrial broadcasting system.

        To complete tuning, sony_cxd_helene_terr_TuneEnd should be called after waiting 50ms.

 @param pTuner       The HELENE tuner instance.
 @param frequencykHz RF frequency to tune. (kHz)
 @param tvSystem     The type of broadcasting system to tune.

 @return sony_cxd_RESULT_OK if tuner successfully tuned.
*/
sony_cxd_result_t sony_cxd_helene_terr_Tune(sony_cxd_helene_t *pTuner, uint32_t frequencykHz,
    sony_cxd_helene_tv_system_t tvSystem);

/**
 @brief Completes the terrestrial tuning sequence.

        This MUST be called after calling sony_cxd_helene_terr_Tune and 50ms wait.

 @param pTuner       The HELENE tuner instance.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_terr_TuneEnd(sony_cxd_helene_t *pTuner);

/**
 @brief Tune to a given RF frequency with satellite broadcasting system.

 @param pTuner         The HELENE tuner instance.
 @param frequencykHz   RF frequency to tune. (kHz)
 @param tvSystem       The type of broadcasting system to tune.
 @param symbolRateksps Symbol rate to tune. (ksps)

 @return sony_cxd_RESULT_OK if tuner successfully tuned.
*/
sony_cxd_result_t sony_cxd_helene_sat_Tune(sony_cxd_helene_t *pTuner, uint32_t frequencykHz,
    sony_cxd_helene_tv_system_t tvSystem, uint32_t symbolRateksps);

/**
 @brief Shift RF frequency.

        This API shift RF frequency without VCO calibration.
        This API is normally useful for analog TV "fine tuning" that
        shift RF frequency without picture distortion.

 @note  Please check the frequency range that VCO calibration is unnecessary.
        (See hardware specification sheet.)

 @param pTuner       The HELENE tuner instance.
 @param frequencykHz RF frequency to tune. (kHz)

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_ShiftFRF(sony_cxd_helene_t *pTuner, uint32_t frequencykHz);

/**
 @brief Put the HELENE tuner into Sleep state.

        From this state the tuner can be directly tuned by calling sony_cxd_helene_Tune.

 @param pTuner       The HELENE tuner instance.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_Sleep(sony_cxd_helene_t *pTuner);

/**
 @brief Write to GPO0 or GPO1.

 @param pTuner       The HELENE tuner instance.
 @param id           Pin ID 0 = GPO0, 1 = GPO1
 @param val          Output logic level, 0 = Low, 1 = High

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_SetGPO(sony_cxd_helene_t *pTuner, uint8_t id, uint8_t val);

/**
 @brief Read GPI1 value.

 @param pTuner       The HELENE tuner instance.
 @param pVal         GPI logic level, 0 = Low, 1 = High

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_GetGPI1(sony_cxd_helene_t *pTuner, uint8_t *pVal);

/**
 @brief Set the RF external circuit control pin. (for terrestrial)
        (Set RF_EXT_CTRL register.)

 @param pTuner       The HELENE tuner instance.
 @param enable       The value to set. (Enable(1) or Disable(0))

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_SetRfExtCtrl(sony_cxd_helene_t *pTuner, uint8_t enable);

/**
 @brief Set UK Freesat mode.

        If this mode is enabled, Freesat optimized setting will be used in sony_cxd_helene_sat_Tune.
        If internal loop filter is used, (sony_cxd_HELENE_CONFIG_LOOPFILTER_INTERNAL is ORed)
        UK Freesat mode has no effect.

 @param pTuner       The HELENE tuner instance.
 @param enable       The value to set. (Enable(1) or Disable(0))

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_SetFreesatMode(sony_cxd_helene_t *pTuner, uint8_t enable);

/**
 @brief Read the RSSI in dBm from the tuner. (for terrestrial)

 @note  Target level of IF signal output (depend on demodulator)
        is not added in default.

 @param pTuner          The HELENE tuner instance.
 @param pRssi           RSSI in dBm * 100

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_ReadRssi(sony_cxd_helene_t *pTuner, int32_t *pRssi);

/**
 @brief RF filter compensation setting for VHF-L band.
        (Please see RFVGA Description of datasheet.)

        New setting will become effective after next tuning.

        mult = coeff / 128
        (compensated value) = (original value) * mult + offset

 @param pTuner       The HELENE tuner instance.
 @param coeff        Multiplier value. (8bit unsigned)
 @param offset       Additional term. (8bit 2s complement)

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_helene_RFFilterConfig(sony_cxd_helene_t *pTuner, uint8_t coeff, uint8_t offset);

#endif /* sony_cxd_HELENE_H */
