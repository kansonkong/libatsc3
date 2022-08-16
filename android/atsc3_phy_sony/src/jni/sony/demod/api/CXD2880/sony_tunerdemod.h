/*------------------------------------------------------------------------------
  Copyright 2014-2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/08/25
  Modification ID : 336b142f3d3abb1b1a6de561f79ef6b01bc89ed4
------------------------------------------------------------------------------*/
/**
 @file  sony_tunerdemod.h

 @brief The common tuner and demodulator control interface.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_TUNERDEMOD_H
#define SONY_TUNERDEMOD_H

/*------------------------------------------------------------------------------
  Includes
------------------------------------------------------------------------------*/
#include "sony_common.h"
#include "sony_regio.h"
#include "sony_dtv.h"
#include "sony_dvbt.h"
#include "sony_dvbt2.h"
#include "sony_isdbt.h"

/*------------------------------------------------------------------------------
  Defines
------------------------------------------------------------------------------*/
/**
 @brief The maximum number of entries in the configuration memory table.
*/
#define SONY_TUNERDEMOD_MAX_CONFIG_MEMORY_COUNT 100

/**
 @brief Unfreeze all registers in the SLV-T device.
*/
#define SLVT_UnFreezeReg(pTunerDemod) ((void)((pTunerDemod)->pRegio->WriteOneRegister ((pTunerDemod)->pRegio, SONY_REGIO_TARGET_DEMOD, 0x01, 0x00)))

/**
 @name  Interrupt type ID

 @brief Interrupt type ID used for sony_tunerdemod_SetConfig(SONY_TUNERDEMOD_CONFIG_INTERRUPT)

        Please see description of SONY_TUNERDEMOD_CONFIG_INTERRUPT in detail.
*/
/*@{*/
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_UNDERFLOW     0x0001 /**< TS buffer underflow */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_OVERFLOW      0x0002 /**< TS buffer overflow */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_ALMOST_EMPTY  0x0004 /**< TS buffer almost empty */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_ALMOST_FULL   0x0008 /**< TS buffer almost full */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_RRDY          0x0010 /**< TS buffer read ready */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_ILLEGAL_COMMAND      0x0020 /**< Illegal command */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_ILLEGAL_ACCESS       0x0040 /**< Illegal access */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_CPU_ERROR            0x0100 /**< CPU error */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_LOCK                 0x0200 /**< Lock */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_INV_LOCK             0x0400 /**< Lock (logic invert) */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_NOOFDM               0x0800 /**< No OFDM (early unlock detection) */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_EWS                  0x1000 /**< EWS (Emergency Warning System) for ISDB-T */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_EEW                  0x2000 /**< EEW (Earthquake Early Warning) for ISDB-T */
#define SONY_TUNERDEMOD_INTERRUPT_TYPE_FEC_FAIL             0x4000 /**< FEC fail */
/*@}*/

/**
 @name Interrupt lock type ID

 @brief Interrupt lock type ID used for
        sony_tunerdemod_SetConfig(SONY_TUNERDEMOD_CONFIG_INTERRUPT_LOCK_SEL) and
        sony_tunerdemod_SetConfig(SONY_TUNERDEMOD_CONFIG_INTERRUPT_INV_LOCK_SEL)

        Please see description of SONY_TUNERDEMOD_CONFIG_INTERRUPT_LOCK_SEL
        and SONY_TUNERDEMOD_CONFIG_INTERRUPT_INV_LOCK_SEL in detail.
*/
/*@{*/
#define SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_L1POST_OK        0x01 /**< L1 Post OK */
#define SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_DMD_LOCK         0x02 /**< Demodulator Lock */
#define SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_TS_LOCK          0x04 /**< TS Lock */
/*@}*/

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Chip ID mapping.
*/
typedef enum {
    SONY_TUNERDEMOD_CHIP_ID_UNKNOWN        = 0x00,  /**< Unknown ID */
    SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_0X = 0x62,  /**< CXD2880 ES1.00, ES1.01 */
    SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_11 = 0x6A   /**< CXD2880 ES1.11 */
} sony_tunerdemod_chip_id_t;

/**
 @brief Macro to check the validity of Chip ID.
*/
#define SONY_TUNERDEMOD_CHIP_ID_VALID(chipID) (((chipID) == SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_0X) || \
                                               ((chipID) == SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_11))

/**
 @brief Tuner and Demodulator software state.
*/
typedef enum {
    SONY_TUNERDEMOD_STATE_UNKNOWN,      /**< Unknown state. */
    SONY_TUNERDEMOD_STATE_SLEEP,        /**< Chip is in Sleep state. */
    SONY_TUNERDEMOD_STATE_ACTIVE,       /**< Chip is in Active state. */
    SONY_TUNERDEMOD_STATE_INVALID       /**< Invalid, result of an error during a state change. */
} sony_tunerdemod_state_t;

/**
 @brief Tuner and Demodulator diver mode.
*/
typedef enum {
    SONY_TUNERDEMOD_DIVERMODE_SINGLE,   /**< Single mode. */
    SONY_TUNERDEMOD_DIVERMODE_MAIN,     /**< Main in diversity. */
    SONY_TUNERDEMOD_DIVERMODE_SUB       /**< Sub in diversity. */
} sony_tunerdemod_divermode_t;

/**
 @brief Tuner and Demodulator clock mode.
*/
typedef enum {
    SONY_TUNERDEMOD_CLOCKMODE_UNKNOWN,  /**< Unknown. */
    SONY_TUNERDEMOD_CLOCKMODE_A,        /**< CLK A. */
    SONY_TUNERDEMOD_CLOCKMODE_B,        /**< CLK B. */
    SONY_TUNERDEMOD_CLOCKMODE_C         /**< CLK C. */
} sony_tunerdemod_clockmode_t;

/**
 @brief TS output interface definition.
*/
typedef enum {
    SONY_TUNERDEMOD_TSOUT_IF_TS,        /**< Output TS from TS I/F. */
    SONY_TUNERDEMOD_TSOUT_IF_SPI,       /**< Output TS from SPI. */
    SONY_TUNERDEMOD_TSOUT_IF_SDIO       /**< Output TS from SDIO. */
} sony_tunerdemod_tsout_if_t;

/**
 @brief Xtal share type definition.
*/
typedef enum {
    SONY_TUNERDEMOD_XTAL_SHARE_NONE,    /**< Not xtal shared with other IC. */
    SONY_TUNERDEMOD_XTAL_SHARE_EXTREF,  /**< External xtal input. (but not dual nor diver) */
    SONY_TUNERDEMOD_XTAL_SHARE_MASTER,  /**< Used as master IC in dual or diver system. */
    SONY_TUNERDEMOD_XTAL_SHARE_SLAVE    /**< Used as slave IC in dual or diver system. */
} sony_tunerdemod_xtal_share_t;

/**
 @brief Enumeration of spectrum inversion monitor values.
*/
typedef enum {
    SONY_TUNERDEMOD_SPECTRUM_NORMAL,    /**< Spectrum normal sense. */
    SONY_TUNERDEMOD_SPECTRUM_INV        /**< Spectrum inverted. */
} sony_tunerdemod_spectrum_sense_t;

/**
 @brief Confiruration ID definition.
*/
typedef enum {
    /**
     @brief Serial output bit order on TS data.

            Value:
            - 0: LSB first
            - 1: MSB first (Default).
    */
    SONY_TUNERDEMOD_CONFIG_OUTPUT_SEL_MSB,

    /**
     @brief TS valid active level.

            Value:
            - 0: Valid low.
            - 1: Valid high (Default)
    */
    SONY_TUNERDEMOD_CONFIG_TSVALID_ACTIVE_HI,

    /**
     @brief TS sync active level.

            Value:
            - 0: Valid low
            - 1: Valid high (Default)
    */
    SONY_TUNERDEMOD_CONFIG_TSSYNC_ACTIVE_HI,

    /**
     @brief TS error active level.

            Value:
            - 0: Valid low
            - 1: Valid high (Default)
    */
    SONY_TUNERDEMOD_CONFIG_TSERR_ACTIVE_HI,

    /**
     @brief TS clock inversion setting.

            Value:
            - 0: Falling/Negative edge
            - 1: Rising/Positive edge (Default)
    */
    SONY_TUNERDEMOD_CONFIG_LATCH_ON_POSEDGE,

    /**
     @brief Serial TS clock gated on valid TS data or is continuous.
            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.

            Value:
            - 0: Gated
            - 1: Continuous (Default)
    */
    SONY_TUNERDEMOD_CONFIG_TSCLK_CONT,

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
    SONY_TUNERDEMOD_CONFIG_TSCLK_MASK,

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
    SONY_TUNERDEMOD_CONFIG_TSVALID_MASK,

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
    SONY_TUNERDEMOD_CONFIG_TSERR_MASK,

    /**
     @brief Disable/Enable TS valid for error TS packets.

            Value:
            - 0 : TS valid is not changed for error TS packets (Default)
            - 1 : TS valid is disabled for error TS packets
    */
    SONY_TUNERDEMOD_CONFIG_TSERR_VALID_DISABLE,

    /**
     @brief Configure the driving current for the TS related pins.

            bit flags: ( can be bitwise ORed )
            - Bit[0] : TSCLK pin
            - Bit[1] : TSDATA pin
            - Bit[2] : TSSYNC pin
            - Bit[3] : TSVALID pin
            - Bit[4] : SDDATA1 pin (TSERR)
            - Bit[5] : SDDATA2 pin

            bit meaning:
            - 0 : 12mA
            - 1 : 16mA (Default)
    */
    SONY_TUNERDEMOD_CONFIG_TSPIN_CURRENT,

    /**
     @brief Configure the TS related pins pull up manual setting.

            Value:
            - 0 : Pull up is controlled automatically (Default)
            - 1 : Manually pull up is enabled. (by SONY_TUNERDEMOD_CONFIG_TSPIN_PULLUP)
    */
    SONY_TUNERDEMOD_CONFIG_TSPIN_PULLUP_MANUAL,

    /**
     @brief Configure the TS related pins pull up.

            If SONY_TUNERDEMOD_CONFIG_TSPIN_PULLUP_MANUAL = 0 (Auto),
            then this configuration will have no effect.

            bit flags: ( can be bitwise ORed )
            - Bit[0] : TSCLK pin
            - Bit[1] : TSDATA pin
            - Bit[2] : TSSYNC pin
            - Bit[3] : TSVALID pin
            - Bit[4] : SDDATA1 pin (TSERR)
            - Bit[5] : SDDATA2 pin

            bit meaning:
            - 0 : Pull up disable
            - 1 : Pull up enable (Default)
    */
    SONY_TUNERDEMOD_CONFIG_TSPIN_PULLUP,

    /**
     @brief Configure the clock frequency for Serial TS in terrestrial active states.

            Value is stored in demodulator structure to be applied during Sleep to Active
            transition.

            Value (Depends on clock mode):
            - 0 : 82.28MHz (A), 93.33MHz (B), 96MHz (C) (Default)
            - 1 : 41.14MHz (A), 46.66MHz (B), 48MHz (C)
    */
    SONY_TUNERDEMOD_CONFIG_TSCLK_FREQ,

    /**
     @brief Enable or disable the auto TS byte clock rate.
            Also, allows to set the TS byte clock rate manually.

     @note  By this setting, not TS clock rate but *TS byte gap length* is changed.
            This is generally NOT required under normal operation.

            Value:
            - 0:       Disable TS byte clock manual setting. (Default)
                       TS byte clock rate is automatic.
            - 1 - 255: Enable TS byte clock rate manual setting.
                       TS byte clock rate will become as follows:
                       - F / Value [MHz]
                         F = 82.28MHz (A), 93.33MHz (B), 96MHz (C) (depends on clock mode)
    */
    SONY_TUNERDEMOD_CONFIG_TSBYTECLK_MANUAL,

    /**
     @brief TS packet gap setting.

            Note: This setting is effective only when SONY_TUNERDEMOD_CONFIG_TSBYTECLK_MANUAL = 0.

            Value:
            - 0:       TS packet gap is controlled as short as possible.
            - 1 - 7:   TS packet gap is controlled about following value.
                       188/(2^value - 1) [byte]
                       Default value is 4.
    */
    SONY_TUNERDEMOD_CONFIG_TS_PACKET_GAP,

    /**
     @brief This configuration can be used to configure the demodulator to output a TS waveform that is
            backwards compatible with previous generation demodulators (CXD2820 / CXD2834 / CXD2835 / CXD2836).

            This option should not be used unless specifically required to overcome a HW configuration issue.
            This option affects all the DVB standards but not the ISDB standards.

            The demodulator will have the following settings, which will override any prior individual
            configuration:
            - Disable TS packet gap insertion.
            - Add TS parity period for DVB-T.

            Values:
            - 0 : Backwards compatible mode disabled (Default)
            - 1 : Backwards compatible mode enabled
    */
    SONY_TUNERDEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE,

    /**
     @brief Writes a value to the PWM output.

            Please note the actual PWM precision.
            0x1000 => DVDD
            0x0000 => GND
    */
    SONY_TUNERDEMOD_CONFIG_PWM_VALUE,

    /**
     @brief Interrupt setting.

            Please set the "OR" value of following definitions which you want to get as interrupt cause.
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_UNDERFLOW
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_OVERFLOW
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_ALMOST_EMPTY
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_ALMOST_FULL
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_BUFFER_RRDY
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_ILLEGAL_COMMAND
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_ILLEGAL_ACCESS
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_CPU_ERROR
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_LOCK
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_INV_LOCK
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_NOOFDM
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_EWS
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_EEW
            - SONY_TUNERDEMOD_INTERRUPT_TYPE_FEC_FAIL
    */
    SONY_TUNERDEMOD_CONFIG_INTERRUPT,

    /**
     @brief Interrupt setting related to LOCK_SEL.

            Please set the "OR" value of following definitions.
            - SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_L1POST_OK
            - SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_DMD_LOCK
            - SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_TS_LOCK

            If you set "SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_DMD_LOCK | SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_TS_LOCK",
            This IC generate interrupt at detecting DMD_LOCK and TS_LOCK.
    */
    SONY_TUNERDEMOD_CONFIG_INTERRUPT_LOCK_SEL,

    /**
     @brief Interrupt setting related to INV_LOCK_SEL.

            Please set the "OR" value of following definitions.
            - SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_L1POST_OK
            - SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_DMD_LOCK
            - SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_TS_LOCK

            If you set "SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_DMD_LOCK | SONY_TUNERDEMOD_INTERRUPT_LOCK_SEL_TS_LOCK",
            This IC generate interrupt at detecting !(DMD_LOCK and TS_LOCK).
            It is equal to "!DMD_LOCK or !TS_LOCK".
    */
    SONY_TUNERDEMOD_CONFIG_INTERRUPT_INV_LOCK_SEL,

    /**
     @brief TS buffer almost empty threshold. (0 - 2047)

            The default value is 0.
    */
    SONY_TUNERDEMOD_CONFIG_TS_BUFFER_ALMOST_EMPTY_THRESHOLD,

    /**
     @brief TS buffer almost full threshold. (0 - 2047)

            The default value is 418.
    */
    SONY_TUNERDEMOD_CONFIG_TS_BUFFER_ALMOST_FULL_THRESHOLD,

    /**
     @brief TS buffer read ready threshold. (0 - 2047)

            The default value is 1.
    */
    SONY_TUNERDEMOD_CONFIG_TS_BUFFER_RRDY_THRESHOLD,

    /**
     @brief Fixed clock mode setting. (For debug)

            Value:
            - 0:       Auto. (Default)
            - 1:       Clock mode A
            - 2:       Clock mode B
            - 3:       Clock mode C
    */
    SONY_TUNERDEMOD_CONFIG_FIXED_CLOCKMODE,

    /**
     @brief Configure the cable input (CAIN) is used or not.

            This parameter will make effect in next tuning.

            Values:
            - 0 : Air input (Default)
            - 1 : Cable input
    */
    SONY_TUNERDEMOD_CONFIG_CABLE_INPUT,

    /**
     @brief Disable/Enable FEF intermittent control for T2-base profile

            This parameter will make effect in next tuning.

            Values:
            - 0 : Disable
            - 1 : Enable (Default)
    */
    SONY_TUNERDEMOD_CONFIG_DVBT2_FEF_INTERMITTENT_BASE,

    /**
     @brief Disable/Enable FEF intermittent control for T2-Lite profile

            This parameter will make effect in next tuning.

            Values:
            - 0 : Disable
            - 1 : Enable (Default)
    */
    SONY_TUNERDEMOD_CONFIG_DVBT2_FEF_INTERMITTENT_LITE,

    /**
     @brief Configure the order in which systems are attempted in Blind Tune and Scan.

            This can be used to optimize scan duration where specific
            details on system split ratio are known about the spectrum.

            Value:
            - 0: DVB-T followed by DVBT2 (Default).
            - 1: DVB-T2 followed by DVBT.
    */
    SONY_TUNERDEMOD_CONFIG_BLINDTUNE_DVBT2_FIRST,

    /* -- BER period --- */
    /**
     @brief Set the measurement period for Pre-RS BER (DVB-T).

            This is a 5 bit value with a default of 11.
    */
    SONY_TUNERDEMOD_CONFIG_DVBT_BERN_PERIOD,

    /**
     @brief Set the measurement period for Pre-Viterbi BER (DVB-T).

            This is a 3 bit value with a default of 1.
    */
    SONY_TUNERDEMOD_CONFIG_DVBT_VBER_PERIOD,

    /**
     @brief Set the measurement period for PER (DVB-T).

            This is a 4 bit value with a default of 10.
    */
    SONY_TUNERDEMOD_CONFIG_DVBT_PER_MES,

    /**
     @brief Set the measurement period for Pre-BCH BER (DVB-T2) and Post-BCH FER (DVB-T2).

            This is a 4 bit value with a default of 8.
    */
    SONY_TUNERDEMOD_CONFIG_DVBT2_BBER_MES,

    /**
     @brief Set the measurement period for Pre-LDPC BER (DVB-T2).

            This is a 4 bit value with a default of 8.
    */
    SONY_TUNERDEMOD_CONFIG_DVBT2_LBER_MES,

    /**
     @brief Set the measurement period for PER (DVB-T2).

            This is a 4 bit value with a default of 10.
    */
    SONY_TUNERDEMOD_CONFIG_DVBT2_PER_MES,

    /**
     @brief Set the measurement period for Pre-RS and PER (ISDB-T).

            This is a 15 bit value with a default of 512.
    */
    SONY_TUNERDEMOD_CONFIG_ISDBT_BERPER_PERIOD

} sony_tunerdemod_config_id_t;

/**
 @brief Lock status.
*/
typedef enum {
    SONY_TUNERDEMOD_LOCK_RESULT_NOTDETECT, /**< Neither "Lock" nor "Unlock" conditions are met, lock status cannot be determined. */
    SONY_TUNERDEMOD_LOCK_RESULT_LOCKED,    /**< "Lock" condition is found. */
    SONY_TUNERDEMOD_LOCK_RESULT_UNLOCKED   /**< No signal was found or the signal was not the required system. */
} sony_tunerdemod_lock_result_t;

/**
 @brief Mode select for the multi purpose GPIO pins.
*/
typedef enum {
    /**
     @brief GPIO pin is configured as an output.
    */
    SONY_TUNERDEMOD_GPIO_MODE_OUTPUT = 0x00,

    /**
     @brief GPIO pin is configured as an input.
    */
    SONY_TUNERDEMOD_GPIO_MODE_INPUT = 0x01,

    /**
     @brief GPIO pin is configured to output interrupt which can be configured by
            ::sony_tunerdemod_SetConfig function with the config ID
            sony_tunerdemod_config_id_t::SONY_TUNERDEMOD_CONFIG_INTERRUPT.
    */
    SONY_TUNERDEMOD_GPIO_MODE_INT = 0x02,

    /**
     @brief GPIO pin is configured to output fec fail.
    */
    SONY_TUNERDEMOD_GPIO_MODE_FEC_FAIL = 0x03,

    /**
     @brief GPIO pin is configured to output an PWM signal which can be configured using the
            ::sony_tunerdemod_SetConfig function with the config ID
            sony_tunerdemod_config_id_t::SONY_TUNERDEMOD_CONFIG_PWM_VALUE.
    */
    SONY_TUNERDEMOD_GPIO_MODE_PWM = 0x04,

    /**
     @brief GPIO pin is configured to output EWS.
    */
    SONY_TUNERDEMOD_GPIO_MODE_EWS = 0x05,

    /**
     @brief GPIO pin is configured to output EEW.
    */
    SONY_TUNERDEMOD_GPIO_MODE_EEW = 0x06

} sony_tunerdemod_gpio_mode_t;

/**
 @brief TS serial clock frequency options
*/
typedef enum {
    SONY_TUNERDEMOD_SERIAL_TS_CLK_FULL,   /**< Full rate */
    SONY_TUNERDEMOD_SERIAL_TS_CLK_HALF    /**< Half rate */
} sony_tunerdemod_serial_ts_clk_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/**
 @brief The demodulator configuration memory table entry.

        Used to store a register or bit modification made through either
        the ::sony_tunerdemod_SetConfig or ::sony_tunerdemod_SetAndSaveRegisterBits APIs.
*/
typedef struct {
    sony_regio_target_t target;         /**< Register I/O target. */
    uint8_t bank;                       /**< Bank for register. */
    uint8_t address;                    /**< Register address. */
    uint8_t value;                      /**< Value to write to register. */
    uint8_t bitMask;                    /**< Bit mask to apply on the value. */
} sony_tunerdemod_config_memory_t;

/**
 @brief PID config
*/
typedef struct {
    /** Enable flag (0:Disable, 1:Enable) */
    uint8_t isEnable;
    /** PID value (13bit) */
    uint16_t pid;
} sony_tunerdemod_pid_config_t;

/**
 @brief PID filter configuration structure.
*/
typedef struct {
    /**
     @brief The logic of PID filter.

            - 0: Use this PID filter as Positive filter.
                 The packets which listed in this filter are passed.
            - 1: Use this PID filter as Negative filter.
                 The packets which listed in this PID filter are blocked.
    */
    uint8_t isNegative;

    /**
     @brief PID list
    */
    sony_tunerdemod_pid_config_t pidConfig[32];
} sony_tunerdemod_pid_filter_config_t;

/**
 @brief TS buffer information structure.
*/
typedef struct {
    uint8_t readReady;                  /**< Buffer read ready */
    uint8_t almostFull;                 /**< Buffer is almost full */
    uint8_t almostEmpty;                /**< Buffer is almost empty */
    uint8_t overflow;                   /**< Buffer is overflow */
    uint8_t underflow;                  /**< Buffer is underflow */
    uint16_t packetNum;                 /**< Number of packet in TS buffer */
} sony_tunerdemod_ts_buffer_info_t;

/**
 @brief LNA ON/OFF threshold setting for one frequency region.
*/
typedef struct {
    uint8_t off_on;                     /**< OFF -> ON threshold value. Default is 0x00. */
    uint8_t on_off;                     /**< ON -> OFF threshold value. Default is 0x08. */
} sony_tunerdemod_lna_threshold_t;

/**
 @brief LNA ON/OFF threshold setting table for air input.
*/
typedef struct {
    /**
     @brief Threshold data for each frequency region.

            [ 0] 856 <= Freq
            [ 1] 824 <= Freq < 856
            [ 2] 800 <= Freq < 824
            [ 3] 776 <= Freq < 800
            [ 4] 752 <= Freq < 776
            [ 5] 728 <= Freq < 752
            [ 6] 704 <= Freq < 728
            [ 7] 680 <= Freq < 704
            [ 8] 656 <= Freq < 680
            [ 9] 632 <= Freq < 656
            [10] 608 <= Freq < 632
            [11] 584 <= Freq < 608
            [12] 560 <= Freq < 584
            [13] 536 <= Freq < 560
            [14] 512 <= Freq < 536
            [15] 468 <= Freq < 512
            [16] 256 <= Freq < 468
            [17] 244 <= Freq < 256
            [18] 232 <= Freq < 244
            [19] 220 <= Freq < 232
            [20] 208 <= Freq < 220
            [21] 196 <= Freq < 208
            [22] 184 <= Freq < 196
            [23]        Freq < 184

            (Unit : MHz)
    */
    sony_tunerdemod_lna_threshold_t threshold[24];
} sony_tunerdemod_lna_threshold_table_air_t;

/**
 @brief LNA ON/OFF threshold setting table for cable input.
*/
typedef struct {
    /**
     @brief Threshold data for each frequency region.

            [ 0] 856 <= Freq
            [ 1] 824 <= Freq < 856
            [ 2] 800 <= Freq < 824
            [ 3] 776 <= Freq < 800
            [ 4] 752 <= Freq < 776
            [ 5] 728 <= Freq < 752
            [ 6] 704 <= Freq < 728
            [ 7] 680 <= Freq < 704
            [ 8] 656 <= Freq < 680
            [ 9] 632 <= Freq < 656
            [10] 608 <= Freq < 632
            [11] 584 <= Freq < 608
            [12] 560 <= Freq < 584
            [13] 536 <= Freq < 560
            [14] 512 <= Freq < 536
            [15] 468 <= Freq < 512
            [16] 472 <= Freq < 468
            [17] 440 <= Freq < 472
            [18] 412 <= Freq < 440
            [19] 384 <= Freq < 412
            [20] 356 <= Freq < 384
            [21] 328 <= Freq < 356
            [22] 300 <= Freq < 328
            [23] 272 <= Freq < 300
            [24] 244 <= Freq < 272
            [25] 216 <= Freq < 244
            [26] 188 <= Freq < 216
            [27] 160 <= Freq < 188
            [28] 132 <= Freq < 160
            [29] 104 <= Freq < 132
            [30] 76  <= Freq < 104
            [31]        Freq < 76

            (Unit : MHz)
    */
    sony_tunerdemod_lna_threshold_t threshold[32];
} sony_tunerdemod_lna_threshold_table_cable_t;

/**
 @brief The preset information for a ISDB-T signal.
*/
typedef struct sony_tunerdemod_isdbt_preset_info_t {
    uint8_t data[13];                   /**< Preset information for fast acquisition mode. */
} sony_tunerdemod_isdbt_preset_info_t;

/**
 @brief Parameters for ::sony_tunerdemod_Create().
*/
typedef struct {
    /**
     @brief TS output interface.
    */
    sony_tunerdemod_tsout_if_t tsOutputIF;

    /**
     @brief Enable/Disable internal LDO.
    */
    uint8_t enableInternalLDO;

    /**
     @brief Xtal share type.
    */
    sony_tunerdemod_xtal_share_t xtalShareType;

    /**
     @brief Driver current setting for crystal oscillator.

            This value is set to SLV-X bank 0x00 addr 0x13 [5:0].
            The default value is as follows. (depend on xtal share type)

            SONY_TUNERDEMOD_XTAL_SHARE_NONE   : 0x12
            SONY_TUNERDEMOD_XTAL_SHARE_EXTREF : 0x00 (Don't care. (ignored by HW))
            SONY_TUNERDEMOD_XTAL_SHARE_MASTER : 0x00
            SONY_TUNERDEMOD_XTAL_SHARE_SLAVE  : 0x00 (Don't care. (ignored by HW))

            This value may need to be changed depend on board environment.
    */
    uint8_t xosc_cap;

    /**
     @brief Driver current setting for crystal oscillator.

            This value is set to SLV-X bank 0x00 addr 0x14 [5:0].
            The default value is 0x08.
            This value may need to be changed depend on board environment.
    */
    uint8_t xosc_i;

    /**
     @brief Configure CXD2881GG or not.

            The driver should specify that used IC is CXD2881GG or not.
            (CXD2881GG is diver IC mainly for automotive.)

            0 : Not CXD2881GG
            1 : CXD2881GG
    */
    uint8_t isCXD2881GG;

} sony_tunerdemod_create_param_t;

/**
 @brief Parameters for ::sony_tunerdemod_diver_Create().
*/
typedef struct {
    /**
     @brief TS output mode.
    */
    sony_tunerdemod_tsout_if_t tsOutputIF;

    /**
     @brief Enable/Disable internal LDO.
    */
    uint8_t enableInternalLDO;

    /**
     @brief Driver current setting for crystal oscillator. (For Main IC)

            This value is set to main IC SLV-X bank 0x00 addr 0x13 [5:0].
            Default value is 0x00.
            This value may need to be changed depend on board environment.

            For Sub IC, this register value is ignored, so 0x00 is used.
    */
    uint8_t xosc_cap_main;

    /**
     @brief Driver current setting for crystal oscillator. (For Main IC)

            This value is set to SLV-X bank 0x00 addr 0x14 [5:0].
            The default value is 0x08.
            This value may need to be changed depend on board environment.
    */
    uint8_t xosc_i_main;

    /**
     @brief Driver current setting for crystal oscillator. (For Sub IC)

            This value is set to SLV-X bank 0x00 addr 0x14 [5:0].
            The default value is 0x08.
            This value may need to be changed depend on board environment.
    */
    uint8_t xosc_i_sub;

    /**
     @brief Configure CXD2881GG or not.

            The driver should specify that used IC is CXD2881GG or not.
            (CXD2881GG is diver IC mainly for automotive.)

            0 : Not CXD2881GG
            1 : CXD2881GG
    */
    uint8_t isCXD2881GG;

} sony_tunerdemod_diver_create_param_t;

/**
 @brief Tuner and Demodulator driver structure.

        The tuner and demodulator definition which allows control of the device
        through the defined set of functions.
*/
typedef struct sony_tunerdemod_t {

    /* --- Following members are NOT cleared by sony_tunerdemod_Initialize1 --- */

    /**
     @brief The pointer of driver for sub tuner in case of diversity.
    */
    struct sony_tunerdemod_t * pDiverSub;

    /**
     @brief Register I/O interface.
    */
    sony_regio_t * pRegio;

    /**
     @brief Parameters of ::sony_tunerdemod_Create().
    */
    sony_tunerdemod_create_param_t createParam;

    /**
     @brief Tuner and Demodulator diver mode.
    */
    sony_tunerdemod_divermode_t diverMode;

    /**
     @brief Fixed clock mode setting.

            Use ::sony_tunerdemod_SetConfig to configure this flag.
    */
    sony_tunerdemod_clockmode_t fixedClockMode;

    /**
     @brief Cable input is currently used.

            Use ::sony_tunerdemod_SetConfig to configure this flag.
    */
    uint8_t isCableInput;

    /**
     @brief FEF intermittent control for T2-base profile enable/disable.

            Use ::sony_tunerdemod_SetConfig to configure this flag.
    */
    uint8_t enableFEFIntermittentBase;

    /**
     @brief FEF intermittent control for T2-Lite profile enable/disable.

            Use ::sony_tunerdemod_SetConfig to configure this flag.
    */
    uint8_t enableFEFIntermittentLite;

    /**
     @brief The order in which Blind Tune attempts acquisition.

            This value can be configured using ::sony_tunerdemod_SetConfig
            with the SONY_TUNERDEMOD_CONFIG_TERR_BLINDTUNE_DVBT2_FIRST option.
    */
    uint8_t blindTuneDvbt2First;

    /**
     @brief RF level compensation function pointer.

            This function pointer is called from ::sony_tunerdemod_monitor_RFLevel.
            If RF level compensation is necessary, please set this function pointer
            using ::sony_tunerdemod_SetRFLevelCompensation function.

     @param pTunerDemod The driver instance.
     @param pRFLeveldB The RF Level estimation in dB * 1000.

     @return SONY_RESULT_OK if successful.
    */
    sony_result_t (* RFLevelCompensation) (struct sony_tunerdemod_t * pTunerDemod, int * pRFLeveldB);

    /**
     @brief LNA ON/OFF threshold setting for air input.

            If this pointer is NULL, default setting is automatically used.
            Please use sony_tunerdemod_SetLNAThreshold function to set this pointer.
    */
    sony_tunerdemod_lna_threshold_table_air_t * pLNAThresholdTableAir;

    /**
     @brief LNA ON/OFF threshold setting for cable input.

            If this pointer is NULL, default setting is automatically used.
            Please use sony_tunerdemod_SetLNAThreshold function to set this pointer.
    */
    sony_tunerdemod_lna_threshold_table_cable_t * pLNAThresholdTableCable;

    /**
     @brief The serial TS clock mode for all active states.

            This is configured using ::sony_tunerdemod_SetConfig
            with the SONY_TUNERDEMOD_CONFIG_TSCLK_CONT option.
    */
    uint8_t serialTsClockModeContinuous;

    /**
     @brief The serial TS clock frequency option for terrestrial active states.

            This is configured using ::sony_tunerdemod_SetConfig with the
            SONY_TUNERDEMOD_CONFIG_TSCLK_FREQ option.
    */
    sony_tunerdemod_serial_ts_clk_t serialTsClkFreq;

    /**
     @brief The TS clock Manual setting option for all active states.

            This is configured using ::sony_tunerdemod_SetConfig with the
            SONY_TUNERDEMOD_CONFIG_TSCLK_MANUAL option.
    */
    uint8_t tsByteClkManualSetting;

    /**
     @brief TS backwards compatible mode state.

            This is configured using ::sony_tunerdemod_SetConfig
            with the SONY_TUNERDEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE.
    */
    uint8_t isTsBackwardsCompatibleMode;

    /**
     @brief A table of the configuration.

            This table is stored by ::sony_tunerdemod_SetConfig
            and ::sony_tunerdemod_SetAndSaveRegisterBits functions.
    */
    sony_tunerdemod_config_memory_t configMemory[SONY_TUNERDEMOD_MAX_CONFIG_MEMORY_COUNT];

    /**
     @brief The index of the last valid entry in the configMemory table.
    */
    uint8_t configMemoryLastEntry;

    /**
     @brief PID filter configuration to be used in tuning APIs.

            This is configured using ::sony_tunerdemod_SetPIDFilter.
    */
    sony_tunerdemod_pid_filter_config_t pidFilterConfig;

    /**
     @brief PID filter configuration (pidFilterConfig) is enabled or not.

            This is configured using ::sony_tunerdemod_SetPIDFilter.
    */
    uint8_t pidFilterConfigEnable;

    /**
     @brief ISDB-T preset information to be used in ISDB-T tuning APIs.

            This is configured using ::sony_tunerdemod_isdbt_SetPreset.
    */
    sony_tunerdemod_isdbt_preset_info_t isdbtPresetInfo;

    /**
     @brief ISDB-T preset information (isdbtPresetInfo) is enabled or not.

            This is configured using ::sony_tunerdemod_isdbt_SetPreset.
    */
    uint8_t isdbtPresetInfoEnable;

    /**
     @brief User defined data.
    */
    void * user;

    /* --- Following members are cleared by sony_tunerdemod_Initialize1 (Temporary information) --- */

    /**
     @brief Chip ID.
    */
    sony_tunerdemod_chip_id_t chipID;

    /**
     @brief Tuner and Demodulator software state.
    */
    sony_tunerdemod_state_t state;

    /**
     @brief Tuner and Demodulator clock mode. (A/B/C)
    */
    sony_tunerdemod_clockmode_t clockMode;

    /**
     @brief Current RF frequency (kHz).
    */
    unsigned int frequencyKHz;

    /**
     @brief The current system.
    */
    sony_dtv_system_t system;

    /**
     @brief The current bandwidth.
    */
    sony_dtv_bandwidth_t bandwidth;

    /**
     @brief Scan mode enable/disable flag.
    */
    uint8_t scanMode;

    /**
     @brief Cancellation indicator variable.
    */
    sony_atomic_t cancel;

} sony_tunerdemod_t;

/*------------------------------------------------------------------------------
  Functions
------------------------------------------------------------------------------*/
/**
 @brief Create a driver instance.

        This MUST be called before calling ::sony_integ_Initialize.

 @param pTunerDemod Reference to memory allocated for the demodulator instance.
                    The create function will setup this demodulator instance.
 @param pRegio Register I/O interface.
 @param pCreateParam Parameters to create this driver.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_Create (sony_tunerdemod_t * pTunerDemod,
                                      sony_regio_t * pRegio,
                                      sony_tunerdemod_create_param_t * pCreateParam);

/**
 @brief Create a driver instance for diversity system.

        This MUST be called before calling ::sony_integ_Initialize.

 @param pTunerDemodMain Reference to memory allocated for the demodulator instance for main tuner.
                    The create function will setup this demodulator instance.
 @param pRegioMain Register I/O interface for main tuner.
 @param pTunerDemodSub Reference to memory allocated for the demodulator instance for sub tuner.
                    The create function will setup this demodulator instance.
 @param pRegioSub Register I/O interface for sub tuner.
 @param pCreateParam Parameters to create this driver.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_diver_Create (sony_tunerdemod_t * pTunerDemodMain,
                                            sony_regio_t * pRegioMain,
                                            sony_tunerdemod_t * pTunerDemodSub,
                                            sony_regio_t * pRegioSub,
                                            sony_tunerdemod_diver_create_param_t * pCreateParam);

/**
 @brief Initialize the device and into Sleep state. (1st step)

        Initialization is separated in sony_tunerdemod_Initialize1 and sony_tunerdemod_Initialize2
        because need long sleep between them.
        These APIs are called from ::sony_integ_Initialize.
        To know how to call these APIs, please check ::sony_integ_Initialize code.

        These APIs do not clear configuration information stored in sony_tunerdemod_t.
        (In member variables and config memory.)
        To clear all settings stored in sony_tunerdemod_t, please call sony_tunerdemod_Create
        or sony_tunerdemod_diver_Create.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_Initialize1 (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Initialize the device and into Sleep state. (2nd step)

        Initialization is separated in sony_tunerdemod_Initialize1 and sony_tunerdemod_Initialize2
        because need long sleep between them.
        These APIs are called from ::sony_integ_Initialize.
        To know how to call these APIs, please check ::sony_integ_Initialize code.

        These APIs do not clear configuration information stored in sony_tunerdemod_t.
        (In member variables and config memory.)
        To clear all settings stored in sony_tunerdemod_t, please call sony_tunerdemod_Create
        or sony_tunerdemod_diver_Create.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_Initialize2 (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Check internal CPU status that internal CPU's task is completed or not.

        In diver system, both main and sub IC status will be checked.

 @param pTunerDemod The driver instance.
 @param pTaskCompleted Internal CPU task status. (0: not completed, 1: completed)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_CheckInternalCPUStatus (sony_tunerdemod_t * pTunerDemod, uint8_t * pTaskCompleted);

/**
 @brief Common (broadcasting system independent) state transition for tuning.

        Called internally as part of each Sleep to Active state transition.
        The user should not call these APIs.
        Please call sony_tunerdemod_xxx_Tune APIs for Sleep to Active state transition.

 @param pTunerDemod The driver instance.
 @param system The broadcasting system to be tuned.
 @param frequencyKHz Frequency to be tuned.
 @param bandwidth Bandwidth to be tuned.
 @param oneSegmentOptimize One segment optimization mode enable/disable. (only for ISDB-T)
 @param oneSegmentOptimizeShiftDirection Frequency shift direction for one segment optimization setting. (only for ISDB-T)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_CommonTuneSetting1 (sony_tunerdemod_t * pTunerDemod, sony_dtv_system_t system,
                                                  unsigned int frequencyKHz, sony_dtv_bandwidth_t bandwidth,
                                                  uint8_t oneSegmentOptimize, uint8_t oneSegmentOptimizeShiftDirection);

/**
 @brief Common (broadcasting system independent) state transition for tuning.

        Called internally as part of each Sleep to Active state transition.
        The user should not call these APIs.
        Please call sony_tunerdemod_xxx_Tune APIs for Sleep to Active state transition.

 @param pTunerDemod The driver instance.
 @param system The broadcasting system to be tuned.
 @param enableFEFIntermittentControl FEF intermittent control enable/disable (Only for T2)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_CommonTuneSetting2 (sony_tunerdemod_t * pTunerDemod, sony_dtv_system_t system,
                                                  uint8_t enableFEFIntermittentControl);

/**
 @brief Put the demodulator into Sleep state.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_Sleep (sony_tunerdemod_t * pTunerDemod);

/**
 @brief Set configuration options on the demodulator.

        This API should be called after initialization API (sony_integ_Initialize).
        All of configurations by this API should be done in Sleep or Active states.

        All configurations done by this API are stored to configration memory table
        in sony_tunerdemod_t struct instance.
        Since CXD2880 is for mobile, the host can turn off CXD2880 power if TV tuner is not used for long time.
        And then, when the host turn on CXD2880 power and calls initialization API again,
        the configuration memory table will be loaded back into the device automatically.
        So the user application do not need to call this API again in 2nd power on step.

 @param pTunerDemod The driver instance.
 @param id The configuration ID to set. See ::sony_tunerdemod_config_id_t.
 @param value The associated value. Depends on the configId.

 @return SONY_RESULT_OK if successfully set the configuration option.
*/
sony_result_t sony_tunerdemod_SetConfig (sony_tunerdemod_t * pTunerDemod,
                                         sony_tunerdemod_config_id_t id,
                                         int value);

/**
 @brief Setup the GPIO.

 @note  If the driver is configured for diver, sony_tunerdemod_GPIOSetConfig configures
        main IC GPIO, and sony_tunerdemod_GPIOSetConfig_sub configures sub IC GPIO.

 @param pTunerDemod The driver instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param enable Set enable (1) or disable (0).
 @param mode GPIO pin mode.
 @param openDrain Open drain control (0: normal, 1: open drain)
 @param invert Logic of the pin (0: normal, 1: inverted)

 @note If openDrain == 1 && invert == 0  ->  OD output is set in L case of output signal.
       If openDrain == 1 && invert == 1  ->  OD output is set in H case of output signal.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_GPIOSetConfig (sony_tunerdemod_t * pTunerDemod,
                                             uint8_t id,
                                             uint8_t enable,
                                             sony_tunerdemod_gpio_mode_t mode,
                                             uint8_t openDrain,
                                             uint8_t invert);

/**
 @brief Setup the GPIO. (Sub IC)

 @note  If the driver is configured for diver, sony_tunerdemod_GPIOSetConfig configures
        main IC GPIO, and sony_tunerdemod_GPIOSetConfig_sub configures sub IC GPIO.

 @param pTunerDemod The driver instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param enable Set enable (1) or disable (0).
 @param mode GPIO pin mode.
 @param openDrain Open drain control (0: normal, 1: open drain)
 @param invert Logic of the pin (0: normal, 1: inverted)

 @note If openDrain == 1 && invert == 0  ->  OD output is set in L case of output signal.
       If openDrain == 1 && invert == 1  ->  OD output is set in H case of output signal.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_GPIOSetConfig_sub (sony_tunerdemod_t * pTunerDemod,
                                                 uint8_t id,
                                                 uint8_t enable,
                                                 sony_tunerdemod_gpio_mode_t mode,
                                                 uint8_t openDrain,
                                                 uint8_t invert);

/**
 @brief Read the GPIO value.

        The GPIO should have been configured as an input (Read) GPIO.

 @note  If the driver is configured for diver, sony_tunerdemod_GPIORead returns
        main IC value, and sony_tunerdemod_GPIORead_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param pValue The current value of the GPIO.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_GPIORead (sony_tunerdemod_t * pTunerDemod,
                                        uint8_t id,
                                        uint8_t * pValue);

/**
 @brief Read the GPIO value. (Sub IC)

        The GPIO should have been configured as an input (Read) GPIO.

 @note  If the driver is configured for diver, sony_tunerdemod_GPIORead returns
        main IC value, and sony_tunerdemod_GPIORead_sub returns sub IC value.

 @param pTunerDemod The driver instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param pValue The current value of the GPIO.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_GPIORead_sub (sony_tunerdemod_t * pTunerDemod,
                                            uint8_t id,
                                            uint8_t * pValue);

/**
 @brief Write the GPIO value.

        The GPIO should have been configured as an output (Write) GPIO.

 @note  If the driver is configured for diver, sony_tunerdemod_GPIOWrite configures
        main IC GPIO, and sony_tunerdemod_GPIOWrite_sub configures sub IC GPIO.

 @param pTunerDemod The driver instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param value The value to set as output.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_GPIOWrite (sony_tunerdemod_t * pTunerDemod,
                                         uint8_t id,
                                         uint8_t value);

/**
 @brief Write the GPIO value. (Sub IC)

        The GPIO should have been configured as an output (Write) GPIO.

 @note  If the driver is configured for diver, sony_tunerdemod_GPIOWrite configures
        main IC GPIO, and sony_tunerdemod_GPIOWrite_sub configures sub IC GPIO.

 @param pTunerDemod The driver instance.
 @param id GPIO number (0 or 1 or 2 ).
 @param value The value to set as output.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_GPIOWrite_sub (sony_tunerdemod_t * pTunerDemod,
                                             uint8_t id,
                                             uint8_t value);

/**
 @brief Read interrupt cause.

 @param pTunerDemod The driver instance.
 @param pValue The cause of interruput.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_InterruptRead (sony_tunerdemod_t * pTunerDemod,
                                             uint16_t * pValue);

/**
 @brief Clear interrupt register.

 @param pTunerDemod The driver instance.
 @param value Set "1" to the bits which you want to clear.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_InterruptClear (sony_tunerdemod_t * pTunerDemod,
                                              uint16_t value);

/**
 @brief Clear TS buffer and Overflow/Underflow flags.

        This function is valid only for SPI/SDIO.

 @param pTunerDemod The driver instance.
 @param clearOverflowFlag Clear overflow flag.
 @param clearUnderflowFlag Clear underflow flag.
 @param clearBuffer Clear TS buffer.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_TSBufferClear (sony_tunerdemod_t * pTunerDemod,
                                             uint8_t clearOverflowFlag,
                                             uint8_t clearUnderflowFlag,
                                             uint8_t clearBuffer);

/**
 @brief Get the Chip ID of the connected IC.

 @param pTunerDemod The driver instance.
 @param pChipID Pointer to receive the IP ID into.

 @return SONY_RESULT_OK if successful and pChipID valid.
*/
sony_result_t sony_tunerdemod_ChipID (sony_tunerdemod_t * pTunerDemod,
                                      sony_tunerdemod_chip_id_t * pChipID);

/**
 @brief Set a specific value with bit mask to any demod register.

 @note  This API should only be used under instruction from Sony support.
        Manually modifying any demodulator register could have a negative
        effect for performance or basic functionality.

 @param pTunerDemod The driver instance.
 @param target Target regio I/F.
 @param bank Register bank of configuration setting.
 @param address Register address of configuration setting.
 @param value The value being written to this register.
 @param bitMask The bit mask used on the register.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetAndSaveRegisterBits (sony_tunerdemod_t * pTunerDemod,
                                                      sony_regio_target_t target,
                                                      uint8_t bank,
                                                      uint8_t address,
                                                      uint8_t value,
                                                      uint8_t bitMask);

/**
 @brief Enable / disable scan mode for acquisition in the demodulator.

 @param pTunerDemod The driver instance.
 @param system The system used for scanning.
 @param scanModeEnabled State of scan mode to set.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetScanMode (sony_tunerdemod_t * pTunerDemod,
                                           sony_dtv_system_t system,
                                           uint8_t scanModeEnabled);

/**
 @brief Set PID filter configuration.

 @param pTunerDemod The driver instance.
 @param pPIDFilterConfig The PID filter configuration.
                         If you set NULL, PID filter is disabled.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetPIDFilter (sony_tunerdemod_t * pTunerDemod,
                                            sony_tunerdemod_pid_filter_config_t * pPIDFilterConfig);

/**
 @brief Set RF level compensation function.

        If RF level compensation is necessary, the user can specify own compensation function pointer.
        The function pointer is called from ::sony_tunerdemod_monitor_RFLevel to compensate RF level.
        If NULL is used, RF level compensation will be disabled.

 @note  If the driver is configured for diver, sony_tunerdemod_SetRFLevelCompensation should be used for main IC,
        and sony_tunerdemod_SetRFLevelCompensation_sub should be used for sub IC.

 @param pTunerDemod The driver instance.
 @param pRFLevelCompensation User defined RF level compensation function.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetRFLevelCompensation (sony_tunerdemod_t * pTunerDemod,
                                                      sony_result_t (* pRFLevelCompensation) (sony_tunerdemod_t *, int *));

/**
 @brief Set RF level compensation function. (Sub IC)

        If RF level compensation is necessary, the user can specify own compensation function pointer.
        The function pointer is called from ::sony_tunerdemod_monitor_RFLevel_sub to compensate RF level.
        If NULL is used, RF level compensation will be disabled.

 @note  If the driver is configured for diver, sony_tunerdemod_SetRFLevelCompensation should be used for main IC,
        and sony_tunerdemod_SetRFLevelCompensation_sub should be used for sub IC.

 @param pTunerDemod The driver instance.
 @param pRFLevelCompensation User defined RF level compensation function.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetRFLevelCompensation_sub (sony_tunerdemod_t * pTunerDemod,
                                                          sony_result_t (* pRFLevelCompensation) (sony_tunerdemod_t *, int *));

/**
 @brief Set LNA ON/OFF threshold table.

        This is advanced function to optimize IC internal LNA ON/OFF threshold.
        If NULL is passed, default value will be used.
        This function should be called before sony_integ_Initialize.

 @note  If the driver is configured for diver, sony_tunerdemod_SetLNAThreshold should be used for main IC,
        and sony_tunerdemod_SetLNAThreshold_sub should be used for sub IC.

 @param pTunerDemod The driver instance.
 @param pTableAir LNA ON/OFF threshold table address for air input. (NULL : use default value)
 @param pTableCable LNA ON/OFF threshold table address for cable input. (NULL : use default value)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetLNAThreshold (sony_tunerdemod_t * pTunerDemod,
                                               sony_tunerdemod_lna_threshold_table_air_t * pTableAir,
                                               sony_tunerdemod_lna_threshold_table_cable_t * pTableCable);

/**
 @brief Set LNA ON/OFF threshold table. (Sub IC)

        This is advanced function to optimize IC internal LNA ON/OFF threshold.
        If NULL is passed, default value will be used.
        This function should be called before sony_integ_Initialize.

 @note  If the driver is configured for diver, sony_tunerdemod_SetLNAThreshold should be used for main IC,
        and sony_tunerdemod_SetLNAThreshold_sub should be used for sub IC.

 @param pTunerDemod The driver instance.
 @param pTableAir LNA ON/OFF threshold table address for air input. (NULL : use default value)
 @param pTableCable LNA ON/OFF threshold table address for cable input. (NULL : use default value)

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetLNAThreshold_sub (sony_tunerdemod_t * pTunerDemod,
                                                   sony_tunerdemod_lna_threshold_table_air_t * pTableAir,
                                                   sony_tunerdemod_lna_threshold_table_cable_t * pTableCable);

/**
 @brief TS pins High/Low test.

        The user can set each TS pins High/Low for debug/test purpose.
        The "value" argument is ORed value of each pin setting.

        bit flags: ( can be bitwise ORed )
        - Bit[0] : TSCLK pin
        - Bit[1] : TSDATA pin
        - Bit[2] : TSSYNC pin
        - Bit[3] : TSVALID pin
        - Bit[4] : SDDATA1 pin (TSERR)

        bit meaning:
        - 0 : Set Low
        - 1 : Set High

 @note  This API is available in SLEEP state only.
        After TS pins High/Low test, the user should call this API again with enable == 0
        to roll back TS pin settings.

 @param pTunerDemod The driver instance.
 @param enable Enable or disable TS pins High/Low setting.
 @param value TS pins setting described above.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetTSPinHighLow (sony_tunerdemod_t * pTunerDemod, uint8_t enable, uint8_t value);

/**
 @brief TS output setting (enable/disable).

 @note  This API is called internally.
        This need not be manually called from user application.
        This API is remained as public because some user need to stop TS for testing purpose.

 @param pTunerDemod The driver instance.
 @param enable TS output setting.
               - 0: Disable
               - 1: Enable

 @return SONY_RESULT_OK if successful.
*/
sony_result_t sony_tunerdemod_SetTSOutput (sony_tunerdemod_t * pTunerDemod,
                                           uint8_t enable);

/**
 @brief Freeze all registers in SLV-T.

        This API is used by the monitor functions to ensure multiple separate
        register reads are from the same snapshot.

 @note  This should not be manually called or additional instances added into the driver unless under specific instruction.

 @param pTunerDemod The driver instance.

 @return SONY_RESULT_OK if successful.
*/
sony_result_t SLVT_FreezeReg(sony_tunerdemod_t * pTunerDemod);

#endif /* SONY_TUNERDEMOD_H */
