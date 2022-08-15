#ifndef __SONY_CXD_APIS_H__
#define __SONY_CXD_APIS_H__

#include "drvi2c_cxd_ite.h"
#include "sony_cxd_demod.h"
#include "sony_cxd_tuner_helene.h"
#include "sony_cxd_integ.h"
#include "sony_cxd_integ_isdbt.h"
#include "sony_cxd_demod_isdbt_monitor.h"
#include "sony_cxd_integ_dvbt_t2.h"
#include "sony_cxd_demod_dvbt_monitor.h"
#include "sony_cxd_demod_dvbt2_monitor.h"

typedef struct
{
	sony_cxd_demod_t					demod;
	sony_cxd_integ_t					integ;
	sony_cxd_i2c_t						i2c;
	sony_cxd_tuner_terr_cable_t			tunerTerrCable;
	sony_cxd_tuner_sat_t				tunerSat;
	sony_cxd_helene_t					helene;
	drvi2c_cxd_ite_t					iteI2c;
} sony_cxd_CXD285x_driver_instance_t;


#endif  //__SONY_CXD_APIS_H__
