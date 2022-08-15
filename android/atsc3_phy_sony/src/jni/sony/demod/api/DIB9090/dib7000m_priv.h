#ifndef DEMOD_DIB7000M_PRIV_H
#define DEMOD_DIB7000M_PRIV_H

#include "dibfrontend.h"

#include "dib7000m.h"
#include "dibx000_i2c_common.h"

struct dib7000m_fe_memory_map {
	uint16_t addr;
	uint16_t size;
};

struct dib7000m_state {
	struct dibDemodInfo info;
    struct dibFrontend *fe;

	struct dibx000_i2c_master i2c_master;

    uint16_t revision;

    uint16_t gpio_dir;
    uint16_t gpio_val;
    uint16_t gpio_pwm_pos;
    uint16_t pwm_freq_div;

    /* offset is 1 in case of the 7000MC/9000 */
    uint8_t reg_offs;

    union { /* common for all chips */
        struct {
            uint16_t wbd_ref;
            uint8_t current_band;

            const struct dibx000_agc_config *current_agc;
            uint32_t timf_default;
            uint32_t timf;
            uint32_t internal_clk;

            uint8_t div_force_off : 1;
            uint8_t div_state : 1;
            uint16_t div_sync_wait;

            uint8_t hostbus_diversity : 1;
            uint8_t output_mpeg2_in_188_bytes : 1;
            uint8_t mobile_mode       : 1;
        } host;

        struct {
            struct dib7000m_fe_memory_map fe_mm[18];
            uint8_t memcmd;

            /* mailbox */
            DIB_LOCK mbx_if_lock;  /* to protect read/write operations */
            DIB_LOCK mbx_lock;     /* to protect the whole mailbox handling */

            DIB_LOCK mem_lock;     /* to protect the memory accesses */
            DIB_LOCK mem_mbx_lock; /* to protect the memory-based mailbox */

#define MBX_MAX_WORDS (256 - 200 - 2)
#define DIB7000M_MSG_CACHE_SIZE 2
            uint16_t message_cache[DIB7000M_MSG_CACHE_SIZE][MBX_MAX_WORDS];
            uint8_t fw_is_running;

            struct dibDataBusClient orig_client;
            struct dibDataBusHost  fw_host;
        } risc;
    } platform;

    union { /* common for all platforms */
        struct {
            struct dibDataBusHost tuner_if;
            struct dibDataBusHost antenna_if;
            struct dib9000_config cfg;
        } d9;

        struct {
            struct dib7000m_config cfg;
        } d7;
    } chip;
};

extern const struct dibDebugObject dib7000m_dbg;

#define dib7000m_read(st, subaddress, buf, len)  data_bus_client_read( demod_get_data_bus_client(st->fe), subaddress, buf, len)
#define dib7000m_write(st, subaddress, buf, len) data_bus_client_write( demod_get_data_bus_client(st->fe), subaddress, buf, len)
#define dib7000m_read_word(st, subaddress)       data_bus_client_read16( demod_get_data_bus_client(st->fe), subaddress)
#define dib7000m_write_word(st, subaddress, v)   data_bus_client_write16( demod_get_data_bus_client(st->fe), subaddress, v)
#define dib7000m_read32(st, address)             data_bus_client_read32(demod_get_data_bus_client(st->fe), address)

#define dib7000m_write8_noinc(st, a, b, l) \
    data_bus_client_write_attr( demod_get_data_bus_client(st->fe), a, DATA_BUS_ACCESS_MODE_8BIT | DATA_BUS_ACCESS_MODE_NO_ADDRESS_INCREMENT, b, l)
#define dib7000m_read8_noinc(st, a, b, l) \
    data_bus_client_read_attr( demod_get_data_bus_client(st->fe), a, DATA_BUS_ACCESS_MODE_8BIT | DATA_BUS_ACCESS_MODE_NO_ADDRESS_INCREMENT, b, l)

#define dib7000m_write16_noinc(st, a, b, l) \
    data_bus_client_write_attr( demod_get_data_bus_client(st->fe), a, DATA_BUS_ACCESS_MODE_16BIT | DATA_BUS_ACCESS_MODE_NO_ADDRESS_INCREMENT, b, l)
#define dib7000m_read16_noinc(st, a, b, l) \
    data_bus_client_read_attr( demod_get_data_bus_client(st->fe), a, DATA_BUS_ACCESS_MODE_16BIT | DATA_BUS_ACCESS_MODE_NO_ADDRESS_INCREMENT, b, l)

/* doing the same things with additional attributes */
#define dib7000m_write16_noinc_attr(st, a, b, l, attr) \
    data_bus_client_write_attr( demod_get_data_bus_client(st->fe), a, DATA_BUS_ACCESS_MODE_16BIT | DATA_BUS_ACCESS_MODE_NO_ADDRESS_INCREMENT | attr, b, l)
#define dib7000m_read16_noinc_attr(st, a, b, l, attr) \
    data_bus_client_read_attr( demod_get_data_bus_client(st->fe), a, DATA_BUS_ACCESS_MODE_16BIT | DATA_BUS_ACCESS_MODE_NO_ADDRESS_INCREMENT | attr, b, l)

#define dib7000m_write_word_attr(st, subaddress, v, attr) data_bus_client_write16_attr( demod_get_data_bus_client(st->fe), subaddress, v, attr)
#define dib7000m_read_word_attr(st, subaddress, attr) data_bus_client_read16_attr( demod_get_data_bus_client(st->fe), subaddress, attr)

/* private use only from host */
enum dib7000m_power_mode {
	DIB7000M_POWER_ALL = 0,

	DIB7000M_POWER_NO,
	DIB7000M_POWER_INTERF_ANALOG_AGC,
	DIB7000M_POWER_COR4_DINTLV_ICIRM_EQUAL_CFROD,
	DIB7000M_POWER_COR4_CRY_ESRAM_MOUT_NUD,
	DIB7000M_POWER_INTERFACE_ONLY,
};
extern int dib7000m_set_power_mode(struct dib7000m_state *, enum dib7000m_power_mode);

extern uint16_t dib7000m_identify(struct dibDataBusClient *);

extern int dib7000m_reset_gpio(struct dib7000m_state *);

extern int dib7000m_set_adc_state(struct dib7000m_state *state, enum dibx000_adc_states);

extern int dib7000m_set_output_mode(struct dib7000m_state *state, int mode);

extern int dib7000m_set_bandwidth(struct dib7000m_state *state, uint32_t bw);

extern const uint16_t dib7000m_defaults_common[];
extern void dib7000m_write_tab(struct dib7000m_state *state, const uint16_t *buf);

extern int dib7000m_select_data_bus_client(struct dibFrontend *fe, struct dibDataBusHost *host, uint8_t i2c_addr);

/* demod operations */
extern int dib7000m_set_diversity_in(struct dibFrontend *demod, int onoff);
extern int dib7000m_autosearch_start(struct dibFrontend *demod, struct dibChannel *ch);
extern int dib7000m_autosearch_is_irq(struct dibFrontend *demod);
extern int dib7000m_pid_filter_ctrl(struct dibFrontend *demod, uint8_t onoff);
extern int dib7000m_pid_filter(struct dibFrontend *demod, uint8_t id, uint16_t pid, uint8_t onoff);
extern int dib7000m_dvb_t_get_channel_description(struct dibFrontend *demod, struct dibChannel *cd);

extern int dib7000m_agc_startup(struct dibFrontend *demod, struct dibChannel *ch);
extern int dib7000m_tune(struct dibFrontend *demod, struct dibChannel *ch);
extern void dib7000m_update_timf(struct dib7000m_state *state);
extern int dib7000m_demod_set_output_mode(struct dibFrontend *demod, int mode);
extern uint32_t dib7000m_ctrl_timf(struct dibFrontend *demod, uint8_t op, uint32_t timf);
extern int dib7000m_release(struct dibFrontend *demod);

/* extended monitoring */
extern int dib7000m_generic_monitoring(struct dibFrontend *fe, uint32_t type, uint8_t *buf, uint32_t size, uint32_t offset);
extern uint32_t dib7000m_get_snr(uint16_t a, uint16_t b);

extern int dib9000_tuner_xfer(struct dibDataBusClient *client, struct dibDataBusAccess *acc);

#endif
