/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: sony_tunermodule.h $
 *   Modified    : $Date: 2015/05/25 $ By $Author: $
 *   Revision    : $Revision: 1.3.0.0 $
 *   Description : SMT-EWX00 Tuner Control Sample Code
 *-----------------------------------------------------------------------------
 * This program may contain information about unfinished products and is subject
 * to change without notice.
 * Sony cannot assume responsibility for any problems arising out of the
 * use of the program
 *****************************************************************************/
/******************************************************************************
 *   includes:
 *    - system includes
 *    - application includes
 *****************************************************************************/

#ifndef SONY_TUNERMODULE_H
#define SONY_TUNERMODULE_H

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_stdlib.h"
#include "sony_common.h"
#include "sony_tunerdemod.h"



/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/

sony_result_t sony_tunermodule_RFLevelCompensation(sony_tunerdemod_t * pTunerDemod, signed int * pRFLeveldB);

sony_result_t sony_tunermodule_RFLevelCompensation_sub(sony_tunerdemod_t * pTunerDemod, signed int * pRFLeveldB);

sony_result_t sony_tunermodule_Create(sony_tunerdemod_t * pTunerDemod);

sony_result_t sony_tunermodule_diver_Create(sony_tunerdemod_t * pTunerDemod);
#endif /* SONY_TUNERMODULE_H */
