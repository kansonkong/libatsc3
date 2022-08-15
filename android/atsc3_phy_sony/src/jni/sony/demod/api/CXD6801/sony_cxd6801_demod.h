/*------------------------------------------------------------------------------
  Copyright 2016-2019 Sony Semiconductor Solutions Corporation

  Last Updated    : 2019/06/28
  Modification ID : 4d12c6351929bcae29649d686be7d406fd4718c8
------------------------------------------------------------------------------*/
/**
 @file    sony_demod.h

          This file provides the common demodulator control interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_DEMOD_H
#define SONY_CXD6801_DEMOD_H

//#define SONY_CXD6801_DEMOD_SUPPORT_DVBT                /**< Driver supports DVBT. */
//#define SONY_CXD6801_DEMOD_SUPPORT_DVBT2               /**< Driver supports DVBT2. */
//#define SONY_CXD6801_DEMOD_SUPPORT_DVBC                /**< Driver supports DVBC(J.83A). */
//#define SONY_CXD6801_DEMOD_SUPPORT_DVBC2               /**< Driver supports DVBC2(J.382). */
//#define SONY_CXD6801_DEMOD_SUPPORT_ISDBT               /**< Driver supports ISDBT. */
//#define SONY_CXD6801_DEMOD_SUPPORT_ISDBC               /**< Driver supports ISDBC(J.83C). */
//#define SONY_CXD6801_DEMOD_SUPPORT_J83B                /**< Driver supports J.83B. */
#define SONY_CXD6801_DEMOD_SUPPORT_ATSC                /**< Driver supports ASTC. */
#define SONY_CXD6801_DEMOD_SUPPORT_ATSC3               /**< Driver supports ATSC 3.0. */




#if    (defined SONY_CXD6801_DEMOD_SUPPORT_DVBT)  || (defined SONY_CXD6801_DEMOD_SUPPORT_DVBT2) \
	|| (defined SONY_CXD6801_DEMOD_SUPPORT_DVBC) || (defined SONY_CXD6801_DEMOD_SUPPORT_DVBC2) \
	|| (defined SONY_CXD6801_DEMOD_SUPPORT_ISDBT) || (defined SONY_CXD6801_DEMOD_SUPPORT_ISDBC) \
	|| (defined SONY_CXD6801_DEMOD_SUPPORT_J83B) || (defined SONY_CXD6801_DEMOD_SUPPORT_ATSC)  \
	|| (defined SONY_CXD6801_DEMOD_SUPPORT_ATSC3)
#define SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE  /**< Enable common terrestial and cable code */
#endif

#if (defined SONY_CXD6801_DEMOD_SUPPORT_DVBS_S2) || (defined SONY_CXD6801_DEMOD_SUPPORT_ISDBS) || (defined SONY_CXD6801_DEMOD_SUPPORT_ISDBS3)
#define SONY_CXD6801_DEMOD_SUPPORT_SAT  /**< Enable common satellite code */
#endif

#if defined SONY_CXD6801_DEMOD_SUPPORT_DVBS_S2
#define SONY_CXD6801_DEMOD_SUPPORT_SAT_DEVICE_CTRL /**< Enable satellite device ctrl code */
#endif

#if defined SONY_CXD6801_DEMOD_SUPPORT_REMOVE_SAT_DEVICE_CTRL
#undef SONY_CXD6801_DEMOD_SUPPORT_SAT_DEVICE_CTRL
#endif

#if (defined SONY_CXD6801_DEMOD_SUPPORT_ISDBS3) || (defined SONY_CXD6801_DEMOD_SUPPORT_DVBC2) || (defined SONY_CXD6801_DEMOD_SUPPORT_ISDBC)
#define SONY_CXD6801_DEMOD_SUPPORT_TLV  /**< Enable TLV output related code */
#endif

#if defined SONY_CXD6801_DEMOD_SUPPORT_REMOVE_TLV
#undef SONY_CXD6801_DEMOD_SUPPORT_TLV
#endif

#if defined SONY_CXD6801_DEMOD_SUPPORT_ATSC3
#define SONY_CXD6801_DEMOD_SUPPORT_ALP  /**< Enable ALP output related code */
#endif

#if defined SONY_CXD6801_DEMOD_SUPPORT_REMOVE_ALP
#undef SONY_CXD6801_DEMOD_SUPPORT_ALP
#endif

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_cxd6801_common.h"
#include "sony_cxd6801_i2c.h"
#include "sony_cxd6801_dtv.h"

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBT
#include "sony_dvbt.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBC
#include "sony_dvbc.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBT2
#include "sony_dvbt2.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBC2
#include "sony_dvbc2.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_DVBS_S2
#include "sony_dvbs.h"
#include "sony_dvbs2.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ISDBT
#include "sony_isdbt.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ISDBC
#include "sony_isdbc.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ISDBS
#include "sony_isdbs.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ISDBS3
#include "sony_isdbs3.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_J83B
#include "sony_j83b.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC
#include "sony_cxd6801_atsc.h"
#endif

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC3
#include "sony_cxd6801_atsc3.h"
#endif

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
/**
 @brief Calculate the demodulator IF Freq setting ::sony_demod_t::iffreqConfig.
        ((IFFREQ/Sampling Freq at Down Converter DSP module) * Down converter's dynamic range + 0.5
*/
#define SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG(iffreq) ((uint32_t)(((iffreq)/48.0)*16777216.0 + 0.5))

/**
 @brief Calculate the demodulator IF Freq setting ::sony_demod_t::iffreqConfig for ATSC 1.0.
        ((IFFREQ/Sampling Freq at Down Converter DSP module) * Down converter's dynamic range + 0.5
*/
#define SONY_CXD6801_DEMOD_ATSC_MAKE_IFFREQ_CONFIG(iffreq) ((uint32_t)(((iffreq)/24.0)*4294967296.0 + 0.5))

#ifndef SONY_CXD6801_DEMOD_MAX_CONFIG_MEMORY_COUNT
#define SONY_CXD6801_DEMOD_MAX_CONFIG_MEMORY_COUNT 100 /**< The maximum number of entries in the configuration memory table */
#endif

/**
 @brief Freeze all registers in the SLV-T device.  This API is used by the monitor functions to ensure multiple separate
        register reads are from the same snapshot

 @note This should not be manually called or additional instances added into the driver unless under specific instruction.
*/
#define CXD6801_SLVT_FreezeReg(pDemod) ((pDemod)->pI2c->WriteOneRegister ((pDemod)->pI2c, (pDemod)->i2cAddressSLVT, 0x01, 0x01))

/**
 @brief Unfreeze all registers in the SLV-T device
*/
#define CXD6801_SLVT_UnFreezeReg(pDemod) ((void)((pDemod)->pI2c->WriteOneRegister ((pDemod)->pI2c, (pDemod)->i2cAddressSLVT, 0x01, 0x00)))

/**
 @brief This macro is used to determine that CXD2856 family IC is used.
*/
#define SONY_CXD6801_DEMOD_CHIP_ID_2856_FAMILY(chipId) ((chipId == SONY_CXD6801_DEMOD_CHIP_ID_CXD2856) || \
	(chipId == SONY_CXD6801_DEMOD_CHIP_ID_CXD2857))

/**
 @brief This macro is used to determine that CXD2878 family IC is used.
*/
#define SONY_CXD6801_DEMOD_CHIP_ID_2878_FAMILY(chipId) ((chipId == SONY_CXD6801_DEMOD_CHIP_ID_CXD2878) || \
	(chipId == SONY_CXD6801_DEMOD_CHIP_ID_CXD2879) || \
	(chipId == SONY_CXD6801_DEMOD_CHIP_ID_CXD6802))

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief The demodulator Chip ID mapping.
*/
typedef enum {
	SONY_CXD6801_DEMOD_CHIP_ID_UNKNOWN = 0,      /**< Unknown */
	SONY_CXD6801_DEMOD_CHIP_ID_CXD2856 = 0x090,  /**< CXD2856 / CXD6800(SiP) */
	SONY_CXD6801_DEMOD_CHIP_ID_CXD2857 = 0x091,  /**< CXD2857 */
	SONY_CXD6801_DEMOD_CHIP_ID_CXD2878 = 0x396,  /**< CXD2878 / CXD6801(SiP) */
	SONY_CXD6801_DEMOD_CHIP_ID_CXD2879 = 0x297,  /**< CXD2879 */
	SONY_CXD6801_DEMOD_CHIP_ID_CXD6802 = 0x197   /**< CXD6802(SiP) */
} sony_cxd6801_demod_chip_id_t;

/**
 @brief Demodulator crystal frequency.
*/
typedef enum {
	SONY_CXD6801_DEMOD_XTAL_16000KHz = 0,       /**< 16 MHz */
	SONY_CXD6801_DEMOD_XTAL_24000KHz = 1,       /**< 24 MHz */
	SONY_CXD6801_DEMOD_XTAL_32000KHz = 2        /**< 32 MHz */
} sony_cxd6801_demod_xtal_t;

/**
 @brief Demodulator software state.
*/
typedef enum {
	SONY_CXD6801_DEMOD_STATE_UNKNOWN,           /**< Unknown. */
	SONY_CXD6801_DEMOD_STATE_SHUTDOWN,          /**< Chip is in Shutdown state. */
	SONY_CXD6801_DEMOD_STATE_SLEEP,             /**< Chip is in Sleep state. */
	SONY_CXD6801_DEMOD_STATE_ACTIVE,            /**< Chip is in Active state. */
	SONY_CXD6801_DEMOD_STATE_INVALID            /**< Invalid, result of an error during a state change. */
} sony_cxd6801_demod_state_t;

/**
 @brief Tuner I2C bus config.
*/
typedef enum {
	SONY_CXD6801_DEMOD_TUNER_I2C_CONFIG_DISABLE,       /**< Tuner I2C disabled. */
	SONY_CXD6801_DEMOD_TUNER_I2C_CONFIG_REPEATER,      /**< I2C repeater. */
	SONY_CXD6801_DEMOD_TUNER_I2C_CONFIG_GATEWAY        /**< I2C gateway. */
} sony_cxd6801_demod_tuner_i2c_config_t;

/**
 @brief Enumeration of terrestrial/cable tuner types used for optimising the
        demodulator configuration.
*/
typedef enum {
	SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_NONSONY,     /**< Non-Sony tuners. */
	SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_SONYSILICON  /**< Sony silicon tuners. */
} sony_cxd6801_demod_tuner_optimize_t;

/**
 @brief Demodulator EWS software sub state for ISDB-T.

        EWS mode is for receiving EWS (Emergency Warning Broadcasting System) information, without TS output.
*/
typedef enum {
	SONY_CXD6801_DEMOD_ISDBT_EWS_STATE_NORMAL,    /**< Normal state.  TS output is enabled */
	SONY_CXD6801_DEMOD_ISDBT_EWS_STATE_EWS        /**< EWS state.  TS output is disabled */
} sony_cxd6801_demod_isdbt_ews_state_t;

/**
 @brief Demodulator EAS software sub state for ATSC 3.0.

        EAS mode is for receiving EAS (Emergency Alert System) information, without ALP output.
*/
typedef enum {
	SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_NORMAL,    /**< Normal state.  ALP output is enabled */
	SONY_CXD6801_DEMOD_ATSC3_EAS_STATE_EAS        /**< EAS state.  ALP output is disabled */
} sony_cxd6801_demod_atsc3_eas_state_t;

/**
 @brief Enumeration of spectrum inversion monitor values.
*/
typedef enum {
	SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_NORMAL = 0,             /**< Spectrum normal sense. */
	SONY_CXD6801_DEMOD_TERR_CABLE_SPECTRUM_INV                     /**< Spectrum inverted. */
} sony_cxd6801_demod_terr_cable_spectrum_sense_t;

/**
 @brief Enumeration of I/Q inversion monitor values.
*/
typedef enum {
	SONY_CXD6801_DEMOD_SAT_IQ_SENSE_NORMAL = 0,   /**< I/Q normal sense. */
	SONY_CXD6801_DEMOD_SAT_IQ_SENSE_INV           /**< I/Q inverted. */
} sony_cxd6801_demod_sat_iq_sense_t;

/**
 @brief Configuration options for the demodulator.
*/
typedef enum {
    /**
     @brief Parallel or serial TS output selection.

            2bit parallel TS output is available for CXD2878 family only,
            and verified on TLV divided into 188byte TS case.

            Value:
            - 0: Serial output.
            - 1: Parallel output (Default).
            - 2: 2 bit parallel output. (CXD2878 family only)
    */
	SONY_CXD6801_DEMOD_CONFIG_PARALLEL_SEL,

    /**
     @brief Serial output pin of TS data.

            Value:
            - 0: Output from TSDATA0
            - 1: Output from TSDATA7 (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_SER_DATA_ON_MSB,

    /**
     @brief Parallel/Serial output bit order on TS data.

            Value (Parallel):
            - 0: MSB TSDATA[0]
            - 1: MSB TSDATA[7] (Default).
            Value (Serial/2bit parallel):
            - 0: LSB first
            - 1: MSB first (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_OUTPUT_SEL_MSB,

    /**
     @brief TS valid active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TSVALID_ACTIVE_HI,

    /**
     @brief TS sync active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TSSYNC_ACTIVE_HI,

    /**
     @brief TS error active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TSERR_ACTIVE_HI,

    /**
     @brief TS clock inversion setting.

            TS data latch timing configuration.

            Value:
            - 0: Falling/Negative edge.
            - 1: Rising/Positive edge (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_LATCH_ON_POSEDGE,

    /**
     @brief Serial TS clock gated on valid TS data or is continuous.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.

            Value:
            - 0: Gated
            - 1: Continuous (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_TSCLK_CONT,

    /**
     @brief Disable/Enable TS clock during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Disable during TS packet gap
            - 2 : Disable during TS parity
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
	SONY_CXD6801_DEMOD_CONFIG_TSCLK_MASK,

    /**
     @brief Disable/Enable TSVALID during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active
            - 1 : Disable during TS packet gap (default)
            - 2 : Disable during TS parity (default)
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
	SONY_CXD6801_DEMOD_CONFIG_TSVALID_MASK,

    /**
     @brief Disable/Enable TSERR during specified TS region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Disable during TS packet gap
            - 2 : Disable during TS parity
            - 4 : Disable during TS payload
            - 8 : Disable during TS header
            - 16: Disable during TS sync
    */
	SONY_CXD6801_DEMOD_CONFIG_TSERR_MASK,

    /**
     @brief Enable or disable the parallel auto TS clock rate (data period).
            Also, allows to set the parallel TS clock rate (data period) manually. \n*
            Note: This is generally NOT required under normal operation. \n
            If DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS), then this configuration will
            have no effect.
            Value:
            - 0:       Disable parallel TS clock manual setting. (Default)
                       TS clock rate is automatic.
            - 1 - 255: Enable parallel TS clock manual setting.
                       TS clock rate will become as follows:
                       - 109.33 / Value [MHz] (terrestrial, cable)
                       - 129.83 / Value [MHz] (satellite)
    */
	SONY_CXD6801_DEMOD_CONFIG_PARALLEL_TSCLK_MANUAL,

    /**
     @brief TS packet gap setting.

            Note: This setting is effective only when DEMOD_CONFIG_PARALLEL_TSCLK_MANUAL = 0.

            Value:
            - 0:       TS packet gap is controlled as short as possible.
            - 1 - 7:   TS packet gap is controlled about following value.
                       188/(2^value - 1) [byte]
                       Default value is 4.
    */
	SONY_CXD6801_DEMOD_CONFIG_TS_PACKET_GAP,

    /**
     @brief This configuration can be used to configure the demodulator to output a TS waveform that is
            backwards compatible with previous generation demodulators (CXD2820 / CXD2834 / CXD2835 / CXD2836).
            This option should not be used unless specifically required to overcome a HW configuration issue.
            This option affects all the DVB standards but not the ISDB standards.

            The demodulator will have the following settings, which will override any prior individual
            configuration:
            - Disable TS packet gap insertion.
            - Parallel TS maximum bit rate of 82MBps
            - Serial TS clock frequency fixed at 82MHz

            Values:
            - 0 : Backwards compatible mode disabled (Default)
            - 1 : Backwards compatible mode enabled
    */
	SONY_CXD6801_DEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE,

    /**
     @brief Writes a value to the PWM output.
            Please note the actual PWM precision.

            0x1000 => DVDD
            0x0000 => GND
    */
	SONY_CXD6801_DEMOD_CONFIG_PWM_VALUE,

    /**
     @brief Configure the driving current for the TS Clk pin.

            - 0 : 2mA
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_TSCLK_CURRENT,

    /**
     @brief Configure the driving current for the TS Sync / TS Valid
            / TS Data pins.

            - 0 : 2mA
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_TS_CURRENT,

    /**
     @brief Configure the driving current for the GPIO 0 pin.

            - 0 : 2mA (Default)
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA
    */
	SONY_CXD6801_DEMOD_CONFIG_GPIO0_CURRENT,

    /**
     @brief Configure the driving current for the GPIO 1 pin.

            - 0 : 2mA (Default)
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA
    */
	SONY_CXD6801_DEMOD_CONFIG_GPIO1_CURRENT,

    /**
     @brief Configure the driving current for the GPIO 2 pin.

            - 0 : 2mA (Default)
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA
    */
	SONY_CXD6801_DEMOD_CONFIG_GPIO2_CURRENT,

    /**
     @brief Configure channel bonding setting.

            CXD2878 family only.

            - 0 : Disabled (Default)
            - 1 : ISDB-C channel bonding (J.183)
            - 2 : ATSC 3.0 channel bonding (Main)
            - 3 : ATSC 3.0 channel bonding (Sub)
    */
	SONY_CXD6801_DEMOD_CONFIG_CHBOND,

    /**
     @brief Configure channel bonding stream input.

            CXD2878 family only.

            bit flags: ( can be bitwise ORed )
            - 0 : Disable (Default)
            - 1 : Enable input 0
            - 2 : Enable input 1 (Only for ISDB-C)
            - 4 : Enable input 2 (Only for ISDB-C)
    */
	SONY_CXD6801_DEMOD_CONFIG_CHBOND_STREAMIN,

    /* ---- For terrestrial and cable ---- */

    /**
     @brief Configure the clock frequency for Serial TS in terrestrial and cable active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when SONY_CXD6801_DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS).

            - 0 : 128.00MHz (ISDB-C channel bonding (J.183) only)
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
            - 3 : 64.00MHz
            - 4 : 48.00MHz
            - 5 : 38.40MHz
    */
	SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ,

    /**
     @brief Configure the clock frequency for 2bit Parallel TS in terrestrial and cable active states.

            CXD2878 family only.
            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when SONY_CXD6801_DEMOD_CONFIG_PARALLEL_SEL = 2 (2bit parallel TS).

            - 0 : Invalid
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
    */
	SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TS_2BIT_PARALLEL_CLK_FREQ,

    /**
     @brief Terrestrial / cable tuner type for demodulator specific optimisations.

            Value:
            - SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_NONSONY (0)     : Non-Sony tuner.
            - SONY_CXD6801_DEMOD_TUNER_OPTIMIZE_SONYSILICON (1) : Sony sillicion tuners. (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_TUNER_OPTIMIZE,

    /**
     @brief IFAGC sense configuration.

            Value:
            - 0: Positive IFAGC.
            - 1: Inverted IFAGC (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_IFAGCNEG,

    /**
     @brief Configure the full-scale range of the ADC input to the IFAGC.

            Value:
            - 0: 1.4Vpp (Default)
            - 1: 1.0Vpp
            - 2: 0.7Vpp
    */
	SONY_CXD6801_DEMOD_CONFIG_IFAGC_ADC_FS,

    /**
     @brief Terrestrial / Cable tuner IF spectrum sense configuration.

     @note  This setting configures the type of TUNER. (The tuner inverts spectrum or not)

            Value:
            - 0: IF spectrum sense is not same as RF. Used for Normal / Ordinary tuners i.e. ASCOT. (Default)
            - 1: IF spectrum sense is same as RF.
    */
	SONY_CXD6801_DEMOD_CONFIG_SPECTRUM_INV,

    /**
     @brief Terrestrial / cable input RF spectrum sense setting for ATSC/ATSC 3.0.

     @note  This setting configures the spectrum of INPUT RF SIGNAL.
            This setting is necessary for ATSC 1.0 because ATSC 1.0 demod does not support auto spectrum inversion.
            And for ATSC 3.0, this setting is necessary because ATSC 3.0 demod do auto spectrum inversion by software.
            This setting will be used to decide first spectrum setting to be tried on ATSC 3.0 acquisition.

            Value:
            - 0: RF spectrum is not inverted. (Default)
            - 1: RF spectrum is inverted.
    */
	SONY_CXD6801_DEMOD_CONFIG_RF_SPECTRUM_INV,

    /**
     @brief Configure the order in which systems are attempted in Blind Tune and
            Scan.  This can be used to optimize scan duration where specific
            details on system split ratio are known about the spectrum.

            Value:
            - 0: DVB-T followed by DVBT2 (default).
            - 1: DVB-T2 followed by DVBT.
    */
	SONY_CXD6801_DEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST,

    /**
     @brief Set the measurement period for Pre-RS BER (DVB-T).

            This is a 5 bit value with a default of 11.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT_BERN_PERIOD,

    /**
     @brief Set the measurement period for Pre-RS BER (DVB-C/ISDB-C/J.83B).

            This is a 5 bit value with a default of 11.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBC_BERN_PERIOD,

    /**
     @brief Set the measurement period for Pre-Viterbi BER (DVB-T).

            This is a 3 bit value with a default of 1.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT_VBER_PERIOD,

    /**
     @brief Set the measurement period for Pre-BCH BER (DVB-T2/C2) and
            Post-BCH FER (DVB-T2/C2).

            This is a 4 bit value with a default of 8.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT2C2_BBER_MES,

    /**
     @brief Set the measurement period for Pre-LDPC BER (DVB-T2/C2).

            This is a 4 bit value with a default of 8.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT2C2_LBER_MES,

    /**
     @brief Set the measurement period for PER (DVB-T).

            This is a 4 bit value with a default of 10.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT_PER_MES,

    /**
     @brief Set the measurement period for PER (DVB-C/ISDB-C/J.83B).

            This is a 5 bit value with a default of 10.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBC_PER_MES,

    /**
     @brief Set the measurement period for PER (DVB-T2/C2).

            This is a 4 bit value with a default of 10.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT2C2_PER_MES,

    /**
     @brief Set the measurement period for Pre-RS and PER (ISDB-T).

            This is a 15 bit value with a default of 512.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBT_BERPER_PERIOD,

    /**
     @brief Set the measurement period for Pre-RS BER, SER, Post-RS WER (ATSC 1.0).

            This is a 24 bit value with a default of 0x0064BC.
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC_RSERR_BKLEN,

    /**
     @brief Set the measurement period for Pre-BCH BER (ATSC 3.0) and
            Post-BCH FER (ATSC 3.0).

            This is a 4 x 4 bit value with a default of 8.

            Bit[15:12] : PLP 0
            Bit[11:8]  : PLP 1
            Bit[7:4]   : PLP 2
            Bit[3:0]   : PLP 3
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_BBER_MES,

    /**
     @brief Set the measurement period for Pre-LDPC BER (ATSC 3.0).

            This is a 4 x 4 bit value with a default of 8.

            Bit[15:12] : PLP 0
            Bit[11:8]  : PLP 1
            Bit[7:4]   : PLP 2
            Bit[3:0]   : PLP 3
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_LBER_MES,

    /**
     @brief Averaged Pre-RS BER period type for DVB-T.

            If data length is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_DVBT_BERN_PERIOD setting.
            If time is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_DVBT_AVEBER_PERIOD_TIME setting.

            Value:
            - 0: Determined by data length. (default)
            - 1: Determined by time.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT_AVEBER_PERIOD_TYPE,

    /**
     @brief Time period (ms) of averaged Pre-RS BER for DVB-T.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT_AVEBER_PERIOD_TIME,

    /**
     @brief Averaged Pre-BCH BER period type for DVB-T2.

            If data length is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_DVBT2C2_BBER_MES setting.
            If time is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_DVBT2_AVEBER_PERIOD_TIME setting.

            Value:
            - 0: Determined by data length. (default)
            - 1: Determined by time.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT2_AVEBER_PERIOD_TYPE,

    /**
     @brief Time period (ms) of averaged Pre-BCH BER for DVB-T2.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBT2_AVEBER_PERIOD_TIME,

    /**
     @brief Averaged Pre-BCH BER period type for ATSC 3.0 PLP 0.

            If data length is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_ATSC3_BBER_MES setting.
            If time is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_0 setting.

            Value:
            - 0: Determined by data length. (default)
            - 1: Determined by time.
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TYPE_0,

    /**
     @brief Time period (ms) of averaged Pre-BCH BER for ATSC 3.0 PLP 0.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_0,

    /**
     @brief Averaged Pre-BCH BER period type for ATSC 3.0 PLP 1.

            If data length is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_ATSC3_BBER_MES setting.
            If time is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_0 setting.

            Value:
            - 0: Determined by data length. (default)
            - 1: Determined by time.
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TYPE_1,

    /**
     @brief Time period (ms) of averaged Pre-BCH BER for ATSC 3.0 PLP 1.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_1,

    /**
     @brief Averaged Pre-BCH BER period type for ATSC 3.0 PLP 2.

            If data length is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_ATSC3_BBER_MES setting.
            If time is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_2 setting.

            Value:
            - 0: Determined by data length. (default)
            - 1: Determined by time.
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TYPE_2,

    /**
     @brief Time period (ms) of averaged Pre-BCH BER for ATSC 3.0 PLP 2.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_2,

    /**
     @brief Averaged Pre-BCH BER period type for ATSC 3.0 PLP 3.

            If data length is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_ATSC3_BBER_MES setting.
            If time is selected, the measurement period is determined by
            ::SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_3 setting.

            Value:
            - 0: Determined by data length. (default)
            - 1: Determined by time.
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TYPE_3,

    /**
     @brief Time period (ms) of averaged Pre-BCH BER for ATSC 3.0 PLP 3.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AVEBER_PERIOD_TIME_3,

    /**
     @brief Time period (ms) of averaged SNR for DVB-T/T2, ATSC 3.0.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
	SONY_CXD6801_DEMOD_CONFIG_AVESNR_PERIOD_TIME,

    /**
     @brief Configure which type of Emergency Warning flags is to be output to GPIO (ISDB-T only).

            The EWS flag can be used to inform the existence of EWS( Emergency Warning Broadcast System)
            or AC EEW (Earthquake Early Warning by AC signal) through the GPIO output.
            The possible settings for this configuration are as follows:

            Value:
            - 0: EWS (Default)
            - 1: AC EEW
            - 2: EWS or AC EEW
    */
	SONY_CXD6801_DEMOD_CONFIG_GPIO_EWS_FLAG,

    /**
     @brief ISDB-C configuration for TSMF header packet NULL replacement.

            It's effective in multiple-TS mode or auto-detect mode.

            Value:
            - 0: Do nothing.
            - 1: TSMF header included packet is replaced to NULL. (PID is only replaced. (0x1FFF)) (Default)
     */
	 SONY_CXD6801_DEMOD_CONFIG_ISDBC_TSMF_HEADER_NULL,

    /**
     @brief ISDB-C configuration for TSVALID in NULL replaced packets.

            It's effective in multiple-TS mode or auto-detect mode.

            Value:
            - 0: Do nothing.
            - 1: TSVALID is low for NULL replaced TS packets. (Default)
                 - TS packets including unselected TSID,
                 - TSMF header packets (if SONY_CXD6801_DEMOD_CONFIG_ISDBC_TSMF_HEADER_NULL = 1)
     */
	 SONY_CXD6801_DEMOD_CONFIG_ISDBC_NULL_REPLACED_TS_TSVALID_LOW,

    /**
     @brief DVB-C2 (J.382) output selection.

            Values:
            - 0 : TS output (Default)
            - 1 : TLV output
    */
	SONY_CXD6801_DEMOD_CONFIG_OUTPUT_DVBC2,

    /**
     @brief ATSC 3.0 output selection.

            Values:
            - 0 : ALP output (Default)
            - 1 : ALP packet divided into 188bytes
            - 2 : BBP (Baseband packet) output
                  (96MHz serial only, cannot be used for channel bonding)
    */
	SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ATSC3,

    /**
     @brief ISDB-C channel bonding (J.183) output selection.

     @note  Channel bonding is available for CXD2878 family only.

            Values:
            - 0 : TS/TLV auto switching (Default)
            - 1 : TS output (TLV packet divided into 188bytes)
            - 2 : TLV output
    */
	SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ISDBC_CHBOND,

    /**
     @brief ATSC 3.0 auto spectrum inversion sequence enable or not.

            Values:
            - 0 : Disable
            - 1 : Enable (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_AUTO_SPECTRUM_INV,

    /**
     @brief ATSC 3.0 CW detection sequence enable or not.

            Values:
            - 0 : Disable
            - 1 : Enable (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_CW_DETECTION,

    /**
     @brief ATSC 3.0 GPIO EAS latch mode setting. (Positive/Negative state)

            Latch mode configuration to set "Positive" or "Negative" for
            all EA Wakeup states.

            bit flags: ( can be bitwise ORed )
            - 0 : No emergency (00)
            - 1 : Setting 1 (01) (Default)
            - 2 : Setting 2 (10) (Default)
            - 4 : Setting 3 (11) (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_GPIO_EAS_PN_STATE,

    /**
     @brief ATSC 3.0 GPIO EAS latch mode setting. (Positive/Negative state transiton)

            Latch mode configuration to configure condition of GPIO Hi
            between "Positive" and "Negative" state.
            Note that GPIO must be Hi if Negative -> Positive transition is occurred.

            bit flags: ( can be bitwise ORed )
            - 0 : GPIO Hi if Positive -> Negative is occurred
            - 1 : GPIO Hi if Positive -> Positive is occurred (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC3_GPIO_EAS_PN_TRANS,

    /**
     @brief ATSC unlock detection is used in tune sequence or not.

            Values:
            - 0 : Disable (Default)
            - 1 : Enable
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC_UNLOCK_DETECTION,

    /**
     @brief ATSC auto signal check OFF setting.

            Auto signal check is done by the demodulator chip to detect
            input signal on/off in short time.

            This setting will be used if the user would like to change
            signal level threshold, and need to call
            ::sony_demod_atsc_monitor_SignalLevelData_ForUnlockOptimization.

            Value:
            - 0: Auto signal check ON. (Default)
            - 1: Auto signal check OFF.
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC_AUTO_SIGNAL_CHECK_OFF,

    /**
     @brief ATSC no signal level threshold setting for unlock detection.

            This threshold value is used for the signal change
            from "signal not exist" to "signal exist".
            This is a 24 bit value.

            The default value is 0x7FFB61.
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC_NO_SIGNAL_THRESH,

    /**
     @brief ATSC/J83B signal level threshold setting for unlock detection.

            This threshold value is used for the signal change
            from "signal exist" to "signal not exist".
            This is a 24 bit value.

            The default value is 0x7C4926.
    */
	SONY_CXD6801_DEMOD_CONFIG_ATSC_SIGNAL_THRESH,

    /* ---- For satellite ---- */

    /**
     @brief Configure the clock frequency for Serial TS in Satellite active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when SONY_CXD6801_DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS).

            - 0 : 128.00MHz
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
            - 3 : 64.00MHz
            - 4 : 48.00MHz
            - 5 : 38.40MHz
    */
	SONY_CXD6801_DEMOD_CONFIG_SAT_TS_SERIAL_CLK_FREQ,

    /**
     @brief Configure the clock frequency for 2bit Parallel TS in Satellite active states.

            CXD2878 family only.
            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when SONY_CXD6801_DEMOD_CONFIG_PARALLEL_SEL = 2 (2bit parallel TS).

            - 0 : Invalid
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
    */
	SONY_CXD6801_DEMOD_CONFIG_SAT_TS_2BIT_PARALLEL_CLK_FREQ,

    /**
     @brief I/Q connection sense inversion between tuner and demod.

            - 0: Normal (Default).
            - 1: Inverted. (I/Q signal input to Q/I pin of demodulator)
    */
	SONY_CXD6801_DEMOD_CONFIG_SAT_TUNER_IQ_SENSE_INV,

    /**
     @brief Single cable switch inverts spectrum or not.

            Value:
            - 0: Not invert type switch.
            - 1: Invert type switch. (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_SAT_SINGLECABLE_SPECTRUM_INV,

    /**
     @brief IFAGC sense configuration for satellite.

            Value:
            - 0: Positive IFAGC. (Default)
            - 1: Negative IFAGC.
    */
	SONY_CXD6801_DEMOD_CONFIG_SAT_IFAGCNEG,

    /**
     @brief Measurement period for Pre-RS BER(DVB-S), PER(DVB-S) and PER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBSS2_BER_PER_MES,

    /**
     @brief Measurement period for Pre-BCH BER(DVB-S2) and Post-BCH FER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBS2_BER_FER_MES,

    /**
     @brief Measurement period for Pre-Viterbi BER(DVB-S).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBS_VBER_MES,

    /**
     @brief Measurement period for Pre-LDPC BER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBS2_LBER_MES,

    /**
     @brief Power spectrum smoothing setting used in sony_integ_dvbs_s2_BlindScan
            and sony_integ_dvbs_s2_TuneSRS.

            The scanning/tuning time will become longer if the user set bigger value.

            Value:
            - 0: Reduce smoothing time from normal
            - 1: Normal (Default)
            - 2: 2 times smoother than normal
            - 3: 4 times smoother than normal
            - 4: 8 times smoother than normal
            - 5: 16 times smoother than normal
            - 6: 32 times smoother than normal
            - 7: 64 times smoother than normal
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBSS2_BLINDSCAN_POWER_SMOOTH,

    /**
     @brief BlindScan algorithm used in sony_integ_dvbs_s2_BlindScan

            Note that this ID is available only if SONY_CXD6801_DEMOD_SUPPORT_DVBS_S2_BLINDSCAN_VER2
            is defined and blind scan version 2 is supported.
            If SONY_CXD6801_DEMOD_SUPPORT_DVBS_S2_BLINDSCAN_VER2 is not defined,
            blind scan version 1 is used.

            Value:
            - 0: BlindScan version 1.
            - 1: BlindScan version 2. (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_DVBSS2_BLINDSCAN_VERSION,

    /**
     @brief Measurement unit for Pre-RS BER(ISDB-S) and and Post-RS PER(ISDB-S).

            Value:
            - 0: Measurement unit is in Super Frames.(Default)
            - 1: Measurement unit is in Slots.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS_BERNUMCONF,

    /**
     @brief Measurement period for Pre-RS BER(ISDB-S) and Post-RS PER(ISDB-S) at transmission mode 1.

            - The period is 2^(value) super frames or slots.
              If SONY_CXD6801_DEMOD_CONFIG_ISDBS_BERNUMCONF
                 = 0 then the measurement period is 2^ super frames.
                 = 1 then the measurement period is 2^ slots.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS_BER_PERIOD1,

    /**
     @brief Measurement period for Pre-RS BER(ISDB-S) and Post-RS PER(ISDB-S)  at transmission mode 2.

            - The period is 2^(value) super frames or slots.
              If SONY_CXD6801_DEMOD_CONFIG_ISDBS_BERNUMCONF
                 = 0 then the measurement period is 2^ super frames.
                 = 1 then the measurement period is 2^ slots.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS_BER_PERIOD2,

    /**
     @brief Measurement period for Pre-RS BER(ISDB-S) at TMCC.

            - The period is 2^(value) super frames.
            - Valid range is 0 <= value <= 7.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS_BER_PERIODT,

    /**
     @brief Measurement period for Pre-LDPC BER(ISDB-S3) at transmission mode 1.

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS3_LBER_MES1,

    /**
     @brief Measurement period for Pre-LDPC BER(ISDB-S3) at transmission mode 2.

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS3_LBER_MES2,

    /**
     @brief Measurement period for Pre-BCH BER(ISDB-S3) and Post-BCH FER(ISDB-S3) at transmission mode 1.

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS3_BER_FER_MES1,

    /**
     @brief Measurement period for Pre-BCH BER(ISDB-S3) and Post-BCH FER(ISDB-S3) at transmission mode 2.

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS3_BER_FER_MES2,

    /**
     @brief ISDB-S3 output selection.

            Values:
            - 0 : TLV output (Default)
            - 1 : TS output
            - 2 : TLV packet divided into 188bytes (CXD2878 family only)
    */
	SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ISDBS3,

    /**
     @brief ISDB-S Low CN monitor hold type selection.

            Values:
            - 0 : Instant (Default)
            - 1 : Hold
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS_LOWCN_HOLD,

    /**
     @brief ISDB-S Low CN monitor thresold (low CN -> not low CN).

            - 13bit value.
            - The default value is 0x00FA. (about 9dB)
              To convert thresold -> CN, please refer table in
              sony_demod_isdbs_monitor_CNR function.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS_LOWCN_THRESH_H,

    /**
     @brief ISDB-S Low CN monitor thresold (not low CN -> low CN).

            - 13bit value.
            - The default value is 0x0134. (about 8dB)
              To convert thresold -> CN, please refer table in
              sony_demod_isdbs_monitor_CNR function.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS_LOWCN_THRESH_L,

    /**
     @brief ISDB-S3 Low CN monitor hold type selection.

            Values:
            - 0 : Instant (Default)
            - 1 : Hold
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS3_LOWCN_HOLD,

    /**
     @brief ISDB-S3 Low CN monitor thresold (low CN -> not low CN).

            - 17bit value.
            - The default value is 0x0086D. (about 12dB)
              To convert thresold -> CN, please refer table in
              sony_demod_isdbs3_monitor_CNR function.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS3_LOWCN_THRESH_H,

    /**
     @brief ISDB-S3 Low CN monitor thresold (not low CN -> low CN).

            - 17bit value.
            - The default value is 0x00A89. (about 11dB)
              To convert thresold -> CN, please refer table in
              sony_demod_isdbs3_monitor_CNR function.
    */
	SONY_CXD6801_DEMOD_CONFIG_ISDBS3_LOWCN_THRESH_L,

    /* ---- For TLV output (ISDB-S3/DVB-C2) ---- */

    /**
     @brief Parallel or serial TLV output selection.

            Value:
            - 0: Serial output.
            - 1: Parallel (8bit) output (Default).
            - 2: Parallel (2bit) output.
    */
	SONY_CXD6801_DEMOD_CONFIG_TLV_PARALLEL_SEL,

    /**
     @brief Serial output pin of TLV data.

            Value:
            - 0: Output from TSDATA0
            - 1: Output from TSDATA7 (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TLV_SER_DATA_ON_MSB,

    /**
     @brief Parallel/Serial output bit order on TLV data.

            Value (Parallel):
            - 0: MSB TSDATA[0]
            - 1: MSB TSDATA[7] (Default).
            Value (Serial/2bit parallel):
            - 0: LSB first
            - 1: MSB first (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TLV_OUTPUT_SEL_MSB,

    /**
     @brief TLV valid active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TLVVALID_ACTIVE_HI,

    /**
     @brief TLV sync active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TLVSYNC_ACTIVE_HI,

    /**
     @brief TLV error active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TLVERR_ACTIVE_HI,

    /**
     @brief TLV clock inversion setting.

            TLV data latch timing configuration.

            Value:
            - 0: Falling/Negative edge.
            - 1: Rising/Positive edge (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_TLV_LATCH_ON_POSEDGE,

    /**
     @brief Serial TLV clock gated on valid TLV data or is continuous.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.

            Value:
            - 0: Gated
            - 1: Continuous (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_TLVCLK_CONT,

    /**
     @brief Disable/Enable TLV clock during specified TLV region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Reserved
            - 2 : Reserved
            - 4 : Disable during 5th - length bytes (TLV payload) + packet gap
            - 8 : Disable during 2nd - 4th bytes (TLV header)
            - 16: Disable during 1st byte (TLV sync)
    */
	SONY_CXD6801_DEMOD_CONFIG_TLVCLK_MASK,

    /**
     @brief Disable/Enable TLVVALID during specified TLV region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active
            - 1 : Reserved (default)
            - 2 : Reserved (default)
            - 4 : Disable during 5th - length bytes (TLV payload) + packet gap
            - 8 : Disable during 2nd - 4th bytes (TLV header)
            - 16: Disable during 1st byte (TLV sync)
    */
	SONY_CXD6801_DEMOD_CONFIG_TLVVALID_MASK,

    /**
     @brief Disable/Enable TLVERR during specified TLV region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Reserved
            - 2 : Reserved
            - 4 : Disable during 5th - length bytes (TLV payload) + packet gap
            - 8 : Disable during 2nd - 4th bytes (TLV header)
            - 16: Disable during 1st byte (TLV sync)
    */
	SONY_CXD6801_DEMOD_CONFIG_TLVERR_MASK,

    /**
     @brief Disable/Enable TLVVALID mask during TLV NULL packets replaced by demodulator.

            Note that the demodulator replaces all TLV errored packet with TLV NULL packet.

            Value:
            - 0: Do nothing (Default)
            - 1: TLVVALID is disabled during TLV error replaced NULL packets
    */
	SONY_CXD6801_DEMOD_CONFIG_TLVVALID_MASK_IN_ERRNULL,

    /**
     @brief Disable/Enable TLV filter during ALL TLV NULL packets.

            CXD2878 family only.
            If this setting is enabled, ALL TLV NULL packets are filtered.
            (TLVSYNC and TLVVALID become low in NULL packets.)

            Value:
            - 0: Do nothing (Default)
            - 1: Enable TLV NULL packet filter
    */
	SONY_CXD6801_DEMOD_CONFIG_TLV_NULL_FILTER,

    /**
     @brief Configure the clock frequency for Serial TLV in terrestrial and cable active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when SONY_CXD6801_DEMOD_CONFIG_TLV_PARALLEL_SEL = 0 (serial TLV).

            - 0 : 128.00MHz
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
            - 3 : 64.00MHz
            - 4 : 48.00MHz
            - 5 : 38.40MHz
    */
	SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TLV_SERIAL_CLK_FREQ,

    /**
     @brief Configure the clock frequency for Serial TLV in Satellite active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when SONY_CXD6801_DEMOD_CONFIG_TLV_PARALLEL_SEL = 0 (serial TLV).

            - 0 : 128.00MHz
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
            - 3 : 64.00MHz
            - 4 : 48.00MHz
            - 5 : 38.40MHz
    */
	SONY_CXD6801_DEMOD_CONFIG_SAT_TLV_SERIAL_CLK_FREQ,

    /**
     @brief Configure the clock frequency for 2bit Parallel TLV in terrestrial and cable active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when SONY_CXD6801_DEMOD_CONFIG_TLV_PARALLEL_SEL = 2 (2bit parallel TLV).

            - 0 : Invalid
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
    */
	SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TLV_2BIT_PARALLEL_CLK_FREQ,

    /**
     @brief Configure the clock frequency for 2bit Parallel TLV in Satellite active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when SONY_CXD6801_DEMOD_CONFIG_TLV_PARALLEL_SEL = 2 (2bit parallel TLV).

            - 0 : Invalid
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
    */
	SONY_CXD6801_DEMOD_CONFIG_SAT_TLV_2BIT_PARALLEL_CLK_FREQ,

    /* ---- For ALP output (ATSC 3.0) ---- */

    /**
     @brief Parallel or serial ALP output selection.

            Value:
            - 0: Serial output.
            - 1: Parallel output (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_ALP_PARALLEL_SEL,

    /**
     @brief Serial output pin of ALP data.

            Value:
            - 0: Output from TSDATA0
            - 1: Output from TSDATA7 (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_ALP_SER_DATA_ON_MSB,

    /**
     @brief Parallel/Serial output bit order on ALP data.

            Value (Parallel):
            - 0: MSB TSDATA[0]
            - 1: MSB TSDATA[7] (Default).
            Value (Serial):
            - 0: LSB first
            - 1: MSB first (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_ALP_OUTPUT_SEL_MSB,

    /**
     @brief Configure ALPSYNC length under ALP serial.

            Only valid when SONY_CXD6801_DEMOD_CONFIG_ALP_PARALLEL_SEL = 0 (serial ALP).

            Value:
            - 0: ALPSYNC for the first byte = 8bit. (Default)
            - 1: ALPSYNC for the first bit.
    */
	SONY_CXD6801_DEMOD_CONFIG_ALP_SYNC_LENGTH,

    /**
     @brief ALP valid active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_ALPVALID_ACTIVE_HI,

    /**
     @brief ALP sync active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_ALPSYNC_ACTIVE_HI,

    /**
     @brief ALP error active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_ALPERR_ACTIVE_HI,

    /**
     @brief ALP clock inversion setting.

            ALP data latch timing configuration.

            Value:
            - 0: Falling/Negative edge.
            - 1: Rising/Positive edge (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_ALP_LATCH_ON_POSEDGE,

    /**
     @brief Serial ALP clock gated on valid ALP data or is continuous.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.

            Value:
            - 0: Gated
            - 1: Continuous (Default)
    */
	SONY_CXD6801_DEMOD_CONFIG_ALPCLK_CONT,

    /**
     @brief Disable/Enable ALP clock during specified ALP region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Reserved
            - 2 : Reserved
            - 4 : Disable during next of header - length bytes (ALP payload) + ALP packet gap
            - 8 : Disable during 1st - header length bytes (ALP header)
            - 16: Reserved
    */
	SONY_CXD6801_DEMOD_CONFIG_ALPCLK_MASK,

    /**
     @brief Disable/Enable ALPVALID during specified ALP region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active
            - 1 : Reserved
            - 2 : Reserved
            - 4 : Disable during next of header - length bytes (ALP payload) + ALP packet gap
            - 8 : Disable during 1st - header length bytes (ALP header)
            - 16: Reserved
    */
	SONY_CXD6801_DEMOD_CONFIG_ALPVALID_MASK,

    /**
     @brief Disable/Enable ALPERR during specified ALP region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Reserved
            - 2 : Reserved
            - 4 : Disable during next of header - length bytes (ALP payload) + ALP packet gap
            - 8 : Disable during 1st - header length bytes (ALP header)
            - 16: Reserved
    */
	SONY_CXD6801_DEMOD_CONFIG_ALPERR_MASK,

    /**
     @brief Configure ALPVALID and ALPCLK during gap.

            Value:
            - 0: ALPCLK is always active and ALPVALID is fixed to low in packet gap. (Default)
            - 1: ALPCLK is disabled in gap in packet and packet gap, and ALPVALID is always fixed to high.
                 Available in parallel or serial data only mode only.
    */
	SONY_CXD6801_DEMOD_CONFIG_ALP_VALIDCLK_IN_GAP,

    /**
     @brief BBP clock inversion setting.

            CXD2878 family only.
            BBP data latch timing configuration.

            Value:
            - 0: Falling/Negative edge.
            - 1: Rising/Positive edge (Default).
    */
	SONY_CXD6801_DEMOD_CONFIG_BBP_LATCH_ON_POSEDGE

} sony_cxd6801_demod_config_id_t;

/**
 @brief Demodulator lock status.
*/
typedef enum {
	SONY_CXD6801_DEMOD_LOCK_RESULT_NOTDETECT, /**< Neither "Lock" or "Unlock" conditions are met, lock status cannot be determined */
	SONY_CXD6801_DEMOD_LOCK_RESULT_LOCKED,    /**< "Lock" condition is found. */
	SONY_CXD6801_DEMOD_LOCK_RESULT_UNLOCKED   /**< No signal was found or the signal was not the required system. */
} sony_cxd6801_demod_lock_result_t;

/**
 @brief GPIO pin IDs
*/
typedef enum {
	SONY_CXD6801_DEMOD_GPIO_PIN_GPIO0,
	SONY_CXD6801_DEMOD_GPIO_PIN_GPIO1,
	SONY_CXD6801_DEMOD_GPIO_PIN_GPIO2,
	SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA0,
	SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA1,
	SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA2,
	SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA3,
	SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA4,
	SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA5,
	SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA6,
	SONY_CXD6801_DEMOD_GPIO_PIN_TSDATA7
} sony_cxd6801_demod_gpio_pin_t;

/**
 @brief Mode select for the multi purpose GPIO pins
*/
typedef enum {
    /** @brief GPIO pin is configured as an output */
	SONY_CXD6801_DEMOD_GPIO_MODE_OUTPUT = 0x00,

    /** @brief GPIO pin is configured as an input */
	SONY_CXD6801_DEMOD_GPIO_MODE_INPUT = 0x01,

    /**
     @brief GPIO pin is configured to output an PWM signal which can be configured using the
            ::sony_demod_SetConfig function with the config ID ::SONY_CXD6801_DEMOD_CONFIG_PWM_VALUE.
    */
	SONY_CXD6801_DEMOD_GPIO_MODE_PWM = 0x03,

    /** @brief GPIO pin is configured for stream (TS/TLV/ALP) output (stream error for GPIO2, stream data for TS data pins) */
	SONY_CXD6801_DEMOD_GPIO_MODE_TS_OUTPUT = 0x04,

    /** @brief GPIO pin is configured to output the FEF timing indicator (DVB-T2 Only) */
	SONY_CXD6801_DEMOD_GPIO_MODE_FEF_PART = 0x05,

    /** @brief GPIO pin is configured to EWS or EAS flag (ISDB-T/S/S3, ISDB-C/ATSC 3.0 (CXD2878 family)) */
	SONY_CXD6801_DEMOD_GPIO_MODE_EWS_FLAG = 0x06,

    /** @brief GPIO pin is configured to UPLINK flag (ISDB-S3 Only) */
	SONY_CXD6801_DEMOD_GPIO_MODE_UPLINK_FLAG = 0x07,

    /** @brief GPIO pin is configured to output DiSEqC Transmit Enable */
	SONY_CXD6801_DEMOD_GPIO_MODE_DISEQC_TX_EN = 0x08,

    /** @brief GPIO pin is configured to output DiSEqC Receive Enable */
	SONY_CXD6801_DEMOD_GPIO_MODE_DISEQC_RX_EN = 0x09
} sony_cxd6801_demod_gpio_mode_t;

/**
 @brief TS/TLV/ALP serial clock frequency options
*/
typedef enum {
	SONY_CXD6801_DEMOD_SERIAL_TS_CLK_HIGH_FULL,   /**< High frequency, full rate */
	SONY_CXD6801_DEMOD_SERIAL_TS_CLK_MID_FULL,    /**< Mid frequency, full rate */
	SONY_CXD6801_DEMOD_SERIAL_TS_CLK_LOW_FULL,    /**< Low frequency, full rate */
	SONY_CXD6801_DEMOD_SERIAL_TS_CLK_HIGH_HALF,   /**< High frequency, half rate */
	SONY_CXD6801_DEMOD_SERIAL_TS_CLK_MID_HALF,    /**< Mid frequency, half rate */
	SONY_CXD6801_DEMOD_SERIAL_TS_CLK_LOW_HALF     /**< Low frequency, half rate */
} sony_cxd6801_demod_serial_ts_clk_t ;

/**
 @brief TS/TLV 2bit parallel clock frequency options
*/
typedef enum {
	SONY_CXD6801_DEMOD_2BIT_PARALLEL_TS_CLK_HIGH,    /**< High frequency */
	SONY_CXD6801_DEMOD_2BIT_PARALLEL_TS_CLK_MID,     /**< Mid frequency */
	SONY_CXD6801_DEMOD_2BIT_PARALLEL_TS_CLK_LOW      /**< Low frequency */
} sony_cxd6801_demod_2bit_parallel_ts_clk_t ;

/**
 @brief DVB-S/S2 BlindScan version.
*/
typedef enum {
	SONY_CXD6801_DEMOD_DVBSS2_BLINDSCAN_VERSION1,   /**< BlindScan version 1 */
	SONY_CXD6801_DEMOD_DVBSS2_BLINDSCAN_VERSION2    /**< BlindScan version 2 */
} sony_cxd6801_demod_dvbss2_blindscan_version_t;

/**
 @brief ISDB-S3 output options.
*/
typedef enum {
	SONY_CXD6801_DEMOD_OUTPUT_ISDBS3_TLV,        /**< TLV output */
	SONY_CXD6801_DEMOD_OUTPUT_ISDBS3_TS,         /**< TS output */
	SONY_CXD6801_DEMOD_OUTPUT_ISDBS3_TLV_DIV_TS  /**< TLV divided into 188byte */
} sony_cxd6801_demod_output_isdbs3_t;

/**
 @brief DVB-C2 (J.382) output options.
*/
typedef enum {
	SONY_CXD6801_DEMOD_OUTPUT_DVBC2_TS,          /**< TS output */
	SONY_CXD6801_DEMOD_OUTPUT_DVBC2_TLV          /**< TLV output */
} sony_cxd6801_demod_output_dvbc2_t;

/**
 @brief ATSC 3.0 output options.
*/
typedef enum {
	SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP,         /**< ALP output */
	SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP_DIV_TS,  /**< ALP divided into 188byte */
	SONY_CXD6801_DEMOD_OUTPUT_ATSC3_BBP          /**< BBP output */
} sony_cxd6801_demod_output_atsc3_t;

/**
 @brief ISDB-C channel bonding (J.183) output options.
*/
typedef enum {
	SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TS_TLV_AUTO, /**< TS/TLV auto switching */
	SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TS,          /**< TS output (TLV divided into 188byte) */
	SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TLV          /**< TLV output */
} sony_cxd6801_demod_output_isdbc_chbond_t;

/**
 @brief ATSC 1.0 internal CPU state.
*/
typedef enum {
	SONY_CXD6801_DEMOD_ATSC_CPU_STATE_IDLE,      /**< CPU is idle. */
	SONY_CXD6801_DEMOD_ATSC_CPU_STATE_BUSY       /**< CPU is running for demodulation. */
} sony_cxd6801_demod_atsc_cpu_state_t;

/**
 @brief Channel bonding state.
*/
typedef enum {
	SONY_CXD6801_DEMOD_CHBOND_CONFIG_DISABLE,     /**< Channel bonding is disabled */
	SONY_CXD6801_DEMOD_CHBOND_CONFIG_ISDBC,       /**< ISDB-C channel bonding (Main) */
	SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_MAIN,  /**< ATSC 3.0 channel bonding (Main) */
	SONY_CXD6801_DEMOD_CHBOND_CONFIG_ATSC3_SUB    /**< ATSC 3.0 channel bonding (Sub) */
} sony_cxd6801_demod_chbond_config_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/**
 @brief List of register values for IF frequency configuration.

        Used for handling tuners that output a different IF depending on the expected channel BW.
        Should be set using ::SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG macro.

        ::sony_demod_SetIFFreqConfig can be used to setup Sony silicon tuner optimized value
        for this struct instance.
*/
typedef struct {
    uint32_t configDVBT_5;              /**< DVB-T 5MHz */
    uint32_t configDVBT_6;              /**< DVB-T 6MHz */
    uint32_t configDVBT_7;              /**< DVB-T 7MHz */
    uint32_t configDVBT_8;              /**< DVB-T 8MHz */
    uint32_t configDVBT2_1_7;           /**< DVB-T2 1.7MHz */
    uint32_t configDVBT2_5;             /**< DVB-T2 5MHz */
    uint32_t configDVBT2_6;             /**< DVB-T2 6MHz */
    uint32_t configDVBT2_7;             /**< DVB-T2 7MHz */
    uint32_t configDVBT2_8;             /**< DVB-T2 8MHz */
    uint32_t configDVBC2_6;             /**< DVB-C2 6MHz */
    uint32_t configDVBC2_8;             /**< DVB-C2 8MHz */
    uint32_t configDVBC_6;              /**< DVB-C  6MHz */
    uint32_t configDVBC_7;              /**< DVB-C  7MHz */
    uint32_t configDVBC_8;              /**< DVB-C  8MHz */
    uint32_t configATSC;                /**< ATSC 1.0 */
    uint32_t configATSC3_6;             /**< ATSC 3.0 6MHz */
    uint32_t configATSC3_7;             /**< ATSC 3.0 7MHz */
    uint32_t configATSC3_8;             /**< ATSC 3.0 8MHz */
    uint32_t configISDBT_6;             /**< ISDB-T 6MHz */
    uint32_t configISDBT_7;             /**< ISDB-T 7MHz */
    uint32_t configISDBT_8;             /**< ISDB-T 8MHz */
    uint32_t configISDBC_6;             /**< ISDB-C 6MHz */
    uint32_t configJ83B_5_06_5_36;      /**< J.83B 5.06/5.36Msps auto selection */
    uint32_t configJ83B_5_60;           /**< J.83B. 5.6Msps */
} sony_cxd6801_demod_iffreq_config_t;

/**
 @brief The demodulator configuration memory table entry. Used to store a register or
        bit modification made through either the ::sony_demod_SetConfig or
        ::sony_demod_SetAndSaveRegisterBits APIs.
*/
typedef struct {
    uint8_t slaveAddress;               /**< Slave address of register */
    uint8_t bank;                       /**< Bank for register */
    uint8_t registerAddress;            /**< Register address */
    uint8_t value;                      /**< Value to write to register */
    uint8_t bitMask;                    /**< Bit mask to apply on the value */
} sony_cxd6801_demod_config_memory_t;

/**
 @brief Demod parameters for ::sony_demod_Create().
*/
typedef struct {
    /**
    @brief The demodulator crystal frequency.
    */
    sony_cxd6801_demod_xtal_t xtalFreq;

    /**
     @brief SLVT I2C address (8-bit form - 8'bxxxxxxx0).
    */
    uint8_t i2cAddressSLVT;

    /**
     @brief Tuner I2C bus setting. (disable/repeater/gateway)
    */
    sony_cxd6801_demod_tuner_i2c_config_t tunerI2cConfig;

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ATSC
    /**
     @brief Disable ATSC 1.0 core if ATSC 1.0 is unnecessary.

     @note  Only for CXD2878 family.
    */
    uint8_t atscCoreDisable;
#endif

} sony_cxd6801_demod_create_param_t;

/**
 @brief The demodulator definition which allows control of the demodulator device
        through the defined set of functions. This portion of the driver is separate
        from the tuner portion and so can be operated independently of the tuner.
*/
typedef struct sony_demod_t {

    /* ======== Following members are NOT cleared by sony_demod_Initialize ======== */

    /**
    @brief The demodulator crystal frequency.
           This is configured by ::sony_demod_create_param_t argument of ::sony_demod_Create.
    */
    sony_cxd6801_demod_xtal_t xtalFreq;

    /**
     @brief SLVT I2C address (8-bit form - 8'bxxxxxxx0). 0xC8 or 0xCA or 0xD8 or 0xDA.
            This is configured by ::sony_demod_create_param_t argument of ::sony_demod_Create.
    */
    uint8_t i2cAddressSLVT;

    /**
     @brief SLVX I2C address (8-bit form - 8'bxxxxxxx0). Fixed to i2cAddressSLVT + 4.
            This is configured by ::sony_demod_create_param_t argument of ::sony_demod_Create.
    */
    uint8_t i2cAddressSLVX;

    /**
     @brief SLVR for ATSC demod I2C address (8-bit form - 8'bxxxxxxx0). Fixed to i2cAddressSLVT - 0x40.
    */
    uint8_t i2cAddressSLVR;

    /**
     @brief SLVM for ATSC demod monitor I2C address (8-bit form - 8'bxxxxxxx0). Fixed to i2cAddressSLVT - 0xA8.
    */
    uint8_t i2cAddressSLVM;

    /**
     @brief I2C API instance.
            This is configured by ::sony_demod_Create.
    */
    sony_cxd6801_i2c_t * pI2c;

    /**
     @brief Tuner I2C bus setting. (disable/repeater/gateway)
            This is configured by ::sony_demod_create_param_t argument of ::sony_demod_Create.
    */
    sony_cxd6801_demod_tuner_i2c_config_t tunerI2cConfig;

    /**
     @brief ATSC 1.0 core disable setting.
            This is configured by ::sony_demod_create_param_t argument of ::sony_demod_Create.
    */
    uint8_t atscCoreDisable;

    /**
     @brief The parallel TS clock Manual setting option for all active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_PARALLEL_TSCLK_MANUAL option.
    */
    uint8_t parallelTSClkManualSetting;

    /**
     @brief The serial TS clock mode for all active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_TSCLK_CONT option.
    */
    uint8_t serialTSClockModeContinuous;

    /**
     @brief TS backwards compatible mode state.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE.
    */
    uint8_t isTSBackwardsCompatibleMode;

    /**
     @brief Channel bonding configuration.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_CHBOND.
    */
    sony_cxd6801_demod_chbond_config_t chbondConfig;

    /**
     @brief Channel bonding stream input setting.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_CHBOND_STREAMIN.
    */
    uint8_t chbondStreamIn;

    /**
     @brief A table of the demodulator configuration changes stored from the
            ::sony_demod_SetConfig and ::sony_demod_SetAndSaveRegisterBits functions.
    */
    sony_cxd6801_demod_config_memory_t configMemory[SONY_CXD6801_DEMOD_MAX_CONFIG_MEMORY_COUNT];

    /**
     @brief The index of the last valid entry in the configMemory table
    */
    uint8_t configMemoryLastEntry;

    /**
     @brief User defined data.
    */
    void * user;

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE

    /* ---- For terrestrial and cable ---- */

    /**
     @brief IF frequency configuration for terrestrial and cable systems.

            This is configured by ::sony_demod_SetIFFreqConfig.
            Use the ::SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG macro for setting each member of
            sony_demod_iffreq_config_t.

    */
    sony_cxd6801_demod_iffreq_config_t iffreqConfig;

    /**
     @brief Stores the terrestrial / cable tuner type for demodulator specific optimisations.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_TUNER_OPTIMIZE option.
    */
    sony_cxd6801_demod_tuner_optimize_t tunerOptimize;

    /**
     @brief The terrestrial / cable tuner IF spectrum sense.

     @note  This setting configures the type of TUNER. (inverts spectrum or not)
            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_SPECTRUM_INV option.
    */
    sony_cxd6801_demod_terr_cable_spectrum_sense_t confSense;

    /**
     @brief Terrestrial / cable input RF spectrum sense setting for ATSC/ATSC 3.0.

     @note  This setting configures the spectrum of INPUT RF SIGNAL.
            This setting is necessary for ATSC 1.0 because ATSC 1.0 demod does not support auto spectrum inversion.
            And for ATSC 3.0, this setting is necessary because ATSC 3.0 demod do auto spectrum inversion by software.
            This setting will be used to decide first spectrum setting to be tried on ATSC 3.0 acquisition.
            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_RF_SPECTRUM_INV option.
    */
    sony_cxd6801_demod_terr_cable_spectrum_sense_t rfSpectrumSense;

    /**
     @brief The serial TS clock frequency option for terrestrial and cable active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ option.
    */
    sony_cxd6801_demod_serial_ts_clk_t serialTSClkFreqTerrCable;

    /**
     @brief The 2bit parallel TS clock frequency option for terrestrial and cable active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TS_2BIT_PARALLEL_CLK_FREQ option.
    */
    sony_cxd6801_demod_2bit_parallel_ts_clk_t twoBitParallelTSClkFreqTerrCable;

    /**
     @brief The order in which Blind Tune attempts acquisition.

            This value can be configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST option.
    */
    uint8_t blindTuneDvbt2First;

    /**
     @brief DVB-C2 (J.382) output selection.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_OUTPUT_DVBC2 option.
            Default value is SONY_CXD6801_DEMOD_OUTPUT_DVBC2_TS.
    */
    sony_cxd6801_demod_output_dvbc2_t dvbc2Output;

    /**
     @brief ATSC 3.0 output selection.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ATSC3 option.
            Default value is SONY_CXD6801_DEMOD_OUTPUT_ATSC3_ALP.
    */
    sony_cxd6801_demod_output_atsc3_t atsc3Output;

    /**
     @brief ISDB-C channel bonding (J.183) output selection.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ISDBC_CHBOND option.
            Default value is SONY_CXD6801_DEMOD_OUTPUT_ISDBC_CHBOND_TS_TLV_AUTO.
    */
    sony_cxd6801_demod_output_isdbc_chbond_t isdbcChBondOutput;

    /**
     @brief ATSC 3.0 auto spectrum inversion sequence is enabled or not.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_ATSC3_AUTO_SPECTRUM_INV.
    */
    uint8_t atsc3AutoSpectrumInv;

    /**
     @brief ATSC 3.0 CW detection sequence is enabled or not.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_ATSC3_CW_DETECTION.
    */
    uint8_t atsc3CWDetection;

    /**
     @brief ATSC unlock detection is used in tune sequence or not.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_ATSC_UNLOCK_DETECTION.
    */
    uint8_t atscUnlockDetection;

    /**
     @brief ATSC auto signal check OFF setting.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_ATSC_AUTO_SIGNAL_CHECK_OFF.
    */
    uint8_t atscAutoSignalCheckOff;

    /**
     @brief ATSC no signal level threshold setting for unlock detection.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_ATSC_NO_SIGNAL_THRESH.
    */
    uint32_t atscNoSignalThresh;

    /**
     @brief ATSC signal level threshold setting for unlock detection.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_ATSC_SIGNAL_THRESH.
    */
    uint32_t atscSignalThresh;

#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT

    /* ---- For satellite ---- */

    /**
     @brief The flag whether current mode is "Single cable" mode or not.
            with sony_integ_sat_device_ctrl_EnterSinglecable and
            sony_integ_sat_device_ctrl_ExitSinglecable.
    */
    uint8_t isSinglecable;

    /**
     @brief The I/Q sense configured on the demodulator with ::sony_demod_SetConfig.
    */
    sony_demod_sat_iq_sense_t satTunerIqSense;

    /**
     @brief Currently used single cable switch inverts spectrum or not.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_SAT_SINGLECABLE_SPECTRUM_INV option.
    */
    uint8_t isSinglecableSpectrumInv;

    /**
     @brief The serial TS clock frequency option for Satellite active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_SAT_TS_SERIAL_CLK_FREQ option.
    */
    sony_demod_serial_ts_clk_t serialTSClkFreqSat;

    /**
     @brief The 2bit parallel TS clock frequency option for Satellite active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_SAT_TS_2BIT_PARALLEL_CLK_FREQ option.
    */
    sony_demod_2bit_parallel_ts_clk_t twoBitParallelTSClkFreqSat;

    /**
     @brief The count of calculation for power spectrum calculation in BlindScan and TuneSRS.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_DVBSS2_BLINDSCAN_POWER_SMOOTH option.
    */
    uint8_t dvbss2PowerSmooth;

    /**
     @brief The BlindScan algorithm for DVB-S/S2.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_DVBSS2_BLINDSCAN_VERSION option.
    */
    sony_demod_dvbss2_blindscan_version_t dvbss2BlindScanVersion;

    /**
     @brief ISDB-S3 output selection.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_OUTPUT_ISDBS3 option.
            Default value is SONY_CXD6801_DEMOD_OUTPUT_ISDBS3_TLV.
    */
    sony_demod_output_isdbs3_t isdbs3Output;

#endif /* SONY_CXD6801_DEMOD_SUPPORT_SAT */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TLV

    /* ---- For TLV output (ISDB-S3/DVB-C2) ---- */

    /**
     @brief The serial TLV clock mode for all active states.

            This is configured using
            ::sony_demod_SetConfig with the SONY_CXD6801_DEMOD_CONFIG_TLVCLK_CONT option.
    */
    uint8_t serialTLVClockModeContinuous;

    /**
     @brief The serial TLV clock frequency option for terrestrial and cable active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TLV_SERIAL_CLK_FREQ option.
    */
    sony_cxd6801_demod_serial_ts_clk_t serialTLVClkFreqTerrCable;

    /**
     @brief The serial TLV clock frequency option for Satellite active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_SAT_TLV_SERIAL_CLK_FREQ option.
    */
    sony_cxd6801_demod_serial_ts_clk_t serialTLVClkFreqSat;

    /**
     @brief The 2bit parallel TLV clock frequency option for terrestrial and cable active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_TERR_CABLE_TLV_2BIT_PARALLEL_CLK_FREQ option.
    */
    sony_cxd6801_demod_2bit_parallel_ts_clk_t twoBitParallelTLVClkFreqTerrCable;

    /**
     @brief The 2bit parallel TLV clock frequency option for Satellite active states.

            This is configured using ::sony_demod_SetConfig with the
            SONY_CXD6801_DEMOD_CONFIG_SAT_TLV_2BIT_PARALLEL_CLK_FREQ option.
    */
    sony_cxd6801_demod_2bit_parallel_ts_clk_t twoBitParallelTLVClkFreqSat;

#endif /* SONY_CXD6801_DEMOD_SUPPORT_TLV */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ALP

    /* ---- For ALP output (ATSC 3.0) ---- */

    /**
     @brief The serial ALP clock mode for all active states.

            This is configured using
            ::sony_demod_SetConfig with the SONY_CXD6801_DEMOD_CONFIG_ALPCLK_CONT option.
    */
    uint8_t serialALPClockModeContinuous;

#endif /* SONY_CXD6801_DEMOD_SUPPORT_ALP */

    /* ======== Following members are cleared by sony_demod_Initialize (Temporary information) ======== */

    /**
    @brief The driver operating state.
    */
    sony_cxd6801_demod_state_t state;

    /**
    @brief The current system.
    */
    sony_cxd6801_dtv_system_t system;

    /**
     @brief Auto detected chip ID at initialisation.
    */
    sony_cxd6801_demod_chip_id_t chipId;

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE

    /* ---- For terrestrial and cable ---- */

    /**
     @brief The current bandwidth, terrestrial and cable systems only.
    */
    sony_cxd6801_dtv_bandwidth_t bandwidth;

    /**
     @brief Scan mode enable/disable.

            Only change this indicator during the SONY_CXD6801_DEMOD_STATE_SLEEP.
    */
    uint8_t scanMode;

    /**
     @brief This flag indicates whether the current ISDB-T sub state is in EWS mode or normal mode.

            This value is only effective when the current system is ISDB-T and it is in
            SONY_CXD6801_DEMOD_STATE_ACTIVE state.
    */
    sony_cxd6801_demod_isdbt_ews_state_t isdbtEwsState;

    /**
     @brief This flag indicates whether the current ATSC 3.0 sub state is in EAS mode or normal mode.

            This value is only effective when the current system is ATSC 3.0 and it is in
            SONY_CXD6801_DEMOD_STATE_ACTIVE state.
    */
    sony_cxd6801_demod_atsc3_eas_state_t atsc3EasState;

    /**
     @brief Flag used in ATSC 3.0 auto spectrum inversion sequence.
    */
    uint8_t atsc3AutoSpectrumInv_flag;

    /**
     @brief Flag used in ATSC 3.0 CW detection sequence.
    */
    uint8_t atsc3CWDetection_flag;

    /**
     @brief ATSC 1.0 internal CPU state.
    */
    sony_cxd6801_demod_atsc_cpu_state_t atscCPUState;

#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_SAT

    /* ---- For satellite ---- */

    /**
     @brief The scan mode for DVB-S/S2.
    */
    uint8_t dvbss2ScanMode;

#endif /* SONY_CXD6801_DEMOD_SUPPORT_SAT */

} sony_cxd6801_demod_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Set up the demodulator.

        This MUST be called before calling ::sony_demod_Initialize.
        This API also clears the demodulator structs members including
        demodulator configuration memory table.

 @param pDemod Reference to memory allocated for the demodulator instance. The create
        function will setup this demodulator instance.
 @param pCreateParam Parameters to create this driver. (I2C slave address, Xtal frequency...)
 @param pDemodI2c The I2C driver that the demod will use as the I2C interface.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_Create(sony_cxd6801_demod_t * pDemod,
                                 sony_cxd6801_demod_create_param_t * pCreateParam,
                                 sony_cxd6801_i2c_t * pDemodI2c);

/**
 @brief Initialize the demodulator, into Sleep state from a power on state.

        Can also be used to reset the demodulator from any state back to
        ::SONY_CXD6801_DEMOD_STATE_SLEEP.  Please note this will reset all SLV-T demodulator registers
        clearing any configuration settings.

 @param pDemod The demodulator instance.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_Initialize(sony_cxd6801_demod_t * pDemod);

/**
 @brief Put the demodulator into Sleep state.
        From this state the demodulator can be directly tuned to any terrestrial/cable/satellite signal,

        If currently in ::SONY_CXD6801_DEMOD_STATE_SHUTDOWN the configuration memory will be loaded
        back into the demodulator.

 @param pDemod The demodulator instance.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_Sleep(sony_cxd6801_demod_t * pDemod);

/**
 @brief Shutdown the demodulator.

        The device is placed in "Shutdown" state.
        ::sony_demod_Sleep must be called to re-initialise the
        device and driver for future acquisitions.

 @param pDemod The demodulator instance.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_Shutdown(sony_cxd6801_demod_t * pDemod);

/**
 @brief Completes the demodulator acquisition setup.
        Must be called after system specific demod and RF tunes.

 @param pDemod The demodulator instance.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_TuneEnd(sony_cxd6801_demod_t * pDemod);

/**
 @brief Soft reset the demodulator.
        The soft reset will begin the devices acquisition process.

 @param pDemod The demod instance.

 @return SONY_CXD6801_RESULT_OK if successfully reset.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SoftReset(sony_cxd6801_demod_t * pDemod);

/**
 @brief Set configuration options on the demodulator.

 @param pDemod The demodulator instance.
 @param configId The configuration ID to set. See ::sony_demod_config_id_t.
 @param value The associated value. Depends on the configId.

 @return SONY_CXD6801_RESULT_OK if successfully set the configuration option.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SetConfig(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_config_id_t configId,
                                    int32_t value);

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Set the IF frequency config to the sony_demod_t.

        Used for handling tuners that output a different IF depending on the expected channel BW.
        Should be set each members using ::SONY_CXD6801_DEMOD_MAKE_IFFREQ_CONFIG macro.

 @param pDemod The demodulator instance.
 @param pIffreqConfig ::sony_demod_iffreq_config_t instance.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SetIFFreqConfig(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_iffreq_config_t * pIffreqConfig);
#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

/**
 @brief Configure the tuner I2C enable.

 @note  Tuner I2C must be enabled if the tuner is connected to tuner I2C bus and
        use I2C repeater to control I2C access to the tuner.
        This function is automatically called in ::sony_demod_Initialize.

 @param pDemod The demodulator instance.
 @param tunerI2cConfig Tuner I2C setting. (disable/repeater/gateway)

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_TunerI2cEnable(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_tuner_i2c_config_t tunerI2cConfig);

/**
 @brief Configure the demodulator to forward I2C messages to the
        output port for tuner control.

 @note  This function has effect only when I2C repeater is used.
        (SONY_CXD6801_DEMOD_TUNER_I2C_REPEATER)

 @param pDemod The demodulator instance.
 @param enable Enable / Disable I2C repeater

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_I2cRepeaterEnable(sony_cxd6801_demod_t * pDemod,
                                            uint8_t enable);

/**
 @brief Setup the GPIO.

 @note  For TSDATA pin setting, this function does NOT check consistency between
        TS pin settings by ::sony_demod_SetConfig function.
        It should be take care by application.

 @param pDemod The demodulator instance.
 @param pin GPIO pin (GPIO0 - 2, TSDATA0 - 7).
 @param enable Set enable (1) or disable (0).
 @param mode GPIO pin mode

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_GPIOSetConfig(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_gpio_pin_t pin,
                                        uint8_t enable,
										sony_cxd6801_demod_gpio_mode_t mode);

/**
 @brief Read the GPIO value.
        The GPIO should have been configured as an input (Read) GPIO.

 @param pDemod The demodulator instance.
 @param pin GPIO pin (GPIO0 - 2, TSDATA0 - 7).
 @param pValue The current value of the GPIO.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_GPIORead(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_gpio_pin_t pin,
                                   uint8_t * pValue);

/**
 @brief Write the GPIO value.
        The GPIO should have been configured as an output (Write) GPIO.

 @param pDemod The demodulator instance.
 @param pin GPIO pin (GPIO0 - 2, TSDATA0 - 7).
 @param value The value to set as output.

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_GPIOWrite(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_gpio_pin_t pin,
                                    uint8_t value);

/**
 @brief Get the Chip ID of the connected demodulator.

 @param pDemod The demodulator instance.
 @param pChipId Pointer to receive the IP ID into.

 @return SONY_CXD6801_RESULT_OK if pChipId is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_ChipID(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_demod_chip_id_t * pChipId);

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Monitor the value of the Internal Digital AGC output.

        In the case of very weak electric field, this value may be useful for SSI adjustment.

 @note  This monitor is available for terrestrial and cable systems except for ATSC.

 @param pDemod The demodulator instance.
 @param pDigitalAGCOut The value of the Internal Digital AGC output.

 @return SONY_CXD6801_RESULT_OK if pDigitalAGCOut is valid.
*/
sony_cxd6801_result_t sony_cxd6801_demod_terr_cable_monitor_InternalDigitalAGCOut(sony_cxd6801_demod_t * pDemod,
                                                                   uint16_t * pDigitalAGCOut);
#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

/**
 @brief Set a specific value with bit mask to any demod register.

        NOTE : This API should only be used under instruction from Sony
        support. Manually modifying any demodulator register could have a negative
        effect for performance or basic functionality.

 @param pDemod The demodulator instance.
 @param slaveAddress Slave address of configuration setting
 @param bank Demodulator bank of configuration setting
 @param registerAddress Register address of configuration setting
 @param value The value being written to this register
 @param bitMask The bit mask used on the register

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SetAndSaveRegisterBits(sony_cxd6801_demod_t * pDemod,
                                                 uint8_t slaveAddress,
                                                 uint8_t bank,
                                                 uint8_t registerAddress,
                                                 uint8_t value,
                                                 uint8_t bitMask);

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Enable / disable scan mode for acquisition in the demodulator.

 @param pDemod The demodulator instance
 @param system The system used for scanning
 @param scanModeEnabled State of scan mode to set

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_terr_cable_SetScanMode(sony_cxd6801_demod_t * pDemod,
	sony_cxd6801_dtv_system_t system,
                                                 uint8_t scanModeEnabled);
#endif /* SONY_CXD6801_DEMOD_SUPPORT_TERR_OR_CABLE */

/**
 @brief Set the TS clock mode and frequency based on the demod struct members.

        Called internally as part of each Sleep to Active
        state transition.

 @param pDemod The demodulator instance
 @param system The tuning system

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SetTSClockModeAndFreq(sony_cxd6801_demod_t * pDemod, sony_cxd6801_dtv_system_t system);

#ifdef SONY_CXD6801_DEMOD_SUPPORT_TLV
/**
 @brief Set the TLV clock mode and frequency based on the demod struct members.

        Called internally as part of each Sleep to Active
        state transition.

 @param pDemod The demodulator instance
 @param system The tuning system

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SetTLVClockModeAndFreq (sony_cxd6801_demod_t * pDemod, sony_cxd6801_dtv_system_t system);
#endif /* SONY_CXD6801_DEMOD_SUPPORT_TLV */

#ifdef SONY_CXD6801_DEMOD_SUPPORT_ALP
/**
 @brief Set the ALP clock mode and frequency based on the demod struct members.

        Called internally as part of each Sleep to Active
        state transition.

 @param pDemod The demodulator instance
 @param system The tuning system

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SetALPClockModeAndFreq (sony_cxd6801_demod_t * pDemod, sony_cxd6801_dtv_system_t system);
#endif /* SONY_CXD6801_DEMOD_SUPPORT_ALP */

/**
 @brief Set TSDATA pin Hi-Z depend on TS output setting.

        Called internally as part of each Sleep to Active
        state transition.

 @param pDemod The demodulator instance
 @param enable Hi-Z enable(1) or disable(0)

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SetTSDataPinHiZ(sony_cxd6801_demod_t * pDemod, uint8_t enable);

/**
 @brief Set TS/TLV/ALP stream output enable/disable setting.

        Called internally as part of state transition.

 @param pDemod The demodulator instance
 @param enable Stream output enable(1) or disable(0)

 @return SONY_CXD6801_RESULT_OK if successful.
*/
sony_cxd6801_result_t sony_cxd6801_demod_SetStreamOutput(sony_cxd6801_demod_t * pDemod, uint8_t enable);

#endif /* SONY_CXD6801_DEMOD_H */
