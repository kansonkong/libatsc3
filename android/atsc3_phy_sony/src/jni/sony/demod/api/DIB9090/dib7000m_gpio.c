#include "dib7000m_priv.h"

int dib7000m_cfg_gpio(struct dib7000m_state *st, uint8_t num, uint8_t dir, uint8_t val)
{
	st->gpio_dir = dib7000m_read_word(st, 773);
	st->gpio_dir &= ~(1 << num);         /* reset the direction bit */
	st->gpio_dir |=  (dir & 0x1) << num; /* set the new direction */
	if((dib7000m_write_word(st, 773, st->gpio_dir))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

	st->gpio_val = dib7000m_read_word(st, 774);
	st->gpio_val &= ~(1 << num);          /* reset the direction bit */
	st->gpio_val |=  (val & 0x01) << num; /* set the new value */
	if((dib7000m_write_word(st, 774, st->gpio_val))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

	dbgpl(&dib7000m_dbg, "gpio dir: %04x: gpio val: %04x",
		st->gpio_dir, st->gpio_val);

	return DIB_RETURN_SUCCESS;
}

/* exported function for the user */
int dib7000m_set_gpio(struct dibFrontend *demod, uint8_t num, uint8_t dir, uint8_t val)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(demod->demod_priv);
	return dib7000m_cfg_gpio(state, num, dir, val);
}

int dib7000m_reset_gpio(struct dib7000m_state *st)
{
	/* reset the GPIOs */
	if((dib7000m_write_word(st, 773, st->gpio_dir))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(st, 774, st->gpio_val))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

	/* TODO 782 is P_gpio_od */
	if((dib7000m_write_word(st, 775, st->gpio_pwm_pos))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

	if((dib7000m_write_word(st, 780, st->pwm_freq_div))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	return DIB_RETURN_SUCCESS;
}

int dib7000m_set_pwm_gpio(struct dibFrontend *fe, uint8_t num, uint8_t id, uint8_t freq, uint32_t rate)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
    uint16_t value;
    uint8_t bit = (3 - id)*4;

    if (id > 4)
        return DIB_RETURN_ERROR;

    dib7000m_cfg_gpio(state, num, 0, 0);

    value  = dib7000m_read_word(state, 775);
    value &= ~(0xf << bit);
    value |=  (num << bit);
    dib7000m_write_word(state, 775, value);
    dib7000m_write_word(state, 776 + id, (uint16_t)rate);
    dib7000m_write_word(state, 780, freq);
    return DIB_RETURN_SUCCESS;
}

