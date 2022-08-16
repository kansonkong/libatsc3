#ifndef DIBX09X_COMMON_H
#define DIBX09X_COMMON_H      

#include "dib0090.h"
#include "dibx09x.h"
#include "dibhost.h"
#include "dibfrontend.h"
#include "dibcomponent_monitor.h"
#include "dib7000m.h"
#include "dibosiris.h"
#include "dibfrontend_tune.h"
#include "dibpmu.h"

extern const struct dib0090_io_config dibx09x_io_12mhz_120;
extern const struct dib0090_io_config dibx09x_io_26mhz_120;
extern const struct dib0090_io_config dibx09x_io_30mhz_120;

extern const struct dib0090_io_config dibx09x_io_12mhz;
extern const struct dib0090_io_config dibx09x_io_26mhz;
extern const struct dib0090_io_config dibx09x_io_30mhz;

extern struct dibFrontend * dib29000_remote_fe_host_register(struct dibFrontend *fe, int id);
extern void ProcessSlaveSpalMsg(uint32_t * msg);

#endif
