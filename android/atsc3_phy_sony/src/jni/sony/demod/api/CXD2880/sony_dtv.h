/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2016/03/01
  Modification ID : 821957233ed7087a9a4711da5ab28f0c008ce6a5
------------------------------------------------------------------------------*/
/**
 @file  sony_dtv.h

 @brief Common DTV system specific definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_DTV_H
#define SONY_DTV_H

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief System (DVB-T/T2/C/C2/S/S2 and ISDB-T/S)
*/
typedef enum {
    SONY_DTV_SYSTEM_UNKNOWN,        /**< Unknown. */
    SONY_DTV_SYSTEM_DVBT,           /**< DVB-T. */
    SONY_DTV_SYSTEM_DVBT2,          /**< DVB-T2. */
    SONY_DTV_SYSTEM_ISDBT,          /**< ISDB-T. */
    SONY_DTV_SYSTEM_ISDBTSB,        /**< ISDB-Tsb. */
    SONY_DTV_SYSTEM_ISDBTMM_A,      /**< ISDB-Tmm Type-A. */
    SONY_DTV_SYSTEM_ISDBTMM_B,      /**< ISDB-Tmm Type-B. */
    SONY_DTV_SYSTEM_ANY             /**< Used for multiple system scanning / blind tuning */
} sony_dtv_system_t;

/**
 @brief System bandwidth.
*/
typedef enum {
    SONY_DTV_BW_UNKNOWN = 0,          /**< Unknown bandwidth. */
    SONY_DTV_BW_1_7_MHZ = 1,          /**< 1.7MHz bandwidth. */
    SONY_DTV_BW_5_MHZ = 5,            /**< 5MHz bandwidth. */
    SONY_DTV_BW_6_MHZ = 6,            /**< 6MHz bandwidth. */
    SONY_DTV_BW_7_MHZ = 7,            /**< 7MHz bandwidth. */
    SONY_DTV_BW_8_MHZ = 8             /**< 8MHz bandwidth. */
} sony_dtv_bandwidth_t;

#endif /* SONY_DTV_H */
