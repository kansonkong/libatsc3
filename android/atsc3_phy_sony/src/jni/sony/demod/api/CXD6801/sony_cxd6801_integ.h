/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/
/**
 @file    sony_integ.h

          This file provides the integration layer control interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_INTEG_H
#define SONY_CXD6801_INTEG_H

#include "sony_cxd6801_demod.h"
#include "sony_cxd6801_tuner.h"


/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief The high level driver object.
        This is the primary interface used for controlling the demodulator and
        connected tuner devices.

        This object is the combination of the demodulator and the tuner devices
        to represent a single front end system. It can be used wholly, partly or
        just as a reference for developing the application software.
*/
typedef struct sony_cxd6801_integ_t {
	sony_cxd6801_demod_t * pDemod;                          /**< Instance of the demodulator. */
	sony_cxd6801_tuner_t * pTuner;                          /**< Instance of the connected tuner. */


	sony_cxd6801_atomic_t cancel;                           /**< Cancellation indicator variable. */

    void * user;                                    /**< Storage for user specific data to be passed in/out of the integration layer. */
} sony_cxd6801_integ_t;

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
/**
 @brief Construct the driver.

        This function is called by the application in order to setup the
        ::sony_demod_t structure and provide references for the
        ::sony_integ_t::pDemod, ::sony_integ_t::pTuner,
        and ::sony_integ_t::pLnbc members.

        This MUST be called before calling ::sony_integ_Initialize.

@note Passing NULL as pTuner or pLnbc will disable communication
      with the device without throwing an argument error.

        Notes on driver configuration:
        - By default, the demodulator uses an inverted AGC.  If the connected
          tuner uses a non-inverted IFAGC, call ::sony_demod_SetConfig with
          ::SONY_DEMOD_CONFIG_IFAGCNEG = 0 to setup the demodulator with positive
          IFAGC sense, after calling ::sony_integ_Initialize.

 @note  Memory is not allocated dynamically.

 @param pInteg The driver object to create. It must be a valid pointer to
        allocated memory for a ::sony_integ_t structure.
 @param pDemod Reference to memory allocated for the demodulator instance. The
        create function will setup this demodulator instance also.
 @param pCreateParam Parameters to create this driver. (I2C slave address, Xtal frequency...)
 @param pDemodI2c The I2C driver that the demod will use as the I2C interface.
 @param pTuner The tuner driver to use with this instance of the driver.
 @param pLnbc The LNB control interface.

 @return SONY_RESULT_OK if successfully created integration layer driver structure.
*/
sony_cxd6801_result_t sony_cxd6801_integ_Create(sony_cxd6801_integ_t * pInteg,
	sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_create_param_t * pCreateParam,
	sony_cxd6801_i2c_t * pDemodI2c,
	sony_cxd6801_tuner_t * pTuner
                                 );

/**
 @brief Initialize the demodulator and tuners.

        After initialization, the demodulator, tuner and LNB controller are in a low power state
        (demod state = ::SONY_DEMOD_STATE_SLEEP) awaiting tune or scan commands.

        Normally be used from Power On (::SONY_DEMOD_STATE_UNKNOWN).
        Calling from Active, Sleep or Shutdown states will cause a demodulator reset.

 @param pInteg The driver instance.

 @return SONY_RESULT_OK if OK.
*/
sony_cxd6801_result_t sony_cxd6801_integ_Initialize(sony_cxd6801_integ_t * pInteg);

/**
 @brief Put the demodulator and tuner devices into a low power state.

        This function can be called from Shutdown or Active states.
        Note that LNB controller is not controlled by this function.

 @param pInteg The driver instance.

 @return SONY_RESULT_OK if OK.
*/
sony_cxd6801_result_t sony_cxd6801_integ_Sleep(sony_cxd6801_integ_t * pInteg);

/**
 @brief Shutdown the demodulator, tuner and LNB controller into a low power disabled state.

        Call ::sony_integ_Sleep to go out from Shutdown state.
        ::sony_integ_Shutdown can be directly called from Sleep or Active states.

 @param pInteg The driver instance.

 @return SONY_RESULT_OK if OK.
*/
sony_cxd6801_result_t sony_cxd6801_integ_Shutdown(sony_cxd6801_integ_t * pInteg);

/**
 @brief Cancels current Tune or Scan operation in the demod and tuner parts.

        This function is thread safe, calling thread will get the result
        SONY_RESULT_ERROR_CANCEL.

 @param pInteg The driver instance.

 @return SONY_RESULT_OK if able to cancel the pending operation.
*/
sony_cxd6801_result_t sony_cxd6801_integ_Cancel(sony_cxd6801_integ_t * pInteg);

/**
 @brief Clear cancel flag.

        If sony_integ_Cancel is used,
        this API should be called to clear cancel information.

 @param pInteg The driver instance.

 @return SONY_RESULT_OK if able to cancel the pending operation.
*/
sony_cxd6801_result_t sony_cxd6801_integ_ClearCancel(sony_cxd6801_integ_t * pInteg);

/**
 @brief Check cancel request from integ APIs.

        This API is called from sony_integ_XXX APIs to check that
        cancellation request by ::sony_integ_Cancel API is exist or not.
        The user can change this API code to check their system dependent
        cancellation request.

 @param pInteg The driver instance.

 @return SONY_RESULT_OK if there is no cancellation request.
         SONY_RESULT_ERROR_CANCEL if cancellation request exists.
*/
sony_cxd6801_result_t sony_cxd6801_integ_CheckCancellation(sony_cxd6801_integ_t * pInteg);

#endif /* SONY_CXD6801_INTEG_H */
