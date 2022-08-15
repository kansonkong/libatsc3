#include "dibfrontend.h"
#include "dib0090.h"
#include "dib0090_priv.h"

//#define EFUSE
#define LPF	100

DEBUG_OBJECT(dib0090_dbg, DEBUG_TUNER, "DiB0090")

static int dib0090_release(struct dibFrontend *fe) /* MERGE done */
{
    struct dib0090_state *st = (struct dib0090_state *)(fe->tuner_priv);
    MemFree(st,sizeof(struct dib0090_state));
    return 0;
}

/* wakeup from sleep */
static int dib0090_wakeup(struct dibFrontend *fe)
{
    struct dib0090_state *state = (struct dib0090_state *)(fe->tuner_priv);
    if (state->config->sleep)
        state->config->sleep(fe, 0);
    //else {
    //    dib0090_write_reg(state, TUNER_EN, 0x22ed);
    //}

	return DIB_RETURN_SUCCESS;
}

int dib0090_sleep(struct dibFrontend *fe)
{
    struct dib0090_state *state = (struct dib0090_state *)(fe->tuner_priv);
    if (state->config->sleep)
        state->config->sleep(fe, 1);
    else {
        //for now, do not sleep the tuner because of side effects on octopus
        //dib0090_write_reg(state, TUNER_EN, 0x1);
    }
	return DIB_RETURN_SUCCESS;
}

uint32_t dib0090_get_slow_adc_val(struct dib0090_state *state)
{
    uint16_t adc_val = dib0090_read_reg(state, ADCVAL);
    if (state->identity.in_soc) {
#ifdef CONFIG_TUNER_DIB0090_DEBUG
        //dbgpl(&dib0090_dbg, " *** The 10bits Sad has been shifted");
#endif
        adc_val >>= 2;
    }
#ifdef CONFIG_TUNER_DIB0090_DEBUG
    //  dbgpl(&dib0090_dbg, " Slow ADC val = %d,%d", adc_val & 0xfff,(adc_val & 0xfff) * 1.8 / 4096);
    //  dbgpl(&dib0090_dbg, " Slow ADC val = %d (%d)", adc_val>>12, adc_val);
#endif
    return adc_val;
}

int dib0090_get_digital_clk_out(struct dibFrontend *fe)
{
    struct dib0090_state *state = (struct dib0090_state *)(fe->tuner_priv);
    return (state->config->io.clock_khz * state->config->io.pll_loopdiv) / state->config->io.pll_prediv;
}

/* Changes the baseband filter settings */
static void dib0090_set_bandwidth(struct dib0090_state *state, struct dibChannel *ch)
{
    uint16_t tmp;

    if (ch->bandwidth_kHz <= 5000)
        tmp = (3 << 14);
    else if (ch->bandwidth_kHz <= 6000)
        tmp = (2 << 14);
    else if (ch->bandwidth_kHz <= 7000)
        tmp = (1 << 14);
    else
        tmp = (0 << 14);

    state->bb_1_def &= 0x3fff;
    state->bb_1_def |= tmp;
    dib0090_write_reg(state, CTRL_BB_1, state->bb_1_def); /* be sure that we have the right bb-filter */

#ifdef CONFIG_TUNER_DIB0090_DEBUG
        tmp = dib0090_read_reg(state, CTRL_BB_2);
        dbgpl(&dib0090_dbg, "CTRL_BB_2 = 0x%04x", tmp);
        dbgpl(&dib0090_dbg, "CTRL_BB_2_cap_value            = %d (<<  0)", (tmp >>  0) &0x3f);
        dbgpl(&dib0090_dbg, "CTRL_BB_2_res_hr               = %d (<<  6)", (tmp >>  6) &0x1f);
        dbgpl(&dib0090_dbg, "CTRL_BB_2_res_np               = %d (<< 11)", (tmp >> 11) &0xf);
        dbgpl(&dib0090_dbg, "CTRL_BB_2_nc                   = %d (<< 15)", (tmp >> 15) &0x1);

        tmp = dib0090_read_reg(state, CTRL_BB_3);
        dbgpl(&dib0090_dbg, "*** CTRL_BB_3 = 0x%04x", tmp);
        dbgpl(&dib0090_dbg, "CTRL_BB_3_filter2_cutoff_freq  = %d (<<  0)", (tmp >> 0)  &0x1f);
        dbgpl(&dib0090_dbg, "CTRL_BB_3_filt2missmatch_trim  = %d (<<  5)", (tmp >> 5)  &0x1f);
        dbgpl(&dib0090_dbg, "CTRL_BB_3_filter2_gm1_trim     = %d (<< 10)", (tmp >> 10) &0xf);
        dbgpl(&dib0090_dbg, "CTRL_BB_3_vcm3_trim            = %d (<< 14)", (tmp >> 14) &0x3);

        tmp = dib0090_read_reg(state, CTRL_BB_4);
        dbgpl(&dib0090_dbg, "*** CTRL_BB_4 = 0x%04x", tmp);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_servo_cutoff         = %d (<<  0)", (tmp >>  0)& 0x3);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_servo_fast           = %d (<<  2)", (tmp >>  2)& 0x1);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_servo_ibias_trim     = %d (<<  3)", (tmp >>  3)& 0x7);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_servo_dc_offset_curr = %d (<<  6)", (tmp >>  6)& 0xf);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_en_DAB               = %d (<< 10)", (tmp >> 10)& 0x1);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_servo_hold           = %d (<< 11)", (tmp >> 11)& 0x1);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_servo_off            = %d (<< 12)", (tmp >> 12)& 0x1);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_adc_test_buffer      = %d (<< 13)", (tmp >> 13)& 0x1);
        dbgpl(&dib0090_dbg, "CTRL_BB_4_vcm_out1             = %d (<< 14)", (tmp >> 14)& 0x3);

        tmp = dib0090_read_reg(state, CTRL_BB_5);
        dbgpl(&dib0090_dbg, "*** CTRL_BB_5 = 0x%04x", tmp);
        dbgpl(&dib0090_dbg, "CTRL_BB_5_filter13_gm2_ibias_t = %d (<<  0)", (tmp >>  0) &0xf);
        dbgpl(&dib0090_dbg, "CTRL_BB_5_obuf_ibias_trim      = %d (<<  4)", (tmp >>  4) &0x7);
        dbgpl(&dib0090_dbg, "CTRL_BB_5_ext_gm_trim          = %d (<<  7)", (tmp >>  7) &0x1);
        dbgpl(&dib0090_dbg, "CTRL_BB_5_nc                   = %d (<<  8)", (tmp >>  8) &0xf);
        dbgpl(&dib0090_dbg, "CTRL_BB_5_input_stage_ibias_tr = %d (<< 12)", (tmp >> 12) &0x3);
        dbgpl(&dib0090_dbg, "CTRL_BB_5_attenuator_ibias_tri = %d (<< 14)", (tmp >> 14) &0x3);
#endif

#if defined CONFIG_STANDARD_DAB || defined CONFIG_STANDARD_ISDBT
    if ((
#if defined CONFIG_STANDARD_ISDBT
        (ch->type == STANDARD_ISDBT && ch->u.isdbt.sb_mode == 1) ||
#else
        0 ||
#endif
#if defined CONFIG_STANDARD_DAB
        (ch->type == STANDARD_DAB)
#else
        0
#endif
        ) && state->identity.p1g) { //if 1 seg or dab and P1G

#ifdef CONFIG_TUNER_DIB0090_DEBUG
        dbgpl(&dib0090_dbg, "*** Narrow Bandwidth requested, what a luck we have a P1G : Bandwidth =%d ***",ch->bandwidth_kHz);
#endif
        dib0090_write_reg(state, CTRL_BB_2, (15 << 11) | (31 << 6) | 63); /* cap value = 63 : narrow BB filter */
        dib0090_write_reg(state, CTRL_BB_4, ( 1 << 10) | (15 << 6) |  3); /* DAB En,DCoffset set to 8 */

        tmp = dib0090_read_reg(state, CTRL_BB_3) & (~(0xf<<10)); //Clear the bit
        dib0090_write_reg(state, CTRL_BB_3, tmp);

        tmp = dib0090_read_reg(state, CTRL_BB_5) | (7<<9) | 0xf;
        dib0090_write_reg(state, CTRL_BB_5, tmp);
    } else {
        dbgpl(&dib0090_dbg, "*** Set default bandwidth setting ***");

#ifndef FIRMWARE_FIREFLY
    tmp = (state->config->cap_value != 0)? state->config->cap_value : 22;
#else
    tmp = 22; /* cap_value = 22 */
#endif
        dib0090_write_reg(state, CTRL_BB_2, (5 << 11) | (8 << 6) | (tmp & 0x3f)); /* tmp = cap_value */

        dib0090_write_reg(state, CTRL_BB_3, 0x6008); /* = 0x6008 : vcm3_trim = 1 ; filter2_gm1_trim = 8 ; filter2_cutoff_freq = 0 */
        dib0090_write_reg(state, CTRL_BB_4, 0x1);    /* 0 = 1KHz ; 1 = 50Hz ; 2 = 150Hz ; 3 = 50KHz ; 4 = servo fast*/
        dib0090_write_reg(state, CTRL_BB_5, 0xabcd); /* = 0xabcd : attenuator_ibias_tri = 2 ; input_stage_ibias_tr = 2 ; nc = 11 ; ext_gm_trim = 1 ; obuf_ibias_trim = 4 ; filter13_gm2_ibias_t = 13 */
    }
#endif

}

static const struct dib0090_wbd_slope dib0090_wbd_table_default[] = {
//  { 0,      0,   0, 130, 130 },
    { 470,    0, 250, 0,   100 ,4},
    { 860,   51, 866, 21,  375 ,4},
    {1700,    0, 800, 0,   850, 4}, //LBAND Predefinition , to calibrate
    {2600,    0, 250, 0,   100, 6}, //SBAND Predefinition , NOT tested Yet
    { 0xFFFF, 0,   0, 0,   0   ,0},
};

/********************************************* dib0090 P1G SOC 7090 tuning tables **************************************/
#ifdef CONFIG_TUNER_DIB0090_P1G_SOC7090_SUPPORT
/* SOC 7090 is P1G type */
static const struct dib0090_tuning dib0090_tuning_table_cband_7090[] =
{
    //max_freq, switch_trim, lna_tune, lna_bias, v2i, mix, load, tuner_enable;
#ifdef CONFIG_BAND_CBAND
    { 184000,  4,  3, 0x4187, 0x2c0, 0x2d22, 0xb9ce, EN_CAB }, // 0x81ce 0x8190 Good perf but higher current //0x4187 Low current
    { 227000,  4,  3, 0x4187, 0x2c0, 0x2d22, 0xb9ce, EN_CAB },
    { 380000,  4,  3, 0x4187, 0x2c0, 0x2d22, 0xb9ce, EN_CAB },
    { 570000,  4, 10, 0x8190, 0x2c0, 0x2d22, 0xb9ce, EN_CAB },
    { 858000,  4,  5, 0x8190, 0x2c0, 0x2d22, 0xb9ce, EN_CAB },
#endif
};
#endif //CONFIG_TUNER_DIB0090_P1G_SOC7090_SUPPORT

/********************************************* dib0090 P1G tuning tables **************************************/
#ifdef CONFIG_TUNER_DIB0090_P1G_SUPPORT
static const struct dib0090_pll dib0090_p1g_pll_table[] =
{
#ifdef CONFIG_BAND_CBAND
      { 57000, 	0, 11, 48, 6},  // CAB
      { 70000, 	1, 11, 48, 6},  // CAB
      { 86000, 	0, 10, 32, 4},  // CAB
      { 105000,	1, 10, 32, 4}, // FM
      { 115000,	0, 9, 24, 6}, // FM
      { 140000,	1, 9, 24, 6}, // MID FM VHF
      { 170000,	0, 8, 16, 4}, // MID FM VHF
#endif
#ifdef CONFIG_BAND_VHF
      { 200000, 1, 8, 16, 4}, // VHF
      { 230000, 0, 7, 12, 6}, // VHF
      { 280000, 1, 7, 12, 6}, // MID VHF UHF
      { 340000, 0, 6, 8, 4},  // MID VHF UHF
      { 380000, 1, 6, 8, 4},  // MID VHF UHF
      { 455000, 0, 5, 6, 6},  // MID VHF UHF
#endif
#ifdef CONFIG_BAND_UHF
      { 580000, 1, 5,  6, 6}, // UHF
      { 680000, 0, 4,  4, 4}, // UHF
      { 860000, 1, 4,  4, 4}, // UHF
#endif
#ifdef CONFIG_BAND_LBAND
      { 1800000, 1, 2,  2, 4}, // LBD
#endif
#ifdef CONFIG_BAND_SBAND
      { 2900000, 0, 1, 1, 6}, // SBD
#endif
};

static const struct dib0090_tuning dib0090_p1g_tuning_table_fm_vhf_on_cband[] =
{
    //max_freq, switch_trim, lna_tune, lna_bias, v2i, mix, load, tuner_enable;
#ifdef CONFIG_BAND_CBAND
    { 184000,  4, 3, 0x4187, 0x2c0, 0x2d22, 0x81cb, EN_CAB }, // FM EN_CAB	// 0x8190 Good perf but higher current //0x4187 Low current
    { 227000,  4, 3, 0x4187, 0x2c0, 0x2d22, 0x81cb, EN_CAB }, // FM EN_CAB
    { 380000,  4, 3, 0x4187, 0x2c0, 0x2d22, 0x81cb, EN_CAB }, // FM EN_CAB
#endif
#ifdef CONFIG_BAND_UHF
    { 520000,  2, 0, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 550000,  2, 2, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 650000,  2, 3, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 750000,  2, 5, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 850000,  2, 6, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 900000,  2, 7, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
#endif
#ifdef CONFIG_BAND_LBAND
    { 1500000, 4, 0, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
    { 1600000, 4, 1, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
    { 1800000, 4, 3, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
#endif
#ifdef CONFIG_BAND_SBAND
    { 2300000, 1, 4, 20, 0x300, 0x2d2A, 0x82c7, EN_SBD }, // SBD EN_SBD
    { 2900000, 1, 7, 20, 0x280, 0x2deb, 0x8347, EN_SBD }, // SBD EN_SBD
#endif
};

static const struct dib0090_tuning dib0090_p1g_tuning_table[] =
{
    //max_freq, switch_trim, lna_tune, lna_bias, v2i, mix, load, tuner_enable;
#ifdef CONFIG_BAND_CBAND
    { 170000,  4, 1, 0x820f, 0x300, 0x2d22, 0x82cb, EN_CAB }, // FM EN_CAB
#endif
#ifdef CONFIG_BAND_VHF
    { 184000,  1, 1, 15, 0x300, 0x4d12, 0xb94e, EN_VHF }, // VHF EN_VHF
    { 227000,  1, 3, 15, 0x300, 0x4d12, 0xb94e, EN_VHF }, // VHF EN_VHF
    { 380000,  1, 7, 15, 0x300, 0x4d12, 0xb94e, EN_VHF }, // VHF EN_VHF
#endif
#ifdef CONFIG_BAND_UHF
    { 510000,  2, 0, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 540000,  2, 1, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 600000,  2, 3, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 630000,  2, 4, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 680000,  2, 5, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 720000,  2, 6, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 900000,  2, 7, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
#endif
#ifdef CONFIG_BAND_LBAND
    { 1500000, 4, 0, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
    { 1600000, 4, 1, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
    { 1800000, 4, 3, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
#endif
#ifdef CONFIG_BAND_SBAND
    { 2300000, 1, 4, 20, 0x300, 0x2d2A, 0x82c7, EN_SBD }, // SBD EN_SBD
    { 2900000, 1, 7, 20, 0x280, 0x2deb, 0x8347, EN_SBD }, // SBD EN_SBD
#endif
};
#endif //CONFIG_TUNER_DIB0090_P1G_SUPPORT

/********************************************* dib0090 P1C_F tuning tables **************************************/
#ifdef CONFIG_TUNER_DIB0090_P1C_F_SUPPORT
static const struct dib0090_pll dib0090_pll_table[] =
{
#ifdef CONFIG_BAND_CBAND
    { 56000, 0, 9, 48, 6},  // CAB
    { 70000, 1, 9, 48, 6},  // CAB
    { 87000, 0, 8, 32, 4},  // CAB
    { 105000, 1, 8, 32, 4}, // FM
    { 115000, 0, 7, 24, 6}, // FM
    { 140000, 1, 7, 24, 6}, // MID FM VHF
    { 170000, 0, 6, 16, 4}, // MID FM VHF
#endif
#ifdef CONFIG_BAND_VHF
    { 200000, 1, 6, 16, 4}, // VHF
    { 230000, 0, 5, 12, 6}, // VHF
    { 280000, 1, 5, 12, 6}, // MID VHF UHF
    { 340000, 0, 4, 8, 4},  // MID VHF UHF
    { 380000, 1, 4, 8, 4},  // MID VHF UHF
    { 450000, 0, 3, 6, 6},  // MID VHF UHF
#endif
#ifdef CONFIG_BAND_UHF
    { 580000, 1, 3,  6, 6}, // UHF
    { 700000, 0, 2,  4, 4}, // UHF
    { 860000, 1, 2,  4, 4}, // UHF
#endif
#ifdef CONFIG_BAND_LBAND
    { 1800000, 1, 0,  2, 4}, // LBD
#endif
#ifdef CONFIG_BAND_SBAND
    { 2900000, 0, 14, 1, 4}, // SBD
#endif
};

static const struct dib0090_tuning dib0090_tuning_table_fm_vhf_on_cband[] =
{
    //max_freq, switch_trim, lna_tune, lna_bias, v2i, mix, load, tuner_enable;
#ifdef CONFIG_BAND_CBAND
    { 184000,  4, 1, 15, 0x280, 0x2912, 0xb94e, EN_CAB }, // FM EN_CAB
    { 227000,  4, 3, 15, 0x280, 0x2912, 0xb94e, EN_CAB }, // FM EN_CAB
    { 380000,  4, 7, 15, 0x280, 0x2912, 0xb94e, EN_CAB }, // FM EN_CAB
#endif
#ifdef CONFIG_BAND_UHF
    { 520000,  2, 0, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 550000,  2, 2, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 650000,  2, 3, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 750000,  2, 5, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 850000,  2, 6, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 900000,  2, 7, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
#endif
#ifdef CONFIG_BAND_LBAND
    { 1500000, 4, 0, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
    { 1600000, 4, 1, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
    { 1800000, 4, 3, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
#endif
#ifdef CONFIG_BAND_SBAND
    { 2300000, 1, 4, 20, 0x300, 0x2d2A, 0x82c7, EN_SBD }, // SBD EN_SBD
    { 2900000, 1, 7, 20, 0x280, 0x2deb, 0x8347, EN_SBD }, // SBD EN_SBD
#endif
};

static const struct dib0090_tuning dib0090_tuning_table[] =
{
    //max_freq, switch_trim, lna_tune, lna_bias, v2i, mix, load, tuner_enable;
#ifdef CONFIG_BAND_CBAND
    { 170000,  4, 1, 15, 0x280, 0x2912, 0xb94e, EN_CAB }, // FM EN_CAB
#endif
#ifdef CONFIG_BAND_VHF
    { 184000,  1, 1, 15, 0x300, 0x4d12, 0xb94e, EN_VHF }, // VHF EN_VHF
    { 227000,  1, 3, 15, 0x300, 0x4d12, 0xb94e, EN_VHF }, // VHF EN_VHF
    { 380000,  1, 7, 15, 0x300, 0x4d12, 0xb94e, EN_VHF }, // VHF EN_VHF
#endif
#ifdef CONFIG_BAND_UHF
    { 520000,  2, 0, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 550000,  2, 2, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 650000,  2, 3, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 750000,  2, 5, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 850000,  2, 6, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
    { 900000,  2, 7, 15, 0x300, 0x1d12, 0xb9ce, EN_UHF }, // UHF
#endif
#ifdef CONFIG_BAND_LBAND
    { 1500000, 4, 0, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
    { 1600000, 4, 1, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
    { 1800000, 4, 3, 20, 0x300, 0x1912, 0x82c9, EN_LBD }, // LBD EN_LBD
#endif
#ifdef CONFIG_BAND_SBAND
    { 2300000, 1, 4, 20, 0x300, 0x2d2A, 0x82c7, EN_SBD }, // SBD EN_SBD
    { 2900000, 1, 7, 20, 0x280, 0x2deb, 0x8347, EN_SBD }, // SBD EN_SBD
#endif
};
#endif //CONFIG_TUNER_DIB0090_P1C_F_SUPPORT

static int dib0090_tune(struct dibFrontend *fe, struct dibChannel *ch)
{
    struct dib0090_state *state = (struct dib0090_state *)(fe->tuner_priv);
    const struct dib0090_tuning *tune = state->current_tune_table_index;
    const struct dib0090_pll *pll = state->current_pll_table_index;
    enum frontend_tune_state *tune_state = &fe->tune_state;

    uint16_t lo5, lo6, Den;
    uint32_t FBDiv, Rest, FREF, VCOF_kHz = 0;
    uint16_t tmp;
    int ret = 10; /* 1ms is the default delay most of the time */
    uint8_t c, i;

    /************************* VCO ***************************/
    /* Default values for FG                                 */
    /* from these are needed :                               */
    /* Cp,HFdiv,VCOband,SD,Num,Den,FB and REFDiv             */

    //dbgpl(&dib0090_dbg, "fe%hd: Tuning for Band: %d (%d kHz); tune_state = %d",fe->id, state->current_band, rf, fe->tune_state);
    //dbgpl(&dib0090_dbg, "FE %d TUNER STEP %d callback time = %d",fe->id, fe->tune_state,fe->tuner_info->callback_time);

    /* in any case we first need to do a calibration if needed */
    //Since

    if (state->calibrate & DC_CAL)
        return dib0090_dc_offset_calibration(state, tune_state);
    else if (state->calibrate & WBD_CAL) {
        if (state->current_rf == 0) {
#ifdef CONFIG_TUNER_DIB0090_DEBUG
            //dbgpl(&dib0090_dbg,"****** current_rf = 0, Be carefull Wideband Calibration will be wrong****");
#endif
            state->current_rf = ch-> RF_kHz;
        }
        return dib0090_wbd_calibration(state, tune_state);
    }
    else if (state->calibrate & TEMP_CAL)
        return dib0090_get_temperature(state, tune_state);
    else if (state->calibrate & CAPTRIM_CAL)
        return dib0090_captrim_search(state, tune_state);

    if (*tune_state == CT_TUNER_START) {
        state->current_band = (uint8_t)BAND_OF_FREQUENCY(ch->RF_kHz);
        state->rf_request = ch->RF_kHz + (state->current_band == BAND_UHF ? state->config->freq_offset_khz_uhf : state->config->freq_offset_khz_vhf);

#ifdef CONFIG_STANDARD_ISDBT
        /* in ISDB-T 1seg we shift tuning frequency */
        if ((ch->type == STANDARD_ISDBT && ch->u.isdbt.sb_mode == 1 && ch->u.isdbt.partial_reception == 0)) {
            const struct dib0090_low_if_offset_table *LUT_offset = state->config->low_if;
            uint8_t found_offset = 0;
            uint32_t margin_khz = 100;

            if(LUT_offset != NULL) {
                while(LUT_offset->RF_freq != 0xffff) {
#ifdef CONFIG_TUNER_DIB0090_DEBUG
                    dbgpl(NULL,"rf = %d fe->current_rf = %d offset->RF_freq = %d offset->offset_khz = %d", state->rf_request, fe->current_rf, LUT_offset->RF_freq, LUT_offset->offset_khz);
#endif
                    if(((state->rf_request > (LUT_offset->RF_freq - margin_khz)) && (state->rf_request < (LUT_offset->RF_freq + margin_khz))) && LUT_offset->std == ch->type) {
                        state->rf_request += LUT_offset->offset_khz;
                        found_offset = 1;
                        break;
                    }
                    LUT_offset++;
                }
            }

            if(found_offset == 0)
                state->rf_request +=400;
        }
#endif
        if (fe->current_rf != state->rf_request  || (state->current_standard != ch->type)) {
            state->tuner_is_tuned = 0;
            fe->current_rf = 0;
            state->current_standard = 0;

#ifdef CONFIG_TUNER_DIB0090_P1C_F_SUPPORT
            tune = dib0090_tuning_table;
#endif
#ifdef CONFIG_TUNER_DIB0090_P1G_SUPPORT
            if(state->identity.p1g)
                tune = dib0090_p1g_tuning_table;
#endif

            tmp = (state->identity.version >> 5)&0x7;

#ifdef CONFIG_TUNER_DIB0090_P1G_SOC7090_SUPPORT
            if (state->identity.version == SOC_7090_P1G_11R1 || state->identity.version == SOC_7090_P1G_21R1) {
                if(state->config->force_cband_input) { /* Use the CBAND input for all band */
                    if(state->current_band & BAND_CBAND || state->current_band & BAND_FM || state->current_band & BAND_VHF || state->current_band & BAND_UHF) {
                        state->current_band = BAND_CBAND;
#ifdef CONFIG_TUNER_DIB0090_DEBUG
                        dbgpl(&dib0090_dbg, "*** 7090 : Force FM, VHF & UHF in CBD input ***");
#endif
                        tune = dib0090_tuning_table_cband_7090;
                    }
                } else { /* Use the CBAND input for all band under UHF */
                    if(state->current_band & BAND_CBAND || state->current_band & BAND_FM || state->current_band & BAND_VHF) {
                        state->current_band = BAND_CBAND;
#ifdef CONFIG_TUNER_DIB0090_DEBUG
                        dbgpl(&dib0090_dbg, "*** 7090 : Force FM, VHF in CBD input ***");
#endif
                        tune = dib0090_tuning_table_cband_7090;
                    }
                }
            } else
#endif

#if defined (CONFIG_TUNER_DIB0090_P1C_F_SUPPORT) || defined (CONFIG_TUNER_DIB0090_P1G_SUPPORT)
                if(tmp == 0x4 || tmp == 0x7) {
                    /* CBAND tuner version for VHF */
                    if(state->current_band == BAND_FM || state->current_band == BAND_VHF) {
                        state->current_band = BAND_CBAND; /* Force CBAND */
#ifdef CONFIG_TUNER_DIB0090_DEBUG
                        dbgpl(&dib0090_dbg, "*** Force Vhf and Cb on CBD input***");
#endif


#ifdef CONFIG_TUNER_DIB0090_P1C_F_SUPPORT
                    tune = dib0090_tuning_table_fm_vhf_on_cband;
#endif
#ifdef CONFIG_TUNER_DIB0090_P1G_SUPPORT
                    if(state->identity.p1g)
                        tune = dib0090_p1g_tuning_table_fm_vhf_on_cband;
#endif
                }
            }
#endif

#ifdef CONFIG_TUNER_DIB0090_P1C_F_SUPPORT
            pll = dib0090_pll_table;
#endif
#ifdef CONFIG_TUNER_DIB0090_P1G_SUPPORT
            if(state->identity.p1g)
                pll = dib0090_p1g_pll_table;
#endif

            /* Look for the interval */
            while (state->rf_request > tune->max_freq)
                tune++;
            while (state->rf_request > pll->max_freq)
                pll++;

            state->current_tune_table_index = tune;
            state->current_pll_table_index = pll;

            // select internal switch
            dib0090_write_reg(state, CTRL_RF_SW, 0xb800 | (tune->switch_trim));

            // Find the VCO frequency in MHz
            VCOF_kHz = (pll->hfdiv * state->rf_request) * 2;

            FREF = state->config->io.clock_khz; // REFDIV is 1FREF Has to be as Close as possible to 10MHz
#ifndef FIRMWARE_FIREFLY
            if (state->config->fref_clock_ratio != 0)
                FREF /= state->config->fref_clock_ratio;
#endif

            // Determine the FB divider
            // The reference is 10MHz, Therefore the FBdivider is on the first digits
            FBDiv = (VCOF_kHz / pll->topresc / FREF);
            Rest  = (VCOF_kHz / pll->topresc) - FBDiv * FREF; //in kHz

            // Avoid Spurs in the loopfilter bandwidth
            if (Rest < LPF)                   Rest = 0;
            else if (Rest < 2 * LPF)          Rest = 2 * LPF;
            else if (Rest > (FREF - LPF))   { Rest = 0 ; FBDiv += 1; } //Go to the next FB
            else if (Rest > (FREF - 2 * LPF)) Rest = FREF - 2 * LPF;
            Rest = (Rest * 6528) / (FREF / 10);
            state->rest = Rest;

            /* external loop filter, otherwise:
             * lo5 = (0 << 15) | (0 << 12) | (0 << 11) | (3 << 9) | (4 << 6) | (3 << 4) | 4;
             * lo6 = 0x0e34 */

#ifdef CONFIG_TUNER_DIB0090_P1C_F_SUPPORT
             if (Rest == 0) {
                 if (pll->vco_band)
                     lo5 = 0x049f;
                 //else if (state->config->analog_output)
                 //    lo5 = 0x041f;
                 else
                     lo5 = 0x041f;
             } else {
                 if (pll->vco_band)
                     lo5 = 0x049e;
                 else if (state->config->analog_output)
                     lo5 = 0x041d;
                 else
                     lo5 = 0x041c;
             }
#endif

#ifdef CONFIG_TUNER_DIB0090_P1G_SUPPORT
           if(state->identity.p1g) { /* Bias is done automatically in P1G */
#ifdef CONFIG_TUNER_DIB0090_P1G_SOC7090_SUPPORT
                if(state->identity.version == SOC_7090_P1G_11R1 || state->identity.version == SOC_7090_P1G_21R1)
                   lo5 = 0x42f;
                else
#endif
                   lo5 = 0x42c; //BIAS Lo set to 4 by default in case of the Captrim search does not take care of the VCO Bias
           }
#endif

            lo5 |= (pll->hfdiv_code << 11) | (pll->vco_band << 7); /* bit 15 is the split to the slave, we do not do it here */
#ifdef CONFIG_TUNER_DIB0090_DEBUG
            dbgpl(&dib0090_dbg, "***lo5 is loaded = 0x%04x *******", lo5);
#endif

            //Internal loop filter set...
            if(!state->config->io.pll_int_loop_filt) {
                if(state->identity.p1g || (Rest == 0))
                    lo6 = 0xfff8;
                else
                    lo6 = 0xff28;
            } else
                lo6 = (state->config->io.pll_int_loop_filt << 3); // take the loop filter value given by the layout
            //dbgpl(&dib0090_dbg, "FE %d lo6 = 0x%04x", (uint32_t)fe->id, (uint32_t)lo6);

            Den = 1;
#ifdef CONFIG_TUNER_DIB0090_DEBUG
            dbgpl(&dib0090_dbg, "***Rest value = %d", Rest );
#endif

            if (Rest > 0) {
                if (state->config->analog_output)
                    lo6 |= (1 << 2) | 2; //SigmaDelta and Dither
                else
                    lo6 |= (1 << 2) | 1; //SigmaDelta and Dither
                Den = 255;
            }

#ifdef CONFIG_BAND_SBAND
            if (state->current_band == BAND_SBAND)
                lo6 &= 0xfffb; //Remove the Dither for SBAND
#endif

            // Now we have to define the Num and Denum
            // LO1 gets the FBdiv
            dib0090_write_reg(state, CTRL_LO_1, (uint16_t) FBDiv);
            // LO2 gets the REFDiv
#ifndef FIRMWARE_FIREFLY
            if (state->config->fref_clock_ratio != 0)
                dib0090_write_reg(state, CTRL_LO_2, (Den << 8) | state->config->fref_clock_ratio);
            else
#endif
            dib0090_write_reg(state, CTRL_LO_2, (Den << 8) | 1);
            // LO3 for the Numerator
            dib0090_write_reg(state, CTRL_LO_3, (uint16_t)Rest);
            // VCO and HF DIV
            dib0090_write_reg(state, CTRL_LO_5, lo5);
            // SIGMA Delta
            dib0090_write_reg(state, CTRL_LO_6, lo6);


            // Check if the 0090 is analogged configured
            //Disable ADC and DigPLL =0xFF9F, 0xffbf for test purposes.
            //Enable The Outputs of the BB on DATA_Tx
            lo6 = tune->tuner_enable;
            if (state->config->analog_output)
                lo6 = (lo6 & 0xff9f) | 0x2;

            dib0090_write_reg(state, TUNER_EN, lo6 | EN_LO
#ifdef CONFIG_DIB0090_USE_PWM_AGC
                    | state->config->use_pwm_agc * EN_CRYSTAL
#endif
                    );

        }

        fe->current_rf = state->rf_request;
        state->current_standard = ch->type;

        ret = 20;
        state->calibrate = CAPTRIM_CAL; /* captrim serach now */
    }

    else if (*tune_state == CT_TUNER_STEP_0) {   /* Warning : because of captrim cal, if you change this step, change it also in _cal.c file because it is the step following captrim cal state machine */
        const struct dib0090_wbd_slope *wbd = state->current_wbd_table;

//        if(!state->identity.p1g) {
              while (fe->current_rf/1000 > wbd->max_freq)
                  wbd++;
//        }

        dib0090_write_reg(state, ADCCLK, 0x07ff);
        dbgpl(&dib0090_dbg, "FE %d Final Captrim: %d",(uint32_t)fe->id, (uint32_t)state->fcaptrim);
        dbgpl(&dib0090_dbg, "FE %d HFDIV code: %d",(uint32_t)fe->id, (uint32_t)pll->hfdiv_code);
        dbgpl(&dib0090_dbg, "FE %d VCO = %d",(uint32_t)fe->id, (uint32_t)pll->vco_band);
        dbgpl(&dib0090_dbg, "FE %d VCOF in kHz: %d ((%d*%d) << 1))",(uint32_t)fe->id, (uint32_t)((pll->hfdiv * state->rf_request) * 2), (uint32_t)pll->hfdiv, (uint32_t)state->rf_request);
        dbgpl(&dib0090_dbg, "FE %d REFDIV: %d, FREF: %d", (uint32_t)fe->id, (uint32_t)1, (uint32_t)state->config->io.clock_khz);
        dbgpl(&dib0090_dbg, "FE %d FBDIV: %d, Rest: %d",(uint32_t)fe->id, (uint32_t)dib0090_read_reg(state, CTRL_LO_1), (uint32_t)dib0090_read_reg(state, CTRL_LO_3));
        dbgpl(&dib0090_dbg, "FE %d Num: %d, Den: %d, SD: %d",(uint32_t)fe->id, (uint32_t)dib0090_read_reg(state, CTRL_LO_3), (uint32_t)(dib0090_read_reg(state, CTRL_LO_2) >> 8), (uint32_t)dib0090_read_reg(state, CTRL_LO_6) & 0x3);

#define WBD     0x781 /* 1 1 1 1 0000 0 0 1 */
        c=4;
        i=3; //wbdmux_bias
#if defined(CONFIG_BAND_LBAND) || defined(CONFIG_BAND_SBAND)
        if ((state->current_band==BAND_LBAND) || (state->current_band==BAND_SBAND)) {
            c=2;
            i=2;
        }
#endif

	if (wbd->wbd_gain!=0) //&& !state->identity.p1g)
            c = wbd->wbd_gain;

        //Store wideband mux register.
        state->wbdmux=(c << 13) | (i << 11) | (WBD
#ifdef CONFIG_DIB0090_USE_PWM_AGC
                |(state->config->use_pwm_agc<<1)
#endif
                );
#ifdef CONFIG_TUNER_DIB0090_DEBUG
        dbgpl(&dib0090_dbg, "in WbdMux:%x with Gain %d",state->wbdmux,c);
#endif
        dib0090_write_reg(state, CTRL_WBDMUX, state->wbdmux);


#ifdef CONFIG_TUNER_DIB0090_P1G_SUPPORT
        if ((tune->tuner_enable == EN_CAB) && state->identity.p1g) {
            dbgpl(&dib0090_dbg, "P1G : The cable band is selected and lna_tune = %d",tune->lna_tune);
            dib0090_write_reg(state, CTRL_RXRF2, tune->lna_bias);
            dib0090_write_reg(state, CTRL_RF_SW, 0xb800 | (tune->lna_tune << 6) | (tune->switch_trim));
        } else
#endif
            dib0090_write_reg(state, CTRL_RXRF2, (tune->lna_tune << 5) | tune->lna_bias);

        dib0090_write_reg(state, CTRL_RF_V2I, tune->v2i);
        dib0090_write_reg(state, CTRL_RF_MIX, tune->mix);
        dib0090_write_reg(state, CTRL_RF_LOAD, tune->load);
        *tune_state = CT_TUNER_STEP_1;

    } else if (*tune_state == CT_TUNER_STEP_1) {
        /* initialize the lt gain register */
        state->rf_lt_def = 0x7c00;
        // dib0090_write_reg(state, CTRL_RF_LT, state->rf_lt_def);

        dib0090_set_bandwidth(state, ch);
	state->tuner_is_tuned = 1;

//       if(!state->identity.p1g)
	 state->calibrate |= WBD_CAL; // TODO: only do the WBD calibration for new tune
//
        state->calibrate |= TEMP_CAL; // Force the Temperature to be remesured at next TUNE.
        *tune_state = CT_TUNER_STOP;
    } else
        ret = FE_CALLBACK_TIME_NEVER;
    return ret;
}

#define pgm_read_word(w) (*w)
static const uint16_t dib0090_defaults[] =
{
    // RF INITIALISATION + BB
    25, CTRL_BB_1, // nb of values, start register
        0x0000, // 0x01  CTRL_BB_1
        0x99a0,   // 0x02  CTRL_BB_2 Captrim
        0x6008, // 0x03  CTRL_BB_3
        0x0000, // 0x04  CTRL_BB_4 DC servo loop 1KHz
        0x8bcb, // 0x05  CTRL_BB_5
        0x0000, // 0x06  CTRL_BB_6
        0x0405, // 0x07  CTRL_BB_7
        0x0000, // 0x08  CTRL_RF_1
        0x0000, // 0x09  CTRL_RF_2 Bias and Tune not set
        0x0000, // 0x0a	 CTRL_RF_3
        0xb802, // 0x0b	 CTRL_RF_SW
        0x0300, // 0x0c  CTRL_RF_V2I
        0x2d12, // 0x0d	 CTRL_RF_MIX
        0xbac0, // 0x0e  CTRL_LOAD
        0x7c00, // 0x0f	 CTRL_LT //CAB
        0xdbb9, // 0x10  CTRL_WBDMUX   // WBD MUX TOWARD BB
        0x0954, // 0x11  CTRL_TX
        0x0743,	// 0x12  IQ_ADC
        0x8000,	// 0x13	 CTRL_BIAS 0x4106 Internal bias
        0x0001, // 0x14  CTRL_CRYSTAL
        0x0040, // 0x15  CTRL_LO_1
        0x0100, // 0x16  CTRL_LO_2
        0x0000, // 0x17  CTRL_LO_3
        0xe910, // 0x18  CTRL_LO_4
        0x149e, // 0x19  CTRL_LO_5 1734

    1, CTRL_LO_6,
        0xff2d,	// 0x1a  CTRL_LO_6

    1, BB_RAMP7,
        0x0000,  // deactivate the ramp and ramp_mux to adjust the gain manually

    2, ADCCLK,
        0x07FF,	// ADC_CLK
        0x0007, // VGA_MODE

    1, TUNER_EN,
        EN_UHF | EN_CRYSTAL, // 0x33 TUNER_EN

    2, PWM1_REG,
        0x3ff, // pwm accu period
        0x111, // (alpha_bb<<8)|(resetn_pwm<<4)|(alpha_rf)

    0
};

#ifdef CONFIG_TUNER_DIB0090_P1G_SUPPORT
static const uint16_t dib0090_p1g_additionnal_defaults[] =
{
    // additionnal INITIALISATION for p1g to be written after dib0090_defaults
    1, CTRL_BB_5, // nb of values, start register
        0xabcd, // 0x05  CTRL_BB_5

    1, CTRL_TX, // nb of values, start register
	0x00b4, // 0x11  CTRL_LO_7 Now prepare for auto captrim

    1, CTRL_LO_6,
	0xfffd, // 0x19  CTRL_LO_6

    1, CAPTRIM_CFG,
	0x108,
    0
};
#endif

uint8_t dib0090_is_SH_capable(struct dibFrontend *slave)
{

    struct dib0090_state *state = (struct dib0090_state *)(slave->tuner_priv);

    if((dib0090_read_reg(state,EFUSE_1)&0xf) < 0xf) {
        dbgpl(&dib0090_dbg, "This device can not demodulate DVB-SH");
        return 0;
    }
    else
        return 1;
}

static int dib0090_set_default_config(struct dib0090_state *state, const uint16_t * n)
{
    uint16_t l, r;

    l = pgm_read_word(n++);
    while (l) {
        r = pgm_read_word(n++);
        do {
          /* DEBUG_TUNER */
	  //dbgpl(&dib0090_dbg,"%d, %d, %x", l, r, pgm_read_word(n));
            if((dib0090_write_reg(state, r, pgm_read_word(n++)))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
            r++;
        } while (--l);
	    l = pgm_read_word(n++);
    }
	return DIB_RETURN_SUCCESS;
}

static int dib0090_reset(struct dibFrontend *fe) /* MERGE done */
{
    struct dib0090_state *state = (struct dib0090_state *)(fe->tuner_priv);

    if((dib0090_reset_digital(fe, state->config))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* Revision definition */
    if (dib0090_identify(fe) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

#ifdef CONFIG_TUNER_DIB0090_P1B_SUPPORT
    if (!(state->identity.version & 0x1)) /* it is P1B - reset is already done */
        return DIB_RETURN_SUCCESS;
#endif

    /* Diversity/single sleep mode configuration :
         - do not turn off the BB/IQADC/BIAS of tuner in case of diversity die because of PVR mode issue
    */

    if(!state->identity.in_soc) {
        if((dib0090_read_reg(state, DIG_CFG_RO)>>5) & 0x2) {/* this is a diversity krosus die */
            dbgpl(&dib0090_dbg, "Diversity tuner die detected DIG_CFG_RO = 0x%04x", dib0090_read_reg(state, DIG_CFG_RO));
			if(dib0090_write_reg(state, SLEEP_EN, (EN_IQADC | EN_BB | EN_BIAS | EN_DIGCLK | EN_PLL | EN_CRYSTAL))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
        }
        else { /* Single die : optimize Sleep mode power consumption */
            dbgpl(&dib0090_dbg, "Single tuner die detected DIG_CFG_RO = 0x%04x", dib0090_read_reg(state, DIG_CFG_RO));
            if((dib0090_write_reg(state, SLEEP_EN, (EN_DIGCLK | EN_PLL | EN_CRYSTAL)))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
        }
    }

    /* Upload the default values */
    if((dib0090_set_default_config(state, dib0090_defaults))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

#ifdef CONFIG_TUNER_DIB0090_P1G_SUPPORT
    if(state->identity.p1g)
	{
        if((dib0090_set_default_config(state, dib0090_p1g_additionnal_defaults))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}
#endif

#ifdef EFUSE
    if (((state->identity.version & 0x1f) >= P1D_E_F) || (state->identity.in_soc)) /* Update the efuse : Only available for KROSUS > P1C  and SOC as well*/
      dib0090_set_EFUSE(state);

#endif

#ifndef FIRMWARE_FIREFLY
    if (state->config->cap_value != 0)
        if((dib0090_write_reg(state, CTRL_BB_2, (dib0090_read_reg(state, CTRL_BB_2)&0xffc0)|(state->config->cap_value&0x3f)))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
#endif

    /* Congigure in function of the crystal */
    if (state->config->io.clock_khz >= 24000)
	{
        if((dib0090_write_reg(state, CTRL_CRYSTAL, 1))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}
    else
	{
        if((dib0090_write_reg(state, CTRL_CRYSTAL, 2))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}

#ifdef CONFIG_TUNER_DIB0090_DEBUG
    dbgpl(&dib0090_dbg, "Pll lock : %d", (dib0090_read_reg(state, DIG_CFG_RO)>>11)&0x1);
#endif

    state->calibrate = DC_CAL | WBD_CAL | TEMP_CAL; /* enable iq-offset-calibration and wbd-calibration when tuning next time */
    return DIB_RETURN_SUCCESS;
}

#if !defined(FIRMWARE_FIREFLY)
static int dib0090_generic_monitoring(struct dibFrontend *fe, uint32_t cmd, uint8_t *b, uint32_t size, uint32_t offset)
{
    struct dib0090_state *state = (struct dib0090_state *)(fe->tuner_priv);
    int ret = DIB_RETURN_NOT_SUPPORTED;

    switch (cmd & ~GENERIC_MONIT_REQUEST_SIZE) {
    case GENERIC_MONIT_AGC_FREEZE:
        if(!state->config->use_pwm_agc) {
            ret = 0;
            state->agc_freeze = (uint8_t)offset;
        }
        break;
    case GENERIC_MONIT_FORCE_MAX_GAIN:
        state->force_max_gain = (uint8_t)offset;
        break;
    default:
        break;
    }

    return ret;
}
#endif

static const struct dibTunerInfo dib0090_info = {
    INFO_TEXT("DiBcom DiB0090 (Kroesus)"),   // name

    TUNER_CAN_VHF | TUNER_CAN_UHF | TUNER_CAN_LBAND | TUNER_CAN_SBAND | TUNER_CAN_FAST_TUNE, // caps

    { // ops
        dib0090_reset,   // reset
        dib0090_wakeup,  // wakeup
        dib0090_sleep,   // sleep

        dib0090_tune,  // tune_digital

        NULL,
#if !defined(FIRMWARE_FIREFLY)
        dib0090_generic_monitoring,
#else
        NULL,
#endif

        dib0090_release, // release
    }
};

struct dibFrontend * dib0090_register(struct dibFrontend *fe, struct dibDataBusHost *data, const struct dib0090_config *config)
{
	struct dib0090_state *st = (struct dib0090_state *)(MemAlloc(sizeof(struct dib0090_state)));
	struct dibDataBusClient *client;
	if (st == NULL)
		return NULL;
	DibZeroMemory(st,sizeof(struct dib0090_state));

#ifdef CONFIG_DIB0090_USE_PWM_AGC
        if(!config->use_pwm_agc) {
#endif

    if (fe->demod_info == NULL)
        dbgpl(&dib0090_dbg, "this driver should have a demodulator in the same frontend - not registering the GAIN control functions");
    else
        fe->demod_info->ops.agc_startup = dib0090_gain_control;

#ifdef CONFIG_DIB0090_USE_PWM_AGC
        }
#endif
    frontend_register_tuner(fe, &st->info, &dib0090_info, st);

	//client = tuner_get_data_bus_client(fe);
		if ((fe != NULL) && (fe->tuner_info != NULL))
        client = &fe->tuner_info->data_bus_client;
    else
        client = NULL;

    switch(data_bus_host_type(data)) {
#ifndef CONFIG_BUILD_LEON
        case DATA_BUS_I2C:
            data_bus_client_init(client, &data_bus_client_template_i2c_8b_16b, data);
            data_bus_client_set_speed(client, 340);
            data_bus_client_set_device_id(client, config->i2c_address);
            break;
#endif
        case DATA_BUS_DIBCTRL_IF:
            data_bus_client_init(client, &data_bus_client_template_8b_16b, data);
            break;
        default:
            dbgpl(&dib0090_dbg, "data_bus host type not handled");
    }

    st->config = config;
    st->fe = fe;
    st->tune = dib0090_tune;
    st->rf_request = 0;


    if (config->wbd == NULL) {
#ifdef CONFIG_TUNER_DIB0090_DEBUG
      dbgpl (&dib0090_dbg, "******* Default wideband table is used ******* ");
#endif
      st->current_wbd_table = dib0090_wbd_table_default;
    }
    else {
#ifdef CONFIG_TUNER_DIB0090_DEBUG
      dbgpl (&dib0090_dbg, "******* Layout wideband table is used ******* ");
#endif
      st->current_wbd_table = config->wbd;
    }
    return fe;
}

