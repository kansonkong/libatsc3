/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/09/28
  Modification ID : 1ea04def104d657deb64f0d0b0cae6ef992a9dd8
------------------------------------------------------------------------------*/
/**
 @file    sony_demod_atsc3_chbond.h

          This file provides the demodulator control and monitor interface for ATSC 3.0 channel bonding.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_DEMOD_ATSC3_CHBOND_H
#define SONY_CXD6801_DEMOD_ATSC3_CHBOND_H

#include "sony_cxd6801_demod.h"
#include "sony_cxd6801_atsc3_chbond.h"

/*------------------------------------------------------------------------------
 Enumurations
------------------------------------------------------------------------------*/
/**
 @brief PLP channel bonding configuration.
*/
typedef enum {
    SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_ENABLE,          /**< PLP bonding enable */
    SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_MAIN,       /**< PLP bonding disable (from main only) */
    SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_BOND_FROM_SUB         /**< PLP bonding disable (from sub only) */
} sony_cxd6801_demod_atsc3_chbond_plp_bond_t;

/**
 @brief PLP channel bonding configuration check status.
*/
typedef enum {
    SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_UNUSED,         /**< The PLP ID is not used */
    SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_FROM_SUB,       /**< The PLP is configured as input from sub IC */
    SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_ERR_ID,         /**< One or more PLP IDs are not found */
    SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_ERR_BONDCONF,   /**< PLP bond configuration is incorrect */
    SONY_CXD6801_DEMOD_ATSC3_CHBOND_PLP_VALID_OK              /**< PLP ID configuration is no problem */
} sony_cxd6801_demod_atsc3_chbond_plp_valid_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Enable/disable ATSC 3.0 channel bonding function.

        This API is called internally to enable/disable channel bonding.

 @param pDemod  The demodulator instance
 @param enable  Enable/disable setting.
                    - 0: Disable
                    - 1: Enable

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_Enable (sony_cxd6801_demod_t * pDemod,
                                              uint8_t enable);

/**
 @brief Setup the PLP configuration of the demodulator for channel bonding case.

 @param pDemod The demodulator instance.
 @param plpIDNum Number of valid PLP ID in plpID[]. (1 - 4)
 @param plpID The pointer of PLP ID 4 length array to set.
 @param plpBond The pointer of PLP bond option 4 length array to set.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_SetPLPConfig (sony_cxd6801_demod_t * pDemod,
                                                    uint8_t plpIDNum,
                                                    uint8_t plpID[4],
                                                    sony_cxd6801_demod_atsc3_chbond_plp_bond_t plpBond[4]);

/**
 @brief Monitors the synchronisation state of the ATSC 3.0 demodulator.

 @note  Note that early unlock condition should be used in tuning stage ONLY
        to detect that there are no desired signal in current frequency quickly.
        After tuning, early unlock condition should NOT be used to
        know current demodulator lock status basically.

 @param pDemod The demodulator instance.
 @param bondLockStat Pointer of 4 length array to store ALP lock status for each PLPs.
        - 0: Bonding not locked.
        - 1: Bonding locked.
 @param pBondLockAll Indicates all selected valid PLPs by ::sony_demod_atsc3_chbond_SetPLPConfig are bonding locked or not.
        - 0: Not all PLPs are locked.
        - 1: All PLPs are locked.
 @param pBondUnlockDetected Indicates the channel bonding unlock is detected.
        - 0: No unlock.
        - 1: Unlock detected.

 @return SONY_RESULT_OK if pSyncStat, pTSLockStat is valid, otherwise an error is returned.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_monitor_BondStat (sony_cxd6801_demod_t * pDemod,
                                                        uint8_t bondLockStat[4],
                                                        uint8_t * pBondLockAll,
                                                        uint8_t * pBondUnlockDetected);

/**
 @brief Monitor the selected 4 PLPs are valid or not.

        This API can check that the wrong channel bonding setting for each PLPs.
        For example, the PLP is "channel bonded" but tuned as normal PLP.
        Note that the tune APIs can finish successfully even if such wrong setting was done.

 @param pDemod The demodulator instance.
 @param plpValid Pointer of 4 length array to store PLP valid information for each PLPs.
        See ::sony_demod_atsc3_chbond_plp_valid_t definition.

 @return SONY_RESULT_OK if successful.
         SONY_RESULT_ERROR_HW_STATE if all 4 PLPs are invalid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_monitor_SelectedPLPValid (sony_cxd6801_demod_t * pDemod,
                                                                sony_cxd6801_demod_atsc3_chbond_plp_valid_t plpValid[4]);

/**
 @brief Get channel bonding related information in L1-Detail.

 @param pDemod         The demodulator instance.
 @param pL1Detail      Pointer to receive the channel bonding related L1-Detail information.

 @return SONY_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_atsc3_chbond_monitor_L1Detail (sony_cxd6801_demod_t * pDemod,
                                                        sony_cxd6801_atsc3_chbond_l1detail_t * pL1Detail);

#endif /* SONY_DEMOD_ATSC3_CHBOND_H */
