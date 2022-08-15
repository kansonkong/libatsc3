/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: smt_common.h $
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


#ifndef SMT_COMMON_H
#define SMT_COMMON_H

#include "sony_common.h"

typedef struct sony_tunermodule_RfLevelParam {
	unsigned int	freq;
	signed int		param;
} sony_tunermodule_RfLevelParam;

sony_tunermodule_RfLevelParam SMT_NO_OFFSET[] = {
    {      0,    0 }
};

sony_tunermodule_RfLevelParam
   *pRfLevelParamAir   = SMT_NO_OFFSET;

sony_tunermodule_RfLevelParam
   *pRfLevelParamCable = SMT_NO_OFFSET;

sony_tunermodule_RfLevelParam
   *pRfLevelParamAirSub    = SMT_NO_OFFSET;

sony_tunermodule_RfLevelParam
   *pRfLevelParamCableSub  = SMT_NO_OFFSET;
#endif