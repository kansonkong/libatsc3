/*------------------------------------------------------------------------------
  Copyright 2013-2019 Sony Semiconductor Solutions Corporation

  Last Updated  : 2019/06/28
  File Revision : 2.1.3.0
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
 Based on ASCOT3 application note 2.18.0
          ASCOT3I application note 1.6.0
------------------------------------------------------------------------------*/

#include "sony_cxd6801_ascot3.h"

#define SONY_CXD6801_ASCOT3_USE_CXD2856_GENERATIONS /* For CXD2856/2857 or newer */

/*------------------------------------------------------------------------------
 Definitions of static const table
------------------------------------------------------------------------------*/
#define AUTO         (0xFF) /* For IF_OUT_SEL and AGC_SEL, it means that the value is desided by config flags. */
                            /* For RF_GAIN, it means that RF_GAIN_SEL(SubAddr:0x4E) = 1 */
#define OFFSET(ofs)  ((uint8_t)(ofs) & 0x1F)
#define BW_6         (0x00)
#define BW_7         (0x01)
#define BW_8         (0x02)
#define BW_1_7       (0x03)

/**
  @brief Sony silicon tuner setting for each broadcasting system.

         These values are optimized for Sony demodulators.
         The user have to change these values if other demodulators are used.
         Please check Sony silicon tuner application note for detail.
*/
#ifdef SONY_CXD6801_ASCOT3_USE_CXD2856_GENERATIONS
/* For CXD2856 or newer generation ICs */
static const sony_cxd6801_ascot3_adjust_param_t g_param_table[SONY_CXD6801_ASCOT3_TV_SYSTEM_NUM] = {
    /*
    OUTLMT    IF_BPF_GC                                           BW              BW_OFFSET         IF_OUT_SEL
      |  RF_GAIN  |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |        AGC_SEL |  IS_LOWERLOCAL
      |     |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |     |     |          */
    {0x00, AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_EIAJ   (System-M (Japan)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_SAP    (System-M (US)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_A2     (System-M (Korea)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_BG        (System-B/G) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_I         (System-I) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_DK        (System-D/K) */
    {0x00, AUTO, 0x03, 0x03, 0x06, 0x03, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_L         (System-L) */
    {0x00, AUTO, 0x03, 0x03, 0x06, 0x03, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_8VSB      (ATSC 8VSB) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_QAM       (US QAM) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {0x00, AUTO, 0x04, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {0x00, AUTO, 0x04, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {0x00, AUTO, 0x02, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {0x00, AUTO, 0x02, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_6   (ATSC 3.0 6MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_7   (ATSC 3.0 7MHzBW) */
    {0x00, AUTO, 0x08, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_8   (ATSC 3.0 8MHzBW) */
    {0x00, AUTO, 0x04, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-5), OFFSET(2),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_J83B_5_6  (J.83B 5.6Msps) */
    {0x00, AUTO, 0x03, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  AUTO, AUTO, 0x00}  /**< SONY_CXD6801_ASCOT3_DTV_DTMB      (DTMB) */
};
#else /* SONY_CXD6801_ASCOT3_USE_CXD2856_GENERATIONS */
/* For CXD2854 or older generation ICs */
static const sony_cxd6801_ascot3_adjust_param_t g_param_table[SONY_CXD6801_ASCOT3_TV_SYSTEM_NUM] = {
    /*
    OUTLMT    IF_BPF_GC                                           BW              BW_OFFSET         IF_OUT_SEL
      |  RF_GAIN  |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |        AGC_SEL |  IS_LOWERLOCAL
      |     |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |     |     |          */
    {0x00, AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_EIAJ   (System-M (Japan)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_SAP    (System-M (US)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_A2     (System-M (Korea)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_BG        (System-B/G) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_I         (System-I) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_DK        (System-D/K) */
    {0x00, AUTO, 0x03, 0x03, 0x06, 0x03, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_L         (System-L) */
    {0x00, AUTO, 0x03, 0x03, 0x06, 0x03, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_8VSB      (ATSC 8VSB) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_QAM       (US QAM) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {0x00, AUTO, 0x05, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {0x00, AUTO, 0x05, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {0x00, AUTO, 0x03, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {0x00, AUTO, 0x03, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_6   (ATSC 3.0 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_7   (ATSC 3.0 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_8   (ATSC 3.0 8MHzBW) */
    {0x00, AUTO, 0x05, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-5), OFFSET(2),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_J83B_5_6  (J.83B 5.6Msps) */
    {0x00, AUTO, 0x04, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  AUTO, AUTO, 0x00}  /**< SONY_CXD6801_ASCOT3_DTV_DTMB      (DTMB) */
};
#endif /* SONY_CXD6801_ASCOT3_USE_CXD2856_GENERATIONS */

/**
  @brief Sony silicon tuner setting for each broadcasting system. (for CXD2871A (ASCOT3I generation))

         These values are optimized for Sony demodulators.
         The user have to change these values if other demodulators are used.
         Please check Sony silicon tuner application note for detail.
*/
#ifdef SONY_CXD6801_ASCOT3_USE_CXD2856_GENERATIONS
/* For CXD2856 or newer generation ICs */
static const sony_cxd6801_ascot3_adjust_param_t g_param_table_ascot3i[SONY_CXD6801_ASCOT3_TV_SYSTEM_NUM] = {
    /*
    OUTLMT    IF_BPF_GC                                           BW              BW_OFFSET         IF_OUT_SEL
      |  RF_GAIN  |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |        AGC_SEL |  IS_LOWERLOCAL
      |     |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |     |     |          */
    {0x00, AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_EIAJ   (System-M (Japan)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_SAP    (System-M (US)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_A2     (System-M (Korea)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_BG        (System-B/G) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_I         (System-I) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_DK        (System-D/K) */
    {0x00, AUTO, 0x03, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_L         (System-L) */
    {0x00, AUTO, 0x03, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_8VSB      (ATSC 8VSB) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_QAM       (US QAM) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {0x00, AUTO, 0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {0x00, AUTO, 0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {0x00, AUTO, 0x02, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {0x00, AUTO, 0x02, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_6   (ATSC 3.0 6MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_7   (ATSC 3.0 7MHzBW) */
    {0x00, AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_8   (ATSC 3.0 8MHzBW) */
    {0x00, AUTO, 0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-5), OFFSET(2),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_J83B_5_6  (J.83B 5.6Msps) */
    {0x00, AUTO, 0x03, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  AUTO, AUTO, 0x00}  /**< SONY_CXD6801_ASCOT3_DTV_DTMB      (DTMB) */
};
#else /* SONY_CXD6801_ASCOT3_USE_CXD2856_GENERATIONS */
/* For CXD2854 or older generation ICs */
static const sony_cxd6801_ascot3_adjust_param_t g_param_table_ascot3i[SONY_CXD6801_ASCOT3_TV_SYSTEM_NUM] = {
    /*
    OUTLMT    IF_BPF_GC                                           BW              BW_OFFSET         IF_OUT_SEL
      |  RF_GAIN  |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |        AGC_SEL |  IS_LOWERLOCAL
      |     |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |     |     |          */
    {0x00, AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_EIAJ   (System-M (Japan)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_SAP    (System-M (US)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_MN_A2     (System-M (Korea)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_BG        (System-B/G) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_I         (System-I) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_DK        (System-D/K) */
    {0x00, AUTO, 0x03, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_L         (System-L) */
    {0x00, AUTO, 0x03, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_8VSB      (ATSC 8VSB) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_QAM       (US QAM) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {0x00, AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {0x00, AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {0x00, AUTO, 0x03, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {0x00, AUTO, 0x03, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_6   (ATSC 3.0 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_7   (ATSC 3.0 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_ATSC3_8   (ATSC 3.0 8MHzBW) */
    {0x00, AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-5), OFFSET(2),  AUTO, AUTO, 0x00}, /**< SONY_CXD6801_ASCOT3_DTV_J83B_5_6  (J.83B 5.6Msps) */
    {0x00, AUTO, 0x04, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  AUTO, AUTO, 0x00}  /**< SONY_CXD6801_ASCOT3_DTV_DTMB      (DTMB) */
};
#endif /* SONY_CXD6801_ASCOT3_USE_CXD2856_GENERATIONS */

/*------------------------------------------------------------------------------
 Definitions of static functions
------------------------------------------------------------------------------*/
/**
 @brief Configure the ASCOT3 tuner from Power On to Sleep state.
*/
static sony_cxd6801_result_t X_pon(sony_cxd6801_ascot3_t *pTuner);
/**
 @brief Configure the ASCOT3 tuner for specified broadcasting system.
*/
static sony_cxd6801_result_t X_tune(sony_cxd6801_ascot3_t *pTuner, uint32_t frequencykHz, sony_cxd6801_ascot3_tv_system_t tvSystem, uint8_t vcoCal);
/**
 @brief The last part of tuning sequence.
*/
static sony_cxd6801_result_t X_tune_end(sony_cxd6801_ascot3_t *pTuner, sony_cxd6801_ascot3_tv_system_t tvSystem);
/**
 @brief Configure the ASCOT3 tuner to Power Save state.
*/
static sony_cxd6801_result_t X_fin(sony_cxd6801_ascot3_t *pTuner);
/**
 @brief Configure the ASCOT3 tuner to Oscillation Stop state.
*/
static sony_cxd6801_result_t X_oscdis(sony_cxd6801_ascot3_t *pTuner);
/**
 @brief Configure the ASCOT3 tuner back from Oscillation Stop state.
*/
static sony_cxd6801_result_t X_oscen(sony_cxd6801_ascot3_t *pTuner);
/**
 @brief Reading gain information to calculate IF and RF gain levels.
*/
static sony_cxd6801_result_t X_read_agc(sony_cxd6801_ascot3_t *pTuner, uint8_t *pIFAGCReg, uint8_t *pRFAGCReg);
/**
 @brief Reading gain information to calculate IF and RF gain levels. (Legacy, only for CXD2871/2872/2875)
*/
static sony_cxd6801_result_t X_rssi(sony_cxd6801_ascot3_t *pTuner, int32_t *pRssiOut);

/*------------------------------------------------------------------------------
 Definitions of internal used macros
------------------------------------------------------------------------------*/
/**
 @brief Checking that the internal loop filter can be used for the broadcasting system.
*/
#define INTERNAL_LOOPFILTER_AVAILABLE(tvSystem) (SONY_CXD6801_ASCOT3_IS_DTV(tvSystem)\
    && ((tvSystem) != SONY_CXD6801_ASCOT3_DTV_DVBC_6) && ((tvSystem) != SONY_CXD6801_ASCOT3_DTV_DVBC_8)\
    && ((tvSystem) != SONY_CXD6801_ASCOT3_DTV_DVBC2_6) && ((tvSystem) != SONY_CXD6801_ASCOT3_DTV_DVBC2_8)\
    && ((tvSystem) != SONY_CXD6801_ASCOT3_DTV_J83B_5_6))

/*------------------------------------------------------------------------------
 Implementation of public functions.
------------------------------------------------------------------------------*/

sony_cxd6801_result_t sony_cxd6801_ascot3_Create(sony_cxd6801_ascot3_t *pTuner, sony_cxd6801_ascot3_xtal_t xtalFreq,
    uint8_t i2cAddress, sony_cxd6801_i2c_t *pI2c, uint32_t flags)
{
    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_Create");

    if((!pTuner) || (!pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* 41MHz is only for external reference input */
    if((xtalFreq == SONY_CXD6801_ASCOT3_XTAL_41000KHz) && !(flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    /* SONY_CXD6801_ASCOT3_CONFIG_EXT_REF and SONY_CXD6801_ASCOT3_CONFIG_SLEEP_DISABLEXTAL cannot be used at the same time. */
    if((flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF) && (flags & SONY_CXD6801_ASCOT3_CONFIG_SLEEP_DISABLEXTAL)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    pTuner->state = SONY_CXD6801_ASCOT3_STATE_UNKNOWN; /* Chip is not accessed for now. */
    pTuner->xtalFreq = xtalFreq;
    pTuner->pI2c = pI2c;
    pTuner->i2cAddress = i2cAddress;
    pTuner->flags = flags;
    pTuner->frequencykHz = 0;
    pTuner->tvSystem = SONY_CXD6801_ASCOT3_TV_SYSTEM_UNKNOWN;
    pTuner->chipId = SONY_CXD6801_ASCOT3_CHIP_ID_UNKNOWN;
    pTuner->pParamTable = g_param_table; /* Set CXD2871/2872/2875 table in default */
    pTuner->xosc_sel = 0x04; /* 4 x 25 = 100uA is default. */
    pTuner->xosc_cap_set = 0x30; /* 48 x 0.25 = 12pF is default. */
    pTuner->isSiP = 0;
    pTuner->user = NULL;

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_2875_Create(sony_cxd6801_ascot3_t *pTuner,
    uint8_t i2cAddress, sony_cxd6801_i2c_t *pI2c, uint32_t flags)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_2875_Create");

    if((!pTuner) || (!pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Following configurations are not supported for CXD2875 */
    if((flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF) || (flags & SONY_CXD6801_ASCOT3_CONFIG_LOOPFILTER_INTERNAL)
        || (flags & SONY_CXD6801_ASCOT3_CONFIG_LOOPTHRU_ENABLE) || (flags & SONY_CXD6801_ASCOT3_CONFIG_RFIN_MATCHING_ENABLE)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    result = sony_cxd6801_ascot3_Create(pTuner, SONY_CXD6801_ASCOT3_XTAL_24000KHz, i2cAddress, pI2c,
        SONY_CXD6801_ASCOT3_CONFIG_IFAGCSEL_ALL2 | flags);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }

    pTuner->xosc_sel = 0x04; /* 4 x 25 = 100uA */
    pTuner->xosc_cap_set = 0x36; /* 54 * 0.25 = 13.5pF */

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_Initialize(sony_cxd6801_ascot3_t *pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_Initialize");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    {
        uint8_t data = 0x00;

        /* Confirm connected device is ASCOT3 */
        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x7F, &data, 1);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        switch(data & 0xF0){
        case 0xC0: /* ASCOT3 ES1 */
        case 0xD0: /* ASCOT3 ES2 */
            /* Determined in X_pon function */
            pTuner->chipId = SONY_CXD6801_ASCOT3_CHIP_ID_UNKNOWN;
            break;
        case 0xE0: /* ASCOT3I */
            pTuner->chipId = SONY_CXD6801_ASCOT3_CHIP_ID_2871A;
            if(pTuner->pParamTable == g_param_table){
                /* CXD2871/2872/2875 tuner parameter default table is set now.
                   CXD2871A tuner parameter default table is used instead of it. */
                pTuner->pParamTable = g_param_table_ascot3i;
            }
            break;
        default:
            /* Not ASCOT3 family */
            SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }
    }

    /* X_pon sequence */
    result = X_pon(pTuner);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }

    if((pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF)){
        /* Disable Xtal */
        result = X_oscdis(pTuner);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }
    }

    pTuner->state = SONY_CXD6801_ASCOT3_STATE_SLEEP;
    pTuner->frequencykHz = 0;
    pTuner->tvSystem = SONY_CXD6801_ASCOT3_TV_SYSTEM_UNKNOWN;

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_Tune(sony_cxd6801_ascot3_t *pTuner, uint32_t frequencykHz, sony_cxd6801_ascot3_tv_system_t tvSystem)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_Tune");

    /* Argument check */
    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if(!SONY_CXD6801_ASCOT3_IS_ATV(tvSystem) && !SONY_CXD6801_ASCOT3_IS_DTV(tvSystem)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* State check */
    if((pTuner->state != SONY_CXD6801_ASCOT3_STATE_SLEEP) && (pTuner->state != SONY_CXD6801_ASCOT3_STATE_ACTIVE)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Rough frequency range check */
    if((frequencykHz < 1000) || (frequencykHz > 1200000)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
    }

    if(pTuner->state == SONY_CXD6801_ASCOT3_STATE_SLEEP){
        /* Set system to "Unknown". (for safe) */
        pTuner->tvSystem = SONY_CXD6801_ASCOT3_TV_SYSTEM_UNKNOWN;

        if((pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF)){
            /* Enable Xtal */
            result = X_oscen(pTuner);
            if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }
        }
    }

    /* Broadcasting system dependent setting and tuning. */
    result = X_tune(pTuner, frequencykHz, tvSystem, 1);
    if(result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_RETURN(result);
    }

    pTuner->state = SONY_CXD6801_ASCOT3_STATE_ACTIVE;
    pTuner->frequencykHz = frequencykHz;
    pTuner->tvSystem = tvSystem;

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_TuneEnd(sony_cxd6801_ascot3_t *pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_TuneEnd");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if(pTuner->state != SONY_CXD6801_ASCOT3_STATE_ACTIVE){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    result = X_tune_end(pTuner, pTuner->tvSystem);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_ShiftFRF(sony_cxd6801_ascot3_t *pTuner, uint32_t frequencykHz)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_ShiftFRF");

    /* Argument check */
    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* State check */
    if(pTuner->state != SONY_CXD6801_ASCOT3_STATE_ACTIVE){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* Rough frequency range check */
    if((frequencykHz < 1000) || (frequencykHz > 1200000)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_RANGE);
    }

    /* Tune using current system, no VCO calibration */
    result = X_tune(pTuner, frequencykHz, pTuner->tvSystem, 0);
    if(result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_RETURN(result);
    }

    pTuner->frequencykHz = frequencykHz;

    SONY_CXD6801_SLEEP(10);

    result = X_tune_end(pTuner, pTuner->tvSystem);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_Sleep(sony_cxd6801_ascot3_t *pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_Sleep");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* State check */
    if((pTuner->state != SONY_CXD6801_ASCOT3_STATE_SLEEP) && (pTuner->state != SONY_CXD6801_ASCOT3_STATE_ACTIVE)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    if(pTuner->state == SONY_CXD6801_ASCOT3_STATE_SLEEP){
        /* Nothing to do */
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
    }

    /* To Power Save state */
    result = X_fin(pTuner);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }

    if((pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF)){
        /* Disable Xtal */
        result = X_oscdis(pTuner);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }
    }

    pTuner->state = SONY_CXD6801_ASCOT3_STATE_SLEEP;
    pTuner->frequencykHz = 0;
    pTuner->tvSystem = SONY_CXD6801_ASCOT3_TV_SYSTEM_UNKNOWN;

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_SetGPO(sony_cxd6801_ascot3_t *pTuner, uint8_t id, uint8_t val)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_SetGPO");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* CXD2875 does not support GPO */
    if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2875){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    switch(id){
    case 0:
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x85, (uint8_t)(val ? 0x01 : 0x00));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
        break;
    case 1:
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x86, (uint8_t)(val ? 0x01 : 0x00));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
        break;
    default:
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_SetRfExtCtrl(sony_cxd6801_ascot3_t *pTuner, uint8_t enable)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_SetRfExtCtrl");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Only for CXD2871/2871A */
    if((pTuner->chipId != SONY_CXD6801_ASCOT3_CHIP_ID_2871) && (pTuner->chipId != SONY_CXD6801_ASCOT3_CHIP_ID_2871A)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    /* RF_EXT bit setting (0x67) */
    result = sony_cxd6801_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x67, (uint8_t)(enable ? 0x01 : 0x00), 0x01);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_ReadRssi(sony_cxd6801_ascot3_t *pTuner, int32_t *pRssi)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t ifagcreg = 0;
    uint8_t rfagcreg = 0;
    int32_t ifgain = 0;
    int32_t rfgain = 0;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_ReadRssi");

    if((!pTuner) || (!pTuner->pI2c) || (!pRssi)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Terrestrial only */
    if(pTuner->state != SONY_CXD6801_ASCOT3_STATE_ACTIVE){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    result = X_read_agc(pTuner, &ifagcreg, &rfagcreg);
    if(result != SONY_CXD6801_RESULT_OK){
        SONY_CXD6801_TRACE_RETURN(result);
    }

    /*
        IFGAIN = if(AGC > 0.39){
                     8.7 + IF_BPF_GC
                 }else{
                     8.7 + IF_BPF_GC + 76.9 * (0.39 - AGC)
                 }
        Note that AGC(V) = IFAGCReg(by X_read_agc) * 1.4 / 255
        So...
        IFGAIN(100xdB) = if(IFAGCReg * 140 > 9945){
                             870 + (IF_BPF_GC * 100)
                         }else{
                             870 + (IF_BPF_GC * 100) + (769 * (9945 - IFAGCReg * 140)) / 2550
                         }
    */
    {
        const int32_t if_bpf_gc_table[] = {0, 0, 0, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 20};
        uint8_t data = 0;
        int32_t if_bpf_gc_x100 = 0;
        int32_t agcreg_x140 = ifagcreg * 140;

        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x69, &data, 1);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        if_bpf_gc_x100 = if_bpf_gc_table[data & 0x0F] * 100;

        if(agcreg_x140 > 9945){
            ifgain = 870 + if_bpf_gc_x100;
        }else{
            ifgain = 870 + if_bpf_gc_x100 + (769 * (9945 - agcreg_x140) + 1275) / 2550; /* Round */
        }
    }

    /*
        RFGAIN = if(max(AGC,RFAGC) < 0.192){
                     RFGAIN_MAX
                 }else if(max(AGC,RFAGC) < 0.214){
                     RF_GAIN_MAX - 70 * (max(AGC,RFAGC) - 0.192)
                 }else if(max(AGC,RFAGC) < 0.346){
                     RF_GAIN_MAX - 70 * 0.022
                 }else if(max(AGC,RFAGC) < 0.972){
                     RF_GAIN_MAX - 70 * 0.022 - 70 * (max(AGC,RFAGC) - 0.346)
                 }else if(max(AGC,RFAGC) < 1.18){
                     RF_GAIN_MAX - 70 * 0.648 - 57 * (max(AGC,RFAGC) - 0.972)
                 }else{
                     RF_GAIN_MAX - 70 * 0.648 - 57 * 0.208 - 160 * (max(AGC,RFAGC) - 1.18)
                 }
        Note that AGC(V) = IFAGCReg(by X_read_agc) * 1.4 / 255
                  RFAGC(V) = RFAGCReg(by X_read_ss) * 1.4 / 255
        So...
        RFGAIN(100xdB) = if(maxagcreg * 140 < 4896){
                             RFGAIN_MAX * 100
                         }else if(maxagcreg * 140 < 5457){
                             RFGAIN_MAX * 100 - (70 * (maxagcreg * 140 - 4896))/255
                         }else if(maxagcreg * 140 < 8823){
                             RFGAIN_MAX * 100 - 70 * 2.2
                         }else if(maxagcreg * 140 < 24786){
                             RFGAIN_MAX * 100 - 70 * 2.2 - (70 * (maxagcreg * 140 - 8823))/255
                         }else if(maxagcreg * 140 < 30090){
                             RFGAIN_MAX * 100 - 70 * 64.8 - (57 * (maxagcreg * 140 - 24786))/255
                         }else{
                             RFGAIN_MAX * 100 - 70 * 64.8 - 57 * 20.8 - 160 * (maxagcreg * 140 - 30090))/255
                         }
        (NOTE: maxagcreg = max(IFAGCReg, RFAGCReg))
    */
    {
        int32_t maxagcreg_x140 = 0;
        int32_t rfgainmax_x100 = 0;

        if(ifagcreg > rfagcreg){
            maxagcreg_x140 = ifagcreg * 140;
        }else{
            maxagcreg_x140 = rfagcreg * 140;
        }

        if(pTuner->frequencykHz > 700000){
            rfgainmax_x100 = 4150;
        }else if(pTuner->frequencykHz > 600000){
            rfgainmax_x100 = 4130;
        }else if(pTuner->frequencykHz > 532000){
            rfgainmax_x100 = 4170;
        }else if(pTuner->frequencykHz > 464000){
            rfgainmax_x100 = 4050;
        }else if(pTuner->frequencykHz > 400000){
            rfgainmax_x100 = 4280;
        }else if(pTuner->frequencykHz > 350000){
            rfgainmax_x100 = 4260;
        }else if(pTuner->frequencykHz > 320000){
            rfgainmax_x100 = 4110;
        }else if(pTuner->frequencykHz > 285000){
            rfgainmax_x100 = 4310;
        }else if(pTuner->frequencykHz > 215000){
            rfgainmax_x100 = 4250;
        }else if(pTuner->frequencykHz > 184000){
            rfgainmax_x100 = 4020;
        }else if(pTuner->frequencykHz > 172000){
            rfgainmax_x100 = 3920;
        }else if(pTuner->frequencykHz > 150000){
            rfgainmax_x100 = 4080;
        }else if(pTuner->frequencykHz > 86000){
            rfgainmax_x100 = 4180;
        }else if(pTuner->frequencykHz > 65000){
            rfgainmax_x100 = 4200;
        }else if(pTuner->frequencykHz > 50000){
            rfgainmax_x100 = 4020;
        }else{
            rfgainmax_x100 = 4020;
        }

        if(maxagcreg_x140 < 4896){
            rfgain = rfgainmax_x100;
        }else if(maxagcreg_x140 < 5457){
            rfgain = rfgainmax_x100 - (70 * (maxagcreg_x140 - 4896) + 127) / 255; /* Round */
        }else if(maxagcreg_x140 < 8823){
            /* 154 = 70 * 2.2 */
            rfgain = rfgainmax_x100 - 154;
        }else if(maxagcreg_x140 < 24786){
            rfgain = rfgainmax_x100 - 154 - (70 * (maxagcreg_x140 - 8823) + 127) / 255; /* Round */
        }else if(maxagcreg_x140 < 30090){
            /* 4536 = 70 * 64.8 */
            rfgain = rfgainmax_x100 - 4536 - (57 * (maxagcreg_x140 - 24786) + 127) / 255; /* Round */
        }else{
            /* 1185.6 = 57 * 20.8 */
            rfgain = rfgainmax_x100 - 4536 - 1186 - (160 * (maxagcreg_x140 - 30090) + 127) / 255; /* Round */
        }
    }

    *pRssi = - ifgain - rfgain;

    if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2872){
        *pRssi += 220; /* + 2.2dBm */
    }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_ReadRssi_CPU(sony_cxd6801_ascot3_t *pTuner, int32_t *pRssi)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    int32_t rssiOut;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_ReadRssi_CPU");

    if((!pTuner) || (!pTuner->pI2c) || (!pRssi)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if(pTuner->state != SONY_CXD6801_ASCOT3_STATE_ACTIVE){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* CXD2871A does not support RSSI calculation by CPU */
    if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    result = X_rssi(pTuner, &rssiOut);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }

    /* RSSI_OUT[10:0] is (dBm x 8) integer value */
    if(rssiOut >= 0){
        *pRssi = (rssiOut * 100 + 4) / 8;
    }else{
        *pRssi = (rssiOut * 100 - 4) / 8;
    }

    /* RSSI adjustment (RSSI_OFS) */
    if(pTuner->frequencykHz > 532000){
        *pRssi += 100;
    }else if(pTuner->frequencykHz > 464000){
        *pRssi += 300;
    }else if(pTuner->frequencykHz > 350000){
        *pRssi += 100;
    }else if(pTuner->frequencykHz > 320000){
        *pRssi += 300;
    }else if(pTuner->frequencykHz > 215000){
        *pRssi += 100;
    }else if(pTuner->frequencykHz > 184000){
        *pRssi += 300;
    }else if(pTuner->frequencykHz > 172000){
        *pRssi += 400;
    }else if(pTuner->frequencykHz > 150000){
        *pRssi += 300;
    }else{
        *pRssi += 200;
    }

    if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2872){
        *pRssi += 220; /* + 2.2dBm */
    }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

sony_cxd6801_result_t sony_cxd6801_ascot3_RFFilterConfig(sony_cxd6801_ascot3_t *pTuner, uint8_t coeff, uint8_t offset)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("sony_cxd6801_ascot3_RFFilterConfig");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if((pTuner->state != SONY_CXD6801_ASCOT3_STATE_SLEEP) && (pTuner->state != SONY_CXD6801_ASCOT3_STATE_ACTIVE)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_SW_STATE);
    }

    /* CXD2871A/2875 does not support RF filter adjustment */
    if((pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A) || (pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2875)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    if(pTuner->state == SONY_CXD6801_ASCOT3_STATE_SLEEP){
        if((pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF)){
            /* Enable Xtal */
            result = X_oscen(pTuner);
            if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }
        }
    }

    /* Clock enable for internal logic block, CPU wake-up (0x87, 0x88) */
    {
        const uint8_t cdata[2] = {0xC4, 0x40};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    {
        uint8_t data[3];

        /* Write multiplier */
        data[0] = coeff;
        data[1] = 0x49;
        data[2] = 0x03;
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x16, data, sizeof(data));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        SONY_CXD6801_SLEEP(1);

        /* Write offset */
        data[0] = offset;
        data[1] = 0x4B;
        data[2] = 0x03;
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x16, data, sizeof(data));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        SONY_CXD6801_SLEEP(1);
    }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    if(pTuner->state == SONY_CXD6801_ASCOT3_STATE_SLEEP){
        if((pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF)){
            /* Disable Xtal */
            result = X_oscdis(pTuner);
            if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }
        }
    }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Implementation of static functions
------------------------------------------------------------------------------*/

static sony_cxd6801_result_t X_pon(sony_cxd6801_ascot3_t *pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("X_pon");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* 0x99, 0x9A */
    {
        /* Initial setting for internal logic block */
        const uint8_t cdata[] = {0x7A, 0x01};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x99, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* 0x81 - 0x94 */
    {
        uint8_t data[20];

        /* Frequency setting for crystal oscillator (0x81) */
        switch(pTuner->xtalFreq){
        case SONY_CXD6801_ASCOT3_XTAL_16000KHz:
            data[0] = 0x10;
            break;
        case SONY_CXD6801_ASCOT3_XTAL_20500KHz:
            data[0] = 0xD4;
            break;
        case SONY_CXD6801_ASCOT3_XTAL_24000KHz:
            data[0] = 0x18;
            break;
        case SONY_CXD6801_ASCOT3_XTAL_41000KHz:
            data[0] = 0x69;
            break;
        default:
            SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* Driver current setting for crystal oscillator (0x82) */
        /* Load capacitance setting for crystal oscillator (0x83) */
        if(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_EXT_REF){
            /* XOSC_APC_EN = 0, XOSC_SEL= 0uA */
            data[1] = 0x00;
            /* XOSC_CALC_EN = 0, XOSC_CAP_SET = 0pF */
            data[2] = 0x00;
        }else{
            /* XOSC_APC_EN = 1, XOSC_SEL = xosc_sel (sony_cxd6801_ascot3_t member) */
            data[1] = (uint8_t)(0x80 | (pTuner->xosc_sel & 0x1F));
            /* XOSC_CALC_EN = 1, XOSC_CAP_SET = xosc_cap_set (sony_cxd6801_ascot3_t member) */
            data[2] = (uint8_t)(0x80 | (pTuner->xosc_cap_set & 0x7F));
        }

        /* Setting for REFOUT signal output (0x84) */
        switch(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_REFOUT_MASK){
        case 0:
            data[3] = 0x00; /* REFOUT_EN = 0, REFOUT_CNT = 0 */
            break;
        case SONY_CXD6801_ASCOT3_CONFIG_REFOUT_500mVpp:
            data[3] = 0x80; /* REFOUT_EN = 1, REFOUT_CNT = 0 */
            break;
        case SONY_CXD6801_ASCOT3_CONFIG_REFOUT_400mVpp:
            data[3] = 0x81; /* REFOUT_EN = 1, REFOUT_CNT = 1 */
            break;
        case SONY_CXD6801_ASCOT3_CONFIG_REFOUT_600mVpp:
            data[3] = 0x82; /* REFOUT_EN = 1, REFOUT_CNT = 2 */
            break;
        case SONY_CXD6801_ASCOT3_CONFIG_REFOUT_800mVpp:
            data[3] = 0x83; /* REFOUT_EN = 1, REFOUT_CNT = 3 */
            break;
        default:
            SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* GPIO0, GPIO1 port setting (0x85, 0x86) */
        /* GPIO setting should be done by sony_cxd6801_ascot3_SetGPO after initialization */
        data[4] = 0x00;
        data[5] = 0x00;

        /* Clock enable for internal logic block (0x87) */
        data[6] = 0xC4;

        /* Start CPU boot-up (0x88) */
        data[7] = 0x40;

        /* For burst-write (0x89) */
        data[8] = 0x10;

        /* Setting for internal RFAGC (0x8A, 0x8B, 0x8C) */
        data[9] = 0x00;
        data[10] = 0x45;
        data[11] = 0x75;

        /* Setting for analog block (0x8D) */
        data[12] = 0x07;

        /* Initial setting for internal analog block (0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94) */
        data[13] = 0x1C;
        data[14] = 0x3F;
        data[15] = 0x02;
        data[16] = 0x10;
        data[17] = 0x20;
        data[18] = 0x0A;
        data[19] = 0x00;

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x81, data, sizeof(data));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* Setting for internal RFAGC (0x9B) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x9B, 0x00);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Wait 10ms */
    SONY_CXD6801_SLEEP(10);

    /* Check CPU_STT (0x1A) */
    {
        uint8_t rdata;

        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1A, &rdata, sizeof(rdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        if(rdata != 0x00){
            SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_HW_STATE); /* CPU_STT != 0x00 */
        }
    }

    /* Chip ID auto detection (for CXD2871/2872/2875) */
    if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_UNKNOWN){
        {
            const uint8_t cdata[] = {0x8C, 0x06}; /* 0x17, 0x18 */
            uint8_t data = 0;

            result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x17, cdata, sizeof(cdata));
            if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

            SONY_CXD6801_SLEEP(1);

            result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x19, &data, 1);
            if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

            if(data & 0x08){
                pTuner->chipId = SONY_CXD6801_ASCOT3_CHIP_ID_2875;
            }
        }

        if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_UNKNOWN){
            const uint8_t cdata[] = {0x96, 0x06}; /* 0x17, 0x18 */
            uint8_t data = 0;

            result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x17, cdata, sizeof(cdata));
            if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

            SONY_CXD6801_SLEEP(1);

            result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x19, &data, 1);
            if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

            if(data & 0x40){
                pTuner->chipId = SONY_CXD6801_ASCOT3_CHIP_ID_2871;
            }else{
                pTuner->chipId = SONY_CXD6801_ASCOT3_CHIP_ID_2872;
            }
        }
    }

    /* VCO current setting */
    if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A){
        const uint8_t cdata[] = {0x2A, 0x0E}; /* 0x17, 0x18 */
        uint8_t data = 0;

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x17, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        SONY_CXD6801_SLEEP(1);

        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x19, &data, 1);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x95, (uint8_t)(data & 0x0F));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }else{
        const uint8_t cdata[] = {0x8D, 0x06}; /* 0x17, 0x18 */
        uint8_t data = 0;

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x17, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        SONY_CXD6801_SLEEP(1);

        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x19, &data, 1);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x95, (uint8_t)((data >> 4) & 0x0F));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* SiP detection */
    if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A){
        uint8_t data = 0;

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0xB0, 0x00);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x30, 0xE0);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0xB1, 0x1E);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0xB3, 0x02);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        SONY_CXD6801_SLEEP(1);

        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0xB4, &data, 1);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0xB3, 0x00);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        SONY_CXD6801_SLEEP(1);

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0xB1, 0x00);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x30, 0xE1);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0xB0, 0x01);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        if((data & 0x04) == 0x00){
            pTuner->isSiP = 1; /* SiP internal tuner */
        }
    }

    result = X_fin(pTuner);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(result); }

    /* Load capacitance control setting for crystal oscillator (0x80) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x80, 0x01);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t X_tune(sony_cxd6801_ascot3_t *pTuner, uint32_t frequencykHz, sony_cxd6801_ascot3_tv_system_t tvSystem, uint8_t vcoCal)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("X_tune");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    if(vcoCal){
        /* Disable IF signal output (IF_OUT_SEL setting) (0x74) */
        result = sony_cxd6801_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x74, 0x02, 0x03);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* 0x85, 0x86 */
    /* GPIO0, GPIO1 is changed by sony_cxd6801_ascot3_SetGPO */

    /* Clock enable for internal logic block, CPU wake-up (0x87, 0x88) */
    {
        const uint8_t cdata[2] = {0xC4, 0x40};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* 0x91, 0x92 */
    {
        uint8_t data[2];

        /* Initial setting for internal analog block (0x91, 0x92) */
        if((tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_6) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_8) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_J83B_5_6)){
            data[0] = 0x16;
            data[1] = 0x26;
        }else{
            data[0] = 0x10;
            data[1] = 0x20;
        }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x91, data, sizeof(data));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* 0x9C, 0x9D */
    {
        uint8_t data[2];
        /* Setting for analog block (0x9C) */
        if((pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_LOOPFILTER_INTERNAL) && INTERNAL_LOOPFILTER_AVAILABLE(tvSystem)){
            data[0] = 0x90;
        }else{
            data[0] = 0x00;
        }

        /* Setting for local polarity (0x9D) */
        data[1] = (uint8_t)(pTuner->pParamTable[tvSystem].IS_LOWERLOCAL & 0x01);

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x9C, data, sizeof(data));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* 0x5E - 0x66 */
    {
        uint8_t data[9];

        /* Enable for analog block (0x5E, 0x5F, 0x60) */
        data[0] = 0xEE;
        data[1] = 0x02;
        data[2] = 0x1E;

        /* Tuning setting for CPU (0x61) */
        if(vcoCal){
            data[3] = 0x67;
        }else{
            data[3] = 0x45;
        }

        /* Setting for PLL reference divider (REF_R) (0x62) */
        if(SONY_CXD6801_ASCOT3_IS_ATV(tvSystem)){
            /* Analog (Fref = 2MHz (frequency > 464MHz) | Fref = 0.5MHz (frequency <= 464MHz)) */
            switch(pTuner->xtalFreq){
            case SONY_CXD6801_ASCOT3_XTAL_16000KHz:
                data[4] = (uint8_t)((frequencykHz > 464000) ? 0x08 : 0x20);
                break;
            case SONY_CXD6801_ASCOT3_XTAL_20500KHz:
                data[4] = (uint8_t)((frequencykHz > 464000) ? 0x0A : 0x29);
                break;
            case SONY_CXD6801_ASCOT3_XTAL_24000KHz:
                data[4] = (uint8_t)((frequencykHz > 464000) ? 0x0C : 0x30);
                break;
            case SONY_CXD6801_ASCOT3_XTAL_41000KHz:
                data[4] = (uint8_t)((frequencykHz > 464000) ? 0x14 : 0x52);
                break;
            default:
                SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
            }
        }else if((pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A)
            && ((tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_6) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_8) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_J83B_5_6))){
            /* DVB-C (ASCOT3I) (Fref = 1MHz) */
            switch(pTuner->xtalFreq){
            case SONY_CXD6801_ASCOT3_XTAL_16000KHz:
                data[4] = 0x10;
                break;
            case SONY_CXD6801_ASCOT3_XTAL_20500KHz:
                data[4] = 0x14;
                break;
            case SONY_CXD6801_ASCOT3_XTAL_24000KHz:
                data[4] = 0x18;
                break;
            case SONY_CXD6801_ASCOT3_XTAL_41000KHz:
                data[4] = 0x28;
                break;
            default:
                SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
            }
        }else{
            /* Digital (Fref = 8MHz) */
            switch(pTuner->xtalFreq){
            case SONY_CXD6801_ASCOT3_XTAL_16000KHz:
                data[4] = 0x02;
                break;
            case SONY_CXD6801_ASCOT3_XTAL_20500KHz:
                data[4] = 0x02;
                break;
            case SONY_CXD6801_ASCOT3_XTAL_24000KHz:
                data[4] = 0x03;
                break;
            case SONY_CXD6801_ASCOT3_XTAL_41000KHz:
                data[4] = 0x05;
                break;
            default:
                SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
            }
        }

        /* Tuning setting for analog block (0x63, 0x64, 0x65, 0x66) */
        if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A){
            if((pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_LOOPFILTER_INTERNAL) && INTERNAL_LOOPFILTER_AVAILABLE(tvSystem)){
                if(pTuner->xtalFreq == SONY_CXD6801_ASCOT3_XTAL_20500KHz){
                    data[5] = 0x2C;
                }else{
                    data[5] = 0x38;
                }
                data[6] = 0x1E;
                data[7] = 0x02;
                data[8] = 0x24;
            }else if(SONY_CXD6801_ASCOT3_IS_ATV(tvSystem)){
                if(pTuner->xtalFreq == SONY_CXD6801_ASCOT3_XTAL_20500KHz){
                    data[5] = (uint8_t)((frequencykHz > 464000) ? 0xB4 : 0x38);
                }else{
                    data[5] = (uint8_t)((frequencykHz > 464000) ? 0xB4 : 0x38);
                }
                data[6] = 0x78;
                data[7] = 0x08;
                data[8] = (uint8_t)((frequencykHz > 464000) ? 0x3C : 0x1C);
            }else if((tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_6) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_8) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_J83B_5_6)){
                if(pTuner->xtalFreq == SONY_CXD6801_ASCOT3_XTAL_20500KHz){
                    data[5] = 0x1C;
                }else{
                    data[5] = 0x1C;
                }
                data[6] = 0x78;
                data[7] = 0x08;
                data[8] = 0x1C;
            }else{
                if(pTuner->xtalFreq == SONY_CXD6801_ASCOT3_XTAL_20500KHz){
                    data[5] = 0x8C;
                }else{
                    data[5] = 0xB4;
                }
                data[6] = 0x78;
                data[7] = 0x08;
                data[8] = 0x30;
            }
        }else{
            if((pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_LOOPFILTER_INTERNAL) && INTERNAL_LOOPFILTER_AVAILABLE(tvSystem)){
                if(pTuner->xtalFreq == SONY_CXD6801_ASCOT3_XTAL_20500KHz){
                    data[5] = 0x2C;
                }else{
                    data[5] = 0x38;
                }
                data[6] = 0x1E;
                data[7] = 0x02;
                data[8] = 0x24;
            }else if(SONY_CXD6801_ASCOT3_IS_ATV(tvSystem)){
                data[5] = (uint8_t)((frequencykHz > 464000) ? 0x80 : 0x40);
                data[6] = 0x78;
                data[7] = 0x08;
                data[8] = (uint8_t)((frequencykHz > 464000) ? 0x04 : 0x08);
            }else if((tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_6) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_8) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_J83B_5_6)){
                if(pTuner->xtalFreq == SONY_CXD6801_ASCOT3_XTAL_20500KHz){
                    data[5] = 0x40;
                }else{
                    data[5] = 0x50;
                }
                data[6] = 0x78;
                data[7] = 0x08;
                data[8] = 0x30;
            }else{
                if(pTuner->xtalFreq == SONY_CXD6801_ASCOT3_XTAL_20500KHz){
                    data[5] = 0x8C;
                }else{
                    data[5] = 0xAF;
                }
                data[6] = 0x78;
                data[7] = 0x08;
                data[8] = 0x30;
            }
        }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5E, data, sizeof(data));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* 0x67 */
    if((pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2872) && (pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_LOOPTHRU_ENABLE)){
        /* LT_AMP_EN and LT_SF_EN should be 1. */
        result = sony_cxd6801_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x67, 0x06, 0x06);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }else{
        /* LT_AMP_EN and LT_SF_EN should be 0 in tuning. */
        result = sony_cxd6801_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x67, 0x00, 0x06);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* 0x68 - 0x78 */
    {
        uint8_t data[17];

        /* Setting for IFOUT_LIMIT (0x68) */
        data[0] = (uint8_t)(pTuner->pParamTable[tvSystem].OUTLMT & 0x03);

        /* Setting for IF BPF buffer gain (0x69) */
        /* RF_GAIN setting */
        if(pTuner->pParamTable[tvSystem].RF_GAIN == AUTO){
            data[1] = 0x80; /* RF_GAIN_SEL = 1 */
        }else{
            data[1] = (uint8_t)((pTuner->pParamTable[tvSystem].RF_GAIN << 4) & 0x70);
        }

        /* IF_BPF_GC setting */
        data[1] |= (uint8_t)(pTuner->pParamTable[tvSystem].IF_BPF_GC & 0x0F);

        /* Setting for internal RFAGC (0x6A, 0x6B, 0x6C) */
        data[2] = 0x00;
        if(frequencykHz <= 172000){
            data[3] = (uint8_t)(pTuner->pParamTable[tvSystem].RFOVLD_DET_LV1_VL & 0x0F);
            data[4] = (uint8_t)(pTuner->pParamTable[tvSystem].IFOVLD_DET_LV_VL & 0x07);
        }else if(frequencykHz <= 464000){
            data[3] = (uint8_t)(pTuner->pParamTable[tvSystem].RFOVLD_DET_LV1_VH & 0x0F);
            data[4] = (uint8_t)(pTuner->pParamTable[tvSystem].IFOVLD_DET_LV_VH & 0x07);
        }else{
            data[3] = (uint8_t)(pTuner->pParamTable[tvSystem].RFOVLD_DET_LV1_U & 0x0F);
            data[4] = (uint8_t)(pTuner->pParamTable[tvSystem].IFOVLD_DET_LV_U & 0x07);
        }
        data[4] |= 0x20;

        /* Setting for IF frequency and bandwidth */

        /* IF filter center frequency offset (IF_BPF_F0) (0x6D) */
        data[5] = (uint8_t)((pTuner->pParamTable[tvSystem].IF_BPF_F0 << 4) & 0x30);

        /* IF filter band width (BW) (0x6D) */
        data[5] |= (uint8_t)(pTuner->pParamTable[tvSystem].BW & 0x03);

        /* IF frequency offset value (FIF_OFFSET) (0x6E) */
        data[6] = (uint8_t)(pTuner->pParamTable[tvSystem].FIF_OFFSET & 0x1F);

        /* IF band width offset value (BW_OFFSET) (0x6F) */
        data[7] = (uint8_t)(pTuner->pParamTable[tvSystem].BW_OFFSET & 0x1F);

        /* RF tuning frequency setting (0x70, 0x71, 0x72) */
        data[8]  = (uint8_t)(frequencykHz & 0xFF);         /* FRF_L */
        data[9]  = (uint8_t)((frequencykHz >> 8) & 0xFF);  /* FRF_M */
        data[10] = (uint8_t)((frequencykHz >> 16) & 0x0F); /* FRF_H (bit[3:0]) */

        if(tvSystem == SONY_CXD6801_ASCOT3_ATV_L_DASH){
            data[10] |= 0x40; /* IS_L_DASH (bit[6]) */
        }

        if(SONY_CXD6801_ASCOT3_IS_ATV(tvSystem)){
            data[10] |= 0x80; /* IS_FP (bit[7]) */
        }

        /* Tuning command (0x73) */
        if(vcoCal){
            data[11] = 0xFF;
        }else{
            data[11] = 0x8F;
        }

        /* Enable IF output, AGC and IFOUT pin selection (0x74) */
        if((pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871) || (pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A)){
            data[12] = 0;

            if(pTuner->pParamTable[tvSystem].AGC_SEL == AUTO){
                /* AGC pin setting by config flags */
                if(SONY_CXD6801_ASCOT3_IS_ATV(tvSystem)){
                    /* Analog */
                    if(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_AGC2_ATV){
                        data[12] |= 0x10;
                    }
                }else{
                    /* Digital */
                    if(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_AGC2_DTV){
                        data[12] |= 0x10;
                    }
                }
            }else{
                /* AGC pin setting from parameter table */
                data[12] |= (uint8_t)((pTuner->pParamTable[tvSystem].AGC_SEL << 4) & 0x30);
            }

            if(pTuner->pParamTable[tvSystem].IF_OUT_SEL == AUTO){
                /* IFOUT pin setting by config flags */
                if(SONY_CXD6801_ASCOT3_IS_ATV(tvSystem)){
                    /* Analog */
                    if(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_IF2_ATV){
                        data[12] |= 0x01;
                    }
                }else{
                    /* Digital */
                    if(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_IF2_DTV){
                        data[12] |= 0x01;
                    }
                }
            }else{
                /* IFOUT pin setting from parameter table */
                data[12] |= (uint8_t)(pTuner->pParamTable[tvSystem].IF_OUT_SEL & 0x03);
            }
        }else if((pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2872) || (pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2875)){
            data[12] = 0x11;
        }else{
            SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* Tuning setting for analog block (0x75, 0x76, 0x77, 0x78) */
        if(SONY_CXD6801_ASCOT3_IS_ATV(tvSystem) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_6) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_DVBC_8) || (tvSystem == SONY_CXD6801_ASCOT3_DTV_J83B_5_6)){
            if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A){
                data[13] = 0xD9;
                data[14] = 0x0F;
                data[15] = 0x24;
                data[16] = 0x87;
            }else{
                data[13] = 0xD9;
                data[14] = 0x0F;
                data[15] = 0x25;
                data[16] = 0x87;
            }
        }else{
            if((pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2875) && (frequencykHz <= 100000)){
                data[13] = 0x09;
                data[14] = 0x00;
                data[15] = 0x27;
                data[16] = 0x87;
            }else{
                data[13] = 0x99;
                data[14] = 0x00;
                data[15] = 0x24;
                data[16] = 0x87;
            }
        }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x68, data, sizeof(data));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t X_tune_end(sony_cxd6801_ascot3_t *pTuner, sony_cxd6801_ascot3_tv_system_t tvSystem)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("X_tune_end");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t X_fin(sony_cxd6801_ascot3_t *pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("X_fin");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Loop Through setting And RFIN matching in Power Save (0x67) */
    {
        uint8_t data = 0;

        if((pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871) || (pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2871A)){
            if(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_RFIN_MATCHING_ENABLE){
                data = 0x02;
            }else{
                data = 0x00;
            }
        }else if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2872){
            if(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_LOOPTHRU_ENABLE){
                data = 0x06;
            }else if(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_RFIN_MATCHING_ENABLE){
                data = 0x02;
            }else{
                data = 0x00;
            }
        }else if(pTuner->chipId == SONY_CXD6801_ASCOT3_CHIP_ID_2875){
            data = 0x00;
        }else{
            SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
        }

        /* Keep RF_EXT bit */
        result = sony_cxd6801_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x67, data, 0xFE);
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* Disable IF signal output (IF_OUT_SEL setting) (0x74) */
    result = sony_cxd6801_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x74, 0x02, 0x03);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Power save setting for analog block (0x5E, 0x5F, 0x60) */
    {
        const uint8_t cdata[] = {0x15, 0x00, 0x00};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5E, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t X_oscdis(sony_cxd6801_ascot3_t *pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("X_oscdis");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Setting for REFOUT signal output (0x84) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x00);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Driver current setting for crystal oscillator (0x82) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x82, 0x00);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t X_oscen(sony_cxd6801_ascot3_t *pTuner)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;
    uint8_t data = 0;

    SONY_CXD6801_TRACE_ENTER("X_oscen");

    if((!pTuner) || (!pTuner->pI2c)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Driver current setting for crystal oscillator (0x82) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x82, 0x9F);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Setting for REFOUT signal output (0x84) */
    switch(pTuner->flags & SONY_CXD6801_ASCOT3_CONFIG_REFOUT_MASK){
    case 0:
        data = 0x00; /* REFOUT_EN = 0, REFOUT_CNT = 0 */
        break;
    case SONY_CXD6801_ASCOT3_CONFIG_REFOUT_500mVpp:
        data = 0x80; /* REFOUT_EN = 1, REFOUT_CNT = 0 */
        break;
    case SONY_CXD6801_ASCOT3_CONFIG_REFOUT_400mVpp:
        data = 0x81; /* REFOUT_EN = 1, REFOUT_CNT = 1 */
        break;
    case SONY_CXD6801_ASCOT3_CONFIG_REFOUT_600mVpp:
        data = 0x82; /* REFOUT_EN = 1, REFOUT_CNT = 2 */
        break;
    case SONY_CXD6801_ASCOT3_CONFIG_REFOUT_800mVpp:
        data = 0x83; /* REFOUT_EN = 1, REFOUT_CNT = 3 */
        break;
    default:
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_NOSUPPORT);
    }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, data);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_SLEEP(10);

    /* Driver current setting for crystal oscillator (0x82) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x82,
        (uint8_t)(0x80 | (pTuner->xosc_sel & 0x1F)));
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t X_read_agc(sony_cxd6801_ascot3_t *pTuner, uint8_t *pIFAGCReg, uint8_t *pRFAGCReg)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("X_read_agc");

    if((!pTuner) || (!pTuner->pI2c) || (!pIFAGCReg) || (!pRFAGCReg)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Clock enable for internal logic block, CPU wake-up (0x87, 0x88) */
    {
        const uint8_t cdata[2] = {0xC4, 0x41};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* Connect IFAGC, Start ADC (0x59, 0x5A) */
    {
        const uint8_t cdata[2] = {0x05, 0x01};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* ADC read out (0x5B) */
    result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5B, pIFAGCReg, 1);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Connect RFAGC, Start ADC (0x59, 0x5A) */
    {
        const uint8_t cdata[2] = {0x03, 0x01};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* ADC read out (0x5B) */
    result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5B, pRFAGCReg, 1);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* ADC disable (0x59) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, 0x04);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}

static sony_cxd6801_result_t X_rssi(sony_cxd6801_ascot3_t *pTuner, int32_t *pRssiOut)
{
    sony_cxd6801_result_t result = SONY_CXD6801_RESULT_OK;

    SONY_CXD6801_TRACE_ENTER("X_rssi");

    if((!pTuner) || (!pTuner->pI2c) || (!pRssiOut)){
        SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_ARG);
    }

    /* Clock enable for internal logic block, CPU wake-up (0x87, 0x88) */
    {
        const uint8_t cdata[2] = {0xC4, 0x40};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, cdata, sizeof(cdata));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }
    }

    /* Enable RSSI calculation (0x73) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x73, 0x30);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_SLEEP(5);

    {
        uint8_t data[2];

        /* The result of RSSI calculation (0x7A, 0x7B) */
        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x7A, data, sizeof(data));
        if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

        *pRssiOut = sony_cxd6801_Convert2SComplement((data[1] << 3) | ((data[0] >> 5) & 0x07), 11);
    }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != SONY_CXD6801_RESULT_OK){ SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_ERROR_I2C); }

    SONY_CXD6801_TRACE_RETURN(SONY_CXD6801_RESULT_OK);
}
