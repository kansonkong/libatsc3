#include "dib7000m_priv.h"

int dib7000m_set_power_mode(struct dib7000m_state *state, enum dib7000m_power_mode mode)
{
	/* by default everything is going to be powered off */
	uint16_t reg_903 = 0x3fff, reg_904 = 0xffff, reg_905 = 0xffff, reg_906;
	uint8_t  offset;

    if (state->revision == 0x4003 ||
        state->revision == 0x4004 ||
        state->revision == 0x4005)
        offset = 1;
    else
        offset = 0;

    reg_906 = dib7000m_read_word(state, 906 + offset) | 0x3; /* keep settings for RISC */

	/* now, depending on the requested mode, we power on */
	switch (mode) {
		/* power up everything in the demod */
		case DIB7000M_POWER_ALL:
			reg_903 = 0x0000; reg_904 = 0x0000; reg_905 = 0x0000; reg_906 = 0x0000;
			break;

		/* just leave power on the control-interfaces: GPIO and (I2C or SDIO or SRAM) */
		case DIB7000M_POWER_INTERFACE_ONLY: /* TODO power up either SDIO or I2C or SRAM */
			reg_905 &= ~((1 << 7) | (1 << 6) | (1 << 5) | (1 << 2));
			break;

		case DIB7000M_POWER_INTERF_ANALOG_AGC:
			reg_903 &= ~((1 << 15) | (1 << 14) | (1 << 11) | (1 << 10));
			reg_905 &= ~((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (1 << 2));
			reg_906 &= ~((1 << 0));
			break;

		case DIB7000M_POWER_COR4_DINTLV_ICIRM_EQUAL_CFROD:
			reg_903 = 0x0000; reg_904 = 0x801f; reg_905 = 0x0000; reg_906 &= ~((1 << 0));
			break;

		case DIB7000M_POWER_COR4_CRY_ESRAM_MOUT_NUD:
			reg_903 = 0x0000; reg_904 = 0x8000; reg_905 = 0x010b; reg_906 &= ~((1 << 0));
			break;
		case DIB7000M_POWER_NO:
			break;
	}

	/* always power down unused parts */
	if (!state->platform.host.mobile_mode)
		reg_904 |= (1 << 7) | (1 << 6) | (1 << 4) | (1 << 2) | (1 << 1);

	/* P_sdio_select_clk = 0 on MC and after*/
	if (state->revision != 0x4000)
		reg_906 <<= 1;

	if((dib7000m_write_word(state, 903 + offset, reg_903))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state, 904 + offset, reg_904))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state, 905 + offset, reg_905))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state, 906 + offset, reg_906))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

	return DIB_RETURN_SUCCESS;
}

int dib7000m_enable_vbg_voltage(struct dibFrontend *demod)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(demod->demod_priv);
	dbgpl(&dib7000m_dbg, "enabling VBG voltage in the ADC");
	/* P_dual_adc_cfg0 */
	dib7000m_write_word(state, 913, 0x0000);
	/* P_dual_adc_cfg1 = 3, P_sar_adc_cfg = 2 */
	dib7000m_write_word(state, 914, (3 << 2) | (2 << 0));

	return DIB_RETURN_SUCCESS;
}
