/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/05
  Modification ID : c7c68d70868a4a9fb9e0f480d8b940e8a1e8f651
------------------------------------------------------------------------------*/
/**

 @file    sony_tuner_ascot3.h

          This file provides the DVB port of the Sony ASCOT3 tuner driver.

          This driver wraps around the Ascot3 driver provided by
          sony_ascot3.h by using an instance of the Ascot3 (sony_ascot3_t)
          driver in the ::sony_tuner_t::user pointer.
*/
/*----------------------------------------------------------------------------*/
#ifndef SONY_CXD6801_TUNER_ASCOT3_H
#define SONY_CXD6801_TUNER_ASCOT3_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_cxd6801_tuner.h"
#include "sony_cxd6801_ascot3.h"

/*------------------------------------------------------------------------------
 Driver Version
------------------------------------------------------------------------------*/
extern const char* sony_cxd6801_tuner_ascot3_version;  /**< ASCOT3 driver version */

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define SONY_CXD6801_TUNER_ASCOT3_OFFSET_CUTOFF_HZ         50000   /**< Maximum carrier offset frequency before requiring a retune */

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/

/**
 @brief Creates an instance of the ASCOT3 tuner driver

 @param pTuner The tuner driver instance to create. Memory
        must have been allocated for this instance before
        creation.
 @param xtalFreq The crystal frequency of the tuner.
 @param i2cAddress The I2C address of the ASCOT3 device.
        Typically 0xC0.
 @param pI2c The I2C driver that the tuner driver will use for
        communication.
 @param configFlags See "#define" ::SONY_ASCOT3_CONFIG_SLEEP_DISABLEXTAL,
        ::SONY_ASCOT3_CONFIG_LOOPFILTER_INTERNAL,
        ::SONY_ASCOT3_CONFIG_LOOPTHRU_ENABLE,
        ::SONY_ASCOT3_CONFIG_RFIN_MATCHING_ENABLE,
        ::SONY_ASCOT3_CONFIG_IF2_ATV,
        ::SONY_ASCOT3_CONFIG_AGC2_ATV,
        ::SONY_ASCOT3_CONFIG_IF2_DTV,
        ::SONY_ASCOT3_CONFIG_AGC2_DTV,
        ::SONY_ASCOT3_CONFIG_IFAGCSEL_ALL1,
        ::SONY_ASCOT3_CONFIG_IFAGCSEL_ALL2,
        ::SONY_ASCOT3_CONFIG_IFAGCSEL_A1D2,
        ::SONY_ASCOT3_CONFIG_IFAGCSEL_D1A2,
        ::SONY_ASCOT3_CONFIG_REFOUT_500mVpp
        ::SONY_ASCOT3_CONFIG_REFOUT_400mVpp
        ::SONY_ASCOT3_CONFIG_REFOUT_600mVpp
        ::SONY_ASCOT3_CONFIG_REFOUT_800mVpp defined in
        \link sony_ascot3.h \endlink
 @param pAscot3Tuner The Ascot3 tuner driver pointer to use.
        Memory must have been allocated for the Ascot3 driver structure.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_Create(sony_cxd6801_tuner_t * pTuner,
	sony_cxd6801_ascot3_xtal_t xtalFreq,
                                        uint8_t i2cAddress,
										sony_cxd6801_i2c_t * pI2c,
                                        uint32_t configFlags,
										sony_cxd6801_ascot3_t * pAscot3Tuner);

/**
 @brief Creates an instance of the ASCOT3R (CXD2875) tuner driver

 @param pTuner The tuner driver instance to create. Memory
        must have been allocated for this instance before
        creation.
 @param i2cAddress The I2C address of the ASCOT3R (CXD2875) device.
        Typically 0xC0.
 @param pI2c The I2C driver that the tuner driver will use for
        communication.
 @param configFlags See "#define" ::SONY_ASCOT3_CONFIG_SLEEP_DISABLEXTAL,
        ::SONY_ASCOT3_CONFIG_REFOUT_500mVpp
        ::SONY_ASCOT3_CONFIG_REFOUT_400mVpp
        ::SONY_ASCOT3_CONFIG_REFOUT_600mVpp
        ::SONY_ASCOT3_CONFIG_REFOUT_800mVpp defined in
        \link sony_ascot3.h \endlink
 @param pAscot3Tuner The Ascot3 tuner driver pointer to use.
        Memory must have been allocated for the Ascot3 driver structure.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_2875_Create(sony_cxd6801_tuner_t * pTuner,
                                             uint8_t i2cAddress,
											 sony_cxd6801_i2c_t * pI2c,
                                             uint32_t configFlags,
											 sony_cxd6801_ascot3_t * pAscot3Tuner);

/**
 @brief RF filter compensation setting for VHF-L band.
        (Please see RFVGA Description of datasheet.)
        New setting will become effective after next tuning.

        mult = VL_TRCKOUT_COEFF(8bit unsigned) / 128
        ofs  = VL_TRCKOUT_OFS(8bit 2s complement)
        (compensated value) = (original value) * mult + ofs

 @param pTuner Instance of the tuner driver.
 @param coeff  VL_TRCKOUT_COEFF (multiplier)
 @param offset VL_TRCKOUT_OFS (additional term)

 @return SONY_RESULT_OK if successful.
 */
sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_RFFilterConfig(sony_cxd6801_tuner_t * pTuner, uint8_t coeff, uint8_t offset);
/**
 @brief Write to GPIO0 or GPIO1.

 @param pTuner  Instance of the tuner driver.
 @param id      Pin ID 0 = GPIO0, 1 = GPIO1
 @param value   Output logic level, 0 = Low, 1 = High

 @return SONY_RESULT_OK if successful.
 */
sony_cxd6801_result_t sony_cxd6801_tuner_ascot3_SetGPO(sony_cxd6801_tuner_t * pTuner, uint8_t id, uint8_t value);

#endif /* SONY_CXD6801_TUNER_ASCOT3_H */
