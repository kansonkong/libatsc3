/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: smt_ew300_lna_thresh.h $
 *   Modified    : $Date: 2015/05/13 $ By $Author: $
 *   Revision    : $Revision: 1.2.0.0 $
 *   Description : SMT-EW300 Tuner Control Sample Code
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


#ifndef SMT_EW300_LNA_THRESH_H
#define SMT_EW300_LNA_THRESH_H

#include "smt_common.h"


sony_tunerdemod_lna_threshold_table_air_t SMT_EW300_AIR_LNA_THRESH_MAIN =
{
    {
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 }
    }
};

sony_tunerdemod_lna_threshold_table_air_t SMT_EW300_AIR_LNA_THRESH_SUB =
{
    {
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 },
        { 0x00 , 0x05 }
    }
};


#if 0   // No change for CABLE input
sony_tunerdemod_lna_threshold_table_cable_t SMT_EW300_CABLE_LNA_THRESH_MAIN =
{
    {
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 }
    }
};

sony_tunerdemod_lna_threshold_table_cable_t SMT_EW300_CABLE_LNA_THRESH_SUB =
{
    {
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 },
        { 0x00 , 0x08 }
    }
};
#endif


sony_tunerdemod_lna_threshold_table_air_t
      *pLnaThreshAir_DiverMain = &SMT_EW300_AIR_LNA_THRESH_MAIN;
sony_tunerdemod_lna_threshold_table_air_t
      *pLnaThreshAir_DiverSub  = &SMT_EW300_AIR_LNA_THRESH_SUB;


sony_tunerdemod_lna_threshold_table_cable_t
      *pLnaThreshCable_DiverMain = NULL;
sony_tunerdemod_lna_threshold_table_cable_t
      *pLnaThreshCable_DiverSub  = NULL;



#endif