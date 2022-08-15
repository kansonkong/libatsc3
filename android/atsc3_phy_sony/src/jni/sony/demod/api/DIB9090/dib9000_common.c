#include "dib7000m_priv.h"

static const struct dibDebugObject dib9000_dbg = {
	DEBUG_DEMOD,
	"DiB9000"
};


/*
n_addr                     0             0     8     0     0     1     784   7     0
n_data                     0             0     16    0     0     1     785   15    0
n_rw                       0             0     1     0     0     1     786   0     0
n_nbOfData                 0             0     2     0     0     1     787   1     0
n_clockRate                30            0     16    0     0     1     788   15    0
n_fifoFull                 0             0     1     0     1     1     789   0     0
n_nbOfData_rd              0             0     3     0     1     1     790   2     0
n_nbOfData_wr              0             0     3     0     1     1     791   2     0
n_nbOfCmd                  0             0     3     0     1     1     792   2     0
*/

int dib9000_tuner_xfer(struct dibDataBusClient *client, struct dibDataBusAccess *acc)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(client->host->priv);
	uint16_t i, len, t;

	if (acc->rxlen == 0 || acc->rx == NULL) { /* write */

		if (!(acc->mode & DATA_BUS_ACCESS_MODE_NO_SYNC)) {
			i = 1000;
			while (dib7000m_read_word(state, 789) && i)
				i--;
			if (i == 0)
				dbgpl(&dib9000_dbg, "TunerITF: write busy");
		}

		len = (uint16_t)(acc->txlen);
		if (len > 16)
			len = 16;

		for (i = 0; i < len; i +=2 ) /* TODO use dib7000m_write without incrementing the address */
			if((dib7000m_write_word(state, 785, (acc->tx[i] << 8) | acc->tx[i+1]))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;

		if((dib7000m_write_word(state, 784, (uint16_t) acc->address))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

		if((dib7000m_write_word(state, 787, (len/2) - 1))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

		if((dib7000m_write_word(state, 786, 0))!=DIB_RETURN_SUCCESS) /* start write */
			return DIB_RETURN_ERROR;

		if (!(acc->mode & DATA_BUS_ACCESS_MODE_NO_SYNC)) {
			i = 1000;
			while (dib7000m_read_word(state, 791) > 0 && i) i--;
			if (i == 0)
				dbgpl(&dib9000_dbg, "TunerITF: write failed");
		}
	} else {
		len = (uint16_t)(acc->rxlen);
		if (len > 16)
			len = 16;

		if (dib7000m_read_word(state, 790) != 0)
			dbgpl(&dib9000_dbg, "TunerITF: read busy");

		if((dib7000m_write_word(state, 784, (uint16_t) acc->address))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
		if((dib7000m_write_word(state, 787, (len/2) - 1))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
		if((dib7000m_write_word(state, 786, 1))!=DIB_RETURN_SUCCESS) /* start read */
			return DIB_RETURN_ERROR;

		i = 1000;
		while (dib7000m_read_word(state, 790) != (len/2) && i)
			i--;

		if (i == 0)
			dbgpl(&dib9000_dbg, "TunerITF: read failed");

		for (i = 0; i < len; i += 2) {
			t = dib7000m_read_word(state, 785);
			acc->rx[i]   = (t >> 8) & 0xff;
			acc->rx[i+1] = (t     ) & 0xff;
		}
		if (dib7000m_read_word(state, 790) != 0)
			dbgpl(&dib9000_dbg, "TunerITF: read more data than expected");
	}
	return DIB_RETURN_SUCCESS;
}

struct dibDataBusHost * dib9000_get_tuner_interface(struct dibFrontend *demod)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(demod->demod_priv);
	return &state->chip.d9.tuner_if;
}

void dib9000_post_pll_init(struct dibFrontend *fe)
{
    switch (data_bus_host_type( demod_get_data_bus_client(fe)->host )) {
        case DATA_BUS_MMIO_SRAM_LIKE:
            data_bus_client_set_speed(demod_get_data_bus_client(fe), 1);
            break;
        case DATA_BUS_I2C:
            data_bus_client_set_speed(demod_get_data_bus_client(fe), 1000);
            break;
    }
}

