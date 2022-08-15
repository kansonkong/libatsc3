/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/09/28
  Modification ID : 1ea04def104d657deb64f0d0b0cae6ef992a9dd8
------------------------------------------------------------------------------*/
/**
 @file    sony_atsc3_chbond.h

          This file provides ATSC 3.0 channel bonding related type definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_ATSC3_CHBOND_H
#define SONY_CXD6801_ATSC3_CHBOND_H

#include "sony_cxd6801_common.h"

/*------------------------------------------------------------------------------
 Structs
------------------------------------------------------------------------------*/
/**
 @brief ATSC 3.0 channel bonding related information in L1-Detail.
*/
typedef struct sony_cxd6801_atsc3_chbond_l1detail_t {
    uint8_t                     num_rf;              /**< Number of other frequencies in channel bonding (0 or 1) */
    uint16_t                    bonded_bsid;         /**< BSID of separate RF channel (valid if num_rf == 1) */
    uint16_t                    bsid;                /**< BSID of the current channel (valid if num_rf == 1) */
} sony_cxd6801_atsc3_chbond_l1detail_t;

#endif /* SONY_ATSC3_CHBOND_H */
