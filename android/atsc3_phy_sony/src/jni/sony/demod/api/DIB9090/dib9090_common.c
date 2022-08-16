#include "dib9090_common.h"

#include "dibdemod.h"
#include "dibpmu.h"

DEBUG_OBJECT_STATIC(dib9090_dbg, DEBUG_SIP, "DIB9090");

static int dib9090_tuner_reset_cb(struct dibFrontend *fe, int onoff)
{
    dbgpl(&dib9090_dbg, "DiB0090 reset: %d", onoff);
    return demod_set_gpio(fe, 5, 0, !onoff);
}

static int dib9090_tuner_sleep_cb(struct dibFrontend *fe, int onoff)
{
    dbgpl(&dib9090_dbg, "DiB0090 sleep: %d", onoff);
    return demod_set_gpio(fe, 0, 0, (uint8_t) onoff);
}

static const struct dib9000_config default_dib9000_config = {
    1,
    DIB7000M_GPIO_DEFAULT_DIRECTIONS, //& ~(1 << 4),
    DIB7000M_GPIO_DEFAULT_VALUES, //     |  (1 << 4),
    DIB7000M_GPIO_DEFAULT_PWM_POS,
    0,
};

static const struct dib0090_wbd_slope dib9090_wbd_table[] = {
  { 0,      0,   0, 130, 130 },  // Temp=0 means -30deg
  { 470,    0, 250,   0, 100, 6},  // VHF Slopes
  { 858,   51, 866,  21, 375, 4},  // UHF Slopes
  { 0xFFFF, 0,   0,   0, 0,   0},  // End
};

static const struct dib0090_config default_dib0090_config = {
    { 0 },

    dib9090_tuner_reset_cb,
    dib9090_tuner_sleep_cb,

    0, /* freq_offset_khz_uhf */
	0, /* freq_offset_khz_vhf */
	0, /* get_adc_power */
	0, /* clkouttobamse */
	0, /* analog_output */
	0, /* i2c_address */
	0,
	0,
	0, /* use_pwm_agc */
	0, /* clkoutdrive */
	0, /* fref_clock_ratio */
	0, /* cap_value */
	dib9090_wbd_table,
};

static void dib9090_release(struct dibFrontend *fe)
{
    struct dib9090_state *state = (struct dib9090_state *)(fe->sip->priv);
    pmu_release(state->pmu);
    MemFree(state, sizeof(struct dib9090_state));
}

static const struct dibSIPInfo dib9090_info = {
    INFO_TEXT("DiBcom DiB9090"),
    {
        dib9090_release
    }
};

void dib9090_set_pmu(struct dibFrontend *fe, struct dibPMU *pmu)
{
    struct dib9090_state *state = (struct dib9090_state *)(fe->sip->priv);
    state->pmu = pmu;
}

struct dib9090_state * dib9090_sip_register_common(struct dibFrontend *fe, struct dibDataBusHost *bus, const struct dib9090_config *cfg)
{
    struct dib9090_state *state = (struct dib9090_state *)(MemAlloc(sizeof(struct dib9090_state)));
    uint8_t i;
    if (state == NULL)
        return NULL;
    DibZeroMemory(state, sizeof(struct dib9090_state));

    frontend_register_sip(fe, &state->info, &dib9090_info, state);

    memcpy(&state->dib9000_cfg, &default_dib9000_config, sizeof(struct dib9000_config));
    memcpy(&state->dib0090_cfg, &default_dib0090_config, sizeof(struct dib0090_config));

    state->dib0090_cfg.freq_offset_khz_uhf = cfg->freq_offset_khz_uhf;
    state->dib0090_cfg.freq_offset_khz_vhf = cfg->freq_offset_khz_vhf;

    switch (cfg->clock_khz) {
        case 30000:
            memcpy(&state->dib0090_cfg.io, &dibx09x_io_30mhz_120, sizeof(dibx09x_io_30mhz_120));
            state->dib9000_cfg.timing_frequency = 20452225;
            state->dib9000_cfg.vcxo_timer       = 279620;
            break;
        case 26000:
            memcpy(&state->dib0090_cfg.io, &dibx09x_io_26mhz_120, sizeof(dibx09x_io_26mhz_120));
            state->dib9000_cfg.timing_frequency = 20976641;
            state->dib9000_cfg.vcxo_timer       = 286790;
            break;
        case 12000:
            memcpy(&state->dib0090_cfg.io, &dibx09x_io_12mhz_120, sizeof(dibx09x_io_12mhz_120));
            state->dib9000_cfg.timing_frequency = 20452225;
            state->dib9000_cfg.vcxo_timer       = 279620;
            break;
        default:
            dbgpl(&dib9090_dbg, "unsupported input clock");
            goto demod_error;
    }
    state->dib0090_cfg.clkouttobamse = cfg->clkouttobamse;

	if ( cfg->dib0090_wbd_table != NULL)
		state->dib0090_cfg.wbd = cfg->dib0090_wbd_table;

    state->dib9000_cfg.demod_clock_khz = cfg->clock_khz * state->dib0090_cfg.io.pll_loopdiv / state->dib0090_cfg.io.pll_prediv / 4;
    state->dib9000_cfg.xtal_clock_khz  = cfg->clock_khz;
    state->dib9000_cfg.adc_clock_ratio = state->dib0090_cfg.io.adc_clock_ratio;
    state->dib9000_cfg.if_drives       = cfg->if_drives;
    state->dib9000_cfg.output_mpeg2_in_188_bytes = cfg->output_mpeg2_in_188_bytes;
    state->dib9000_cfg.subband         = cfg->subband;
    state->dib0090_cfg.ls_cfg_pad_drv  = cfg->ls_cfg_pad_drv;
    state->dib0090_cfg.data_tx_drv     = cfg->data_tx_drv;

    /* add the internal GPIOs for sleep/reset */
    for (i = 0; i < ARRAY_SIZE(state->dib9000_cfg.gpio_function); i++) {
        struct dibGPIOFunction *f = &state->dib9000_cfg.gpio_function[i];
        *f = cfg->gpio_function[i];
        if (f->mask)
            switch (f->function) {
                case BOARD_GPIO_FUNCTION_COMPONENT_ON:  f->mask |= 0x21; f->direction &= ~0x21; f->value &= ~0x1; f->value |= 0x20; break;
                case BOARD_GPIO_FUNCTION_COMPONENT_OFF: f->mask |= 0x21; f->direction &= ~0x21; f->value |= 0x21; break;
            }
    }

    return state;

demod_error:
    MemFree(state, sizeof(struct dib9090_state));
    return NULL;

}
