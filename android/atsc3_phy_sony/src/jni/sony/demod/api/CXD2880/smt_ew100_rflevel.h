/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: smt_ew100_rflevel.h $
 *   Modified    : $Date: 2015/06/12 $ By $Author: $
 *   Revision    : $Revision: 1.3.0.0 $
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


#ifndef SMT_EW100_RFLEVEL_H
#define SMT_EW100_RFLEVEL_H

#include "smt_common.h"

sony_tunermodule_RfLevelParam SMT_EW100_AIR[] = {
{   864000      ,    18250   },
{   766000      ,     7340   },
{   726000      ,     5470   },
{   670000      ,     5870   },
{   582000      ,     4030   },
{   502000      ,     2660   },
{   468000      ,     1620   },
{   467000      ,    19100   },
{   398000      ,    15920   },
{   350000      ,    12380   },
{   301000      ,     9110   },
{   300000      ,     8450   },
{   174000      ,     2200   },
{        0      ,     0      }
};


sony_tunermodule_RfLevelParam SMT_EW100_CABLE[] = {
{   864000      ,     7650   },
{   766000      ,     6510   },
{   726000      ,     5610   },
{   670000      ,     4680   },
{   582000      ,     2910   },
{   502000      ,     1700   },
{   468000      ,     1800   },
{   467000      ,      950   },
{   398000      ,     1080   },
{   350000      ,     1420   },
{   301000      ,     1210   },
{   300000      ,     2570   },
{   174000      ,     1270   },
{    90000      ,     1310   },
{        0      ,        0   }
};

#if 0
sony_tunermodule_RfLevelParam SMT_EW100_NO_OFFSET[] = {
    {      0,    0 }
};
#endif

sony_tunermodule_RfLevelParam
   *pRfLevelParamAir_Sinlge    = SMT_EW100_AIR;

sony_tunermodule_RfLevelParam
   *pRfLevelParamCable_Sinlge  = SMT_EW100_CABLE;

#endif