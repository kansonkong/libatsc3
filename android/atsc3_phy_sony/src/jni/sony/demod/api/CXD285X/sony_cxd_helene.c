/*------------------------------------------------------------------------------
  Copyright 2014-2016 Sony Semiconductor Solutions Corporation

  Last Updated  : 2016/07/01
  File Revision : 1.1.1.0
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
 Based on HELENE application note 1.6.2
------------------------------------------------------------------------------*/

#include "sony_cxd_helene.h"

#define sony_cxd_HELENE_USE_CXD2856_GENERATIONS /* For CXD2856/2857 */

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
#ifdef sony_cxd_HELENE_USE_CXD2856_GENERATIONS
/* For CXD2856 generation ICs */
static const sony_cxd_helene_terr_adjust_param_t g_terr_param_table[sony_cxd_HELENE_TERR_TV_SYSTEM_NUM] = {
    /*
         IF_BPF_GC                                           BW              BW_OFFSET
     RF_GAIN |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |     IS_LOWERLOCAL
       |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |        */
    {AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  0x00}, /**< sony_cxd_HELENE_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  0x00}, /**< sony_cxd_HELENE_ATV_MN_EIAJ   (System-M (Japan)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  0x00}, /**< sony_cxd_HELENE_ATV_MN_SAP    (System-M (US)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  0x00}, /**< sony_cxd_HELENE_ATV_MN_A2     (System-M (Korea)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  0x00}, /**< sony_cxd_HELENE_ATV_BG        (System-B/G) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_ATV_I         (System-I) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_ATV_DK        (System-D/K) */
    {AUTO, 0x03, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_ATV_L         (System-L) */
    {AUTO, 0x03, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  0x00}, /**< sony_cxd_HELENE_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_8VSB      (ATSC 8VSB) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_QAM       (US QAM) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), 0x00}, /**< sony_cxd_HELENE_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), 0x00}, /**< sony_cxd_HELENE_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), 0x00}, /**< sony_cxd_HELENE_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {AUTO, 0x08, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {AUTO, 0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), 0x00}, /**< sony_cxd_HELENE_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {AUTO, 0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {AUTO, 0x02, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), 0x00}, /**< sony_cxd_HELENE_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {AUTO, 0x02, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  0x00}, /**< sony_cxd_HELENE_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {AUTO, 0x04, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-5), OFFSET(2),  0x00}, /**< sony_cxd_HELENE_DTV_J83B_5_6  (J.83B 5.6Msps) */
    {AUTO, 0x03, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  0x00}  /**< sony_cxd_HELENE_DTV_DTMB      (DTMB) */
};
#else /* sony_cxd_HELENE_USE_CXD2856_GENERATIONS */
/* For CXD2854 or older generation ICs */
static const sony_cxd_helene_terr_adjust_param_t g_terr_param_table[sony_cxd_HELENE_TERR_TV_SYSTEM_NUM] = {
    /*
         IF_BPF_GC                                           BW              BW_OFFSET
     RF_GAIN |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |     IS_LOWERLOCAL
       |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |        */
    {AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  0x00}, /**< sony_cxd_HELENE_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  0x00}, /**< sony_cxd_HELENE_ATV_MN_EIAJ   (System-M (Japan)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  0x00}, /**< sony_cxd_HELENE_ATV_MN_SAP    (System-M (US)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  0x00}, /**< sony_cxd_HELENE_ATV_MN_A2     (System-M (Korea)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  0x00}, /**< sony_cxd_HELENE_ATV_BG        (System-B/G) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_ATV_I         (System-I) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_ATV_DK        (System-D/K) */
    {AUTO, 0x03, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_ATV_L         (System-L) */
    {AUTO, 0x03, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  0x00}, /**< sony_cxd_HELENE_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_8VSB      (ATSC 8VSB) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_QAM       (US QAM) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), 0x00}, /**< sony_cxd_HELENE_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), 0x00}, /**< sony_cxd_HELENE_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), 0x00}, /**< sony_cxd_HELENE_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), 0x00}, /**< sony_cxd_HELENE_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), 0x00}, /**< sony_cxd_HELENE_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), 0x00}, /**< sony_cxd_HELENE_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {AUTO, 0x03, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), 0x00}, /**< sony_cxd_HELENE_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {AUTO, 0x03, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  0x00}, /**< sony_cxd_HELENE_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-5), OFFSET(2),  0x00}, /**< sony_cxd_HELENE_DTV_J83B_5_6  (J.83B 5.6Msps) */
    {AUTO, 0x04, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  0x00}  /**< sony_cxd_HELENE_DTV_DTMB      (DTMB) */
};
#endif /* sony_cxd_HELENE_USE_CXD2856_GENERATIONS */

/*------------------------------------------------------------------------------
 Definitions of static functions
------------------------------------------------------------------------------*/
/**
 @brief Configure the HELENE tuner from Power On to Sleep state.
*/
static sony_cxd_result_t X_pon(sony_cxd_helene_t *pTuner);
/**
 @brief Configure the HELENE tuner for specified terrestrial broadcasting system.
*/
static sony_cxd_result_t TER_tune(sony_cxd_helene_t *pTuner, uint32_t frequencykHz,
    sony_cxd_helene_tv_system_t tvSystem, uint8_t vcoCal);
/**
 @brief Configure the HELENE tuner for specified satellite broadcasting system.
*/
static sony_cxd_result_t SAT_tune(sony_cxd_helene_t *pTuner, uint32_t frequencykHz,
    sony_cxd_helene_tv_system_t tvSystem,  uint32_t symbolRateksps, uint8_t vcoCal);
/**
 @brief The last part of terrestrial tuning sequence.
*/
static sony_cxd_result_t TER_tune_end(sony_cxd_helene_t *pTuner, sony_cxd_helene_tv_system_t tvSystem);
/**
 @brief The last part of satellite tuning sequence.
*/
static sony_cxd_result_t SAT_tune_end(sony_cxd_helene_t *pTuner, sony_cxd_helene_tv_system_t tvSystem);
/**
 @brief Configure the HELENE tuner from terrestrial to Power Save state.
*/
static sony_cxd_result_t TER_fin(sony_cxd_helene_t *pTuner);
/**
 @brief Configure the HELENE tuner from satellite to Power Save state.
*/
static sony_cxd_result_t SAT_fin(sony_cxd_helene_t *pTuner);
/**
 @brief Configure the HELENE tuner to Oscillation Stop state.
*/
static sony_cxd_result_t X_oscdis(sony_cxd_helene_t *pTuner);
/**
 @brief Configure the HELENE tuner back from Oscillation Stop state.
*/
static sony_cxd_result_t X_oscen(sony_cxd_helene_t *pTuner);
/**
 @brief Reading gain information to calculate IF and RF gain levels.
*/
static sony_cxd_result_t X_read_agc(sony_cxd_helene_t *pTuner, uint8_t *pIFAGCReg, uint8_t *pRFAGCReg);

/*------------------------------------------------------------------------------
 Definitions of internal used macros
------------------------------------------------------------------------------*/
/**
 @brief Checking that the internal loop filter can be used for the terrestrial broadcasting system.
*/
#define TERR_INTERNAL_LOOPFILTER_AVAILABLE(tvSystem) (sony_cxd_HELENE_IS_DTV(tvSystem)\
    && ((tvSystem) != sony_cxd_HELENE_DTV_DVBC_6) && ((tvSystem) != sony_cxd_HELENE_DTV_DVBC_8)\
    && ((tvSystem) != sony_cxd_HELENE_DTV_DVBC2_6) && ((tvSystem) != sony_cxd_HELENE_DTV_DVBC2_8)\
    && ((tvSystem) != sony_cxd_HELENE_DTV_J83B_5_6))

/*------------------------------------------------------------------------------
 Implementation of public functions.
------------------------------------------------------------------------------*/

sony_cxd_result_t sony_cxd_helene_Create(sony_cxd_helene_t *pTuner, sony_cxd_helene_xtal_t xtalFreq,
    uint8_t i2cAddress, sony_cxd_i2c_t *pI2c, uint32_t flags)
{
    sony_cxd_TRACE_ENTER("sony_cxd_helene_Create");

    if((!pTuner) || (!pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* 41MHz is only for external reference input */
    if((xtalFreq == sony_cxd_HELENE_XTAL_41000KHz) && !(flags & sony_cxd_HELENE_CONFIG_EXT_REF)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    /* sony_cxd_HELENE_CONFIG_EXT_REF and sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL cannot be used at the same time. */
    if((flags & sony_cxd_HELENE_CONFIG_EXT_REF) && (flags & sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    pTuner->state = sony_cxd_HELENE_STATE_UNKNOWN; /* Chip is not accessed for now. */
    pTuner->xtalFreq = xtalFreq;
    pTuner->pI2c = pI2c;
    pTuner->i2cAddress = i2cAddress;
    pTuner->flags = flags;
    pTuner->frequencykHz = 0;
    pTuner->symbolRateksps = 0;
    pTuner->tvSystem = sony_cxd_HELENE_TV_SYSTEM_UNKNOWN;
    pTuner->isFreesatMode = 0;
    pTuner->pTerrParamTable = g_terr_param_table;

    /* Xtal OSC reference value */
    if(flags & sony_cxd_HELENE_CONFIG_EXT_REF){
        pTuner->xosc_sel = 0x00;
        pTuner->xosc_cap_set = 0x00;
    }else{
        switch(xtalFreq){
        case sony_cxd_HELENE_XTAL_16000KHz:
            pTuner->xosc_sel = 0x04;     /* 4 x 25 = 100uA */
            pTuner->xosc_cap_set = 0x33; /* 51 x 0.25 = 12.75pF */
            break;
        case sony_cxd_HELENE_XTAL_20500KHz:
        case sony_cxd_HELENE_XTAL_24000KHz:
            pTuner->xosc_sel = 0x04;     /* 4 x 25 = 100uA */
            pTuner->xosc_cap_set = 0x26; /* 38 x 0.25 = 9.5pF */
            break;
        case sony_cxd_HELENE_XTAL_41000KHz:  /* 41MHz is for external reference input only. */
        default:
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
        }
    }

    pTuner->user = NULL;

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_Initialize(sony_cxd_helene_t *pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_Initialize");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    {
        uint8_t data = 0x00;

        /* Confirm connected device is HELENE */
        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x7F, &data, 1);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        if(((data & 0xF0) != 0x10) && ((data & 0xF0) != 0x80)){
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
        }
    }

    /* X_pon sequence */
    result = X_pon(pTuner);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

    /* Dummy tune to set RF parameter if terrestrial RF active mode is used */
    if((pTuner->flags & sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_MASK) == sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RF_ACTIVE){
        result = TER_tune(pTuner, 666000, sony_cxd_HELENE_DTV_DVBT_8, 1);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

        sony_cxd_SLEEP(50);

        result = TER_tune_end(pTuner, sony_cxd_HELENE_DTV_DVBT_8);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

        result = TER_fin(pTuner);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
    }

    if((pTuner->flags & sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & sony_cxd_HELENE_CONFIG_EXT_REF)){
        /* Disable Xtal */
        result = X_oscdis(pTuner);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
    }

    pTuner->state = sony_cxd_HELENE_STATE_SLEEP;
    pTuner->frequencykHz = 0;
    pTuner->tvSystem = sony_cxd_HELENE_TV_SYSTEM_UNKNOWN;
    pTuner->symbolRateksps = 0;

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_terr_Tune(sony_cxd_helene_t *pTuner, uint32_t frequencykHz,
    sony_cxd_helene_tv_system_t tvSystem)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_terr_Tune");

    /* Argument check */
    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    if(!sony_cxd_HELENE_IS_ATV(tvSystem) && !sony_cxd_HELENE_IS_DTV(tvSystem)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Rough frequency range check */
    if((frequencykHz < 1000) || (frequencykHz > 1200000)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_RANGE);
    }

    /* State check */
    switch(pTuner->state){
    case sony_cxd_HELENE_STATE_SLEEP:
        /* Set system to "Unknown". (for safe) */
        pTuner->tvSystem = sony_cxd_HELENE_TV_SYSTEM_UNKNOWN;

        if((pTuner->flags & sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & sony_cxd_HELENE_CONFIG_EXT_REF)){
            /* Enable Xtal */
            result = X_oscen(pTuner);
            if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
        }

        break;

    case sony_cxd_HELENE_STATE_ACTIVE_S:
        /* Set system to "Unknown". (for safe) */
        pTuner->tvSystem = sony_cxd_HELENE_TV_SYSTEM_UNKNOWN;

        result = SAT_fin(pTuner);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

        break;

    case sony_cxd_HELENE_STATE_ACTIVE_T:
        break;

    default:
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Broadcasting system dependent setting and tuning. */
    result = TER_tune(pTuner, frequencykHz, tvSystem, 1);
    if(result != sony_cxd_RESULT_OK){
        sony_cxd_TRACE_RETURN(result);
    }

    pTuner->state = sony_cxd_HELENE_STATE_ACTIVE_T;
    pTuner->frequencykHz = frequencykHz;
    pTuner->tvSystem = tvSystem;
    pTuner->symbolRateksps = 0;

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_terr_TuneEnd(sony_cxd_helene_t *pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_terr_TuneEnd");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* State check (terrestrial only) */
    if(pTuner->state != sony_cxd_HELENE_STATE_ACTIVE_T){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    result = TER_tune_end(pTuner, pTuner->tvSystem);
    if(result != sony_cxd_RESULT_OK){
        sony_cxd_TRACE_RETURN(result);
    }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_sat_Tune(sony_cxd_helene_t *pTuner, uint32_t frequencykHz,
    sony_cxd_helene_tv_system_t tvSystem, uint32_t symbolRateksps)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_sat_Tune");

    /* Argument check */
    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    if(!sony_cxd_HELENE_IS_STV(tvSystem)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Rough frequency range check */
    if((frequencykHz < 500000) || (frequencykHz > 2500000)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_RANGE);
    }

    /* Symbol rate is fixed in ISDB-S */
    if(tvSystem == sony_cxd_HELENE_STV_ISDBS){
        symbolRateksps = 28860;
    }

    /* State check */
    switch(pTuner->state){
    case sony_cxd_HELENE_STATE_SLEEP:
        /* Set system to "Unknown". (for safe) */
        pTuner->tvSystem = sony_cxd_HELENE_TV_SYSTEM_UNKNOWN;

        if((pTuner->flags & sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & sony_cxd_HELENE_CONFIG_EXT_REF)){
            /* Enable Xtal */
            result = X_oscen(pTuner);
            if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
        }

        break;

    case sony_cxd_HELENE_STATE_ACTIVE_T:
        /* Set system to "Unknown". (for safe) */
        pTuner->tvSystem = sony_cxd_HELENE_TV_SYSTEM_UNKNOWN;

        result = TER_fin(pTuner);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

        break;

    case sony_cxd_HELENE_STATE_ACTIVE_S:
        break;

    default:
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    /* Broadcasting system dependent setting and tuning. */
    result = SAT_tune(pTuner, frequencykHz, tvSystem, symbolRateksps, 1);
    if(result != sony_cxd_RESULT_OK){
        sony_cxd_TRACE_RETURN(result);
    }

    sony_cxd_SLEEP(10);

    result = SAT_tune_end(pTuner, tvSystem);
    if(result != sony_cxd_RESULT_OK){
        sony_cxd_TRACE_RETURN(result);
    }

    pTuner->state = sony_cxd_HELENE_STATE_ACTIVE_S;
    pTuner->frequencykHz = ((frequencykHz + 2) / 4) * 4;
    pTuner->tvSystem = tvSystem;
    pTuner->symbolRateksps = symbolRateksps;

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_ShiftFRF(sony_cxd_helene_t *pTuner, uint32_t frequencykHz)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_ShiftFRF");

    /* Argument check */
    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* State check */
    switch(pTuner->state){
    case sony_cxd_HELENE_STATE_ACTIVE_T:
        /* Rough frequency range check */
        if((frequencykHz < 1000) || (frequencykHz > 1200000)){
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_RANGE);
        }

        result = TER_tune(pTuner, frequencykHz, pTuner->tvSystem, 0);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

        pTuner->frequencykHz = frequencykHz;

        sony_cxd_SLEEP(10);

        result = TER_tune_end(pTuner, pTuner->tvSystem);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

        break;

    case sony_cxd_HELENE_STATE_ACTIVE_S:
        /* Rough frequency range check */
        if((frequencykHz < 500000) || (frequencykHz > 2500000)){
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_RANGE);
        }

        result = SAT_tune(pTuner, frequencykHz, pTuner->tvSystem, pTuner->symbolRateksps, 0);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

        pTuner->frequencykHz = frequencykHz;

        sony_cxd_SLEEP(10);

        result = SAT_tune_end(pTuner, pTuner->tvSystem);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }

        break;

    default:
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_Sleep(sony_cxd_helene_t *pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_Sleep");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* State check */
    switch(pTuner->state){
    case sony_cxd_HELENE_STATE_SLEEP:
        /* Nothing to do */
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);

    case sony_cxd_HELENE_STATE_ACTIVE_T:
        result = TER_fin(pTuner);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
        break;

    case sony_cxd_HELENE_STATE_ACTIVE_S:
        result = SAT_fin(pTuner);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
        break;

    default:
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if((pTuner->flags & sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & sony_cxd_HELENE_CONFIG_EXT_REF)){
        /* Disable Xtal */
        result = X_oscdis(pTuner);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
    }

    pTuner->state = sony_cxd_HELENE_STATE_SLEEP;
    pTuner->frequencykHz = 0;
    pTuner->tvSystem = sony_cxd_HELENE_TV_SYSTEM_UNKNOWN;
    pTuner->symbolRateksps = 0;

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_SetGPO(sony_cxd_helene_t *pTuner, uint8_t id, uint8_t val)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_SetGPO");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    switch(id){
    case 0:
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x85, (uint8_t)(val ? 0x01 : 0x00));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
        break;
    case 1:
        /* GPIO1_IN_SEL = 0 (0x86) */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x86, (uint8_t)(val ? 0x01 : 0x00));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
        break;
    default:
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_GetGPI1(sony_cxd_helene_t *pTuner, uint8_t *pVal)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t data = 0;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_GetGPI1");

    if((!pTuner) || (!pTuner->pI2c) || (!pVal)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* GPIO1_IN_SEL = 1 (0x86) */
    result = sony_cxd_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x86, 0x10, 0x10);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Read GPIO1_IN (0x4B) */
    result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x4B, &data, 1);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    *pVal = (uint8_t)(data & 0x01);

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_SetRfExtCtrl(sony_cxd_helene_t *pTuner, uint8_t enable)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_SetRfExtCtrl");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* RF_EXT bit setting (0x67) */
    result = sony_cxd_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x67, (uint8_t)(enable ? 0x01 : 0x00), 0x01);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_SetFreesatMode(sony_cxd_helene_t *pTuner, uint8_t enable)
{
    sony_cxd_TRACE_ENTER("sony_cxd_helene_SetFreesatMode");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    pTuner->isFreesatMode = enable ? 1 : 0;

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_ReadRssi(sony_cxd_helene_t *pTuner, int32_t *pRssi)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t ifagcreg = 0;
    uint8_t rfagcreg = 0;
    int32_t ifgain = 0;
    int32_t rfgain = 0;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_ReadRssi");

    if((!pTuner) || (!pTuner->pI2c) || (!pRssi)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Terrestrial only */
    if(pTuner->state != sony_cxd_HELENE_STATE_ACTIVE_T){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    result = X_read_agc(pTuner, &ifagcreg, &rfagcreg);
    if(result != sony_cxd_RESULT_OK){
        sony_cxd_TRACE_RETURN(result);
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
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

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

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

sony_cxd_result_t sony_cxd_helene_RFFilterConfig(sony_cxd_helene_t *pTuner, uint8_t coeff, uint8_t offset)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("sony_cxd_helene_RFFilterConfig");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    if((pTuner->state != sony_cxd_HELENE_STATE_SLEEP) && (pTuner->state != sony_cxd_HELENE_STATE_ACTIVE_T)
        && (pTuner->state != sony_cxd_HELENE_STATE_ACTIVE_S)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_SW_STATE);
    }

    if(pTuner->state == sony_cxd_HELENE_STATE_SLEEP){
        if((pTuner->flags & sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & sony_cxd_HELENE_CONFIG_EXT_REF)){
            /* Enable Xtal */
            result = X_oscen(pTuner);
            if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
        }
    }

    /* Clock enable for internal logic block, CPU wake-up (0x87, 0x88) */
    {
        const uint8_t cdata[2] = {0xC4, 0x40};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    {
        uint8_t data[3];

        /* Write multiplier */
        data[0] = coeff;
        data[1] = 0x49;
        data[2] = 0x03;
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x16, data, sizeof(data));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        sony_cxd_SLEEP(1);

        /* Write offset */
        data[0] = offset;
        data[1] = 0x4B;
        data[2] = 0x03;
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x16, data, sizeof(data));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        sony_cxd_SLEEP(1);
    }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    if(pTuner->state == sony_cxd_HELENE_STATE_SLEEP){
        if((pTuner->flags & sony_cxd_HELENE_CONFIG_SLEEP_DISABLEXTAL) && !(pTuner->flags & sony_cxd_HELENE_CONFIG_EXT_REF)){
            /* Disable Xtal */
            result = X_oscdis(pTuner);
            if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(result); }
        }
    }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Implementation of static functions
------------------------------------------------------------------------------*/

static sony_cxd_result_t X_pon(sony_cxd_helene_t *pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("X_pon");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Mode select (0x01) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x01, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* RFIN matching in power save (terrestrial) (0x67) */
    /* RFIN matching in power save (satellite) (0x43) */
    /* Power save setting for analog block (0x5E, 0x5F, 0x60) */
    /* Power save setting for analog block (0x0C) */
    {
        uint8_t dataT[4]; /* 0x5E - 0x60, 0x67 */
        uint8_t dataS[2]; /* 0x0C, 0x43 */

        switch(pTuner->flags & sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_MASK){
        case sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_NORMAL:
            dataT[0] = 0x15;
            dataT[1] = 0x00;
            dataT[2] = 0x00;
            dataT[3] = 0x00;
            break;
        case sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RFIN_MATCHING:
            dataT[0] = 0x15;
            dataT[1] = 0x00;
            dataT[2] = 0x00;
            dataT[3] = 0x02;
            break;
        case sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RF_ACTIVE:
            dataT[0] = 0x06;
            dataT[1] = 0x00;
            dataT[2] = 0x02;
            dataT[3] = 0x00;
            break;
        default:
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
        }

        {
            uint8_t lnaOff = (pTuner->flags & sony_cxd_HELENE_CONFIG_SAT_LNA_OFF) ? 1 : 0;
            uint8_t terrRfActive = ((pTuner->flags & sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_MASK)
                == sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RF_ACTIVE) ? 1 : 0;

            switch(pTuner->flags & sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_MASK){
            case sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_NORMAL:
                dataS[0] = terrRfActive ? 0x04 : 0x14;
                dataS[1] = lnaOff       ? 0x05 : 0x07;
                break;
            case sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_RFIN_MATCHING:
                dataS[0] = terrRfActive ? 0x04 : 0x14;
                dataS[1] = lnaOff       ? 0x04 : 0x06;
                break;
            case sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_RF_ACTIVE:
                if(lnaOff){
                    dataS[0] = terrRfActive ? 0x06 : 0x16;
                }else{
                    dataS[0] = terrRfActive ? 0x05 : 0x15;
                }
                dataS[1] = lnaOff       ? 0x04 : 0x06;
                break;
            default:
                sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
            }
        }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x67, dataT[3]);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x43, dataS[1]);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5E, &dataT[0], 3);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x0C, dataS[0]);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* 0x99, 0x9A */
    {
        /* Initial setting for internal logic block */
        const uint8_t cdata[] = {0x7A, 0x01};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x99, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* 0x81 - 0x94 */
    {
        uint8_t data[20];

        /* Frequency setting for crystal oscillator (0x81) */
        switch(pTuner->xtalFreq){
        case sony_cxd_HELENE_XTAL_16000KHz:
            data[0] = 0x10;
            break;
        case sony_cxd_HELENE_XTAL_20500KHz:
            data[0] = 0xD4;
            break;
        case sony_cxd_HELENE_XTAL_24000KHz:
            data[0] = 0x18;
            break;
        case sony_cxd_HELENE_XTAL_41000KHz:
            data[0] = 0x69;
            break;
        default:
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
        }

        /* Driver current setting for crystal oscillator (0x82) */
        /* Load capacitance setting for crystal oscillator (0x83) */
        if(pTuner->flags & sony_cxd_HELENE_CONFIG_EXT_REF){
            /* XOSC_APC_EN = 0, XOSC_SEL= 0uA */
            data[1] = 0x00;
            /* XOSC_CALC_EN = 0, XOSC_CAP_SET = 0pF */
            data[2] = 0x00;
        }else{
            /* XOSC_APC_EN = 1, XOSC_SEL = xosc_sel (sony_cxd_helene_t member) */
            data[1] = (uint8_t)(0x80 | (pTuner->xosc_sel & 0x1F));
            /* XOSC_CALC_EN = 1, XOSC_CAP_SET = xosc_cap_set (sony_cxd_helene_t member) */
            data[2] = (uint8_t)(0x80 | (pTuner->xosc_cap_set & 0x7F));
        }

        /* Setting for REFOUT signal output (0x84) */
        switch(pTuner->flags & sony_cxd_HELENE_CONFIG_REFOUT_MASK){
        case 0:
            data[3] = 0x00; /* REFOUT_EN = 0, REFOUT_CNT = 0 */
            break;
        case sony_cxd_HELENE_CONFIG_REFOUT_500mVpp:
            data[3] = 0x80; /* REFOUT_EN = 1, REFOUT_CNT = 0 */
            break;
        case sony_cxd_HELENE_CONFIG_REFOUT_400mVpp:
            data[3] = 0x81; /* REFOUT_EN = 1, REFOUT_CNT = 1 */
            break;
        case sony_cxd_HELENE_CONFIG_REFOUT_600mVpp:
            data[3] = 0x82; /* REFOUT_EN = 1, REFOUT_CNT = 2 */
            break;
        case sony_cxd_HELENE_CONFIG_REFOUT_800mVpp:
            data[3] = 0x83; /* REFOUT_EN = 1, REFOUT_CNT = 3 */
            break;
        default:
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
        }

        /* GPIO0, GPIO1 port setting (0x85, 0x86) */
        /* GPIO setting should be done by sony_cxd_helene_SetGPO/sony_cxd_helene_GetGPI1 after initialization */
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
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* Setting for internal RFAGC (0x9B) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x9B, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Wait 10ms */
    sony_cxd_SLEEP(10);

    /* Check CPU_STT (0x1A) */
    {
        uint8_t rdata;

        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x1A, &rdata, sizeof(rdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        if(rdata != 0x00){
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_HW_STATE); /* CPU_STT != 0x00 */
        }
    }

    /* VCO current setting */
    {
        const uint8_t cdata[] = {0x90, 0x06}; /* 0x17, 0x18 */
        uint8_t data = 0;

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x17, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        sony_cxd_SLEEP(1);

        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x19, &data, 1);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x95, (uint8_t)((data >> 4) & 0x0F));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* Disable IF signal output (IF_OUT_SEL setting) (0x74) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x74, 0x02);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Load capacitance control setting for crystal oscillator (0x80) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x80, 0x01);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Satellite initial setting (0x41, 0x42) */
    {
        const uint8_t cdata[] = {0x07, 0x00};

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x41, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t TER_tune(sony_cxd_helene_t *pTuner, uint32_t frequencykHz,
    sony_cxd_helene_tv_system_t tvSystem, uint8_t vcoCal)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("TER_tune");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Mode select (0x01) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x01, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    if(vcoCal){
        /* Disable IF signal output (IF_OUT_SEL setting) (0x74) */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x74, 0x02);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* 0x85, 0x86 */
    /* GPIO0, GPIO1 is changed by sony_cxd_helene_SetGPO/sony_cxd_helene_GetGPI1 */

    /* Clock enable for internal logic block, CPU wake-up (0x87, 0x88) */
    {
        const uint8_t cdata[2] = {0xC4, 0x40};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* 0x91, 0x92 */
    {
        uint8_t data[2];

        /* Initial setting for internal analog block (0x91, 0x92) */
        if((tvSystem == sony_cxd_HELENE_DTV_DVBC_6) || (tvSystem == sony_cxd_HELENE_DTV_DVBC_8) || (tvSystem == sony_cxd_HELENE_DTV_J83B_5_6)){
            data[0] = 0x16;
            data[1] = 0x26;
        }else{
            data[0] = 0x10;
            data[1] = 0x20;
        }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x91, data, sizeof(data));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* 0x9C, 0x9D */
    {
        uint8_t data[2];
        /* Setting for analog block (0x9C) */
        if((pTuner->flags & sony_cxd_HELENE_CONFIG_LOOPFILTER_INTERNAL) && TERR_INTERNAL_LOOPFILTER_AVAILABLE(tvSystem)){
            data[0] = 0x90;
        }else{
            data[0] = 0x00;
        }

        /* Setting for local polarity (0x9D) */
        data[1] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].IS_LOWERLOCAL & 0x01);

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x9C, data, sizeof(data));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
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
        if(sony_cxd_HELENE_IS_ATV(tvSystem)){
            /* Analog (Fref = 2MHz (frequency > 464MHz) | Fref = 0.5MHz (frequency <= 464MHz)) */
            switch(pTuner->xtalFreq){
            case sony_cxd_HELENE_XTAL_16000KHz:
                data[4] = (uint8_t)((frequencykHz > 464000) ? 0x08 : 0x20);
                break;
            case sony_cxd_HELENE_XTAL_20500KHz:
                data[4] = (uint8_t)((frequencykHz > 464000) ? 0x0A : 0x29);
                break;
            case sony_cxd_HELENE_XTAL_24000KHz:
                data[4] = (uint8_t)((frequencykHz > 464000) ? 0x0C : 0x30);
                break;
            case sony_cxd_HELENE_XTAL_41000KHz:
                data[4] = (uint8_t)((frequencykHz > 464000) ? 0x14 : 0x52);
                break;
            default:
                sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
            }
        }else if((tvSystem == sony_cxd_HELENE_DTV_DVBC_6) || (tvSystem == sony_cxd_HELENE_DTV_DVBC_8) || (tvSystem == sony_cxd_HELENE_DTV_J83B_5_6)){
            /* DVB-C (Fref = 1MHz) */
            switch(pTuner->xtalFreq){
            case sony_cxd_HELENE_XTAL_16000KHz:
                data[4] = 0x10;
                break;
            case sony_cxd_HELENE_XTAL_20500KHz:
                data[4] = 0x14;
                break;
            case sony_cxd_HELENE_XTAL_24000KHz:
                data[4] = 0x18;
                break;
            case sony_cxd_HELENE_XTAL_41000KHz:
                data[4] = 0x28;
                break;
            default:
                sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
            }
        }else{
            /* Digital (Fref = 8MHz) */
            switch(pTuner->xtalFreq){
            case sony_cxd_HELENE_XTAL_16000KHz:
                data[4] = 0x02;
                break;
            case sony_cxd_HELENE_XTAL_20500KHz:
                data[4] = 0x02;
                break;
            case sony_cxd_HELENE_XTAL_24000KHz:
                data[4] = 0x03;
                break;
            case sony_cxd_HELENE_XTAL_41000KHz:
                data[4] = 0x05;
                break;
            default:
                sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
            }
        }

        /* Tuning setting for analog block (0x63, 0x64, 0x65, 0x66) */
        if((pTuner->flags & sony_cxd_HELENE_CONFIG_LOOPFILTER_INTERNAL) && TERR_INTERNAL_LOOPFILTER_AVAILABLE(tvSystem)){
            if(pTuner->xtalFreq == sony_cxd_HELENE_XTAL_20500KHz){
                data[5] = 0x2C;
            }else{
                data[5] = 0x38;
            }
            data[6] = 0x1E;
            data[7] = 0x02;
            data[8] = 0x24;
        }else if(sony_cxd_HELENE_IS_ATV(tvSystem)){
            if(pTuner->xtalFreq == sony_cxd_HELENE_XTAL_20500KHz){
                data[5] = (uint8_t)((frequencykHz > 464000) ? 0xB4 : 0x38);
            }else{
                data[5] = (uint8_t)((frequencykHz > 464000) ? 0xB4 : 0x38);
            }
            data[6] = 0x78;
            data[7] = 0x08;
            data[8] = (uint8_t)((frequencykHz > 464000) ? 0x3C : 0x1C);
        }else if((tvSystem == sony_cxd_HELENE_DTV_DVBC_6) || (tvSystem == sony_cxd_HELENE_DTV_DVBC_8) || (tvSystem == sony_cxd_HELENE_DTV_J83B_5_6)){
            if(pTuner->xtalFreq == sony_cxd_HELENE_XTAL_20500KHz){
                data[5] = 0x1C;
            }else{
                data[5] = 0x1C;
            }
            data[6] = 0x78;
            data[7] = 0x08;
            data[8] = 0x1C;
        }else{
            if(pTuner->xtalFreq == sony_cxd_HELENE_XTAL_20500KHz){
                data[5] = 0x8C;
            }else{
                data[5] = 0xB4;
            }
            data[6] = 0x78;
            data[7] = 0x08;
            data[8] = 0x30;
        }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5E, data, sizeof(data));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* LT_AMP_EN should be 0 (0x67) */
    result = sony_cxd_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x67, 0x00, 0x02);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* 0x68 - 0x78 */
    {
        uint8_t data[17];

        /* Setting for IFOUT_LIMIT (0x68) */
        if(sony_cxd_HELENE_IS_ATV(tvSystem)){
            data[0] = (uint8_t)((pTuner->flags & sony_cxd_HELENE_CONFIG_OUTLMT_ATV_1_2Vpp) ? 0x01 : 0x00);
        }else if(sony_cxd_HELENE_IS_DTV(tvSystem)){
            data[0] = (uint8_t)((pTuner->flags & sony_cxd_HELENE_CONFIG_OUTLMT_DTV_1_2Vpp) ? 0x01 : 0x00);
        }else{
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
        }

        /* Setting for IF BPF buffer gain (0x69) */
        /* RF_GAIN setting */
        if(pTuner->pTerrParamTable[tvSystem].RF_GAIN == AUTO){
            data[1] = 0x80; /* RF_GAIN_SEL = 1 */
        }else{
            data[1] = (uint8_t)((pTuner->pTerrParamTable[tvSystem].RF_GAIN << 4) & 0x70);
        }

        /* IF_BPF_GC setting */
        data[1] |= (uint8_t)(pTuner->pTerrParamTable[tvSystem].IF_BPF_GC & 0x0F);

        /* Setting for internal RFAGC (0x6A, 0x6B, 0x6C) */
        data[2] = 0x00;
        if(frequencykHz <= 172000){
            data[3] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].RFOVLD_DET_LV1_VL & 0x0F);
            data[4] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].IFOVLD_DET_LV_VL & 0x07);
        }else if(frequencykHz <= 464000){
            data[3] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].RFOVLD_DET_LV1_VH & 0x0F);
            data[4] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].IFOVLD_DET_LV_VH & 0x07);
        }else{
            data[3] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].RFOVLD_DET_LV1_U & 0x0F);
            data[4] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].IFOVLD_DET_LV_U & 0x07);
        }
        data[4] |= 0x20;

        /* Setting for IF frequency and bandwidth */

        /* IF filter center frequency offset (IF_BPF_F0) (0x6D) */
        data[5] = (uint8_t)((pTuner->pTerrParamTable[tvSystem].IF_BPF_F0 << 4) & 0x30);

        /* IF filter band width (BW) (0x6D) */
        data[5] |= (uint8_t)(pTuner->pTerrParamTable[tvSystem].BW & 0x03);

        /* IF frequency offset value (FIF_OFFSET) (0x6E) */
        data[6] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].FIF_OFFSET & 0x1F);

        /* IF band width offset value (BW_OFFSET) (0x6F) */
        data[7] = (uint8_t)(pTuner->pTerrParamTable[tvSystem].BW_OFFSET & 0x1F);

        /* RF tuning frequency setting (0x70, 0x71, 0x72) */
        data[8]  = (uint8_t)(frequencykHz & 0xFF);         /* FRF_L */
        data[9]  = (uint8_t)((frequencykHz >> 8) & 0xFF);  /* FRF_M */
        data[10] = (uint8_t)((frequencykHz >> 16) & 0x0F); /* FRF_H (bit[3:0]) */

        if(tvSystem == sony_cxd_HELENE_ATV_L_DASH){
            data[10] |= 0x40; /* IS_L_DASH (bit[6]) */
        }

        if(sony_cxd_HELENE_IS_ATV(tvSystem)){
            data[10] |= 0x80; /* IS_FP (bit[7]) */
        }

        /* Tuning command (0x73) */
        if(vcoCal){
            data[11] = 0xFF;
        }else{
            data[11] = 0x8F;
        }

        /* Enable IF output, AGC and IFOUT pin selection (0x74) */
        data[12] = 0x01;

        /* Tuning setting for analog block (0x75, 0x76, 0x77, 0x78) */
        if(sony_cxd_HELENE_IS_ATV(tvSystem) || (tvSystem == sony_cxd_HELENE_DTV_DVBC_6) || (tvSystem == sony_cxd_HELENE_DTV_DVBC_8) || (tvSystem == sony_cxd_HELENE_DTV_J83B_5_6)){
            data[13] = 0xD9;
            data[14] = 0x0F;
            data[15] = 0x24;
            data[16] = 0x87;
        }else{
            data[13] = 0x99;
            data[14] = 0x00;
            data[15] = 0x24;
            data[16] = 0x87;
        }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x68, data, sizeof(data));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t TER_tune_end(sony_cxd_helene_t *pTuner, sony_cxd_helene_tv_system_t tvSystem)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("TER_tune_end");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t SAT_tune(sony_cxd_helene_t *pTuner, uint32_t frequencykHz,
    sony_cxd_helene_tv_system_t tvSystem, uint32_t symbolRateksps, uint8_t vcoCal)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("SAT_tune");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    if(vcoCal){
        /* Disable IF signal output (IF_OUT_SEL setting) (0x15) */
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x15, 0x02);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* RFIN matching in power save (Sat) reset (0x43) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x43,
        (uint8_t)((pTuner->flags & sony_cxd_HELENE_CONFIG_SAT_LNA_OFF) ? 0x04 : 0x06));
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Analog block setting (0x6A, 0x6B) */
    {
        const uint8_t cdata[2] = {0x00, 0x00};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x6A, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* Analog block setting (0x75) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x75, 0x99);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Analog block setting (0x9D) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x9D, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Tuning setting for CPU (0x61) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x61, (uint8_t)(vcoCal ? 0x07 : 0x05));
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Satellite mode select (0x01) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x01, 0x01);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* 0x02, 0x03 */
    /* GPIO0, GPIO1 is changed by sony_cxd_helene_SetGPO/sony_cxd_helene_GetGPI1 */

    /* 0x04 - 0x15 */
    {
        uint8_t data[18];

        /* Clock enable for internal logic block, CPU wake-up (0x04, 0x05) */
        data[0] = 0xC4;
        data[1] = 0x40;

        /* Setting for PLL reference divider (REF_R) (0x06) */
        switch(pTuner->xtalFreq){
        case sony_cxd_HELENE_XTAL_16000KHz:
            data[2] = 0x02;
            break;
        case sony_cxd_HELENE_XTAL_20500KHz:
            data[2] = 0x02;
            break;
        case sony_cxd_HELENE_XTAL_24000KHz:
            data[2] = 0x03;
            break;
        case sony_cxd_HELENE_XTAL_41000KHz:
            data[2] = 0x05;
            break;
        default:
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
        }

        /* Setting for analog block (0x07) */
        if(pTuner->flags & sony_cxd_HELENE_CONFIG_LOOPFILTER_INTERNAL){
            data[3] = 0x80;
        }else{
            data[3] = 0x00;
        }

        /* Tuning setting for analog block (0x08, 0x09, 0x0A, 0x0B) */
        if(pTuner->flags & sony_cxd_HELENE_CONFIG_LOOPFILTER_INTERNAL){
            if(pTuner->xtalFreq == sony_cxd_HELENE_XTAL_20500KHz){
                data[4] = 0x58;
            }else{
                data[4] = 0x70;
            }
            data[5] = 0x1E;
            data[6] = 0x02;
            data[7] = 0x24;
        }else{
            if(pTuner->isFreesatMode){
                if(pTuner->xtalFreq == sony_cxd_HELENE_XTAL_20500KHz){
                    data[4] = 0x18;
                }else{
                    data[4] = 0x20;
                }
            }else{
                if(pTuner->xtalFreq == sony_cxd_HELENE_XTAL_20500KHz){
                    data[4] = 0x8C;
                }else{
                    data[4] = 0xB4;
                }
            }
            data[5] = 0x78;
            data[6] = 0x08;
            data[7] = 0x30;
        }

        /* Enable for analog block (0x0C, 0x0D, 0x0E) */
        if(pTuner->flags & sony_cxd_HELENE_CONFIG_SAT_LNA_OFF){
            data[8] = 0x0E; /* Clear SAT_RF_LNA_EN (Bit[0]) */
        }else{
            data[8] = 0x0F;
        }

        if((pTuner->flags & sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_MASK)
            == sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RF_ACTIVE){
            data[8] |= 0xE0; /* Clear RFOVLD_DET_ENX (Bit[4]) */
        }else{
            data[8] |= 0xF0;
        }

        data[9]  = 0x02;
        data[10] = 0x1E;

        /* Setting for LPF cutoff frequency (0x0F) */
        switch(tvSystem){
        case sony_cxd_HELENE_STV_ISDBS:
            data[11] = 22; /* 22MHz */
            break;

        case sony_cxd_HELENE_STV_DVBS:
            /*
                rolloff = 0.35

                SR <= 4
                  lpf_cutoff = 5
                4 < SR <= 10
                  lpf_cutoff = SR / 2 * (2 + rolloff) = SR * 1.175 = SR * (47/40)
                10 < SR
                  lpf_cutoff = SR / 2 * (1 + rolloff) + 5 = SR * 0.675 + 5 = SR * (27/40) + 5
                NOTE: The result should be round up.
            */
            if(symbolRateksps <= 4000){
                data[11] = 5;
            }else if(symbolRateksps <= 10000){
                data[11] = (uint8_t)((symbolRateksps * 47 + (40000-1)) / 40000);
            }else{
                data[11] = (uint8_t)((symbolRateksps * 27 + (40000-1)) / 40000 + 5);
            }

            if(data[11] > 36){
                data[11] = 36; /* 5 <= lpf_cutoff <= 36 is valid */
            }
            break;

        case sony_cxd_HELENE_STV_DVBS2:
            /*
                rolloff = 0.2

                SR <= 4
                  lpf_cutoff = 5
                4 < SR <= 10
                  lpf_cutoff = SR / 2 * (2 + rolloff) = SR * 1.1 = SR * (11/10)
                10 < SR
                  lpf_cutoff = SR / 2 * (1 + rolloff) + 5 = SR * 0.6 + 5 = SR * (3/5) + 5
                NOTE: The result should be round up.
            */
            if(symbolRateksps <= 4000){
                data[11] = 5;
            }else if(symbolRateksps <= 10000){
                data[11] = (uint8_t)((symbolRateksps * 11 + (10000-1)) / 10000);
            }else{
                data[11] = (uint8_t)((symbolRateksps * 3 + (5000-1)) / 5000 + 5);
            }

            if(data[11] > 36){
                data[11] = 36; /* 5 <= lpf_cutoff <= 36 is valid */
            }
            break;

        default:
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG); /* Invalid system */
        }

        /* RF tuning frequency setting (0x10, 0x11, 0x12) */
        {
            uint32_t frequency4kHz = (frequencykHz + 2) / 4;
            data[12] = (uint8_t)(frequency4kHz & 0xFF);         /* FRF_L */
            data[13] = (uint8_t)((frequency4kHz >> 8) & 0xFF);  /* FRF_M */
            data[14] = (uint8_t)((frequency4kHz >> 16) & 0x0F); /* FRF_H (bit[3:0]) */
        }

        /* Tuning command (0x13) */
        if(vcoCal){
            data[15] = 0xFF;
        }else{
            data[15] = 0x8F;
        }

        /* Setting for IQOUT_LIMIT (0x14) */
        data[16] = (uint8_t)((pTuner->flags & sony_cxd_HELENE_CONFIG_OUTLMT_STV_0_6Vpp) ? 0x01 : 0x00);

        /* Enable IQ output (0x15) */
        data[17] = 0x01;

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x04, data, sizeof(data));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t SAT_tune_end(sony_cxd_helene_t *pTuner, sony_cxd_helene_tv_system_t tvSystem)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("SAT_tune_end");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Standby setting for CPU (0x05) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x05, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for internal logic block (0x04) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x04, 0xC0);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t TER_fin(sony_cxd_helene_t *pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("TER_fin");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Disable IF signal output (IF_OUT_SEL setting) (0x74) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x74, 0x02);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* RFIN matching in power save (terrestrial) (0x67) */
    /* Power save setting for analog block (0x5E, 0x5F, 0x60) */
    {
        uint8_t data[4]; /* 0x5E - 0x60, 0x67 */

        switch(pTuner->flags & sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_MASK){
        case sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_NORMAL:
            data[0] = 0x15;
            data[1] = 0x00;
            data[2] = 0x00;
            data[3] = 0x00;
            break;
        case sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RFIN_MATCHING:
            data[0] = 0x15;
            data[1] = 0x00;
            data[2] = 0x00;
            data[3] = 0x02;
            break;
        case sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RF_ACTIVE:
            data[0] = 0x06;
            data[1] = 0x00;
            data[2] = 0x02;
            data[3] = 0x00;
            break;
        default:
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
        }

        /* Keep RF_EXT bit */
        result = sony_cxd_i2c_SetRegisterBits(pTuner->pI2c, pTuner->i2cAddress, 0x67, data[3], 0xFE);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5E, &data[0], 3);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t SAT_fin(sony_cxd_helene_t *pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("SAT_fin");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Disable IQ signal output (IF_OUT_SEL setting) (0x15) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x15, 0x02);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* RFIN matching in power save (satellite) (0x43) */
    /* Power save setting for analog block (0x0C, 0x0D, 0x0E) */
    {
        uint8_t data[4]; /* 0x0C - 0x0E, 0x43 */
        uint8_t lnaOff = (pTuner->flags & sony_cxd_HELENE_CONFIG_SAT_LNA_OFF) ? 1 : 0;
        uint8_t terrRfActive = ((pTuner->flags & sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_MASK)
            == sony_cxd_HELENE_CONFIG_POWERSAVE_TERR_RF_ACTIVE) ? 1 : 0;

        switch(pTuner->flags & sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_MASK){
        case sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_NORMAL:
            data[0] = terrRfActive ? 0x04 : 0x14;
            data[1] = 0x00;
            data[2] = terrRfActive ? 0x02 : 0x00;
            data[3] = lnaOff       ? 0x05 : 0x07;
            break;
        case sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_RFIN_MATCHING:
            data[0] = terrRfActive ? 0x04 : 0x14;
            data[1] = 0x00;
            data[2] = terrRfActive ? 0x02 : 0x00;
            data[3] = lnaOff       ? 0x04 : 0x06;
            break;
        case sony_cxd_HELENE_CONFIG_POWERSAVE_SAT_RF_ACTIVE:
            if(lnaOff){
                data[0] = terrRfActive ? 0x06 : 0x16;
            }else{
                data[0] = terrRfActive ? 0x05 : 0x15;
            }
            data[1] = 0x00;
            data[2] = terrRfActive ? 0x02 : 0x00;
            data[3] = lnaOff       ? 0x04 : 0x06;
            break;
        default:
            sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
        }

        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x43, data[3]);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x0C, &data[0], 3);
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* Return to terrestrial mode (0x01) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x01, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for CPU (0x05) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x05, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for internal logic block (0x04) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x04, 0xC0);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t X_oscdis(sony_cxd_helene_t *pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("X_oscdis");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Setting for REFOUT signal output (0x84) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Driver current setting for crystal oscillator (0x82) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x82, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t X_oscen(sony_cxd_helene_t *pTuner)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;
    uint8_t data = 0;

    sony_cxd_TRACE_ENTER("X_oscen");

    if((!pTuner) || (!pTuner->pI2c)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Driver current setting for crystal oscillator (0x82) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x82, 0x9F);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Setting for REFOUT signal output (0x84) */
    switch(pTuner->flags & sony_cxd_HELENE_CONFIG_REFOUT_MASK){
    case 0:
        data = 0x00; /* REFOUT_EN = 0, REFOUT_CNT = 0 */
        break;
    case sony_cxd_HELENE_CONFIG_REFOUT_500mVpp:
        data = 0x80; /* REFOUT_EN = 1, REFOUT_CNT = 0 */
        break;
    case sony_cxd_HELENE_CONFIG_REFOUT_400mVpp:
        data = 0x81; /* REFOUT_EN = 1, REFOUT_CNT = 1 */
        break;
    case sony_cxd_HELENE_CONFIG_REFOUT_600mVpp:
        data = 0x82; /* REFOUT_EN = 1, REFOUT_CNT = 2 */
        break;
    case sony_cxd_HELENE_CONFIG_REFOUT_800mVpp:
        data = 0x83; /* REFOUT_EN = 1, REFOUT_CNT = 3 */
        break;
    default:
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_NOSUPPORT);
    }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, data);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_SLEEP(10);

    /* Driver current setting for crystal oscillator (0x82) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x82,
        (uint8_t)(0x80 | (pTuner->xosc_sel & 0x1F)));
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}

static sony_cxd_result_t X_read_agc(sony_cxd_helene_t *pTuner, uint8_t *pIFAGCReg, uint8_t *pRFAGCReg)
{
    sony_cxd_result_t result = sony_cxd_RESULT_OK;

    sony_cxd_TRACE_ENTER("X_read_agc");

    if((!pTuner) || (!pTuner->pI2c) || (!pIFAGCReg) || (!pRFAGCReg)){
        sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_ARG);
    }

    /* Clock enable for internal logic block, CPU wake-up (0x87, 0x88) */
    {
        const uint8_t cdata[2] = {0xC4, 0x41};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* Connect IFAGC, Start ADC (0x59, 0x5A) */
    {
        const uint8_t cdata[2] = {0x05, 0x01};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* ADC read out (0x5B) */
    result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5B, pIFAGCReg, 1);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Connect RFAGC, Start ADC (0x59, 0x5A) */
    {
        const uint8_t cdata[2] = {0x03, 0x01};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, cdata, sizeof(cdata));
        if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }
    }

    /* ADC read out (0x5B) */
    result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5B, pRFAGCReg, 1);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* ADC disable (0x59) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, 0x04);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for CPU (0x88) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    /* Standby setting for internal logic block (0x87) */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != sony_cxd_RESULT_OK){ sony_cxd_TRACE_RETURN(sony_cxd_RESULT_ERROR_IO); }

    sony_cxd_TRACE_RETURN(sony_cxd_RESULT_OK);
}
