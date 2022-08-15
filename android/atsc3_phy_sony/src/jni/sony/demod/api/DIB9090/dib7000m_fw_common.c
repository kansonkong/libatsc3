#include "dib7000m_priv.h"
#include "dib7000m_fw_priv.h"

#define DIB_MICROCODE_A_KEY     0x1234
#define DIB_MICROCODE_B_KEY     0x1234

#define REG_MAC_CPUA_CTRL       1024
#define REG_MAC_CPUA_CADDR      1025
#define REG_MAC_CPUA_CDATA      1026

#define REG_MAC_MBXA_OUT        1027
#define REG_MAC_MBXA_IN         1028
#define REG_MAC_MBXA_DATA       1029

#define REG_MAC_CPUA_CKEY       1031

#define REG_MAC_CPUB_CTRL       1040
#define REG_MAC_CPUB_CADDR      1041
#define REG_MAC_CPUB_CDATA      1042

#define REG_MAC_MBXB_OUT        1043
#define REG_MAC_MBXB_IN         1044
#define REG_MAC_MBXB_DATA       1045

#define REG_MAC_CPUB_CKEY       1047

#define REG_RAMIF_MODE          1056
#define REG_RAMIF_BASEH         1057
#define REG_RAMIF_BASEL         1058
#define REG_RAMIF_MAXH          1059
#define REG_RAMIF_MAXL          1060
#define REG_RAMIF_MINH          1061
#define REG_RAMIF_MINL          1062
#define REG_RAMIF_DATA          1063

#define REG_RAMIF_NOJMP         1074
#define REG_RAMIF_JMP           1075
#define REG_RAMIF_IRAMCFG       1076

#define REG_MAC_IRQ_MASK        1224
#define REG_MAC_MEMCFG1         1225
#define REG_MAC_MEMCFG2         1226
#define REG_MAC_RESET           1227

#define REG_MAC_IRQ             1229

#define REG_MAC_APBSW           1237

#define REG_IPCRP_BSEL          1542

#define MASTER_IRQ_CTRL         1792

#define MAC_IRQ      (1 << 1)
#define IRQ_POL_MSK  (1 << 4)

//#define DUMP_MSG

int dib7000m_risc_mem_setup_cmd(struct dib7000m_state *state, uint32_t addr, uint32_t len, uint8_t reading)
{
    uint8_t b[14] = { 0 };
	int ret = 0;

//    dbgp("%d memcmd: %d %d %d\n", state->fe->id, addr, addr+len, len);
//  b[0] = 0 << 7;
    b[1] = 1;

//  b[2] = 0; // REG_RAMIF_BASEH
//  b[3] = 0;
    b[4] = (uint8_t) (addr >> 8); // REG_RAMIF_BASEL
    b[5] = (uint8_t) (addr & 0xff);

//  b[10] = 0;        // REG_RAMIF_MINH
//  b[11] = 0;
    b[12] = (uint8_t) (addr >> 8); // REG_RAMIF_MINL
    b[13] = (uint8_t) (addr & 0xff);

    addr += len;
//  b[6] = 0;      // REG_RAMIF_MAXH
//  b[7] = 0;
    b[8] = (uint8_t) (addr >> 8);    // REG_RAMIF_MAXL
    b[9] = (uint8_t) (addr & 0xff);

    if((dib7000m_write(state, REG_RAMIF_MODE, b, 14))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    if (reading)
        if((dib7000m_write_word(state, REG_RAMIF_MODE, (1 << 15) | 1))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
    state->platform.risc.memcmd = 0xff;//-1; /* if it was called directly reset it - to force a future setup-call to set it */
	return ret;
}

static void dib7000m_risc_mem_setup(struct dib7000m_state *state, uint8_t cmd)
{
    struct dib7000m_fe_memory_map *m = (struct dib7000m_fe_memory_map*)(&state->platform.risc.fe_mm[cmd & 0x7f]);
    /* decide whether we need to "refresh" the memory controller */
    if (state->platform.risc.memcmd == cmd && /* same command */
        !(cmd & 0x80 && m->size < 67)) /* and we do not want to read something with less than 67 bytes looping - working around a bug in the memory controller */
        return;
    dib7000m_risc_mem_setup_cmd(state, m->addr, m->size, cmd & 0x80);
    state->platform.risc.memcmd = cmd;
}

int dib7000m_risc_mem_read_chunks(struct dib7000m_state *state, uint8_t *b, uint32_t len)
{
     return  data_bus_client_read_attr( demod_get_data_bus_client(state->fe),
                REG_RAMIF_DATA, DATA_BUS_ACCESS_MODE_8BIT | DATA_BUS_ACCESS_MODE_NO_ADDRESS_INCREMENT, b, (uint16_t)len);
}

int dib7000m_risc_mem_write_chunks(struct dib7000m_state *state, const uint8_t *b, uint32_t len)
{
    return data_bus_client_write_attr(demod_get_data_bus_client(state->fe), REG_RAMIF_DATA,
        DATA_BUS_ACCESS_MODE_8BIT | DATA_BUS_ACCESS_MODE_NO_ADDRESS_INCREMENT, b, (uint16_t)len);
}

int dib7000m_risc_mem_read(struct dib7000m_state *state, uint8_t cmd, uint8_t *b, uint16_t len)
{
	int ret = DIB_RETURN_SUCCESS;

    if (!state->platform.risc.fw_is_running)
        return DIB_RETURN_SUCCESS;

    DibAcquireLock(&state->platform.risc.mem_lock);
    dib7000m_risc_mem_setup(state, cmd | 0x80);
    if((dib7000m_risc_mem_read_chunks(state, b, len))!=DIB_RETURN_SUCCESS)
		ret = DIB_RETURN_ERROR;
    DibReleaseLock(&state->platform.risc.mem_lock);
	return ret;
}

int dib7000m_risc_mem_write(struct dib7000m_state *state, uint8_t cmd, const uint8_t *b)
{
	struct dib7000m_fe_memory_map *m = &state->platform.risc.fe_mm[cmd];
	int ret = DIB_RETURN_SUCCESS;

    if (!state->platform.risc.fw_is_running)
        return DIB_RETURN_SUCCESS;

    DibAcquireLock(&state->platform.risc.mem_lock);
    dib7000m_risc_mem_setup(state, cmd);
    if((dib7000m_risc_mem_write_chunks(state, b, m->size))!=DIB_RETURN_SUCCESS)
		ret =  DIB_RETURN_ERROR;

    DibReleaseLock(&state->platform.risc.mem_lock);
	return ret;
}

/* downloading a firmware */
static int dib7000m_firmware_download(struct dib7000m_state *state, uint8_t risc_id, uint16_t key, const uint8_t *code, uint32_t len)
{
    uint16_t offs;

    if(risc_id == 1) offs = 16;
    else offs = 0;

    /* config crtl reg */
    if((dib7000m_write_word(state, REG_MAC_CPUA_CTRL + offs, 0x000f))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    
	if((dib7000m_write_word(state, REG_MAC_CPUA_CADDR + offs, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    
	if((dib7000m_write_word(state, REG_MAC_CPUA_CKEY + offs, key))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    
	dbgpl(&dib7000m_dbg, "going to download %dB of microcode", len);
    if(dib7000m_write16_noinc(state,  REG_MAC_CPUA_CDATA + offs, (uint8_t *) code, (uint16_t)len) != DIB_RETURN_SUCCESS) {
            dbgpl(&dib7000m_dbg, "error while downloading microcode for RISC %c", 'A' + risc_id);
            return DIB_RETURN_ERROR;
	}

    dbgpl(&dib7000m_dbg, "Microcode for RISC %c loaded", 'A' + risc_id);

    return DIB_RETURN_SUCCESS;
}

/* functions to handle the mailbox */
static int dib7000m_mbx_host_init(struct dib7000m_state *state, uint8_t risc_id)
{
    uint16_t mbox_offs;
    uint16_t reset_reg;
    uint16_t tries = 1000;

    if(risc_id == 1)
        mbox_offs = 16;
    else
        mbox_offs = 0;

    DibZeroMemory(state->platform.risc.message_cache, sizeof(state->platform.risc.message_cache));

    /* Reset mailbox  */
    if((dib7000m_write_word(state, REG_MAC_MBXA_OUT + mbox_offs, 0x8000))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* Read reset status */
    do {
        reset_reg = dib7000m_read_word(state, REG_MAC_MBXA_OUT + mbox_offs);
        DibUSleep(100);
    } while ((reset_reg & 0x8000) && --tries);

    if (reset_reg & 0x8000) {
        dbgpl(&dib7000m_dbg, "MBX: init ERROR, no response from RISC %c", 'A' + risc_id);
        return DIB_RETURN_ERROR;
    }
    dbgpl(&dib7000m_dbg, "MBX: initialized");
    return DIB_RETURN_SUCCESS;
}

#define MAX_MAILBOX_TRY 100
int dib7000m_mbx_send_attr(struct dib7000m_state *state, uint8_t id, uint16_t *data, uint8_t len, uint16_t attr)
{
    uint8_t ret = DIB_RETURN_SUCCESS, *d, b[2];
    uint16_t tmp;
    uint16_t size;
    uint32_t i;

    if (!state->platform.risc.fw_is_running)
        return DIB_RETURN_ERROR;

    DibAcquireLock(&state->platform.risc.mbx_if_lock);

    tmp = MAX_MAILBOX_TRY;
    do {
        size = dib7000m_read_word_attr(state, REG_MAC_MBXB_OUT, attr) & 0xff;
        if ((size + len + 1) > MBX_MAX_WORDS && --tmp) {
            dbgpl(&dib7000m_dbg, "MBX: RISC mbx full, retrying (%04X, %d + %d/%d)", *data, size, len+1, MBX_MAX_WORDS);
            DibUSleep(100);
        } else
            break;
    } while (1);

    //dbgpl(&dib7000m_dbg, "MBX: size: %d", size);

    if (tmp == 0) {
        ret = (uint8_t)DIB_RETURN_ERROR;
        goto out;
    }

#ifdef DUMP_MSG
    dbgp("--> %02x %d ", id, len + 1);
    for (i = 0; i < len; i++)
        dbgp("%04x %c%c  ", data[i], isprint(data[i]>>8) ? data[i]>>8 : ' ',isprint(data[i] & 0xff) ? data[i] & 0xff : ' ');
    dbgp("\n");
#endif

    /* byte-order conversion - works on big (where it is not necessary) or little endian */
    d = (uint8_t *) data;
    for (i = 0; i < len; i++) {
        tmp = data[i];
        *d++ = tmp >> 8;
        *d++ = tmp & 0xff;
    }

    /* write msg */
    b[0] = id;
    b[1] = len+1;
    if (dib7000m_write16_noinc_attr(state, REG_MAC_MBXB_DATA, b, 2, attr) != DIB_RETURN_SUCCESS ||
        dib7000m_write16_noinc_attr(state, REG_MAC_MBXB_DATA, (uint8_t *) data, len*2, attr) != DIB_RETURN_SUCCESS) {
        ret = (uint8_t)DIB_RETURN_ERROR;
        goto out;
    }

    /* update register nb_mes_in_RX */
    ret = (uint8_t)dib7000m_write_word_attr(state, REG_MAC_MBXB_OUT, 1 << 14, attr);

out:
    DibReleaseLock(&state->platform.risc.mbx_if_lock);

    return ret;
}

static uint8_t dib7000m_mbx_read(struct dib7000m_state *state, uint16_t *data, uint8_t risc_id, uint16_t attr)
{
#ifdef DUMP_MSG
    uint16_t *d = data;
#endif

    uint16_t tmp, i;
    uint8_t size;
    uint8_t mc_base;

    if (!state->platform.risc.fw_is_running)
        return 0;

    DibAcquireLock(&state->platform.risc.mbx_if_lock);

    if (risc_id == 1)
        mc_base = 16;
    else
        mc_base = 0;

    /* Length and type in the first word */
    *data = dib7000m_read_word_attr(state, REG_MAC_MBXA_DATA + mc_base, attr);
    size = *data & 0xff;
    if (size <= MBX_MAX_WORDS) {
        data++;
        size--;       /* Initial word already read */

        if((dib7000m_read16_noinc_attr(state, REG_MAC_MBXA_DATA + mc_base, (uint8_t *) data, size*2, attr))!=DIB_RETURN_SUCCESS)
			return (uint8_t)DIB_RETURN_ERROR;

        /* to word conversion */
        for (i = 0; i < size; i++) {
            tmp = *data;
            *data = (tmp >> 8) | (tmp << 8);
            data++;
        }

#ifdef DUMP_MSG
        dbgp("<-- ");
        for (i = 0; i < size+1; i++)
            dbgp("%04x %c%c  ", d[i], isprint(d[i]>>8) ? d[i]>>8 : ' ',isprint(d[i] & 0xff) ? d[i] & 0xff : ' ');
        dbgp("\n");
#endif
    } else {
        dbgpl(&dib7000m_dbg, "MBX: message is too big for message cache (%d), flushing message", size);
        size--; /* Initial word already read */
        while (size--)
            if((dib7000m_read16_noinc_attr(state, REG_MAC_MBXA_DATA + mc_base, (uint8_t *) data, 2, attr))!=DIB_RETURN_SUCCESS)
				return (uint8_t)DIB_RETURN_ERROR;
    }
    /* Update register nb_mes_in_TX */
    if((dib7000m_write_word_attr(state, REG_MAC_MBXA_IN + mc_base, 1 << 14, attr))!=DIB_RETURN_SUCCESS)
		return (uint8_t)DIB_RETURN_ERROR;

    DibReleaseLock(&state->platform.risc.mbx_if_lock);

    return size + 1;
}

/* incoming message for DEBUG-print */
static int dib7000m_risc_debug_buf(struct dib7000m_state *state, uint16_t *data, uint8_t size)
{
    uint32_t ts = data[1] << 16 | data[0];
    char  *b = (char *) &data[2];

    b[2 * (size - 2) - 1] = '\0'; /* Bullet proof the buffer */
    if(*b == '~') {
        b++;
        dbgp(b);
    } else
        dbgpl(&dib7000m_dbg, "RISC%d: %d.%04d %s", state->fe->id, ts/10000, ts % 10000, *b ? b : "<emtpy>");
    return 1;
}

static int dib7000m_mbx_fetch_to_cache(struct dib7000m_state *state, uint16_t attr)
{
    int i;
    uint8_t size;
    uint16_t *block;
    /* find a free slot */

    for (i = 0; i < DIB7000M_MSG_CACHE_SIZE; i++) {
        block = state->platform.risc.message_cache[i];
        if (*block == 0) {
            size = dib7000m_mbx_read(state, block, 1, attr);
			if(size == ((uint8_t)DIB_RETURN_ERROR))
				return DIB_RETURN_ERROR;

//            dbgpl(&dib7000m_dbg, "MBX: fetched %04x message to cache", *block);

            switch (*block >> 8) {
            case IN_MSG_DEBUG_BUF:
                dib7000m_risc_debug_buf(state, block+1, size); /* debug-messages are going to be printed right away */
                *block = 0; /* free the block */
                break;
#if 0
            case IN_MSG_DATA: /* FE-TRACE */
                dib7000m_risc_data_process(state, block+1, size);
                *block = 0;
                break;
#endif
            default:
                break;
            }

            return 1;
        }
    }
    dbgpl(&dib7000m_dbg, "MBX: no free cache-slot found for new message...");
    return -1;
}

/* Check population of a RISC mailbox */
static uint8_t dib7000m_mbx_count(struct dib7000m_state *state, uint8_t risc_id, uint16_t attr)
{
    if (risc_id == 0)
        return (uint8_t) (dib7000m_read_word_attr(state, REG_MAC_MBXA_IN, attr) >> 10) & 0x1f; /* 5 bit field */
    else
        return (uint8_t) (dib7000m_read_word_attr(state, REG_MAC_MBXB_IN, attr) >> 8) & 0x7f; /* 7 bit field */
}

/* called when there is an IRQ */
static int dib7000m_mbx_process(struct dib7000m_state *state, uint16_t attr)
{
    int ret = 0;
    uint16_t tmp;

    if (!state->platform.risc.fw_is_running)
        return -1;

    DibAcquireLock(&state->platform.risc.mbx_lock);
    if (dib7000m_mbx_count(state, 1, attr)) /* 1=RiscB */
        ret = dib7000m_mbx_fetch_to_cache(state, attr);
		if(ret == DIB_RETURN_ERROR)
			return ret;

    tmp = dib7000m_read_word_attr(state, REG_MAC_IRQ, attr); /* Clear the IRQ */
	if(tmp == 0) return DIB_RETURN_ERROR;
//    if (tmp)
//        dbgpl(&dib7000m_dbg, "cleared IRQ: %x", tmp);

    DibReleaseLock(&state->platform.risc.mbx_lock);
    return ret;
}

int dib7000m_mbx_get_message_attr(struct dib7000m_state *state, uint16_t id, uint16_t *msg, uint8_t *size, uint16_t attr)
{
    uint8_t i;
    uint16_t *block;
    //uint16_t timeout = 3000;
    uint16_t timeout = 30000;

    *msg = 0;
    do {
        DibAcquireLock(&state->platform.risc.mbx_lock);
        /* dib7000m_mbx_get_from_cache(); */

        for (i = 0; i < DIB7000M_MSG_CACHE_SIZE; i++) {
            block = state->platform.risc.message_cache[i];
            if ((*block >> 8) == id) {
                *size = (*block & 0xff) - 1;
                memcpy(msg, block+1, (*size)*2 );
                *block = 0; /* free the block */
                i = 0; /* signal that we found a message */
                break;
            }
        }
        DibReleaseLock(&state->platform.risc.mbx_lock);

        if (i == 0)
            break;

        if (dib7000m_mbx_process(state, attr) == -1) /* try to fetch one message - if any */
            return -1;

    } while (--timeout);

    if (timeout == 0)
        dbgpl(&dib7000m_dbg, "waiting for message %d timed out", id);

    return i == 0;
}

/* message to get versions */
static int dib7000m_risc_check_version(struct dib7000m_state *state)
{
    uint8_t r[4];
    uint8_t size;
    uint16_t fw_version = 0;

    if (dib7000m_mbx_send(state, OUT_MSG_REQ_VERSION, &fw_version, 1) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

    if (dib7000m_mbx_get_message(state, IN_MSG_VERSION, (uint16_t *) r, &size) == -1)
        return DIB_RETURN_ERROR;

    fw_version = (r[0] << 8) | r[1];
    dbgpl(&dib7000m_dbg, "RISC: ver: %d.%02d (IC: %d)", fw_version >> 10, fw_version & 0x3ff, (r[2] << 8) | r[3]);

    if ((fw_version >> 10) != 7)
        return DIB_RETURN_ERROR;

    switch (fw_version & 0x3ff) {
    case 11:
    case 12:
    case 14:
    case 15:
    case 16:
    case 17:
        break;
    default:
        dbgpl(&dib7000m_dbg, "RISC: invalid firmware version");
        return DIB_RETURN_ERROR;
    }

    dbgpl(&dib7000m_dbg, "RISC: valid firmware version");
    return DIB_RETURN_SUCCESS;
}

int dib7000m_irq_poll(struct dibFrontend *fe)
{
    struct dib7000m_state *state = (struct dib7000m_state *)(fe->demod_priv);
    if (state->platform.risc.fw_is_running)
        return dib7000m_mbx_process(state, 0);
    return 0;
}

int dib7000m_fw_boot(struct dib7000m_state *state, const uint8_t *codeA, uint32_t lenA, const uint8_t *codeB, uint32_t lenB)
{
    /* Reconfig pool mac ram */
    if((dib7000m_write_word(state, REG_MAC_MEMCFG1, 0x02))!=DIB_RETURN_SUCCESS)      /* A: 8k C, 4 k D - B: 32k C 6 k D - IRAM 96k */
		return DIB_RETURN_ERROR;
    if((dib7000m_write_word(state, REG_MAC_MEMCFG2, 0x05))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* Toggles IP crypto to Host APB interface. */
    if((dib7000m_write_word(state, REG_IPCRP_BSEL, 1))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* Set jump and no jump in the dma box */
    if((dib7000m_write_word(state, REG_RAMIF_NOJMP, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    if((dib7000m_write_word(state, REG_RAMIF_JMP, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* Set MAC as APB Master. */
    if((dib7000m_write_word(state, REG_MAC_APBSW, 0))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* Reset the RISCs */
    if (codeA != NULL)
	{
        if((dib7000m_write_word(state, REG_MAC_CPUA_CTRL, 2))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}
    else
	{
        if((dib7000m_write_word(state, REG_MAC_CPUA_CTRL, 15))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}
    if (codeB != NULL)
	{
        if((dib7000m_write_word(state, REG_MAC_CPUB_CTRL, 2))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}
    if (codeA != NULL)
	{
        if((dib7000m_firmware_download(state, 0, DIB_MICROCODE_A_KEY, codeA, lenA))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}
    if (codeB != NULL)
	{
        if((dib7000m_firmware_download(state, 1, DIB_MICROCODE_B_KEY, codeB, lenB))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}
    /* Run the RISCs */
    if (codeA != NULL)
    { 
	   if((dib7000m_write_word(state, REG_MAC_CPUA_CTRL, 0))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}		
    if (codeB != NULL)
	{
        if((dib7000m_write_word(state, REG_MAC_CPUB_CTRL, 0))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
	}
    if (codeA != NULL)
	{
        if (dib7000m_mbx_host_init(state, 0) != DIB_RETURN_SUCCESS)
            return DIB_RETURN_ERROR;
	}
    if (codeB != NULL)
	{
        if (dib7000m_mbx_host_init(state, 1) != DIB_RETURN_SUCCESS)
            return DIB_RETURN_ERROR;
	}
    DibMSleep(100);
    state->platform.risc.fw_is_running = 1;

    if (dib7000m_risc_check_version(state) != DIB_RETURN_SUCCESS)
        return DIB_RETURN_ERROR;

    state->platform.risc.memcmd = 0xff;
    return DIB_RETURN_SUCCESS;
}
