/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/06/03
  Modification ID : 9e86320eeaf38db8a54afe84367509a18c3de734
------------------------------------------------------------------------------*/
/**
 @file  sony_integ.h

 @brief The integration layer interface independent of broadcasting systems.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_INTEG_H
#define SONY_INTEG_H

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_tunerdemod.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
#define SONY_TUNERDEMOD_WAIT_INITIALIZE_TIMEOUT        500    /**< Timeout to check for finishing initialize.(ms) */
#define SONY_TUNERDEMOD_WAIT_INITIALIZE_INTERVAL        10    /**< Polling interval to check for finishing initialize.(ms) */

#define SONY_TUNERDEMOD_WAIT_AGC_STABLE                100    /**< Wait for AGC stable before soft reset. */

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Initialize the device and into Sleep state.

        sony_tunerdemod_Initialize1 and sony_tunerdemod_Initialize2 are called.

 @note  The integration layer (sony_integ_xxx) APIs are provided to simplify
        the external API and therefore make end user driver porting an easier process.
        But note that the integration layer APIs includes long time sleep,
        that is occasionally prohibited to avoid long time blocking by driver APIs.
        In such cases, integration layer APIs may need be modified to fit the user's software framework.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successfull.
*/
sony_result_t sony_integ_Initialize (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Cancels current Tune or Scan operation.

        This function is thread safe, calling thread will get the result
        SONY_RESULT_ERROR_CANCEL.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if able to cancel the pending operation.
*/
sony_result_t sony_integ_Cancel (sony_tunerdemod_t * pTunerDemod);

/**
 @brief This API is called from sony_integ_XXX APIs to check that
        cancellation request by ::sony_integ_Cancel API is exist or not.

        The user can change this API code to check their system dependent
        cancellation request.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if there is no cancellation request.
         SONY_RESULT_ERROR_CANCEL if cancellation request exists.
*/
sony_result_t sony_integ_CheckCancellation (sony_tunerdemod_t * pTunerDemod);

#endif /* SONY_INTEG_H */
