/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/
/**
 @file    sony_tuner.h

          This file provides the tuner control interface.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONY_CXD6801_TUNER_H
#define SONY_CXD6801_TUNER_H

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_cxd6801_common.h"
#include "sony_cxd6801_i2c.h"
#include "sony_cxd6801_dtv.h"

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Recommended RF level function type.
*/
typedef enum {
	SONY_CXD6801_TUNER_RFLEVEL_FUNC_NOSUPPORT,  /** RF level function is not supported */
	SONY_CXD6801_TUNER_RFLEVEL_FUNC_READ,       /** ReadRFLevel function is recommended */
	SONY_CXD6801_TUNER_RFLEVEL_FUNC_CALCFROMAGC /** CalcRFLevelFromAGC function is recommended */
} sony_cxd6801_tuner_rflevel_func_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/**
 @brief Definition of the tuner driver API.
*/
typedef struct sony_cxd6801_tuner_t {
    /* ======== Following members are for configuration ======== */
    uint8_t i2cAddress;                 /**< I2C address. */
	sony_cxd6801_i2c_t * pI2c;                  /**< I2C driver instance. */
    uint32_t flags;                     /**< Flags that can be used by tuner drivers. */
    uint32_t symbolRateKSpsForSpectrum; /**< Symbol rate setting to get power spectrum. (for satellite blindscan) */

    /*
     * Following members are used in sony_integ_xxxx_monitor_RFLevel functions.
     * If ::SONY_TUNER_RFLEVEL_FUNC_READ is set, ReadRFLevel function pointer is called to get RF level.
     * If ::SONY_TUNER_RFLEVEL_FUNC_CALCFROMAGC is set, CalcRFLevelFromAGC function pointer is called to get RF level.
     */
	sony_cxd6801_tuner_rflevel_func_t rfLevelFuncTerr; /**< Recommended RF level function type for terrestrial/cable */
	sony_cxd6801_tuner_rflevel_func_t rfLevelFuncSat;  /**< Recommended RF level function type for satellite */

    /*
     * Following members are only used for software combined tuner case.
     * If the system supports both terrestrial/cable and satellite system,
     * terrestrial/satellite combined tuner is normally used. (Like Sony HELENE/HELENE2 tuner)
     * But if the user would like to use 2 tuners, (terrestrial/cable only tuner + satellite only tuner)
     * special sony_tuner_t object, "software combined" tuner object should be prepared.
     * Please check ::sony_tuner_sw_combined_Create API to configure software combined tuner.
     */
	struct sony_cxd6801_tuner_t * pTunerTerrCable;     /**< Terrestrial/cable only tuner object. */
	struct sony_cxd6801_tuner_t * pTunerSat;           /**< Satellite only tuner object. */

    void * user;                        /**< User defined data. */

    /* ======== Following members are for storing current status ======== */
    uint32_t frequencyKHz;              /**< Current RF frequency(kHz) tuned. */
	sony_cxd6801_dtv_system_t system;           /**< Current system tuned. */
	sony_cxd6801_dtv_bandwidth_t bandwidth;     /**< Current bandwidth tuned. (for terrestrial/cable) */
    uint32_t symbolRateKSps;            /**< Current symbol rate (KSps) tuned. (for satellite) */

    /* ======== Function pointers ======== */
    /**
     @brief Initialize the tuner.

     @param pTuner Instance of the tuner driver.

     @return SONY_RESULT_OK if successful.
    */
	sony_cxd6801_result_t(*Initialize) (struct sony_cxd6801_tuner_t * pTuner);

    /**
     @brief Tune to a given frequency with a given bandwidth. (Terrestrial/Cable)

            Tuner driver can block while waiting for PLL locked indication (if required).

     @param pTuner Instance of the tuner driver.
     @param centerFreqKHz RF frequency to tune to (kHz)
     @param system The broadcasting system of the channel to tune to.
     @param bandwidth The bandwidth of the channel.

     @return SONY_RESULT_OK if tuner successfully tuned.
    */
	sony_cxd6801_result_t(*TerrCableTune) (struct sony_cxd6801_tuner_t * pTuner,
                                    uint32_t centerFreqKHz,
									sony_cxd6801_dtv_system_t system,
									sony_cxd6801_dtv_bandwidth_t bandwidth);

    /**
     @brief Tune to a given frequency with a given system and symbol rate. (Satellite)

            Tuner driver can block while waiting for PLL locked indication (if required).

     @param pTuner Instance of the tuner driver.
     @param centerFreqKHz RF frequency to tune to (kHz)
     @param system The broadcasting system of the channel to tune to.
     @param symbolRateKSps The symbol rate of the channel in KHz.

     @return SONY_RESULT_OK if tuner successfully tuned.
    */
	sony_cxd6801_result_t(*SatTune) (struct sony_cxd6801_tuner_t * pTuner,
                              uint32_t centerFreqKHz,
							  sony_cxd6801_dtv_system_t system,
                              uint32_t symbolRateKSps);

    /**
     @brief Sleep the tuner device (if supported).

     @param pTuner Instance of the tuner driver.

     @return SONY_RESULT_OK if successful.
    */
	sony_cxd6801_result_t(*Sleep) (struct sony_cxd6801_tuner_t * pTuner);

    /**
     @brief Shutdown the tuner device (if supported).

     @param pTuner Instance of the tuner driver.

     @return SONY_RESULT_OK if successful.
    */
	sony_cxd6801_result_t(*Shutdown) (struct sony_cxd6801_tuner_t * pTuner);

    /**
     @brief Read RF level from the tuner device (if supported).

     @param pTuner Instance of the tuner driver.
     @param pRFLevel The RF Level estimation in dBm * 100.

     @return SONY_RESULT_OK if tuner successfully tuned.
    */
	sony_cxd6801_result_t(*ReadRFLevel) (struct sony_cxd6801_tuner_t * pTuner,
                                  int32_t *pRFLevel);

    /**
     @brief Calculate RF level from demod AGC monitor value (if supported).

            Note: This function should not access to tuner.

     @param pTuner Instance of the tuner driver.
     @param agcValue Sony demodulator AGC value.
     @param pRFLevel The RF Level estimation in dBm * 100.

     @return SONY_RESULT_OK if tuner successfully tuned.
    */
	sony_cxd6801_result_t(*CalcRFLevelFromAGC) (struct sony_cxd6801_tuner_t * pTuner,
                                         uint32_t agcValue,
                                         int32_t *pRFLevel);
} sony_cxd6801_tuner_t;

/**
 @brief Create both terrestrial/cable and satellite supported virtual tuner object.

        This API should be used if terrestrial/cable only tuner and satellite only tuner are both used in the system.

        NOTE: terrestrial/cable tuner instance (*pTunerTerrCable) and satellite tuner instance (*pTunerSat)
        should be correctly initialized (by sony_tuner_xxx_Create function) before this function call.

 @param pTuner Instance of the for software combined virtual tuner object.
 @param pTunerTerrCable Instance of the terrestrial/cable only tuner driver.
 @param pTunerSat Instance of the satellite only tuner driver.

 @return SONY_RESULT_OK if tuner successfully tuned.
*/
sony_cxd6801_result_t sony_cxd6801_tuner_sw_combined_Create(sony_cxd6801_tuner_t * pTuner,
	sony_cxd6801_tuner_t * pTunerTerrCable,
	sony_cxd6801_tuner_t * pTunerSat);

#endif /* SONY_CXD6801_TUNER_H */
