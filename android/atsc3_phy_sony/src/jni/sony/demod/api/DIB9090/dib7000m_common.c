#include "dib7000m_priv.h"

uint16_t dib7000m_identify(struct dibDataBusClient *client)
{
	uint16_t value;

	if ((value = data_bus_client_read16(client, 896)) != 0x01b3) {
		dbgpl(&dib7000m_dbg, "wrong Vendor ID (0x%x)",value);
		return 0;
	}

	value = data_bus_client_read16(client, 897);
	if (value != 0x4000 &&
		value != 0x4001 &&
		value != 0x4002 &&
		value != 0x4003 &&
		value != 0x4004 &&
		value != 0x4005) {
		dbgpl(&dib7000m_dbg, "wrong Device ID (0x%x)",value);
		return 0;
	}

	/* protect this driver to be used with 7000PC */
	if (value == 0x4000 && data_bus_client_read16(client, 769) == 0x4000) {
		dbgpl(&dib7000m_dbg, "this driver does not work with DiB7000PC");
		return 0;
	}

	switch (value) {
		case 0x4000: dbgpl(&dib7000m_dbg, "found DiB7000MA/PA/MB/PB"); break;
		case 0x4001: dbgpl(&dib7000m_dbg, "found DiB7000HC"); break;
		case 0x4002: dbgpl(&dib7000m_dbg, "found DiB7000MC"); break;
		case 0x4003: dbgpl(&dib7000m_dbg, "found DiB9000A"); break;
		case 0x4004: dbgpl(&dib7000m_dbg, "found DiB9000H"); break;
		case 0x4005: dbgpl(&dib7000m_dbg, "found DiB9000M"); break;
	}

	return value;
}

/* first_addr is 0x80 for the DiB7000 */
int dib7000m_i2c_enumeration(struct dibDataBusHost *host, int no_of_demods, uint8_t default_addr, uint8_t first_addr)
{
    int k = 0;
    uint8_t new_addr = 0;
    struct dibDataBusClient client;

    data_bus_client_init(&client, &dibx000_i2c_data_bus_client, host);
    data_bus_client_set_speed(&client, 340);

    /* disable Mout HB12 problems in diversity-3 or more solutions */
    data_bus_client_set_device_id(&client, default_addr + 16);
    data_bus_client_write16(&client, 1796, 0x0);

    for (k = no_of_demods-1; k >= 0; k--) {
        /* designated i2c address */
        new_addr = (uint8_t)(first_addr + (k << 1));

        data_bus_client_set_device_id(&client, default_addr);
//        data_bus_client_write16(&client, 0xffff, 0xffff);

        if((data_bus_client_write16(&client, 1817, 0x0003))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
        if((data_bus_client_write16(&client, 1796, 0x0))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        if((data_bus_client_write16(&client, 1227, 1))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        if((data_bus_client_write16(&client, 1227, 0))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        data_bus_client_set_device_id(&client, new_addr);
//        data_bus_client_write16(&client, 0xffff, 0xffff);

		//----the following 4 client_write16 could not get error handle or the dibcom would init failed ----/
        data_bus_client_write16(&client, 1817, 0x0003);
        data_bus_client_write16(&client, 1796, 0x0);
        data_bus_client_write16(&client, 1227, 1);
        data_bus_client_write16(&client, 1227, 0);

        
		if (dib7000m_identify(&client) == 0) {
            data_bus_client_set_device_id(&client, default_addr);
			if (dib7000m_identify(&client) == 0) {
                dbgpl(&dib7000m_dbg, "i2c-enum: #%d not identified", k);
                return DIB_RETURN_ERROR;
            }
        }

        /* start diversity to pull_down div_str - just for i2c-enumeration */
        if((data_bus_client_write16(&client, 1795, (1 << 10) | (4 << 6)))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        /* set new i2c address and force divstart */
        if((data_bus_client_write16(&client, 1794, (new_addr << 2) | 0x2))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        dbgpl(&dib7000m_dbg, "IC #%d initialized (to i2c_address 0x%x)", k, new_addr);

    }

    for (k = 0; k < no_of_demods; k++) {
        new_addr = (uint8_t)(first_addr | (k << 1));
        data_bus_client_set_device_id(&client, new_addr);

        // unforce divstr
        if((data_bus_client_write16(&client, 1794, new_addr << 2))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        /* deactivate div - it was just for i2c-enumeration */
        if((data_bus_client_write16(&client, 1795, 0))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
    }
    return DIB_RETURN_SUCCESS;
}

int dib7000m_release(struct dibFrontend *demod)
{
	struct dib7000m_state *st = (struct dib7000m_state *)(demod->demod_priv);
	dibx000_exit_i2c_master(&st->i2c_master);
	if (st != NULL)
		MemFree(st,sizeof(struct dib7000m_state));
	return DIB_RETURN_SUCCESS;
}

struct dibDataBusHost * dib7000m_get_i2c_master(struct dibFrontend *demod, enum dibx000_i2c_interface intf, int gating)
{
	struct dib7000m_state *st = (struct dib7000m_state *)(demod->demod_priv);
	return dibx000_get_i2c_adapter(&st->i2c_master, intf, gating);
}

void dib7000m_close_gate(struct dibFrontend *fe)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
	dib7000m_write_word(state, 769, 1 << 8);
}

int dib7000m_select_data_bus_client(struct dibFrontend *fe, struct dibDataBusHost *host, uint8_t i2c_addr)
{
    struct dibDataBusClient *client = demod_get_data_bus_client(fe);

    switch (data_bus_host_type(host)) {
        case DATA_BUS_I2C:
            data_bus_client_init(client, &dibx000_i2c_data_bus_client, host);
            data_bus_client_set_speed(client, 340);
            break;
        case DATA_BUS_MMIO_SRAM_LIKE:
        case DATA_BUS_SDIO:
            data_bus_client_init(client, &dibx000_mmio_data_bus_client, host);
            data_bus_client_set_speed(client, 0);
            break;
        default:
            return DIB_RETURN_ERROR;
    }
    data_bus_client_set_device_id(client, i2c_addr);
    return DIB_RETURN_SUCCESS;
}

DEBUG_OBJECT(dib7000m_dbg, DEBUG_DEMOD, "DiB7000M");

void dib7000m_write_tab(struct dib7000m_state *state, const uint16_t *buf)
{
	uint16_t l = 0, r;
    const uint16_t *n;
	n = buf;
	l = *n++;
	while (l) {
		r = *n++;

		if (state->reg_offs && (r >= 112 && r <= 331)) // compensate for 7000MC
			r++;

		do {
			dib7000m_write_word(state, r, *n++);
			r++;
		} while (--l);
		l = *n++;
	}
}


uint32_t dib7000m_get_snr(uint16_t a, uint16_t b)
{
    uint32_t s,n;
    int8_t e;

    /* SNR */
    n = a >> 4;

    e = (a & 0xf) << 2;
    e |= b >> 14;
    if (e & 0x20)
        e -= 0x40;
    n <<= e + 16;

    s = (b >> 6) & 0xff;
    e = (uint8_t) (b & 0x3f);
    if (e & 0x20)
        e -= 0x40;
    s <<= e + 16;

    return uint32_finite_div(s, n, 16);
}

