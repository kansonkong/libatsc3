/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/06/24
  Modification ID : 3b74e280b7ad8ce430b6a9419ac53e8f2e3737f9
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_isdbt.h

          This file provides ISDB-T related type definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_ISDBT_H
#define sony_cxd_ISDBT_H

#include "sony_cxd_common.h"

/*------------------------------------------------------------------------------
  Enumeration
------------------------------------------------------------------------------*/
/**
 @brief ISDB-T system mode.
*/
typedef enum sony_cxd_isdbt_mode_t {
	sony_cxd_ISDBT_MODE_1,       /**< ISDB-T Mode 1 (Number of carriers = 1405) */
	sony_cxd_ISDBT_MODE_2,       /**< ISDB-T Mode 2 (Number of carriers = 2809) */
	sony_cxd_ISDBT_MODE_3,       /**< ISDB-T Mode 3 (Number of carriers = 5617) */
	sony_cxd_ISDBT_MODE_UNKNOWN  /**< ISDB-T Mode Unknown */
} sony_cxd_isdbt_mode_t;

/**
 @brief ISDB-T guard interval.
*/
typedef enum sony_cxd_isdbt_guard_t {
	sony_cxd_ISDBT_GUARD_1_32,    /**< Guard Interval : 1/32 */
	sony_cxd_ISDBT_GUARD_1_16,    /**< Guard Interval : 1/16 */
	sony_cxd_ISDBT_GUARD_1_8,     /**< Guard Interval : 1/8  */
	sony_cxd_ISDBT_GUARD_1_4,     /**< Guard Interval : 1/4  */
	sony_cxd_ISDBT_GUARD_UNKNOWN  /**< Guard Interval : Unknown */
} sony_cxd_isdbt_guard_t;


/* ===== TMCC Information ===== */

/**
 @brief ISDB-T system identification described in the TMCC information.
*/
typedef enum sony_cxd_isdbt_system_t {
	sony_cxd_ISDBT_SYSTEM_ISDB_T,      /**< ISDB for Terrestrial Television Broadcasting */
	sony_cxd_ISDBT_SYSTEM_ISDB_TSB,    /**< ISDB for Terrestrial Sound Broadcasting      */
	sony_cxd_ISDBT_SYSTEM_RESERVED_2,   /**< reserved by specification (10) */
	sony_cxd_ISDBT_SYSTEM_RESERVED_3    /**< reserved by specification (11) */
} sony_cxd_isdbt_system_t;

/**
 @brief ISDB-T modulation described in the TMCC information.
*/
typedef enum sony_cxd_isdbt_modulation_t {
	sony_cxd_ISDBT_MODULATION_DQPSK,        /**< DQPSK */
	sony_cxd_ISDBT_MODULATION_QPSK,         /**< QPSK  */
	sony_cxd_ISDBT_MODULATION_16QAM,        /**< 16QAM */
	sony_cxd_ISDBT_MODULATION_64QAM,        /**< 64QAM */
	sony_cxd_ISDBT_MODULATION_RESERVED_4,   /**< reserved by specification (100) */
	sony_cxd_ISDBT_MODULATION_RESERVED_5,   /**< reserved by specification (101) */
	sony_cxd_ISDBT_MODULATION_RESERVED_6,   /**< reserved by specification (110) */
	sony_cxd_ISDBT_MODULATION_UNUSED_7      /**< unused (111) */
} sony_cxd_isdbt_modulation_t;

/**
 @brief ISDB-T code rate described in the TMCC information.
*/
typedef enum sony_cxd_isdbt_coding_rate_t {
	sony_cxd_ISDBT_CODING_RATE_1_2,         /**< Code Rate : 1/2 */
	sony_cxd_ISDBT_CODING_RATE_2_3,         /**< Code Rate : 2/3 */
	sony_cxd_ISDBT_CODING_RATE_3_4,         /**< Code Rate : 3/4 */
	sony_cxd_ISDBT_CODING_RATE_5_6,         /**< Code Rate : 5/6 */
	sony_cxd_ISDBT_CODING_RATE_7_8,         /**< Code Rate : 7/8 */
	sony_cxd_ISDBT_CODING_RATE_RESERVED_5,  /**< reserved by specification (101) */
	sony_cxd_ISDBT_CODING_RATE_RESERVED_6,  /**< reserved by specification (110) */
	sony_cxd_ISDBT_CODING_RATE_UNUSED_7     /**< unused (111) */
} sony_cxd_isdbt_coding_rate_t;

/**
 @brief ISDB-T interleave length described in the TMCC information.
*/
typedef enum sony_cxd_isdbt_il_length_t {
	sony_cxd_ISDBT_IL_LENGTH_0_0_0,        /**< Mode1: 0, Mode2: 0, Mode3: 0 */
	sony_cxd_ISDBT_IL_LENGTH_4_2_1,        /**< Mode1: 4, Mode2: 2, Mode3: 1 */
	sony_cxd_ISDBT_IL_LENGTH_8_4_2,        /**< Mode1: 8, Mode2: 4, Mode3: 2 */
	sony_cxd_ISDBT_IL_LENGTH_16_8_4,       /**< Mode1:16, Mode2: 8, Mode3: 4 */
	sony_cxd_ISDBT_IL_LENGTH_RESERVED_4,   /**< reserved by specification (100) */
	sony_cxd_ISDBT_IL_LENGTH_RESERVED_5,   /**< reserved by specification (101) */
	sony_cxd_ISDBT_IL_LENGTH_RESERVED_6,   /**< reserved by specification (110) */
	sony_cxd_ISDBT_IL_LENGTH_UNUSED_7      /**< unused (111) */
} sony_cxd_isdbt_il_length_t;

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/
/**
 @brief ISDB-T layer information struct. (part of TMCC information)
*/
typedef struct sony_cxd_isdbt_tmcc_layer_info_t {
	sony_cxd_isdbt_modulation_t  modulation; /**< Modulation */
	sony_cxd_isdbt_coding_rate_t codingRate; /**< Code rate */
	sony_cxd_isdbt_il_length_t   ilLength;   /**< Interleave length */
    uint8_t segmentsNum;                 /**< 1-13:Seguments number, 0,14:Reserved, 15:Unused */
} sony_cxd_isdbt_tmcc_layer_info_t;

/**
 @brief ISDB-T current/next information group struct. (part of TMCC information)
*/
typedef struct sony_cxd_isdbt_tmcc_group_param_t {
    uint8_t isPartial;                        /**< 0:No partial reception, 1:Partial reception available */
	sony_cxd_isdbt_tmcc_layer_info_t layerA;      /**< Information of layer A */
	sony_cxd_isdbt_tmcc_layer_info_t layerB;      /**< Information of layer B */
	sony_cxd_isdbt_tmcc_layer_info_t layerC;      /**< Information of layer C */
} sony_cxd_isdbt_tmcc_group_param_t;

/**
 @brief ISDB-T TMCC information struct.
*/
typedef struct sony_cxd_isdbt_tmcc_info_t {
	sony_cxd_isdbt_system_t                systemId;       /**< ISDB-T system identification */
    uint8_t                            countDownIndex; /**< Indicator of transmission-parameter switching */
    uint8_t                            ewsFlag;        /**< Start flag for emergency-alarm broadcasting */
	sony_cxd_isdbt_tmcc_group_param_t      currentInfo;    /**< Current information */
	sony_cxd_isdbt_tmcc_group_param_t      nextInfo;       /**< Next information */
} sony_cxd_isdbt_tmcc_info_t;

/**
 @brief ISDB-T ACEEW area information struct. (part of ACEEW information)
*/
typedef struct sony_cxd_isdbt_aceew_area_t {
    uint8_t data[11];                             /**< Area data defined by ISDB-T specification */
} sony_cxd_isdbt_aceew_area_t;

/**
 @brief ISDB-T ACEEW epicenter information struct. (part of ACEEW information)
*/
typedef struct sony_cxd_isdbt_aceew_epicenter_t {
    uint8_t data[11];                             /**< Epicenter data defined by ISDB-T specification */
} sony_cxd_isdbt_aceew_epicenter_t;

/**
 @brief ISDB-T ACEEW (Earthquake Early Warning by AC signal) information struct.
*/
typedef struct sony_cxd_isdbt_aceew_info_t {
    uint8_t startEndFlag;                         /**< Start/End flag (B17-B18) */
    uint8_t updateFlag;                           /**< Update flag (B19-B20) */
    uint8_t signalId;                             /**< Signal indentification (B21-B23) */

    /* Area information */
    uint8_t isAreaValid;                          /**< areaInfo is (0: Invalid, 1:Valid) */
	sony_cxd_isdbt_aceew_area_t areaInfo;             /**< Area information (B24 - B111) */

    /* Epicenter1 information */
    uint8_t isEpicenter1Valid;                    /**< epicenter1Info is (0: Invalid, 1:Valid) */
	sony_cxd_isdbt_aceew_epicenter_t epicenter1Info;  /**< Epicenter1 information data (B24 - B111) */

    /* Epicenter2 information */
    uint8_t isEpicenter2Valid;                    /**< epicenter2Info is (0: Invalid, 1:Valid) */
	sony_cxd_isdbt_aceew_epicenter_t epicenter2Info;  /**< Epicenter2 information data (B24 - B111) */

} sony_cxd_isdbt_aceew_info_t;

#endif /* sony_cxd_ISDBT_H */
