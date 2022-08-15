/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: smt_ew300_rflevel.h $
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


#ifndef SMT_EW300_RFLEVEL_H
#define SMT_EW300_RFLEVEL_H

#include "smt_common.h"

sony_tunermodule_RfLevelParam SMT_EW300_AIR_MAIN[] = {
{   864000      ,     4260   },
{   766000      ,     3820   },
{   726000      ,     3620   },
{   670000      ,     3440   },
{   582000      ,     2520   },
{   502000      ,     2850   },
{   468000      ,     3570   },
{   467000      ,    14790   },
{   398000      ,     6930   },
{   350000      ,     4910   },
{   301000      ,     3910   },
{   300000      ,     3700   },
{   174000      ,     3220   },
{        0      ,        0   }
};


sony_tunermodule_RfLevelParam SMT_EW300_AIR_SUB[] = {
{   864000      ,     4810   },
{   766000      ,     3990   },
{   726000      ,     3730   },
{   670000      ,     3270   },
{   582000      ,     2320   },
{   502000      ,     2820   },
{   468000      ,     3690   },
{   467000      ,    15730   },
{   398000      ,     7530   },
{   350000      ,     5230   },
{   301000      ,     4450   },
{   300000      ,     4150   },
{   174000      ,     3470   },
{        0      ,        0   }
};


sony_tunermodule_RfLevelParam SMT_EW300_NO_OFFSET[] = {
    {      0,    0 }
};

sony_tunermodule_RfLevelParam
   *pRfLevelParamAir_DiverMain   = SMT_EW300_AIR_MAIN;

sony_tunermodule_RfLevelParam
   *pRfLevelParamCable_DiverMain = SMT_EW300_NO_OFFSET;

sony_tunermodule_RfLevelParam
   *pRfLevelParamAir_DiverSub    = SMT_EW300_AIR_SUB;

sony_tunermodule_RfLevelParam
   *pRfLevelParamCable_DiverSub  = SMT_EW300_NO_OFFSET;
#endif