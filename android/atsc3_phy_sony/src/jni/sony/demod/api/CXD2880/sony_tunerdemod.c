/*------------------------------------------------------------------------------
  Copyright 2014-2016 Sony Semiconductor Solutions Corporation

  Last Updated    : 2016/05/31
  Modification ID : 81be8b74dbc95676aa75385cdbaabb182ddeecc8
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 Includes
------------------------------------------------------------------------------*/
#include "sony_common.h"
#include "sony_stdlib.h"
#include "sony_tunerdemod.h"
#include "sony_tunerdemod_monitor.h"
#include "sony_tunerdemod_dvbt.h"
#include "sony_tunerdemod_dvbt2.h"
#include "sony_tunerdemod_isdbt.h"
#include "sony_tunerdemod_isdbtsb.h"
#include "sony_tunerdemod_isdbtmm.h"

/*------------------------------------------------------------------------------
 Static Functions
------------------------------------------------------------------------------*/
static sony_result_t P_init1 (sony_tunerdemod_t * pTunerDemod)
{
    uint8_t data = 0;

    SONY_TRACE_ENTER ("P_init1");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if ((pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) || (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN)) {
        /* TSIF Setting (Single or Diver(main)) */
        switch (pTunerDemod->createParam.tsOutputIF) {
        case SONY_TUNERDEMOD_TSOUT_IF_TS:
            data = 0x00;
            break;
        case SONY_TUNERDEMOD_TSOUT_IF_SPI:
            data = 0x01;
            break;
        case SONY_TUNERDEMOD_TSOUT_IF_SDIO:
            data = 0x02;
            break;
        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
            break;
        }
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x10, data) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* LDO Setting */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x11, 0x16) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    switch (pTunerDemod->chipID) {
    case SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_0X:
        data = 0x1A;
        break;
    case SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_11:
        data = 0x16;
        break;
    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x10, data) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->createParam.enableInternalLDO) {
        data = 0x01;
    } else {
        data = 0x00;
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x11, data) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x13, data) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x12, data) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    switch (pTunerDemod->chipID) {
    case SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_0X:
        data = 0x01;
        break;
    case SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_11:
        data = 0x00;
        break;
    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x69, data) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t P_init2 (sony_tunerdemod_t * pTunerDemod)
{
    uint8_t data[6] = {0};

    SONY_TRACE_ENTER ("P_init2");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = pTunerDemod->createParam.xosc_cap;
    data[1] = pTunerDemod->createParam.xosc_i;
    switch (pTunerDemod->createParam.xtalShareType) {
    case SONY_TUNERDEMOD_XTAL_SHARE_NONE:
        data[2] = 0x01;
        data[3] = 0x00;
        break;
    case SONY_TUNERDEMOD_XTAL_SHARE_EXTREF:
        data[2] = 0x00;
        data[3] = 0x00;
        break;
    case SONY_TUNERDEMOD_XTAL_SHARE_MASTER:
        data[2] = 0x01;
        data[3] = 0x01;
        break;
    case SONY_TUNERDEMOD_XTAL_SHARE_SLAVE:
        data[2] = 0x00;
        data[3] = 0x01;
        break;
    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    data[4] = 0x06;
    data[5] = 0x00;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x13, data, 6) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t P_init3 (sony_tunerdemod_t * pTunerDemod)
{
    uint8_t data[2] = {0};

    SONY_TRACE_ENTER ("P_init3");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    switch (pTunerDemod->diverMode) {
    case SONY_TUNERDEMOD_DIVERMODE_SINGLE:
        data[0] = 0x00;
        break;
    case SONY_TUNERDEMOD_DIVERMODE_MAIN:
        data[0] = 0x03;
        break;
    case SONY_TUNERDEMOD_DIVERMODE_SUB:
        data[0] = 0x02;
        break;
    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    data[1] = 0x01;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x1F, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t RF_init1 (sony_tunerdemod_t * pTunerDemod)
{
    uint8_t data[80] = {0};

    SONY_TRACE_ENTER ("RF_init1");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x01; /* 0x21 */
    data[1] = 0x00;
    data[2] = 0x01;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x21, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x01; /* 0x17 */
    data[1] = 0x01;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x17, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* RF parameter setting */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x4F, 0x18) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x61, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x71, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x9D, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x7D, 0x02) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x8F, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x8B, 0xC6) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x9A, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x1C, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x00; /* 0xB5 */
    if ((pTunerDemod->createParam.isCXD2881GG) && (pTunerDemod->createParam.xtalShareType == SONY_TUNERDEMOD_XTAL_SHARE_SLAVE)) {
        data[1] = 0x00;
    } else {
        data[1] = 0x1F;
    }
    data[2] = 0x0A;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xB5, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xB9, 0x07) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x33, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xC1, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xC4, 0x1E) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->chipID == SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_0X) {
        data[0] = 0x34; /* 0xD9 */
        data[1] = 0x2C;
    } else {
        data[0] = 0x2F; /* 0xD9 */
        data[1] = 0x25;
    }
    data[2] = 0x15;
    data[3] = 0x19;
    data[4] = 0x1B;
    data[5] = 0x15;
    data[6] = 0x19;
    data[7] = 0x1B; /* 0xE0 */
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xD9, data, 8) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x11 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x6C; /* 0x44 */
    data[1] = 0x10;
    data[2] = 0xA6;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x44, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x16; /* 0x50 */
    data[1] = 0xA8;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x50, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x00; /* 0x62 */
    data[1] = 0x22;
    data[2] = 0x00;
    data[3] = 0x88;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x62, data, 4) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x74, 0x75) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x05; /* 0x7F */
    data[1] = 0x05; /* 0x80 */
    data[2] = 0x05;
    data[3] = 0x05;
    data[4] = 0x05;
    data[5] = 0x05;
    data[6] = 0x05;
    data[7] = 0x05;
    data[8] = 0x05;
    data[9] = 0x04; /* 0x88 */
    data[10] = 0x04;
    data[11] = 0x04;
    data[12] = 0x03;
    data[13] = 0x03;
    data[14] = 0x03;
    data[15] = 0x04;
    data[16] = 0x04;
    data[17] = 0x05; /* 0x90 */
    data[18] = 0x05;
    data[19] = 0x05;
    data[20] = 0x02;
    data[21] = 0x02;
    data[22] = 0x02;
    data[23] = 0x02;
    data[24] = 0x02;
    data[25] = 0x02; /* 0x98 */
    data[26] = 0x02;
    data[27] = 0x02;
    data[28] = 0x02;
    data[29] = 0x03;
    data[30] = 0x02;
    data[31] = 0x01;
    data[32] = 0x01;
    data[33] = 0x01; /* 0xA0 */
    data[34] = 0x02;
    data[35] = 0x02;
    data[36] = 0x03;
    data[37] = 0x04;
    data[38] = 0x04;
    data[39] = 0x04;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x7F, data, 40) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x16 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x16) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x00; /* 0x10 */
    data[1] = 0x71;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x10, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x23, 0x89) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0xFF; /* 0x27 */
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    data[4] = 0x00;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x27, data, 5) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x00; /* 0x3A */
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    data[4] = 0x00;
    data[5] = 0x01;
    data[6] = 0x00; /* 0x40 */
    data[7] = 0x01;
    data[8] = 0x00;
    data[9] = 0x02;
    data[10] = 0x00;
    data[11] = 0x63;
    data[12] = 0x00;
    data[13] = 0x00;
    data[14] = 0x00; /* 0x48 */
    data[15] = 0x03;
    data[16] = 0x00;
    data[17] = 0x04;
    data[18] = 0x00;
    data[19] = 0x04;
    data[20] = 0x00;
    data[21] = 0x06;
    data[22] = 0x00; /* 0x50 */
    data[23] = 0x06;
    data[24] = 0x00;
    data[25] = 0x08;
    data[26] = 0x00;
    data[27] = 0x09;
    data[28] = 0x00;
    data[29] = 0x0B;
    data[30] = 0x00; /* 0x58 */
    data[31] = 0x0B;
    data[32] = 0x00;
    data[33] = 0x0D;
    data[34] = 0x00;
    data[35] = 0x0D;
    data[36] = 0x00;
    data[37] = 0x0F;
    data[38] = 0x00; /* 0x60 */
    data[39] = 0x0F;
    data[40] = 0x00;
    data[41] = 0x0F;
    data[42] = 0x00;
    data[43] = 0x10;
    data[44] = 0x00;
    data[45] = 0x79;
    data[46] = 0x00; /* 0x68 */
    data[47] = 0x00;
    data[48] = 0x00;
    data[49] = 0x02;
    data[50] = 0x00;
    data[51] = 0x00;
    data[52] = 0x00;
    data[53] = 0x03;
    data[54] = 0x00; /* 0x70 */
    data[55] = 0x01;
    data[56] = 0x00;
    data[57] = 0x03;
    data[58] = 0x00;
    data[59] = 0x03;
    data[60] = 0x00;
    data[61] = 0x03;
    data[62] = 0x00; /* 0x78 */
    data[63] = 0x04;
    data[64] = 0x00;
    data[65] = 0x04;
    data[66] = 0x00;
    data[67] = 0x06;
    data[68] = 0x00;
    data[69] = 0x05;
    data[70] = 0x00; /* 0x80 */
    data[71] = 0x07;
    data[72] = 0x00;
    data[73] = 0x07;
    data[74] = 0x00;
    data[75] = 0x08;
    data[76] = 0x00;
    data[77] = 0x0A;
    data[78] = 0x03; /* 0x88 */
    data[79] = 0xE0;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x3A, data, 80) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    data[0] = 0x03; /* 0xBC */
    data[1] = 0xE0;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xBC, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x51, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xC5, 0x07) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x11 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x70, 0xE9) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x76, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x78, 0x32) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x7A, 0x46) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x7C, 0x86) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x7E, 0xA4) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xE1, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    data[0] = 0x00;
    data[1] = 0x08;
    data[2] = 0x19;
    data[3] = 0x0E;
    data[4] = 0x09;
    data[5] = 0x0E;
    {
        uint8_t addr = 0;

        /* SLV-X bank 0x12 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x12) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        for (addr = 0x10; addr < 0x9F; addr += 6) {
            if (pTunerDemod->pLNAThresholdTableAir) {
                data[0] = pTunerDemod->pLNAThresholdTableAir->threshold[(addr - 0x10) / 6].off_on;
                data[1] = pTunerDemod->pLNAThresholdTableAir->threshold[(addr - 0x10) / 6].on_off;
            }
            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, addr, data, 6) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }

        data[0] = 0x00;
        data[1] = 0x08;

        /* SLV-X bank 0x13 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x13) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        for (addr = 0x10; addr < 0xCF; addr += 6) {
            if (pTunerDemod->pLNAThresholdTableCable) {
                data[0] = pTunerDemod->pLNAThresholdTableCable->threshold[(addr - 0x10) / 6].off_on;
                data[1] = pTunerDemod->pLNAThresholdTableCable->threshold[(addr - 0x10) / 6].on_off;
            }
            if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, addr, data, 6) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
    }

    /* SLV-X bank 0x11 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x08; /* 0xBD */
    data[1] = 0x09;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xBD, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x08; /* 0xC4 */
    data[1] = 0x09;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xC4, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x20; /* 0xC9 */
    data[1] = 0x20;
    data[2] = 0x30;
    data[3] = 0x41;
    data[4] = 0x50;
    data[5] = 0x5F;
    data[6] = 0x6F;
    data[7] = 0x80; /* 0xD0 */
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xC9, data, 8) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x14 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x14) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x15; /* 0x10 */
    data[1] = 0x18;
    data[2] = 0x00;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x10, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x15, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x16 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x16) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x00; /* 0x12 */
    data[1] = 0x09;
    data[2] = 0x00;
    data[3] = 0x08;
    data[4] = 0x00;
    data[5] = 0x07;
    data[6] = 0x00; /* 0x18 */
    data[7] = 0x06;
    data[8] = 0x00;
    data[9] = 0x05;
    data[10] = 0x00;
    data[11] = 0x03;
    data[12] = 0x00;
    data[13] = 0x02;
    data[14] = 0x00; /* 0x20 */
    data[15] = 0x00;
    data[16] = 0x00;
    data[17] = 0x78;
    data[18] = 0x00;
    data[19] = 0x00;
    data[20] = 0x00;
    data[21] = 0x06;
    data[22] = 0x00; /* 0x28 */
    data[23] = 0x08;
    data[24] = 0x00;
    data[25] = 0x08;
    data[26] = 0x00;
    data[27] = 0x0C;
    data[28] = 0x00;
    data[29] = 0x0C;
    data[30] = 0x00; /* 0x30 */
    data[31] = 0x0D;
    data[32] = 0x00;
    data[33] = 0x0F;
    data[34] = 0x00;
    data[35] = 0x0E;
    data[36] = 0x00;
    data[37] = 0x0E;
    data[38] = 0x00; /* 0x38 */
    data[39] = 0x10;
    data[40] = 0x00;
    data[41] = 0x0F;
    data[42] = 0x00;
    data[43] = 0x0E;
    data[44] = 0x00;
    data[45] = 0x10;
    data[46] = 0x00; /* 0x40 */
    data[47] = 0x0F;
    data[48] = 0x00;
    data[49] = 0x0E;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x12, data, 50) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    /* SLV-X bank 0x0A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x10, data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if ((data[0] & 0x01) == 0x00) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x25, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    /* SLV-X bank 0x0A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x11, data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if ((data[0] & 0x01) == 0x00) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* SLV-T register all clear */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x02, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* XZIF setting */
    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0xE1 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0xE1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x8F, 0x16) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x67, 0x60) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6A, 0x0F) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6C, 0x17) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x00; /* 0x6E */
    data[1] = 0xFE;
    data[2] = 0xEE;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x6E, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0xA1; /* 0x8D */
    data[1] = 0x8B;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x8D, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x08; /* 0x77 */
    data[1] = 0x09;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x77, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0xE2 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0xE2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x41, 0xA0) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x4B, 0x68) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x25, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    /* SLV-X bank 0x1A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x1A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x10, data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if ((data[0] & 0x01) == 0x00) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x14, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x26, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t RF_init2 (sony_tunerdemod_t * pTunerDemod)
{
    uint8_t data[5] = {0};

    SONY_TRACE_ENTER ("RF_init2");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x40; /* 0xEA */
    data[1] = 0x40;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xEA, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    data[0] = 0x00; /* 0x30 */
    if (pTunerDemod->chipID == SONY_TUNERDEMOD_CHIP_ID_CXD2880_ES1_0X) {
        data[1] = 0x00;
    } else {
        data[1] = 0x01;
    }
    data[2] = 0x01;
    data[3] = 0x03;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x30, data, 4) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x14 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x14) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x1B, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0xE1 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0xE1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD3, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_tune1(sony_tunerdemod_t * pTunerDemod, sony_dtv_system_t system, unsigned int freqKHz, sony_dtv_bandwidth_t bandwidth,
                              uint8_t isCable, int shiftFrequencyKHz)
{
    uint8_t data[11] = {0};

    SONY_TRACE_ENTER ("X_tune1");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    data[0] = 0x00; /* 0xE7 */
    data[1] = 0x00;
    data[2] = 0x0E;
    data[3] = 0x00;
    data[4] = 0x03;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xE7, data, 5) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    data[0] = 0x1F; /* 0xE7 */
    data[1] = 0x80;
    data[2] = 0x18;
    data[3] = 0x00;
    data[4] = 0x07;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xE7, data, 5) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    data[0] = 0x72; /* 0x44 */
    data[1] = 0x81;
    /* data[2] (0x46) : Depend on broadcasting system */
    data[3] = 0x1D;
    data[4] = 0x6F;
    data[5] = 0x7E;
    /* data[6] (0x4A) : Depend on broadcasting system */
    data[7] = 0x1C;
    switch (system) {
    case SONY_DTV_SYSTEM_DVBT:
    case SONY_DTV_SYSTEM_ISDBT:
    case SONY_DTV_SYSTEM_ISDBTSB:
    case SONY_DTV_SYSTEM_ISDBTMM_A:
    case SONY_DTV_SYSTEM_ISDBTMM_B:
        data[2] = 0x94;
        data[6] = 0x91;
        break;
    case SONY_DTV_SYSTEM_DVBT2:
        data[2] = 0x96;
        data[6] = 0x93;
        break;
    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x44, data, 8) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x62, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x15 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x15) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x03;
    data[1] = 0xE2;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x1E, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    data[0] = (uint8_t)(isCable ? 0x01 : 0x00); /* 0x52 */
    data[1] = 0x00;
    data[2] = 0x6B;
    data[3] = 0x4D;

    /* 0x56 */
    switch (bandwidth) {
    case SONY_DTV_BW_1_7_MHZ:
        data[4] = 0x03;
        break;
    case SONY_DTV_BW_5_MHZ:
    case SONY_DTV_BW_6_MHZ:
        data[4] = 0x00;
        break;
    case SONY_DTV_BW_7_MHZ:
        data[4] = 0x01;
        break;
    case SONY_DTV_BW_8_MHZ:
        data[4] = 0x02;
        break;
    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    data[5] = 0x00; /* 0x57 */

    /* Frequency shift */
    freqKHz += shiftFrequencyKHz;

    data[6] = (uint8_t)((freqKHz >> 16) & 0x0F);
    data[7] = (uint8_t)((freqKHz >>  8) & 0xFF);
    data[8] = (uint8_t)( freqKHz        & 0xFF);
    data[9] = 0xFF;
    data[10] = 0xFE;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x52, data, 11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_tune2 (sony_tunerdemod_t * pTunerDemod, sony_dtv_bandwidth_t bandwidth,
                              sony_tunerdemod_clockmode_t clockMode, int shiftFrequencyKHz)
{
    uint8_t data[3] = {0};

    SONY_TRACE_ENTER ("X_tune2");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x11 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    data[0] = 0x01;
    data[1] = 0x0E;
    data[2] = 0x01;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x2D, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x1A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x1A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x29, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    /* Read monitor data */
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x2C, data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    /* Write monitor data */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x60, data[0]) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x62, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x11 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x11) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x2D, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x2F, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Frequency shift */
    if (shiftFrequencyKHz != 0) {
        int shiftFreq = 0;

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x01) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* SLV-T bank 0xE1 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0xE1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /*
         * OFRQSHFT setting
         *
         * Fshift = OFRQSHFT * fADC / 6291456 [Hz]
         *
         * (Clock mode A, C : fADC = 1152000000)
         * Fshift = OFRQSHFT * 183.105
         * (Clock mode B : fADC = 1120000000)
         * Fshift = OFRQSHFT * 178.019
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        shiftFreq = shiftFrequencyKHz * 1000; /* Hz */

        /* frequency -> OFRQSHFT */
        switch (clockMode) {
        case SONY_TUNERDEMOD_CLOCKMODE_A:
        case SONY_TUNERDEMOD_CLOCKMODE_C:
        default:
            if (shiftFreq >= 0) {
                shiftFreq = (shiftFreq + 183/2) / 183;
            } else {
                shiftFreq = (shiftFreq - 183/2) / 183;
            }
            break;
        case SONY_TUNERDEMOD_CLOCKMODE_B:
            if (shiftFreq >= 0) {
                shiftFreq = (shiftFreq + 178/2) / 178;
            } else {
                shiftFreq = (shiftFreq - 178/2) / 178;
            }
            break;
        }

        /* Add current value */
        shiftFreq += sony_Convert2SComplement ((data[0] << 8) | data[1], 16);

        if (shiftFreq > 32767) {
            shiftFreq = 32767;
        } else if (shiftFreq < -32768) {
            shiftFreq = -32768;
        }

		data[0] = (uint8_t)(((unsigned int)shiftFreq >> 8) & 0xFF);
		data[1] = (uint8_t)((unsigned int)shiftFreq & 0xFF);

        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x60, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /*
         * OFOFFSET setting
         * Note: Fshift value should be opposite to Fshift of OFRQSHFT.
         *
         * Fshift = OFOFFSET * fADC / 32768 [kHz] (BW != 1.7MHz)
         * (Clock mode A, C : fADC = 1152000)
         * Fshift = OFOFFSET * 35.156
         * (Clock mode B : fADC = 1120000)
         * Fshift = OFOFFSET * 34.180
         *
         * Fshift = OFOFFSET * fADC / 65536 [kHz] (BW = 1.7MHz)
         * (Clock mode A, C : fADC = 1152000)
         * Fshift = OFOFFSET * 17.578
         * (Clock mode B : fADC = 1120000)
         * Fshift = OFOFFSET * 17.090
         */
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x69, data, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        shiftFreq = -shiftFrequencyKHz; /* Shift opposite */

        /* frequency -> OFOFFSET */
        if (bandwidth == SONY_DTV_BW_1_7_MHZ) {
            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_C:
            default:
                if (shiftFreq >= 0) {
                    shiftFreq = (shiftFreq * 1000 + 17578/2) / 17578;
                } else {
                    shiftFreq = (shiftFreq * 1000 - 17578/2) / 17578;
                }
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                if (shiftFreq >= 0) {
                    shiftFreq = (shiftFreq * 1000 + 17090/2) / 17090;
                } else {
                    shiftFreq = (shiftFreq * 1000 - 17090/2) / 17090;
                }
                break;
            }
        } else {
            switch (clockMode) {
            case SONY_TUNERDEMOD_CLOCKMODE_A:
            case SONY_TUNERDEMOD_CLOCKMODE_C:
            default:
                if (shiftFreq >= 0) {
                    shiftFreq = (shiftFreq * 1000 + 35156/2) / 35156;
                } else {
                    shiftFreq = (shiftFreq * 1000 - 35156/2) / 35156;
                }
                break;
            case SONY_TUNERDEMOD_CLOCKMODE_B:
                if (shiftFreq >= 0) {
                    shiftFreq = (shiftFreq * 1000 + 34180/2) / 34180;
                } else {
                    shiftFreq = (shiftFreq * 1000 - 34180/2) / 34180;
                }
                break;
            }
        }

        /* Add current value */
        shiftFreq += sony_Convert2SComplement (data[0], 8);

        if (shiftFreq > 127) {
            shiftFreq = 127;
        } else if (shiftFreq < -128) {
            shiftFreq = -128;
        }

		data[0] = (uint8_t)((unsigned int)shiftFreq & 0xFF);

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x69, data[0]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* SLV-T bank 0x00 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_tune3 (sony_tunerdemod_t * pTunerDemod, sony_dtv_system_t system, uint8_t enableFEFIntermittentControl)
{
    uint8_t data[6] = {0};

    SONY_TRACE_ENTER ("X_tune3");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0xE2 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0xE2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x41, 0xA0) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    /* Soft reset (OREG_SRST) */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xFE, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if ((system == SONY_DTV_SYSTEM_DVBT2) && enableFEFIntermittentControl) {
        data[0] = 0x01;
        data[1] = 0x01;
        data[2] = 0x01;
        data[3] = 0x01;
        data[4] = 0x01;
        data[5] = 0x01;
    }else{
        data[0] = 0x00;
        data[1] = 0x00;
        data[2] = 0x00;
        data[3] = 0x00;
        data[4] = 0x00;
        data[5] = 0x00;
    }
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xEF, data, 6) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x2D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x2D) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if ((system == SONY_DTV_SYSTEM_DVBT2) && enableFEFIntermittentControl) {
            data[0] = 0x00;
    }else{
            data[0] = 0x01;
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB1, data[0]) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_tune4 (sony_tunerdemod_t * pTunerDemod)
{
    /* Diver pin setting */
    uint8_t data[2] = {0};

    SONY_TRACE_ENTER ("X_tune4");

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    {
        /* Setting to sub */
        /* SLV-X bank 0x00 */
        if (pTunerDemod->pDiverSub->pRegio->WriteOneRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        data[0] = 0x14;
        data[1] = 0x00;
        if (pTunerDemod->pDiverSub->pRegio->WriteRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x55, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    {
        /* Setting to main */
        /* SLV-X bank 0x00 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        data[0] = 0x0B;
        data[1] = 0xFF;
        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x53, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x57, 0x01) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        data[0] = 0x0B;
        data[1] = 0xFF;
        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x55, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    {
        /* Setting to sub */
        /* SLV-X bank 0x00 */
        if (pTunerDemod->pDiverSub->pRegio->WriteOneRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        data[0] = 0x14;
        data[1] = 0x00;
        if (pTunerDemod->pDiverSub->pRegio->WriteRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x53, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if (pTunerDemod->pDiverSub->pRegio->WriteOneRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x57, 0x02) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    /* Soft reset (OREG_SRST) */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xFE, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pDiverSub->pRegio->WriteOneRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    /* Soft reset (OREG_SRST) */
    if (pTunerDemod->pDiverSub->pRegio->WriteOneRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_DEMOD, 0xFE, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_sleep1 (sony_tunerdemod_t * pTunerDemod)
{
    /* Diver pin setting */
    uint8_t data[3] = {0};

    SONY_TRACE_ENTER ("X_sleep1");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    {
        /* Setting to main */
        /* SLV-X bank 0x00 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x57, 0x03) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        data[0] = 0x00;
        data[1] = 0x00;
        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x53, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    {
        /* Setting to sub */
        /* SLV-X bank 0x00 */
        if (pTunerDemod->pDiverSub->pRegio->WriteOneRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        data[0] = 0x1F;
        data[1] = 0xFF;
        data[2] = 0x03;
        if (pTunerDemod->pDiverSub->pRegio->WriteRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x55, data, 3) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        data[0] = 0x00;
        data[1] = 0x00;
        if (pTunerDemod->pDiverSub->pRegio->WriteRegister (pTunerDemod->pDiverSub->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x53, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    {
        /* Setting to main */
        /* SLV-X bank 0x00 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        data[0] = 0x1F;
        data[1] = 0xFF;
        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x55, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_sleep2 (sony_tunerdemod_t * pTunerDemod)
{
    /* FEF intermittent control disable */
    uint8_t data = 0;
    SONY_TRACE_ENTER ("X_sleep2");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-T bank 0x2D */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x2D) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB1, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xB2, &data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if ((data & 0x01) == 0x00) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xF4, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xF3, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xF2, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xF1, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xF0, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xEF, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_sleep3 (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("X_sleep3");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xFD, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t X_sleep4 (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("X_sleep4");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0xE2 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0xE2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x41, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x21, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t SPLL_reset (sony_tunerdemod_t * pTunerDemod, sony_tunerdemod_clockmode_t clockmode)
{
    uint8_t data[4] = {0};

    SONY_TRACE_ENTER ("SPLL_reset");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x29, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x28, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x27, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x26, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x27, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x22, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    switch (clockmode) {
    case SONY_TUNERDEMOD_CLOCKMODE_A:
        data[0] = 0x00;
        break;

    case SONY_TUNERDEMOD_CLOCKMODE_B:
        data[0] = 0x01;
        break;

    case SONY_TUNERDEMOD_CLOCKMODE_C:
        data[0] = 0x02;
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x30, data[0]) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x22, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (2);

    /* SLV-X bank 0x0A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x10, data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if ((data[0] & 0x01) == 0x00) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x27, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x26, data, 4) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t T_powerX (sony_tunerdemod_t * pTunerDemod, uint8_t on)
{
    uint8_t data[3] = {0};

    SONY_TRACE_ENTER ("T_powerX");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x29, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x28, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x27, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x27, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x25, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (on) {
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x2B, 0x01) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SONY_SLEEP (1);

        /* SLV-X bank 0x0A */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x0A) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x12, data, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if ((data[0] & 0x01) == 0) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }
        /* SLV-X bank 0x00 */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x2A, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    } else {
        data[0] = 0x03;
        data[1] = 0x00;
        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x2A, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        SONY_SLEEP (1);

        /* SLV-X bank 0x0A */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x0A) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x13, data, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
        if ((data[0] & 0x01) == 0) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x25, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    /* SLV-X bank 0x0A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x11, data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if ((data[0] & 0x01) == 0) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x27, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_SLEEP (1);

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x10, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* SLV-X bank 0x10 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x10) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    data[0] = 0x00;
    data[1] = 0x00;
    data[2] = 0x00;
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x27, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

/**
 @brief Register definition structure for TS clock configurations.
 */
typedef struct {
    uint8_t serialClkMode;      /**< Serial clock mode (gated or continuous) */
    uint8_t serialDutyMode;     /**< Serial clock duty mode (full rate or half rate) */
    uint8_t tsClkPeriod;        /**< TS clock period */
} sony_tunerdemod_ts_clk_configuration_t;

static sony_result_t setTSClockModeAndFreq (sony_tunerdemod_t * pTunerDemod,
                                            sony_dtv_system_t system)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t backwardsCompatible = 0;
    sony_tunerdemod_ts_clk_configuration_t tsClkConfiguration;

    const sony_tunerdemod_ts_clk_configuration_t serialTsClkSettings [2][6] =
    {{ /* Gated Clock */
       /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD                 */
        {      3,          1,            8,     }, /* Full rate */
        {      0,          2,            16,    }  /* Half rate */
    },
    {  /* Continuous Clock */
       /* OSERCKMODE  OSERDUTYMODE  OTSCKPERIOD                 */
        {      1,          1,            8,     }, /* Full rate */
        {      2,          2,            16,    }  /* Half rate */
    }};

    SONY_TRACE_ENTER ("setTSClockModeAndFreq");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Slave    Bank    Addr    Bit      Default    Name
     * -----------------------------------------------------------
     * <SLV-T>  00h     D3h     [0]      1'b0       OTSRATECTRLOFF
     * <SLV-T>  00h     DEh     [0]      1'b0       OTSIN_OFF
     *
     * (0, 0): Packet gap insertion On  (DVB-T/T2)
     * (1, 0): Packet gap insertion Off (ISDB-T)
     * (1, 1): Packet gap insertion Off (for backwards compatibility)
     *
     *
     * Slave    Bank    Addr    Bit      Default    Name
     * -----------------------------------------------------------
     * <SLV-T>  00h     DAh     [0]      1'b0       OTSRC_TSCKMANUALON
     *
     * 0 : TS clock manual setting off.
     * 1 : TS clock manual setting on.
     *     (Note: OTSRATECTRLOFF should be 0.)
     */
    {
        uint8_t tsRateCtrlOff = 0;
        uint8_t tsInOff = 0;
        uint8_t tsClkManaulOn = 0;

        if ((system == SONY_DTV_SYSTEM_ISDBT) || (system == SONY_DTV_SYSTEM_ISDBTSB)
            || (system == SONY_DTV_SYSTEM_ISDBTMM_A) || (system == SONY_DTV_SYSTEM_ISDBTMM_B)) {
            backwardsCompatible = 0;
            tsRateCtrlOff = 1;
            tsInOff = 0;
        } else if (pTunerDemod->isTsBackwardsCompatibleMode) {
            /* DVB only */
            backwardsCompatible = 1;
            tsRateCtrlOff = 1;
            tsInOff = 1;
        } else {
            backwardsCompatible = 0;
            tsRateCtrlOff = 0;
            tsInOff = 0;
        }

        if (pTunerDemod->tsByteClkManualSetting) {
            tsClkManaulOn = 1;
            tsRateCtrlOff = 0;
        }

        result = sony_regio_SetRegisterBits(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD3, tsRateCtrlOff, 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = sony_regio_SetRegisterBits(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xDE, tsInOff, 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        result = sony_regio_SetRegisterBits(pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xDA, tsClkManaulOn, 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    tsClkConfiguration = serialTsClkSettings[pTunerDemod->serialTsClockModeContinuous][(uint8_t)pTunerDemod->serialTsClkFreq];

    /* Overwrite TSCKPERIOD if TS byte clock manual setting is enabled */
    if (pTunerDemod->tsByteClkManualSetting) {
        tsClkConfiguration.tsClkPeriod = pTunerDemod->tsByteClkManualSetting;
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -----------------------------------------------------
     * <SLV-T>  00h     C4h     [1:0]  2'b01      OSERCKMODE
     */
    result = sony_regio_SetRegisterBits (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xC4, tsClkConfiguration.serialClkMode, 0x03);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * -------------------------------------------------------
     * <SLV-T>  00h     D1h     [1:0]  2'b01      OSERDUTYMODE
     */
    result = sony_regio_SetRegisterBits (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD1, tsClkConfiguration.serialDutyMode, 0x03);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ------------------------------------------------------
     * <SLV-T>  00h     D9h     [7:0]  8'h08      OTSCKPERIOD
     */
    result = pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xD9, tsClkConfiguration.tsClkPeriod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    {
        /* Set parity period enable / disable based on backwards compatible TS configuration */
        uint8_t data = (uint8_t)(backwardsCompatible ? 0x00 : 0x01);

        if (system == SONY_DTV_SYSTEM_DVBT) {
            /* Enable parity period for DVB-T */
            /* Set SLV-T Bank : 0x10 */
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x10) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }

            /* slave    Bank    Addr    Bit    default    Name
             * ---------------------------------------------------------------
             * <SLV-T>  10h     66h     [0]    1'b1       OREG_TSIF_PCK_LENGTH
             */
            result = sony_regio_SetRegisterBits (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x66, data, 0x01);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t pidFilterSetting (sony_tunerdemod_t * pTunerDemod, sony_tunerdemod_pid_filter_config_t * pPIDFilterConfig)
{
    /* IF pPIDFilterConfig is NULL, PID filter will be disabled. */

    SONY_TRACE_ENTER ("pidFilterSetting");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (!pPIDFilterConfig) {
        /* Disable PID filter */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x50, 0x02) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    } else {
        uint8_t data[65];

        /* Enable PID filter */
        data[0] = (uint8_t)(pPIDFilterConfig->isNegative ? 0x01 : 0x00);
        {
            int i = 0;
            for (i = 0; i < 32; i++) {
                if (pPIDFilterConfig->pidConfig[i].isEnable) {
                    data[1 + (i * 2)] = (uint8_t)((uint8_t)(pPIDFilterConfig->pidConfig[i].pid >> 8) | 0x20);
                    data[2 + (i * 2)] = (uint8_t)(pPIDFilterConfig->pidConfig[i].pid & 0xFF);
                } else {
                    data[1 + (i * 2)] = 0x00;
                    data[2 + (i * 2)] = 0x00;
                }
            }
        }
        if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x50, data, 65) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t clearConfigMemory (sony_tunerdemod_t * pTunerDemod)
{
    SONY_TRACE_ENTER ("clearConfigMemory");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pTunerDemod->configMemoryLastEntry = 0;
    sony_memset (pTunerDemod->configMemory, 0x00, sizeof(pTunerDemod->configMemory));

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

static sony_result_t loadConfigMemory (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t i;

    SONY_TRACE_ENTER ("loadConfigMemory");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    for (i = 0; i < pTunerDemod->configMemoryLastEntry; i++) {
        /* Set the bank */
        result = pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio,
                                                        pTunerDemod->configMemory[i].target,
                                                        0x00,
                                                        pTunerDemod->configMemory[i].bank);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Write the register value */
        result = sony_regio_SetRegisterBits (pTunerDemod->pRegio,
                                             pTunerDemod->configMemory[i].target,
                                             pTunerDemod->configMemory[i].address,
                                             pTunerDemod->configMemory[i].value,
                                             pTunerDemod->configMemory[i].bitMask);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (result);
}

static sony_result_t setConfigMemory (sony_tunerdemod_t * pTunerDemod,
                                      sony_regio_target_t target,
                                      uint8_t bank,
                                      uint8_t address,
                                      uint8_t value,
                                      uint8_t bitMask)
{
    uint8_t i;
    uint8_t valueStored = 0;

    SONY_TRACE_ENTER ("setConfigMemory");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Search for matching address entry already in table */
    for (i = 0; i < pTunerDemod->configMemoryLastEntry; i++){
        if ((valueStored == 0) &&
            (pTunerDemod->configMemory[i].target == target) &&
            (pTunerDemod->configMemory[i].bank == bank) &&
            (pTunerDemod->configMemory[i].address == address)) {
                /* Clear bits to overwrite / set  and then store the new value */
                pTunerDemod->configMemory[i].value &= ~bitMask;
                pTunerDemod->configMemory[i].value |= (value & bitMask);

                /* Add new bits to the bit mask */
                pTunerDemod->configMemory[i].bitMask |= bitMask;

                valueStored = 1;
        }
    }

    /* If current register does not exist in the table, add a new entry to the end */
    if (valueStored == 0) {
        if (pTunerDemod->configMemoryLastEntry < SONY_TUNERDEMOD_MAX_CONFIG_MEMORY_COUNT) {
            pTunerDemod->configMemory[pTunerDemod->configMemoryLastEntry].target = target;
            pTunerDemod->configMemory[pTunerDemod->configMemoryLastEntry].bank = bank;
            pTunerDemod->configMemory[pTunerDemod->configMemoryLastEntry].address = address;
            pTunerDemod->configMemory[pTunerDemod->configMemoryLastEntry].value = (value & bitMask);
            pTunerDemod->configMemory[pTunerDemod->configMemoryLastEntry].bitMask = bitMask;
            pTunerDemod->configMemoryLastEntry++;
        }
        else {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_OVERFLOW);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_tunerdemod_Create (sony_tunerdemod_t * pTunerDemod,
                                      sony_regio_t * pRegio,
                                      sony_tunerdemod_create_param_t * pCreateParam)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_Create");

    if ((!pTunerDemod) || (!pRegio) || (!pCreateParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    sony_memset (pTunerDemod, 0, sizeof (sony_tunerdemod_t));

    pTunerDemod->pRegio = pRegio;
    pTunerDemod->createParam = *pCreateParam;

    /* Settings for non-diver (Single) */
    pTunerDemod->diverMode = SONY_TUNERDEMOD_DIVERMODE_SINGLE;
    pTunerDemod->pDiverSub = NULL;

    /* Set initial value */
    pTunerDemod->serialTsClockModeContinuous = 1; /* HW default is continuous */
    pTunerDemod->enableFEFIntermittentBase = 1;
    pTunerDemod->enableFEFIntermittentLite = 1;
    pTunerDemod->RFLevelCompensation = NULL;
    pTunerDemod->pLNAThresholdTableAir = NULL;
    pTunerDemod->pLNAThresholdTableCable = NULL;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_diver_Create (sony_tunerdemod_t * pTunerDemodMain,
                                            sony_regio_t * pRegioMain,
                                            sony_tunerdemod_t * pTunerDemodSub,
                                            sony_regio_t * pRegioSub,
                                            sony_tunerdemod_diver_create_param_t * pCreateParam)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_diver_Create");

    if ((!pTunerDemodMain) || (!pRegioMain) || (!pTunerDemodSub) || (!pRegioSub) || (!pCreateParam)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    sony_memset (pTunerDemodMain, 0, sizeof (sony_tunerdemod_t));
    sony_memset (pTunerDemodSub,  0, sizeof (sony_tunerdemod_t));

    /* Settings for main tuner */
    pTunerDemodMain->pRegio = pRegioMain;
    pTunerDemodMain->diverMode = SONY_TUNERDEMOD_DIVERMODE_MAIN;
    pTunerDemodMain->pDiverSub = pTunerDemodSub;
    pTunerDemodMain->createParam.enableInternalLDO = pCreateParam->enableInternalLDO;
    pTunerDemodMain->createParam.tsOutputIF = pCreateParam->tsOutputIF;
    pTunerDemodMain->createParam.xtalShareType = SONY_TUNERDEMOD_XTAL_SHARE_MASTER;
    pTunerDemodMain->createParam.xosc_cap = pCreateParam->xosc_cap_main;
    pTunerDemodMain->createParam.xosc_i = pCreateParam->xosc_i_main;
    pTunerDemodMain->createParam.isCXD2881GG = pCreateParam->isCXD2881GG;

    /* Settings for sub tuner */
    pTunerDemodSub->pRegio = pRegioSub;
    pTunerDemodSub->diverMode = SONY_TUNERDEMOD_DIVERMODE_SUB;
    pTunerDemodSub->pDiverSub = NULL;
    pTunerDemodSub->createParam.enableInternalLDO = pCreateParam->enableInternalLDO;
    pTunerDemodSub->createParam.tsOutputIF = pCreateParam->tsOutputIF;
    pTunerDemodSub->createParam.xtalShareType = SONY_TUNERDEMOD_XTAL_SHARE_SLAVE;
    pTunerDemodSub->createParam.xosc_cap = 0;
    pTunerDemodSub->createParam.xosc_i = pCreateParam->xosc_i_sub;
    pTunerDemodSub->createParam.isCXD2881GG = pCreateParam->isCXD2881GG;

    /* Set initial value */
    pTunerDemodMain->serialTsClockModeContinuous = 1; /* HW default is continuous */
    pTunerDemodMain->enableFEFIntermittentBase = 1;
    pTunerDemodMain->enableFEFIntermittentLite = 1;
    pTunerDemodMain->RFLevelCompensation = NULL;
    pTunerDemodMain->pLNAThresholdTableAir = NULL;
    pTunerDemodMain->pLNAThresholdTableCable = NULL;

    pTunerDemodSub->serialTsClockModeContinuous = 1; /* HW default is continuous */
    pTunerDemodSub->enableFEFIntermittentBase = 1;
    pTunerDemodSub->enableFEFIntermittentLite = 1;
    pTunerDemodSub->RFLevelCompensation = NULL;
    pTunerDemodSub->pLNAThresholdTableAir = NULL;
    pTunerDemodSub->pLNAThresholdTableCable = NULL;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_Initialize1 (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_Initialize1");

    if ((!pTunerDemod) || (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Initialize temporary variables in sony_tunerdemod_t */
    pTunerDemod->chipID = SONY_TUNERDEMOD_CHIP_ID_UNKNOWN;
    pTunerDemod->state = SONY_TUNERDEMOD_STATE_UNKNOWN;
    pTunerDemod->clockMode = SONY_TUNERDEMOD_CLOCKMODE_UNKNOWN;
    pTunerDemod->frequencyKHz = 0;
    pTunerDemod->system = SONY_DTV_SYSTEM_UNKNOWN;
    pTunerDemod->bandwidth = SONY_DTV_BW_UNKNOWN;
    pTunerDemod->scanMode = 0;
    sony_atomic_set (&(pTunerDemod->cancel), 0);

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        pTunerDemod->pDiverSub->chipID = SONY_TUNERDEMOD_CHIP_ID_UNKNOWN;
        pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_UNKNOWN;
        pTunerDemod->pDiverSub->clockMode = SONY_TUNERDEMOD_CLOCKMODE_UNKNOWN;
        pTunerDemod->pDiverSub->frequencyKHz = 0;
        pTunerDemod->pDiverSub->system = SONY_DTV_SYSTEM_UNKNOWN;
        pTunerDemod->pDiverSub->bandwidth = SONY_DTV_BW_UNKNOWN;
        pTunerDemod->pDiverSub->scanMode = 0;
        sony_atomic_set (&(pTunerDemod->pDiverSub->cancel), 0);
    }

    /* Read Chip ID */
    result = sony_tunerdemod_ChipID (pTunerDemod, &pTunerDemod->chipID);
    if (result != SONY_RESULT_OK) {
		//printk("error ttttttttttttt\n");
        SONY_TRACE_RETURN (result);
    }

//printk('bbbbbbbbbbbb\n');
    if (!SONY_TUNERDEMOD_CHIP_ID_VALID (pTunerDemod->chipID)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }
//printk('ccccccccccc\n');
    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = sony_tunerdemod_ChipID (pTunerDemod->pDiverSub, &pTunerDemod->pDiverSub->chipID);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (!SONY_TUNERDEMOD_CHIP_ID_VALID (pTunerDemod->pDiverSub->chipID)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
        }
    }
//printk('dddddddddddddd\n');
    /* TS IF, LDO setting */
    result = P_init1 (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = P_init1 (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_SLEEP (1);

    /* Oscillator related setting */
    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = P_init2 (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    result = P_init2 (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_SLEEP (5);

    result = P_init3 (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = P_init3 (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Tuner part initialization */
    result = RF_init1 (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = RF_init1 (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_Initialize2 (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_Initialize2");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Check that power on calibration was successfully finished. */
    {
        uint8_t cpuTaskCompleted = 0;

        result = sony_tunerdemod_CheckInternalCPUStatus (pTunerDemod, &cpuTaskCompleted);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (!cpuTaskCompleted) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
        }
    }

    result = RF_init2 (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = RF_init2 (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    /* Load Config memory to restore previous settings */
    result = loadConfigMemory (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = loadConfigMemory (pTunerDemod->pDiverSub);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    pTunerDemod->state = SONY_TUNERDEMOD_STATE_SLEEP;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_SLEEP;
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_CheckInternalCPUStatus (sony_tunerdemod_t * pTunerDemod, uint8_t * pTaskCompleted)
{
    uint16_t cpuStatus = 0;
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_CheckInternalCPUStatus");

    if ((!pTunerDemod) || (!pTaskCompleted)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_monitor_InternalCPUStatus (pTunerDemod, &cpuStatus);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SINGLE) {
        if (cpuStatus == 0) {
            *pTaskCompleted = 1;
        } else {
            *pTaskCompleted = 0;
        }

        SONY_TRACE_RETURN (result);
    } else {
        /* Diver */
        if (cpuStatus != 0) {
            *pTaskCompleted = 0;
            SONY_TRACE_RETURN (result);
        }

        result = sony_tunerdemod_monitor_InternalCPUStatus_sub (pTunerDemod, &cpuStatus);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (cpuStatus == 0) {
            *pTaskCompleted = 1;
        } else {
            *pTaskCompleted = 0;
        }

        SONY_TRACE_RETURN (result);
    }
}

sony_result_t sony_tunerdemod_CommonTuneSetting1 (sony_tunerdemod_t * pTunerDemod, sony_dtv_system_t system,
												  unsigned int frequencyKHz, sony_dtv_bandwidth_t bandwidth,
                                                  uint8_t oneSegmentOptimize, uint8_t oneSegmentOptimizeShiftDirection)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_CommonTuneSetting1");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (frequencyKHz < 4000) {
        /* Too small frequency */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
    }

    /* Go to sleep state at first. */
    result = sony_tunerdemod_Sleep (pTunerDemod);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    /* Handle Sleep <-> Sleep_PDT transition */
    {
        uint8_t data = 0;

        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x2B, &data, 1) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        switch (system) {
        case SONY_DTV_SYSTEM_DVBT:
        case SONY_DTV_SYSTEM_ISDBT:
        case SONY_DTV_SYSTEM_ISDBTSB:
        case SONY_DTV_SYSTEM_ISDBTMM_A:
        case SONY_DTV_SYSTEM_ISDBTMM_B:
            if (data == 0x00) {
                result = T_powerX (pTunerDemod, 1); /* T part Power ON */
                if (result != SONY_RESULT_OK) {
                    SONY_TRACE_RETURN (result);
                }

                /* Handle diver sub */
                if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                    result = T_powerX (pTunerDemod->pDiverSub, 1);
                    if (result != SONY_RESULT_OK) {
                        SONY_TRACE_RETURN (result);
                    }
                }

                /*
                 * T part register values were gone in T2 tuning state.
                 * This means that loadConfigMemory() should be called.
                 * But in current sequence, it will be called after SPLL_reset.
                 * So it's unnecessary to call it here.
                 */
            }
            break;

        case SONY_DTV_SYSTEM_DVBT2:
            if (data == 0x01) {
                result = T_powerX (pTunerDemod, 0); /* T part Power OFF */
                if (result != SONY_RESULT_OK) {
                    SONY_TRACE_RETURN (result);
                }

                /* Handle diver sub */
                if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                    result = T_powerX (pTunerDemod->pDiverSub, 0);
                    if (result != SONY_RESULT_OK) {
                        SONY_TRACE_RETURN (result);
                    }
                }
            }
            break;

        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
        }
    }

    /* Clock mode selection */
    {
        sony_tunerdemod_clockmode_t newClockMode = SONY_TUNERDEMOD_CLOCKMODE_UNKNOWN;

        if (((system == SONY_DTV_SYSTEM_ISDBT) || (system == SONY_DTV_SYSTEM_ISDBTSB))
            && ((bandwidth == SONY_DTV_BW_7_MHZ) || (bandwidth == SONY_DTV_BW_8_MHZ))) {
            newClockMode = SONY_TUNERDEMOD_CLOCKMODE_C;
        } else {
            {
                /* Check exception frequency table first. */
                int i = 0;

                typedef struct {
					unsigned int freqLower;
					unsigned int freqUpper;
                    sony_tunerdemod_clockmode_t clockMode;
                } ex_freq_info_t;

                ex_freq_info_t exTableS[] = {
                    {59001,  60999,  SONY_TUNERDEMOD_CLOCKMODE_B},
                    {86501,  88499,  SONY_TUNERDEMOD_CLOCKMODE_A},
                    {89001,  90999,  SONY_TUNERDEMOD_CLOCKMODE_B},
                    {119001, 120999, SONY_TUNERDEMOD_CLOCKMODE_B},
                    {149001, 150999, SONY_TUNERDEMOD_CLOCKMODE_B},
                    {174001, 175999, SONY_TUNERDEMOD_CLOCKMODE_A},
                    {179001, 180999, SONY_TUNERDEMOD_CLOCKMODE_B},
                    {209001, 210999, SONY_TUNERDEMOD_CLOCKMODE_B},
                    {239001, 240999, SONY_TUNERDEMOD_CLOCKMODE_B},
                    {349001, 350999, SONY_TUNERDEMOD_CLOCKMODE_A},
                    {359001, 360999, SONY_TUNERDEMOD_CLOCKMODE_B},
                    {479001, 480999, SONY_TUNERDEMOD_CLOCKMODE_B},
                    {699001, 700999, SONY_TUNERDEMOD_CLOCKMODE_C},
                    {719001, 720999, SONY_TUNERDEMOD_CLOCKMODE_B}
                };

                ex_freq_info_t exTableA[] = {
                    {174000, 179000, SONY_TUNERDEMOD_CLOCKMODE_A},
                    {179001, 226499, SONY_TUNERDEMOD_CLOCKMODE_C},
                    {226500, 230000, SONY_TUNERDEMOD_CLOCKMODE_A}
                };

                for (i = 0; i < sizeof(exTableS) / sizeof(exTableS[0]); i++) {
                    if ((exTableS[i].freqLower <= frequencyKHz) && (frequencyKHz <= exTableS[i].freqUpper)) {
                        /* Hit the table. */
                        newClockMode = exTableS[i].clockMode;
                        break;
                    }

                    if (frequencyKHz < exTableS[i].freqLower) {
                        /* Unnecessary to check this table any more. */
                        break;
                    }
                }

                if (newClockMode == SONY_TUNERDEMOD_CLOCKMODE_UNKNOWN) {
                    for (i = 0; i < sizeof(exTableA) / sizeof(exTableA[0]); i++) {
                        if ((exTableA[i].freqLower <= frequencyKHz) && (frequencyKHz <= exTableA[i].freqUpper)) {
                            /* Hit the table. */
                            newClockMode = exTableA[i].clockMode;
                            break;
                        }

                        if (frequencyKHz < exTableA[i].freqLower) {
                            /* Unnecessary to check this table any more. */
                            break;
                        }
                    }
                }
            }

            if (newClockMode == SONY_TUNERDEMOD_CLOCKMODE_UNKNOWN) {
                /* Decide by normal calculation. */
                int i = 0;
				unsigned int Fu = 0;
				unsigned int Fl = 0;
                                      /*  A        B        C   */
				unsigned int PLLCLK[] = { 1152000, 1120000, 1152000 };
                uint8_t DIV_DEMOD[] = {28,      24,      24};

                switch (bandwidth) {
                case SONY_DTV_BW_5_MHZ:
                    Fu = frequencyKHz + 2500;
                    Fl = frequencyKHz - 2500;
                    break;
                case SONY_DTV_BW_6_MHZ:
                    Fu = frequencyKHz + 3000;
                    Fl = frequencyKHz - 3000;
                    break;
                case SONY_DTV_BW_7_MHZ:
                    Fu = frequencyKHz + 3500;
                    Fl = frequencyKHz - 3500;
                    break;
                case SONY_DTV_BW_8_MHZ:
                    Fu = frequencyKHz + 4000;
                    Fl = frequencyKHz - 4000;
                    break;
                case SONY_DTV_BW_1_7_MHZ:
                    Fu = frequencyKHz + 850;
                    Fl = frequencyKHz - 850;
                    break;
                default:
                    SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
                }

                /*
                 * FCLK = PLLCLK / DIV_DEMOD
                 * CLKINBAND = floor(Fu / FCLK) - floor(Fl / FCLK)
                 *           = floor(Fu * DIV_DEMOD / PLLCLK) - floor(Fl * DIV_DEMOD / PLLCLK)
                 */
                for (i = 0; i < 3; i++) {
					unsigned int Vu = (Fu * DIV_DEMOD[i] + PLLCLK[i] - 1) / PLLCLK[i];
					unsigned int Vl = (Fl * DIV_DEMOD[i] + PLLCLK[i] - 1) / PLLCLK[i];

                    if (Vu == Vl) {
                        break;
                    }
                }

                switch (i) {
                case 0:
                    newClockMode = SONY_TUNERDEMOD_CLOCKMODE_A;
                    break;
                case 1:
                    newClockMode = SONY_TUNERDEMOD_CLOCKMODE_B;
                    break;
                case 2:
                    newClockMode = SONY_TUNERDEMOD_CLOCKMODE_C;
                    break;
                default:
                    /* This case should not be occurred */
                    newClockMode = SONY_TUNERDEMOD_CLOCKMODE_A;
                    break;
                }
            }
        }

        /* If fixed clock mode setting is set, overwrite it */
        if (pTunerDemod->fixedClockMode != SONY_TUNERDEMOD_CLOCKMODE_UNKNOWN) {
            newClockMode = pTunerDemod->fixedClockMode;
        }

        /* SPLL_reset (every time) */
        result = SPLL_reset (pTunerDemod, newClockMode);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        pTunerDemod->clockMode = newClockMode;

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = SPLL_reset (pTunerDemod->pDiverSub, newClockMode);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }

            pTunerDemod->pDiverSub->clockMode = newClockMode;
        }

        /* Load Config memory to restore previous settings */
        /* NOTE: SPLL_reset clears SLV-T register. */
        result = loadConfigMemory (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = loadConfigMemory (pTunerDemod->pDiverSub);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
    }

    {
        int shiftFrequencyKHz = 0;

        /* Decide shift frequency */
        if (oneSegmentOptimize) {
            /* One segment optimization for ISDB-T/Tmm */
            if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                /* Diver */
                shiftFrequencyKHz = 350; /* Main : +350kHz, Sub : -350kHz */
            } else {
                /* Single */
                if (oneSegmentOptimizeShiftDirection) {
                    shiftFrequencyKHz = 350;
                } else {
                    shiftFrequencyKHz = -350;
                }

                if (pTunerDemod->createParam.xtalShareType == SONY_TUNERDEMOD_XTAL_SHARE_SLAVE) {
                    /* Dual Sub -> invert direction */
                    shiftFrequencyKHz *= -1;
                }
            }
        } else {
            if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
                /* Diver */
                shiftFrequencyKHz = 150; /* Main : +150kHz, Sub : -150kHz */
            } else {
                /* Single */
                switch (pTunerDemod->createParam.xtalShareType) {
                case SONY_TUNERDEMOD_XTAL_SHARE_NONE:
                case SONY_TUNERDEMOD_XTAL_SHARE_EXTREF:
                default:
                    shiftFrequencyKHz = 0;
                    break;
                case SONY_TUNERDEMOD_XTAL_SHARE_MASTER:
                    /* Dual Main */
                    shiftFrequencyKHz = 150;
                    break;
                case SONY_TUNERDEMOD_XTAL_SHARE_SLAVE:
                    /* Dual Sub */
                    shiftFrequencyKHz = -150;
                    break;
                }
            }
        }

        /* Start X_tune sequence */
        result = X_tune1 (pTunerDemod, system, frequencyKHz, bandwidth, pTunerDemod->isCableInput, shiftFrequencyKHz);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = X_tune1 (pTunerDemod->pDiverSub, system, frequencyKHz, bandwidth, pTunerDemod->isCableInput, -shiftFrequencyKHz);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }

        SONY_SLEEP (10);

        /* Check that tuning was successfully finished. */
        {
            uint8_t cpuTaskCompleted = 0;

            result = sony_tunerdemod_CheckInternalCPUStatus (pTunerDemod, &cpuTaskCompleted);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }

            if (!cpuTaskCompleted) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
            }
        }

        result = X_tune2 (pTunerDemod, bandwidth, pTunerDemod->clockMode, shiftFrequencyKHz);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = X_tune2 (pTunerDemod->pDiverSub, bandwidth, pTunerDemod->pDiverSub->clockMode, -shiftFrequencyKHz);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }
    }

    if (pTunerDemod->createParam.tsOutputIF == SONY_TUNERDEMOD_TSOUT_IF_TS) {
        result = setTSClockModeAndFreq (pTunerDemod, system);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    } else {
        /* Write PID filter setting for SPI/SDIO. */
        sony_tunerdemod_pid_filter_config_t * pPIDFilterConfig;

        if (pTunerDemod->pidFilterConfigEnable) {
            pPIDFilterConfig = &pTunerDemod->pidFilterConfig;
        } else {
            pPIDFilterConfig = NULL;
        }

        result = pidFilterSetting (pTunerDemod, pPIDFilterConfig);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_CommonTuneSetting2 (sony_tunerdemod_t * pTunerDemod, sony_dtv_system_t system,
                                                  uint8_t enableFEFIntermittentControl)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_CommonTuneSetting2");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Soft Reset, FEF intermittent control setting */
    result = X_tune3 (pTunerDemod, system, enableFEFIntermittentControl);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        result = X_tune3 (pTunerDemod->pDiverSub, system, enableFEFIntermittentControl);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* Diver pin setting */
        result = X_tune4 (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    result = sony_tunerdemod_SetTSOutput (pTunerDemod, 1);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_Sleep (sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_Sleep");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state == SONY_TUNERDEMOD_STATE_SLEEP) {
        /* Do nothing. */
    } else if (pTunerDemod->state == SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* TS output disable */
        result = sony_tunerdemod_SetTSOutput (pTunerDemod, 0);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            /* Diver pin setting */
            result = X_sleep1 (pTunerDemod);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }

        /* FEF intermittent control disable */
        result = X_sleep2 (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = X_sleep2 (pTunerDemod->pDiverSub);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }

        /* System dependent demodulator setting */
        switch (pTunerDemod->system) {
        case SONY_DTV_SYSTEM_DVBT:
            result = sony_tunerdemod_dvbt_SleepSetting (pTunerDemod);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
            break;

        case SONY_DTV_SYSTEM_DVBT2:
            result = sony_tunerdemod_dvbt2_SleepSetting (pTunerDemod);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
            break;

        case SONY_DTV_SYSTEM_ISDBT:
            result = sony_tunerdemod_isdbt_SleepSetting (pTunerDemod);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
            break;

        case SONY_DTV_SYSTEM_ISDBTSB:
            result = sony_tunerdemod_isdbtsb_SleepSetting (pTunerDemod);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
            break;

        case SONY_DTV_SYSTEM_ISDBTMM_A:
            result = sony_tunerdemod_isdbtmm_A_SleepSetting (pTunerDemod);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
            break;

       case SONY_DTV_SYSTEM_ISDBTMM_B:
            result = sony_tunerdemod_isdbtmm_B_SleepSetting (pTunerDemod);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
            break;

        default:
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        result = X_sleep3 (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = X_sleep3 (pTunerDemod->pDiverSub);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }

        result = X_sleep4 (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            result = X_sleep4 (pTunerDemod->pDiverSub);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }

        pTunerDemod->state = SONY_TUNERDEMOD_STATE_SLEEP;
        pTunerDemod->frequencyKHz = 0;
        pTunerDemod->system = SONY_DTV_SYSTEM_UNKNOWN;
        pTunerDemod->bandwidth = SONY_DTV_BW_UNKNOWN;

        if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
            pTunerDemod->pDiverSub->state = SONY_TUNERDEMOD_STATE_SLEEP;
            pTunerDemod->pDiverSub->frequencyKHz = 0;
            pTunerDemod->pDiverSub->system = SONY_DTV_SYSTEM_UNKNOWN;
            pTunerDemod->pDiverSub->bandwidth = SONY_DTV_BW_UNKNOWN;
        }
    } else {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_SetConfig (sony_tunerdemod_t * pTunerDemod,
                                         sony_tunerdemod_config_id_t id,
                                         int value)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data[2] = {0};
    uint8_t needSubSetting = 0; /* Need to set to diver sub */

    SONY_TRACE_ENTER ("sony_tunerdemod_SetConfig");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    switch (id) {
    case SONY_TUNERDEMOD_CONFIG_OUTPUT_SEL_MSB:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-M>  00h     C4h     [4]    1'b0       OWFMT_LSB1STON
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0xC4, (uint8_t) (value ? 0x00 : 0x10), 0x10);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSVALID_ACTIVE_HI:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-M>  00h     C5h     [1]    1'b0       OWFMT_VALINV
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0xC5, (uint8_t) (value ? 0x00 : 0x02), 0x02);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSSYNC_ACTIVE_HI:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-M>  00h     C5h     [2]    1'b0       OWFMT_STINV
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0xC5, (uint8_t) (value ? 0x00 : 0x04), 0x04);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSERR_ACTIVE_HI:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-M>  00h     CBh     [0]    1'b0       OWFMT_ERRINV
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0xCB, (uint8_t) (value ? 0x00 : 0x01), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_LATCH_ON_POSEDGE:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-M>  00h     C5h     [0]    1'b1       OWFMT_CKINV
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0xC5, (uint8_t) (value ? 0x01 : 0x00), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSCLK_CONT:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* Store the serial clock mode */
        pTunerDemod->serialTsClockModeContinuous = (uint8_t) (value ? 0x01 : 0x00);
        break;

    case SONY_TUNERDEMOD_CONFIG_TSCLK_MASK:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-M>  00h    C6h     [4:0]     5'b00000    OWFMT_CKDISABLE
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00,  0xC6, (uint8_t) value, 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSVALID_MASK:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-M>  00h     C8h     [4:0]    5'b00011    OWFMT_VALDISABLE
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0xC8, (uint8_t) value, 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSERR_MASK:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0x1F)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit      default     Name
         * -------------------------------------------------------------
         * <SLV-M>  00h     C9h     [4:0]    5'b00000    OWFMT_ERRDISABLE
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0xC9, (uint8_t) value, 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSERR_VALID_DISABLE:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-M>  00h     91h     [0]    1'b0       OREG_ERR_VALID_DISABLE
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x91, (uint8_t) (value ? 0x01 : 0x00), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSPIN_CURRENT:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-X>  00h     51h     [0]    1'b1       OREG_TSCLK_DS
         * <SLV-X>  00h     51h     [1]    1'b1       OREG_TSDATA_DS
         * <SLV-X>  00h     51h     [2]    1'b1       OREG_TSSYNC_DS
         * <SLV-X>  00h     51h     [3]    1'b1       OREG_TSVALID_DS
         * <SLV-X>  00h     51h     [4]    1'b1       OREG_SDDATA1_DS
         * <SLV-X>  00h     51h     [5]    1'b1       OREG_SDDATA2_DS
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x51, (uint8_t) value, 0x3F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSPIN_PULLUP_MANUAL:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-X>  00h     50h     [7]    1'b0       OREG_TS_IO_PU_MANUAL
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x50, (uint8_t) (value ? 0x80 : 0x00), 0x80);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSPIN_PULLUP:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        /* slave    Bank    Addr    Bit    default    Name
         * ---------------------------------------------------
         * <SLV-X>  00h     50h     [0]    1'b1       OREG_TSCLK_PE
         * <SLV-X>  00h     50h     [1]    1'b1       OREG_TSDATA_PE
         * <SLV-X>  00h     50h     [2]    1'b1       OREG_TSSYNC_PE
         * <SLV-X>  00h     50h     [3]    1'b1       OREG_TSVALID_PE
         * <SLV-X>  00h     50h     [4]    1'b1       OREG_SDDATA1_PE
         * <SLV-X>  00h     50h     [5]    1'b1       OREG_SDDATA2_pE
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x50, (uint8_t) value, 0x3F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TSCLK_FREQ:
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 1)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* Store the clock frequency mode for terrestrial and cable systems */
        pTunerDemod->serialTsClkFreq = (sony_tunerdemod_serial_ts_clk_t) value;
        break;

    case SONY_TUNERDEMOD_CONFIG_TSBYTECLK_MANUAL:
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 0xFF)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        pTunerDemod->tsByteClkManualSetting = (uint8_t) value;

        break;

    case SONY_TUNERDEMOD_CONFIG_TS_PACKET_GAP:
        /* This register can change only in SLEEP state */
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        if ((value < 0) || (value > 7)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* Slave    Bank    Addr    Bit      Default    Value    Name
         * -------------------------------------------------------------------
         * <SLV-M>  00h     D6h    [2:0]      3'd4       3'dx    OTSRC_DIVCKREV
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0xD6, (uint8_t)value, 0x07);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        break;

    case SONY_TUNERDEMOD_CONFIG_TS_BACKWARDS_COMPATIBLE:
        if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
        }

        pTunerDemod->isTsBackwardsCompatibleMode = (uint8_t) (value ? 1 : 0);

        break;

    case SONY_TUNERDEMOD_CONFIG_PWM_VALUE:
        /* Perform range checking on value */
        if ((value < 0) || (value > 0x1000)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* Slave    Bank    Addr    Bit      Default    Value    Name
         * -------------------------------------------------------------------
         * <SLV-T>  00h     22h     [0]      1'b0       1'b1     OREG_RFAGCSEL
         * <SLV-T>  00h     23h     [4]      1'b0       1'bx     OREG_GDA_ALLONE_RFAGC
         * <SLV-T>  00h     23h     [3:0]    4'h0       4'hx     OREG_GDA_VAL_RFAGC[11:8]
         * <SLV-T>  00h     24h     [7:0]    8'h00      8'hxx    OREG_GDA_VAL_RFAGC[7:0]
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x22, (uint8_t)(value ? 0x01 : 0x00), 0x01);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        {
            uint8_t data[2];
            data[0] = (uint8_t) (((uint16_t)value >> 8) & 0x1F);
            data[1] = (uint8_t) ((uint16_t)value & 0xFF);

            result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x23, data[0], 0x1F);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }

            result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x24, data[1], 0xFF);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (result);
            }
        }

        break;

    case SONY_TUNERDEMOD_CONFIG_INTERRUPT:
        data[0] = (uint8_t)((value >> 8) & 0xFF);
        data[1] = (uint8_t)( value       & 0xFF);
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x48, data[0], 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x49, data[1], 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_INTERRUPT_LOCK_SEL:
        data[0] = (uint8_t)(value & 0x07);
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x4A, data[0], 0x07);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_INTERRUPT_INV_LOCK_SEL:
        data[0] = (uint8_t)((value & 0x07) << 3);
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x4A, data[0], 0x38);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_FIXED_CLOCKMODE:
        if ((value < (int)SONY_TUNERDEMOD_CLOCKMODE_UNKNOWN) || (value > (int)SONY_TUNERDEMOD_CLOCKMODE_C)) {
             SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }
        pTunerDemod->fixedClockMode = (sony_tunerdemod_clockmode_t)value;
        break;

    case SONY_TUNERDEMOD_CONFIG_CABLE_INPUT:
        pTunerDemod->isCableInput = (uint8_t) (value ? 1 : 0);
        break;

    case SONY_TUNERDEMOD_CONFIG_DVBT2_FEF_INTERMITTENT_BASE:
        pTunerDemod->enableFEFIntermittentBase = (uint8_t) (value ? 1 : 0);
        break;

    case SONY_TUNERDEMOD_CONFIG_DVBT2_FEF_INTERMITTENT_LITE:
        pTunerDemod->enableFEFIntermittentLite = (uint8_t) (value ? 1 : 0);
        break;

    case SONY_TUNERDEMOD_CONFIG_TS_BUFFER_ALMOST_EMPTY_THRESHOLD:
        data[0] = (uint8_t)((value >> 8) & 0x07);
        data[1] = (uint8_t)( value       & 0xFF);
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x99, data[0], 0x07);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x9A, data[1], 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TS_BUFFER_ALMOST_FULL_THRESHOLD:
        data[0] = (uint8_t)((value >> 8) & 0x07);
        data[1] = (uint8_t)( value       & 0xFF);
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x9B, data[0], 0x07);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x9C, data[1], 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_TS_BUFFER_RRDY_THRESHOLD:
        data[0] = (uint8_t)((value >> 8) & 0x07);
        data[1] = (uint8_t)( value       & 0xFF);
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x9D, data[0], 0x07);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x00, 0x9E, data[1], 0xFF);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_BLINDTUNE_DVBT2_FIRST:
        /* Store the blindTune / Scan system priority setting */
        pTunerDemod->blindTuneDvbt2First = (uint8_t)(value ? 1 : 0);
        break;

    case SONY_TUNERDEMOD_CONFIG_DVBT_BERN_PERIOD:
        /* Set the measurment period for Pre-RS BER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 31)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit    default          Name
         * -----------------------------------------------------------------------
         * <SLV-M>   10h     60h    [4:0]     5'h0B        OREG_BERN_PERIOD[4:0]
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x10, 0x60, (uint8_t) (value & 0x1F), 0x1F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_DVBT_VBER_PERIOD:
        /* Set the measurment period for Pre-Viterbi BER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 7)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ----------------------------------------------------
        * <SLV-M>  10h     6Fh    [2:0]   OREG_VBER_PERIOD_SEL
        */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x10, 0x6F, (uint8_t) (value & 0x07), 0x07);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_DVBT2_BBER_MES:
        /* Set the measurment period for Pre-BCH BER (DVB-T2) and Post-BCH FER (DVB-T2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ---------------------------------------------
        * <SLV-M>  20h     72h    [3:0]   OREG_BBER_MES
        */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x20, 0x72, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_DVBT2_LBER_MES:
        /* Set the measurment period for Pre-LDPC BER (DVB-T2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * ---------------------------------------------
        * <SLV-M>  20h     6Fh    [3:0]   OREG_LBER_MES
        */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x20, 0x6F, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_DVBT_PER_MES:
        /* Set the measurment period for PER (DVB-T). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr    Bit              Name
         * ---------------------------------------------------------
         * <SLV-M>   10h     5Ch     [3:0]        OREG_PER_MES[3:0]
         */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x10, 0x5C, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_DVBT2_PER_MES:
        /* Set the measurment period for PER (DVB-T2). */
        /* Verify range of value */
        if ((value < 0) || (value > 15)) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_RANGE);
        }

        /* slave    Bank    Addr   Bit     Name
        * -----------------------------------------------
        * <SLV-M>  24h     DCh    [3:0]   OREG_SP_PER_MES
        */
        result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x24, 0xDC, (uint8_t) (value & 0x0F), 0x0F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
        break;

    case SONY_TUNERDEMOD_CONFIG_ISDBT_BERPER_PERIOD:
        {
            uint8_t data[2];

            data[0] = (uint8_t)((value & 0x00007F00) >> 8);
            data[1] = (uint8_t)(value & 0x000000FF);

            /* slave     Bank    Addr    Bit     Name
             * -----------------------------------------------
             * <SLV-M>    60h    5Bh     [6:0]    OBER_CDUR_RSA[14:8]
             * <SLV-M>    60h    5Ch     [7:0]    OBER_CDUR_RSA[7:0]
             */
            result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x60, 0x5B, data[0], 0x7F);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN(result);
            }
            result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_DEMOD, 0x60, 0x5C, data[1], 0xFF);
            if (result != SONY_RESULT_OK) {
                SONY_TRACE_RETURN(result);
            }
        }
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (needSubSetting && (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN)) {
        /* Same setting to sub. */
        result = sony_tunerdemod_SetConfig (pTunerDemod->pDiverSub, id, value);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_GPIOSetConfig (sony_tunerdemod_t * pTunerDemod,
                                             uint8_t id,
                                             uint8_t enable,
                                             sony_tunerdemod_gpio_mode_t mode,
                                             uint8_t openDrain,
                                             uint8_t invert)
{
    sony_result_t result = SONY_RESULT_OK;
    SONY_TRACE_ENTER ("sony_tunerdemod_GPIOSetConfig");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (id > 2) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (mode > SONY_TUNERDEMOD_GPIO_MODE_EEW) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /*
     * slave    Bank    Addr    Bit     default      Name
     * -----------------------------------------------------------------------------------------
     * <SLV-X>  00h     40h     [3:0]    8'h00       OREG_GPIO_SEL0
     * <SLV-X>  00h     41h     [3:0]    8'h00       OREG_GPIO_SEL1
     * <SLV-X>  00h     42h     [3:0]    8'h00       OREG_GPIO_SEL2
     * <SLV-X>  00h     43h     [2:0]    8'h00       OREG_GPIO_OPD
     * <SLV-X>  00h     44h     [2:0]    8'h00       OREG_GPIO_INV
     * <SLV-X>  00h     45h     [2:0]    8'h07       OREG_GPIO_HIZ
     */

    result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x40 + id, (uint8_t)mode, 0x0F);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x43, (uint8_t)(openDrain ? (1 << id) : 0), (uint8_t)(1 << id));
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x44, (uint8_t)(invert ? (1 << id) : 0), (uint8_t)(1 << id));
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    result = sony_tunerdemod_SetAndSaveRegisterBits(pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x45, (uint8_t)(enable ? 0 : (1 << id)), (uint8_t)(1 << id));
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_GPIOSetConfig_sub (sony_tunerdemod_t * pTunerDemod,
                                                 uint8_t id,
                                                 uint8_t enable,
                                                 sony_tunerdemod_gpio_mode_t mode,
                                                 uint8_t openDrain,
                                                 uint8_t invert)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_GPIOSetConfig_sub");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_GPIOSetConfig (pTunerDemod->pDiverSub, id, enable, mode, openDrain, invert);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_GPIORead (sony_tunerdemod_t * pTunerDemod,
                                        uint8_t id,
                                        uint8_t * pValue)
{
    uint8_t data = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_GPIORead");

    if ((!pTunerDemod) || (!pValue)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (id > 2) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /*
     * slave    Bank    Addr    Bit     default      Name
     * -----------------------------------------------------------------------------------------
     * <SLV-X>  0Ah     20h     [2:0]    8'h00       IGPI
     */

    /* SLV-X bank 0x0A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x20, &data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    *pValue = (uint8_t)((data >> id) & 0x01);

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_GPIORead_sub (sony_tunerdemod_t * pTunerDemod,
                                            uint8_t id,
                                            uint8_t * pValue)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_GPIORead_sub");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_GPIORead (pTunerDemod->pDiverSub, id, pValue);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_GPIOWrite (sony_tunerdemod_t * pTunerDemod,
                                         uint8_t id,
                                         uint8_t value)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_GPIOWrite");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (id > 2) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /*
     * slave    Bank    Addr    Bit     default      Name
     * -----------------------------------------------------------------------------------------
     * <SLV-X>  00h     46h     [2:0]    8'h00       OREG_GPO
     */

    result = sony_tunerdemod_SetAndSaveRegisterBits (pTunerDemod, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x46, (uint8_t)(value ? (1 << id) : 0), (uint8_t)(1 << id));
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_GPIOWrite_sub (sony_tunerdemod_t * pTunerDemod,
                                             uint8_t id,
                                             uint8_t value)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_GPIOWrite_sub");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_GPIOWrite (pTunerDemod->pDiverSub, id, value);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_InterruptRead (sony_tunerdemod_t * pTunerDemod,
                                             uint16_t * pValue)
{
    uint8_t data[2] = {0};

    SONY_TRACE_ENTER ("sony_tunerdemod_InterruptRead");

    if ((!pTunerDemod) || (!pValue)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* SLV-X bank 0x0A */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x0A) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x15, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    *pValue = (uint16_t)(((uint16_t)data[0] << 8) | (data[1]));

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_InterruptClear (sony_tunerdemod_t * pTunerDemod,
                                              uint16_t value)
{
    uint8_t data[2] = {0};

    SONY_TRACE_ENTER ("sony_tunerdemod_InterruptClear");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    data[0] = (uint8_t)((value >> 8) & 0xFF);
    data[1] = (uint8_t)( value       & 0xFF);
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x3C, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_TSBufferClear (sony_tunerdemod_t * pTunerDemod,
                                             uint8_t clearOverflowFlag,
                                             uint8_t clearUnderflowFlag,
                                             uint8_t clearBuffer)
{
    uint8_t data[2] = {0};

    SONY_TRACE_ENTER ("sony_tunerdemod_TSBufferClear");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* SLV-T bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    data[0]  = (uint8_t)(clearOverflowFlag ? 0x02 : 0x00);
    data[0] |= (uint8_t)(clearUnderflowFlag ? 0x01 : 0x00);
    data[1]  = (uint8_t)(clearBuffer ? 0x01 : 0x00);
    if (pTunerDemod->pRegio->WriteRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x9F, data, 2) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_ChipID (sony_tunerdemod_t * pTunerDemod,
                                      sony_tunerdemod_chip_id_t * pChipID)
{
    uint8_t data = 0;

    SONY_TRACE_ENTER ("sony_tunerdemod_ChipID");
    
    if ((!pTunerDemod) || (!pChipID)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }
    if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0xFD, &data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    *pChipID = (sony_tunerdemod_chip_id_t) data;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_SetAndSaveRegisterBits (sony_tunerdemod_t * pTunerDemod,
                                                      sony_regio_target_t target,
                                                      uint8_t bank,
                                                      uint8_t address,
                                                      uint8_t value,
                                                      uint8_t bitMask)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_SetAndSaveRegisterBits");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set the bank */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, target, 0x00, bank) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Write the register value */
    if (sony_regio_SetRegisterBits (pTunerDemod->pRegio, target, address, value, bitMask) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    /* Store the updated setting */
    result = setConfigMemory (pTunerDemod, target, bank, address, value, bitMask);
    if (result != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (result);
    }

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_SetScanMode (sony_tunerdemod_t * pTunerDemod,
                                           sony_dtv_system_t system,
                                           uint8_t scanModeEnabled)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_SetScanMode");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    SONY_ARG_UNUSED (system);

    pTunerDemod->scanMode = scanModeEnabled;

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        sony_result_t result = SONY_RESULT_OK;

        result = sony_tunerdemod_SetScanMode (pTunerDemod->pDiverSub, system, scanModeEnabled);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_SetPIDFilter (sony_tunerdemod_t * pTunerDemod,
                                            sony_tunerdemod_pid_filter_config_t * pPIDFilterConfig)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_SetPIDFilter");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->createParam.tsOutputIF == SONY_TUNERDEMOD_TSOUT_IF_TS) {
        /* PID filter is for SPI/SDIO. */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    if (pPIDFilterConfig) {
        pTunerDemod->pidFilterConfig = *pPIDFilterConfig;
        pTunerDemod->pidFilterConfigEnable = 1;
    } else {
        pTunerDemod->pidFilterConfigEnable = 0;
    }

    if (pTunerDemod->state == SONY_TUNERDEMOD_STATE_ACTIVE) {
        /* Write setting now. */
        result = pidFilterSetting (pTunerDemod, pPIDFilterConfig);
        if (result != SONY_RESULT_OK) {
             SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_SetRFLevelCompensation (sony_tunerdemod_t * pTunerDemod,
                                                      sony_result_t (* pRFLevelCompensation) (sony_tunerdemod_t *, int *))
{
    SONY_TRACE_ENTER ("sony_tunerdemod_SetRFLevelCompensation");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pTunerDemod->RFLevelCompensation = pRFLevelCompensation;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_SetRFLevelCompensation_sub (sony_tunerdemod_t * pTunerDemod,
                                                          sony_result_t (* pRFLevelCompensation) (sony_tunerdemod_t *, int *))
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_SetRFLevelCompensation_sub");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_SetRFLevelCompensation (pTunerDemod->pDiverSub, pRFLevelCompensation);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_SetLNAThreshold (sony_tunerdemod_t * pTunerDemod,
                                               sony_tunerdemod_lna_threshold_table_air_t * pTableAir,
                                               sony_tunerdemod_lna_threshold_table_cable_t * pTableCable)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_SetLNAThreshold");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    pTunerDemod->pLNAThresholdTableAir = pTableAir;
    pTunerDemod->pLNAThresholdTableCable = pTableCable;

    SONY_TRACE_RETURN (SONY_RESULT_OK);

}

sony_result_t sony_tunerdemod_SetLNAThreshold_sub (sony_tunerdemod_t * pTunerDemod,
                                                   sony_tunerdemod_lna_threshold_table_air_t * pTableAir,
                                                   sony_tunerdemod_lna_threshold_table_cable_t * pTableCable)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_SetLNAThreshold_sub");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode != SONY_TUNERDEMOD_DIVERMODE_MAIN) {
        /* This api is accepted for Diver Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    result = sony_tunerdemod_SetLNAThreshold (pTunerDemod->pDiverSub, pTableAir, pTableCable);

    SONY_TRACE_RETURN (result);
}

sony_result_t sony_tunerdemod_SetTSPinHighLow (sony_tunerdemod_t * pTunerDemod, uint8_t enable, uint8_t value)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("sony_tunerdemod_SetTSPinHighLow");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) {
        /* This api is accepted in SLEEP state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pTunerDemod->createParam.tsOutputIF != SONY_TUNERDEMOD_TSOUT_IF_TS) {
        /* Not supported for SPI/SDIO. */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_NOSUPPORT);
    }

    /* slave    Bank    Addr    Bit    default    Name
     * ---------------------------------------------------
     * <SLV-X>  00h     50h     [0]    1'b1       OREG_TSCLK_PE
     * <SLV-X>  00h     50h     [1]    1'b1       OREG_TSDATA_PE
     * <SLV-X>  00h     50h     [2]    1'b1       OREG_TSSYNC_PE
     * <SLV-X>  00h     50h     [3]    1'b1       OREG_TSVALID_PE
     * <SLV-X>  00h     50h     [4]    1'b1       OREG_SDDATA1_PE
     * <SLV-X>  00h     50h     [5]    1'b1       OREG_SDDATA2_PE
     * <SLV-X>  00h     50h     [7]    1'b0       OREG_TS_IO_PU_MANUAL
     *
     * <SLV-X>  00h     52h     [0]    1'b1       OREG_TSCLK_HIZ
     * <SLV-X>  00h     52h     [1]    1'b1       OREG_TSDATA_HIZ
     * <SLV-X>  00h     52h     [2]    1'b1       OREG_TSSYNC_HIZ
     * <SLV-X>  00h     52h     [3]    1'b1       OREG_TSVALID_HIZ
     * <SLV-X>  00h     52h     [4]    1'b1       OREG_SDDATA1_HIZ
     */

    /* SLV-X bank 0x00 */
    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    if (enable) {
        /* Pull up setting */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x50, ((value & 0x1F) | 0x80)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Hi-Z setting */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x52, (value & 0x1F)) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }
    } else {
        /* Pull up setting (HW default value) */
        result = pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x50, 0x3F);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }

        /* Hi-Z setting */
        if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x52, 0x1F) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
        }

        /* Load config memory to roll back PULLUP related setting by SetConfig if existed. */
        result = loadConfigMemory (pTunerDemod);
        if (result != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (result);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_tunerdemod_SetTSOutput (sony_tunerdemod_t * pTunerDemod,
                                           uint8_t enable)
{
    SONY_TRACE_ENTER ("sony_tunerdemod_SetTSOutput");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pTunerDemod->diverMode == SONY_TUNERDEMOD_DIVERMODE_SUB) {
        /* This api is accepted for Single/Main instance only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pTunerDemod->state != SONY_TUNERDEMOD_STATE_SLEEP) && (pTunerDemod->state != SONY_TUNERDEMOD_STATE_ACTIVE)) {
        /* This api is accepted in SLEEP and ACTIVE states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    switch (pTunerDemod->createParam.tsOutputIF) {
    case SONY_TUNERDEMOD_TSOUT_IF_TS:
        if (enable) {
            /* SLV-X bank 0x00 */
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x52, 0x00) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
            /* SLV-T bank 0x00 */
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xC3, 0x00) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        } else {
            /* SLV-T bank 0x00 */
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, 0x00) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0xC3, 0x01) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
            /* SLV-X bank 0x00 */
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x00, 0x00) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
            if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_SYSTEM, 0x52, 0x1F) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;

    case SONY_TUNERDEMOD_TSOUT_IF_SPI:
        break;

    case SONY_TUNERDEMOD_TSOUT_IF_SDIO:
        break;

    default:
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t SLVT_FreezeReg(sony_tunerdemod_t * pTunerDemod)
{
    sony_result_t result = SONY_RESULT_OK;

    SONY_TRACE_ENTER ("SLVT_FreezeReg");

    if (!pTunerDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    switch (pTunerDemod->createParam.tsOutputIF) {
    case SONY_TUNERDEMOD_TSOUT_IF_SPI:
    case SONY_TUNERDEMOD_TSOUT_IF_SDIO:
        {
            uint8_t data = 0;

            /* For SPI/SDIO, dummy read is necessary to update register value. */
            if (pTunerDemod->pRegio->ReadRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x00, &data, 1) != SONY_RESULT_OK) {
                SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
            }
        }
        break;
    case SONY_TUNERDEMOD_TSOUT_IF_TS:
    default:
        break;
    }

    if (pTunerDemod->pRegio->WriteOneRegister (pTunerDemod->pRegio, SONY_REGIO_TARGET_DEMOD, 0x01, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_IO);
    }

    SONY_TRACE_RETURN (result);
}
