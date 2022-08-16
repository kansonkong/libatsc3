/*------------------------------------------------------------------------------
  Copyright 2015 Sony Corporation

  Last Updated    : 2015/08/20
  Modification ID : e900afa993b570691bd0d6f70a8d6d3ce80099f9
------------------------------------------------------------------------------*/
/**
 @file  sony_isdbtmm.h

 @brief ISDB-Tmm system related type definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_ISDBTMM_H
#define SONY_ISDBTMM_H

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_common.h"
#define uint32_t unsigned int
#define int32_t signed int
#define int8_t signed char

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/
/**
 @brief Center frequency of ISDB-Tmm carrier. (in MHz x 7)

        STD-B46 defines that the center frequency is 214 + 5/7 MHz
*/
#define SONY_ISDBTMM_CENTER_FREQ_MHZx7 (214 * 7 + 5)

/**
 @brief Center frequency of ISDB-Tmm segment. (in MHz x 7)

        STD-B46 said that there are 33 segments in ISDB-Tmm signal.
        Each segument has 3/7 MHz width.
        Segment index (segmentIndex) should be 0 - 32.
*/
#define SONY_ISDBTMM_SEGMENT_FREQ_MHZx7(segmentIndex) (SONY_ISDBTMM_CENTER_FREQ_MHZx7 + ((segmentIndex) - 16) * 3)

/**
 @brief Center frequency of ISDB-Tmm segment. (in kHz)

        Segment index (segmentIndex) should be 0 - 32.
*/
#define SONY_ISDBTMM_SEQMENT_FREQ_KHZ(segmentIndex) ((SONY_ISDBTMM_SEGMENT_FREQ_MHZx7(segmentIndex) * 1000 + 3) / 7)

/*------------------------------------------------------------------------------
  Enumeration
------------------------------------------------------------------------------*/
/**
 @brief ISDB-Tmm super segument type.
*/
typedef enum {
    SONY_ISDBTMM_SUPER_SEGMENT_A, /**< Type-A super segument */
    SONY_ISDBTMM_SUPER_SEGMENT_B  /**< Type-B super segument */
} sony_isdbtmm_super_segment_t;

#undef uint32_t 
#undef int32_t 
#undef int8_t 
#endif /* SONY_ISDBTMM_H */
