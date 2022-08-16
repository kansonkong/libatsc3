/* frontend-only firmware support */

#include "dibfrontend.h"
#include "dibsip.h"
#include "dibpmu.h"

#include "dibx09x.h"

#include "dib9090_common.h"

#include "dibmicrocode_B_9090-fe.h"

static int dib9090_firmware_post_pll(struct dibFrontend *fe)
{
    struct dib9090_state *state = (struct dib9090_state *)(fe->sip->priv);
    if (state->tuner_rst(fe) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

    if (state->pmu)
        pmu_reset(state->pmu);

    return dib9000_firmware_post_pll_init(fe);
}

struct dibFrontend * dib9090_firmware_sip_register(struct dibFrontend *fe, uint8_t i2c_addr, struct dibDataBusHost *bus, const struct dib9090_config *cfg)
{
    struct dibDataBusHost *intf;
    struct dib9090_state *state = dib9090_sip_register_common(fe, bus, cfg);
    if (state == NULL)
        return NULL;

	state->dib9000_cfg.microcode_B_fe_buffer = microcode_B_fe_buffer;
	state->dib9000_cfg.microcode_B_fe_size = microcode_B_fe_size;

    if (dib9000_firmware_register(fe, bus, i2c_addr, &state->dib9000_cfg) == NULL)
        goto demod_error;

    intf = dib9000_get_tuner_interface(fe);
    if (dib0090_reset_register(fe, intf, &state->dib0090_cfg) == NULL)
        goto tuner_error;

    state->tuner_rst = fe->tuner_info->ops.reset;
    fe->tuner_info->ops.reset = dib9090_firmware_post_pll;

    return fe;

tuner_error:
    frontend_unregister_demod(fe);
demod_error:
    MemFree(state, sizeof(struct dib9090_state));
    return NULL;
}

