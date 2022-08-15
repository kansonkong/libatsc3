/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/
/**
 @file    sony_dtv.h

          This file provides DTV system specific definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_DTV_H
#define SONY_CXD6801_DTV_H

/*------------------------------------------------------------------------------
  Enumerations
------------------------------------------------------------------------------*/
/**
 @brief Broadcasting system
*/
typedef enum {
	SONY_CXD6801_DTV_SYSTEM_UNKNOWN,        /**< Unknown. */
	SONY_CXD6801_DTV_SYSTEM_DVBT,           /**< DVB-T */
	SONY_CXD6801_DTV_SYSTEM_DVBT2,          /**< DVB-T2 */
	SONY_CXD6801_DTV_SYSTEM_DVBC,           /**< DVB-C(J.83A) */
	SONY_CXD6801_DTV_SYSTEM_DVBC2,          /**< DVB-C2(J.382) */
	SONY_CXD6801_DTV_SYSTEM_ATSC,           /**< ATSC */
	SONY_CXD6801_DTV_SYSTEM_ATSC3,          /**< ATSC 3.0 */
	SONY_CXD6801_DTV_SYSTEM_ISDBT,          /**< ISDB-T */
	SONY_CXD6801_DTV_SYSTEM_ISDBC,          /**< ISDB-C(J.83C) */
	SONY_CXD6801_DTV_SYSTEM_J83B,           /**< J.83B */
	SONY_CXD6801_DTV_SYSTEM_DVBS,           /**< DVB-S */
	SONY_CXD6801_DTV_SYSTEM_DVBS2,          /**< DVB-S2 */
	SONY_CXD6801_DTV_SYSTEM_ISDBS,          /**< ISDB-S */
	SONY_CXD6801_DTV_SYSTEM_ISDBS3,         /**< ISDB-S3 */
	SONY_CXD6801_DTV_SYSTEM_ANY             /**< Used for multiple system scanning / blind tuning */
} sony_cxd6801_dtv_system_t;

/**
 @brief Macro to check that the system is terrestrial/cable or not.
*/
#define SONY_CXD6801_DTV_SYSTEM_IS_TERR_CABLE(dtvSystem) (((dtvSystem) >= SONY_CXD6801_DTV_SYSTEM_DVBT) && ((dtvSystem) <= SONY_CXD6801_DTV_SYSTEM_J83B))

/**
 @brief Macro to check that the system is satellite or not.
*/
#define SONY_CXD6801_DTV_SYSTEM_IS_SAT(dtvSystem) (((dtvSystem) >= SONY_CXD6801_DTV_SYSTEM_DVBS) && ((dtvSystem) <= SONY_CXD6801_DTV_SYSTEM_ISDBS3))

/**
 @brief System bandwidth.
*/
typedef enum {
	SONY_CXD6801_DTV_BW_UNKNOWN = 0,              /**< Unknown bandwidth. */
	SONY_CXD6801_DTV_BW_1_7_MHZ = 1,              /**< 1.7MHz bandwidth. */
	SONY_CXD6801_DTV_BW_5_MHZ = 5,                /**< 5MHz bandwidth. */
	SONY_CXD6801_DTV_BW_6_MHZ = 6,                /**< 6MHz bandwidth. */
	SONY_CXD6801_DTV_BW_7_MHZ = 7,                /**< 7MHz bandwidth. */
	SONY_CXD6801_DTV_BW_8_MHZ = 8,                /**< 8MHz bandwidth. */

	SONY_CXD6801_DTV_BW_J83B_5_06_5_36_MSPS = 50, /**< For J.83B. 5.06/5.36Msps auto selection commonly used in US. */
	SONY_CXD6801_DTV_BW_J83B_5_60_MSPS = 51       /**< For J.83B. 5.6Msps used by SKY PerfecTV! Hikari in Japan. */
} sony_cxd6801_dtv_bandwidth_t;

#endif /* SONY_CXD6801_DTV_H */
