#ifndef DIB9080_COMMON_H
#define DIB9080_COMMON_H

#include "dibsip.h"

#include "dib7000m.h"

#include "dibx09x_common.h"

struct dibPMU;

struct dib9090_state {
	struct dibSIPInfo info;
	struct dib0090_config dib0090_cfg;
	struct dib9000_config dib9000_cfg;
	int (*tuner_rst)(struct dibFrontend *);
	struct dibPMU *pmu; /* there is a PMU attached to this SIP */

};

extern struct dib9090_state * dib9090_sip_register_common(struct dibFrontend *fe, struct dibDataBusHost *bus, const struct dib9090_config *cfg);

#endif
