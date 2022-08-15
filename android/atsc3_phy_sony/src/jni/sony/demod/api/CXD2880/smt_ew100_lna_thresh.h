/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: smt_ew100_lna_thresh.h $
 *   Modified    : $Date: 2015/05/13 $ By $Author: $
 *   Revision    : $Revision: 1.2.0.0 $
 *   Description : SMT-EW100 Tuner Control Sample Code
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


#ifndef SMT_EW100_LNA_THRESH_H
#define SMT_EW100_LNA_THRESH_H

#include "smt_common.h"


sony_tunerdemod_lna_threshold_table_air_t SMT_EW100_AIR_LNA_THRESH =
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


sony_tunerdemod_lna_threshold_table_cable_t SMT_EW100_CABLE_LNA_THRESH =
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




sony_tunerdemod_lna_threshold_table_air_t
      *pLnaThreshAir_Single = &SMT_EW100_AIR_LNA_THRESH;



sony_tunerdemod_lna_threshold_table_cable_t
      *pLnaThreshCable_Single = &SMT_EW100_CABLE_LNA_THRESH;

#endif