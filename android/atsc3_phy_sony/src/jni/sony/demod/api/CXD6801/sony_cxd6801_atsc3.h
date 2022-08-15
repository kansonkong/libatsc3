/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/09/28
  Modification ID : 1ea04def104d657deb64f0d0b0cae6ef992a9dd8
------------------------------------------------------------------------------*/
/**
 @file    sony_atsc3.h

          This file provides ATSC 3.0 related type definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_ATSC3_H
#define SONY_CXD6801_ATSC3_H

#include "sony_cxd6801_common.h"

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
#define SONY_CXD6801_ATSC3_NUM_PLP_MAX        64  /**< Max number of PLPs */

/*------------------------------------------------------------------------------
 Enumerations
------------------------------------------------------------------------------*/
/**
 @brief System bandwidth signaled in bootstrap.
*/
typedef enum {
	SONY_CXD6801_ATSC3_SYSTEM_BW_6_MHZ,       /**< 6MHz */
	SONY_CXD6801_ATSC3_SYSTEM_BW_7_MHZ,       /**< 7MHz */
	SONY_CXD6801_ATSC3_SYSTEM_BW_8_MHZ,       /**< 8MHz */
	SONY_CXD6801_ATSC3_SYSTEM_BW_OVER_8_MHZ   /**< Greater than 8 MHz */
} sony_cxd6801_atsc3_system_bw_t;

/**
 @brief Preamble pilot pattern signaled in bootstrap.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PREAMBLE_PILOT_3,      /**< Dx = 3 */
	SONY_CXD6801_ATSC3_PREAMBLE_PILOT_4,      /**< Dx = 4 */
	SONY_CXD6801_ATSC3_PREAMBLE_PILOT_6,      /**< Dx = 6 */
	SONY_CXD6801_ATSC3_PREAMBLE_PILOT_8,      /**< Dx = 8 */
	SONY_CXD6801_ATSC3_PREAMBLE_PILOT_12,     /**< Dx = 12 */
	SONY_CXD6801_ATSC3_PREAMBLE_PILOT_16,     /**< Dx = 16 */
	SONY_CXD6801_ATSC3_PREAMBLE_PILOT_24,     /**< Dx = 24 */
	SONY_CXD6801_ATSC3_PREAMBLE_PILOT_32,     /**< Dx = 32 */
} sony_cxd6801_atsc3_preamble_pilot_t;

/**
 @brief L1-Basic fec type signaled in bootstrap.
*/
typedef enum {
	SONY_CXD6801_ATSC3_L1B_FEC_TYPE_MODE1,    /**< Mode 1 */
	SONY_CXD6801_ATSC3_L1B_FEC_TYPE_MODE2,    /**< Mode 2 */
	SONY_CXD6801_ATSC3_L1B_FEC_TYPE_MODE3,    /**< Mode 3 */
	SONY_CXD6801_ATSC3_L1B_FEC_TYPE_MODE4,    /**< Mode 4 */
	SONY_CXD6801_ATSC3_L1B_FEC_TYPE_MODE5,    /**< Mode 5 */
	SONY_CXD6801_ATSC3_L1B_FEC_TYPE_MODE6,    /**< Mode 6 */
	SONY_CXD6801_ATSC3_L1B_FEC_TYPE_MODE7,    /**< Mode 7 */
	SONY_CXD6801_ATSC3_L1B_FEC_TYPE_RESERVED  /**< Reserved */
} sony_cxd6801_atsc3_l1b_fec_type_t;

/**
 @brief Presence or absence of timing information, and precision indicator signaled in L1-Basic.
*/
typedef enum {
	SONY_CXD6801_ATSC3_TIME_INFO_FLAG_NONE,   /**< Time information is not included */
	SONY_CXD6801_ATSC3_TIME_INFO_FLAG_MS,     /**< ms precision */
	SONY_CXD6801_ATSC3_TIME_INFO_FLAG_US,     /**< us precision */
	SONY_CXD6801_ATSC3_TIME_INFO_FLAG_NS      /**< ns precision */
} sony_cxd6801_atsc3_time_info_flag_t;

/**
 @brief PAPR (Peak to Average Power Ratio) reduction indicator signaled in L1-Basic.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PAPR_NONE,    /**< No PAPR reduction used */
	SONY_CXD6801_ATSC3_PAPR_TR,      /**< Tone reservation only */
	SONY_CXD6801_ATSC3_PAPR_ACE,     /**< ACE (Active Constellation Extension) only */
	SONY_CXD6801_ATSC3_PAPR_TR_ACE   /**< Both TR and ACE */
} sony_cxd6801_atsc3_papr_t;

/**
 @brief L1-Detail fec type signaled in L1-Basic.
*/
typedef enum {
	SONY_CXD6801_ATSC3_L1D_FEC_TYPE_MODE1,    /**< Mode 1 */
	SONY_CXD6801_ATSC3_L1D_FEC_TYPE_MODE2,    /**< Mode 2 */
	SONY_CXD6801_ATSC3_L1D_FEC_TYPE_MODE3,    /**< Mode 3 */
	SONY_CXD6801_ATSC3_L1D_FEC_TYPE_MODE4,    /**< Mode 4 */
	SONY_CXD6801_ATSC3_L1D_FEC_TYPE_MODE5,    /**< Mode 5 */
	SONY_CXD6801_ATSC3_L1D_FEC_TYPE_MODE6,    /**< Mode 6 */
	SONY_CXD6801_ATSC3_L1D_FEC_TYPE_MODE7,    /**< Mode 7 */
	SONY_CXD6801_ATSC3_L1D_FEC_TYPE_RESERVED  /**< Reserved */
} sony_cxd6801_atsc3_l1d_fec_type_t;

/**
 @brief MISO (Multiple Input Single Output) option used in L1-Basic and L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_MISO_NONE,             /**< No MISO */
	SONY_CXD6801_ATSC3_MISO_64,               /**< MISO with 64 coefficients */
	SONY_CXD6801_ATSC3_MISO_256,              /**< MISO with 256 coefficients */
	SONY_CXD6801_ATSC3_MISO_RESERVED          /**< Reserved */
} sony_cxd6801_atsc3_miso_t;

/**
 @brief FFT size indicator used in L1-Basic and L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_FFT_SIZE_8K,        /**< 8K */
	SONY_CXD6801_ATSC3_FFT_SIZE_16K,       /**< 16K */
	SONY_CXD6801_ATSC3_FFT_SIZE_32K,       /**< 32K */
	SONY_CXD6801_ATSC3_FFT_SIZE_RESERVED   /**< Reserved */
} sony_cxd6801_atsc3_fft_size_t;

/**
 @brief Guard interval indicator used in L1-Basic and L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_GI_RESERVED_0,         /**< Reserved */
	SONY_CXD6801_ATSC3_GI_1_192,              /**< 1/192 */
	SONY_CXD6801_ATSC3_GI_2_384,              /**< 2/384 */
	SONY_CXD6801_ATSC3_GI_3_512,              /**< 3/512 */
	SONY_CXD6801_ATSC3_GI_4_768,              /**< 4/768 */
	SONY_CXD6801_ATSC3_GI_5_1024,             /**< 5/1024 */
	SONY_CXD6801_ATSC3_GI_6_1536,             /**< 6/1536 */
	SONY_CXD6801_ATSC3_GI_7_2048,             /**< 7/2048 */
	SONY_CXD6801_ATSC3_GI_8_2432,             /**< 8/2432 */
	SONY_CXD6801_ATSC3_GI_9_3072,             /**< 9/3072 */
	SONY_CXD6801_ATSC3_GI_10_3648,            /**< 10/3648 */
	SONY_CXD6801_ATSC3_GI_11_4096,            /**< 11/4096 */
	SONY_CXD6801_ATSC3_GI_12_4864,            /**< 12/4864 */
	SONY_CXD6801_ATSC3_GI_RESERVED_13,        /**< Reserved */
	SONY_CXD6801_ATSC3_GI_RESERVED_14,        /**< Reserved */
	SONY_CXD6801_ATSC3_GI_RESERVED_15         /**< Reserved */
} sony_cxd6801_atsc3_gi_t;

/**
 @brief Scattered pilot pattern indicator signaled in L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_SP_3_2,          /**< SP3_2 for SISO,  MP3_2 for MISO */
	SONY_CXD6801_ATSC3_SP_3_4,          /**< SP3_4 for SISO,  MP3_4 for MISO */
	SONY_CXD6801_ATSC3_SP_4_2,          /**< SP4_2 for SISO,  MP4_2 for MISO */
	SONY_CXD6801_ATSC3_SP_4_4,          /**< SP4_4 for SISO,  MP4_4 for MISO */
	SONY_CXD6801_ATSC3_SP_6_2,          /**< SP6_2 for SISO,  MP6_2 for MISO */
	SONY_CXD6801_ATSC3_SP_6_4,          /**< SP6_4 for SISO,  MP6_4 for MISO */
	SONY_CXD6801_ATSC3_SP_8_2,          /**< SP8_2 for SISO,  MP8_2 for MISO */
	SONY_CXD6801_ATSC3_SP_8_4,          /**< SP8_4 for SISO,  MP8_4 for MISO */
	SONY_CXD6801_ATSC3_SP_12_2,         /**< SP12_2 for SISO, MP12_2 for MISO */
	SONY_CXD6801_ATSC3_SP_12_4,         /**< SP12_4 for SISO, MP12_4 for MISO */
	SONY_CXD6801_ATSC3_SP_16_2,         /**< SP16_2 for SISO, MP16_2 for MISO */
	SONY_CXD6801_ATSC3_SP_16_4,         /**< SP16_4 for SISO, MP16_4 for MISO */
	SONY_CXD6801_ATSC3_SP_24_2,         /**< SP24_2 for SISO, MP24_2 for MISO */
	SONY_CXD6801_ATSC3_SP_24_4,         /**< SP24_4 for SISO, MP24_4 for MISO */
	SONY_CXD6801_ATSC3_SP_32_2,         /**< SP32_2 for SISO, MP32_2 for MISO */
	SONY_CXD6801_ATSC3_SP_32_4,         /**< SP32_4 for SISO, MP32_4 for MISO */
	SONY_CXD6801_ATSC3_SP_RESERVED_16,  /**< Reserved */
	SONY_CXD6801_ATSC3_SP_RESERVED_17,  /**< Reserved */
	SONY_CXD6801_ATSC3_SP_RESERVED_18,  /**< Reserved */
	SONY_CXD6801_ATSC3_SP_RESERVED_19,  /**< Reserved */
	SONY_CXD6801_ATSC3_SP_RESERVED_20,  /**< Reserved */
	SONY_CXD6801_ATSC3_SP_RESERVED_21,  /**< Reserved */
	SONY_CXD6801_ATSC3_SP_RESERVED_22,  /**< Reserved */
	SONY_CXD6801_ATSC3_SP_RESERVED_23   /**< Reserved */
} sony_cxd6801_atsc3_sp_t;

/**
 @brief PLP FEC type definition signaled in L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PLP_FEC_BCH_LDPC_16K,  /**< BCH + 16K LDPC */
	SONY_CXD6801_ATSC3_PLP_FEC_BCH_LDPC_64K,  /**< BCH + 64K LDPC */
	SONY_CXD6801_ATSC3_PLP_FEC_CRC_LDPC_16K,  /**< CRC + 16K LDPC */
	SONY_CXD6801_ATSC3_PLP_FEC_CRC_LDPC_64K,  /**< CRC + 64K LDPC */
	SONY_CXD6801_ATSC3_PLP_FEC_LDPC_16K,      /**< 16K LDPC only */
	SONY_CXD6801_ATSC3_PLP_FEC_LDPC_64K,      /**< 64K LDPC only */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_6,    /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_7,    /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_8,    /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_9,    /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_10,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_11,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_12,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_13,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_14,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_FEC_RESERVED_15    /**< Reserved */
} sony_cxd6801_atsc3_plp_fec_t;

/**
 @brief PLP modulation signaled in L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PLP_MOD_QPSK,          /**< QPSK */
	SONY_CXD6801_ATSC3_PLP_MOD_16QAM,         /**< 16QAM-NUC */
	SONY_CXD6801_ATSC3_PLP_MOD_64QAM,         /**< 64QAM-NUC */
	SONY_CXD6801_ATSC3_PLP_MOD_256QAM,        /**< 256QAM-NUC */
	SONY_CXD6801_ATSC3_PLP_MOD_1024QAM,       /**< 1024QAM-NUC */
	SONY_CXD6801_ATSC3_PLP_MOD_4096QAM,       /**< 4096QAM-NUC */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_6,    /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_7,    /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_8,    /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_9,    /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_10,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_11,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_12,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_13,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_14,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_MOD_RESERVED_15    /**< Reserved */
} sony_cxd6801_atsc3_plp_mod_t;

/**
 @brief PLP code rate signaled in L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PLP_COD_2_15,          /**< 2/15 */
	SONY_CXD6801_ATSC3_PLP_COD_3_15,          /**< 3/15 */
	SONY_CXD6801_ATSC3_PLP_COD_4_15,          /**< 4/15 */
	SONY_CXD6801_ATSC3_PLP_COD_5_15,          /**< 5/15 */
	SONY_CXD6801_ATSC3_PLP_COD_6_15,          /**< 6/15 */
	SONY_CXD6801_ATSC3_PLP_COD_7_15,          /**< 7/15 */
	SONY_CXD6801_ATSC3_PLP_COD_8_15,          /**< 8/15 */
	SONY_CXD6801_ATSC3_PLP_COD_9_15,          /**< 9/15 */
	SONY_CXD6801_ATSC3_PLP_COD_10_15,         /**< 10/15 */
	SONY_CXD6801_ATSC3_PLP_COD_11_15,         /**< 11/15 */
	SONY_CXD6801_ATSC3_PLP_COD_12_15,         /**< 12/15 */
	SONY_CXD6801_ATSC3_PLP_COD_13_15,         /**< 13/15 */
	SONY_CXD6801_ATSC3_PLP_COD_RESERVED_12,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_COD_RESERVED_13,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_COD_RESERVED_14,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_COD_RESERVED_15    /**< Reserved */
} sony_cxd6801_atsc3_plp_cod_t;

/**
 @brief PLP time interleaving mode signaled in L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PLP_TI_NONE,           /**< No time interleaving mode */
	SONY_CXD6801_ATSC3_PLP_TI_CTI,            /**< Convolutional time interleaving */
	SONY_CXD6801_ATSC3_PLP_TI_HTI,            /**< Hybrid time interleaving */
	SONY_CXD6801_ATSC3_PLP_TI_RESERVED        /**< Reserved */
} sony_cxd6801_atsc3_plp_ti_t;

/**
 @brief PLP channel bonding format signaled in L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PLP_CH_BOND_FMT_PLAIN,      /**< Plain channel bonding */
	SONY_CXD6801_ATSC3_PLP_CH_BOND_FMT_SNR,        /**< SNR averaged channel bonding */
	SONY_CXD6801_ATSC3_PLP_CH_BOND_FMT_RESERVED_2, /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_CH_BOND_FMT_RESERVED_3  /**< Reserved */
} sony_cxd6801_atsc3_plp_ch_bond_fmt_t;

/**
 @brief Number of rows used in the CTI signaled in L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PLP_CTI_DEPTH_512,          /**< 512 */
	SONY_CXD6801_ATSC3_PLP_CTI_DEPTH_724,          /**< 724 */
	SONY_CXD6801_ATSC3_PLP_CTI_DEPTH_887_1254,     /**< 887 (non-extended interleaving) or 1254 (extended interleaving) */
	SONY_CXD6801_ATSC3_PLP_CTI_DEPTH_1024_1448,    /**< 1024 (non-extended interleaving) or 1448 (extended interleaving) */
	SONY_CXD6801_ATSC3_PLP_CTI_DEPTH_RESERVED_4,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_CTI_DEPTH_RESERVED_5,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_CTI_DEPTH_RESERVED_6,   /**< Reserved */
	SONY_CXD6801_ATSC3_PLP_CTI_DEPTH_RESERVED_7    /**< Reserved */
} sony_cxd6801_atsc3_plp_cti_depth_t;

/**
 @brief Enhanced PLP's injection level signaled in L1-Detail.
*/
typedef enum {
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_0_0,      /**< 0.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_0_5,      /**< 0.5 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_1_0,      /**< 1.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_1_5,      /**< 1.5 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_2_0,      /**< 2.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_2_5,      /**< 2.5 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_3_0,      /**< 3.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_3_5,      /**< 3.5 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_4_0,      /**< 4.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_4_5,      /**< 4.5 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_5_0,      /**< 5.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_6_0,      /**< 6.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_7_0,      /**< 7.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_8_0,      /**< 8.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_9_0,      /**< 9.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_10_0,     /**< 10.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_11_0,     /**< 11.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_12_0,     /**< 12.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_13_0,     /**< 13.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_14_0,     /**< 14.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_15_0,     /**< 15.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_16_0,     /**< 16.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_17_0,     /**< 17.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_18_0,     /**< 18.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_19_0,     /**< 19.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_20_0,     /**< 20.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_21_0,     /**< 21.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_22_0,     /**< 22.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_23_0,     /**< 23.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_24_0,     /**< 24.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_25_0,     /**< 25.0 dB */
	SONY_CXD6801_ATSC3_PLP_LDM_INJ_LEVEL_RESERVED, /**< Reserved */
} sony_cxd6801_atsc3_plp_ldm_inj_level_t;

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief ATSC 3.0 bootstrap information.
*/
typedef struct sony_atsc3_bootstrap_t {
    uint8_t bw_diff;                     /**< If 1, system bandwidth setting is different from signal. */
	sony_cxd6801_atsc3_system_bw_t system_bw;    /**< Signal system bandwidth. */
    uint8_t ea_wake_up;                  /**< Emergency alert wake up field. (2bit) */
} sony_cxd6801_atsc3_bootstrap_t;

/**
 @brief ATSC 3.0 parameters estimated by demodulator.

 @note  Several parameters can be found in bootstrap, L1 too.
        But note that they may NOT be latest L1 data received by demod.
        These parameters are currently used by demodulator circuit.
*/
typedef struct sony_atsc3_ofdm_t {
	sony_cxd6801_atsc3_system_bw_t      system_bw;           /**< System bandwidth. (6 or 7 or 8) */
    uint8_t                     bs_num_symbol;       /**< Number of bootstrap symbol - 1 */
    uint8_t                     num_subframe;        /**< Number of subframe - 1 */
	sony_cxd6801_atsc3_l1b_fec_type_t   l1b_fec_type;        /**< L1-Basic fec type */
	sony_cxd6801_atsc3_papr_t           papr;                /**< PAPR reduction */
    uint8_t                     pb_num_symbol;       /**< Number of preamble symbol - 1 */
	sony_cxd6801_atsc3_fft_size_t       pb_fft_size;         /**< Preamble FFT size */
	sony_cxd6801_atsc3_preamble_pilot_t pb_pilot;            /**< Preamble pilot pattern */
    uint8_t                     pb_reduced_carriers; /**< Cred_coeff indicator for preamble (0 - 4) */
	sony_cxd6801_atsc3_gi_t             pb_gi;               /**< Preamble guard interval */
	sony_cxd6801_atsc3_fft_size_t       sf0_fft_size;        /**< Subframe #0 FFT size */
	sony_cxd6801_atsc3_sp_t             sf0_sp;              /**< Subframe #0 scattered pilot pattern */
    uint8_t                     sf0_reduced_carriers;/**< Cred_coeff indicator for subframe #0 (0 - 4) */
	sony_cxd6801_atsc3_gi_t             sf0_gi;              /**< Subframe #0 guard interval */
    uint8_t                     sf0_sp_boost;        /**< Subframe #0 scattered pilot boost (0 - 4) */
    uint8_t                     sf0_sbs_first;       /**< Subframe #0 subframe boundary symbol first flag */
    uint8_t                     sf0_sbs_last;        /**< Subframe #0 subframe boundary symbol last flag */
    uint16_t                    sf0_num_ofdm_symbol; /**< Subframe #0 number of data payload OFDM symbols - 1 */
} sony_cxd6801_atsc3_ofdm_t;

/**
 @brief ATSC 3.0 L1-Basic information.
*/
typedef struct sony_atsc3_l1basic_t {
    /* System and Frame Parameters */
    uint8_t                     version;             /**< L1-Basic structure version. (currently 0) */
    uint8_t                     mimo_sp_enc;         /**< MIMO pilot encoding scheme (0 or 1) */
    uint8_t                     lls_flg;             /**< Presence or absence of Low Level Signaling (LLS) */
	sony_cxd6801_atsc3_time_info_flag_t time_info_flg;       /**< Presence or absence, and precision of timing information */
    uint8_t                     return_ch_flg;       /**< Dedicated return channel (DRC) is present or not */
	sony_cxd6801_atsc3_papr_t           papr;                /**< PAPR reduction */
    uint8_t                     frame_length_mode;   /**< Frame is time-aligned (0) or symbol-aligned (1) */
    /* Valid if frame_length_mode == 0 (time-aligned frame) */
    uint16_t                    frame_length;        /**< Frame length (5ms unit) */
    uint16_t                    excess_smp_per_sym;  /**< Additional number of excess samples included in the guard interval */
    /* Valid if frame_length_mode == 1 (symbol-aligned frame) */
    uint16_t                    time_offset;         /**< Number of sample periods */
    uint8_t                     additional_smp;      /**< Number of additional samples at the end of frames */
    /* endif */
    uint8_t                     num_subframe;        /**< Number of subframe - 1 */

    /* Parameters for L1-Detail */
    uint8_t                     pb_num_symbol;       /**< Number of preamble symbol - 1 */
    uint8_t                     pb_reduced_carriers; /**< Cred_coeff indicator for preamble (0 - 4) */
    uint8_t                     l1d_content_tag;     /**< Incremented if L1-Detail content is modified */
    uint16_t                    l1d_size;            /**< L1-Detail information size (byte) */
	sony_cxd6801_atsc3_l1d_fec_type_t   l1d_fec_type;        /**< L1-Detail fec type */
    uint8_t                     l1d_add_parity_mode; /**< L1-Detail Additional parity mode (0 - 2) */
    uint32_t                    l1d_total_cells;     /**< The total size (specified in OFDM cells) of L1-Detail signaling */

    /* Parameters for First Subframe */
    uint8_t                     sf0_mimo;            /**< Subframe #0 MIMO or not */
	sony_cxd6801_atsc3_miso_t           sf0_miso;            /**< Subframe #0 MISO option */
	sony_cxd6801_atsc3_fft_size_t       sf0_fft_size;        /**< Subframe #0 FFT size */
    uint8_t                     sf0_reduced_carriers;/**< Cred_coeff indicator for subframe #0 (0 - 4) */
	sony_cxd6801_atsc3_gi_t             sf0_gi;              /**< Subframe #0 guard interval */
    uint16_t                    sf0_num_ofdm_symbol; /**< Subframe #0 number of data payload OFDM symbols - 1 */
	sony_cxd6801_atsc3_sp_t             sf0_sp;              /**< Subframe #0 scattered pilot pattern */
    uint8_t                     sf0_sp_boost;        /**< Subframe #0 scattered pilot boost (0 - 4) */
    uint8_t                     sf0_sbs_first;       /**< Subframe #0 subframe boundary symbol first flag */
    uint8_t                     sf0_sbs_last;        /**< Subframe #0 subframe boundary symbol last flag */

    /* Miscellaneous Parameters */
    uint8_t                     reserved[6];         /**< Reserved for future use */
} sony_cxd6801_atsc3_l1basic_t;

/**
 @brief ATSC 3.0 L1-Detail information raw data.
*/
typedef struct sony_atsc3_l1detail_raw_t {
    uint16_t                    size;                /**< L1-Detail information size (byte) */
    uint8_t                     data[8191];          /**< Raw data buffer */
} sony_cxd6801_atsc3_l1detail_raw_t;

/**
 @brief ATSC 3.0 L1-Detail common information.
*/
typedef struct sony_atsc3_l1detail_common_t {
    uint8_t                     version;             /**< L1-Detail structure version */
    uint8_t                     num_rf;              /**< Number of other frequencies in channel bonding (0 or 1) */
    uint16_t                    bonded_bsid;         /**< BSID of separate RF channel (valid if num_rf == 1) */
    /* Valid if time_info_flg (sony_atsc3_l1base_t) != NONE */
    uint32_t                    time_sec;            /**< Time information (sec) */
    uint16_t                    time_msec;           /**< Time information (msec) */
    /* Valid if time_info_flg (sony_atsc3_l1base_t) == US or NS */
    uint16_t                    time_usec;           /**< Time information (usec) */
    /* Valid if time_info_flg (sony_atsc3_l1base_t) == NS */
    uint16_t                    time_nsec;           /**< Time information (nsec) */

    uint16_t                    bsid;                /**< BSID of the current channel */
    uint16_t                    reserved_bitlen;     /**< Size of reserved data (bit) */
} sony_cxd6801_atsc3_l1detail_common_t;

/**
 @brief ATSC 3.0 L1-Detail subframe specific information.
*/
typedef struct sony_atsc3_l1detail_subframe_t {
    uint8_t                     index;               /**< Subframe index */
    uint8_t                     mimo;                /**< MIMO or not */
	sony_cxd6801_atsc3_miso_t           miso;                /**< MISO option */
	sony_cxd6801_atsc3_fft_size_t       fft_size;            /**< FFT size */
    uint8_t                     reduced_carriers;    /**< Cred_coeff indicator (0 - 4) */
	sony_cxd6801_atsc3_gi_t             gi;                  /**< Guard interval */
    uint16_t                    num_ofdm_symbol;     /**< Number of data payload OFDM symbols - 1 */
	sony_cxd6801_atsc3_sp_t             sp;                  /**< Scattered pilot pattern */
    uint8_t                     sp_boost;            /**< Scattered pilot boost (0 - 4) */
    uint8_t                     sbs_first;           /**< Subframe boundary symbol first flag */
    uint8_t                     sbs_last;            /**< Subframe boundary symbol last flag */
    /* Valid if num_subframe (sony_atsc3_l1base_t) > 0 */
    uint8_t                     subframe_mux;        /**< Time-division multiplexed or concatenated in time */

    uint8_t                     freq_interleaver;    /**< Frequency interleaver is enabled or not */
    /* Valid if sbs_first || sbs_last */
    uint16_t                    sbs_null_cells;      /**< Number of null cells */

    uint8_t                     num_plp;             /**< Number of PLPs in the current subframe */
} sony_cxd6801_atsc3_l1detail_subframe_t;

/**
 @brief ATSC 3.0 L1-Detail PLP specific information.
*/
typedef struct sony_atsc3_l1detail_plp_t {
    uint8_t                     id;                    /**< PIP ID (0 - 63) assigned uniquely in each RF channel */
    uint8_t                     lls_flg;               /**< Current PLP contains LLS information or not */
    uint8_t                     layer;                 /**< Layer index of current PLP (0 (Core Layer) or 1 (Enhanced Layer)) */
    uint32_t                    start;                 /**< First data cell index */
    uint32_t                    size;                  /**< Number of data cells allocated to the current PLP */
    uint8_t                     scrambler_type;        /**< Choice of scrambler (only 0 in current version) */
	sony_cxd6801_atsc3_plp_fec_t        fec_type;              /**< PLP FEC method */
	sony_cxd6801_atsc3_plp_mod_t        mod;                   /**< PLP modulation */
	sony_cxd6801_atsc3_plp_cod_t        cod;                   /**< PLP code rate */
	sony_cxd6801_atsc3_plp_ti_t         ti_mode;               /**< PLP time interleaving mode */
    /* Valid if ti_mode == SONY_ATSC3_PLP_TI_NONE */
    uint16_t                    fec_block_start;       /**< Start position of the first FEC Block */
    /* Valid if ti_mode == SONY_ATSC3_PLP_TI_CTI */
    uint32_t                    cti_fec_block_start;   /**< Position for CTI. (See spec) */

    /* Valid if num_rf (sony_atsc3_l1detail_common_t) > 0 */
    uint8_t                     num_ch_bonded;         /**< Number of other frequencies in channel bonding for this PLP (0 or 1)*/
    /* Valid if num_ch_bonded > 0 */
	sony_cxd6801_atsc3_plp_ch_bond_fmt_t ch_bonding_format;    /**< Channel bonding format for the current PLP */
    uint8_t                     bonded_rf_id_0;        /**< Bonded RF ID #0 */
    uint8_t                     bonded_rf_id_1;        /**< Bonded RF ID #1 */

    /* Valid if mimo (sony_atsc3_l1detail_subframe_t) == 1 */
    uint8_t                     mimo_stream_combine;   /**< Stream combining option is used or not */
    uint8_t                     mimo_iq_interleave;    /**< IQ polarization interleaving option is used or not */
    uint8_t                     mimo_ph;               /**< Phase hopping option is used nor not */

    /* Valid if layer (sony_atsc3_l1detail_plp_t) == 0 */
    uint8_t                     plp_type;              /**< Current PLP is non-dispersed (0) or dispersed (1) */
    /* Valid if plp_type == 1 */
    uint16_t                    num_subslice;          /**< Number of subslice - 1 (1 - 16383) */
    uint32_t                    subslice_interval;     /**< Subslice interval (data cell unit) */

    /* Valid if ti_mode == SONY_ATSC3_PLP_TI_CTI or
               (ti_mode == SONY_ATSC3_PLP_TI_HTI and mod == SONY_ATSC3_PLP_MOD_QPSK) */
    uint8_t                     ti_ext_interleave;     /**< Extended interleaving is used or not */

    /* Valid if ti_mode == SONY_ATSC3_PLP_TI_CTI */
	sony_cxd6801_atsc3_plp_cti_depth_t  cti_depth;             /**< Number of rows used in the Convolutional Time Interleaver */
    uint16_t                    cti_start_row;         /**< Position of the interleaver selector at the start of the subframe*/

    /* Valid if ti_mode == SONY_ATSC3_PLP_TI_HTI */
    uint8_t                     hti_inter_subframe;    /**< HTI is used or not in this subframe */
    uint8_t                     hti_num_ti_blocks;     /**< Number of TI blocks - 1 (0 - 15) */
    uint16_t                    hti_num_fec_block_max; /**< Maximum number of FEC Blocks - 1 */
    uint16_t                    hti_num_fec_block[16]; /**< Number of FEC Blocks - 1 */
    uint8_t                     hti_cell_interleave;   /**< Cell Interleaver is used or not */

    /* Valid if layer (sony_atsc3_l1detail_plp_t) != 0 */
	sony_cxd6801_atsc3_plp_ldm_inj_level_t ldm_inj_level;      /**< Enhanced PLP's injection level */
} sony_cxd6801_atsc3_l1detail_plp_t;

/**
 @brief Minimum required PLP information got by ::sony_demod_atsc3_monitor_PLPList.
*/
typedef struct sony_atsc3_plp_list_entry_t {
    uint8_t                     id;                    /**< PIP ID (0 - 63) assigned uniquely in each RF channel */
    uint8_t                     lls_flg;               /**< PLP contains LLS information or not */
    uint8_t                     layer;                 /**< Layer index of current PLP (0 (Core Layer) or 1 (Enhanced Layer)) */
    uint8_t                     chbond;                /**< The PLP is channel bonded or not */
} sony_cxd6801_atsc3_plp_list_entry_t;

/**
 @brief Quick PLP parameter monitor values for ::sony_demod_atsc3_monitor_FecModCod.
*/
typedef struct sony_atsc3_plp_fecmodcod_t {
    uint8_t                     valid;                 /**< This PLP is valid or not */
	sony_cxd6801_atsc3_plp_fec_t        fec_type;              /**< PLP FEC method */
	sony_cxd6801_atsc3_plp_mod_t        mod;                   /**< PLP modulation */
	sony_cxd6801_atsc3_plp_cod_t        cod;                   /**< PLP code rate */
} sony_cxd6801_atsc3_plp_fecmodcod_t;

#endif /* SONY_CXD6801_ATSC3_H */
