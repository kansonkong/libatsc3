#ifndef SIP_DIB9090MA_H
#define SIP_DIB9090MA_H

#include "dibcommon.h"

struct dibFrontend;
struct dib0090_wbd_slope;

struct dib9090_config {
	int (*update_lna) (struct dibFrontend *, uint16_t);

	int freq_offset_khz_uhf;
	int freq_offset_khz_vhf;

	uint32_t clock_khz;
	uint8_t  clkouttobamse : 1;

    /*  AGCdrain    AGC drv     AGC slew    I2C drv     I2C slew   IOCLK drv  IOCLK sl   HOST drv   HOST slew  SRAM drv   SRAM slew */
    uint16_t if_drives;

    uint8_t output_mpeg2_in_188_bytes : 1;

    struct dibGPIOFunction gpio_function[2];
    struct dibSubbandSelection subband;
    uint16_t diversity_delay;

    const struct dib0090_wbd_slope *dib0090_wbd_table;

    uint16_t tuner_version;

    uint8_t ls_cfg_pad_drv;
    uint8_t data_tx_drv;

};

struct dib29098_config {
    uint32_t clock_khz;
    uint16_t diversity_delay;
    uint8_t clkout : 1;
    uint32_t mac_freq_khz;
    uint16_t pll_int_loop_filt; // internal loop filt value. If not fill in , default is 8165
    const struct dib0090_wbd_slope *dib0090_wbd_table;
};

struct dib19098_config {
    uint32_t clock_khz;
    uint16_t diversity_delay;
    uint8_t clkout : 1;
    const struct dib0090_wbd_slope *dib0090_wbd_table;
};

struct dibFrontend;
struct dibTunerInterface;
struct dibDataBusHost;
struct dibPMU;

extern struct dibFrontend * dib9090_sip_register(struct dibFrontend *, uint8_t i2c_addr, struct dibDataBusHost *, const struct dib9090_config *cfg);

//! Register one DiB9090-SIP to a Frontend using I2C - which will be driven by a firmware
/**
 * This function can be used to register a DiB9090-SIP connected to an
 * I2C-bus to a frontend. It represents the digital demodulator part
 * and the RF tuner part of a frontend. A SIP is a complete frontend.
 * This function registers a DiB9090 in a way so that it will be
 * controlled by a firmware and the host-driver only sends high level
 * commands to request functionality.
 * \param fe dibFrontend to register this SIP to
 * \param i2c_addr I2C of this demodulator on the I2C-Bus (the I2C address of the tuner is fixed in a SIP)
 * \param bus the DataBusHost (I2C-Adapter) which can be used to reach this demodulator
 * \param cfg pointer to a configuration-struct describing the demod/tuner-setup. Has to be given by the caller.
 * \return fe on success , NULL otherwise.
 */
extern struct dibFrontend * dib9090_firmware_sip_register(struct dibFrontend *fe, uint8_t i2c_addr, struct dibDataBusHost *bus, const struct dib9090_config *cfg);

//! Assign a power-management-unit to this DiB9090 frontend
/**
 * Assigning a PMU to a frontend reflects that this particular
 * frontend is control by a dedicated PMU. This needs to be done to
 * issue the reset of the PMU at the right moment
 * \param fe dibFrontend where the PMU is assigned to
 * \param pmu dibPMU which is assigned to the frontend
 */
extern void dib9090_set_pmu(struct dibFrontend *fe, struct dibPMU *pmu);

extern int dib29098_sip_register(struct dibFrontend *feA, struct dibFrontend *feB, struct dibFrontend *feSH, uint8_t i2c_addr, struct dibDataBusHost *bus, const struct dib29098_config *cfg);
extern int dib29090_register(struct dibFrontend *feA, struct dibFrontend *feB, struct dibFrontend *feSH, uint8_t i2c_addr, struct dibDataBusHost *bus, const struct dib29098_config *cfg);
extern struct dibFrontend * dib19098_sip_register(struct dibFrontend *fe, uint8_t i2c_addr, struct dibDataBusHost *bus, struct dib19098_config *cfg);

#ifdef CONFIG_BUILD_LEON_FE_ONLY
extern void dib29098_fe_fw_task(void);
extern int dib29098_fe_fw_process_msg(uint16_t id, const uint8_t *b, uint32_t size, volatile uint8_t *o);
#endif

struct osiris_config;
struct dibMac;
extern int dib29090_host_pmu_config(struct dibMac *mac, uint8_t i2c_bus_id, const struct osiris_config *config);
extern int dib29090_host_init(struct dibMac *mac, const struct dib29098_config *cfg, const struct dibFeGPIOAction *a, uint8_t n);



#endif
