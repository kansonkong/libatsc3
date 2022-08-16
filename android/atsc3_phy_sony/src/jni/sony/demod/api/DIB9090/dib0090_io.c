#include "dibfrontend.h"
#include "dib0090.h"
#include "dib0090_priv.h"

int dib0090_write_reg(struct dib0090_state *st, uint32_t r, uint16_t v)
{
//    data_bus_client_write16(tuner_get_data_bus_client(st->fe), r, v);

	struct dibDataBusClient *client;

    if ((st->fe != NULL) && (st->fe->tuner_info != NULL))
        client = &st->fe->tuner_info->data_bus_client;
    else
        client = NULL;

  if((data_bus_client_write16(client, r, v))!=DIB_RETURN_SUCCESS)
	  return DIB_RETURN_ERROR;

  return DIB_RETURN_SUCCESS;
}

uint16_t dib0090_read_reg(struct dib0090_state *st, uint32_t reg)
{
//    return data_bus_client_read16(tuner_get_data_bus_client(st->fe), reg);
	struct dibDataBusClient *client;

    if ((st->fe != NULL) && (st->fe->tuner_info != NULL))
        client = &st->fe->tuner_info->data_bus_client;
    else
        client = NULL;

	return data_bus_client_read16(client, reg);	
}

#ifdef CONFIG_DIB0090_USE_PWM_AGC
void dib0090_write_regs(struct dib0090_state *state, uint8_t r, const uint16_t *b, uint8_t c)
{
    do {
        dib0090_write_reg(state, r++, *b++);
    } while (--c);
}
#endif
