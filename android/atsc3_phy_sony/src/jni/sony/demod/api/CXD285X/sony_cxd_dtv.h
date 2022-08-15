/*------------------------------------------------------------------------------
  Copyright 2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/08/01
  Modification ID : b30d76210d343216ea52b88e9b450c8fd5c0359f
------------------------------------------------------------------------------*/
/**
 @file    sony_cxd_dtv.h

          This file provides DTV system specific definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_DTV_H
#define sony_cxd_DTV_H

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Broadcasting system
*/
typedef enum {
    sony_cxd_DTV_SYSTEM_UNKNOWN,        /**< Unknown. */
	sony_cxd_DTV_SYSTEM_DVBT,           /**< DVB-T. */
	sony_cxd_DTV_SYSTEM_DVBT2,          /**< DVB-T2. */
	sony_cxd_DTV_SYSTEM_ISDBT,          /**< ISDB-T. */
	sony_cxd_DTV_SYSTEM_ISDBTSB,        /**< ISDB-Tsb. */
	sony_cxd_DTV_SYSTEM_ISDBTMM_A,      /**< ISDB-Tmm Type-A. */
	sony_cxd_DTV_SYSTEM_ISDBTMM_B,      /**< ISDB-Tmm Type-B. */

	sony_cxd_DTV_SYSTEM_DVBC,           /**< DVB-C(J.83A) */
	sony_cxd_DTV_SYSTEM_DVBC2,          /**< DVB-C2(J.382) */
	sony_cxd_DTV_SYSTEM_DVBS,           /**< DVB-S */
	sony_cxd_DTV_SYSTEM_DVBS2,          /**< DVB-S2 */
	sony_cxd_DTV_SYSTEM_ISDBC,          /**< ISDB-C(J.83C) */
	sony_cxd_DTV_SYSTEM_ISDBS,          /**< ISDB-S */
	sony_cxd_DTV_SYSTEM_ISDBS3,         /**< ISDB-S3 */
	sony_cxd_DTV_SYSTEM_J83B,           /**< J.83B */
	sony_cxd_DTV_SYSTEM_ANY             /**< Used for multiple system scanning / blind tuning */
} sony_cxd_dtv_system_t;

/**
 @brief System bandwidth.
*/
typedef enum {
    sony_cxd_DTV_BW_UNKNOWN = 0,              /**< Unknown bandwidth. */
    sony_cxd_DTV_BW_1_7_MHZ = 1,              /**< 1.7MHz bandwidth. */
    sony_cxd_DTV_BW_5_MHZ = 5,                /**< 5MHz bandwidth. */
    sony_cxd_DTV_BW_6_MHZ = 6,                /**< 6MHz bandwidth. */
    sony_cxd_DTV_BW_7_MHZ = 7,                /**< 7MHz bandwidth. */
    sony_cxd_DTV_BW_8_MHZ = 8,                /**< 8MHz bandwidth. */

    sony_cxd_DTV_BW_J83B_5_06_5_36_MSPS = 50, /**< For J.83B. 5.06/5.36Msps auto selection commonly used in US. */
    sony_cxd_DTV_BW_J83B_5_60_MSPS = 51       /**< For J.83B. 5.6Msps used by SKY PerfecTV! Hikari in Japan. */
} sony_cxd_dtv_bandwidth_t;

#endif /* sony_cxd_DTV_H */
