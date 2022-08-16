/****************************************************************************
* MONTAGE PROPRIETARY AND CONFIDENTIAL
* Montage Technology (Shanghai) Inc.
* All Rights Reserved
* --------------------------------------------------------------------------
*
* File:				mt_fe_def.h
*
* Current version:	6.03.04
*
* Description:
*
* Log:	Description			Version			Date			Author
*		---------------------------------------------------------------------
*		Create				6.00.00			2010.09.13		YZ.Huang
*		Modify				6.01.00			2010.11.19		YZ.Huang
*		Modify				6.01.03			2010.11.30		YZ.Huang
*		Modify				6.02.04			2011.06.17		YZ.Huang
*		Modify				6.02.07			2011.09.15		YZ.Huang
*		Modify				6.02.08			2011.10.10		YZ.Huang
*		Modify				6.02.09			2011.11.25		YZ.Huang
*		Modify				6.03.00			2012.01.09		YZ.Huang
*		Modify				6.03.03			2012.02.20		YZ.Huang
*		Modify				6.03.04			2012.07.31		YZ.Huang
****************************************************************************/
#ifndef __MT_FE_DEF_H__
#define __MT_FE_DEF_H__

/*
#ifdef __cplusplus
extern "C" {
#endif
*/
#include <stdio.h>
#include "IT9300.h"

/*TUNER SUPPORTED TYPE, Please Select The Tuner You Used.*/
#define	MT_FE_TN_SUPPORT_TS2020					0
#define	MT_FE_TN_SUPPORT_TS2022					1
#define	MT_FE_TN_SUPPORT_SHARP6306				0
#define	MT_FE_TN_SUPPORT_SHARP7306				0
#define	MT_FE_TN_SUPPORT_ST6110					0 /*ST6110 chip*/
#define	MT_FE_TN_SUPPORT_ST6110_ON_BOARD		0
#define	MT_FE_TN_SUPPORT_SHARP7803				0
#define	MT_FE_TN_SUPPORT_SHARP7903				0
#define	MT_FE_TN_SUPPORT_AV2011					0
#define	MT_FE_TN_SUPPORT_AV2026					0
#define MT_FE_TN_SUPPORT_RAON_MTV600			0


#if MT_FE_TN_SUPPORT_SHARP7306
	#undef MT_FE_TN_SUPPORT_SHARP6306
	#define MT_FE_TN_SUPPORT_SHARP6306	1
#endif

#if MT_FE_TN_SUPPORT_ST6110_ON_BOARD
	#undef MT_FE_TN_SUPPORT_ST6110
	#define MT_FE_TN_SUPPORT_ST6110	1
#endif


#define MT_FE_DEBUG							0		/*	0 off, 1 on*/
#if (MT_FE_DEBUG == 1)
	#define mt_fe_print(str)				printf str;
	#define mt_fe_assert(bool,msg)
#else
	#define mt_fe_print(str)
	#define mt_fe_assert(bool,msg)
#endif


#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x)		((void)(x))
#endif


/*	VARIABLE TYPE DEFINES*/
#if 1
#define	U8	unsigned char							/* 8bit unsigned					*/
#define	S8	signed char								/* 8bit unsigned					*/
#define	U16	unsigned short							/* 16bit unsigned					*/
#define	S16	signed short							/* 16bit unsigned					*/
#define	U32	unsigned int							/* 32bit unsigned					*/
#define	S32	signed int								/* 16bit unsigned					*/
#define	DB	double
#else
typedef	unsigned char	U8;							/* 8bit unsigned					*/
typedef	unsigned char	S8;							/* 8bit unsigned					*/
typedef	unsigned short	U16;						/* 16bit unsigned					*/
typedef	signed short	S16;						/* 16bit unsigned					*/
typedef	unsigned int	U32;						/* 32bit unsigned					*/
typedef	signed int		S32;						/* 16bit unsigned					*/
typedef	double			DB;
#endif


#ifndef NULL
#define NULL	0
#endif

#ifndef BOOL
#define BOOL	int
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif



typedef enum _MT_FE_BOOL
{
	MtFe_False = 0
	,MtFe_True
} MT_FE_BOOL;

typedef enum _MT_FE_ON_OFF
{
	MtFe_Off = 0
	,MtFe_On
} MT_FE_ON_OFF;

typedef enum _MT_FE_RET
{
	MtFeErr_Ok					= 0
	,MtFeErr_Undef				= -1
	,MtFeErr_Uninit				= -2
	,MtFeErr_Param				= -3
	,MtFeErr_NoSupportFunc		= -4
	,MtFeErr_NoSupportTuner		= -5
	,MtFeErr_NoSupportDemod		= -6
	,MtFeErr_UnLock				= -7
	,MtFeErr_I2cErr				= -8
	,MtFeErr_DiseqcBusy			= -9
	,MtFeErr_NoMemory			= -10
	,MtFeErr_NullPointer		= -11
	,MtFeErr_TimeOut			= -12
	,MtFeErr_Fail				= -13
} MT_FE_RET;

typedef enum _MT_FE_TYPE
{
	MtFeType_Undef = 0
	,MtFeType_DvbC
	,MtFeType_DvbT
	,MtFeType_Dtmb
	,MtFeType_DvbS
	,MtFeType_DvbS2
	,MtFeType_DvbS_Unknown
	,MtFeType_DvbS_Checked
} MT_FE_TYPE;

typedef enum _MT_FE_MOD_MODE
{
	MtFeModMode_Undef = 0
	,MtFeModMode_4Qam
	,MtFeModMode_4QamNr
	,MtFeModMode_16Qam
	,MtFeModMode_32Qam
	,MtFeModMode_64Qam
	,MtFeModMode_128Qam
	,MtFeModMode_256Qam
	,MtFeModMode_Qpsk
	,MtFeModMode_8psk
	,MtFeModMode_16Apsk
	,MtFeModMode_32Apsk
	,MtFeModMode_Auto
} MT_FE_MOD_MODE;


typedef enum _MT_FE_LOCK_STATE
{
	MtFeLockState_Undef = 0
	,MtFeLockState_Unlocked
	,MtFeLockState_Locked
	,MtFeLockState_TunerLocked
	,MtFeLockState_Waiting
} MT_FE_LOCK_STATE;

typedef enum _MT_FE_LNB_VOLTAGE
{
	MtFeLNB_13V = 0
	,MtFeLNB_18V
} MT_FE_LNB_VOLTAGE;


typedef enum _MT_FE_CODE_RATE
{
	MtFeCodeRate_Undef = 0
	,MtFeCodeRate_1_4
	,MtFeCodeRate_1_3
	,MtFeCodeRate_2_5
	,MtFeCodeRate_1_2
	,MtFeCodeRate_3_5
	,MtFeCodeRate_2_3
	,MtFeCodeRate_3_4
	,MtFeCodeRate_4_5
	,MtFeCodeRate_5_6
	,MtFeCodeRate_7_8
	,MtFeCodeRate_8_9
	,MtFeCodeRate_9_10
} MT_FE_CODE_RATE;

typedef enum _MT_FE_ROLL_OFF
{
	MtFeRollOff_Undef = 0
	,MtFeRollOff_0p35
	,MtFeRollOff_0p25
	,MtFeRollOff_0p20
} MT_FE_ROLL_OFF;


typedef enum _MT_FE_SPECTRUM_MODE
{
	MtFeSpectrum_Undef = 0
	,MtFeSpectrum_Normal
	,MtFeSpectrum_Inversion
} MT_FE_SPECTRUM_MODE;


typedef enum _MT_FE_DS3K_CODE_RATE_SELECT
{
	MtFeDs3kCodeRate_None	 = 0
	,MtFeDs3kCodeRate_AllEn  = 0xf8
	,MtFeDs3kCodeRate_1_2_en = 0x08
	,MtFeDs3kCodeRate_2_3_en = 0x10
	,MtFeDs3kCodeRate_3_4_en = 0x20
	,MtFeDs3kCodeRate_5_6_en = 0x40
	,MtFeDs3kCodeRate_6_7_en = 0x80
	,MtFeDs3kCodeRate_7_8_en = 0x80
} MT_FE_DS3K_CODE_RATE_SELECT;


typedef enum _MT_FE_LNB_LOCAL_OSC
{
	MtFeLNB_Single_Local_OSC = 0
	,MtFeLNB_Dual_Local_OSC
} MT_FE_LNB_LOCAL_OSC;


typedef enum _MT_FE_BAND_TYPE
{
	MtFeBand_C = 0
	,MtFeBand_Ku
} MT_FE_BAND_TYPE;


typedef enum _MT_FE_MSG
{
	MtFeMsg_BSTpFind = 0
	,MtFeMsg_BSTpLocked
	,MtFeMsg_BSTpUnlock
	,MtFeMsg_BSStart
	,MtFeMsg_BSFinish
	,MtFeMsg_BSOneWinFinish
	,MtFeMsg_BSAbort
} MT_FE_MSG;

typedef struct _MT_FE_CHAN_INFO_DVBS2
{
	MT_FE_TYPE			type;
	MT_FE_MOD_MODE		mod_mode;
	MT_FE_ROLL_OFF		roll_off;
	MT_FE_CODE_RATE		code_rate;
	MT_FE_BOOL			is_pilot_on;
	MT_FE_SPECTRUM_MODE	is_spectrum_inv;
} MT_FE_CHAN_INFO_DVBS2;

typedef enum _MT_FE_DiSEqC_TONE_BURST
{
	MtFeDiSEqCToneBurst_Moulated = 0
	,MtFeDiSEqCToneBurst_Unmoulated
} MT_FE_DiSEqC_TONE_BURST;


typedef struct _MT_FE_DiSEqC_MSG
{
	U8		data_send[8];
	U8		size_send;
	BOOL	is_enable_receive;
	BOOL	is_envelop_mode;
	U8		data_receive[8];
	U8		size_receive;
} MT_FE_DiSEqC_MSG;

typedef enum _MT_FE_DMD_ID
{
	MtFeDmdId_Undef,
	MtFeDmdId_DS300X,
	MtFeDmdId_DS3002B,
	MtFeDmdId_DS3103,
	MtFeDmdId_Unknown
} MT_FE_DMD_ID;

typedef struct _MT_FE_TP_INFO
{
	U32  freq_KHz;
	U16  sym_rate_KSs;
	MT_FE_TYPE dvb_type;
	MT_FE_CODE_RATE code_rate;
}MT_FE_TP_INFO;

typedef struct _MT_FE_BS_TP_INFO
{
	U8  bs_times;
	U8  bs_algorithm;
	U16	tp_num;
	U16	tp_max_num;
	MT_FE_TP_INFO *p_tp_info;
} MT_FE_BS_TP_INFO;



/* Tuner DEFINES */
#if (MT_FE_TN_SUPPORT_TS2020 > 0)
#define MT_FE_TN_I2C_ADDR	0xc0		/* Tuner address */

#define M88TS2020_RFBYPASS_ON				1 	 /* If you need loop through function, please set it to 1 , else set it to 0 */
#define M88TS2020_CKOUT_ON					1	 /* If you need clock out from tuner to demodulator use function, please set it to 1 , else set it 0*/
#define M88TS2020_CKOUT_DIV					0x01 /* The clock out division, when M88TS2020_CKOUT_ON == 1 ,the  Fckout =  M88TS2020_FXTAL / M88TS2020_CKOUT_DIV  */
												 /* M88TS2020_CKOUT_DIV can set from 0x00 to 0x1f */

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		3000

/* Tuner APIs */
extern MT_FE_RET mt_fe_tn_init_ts2020(void);
extern MT_FE_RET mt_fe_tn_sleep_ts2020(void);
extern MT_FE_RET mt_fe_tn_wake_up_ts2020(void);
extern MT_FE_RET mt_fe_tn_get_strength_ts2020(S8 *p_strength);
extern MT_FE_RET mt_fe_tn_set_freq_ts2020(U32 freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern S32 mt_fe_tn_get_tuner_freq_offset_ts2020(void);

extern MT_FE_RET mt_fe_tn_get_gain_ts2020(U32 *p_gain);

#define	AGC_POLAR			0
#define IQ_INVERTED			0
#endif


#if (MT_FE_TN_SUPPORT_TS2022 > 0)
#define MT_FE_TN_I2C_ADDR	0xc0		/* Tuner address */

#define TUNER_RFBYPASS_ON				1		/* If you need loop through function, please set it to 1 , else set it to 0 */
#define TUNER_CKOUT_ON					0		/* If you need clock out from tuner clkout pin to demodulator use, please set it to 1 , else set it 0*/
#define TUNER_CKOUT_DIV					0x01	/* The clock out division */
#define TUNER_CKOUT_XTAL				1		/* If you need clock out from tuner xtalout pin to demodulator use, please set it to 1 , else set it 0*/

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		3000

/* Tuner APIs */
extern MT_FE_RET mt_fe_tn_init_ts2022(void);
extern MT_FE_RET mt_fe_tn_sleep_ts2022(void);
extern MT_FE_RET mt_fe_tn_wake_up_ts2022(void);
extern MT_FE_RET mt_fe_tn_get_strength_ts2022(S8 *p_strength);
extern MT_FE_RET mt_fe_tn_set_freq_ts2022(U32 freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern S32 mt_fe_tn_get_tuner_freq_offset_ts2022(void);

extern MT_FE_RET mt_fe_tn_get_gain_ts2022(U32 *p_gain);

#define	AGC_POLAR			0
#define IQ_INVERTED			0
#endif




#if (MT_FE_TN_SUPPORT_SHARP6306 > 0)
#define MT_FE_TN_I2C_ADDR	0xc0		/* Tuner address */

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		3000


extern MT_FE_RET mt_fe_tn_init_sharp6306(void);
extern MT_FE_RET mt_fe_tn_get_strength_sharp6306(S8 *p_strength);
extern MT_FE_RET mt_fe_tn_set_freq_sharp6306(U32 freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern S32 mt_fe_tn_get_tuner_freq_offset_sharp6306(void);

extern MT_FE_RET mt_fe_tn_get_gain_sharp6306(U32 * p_gain);

#define	AGC_POLAR			1
#define IQ_INVERTED			0
#endif


#if (MT_FE_TN_SUPPORT_SHARP7803 > 0)
#define MT_FE_TN_I2C_ADDR	0xc0		/* Tuner address */

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		3000


extern MT_FE_RET mt_fe_tn_init_sharp7803(void);
extern MT_FE_RET mt_fe_tn_get_strength_sharp7803(S8 *p_strength);
extern MT_FE_RET mt_fe_tn_set_freq_sharp7803(U32 freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern S32 mt_fe_tn_get_tuner_freq_offset_sharp7803(void);

extern MT_FE_RET mt_fe_tn_get_gain_sharp7803(U32 * p_gain);

#define	AGC_POLAR			0
#define IQ_INVERTED			0
#endif

#if (MT_FE_TN_SUPPORT_SHARP7903 > 0)
#define MT_FE_TN_I2C_ADDR	0xc0		/* Tuner address */

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		3000


extern MT_FE_RET mt_fe_tn_init_sharp7903(void);
extern MT_FE_RET mt_fe_tn_get_strength_sharp7903(S8 *p_strength);
extern MT_FE_RET mt_fe_tn_set_freq_sharp7903(U32 freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern S32 mt_fe_tn_get_tuner_freq_offset_sharp7903(void);

extern MT_FE_RET mt_fe_tn_get_gain_sharp7903(U32 * p_gain);

#define	AGC_POLAR			1
#define IQ_INVERTED			0
#endif


#if (MT_FE_TN_SUPPORT_ST6110 > 0)
#define MT_FE_TN_I2C_ADDR	0xc0		/* Tuner address */

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		3000

/* Tuner APIs */
extern MT_FE_RET mt_fe_tn_init_st6110(void);
extern S32	mt_fe_tn_get_tuner_freq_offset_st6110(void);
extern MT_FE_RET mt_fe_tn_set_freq_st6110(U32 freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern MT_FE_RET mt_fe_tn_get_strength_st6110(S8 *p_strength);

extern MT_FE_RET mt_fe_tn_get_gain_st6110(U32 *p_gain);

#define	AGC_POLAR			0
#define IQ_INVERTED			0
#endif

#if (MT_FE_TN_SUPPORT_AV2011 > 0)
#define MT_FE_TN_I2C_ADDR	0xc6		/* Tuner address */

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		3000

/* Tuner APIs */
extern MT_FE_RET mt_fe_tn_init_AV2011(void);
extern MT_FE_RET mt_fe_tn_set_freq_AV2011(U32 Freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern MT_FE_RET mt_fe_tn_get_strength_AV2011(S8 *p_strength);
extern S32 mt_fe_tn_get_tuner_freq_offset_AV2011(void);

extern MT_FE_RET mt_fe_tn_get_gain_AV2011(U32 *p_gain);

#define	AGC_POLAR			1
#define IQ_INVERTED			0
#endif

#if (MT_FE_TN_SUPPORT_AV2026 > 0)

#define MT_FE_TN_I2C_ADDR	0xc0		/* Tuner address */

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		3000

/* Tuner APIs */
extern MT_FE_RET mt_fe_tn_init_AV2026(void);
extern MT_FE_RET mt_fe_tn_set_freq_AV2026(U32 Freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern MT_FE_RET mt_fe_tn_get_strength_AV2026(S8 *p_strength);
extern S32 mt_fe_tn_get_tuner_freq_offset_AV2026(void);

extern MT_FE_RET mt_fe_tn_get_gain_AV2026(U32 *p_gain);

#define	AGC_POLAR			1
#define IQ_INVERTED			1
#endif


#if (MT_FE_TN_SUPPORT_RAON_MTV600 > 0)
#define MT_FE_TN_I2C_ADDR	0xc2		/* Tuner address */

/* set Frequency Offset to tuner When symbol rate < 5000 KSs */
#define FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz		0

/* Tuner APIs */
extern MT_FE_RET mt_fe_tn_init_raon_mtv600(void);
extern MT_FE_RET mt_fe_tn_set_freq_raon_mtv600(U32 Freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
extern MT_FE_RET mt_fe_tn_get_strength_raon_mtv600(S8 *p_strength);
extern S32 mt_fe_tn_get_tuner_freq_offset_raon_mtv600(void);

extern MT_FE_RET mt_fe_tn_get_gain_raon_mtv600(U32 *p_gain);

#define	AGC_POLAR			0
#define IQ_INVERTED			0
#endif


/* demod&tuner address */
#define MT_FE_DMD_I2C_ADDR_DS3K				0xd0		/* Demodulator address	*/

/* define to enhance performance*/
#define MT_FE_ENHANCE_PERFORMANCE			1


/* AC coupling control*/
#define MT_FE_ENABLE_AC_COUPLING			1		/*1: AC coupling (recommended in reference design) 0: DC coupling*/


/*	CCI	threshold */
#define MT_FE_CCI_THRESHOLD					2

/*	DVB-S2 equalizer coefficient ratio threshold */
#define MT_FE_S2_EQ_COEF_THRESHOLD			5


/*	CLOCK DEFINES*/
#define MT_FE_MCLK_KHZ						96000

#define MT_FE_MCLK_KHZ_SERIAL_S2			144000//96000


// Improve the driver capability or not
#define MT_FE_ENHANCE_MCLK_LEVEL_PARALLEL_CI	0	// Parallel Mode or Common Interface Mode
#define MT_FE_ENHANCE_MCLK_LEVEL_SERIAL			1	// Serial Mode

/* CLOCK OUTPUT TO DECODER*/
#define MT_FE_ENABLE_27MHZ_CLOCK_OUT		0
#define MT_FE_ENABLE_13_P_5_MHZ_CLOCK_OUT	0



/*****************  CHECK CARRIER OFFSET DEFINEs  *********************
**    This define is used to unlock DS300X if the carrier offset
** is larger than the defined carrier offset.
**	  User can set this define to "1" and set the carrier offset
** (4MHz offset as default).
**	  If the carrier offset if larger than the defined value, DS300X
** will be unlocked even if it locked.
**
**********************************************************************/
#define MT_FE_CHECK_CARRIER_OFFSET			1
#if (MT_FE_CHECK_CARRIER_OFFSET != 0)
	#define MT_FE_CARRIER_OFFSET_KHz		4000 /*4MHz carrier offset*/
#endif


/******************** LNB and DISEQC DEFINES ************************/
/*  Maybe user need change the defines according to reference design  */
#define LNB_ENABLE_WHEN_LNB_EN_HIGH			0
#define LNB_13V_WHEN_VSEL_HIGH				1
#define LNB_VSEL_STANDBY_HIGH				1
#define LNB_DISEQC_OUT_FORCE_HIGH			0



/************************ TS OUTPUT DEFINES *****************************/
#define MtFeTsOutMode_Serial		1
#define MtFeTsOutMode_Parallel		2
#define MtFeTsOutMode_Common		3

#define MT_FE_TS_OUTPUT_MODE		MtFeTsOutMode_Common


/* Select TS output pin D0 or D7 for Serial mode only, support DS3002B & DS3103 */
#define MT_FE_TS_SERIAL_PIN_SELECT_D0_D7			0		// 0: D0, 1: D7

#define	MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE		1

#if (MT_FE_TS_OUTPUT_MODE == MtFeTsOutMode_Parallel)
	/***************************************************************
	     In parallel mode, user can select the max clock out frequency
	 according to the decoder's max clock frequency.

	   Four Options:
	   MtFeTSOut_Max_Clock_12_MHz; 		MtFeTSOut_Max_Clock_16_MHz
	   MtFeTSOut_Max_Clock_19_p_2_MHz; 	MtFeTSOut_Max_Clock_24_MHz;

	   Default setting is 24MHz.
	***************************************************************/
	#define MtFeTSOut_Max_Clock_12_MHz		0
	#define MtFeTSOut_Max_Clock_16_MHz		0
	#define MtFeTSOut_Max_Clock_19_p_2_MHz	0
	#define MtFeTSOut_Max_Clock_24_MHz		0
	#define MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE		1

#elif (MT_FE_TS_OUTPUT_MODE == MtFeTsOutMode_Common)

	/***********************  TS Clock Auto Set  ************************
	** MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE == 1  Automatically set TS clock
	**
	**	   TS clock will be automatically set just according to the
	** modulation mode/code rate/symbol rate after TP locked.
	*********************************************************************/
	#define MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE		1

#endif




/**************	Blind Scan DEFINEs *****************/
#define MT_FE_BS_TIMES			2

#define MT_FE_BS_TIMES_1ST		0
#define MT_FE_BS_TIMES_2ND		1
#define MT_FE_BS_TIMES_3RD		2
#define MT_FE_BS_TIMES_ONE_LOOP	4


#define MT_FE_BS_ALGORITHM_A	1
#define MT_FE_BS_ALGORITHM_B	2
#define MT_FE_BS_ALGORITHM_DEF	MT_FE_BS_ALGORITHM_A

/*process scanned TP defines*/
#define BLINDSCAN_LPF_OFFSET_KHz	3000
#define FREQ_MAX_KHz				9999000


/*blind scan register defines*/
#if MT_FE_TN_SUPPORT_SHARP6306

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512
	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	34000

	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64// 80
	#define	ERR_BOUND_FACTOR_ONE_LOOP		36// 44
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		10

#elif MT_FE_TN_SUPPORT_TS2020

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512

	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	40000


	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64
	#define	ERR_BOUND_FACTOR_ONE_LOOP		44
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		11

#elif MT_FE_TN_SUPPORT_TS2022

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512

	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	40000

	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64
	#define	ERR_BOUND_FACTOR_ONE_LOOP		36//52
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		11

#elif MT_FE_TN_SUPPORT_ST6110

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512

	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	36000

	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64
	#define	ERR_BOUND_FACTOR_ONE_LOOP		36//48
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		10
#elif MT_FE_TN_SUPPORT_SHARP7803

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512
	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	34000

	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64// 80
	#define	ERR_BOUND_FACTOR_ONE_LOOP		36// 44
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		10

#elif MT_FE_TN_SUPPORT_SHARP7903

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512
	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	34000

	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64// 80
	#define	ERR_BOUND_FACTOR_ONE_LOOP		36// 44
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		10

#elif MT_FE_TN_SUPPORT_AV2011

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512

	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	40000

	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64
	#define	ERR_BOUND_FACTOR_ONE_LOOP		36//52
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		11

#elif MT_FE_TN_SUPPORT_AV2026

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512

	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	40000

	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64
	#define	ERR_BOUND_FACTOR_ONE_LOOP		36//52
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		11

#elif MT_FE_TN_SUPPORT_RAON_MTV600

	#define	FFT_LENGTH		512
	#define	PSD_OVERLAP		96
	#define	AVE_TIMES		256
	#define	DATA_LENGTH		512

	#define	NOTCH_RANGE_F	8
	#define	START_RANGE_F	15
	#define	FIND_NOTCH_STEP	1
	#define	PSD_SCALE		0
	#define	SM_BUFDEP		8

	#define	BLINDSCAN_SYMRATEKSs	40000

	#define	ONE_LOOP_SM_BUF					4
	#define	SNR_THR_ONE_LOOP				64
	#define	ERR_BOUND_FACTOR_ONE_LOOP		36//52
	#define	FLAT_THR_ONE_LOOP				28
	#define	THR_FACTOR_ONE_LOOP				5

	#define	TWO_LOOP_SM_BUF_1ST				24
	#define	TWO_LOOP_SNR_THR_1ST			96
	#define	TWO_LOOP_ERR_BOUND_FACTOR_1ST	56
	#define	TWO_LOOP_FLAT_THR_1ST			24
	#define	TWO_LOOP_THR_FACTOR_1ST			1

	#define	TWO_LOOP_SM_BUF_2ND				2
	#define	TWO_LOOP_SNR_THR_2ND			64
	#define	TWO_LOOP_ERR_BOUND_FACTOR_2ND	36
	#define	TWO_LOOP_FLAT_THR_2ND			28
	#define	TWO_LOOP_THR_FACTOR_2ND			5

	#define	TUNER_SLIP_STEP		11

#endif

#define	THR_FACTOR_1ST			TWO_LOOP_THR_FACTOR_1ST			//1
#define	THR_FACTOR_2ND			THR_FACTOR_ONE_LOOP				//5
#define	THR_FACTOR_3RD			TWO_LOOP_THR_FACTOR_2ND			//10

#define	ERR_BOUND_FACTOR_1ST	TWO_LOOP_ERR_BOUND_FACTOR_1ST	//56
#define	ERR_BOUND_FACTOR_2ND	ERR_BOUND_FACTOR_ONE_LOOP		//40
#define	ERR_BOUND_FACTOR_3RD	TWO_LOOP_ERR_BOUND_FACTOR_2ND	//40

#define	FLAT_THR_1ST			TWO_LOOP_FLAT_THR_1ST			//24
#define	FLAT_THR_2ND			FLAT_THR_ONE_LOOP				//24
#define	FLAT_THR_3RD			TWO_LOOP_FLAT_THR_2ND			//24

#define	SNR_THR_1ST				TWO_LOOP_SNR_THR_1ST			//96
#define	SNR_THR_2ND				SNR_THR_ONE_LOOP				//80
#define	SNR_THR_3RD				TWO_LOOP_SNR_THR_2ND			//64

#define	SM_BUF_1ST				TWO_LOOP_SM_BUF_1ST				//14
#define	SM_BUF_2ND				ONE_LOOP_SM_BUF					//10
#define	SM_BUF_3RD				TWO_LOOP_SM_BUF_2ND				//4



#define TMP1   ((AVE_TIMES/32 - 1)*16 + (DATA_LENGTH/128 - 1))
#define TMP2   (64 + PSD_OVERLAP/16)
#define TMP3   (PSD_SCALE*32)
#define TMP4   (NOTCH_RANGE_F*16 + START_RANGE_F)


/* Demodulator APIs */
MT_FE_RET mt_fe_dmd_ds3k_init(Endeavour* endeavour, U8 endeavourChipidx, U8 i2cbus);
MT_FE_RET mt_fe_dmd_ds3k_soft_reset(void);
MT_FE_RET mt_fe_dmd_ds3k_global_reset(void);
MT_FE_RET mt_fe_dmd_ds3k_get_demod_id(MT_FE_DMD_ID *dmd_id);
MT_FE_RET mt_fe_dmd_ds3k_hard_reset(void);
MT_FE_RET mt_fe_dmd_ds3k_sleep(void);
MT_FE_RET mt_fe_dmd_ds3k_wake_up(void);
MT_FE_RET mt_fe_dmd_ds3k_get_driver_version(U32* p_version);

MT_FE_RET mt_fe_dmd_ds3k_change_ts_out_mode(U8 mode);

MT_FE_RET mt_fe_dmd_ds3k_connect(U32 freq_MHz, U32 sym_rate_KSs, MT_FE_TYPE dvbs_type);
MT_FE_RET mt_fe_dmd_ds3k_get_snr(S8 *p_snr);
MT_FE_RET mt_fe_dmd_ds3k_get_strength(S8 *p_strength);
MT_FE_RET mt_fe_dmd_ds3k_get_lock_state(MT_FE_LOCK_STATE *p_state);
MT_FE_RET mt_fe_dmd_ds3k_get_per(U32 *p_total_packags, U32 *p_err_packags);
MT_FE_RET mt_fe_dmd_ds3k_get_channel_info(MT_FE_CHAN_INFO_DVBS2 *p_info);

MT_FE_RET mt_fe_ds3k_get_quality(MT_FE_TYPE dtv_type, U32* quality);

MT_FE_RET mt_fe_dmd_ds3k_set_LNB(MT_FE_BOOL is_LNB_enable, MT_FE_BOOL is_22k_enable, MT_FE_LNB_VOLTAGE voltage_type, MT_FE_BOOL is_envelop_mode);
MT_FE_RET mt_fe_dmd_ds3k_DiSEqC_send_tone_burst(MT_FE_DiSEqC_TONE_BURST mode, MT_FE_BOOL is_envelop_mode);
MT_FE_RET mt_fe_dmd_ds3k_DiSEqC_send_receive_msg(MT_FE_DiSEqC_MSG *msg);

MT_FE_RET mt_fe_dmd_ds3k_blindscan_abort(MT_FE_BOOL bs_abort);
MT_FE_RET mt_fe_dmd_ds3k_register_notify(void (*callback)(MT_FE_MSG msg, void *p_tp_info));
MT_FE_RET mt_fe_dmd_ds3k_blindscan(U32 begin_freq_MHz, U32 end_freq_MHz, MT_FE_BS_TP_INFO *p_bs_info);


typedef struct _MT_FE_UNICABLE_DEVICE
{
	U32		freq_MHz;
	BOOL	UB_ok;

	U32		result_MHz;
	U32		result_PSD;
	U32		result_gain;
} MT_FE_UNICABLE_DEVICE;

void mt_fe_dmd_ds3k_unicable_blind_detect(U16 start_freq_MHz, U16 stop_freq_MHz, MT_FE_UNICABLE_DEVICE *p_ub_list, BOOL need_init);

MT_FE_RET mt_fe_unicable_set_tuner(U32 freq_KHz, U32 symbol_rate_KSs, U32* real_freq_KHz, U8 ub_select, U8 bank_select, U32 ub_freq_MHz);

/*
#ifdef __cplusplus
}
#endif
*/

#endif /* __MT_FE_DEF_H__ */
