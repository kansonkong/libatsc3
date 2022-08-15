#include "dibi2c.h"
#include "dibpmu.h"
#include "dibcommon.h"

#include "dibx000_i2c_common.h"

#include "dibosiris.h"

struct osiris_state {
    struct dibPMU pmu;
    const struct osiris_config *cfg;
    uint16_t device_id;
};

#define OSIRIS_DEFAULT_I2C_ADDRESS 0x1e
//static int OSIRIS_DEFAULT_I2C_ADDRESS = 18;


/** -- Registers Mapping: Array of couples (value, register_index)
    -- Must be ordered by ascending values
    -- Mark the end of the table with value = 0, register_index = index of the last real value
*/
struct register_element {
  uint16_t value;
  uint8_t  reg_idx;
};

/*! LDO TUNER Trim Register Mapping */
static const struct register_element ldo_tuner_trim_table[] = {
  { 1750, 5 },
  { 1800, 4 },
  { 1850, 3 },
  { 1910, 2 },
  { 1970, 1 },
  { 2000, 0 },
  {    0, 0 }
};

/*! LDO SRAM Trim Register Mapping */
static const struct register_element ldo_sram_trim_table[] = {
  { 1750, 3 },
  { 1800, 2 },
  { 1850, 1 },
  { 1800, 0 },
  {    0, 0 }
};

/*! LDO ADC2 Trim Register Mapping */
static const struct register_element ldo_adc2_trim_table[] = {
  { 2700, 0 },
  { 2800, 1 },
  { 2900, 2 },
  { 3000, 3 },
  {    0, 3 }
};

/*! DCDC2V Trim Register Mapping */
static const struct register_element dcdc2v_trim_table[] = {
  { 1950,  7 },
  { 2000,  6 },
  { 2050,  5 },
  { 2100,  4 },
  { 2150,  3 },
  { 2200,  2 },
  { 2250,  1 },
  { 2300,  0 },
  {    0,  0 }
};

/*! LDO ADC1 Trim Register Mapping */
// Table used when ADC1 < 1.4V
static const struct register_element ldo_adc_1v2_trim_table[] = {
  {  970, 7 },
  { 1020, 6 },
  { 1070, 5 },
  { 1120, 4 },
  { 1170, 3 },
  { 1220, 2 },
  { 1270, 1 },
  { 1320, 0 },
  {    0, 0 }
};

// Table used when ADC1 >= 1.4V
static const struct register_element ldo_adc_1v8_trim_table[] = {
  { 1430, 7 },
  { 1500, 6 },
  { 1580, 5 },
  { 1650, 4 },
  { 1720, 3 },
  { 1800, 2 },
  { 1870, 1 },
  { 1950, 0 },
  {    0, 0 }
};

/*! LDO CORE MAC (RISC) Trim Register Mapping */
// Same table than ldo_adc_1v2 -> we will use that one instead

/*! DCDC1V2 Trim Register Mapping */
static const struct register_element dcdc1v2_trim_table[] = {
  {  900, 7 },
  {  950, 6 },
  { 1000, 5 },
  { 1050, 4 },
  { 1100, 3 },
  { 1150, 2 },
  { 1200, 1 },
  { 1250, 0 },
  {    0, 0 },
};

/** Return Osiris register index
*/
static uint8_t osiris_get_register_index(uint16_t value,
					 const struct register_element register_table[])
{
  while ((register_table->value != 0) &&
	 (value > register_table->value) &&
	 (value >= (register_table + 1)->value))
    register_table++;

  return register_table->reg_idx;
}

static uint16_t osiris_read_reg(struct osiris_state *state, uint16_t addr)
{
    uint8_t a[2];
    data_bus_client_read(pmu_get_register_access(&state->pmu), addr, a, 2);
    return (a[0] << 8) | a[1];
}

static int osiris_write_reg(struct osiris_state *state, uint16_t addr, uint16_t val)
{
    uint8_t a[2];
    a[0] = (val  >> 8) & 0xff;
    a[1] = (val      ) & 0xff;
    return data_bus_client_write(pmu_get_register_access(&state->pmu), addr, a, 2);
}

static int osiris_reset(struct dibPMU *pmu)
{
    struct osiris_state *state = (struct osiris_state *)(pmu->priv);
    uint16_t reg;

    state->device_id = osiris_read_reg(state, 0x0c);
    switch (state->device_id) {
    case 0:
        dbgpl(NULL, "Found DiB0170 rev1");
        if ((osiris_read_reg(state, 0xa) & 0xff00) != 0xff00) {
            dbgpl(NULL, "Osiris not found");
            return DIB_RETURN_ERROR;
        }
        dbgpl(NULL, "This version of Osiris is not supported any longer.");
        return DIB_RETURN_ERROR;
        break;

    case 1:
        dbgpl(NULL, "Found DiB0170 rev2");
        break;

    case 2:
        dbgpl(NULL, "Found DiB0190 rev2");
        break;

    default:
        dbgpl(NULL, "Osiris was not found");
        return DIB_RETURN_ERROR;
    }
    dbgpl(NULL, "Osiris detected");

    reg  = osiris_get_register_index(state->cfg->ldo_tuner, ldo_tuner_trim_table) <<  0;
    reg |= osiris_get_register_index(state->cfg->ldo_sram , ldo_sram_trim_table ) <<  3;
    reg |= osiris_get_register_index(state->cfg->ldo_adc2 , ldo_adc2_trim_table ) <<  5;
    reg |= osiris_get_register_index(state->cfg->dcdc2v   , dcdc2v_trim_table   ) <<  7;
    if (state->cfg->ldo_adc < 1400)
      reg |= osiris_get_register_index(state->cfg->ldo_adc, ldo_adc_1v2_trim_table) << 11;
    else {
      reg |= (1 << 14); // Set the OUT1V8ADC1 bit
      reg |= osiris_get_register_index(state->cfg->ldo_adc, ldo_adc_1v8_trim_table) << 11;
    }
    if((osiris_write_reg(state, 0x0001, reg))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    reg = (1 << 8);
    if (state->device_id == 2) {
      // DiB0190
      reg |= (1 << 7);
    }
    // LDO RISC - same register mapping as ADC1 (1.2V setting)
    reg |= osiris_get_register_index(state->cfg->ldo_risc, ldo_adc_1v2_trim_table) << 0;
    reg |= osiris_get_register_index(state->cfg->dcdc1v2 , dcdc1v2_trim_table    ) << 3;
    if((osiris_write_reg(state, 0x0002, reg))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    if((osiris_write_reg(state, 0x0007, 0x20 | state->cfg->boost))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    reg = osiris_read_reg(state, 0) & ~(3 << 4);
    if((osiris_write_reg(state, 0, reg | (state->cfg->enable_ldo_adc << 4)))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    reg = (1 << 10) | (state->cfg->dcdc2_current_sense << 3) | (1 << 2) | (1 << 1);
    reg |= (state->cfg->dcdc2_divider & 0x07) << 7;
    if((osiris_write_reg(state, 0x0008, reg))!=DIB_RETURN_SUCCESS)
		return DIB_RETURN_ERROR;

    /* osiris_write_reg(state, 0x0, 0xf); //Force DCDC off for test */

#if 0
    for (reg = 0; reg < 0xd; reg++)
        dbgp("0x%04x, ", osiris_read_reg(state, reg));
    dbgp("\n");
#endif

    return DIB_RETURN_SUCCESS;
}

static void osiris_release(struct dibPMU *pmu)
{
    struct osiris_state *state = (struct osiris_state *)(pmu->priv);
    MemFree(state, sizeof(*state));
}

static const struct dibPMU osiris_info = {
    INFO_TEXT("DiBcom Osiris PMU"),
    {
        osiris_reset,

        osiris_release
    }
};

struct dibPMU * osiris_create(struct dibDataBusHost *i2c_adap, const struct osiris_config *cfg)
{
    struct osiris_state *state = (struct osiris_state *)(MemAlloc(sizeof(struct osiris_state)));
    if (state == NULL)
        return NULL;
    DibZeroMemory(state, sizeof(struct osiris_state));

    pmu_init(&state->pmu, &osiris_info, state);
    data_bus_client_init(pmu_get_register_access(&state->pmu), &dibx000_i2c_data_bus_client, i2c_adap);
    data_bus_client_set_device_id(pmu_get_register_access(&state->pmu), OSIRIS_DEFAULT_I2C_ADDRESS);
    data_bus_client_set_speed(pmu_get_register_access(&state->pmu), 250);

    state->cfg = cfg;

    return &state->pmu;
}
