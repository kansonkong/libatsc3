/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/11/01
  Modification ID : d7aad0326b97a1da232f01f412ed67152f80f3b8
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_demod.h

          This file provides the common demodulator control interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_DEMOD_H
#define sony_cxd_DEMOD_H

#define sony_cxd_DRIVER_BUILD_OPTION_CXD2856

/*------------------------------------------------------------------------------
  Device Defines based on pre-compiler BUILD_OPTION
------------------------------------------------------------------------------*/
#if defined sony_cxd_DRIVER_BUILD_OPTION_CXD2856   /* DVB-T/T2/C/C2/S/S2, ISDB-T/C/S, J.83A/B/C */
#define sony_cxd_DEMOD_SUPPORT_DVBT                /**< Driver supports DVBT. */
#define sony_cxd_DEMOD_SUPPORT_DVBT2               /**< Driver supports DVBT2. */
//#define sony_cxd_DEMOD_SUPPORT_DVBC                /**< Driver supports DVBC(J.83A). */
//#define sony_cxd_DEMOD_SUPPORT_DVBC2               /**< Driver supports DVBC2(J.382). */
//#define sony_cxd_DEMOD_SUPPORT_DVBS_S2             /**< Driver supports DVBS and S2. */
#define sony_cxd_DEMOD_SUPPORT_ISDBT               /**< Driver supports ISDBT. */
//#define sony_cxd_DEMOD_SUPPORT_ISDBC               /**< Driver supports ISDBC(J.83C). */
//#define sony_cxd_DEMOD_SUPPORT_ISDBS               /**< Driver supports ISDBS. */
//#define sony_cxd_DEMOD_SUPPORT_J83B                /**< Driver supports J.83B. */
#elif defined sony_cxd_DRIVER_BUILD_OPTION_CXD2857 /* DVB-T/T2/C/C2/S/S2, ISDB-T/C/S/S3, J.83A/B/C */
#define sony_cxd_DEMOD_SUPPORT_DVBT                /**< Driver supports DVBT. */
#define sony_cxd_DEMOD_SUPPORT_DVBT2               /**< Driver supports DVBT2. */
#define sony_cxd_DEMOD_SUPPORT_DVBC                /**< Driver supports DVBC(J.83A). */
#define sony_cxd_DEMOD_SUPPORT_DVBC2               /**< Driver supports DVBC2(J.382). */
#define sony_cxd_DEMOD_SUPPORT_DVBS_S2             /**< Driver supports DVBS and S2. */
#define sony_cxd_DEMOD_SUPPORT_ISDBT               /**< Driver supports ISDBT. */
#define sony_cxd_DEMOD_SUPPORT_ISDBC               /**< Driver supports ISDBC(J.83C). */
#define sony_cxd_DEMOD_SUPPORT_ISDBS               /**< Driver supports ISDBS. */
#define sony_cxd_DEMOD_SUPPORT_ISDBS3              /**< Driver supports ISDBS3. */
#define sony_cxd_DEMOD_SUPPORT_J83B                /**< Driver supports J.83B. */
#else
#error sony_cxd_DRIVER_BUILD_OPTION value not recognised
#endif

/*------------------------------------------------------------------------------
  sony_cxd_DEMOD_SUPPORT_REMOVE_XXXX can be used to remove unused broadcasting systems.
------------------------------------------------------------------------------*/
#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_DVBT
#undef sony_cxd_DEMOD_SUPPORT_DVBT
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_DVBT2
#undef sony_cxd_DEMOD_SUPPORT_DVBT2
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_DVBC
#undef sony_cxd_DEMOD_SUPPORT_DVBC
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_DVBC2
#undef sony_cxd_DEMOD_SUPPORT_DVBC2
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_DVBS_S2
#undef sony_cxd_DEMOD_SUPPORT_DVBS_S2
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_ISDBT
#undef sony_cxd_DEMOD_SUPPORT_ISDBT
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_ISDBC
#undef sony_cxd_DEMOD_SUPPORT_ISDBC
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_ISDBS
#undef sony_cxd_DEMOD_SUPPORT_ISDBS
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_ISDBS3
#undef sony_cxd_DEMOD_SUPPORT_ISDBS3
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_J83B
#undef sony_cxd_DEMOD_SUPPORT_J83B
#endif

#if    (defined sony_cxd_DEMOD_SUPPORT_DVBT)  || (defined sony_cxd_DEMOD_SUPPORT_DVBT2) \
    || (defined sony_cxd_DEMOD_SUPPORT_DVBC)  || (defined sony_cxd_DEMOD_SUPPORT_DVBC2) \
    || (defined sony_cxd_DEMOD_SUPPORT_ISDBT) || (defined sony_cxd_DEMOD_SUPPORT_ISDBC) \
    || (defined sony_cxd_DEMOD_SUPPORT_J83B)
#define sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE  /**< Enable common terrestial and cable code */
#endif

#if (defined sony_cxd_DEMOD_SUPPORT_DVBS_S2) || (defined sony_cxd_DEMOD_SUPPORT_ISDBS) || (defined sony_cxd_DEMOD_SUPPORT_ISDBS3)
#define sony_cxd_DEMOD_SUPPORT_SAT  /**< Enable common satellite code */
#endif

#if (defined sony_cxd_DEMOD_SUPPORT_ISDBS3) || (defined sony_cxd_DEMOD_SUPPORT_DVBC2)
#define sony_cxd_DEMOD_SUPPORT_TLV  /**< Enable TLV output related code */
#endif

#if defined sony_cxd_DEMOD_SUPPORT_REMOVE_TLV
#undef sony_cxd_DEMOD_SUPPORT_TLV
#endif

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_cxd_common.h"
#include "sony_cxd_i2c.h"
#include "sony_cxd_dtv.h"

#ifdef sony_cxd_DEMOD_SUPPORT_DVBT
#include "sony_cxd_dvbt.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_DVBC
#include "sony_cxd_dvbc.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_DVBT2
#include "sony_cxd_dvbt2.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_DVBC2
#include "sony_cxd_dvbc2.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_DVBS_S2
#include "sony_cxd_dvbs.h"
#include "sony_cxd_dvbs2.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_ISDBT
#include "sony_cxd_isdbt.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_ISDBC
#include "sony_cxd_isdbc.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_ISDBS
#include "sony_cxd_isdbs.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_ISDBS3
#include "sony_cxd_isdbs3.h"
#endif

#ifdef sony_cxd_DEMOD_SUPPORT_J83B
#include "sony_cxd_j83b.h"
#endif

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
/**
 @brief Calculate the demodulator IF Freq setting ::sony_cxd_demod_t::iffreqConfig.
        ((IFFREQ/Sampling Freq at Down Converter DSP module) * Down converter's dynamic range + 0.5
*/
#define sony_cxd_DEMOD_MAKE_IFFREQ_CONFIG(iffreq) ((uint32_t)(((iffreq)/48.0)*16777216.0 + 0.5))

#define sony_cxd_DEMOD_MAX_CONFIG_MEMORY_COUNT 100 /**< The maximum number of entries in the configuration memory table */

/**
 @brief Freeze all registers in the SLV-T device.  This API is used by the monitor functions to ensure multiple separate
        register reads are from the same snapshot

 @note This should not be manually called or additional instances added into the driver unless under specific instruction.
*/
#define SLVT_CXD_FreezeReg(pDemod) ((pDemod)->pI2c->WriteOneRegister ((pDemod)->pI2c, (pDemod)->i2cAddressSLVT, 0x01, 0x01))

/**
 @brief Unfreeze all registers in the SLV-T device
*/
#define SLVT_CXD_UnFreezeReg(pDemod) ((void)((pDemod)->pI2c->WriteOneRegister ((pDemod)->pI2c, (pDemod)->i2cAddressSLVT, 0x01, 0x00)))

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief The demodulator Chip ID mapping.
*/
typedef enum {
    sony_cxd_DEMOD_CHIP_ID_UNKNOWN = 0,     /**< Unknown */
    sony_cxd_DEMOD_CHIP_ID_CXD2856 = 0x90,  /**< CXD2856 */
    sony_cxd_DEMOD_CHIP_ID_CXD2857 = 0x91   /**< CXD2857 */
} sony_cxd_demod_chip_id_t;

/**
 @brief Demodulator crystal frequency.
*/
typedef enum {
    sony_cxd_DEMOD_XTAL_16000KHz = 0,       /**< 16 MHz */
    sony_cxd_DEMOD_XTAL_24000KHz = 1,       /**< 24 MHz */
    sony_cxd_DEMOD_XTAL_32000KHz = 2        /**< 32 MHz */
} sony_cxd_demod_xtal_t;

/**
 @brief Demodulator software state.
*/
typedef enum {
    sony_cxd_DEMOD_STATE_UNKNOWN,           /**< Unknown. */
    sony_cxd_DEMOD_STATE_SHUTDOWN,          /**< Chip is in Shutdown state. */
    sony_cxd_DEMOD_STATE_SLEEP,             /**< Chip is in Sleep state. */
    sony_cxd_DEMOD_STATE_ACTIVE,            /**< Chip is in Active state. */
    sony_cxd_DEMOD_STATE_INVALID            /**< Invalid, result of an error during a state change. */
} sony_cxd_demod_state_t;

/**
 @brief Tuner I2C bus config.
*/
typedef enum {
    sony_cxd_DEMOD_TUNER_I2C_CONFIG_DISABLE,       /**< Tuner I2C disabled. */
    sony_cxd_DEMOD_TUNER_I2C_CONFIG_REPEATER,      /**< I2C repeater. */
    sony_cxd_DEMOD_TUNER_I2C_CONFIG_GATEWAY        /**< I2C gateway. */
} sony_cxd_demod_tuner_i2c_config_t;

/**
 @brief Enumeration of terrestrial/cable tuner types used for optimising the
        demodulator configuration.
*/
typedef enum {
    sony_cxd_DEMOD_TUNER_OPTIMIZE_NONSONY,     /**< Non-Sony tuners. */
    sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON  /**< Sony silicon tuners. */
} sony_cxd_demod_tuner_optimize_t;

/**
 @brief Demodulator software sub state for ISDB-T.
        EWS mode is for receiving EWS (Emargency Warning Broadcasting System) information, without TS output.
        This state is effective only when sony_cxd_demod_state_t is ACTIVE_T_C.
*/
typedef enum {
    sony_cxd_DEMOD_ISDBT_STATE_NORMAL = 0,   /**< Normal state.  TS output is enabled */
    sony_cxd_DEMOD_ISDBT_STATE_EWS           /**< EWS state.  TS output is disabled */
} sony_cxd_demod_isdbt_ews_state_t;

/**
 @brief Enumeration of spectrum inversion monitor values.
*/
typedef enum {
    sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_NORMAL = 0,             /**< Spectrum normal sense. */
    sony_cxd_DEMOD_TERR_CABLE_SPECTRUM_INV                     /**< Spectrum inverted. */
} sony_cxd_demod_terr_cable_spectrum_sense_t;

/**
 @brief Enumeration of I/Q inversion monitor values.
*/
typedef enum {
    sony_cxd_DEMOD_SAT_IQ_SENSE_NORMAL = 0,   /**< I/Q normal sense. */
    sony_cxd_DEMOD_SAT_IQ_SENSE_INV           /**< I/Q inverted. */
} sony_cxd_demod_sat_iq_sense_t;

/**
 @brief Configuration options for the demodulator.
*/
typedef enum {
    /**
     @brief Parallel or serial TS output selection.

            Value:
            - 0: Serial output.
            - 1: Parallel output (Default).
    */
    sony_cxd_DEMOD_CONFIG_PARALLEL_SEL,

    /**
     @brief Serial output pin of TS data.

            Value:
            - 0: Output from TSDATA0
            - 1: Output from TSDATA7 (Default).
    */
    sony_cxd_DEMOD_CONFIG_SER_DATA_ON_MSB,

    /**
     @brief Parallel/Serial output bit order on TS data.

            Value (Parallel):
            - 0: MSB TSDATA[0]
            - 1: MSB TSDATA[7] (Default).
            Value (Serial):
            - 0: LSB first
            - 1: MSB first (Default).
    */
    sony_cxd_DEMOD_CONFIG_OUTPUT_SEL_MSB,

    /**
     @brief TS valid active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    sony_cxd_DEMOD_CONFIG_TSVALID_ACTIVE_HI,

    /**
     @brief TS sync active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    sony_cxd_DEMOD_CONFIG_TSSYNC_ACTIVE_HI,

    /**
     @brief TS error active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    sony_cxd_DEMOD_CONFIG_TSERR_ACTIVE_HI,

    /**
     @brief TS clock inversion setting.

            Value:
            - 0: Falling/Negative edge.
            - 1: Rising/Positive edge (Default).
    */
    sony_cxd_DEMOD_CONFIG_LATCH_ON_POSEDGE,

    /**
     @brief Serial TS clock gated on valid TS data or is continuous.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.

            Value:
            - 0: Gated
            - 1: Continuous (Default)
    */
    sony_cxd_DEMOD_CONFIG_TSCLK_CONT,

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
    sony_cxd_DEMOD_CONFIG_TSCLK_MASK,

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
    sony_cxd_DEMOD_CONFIG_TSVALID_MASK,

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
    sony_cxd_DEMOD_CONFIG_TSERR_MASK,

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
    sony_cxd_DEMOD_CONFIG_PARALLEL_TSCLK_MANUAL,

    /**
     @brief TS packet gap setting.

            Note: This setting is effective only when DEMOD_CONFIG_PARALLEL_TSCLK_MANUAL = 0.

            Value:
            - 0:       TS packet gap is controlled as short as possible.
            - 1 - 7:   TS packet gap is controlled about following value.
                       188/(2^value - 1) [byte]
                       Default value is 4.
    */
    sony_cxd_DEMOD_CONFIG_TS_PACKET_GAP,

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
    sony_cxd_DEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE,

    /**
     @brief Writes a value to the PWM output.
            Please note the actual PWM precision.

            0x1000 => DVDD
            0x0000 => GND
    */
    sony_cxd_DEMOD_CONFIG_PWM_VALUE,

    /**
     @brief Configure the driving current for the TS Clk pin.

            - 0 : 2mA
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA (Default)
    */
    sony_cxd_DEMOD_CONFIG_TSCLK_CURRENT,

    /**
     @brief Configure the driving current for the TS Sync / TS Valid
            / TS Data pins.

            - 0 : 2mA
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA (Default)
    */
    sony_cxd_DEMOD_CONFIG_TS_CURRENT,

    /**
     @brief Configure the driving current for the GPIO 0 pin.

            - 0 : 2mA (Default)
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA
    */
    sony_cxd_DEMOD_CONFIG_GPIO0_CURRENT,

    /**
     @brief Configure the driving current for the GPIO 1 pin.

            - 0 : 2mA (Default)
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA
    */
    sony_cxd_DEMOD_CONFIG_GPIO1_CURRENT,

    /**
     @brief Configure the driving current for the GPIO 2 pin.

            - 0 : 2mA (Default)
            - 1 : 4mA
            - 2 : 8mA
            - 3 : 10mA
    */
    sony_cxd_DEMOD_CONFIG_GPIO2_CURRENT,

    /* ---- For terrestrial and cable ---- */

    /**
     @brief Configure the clock frequency for Serial TS in terrestrial and cable active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when sony_cxd_DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS).

            - 0 : Invalid
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
            - 3 : 64.00MHz
            - 4 : 48.00MHz
            - 5 : 38.40MHz
    */
    sony_cxd_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ,

    /**
     @brief Terrestrial / cable tuner type for demodulator specific optimisations.

            Value:
            - sony_cxd_DEMOD_TUNER_OPTIMIZE_NONSONY (0)     : Non-Sony tuner.
            - sony_cxd_DEMOD_TUNER_OPTIMIZE_SONYSILICON (1) : Sony sillicion tuners. (Default)
    */
    sony_cxd_DEMOD_CONFIG_TUNER_OPTIMIZE,

    /**
     @brief IFAGC sense configuration.

            Value:
            - 0: Positive IFAGC.
            - 1: Inverted IFAGC (Default)
    */
    sony_cxd_DEMOD_CONFIG_IFAGCNEG,

    /**
     @brief Configure the full-scale range of the ADC input to the IFAGC.

            Value:
            - 0: 1.4Vpp (Default)
            - 1: 1.0Vpp
            - 2: 0.7Vpp
    */
    sony_cxd_DEMOD_CONFIG_IFAGC_ADC_FS,

    /**
     @brief Terrestrial / Cable tuner IF spectrum sense configuration.

            Value:
            - 0: IF spectrum sense is not same as RF. Used for Normal / Ordinary tuners i.e. ASCOT. (Default)
            - 1: IF spectrum sense is same as RF.
    */
    sony_cxd_DEMOD_CONFIG_SPECTRUM_INV,

    /**
     @brief Configure the order in which systems are attempted in Blind Tune and
            Scan.  This can be used to optimize scan duration where specific
            details on system split ratio are known about the spectrum.

            Value:
            - 0: DVB-T followed by DVBT2 (default).
            - 1: DVB-T2 followed by DVBT.
    */
    sony_cxd_DEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST,

    /**
     @brief Set the measurment period for Pre-RS BER (DVB-T).

            This is a 5 bit value with a default of 11.
    */
    sony_cxd_DEMOD_CONFIG_DVBT_BERN_PERIOD,

    /**
     @brief Set the measurment period for Pre-RS BER (DVB-C/ISDB-C/J.83B).

            This is a 5 bit value with a default of 11.
    */
    sony_cxd_DEMOD_CONFIG_DVBC_BERN_PERIOD,

    /**
     @brief Set the measurment period for Pre-Viterbi BER (DVB-T).

            This is a 3 bit value with a default of 1.
    */
    sony_cxd_DEMOD_CONFIG_DVBT_VBER_PERIOD,

    /**
     @brief Set the measurment period for Pre-BCH BER (DVB-T2/C2) and
            Post-BCH FER (DVB-T2/C2).

            This is a 4 bit value with a default of 8.
    */
    sony_cxd_DEMOD_CONFIG_DVBT2C2_BBER_MES,

    /**
     @brief Set the measurment period for Pre-LDPC BER (DVB-T2/C2).

            This is a 4 bit value with a default of 8.
    */
    sony_cxd_DEMOD_CONFIG_DVBT2C2_LBER_MES,

    /**
     @brief Set the measurment period for PER (DVB-T).

            This is a 4 bit value with a default of 10.
    */
    sony_cxd_DEMOD_CONFIG_DVBT_PER_MES,

    /**
     @brief Set the measurment period for PER (DVB-C/ISDB-C/J.83B).

            This is a 5 bit value with a default of 10.
    */
    sony_cxd_DEMOD_CONFIG_DVBC_PER_MES,

    /**
     @brief Set the measurment period for PER (DVB-T2/C2).

            This is a 4 bit value with a default of 10.
    */
    sony_cxd_DEMOD_CONFIG_DVBT2C2_PER_MES,

    /**
     @brief Set the measurment period for Pre-RS and PER (ISDB-T).

            This is a 15 bit value with a default of 512.
    */
    sony_cxd_DEMOD_CONFIG_ISDBT_BERPER_PERIOD,

    /**
     @brief Averaged Pre-RS BER period type for DVB-T.

            If data length is selected, the measurement period is determied by
            ::sony_cxd_DEMOD_CONFIG_DVBT_BERN_PERIOD setting.
            If time is selected, the measurement period is determined by
            ::sony_cxd_DEMOD_CONFIG_DVBT_AVEBER_PERIOD_TIME setting.

            Value:
            - 0: Determined by data length. (default)
            - 1: Determined by time.
    */
    sony_cxd_DEMOD_CONFIG_DVBT_AVEBER_PERIOD_TYPE,

    /**
     @brief Time period (ms) of averaged Pre-RS BER for DVB-T.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
    sony_cxd_DEMOD_CONFIG_DVBT_AVEBER_PERIOD_TIME,

    /**
     @brief Averaged Pre-BCH BER period type for DVB-T2.

            If data length is selected, the measurement period is determied by
            ::sony_cxd_DEMOD_CONFIG_DVBT2C2_BBER_MES setting.
            If time is selected, the measurement period is determined by
            ::sony_cxd_DEMOD_CONFIG_DVBT2_AVEBER_PERIOD_TIME setting.

            Value:
            - 0: Determined by data length. (default)
            - 1: Determined by time.
    */
    sony_cxd_DEMOD_CONFIG_DVBT2_AVEBER_PERIOD_TYPE,

    /**
     @brief Time period (ms) of averaged Pre-BCH BER for DVB-T2.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
    sony_cxd_DEMOD_CONFIG_DVBT2_AVEBER_PERIOD_TIME,

    /**
     @brief Time period (ms) of averaged SNR for DVB-T/T2.

            From 1 to 2795 (ms) is available.
            Default is 500 (ms).
    */
    sony_cxd_DEMOD_CONFIG_DVBTT2_AVESNR_PERIOD_TIME,

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
    sony_cxd_DEMOD_CONFIG_GPIO_EWS_FLAG,

    /**
     @brief ISDB-C configuration for TSMF header packet NULL replacement.

            It's effective in multiple-TS mode or auto-detect mode.

            Value:
            - 0: Do nothing.
            - 1: TSMF header included packet is replaced to NULL. (PID is only replaced. (0x1FFF)) (Default)
     */
    sony_cxd_DEMOD_CONFIG_ISDBC_TSMF_HEADER_NULL,

    /**
     @brief ISDB-C configuration for TSVALID in NULL replaced packets.

            It's effective in multiple-TS mode or auto-detect mode.

            Value:
            - 0: Do nothing.
            - 1: TSVALID is low for NULL replaced TS packets. (Default)
                 - TS packets including unselected TSID,
                 - TSMF header packets (if sony_cxd_DEMOD_CONFIG_ISDBC_TSMF_HEADER_NULL = 1)
     */
    sony_cxd_DEMOD_CONFIG_ISDBC_NULL_REPLACED_TS_TSVALID_LOW,

    /* ---- For satellite ---- */

    /**
     @brief Configure the clock frequency for Serial TS in Satellite active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when sony_cxd_DEMOD_CONFIG_PARALLEL_SEL = 0 (serial TS).

            - 0 : 128.00MHz
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
            - 3 : 64.00MHz
            - 4 : 48.00MHz
            - 5 : 38.40MHz
    */
    sony_cxd_DEMOD_CONFIG_SAT_TS_SERIAL_CLK_FREQ,

    /**
     @brief I/Q connection sense inversion between tuner and demod.

            - 0: Normal (Default).
            - 1: Inverted. (I/Q signal input to Q/I pin of demodulator)
    */
    sony_cxd_DEMOD_CONFIG_SAT_TUNER_IQ_SENSE_INV,

    /**
     @brief IFAGC sense configuration for satellite.

            Value:
            - 0: Positive IFAGC (Default)
            - 1: Negative IFAGC.
    */
    sony_cxd_DEMOD_CONFIG_SAT_IFAGCNEG,

    /**
     @brief Measurement period for Pre-RS BER(DVB-S), PER(DVB-S) and PER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_DVBSS2_BER_PER_MES,

    /**
     @brief Measurement period for Pre-BCH BER(DVB-S2) and Post-BCH FER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_DVBS2_BER_FER_MES,

    /**
     @brief Measurement period for Pre-Viterbi BER(DVB-S).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_DVBS_VBER_MES,

    /**
     @brief Measurement period for Pre-LDPC BER(DVB-S2).

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_DVBS2_LBER_MES,

    /**
     @brief Power spectrum smoothing setting used in sony_cxd_integ_dvbs_s2_BlindScan
            and sony_cxd_integ_dvbs_s2_TuneSRS.

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
    sony_cxd_DEMOD_CONFIG_DVBSS2_BLINDSCAN_POWER_SMOOTH,

    /**
     @brief Measurement unit for Pre-RS BER(ISDB-S) and and Post-RS PER(ISDB-S).

            Value:
            - 0: Measurement unit is in Super Frames.(Default)
            - 1: Measurement unit is in Slots.
    */
    sony_cxd_DEMOD_CONFIG_ISDBS_BERNUMCONF,

    /**
     @brief Measurement period for Pre-RS BER(ISDB-S) and Post-RS PER(ISDB-S) at transmission mode 1.

            - The period is 2^(value) super frames or slots.
              If sony_cxd_DEMOD_CONFIG_ISDBS_BERNUMCONF
                 = 0 then the measurement period is 2^ super frames.
                 = 1 then the measurement period is 2^ slots.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_ISDBS_BER_PERIOD1,

    /**
     @brief Measurement period for Pre-RS BER(ISDB-S) and Post-RS PER(ISDB-S)  at transmission mode 2.

            - The period is 2^(value) super frames or slots.
              If sony_cxd_DEMOD_CONFIG_ISDBS_BERNUMCONF
                 = 0 then the measurement period is 2^ super frames.
                 = 1 then the measurement period is 2^ slots.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_ISDBS_BER_PERIOD2,

    /**
     @brief Measurement period for Pre-RS BER(ISDB-S) at TMCC.

            - The period is 2^(value) super frames.
            - Valid range is 0 <= value <= 7.
    */
    sony_cxd_DEMOD_CONFIG_ISDBS_BER_PERIODT,

    /**
     @brief Measurement period for Pre-LDPC BER(ISDB-S3) at transmission mode 1.

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_ISDBS3_LBER_MES1,

    /**
     @brief Measurement period for Pre-LDPC BER(ISDB-S3) at transmission mode 2.

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_ISDBS3_LBER_MES2,

    /**
     @brief Measurement period for Pre-BCH BER(ISDB-S3) and Post-BCH FER(ISDB-S3) at transmission mode 1.

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_ISDBS3_BER_FER_MES1,

    /**
     @brief Measurement period for Pre-BCH BER(ISDB-S3) and Post-BCH FER(ISDB-S3) at transmission mode 2.

            - The period is 2^(value) frames.
            - Valid range is 0 <= value <= 15.
    */
    sony_cxd_DEMOD_CONFIG_ISDBS3_BER_FER_MES2,

    /* ---- For TLV output (ISDB-S3/DVB-C2) ---- */

    /**
     @brief TLV output setting for ISDB-S3.

            Values:
            - 0 : TS output
            - 1 : TLV output (Default)
    */
    sony_cxd_DEMOD_CONFIG_TLV_OUTPUT_ISDBS3,

    /**
     @brief TLV output setting for DVB-C2 (J.382).

            Values:
            - 0 : TS output (Default)
            - 1 : TLV output
    */
    sony_cxd_DEMOD_CONFIG_TLV_OUTPUT_DVBC2,

    /**
     @brief Parallel or serial TLV output selection.

            Value:
            - 0: Serial output.
            - 1: Parallel (8bit) output (Default).
            - 2: Parallel (2bit) output.
    */
    sony_cxd_DEMOD_CONFIG_TLV_PARALLEL_SEL,

    /**
     @brief Serial output pin of TLV data.

            Value:
            - 0: Output from TSDATA0
            - 1: Output from TSDATA7 (Default).
    */
    sony_cxd_DEMOD_CONFIG_TLV_SER_DATA_ON_MSB,

    /**
     @brief Parallel/Serial output bit order on TLV data.

            Value (Parallel):
            - 0: MSB TSDATA[0]
            - 1: MSB TSDATA[7] (Default).
            Value (Serial):
            - 0: LSB first
            - 1: MSB first (Default).
    */
    sony_cxd_DEMOD_CONFIG_TLV_OUTPUT_SEL_MSB,

    /**
     @brief TLV valid active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    sony_cxd_DEMOD_CONFIG_TLVVALID_ACTIVE_HI,

    /**
     @brief TLV sync active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    sony_cxd_DEMOD_CONFIG_TLVSYNC_ACTIVE_HI,

    /**
     @brief TLV error active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default).
    */
    sony_cxd_DEMOD_CONFIG_TLVERR_ACTIVE_HI,

    /**
     @brief TLV clock inversion setting.

            Value:
            - 0: Falling/Negative edge.
            - 1: Rising/Positive edge (Default).
    */
    sony_cxd_DEMOD_CONFIG_TLV_LATCH_ON_POSEDGE,

    /**
     @brief Serial TLV clock gated on valid TLV data or is continuous.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.

            Value:
            - 0: Gated
            - 1: Continuous (Default)
    */
    sony_cxd_DEMOD_CONFIG_TLVCLK_CONT,

    /**
     @brief Disable/Enable TLV clock during specified TLV region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Disable during TLV packet gap
            - 2 : Reserved
            - 4 : 5th - length bytes (TLV payload)
            - 8 : 2nd - 4th bytes (TLV header)
            - 16: 1st byte (TLV sync)
    */
    sony_cxd_DEMOD_CONFIG_TLVCLK_MASK,

    /**
     @brief Disable/Enable TLVVALID during specified TLV region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active
            - 1 : Disable during TLV packet gap (default)
            - 2 : Reserved (default)
            - 4 : 5th - length bytes (TLV payload)
            - 8 : 2nd - 4th bytes (TLV header)
            - 16: 1st byte (TLV sync)
    */
    sony_cxd_DEMOD_CONFIG_TLVVALID_MASK,

    /**
     @brief Disable/Enable TLVERR during specified TLV region.

            bit flags: ( can be bitwise ORed )
            - 0 : Always Active (default)
            - 1 : Disable during TLV packet gap
            - 2 : Reserved
            - 4 : 5th - length bytes (TLV payload)
            - 8 : 2nd - 4th bytes (TLV header)
            - 16: 1st byte (TLV sync)
    */
    sony_cxd_DEMOD_CONFIG_TLVERR_MASK,

    /**
     @brief Configure the clock frequency for Serial TLV in terrestrial and cable active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when sony_cxd_DEMOD_CONFIG_TLV_PARALLEL_SEL = 0 (serial TLV).

            - 0 : Invalid
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
            - 3 : 64.00MHz
            - 4 : 48.00MHz
            - 5 : 38.40MHz
    */
    sony_cxd_DEMOD_CONFIG_TERR_CABLE_TLV_SERIAL_CLK_FREQ,

    /**
     @brief Configure the clock frequency for Serial TLV in Satellite active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when sony_cxd_DEMOD_CONFIG_TLV_PARALLEL_SEL = 0 (serial TLV).

            - 0 : 128.00MHz
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
            - 3 : 64.00MHz
            - 4 : 48.00MHz
            - 5 : 38.40MHz
    */
    sony_cxd_DEMOD_CONFIG_SAT_TLV_SERIAL_CLK_FREQ,

    /**
     @brief Configure the clock frequency for 2bit Parallel TLV in terrestrial and cable active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when sony_cxd_DEMOD_CONFIG_TLV_PARALLEL_SEL = 2 (2bit parallel TLV).

            - 0 : Invalid
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
    */
    sony_cxd_DEMOD_CONFIG_TERR_CABLE_TLV_2BIT_PARALLEL_CLK_FREQ,

    /**
     @brief Configure the clock frequency for 2bit Parallel TLV in Satellite active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.
            Only valid when sony_cxd_DEMOD_CONFIG_TLV_PARALLEL_SEL = 2 (2bit parallel TLV).

            - 0 : Invalid
            - 1 : 96.00MHz (Default)
            - 2 : 76.80MHz
    */
    sony_cxd_DEMOD_CONFIG_SAT_TLV_2BIT_PARALLEL_CLK_FREQ,

} sony_cxd_demod_config_id_t;

/**
 @brief Demodulator lock status.
*/
typedef enum {
    sony_cxd_DEMOD_LOCK_RESULT_NOTDETECT, /**< Neither "Lock" or "Unlock" conditions are met, lock status cannot be determined */
    sony_cxd_DEMOD_LOCK_RESULT_LOCKED,    /**< "Lock" condition is found. */
    sony_cxd_DEMOD_LOCK_RESULT_UNLOCKED   /**< No signal was found or the signal was not the required system. */
} sony_cxd_demod_lock_result_t;

/**
 @brief GPIO pin IDs
*/
typedef enum {
    sony_cxd_DEMOD_GPIO_PIN_GPIO0,
    sony_cxd_DEMOD_GPIO_PIN_GPIO1,
    sony_cxd_DEMOD_GPIO_PIN_GPIO2,
    sony_cxd_DEMOD_GPIO_PIN_TSDATA0,
    sony_cxd_DEMOD_GPIO_PIN_TSDATA1,
    sony_cxd_DEMOD_GPIO_PIN_TSDATA2,
    sony_cxd_DEMOD_GPIO_PIN_TSDATA3,
    sony_cxd_DEMOD_GPIO_PIN_TSDATA4,
    sony_cxd_DEMOD_GPIO_PIN_TSDATA5,
    sony_cxd_DEMOD_GPIO_PIN_TSDATA6,
    sony_cxd_DEMOD_GPIO_PIN_TSDATA7
} sony_cxd_demod_gpio_pin_t;

/**
 @brief Mode select for the multi purpose GPIO pins
*/
typedef enum {
    /** @brief GPIO pin is configured as an output */
    sony_cxd_DEMOD_GPIO_MODE_OUTPUT = 0x00,

    /** @brief GPIO pin is configured as an input */
    sony_cxd_DEMOD_GPIO_MODE_INPUT = 0x01,

    /**
     @brief GPIO pin is configured to output an PWM signal which can be configured using the
            ::sony_cxd_demod_SetConfig function with the config ID ::sony_cxd_DEMOD_CONFIG_PWM_VALUE.
    */
    sony_cxd_DEMOD_GPIO_MODE_PWM = 0x03,

    /** @brief GPIO pin is configured for TS output (TS error for GPIO2, TS data for TS data pins) */
    sony_cxd_DEMOD_GPIO_MODE_TS_OUTPUT = 0x04,

    /** @brief GPIO pin is configured to output the FEF timing indicator (DVB-T2 Only) */
    sony_cxd_DEMOD_GPIO_MODE_FEF_PART = 0x05,

    /** @brief GPIO pin is configured to EWS flag (ISDB-T/S/S3 Only) */
    sony_cxd_DEMOD_GPIO_MODE_EWS_FLAG = 0x06,

    /** @brief GPIO pin is configured to UPLINK flag (ISDB-S3 Only) */
    sony_cxd_DEMOD_GPIO_MODE_UPLINK_FLAG = 0x07,

    /** @brief GPIO pin is configured to output DiSEqC Transmit Enable */
    sony_cxd_DEMOD_GPIO_MODE_DISEQC_TX_EN = 0x08,

    /** @brief GPIO pin is configured to output DiSEqC Receive Enable */
    sony_cxd_DEMOD_GPIO_MODE_DISEQC_RX_EN = 0x09
} sony_cxd_demod_gpio_mode_t;

/**
 @brief TS/TLV serial clock frequency options
*/
typedef enum {
    sony_cxd_DEMOD_SERIAL_TS_TLV_CLK_HIGH_FULL,   /**< High frequency, full rate */
    sony_cxd_DEMOD_SERIAL_TS_TLV_CLK_MID_FULL,    /**< Mid frequency, full rate */
    sony_cxd_DEMOD_SERIAL_TS_TLV_CLK_LOW_FULL,    /**< Low frequency, full rate */
    sony_cxd_DEMOD_SERIAL_TS_TLV_CLK_HIGH_HALF,   /**< High frequency, half rate */
    sony_cxd_DEMOD_SERIAL_TS_TLV_CLK_MID_HALF,    /**< Mid frequency, half rate */
    sony_cxd_DEMOD_SERIAL_TS_TLV_CLK_LOW_HALF     /**< Low frequency, half rate */
} sony_cxd_demod_serial_ts_tlv_clk_t ;

/**
 @brief TLV 2bit parallel clock frequency options
*/
typedef enum {
    sony_cxd_DEMOD_2BIT_PARALLEL_TLV_CLK_HIGH,    /**< High frequency */
    sony_cxd_DEMOD_2BIT_PARALLEL_TLV_CLK_MID,     /**< Mid frequency */
    sony_cxd_DEMOD_2BIT_PARALLEL_TLV_CLK_LOW      /**< Low frequency */
} sony_cxd_demod_2bit_parallel_tlv_clk_t ;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/**
 @brief List of register values for IF frequency configuration.

        Used for handling tuners that output a different IF depending on the expected channel BW.
        Should be set using ::sony_cxd_DEMOD_MAKE_IFFREQ_CONFIG macro.

        ::sony_cxd_demod_SetIFFreqConfig can be used to setup Sony silicon tuner optimized value
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
    uint32_t configISDBT_6;             /**< ISDB-T 6MHz */
    uint32_t configISDBT_7;             /**< ISDB-T 7MHz */
    uint32_t configISDBT_8;             /**< ISDB-T 8MHz */
    uint32_t configISDBC_6;             /**< ISDB-C 6MHz */
    uint32_t configJ83B_5_06_5_36;      /**< J.83B 5.06/5.36Msps auto selection */
    uint32_t configJ83B_5_60;           /**< J.83B. 5.6Msps */
} sony_cxd_demod_iffreq_config_t;

/**
 @brief The demodulator configuration memory table entry. Used to store a register or
        bit modification made through either the ::sony_cxd_demod_SetConfig or
        ::sony_cxd_demod_SetAndSaveRegisterBits APIs.
*/
typedef struct {
    uint8_t slaveAddress;               /**< Slave address of register */
    uint8_t bank;                       /**< Bank for register */
    uint8_t registerAddress;            /**< Register address */
    uint8_t value;                      /**< Value to write to register */
    uint8_t bitMask;                    /**< Bit mask to apply on the value */
} sony_cxd_demod_config_memory_t;

/**
 @brief Demod parameters for ::sony_cxd_demod_Create().
*/
typedef struct {
    /**
    @brief The demodulator crystal frequency.
    */
    sony_cxd_demod_xtal_t xtalFreq;

    /**
     @brief SLVT I2C address (8-bit form - 8'bxxxxxxx0).
    */
    uint8_t i2cAddressSLVT;

    /**
     @brief Tuner I2C bus setting. (disable/repeater/gateway)
    */
    sony_cxd_demod_tuner_i2c_config_t tunerI2cConfig;

} sony_cxd_demod_create_param_t;

/**
 @brief The demodulator definition which allows control of the demodulator device
        through the defined set of functions. This portion of the driver is seperate
        from the tuner portion and so can be operated independently of the tuner.
*/
typedef struct sony_cxd_demod_t {

    /* ======== Following members are NOT cleared by sony_cxd_demod_Initialize ======== */

    /**
    @brief The demodulator crystal frequency.
           This is configured by ::sony_cxd_demod_create_param_t argument of ::sony_cxd_demod_Create.
    */
    sony_cxd_demod_xtal_t xtalFreq;

    /**
     @brief SLVT I2C address (8-bit form - 8'bxxxxxxx0).
            This is configured by ::sony_cxd_demod_create_param_t argument of ::sony_cxd_demod_Create.
    */
    uint8_t i2cAddressSLVT;

    /**
     @brief SLVX I2C address (8-bit form - 8'bxxxxxxx0). Fixed to i2cAddressSLVT + 4.
            This is configured by ::sony_cxd_demod_create_param_t argument of ::sony_cxd_demod_Create.
    */
    uint8_t i2cAddressSLVX;

    /**
     @brief I2C API instance.
            This is configured by ::sony_cxd_demod_Create.
    */
    sony_cxd_i2c_t * pI2c;

    /**
     @brief Tuner I2C bus setting. (disable/repeater/gateway)
            This is configured by ::sony_cxd_demod_create_param_t argument of ::sony_cxd_demod_Create.
    */
    sony_cxd_demod_tuner_i2c_config_t tunerI2cConfig;

    /**
     @brief The parallel TS clock Manual setting option for all active states.
            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_PARALLEL_TSCLK_MANUAL option.
    */
    uint8_t parallelTSClkManualSetting;

    /**
     @brief The serial TS clock mode for all active states.  This is configured using
            ::sony_cxd_demod_SetConfig with the sony_cxd_DEMOD_CONFIG_TSCLK_CONT option.
    */
    uint8_t serialTSClockModeContinuous;

    /**
     @brief TS backwards compatible mode state. This is configured using
            ::sony_cxd_demod_SetConfig with the sony_cxd_DEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE.
    */
    uint8_t isTSBackwardsCompatibleMode;

    /**
     @brief A table of the demodulator configuration changes stored from the
            ::sony_cxd_demod_SetConfig and ::sony_cxd_demod_SetAndSaveRegisterBits functions.
    */
    sony_cxd_demod_config_memory_t configMemory[sony_cxd_DEMOD_MAX_CONFIG_MEMORY_COUNT];

    /**
     @brief The index of the last valid entry in the configMemory table
    */
    uint8_t configMemoryLastEntry;

    /**
     @brief User defined data.
    */
    void * user;

#ifdef sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE

    /* ---- For terrestrial and cable ---- */

    /**
     @brief IF frequency configuration for terrestrial and cable systems.

            This is configured by ::sony_cxd_demod_SetIFFreqConfig.
            Use the ::sony_cxd_DEMOD_MAKE_IFFREQ_CONFIG macro for setting each member of
            sony_cxd_demod_iffreq_config_t.

    */
    sony_cxd_demod_iffreq_config_t iffreqConfig;

    /**
     @brief Stores the terrestrial / cable tuner type for demodulator specific optimisations.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_TUNER_OPTIMIZE option.
    */
    sony_cxd_demod_tuner_optimize_t tunerOptimize;

    /**
     @brief The terrestrial / cable tuner IF spectrum sense configured on
            the demodulator with ::sony_cxd_demod_SetConfig.
    */
    sony_cxd_demod_terr_cable_spectrum_sense_t confSense;

    /**
     @brief The serial TS clock frequency option for terrestrial and cable active states.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_TERR_CABLE_TS_SERIAL_CLK_FREQ option.
    */
    sony_cxd_demod_serial_ts_tlv_clk_t serialTSClkFreqTerrCable;

    /**
     @brief The order in which Blind Tune attempts acquisition.

            This value can be configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST option.
    */
    uint8_t blindTuneDvbt2First;

#endif /* sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE */

#ifdef sony_cxd_DEMOD_SUPPORT_SAT

    /* ---- For satellite ---- */

    /**
     @brief The flag whether current mode is "Single cable" mode or not.
            with sony_cxd_integ_sat_device_ctrl_EnterSinglecable and
            sony_cxd_integ_sat_device_ctrl_ExitSinglecable.
    */
    uint8_t isSinglecable;

    /**
     @brief The I/Q sense configured on the demodulator with ::sony_cxd_demod_SetConfig.
    */
    sony_cxd_demod_sat_iq_sense_t satTunerIqSense;

    /**
     @brief The flag whether the IQ polarity of single cable switch is invert.

            If it is necessary to change this value, please change it directly after initialize.
    */
    uint8_t isSinglecableIqInv;

    /**
     @brief The serial TS clock frequency option for Satellite active states.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_SAT_TS_SERIAL_CLK_FREQ option.
    */
    sony_cxd_demod_serial_ts_tlv_clk_t serialTSClkFreqSat;

    /**
     @brief The count of calculation for power spectrum calculation in BlindScan and TuneSRS.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_DVBSS2_BLINDSCAN_POWER_SMOOTH option.
    */
    uint8_t dvbss2PowerSmooth;

#endif /* sony_cxd_DEMOD_SUPPORT_SAT */

#ifdef sony_cxd_DEMOD_SUPPORT_TLV

    /* ---- For TLV output (ISDB-S3/DVB-C2) ---- */

    /**
     @brief TLV output enabled or not for ISDB-S3.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_TLV_OUTPUT_ISDBS3 option.
            Default value is 1.
    */
    uint8_t isdbs3TLVOutput;

    /**
     @brief TLV output enabled or not for DVB-C2 (J.382).

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_TLV_OUTPUT_DVBC2 option.
            Default value is 0.
    */
    uint8_t dvbc2TLVOutput;

    /**
     @brief The serial TLV clock mode for all active states.

            This is configured using
            ::sony_cxd_demod_SetConfig with the sony_cxd_DEMOD_CONFIG_TLVCLK_CONT option.
    */
    uint8_t serialTLVClockModeContinuous;

    /**
     @brief The serial TLV clock frequency option for terrestrial and cable active states.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_TERR_CABLE_TLV_SERIAL_CLK_FREQ option.
    */
    sony_cxd_demod_serial_ts_tlv_clk_t serialTLVClkFreqTerrCable;

    /**
     @brief The serial TLV clock frequency option for Satellite active states.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_SAT_TLV_SERIAL_CLK_FREQ option.
    */
    sony_cxd_demod_serial_ts_tlv_clk_t serialTLVClkFreqSat;

    /**
     @brief The 2bit parallel TLV clock frequency option for terrestrial and cable active states.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_TERR_CABLE_TLV_2BIT_PARALLEL_CLK_FREQ option.
    */
    sony_cxd_demod_2bit_parallel_tlv_clk_t twoBitParallelTLVClkFreqTerrCable;

    /**
     @brief The 2bit parallel TLV clock frequency option for Satellite active states.

            This is configured using ::sony_cxd_demod_SetConfig with the
            sony_cxd_DEMOD_CONFIG_SAT_TLV_2BIT_PARALLEL_CLK_FREQ option.
    */
    sony_cxd_demod_2bit_parallel_tlv_clk_t twoBitParallelTLVClkFreqSat;

#endif /* sony_cxd_DEMOD_SUPPORT_TLV */

    /* ======== Following members are cleared by sony_cxd_demod_Initialize (Temporary information) ======== */

    /**
    @brief The driver operating state.
    */
    sony_cxd_demod_state_t state;

    /**
    @brief The current system.
    */
    sony_cxd_dtv_system_t system;

    /**
     @brief Auto detected chip ID at initialisation.
    */
    sony_cxd_demod_chip_id_t chipId;

#ifdef sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE

    /* ---- For terrestrial and cable ---- */

    /**
     @brief The current bandwidth, terrestrial and cable systems only.
    */
    sony_cxd_dtv_bandwidth_t bandwidth;

    /**
     @brief Scan mode enable/disable. Only change this indicator during the
            sony_cxd_DEMOD_STATE_SLEEP.
    */
    uint8_t scanMode;

    /**
     @brief This flag indicates whether the current ISDB-T sub state is in EWS mode or normal mode.
            This value is only effective when the current system is ISDB-T and it is in
            sony_cxd_DEMOD_STATE_ACTIVE state.
    */
    sony_cxd_demod_isdbt_ews_state_t isdbtEwsState;

#endif /* sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE */

#ifdef sony_cxd_DEMOD_SUPPORT_SAT

    /* ---- For satellite ---- */

    /**
     @brief The scan mode for DVB-S/S2.
    */
    uint8_t dvbss2ScanMode;

#endif /* sony_cxd_DEMOD_SUPPORT_SAT */

} sony_cxd_demod_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Set up the demodulator.

        This MUST be called before calling ::sony_cxd_demod_Initialize.
        This API also clears the demodulator structs members including
        demodulator configuration memory table.

 @param pDemod Reference to memory allocated for the demodulator instance. The create
        function will setup this demodulator instance.
 @param pCreateParam Parameters to create this driver. (I2C slave address, Xtal frequency...)
 @param pDemodI2c The I2C driver that the demod will use as the I2C interface.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_Create (sony_cxd_demod_t * pDemod,
                                 sony_cxd_demod_create_param_t * pCreateParam,
                                 sony_cxd_i2c_t * pDemodI2c);

/**
 @brief Initialize the demodulator, into Sleep state from a power on state.

        Can also be used to reset the demodulator from any state back to
        ::sony_cxd_DEMOD_STATE_SLEEP.  Please note this will reset all SLV-T demodulator registers
        clearing any configuration settings.

 @param pDemod The demodulator instance.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_Initialize (sony_cxd_demod_t * pDemod);

/**
 @brief Put the demodulator into Sleep state.
        From this state the demodulator can be directly tuned to any terrestrial/cable/satellite signal,

        If currently in ::sony_cxd_DEMOD_STATE_SHUTDOWN the configuration memory will be loaded
        back into the demodulator.

 @param pDemod The demodulator instance.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_Sleep (sony_cxd_demod_t * pDemod);

/**
 @brief Shutdown the demodulator.

        The device is placed in "Shutdown" state.
        ::sony_cxd_demod_Sleep must be called to re-initialise the
        device and driver for future acquisitions.

 @param pDemod The demodulator instance.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_Shutdown (sony_cxd_demod_t * pDemod);

/**
 @brief Completes the demodulator acquisition setup.
        Must be called after system specific demod and RF tunes.

 @param pDemod The demodulator instance.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_TuneEnd (sony_cxd_demod_t * pDemod);

/**
 @brief Soft reset the demodulator.
        The soft reset will begin the devices acquisition process.

 @param pDemod The demod instance.

 @return sony_cxd_RESULT_OK if successfully reset.
*/
sony_cxd_result_t sony_cxd_demod_SoftReset (sony_cxd_demod_t * pDemod);

/**
 @brief Set configuration options on the demodulator.

 @param pDemod The demodulator instance.
 @param configId The configuration ID to set. See ::sony_cxd_demod_config_id_t.
 @param value The associated value. Depends on the configId.

 @return sony_cxd_RESULT_OK if successfully set the configuration option.
*/
sony_cxd_result_t sony_cxd_demod_SetConfig (sony_cxd_demod_t * pDemod,
                                    sony_cxd_demod_config_id_t configId,
                                    int32_t value);

#ifdef sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Set the IF frequency config to the sony_cxd_demod_t.

        Used for handling tuners that output a different IF depending on the expected channel BW.
        Should be set each members using ::sony_cxd_DEMOD_MAKE_IFFREQ_CONFIG macro.

 @param pDemod The demodulator instance.
 @param pIffreqConfig ::sony_cxd_demod_iffreq_config_t instance.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_SetIFFreqConfig (sony_cxd_demod_t * pDemod,
                                          sony_cxd_demod_iffreq_config_t * pIffreqConfig);
#endif /* sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE */

/**
 @brief Configure the tuner I2C enable.

 @note  Tuner I2C must be enabled if the tuner is connected to tuner I2C bus and
        use I2C repeater to control I2C access to the tuner.
        This function is automatically called in ::sony_cxd_demod_Initialize.

 @param pDemod The demodulator instance.
 @param tunerI2cConfig Tuner I2C setting. (disable/repeater/gateway)

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_TunerI2cEnable (sony_cxd_demod_t * pDemod,
                                         sony_cxd_demod_tuner_i2c_config_t tunerI2cConfig);

/**
 @brief Configure the demodulator to forward I2C messages to the
        output port for tuner control.

 @note  This function has effect only when I2C repeater is used.
        (sony_cxd_DEMOD_TUNER_I2C_REPEATER)

 @param pDemod The demodulator instance.
 @param enable Enable / Disable I2C repeater

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_I2cRepeaterEnable (sony_cxd_demod_t * pDemod,
                                            uint8_t enable);

/**
 @brief Setup the GPIO.

 @note  For TSDATA pin setting, this function does NOT check consistency between
        TS pin settings by ::sony_cxd_demod_SetConfig function.
        It should be take care by application.

 @param pDemod The demodulator instance.
 @param pin GPIO pin (GPIO0 - 2, TSDATA0 - 7).
 @param enable Set enable (1) or disable (0).
 @param mode GPIO pin mode

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_GPIOSetConfig (sony_cxd_demod_t * pDemod,
                                        sony_cxd_demod_gpio_pin_t pin,
                                        uint8_t enable,
                                        sony_cxd_demod_gpio_mode_t mode);

/**
 @brief Read the GPIO value.
        The GPIO should have been configured as an input (Read) GPIO.

 @param pDemod The demodulator instance.
 @param pin GPIO pin (GPIO0 - 2, TSDATA0 - 7).
 @param pValue The current value of the GPIO.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_GPIORead (sony_cxd_demod_t * pDemod,
                                   sony_cxd_demod_gpio_pin_t pin,
                                   uint8_t * pValue);

/**
 @brief Write the GPIO value.
        The GPIO should have been configured as an output (Write) GPIO.

 @param pDemod The demodulator instance.
 @param pin GPIO pin (GPIO0 - 2, TSDATA0 - 7).
 @param value The value to set as output.

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_GPIOWrite (sony_cxd_demod_t * pDemod,
                                    sony_cxd_demod_gpio_pin_t pin,
                                    uint8_t value);

/**
 @brief Get the Chip ID of the connected demodulator.

 @param pDemod The demodulator instance.
 @param pChipId Pointer to receive the IP ID into.

 @return sony_cxd_RESULT_OK if pChipId is valid.
*/
sony_cxd_result_t sony_cxd_demod_ChipID (sony_cxd_demod_t * pDemod,
                                 sony_cxd_demod_chip_id_t * pChipId);

#ifdef sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Monitor the value of the Internal Digital AGC output. In the case of
        very weak electric field, this value may be useful for SSI adjustment.

 @param pDemod The demodulator instance.
 @param pDigitalAGCOut The value of the Internal Digital AGC output.

 @return sony_cxd_RESULT_OK if pDigitalAGCOut is valid.
*/
sony_cxd_result_t sony_cxd_demod_terr_cable_monitor_InternalDigitalAGCOut (sony_cxd_demod_t * pDemod,
                                                                   uint16_t * pDigitalAGCOut);
#endif /* sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE */

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

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_SetAndSaveRegisterBits (sony_cxd_demod_t * pDemod,
                                                 uint8_t slaveAddress,
                                                 uint8_t bank,
                                                 uint8_t registerAddress,
                                                 uint8_t value,
                                                 uint8_t bitMask);

#ifdef sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE
/**
 @brief Enable / disable scan mode for acquisition in the demodulator.

 @param pDemod The demodulator instance
 @param system The system used for scanning
 @param scanModeEnabled State of scan mode to set

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_terr_cable_SetScanMode (sony_cxd_demod_t * pDemod,
                                                 sony_cxd_dtv_system_t system,
                                                 uint8_t scanModeEnabled);
#endif /* sony_cxd_DEMOD_SUPPORT_TERR_OR_CABLE */

/**
 @brief Set the TS clock mode and frequency based on the demod struct members.

        Called internally as part of each Sleep to Active
        state transition.

 @param pDemod The demodulator instance
 @param system The tuning system

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_SetTSClockModeAndFreq (sony_cxd_demod_t * pDemod, sony_cxd_dtv_system_t system);

#ifdef sony_cxd_DEMOD_SUPPORT_TLV
/**
 @brief Set the TLV clock mode and frequency based on the demod struct members.

        Called internally as part of each Sleep to Active
        state transition.

 @param pDemod The demodulator instance
 @param system The tuning system

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_SetTLVClockModeAndFreq (sony_cxd_demod_t * pDemod, sony_cxd_dtv_system_t system);
#endif /* sony_cxd_DEMOD_SUPPORT_TLV */

/**
 @brief Set TSDATA pin Hi-Z depend on TS output setting.
        Called internally as part of each Sleep to Active
        state transition.

 @param pDemod The demodulator instance
 @param enable Hi-Z enable(1) or disable(0)

 @return sony_cxd_RESULT_OK if successful.
*/
sony_cxd_result_t sony_cxd_demod_SetTSDataPinHiZ (sony_cxd_demod_t * pDemod, uint8_t enable);

#endif /* sony_cxd_DEMOD_H */
