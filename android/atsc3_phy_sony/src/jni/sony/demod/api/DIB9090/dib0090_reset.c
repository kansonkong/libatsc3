#include "dibfrontend.h"
#include "dib0090.h"

#include "dib0090_priv.h"

/*
struct dib0090_fw_state
{
    const struct dib0090_config *cfg;
    struct dibTunerInfo info;
};
*/

int dib0090_identify(struct dibFrontend *fe)
{
	struct dib0090_state *state;
	struct dib0090_identity *identity;
	uint16_t v;
	
//    struct dibDataBusClient *client = tuner_get_data_bus_client(fe);
	struct dibDataBusClient *client;
	if ((fe != NULL) && (fe->tuner_info != NULL))
        client = &fe->tuner_info->data_bus_client;
    else
        client = NULL;


    state = (struct dib0090_state *)(fe->tuner_priv);
    identity = &state->identity;

    v = data_bus_client_read16(client, DIG_CFG_RO);
    identity->p1g = 0;
    identity->in_soc = 0;

    dbgpl(&dib0090_dbg, "FE %d : Tuner identification (Version = 0x%04x)", fe->id, v);

#ifdef FIRMWARE_FIREFLY
    /* pll is not locked locked */
    if (!(v & KROSUS_PLL_LOCKED))
        dbgpl(&dib0090_dbg, "Identification : pll is not yet locked");
#endif

    /* without PLL lock info */
    v &= ~KROSUS_PLL_LOCKED;
#ifdef CONFIG_TUNER_DIB0090_DEBUG
    dbgpl(&dib0090_dbg, "Version without PLL lock info = 0x%04x:", v);
#endif

    identity->version = v&0xff;
    identity->product = (v >> 8)&0xf;
#ifdef CONFIG_TUNER_DIB0090_DEBUG
    dbgpl(&dib0090_dbg, "Version read : 0x%04x (version_id = 0x%02x & product_id = 0x%01x)", v, identity->version, identity->product);
#endif

    if(identity->product != KROSUS)
        goto identification_error;

    //From the SOC the version definition has changed

    if ((identity->version & 0x3) == SOC) {
        identity->in_soc = 1;
        switch(identity->version) {
            case SOC_8090_P1G_11R1: dbgpl(&dib0090_dbg, "SOC 8090 P1-G11R1 Has been detected"); identity->p1g = 1; break;
            case SOC_8090_P1G_21R1: dbgpl(&dib0090_dbg, "SOC 8090 P1-G21R1 Has been detected"); identity->p1g = 1; break;
            case SOC_7090_P1G_11R1: dbgpl(&dib0090_dbg, "SOC 7090 P1-G11R1 Has been detected"); identity->p1g = 1; break;
            case SOC_7090_P1G_21R1: dbgpl(&dib0090_dbg, "SOC 7090 P1-G21R1 Has been detected"); identity->p1g = 1; break;
            default :
                goto identification_error;
        }
    } else {
        switch ((identity->version >> 5)&0x7) {
            case MP001: dbgpl(&dib0090_dbg, "MP001 : 9090/8096"); break;
            case MP005: dbgpl(&dib0090_dbg, "MP005 : Single Sband"); break;
            case MP008: dbgpl(&dib0090_dbg, "MP008 : diversity VHF-UHF-LBAND"); break;
            case MP009: dbgpl(&dib0090_dbg, "MP009 : diversity 29098 CBAND-UHF-LBAND-SBAND"); break;
            default :
                goto identification_error;
        }

        switch(identity->version & 0x1f) {
            case P1G_21R2: dbgpl(&dib0090_dbg, "P1G_21R2 detected"); identity->p1g = 1; break;
            case      P1G: dbgpl(&dib0090_dbg, "P1G detected");      identity->p1g = 1; break;
            case  P1D_E_F: dbgpl(&dib0090_dbg, "P1D/E/F detected");  break;
            case      P1C: dbgpl(&dib0090_dbg, "P1C detected");      break;
            case    P1A_B:
#ifdef CONFIG_TUNER_DIB0090_P1B_SUPPORT
                dbgpl(&dib0090_dbg, "P1-A/B detected: driver is deactivated - not available");
                dib0090_p1b_register(fe);
#else
                dbgpl(&dib0090_dbg, "P1-A/B detected: driver is deactivated - not available");
                goto identification_error;
#endif
                break;
            default :
                goto identification_error;
        }
    }
#ifdef CONFIG_TUNER_DIB0090_DEBUG
    dbgpl(&dib0090_dbg, "state->identity->version = 0x%04x", state->identity.version);
    dbgpl(&dib0090_dbg, "state->identity->product = 0x%04x", state->identity.product);
    dbgpl(&dib0090_dbg, "state->identity->p1g     = 0x%04x", state->identity.p1g);
    dbgpl(&dib0090_dbg, "state->identity->in_soc  = 0x%04x", state->identity.in_soc);
#endif

    return DIB_RETURN_SUCCESS;

identification_error:
    return DIB_RETURN_ERROR;
}

#define HARD_RESET(state) do {  if (cfg->reset) {  if (cfg->sleep) cfg->sleep(fe, 0); DibMSleep(10);  cfg->reset(fe, 1); DibMSleep(10);  cfg->reset(fe, 0); DibMSleep(10);  }  } while (0)

int dib0090_reset_digital(struct dibFrontend *fe, const struct dib0090_config *cfg)
{
	  uint16_t PllCfg;
    uint16_t v;
    int i;
    //struct dibDataBusClient *client = tuner_get_data_bus_client(fe);
    struct dibDataBusClient *client;

	if ((fe != NULL) && (fe->tuner_info != NULL))
        client = &fe->tuner_info->data_bus_client;
    else
        client = NULL;
    

    HARD_RESET(state);


    if((data_bus_client_write16(client, TUNER_EN, EN_PLL | EN_CRYSTAL))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;
    if((data_bus_client_write16(client, SLEEP_EN, EN_DIGCLK | EN_PLL | EN_CRYSTAL))!=DIB_RETURN_SUCCESS) /* PLL, DIG_CLK and CRYSTAL
    remain */
		return DIB_RETURN_ERROR;
    /* adcClkOutRatio=8->7, release reset */

    if(!cfg->in_soc) {
       if(( data_bus_client_write16(client, DIG_CFG_3, ((cfg->io.adc_clock_ratio-1) << 11) | (0 << 10) | (1 << 9) | (1 << 8) | (cfg->data_tx_drv << 4) | cfg->ls_cfg_pad_drv))!=DIB_RETURN_SUCCESS)
		   return DIB_RETURN_ERROR;

        v = (0 << 15) | ((!cfg->analog_output) << 14) | (1 << 9) | (0 << 8) | (cfg->clkouttobamse << 4) | (0 << 2) | (0);
        if (cfg->clkoutdrive != 0)
            v |= cfg->clkoutdrive << 5;
        else
            v |= 7 << 5;

        /* if chip is in Nautilus 1.1-fix put drive to maximum */
#if 0
        if (cfg->fast_if_drv != 0)
            v |= cfg->fast_if_drv << 10;
        else
#endif
            v |= 2 << 10;
        if((data_bus_client_write16(client, DIG_CFG, v))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
    }

    /* Read Pll current config **/
    PllCfg = data_bus_client_read16(client, PLL_CFG);

    /** Reconfigure PLL if current setting is different from default setting **/
    if ((PllCfg & 0x1FFF) != ((cfg->io.pll_range << 12) | (cfg->io.pll_loopdiv << 6) | (cfg->io.pll_prediv)) && (!cfg->in_soc) && !cfg->io.pll_bypass) {

        /* Set Bypass mode */
        PllCfg |=  (1<<15);
        if((data_bus_client_write16(client, PLL_CFG, PllCfg))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        /* Set Reset Pll */
        PllCfg &=  ~(1<<13);
        if((data_bus_client_write16(client, PLL_CFG, PllCfg))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        /*** Set new Pll configuration in bypass and reset state ***/
        PllCfg =  (1 << 15) | (0 << 13) | (cfg->io.pll_range << 12) | (cfg->io.pll_loopdiv << 6) | (cfg->io.pll_prediv);
        if((data_bus_client_write16(client, PLL_CFG, PllCfg))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

          /* Remove Reset Pll */
        PllCfg |=  (1<<13);
        if((data_bus_client_write16(client, PLL_CFG, PllCfg))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;

        /*** Wait for PLL lock ***/
        i = 100;
        do {
            v = !!(data_bus_client_read16(client, DIG_CFG_RO) & 0x800);
            if (v)
                break;
        } while (--i);

        if (i == 0) {
            dbgpl(&dib0090_dbg, "Pll: Unable to lock Pll");
            return DIB_RETURN_ERROR;
        }

        /* Finally Remove Bypass mode */
        PllCfg &=  ~(1<<15);
        if((data_bus_client_write16(client, PLL_CFG, PllCfg))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
    }

    if(cfg->io.pll_bypass) {
        PllCfg |=  (cfg->io.pll_bypass << 15);
        if((data_bus_client_write16(client, PLL_CFG, PllCfg))!=DIB_RETURN_SUCCESS)
			return DIB_RETURN_ERROR;
    }
	return DIB_RETURN_SUCCESS;
}

#ifdef CONFIG_BUILD_HOST
static int dib0090_fw_reset(struct dibFrontend *fe)
{
/*
	struct dib0090_fw_state *state = (struct dib0090_fw_state *)(fe->tuner_priv);
	dib0090_reset_digital(fe, state->cfg);
*/
	struct dib0090_state *state = (struct dib0090_state *)(fe->tuner_priv);
	if((dib0090_reset_digital(fe, state->config))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

	return dib0090_identify(fe) == 0xff ? DIB_RETURN_ERROR : DIB_RETURN_SUCCESS;
}

static int dib0090_fw_release(struct dibFrontend *tuner)
{
/*
    struct dib0090_fw_state *st = (struct dib0090_fw_state *)(tuner->tuner_priv);
    MemFree(st,sizeof(struct dib0090_fw_state));
*/
    struct dib0090_state *st = (struct dib0090_state *)(tuner->tuner_priv);
    MemFree(st,sizeof(struct dib0090_state));

    return DIB_RETURN_SUCCESS;
}

static const struct dibTunerInfo dib0090_fw_info = {
	INFO_TEXT("DiBcom DiB0090 (Krosus) Firmware"),   // name
	TUNER_CAN_VHF | TUNER_CAN_UHF | TUNER_CAN_LBAND | TUNER_CAN_SBAND, // caps

	{
        dib0090_fw_reset,   // reset
		NULL, // wakeup
		NULL, // sleep

		NULL, // tune_digital

		NULL,
		NULL,

        dib0090_fw_release, // release
	}
};

struct dibFrontend * dib0090_reset_register(struct dibFrontend *fe, struct dibDataBusHost *data, const struct dib0090_config *cfg)
{
	struct dibDataBusClient *client;
/*
	struct dib0090_fw_state *st = (struct dib0090_fw_state *)(MemAlloc(sizeof(struct dib0090_fw_state)));
	if (st == NULL)
		return NULL;
	DibZeroMemory(st,sizeof(struct dib0090_fw_state));
*/
	struct dib0090_state *st = (struct dib0090_state *)(MemAlloc(sizeof(struct dib0090_state)));
	if (st == NULL)
		return NULL;
	DibZeroMemory(st, sizeof(struct dib0090_state));


    frontend_register_tuner(fe, &st->info, &dib0090_fw_info, st);

    //data_bus_client_init(tuner_get_data_bus_client(fe), &data_bus_client_template_8b_16b, data);
    if ((fe != NULL) && (fe->tuner_info != NULL))
        client = &fe->tuner_info->data_bus_client;
    else
        client = NULL;
    data_bus_client_init(client, &data_bus_client_template_8b_16b, data);

    

//    st->cfg = cfg;
	st->config = cfg;

    return fe;
}
#endif
