#ifndef __SONY_CXD6801_APIS_H__
#define __SONY_CXD6801_APIS_H__

#include "drvi2c_cxd6801_ite.h"
#include "sony_cxd6801_demod_atsc.h"
#include "sony_cxd6801_demod_atsc3.h"
#include "sony_cxd6801_integ_atsc.h"
#include "sony_cxd6801_integ_atsc3.h"
#include "sony_cxd6801_demod_atsc_monitor.h"
#include "sony_cxd6801_demod_atsc3_monitor.h"
#include "sony_cxd6801_ascot3.h"
#include "sony_cxd6801_tuner_ascot3.h"

typedef struct
{
	sony_cxd6801_demod_t				demod;
	sony_cxd6801_integ_t				integ;
	sony_cxd6801_i2c_t					i2c;
	sony_cxd6801_tuner_t				tuner;
	sony_cxd6801_ascot3_t				ascot3;
	drvi2c_cxd6801_ite_t				iteI2c;
} sony_cxd6801_driver_instance_t;


#endif  //__SONY_CXD6801_APIS_H__
