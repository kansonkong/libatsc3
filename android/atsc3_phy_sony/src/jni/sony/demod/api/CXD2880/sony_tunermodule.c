/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: sony_tunermodule.c $
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

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/

#include "sony_tunermodule.h"
#include "smt_ew100_lna_thresh.h"
#include "smt_ew100_rflevel.h"
#include "smt_ew300_lna_thresh.h"
#include "smt_ew300_rflevel.h"


sony_result_t sony_tunermodule_RFLevelCalc(sony_tunerdemod_t * pTunerDemod, signed int * pRFLeveldB,
                                            sony_tunermodule_RfLevelParam  *pRFLevelTable);

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/

sony_result_t sony_tunermodule_RFLevelCompensation(sony_tunerdemod_t * pTunerDemod, signed int * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunermodule_RFLevelCompensation");

    if (pTunerDemod->isCableInput == 0){
        sony_tunermodule_RFLevelCalc(pTunerDemod, pRFLeveldB, pRfLevelParamAir);
    } else {
        sony_tunermodule_RFLevelCalc(pTunerDemod, pRFLeveldB, pRfLevelParamCable);
    }
    SONY_TRACE_RETURN (result);
}




sony_result_t sony_tunermodule_RFLevelCompensation_sub(sony_tunerdemod_t * pTunerDemod, signed int * pRFLeveldB)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunermodule_RFLevelCompensation_sub");

    if (pTunerDemod->isCableInput == 0){
        sony_tunermodule_RFLevelCalc(pTunerDemod, pRFLeveldB, pRfLevelParamAirSub);
    } else {
        sony_tunermodule_RFLevelCalc(pTunerDemod, pRFLeveldB, pRfLevelParamCableSub);
    }
    SONY_TRACE_RETURN (result);
}



sony_result_t sony_tunermodule_RFLevelCalc(sony_tunerdemod_t * pTunerDemod, signed int * pRFLeveldB,
                                            sony_tunermodule_RfLevelParam  *pRFLevelTable)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t  i;
	unsigned int  tune_freq;
	signed int  offset_x1000, param_diff, freq_diff;

    tune_freq = pTunerDemod->frequencyKHz;


    SONY_TRACE_ENTER ("sony_tunermodule_RFLevelCalc");


    i = 0;
    while( (tune_freq < pRFLevelTable[i].freq)  ){
        i++;
    }

        if ( ( i == 0 ) || (pRFLevelTable[i].freq == 0) ) {

           offset_x1000 = 0 ;

        } else {

           param_diff = pRFLevelTable[i - 1].param - pRFLevelTable[i].param;
           freq_diff = pRFLevelTable[i - 1].freq - pRFLevelTable[i].freq;

           if (freq_diff == 0) {
               SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
           }

           offset_x1000 = param_diff * (((tune_freq - pRFLevelTable[i].freq) * 1000) / freq_diff )
                         + pRFLevelTable[i].param * 1000;
        }

        *pRFLeveldB += ((offset_x1000 + 500) / 1000);

        SONY_TRACE_RETURN (result);


}


sony_result_t sony_tunermodule_Create(sony_tunerdemod_t * pTunerDemod)
{

    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunermodule_Create");

    result = sony_tunerdemod_SetRFLevelCompensation (pTunerDemod,
                                                     sony_tunermodule_RFLevelCompensation);

    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pRfLevelParamAir   = pRfLevelParamAir_Sinlge; 
    pRfLevelParamCable = pRfLevelParamCable_Sinlge;

    result = sony_tunerdemod_SetLNAThreshold(pTunerDemod,
                                             pLnaThreshAir_Single,
                                             pLnaThreshCable_Single);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunermodule_diver_Create(sony_tunerdemod_t * pTunerDemod)
{

    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunermodule_diver_Create");

    result = sony_tunerdemod_SetRFLevelCompensation (pTunerDemod,
                                                     sony_tunermodule_RFLevelCompensation);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pRfLevelParamAir   = pRfLevelParamAir_DiverMain; 
    pRfLevelParamCable = pRfLevelParamCable_DiverMain;

    result = sony_tunerdemod_SetRFLevelCompensation_sub (pTunerDemod,
                                                         sony_tunermodule_RFLevelCompensation_sub);

    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    pRfLevelParamAirSub   = pRfLevelParamAir_DiverSub; 
    pRfLevelParamCableSub = pRfLevelParamCable_DiverSub;


    result = sony_tunerdemod_SetLNAThreshold(pTunerDemod,
                                             pLnaThreshAir_DiverMain,
                                             pLnaThreshCable_DiverMain);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    result = sony_tunerdemod_SetLNAThreshold_sub (pTunerDemod,
                                                  pLnaThreshAir_DiverSub,
                                                  pLnaThreshCable_DiverSub);

    SONY_TRACE_RETURN (result);
}

