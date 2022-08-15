#ifndef DEMOD_DIB7000M_H
#define DEMOD_DIB7000M_H

#include "dibx000_common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dibDataBusHost;
struct dibDataBusClient;
struct dibFrontend;
struct dibDemodMonitor;

//! Configuration of one demodulator
/**
 * This struct is used to configure the DiB7000-demod-driver to make it handle an IC on a specific board correctly.
 */
struct dib7000m_config {
/**
 * indicates the demod is running in DVB-T mode
 */
	uint8_t dvbt_mode;

/**
 * indicates if the demod shall output the MPEG2-TS as 188 or 204 bytes
 */
	uint8_t output_mpeg2_in_188_bytes;

/**
 * use hostbus for diversity out or standard diversity out (or nothing in single mode)
 */
	uint8_t hostbus_diversity;

/**
 * tell the demod if its tuner is baseband or not
 */
	uint8_t tuner_is_baseband;

/**
 * use mobile mode
 */
	uint8_t mobile_mode;

/**
 * callback of the demodulator to set up an LNA.
 * @param demod is the demod-context
 * @param agc_global is the value n_agc_global - it is used for the implementation to know if the LNA-status can be changed or not.
 * @return you have to return 1 when the LNA-status has changed otherwise 0 (when 1 is returned the demod restarts its AGC)
 */
	int (*update_lna) (struct dibFrontend *demod, uint16_t agc_global);

	uint8_t agc_config_count;
/**
 * AGC parameters struct dib7000m_agc_config
 */
	const struct dibx000_agc_config *agc;

/**
 * Bandwidth parameters struct dib7000m_bandwidth_parameter
 */
	const struct dibx000_bandwidth_config *bw;

/**
 * Initial GPIO setup
 */
#define DIB7000M_GPIO_DEFAULT_DIRECTIONS 0xffff

/**
 * Initial GPIO directions
 */
	uint16_t gpio_dir;

#define DIB7000M_GPIO_DEFAULT_VALUES     0x0000
/**
 * Initial GPIO values
 */
	uint16_t gpio_val;

#define DIB7000M_GPIO_PWM_POS0(v)        ((v & 0xf) << 12)
#define DIB7000M_GPIO_PWM_POS1(v)        ((v & 0xf) << 8 )
#define DIB7000M_GPIO_PWM_POS2(v)        ((v & 0xf) << 4 )
#define DIB7000M_GPIO_PWM_POS3(v)         (v & 0xf)
#define DIB7000M_GPIO_DEFAULT_PWM_POS    0xffff
/**
 * Initial GPIO PWM Positions
 */
	uint16_t gpio_pwm_pos;

/**
 * set the frequency divider for PWM
 */
    uint16_t pwm_freq_div;

	int16_t tun_max_gain;
	int16_t tun_ref_level;
	int16_t lna_gain_step;

/**
 * The clock input of the demod is coming directly from the quartz.
 */
	uint8_t quartz_direct;

/**
 * if the previous demod (in an Diversity chain) uses a baseband tuner it puts
 * out its clock divided by 2. To let that the "next" demod know, set this to 1
 */
	uint8_t input_clk_is_div_2;

/**
 * a callback called, before and after an AGC restart of the demod,
 * @before indicates if the call is before or after
 */
	int (*agc_control) (struct dibFrontend *, uint8_t before);
	uint16_t diversity_delay;
};

struct dib9000_config {
    uint32_t timing_frequency;
    uint32_t demod_clock_khz;
    uint32_t xtal_clock_khz;
    uint32_t vcxo_timer;
    uint8_t  adc_clock_ratio;

    uint16_t if_drives;

    uint8_t output_mpeg2_in_188_bytes;
    struct dibGPIOFunction gpio_function[2];
    struct dibSubbandSelection subband;

    uint16_t diversity_delay;
	const uint8_t *microcode_B_fe_buffer;
	uint32_t microcode_B_fe_size;
};

#define DEFAULT_DIB7000M_I2C_ADDRESS 18
#define DEFAULT_DIB9000_I2C_ADDRESS  18

//! Attaching one or more DiB7000-demodulators using I2C-bus
/**
 * This function can be used to acquire/attach DiB7000-demodulators connected to an I2C-bus
 * \param fe dibFrontend to register this demod to
 * \param host the DataBusHost (I2C-Adapter) which can be used to reach this demodulator
 * \param i2c_addr I2C of this demodulator on the I2C-Bus
 * \param cfg pointer to a configuration-struct describing the demod-setup. Has to be given by the caller.
 * \return fe on success , NULL otherwise.
 */
extern struct dibFrontend * dib7000m_register(struct dibFrontend *fe, struct dibDataBusHost *host, uint8_t i2c_addr, const struct dib7000m_config *cfg);

//! Do the I2C-enumeration of DiB7000M/DiB9000-demodulator on one I2C bus
/**
 * This function must be used to enumerate DiB7000M/DiB9000-devices on an I2C-bus. This is necessary in a multi-demod-setup (such as diversity).
 * \param host the DataBusHost (I2C-Adapter) which can be used to reach the devices
 * \param no_of_demods the number of the device to enumerate
 * \param default_addr the default I2C address of all devices
 * \param first_addr this will be the address of the first enumerated device, the second one will have first_addr+2, the third one first_addr+4 and so on
 * \return DIB_RETURN_SUCCESS on success, something else otherwise
 */
extern int dib7000m_i2c_enumeration(struct dibDataBusHost *host, int no_of_demods, uint8_t default_addr, uint8_t first_addr);

extern struct dibFrontend * dib9000_register(struct dibFrontend *fe, struct dibDataBusHost *host, uint8_t i2c_addr, struct dib9000_config *cfg);
extern struct dibFrontend * dib9000_firmware_register(struct dibFrontend *fe, struct dibDataBusHost *host, uint8_t i2c_addr, struct dib9000_config *cfg);

extern void dib7000m_close_gate(struct dibFrontend *fe);

extern void dib7000m_fw_set_data_bus_host(struct dibFrontend *fe, struct dibDataBusHost *h);

//! get the struct dibDataBusHost for a specific demod
/**
 * This function gives you access to the different I2C-Interfaces (and methods) of the DiB7000 by returning the appropriate pointer to a struct dibDataBusHost.
 * \param demod specifies the demod to use as I2C-master
 * \param intf specifies the interface
 * \param gating indicates if the I2C-Adapter is just opening the gate instead of using the I2C-master for transmitting the I2C-message. (Works only, if the demod itself is on an I2C-bus).
 * \return reference to the appropriate I2C-Adapter or NULL in case of an error
 */
extern struct dibDataBusHost * dib7000m_get_i2c_master(struct dibFrontend *demod, enum dibx000_i2c_interface intf, int gating);

//! GPIOs of the DiB7000M
/**
 * Controlling the GPIOs of the DiB7000-demodulator.
 * \param demod demodulator data
 * \param num number of GPIO to control (if in doubt do not touch at least GPIO1,2 and GPIO3,4)
 * \param dir GPIO director 0 = OUT, 1 = IN
 * \param val GPIO value 1 or 0
 */
extern int dib7000m_set_gpio(struct dibFrontend *demod, uint8_t num, uint8_t dir, uint8_t val);

//! enable the PWM-modulator on GPIOs
extern int dib7000m_set_pwm_gpio(struct dibFrontend *fe, uint8_t num, uint8_t id, uint8_t freq, uint32_t value);

//! enabled the VBG voltage of the ADC
/**
 * On some boards it is necessary to enabled the VBG voltage of the ADC, while
 * the demod is powered off, to improve reception quality of the Analog-Chip.
 */
extern int dib7000m_enable_vbg_voltage(struct dibFrontend *demod);

//! toggle the diversity configuration between hostbus-diversity and standard-div/sram-div
extern void dib7000m_set_hostbus_diversity(struct dibFrontend *demod, uint8_t onoff);

//! do some post tune updates of internal states to improve portable performance
extern int dib7000m_post_tune_adaptation(struct dibFrontend *demod);

//! read the raw AGC global value
extern uint16_t dib7000m_get_current_agc_global(struct dibFrontend *demod);

extern int dib7000m_get_adc_power(struct dibFrontend *, uint8_t);
extern int dib7000m_get_dc_power(struct dibFrontend *, uint8_t);

extern struct dibDataBusHost * dib9000_get_tuner_interface(struct dibFrontend *);
extern struct dibDataBusHost * dib9000_get_antenna_interface(struct dibFrontend *);

extern uint8_t dib7000m_lmod4(struct dibFrontend *demod);

extern void dib9000_post_pll_init(struct dibFrontend *demod);
extern int dib9000_firmware_post_pll_init(struct dibFrontend *fe);

extern int dib7000m_monitoring(struct dibDataBusClient *, struct dib7000m_config *cfg, struct dibDemodMonitor *m);
extern int dib7000m_rw_test (struct dibFrontend *demod);
extern int dib7000m_sram_test(struct dibFrontend *demod);
extern int dib7000m_bist(struct dibFrontend *demod);

//! Attaching one or more DiB7000-demodulators using I2C-bus
/**
 * This function is only provided for backward compatibility. Please use dib7000m_register.
 */
extern int dib7000m_attach(struct dibDataBusHost *i2c_adap, int no_of_demods, uint8_t default_addr, uint8_t do_i2c_enum, const struct dib7000m_config cfg[], struct dibFrontend *demod[]);

#ifdef __cplusplus
}
#endif

#endif
