/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_tuner_terr_cable.h

          This file provides the tuner control interface for Terr/Cable.
*/
/*----------------------------------------------------------------------------*/
#ifndef sony_cxd_TUNER_TERR_CABLE_H
#define sony_cxd_TUNER_TERR_CABLE_H

#include "sony_cxd_common.h"
#include "sony_cxd_i2c.h"
#include "sony_cxd_dtv.h"

/**
 @brief Definition of the Terrestrial / Cable tuner driver API.
*/
typedef struct sony_cxd_tuner_terr_cable_t {
    uint8_t i2cAddress;                 /**< I2C address. */
    sony_cxd_i2c_t * pI2c;                  /**< I2C driver instance. */
    uint32_t flags;                     /**< Flags that can be used by tuner drivers. */
    uint32_t frequencyKHz;              /**< Current RF frequency(kHz) tuned. */
    sony_cxd_dtv_system_t system;           /**< Current system tuned. */
    sony_cxd_dtv_bandwidth_t bandwidth;   /**< Cuurent bandwidth tuned. */

    /**
     @brief Initialize the tuner.

     @param pTuner Instance of the tuner driver.

     @return sony_cxd_RESULT_OK if successful.
    */
    sony_cxd_result_t (*Initialize) (struct sony_cxd_tuner_terr_cable_t * pTuner);

    /**
     @brief Tune to a given frequency with a given bandwidth.
            Tuner driver can block while waiting for PLL locked indication (if required).

     @param pTuner Instance of the tuner driver.
     @param centerFrequencyKHz RF frequency to tune too (kHz)
     @param system The type of channel to tune too.
     @param bandwidth The bandwidth of the channel.

     @return sony_cxd_RESULT_OK if tuner successfully tuned.
    */
    sony_cxd_result_t (*Tune) (struct sony_cxd_tuner_terr_cable_t * pTuner,
                           uint32_t centerFrequencyKHz,
                           sony_cxd_dtv_system_t system,
                           sony_cxd_dtv_bandwidth_t bandwidth);

    /**
     @brief Sleep the tuner device (if supported).

     @param pTuner Instance of the tuner driver.

     @return sony_cxd_RESULT_OK if successful.
    */
    sony_cxd_result_t (*Sleep) (struct sony_cxd_tuner_terr_cable_t * pTuner);

    /**
     @brief Shutdown the tuner device (if supported).

     @param pTuner Instance of the tuner driver.

     @return sony_cxd_RESULT_OK if successful.
    */
    sony_cxd_result_t (*Shutdown) (struct sony_cxd_tuner_terr_cable_t * pTuner);

    /**
     @brief Read RF level from the tuner device (if supported).

     @param pTuner Instance of the tuner driver.
     @param pRFLevel The RF Level estimation in dB * 1000.

     @return sony_cxd_RESULT_OK if tuner successfully tuned.
    */
    sony_cxd_result_t (*ReadRFLevel) (struct sony_cxd_tuner_terr_cable_t * pTuner,
                                  int32_t *pRFLevel);

    /**
     @brief Calculate RF level from demod AGC monitor value (if supported).

            Note: This function should not access to tuner.

     @param pTuner Instance of the tuner driver.
     @param agcValue Sony demodulator AGC value.
     @param pRFLevel The RF Level estimation in dB * 1000.

     @return sony_cxd_RESULT_OK if tuner successfully tuned.
    */
    sony_cxd_result_t (*CalcRFLevelFromAGC) (struct sony_cxd_tuner_terr_cable_t * pTuner,
                                         uint32_t agcValue,
                                         int32_t *pRFLevel);

    void * user;                /**< User defined data. */
} sony_cxd_tuner_terr_cable_t;

#endif /* sony_cxd_TUNER_TERR_CABLE_H */
