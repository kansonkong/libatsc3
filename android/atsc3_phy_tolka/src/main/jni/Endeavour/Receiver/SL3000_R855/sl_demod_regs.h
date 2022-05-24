/*****************************************************************************/
/*  Saankhya Confidential                                                    */
/*  COPYRIGHT (C) 2020 Saankhya Labs Pvt. Ltd - All Rights Reserved          */
/*                                                                           */
/* Saankhya Labs makes no warranty express or implied including but not      */
/* limited to, any warranty of merchantability or fitness for a particular   */
/* purpose and/or requirements, for a particular purpose in relation to the  */
/* LICENSED MATERIALS, which is provided “AS IS”, WITH ALL FAULTS.           */
/*                                                                           */
/* Saankhya Labs does not represent or warrant that the LICENSED MATERIALS   */
/* provided here under is free of infringement of any third party patents,   */
/* copyrights, trade secrets or other intellectual property rights.          */
/* ALL WARRANTIES, CONDITIONS OR OTHER TERMS IMPLIED BY LAW ARE EXCLUDED TO  */
/* THE FULLEST EXTENT PERMITTED BY LAW                                       */
/* NOTICE: All information contained herein is, and remains the property of  */
/* Saankhya Labs Pvt. Ltd and its suppliers, if any. The intellectual and    */
/* technical concepts contained herein are proprietary to Saankhya Labs & its*/
/* suppliers and may be covered by U.S. and Foreign Patents, patents in      */
/* process, and are protected by trade secret or copyright law. Dissemination*/
/* of this information or reproduction of this material is strictly forbidden*/
/* unless prior written permission is obtained from Saankhya Labs Pvt Ltd.   */
/*                                                                           */
/*  File Name   :   sl_demod_regs.h                                          */
/*  version     :   0.9                                                      */
/*  Date        :   07/10/2021                                               */
/*  Description :   SLDemod Register Maping                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef _SL_DEMOD_REGS_H_
#define _SL_DEMOD_REGS_H_

/*
 * SL Chip base Addresses
 */
#define SL_DEMOD_AA_REG_BASE_ADDR  (0x80000C00)       /* AA */
#define SL_DEMOD_BB_REG_BASE_ADDR  (0x80001000)       /* BB */

/* 
 *  Generic Standard Configuration Parameters for ALL DEMOD STANDARDS -
 *  SET_* Read/Write Access
 *  GET_* Read Only Access 
 */
#define GET_DEMOD_STD                  0x00000000
#define SET_IF_SIGNAL_CHAR             0x00000004
#define SET_TUNER_IF_FREQ              0x00000008
#define SET_AGC_REF_VALUE              0x0000000C
#define SET_OUT_CONFIG                 0x00000010
#define SET_CONFIG_DEMOD_PLP_ID        0x00000014
#define GET_DEMOD_BOOT_STATUS          0x00000018
#define GET_DEMOD_LOCK_STATUS          0x0000001C
#define GET_CPU_STATUS                 0x00000020
#define GET_SW_MAJOR_NUM               0x00000024
#define GET_SW_MINOR_NUM               0x00000028
#define SET_DEMOD_STOP                 0x0000002C
#define SET_IQ_SWAP                    0x00000030
#define SET_I_COEFFICIENT_1            0x00000034   
#define SET_Q_COEFFICIENT_1            0x00000038  
#define SET_I_COEFFICIENT_2            0x0000003C  
#define SET_Q_COEFFICIENT_2            0x00000040  
#define SET_TSO_CLK_INV                0x00000044 
#define SET_DEMOD_START                0x00000048 /* Release All DSP's */
#define GET_DEMOD_RESYNC_COUNT         0x0000004C
#define GET_MULTIPLP_LLS_INFO          0x00000050 /* size 0x08 bytes */
#define SET_TUNER_RF_FREQUENCY         0x00000058 
#define SET_TUNER_CRYSTAL_FREQUENCY    0x0000005C 
#define SET_DC_OFFSET_CALIB_START      0x00000060 
#define SET_DC_OFF_CALIB_BLKSIZE       0x00000064
#define SET_DC_OFFSET_STATCOM          0x00000068
#define GET_DC_IREG                    0x0000006C 
#define GET_DC_QREG                    0x00000070
#define SET_DEMOD_CALIB_START          0x00000074
#define SET_LNA_MODE                   0x00000094
#define SET_MCM_TUNER_RESET            0x00000098
#define GET_BB_RMS                     0x0000009C /* Base Band RMS */

/* ATSC 3.0 Standard Specific Parameters */
#define SET_ATSC3_REGION               0x00000090

#define GET_ATSC3_DECODE_PERFORMANCE       0x000000A0
#define ATSC3_DECODE_PERFORMANCE_SIZE      0xBC /* Size in Bytes */
#define GET_ATSC3_BS                       0x0000015C
#define ATSC3_BS_SIZE                      0x20 /* Size in Bytes */
#define GET_ATSC3_L1B                      0x0000017C
#define ATSC3_L1B_SIZE                     0x74 /* Size in Bytes */
#define GET_ATSC3_L1D                      0x000001F0
#define ATSC3_L1D_SIZE                     0xCD0 /* Size in Bytes */  

/* ATSC 1.0 Standard Specific Parameters */
#define SET_ATSC1_BW                       0x00000090
#define SET_ATSC1_BLOCK_SIZE               0x0000009c
#define GET_ATSC1_DECODE_PERFORMANCE       0x000000A0

/* DVB-T Standard Specific Parameters */
#define SET_DVBT_BW                       0x00000090
#define SET_DVBT_CCI_TYPE                 0x00000094
#define GET_DVBT_DECODE_PERFORMANCE       0x000000A0
#define GET_DVBT_L1                       0x000000B0

/* DVB-T2 Standard Specific Parameters */
#define SET_DVBT2_BW                       0x00000090
#define SET_DVBT2_CCI_TYPE                 0x00000094
#define GET_DVBT2_DECODE_PERFORMANCE       0x000000A4
#define GET_DVBT2_L1                       0x0000017C
#define GET_DVBT2_PLP_COMMON               0x000001F0
#define GET_DVBT2_PLP_USER                 0x000001F0  

/* ISDB-T Standard Specific Parameters */
#define SET_ISDBT_BW                       0x00000090
#define SET_ISDBT_CCI_TYPE                 0x00000094
#define SET_ISDBT_SEGMENT_INFO             0x0000009c
#define GET_ISDBT_DECODE_PERFORMANCE       0x000000A0
#define GET_ISDBT_L1                       0x000000B4

/* External Interfaces */
SL_Result_t SL_DemodSendData(int instance, unsigned int len, unsigned char *buf);
SL_Result_t SL_DemodRecieveData(int instance, unsigned int len, unsigned char *buf);
SL_Result_t SL_DemodWriteBytes(int instance, unsigned int address, unsigned int lenBytes, void *buf);
SL_Result_t SL_DemodReadBytes(int instance, unsigned int address, unsigned int lenBytes, void *buf);
#endif
