#include "dib7000m_priv.h"
#include "dib7000m_fw_priv.h"

static int dib9000_fw_reset(struct dibFrontend *fe)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);

    if((dib7000m_write_word(state, 1817, 0x0003))!=DIB_RETURN_SUCCESS) // SRAM read lead in + P_host_rdy_cmos=1
		return DIB_RETURN_ERROR;

    if((dib7000m_write_word(state, REG_MAC_RESET, 1))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    if((dib7000m_write_word(state, REG_MAC_RESET, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    switch ((state->revision = dib7000m_identify( demod_get_data_bus_client(state->fe))) ) {
        case 0x4003:
        case 0x4004:
        case 0x4005: state->reg_offs = 1; break;
        default:
            return DIB_RETURN_ERROR;
    }

    /* reset the i2c-master to use the host interface */
    dibx000_reset_i2c_master(&state->i2c_master);

    if((dib7000m_set_power_mode(state, DIB7000M_POWER_ALL))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* unforce divstr regardless whether i2c enumeration was done or not */
	if((dib7000m_write_word(state, 1794, dib7000m_read_word(state, 1794) & ~(1 << 1) ))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    if((dib7000m_write_word(state, 1796, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    if((dib7000m_write_word(state, 1805, 0x805))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    /* restart all parts */
	if((dib7000m_write_word(state,  898, 0xffff))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state,  899, 0xffff))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state,  900, 0x0001))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state,  901, 0xff19))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state,  902, 0x003c))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

	if((dib7000m_write_word(state,  898, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state,  899, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state,  900, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state,  901, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_write_word(state,  902, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

	if((dib7000m_write_word(state, 911, state->chip.d9.cfg.if_drives))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    if((dib7000m_set_power_mode(state, DIB7000M_POWER_INTERFACE_ONLY))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    state->info.ber_rs_len = 2;

    return DIB_RETURN_SUCCESS;
}

/* DataBusClient using message to read/write registers */
static int dib7000m_risc_apb_access_read(struct dibDataBusClient *client, uint32_t address, uint16_t attribute, const uint8_t *tx, uint32_t txlen, uint8_t *b, uint32_t len)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(client->host->priv);
    uint16_t mb[10];
    uint8_t i, s;

    if (address >= 1024 ||
            !state->platform.risc.fw_is_running)
        return data_bus_client_read_attr(&state->platform.risc.orig_client, address, attribute, b, len);

    //dbgpl(&dib7000m_dbg, "APB access thru rd fw %d %x", address, attribute);

    mb[0] = (uint16_t) address;
    mb[1] = (uint16_t)(len/2);
    if((dib7000m_mbx_send_attr(state, OUT_MSG_BRIDGE_APB_R, mb, 2, attribute))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    switch (dib7000m_mbx_get_message_attr(state, IN_MSG_END_BRIDGE_APB_RW, mb, &s, attribute)) {
    case 1:
        s--; // address
        for (i = 0; i < s; i++) {
            b[i*2]   = (mb[i+1] >> 8) & 0xff;
            b[i*2+1] = (mb[i+1]     ) & 0xff;
        }
        return DIB_RETURN_SUCCESS;
    default:
        return DIB_RETURN_ERROR;
    }
    return DIB_RETURN_ERROR;
}

static int dib7000m_risc_apb_access_write(struct dibDataBusClient *client, uint32_t address, uint16_t attribute, const uint8_t *b, uint32_t len)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(client->host->priv);
    uint16_t mb[10];
    uint8_t s,i;

    if (address >= 1024 ||
            !state->platform.risc.fw_is_running)
        return data_bus_client_write_attr(&state->platform.risc.orig_client, address, attribute, b, len);

    //dbgpl(&dib7000m_dbg, "APB access thru wr fw %d %x", address, attribute);

    mb[0] = (unsigned short)address;
    for (i = 0; i < len && i < 20; i += 2) // 20 bytes max
        mb[1+(i/2)] = (b[i] << 8 | b[i+1]);

    if((dib7000m_mbx_send_attr(state, OUT_MSG_BRIDGE_APB_W, mb, (uint8_t)(1+len/2), attribute))!= DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    return dib7000m_mbx_get_message_attr(state, IN_MSG_END_BRIDGE_APB_RW, mb, &s, attribute) == 1 ? DIB_RETURN_SUCCESS : DIB_RETURN_ERROR;
}

struct dibDataBusClient dib7000m_risc_apb_access_template = {
    NULL,

    12,
    16,

    dib7000m_risc_apb_access_read,
    dib7000m_risc_apb_access_write,

    0,

    100,
};

static int dib7000m_risc_apb_access_pass_thru(struct dibDataBusClient *client, struct dibDataBusAccess *acc)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(client->host->priv);
    data_bus_client_set_device_id(&state->platform.risc.orig_client, data_bus_client_get_device_id(client));
    return data_bus_transfer(&state->platform.risc.orig_client, acc, 1);
}

static int dib7000m_fw_memmbx_sync(struct dib7000m_state *state, uint8_t i)
{
    if (!state->platform.risc.fw_is_running)
        return DIB_RETURN_SUCCESS;
    if((dib7000m_risc_mem_write(state, FE_MM_RW_SYNC, &i))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    do {
        if((dib7000m_risc_mem_read(state, FE_MM_RW_SYNC, &i, 1))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
    } while (i);
	return DIB_RETURN_SUCCESS;

}

static int dib7000m_fw_component_bus_access(struct dib7000m_state *state, uint8_t type, uint8_t port,
        uint8_t device_id, uint16_t scl, struct dibDataBusAccess *acc)
{
    struct dib7000m_fe_memory_map *m = &state->platform.risc.fe_mm[FE_MM_RW_COMPONENT_ACCESS_BUFFER];
    uint8_t p[13] = { 0 };
	int ret = DIB_RETURN_SUCCESS;

    p[0] = type;
    p[1] = port;
    p[2] = device_id;

    p[3] = (uint8_t) scl & 0xff; /* scl */
    p[4] = (uint8_t) (scl >> 8);

//  p[5] = 0; /* attr */
//  p[6] = 0;

    p[7] = (uint8_t) (acc->address     );
    p[8] = (uint8_t) (acc->address >> 8);

    p[9]  = (uint8_t) (acc->txlen     );
    p[10] = (uint8_t) (acc->txlen >> 8);
    p[11] = (uint8_t) (acc->rxlen     );
    p[12] = (uint8_t) (acc->rxlen >> 8);

    DibAcquireLock(&state->platform.risc.mem_mbx_lock);

    //dbgp("toll %d %d %x %d %d %d %d\n", type, port, device_id, scl, acc->address, acc->txlen, acc->rxlen);

    if((dib7000m_risc_mem_write(state, FE_MM_W_COMPONENT_ACCESS, p))!=DIB_RETURN_SUCCESS)
	{
		ret = DIB_RETURN_ERROR;
		goto exit;
	}

    if (acc->txlen) { /* write-part */
        if((dib7000m_risc_mem_setup_cmd(state, m->addr, acc->txlen, 0))!=DIB_RETURN_SUCCESS)
		{
			ret = DIB_RETURN_ERROR;
			goto exit;
		}

        if((dib7000m_risc_mem_write_chunks(state, acc->tx, acc->txlen))!=DIB_RETURN_SUCCESS)
		{
			ret = DIB_RETURN_ERROR;
			goto exit;
		}
    }

    /* do the transaction */
    if((dib7000m_fw_memmbx_sync(state, FE_SYNC_COMPONENT_ACCESS))!=DIB_RETURN_SUCCESS)
	{
		ret = DIB_RETURN_ERROR;
		goto exit;
	}

    /* read back any possible result */
    if (acc->rxlen > 0)
	{
        if((dib7000m_risc_mem_read(state, FE_MM_RW_COMPONENT_ACCESS_BUFFER, acc->rx, (uint16_t)(acc->rxlen)))!=DIB_RETURN_SUCCESS)
	
		{
			ret = DIB_RETURN_ERROR;
			goto exit;
		}
	
	}
exit:
    DibReleaseLock(&state->platform.risc.mem_mbx_lock);

    return ret;
}


/* data bus host to write to slave components through the firmware - software driven gate */
static int dib7000m_fw_component_bus_xfer(struct dibDataBusClient *client, struct dibDataBusAccess *acc)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(client->host->priv);

    return dib7000m_fw_component_bus_access(state,
            data_bus_host_type(client->host) == DATA_BUS_I2C ? 0 : 1,
            client->host->interface_id,
            (unsigned char)data_bus_client_get_device_id(client),
            (unsigned char)data_bus_client_get_speed(client, acc),
            acc);
}

static int dib9000_fw_antenna_xfer(struct dibDataBusClient *client, struct dibDataBusAccess *acc)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(client->host->priv);
    if((dib7000m_fw_component_bus_access(state, 2, 0, 0, 0, acc))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	return DIB_RETURN_SUCCESS;
}

static int dib9000_fw_init(struct dib7000m_state *state)
{
    struct dibGPIOFunction *f;
    uint16_t b[40] = { 0 };
    uint8_t i;
    uint8_t size;

	if (dib7000m_fw_boot(state, NULL, 0, state->chip.d9.cfg.microcode_B_fe_buffer, state->chip.d9.cfg.microcode_B_fe_size) != DIB_RETURN_SUCCESS)
    //if (dib7000m_fw_boot(state, microcode_A_buffer, microcode_A_size, microcode_B_buffer, microcode_B_size) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

    if((data_bus_host_interface_reinit( &state->chip.d9.tuner_if, dib7000m_fw_component_bus_xfer, state, 0)) != DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    if((data_bus_host_interface_reinit( &state->i2c_master.tuner_i2c_adap, dib7000m_fw_component_bus_xfer, state, 0))!= DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    if((data_bus_host_interface_reinit( &state->i2c_master.gpio_1_2_i2c_adap, dib7000m_fw_component_bus_xfer, state, 1))!= DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    if((data_bus_host_interface_reinit( &state->i2c_master.gpio_3_4_i2c_adap, dib7000m_fw_component_bus_xfer, state, 2))!= DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* initialize the firmware */
    for (i = 0; i < ARRAY_SIZE(state->chip.d9.cfg.gpio_function); i++) {
        f = &state->chip.d9.cfg.gpio_function[i];
        if (f->mask) {
            switch (f->function) {
                case BOARD_GPIO_FUNCTION_COMPONENT_ON:  b[0] = (uint16_t) f->mask; b[1] = (uint16_t) f->direction; b[2] = (uint16_t) f->value; break;
                case BOARD_GPIO_FUNCTION_COMPONENT_OFF: b[3] = (uint16_t) f->mask; b[4] = (uint16_t) f->direction; b[5] = (uint16_t) f->value; break;
            }
        }
    }
    if (dib7000m_mbx_send(state, OUT_MSG_CONF_GPIO, b, 15) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

    /* subband */
    DibZeroMemory(b, sizeof(b));
    b[0] = state->chip.d9.cfg.subband.size; /* type == 0 -> GPIO - PWM not yet supported */
    for (i = 0; i < state->chip.d9.cfg.subband.size; i++) {
        b[1 + i*4] = state->chip.d9.cfg.subband.subband[i].f_mhz;
        b[2 + i*4] = (uint16_t) state->chip.d9.cfg.subband.subband[i].gpio.mask;
        b[3 + i*4] = (uint16_t) state->chip.d9.cfg.subband.subband[i].gpio.direction;
        b[4 + i*4] = (uint16_t) state->chip.d9.cfg.subband.subband[i].gpio.value;
        //dbgpl(&dib7000m_dbg, "SBS: %d %d %x %x %x\n", i, b[1 + i*4], b[2 + i*4], b[3 + i*4], b[4 + i*4]);
    }
    b[1 + i*4] = 0; /* fe_id */
    if (dib7000m_mbx_send(state, OUT_MSG_SUBBAND_SEL, b, 2 + 4*i) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

    /* 0 - id, 1 - no_of_frontends */
    b[0]  = (0 << 8) | 1;
    /* 0 = i2c-address demod, 0 = tuner */
    b[1]  = (0 << 8) | (0); //data_bus_client_get_device_id(tuner_get_data_bus_client( state->fe ) ) );
    b[2]  = (uint16_t) (((state->chip.d9.cfg.xtal_clock_khz * 1000) >> 16) & 0xffff);
    b[3]  = (uint16_t) (((state->chip.d9.cfg.xtal_clock_khz * 1000)      ) & 0xffff);
    b[4]  = (uint16_t) (( state->chip.d9.cfg.vcxo_timer >> 16) & 0xffff);
    b[5]  = (uint16_t) (( state->chip.d9.cfg.vcxo_timer      ) & 0xffff);
    b[6]  = (uint16_t) (( state->chip.d9.cfg.timing_frequency >> 16) & 0xffff);
    b[7]  = (uint16_t) (( state->chip.d9.cfg.timing_frequency      ) & 0xffff);
    b[29] =   state->chip.d9.cfg.if_drives;
    if (dib7000m_mbx_send(state, OUT_MSG_INIT_DEMOD, b, ARRAY_SIZE(b)) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

    if (dib7000m_mbx_send(state, OUT_MSG_FE_FW_DL, NULL, 0) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

    if (dib7000m_mbx_get_message(state, IN_MSG_FE_FW_DL_DONE, b, &size) == -1)
        return DIB_RETURN_ERROR;

	if (size >  ARRAY_SIZE(b)) {
		dbgpl(&dib7000m_dbg, "error : firmware returned %dbytes needed but the used buffer has only %dbytes\n Firmware init ABORTED", size, ARRAY_SIZE(b));
		return DIB_RETURN_ERROR;
	}

    for (i = 0; i < size; i += 2) {
        state->platform.risc.fe_mm[i/2].addr = b[i+0];
        state->platform.risc.fe_mm[i/2].size = b[i+1];
    //    dbgpl(&dib7000m_dbg, "MM: %d %d %d", state->platform.risc.fe_mm[i/2].addr, state->platform.risc.fe_mm[i/2].size, ARRAY_SIZE(state->platform.risc.fe_mm));
    }

    return DIB_RETURN_SUCCESS;
}

/* Frontend interface */
static int dib7000m_fw_wakeup(struct dibFrontend *fe)
{
    return DIB_RETURN_SUCCESS;
}

static int dib7000m_fw_set_channel_head(struct dib7000m_state *state, struct dibChannel *ch)
{
    uint8_t b[9];
    uint32_t freq = ch->RF_kHz;
    if (state->fe->id % 2)
        freq += 101;

    b[0] = (uint8_t) ((freq >>  0)    & 0xff);
    b[1] = (uint8_t) ((freq >>  8)    & 0xff);
    b[2] = (uint8_t) ((freq >> 16)    & 0xff);
    b[3] = (uint8_t) ((freq >> 24)    & 0xff);
    b[4] = (uint8_t) ((ch->bandwidth_kHz >>  0)    & 0xff);
    b[5] = (uint8_t) ((ch->bandwidth_kHz >>  8)    & 0xff);
    b[6] = (uint8_t) ((ch->bandwidth_kHz >> 16)    & 0xff);
    b[7] = (uint8_t) ((ch->bandwidth_kHz >> 24)    & 0xff);
    b[8] = ch->type | 0x80; /* do not wait for CELL ID when doing autosearch */
    if((dib7000m_risc_mem_write(state, FE_MM_W_CHANNEL_HEAD, b))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	return DIB_RETURN_SUCCESS;
}

static int dib7000m_fw_get_channel(struct dibFrontend *fe, struct dibChannel *ch)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
	int ret = DIB_RETURN_SUCCESS;

    DibAcquireLock(&state->platform.risc.mem_mbx_lock);
    if((dib7000m_fw_memmbx_sync(state, FE_SYNC_CHANNEL))!=DIB_RETURN_SUCCESS)
	{
		ret = DIB_RETURN_ERROR;
		goto exit;
	}
    if((dib7000m_risc_mem_read(state, FE_MM_R_CHANNEL_UNION, (uint8_t *) &ch->u.dvbt, sizeof(struct dibDVBTChannel)))!=DIB_RETURN_SUCCESS)
	{
		ret = DIB_RETURN_ERROR;
		goto exit;
	}

    /* do not read the context for now, as we cannot sync */
//    dib7000m_risc_mem_read(state, FE_MM_R_FE_INFO, channel_frontend_info(ch, fe)->priv, state->platform.risc.fe_mm[FE_MM_R_FE_INFO].size);
exit:
    DibReleaseLock(&state->platform.risc.mem_mbx_lock);
    return ret;
}

static int dib7000m_fw_tune(struct dibFrontend *fe, struct dibChannel *ch)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
    int ret = 10, search = ch->context.status == CHANNEL_STATUS_PARAMETERS_UNKNOWN;
    int8_t i;

    switch (fe->tune_state) {
    case CT_DEMOD_START:
        if((dib7000m_fw_set_channel_head(state, ch))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        /* write the channel context - a channel is initialized to 0, so it is OK */
        if((dib7000m_risc_mem_write(state, FE_MM_W_CHANNEL_CONTEXT, (uint8_t *) channel_frontend_info(ch, fe->id)->priv))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
        if((dib7000m_risc_mem_write(state, FE_MM_W_FE_INFO, (uint8_t *) channel_frontend_info(ch, fe->id)->priv))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        if (search)
		{
            if((dib7000m_mbx_send(state, OUT_MSG_FE_CHANNEL_SEARCH, NULL, 0))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
		}
        else {
            if((dib7000m_risc_mem_write(state, FE_MM_W_CHANNEL_UNION, (uint8_t *) &ch->u.dvbt))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
            if((dib7000m_mbx_send(state, OUT_MSG_FE_CHANNEL_TUNE, NULL, 0))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
        }
        fe->tune_state = CT_DEMOD_STEP_1;
    break;
    case CT_DEMOD_STEP_1:
        if (search)
		{
            if((dib7000m_risc_mem_read(state, FE_MM_R_CHANNEL_SEARCH_STATE, (uint8_t *) &i, 1))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
		}
		else
		{
            if((dib7000m_risc_mem_read(state, FE_MM_R_CHANNEL_TUNE_STATE, (uint8_t *) &i, 1))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
		}
        switch (i) { /* something happened */
        case 0: break;
        case -2: /* tps locks are "slower" than MPEG locks -> even in autosearch data is OK here */
            if (search)
                fe->status = FE_STATUS_DEMOD_SUCCESS;
            else {
                fe->tune_state = CT_DEMOD_STOP;
                fe->status = FE_STATUS_LOCKED;
            }
        break;
        default:
            fe->status = FE_STATUS_TUNE_FAILED;
            fe->tune_state = CT_DEMOD_STOP;
        break;
        }
    break;
    default:
        ret = FE_CALLBACK_TIME_NEVER;
    break;
    }

    return ret;
}

static int dib7000m_fw_sleep(struct dibFrontend *fe, uint8_t deep)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
    if (deep) {

        state->platform.risc.fw_is_running = 0;

        switch (data_bus_host_type( data_bus_client_get_data_bus( &state->platform.risc.orig_client ) )) {
            case DATA_BUS_SDIO:
            case DATA_BUS_MMIO_SRAM_LIKE: data_bus_client_set_speed(&state->platform.risc.orig_client, 0); break;
            case DATA_BUS_I2C:            data_bus_client_set_speed(&state->platform.risc.orig_client, 340); break;
        }

        data_bus_host_interface_reinit(&state->chip.d9.tuner_if, dib9000_tuner_xfer, state, 0);
        dibx000_deep_sleep_i2c_master(&state->i2c_master);
        return DIB_RETURN_SUCCESS;
    } else
        return dib7000m_mbx_send(state, OUT_MSG_FE_SLEEP, NULL, 0);
}

static int dib7000m_fw_set_diversity_in(struct dibFrontend *fe, int onoff)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
	uint16_t mode = (uint16_t) onoff;
    return dib7000m_mbx_send(state, OUT_MSG_ENABLE_DIVERSITY, &mode, 1);
}

static int dib7000m_fw_set_output_mode(struct dibFrontend *fe, int mode)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
	uint16_t outreg, smo_mode;

    dbgpl(&dib7000m_dbg, "setting output mode for demod %p to %d", state->fe, mode);

	switch (mode) {
		case OUTPUT_MODE_TS_PARALLEL_GATED:  // STBs with parallel gated clock
			outreg = (1 << 10);  /* 0x0400 */
			break;
		case OUTPUT_MODE_TS_PARALLEL_CONT:   // STBs with parallel continues clock
			outreg = (1 << 10) | (1 << 6); /* 0x0440 */
			break;
		case OUTPUT_MODE_TS_SERIAL:    // STBs with serial input
			outreg = (1 << 10) | (2 << 6) | (0 << 1); /* 0x0482 */
			break;
		case OUTPUT_MODE_DIVERSITY:
            outreg = (1 << 10) | (4 << 6); /* 0x0500 */
			break;
		case OUTPUT_MODE_TS_FIFO:            // e.g. USB feeding
			outreg = (1 << 10) | (5 << 6);
			break;
		case OUTPUT_MODE_OFF:  // disable
			outreg = 0;
			break;
		default:
			dbgpl(&dib7000m_dbg, "Unhandled output_mode passed to be set for demod %p", state->fe);
			return DIB_RETURN_ERROR;
	}

	if((dib7000m_write_word(state, 1795, outreg))!=DIB_RETURN_SUCCESS) // has to be written from outside
		return DIB_RETURN_ERROR;

    switch (mode) {
        case OUTPUT_MODE_TS_PARALLEL_GATED:
        case OUTPUT_MODE_TS_PARALLEL_CONT:
        case OUTPUT_MODE_TS_SERIAL:
        case OUTPUT_MODE_TS_FIFO:
            smo_mode = (dib7000m_read_word(state, 295) & 0x0010) | (1 << 1);
            if (state->chip.d9.cfg.output_mpeg2_in_188_bytes)
                smo_mode |= (1 << 5) ;
            if((dib7000m_write_word(state, 295, smo_mode))!=DIB_RETURN_SUCCESS)
				return DIB_RETURN_ERROR;
            break;
    }

    outreg = (uint16_t) mode;
    return dib7000m_mbx_send(state, OUT_MSG_SET_OUTPUT_MODE, &outreg, 1);
}

#if 0
static int dib7000m_fw_set_gpio(struct dibFrontend *fe, uint8_t num, uint8_t dir, uint8_t val)
{
    return DIB_RETURN_SUCCESS;
}
#endif


#ifdef DIBCOM_GENERIC_MONITORING
#include "dibcomponent_monitor.h"

static int dib7000m_fw_monitor(struct dib7000m_state *state, struct dibDemodMonitor *m)
{
    uint16_t b[45];
	uint16_t c[16];
    uint8_t *d,i;
    uint16_t val;

    dib7000m_risc_mem_read(state, FE_MM_R_GENERIC_MONITORING, (uint8_t *) b, sizeof(b));
	dib7000m_fw_memmbx_sync(state, FE_SYNC_CHANNEL);
	dib7000m_risc_mem_read(state, FE_MM_R_FE_MONITOR, (uint8_t *) c, sizeof(c));

    /* byte-order conversion - big but endianias aware - risc is little-endian */
    d = (uint8_t *) b;
    for (i = 0; i < 45; i++) {
        val = b[i];
        *d++ = val >> 8;
        *d++ = val & 0xff;
    }

    val = b[0];
	m->locks.agc        = (val >> 15) & 1;
	m->locks.corm       = (val >> 14) & 1;
	m->locks.coff       = (val >> 13) & 1;
	m->locks.coff_cpil  = (val >> 12) & 1;
	m->locks.lmod4      = (val >> 11) & 1;
	m->locks.pha3       = (val >> 10) & 1;
	m->locks.equal      = (val >>  9) & 1;
	m->locks.vit        = (val >>  8) & 1;
	m->locks.fec_frm    = (val >>  7) & 1;
	m->locks.fec_mpeg   = (val >>  6) & 1;
	m->locks.tps_dec    = (val >>  5) & 1;
	m->locks.tps_sync   = (val >>  4) & 1;
	m->locks.tps_data   = (val >>  3) & 1;
	m->locks.tps_cellid = (val >>  1) & 1;
	m->locks.dvsy       = (val >>  0) & 1;

	m->iq_misgain = b[1];
	m->iq_misphi  = b[2];
	if (m->iq_misphi & 0x800)
		m->iq_misphi -= 0x1000;

	val = b[3];
	m->equal_noise_mant = (val >> 4) & 0xFF;
	m->equal_noise_exp = ((val & 0xF) << 2);

	val = b[4];
	m->equal_noise_exp += ((val >> 14) & 0x3);
	if ((m->equal_noise_exp & 0x20) != 0)
		m->equal_noise_exp -= 0x40;

	m->equal_signal_mant = (val >> 6) & 0xFF;
	m->equal_signal_exp  = (val & 0x3F);
    if ((m->equal_signal_exp & 0x20) != 0)
		m->equal_signal_exp -= 0x40;

	val = b[5];
	m->mer_mant = (val >> 6) & 0xFF;
	m->mer_exp = (val & 0x3F);
	if ((m->mer_exp & 0x20) == 0x20)
		m->mer_exp -= 0x40;

	m->ber_raw = (b[6] << 16) | b[7];
    m->PacketErrors     = b[8];
	m->PacketErrorCount = b[9];

	// Timing offset read
	m->timing_offset = (b[10] << 15) | (b[11] >> 1);
	if (m->timing_offset  & 0x20000000)
		m->timing_offset -= 0x40000000;

	m->timf_current = (b[12] << 16) | b[13];

	m->dds_freq = (b[14] << 16) + b[15]; //(0,0,26)
	if (m->dds_freq  & 0x2000000)
		m->dds_freq -= 0x4000000;

	val = b[16];
	m->invspec = (val >> 9) & 0x01;
	if (m->invspec > 0)
		m->dds_freq = (1 << 26) - m->dds_freq;
	m->p_dds_freq = ((val & 0x1ff) << 16) + b[17];       //(0,-1,26)

	m->viterbi_syndrome = b[18];

	val = b[19];
	INIT_DVB_CHANNEL(&m->cur_channel);
	m->cur_channel.nfft = (val >> 8) & 0x3;
	m->cur_channel.guard = val & 0x3;
	m->cur_channel.nqam = (val >> 14) & 0x3;
	m->cur_channel.vit_hrch = (val >> 13) & 0x1;
	m->cur_channel.vit_alpha = (val >> 10) & 0x7;
	m->cur_channel.vit_code_rate_hp = (val >> 5) & 0x7;
	m->cur_channel.vit_code_rate_lp = (val >> 2) & 0x7;
	val = b[20];
	m->cur_channel.intlv_native = (val >> 5) & 0x1;
	CONVERT_DVB_TO_DIGITAL_CHANNEL(&m->cur_channel, &m->cur_digital_channel);

	val = b[21];
	m->way0_ok = (val >> 1) & 1;
	m->way1_ok = (val     ) & 1;
	m->dvsy_delay = ((val >> 2) & 0x7ff) * 2;
	m->dvsy_first_arrived = (val >> 14) & 1;

	val = b[22];
	m->divin_fifo_overflow = val >> 14;
	m->divin_nberr_sym = val & 0x3fff;

	m->adc_power_i = ((b[23] & 0xf) << 16) + b[24];

	if (state->revision == 0x4003 || state->revision == 0x4004 || state->revision == 0x4005) {
        m->timf_default = state->chip.d9.cfg.timing_frequency;
		m->adc_monitor_only = 0;
		m->adc_power_q      = m->adc_power_i;
        m->rf_power         = c[4];
		m->adc_lnpower_raw  = b[25];
		if (m->adc_lnpower_raw  & 0x1000)
			m->adc_lnpower_raw -= 0x2000;

		m->adsample_lock    = (uint8_t) b[26];
		m->tuner_is_baseband = 1;
		m->agc1_raw = (uint16_t) c[2];
		m->agc2_raw = (uint16_t) c[3];
		m->agc_wbd_raw = (uint16_t) c[6];
		m->agc_split = (uint8_t) c[5];
	} else {
        m->timf_default = state->chip.d7.cfg.bw->timf;
		m->adc_monitor_only = 0;
		m->adc_power_q = ((b[27] & 0xf) << 16) + b[28];
		m->rf_power = b[29];

		m->agc1_raw = b[30];
		m->agc2_raw = b[31];
		m->agc_wbd_raw = b[32];
		m->agc_split = (uint8_t) b[33];

		m->tuner_is_baseband = state->chip.d7.cfg.tuner_is_baseband;
		m->tun_max_gain = state->chip.d7.cfg.tun_max_gain;
		m->tun_ref_level = state->chip.d7.cfg.tun_ref_level;
		m->lna_gain_step = state->chip.d7.cfg.lna_gain_step;
	}
	m->internal_clk = ((b[34] << 16) | b[35]) / 1000;

	m->n_2dpa_monitoring = 1;

    val = b[36];
	m->n_2dpa_mu_int  = (val >> 3) & 0x7f;
	m->n_2dpa_num_fir = (val >> 0) & 0x07;

	for (i = 0; i < 4; i++) {
		val = b[37 + i*2];
		m->cti_def_raw[i] = b[37 + i*2 + 1];
		if (val)
			m->cti_def_raw[i] -= (1 << 16);
	}
#ifdef DIBCOM_EXTENDED_MONITORING
	demod_monitor_post_process(m);
#endif
	return DIB_RETURN_SUCCESS;
}

#ifdef DIBCOM_EXTENDED_MONITORING
static int dib7000m_fw_channel_profile(struct dib7000m_state *state, struct dibDemodChannelProfile *profile)
{
    uint8_t b[128*4 + 2], i, *t;
	int16_t re[128], im[128];

    dib7000m_risc_mem_read(state, FE_MM_R_GENERIC_MONITORING, b, sizeof(b));

	profile->pha_shift = (b[0] << 8) | b[1];

    t = b + 2;
	for (i = 0; i < 128; i++) {
		re[i] = (t[0] << 8) | t[1];
		im[i] = (t[2] << 8) | t[3];
		if (t[0] & 0x40)
			re[i] -= 0x8000;
		if (t[2] & 0x40)
			im[i] -= 0x8000;
        t += 4;
	}

	demod_channel_profile_calc(re, im, profile);

    return DIB_RETURN_SUCCESS;
}
#endif

static int dib7000m_fw_generic_monitoring(struct dibFrontend *fe, uint32_t type, uint8_t *buf, uint32_t size, uint32_t offset)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
    uint8_t b[12]; /* forward the generic request to the firmware */
    int ret = DIB_RETURN_NOT_SUPPORTED;
    uint32_t snr = 0;

    //dbgpl(&dib7000m_dbg, "genmon: %d %d %d", type, size, offset);

    b[0]  = (uint8_t) (type >>  0);
    b[1]  = (uint8_t) (type >>  8);
    b[2]  = (uint8_t) (type >> 16);
    b[3]  = (uint8_t) (type >> 24);
    b[4]  = (uint8_t) (size >>  0);
    b[5]  = (uint8_t) (size >>  8);
    b[6]  = (uint8_t) (size >> 16);
    b[7]  = (uint8_t) (size >> 24);
    b[8]  = (uint8_t) (offset >>  0);
    b[9]  = (uint8_t) (offset >>  8);
    b[10] = (uint8_t) (offset >> 16);
    b[11] = (uint8_t) (offset >> 24);

    DibAcquireLock(&state->platform.risc.mem_mbx_lock);

    /* submit request to the firmware */
	if((dib7000m_risc_mem_write(state, FE_MM_W_GENERIC_MONITORING, b))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
	if((dib7000m_fw_memmbx_sync(state, FE_SYNC_W_GENERIC_MONIT))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    switch (type & ~GENERIC_MONIT_REQUEST_SIZE) {
    case GENERIC_MONIT_RESET_PE_COUNT: /* request is fulfilled */
        ret = 0;
        break;


    case GENERIC_MONIT_COMPAT_EXTENDED:
        if (size > 0)
            dib7000m_fw_monitor(state, (struct dibDemodMonitor *) buf);
        ret = sizeof(struct dibDemodMonitor);
        break;
#ifdef DIBCOM_EXTENDED_MONITORING
    case GENERIC_MONIT_COMPAT_PROFILE:
        if (size > 0)
            dib7000m_fw_channel_profile(state, (struct dibDemodChannelProfile *) buf);
        ret = sizeof(struct dibDemodChannelProfile);
        break;


    case GENERIC_MONIT_SPECTRUM:
        if (size > 0)
            dib7000m_risc_mem_read(state, FE_MM_R_GENERIC_MONITORING, buf, (uint16_t)size);
        dib7000m_risc_mem_read(state, FE_MM_R_GENERIC_MONITORING_SIZE, b, 2);
        ret = b[0] | (b[1] << 8);
        if (ret == 0xffff)
            ret = DIB_RETURN_NOT_SUPPORTED;
        break;
#endif


    case GENERIC_MONIT_SNR:
    case GENERIC_MONIT_SNR_COMBINED:
        if (size > 0) {
            dib7000m_risc_mem_read(state, FE_MM_R_GENERIC_MONITORING, b, 4);
            snr = dib7000m_get_snr((b[0] << 8) | b[1], (b[2] << 8) | b[3]);
        }
        ret = 4;
        break;
    case GENERIC_MONIT_AGC_FREEZE:
        ret = 0;
        break;

    default:
        break;
    }
    DibReleaseLock(&state->platform.risc.mem_mbx_lock);

    /* need unlocked mailbox for the slave's SNR */
   if (type == GENERIC_MONIT_SNR_COMBINED &&
            fe->input != NULL) {
        frontend_generic_monitoring(fe->input, GENERIC_MONIT_SNR_COMBINED, b, 4, 0);
        snr += (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
    }

    switch (type & ~GENERIC_MONIT_REQUEST_SIZE) {
    case GENERIC_MONIT_SNR:
    case GENERIC_MONIT_SNR_COMBINED:
        buf[0] = (uint8_t) (snr >> 24);
        buf[1] = (uint8_t) (snr >> 16);
        buf[2] = (uint8_t) (snr >>  8);
        buf[3] = (uint8_t) (snr      );
        break;

    default:
        break;
    }
    return ret;
}
#endif

static int dib7000m_fw_release(struct dibFrontend *fe)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
    DibMSleep(10); // wait it to handle the last IRQ
    DibFreeLock(&state->platform.risc.mbx_if_lock);
    DibFreeLock(&state->platform.risc.mbx_lock);
    DibFreeLock(&state->platform.risc.mem_lock);
    DibFreeLock(&state->platform.risc.mem_mbx_lock);
    DibDeAllocateLock(&state->platform.risc.mbx_if_lock);
    DibDeAllocateLock(&state->platform.risc.mbx_lock);
    DibDeAllocateLock(&state->platform.risc.mem_lock);
    DibDeAllocateLock(&state->platform.risc.mem_mbx_lock);
    return dib7000m_release(fe);
}

int dib7000m_fw_pid_filter_ctrl(struct dibFrontend *demod, uint8_t onoff)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(demod->demod_priv);
	uint16_t val = dib7000m_read_word(state, 294 + 1) & 0xffef;
	val |= (onoff & 0x1) << 4;

	dbgpl(&dib7000m_dbg, "PID filter enabled %d", onoff);
	return dib7000m_write_word(state, 294 + 1, val);
}

int dib7000m_fw_pid_filter(struct dibFrontend *demod, uint8_t id, uint16_t pid, uint8_t onoff)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(demod->demod_priv);
	dbgpl(&dib7000m_dbg, "Index %x, PID %d, OnOff %d", id, pid, onoff);
	return dib7000m_write_word(state, 300 + 1 + id, onoff ? (1 << 13) | pid : 0);
}

static int dib7000m_fw_get_signal_status(struct dibFrontend *demod, struct dibDVBSignalStatus *status)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(demod->demod_priv);
	uint16_t c[16];

	dib7000m_fw_memmbx_sync(state, FE_SYNC_CHANNEL);
	dib7000m_risc_mem_read(state, FE_MM_R_FE_MONITOR, (uint8_t *) c, sizeof(c));

    status->tps_data_lock   = (c[13] >>  3) & 1;
    status->mpeg_data_lock  = (c[13] >>  6) & 1;
    status->bit_error_rate  = (c[10] << 16) | c[11];
    status->signal_strength_dib = (uint8_t)((100 - ((c[4] * 100) / 65535)));
    return 0;
}

const struct dibDemodInfo dib9000_fw_info = {
	"DiBcom 9000 (FW)", // name
	MODE_DVBT | FE_CAN_FAST_TUNE,  // caps
	32,            // pid_filter_count

	{ // ops
        dib9000_fw_reset,
		dib7000m_fw_wakeup, // init
		dib7000m_fw_sleep, // sleep

		NULL, // agc_startup
		dib7000m_fw_tune, // tune

		NULL, // autosearch_start
		NULL, // autosearch_irq
		dib7000m_fw_get_channel, // get_channel_desc

		dib7000m_fw_set_diversity_in, // set_diversity_in
		dib7000m_fw_set_output_mode, // set_output_mode
		dib7000m_fw_pid_filter_ctrl, // pid_filter_ctrl
		dib7000m_fw_pid_filter, // pid_filter

		NULL, // set_wdb_ref
		NULL, // set_wdb_ref

		dib7000m_set_gpio, // set_gpio

		dib7000m_fw_get_signal_status, // get_signal_status

		NULL, // ctrl_timf
#ifdef DIBCOM_GENERIC_MONITORING
        dib7000m_fw_generic_monitoring,   // generic_monitoring
#else
		NULL,
#endif
        dib7000m_fw_release, //release
	}
};

int dib9000_firmware_post_pll_init(struct dibFrontend *fe)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
    dib9000_post_pll_init(fe);
    switch (data_bus_host_type( data_bus_client_get_data_bus(demod_get_data_bus_client(state->fe)) )) {
    case DATA_BUS_MMIO_SRAM_LIKE:
        data_bus_client_set_speed( &state->platform.risc.orig_client, 1);
        break;
    case DATA_BUS_I2C:
        data_bus_client_set_speed( &state->platform.risc.orig_client, 2000);
        break;
    }
    return dib9000_fw_init(state);
}

struct dibDataBusHost * dib9000_get_antenna_interface(struct dibFrontend *fe)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
	return &state->chip.d9.tuner_if;
}

struct dibFrontend * dib9000_firmware_register(struct dibFrontend *fe, struct dibDataBusHost *host, uint8_t i2c_addr, struct dib9000_config *cfg)
{
	struct dib7000m_state *state = (struct dib7000m_state *)(MemAlloc(sizeof(struct dib7000m_state)));
	if (state == NULL)
		return NULL;
	DibZeroMemory(state, sizeof(struct dib7000m_state));
	memcpy(&state->chip.d9.cfg, cfg, sizeof(struct dib9000_config));

    frontend_register_demod(fe, &state->info, &dib9000_fw_info, state);
    state->fe = fe;

    state->gpio_dir     = DIB7000M_GPIO_DEFAULT_DIRECTIONS;
    state->gpio_val     = DIB7000M_GPIO_DEFAULT_VALUES;
    state->gpio_pwm_pos = DIB7000M_GPIO_DEFAULT_PWM_POS;
    state->pwm_freq_div = 0;

    DibAllocateLock(&state->platform.risc.mbx_if_lock);
    DibInitLock(&state->platform.risc.mbx_if_lock);
    DibAllocateLock(&state->platform.risc.mbx_lock);
    DibInitLock(&state->platform.risc.mbx_lock);
    DibAllocateLock(&state->platform.risc.mem_lock);
    DibInitLock(&state->platform.risc.mem_lock);
    DibAllocateLock(&state->platform.risc.mem_mbx_lock);
    DibInitLock(&state->platform.risc.mem_mbx_lock);

    dib7000m_select_data_bus_client(fe, host, i2c_addr);

    /* store the original client */
    memcpy(&state->platform.risc.orig_client, demod_get_data_bus_client(fe), sizeof(struct dibDataBusClient));

    /* use the register-access methods to read registers */
    if((data_bus_host_init(&state->platform.risc.fw_host, data_bus_host_type(host),dib7000m_risc_apb_access_pass_thru, state))!=DIB_RETURN_SUCCESS)
		return NULL;
    if((data_bus_client_init( demod_get_data_bus_client(fe), &dib7000m_risc_apb_access_template, &state->platform.risc.fw_host))!=DIB_RETURN_SUCCESS)
		return NULL;

    if((data_bus_host_init(&state->chip.d9.tuner_if, DATA_BUS_DIBCTRL_IF, dib9000_tuner_xfer, state))!=DIB_RETURN_SUCCESS)
		return NULL;
    if((data_bus_host_init(&state->chip.d9.antenna_if, DATA_BUS_0WIRE, dib9000_fw_antenna_xfer, state))!=DIB_RETURN_SUCCESS)
		return NULL;

    if((dibx000_init_i2c_master(&state->i2c_master, DIB7000MC, demod_get_data_bus_client(fe) ))!=DIB_RETURN_SUCCESS)
		return NULL;

	return fe;
}
