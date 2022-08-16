/***************************************************************************************
*	Copyright (c) 2010, MONTAGE PROPRIETARY AND CONFIDENTIAL
*	Montage Technology (Shanghai) Inc.
*	All Rights Reserved
*	------------------------------------
*
*	FILE:				mt_fe_dmd_ds3000.c
*
*	VERSION:			6.03.04
*
*	DESCRIPTION:
*			Define the interfaces to initialize and get the channel info from DS3103/DS3002B/DS300X.
*
*	History:
*	Description		Version		Date			Author
*---------------------------------------------------------------------------
*	File Create		6.00.00		2010.09.13		YZ.Huang
*	Modify			6.01.00		2010.11.19		YZ.Huang
*	Modify			6.01.03		2010.11.30		YZ.Huang
*	Modify			6.01.04		2010.12.02		YZ.Huang
*	Modify			6.01.05		2010.12.09		YZ.Huang
*	Modify			6.01.06		2010.12.29		YZ.Huang
*	Modify			6.02.00		2011.01.21		YZ.Huang
*	Modify			6.02.02		2011.04.21		BJ.Wang
*	Modify			6.02.04		2011.06.17		YZ.Huang
*	Modify			6.02.05		2011.06.22		YZ.Huang
*	Modify			6.02.06		2011.08.04		YZ.Huang
*	Modify			6.02.07		2011.09.21		YZ.Huang
*	Modify			6.02.08		2011.10.11		YZ.Huang
*	Modify			6.02.09		2011.11.10		YZ.Huang
*	Modify			6.02.09		2011.12.05		YZ.Huang
*	Modify			6.03.00		2011.01.10		BJ.Wang & YZ.Huang
*	Modify			6.03.01		2012.02.06		YZ.Huang
*	Modify			6.03.03		2012.02.20		YZ.Huang
*	Modify			6.03.04		2012.08.09		YZ.Huang
***************************************************************************************/


#include "mt_fe_def.h"
#include "mt_fe_i2c.h"
#include "mt_fe_dmd_ds3000_fw.h"
#include "mt_fe_dmd_ds3000_reg_tbl.h"

#include <math.h>



/************************
****external function****
*************************/
extern void _mt_sleep(U32 ms);



/************************
****internal function****
*************************/
MT_FE_RET _mt_fe_dmd_ds3k_set_demod(U32 sym_rate_KSs, MT_FE_TYPE type);
MT_FE_RET _mt_fe_dmd_ds3k_init_reg(const U8 (*p_reg_tabl)[2], S32 size);
MT_FE_RET _mt_fe_dmd_ds3k_download_fw(const U8* p_fw);
MT_FE_RET _mt_fe_dmd_ds3k_set_ts_out_mode(U8 mode);
MT_FE_RET _mt_fe_dmd_ds3k_set_CCI(MT_FE_ON_OFF is_cci_on);
MT_FE_RET _mt_fe_dmd_ds3k_check_CCI(MT_FE_LOCK_STATE *p_state);
MT_FE_RET _mt_fe_dmd_ds3k_set_clock_ratio(void);
MT_FE_RET _mt_fe_dmd_ds3k_set_sym_rate(U32 sym_rate_KSs);
MT_FE_RET _mt_fe_dmd_ds3k_get_sym_rate(U32 *sym_rate_KSs);
MT_FE_RET _mt_fe_dmd_ds3k_set_carrier_offset(S32 carrier_offset_KHz);
MT_FE_RET _mt_fe_dmd_ds3k_get_carrier_offset(S32 *carrier_offset_KHz);
MT_FE_RET _mt_fe_dmd_ds3k_get_total_carrier_offset(S32 *carrier_offset_KHz);
MT_FE_RET _mt_fe_dmd_ds3k_get_fec(MT_FE_TP_INFO *p_info, MT_FE_TYPE tp_type);

MT_FE_RET _mt_fe_dmd_ds3k_get_mclk(U32 *p_MCLK_KHz);
MT_FE_RET _mt_fe_dmd_ds3k_set_mclk(U32 MCLK_KHz);
MT_FE_RET _mt_fe_dmd_ds3k_set_ts_divide_ratio(MT_FE_TYPE type, U8 dr_high, U8 dr_low);
MT_FE_RET _mt_fe_dmd_ds3k_get_current_ts_mode(U8 *p_ts_mode);


/* function pointer */
S32 (*mt_fe_tn_get_offset)(void);
MT_FE_RET (*mt_fe_tn_set_tuner)(U32 freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz);
MT_FE_RET (*mt_fe_tn_cal_offset)(U32 freqKHz, U32 *realfreqKHz);
MT_FE_RET (*mt_fe_tn_sleep)(void);
MT_FE_RET (*mt_fe_tn_wake_up)(void);
MT_FE_RET (*mt_fe_tn_get_gain)(U32 *p_gain);



/* function for blind scan*/
MT_FE_RET _mt_fe_dmd_ds3k_bs_set_reg(U8 bs_times);
MT_FE_RET _mt_fe_dmd_ds3k_bs_set_demod(U32 sym_rate_KSs, MT_FE_TYPE type);
MT_FE_RET _mt_fe_dmd_ds3k_bs_remove_unlocked_TP(MT_FE_BS_TP_INFO *p_bs_info, U32 compare_freq_KHz);
MT_FE_RET _mt_fe_dmd_ds3k_bs_process_scanned_TP(MT_FE_BS_TP_INFO *p_bs_info,
													   U32 cur_scan_freq_KHz,
													   U8  bs_times);
MT_FE_RET _mt_fe_dmd_ds3k_bs_save_scanned_TP(MT_FE_BS_TP_INFO *p_bs_info,
													U32 freq_KHz,
													U32 symbol_rate_KSs,
													U8  bs_times);
MT_FE_RET _mt_fe_dmd_ds3k_bs_TP_scan(U32 start_freq_KHz,
									 U32 end_freq_KHz,
									 MT_FE_BS_TP_INFO *p_bs_info,
									 U16 *scanned_tp,
									 U8 bs_times);
MT_FE_RET _mt_fe_dmd_ds3k_bs_A(U32 start_freq_KHz,
							   U32 end_freq_KHz,
							   MT_FE_BS_TP_INFO *p_bs_info,
							   U8 bs_times);
MT_FE_RET _mt_fe_dmd_ds3k_bs_B(U32 start_freq_KHz,
							   U32 end_freq_KHz,
							   MT_FE_BS_TP_INFO *p_bs_info,
							   U8 bs_times);
MT_FE_RET _mt_fe_dmd_ds3k_bs_connect(MT_FE_BS_TP_INFO *p_bs_info,
									 U16 start_index,
									 U16 scanned_tp);


void mt_fe_dummy_function(MT_FE_MSG msg, void* p_param)
{
	UNUSED_PARAMETER(msg);
	UNUSED_PARAMETER(p_param);
}
void (*mt_fe_bs_notify)(MT_FE_MSG msg, void *p_tp_info) = mt_fe_dummy_function;


/**********************
****global variable****
***********************/
/*TP scan*/
U32	g_connect_sym_rate			 = 0;
S32	g_carrier_offset			 = 0;
U32	g_auto_tune_cnt				 = 0;
U32	g_cci_detect				 = 0;
MT_FE_TYPE	g_current_type		 = MtFeType_DvbS;
MT_FE_TYPE	g_connect_type		 = MtFeType_DvbS;
#if ((MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE != 0) || (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0))
	U32 g_check_TS_clock	= 0;
#endif
U32 g_serial_mclk				 = 96000;

U32	g_eq_coef_check				 = 0;

/*blind scan*/
U8	last_tpA_flag				 = 0;
U16	g_tp_index					 = 0;
U16	g_locked_tp_cnt				 = 0;
U16	g_scanned_tp_cnt			 = 0;
U32	g_cur_compare_TPA_KHz		 = 0;
MT_FE_BOOL	g_blindscan_cancel	 = MtFe_False;
U32	g_bs_time_total				 = 0;


MT_FE_DMD_ID g_dmd_id			 = MtFeDmdId_Undef;
U8 agc_polarization				 = AGC_POLAR;
U8 iq_inverted_status			 = IQ_INVERTED;

/* for Unicable use */
U8	use_unicable_device			 = 0;
U8	spectrum_inverted			 = 0;

U8	g_bank_select				 = 1;		/*Bank select, 0~7*/
U8	g_ub_select					 = 0;		/*User band select, 0~7*/
U16	g_ub_freq_MHz				 = 1210;	/*User band center freq MHz*/

U16	g_ub_count					 = 0;		/*Valid UB number*/


U32 g_first_TP_found			 = 0;
U32 g_first_TP_locked			 = 0;
U32 g_first_TP_freq_KHz			 = 0;
U32 g_first_TP_symbol_rate_KSs	 = 0;
U32 g_flattern_thr_val			 = 0;
U32	g_window_center_freq		 = 0;
U32	g_window_width				 = 60000;



/************************************************************************
** Function: mt_fe_dmd_ds3k_get_driver_version
**
**
** Description:	This function is used to get the driver version in number
**				mode.
**
**
** Inputs:   None
**
**
** Outputs:
**
**	  Parameter			Type		Description
**	----------------------------------------------------------------------
**	 p_version			U32*		version number pointer
**
*************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_get_driver_version(U32* p_version)
{
	*p_version = 60304;		/* driver version number */

	return MtFeErr_Ok;
}


/************************************************************************
** Function: mt_fe_dmd_ds3k_soft_reset
**
**
** Description:	This function is used to do software reset.
**
**
** Inputs:   None
**
**
** Outputs:  None
**
*************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_soft_reset(void)
{

	g_cci_detect		 = 1;
	g_auto_tune_cnt		 = 0;
	g_eq_coef_check		 = 0;

	_mt_fe_dmd_set_reg(0xb2, 0x01);
	if(g_dmd_id == MtFeDmdId_DS300X)
	{
		_mt_sleep(1);
	}
	else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_set_reg(0x00, 0x01);
		_mt_sleep(1);
		_mt_fe_dmd_set_reg(0x00, 0x00);
	}
	else
	{
		return MtFeErr_NoSupportDemod;
	}
	_mt_fe_dmd_set_reg(0xb2, 0x00);


	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dmd_ds3k_global_reset(void)
{
	_mt_fe_dmd_set_reg(0x07, 0x80);
	_mt_fe_dmd_set_reg(0x07, 0x00);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_dmd_ds3k_get_demod_id(MT_FE_DMD_ID *dmd_id)
{
	U8 val_01, val_02, val_b2;

	*dmd_id = MtFeDmdId_Undef;

	_mt_fe_dmd_get_reg(0x01, &val_01);
	_mt_fe_dmd_get_reg(0x02, &val_02);
	_mt_fe_dmd_get_reg(0xb2, &val_b2);

	if((val_02 == 0x00) && (val_01 == 0xC0))
	{
		*dmd_id = MtFeDmdId_DS300X;
		mt_fe_print(("\tChip ID = [DS300X]!\n"));
	}
	else if((val_02 == 0x00) && (val_01 == 0xD0) && ((val_b2 & 0xC0) == 0x00))
	{
		*dmd_id = MtFeDmdId_DS3002B;
		mt_fe_print(("\tChip ID = [DS3002B]!\n"));
	}
	else if((val_02 == 0x00) && (val_01 == 0xD0) && ((val_b2 & 0xC0) == 0xC0))
	{
		*dmd_id = MtFeDmdId_DS3103;
		mt_fe_print(("\tChip ID = [DS3103]!\n"));
	}
	else
	{
		*dmd_id = MtFeDmdId_Unknown;
	}


	return MtFeErr_Ok;
}

/******************************************************************
** Function: mt_fe_dmd_ds3k_hard_reset
**
**
** Description:	This function is used to do chip reset by GPIO pin
**
**
** Inputs:   None
**
**
** Outputs:  None
**
*******************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_hard_reset(void)
{
	U8 val;

	#if (MT_FE_CHECK_CARRIER_OFFSET != 0)
		U8 val_regb2;
	#endif

	/*
		TODO:
			 do the hardware reset by the GPIO pin.
	*/

	mt_fe_dmd_ds3k_get_demod_id(&g_dmd_id);

	#if (MT_FE_CHECK_CARRIER_OFFSET != 0)
	if(g_dmd_id == MtFeDmdId_DS300X)
	{
		_mt_fe_dmd_get_reg(0xb2, &val_regb2);
		if(val_regb2 == 0x01)
		{
			_mt_fe_dmd_set_reg(0x05, 0x00);
			_mt_fe_dmd_set_reg(0xb2, 0x00);
		}
	}
	else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_get_reg(0xb2, &val_regb2);
		if(val_regb2 == 0x01)
		{
			_mt_fe_dmd_set_reg(0x00, 0x00);
			_mt_fe_dmd_set_reg(0xb2, 0x00);
		}
	}
	else
	{
		return MtFeErr_NoSupportDemod;
	}
	#endif

	mt_fe_dmd_ds3k_global_reset();

	_mt_sleep(1);

	_mt_fe_dmd_get_reg(0x08, &val);
	val |= 0x01;
	_mt_fe_dmd_set_reg(0x08, val);


	/* reset global defines*/
	g_current_type		 = MtFeType_DvbS;
	g_connect_type		 = MtFeType_DvbS;
	g_connect_sym_rate	 = 0;
	g_auto_tune_cnt		 = 0;
	g_cci_detect		 = 0;
	g_eq_coef_check		 = 0;

	#if ((MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE != 0) || (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0))
		g_check_TS_clock = 0;
	#endif

	_mt_sleep(1);

	return MtFeErr_Ok;
}


MT_FE_RET mt_fe_dmd_ds3k_set_high_Z_state(BOOL high_Z_output)
{
	U8 reg_0x27;

	_mt_fe_dmd_get_reg(0x27, &reg_0x27);

	if(high_Z_output == FALSE)		// TS output normal state
	{
		reg_0x27 |= 0x01;
	}
	else							// TS output high Z state
	{
		reg_0x27 &= ~0x01;
	}

	_mt_fe_dmd_set_reg(0x27, reg_0x27);

	return MtFeErr_Ok;
}


/******************************************************************************
** Function: mt_fe_dmd_ds3k_init
**
**
** Description:    This function is used to initial DS3103/DS3002B/DS300X. Only called
**				once after power on.
**
**
** Note:
**
**		You can change the TS output mode with the define "MT_FE_TS_OUPUT_MODE"
**	in "mt_fe_def.h"
**
**
** Inputs:   None
**
** Outputs:  None
**
*******************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_init(Endeavour* endeavour, U8 endeavourChipidx, U8 i2cbus)
{
	U8 val;

	_mt_fe_dmd_set_endeavour(endeavour, endeavourChipidx, i2cbus);

	mt_fe_dmd_ds3k_hard_reset();

#if MT_FE_TN_SUPPORT_TS2020
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_ts2020;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_ts2020;
	mt_fe_tn_sleep		 = mt_fe_tn_sleep_ts2020;
	mt_fe_tn_wake_up	 = mt_fe_tn_wake_up_ts2020;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_ts2020;

	mt_fe_tn_init_ts2020();
#elif MT_FE_TN_SUPPORT_TS2022
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_ts2022;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_ts2022;
	mt_fe_tn_sleep		 = mt_fe_tn_sleep_ts2022;
	mt_fe_tn_wake_up	 = mt_fe_tn_wake_up_ts2022;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_ts2022;

	mt_fe_tn_init_ts2022();
#elif MT_FE_TN_SUPPORT_SHARP6306
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_sharp6306;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_sharp6306;
	mt_fe_tn_sleep		 = NULL;
	mt_fe_tn_wake_up	 = NULL;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_sharp6306;

	mt_fe_tn_init_sharp6306();
#elif MT_FE_TN_SUPPORT_SHARP7803
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_sharp7803;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_sharp7803;
	mt_fe_tn_sleep		 = NULL;
	mt_fe_tn_wake_up	 = NULL;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_sharp7803;

	mt_fe_tn_init_sharp7803();
#elif MT_FE_TN_SUPPORT_SHARP7903
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_sharp7903;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_sharp7903;
	mt_fe_tn_sleep		 = NULL;
	mt_fe_tn_wake_up	 = NULL;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_sharp7903;

	mt_fe_tn_init_sharp7903();
#elif MT_FE_TN_SUPPORT_ST6110
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_st6110;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_st6110;
	mt_fe_tn_sleep		 = NULL;
	mt_fe_tn_wake_up	 = NULL;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_st6110;

	mt_fe_tn_init_st6110();
#elif MT_FE_TN_SUPPORT_AV2011
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_AV2011;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_AV2011;
	mt_fe_tn_sleep		 = NULL;
	mt_fe_tn_wake_up	 = NULL;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_AV2011;

	mt_fe_tn_init_AV2011();
#elif MT_FE_TN_SUPPORT_AV2026
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_AV2026;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_AV2026;
	mt_fe_tn_sleep		 = NULL;
	mt_fe_tn_wake_up	 = NULL;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_AV2026;

	mt_fe_tn_init_AV2026();
#elif MT_FE_TN_SUPPORT_RAON_MTV600
	mt_fe_tn_set_tuner	 = mt_fe_tn_set_freq_raon_mtv600;
	mt_fe_tn_get_offset	 = mt_fe_tn_get_tuner_freq_offset_raon_mtv600;
	mt_fe_tn_sleep		 = NULL;
	mt_fe_tn_wake_up	 = NULL;

	mt_fe_tn_get_gain	 = mt_fe_tn_get_gain_raon_mtv600;

	mt_fe_tn_init_raon_mtv600();
#else
	mt_fe_tn_set_tuner	 = NULL;
	mt_fe_tn_get_offset	 = NULL;
	mt_fe_tn_sleep		 = NULL;
	mt_fe_tn_wake_up	 = NULL;

	mt_fe_tn_get_gain	 = NULL;
#endif

	spectrum_inverted = (U8)(iq_inverted_status ^ use_unicable_device);		// XOR

	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_set_reg(0x07, 0xE0);		// global reset, global diseqc reset, golbal fec reset
		_mt_fe_dmd_set_reg(0x07, 0x00);

		_mt_fe_dmd_ds3k_download_fw(m88ds310x_fm);
	}
	else if(g_dmd_id == MtFeDmdId_DS300X)
	{
		_mt_fe_dmd_ds3k_download_fw(m88ds300x_fm);
	}
	else
	{
		return MtFeErr_NoSupportDemod;
	}

	_mt_fe_dmd_ds3k_set_ts_out_mode(MT_FE_TS_OUTPUT_MODE);


	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		if(spectrum_inverted == 1)			// 0x4d, bit 1
		{
			_mt_fe_dmd_get_reg(0x4d, &val);
			val |= 0x02;
			_mt_fe_dmd_set_reg(0x4d, val);
		}
		else
		{
			_mt_fe_dmd_get_reg(0x4d, &val);
			val &= ~0x02;
			_mt_fe_dmd_set_reg(0x4d, val);
		}

		if(agc_polarization == 1)			// 0x30, bit 4
		{
			_mt_fe_dmd_get_reg(0x30, &val);
			val |= 0x10;
			_mt_fe_dmd_set_reg(0x30, val);
		}
		else
		{
			_mt_fe_dmd_get_reg(0x30, &val);
			val &= ~0x10;
			_mt_fe_dmd_set_reg(0x30, val);
		}

		_mt_fe_dmd_set_reg(0xf1, 0x01);
	}

	return MtFeErr_Ok;
}


/************************************************************************
** Function: mt_fe_dmd_ds3k_change_ts_out_mode
**
**
** Description:	This function is used to change the TS output mode.
**
**
** Inputs:
**	Parameter				Type		Description
**	----------------------------------------------------------
**	mode					U8			passed by up level
**
**	MtFeTsOutMode_Serial	1			Serial
**	MtFeTsOutMode_Parallel	2			Parallel
**	MtFeTsOutMode_Common	3			Common Interface
**
**
** Outputs:  None
**
*************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_change_ts_out_mode(U8 mode)
{
	U8 tmp, tmp3 = 0, tmp4 = 0;

	MT_FE_LOCK_STATE lockstate = MtFeLockState_Unlocked;

	U32 locked_sym_rate;

	U32 target_mclk = MT_FE_MCLK_KHZ, cur_mclk = MT_FE_MCLK_KHZ;
	U32 ts_clk = 24000;
	U16 divide_ratio = 0;

	if((g_dmd_id != MtFeDmdId_DS3002B) && (g_dmd_id != MtFeDmdId_DS3103))
	{
		return MtFeErr_NoSupportDemod;
	}

	_mt_fe_dmd_get_reg(0xfd, &tmp);
	if (mode == MtFeTsOutMode_Parallel)
	{
		tmp &= ~0x01;
		tmp &= ~0x04;
	}
	else if (mode == MtFeTsOutMode_Serial)
	{
		tmp &= ~0x01;
		tmp |= 0x04;
	}
	else
	{
		tmp |= 0x01;
		tmp &= ~0x04;
	}
	_mt_fe_dmd_set_reg(0xfd, tmp);


	//mt_fe_dmd_ds3k_get_lock_state(&lockstate);
	if(g_current_type == MtFeType_DvbS2)
	{
		_mt_fe_dmd_get_reg(0x0d, &tmp);
		if ((tmp & 0x8f) == 0x8f)
		{
			lockstate = MtFeLockState_Locked;
		}
	}
	else	// DVB-S
	{
		_mt_fe_dmd_get_reg(0xd1, &tmp);

		if ((tmp & 0x07) == 0x07)
		{
			_mt_fe_dmd_get_reg(0x0d, &tmp);
			if((tmp & 0x07) == 0x07)
				lockstate = MtFeLockState_Locked;
		}
	}

	_mt_fe_dmd_ds3k_get_mclk(&cur_mclk);

	if((mode == MtFeTsOutMode_Parallel) || (mode == MtFeTsOutMode_Common))
	{
		if(g_current_type == MtFeType_DvbS2)
		{
			if(lockstate == MtFeLockState_Locked)
			{
				_mt_fe_dmd_ds3k_get_sym_rate(&locked_sym_rate);

				if(locked_sym_rate > 28000)
				{
					target_mclk = 192000;
				}
				else if(locked_sym_rate > 18000)
				{
					target_mclk = 144000;
				}
				else
				{
					target_mclk = 96000;
				}
			}
			else		// Unlock
			{
				if(g_connect_sym_rate > 28000)		// last connect symbol rate
				{
					target_mclk = 192000;
				}
				else if(g_connect_sym_rate > 18000)
				{
					target_mclk = 144000;
				}
				else	// include last connect symbol rate = 0
				{
					target_mclk = 96000;
				}
			}
		}
		else	// DVB-S
		{
			target_mclk = 96000;
		}
	}
	else		// Serial mode
	{
		if(g_current_type == MtFeType_DvbS2)		// DVB-S2
		{
			#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0)
			target_mclk = g_serial_mclk;
			#else
			target_mclk = MT_FE_MCLK_KHZ_SERIAL_S2;
			#endif
		}
		else		// DVB-S
		{
			#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0)
			target_mclk = g_serial_mclk;
			#else
			target_mclk = 96000;
			#endif
		}
	}


	if(cur_mclk != target_mclk)
	{
		_mt_fe_dmd_set_reg(0x06, 0xe0);
		_mt_fe_dmd_ds3k_set_mclk(target_mclk);
		_mt_fe_dmd_set_reg(0x06, 0x00);
	}

	if((mode == MtFeTsOutMode_Parallel) || (mode == MtFeTsOutMode_Common))
	{	// Locked && (Parallel mode || CI mode)
		#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE != 0)
		if(lockstate == MtFeLockState_Locked)
		{
			_mt_fe_dmd_ds3k_set_clock_ratio();
		}
		#else
		if(g_current_type == MtFeType_DvbS2)
		{
			if(mode == MtFeTsOutMode_Parallel)
			{
				#if (MtFeTSOut_Max_Clock_12_MHz != 0)
				ts_clk = 12000;
				#elif (MtFeTSOut_Max_Clock_16_MHz != 0)
				ts_clk = 16000;
				#elif (MtFeTSOut_Max_Clock_19_p_2_MHz != 0)
				ts_clk = 19200;
				#elif (MtFeTSOut_Max_Clock_24_MHz != 0)
				ts_clk = 24000;
				#else
				ts_clk = 24000;
				#endif
			}
			else	// CI
			{
				ts_clk = 6000;//8471
			}
		}
		else		// DVB-S
		{
			if(mode == MtFeTsOutMode_Parallel)
			{
				#if (MtFeTSOut_Max_Clock_12_MHz)
				ts_clk = 12000;
				#elif (MtFeTSOut_Max_Clock_16_MHz)
				ts_clk = 16000;
				#elif (MtFeTSOut_Max_Clock_19_p_2_MHz)
				ts_clk = 19200;
				#elif (MtFeTSOut_Max_Clock_24_MHz)
				ts_clk = 24000;
				#else
				ts_clk = 24000;
				#endif
			}
			else	// CI
			{
				ts_clk = 6000;//8000
			}
		}

		if(ts_clk != 0)
		{
			divide_ratio = (target_mclk + ts_clk - 1) / ts_clk;

			if(divide_ratio > 128)
				divide_ratio = 128;

			if(divide_ratio < 2)
				divide_ratio = 2;

			tmp3 = (U8)(divide_ratio / 2);
			tmp4 = (U8)(divide_ratio / 2);

			if((divide_ratio % 2) != 0)
				tmp4 += 1;
		}
		else
		{
			divide_ratio = 0;
			tmp3 = 0;
			tmp4 = 0;
		}

		//if((tmp3 + tmp4) != 0)
		//{
		//	ts_clk = target_mclk / (tmp3 + tmp4);
		//}

		_mt_fe_dmd_ds3k_set_ts_divide_ratio(g_current_type, tmp3, tmp4);
		#endif
	}
	else	// Serial
	{
		#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0)
		if(lockstate == MtFeLockState_Locked)
		{
			_mt_fe_dmd_ds3k_set_clock_ratio();
		}
		#endif
	}

	_mt_fe_dmd_get_reg(0x29, &tmp);
	if((MT_FE_TS_SERIAL_PIN_SELECT_D0_D7 != 0) && (mode == MtFeTsOutMode_Serial))
	{
		tmp |= 0x20;
	}
	else
	{
		tmp &= ~0x20;
	}
	_mt_fe_dmd_set_reg(0x29, tmp);


	//mt_fe_dmd_ds3k_soft_reset();


	return MtFeErr_Ok;
}

/************************************************************************
** Function: mt_fe_dmd_ds3k_sleep
**
**
** Description:	This function is used to sleep ds300x.
**
**
** Inputs:   None
**
**
** Outputs:  None
**
*************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_sleep(void)
{
	U8 val;


	if(mt_fe_tn_sleep != NULL)
		mt_fe_tn_sleep();

	_mt_fe_dmd_get_reg(0x08, &val);
	val &= ~0x01;
	_mt_fe_dmd_set_reg(0x08, val);

	_mt_fe_dmd_get_reg(0x04, &val);
	val |= 0x01;
	_mt_fe_dmd_set_reg(0x04, val);

	_mt_fe_dmd_get_reg(0x23, &val);
	val |= 0x10;
	_mt_fe_dmd_set_reg(0x23, val);


	return MtFeErr_Ok;
}



/************************************************************************
** Function: mt_fe_dmd_ds3k_wake_up
**
**
** Description:	This function is used to wake up ds300x.
**
**
** Inputs:   None
**
**
** Outputs:  None
**
*************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_wake_up(void)
{
	U8 val;

	_mt_fe_dmd_get_reg(0x08, &val);
	val |= 0x01;
	_mt_fe_dmd_set_reg(0x08, val);

	_mt_fe_dmd_get_reg(0x04, &val);
	val &= ~0x01;
	_mt_fe_dmd_set_reg(0x04, val);


	_mt_fe_dmd_get_reg(0x23, &val);
	val &= ~0x10;
	_mt_fe_dmd_set_reg(0x23, val);


	if(mt_fe_tn_wake_up != NULL)
		mt_fe_tn_wake_up();

	return MtFeErr_Ok;
}



/**********************************************************************************************
** Function: mt_fe_dmd_ds3k_connect
**
**
** Description:	This function is used to lock the specified TP.
**
**
** Inputs:
**
**		Parameter		Type			Description
**		----------------------------------------------------------------------
**		freq_MHz		U32			TP's frequency(MHz) passed by app level
**		sym_rate_KSs	U32			TP's symbol rate(KSs) passed by app level
**		dvbs_type		MT_FE_TYPE	DVBS or DVBS2 mode	passed by app level
**
**
**	Note:
**
**		The dvbs_type passed by app level may be 3 mode.
**		"MtFeType_DvbS" or "MtFeType_DvbS2" or "MtFeType_DvbS_Unknown".
**
**		If the type is 	"MtFeType_DvbS_Unknown", driver will try DVBS and DVBS2
**  automatically.
**
**
**
** Outputs:  None
**
**********************************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_connect(U32 freq_MHz, U32 sym_rate_KSs, MT_FE_TYPE dvbs_type)
{
	S16	lpf_offset_KHz;
	U32	freq_tmp_KHz;

	U32	real_freq_KHz;
	S32	deviceoffset_KHz;

	MT_FE_RET ret = MtFeErr_Ok;

	#if (MT_FE_CHECK_CARRIER_OFFSET != 0)
		U8  val_regb2;
	#endif


	if((mt_fe_tn_set_tuner == NULL)||(mt_fe_tn_get_offset == NULL))
		return MtFeErr_NullPointer;

	g_connect_type		 = dvbs_type;
	g_connect_sym_rate	 = sym_rate_KSs;
	g_auto_tune_cnt		 = 0;
	g_cci_detect		 = 1;
	g_eq_coef_check		 = 0;

	spectrum_inverted = (U8)(iq_inverted_status ^ use_unicable_device);		// XOR

	#if ((MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE != 0) || (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0))
		g_check_TS_clock   = 1;
	#endif

	#if (MT_FE_CHECK_CARRIER_OFFSET != 0)
		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			_mt_fe_dmd_get_reg(0xb2, &val_regb2);
			if(val_regb2 == 0x01)
			{
				_mt_fe_dmd_set_reg(0x05, 0x00);
				_mt_fe_dmd_set_reg(0xb2, 0x00);
			}
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			_mt_fe_dmd_get_reg(0xb2, &val_regb2);
			if(val_regb2 == 0x01)
			{
				_mt_fe_dmd_set_reg(0x00, 0x00);
				_mt_fe_dmd_set_reg(0xb2, 0x00);
			}
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}
	#endif


	if (dvbs_type == MtFeType_DvbS_Unknown)
		g_current_type = MtFeType_DvbS;
	else
		g_current_type = dvbs_type;


	freq_tmp_KHz	 = freq_MHz*1000;
	lpf_offset_KHz	 = 0;

#if ((MT_FE_TN_SUPPORT_TS2020 != 0)||(MT_FE_TN_SUPPORT_SHARP6306 != 0)||(MT_FE_TN_SUPPORT_SHARP7803 != 0)||(MT_FE_TN_SUPPORT_SHARP7903 != 0)||(MT_FE_TN_SUPPORT_TS2022 != 0))
	if ((sym_rate_KSs < 5000) && (use_unicable_device == 0))
	{
		lpf_offset_KHz = FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz;
		freq_tmp_KHz  += FREQ_OFFSET_AT_SMALL_SYM_RATE_KHz;
	}
#endif

	/*set tuner*/
	if(use_unicable_device == 1)
	{
		mt_fe_unicable_set_tuner(freq_tmp_KHz, sym_rate_KSs, &real_freq_KHz, g_ub_select, g_bank_select, g_ub_freq_MHz);
		deviceoffset_KHz = real_freq_KHz - freq_tmp_KHz;
	}
	else
	{
		mt_fe_tn_set_tuner(freq_tmp_KHz, sym_rate_KSs, lpf_offset_KHz);
		real_freq_KHz = freq_tmp_KHz;
		deviceoffset_KHz = 0;
	}

	_mt_fe_dmd_set_reg(0xb2, 0x01);
	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_set_reg(0x00, 0x01);
	}

	/*set demod registers*/
	_mt_fe_dmd_ds3k_set_demod(sym_rate_KSs, g_current_type);

	/*set carrier offset*/
	if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
		_mt_fe_dmd_ds3k_set_carrier_offset(-(S32)mt_fe_tn_get_offset() - lpf_offset_KHz - deviceoffset_KHz);
	else
		_mt_fe_dmd_ds3k_set_carrier_offset((S32)mt_fe_tn_get_offset() + lpf_offset_KHz + deviceoffset_KHz);


	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_set_reg(0x00, 0x00);
	}
	_mt_fe_dmd_set_reg(0xb2, 0x00);

	return ret;
}



/******************************************************************
** Function: mt_fe_dmd_ds3k_get_lock_state
**
**
** Description:	This function is called by app level to get the
**			lock status of DS3103/DS3002B/DS300X.
**
**
** Inputs:   None
**
**
** Outputs:
**
**		Parameter	Type				Description
**		-----------------------------------------------------------
**		p_state		MT_FE_LOCK_STATE*	lock status of DS3103/DS3002B/DS300X
**
**
*******************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_get_lock_state(MT_FE_LOCK_STATE *p_state)
{
	U8	tmp;

	#if (MT_FE_CHECK_CARRIER_OFFSET != 0)
		U8 val_regb2;
		S32 carrier_offset_KHz;
	#endif

	if (p_state == 0)
		return MtFeErr_Param;

	*p_state = MtFeLockState_Waiting;

	#if (MT_FE_CHECK_CARRIER_OFFSET != 0)
		_mt_fe_dmd_get_reg(0xb2, &val_regb2);
		if(val_regb2 == 0x01)
		{
			*p_state = MtFeLockState_Unlocked;

			return MtFeErr_Ok;
		}
	#endif


	if(g_cci_detect == 1)
	{
		_mt_fe_dmd_ds3k_check_CCI(p_state);

		if(*p_state == MtFeLockState_Unlocked)
			return MtFeErr_Ok;

		g_cci_detect = 0;
	}


	if (g_current_type == MtFeType_DvbS2)
	{
		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			_mt_fe_dmd_get_reg(0x0d, &tmp);
			if((tmp & 0x08) == 0x08)		// bit3 = 1
			{
				_mt_fe_dmd_get_reg(0x7e, &tmp);
				if((tmp & 0xc0) == 0x40)			// 8PSK
				{
					if((tmp & 0x0f) > 8)					// 8/9 & 9/10
					{
						_mt_fe_dmd_set_reg(0x7c, 0x01);
						_mt_fe_dmd_set_reg(0x80, 0x88);
					}
					else if((tmp & 0x0f) == 8)				// 5/6
					{
						_mt_fe_dmd_set_reg(0x7c, 0x01);
						_mt_fe_dmd_set_reg(0x80, 0x98);
					}
					else if((tmp & 0x20) == 0x20)			// pilot on
					{
						_mt_fe_dmd_set_reg(0x7c, 0x01);
						_mt_fe_dmd_set_reg(0xae, 0x0f);
						_mt_fe_dmd_set_reg(0x80, 0x54);
						_mt_fe_dmd_set_reg(0x81, 0x85);
					}
					else if((tmp & 0x0f) == 6)				// 3/4
					{
						_mt_fe_dmd_set_reg(0x7c, 0x01);
						_mt_fe_dmd_set_reg(0x80, 0x98);
					}
					else if((tmp & 0x0f) == 5)				// 2/3
					{
						_mt_fe_dmd_set_reg(0x7c, 0x01);
						_mt_fe_dmd_set_reg(0x80, 0xa8);
					}
					else if((tmp & 0x0f) == 4)				// 3/5
					{
						_mt_fe_dmd_set_reg(0x7c, 0x01);
						_mt_fe_dmd_set_reg(0x80, 0xc9);
					}
				}
				else if((tmp & 0xc0) == 0x00)		// QPSK
				{
					_mt_fe_dmd_set_reg(0x7c, 0x00);
					_mt_fe_dmd_set_reg(0x80, 0x88);
					_mt_fe_dmd_set_reg(0x81, 0x88);
				}
			}
		}
		else
		{
			_mt_fe_dmd_get_reg(0x76, &tmp);
			if(((tmp & 0x40) == 0) && (g_eq_coef_check == 0))		// 21 taps and first time
			{
				_mt_fe_dmd_get_reg(0xb3, &tmp);
				if((tmp & 0x3f) >= 0x21)
				{
					_mt_fe_dmd_get_reg(0x7e, &tmp);
					if((tmp & 0x20) == 0)	// pilot off
					{
						U32 tap_val[21], main_tap_value, other_tap_value, ratio;
						U8 i, bit01_00;
						U16 bit09_00, bit09_02;
						S32 real, image;

						for(i = 0; i < 21; i ++)
						{
							_mt_fe_dmd_set_reg(0x7A, (U8)(i << 2));
							_mt_fe_dmd_get_reg(0x7A, &tmp);
							bit01_00 = tmp & 0x03;
							_mt_fe_dmd_get_reg(0x7B, &tmp);
							bit09_02 = tmp << 2;
							bit09_00 = bit09_02 | bit01_00;
							real = (S32)bit09_00;
							if(real >= 512)
							{
								real -= 1024;
							}

							_mt_fe_dmd_set_reg(0x7A, (U8)((i + 21) << 2));
							_mt_fe_dmd_get_reg(0x7A, &tmp);
							bit01_00 = tmp & 0x03;
							_mt_fe_dmd_get_reg(0x7B, &tmp);
							bit09_02 = tmp << 2;
							bit09_00 = bit09_02 | bit01_00;
							image = (int)bit09_00;
							if(image >= 512)
							{
								image -= 1024;
							}

							tap_val[i] = real * real + image * image;
						}

						main_tap_value = tap_val[10];

						other_tap_value = 0;

						for(i = 0; i < 10; i ++)
						{
							other_tap_value += tap_val[i];
							other_tap_value += tap_val[i + 11];
						}

						if(main_tap_value != 0)
						{
							ratio = (other_tap_value * 100 + main_tap_value - 1) / main_tap_value;
						}
						else
						{
							ratio = 0;
						}

						if(ratio > MT_FE_S2_EQ_COEF_THRESHOLD)
						{
							_mt_fe_dmd_set_reg(0xbd, 0x03);

							_mt_fe_dmd_set_reg(0xb2, 0x01);
							_mt_fe_dmd_set_reg(0x00, 0x01);
							_mt_sleep(1);
							_mt_fe_dmd_set_reg(0x00, 0x00);
							_mt_fe_dmd_set_reg(0xb2, 0x00);

							g_eq_coef_check = 1;
						}
					}
				}
			}
		}

		_mt_fe_dmd_get_reg(0x0d, &tmp);
		if ((tmp & 0x8f) == 0x8f)
		{
			*p_state = MtFeLockState_Locked;
		}
	}
	else
	{
		_mt_fe_dmd_get_reg(0xd1, &tmp);

		if ((tmp & 0x07) == 0x07)
		{
			_mt_fe_dmd_get_reg(0x0d, &tmp);
			if((tmp & 0x07) == 0x07)
				*p_state = MtFeLockState_Locked;
		}
	}


	if (*p_state == MtFeLockState_Locked)
	{
		g_auto_tune_cnt = 1;
		#if MT_FE_CHECK_CARRIER_OFFSET
		carrier_offset_KHz = 0;
		_mt_fe_dmd_ds3k_get_total_carrier_offset(&carrier_offset_KHz);

		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			if((carrier_offset_KHz >= MT_FE_CARRIER_OFFSET_KHz)||
			   (carrier_offset_KHz <= - MT_FE_CARRIER_OFFSET_KHz))
			{
				_mt_fe_dmd_set_reg(0xb2, 0x01);
				_mt_fe_dmd_set_reg(0x05, 0x10);

				*p_state = MtFeLockState_Unlocked;
			}
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			if((carrier_offset_KHz >= MT_FE_CARRIER_OFFSET_KHz)||
			   (carrier_offset_KHz <= - MT_FE_CARRIER_OFFSET_KHz))
			{
				_mt_fe_dmd_set_reg(0xb2, 0x01);
				_mt_fe_dmd_set_reg(0x00, 0x01);

				*p_state = MtFeLockState_Unlocked;
			}
		}
		else
		{
			*p_state = MtFeLockState_Unlocked;
			return MtFeErr_NoSupportDemod;
		}
		#endif

		#if ((MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE != 0) || (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0))
		if(g_check_TS_clock == 1)
		{
			g_check_TS_clock = 0;
			_mt_fe_dmd_ds3k_set_clock_ratio();
		}
		#endif

		return MtFeErr_Ok;
	}
	else
	{
		#if ((MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE != 0) || (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0))
		g_check_TS_clock = 1;
		#endif
	}


	_mt_fe_dmd_get_reg(0xb3, &tmp);
	if(g_connect_type != MtFeType_DvbS_Unknown)
	{
		if ((tmp & 0xc0) != 0)
		{
			*p_state = MtFeLockState_Unlocked;
			return MtFeErr_Ok;
		}
	}
	else
	{
		if((tmp & 0x80) == 0x80)
		{
			*p_state = MtFeLockState_Unlocked;
			_mt_fe_dmd_set_reg(0xb2, 0x01);
			_mt_sleep(2);
			_mt_fe_dmd_set_reg(0xb2, 0x00);
			return MtFeErr_Ok;
		}


		if((tmp & 0x40) == 0x40)
		{
			if(g_auto_tune_cnt < 1)
			{
				if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
				{
					mt_fe_dmd_ds3k_global_reset();

					_mt_fe_dmd_ds3k_set_carrier_offset(g_carrier_offset);
				}

				g_current_type = (g_current_type == MtFeType_DvbS) ? MtFeType_DvbS2 : MtFeType_DvbS;
				_mt_fe_dmd_set_reg(0xb2, 0x01);
				if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
				{
					_mt_fe_dmd_set_reg(0x00, 0x01);
				}
				_mt_fe_dmd_ds3k_set_demod(g_connect_sym_rate, g_current_type);
				if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
				{
					_mt_fe_dmd_set_reg(0x00, 0x00);
				}
				_mt_fe_dmd_set_reg(0xb2, 0x00);
			}
			else
			{
				return MtFeErr_Ok;
			}
		}
	}

	return MtFeErr_Ok;
}


/*********************************************************************
** Function: mt_fe_dmd_ds3k_get_per
**
**
** Description:	Get the two package counter value to calculate the per at
**			app level.
**
**
**
** Inputs:   None
**
**
** Outputs:
**
**	  Parameter			Type			Description
**	---------------------------------------------------------------
**	  p_total_packags	U32*			total packags
**	  p_err_packags		U32*			error packags
**
**
** NOTE:	This function outputs error and total packages in DVBS2 mode,
**		and error and total bits in DVBS mode. You can calculate the PER
**		and BER in APP level.
**
**********************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_get_per(U32 *p_total_packags, U32 *p_err_packags)
{
	#define TOTAL_PACKET_S	3000

	U8		tmp1, tmp2, tmp3;
	U8		val_0xF6, val_0xF7, val_0xF8;
	U32		code_rate_fac = 0, ldpc_frame_cnt;
	static  U32 pre_err_packags   = 0;
	static  U32 pre_total_packags = 0xffff;
	MT_FE_CHAN_INFO_DVBS2 p_info;


	*p_err_packags   = pre_err_packags;
	*p_total_packags = pre_total_packags;


	pre_err_packags = 0;


	if (g_current_type == MtFeType_DvbS2)
	{
		mt_fe_dmd_ds3k_get_channel_info(&p_info);
		switch (p_info.code_rate)
		{
			case MtFeCodeRate_1_4:	code_rate_fac = 16008 - 80;	break;
			case MtFeCodeRate_1_3:	code_rate_fac = 21408 - 80;	break;
			case MtFeCodeRate_2_5:	code_rate_fac = 25728 - 80;	break;
			case MtFeCodeRate_1_2:	code_rate_fac = 32208 - 80;	break;
			case MtFeCodeRate_3_5:	code_rate_fac = 38688 - 80;	break;
			case MtFeCodeRate_2_3:	code_rate_fac = 43040 - 80;	break;
			case MtFeCodeRate_3_4:	code_rate_fac = 48408 - 80;	break;
			case MtFeCodeRate_4_5:	code_rate_fac = 51648 - 80;	break;
			case MtFeCodeRate_5_6:	code_rate_fac = 53840 - 80;	break;
			case MtFeCodeRate_8_9:	code_rate_fac = 57472 - 80;	break;
			case MtFeCodeRate_9_10:	code_rate_fac = 58192 - 80;	break;
			default:	break;
		}

		_mt_fe_dmd_get_reg(0xd7, &tmp1);
		_mt_fe_dmd_get_reg(0xd6, &tmp2);
		_mt_fe_dmd_get_reg(0xd5, &tmp3);
		ldpc_frame_cnt = (tmp1 << 16) | (tmp2 << 8) | tmp3;


		_mt_fe_dmd_get_reg(0xf8, &tmp1);
		_mt_fe_dmd_get_reg(0xf7, &tmp2);
		pre_err_packags = tmp1<<8 | tmp2;


		pre_total_packags = code_rate_fac * ldpc_frame_cnt / (188*8);


		if (ldpc_frame_cnt > TOTAL_PACKET_S)
		{
			_mt_fe_dmd_set_reg(0xd1, 0x01);
			_mt_fe_dmd_set_reg(0xf9, 0x01);
			_mt_fe_dmd_set_reg(0xf9, 0x00);
			_mt_fe_dmd_set_reg(0xd1, 0x00);

			*p_err_packags 	  = pre_err_packags;
			*p_total_packags  = pre_total_packags;
		}
	}
	else
	{
		_mt_fe_dmd_set_reg(0xf9, 0x04);
		_mt_fe_dmd_get_reg(0xf8, &val_0xF8);

		if((val_0xF8&0x10) == 0)
		{
			_mt_fe_dmd_get_reg(0xf6, &val_0xF6);
			_mt_fe_dmd_get_reg(0xf7, &val_0xF7);

			pre_err_packags = (val_0xF7<<8) | val_0xF6;

			val_0xF8 |= 0x10;
			_mt_fe_dmd_set_reg(0xf8, val_0xF8);
		}

		*p_err_packags   = pre_err_packags;

		if((val_0xF8&0x08) == 0)
			*p_total_packags = 8388608;
		else
			*p_total_packags = 1048576;
	}


	return MtFeErr_Ok;
}



/******************************************************************
** Function: mt_fe_dmd_ds3k_get_snr
**
**
** Description:	get snr value, unit dB
**
**
** Inputs:   None
**
**
** Outputs:
**
**	  Parameter		Type		Description
**	---------------------------------------------------------------
**	  p_snr			S8*			snr with dB unit
**
*******************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_get_snr(S8 *p_snr)
{
	const U16 mes_log10[] =
	{
		0,		3010,	4771,	6021, 	6990,	7781,	8451,	9031,	9542,	10000,
		10414,	10792,	11139,	11461,	11761,	12041,	12304,	12553,	12788,	13010,
		13222,	13424,	13617,	13802,	13979,	14150,	14314,	14472,	14624,	14771,
		14914,	15052,	15185,	15315,	15441,	15563,	15682,	15798,	15911,	16021,
		16128,	16232,	16335,	16435,	16532,	16628,	16721,	16812,	16902,	16990,
		17076,	17160,	17243,	17324,	17404,	17482,	17559,	17634,	17709,	17782,
		17853,	17924,	17993,	18062,	18129,	18195,	18261,	18325,	18388,	18451,
		18513,	18573,	18633,	18692,	18751,	18808,	18865,	18921,	18976,	19031
	};


	const U16 mes_loge[] =
	{
		0,		6931,	10986,	13863, 	16094,	17918,	19459,	20794,	21972,	23026,
		23979,	24849,	25649,	26391,	27081,	27726,	28332,	28904,	29444,	29957,
		30445,	30910,	31355,	31781,	32189,	32581,	32958,	33322,	33673,	34012,
		34340,	34657
	};



	static S8 snr;

	U8 		val, npow1, npow2, spow1, cnt;
	U16		tmp;
	U32 	npow, spow, snr_total;


	*p_snr = 0;
	if (g_current_type == MtFeType_DvbS2)
	{
		cnt  = 10;
		npow = 0;
		spow = 0;

		while(cnt >0)
		{
			_mt_fe_dmd_get_reg(0x8c, &npow1);
			_mt_fe_dmd_get_reg(0x8d, &npow2);
			npow += (((npow1 & 0x3F) + (U16)(npow2 << 6)) >> 2);

			_mt_fe_dmd_get_reg(0x8e, &spow1);
			spow += ((spow1 * spow1) >> 1);

			cnt--;
		}

		npow /= 10;
		spow /= 10;

		if(spow == 0)
		{
			snr = 0;
		}
		else if(npow == 0)
		{
			snr = 19;
		}
		else
		{
			if(spow > npow)
			{
				tmp = (U16)((spow) / npow);
				if (tmp > 80)
					tmp = 80;

				snr = (S8)(mes_log10[tmp - 1] / 1000);
			}
			else
			{
				tmp = (U16)(npow / spow);
				if (tmp > 80)
					tmp = 80;

				snr = (S8)(-(mes_log10[tmp - 1] / 1000));
			}
		}
	}
	else
	{
		cnt = 10;
		snr_total = 0;

		while(cnt > 0)
		{
			_mt_fe_dmd_get_reg(0xff, &val);
			snr_total += val;

			cnt--;
		}

		tmp = (U16)(snr_total/80);
		if(tmp > 0)
		{
			if (tmp > 32)		// impossible
				tmp = 32;

			snr =  (S8)((mes_loge[tmp - 1] * 10) / 23026);
		}
		else
		{
			snr = 0;
		}
	}



	*p_snr = snr;


	return MtFeErr_Ok;
}



/******************************************************************
** Function: mt_fe_dmd_ds3k_get_strength
**
**
** Description:	get signal strength
**
**
** Inputs:   None
**
**
** Outputs:
**
**	  Parameter			Type		Description
**	---------------------------------------------------------------
**	  p_strength		S8*			signal strength value(0----100)
**
*******************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_get_strength(S8 *p_strength)
{
#if MT_FE_TN_SUPPORT_TS2020
	mt_fe_tn_get_strength_ts2020(p_strength);
#elif MT_FE_TN_SUPPORT_TS2022
	mt_fe_tn_get_strength_ts2022(p_strength);
#elif MT_FE_TN_SUPPORT_SHARP6306
	mt_fe_tn_get_strength_sharp6306(p_strength);
#elif MT_FE_TN_SUPPORT_SHARP7803
	mt_fe_tn_get_strength_sharp7803(p_strength);
#elif MT_FE_TN_SUPPORT_SHARP7903
	mt_fe_tn_get_strength_sharp7903(p_strength);
#elif MT_FE_TN_SUPPORT_ST6110
	mt_fe_tn_get_strength_st6110(p_strength);
#elif MT_FE_TN_SUPPORT_AV2011
	mt_fe_tn_get_strength_AV2011(p_strength);
#elif MT_FE_TN_SUPPORT_AV2026
	mt_fe_tn_get_strength_AV2026(p_strength);
#elif MT_FE_TN_SUPPORT_RAON_MTV600
	mt_fe_tn_get_strength_raon_mtv600(p_strength);
#else
	*p_strength = 0;
#endif

	return MtFeErr_Ok;
}



/******************************************************************
** Function: mt_fe_dmd_ds3k_get_channel_info
**
**
** Description:	get channel info
**
**
** Inputs:   None
**
**
** Outputs:
**
**	  Parameter		Type					Description
**	---------------------------------------------------------------
**	  p_info		MT_FE_CHAN_INFO_DVBS2*	get the channel info.
**
*******************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_get_channel_info(MT_FE_CHAN_INFO_DVBS2 *p_info)
{
	U8	tmp, val_0x7E;


	p_info->type = g_current_type;

	if(g_current_type == MtFeType_DvbS2)
	{
		_mt_fe_dmd_get_reg(0x7e, &val_0x7E);
		tmp = (U8)((val_0x7E&0xC0) >> 6);
		switch(tmp)
		{
			case 0:  p_info->mod_mode = MtFeModMode_Qpsk;	break;
			case 1:  p_info->mod_mode = MtFeModMode_8psk;	break;
			case 2:  p_info->mod_mode = MtFeModMode_16Apsk;	break;
			case 3:  p_info->mod_mode = MtFeModMode_32Apsk;	break;
			default: p_info->mod_mode = MtFeModMode_Undef;	break;
		}


		p_info->is_pilot_on = ((val_0x7E&0x20) != 0) ? MtFe_True : MtFe_False;


		tmp = (U8)(val_0x7E & 0x0f);
		switch(tmp)
		{
			case 0:  p_info->code_rate = MtFeCodeRate_1_4;		break;
			case 1:  p_info->code_rate = MtFeCodeRate_1_3;		break;
			case 2:  p_info->code_rate = MtFeCodeRate_2_5;		break;
			case 3:  p_info->code_rate = MtFeCodeRate_1_2;		break;
			case 4:  p_info->code_rate = MtFeCodeRate_3_5;		break;
			case 5:  p_info->code_rate = MtFeCodeRate_2_3;		break;
			case 6:  p_info->code_rate = MtFeCodeRate_3_4;		break;
			case 7:  p_info->code_rate = MtFeCodeRate_4_5;		break;
			case 8:  p_info->code_rate = MtFeCodeRate_5_6;		break;
			case 9:  p_info->code_rate = MtFeCodeRate_8_9;		break;
			case 10: p_info->code_rate = MtFeCodeRate_9_10;		break;
			default: p_info->code_rate = MtFeCodeRate_Undef;	break;
		}


		_mt_fe_dmd_get_reg(0x89, &tmp);
		p_info->is_spectrum_inv = ((tmp&0x80) != 0) ? MtFeSpectrum_Inversion : MtFeSpectrum_Normal;


		_mt_fe_dmd_get_reg(0xf2, &tmp);
		tmp	&= 0x03;
		switch(tmp)
		{
			case 0:  p_info->roll_off = MtFeRollOff_0p35; 	break;
			case 1:  p_info->roll_off = MtFeRollOff_0p25; 	break;
			case 2:  p_info->roll_off = MtFeRollOff_0p20; 	break;
			default: p_info->roll_off = MtFeRollOff_Undef; 	break;
		}
	}
	else
	{
		p_info->mod_mode = MtFeModMode_Qpsk;


		_mt_fe_dmd_get_reg(0xe6, &tmp);
		tmp = (U8)(tmp >> 5);
		switch(tmp)
		{
			case 0:  p_info->code_rate = MtFeCodeRate_7_8;		break;
			case 1:  p_info->code_rate = MtFeCodeRate_5_6;		break;
			case 2:  p_info->code_rate = MtFeCodeRate_3_4;		break;
			case 3:  p_info->code_rate = MtFeCodeRate_2_3;		break;
			case 4:  p_info->code_rate = MtFeCodeRate_1_2;		break;
			default: p_info->code_rate = MtFeCodeRate_Undef;	break;
		}


		p_info->is_pilot_on = MtFe_False;


		_mt_fe_dmd_get_reg(0xe0, &tmp);
		p_info->is_spectrum_inv = ((tmp&0x04) != 0) ? MtFeSpectrum_Inversion : MtFeSpectrum_Normal;


		p_info->roll_off = MtFeRollOff_0p35;
	}

	return MtFeErr_Ok;
}


/*********************************************************************************
** Function: mt_fe_dmd_ds3k_set_LNB
**
**
** Description:	set LNB	status.
**
**
** Inputs:
**
**		Parameter		Type				Description
**	---------------------------------------------------------------
**		is_LNB_enable	MT_FE_BOOL			Enable or Disable
**		is_22k_enable	MT_FE_BOOL			Enable or Disable
**		mode			MT_FE_LNB_VOLTAGE	13V	or 18V
**		is_envelop_mode	MT_FE_BOOL			Enable or Disable
**
**
** Note:
**
**		Here the LNB setting should cooperate with the hardware circuit.
**
**		You should set the define "LNB_ENABLE_WHEN_LNB_EN_HIGH",
**	"LNB_13V_WHEN_VSEL_HIGH", "LNB_VSEL_STANDBY_HIGH" and
**	"LNB_DISEQC_OUT_STANDBY_HIGH" first according to the circuit designed.
**
**
**
** Outputs:		none
**
**
***********************************************************************************/
MT_FE_RET  mt_fe_dmd_ds3k_set_LNB(MT_FE_BOOL is_LNB_enable, MT_FE_BOOL is_22k_enable, MT_FE_LNB_VOLTAGE voltage_type, MT_FE_BOOL is_envelop_mode)
{
	U8	val_0xa1, val_0xa2;


	_mt_fe_dmd_get_reg(0xa1, &val_0xa1);
	_mt_fe_dmd_get_reg(0xa2, &val_0xa2);


	if(is_LNB_enable != MtFe_True)
	{
		val_0xa1 |= 0x40;

		/*set LNB_EN pin HIGH or LOW */
		#if (LNB_ENABLE_WHEN_LNB_EN_HIGH)
			val_0xa2 &= ~0x02;
		#else
			val_0xa2 |= 0x02;
		#endif


		/*set V_SEL Pin mode*/
		#if (LNB_VSEL_STANDBY_HIGH)
			val_0xa2 |= 0x01;
		#else
			val_0xa2 &= ~0x01;
		#endif


		/*set DiseQc_OUT mode*/
		#if (LNB_DISEQC_OUT_FORCE_HIGH)
			val_0xa2 |= 0xc0;
		#else
			val_0xa2 &= ~0xc0;
			val_0xa2 |= 0x80;
		#endif
	}
	else
	{
		#if (LNB_ENABLE_WHEN_LNB_EN_HIGH)
			val_0xa2 |= 0x02;
		#else
			val_0xa2 &= ~0x02;
		#endif


		#if (LNB_13V_WHEN_VSEL_HIGH)
			if (voltage_type == MtFeLNB_13V)
				val_0xa2 |= 0x01;
			else
				val_0xa2 &= ~0x01;
		#else
			if (voltage_type == MtFeLNB_13V)
				val_0xa2 &= ~0x01;
			else
				val_0xa2 |= 0x01;
		#endif


		if (is_22k_enable == MtFe_True)
		{
			val_0xa1 |= 0x04;
			val_0xa1 &= ~0x03;
			val_0xa1 &= ~0x40;
			val_0xa2 &= ~0xc0;
		}
		else
		{
			#if (LNB_DISEQC_OUT_FORCE_HIGH)
				val_0xa2 |= 0xc0;
			#else
				val_0xa2 &= ~0xc0;
				val_0xa2 |= 0x80;
			#endif
		}
	}

	if(is_envelop_mode != MtFe_True)
	{
		val_0xa2 &= ~0x20;
	}
	else
	{
		val_0xa2 |= 0x20;
	}

	_mt_fe_dmd_set_reg(0xa2, val_0xa2);
	_mt_fe_dmd_set_reg(0xa1, val_0xa1);


	return MtFeErr_Ok;
}




/*************************************************************************************
** Function: mt_fe_dmd_ds3k_DiSEqC_send_tone_burst
**
**
** Description:	send DiSEqC message in tone burst mode
**				1) modulated tone burst
**				2) unmodulated tone burst
**
**
** Inputs:
**
**		Parameter			Type						Description
**	---------------------------------------------------------------------------------
**		mode				MT_FE_DiSEqC_TONE_BURST		modulated/unmodulated tone burst
**		is_envelop_mode		MT_FE_BOOL
**
**
** Outputs:		none
**
**
**************************************************************************************/
MT_FE_RET  mt_fe_dmd_ds3k_DiSEqC_send_tone_burst(MT_FE_DiSEqC_TONE_BURST mode, MT_FE_BOOL is_envelop_mode)
{
	U8	val;
	S32	time_out;


	_mt_fe_dmd_get_reg(0xa2, &val);
	val &= ~0xc0;
	if(is_envelop_mode != MtFe_True)
	{
		val &= ~0x20;
	}
	else
	{
		val |= 0x20;
	}
	_mt_fe_dmd_set_reg(0xa2, val);


	if (mode == MtFeDiSEqCToneBurst_Moulated)
		_mt_fe_dmd_set_reg(0xa1, 0x01);
	else
		_mt_fe_dmd_set_reg(0xa1, 0x02);



	_mt_sleep(13);



	time_out = 5;
	do
	{
		_mt_fe_dmd_get_reg(0xa1, &val);
		if ((val & 0x40) == 0)
			break;

		_mt_sleep(1);
		time_out --;

	} while (time_out > 0);


	_mt_fe_dmd_get_reg(0xa2, &val);
	#if LNB_DISEQC_OUT_FORCE_HIGH
		val |= 0xc0;
	#else
		val &= ~0xc0;
		val |= 0x80;
	#endif
	_mt_fe_dmd_set_reg(0xa2, val);

	return MtFeErr_Ok;
}




/*****************************************************************************
** Function: mt_fe_dmd_ds3k_DiSEqC_send_receive_msg
**
**
** Description:	send data with reply message by device in DiSEqC mode.
**
**
** Inputs:
**
**	  Parameter			Type				Description
**	----------------------------------------------------------------------
**	  msg				MT_FE_DiSEqC_MSG	DiSEqC message pointer
**
**
** Outputs:		none
**
**
**
*******************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_DiSEqC_send_receive_msg(MT_FE_DiSEqC_MSG *msg)
{
	U8	i;
	U8	tmp, tmp1;
	S32	time_out;



	if (msg->size_send > 8)
		return MtFeErr_Param;



	_mt_fe_dmd_get_reg(0xa2, &tmp);
	tmp &= ~0xc0;
	if(msg->is_envelop_mode != TRUE)
	{
		tmp &= ~0x20;
	}
	else
	{
		tmp |= 0x20;
	}
	_mt_fe_dmd_set_reg(0xa2, tmp);


	for (i = 0; i < msg->size_send; i ++)
	{
		_mt_fe_dmd_set_reg((U8)(0xa3 + i), msg->data_send[i]);
	}

	_mt_fe_dmd_get_reg(0xa1, &tmp);
	tmp &= ~0x38;
	tmp &= ~0x40;
	tmp |= ((msg->size_send-1) << 3) | 0x07;
	if (msg->is_enable_receive)
		tmp |= 0x80;
	else
		tmp &= ~0x80;

	_mt_fe_dmd_set_reg(0xa1, tmp);


	/*	1.5 * 9 * 8	= 108ms	*/
	time_out = 150;
	while (time_out > 0)
	{
		_mt_sleep(10);
		time_out -= 10;

		_mt_fe_dmd_get_reg(0xa1, &tmp);
		if ((tmp & 0x40) == 0)
			break;
	}

	if (time_out <= 0)
	{
		_mt_fe_dmd_get_reg(0xa1, &tmp);
		tmp &= ~0x80;
		tmp |= 0x40;
		_mt_fe_dmd_set_reg(0xa1, tmp);


		_mt_fe_dmd_get_reg(0xa2, &tmp);
		#if LNB_DISEQC_OUT_FORCE_HIGH
			tmp |= 0xc0;
		#else
			tmp &= ~0xc0;
			tmp |= 0x80;
		#endif
		_mt_fe_dmd_set_reg(0xa2, tmp);

		return MtFeErr_TimeOut;
	}


	if (!(msg->is_enable_receive))
	{
		_mt_fe_dmd_get_reg(0xa2, &tmp);
		#if LNB_DISEQC_OUT_FORCE_HIGH
			tmp |= 0xc0;
		#else
			tmp &= ~0xc0;
			tmp |= 0x80;
		#endif
		_mt_fe_dmd_set_reg(0xa2, tmp);

		return MtFeErr_Ok;
	}



	time_out = 170;
	while (time_out > 0)
	{
		_mt_fe_dmd_get_reg(0xa1, &tmp);
		tmp =(U8)((tmp >> 3) & 0x07);
		if (tmp != 0)
			break;

		_mt_sleep(10);
		time_out -= 10;
	}

	if (time_out <= 0)
	{
		_mt_fe_dmd_get_reg(0xa1, &tmp);
		tmp &= ~0x80;
		tmp |= 0x40;
		_mt_fe_dmd_set_reg(0xa1, tmp);


		_mt_fe_dmd_get_reg(0xa2, &tmp);
		#if LNB_DISEQC_OUT_FORCE_HIGH
			tmp |= 0xc0;
		#else
			tmp &= ~0xc0;
			tmp |= 0x80;
		#endif
		_mt_fe_dmd_set_reg(0xa2, tmp);

		return MtFeErr_TimeOut;
	}

	/*	1.5 * 9 * 8	= 108ms	*/
	time_out = 150;
	tmp1     = tmp;
	while (time_out > 0)
	{
		_mt_sleep(15);
		time_out -= 15;

		_mt_fe_dmd_get_reg(0xa1, &tmp);
		tmp =(U8)((tmp >> 3) & 0x07);
		if (tmp == tmp1 || tmp == 7)
			break;

		tmp1 = tmp;
	}

	if (time_out <= 0)
	{
		_mt_fe_dmd_get_reg(0xa1, &tmp);
		tmp &= ~0x80;
		tmp |= 0x40;
		_mt_fe_dmd_set_reg(0xa1, tmp);


		_mt_fe_dmd_get_reg(0xa2, &tmp);
		#if LNB_DISEQC_OUT_FORCE_HIGH
			tmp |= 0xc0;
		#else
			tmp &= ~0xc0;
			tmp |= 0x80;
		#endif
		_mt_fe_dmd_set_reg(0xa2, tmp);

		return MtFeErr_TimeOut;
	}


	msg->size_receive = tmp;
	_mt_fe_dmd_set_reg(0xa1, 0x40);

	for (i = 0; i < msg->size_receive; i ++)
	{
		_mt_fe_dmd_get_reg((U8)(0xa3 + i), &(msg->data_receive[i]));
	}



	/*	Indicates the parity error occurs	*/
	_mt_fe_dmd_get_reg(0xab, &tmp);
	tmp <<= (8 - msg->size_receive);
	tmp &= 0xFF;
	if (tmp != 0)
	{
		_mt_fe_dmd_get_reg(0xa1, &tmp);
		tmp &= ~0x80;
		tmp |= 0x40;
		_mt_fe_dmd_set_reg(0xa1, tmp);


		_mt_fe_dmd_get_reg(0xa2, &tmp);
		#if LNB_DISEQC_OUT_FORCE_HIGH
			tmp |= 0xc0;
		#else
			tmp &= ~0xc0;
			tmp |= 0x80;
		#endif
		_mt_fe_dmd_set_reg(0xa2, tmp);


		return MtFeErr_Fail;
	}


	_mt_fe_dmd_get_reg(0xa1, &tmp);
	tmp &= ~0x80;
	tmp |= 0x40;
	_mt_fe_dmd_set_reg(0xa1, tmp);

	_mt_fe_dmd_get_reg(0xa2, &tmp);
	#if LNB_DISEQC_OUT_FORCE_HIGH
		tmp |= 0xc0;
	#else
		tmp &= ~0xc0;
		tmp |= 0x80;
	#endif
	_mt_fe_dmd_set_reg(0xa2, tmp);


	return MtFeErr_Ok;
}


/**********************************************************************
** Function: mt_fe_ds3k_blindscan_abort
**
**
** Description:	cancel blind scan process
**
**
** Inputs:
**
**		Parameter		type					description
**	-------------------------------------------------------------------
**		bool			MT_FE_BOOL				MtFe_True: cancel
**
**
** Outputs:		none
**
**
************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_blindscan_abort(MT_FE_BOOL bs_abort)
{

	g_blindscan_cancel = bs_abort;

	return MtFeErr_Ok;
}



/*********************************************************************************
** Function: mt_fe_dmd_ds3k_register_blindscan_callback
**
**
** Description:	register the blindscan callback function
**
**
** Inputs:
**
**		Parameter		type					description
**	-----------------------------------------------------------------------------
**		callback		MT_FE_RET (*)()			callback function pointer
**
**
** Outputs:		none
**
**
**********************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_register_notify(void (*callback)(MT_FE_MSG msg, void *p_tp_info))
{
	if(callback == NULL)
		return MtFeErr_NullPointer;

	mt_fe_bs_notify = callback;

	return MtFeErr_Ok;
}



/******************************************************************************************
** Function: mt_fe_dmd_ds3k_blindscan
**
**
** Description:	blind scan satellite from the start freq. to the end freq.
**
**
** Inputs:
**
**		Parameter			type								description
**	---------------------------------------------------------------------------------------
**		begin_freq_MHz		U32									blind scan start freq.
**		end_freq_MHz		U32									blind scan stop freq.
**
**
** Outputs:
**
**		Paramet			type								description
**	---------------------------------------------------------------------------------------
**		p_bs_info			MT_FE_BS_TP_INFO* 	blind scan result pointer
**
**
********************************************************************************************/
MT_FE_RET mt_fe_dmd_ds3k_blindscan(U32 begin_freq_MHz, U32 end_freq_MHz, MT_FE_BS_TP_INFO *p_bs_info)
{
	U8 times;
	U32 begin_freq_KHz, end_freq_KHz;

	if((p_bs_info->p_tp_info == NULL)||(p_bs_info->tp_max_num == 0))
		return MtFeErr_Param;

	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		mt_fe_dmd_ds3k_global_reset();
	}

	mt_fe_bs_notify(MtFeMsg_BSStart, 0);

	g_scanned_tp_cnt	 = 0;
	g_locked_tp_cnt		 = 0;
	g_blindscan_cancel	 = MtFe_False;
	g_bs_time_total		 = p_bs_info->bs_times;

	spectrum_inverted = (U8)(iq_inverted_status ^ use_unicable_device);		// XOR

	begin_freq_KHz = begin_freq_MHz*1000;
	end_freq_KHz   = end_freq_MHz*1000;

	p_bs_info->tp_num = 0;

	for(times = MT_FE_BS_TIMES_1ST; times < (p_bs_info->bs_times); times++)
	{
		if(p_bs_info->bs_algorithm == MT_FE_BS_ALGORITHM_A)
		{
			_mt_fe_dmd_ds3k_bs_A(begin_freq_KHz, end_freq_KHz, p_bs_info, times);
		}
		else if(p_bs_info->bs_algorithm == MT_FE_BS_ALGORITHM_B)
		{
			_mt_fe_dmd_ds3k_bs_B(begin_freq_KHz, end_freq_KHz, p_bs_info, times);
		}
		else
		{
			mt_fe_bs_notify(MtFeMsg_BSFinish, 0);
			return MtFeErr_Fail;
		}

		if(g_blindscan_cancel)
		{
			g_blindscan_cancel = MtFe_False;
			break;
		}
	}

	_mt_sleep(1000);

	_mt_fe_dmd_ds3k_bs_remove_unlocked_TP(p_bs_info, (U32)FREQ_MAX_KHz);


	mt_fe_bs_notify(MtFeMsg_BSFinish, 0);

	return MtFeErr_Ok;
}


MT_FE_RET _mt_fe_dmd_ds3k_get_mclk(U32 *p_MCLK_KHz)
{
	*p_MCLK_KHz = MT_FE_MCLK_KHZ;


	if(g_dmd_id == MtFeDmdId_DS300X)
	{
		if(g_current_type == MtFeType_DvbS2)
		{
			*p_MCLK_KHz = 144000;
		}
		else	// DVB-S
		{
			*p_MCLK_KHz = 96000;
		}
	}
	else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		U8 tmp1, tmp2;

		_mt_fe_dmd_get_reg(0x22, &tmp1);
		_mt_fe_dmd_get_reg(0x24, &tmp2);

		tmp1 >>= 6;
		tmp1 &= 0x03;
		tmp2 >>= 6;
		tmp2 &= 0x03;

		if((tmp1 == 0x00) && (tmp2 == 0x01))
		{
			*p_MCLK_KHz = 144000;
		}
		else if((tmp1 == 0x00) && (tmp2 == 0x03))
		{
			*p_MCLK_KHz = 72000;
		}
		else if((tmp1 == 0x01) && (tmp2 == 0x01))
		{
			*p_MCLK_KHz = 115200;
		}
		else if((tmp1 == 0x02) && (tmp2 == 0x01))
		{
			*p_MCLK_KHz = 96000;
		}
		else if((tmp1 == 0x03) && (tmp2 == 0x00))
		{
			*p_MCLK_KHz = 192000;
		}
		else
		{
			return MtFeErr_Param;
		}
	}
	else
	{
		return MtFeErr_NoSupportDemod;
	}


	return MtFeErr_Ok;
}

/******************************************************************
** Function: _mt_fe_dmd_ds3k_set_mclk
**
**
** Description:	set TS Clock Ratio for DS3002B/DS3103.
**
**
** Inputs:
**	MCLK_KHz		U32				The MCLK(KHz) to be set.
**
**
** Outputs:		None
**
**
*******************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_set_mclk(U32 MCLK_KHz)
{
	U8 tmp3 = 0, tmp4 = 0;

	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		// 0x22 bit[7:6] clkxM_d
		_mt_fe_dmd_get_reg(0x22, &tmp3);
		// 0x24 bit[7:6] clkxM_sel
		_mt_fe_dmd_get_reg(0x24, &tmp4);

		switch(MCLK_KHz)
		{
			case 192000:		// 4b'0011 MCLK = 192M
				tmp3 |= 0xc0;		// 0x22 bit[7:6] = 2b'11
				tmp4 &= 0x3f;		// 0x24 bit[7:6] = 2b'00
				break;

			case 144000:		// 4b'0100 MCLK = 144M
				tmp3 &= 0x3f;		// 0x22 bit[7:6] = 2b'00
				tmp4 &= 0x7f;		// 0x24 bit[7:6] = 2b'01
				tmp4 |= 0x40;
				break;

			case 115200:		// 4b'0101 MCLK = 115.2M
				tmp3 &= 0x7f;		// 0x22 bit[7:6] = 2b'01
				tmp3 |= 0x40;
				tmp4 &= 0x7f;		// 0x24 bit[7:6] = 2b'01
				tmp4 |= 0x40;
				break;

			case 72000:			// 4b'1100 MCLK = 72M
				tmp4 |= 0xc0;		// 0x24 bit[7:6] = 2b'11
				tmp3 &= 0x3f;		// 0x22 bit[7:6] = 2b'00
				break;

			case 96000:			// 4b'0110 MCLK = 96M
			default:
				tmp3 &= 0xbf;		// 0x22 bit[7:6] = 2b'10
				tmp3 |= 0x80;

				tmp4 &= 0x7f;		// 0x24 bit[7:6] = 2b'01
				tmp4 |= 0x40;
				break;
		}

		_mt_fe_dmd_set_reg(0x22, tmp3);
		_mt_fe_dmd_set_reg(0x24, tmp4);
	}
	else if(g_dmd_id == MtFeDmdId_DS300X)
	{
		return MtFeErr_NoSupportFunc;
	}
	else
	{
		return MtFeErr_NoSupportDemod;
	}

	return MtFeErr_Ok;
}



/******************************************************************
** Function: _mt_fe_dmd_ds3k_set_ts_divide_ratio
**
**
** Description:	set the TS divide ratio
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	type			MT_FE_TYPE	MtFeType_DvbS2 or MtFeType_DvbS.
**	dr_high			U8			The length of high level of the MPEG clock.
**	dr_low			U8			The length of low level of the MPEG clock.
**
**
** Outputs:		none
**
**
*******************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_set_ts_divide_ratio(MT_FE_TYPE type, U8 dr_high, U8 dr_low)
{
	U8 val, tmp1, tmp2;

	tmp1 = dr_high;
	tmp2 = dr_low;


	if(type == MtFeType_DvbS2)
	{
		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			tmp1 &= 0x0f;
			tmp2 &= 0x0f;

			val = (U8)((tmp1<<4) + tmp2);
			_mt_fe_dmd_set_reg(0xfe, val);
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			tmp1 -= 1;
			tmp2 -= 1;

			tmp1 &= 0x3f;
			tmp2 &= 0x3f;

			//_mt_fe_dmd_set_reg(0xfe, 0x20);	// default value

			_mt_fe_dmd_get_reg(0xfe, &val);		// ci_divrange_h_1 bits[5:2]
			val &= 0xF0;						// bits[3:0]
			val |= (tmp1 >> 2) & 0x0f;
			_mt_fe_dmd_set_reg(0xfe, val);

			val = (U8)((tmp1 & 0x03) << 6);		// ci_divrange_h_0 bits[1:0]
			val |= tmp2;						// ci_divrange_l   bits[5:0]
			_mt_fe_dmd_set_reg(0xea, val);
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}
	}
	else	// DVB-S
	{
		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			tmp1 &= 0x07;
			tmp2 &= 0x07;

			_mt_fe_dmd_get_reg(0xfe, &val);
			val &= 0xc0;
			val |= ((U8)(((tmp1<<3) + tmp2)) & 0x3f);
			_mt_fe_dmd_set_reg(0xfe, val);
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			tmp1 -= 1;
			tmp2 -= 1;

			tmp1 &= 0x3f;
			tmp2 &= 0x3f;

			//_mt_fe_dmd_set_reg(0xfe, 0x20);	// default value

			_mt_fe_dmd_get_reg(0xfe, &val);		// ci_divrange_h_1 bits[5:2]
			val &= 0xF0;						// bits[3:0]
			val |= (tmp1 >> 2) & 0x0f;
			_mt_fe_dmd_set_reg(0xfe, val);

			val = (U8)((tmp1 & 0x03) << 6);		// ci_divrange_h_0 bits[1:0]
			val |= tmp2;						// ci_divrange_l   bits[5:0]
			_mt_fe_dmd_set_reg(0xea, val);
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}
	}

	return MtFeErr_Ok;
}


/******************************************************************
** Function: _mt_fe_dmd_ds3k_set_clock_ratio
**
**
** Description:	set TS Clock Ratio
**
**
** Inputs:		None
**
**
** Outputs:		None
**
**
*******************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_set_clock_ratio(void)
{
	U8	mod_fac, tmp1, tmp2;
	U32	input_datarate, locked_sym_rate_KSs;
	U32 MClk_KHz = 96000;
	U16 divid_ratio = 0;
	MT_FE_CHAN_INFO_DVBS2 p_info;

	U8 ts_mode = MT_FE_TS_OUTPUT_MODE;

	locked_sym_rate_KSs = 0;

	_mt_fe_dmd_ds3k_get_sym_rate(&locked_sym_rate_KSs);

	if(locked_sym_rate_KSs == 0)
	{
		return MtFeErr_Param;
	}

	mt_fe_dmd_ds3k_get_channel_info(&p_info);

	_mt_fe_dmd_ds3k_get_current_ts_mode(&ts_mode);

	#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE == 0)
	if(ts_mode == MtFeTsOutMode_Serial)
	{
		return MtFeErr_Ok;
	}
	#endif

	#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE == 0)
	if((ts_mode == MtFeTsOutMode_Parallel) || (ts_mode == MtFeTsOutMode_Common))
	{
		return MtFeErr_Ok;
	}
	#endif

	if (g_current_type == MtFeType_DvbS2)			// for dvb-s2
	{
		switch(p_info.mod_mode)
		{
			case MtFeModMode_8psk: 		mod_fac = 3; break;
			case MtFeModMode_16Apsk:	mod_fac = 4; break;
			case MtFeModMode_32Apsk:	mod_fac = 5; break;
			case MtFeModMode_Qpsk:
			default:					mod_fac = 2; break;
		}

		switch(p_info.code_rate)
		{
			case MtFeCodeRate_1_4: 	input_datarate = locked_sym_rate_KSs*mod_fac/8/4;  	break;
			case MtFeCodeRate_1_3: 	input_datarate = locked_sym_rate_KSs*mod_fac/8/3;  	break;
			case MtFeCodeRate_2_5: 	input_datarate = locked_sym_rate_KSs*mod_fac*2/8/5;	break;
			case MtFeCodeRate_1_2:	input_datarate = locked_sym_rate_KSs*mod_fac/8/2;	break;
			case MtFeCodeRate_3_5:	input_datarate = locked_sym_rate_KSs*mod_fac*3/8/5;	break;
			case MtFeCodeRate_2_3:	input_datarate = locked_sym_rate_KSs*mod_fac*2/8/3;	break;
			case MtFeCodeRate_3_4:	input_datarate = locked_sym_rate_KSs*mod_fac*3/8/4;	break;
			case MtFeCodeRate_4_5:	input_datarate = locked_sym_rate_KSs*mod_fac*4/8/5;	break;
			case MtFeCodeRate_5_6:	input_datarate = locked_sym_rate_KSs*mod_fac*5/8/6;	break;
			case MtFeCodeRate_8_9:	input_datarate = locked_sym_rate_KSs*mod_fac*8/8/9;	break;
			case MtFeCodeRate_9_10:	input_datarate = locked_sym_rate_KSs*mod_fac*9/8/10;break;
			default:				input_datarate = locked_sym_rate_KSs*mod_fac*2/8/3;	break;
		}

		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			if(ts_mode == MtFeTsOutMode_Serial)
			{
				return MtFeErr_Ok;
			}

			input_datarate = input_datarate * 115 / 100;		// 108 @ 110121

			if(input_datarate < 4800)  {tmp1 = 15;tmp2 = 15;} //4.8MHz         TS clock
			else if(input_datarate < 4966)  {tmp1 = 14;tmp2 = 15;} //4.966MHz  TS clock
			else if(input_datarate < 5143)  {tmp1 = 14;tmp2 = 14;} //5.143MHz  TS clock
			else if(input_datarate < 5333)  {tmp1 = 13;tmp2 = 14;} //5.333MHz  TS clock
			else if(input_datarate < 5538)  {tmp1 = 13;tmp2 = 13;} //5.538MHz  TS clock
			else if(input_datarate < 5760)  {tmp1 = 12;tmp2 = 13;} //5.76MHz   TS clock       allan 0809
			else if(input_datarate < 6000)  {tmp1 = 12;tmp2 = 12;} //6MHz      TS clock
			else if(input_datarate < 6260)  {tmp1 = 11;tmp2 = 12;} //6.26MHz   TS clock
			else if(input_datarate < 6545)  {tmp1 = 11;tmp2 = 11;} //6.545MHz  TS clock
			else if(input_datarate < 6857)  {tmp1 = 10;tmp2 = 11;} //6.857MHz  TS clock
			else if(input_datarate < 7200)  {tmp1 = 10;tmp2 = 10;} //7.2MHz    TS clock
			else if(input_datarate < 7578)  {tmp1 = 9;tmp2 = 10;}  //7.578MHz  TS clock
			else if(input_datarate < 8000)	{tmp1 = 9;tmp2 = 9;}   //8MHz      TS clock
			else if(input_datarate < 8470)	{tmp1 = 8;tmp2 = 9;}   //8.47MHz   TS clock
			else if(input_datarate < 9000)	{tmp1 = 8;tmp2 = 8;}   //9MHz      TS clock
			else if(input_datarate < 9600)	{tmp1 = 7;tmp2 = 8;}   //9.6MHz    TS clock
			else if(input_datarate < 10285)	{tmp1 = 7;tmp2 = 7;}   //10.285MHz TS clock
			#if 1
			else if(input_datarate < 12000)	{tmp1 = 6;tmp2 = 6;}   //12MHz     TS clock
			else if(input_datarate < 14400)	{tmp1 = 5;tmp2 = 5;}   //14.4MHz   TS clock
			else if(input_datarate < 18000)	{tmp1 = 4;tmp2 = 4;}   //18MHz     TS clock
			else							{tmp1 = 3;tmp2 = 3;}   //24MHz     TS clock
			#else
			else if(input_datarate < 11076)	{tmp1 = 6;tmp2 = 7;}   //11.076MHz TS clock
			else if(input_datarate < 12000)	{tmp1 = 6;tmp2 = 6;}   //12MHz     TS clock
			else if(input_datarate < 13090)	{tmp1 = 5;tmp2 = 6;}   //13.09MHz  TS clock
			else if(input_datarate < 14400)	{tmp1 = 5;tmp2 = 5;}   //14.4MHz   TS clock
			else if(input_datarate < 16000)	{tmp1 = 4;tmp2 = 5;}   //16MHz     TS clock
			else if(input_datarate < 18000)	{tmp1 = 4;tmp2 = 4;}   //18MHz     TS clock
			else if(input_datarate < 20571)	{tmp1 = 3;tmp2 = 4;}   //20.571MHz TS clock
			else							{tmp1 = 3;tmp2 = 3;}   //24MHz     TS clock
			#endif
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			_mt_fe_dmd_ds3k_get_mclk(&MClk_KHz);

			if(ts_mode == MtFeTsOutMode_Serial)
			{
				U32 target_mclk = MClk_KHz;

				input_datarate *= 49;
				input_datarate /= 5;			// input_datarate * 1.225 * 8 = input_datarate * 49 / 5

				if(input_datarate > 115200)
				{
					target_mclk = 144000;
				}
				else if(input_datarate > 96000)
				{
					target_mclk = 115200;
				}
				else if(input_datarate > 72000)
				{
					target_mclk = 96000;
				}
				else
				{
					target_mclk = 72000;
				}

				g_serial_mclk = target_mclk;

				if(target_mclk != MClk_KHz)
				{
					_mt_fe_dmd_set_reg(0x06, 0xe0);
					_mt_fe_dmd_ds3k_set_mclk(target_mclk);
					_mt_fe_dmd_set_reg(0x06, 0x00);
				}

				return MtFeErr_Ok;
			}
			else		// Parallel or CI mode
			{
				g_serial_mclk = input_datarate * 49 / 5;
				if(g_serial_mclk > 115200)
				{
					g_serial_mclk = 144000;
				}
				else if(g_serial_mclk > 96000)
				{
					g_serial_mclk = 115200;
				}
				else if(g_serial_mclk > 72000)
				{
					g_serial_mclk = 96000;
				}
				else
				{
					g_serial_mclk = 72000;
				}

				// Set lower limit of TS output clock for Parallel and CI modes
				if(input_datarate < 6000)
				{
					input_datarate = 6000;
				}

				if(input_datarate != 0)
				{
					divid_ratio = (U16)(MClk_KHz / input_datarate);
				}
				else
				{
					divid_ratio = 0xFF;
				}

				if(divid_ratio > 128)
					divid_ratio = 128;

				if(divid_ratio < 2)
					divid_ratio = 2;

				tmp1 = (U8)(divid_ratio / 2);
				tmp2 = (U8)(divid_ratio / 2);

				if((divid_ratio % 2) != 0)
					tmp2 += 1;
			}
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}

	}
	else				// for dvb-s
	{
		mod_fac = 2;

		switch(p_info.code_rate)
		{
			case MtFeCodeRate_1_2:	input_datarate = locked_sym_rate_KSs*mod_fac/2/8;	break;
			case MtFeCodeRate_2_3:	input_datarate = locked_sym_rate_KSs*mod_fac*2/3/8;	break;
			case MtFeCodeRate_3_4:	input_datarate = locked_sym_rate_KSs*mod_fac*3/4/8;	break;
			case MtFeCodeRate_5_6:	input_datarate = locked_sym_rate_KSs*mod_fac*5/6/8;	break;
			case MtFeCodeRate_7_8:	input_datarate = locked_sym_rate_KSs*mod_fac*7/8/8;	break;
			default:				input_datarate = locked_sym_rate_KSs*mod_fac*3/4/8;	break;
		}

		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			input_datarate = input_datarate * 115 / 100;		// 108 @ 110121

			#if 1
			if(input_datarate < 6857)		{tmp1 = 7;tmp2 = 7;} //6.857MHz     TS clock
			else if(input_datarate < 8000)	{tmp1 = 6;tmp2 = 6;} //8MHz     	TS clock
			else if(input_datarate < 9600)	{tmp1 = 5;tmp2 = 5;} //9.6MHz    	TS clock
			else if(input_datarate < 12000)	{tmp1 = 4;tmp2 = 4;} //12MHz 	 	TS clock
			else if(input_datarate < 16000)	{tmp1 = 3;tmp2 = 3;} //16MHz     	TS clock
			else 							{tmp1 = 2;tmp2 = 2;} //24MHz     	TS clock
			#else
			if(input_datarate < 6857)		{tmp1 = 7;tmp2 = 7;} //6.857MHz     TS clock
			else if(input_datarate < 7384)	{tmp1 = 6;tmp2 = 7;} //7.384MHz     TS clock
			else if(input_datarate < 8000)	{tmp1 = 6;tmp2 = 6;} //8MHz     	TS clock
			else if(input_datarate < 8727)	{tmp1 = 5;tmp2 = 6;} //8.727MHz 	TS clock
			else if(input_datarate < 9600)	{tmp1 = 5;tmp2 = 5;} //9.6MHz    	TS clock
			else if(input_datarate < 10666)	{tmp1 = 4;tmp2 = 5;} //10.666MHz 	TS clock
			else if(input_datarate < 12000)	{tmp1 = 4;tmp2 = 4;} //12MHz 	 	TS clock
			else if(input_datarate < 13714)	{tmp1 = 3;tmp2 = 4;} //13.714MHz 	TS clock
			else if(input_datarate < 16000)	{tmp1 = 3;tmp2 = 3;} //16MHz     	TS clock
			else if(input_datarate < 19200)	{tmp1 = 2;tmp2 = 3;} //19.2MHz   	TS clock
			else 							{tmp1 = 2;tmp2 = 2;} //24MHz     	TS clock
			#endif
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			_mt_fe_dmd_ds3k_get_mclk(&MClk_KHz);

			if(ts_mode == MtFeTsOutMode_Serial)
			{
				U32 target_mclk = MClk_KHz;

				input_datarate *= 46;
				input_datarate /= 5;			// input_datarate * 1.15 * 8 = input_datarate * 46 / 5

				if(input_datarate > 72000)
				{
					target_mclk = 96000;
				}
				else
				{
					target_mclk = 72000;
				}

				g_serial_mclk = target_mclk;

				if(target_mclk != MClk_KHz)
				{
					_mt_fe_dmd_set_reg(0x06, 0xe0);
					_mt_fe_dmd_ds3k_set_mclk(target_mclk);
					_mt_fe_dmd_set_reg(0x06, 0x00);
				}

				return MtFeErr_Ok;
			}
			else		// Parallel or CI mode
			{
				g_serial_mclk = input_datarate * 46 / 5;
				if(g_serial_mclk > 72000)
				{
					g_serial_mclk = 96000;
				}
				else
				{
					g_serial_mclk = 72000;
				}

				// Set lower limit of TS output clock for Parallel and CI modes
				if(input_datarate < 6000)
				{
					input_datarate = 6000;
				}

				if(input_datarate != 0)
				{
					divid_ratio = (U16)(MClk_KHz / input_datarate);
				}
				else
				{
					divid_ratio = 0xFF;
				}

				if(divid_ratio > 128)
					divid_ratio = 128;

				if(divid_ratio < 2)
					divid_ratio = 2;

				tmp1 = (U8)(divid_ratio / 2);
				tmp2 = (U8)(divid_ratio / 2);

				if((divid_ratio % 2) != 0)
					tmp2 += 1;
			}
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}
	}


	_mt_fe_dmd_ds3k_set_ts_divide_ratio(g_current_type, tmp1, tmp2);


	return MtFeErr_Ok;
}


/******************************************************************
** Function: _mt_fe_dmd_ds3k_set_ts_out_mode
**
**
** Description:	set the TS ouput mode
**
**
** Inputs:
**
**	Parameter				Type		Description
**	----------------------------------------------------------
**	mode					U8			passed by up level
**
**	MtFeTsOutMode_Serial	1			Serial
**	MtFeTsOutMode_Parallel	2			Parallel
**	MtFeTsOutMode_Common	3			Common Interface
**
** Outputs:		none
**
**
*******************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_set_ts_out_mode(U8 mode)
{
	U8	tmp, tmp1, tmp2, val_0x08, val_0x27;


	_mt_fe_dmd_get_reg(0x08, &val_0x08);

	_mt_fe_dmd_get_reg(0x27, &val_0x27);
	val_0x27 &= ~0x01;
	_mt_fe_dmd_set_reg(0x27, val_0x27);


	/*set DVBS mode*/
	tmp = (U8)(val_0x08 & (~0x04));
	_mt_fe_dmd_set_reg(0x08, tmp);

	if(mode == MtFeTsOutMode_Common)
	{
		//6MHz TS clock output
		tmp1 = 6;
		tmp2 = 6;
	}
	else if(mode == MtFeTsOutMode_Parallel)
	{
		#if (MtFeTSOut_Max_Clock_12_MHz)
			tmp1 = 4;
			tmp2 = 4;
		#elif (MtFeTSOut_Max_Clock_16_MHz)
			tmp1 = 3;
			tmp2 = 3;
		#elif (MtFeTSOut_Max_Clock_19_p_2_MHz)
			tmp1 = 3;
			tmp2 = 2;
		#elif (MtFeTSOut_Max_Clock_24_MHz)
			tmp1 = 2;
			tmp2 = 2;
		#else
			tmp1 = 2;
			tmp2 = 2;
		#endif
	}
	else		// if(mode == MtFeTsOutMode_Serial)
	{
		tmp1 = 0;
		tmp2 = 0;
	}

	_mt_fe_dmd_ds3k_set_ts_divide_ratio(MtFeType_DvbS, tmp1, tmp2);

	if(g_dmd_id == MtFeDmdId_DS300X)
	{
		_mt_fe_dmd_get_reg(0xfd, &tmp);
		if (mode == MtFeTsOutMode_Parallel)
		{
			tmp &= ~0x80;
			tmp &= ~0x40;
		}
		else if (mode == MtFeTsOutMode_Serial)
		{
			tmp &= ~0x80;
			tmp |= 0x40;
		}
		else
		{
			tmp |= 0x80;
			tmp &= ~0x40;
		}
		tmp |= 0x20;
		tmp &= ~0x1f;
		_mt_fe_dmd_set_reg(0xfd, tmp);
	}

	/*set DVBS2 mode*/
	tmp = (U8)(val_0x08|0x04);
	_mt_fe_dmd_set_reg(0x08, tmp);

	if(mode == MtFeTsOutMode_Common)
	{
		//6MHz TS clock output
		tmp1 = 8;
		tmp2 = 8;
	}
	else if(mode == MtFeTsOutMode_Parallel)
	{
		#if (MtFeTSOut_Max_Clock_12_MHz)
			tmp1 = 6;
			tmp2 = 6;
		#elif (MtFeTSOut_Max_Clock_16_MHz)
			tmp1 = 5;
			tmp2 = 4;
		#elif (MtFeTSOut_Max_Clock_19_p_2_MHz)
			/*actually 18MHz in DVBS2 mode*/
			tmp1 = 4;
			tmp2 = 4;
		#elif (MtFeTSOut_Max_Clock_24_MHz)
			tmp1 = 3;
			tmp2 = 3;
		#else
			tmp1 = 2;
			tmp2 = 2;
		#endif
	}
	else	// if(MT_FE_TS_OUTPUT_MODE == MtFeTsOutMode_Serial)
	{
		tmp1 = 0;
		tmp2 = 0;
	}


	_mt_fe_dmd_ds3k_set_ts_divide_ratio(MtFeType_DvbS2, tmp1, tmp2);


	_mt_fe_dmd_get_reg(0xfd, &tmp);
	if (mode == MtFeTsOutMode_Parallel)
	{
		tmp &= ~0x01;
		tmp &= ~0x04;
	}
	else if (mode == MtFeTsOutMode_Serial)
	{
		tmp &= ~0x01;
		tmp |= 0x04;
	}
	else
	{
		tmp |= 0x01;
		tmp &= ~0x04;
	}
	tmp &= ~0xb8;
	tmp |= 0x42;
	_mt_fe_dmd_set_reg(0xfd, tmp);

	_mt_fe_dmd_set_reg(0x08, val_0x08);

	_mt_fe_dmd_get_reg(0x27, &val_0x27);
	val_0x27 |= 0x01;
	if (((MT_FE_ENHANCE_MCLK_LEVEL_PARALLEL_CI != 0) && (mode != MtFeTsOutMode_Serial))
	 || ((MT_FE_ENHANCE_MCLK_LEVEL_SERIAL != 0) && (mode == MtFeTsOutMode_Serial)))
	{
		val_0x27 &= ~0x10;
	}
	else
	{
		val_0x27 |= 0x10;
	}
	_mt_fe_dmd_set_reg(0x27, val_0x27);


	_mt_fe_dmd_get_reg(0x29, &tmp);
	if((MT_FE_TS_SERIAL_PIN_SELECT_D0_D7 != 0) && (mode == MtFeTsOutMode_Serial))
	{
		tmp |= 0x20;
	}
	else
	{
		tmp &= ~0x20;
	}
	_mt_fe_dmd_set_reg(0x29, tmp);


	return MtFeErr_Ok;
}



/*****************************************************************
** Function: _mt_fe_dmd_ds3k_set_CCI
**
**
** Description:	set CCI on/off
**
**
** Inputs:
**
**	Parameter		Type			Description
**	----------------------------------------------------------
**	is_cci_on		MT_FE_ON_OFF	cci on or off
**
** Outputs:
**
**
*****************************************************************/
MT_FE_RET  _mt_fe_dmd_ds3k_set_CCI(MT_FE_ON_OFF is_cci_on)
{
	U8 tmp;


	_mt_fe_dmd_get_reg(0x56, &tmp);
	if (is_cci_on == MtFe_On)
		tmp &= ~0x01;
	else
		tmp |= 0x01;
	_mt_fe_dmd_set_reg(0x56, tmp);


	return MtFeErr_Ok;
}



/*****************************************************************
** Function: _mt_fe_dmd_ds3k_check_CCI
**
**
** Description:	check CCI whether it needs to turn off
**
**
** Inputs:	None
**
**
** Outputs:	None
**
**
*****************************************************************/
MT_FE_RET  _mt_fe_dmd_ds3k_check_CCI(MT_FE_LOCK_STATE *p_state)
{
	U8 i, cnt, reg_0xb3, reg_0x59, CCI_on_flag_cnt;
	S8 val_cci;

	cnt = 0;

	_mt_fe_dmd_get_reg(0xb3, &reg_0xb3);
	while((reg_0xb3 == 0x38)||(reg_0xb3 <= 3))
	{
		cnt ++;

		if (cnt > 10)
		{
			*p_state = MtFeLockState_Unlocked;
			return MtFeErr_Ok;
		}

		_mt_sleep(10);

		_mt_fe_dmd_get_reg(0xb3, &reg_0xb3);
	}


	CCI_on_flag_cnt = 0;
	for(i = 0; i < 15; i++)
	{
		_mt_fe_dmd_get_reg(0x59, &reg_0x59);
		val_cci = (S8)reg_0x59;

		if(val_cci < 0)
		{
			CCI_on_flag_cnt = 0;
			break;
		}

		if(val_cci > MT_FE_CCI_THRESHOLD)
			CCI_on_flag_cnt++;
	}


	if (CCI_on_flag_cnt < 10)
		_mt_fe_dmd_ds3k_set_CCI(MtFe_Off);


	_mt_sleep(1);


	return MtFeErr_Ok;
}


/*****************************************************************************
** Function: _mt_fe_dmd_ds3k_set_demod
**
**
** Description:	set some registers according the param.
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------------------
**	sym_rate_KSs	U8			symbol rate passed by app level
**	type			MT_FE_TYPE	passed by up level
**
**
** Outputs:
**
**
******************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_set_demod(U32 sym_rate_KSs, MT_FE_TYPE type)
{
	MT_FE_RET	ret = MtFeErr_Ok;
	U8			val, tmp, tmp1, tmp2;
	U8			ts_mode = MT_FE_TS_OUTPUT_MODE;
	U32			target_mclk = MT_FE_MCLK_KHZ;
	U32			ts_clk = 24000;
	U16			divide_ratio;


	_mt_fe_dmd_get_reg(0x08, &val);
	if (type == MtFeType_DvbS2)
	{
		val |= 0x04;
		_mt_fe_dmd_set_reg(0x08, val);

		_mt_fe_dmd_ds3k_get_current_ts_mode(&ts_mode);

		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			ret = _mt_fe_dmd_ds3k_init_reg(ds3000_reg_tbl_dvbs2_def, sizeof(ds3000_reg_tbl_dvbs2_def)/2);
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			ret = _mt_fe_dmd_ds3k_init_reg(ds310x_reg_tbl_dvbs2_def, sizeof(ds310x_reg_tbl_dvbs2_def)/2);
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}

		if(ts_mode == MtFeTsOutMode_Common)
		{
			ts_clk = 6000;//8471
		}
		else if(ts_mode == MtFeTsOutMode_Parallel)
		{
			#if (MtFeTSOut_Max_Clock_12_MHz)
			ts_clk = 12000;
			#elif (MtFeTSOut_Max_Clock_16_MHz)
			ts_clk = 16000;
			#elif (MtFeTSOut_Max_Clock_19_p_2_MHz)
			ts_clk = 19200;
			#elif (MtFeTSOut_Max_Clock_24_MHz)
			ts_clk = 24000;
			#else
			ts_clk = 24000;
			#endif
		}
		else	// if(ts_mode == MtFeTsOutMode_Serial)
		{
			ts_clk = 0;
		}

		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			target_mclk = 144000;
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			if(spectrum_inverted == 1)			// 0x4d, bit 1
			{
				_mt_fe_dmd_get_reg(0x4d, &val);
				val |= 0x02;
				_mt_fe_dmd_set_reg(0x4d, val);
			}
			else
			{
				_mt_fe_dmd_get_reg(0x4d, &val);
				val &= ~0x02;
				_mt_fe_dmd_set_reg(0x4d, val);
			}

			if(agc_polarization == 1)			// 0x30, bit 4
			{
				_mt_fe_dmd_get_reg(0x30, &val);
				val |= 0x10;
				_mt_fe_dmd_set_reg(0x30, val);
			}
			else
			{
				_mt_fe_dmd_get_reg(0x30, &val);
				val &= ~0x10;
				_mt_fe_dmd_set_reg(0x30, val);
			}

			if((ts_mode == MtFeTsOutMode_Parallel) || (ts_mode == MtFeTsOutMode_Common))
			{
				if(sym_rate_KSs > 28000)
				{
					target_mclk = 192000;
				}
				else if(sym_rate_KSs > 18000)
				{
					target_mclk = 144000;
				}
				else
				{
					target_mclk = 96000;
				}
			}
			else		// Serial mode
			{
				#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0)
				if(sym_rate_KSs > 18000)
				{
					target_mclk = 144000;
				}
				else
				{
					target_mclk = 96000;
				}
				#else
				target_mclk = MT_FE_MCLK_KHZ_SERIAL_S2;
				#endif
			}
		}

		if(sym_rate_KSs <= 5000)
		{
			_mt_fe_dmd_set_reg(0xc0, 0x04);
			_mt_fe_dmd_set_reg(0x8a, 0x09);
			_mt_fe_dmd_set_reg(0x8b, 0x22);
			_mt_fe_dmd_set_reg(0x8c, 0x88);
		}
	}
	else		// DVB-S
	{
		val &= ~0x04;
		_mt_fe_dmd_set_reg(0x08, val);

		_mt_fe_dmd_ds3k_get_current_ts_mode(&ts_mode);

		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			ret = _mt_fe_dmd_ds3k_init_reg(ds3000_reg_tbl_dvbs_def, sizeof(ds3000_reg_tbl_dvbs_def)/2);
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			ret = _mt_fe_dmd_ds3k_init_reg(ds310x_reg_tbl_dvbs_def, sizeof(ds310x_reg_tbl_dvbs_def)/2);
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}

		if(ts_mode == MtFeTsOutMode_Common)
		{
			ts_clk = 6000;//8000
		}
		else if(ts_mode == MtFeTsOutMode_Parallel)
		{
			#if (MtFeTSOut_Max_Clock_12_MHz)
			ts_clk = 12000;
			#elif (MtFeTSOut_Max_Clock_16_MHz)
			ts_clk = 16000;
			#elif (MtFeTSOut_Max_Clock_19_p_2_MHz)
			ts_clk = 19200;
			#elif (MtFeTSOut_Max_Clock_24_MHz)
			ts_clk = 24000;
			#else
			ts_clk = 24000;
			#endif
		}
		else	// if(ts_mode == MtFeTsOutMode_Serial)
		{
			ts_clk = 0;
		}

		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			target_mclk = 96000;
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			if(spectrum_inverted == 1)			// 0x4d, bit 1
			{
				_mt_fe_dmd_get_reg(0x4d, &val);
				val |= 0x02;
				_mt_fe_dmd_set_reg(0x4d, val);
			}
			else
			{
				_mt_fe_dmd_get_reg(0x4d, &val);
				val &= ~0x02;
				_mt_fe_dmd_set_reg(0x4d, val);
			}

			if(agc_polarization == 1)			// 0x30, bit 4
			{
				_mt_fe_dmd_get_reg(0x30, &val);
				val |= 0x10;
				_mt_fe_dmd_set_reg(0x30, val);
			}
			else
			{
				_mt_fe_dmd_get_reg(0x30, &val);
				val &= ~0x10;
				_mt_fe_dmd_set_reg(0x30, val);
			}

			target_mclk = 96000;
		}
	}

	if(ts_clk != 0)
	{
		divide_ratio = (target_mclk + ts_clk - 1) / ts_clk;

		if(divide_ratio > 128)
			divide_ratio = 128;

		if(divide_ratio < 2)
			divide_ratio = 2;

		tmp1 = (U8)(divide_ratio / 2);
		tmp2 = (U8)(divide_ratio / 2);

		if((divide_ratio % 2) != 0)
			tmp2 += 1;
	}
	else
	{
		divide_ratio = 0;
		tmp1 = 0;
		tmp2 = 0;
	}

	//if((tmp1 + tmp2) != 0)
	//{
	//	ts_clk = target_mclk / (tmp1 + tmp2);
	//}

	_mt_fe_dmd_ds3k_set_ts_divide_ratio(type, tmp1, tmp2);
	_mt_fe_dmd_ds3k_set_mclk(target_mclk);


	_mt_fe_dmd_get_reg(0xfd, &val);
	_mt_fe_dmd_get_reg(0xca, &tmp);
	tmp &= 0xFE;
	tmp |= (val >> 3) & 0x01;
	_mt_fe_dmd_set_reg(0xca, tmp);


#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
	_mt_fe_dmd_set_reg(0x33, 0x38);
#elif MT_FE_TN_SUPPORT_TS2022
	_mt_fe_dmd_set_reg(0x33, 0x99);
#elif MT_FE_TN_SUPPORT_AV2026
	_mt_fe_dmd_set_reg(0x33, 0x58);
#elif MT_FE_TN_SUPPORT_RAON_MTV600
	_mt_fe_dmd_set_reg(0x33, 0x5a);
#else
	_mt_fe_dmd_set_reg(0x33, 0x35);
#endif

#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
	_mt_fe_dmd_set_reg(0x36, 0x10);
	_mt_fe_dmd_set_reg(0x39, 0x00);
#elif MT_FE_TN_SUPPORT_ST6110
	_mt_fe_dmd_set_reg(0x36, 0x07);
	_mt_fe_dmd_set_reg(0x39, 0x00);
#endif

#if MT_FE_ENABLE_27MHZ_CLOCK_OUT
	_mt_fe_dmd_get_reg(0x29, &tmp);
	tmp &= ~0x80;
	_mt_fe_dmd_set_reg(0x29, tmp);
#else
	_mt_fe_dmd_get_reg(0x29, &tmp);
	tmp |= 0x80;
	_mt_fe_dmd_set_reg(0x29, tmp);
#endif

#if MT_FE_ENABLE_13_P_5_MHZ_CLOCK_OUT
	_mt_fe_dmd_get_reg(0x29, &tmp);
	tmp |= 0x10;
	_mt_fe_dmd_set_reg(0x29, tmp);
#else
	_mt_fe_dmd_get_reg(0x29, &tmp);
	tmp &= ~0x10;
	_mt_fe_dmd_set_reg(0x29, tmp);
#endif

#if MT_FE_ENABLE_AC_COUPLING
	_mt_fe_dmd_get_reg(0x25, &tmp);
	tmp |= 0x08;
	_mt_fe_dmd_set_reg(0x25, tmp);
#endif

	_mt_fe_dmd_get_reg(0x27, &tmp);
	if (((MT_FE_ENHANCE_MCLK_LEVEL_PARALLEL_CI != 0) && (ts_mode != MtFeTsOutMode_Serial))
	 || ((MT_FE_ENHANCE_MCLK_LEVEL_SERIAL != 0) && (ts_mode == MtFeTsOutMode_Serial)))
	{
		tmp &= ~0x10;
	}
	else
	{
		tmp |= 0x10;
	}
	_mt_fe_dmd_set_reg(0x27, tmp);


	if(sym_rate_KSs <= 3000)
	{
		_mt_fe_dmd_set_reg(0xc3, 0x08); // 8 * 32 * 100 / 64 = 400
		_mt_fe_dmd_set_reg(0xc8, 0x20);
		_mt_fe_dmd_set_reg(0xc4, 0x08); // 8 * 0 * 100 / 128 = 0
		_mt_fe_dmd_set_reg(0xc7, 0x00);
	}
	else if(sym_rate_KSs <= 10000)
	{
		_mt_fe_dmd_set_reg(0xc3, 0x08); // 8 * 16 * 100 / 64 = 200
		_mt_fe_dmd_set_reg(0xc8, 0x10);
		_mt_fe_dmd_set_reg(0xc4, 0x08); // 8 * 0 * 100 / 128 = 0
		_mt_fe_dmd_set_reg(0xc7, 0x00);
	}
	else
	{
		_mt_fe_dmd_set_reg(0xc3, 0x08); // 8 * 6 * 100 / 64 = 75
		_mt_fe_dmd_set_reg(0xc8, 0x06);
		_mt_fe_dmd_set_reg(0xc4, 0x08); // 8 * 0 * 100 / 128 = 0
		_mt_fe_dmd_set_reg(0xc7, 0x00);
	}

	_mt_fe_dmd_ds3k_set_sym_rate(sym_rate_KSs);

	_mt_fe_dmd_ds3k_set_CCI(MtFe_On);
	_mt_fe_dmd_get_reg(0x76, &val);
	val &= ~0x80;
	_mt_fe_dmd_set_reg(0x76, val);


	return ret;
}



/*****************************************************************************
** Function: _mt_fe_dmd_ds3k_set_sym_rate
**
**
** Description:	set symbol rate
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------------------
**	sym_rate_KSs	U8			symbol rate passed by app level
**
**
** Outputs:
**
**
*******************************************************************************/
MT_FE_RET  _mt_fe_dmd_ds3k_set_sym_rate(U32 sym_rate_KSs)
{
	U32	tmp;
	U8	reg_0x61, reg_0x62;

	tmp = (U32)(((sym_rate_KSs << 15) + (MT_FE_MCLK_KHZ / 4)) / (MT_FE_MCLK_KHZ / 2));

	reg_0x61 = (U8)(tmp & 0x00FF);
	reg_0x62 = (U8)((tmp & 0xFF00) >> 8);

	_mt_fe_dmd_set_reg(0x61, reg_0x61);
	_mt_fe_dmd_set_reg(0x62, reg_0x62);


	return MtFeErr_Ok;
}



/*****************************************************************************
** Function: _mt_fe_dmd_ds3k_get_sym_rate
**
**
** Description:	get symbol rate from register
**
**
** Inputs:	none
**
**
**
** Outputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------------------
**	sym_rate_KSs	U32*		symbol rate read from register
**
**
*******************************************************************************/
 MT_FE_RET _mt_fe_dmd_ds3k_get_sym_rate(U32 *sym_rate_KSs)
{
	U16	tmp;
	U32	sym_rate_tmp;
	U8	val_0x6d, val_0x6e;


	_mt_fe_dmd_get_reg(0x6d, &val_0x6d);
	_mt_fe_dmd_get_reg(0x6e, &val_0x6e);

	tmp = (U16)((val_0x6e<<8) | val_0x6d);


	sym_rate_tmp = (U32)(tmp * MT_FE_MCLK_KHZ);
	sym_rate_tmp = (U32)(sym_rate_tmp / (1<<16));

	*sym_rate_KSs = sym_rate_tmp;


	return MtFeErr_Ok;
}



/*************************************************************************
** Function:  _mt_fe_dmd_ds3k_set_carrier_offset
**
**
** Description:	set blind scan registers
**
**
** Inputs:
**
**	Parameter				Type		Description
**	----------------------------------------------------------------------
**	carrier_offset_KHz		U32			carrier offset
**
**
** Outputs:		none
**
**
*************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_set_carrier_offset(S32 carrier_offset_KHz)
{
	S32 tmp;


	tmp = carrier_offset_KHz;
	tmp *= 65536;

	tmp = (S32)((2*tmp + MT_FE_MCLK_KHZ)/(2*MT_FE_MCLK_KHZ));

	if(tmp < 0)
		tmp += 65536;


	_mt_fe_dmd_set_reg(0x5f, (U8)(tmp>>8));
	_mt_fe_dmd_set_reg(0x5e, (U8)(tmp&0xFF));

	g_carrier_offset = carrier_offset_KHz;

	return MtFeErr_Ok;
}


/***********************************************************************
** Function: _mt_fe_dmd_ds3k_get_carrier_offset
**
**
** Description:	get the carrier offset
**
**
** Inputs:	none
**
**
** Outputs:
**
**	Parameter				Type		Description
**	--------------------------------------------------------------------
**	carrier_offset_KHz		S32*		carrier offset value
**
**
*************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_get_carrier_offset(S32 *carrier_offset_KHz)
{
	S32 val;
	U8	tmp, val_0x5e, val_0x5f;


	_mt_fe_dmd_get_reg(0x5d, &tmp);
	tmp &= 0xf8;
	_mt_fe_dmd_set_reg(0x5d, tmp);

	_mt_fe_dmd_get_reg(0x5e, &val_0x5e);
	_mt_fe_dmd_get_reg(0x5f, &val_0x5f);
	val = (val_0x5f<<8) | val_0x5e;


	if((val&0x8000) == 0x8000)
		val -= (1<<16);


	*carrier_offset_KHz = (S32)((val * MT_FE_MCLK_KHZ) / (1<<16));

	return MtFeErr_Ok;
}


/***********************************************************************
** Function: _mt_fe_dmd_ds3k_get_total_carrier_offset
**
**
** Description:	get the total carrier offset
**
**
** Inputs:	none
**
**
** Outputs:
**
**	Parameter				Type		Description
**	--------------------------------------------------------------------
**	carrier_offset_KHz		S32*		carrier offset value
**
**
*************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_get_total_carrier_offset(S32 *carrier_offset_KHz)
{
	U8	tmp, val_0x5e, val_0x5f;
	S32 val_0x5e_0x5f_1, val_0x5e_0x5f_2, nval_1, nval_2;

	_mt_fe_dmd_get_reg(0x5d, &tmp);
	tmp &= 0xf8;
	_mt_fe_dmd_set_reg(0x5d, tmp);

	_mt_fe_dmd_get_reg(0x5e, &val_0x5e);
	_mt_fe_dmd_get_reg(0x5f, &val_0x5f);
	val_0x5e_0x5f_1 = (val_0x5f<<8) | val_0x5e;

	tmp |= 0x06;
	_mt_fe_dmd_set_reg(0x5d, tmp);

	_mt_fe_dmd_get_reg(0x5e, &val_0x5e);
	_mt_fe_dmd_get_reg(0x5f, &val_0x5f);
	val_0x5e_0x5f_2 = (val_0x5f<<8) | val_0x5e;


	if(((val_0x5e_0x5f_1>>15)&0x01) == 0x01)
		nval_1 = val_0x5e_0x5f_1 - (1<<16);
	else
		nval_1 = val_0x5e_0x5f_1;

	if(((val_0x5e_0x5f_2>>15)&0x01) == 0x01)
		nval_2 = val_0x5e_0x5f_2 - (1<<16);
	else
		nval_2 = val_0x5e_0x5f_2;

	*carrier_offset_KHz = (nval_1 - nval_2) * MT_FE_MCLK_KHZ / (1<<16);

	return MtFeErr_Ok;
}



/*************************************************************************
** Function:  _mt_fe_dmd_ds3k_get_fec
**
**
** Description:	get code rate
**
**
** Inputs:
**
**	Parameter			Type				Description
**	----------------------------------------------------------------------
**	tp_type				MT_FE_TYPE			DVBS/DVBS2
**
**
** Outputs:
**
**	Parameter			Type				Description
**	----------------------------------------------------------------------
**	p_info				MT_FE_TP_INFO*		structure pointer
**
**
*************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_get_fec(MT_FE_TP_INFO *p_info, MT_FE_TYPE tp_type)
{
	U8 tmp, val_0x7E;


	if(tp_type == MtFeType_DvbS2)
	{
		_mt_fe_dmd_get_reg(0x7e, &val_0x7E);
		tmp = (U8)(val_0x7E & 0x0f);
		switch(tmp)
		{
			case 0:  p_info->code_rate = MtFeCodeRate_1_4;		break;
			case 1:  p_info->code_rate = MtFeCodeRate_1_3;		break;
			case 2:  p_info->code_rate = MtFeCodeRate_2_5;		break;
			case 3:  p_info->code_rate = MtFeCodeRate_1_2;		break;
			case 4:  p_info->code_rate = MtFeCodeRate_3_5;		break;
			case 5:  p_info->code_rate = MtFeCodeRate_2_3;		break;
			case 6:  p_info->code_rate = MtFeCodeRate_3_4;		break;
			case 7:  p_info->code_rate = MtFeCodeRate_4_5;		break;
			case 8:  p_info->code_rate = MtFeCodeRate_5_6;		break;
			case 9:  p_info->code_rate = MtFeCodeRate_8_9;		break;
			case 10: p_info->code_rate = MtFeCodeRate_9_10;		break;
			default: p_info->code_rate = MtFeCodeRate_Undef;	break;
		}
	}
	else
	{
		_mt_fe_dmd_get_reg(0xe6, &tmp);
		tmp = (U8)(tmp >> 5);
		switch(tmp)
		{
			case 0:  p_info->code_rate = MtFeCodeRate_7_8;		break;
			case 1:  p_info->code_rate = MtFeCodeRate_5_6;		break;
			case 2:  p_info->code_rate = MtFeCodeRate_3_4;		break;
			case 3:  p_info->code_rate = MtFeCodeRate_2_3;		break;
			case 4:  p_info->code_rate = MtFeCodeRate_1_2;		break;
			default: p_info->code_rate = MtFeCodeRate_Undef;	break;
		}
	}

	return MtFeErr_Ok;
}



/*****************************************************************************
** Function: _mt_fe_dmd_ds3k_get_current_ts_mode
**
**
** Description:	get current TS output mode from register
**
**
** Inputs:	none
**
**
**
** Outputs:
**
**	Parameter					Type		Description
**	----------------------------------------------------------------------
**	p_ts_mode					U8*			Index of current TS output mode.
**		MtFeTsOutMode_Serial				1
**		MtFeTsOutMode_Parallel				2
**		MtFeTsOutMode_Common				3
**
**
*******************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_get_current_ts_mode(U8 *p_ts_mode)
{
	U8 val_0xfd;

	*p_ts_mode = MT_FE_TS_OUTPUT_MODE;

	if(g_dmd_id == MtFeDmdId_DS300X)
	{
		_mt_fe_dmd_get_reg(0xfd, &val_0xfd);

		if(g_current_type == MtFeType_DvbS)
		{
			if((val_0xfd & 0x80) == 0x00)
			{
				*p_ts_mode = ((val_0xfd & 0x40) == 0) ? MtFeTsOutMode_Parallel : MtFeTsOutMode_Serial;
			}
			else
			{
				*p_ts_mode = MtFeTsOutMode_Common;
			}
		}
		else		// DVB-S2
		{
			if((val_0xfd & 0x01) == 0x00)
			{
				*p_ts_mode = ((val_0xfd & 0x04) == 0) ? MtFeTsOutMode_Parallel : MtFeTsOutMode_Serial;
			}
			else
			{
				*p_ts_mode = MtFeTsOutMode_Common;
			}
		}
	}
	else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_get_reg(0xfd, &val_0xfd);

		if((val_0xfd & 0x01) == 0x00)
		{
			*p_ts_mode = ((val_0xfd & 0x04) == 0) ? MtFeTsOutMode_Parallel : MtFeTsOutMode_Serial;
		}
		else
		{
			*p_ts_mode = MtFeTsOutMode_Common;
		}
	}
	else
	{
		return MtFeErr_NoSupportDemod;
	}

	return MtFeErr_Ok;
}


/**********************************************************************
** Function: _mt_fe_dmd_ds3k_init_reg
**
**
** Description:	initialize the register of DS3103/DS3002B/DS300X
**
**
** Inputs:
*
**	Parameter		Type				Description
**	---------------------------------------------------------------
**	p_reg_tabl		const U8 *[2]		pointer to the array
**	size			S32					the array size
**
** Outputs:
**
**
***********************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_init_reg(const U8 (*p_reg_tabl)[2], S32 size)
{
	MT_FE_RET	ret;
	U32			i;


	for (i=0; i < (U32)size; i++)
	{
		ret = _mt_fe_dmd_set_reg(p_reg_tabl[i][0], p_reg_tabl[i][1]);

		if (ret != MtFeErr_Ok)
			return ret;
	}


	return MtFeErr_Ok;
}



/*******************************************************************
** Function: _mt_fe_dmd_ds3k_download_fw
**
**
** Description:	download the mcu code to DS3103/DS3002B/DS300X
**
**
** Inputs:
**
**	Parameter	Type		Description
**	------------------------------------------------------------
**	p_fw		const U8*	pointer of the mcu array
**
** Outputs:
**
**
********************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_download_fw(const U8* p_fw)
{
	U32 i;
	MT_FE_RET	ret = MtFeErr_Ok;

	_mt_fe_dmd_set_reg(0xb2, 0x01);

	for(i = 0; i < 256; i++)
	{
		ret = _mt_fe_write_fw(0xb0, (U8*)&(p_fw[32*i]), 32);
		if (ret != MtFeErr_Ok)
		{
			break;
		}
	}

/*
	for(i = 0; i < 128; i++)
	{
		ret = _mt_fe_write_fw(0xb0, (U8*)&(p_fw[64*i]), 64);
		if (ret != MtFeErr_Ok)
		{
			break;
		}
	}
*/

	_mt_fe_dmd_set_reg(0xb2, 0x00);

	if (ret != MtFeErr_Ok)
		return MtFeErr_I2cErr;


	return MtFeErr_Ok;
}


/*************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_set_reg
**
**
** Description:	set blind scan registers
**
**
** Inputs:
**
**	Parameter		type				description
**	---------------------------------------------------------------------------
**	bs_times		U8					blind scan times
**
**
** Outputs:		none
**
**
*************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_set_reg(U8 bs_times)
{
	U8 tmp, sm_buf;

	switch(bs_times)
	{
		case MT_FE_BS_TIMES_1ST:
			if(g_bs_time_total == 1)
				sm_buf = ONE_LOOP_SM_BUF;
			else if(g_bs_time_total == 2)
				sm_buf = TWO_LOOP_SM_BUF_1ST;
			else
				sm_buf = SM_BUF_1ST;
		break;

		case MT_FE_BS_TIMES_2ND:
			sm_buf = (U8)((g_bs_time_total == 2) ? TWO_LOOP_SM_BUF_2ND: SM_BUF_2ND);
		break;

		case MT_FE_BS_TIMES_3RD:
			sm_buf = SM_BUF_3RD;
		break;

		default:
			sm_buf = ONE_LOOP_SM_BUF;
		break;
	}

	_mt_fe_dmd_set_reg(0xB2, 0x01);
	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_set_reg(0x00, 0x01);
	}


	_mt_fe_dmd_set_reg(0x4A, 0x00);


	_mt_fe_dmd_get_reg(0x4D, &tmp);
	tmp |= 0x91;
	_mt_fe_dmd_set_reg(0x4D, tmp);


	_mt_fe_dmd_get_reg(0x90, &tmp);
	tmp |= TMP1;
	_mt_fe_dmd_set_reg(0x90, tmp);


	if((sm_buf == SM_BUF_1ST) || (sm_buf == TWO_LOOP_SM_BUF_1ST))
		tmp = 128 + TMP2;
	else
		tmp = TMP2;

	_mt_fe_dmd_set_reg(0x91, tmp);

	tmp = (U8)(TMP3 + sm_buf - 1);
	_mt_fe_dmd_set_reg(0x92, tmp);


	_mt_fe_dmd_get_reg(0x93, &tmp);
	tmp &= 0x0F;
	tmp |= TMP4;
	_mt_fe_dmd_set_reg(0x93, tmp);



	_mt_fe_dmd_get_reg(0x94, &tmp);
	switch(bs_times)
	{
		case MT_FE_BS_TIMES_1ST:
			if(g_bs_time_total == 1)
				tmp |= (FIND_NOTCH_STEP*16+THR_FACTOR_ONE_LOOP);
			else if(g_bs_time_total == 2)
				tmp |= (FIND_NOTCH_STEP*16+TWO_LOOP_THR_FACTOR_1ST);
			else
				tmp |= (FIND_NOTCH_STEP*16+THR_FACTOR_1ST);
		break;

		case MT_FE_BS_TIMES_2ND:
			tmp |= (g_bs_time_total == 2) ? (FIND_NOTCH_STEP*16+TWO_LOOP_THR_FACTOR_2ND) : (FIND_NOTCH_STEP*16+THR_FACTOR_2ND);
		break;

		case MT_FE_BS_TIMES_3RD:
			tmp |= (FIND_NOTCH_STEP*16+THR_FACTOR_3RD);
		break;

		default:
			tmp |= (FIND_NOTCH_STEP*16+THR_FACTOR_ONE_LOOP);
		break;
	}
	_mt_fe_dmd_set_reg(0x94, tmp);


	switch(bs_times)
	{
		case MT_FE_BS_TIMES_1ST:
			if(g_bs_time_total == 1)
				tmp = 64 + ERR_BOUND_FACTOR_ONE_LOOP;
			else if(g_bs_time_total == 2)
				tmp = 64 + TWO_LOOP_ERR_BOUND_FACTOR_1ST;
			else
				tmp = 64 + ERR_BOUND_FACTOR_1ST;
		break;

		case MT_FE_BS_TIMES_2ND:
			tmp = (U8)((g_bs_time_total == 2) ? (64+TWO_LOOP_ERR_BOUND_FACTOR_2ND) : (64+ERR_BOUND_FACTOR_2ND));
		break;

		case MT_FE_BS_TIMES_3RD:
			tmp = 64+ERR_BOUND_FACTOR_3RD;
		break;

		default:
			tmp = 64+ERR_BOUND_FACTOR_ONE_LOOP;
		break;
	}
	_mt_fe_dmd_set_reg(0x95, tmp);


	switch(bs_times)
	{
		case MT_FE_BS_TIMES_1ST:
			if(g_bs_time_total == 1)
				tmp = TUNER_SLIP_STEP*16 + SNR_THR_ONE_LOOP/16 - 1;
			else if(g_bs_time_total == 2)
				tmp = TUNER_SLIP_STEP*16 + TWO_LOOP_SNR_THR_1ST/16 - 1;
			else
				tmp = TUNER_SLIP_STEP*16 + SNR_THR_1ST/16 - 1;
		break;

		case MT_FE_BS_TIMES_2ND:
			tmp = (U8)((g_bs_time_total == 2) ? (TUNER_SLIP_STEP*16 + TWO_LOOP_SNR_THR_2ND/16 - 1) : (TUNER_SLIP_STEP*16 + SNR_THR_2ND/16 - 1));
		break;

		case MT_FE_BS_TIMES_3RD:
			tmp = TUNER_SLIP_STEP*16 + SNR_THR_3RD/16 - 1;
		break;

		default:
			tmp = TUNER_SLIP_STEP*16 + SNR_THR_ONE_LOOP/16 - 1;
		break;
	}
	_mt_fe_dmd_set_reg(0x97, tmp);


	switch(bs_times)
	{
		case MT_FE_BS_TIMES_1ST:
			if(g_bs_time_total == 1)
				tmp = FLAT_THR_ONE_LOOP;
			else if(g_bs_time_total == 2)
				tmp = TWO_LOOP_FLAT_THR_1ST;
			else
				tmp = FLAT_THR_1ST;
		break;

		case MT_FE_BS_TIMES_2ND:
			tmp = (U8)((g_bs_time_total == 2) ? TWO_LOOP_FLAT_THR_2ND : FLAT_THR_2ND);
		break;

		case MT_FE_BS_TIMES_3RD:
			tmp = FLAT_THR_3RD;
		break;

		default:
			tmp = FLAT_THR_ONE_LOOP;
		break;
	}
	_mt_fe_dmd_set_reg(0x99, tmp);


	if(g_dmd_id == MtFeDmdId_DS300X)
	{
		_mt_fe_dmd_set_reg(0x30, 0x08);
	}
	else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		if(agc_polarization == 1)			// 0x30, bit 4
		{
			_mt_fe_dmd_set_reg(0x30, 0x18);
		}
		else
		{
			_mt_fe_dmd_set_reg(0x30, 0x08);
		}
	}
	else
	{
		return MtFeErr_NoSupportDemod;
	}

	_mt_fe_dmd_set_reg(0x32, 0x44);
#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
	_mt_fe_dmd_set_reg(0x33, 0x38);
#elif MT_FE_TN_SUPPORT_TS2022
	_mt_fe_dmd_set_reg(0x33, 0x99);
#elif MT_FE_TN_SUPPORT_AV2011
	_mt_fe_dmd_set_reg(0x33, 0x38);
#elif MT_FE_TN_SUPPORT_AV2026
	_mt_fe_dmd_set_reg(0x33, 0x58);
#elif MT_FE_TN_SUPPORT_RAON_MTV600
	_mt_fe_dmd_set_reg(0x33, 0xcd);
#else
	_mt_fe_dmd_set_reg(0x33, 0x35);
#endif

#if (MT_FE_TN_SUPPORT_AV2011 != 0)
	_mt_fe_dmd_set_reg(0x35, 0xFF);
#else
	_mt_fe_dmd_set_reg(0x35, 0x7F);
#endif
	_mt_fe_dmd_set_reg(0x4B, 0x04);
	_mt_fe_dmd_set_reg(0x56, 0x01);
	_mt_fe_dmd_set_reg(0xA0, 0x44);
	_mt_fe_dmd_set_reg(0x08, 0x83);

#if MT_FE_ENABLE_AC_COUPLING
	_mt_fe_dmd_get_reg(0x25, &tmp);
	tmp |= 0x08;
	_mt_fe_dmd_set_reg(0x25, tmp);
#endif

	_mt_fe_dmd_set_reg(0x70, 0x00);
	if(g_dmd_id == MtFeDmdId_DS300X)
	{
		_mt_fe_dmd_set_reg(0x05, 0x04);
		_mt_fe_dmd_set_reg(0x06, 0x08);
		_mt_fe_dmd_set_reg(0x06, 0x00);
		_mt_fe_dmd_set_reg(0x05, 0x00);
	}
	else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		if(spectrum_inverted == 1)			// 0x4d, bit 1
		{
			_mt_fe_dmd_get_reg(0x4d, &tmp);
			tmp |= 0x02;
			_mt_fe_dmd_set_reg(0x4d, tmp);
		}
		else
		{
			_mt_fe_dmd_get_reg(0x4d, &tmp);
			tmp &= ~0x02;
			_mt_fe_dmd_set_reg(0x4d, tmp);
		}

		_mt_fe_dmd_set_reg(0x00, 0x00);
	}
	else
	{
		return MtFeErr_NoSupportDemod;
	}

	_mt_fe_dmd_set_reg(0xB2, 0x00);

	return MtFeErr_Ok;
}



/*****************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_set_demod
**
**
** Description:	set demod registers in blind scan mode
**
**
** Inputs:
**
**	Parameter				type				description
**	---------------------------------------------------------------------------
**	sym_rate_KSs			U32					symbol rate passed by app level
**	type					MT_FE_TYPE			passed by up level
**
**
** Outputs:
**
**
******************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_set_demod(U32 sym_rate_KSs, MT_FE_TYPE type)
{
	U8			tmp1, tmp2, val, tmp;
	MT_FE_RET	ret = MtFeErr_Ok;
	U8			ts_mode = MT_FE_TS_OUTPUT_MODE;
	U32			target_mclk = MT_FE_MCLK_KHZ;
	U32			ts_clk = 24000;
	U16			divide_ratio;


	if(type == MtFeType_DvbS2)
	{
		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			ret = _mt_fe_dmd_ds3k_init_reg(ds3000_reg_tbl_dvbs2_def_blindscan, sizeof(ds3000_reg_tbl_dvbs2_def_blindscan)/2);
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			ret = _mt_fe_dmd_ds3k_init_reg(ds310x_reg_tbl_dvbs2_def_blindscan, sizeof(ds310x_reg_tbl_dvbs2_def_blindscan)/2);
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}

		//_mt_fe_dmd_set_reg(0xc6, 0x40);

		_mt_fe_dmd_get_reg(0x08, &val);
		val |= 0x04;
		_mt_fe_dmd_set_reg(0x08, val);


		_mt_fe_dmd_ds3k_get_current_ts_mode(&ts_mode);


		if(ts_mode == MtFeTsOutMode_Common)
		{
			ts_clk = 6000;//8471
		}
		else if(ts_mode == MtFeTsOutMode_Parallel)
		{
			#if (MtFeTSOut_Max_Clock_12_MHz)
			ts_clk = 12000;
			#elif (MtFeTSOut_Max_Clock_16_MHz)
			ts_clk = 16000;
			#elif (MtFeTSOut_Max_Clock_19_p_2_MHz)
			ts_clk = 19200;
			#elif (MtFeTSOut_Max_Clock_24_MHz)
			ts_clk = 24000;
			#else
			ts_clk = 24000;
			#endif
		}
		else	// if(ts_mode == MtFeTsOutMode_Serial)
		{
			ts_clk = 0;
		}


		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			target_mclk = 144000;
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			if(spectrum_inverted == 1)			// 0x4d, bit 1
			{
				_mt_fe_dmd_get_reg(0x4d, &val);
				val |= 0x02;
				_mt_fe_dmd_set_reg(0x4d, val);
			}
			else
			{
				_mt_fe_dmd_get_reg(0x4d, &val);
				val &= ~0x02;
				_mt_fe_dmd_set_reg(0x4d, val);
			}

			if(agc_polarization == 1)			// 0x30, bit 4
			{
				_mt_fe_dmd_get_reg(0x30, &val);
				val |= 0x10;
				_mt_fe_dmd_set_reg(0x30, val);
			}
			else
			{
				_mt_fe_dmd_get_reg(0x30, &val);
				val &= ~0x10;
				_mt_fe_dmd_set_reg(0x30, val);
			}


			if((ts_mode == MtFeTsOutMode_Parallel) || (ts_mode == MtFeTsOutMode_Common))
			{
				if(sym_rate_KSs > 28000)
				{
					target_mclk = 192000;
				}
				else if(sym_rate_KSs > 18000)
				{
					target_mclk = 144000;
				}
				else
				{
					target_mclk = 96000;
				}
			}
			else		// Serial
			{
				#if (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0)
				if(sym_rate_KSs > 18000)
				{
					target_mclk = 144000;
				}
				else
				{
					target_mclk = 96000;
				}
				#else
				target_mclk = MT_FE_MCLK_KHZ_SERIAL_S2;
				#endif
			}
		}
	}
	else
	{
		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			ret = _mt_fe_dmd_ds3k_init_reg(ds3000_reg_tbl_dvbs_def_blindscan, sizeof(ds3000_reg_tbl_dvbs_def_blindscan)/2);
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			ret = _mt_fe_dmd_ds3k_init_reg(ds310x_reg_tbl_dvbs_def_blindscan, sizeof(ds310x_reg_tbl_dvbs_def_blindscan)/2);
		}
		else
		{
			return MtFeErr_NoSupportDemod;
		}

		_mt_fe_dmd_get_reg(0x08, &val);
		val &= ~0x04;
		_mt_fe_dmd_set_reg(0x08, val);


		_mt_fe_dmd_ds3k_get_current_ts_mode(&ts_mode);


		if(ts_mode == MtFeTsOutMode_Common)
		{
			ts_clk = 6000;//8000
		}
		else if(ts_mode == MtFeTsOutMode_Parallel)
		{
			#if (MtFeTSOut_Max_Clock_12_MHz)
			ts_clk = 12000;
			#elif (MtFeTSOut_Max_Clock_16_MHz)
			ts_clk = 16000;
			#elif (MtFeTSOut_Max_Clock_19_p_2_MHz)
			ts_clk = 19200;
			#elif (MtFeTSOut_Max_Clock_24_MHz)
			ts_clk = 24000;
			#else
			ts_clk = 24000;
			#endif
		}
		else	// if(ts_mode == MtFeTsOutMode_Serial)
		{
			ts_clk = 0;
		}

		if(g_dmd_id == MtFeDmdId_DS300X)
		{
			target_mclk = 96000;
		}
		else if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
		{
			if(spectrum_inverted == 1)			// 0x4d, bit 1
			{
				_mt_fe_dmd_get_reg(0x4d, &val);
				val |= 0x02;
				_mt_fe_dmd_set_reg(0x4d, val);
			}
			else
			{
				_mt_fe_dmd_get_reg(0x4d, &val);
				val &= ~0x02;
				_mt_fe_dmd_set_reg(0x4d, val);
			}

			if(agc_polarization == 1)			// 0x30, bit 4
			{
				_mt_fe_dmd_get_reg(0x30, &val);
				val |= 0x10;
				_mt_fe_dmd_set_reg(0x30, val);
			}
			else
			{
				_mt_fe_dmd_get_reg(0x30, &val);
				val &= ~0x10;
				_mt_fe_dmd_set_reg(0x30, val);
			}

			target_mclk = 96000;
		}
	}

	if(ts_clk != 0)
	{
		divide_ratio = (target_mclk + ts_clk - 1) / ts_clk;

		if(divide_ratio > 128)
			divide_ratio = 128;

		if(divide_ratio < 2)
			divide_ratio = 2;

		tmp1 = (U8)(divide_ratio / 2);
		tmp2 = (U8)(divide_ratio / 2);

		if((divide_ratio % 2) != 0)
			tmp2 += 1;
	}
	else
	{
		divide_ratio = 0;
		tmp1 = 0;
		tmp2 = 0;
	}


	//if((tmp1 + tmp2) != 0)
	//{
	//	ts_clk = target_mclk / (tmp1 + tmp2);
	//}

	_mt_fe_dmd_ds3k_set_ts_divide_ratio(type, tmp1, tmp2);
	_mt_fe_dmd_ds3k_set_mclk(target_mclk);


	_mt_fe_dmd_get_reg(0xfd, &val);
	_mt_fe_dmd_get_reg(0xca, &tmp);
	tmp &= 0xFE;
	tmp |= (val >> 3) & 0x01;
	_mt_fe_dmd_set_reg(0xca, tmp);


#if MT_FE_ENABLE_AC_COUPLING
	_mt_fe_dmd_get_reg(0x25, &val);
	val |= 0x08;
	_mt_fe_dmd_set_reg(0x25, val);
#endif

	_mt_fe_dmd_get_reg(0x27, &tmp);
	if (((MT_FE_ENHANCE_MCLK_LEVEL_PARALLEL_CI != 0) && (ts_mode != MtFeTsOutMode_Serial))
	 || ((MT_FE_ENHANCE_MCLK_LEVEL_SERIAL != 0) && (ts_mode == MtFeTsOutMode_Serial)))
	{
		tmp &= ~0x10;
	}
	else
	{
		tmp |= 0x10;
	}

	_mt_fe_dmd_set_reg(0x27, tmp);

#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
	_mt_fe_dmd_set_reg(0x33, 0x38);
#elif MT_FE_TN_SUPPORT_TS2022
	_mt_fe_dmd_set_reg(0x33, 0x99);
#elif MT_FE_TN_SUPPORT_AV2026
	_mt_fe_dmd_set_reg(0x33, 0x58);
#elif MT_FE_TN_SUPPORT_RAON_MTV600
	_mt_fe_dmd_set_reg(0x33, 0x5a);
#else
	_mt_fe_dmd_set_reg(0x33, 0x35);
#endif


	if(sym_rate_KSs <= 2500)
	{
		_mt_fe_dmd_set_reg(0xc3, 0x08); // 8 * 10 * 100 / 64 = 125
		_mt_fe_dmd_set_reg(0xc8, 0x0a);
		_mt_fe_dmd_set_reg(0xc4, 0x07); // 7 * 40 * 100 / 128 = 218.75
		_mt_fe_dmd_set_reg(0xc7, 0x28);
	}
	else if(sym_rate_KSs <= 5000)
	{
		_mt_fe_dmd_set_reg(0xc3, 0x08); // 8 * 10 * 100 / 64 = 125
		_mt_fe_dmd_set_reg(0xc8, 0x0a);
		_mt_fe_dmd_set_reg(0xc4, 0x08); // 8 * 16 * 100 / 128 = 100
		_mt_fe_dmd_set_reg(0xc7, 0x10);
	}
	else if(sym_rate_KSs <= 20000)
	{
		_mt_fe_dmd_set_reg(0xc3, 0x08); // 8 * 10 * 100 / 64 = 125
		_mt_fe_dmd_set_reg(0xc8, 0x0a);
		_mt_fe_dmd_set_reg(0xc4, 0x08); // 8 * 32 * 100 / 128 = 200
		_mt_fe_dmd_set_reg(0xc7, 0x20);
	}
	else
	{
		_mt_fe_dmd_set_reg(0xc3, 0x08); // 8 * 4 * 100 / 64 = 100
		_mt_fe_dmd_set_reg(0xc8, 0x08);
		_mt_fe_dmd_set_reg(0xc4, 0x08); // 8 * 32 * 100 / 128 = 200
		_mt_fe_dmd_set_reg(0xc7, 0x20);
	}

	_mt_fe_dmd_ds3k_set_sym_rate(sym_rate_KSs);

	_mt_fe_dmd_ds3k_set_CCI(MtFe_On);

	_mt_fe_dmd_get_reg(0x76, &val);
	val &= ~0x80;
	_mt_fe_dmd_set_reg(0x76, val);

	return ret;
}


/******************************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_save_scanned_TP
**
**
** Description:	save the scanned TPs
**
**
** Inputs:
**
**		Parameter			type		description
**	---------------------------------------------------------------------------------------
**		freq_KHz			U32			freq. of the TP need to save
**		symbol_rate_KSs		U32			S.R. of the TP need to save
**		bs_times			U8			blind scan times
**
**
** Outputs:
**
**		Parameter		type						description
**	---------------------------------------------------------------------------------------
**		p_bs_info		MT_FE_BS_TP_INFO *			blind scan struture pointer
**
**
*******************************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_save_scanned_TP(MT_FE_BS_TP_INFO *p_bs_info,
											U32 freq_KHz,
											U32 symbol_rate_KSs,
											U8  bs_times)
{
	U16 i, j;
	MT_FE_TP_INFO *tmp = NULL;

	tmp = p_bs_info->p_tp_info;


	if(g_scanned_tp_cnt > (p_bs_info->tp_max_num))
	{
		return MtFeErr_NoMemory;
	}


	if(((bs_times == MT_FE_BS_TIMES_1ST)||(g_bs_time_total == 1)) &&
		((spectrum_inverted == 0) || (g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103)))
	{
		tmp[g_scanned_tp_cnt-1].freq_KHz		= freq_KHz;
		tmp[g_scanned_tp_cnt-1].sym_rate_KSs	= (U16)symbol_rate_KSs;
		tmp[g_scanned_tp_cnt-1].dvb_type		= MtFeType_DvbS_Unknown;
		tmp[g_scanned_tp_cnt-1].code_rate		= MtFeCodeRate_Undef;
	}
	else
	{
		for(i = 0; i < g_scanned_tp_cnt; i++)
		{
			if(use_unicable_device == 1)
			{
				S32 delta_freq_KHz, delta_sym_rate;

				delta_freq_KHz = freq_KHz - tmp[i].freq_KHz;
				delta_sym_rate = symbol_rate_KSs - tmp[i].sym_rate_KSs;

				if((delta_freq_KHz < 1000) && (delta_freq_KHz > -1000) &&
				   (delta_sym_rate < 1000) && (delta_sym_rate > -1000))
				{
					if(tmp[i].dvb_type == MtFeType_DvbS_Unknown)
					{
						tmp[i].freq_KHz = freq_KHz;
						if(delta_sym_rate > 0)
							tmp[i].sym_rate_KSs = (U16)symbol_rate_KSs;
					}
					else if(tmp[i].dvb_type == MtFeType_DvbS_Checked)
					{
						if(delta_sym_rate > 0)
						{
							tmp[i].freq_KHz = freq_KHz;
							tmp[i].sym_rate_KSs = (U16)symbol_rate_KSs;
							tmp[i].dvb_type = MtFeType_DvbS_Unknown;
						}
					}

					return MtFeErr_Ok;
				}

			}

			if(freq_KHz <= tmp[i].freq_KHz)
			{
				for(j = g_scanned_tp_cnt; j > i; j--)
				{
					tmp[j].freq_KHz 	= tmp[j-1].freq_KHz;
					tmp[j].sym_rate_KSs = tmp[j-1].sym_rate_KSs;
					tmp[j].dvb_type		= tmp[j-1].dvb_type;
					tmp[j].code_rate	= tmp[j-1].code_rate;
				}

				tmp[j].freq_KHz		= freq_KHz;
				tmp[j].sym_rate_KSs	= (U16)symbol_rate_KSs;
				tmp[j].dvb_type		= MtFeType_DvbS_Unknown;
				tmp[j].code_rate	= MtFeCodeRate_Undef;

				break;
			}
		}


		if(i == g_scanned_tp_cnt)
		{
			tmp[g_scanned_tp_cnt-1].freq_KHz	 = freq_KHz;
			tmp[g_scanned_tp_cnt-1].sym_rate_KSs = (U16)symbol_rate_KSs;
			tmp[g_scanned_tp_cnt-1].dvb_type	 = MtFeType_DvbS_Unknown;
			tmp[g_scanned_tp_cnt-1].code_rate	 = MtFeCodeRate_Undef;
		}
	}



	return MtFeErr_Ok;
}



/********************************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_remove_unlocked_TP
**
**
** Description:	remove the unlocked TP if the TP's freq. is lower than the compared freq.
**
**
** Inputs:
**
**	Parameter			type									description
**	-----------------------------------------------------------------------------------------
**	p_bs_info			MT_FE_BS_TP_INFO *						blind scan struture pointer
**	compare_freq_MHz	U32										the campared TP freq.
**
** Outputs:		none
**
**
*********************************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_remove_unlocked_TP(MT_FE_BS_TP_INFO *p_bs_info, U32 compare_freq_KHz)
{
	U8	 i;
	U16	 scanned_TP_num, index = 0, move_steps, sym_tmp;
	U32  fre_tmp;

	scanned_TP_num = g_scanned_tp_cnt;

	for(i = 0; i < scanned_TP_num; i++)
	{
		if(p_bs_info->p_tp_info[i].freq_KHz >= compare_freq_KHz)
		{
			index = i;
			break;
		}
	}


	if(i == scanned_TP_num)
		index = scanned_TP_num;


	i = 0;
	move_steps = 0;

	while(i <= index)
	{
		if(p_bs_info->p_tp_info[i].dvb_type != MtFeType_DvbS_Checked)
		{
			i++;
			continue;
		}

		while((p_bs_info->p_tp_info[i].dvb_type == MtFeType_DvbS_Checked)&&(i <= index))
		{
			move_steps++;
			i++;
		}

		while((p_bs_info->p_tp_info[i].dvb_type != MtFeType_DvbS_Checked)&&(i <= index))
		{
			fre_tmp = p_bs_info->p_tp_info[i-move_steps].freq_KHz;
			sym_tmp = p_bs_info->p_tp_info[i-move_steps].sym_rate_KSs;

			p_bs_info->p_tp_info[i-move_steps].freq_KHz     = p_bs_info->p_tp_info[i].freq_KHz;
			p_bs_info->p_tp_info[i-move_steps].sym_rate_KSs = p_bs_info->p_tp_info[i].sym_rate_KSs;
			p_bs_info->p_tp_info[i-move_steps].dvb_type     = p_bs_info->p_tp_info[i].dvb_type;
			p_bs_info->p_tp_info[i-move_steps].code_rate    = p_bs_info->p_tp_info[i].code_rate;

			p_bs_info->p_tp_info[i].freq_KHz	 = fre_tmp;
			p_bs_info->p_tp_info[i].sym_rate_KSs = sym_tmp;
			p_bs_info->p_tp_info[i].dvb_type	 = MtFeType_DvbS_Checked;
			p_bs_info->p_tp_info[i].code_rate	 = MtFeCodeRate_Undef;

			i++;
		}
	}


	return MtFeErr_Ok;
}


/********************************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_compare_deal_TP
**
**
** Description:	process the scanned new TP in each blind scan
**
**
** Inputs:
**
**		Parameter			type					description
**	---------------------------------------------------------------------------------------
**		cur_scan_freq_KHz	U32						start frequency in each blind scan bandwidth
**		bs_times			U8						blind scan times
**
**
** Outputs:
**
**		Parameter			type					description
**	------------------------------------------------------------------------------------
**		p_bs_info			MT_FE_BS_TP_INFO *		tp structure pointer
**
**
********************************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_compare_deal_TP(MT_FE_BS_TP_INFO *p_bs_info,
											U32 cur_scan_freq_KHz,
											U8  bs_times)
{
	MT_FE_RET	ret = MtFeErr_Ok;

	U8  tmp1, tmp2, tmp3, tmp4, covered_ratio;
	U16	i, last_check_index;
	U32	centerfreqKHz_tpA, symrateKSs_tpA;
	U32	centerfreqKHz_tpB, symrateKSs_tpB;
	U32 left_boundary, right_boundary;
	S32 s_value, delta_freq_KHz, delta_symbol_rate_KSs, freq_delta_KHz;
	S32 similar_symbol_rate_KSs, freq_offset_tmp, symrate_offset_tmp;

#if (MT_FE_DEBUG != 0)
	MT_FE_TP_INFO		tp_info;
#endif

	s_value = 0;
	covered_ratio = 0;

	centerfreqKHz_tpA = 0;
	centerfreqKHz_tpB = 0;
	symrateKSs_tpA 	  = 0;
	symrateKSs_tpB    = 0;

	_mt_fe_dmd_set_reg(0x9A, 0x20);
	_mt_fe_dmd_get_reg(0x9B, &tmp1);
	_mt_fe_dmd_get_reg(0x9B, &tmp2);
	_mt_fe_dmd_get_reg(0x9B, &tmp3);
	_mt_fe_dmd_get_reg(0x9B, &tmp4);


	symrateKSs_tpB = (U32)((((tmp4&0x03)<<8) | tmp3)*MT_FE_MCLK_KHZ/ FFT_LENGTH);

	s_value = (S32)(((tmp2&0x03)<<8) | tmp1);
	if(s_value >= FFT_LENGTH)
		s_value -= 1024;

	if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
	{
		s_value = -s_value;
	}

	centerfreqKHz_tpB = (U32)(cur_scan_freq_KHz + s_value*(MT_FE_MCLK_KHZ/FFT_LENGTH));
	centerfreqKHz_tpA = p_bs_info->p_tp_info[g_tp_index].freq_KHz;
	symrateKSs_tpA    = p_bs_info->p_tp_info[g_tp_index].sym_rate_KSs;


	if(centerfreqKHz_tpB < 950000)
	{
		return MtFeErr_Ok;
	}

#if 1
	if((symrateKSs_tpB > 50000)||(symrateKSs_tpB <= 2000))
	{
		return MtFeErr_Ok;
	}
#else
	if((symrateKSs_tpB > 50000)||(symrateKSs_tpB < 1400))
	{
		return MtFeErr_Ok;
	}
#endif
	if((symrateKSs_tpB < 10000)&&((bs_times == MT_FE_BS_TIMES_1ST)&&(g_bs_time_total > 1)))
	{
		return MtFeErr_Ok;
	}
	if((symrateKSs_tpB > 15000)&&(bs_times == MT_FE_BS_TIMES_3RD))
	{
		return MtFeErr_Ok;
	}


#if 1
	if(symrateKSs_tpB > 45000)
	{
		symrateKSs_tpB = 45000;
	}
	else if(symrateKSs_tpB > 2000)
	{
		symrateKSs_tpB -= 400;
	}
	else
	{
		symrateKSs_tpB -= 500;
	}
#endif

	if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
	{
		if(g_first_TP_found == 0)
		{
			#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
			if((centerfreqKHz_tpB + symrateKSs_tpB/2) <= (g_window_center_freq + g_window_width/2 - 4000))
			{
				g_first_TP_found			 = 1;
				g_first_TP_freq_KHz			 = centerfreqKHz_tpB;
				g_first_TP_symbol_rate_KSs	 = symrateKSs_tpB;
			}
			else
			{
				return ret;
			}
			#else			// TS2022
			if(((centerfreqKHz_tpB + symrateKSs_tpB/2) <= (g_window_center_freq + g_window_width/2 - 4000)) &&
				(centerfreqKHz_tpB <= (g_window_center_freq + 7000)))
			{
				g_first_TP_found			 = 1;
				g_first_TP_freq_KHz			 = centerfreqKHz_tpB;
				g_first_TP_symbol_rate_KSs	 = symrateKSs_tpB;
			}
			else
			{
				return ret;
			}
			#endif
		}
	}


#if (MT_FE_DEBUG != 0)
	tp_info.freq_KHz     = centerfreqKHz_tpB;
	tp_info.sym_rate_KSs = (U16)symrateKSs_tpB;
	mt_fe_bs_notify(MtFeMsg_BSTpFind, &tp_info);
#endif


	if((bs_times == MT_FE_BS_TIMES_1ST)||(g_bs_time_total == 1))
	{
		g_scanned_tp_cnt++;
		ret = _mt_fe_dmd_ds3k_bs_save_scanned_TP(p_bs_info, centerfreqKHz_tpB, symrateKSs_tpB, bs_times);
		if(ret != MtFeErr_Ok)
			return ret;

		return MtFeErr_Ok;
	}


	while(!g_blindscan_cancel)
	{
		right_boundary = centerfreqKHz_tpB + symrateKSs_tpB/2;
		left_boundary  = centerfreqKHz_tpA - symrateKSs_tpA/2;


		if(right_boundary <= left_boundary)
		{
			if(g_tp_index > 0)
				g_tp_index--;
			else
				g_tp_index = 0;

			centerfreqKHz_tpA = p_bs_info->p_tp_info[g_tp_index].freq_KHz;

			g_scanned_tp_cnt++;
			ret = _mt_fe_dmd_ds3k_bs_save_scanned_TP(p_bs_info, centerfreqKHz_tpB, symrateKSs_tpB, bs_times);
			if(ret != MtFeErr_Ok)
				return ret;

			/*update tpA index*/
			for(i = 0; i < g_scanned_tp_cnt; i++)
			{
				if(g_blindscan_cancel) break;

				if(centerfreqKHz_tpA == (p_bs_info->p_tp_info[i].freq_KHz))
				{
					g_tp_index = i;
					break;
				}
			}

			break;
		}


		if((p_bs_info->p_tp_info[g_tp_index].dvb_type == MtFeType_DvbS)||
		   (p_bs_info->p_tp_info[g_tp_index].dvb_type == MtFeType_DvbS2))
		{
			if(symrateKSs_tpB > 6000)	covered_ratio = 2;
			else						covered_ratio = 3;

			if(symrateKSs_tpB > 25000)		freq_delta_KHz = 7000;
			else if(symrateKSs_tpB > 10000)	freq_delta_KHz = 4000;
			else if(symrateKSs_tpB > 5000)	freq_delta_KHz = 3000;
			else if(symrateKSs_tpB > 3000)	freq_delta_KHz = 2000;
			else 							freq_delta_KHz = 1000;

			if(centerfreqKHz_tpB >= centerfreqKHz_tpA)
			{
				delta_freq_KHz = centerfreqKHz_tpB - centerfreqKHz_tpA;
				delta_symbol_rate_KSs = (centerfreqKHz_tpA + symrateKSs_tpA/2) - (centerfreqKHz_tpB - symrateKSs_tpB/2);
			}
			else
			{
				delta_freq_KHz = centerfreqKHz_tpA - centerfreqKHz_tpB;
				delta_symbol_rate_KSs = (centerfreqKHz_tpB + symrateKSs_tpB/2) - (centerfreqKHz_tpA - symrateKSs_tpA/2);
			}


			if((delta_symbol_rate_KSs >= (S32)(symrateKSs_tpB/covered_ratio))&&
			   (delta_freq_KHz <= (S32)(freq_delta_KHz)))
			{
				mt_fe_print(("\n\tOverlap Skip TP!!!!!!!   Freq. == %d\n", centerfreqKHz_tpB));
				break;
			}
		}
		else
		{
			if(symrateKSs_tpB > 25000)
			{
				freq_offset_tmp 	= 1000;
				symrate_offset_tmp	= 400;
			}
			else if(symrateKSs_tpB > 10000)
			{
				freq_offset_tmp 	= 800;
				symrate_offset_tmp	= 250;
			}
			else if(symrateKSs_tpB > 5000)
			{
				freq_offset_tmp 	= 500;
				symrate_offset_tmp	= 200;
			}
			else if(symrateKSs_tpB > 3000)
			{
				freq_offset_tmp 	= 500;
				symrate_offset_tmp	= 200;
			}
			else if(symrateKSs_tpB >= 1000)
			{
				freq_offset_tmp 	= 500;
				symrate_offset_tmp	= 100;
			}
			else
			{
				freq_offset_tmp		= 500;
				symrate_offset_tmp 	= 100;
			}

			if(symrateKSs_tpB >= symrateKSs_tpA)
				similar_symbol_rate_KSs = symrateKSs_tpB - symrateKSs_tpA;
			else
				similar_symbol_rate_KSs = symrateKSs_tpA - symrateKSs_tpB;


			if(centerfreqKHz_tpB >= centerfreqKHz_tpA)
				delta_freq_KHz = centerfreqKHz_tpB - centerfreqKHz_tpA;
			else
				delta_freq_KHz = centerfreqKHz_tpA - centerfreqKHz_tpB;

			if((similar_symbol_rate_KSs <= symrate_offset_tmp)&&
			   (delta_freq_KHz <= freq_offset_tmp))
			{
				mt_fe_print(("\n\tSimilar Skip TP!!!!!!!  Freq. == %d\n", centerfreqKHz_tpB));
				break;
			}
		}


		last_check_index = g_tp_index;
		g_tp_index++;

		/*find next tpA*/
		for(i = g_tp_index; ; i++)
		{
			if(g_blindscan_cancel) break;

			if(g_tp_index >= g_scanned_tp_cnt)
			{
				last_tpA_flag = 1;
				g_tp_index = last_check_index;

				break;
			}

			if(p_bs_info->p_tp_info[i].dvb_type != MtFeType_DvbS_Unknown)
			{
				g_tp_index = i;
				centerfreqKHz_tpA = p_bs_info->p_tp_info[g_tp_index].freq_KHz;
				symrateKSs_tpA    = p_bs_info->p_tp_info[g_tp_index].sym_rate_KSs;
				break;
			}
		}


		if(last_tpA_flag == 1)
		{
			g_tp_index = last_check_index;
			centerfreqKHz_tpA = p_bs_info->p_tp_info[g_tp_index].freq_KHz;

			g_scanned_tp_cnt++;
			ret = _mt_fe_dmd_ds3k_bs_save_scanned_TP(p_bs_info, centerfreqKHz_tpB, symrateKSs_tpB, bs_times);
			if(ret != MtFeErr_Ok)
				return ret;

			break;
		}
	}

	g_cur_compare_TPA_KHz = centerfreqKHz_tpA;


	return MtFeErr_Ok;
}


/********************************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_process_scanned_TP
**
**
** Description:	process the scanned new TP in each blind scan bandwidth
**
**
** Inputs:
**
**		Parameter			type					description
**	---------------------------------------------------------------------------------------
**		cur_scan_freq_KHz	U32						start frequency in each blind scan bandwidth
**		bs_times			U8						blind scan times
**
**
** Outputs:
**
**		Parameter			type					description
**	------------------------------------------------------------------------------------
**		p_bs_info			MT_FE_BS_TP_INFO *		tp structure pointer
**
**
********************************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_process_scanned_TP(MT_FE_BS_TP_INFO *p_bs_info,
												U32 cur_scan_freq_KHz,
												U8  bs_times)
{
	MT_FE_BOOL	b_analog_locked, b_fftdone;

	U8  tmp, cnt;
	U16	i, totaltpnum;

	for(i = 0; i < 2; i++)
	{
		b_fftdone		= MtFe_False;
		b_analog_locked = MtFe_False;

		cnt = 10;
		do
		{
			_mt_fe_dmd_get_reg(0x0d, &tmp);
			b_analog_locked = ((tmp&0x01) == 0x01) ? MtFe_True : MtFe_False;
			_mt_sleep(50);
			cnt--;
		}while((b_analog_locked == MtFe_False)&&(cnt > 0)&&(!g_blindscan_cancel));

		if(cnt == 0)
			break;

		_mt_fe_dmd_set_reg(0x9a, 0x80);

		cnt = 50;
		do
		{
			_mt_fe_dmd_get_reg(0x9a, &tmp);
			b_fftdone = ((tmp&0x80) == 0x00) ? MtFe_True : MtFe_False;
			_mt_sleep(10);
			cnt--;
		}while((b_fftdone == MtFe_False)&&(cnt > 0)&&(!g_blindscan_cancel));

		if(b_fftdone || (1 == i))
		{
			break;
		}
		else
		{
			_mt_fe_dmd_set_reg(0x5f, 0x00);
			_mt_fe_dmd_set_reg(0x5e, 0x70);
		}
	}

	_mt_fe_dmd_get_reg(0x9a, &tmp);
	totaltpnum = (U16)(tmp & 0x1F);

	g_first_TP_found = 0;
	g_first_TP_symbol_rate_KSs = 0;
	g_first_TP_freq_KHz = 0;


	g_tp_index 	  = 0;
	last_tpA_flag = 0;
	while((totaltpnum > 0)&&(!g_blindscan_cancel))
	{
		totaltpnum--;

		_mt_fe_dmd_ds3k_bs_compare_deal_TP(p_bs_info, cur_scan_freq_KHz, bs_times);
	}


	if(bs_times == MT_FE_BS_TIMES_3RD)
	{
		//_mt_fe_dmd_ds3k_bs_remove_unlocked_TP(p_bs_info, g_cur_compare_TPA_KHz);

		for(i = 0; i < p_bs_info->tp_max_num; i++)
		{
			if(g_cur_compare_TPA_KHz == (p_bs_info->p_tp_info[i].freq_KHz))
			{
				g_tp_index = i;
				break;
			}
		}
	}


	return MtFeErr_Ok;
}



/******************************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_A
**
**
** Description:	scan the TP from the start_freq_KHz to end_freq_KHz
**
**
** Inputs:
**
**		Parameter			type								description
**	---------------------------------------------------------------------------------------
**		start_freq_KHz		U32									blind scan start freq.
**		end_freq_KHz		U32									blind scan stop freq.
**		bs_times			U8									blind scan times
**
** Outputs:
**
**		Parameter			type								description
**	---------------------------------------------------------------------------------------
**		p_bs_info			MT_FE_BS_TP_INFO* 					blind scan result pointer
**
**
*******************************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_A(U32 start_freq_KHz,
							  U32 end_freq_KHz,
							  MT_FE_BS_TP_INFO *p_bs_info,
							  U8 bs_times)
{
	U8	val_0x95, val_0x96, val_0x91, end_flag = 0, tmp, TP_found_flag;
	U32 freq_KHz, tmp_freq_KHz, tmp_tp_scanned;
	MT_FE_RET	ret = MtFeErr_Ok;

	S32 delta_freq_KHz, tp_freq_KHz;

	U32 real_freq_KHz, val;


	if(mt_fe_tn_set_tuner == NULL)
		return MtFeErr_NullPointer;

	tmp_tp_scanned = 0;
	freq_KHz  = start_freq_KHz;

	while((freq_KHz < end_freq_KHz)&&(!g_blindscan_cancel))
	{
		_mt_fe_dmd_ds3k_bs_set_reg(bs_times);
		if(use_unicable_device)
		{
			mt_fe_unicable_set_tuner(freq_KHz, BLINDSCAN_SYMRATEKSs, &real_freq_KHz, g_ub_select, g_bank_select, g_ub_freq_MHz);
			freq_KHz = real_freq_KHz;
		}
		else
		{
			mt_fe_tn_set_tuner(freq_KHz, BLINDSCAN_SYMRATEKSs, 0);
		}

		if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
		{
			_mt_fe_dmd_ds3k_set_carrier_offset((S32)(-mt_fe_tn_get_offset()));
		}
		else
		{
			_mt_fe_dmd_ds3k_set_carrier_offset((S32)(+mt_fe_tn_get_offset()));
		}

		tmp_freq_KHz = 0;
		g_first_TP_freq_KHz = freq_KHz;
		g_first_TP_symbol_rate_KSs = BLINDSCAN_SYMRATEKSs;
		g_first_TP_locked = 0;
		TP_found_flag = 0;

		g_window_center_freq = freq_KHz;


		ret = _mt_fe_dmd_ds3k_bs_process_scanned_TP(p_bs_info, freq_KHz, bs_times);
		if(ret != MtFeErr_Ok)
			return ret;

		_mt_fe_dmd_get_reg(0x95, &val_0x95);
		_mt_fe_dmd_get_reg(0x96, &val_0x96);

		if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
		{
			_mt_fe_dmd_get_reg(0x91, &val_0x91);
			g_flattern_thr_val = (U32)((((TUNER_SLIP_STEP + 1) << (((val_0x91 & 0x60) >> 5) + 2))));

			_mt_fe_dmd_get_reg(0x9a, &tmp);
			if((tmp &= 0x1F) == 0)
			{
				TP_found_flag = 0;

				val = (U32)((((val_0x95&0xc0)>>6) << 8) | val_0x96);

				if(val >= g_flattern_thr_val)
				{
					tmp_freq_KHz = g_flattern_thr_val * MT_FE_MCLK_KHZ / FFT_LENGTH;
					tmp_freq_KHz -= 1000;
				}
				else
				{
					tmp_freq_KHz = val * MT_FE_MCLK_KHZ / FFT_LENGTH;
				}

				if(tmp_freq_KHz < 8000)
				{
					tmp_freq_KHz = 8000;
				}
				else if(tmp_freq_KHz > 24000)
				{
					tmp_freq_KHz = 24000;
				}

				freq_KHz += tmp_freq_KHz;
			}
			else
			{
				TP_found_flag = 1;
			}

		}
		else
		{
			tmp_freq_KHz = (U32)((((((val_0x95&0xc0)>>6) << 8) | val_0x96)*MT_FE_MCLK_KHZ)/FFT_LENGTH);
			if(tmp_freq_KHz > 96000)
			{
				tmp_freq_KHz = 10000;
			}

			if(use_unicable_device == 1)
			{
				if((tmp_freq_KHz < 4000) && (tmp_freq_KHz != 0))
				{
					tmp_freq_KHz = 4000;
				}
			}

			freq_KHz += tmp_freq_KHz;

			#if (MT_FE_TN_SUPPORT_TS2020||MT_FE_TN_SUPPORT_ST6110||MT_FE_TN_SUPPORT_TS2022)
			if(tmp_freq_KHz == 0)
			{
				freq_KHz += 36000;
			}
			#elif ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
			if(tmp_freq_KHz == 0)
			{
				freq_KHz += 30000;
			}
			#else
			if(tmp_freq_KHz == 0)
			{
				freq_KHz += 30000;
			}
			#endif
		}


		if(tmp_tp_scanned < g_scanned_tp_cnt)
		{
			tmp_tp_scanned = g_scanned_tp_cnt;

			_mt_fe_dmd_ds3k_bs_connect(p_bs_info, 0, g_scanned_tp_cnt);
		}

		if((spectrum_inverted == 1) && (TP_found_flag == 1) && (g_dmd_id == MtFeDmdId_DS300X))
		{
			delta_freq_KHz = 0;

			if(g_first_TP_locked == 1)
			{
				delta_freq_KHz = g_first_TP_freq_KHz + g_first_TP_symbol_rate_KSs/2 - (g_window_center_freq - g_window_width/2);

				if(delta_freq_KHz > 24000)
					delta_freq_KHz = 24000;
			}
			else
			{
				if((g_first_TP_freq_KHz != 0) &&(g_first_TP_symbol_rate_KSs != 0))
				{
					tp_freq_KHz = g_first_TP_freq_KHz + g_first_TP_symbol_rate_KSs/2;

					delta_freq_KHz = tp_freq_KHz - (g_window_center_freq - g_window_width/2);

					#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
					if(delta_freq_KHz > 30000)
					{
						delta_freq_KHz -= 12000;
					}
					if(delta_freq_KHz > 30000)
						delta_freq_KHz = 30000;
					#else		// TS2022
					if(delta_freq_KHz > 20000)
						delta_freq_KHz = 20000;
					#endif
				}
				else
				{
					#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
					delta_freq_KHz = 12000;
					#else		// TS2022
					delta_freq_KHz = 8000;
					#endif
				}
			}

			if(delta_freq_KHz < 8000)
				delta_freq_KHz = 8000;

			delta_freq_KHz -= 4000;

			freq_KHz += delta_freq_KHz;
		}

		if(end_flag == 1)
		{
			end_flag = 0;
			mt_fe_bs_notify(MtFeMsg_BSOneWinFinish, &(freq_KHz));
			break;
		}
		if(freq_KHz >= 2149000)
		{
			freq_KHz = 2149000;
			end_flag = 1;
		}

		mt_fe_bs_notify(MtFeMsg_BSOneWinFinish, &(freq_KHz));
	}

	return MtFeErr_Ok;
}



/******************************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_B
**
**
** Description:	scan the TP from the start_freq_KHz to end_freq_KHz
**
**
** Inputs:
**
**		Parameter			type								description
**	---------------------------------------------------------------------------------------
**		start_freq_KHz		U32									blind scan start freq.
**		end_freq_KHz		U32									blind scan stop freq.
**		bs_times			U8									blind scan times
**
**
** Outputs:
**
**		Parameter			type								description
**	---------------------------------------------------------------------------------------
**		p_bs_info			MT_FE_BS_TP_INFO* 					blind scan result pointer
**
**
*******************************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_B(U32 start_freq_KHz,
							  U32 end_freq_KHz,
							  MT_FE_BS_TP_INFO *p_bs_info,
							  U8 bs_times)
{
	U8	val_0x95, val_0x96, val_0x91, end_flag = 0, tmp, TP_found_flag;
	U32 freq_KHz, tmp_freq_KHz;
	MT_FE_RET	ret = MtFeErr_Ok;

	U32 real_freq_KHz, val;

	S32 delta_freq_KHz, tp_freq_KHz;

	if(mt_fe_tn_set_tuner == NULL)
		return MtFeErr_NullPointer;

	freq_KHz = start_freq_KHz;

	_mt_fe_dmd_ds3k_bs_set_reg(bs_times);

	while((freq_KHz < end_freq_KHz)&&(!g_blindscan_cancel))
	{
		if(use_unicable_device)
		{
			mt_fe_unicable_set_tuner(freq_KHz, BLINDSCAN_SYMRATEKSs, &real_freq_KHz, g_ub_select, g_bank_select, g_ub_freq_MHz);
			freq_KHz = real_freq_KHz;
		}
		else
		{
			mt_fe_tn_set_tuner(freq_KHz, BLINDSCAN_SYMRATEKSs, 0);
		}

		if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
		{
			_mt_fe_dmd_ds3k_set_carrier_offset((S32)(-mt_fe_tn_get_offset()));
		}
		else
		{
			_mt_fe_dmd_ds3k_set_carrier_offset((S32)(+mt_fe_tn_get_offset()));
		}

		tmp_freq_KHz = 0;
		g_first_TP_freq_KHz = freq_KHz;
		g_first_TP_symbol_rate_KSs = BLINDSCAN_SYMRATEKSs;
		g_first_TP_locked = 0;
		TP_found_flag = 0;

		g_window_center_freq = freq_KHz;

		ret = _mt_fe_dmd_ds3k_bs_process_scanned_TP(p_bs_info, freq_KHz, bs_times);
		if(ret != MtFeErr_Ok)
			return ret;

		_mt_fe_dmd_get_reg(0x95, &val_0x95);
		_mt_fe_dmd_get_reg(0x96, &val_0x96);

		if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
		{
			_mt_fe_dmd_get_reg(0x91, &val_0x91);
			g_flattern_thr_val = (U32)((((TUNER_SLIP_STEP + 1) << (((val_0x91 & 0x60) >> 5) + 2))));

			_mt_fe_dmd_get_reg(0x9a, &tmp);
			if((tmp &= 0x1F) == 0)
			{
				TP_found_flag = 0;

				val = (U32)((((val_0x95&0xc0)>>6) << 8) | val_0x96);

				if(val >= g_flattern_thr_val)
				{
					tmp_freq_KHz = g_flattern_thr_val * MT_FE_MCLK_KHZ / FFT_LENGTH;
					tmp_freq_KHz -= 1000;
				}
				else
				{
					tmp_freq_KHz = val * MT_FE_MCLK_KHZ / FFT_LENGTH;
				}

				if(tmp_freq_KHz < 8000)
				{
					tmp_freq_KHz = 8000;
				}
				else if(tmp_freq_KHz > 24000)
				{
					tmp_freq_KHz = 24000;
				}

				#if ((MT_FE_TN_SUPPORT_SHARP6306 == 0) && (MT_FE_TN_SUPPORT_SHARP7803 == 0) && (MT_FE_TN_SUPPORT_SHARP7903 == 0))		// TS2022
				if(tmp_freq_KHz > 20000)
					tmp_freq_KHz = 20000;
				#endif

				freq_KHz += tmp_freq_KHz;
			}
			else
			{
				TP_found_flag = 1;
			}

		}
		else
		{
			tmp_freq_KHz = (U32)((((((val_0x95&0xc0)>>6) << 8) | val_0x96)*MT_FE_MCLK_KHZ)/FFT_LENGTH);
			if(tmp_freq_KHz > 96000)
			{
				tmp_freq_KHz = 10000;
			}

			if(use_unicable_device == 1)
			{
				if((tmp_freq_KHz < 4000) && (tmp_freq_KHz != 0))
				{
					tmp_freq_KHz = 4000;
				}
			}

			freq_KHz = (U32)(freq_KHz + tmp_freq_KHz);

			#if (MT_FE_TN_SUPPORT_TS2020||MT_FE_TN_SUPPORT_ST6110||MT_FE_TN_SUPPORT_TS2022)
			if(tmp_freq_KHz == 0)
			{
				freq_KHz += 36000;
			}
			#elif ((MT_FE_TN_SUPPORT_SHARP6306 != 0) ||(MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
			if(tmp_freq_KHz == 0)
			{
				freq_KHz += 30000;
			}
			#else
			if(tmp_freq_KHz == 0)
			{
				freq_KHz += 30000;
			}
			#endif
		}

		if((spectrum_inverted == 1) && (TP_found_flag == 1) && (g_dmd_id == MtFeDmdId_DS300X))
		{
			delta_freq_KHz = 0;

			if((g_first_TP_freq_KHz != 0) &&(g_first_TP_symbol_rate_KSs != 0))
			{
				tp_freq_KHz = g_first_TP_freq_KHz + g_first_TP_symbol_rate_KSs/2;

				delta_freq_KHz = tp_freq_KHz - (g_window_center_freq - g_window_width/2);


				#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
				if(delta_freq_KHz > 30000)
				{
					delta_freq_KHz -= 12000;
				}
				if(delta_freq_KHz > 30000)
					delta_freq_KHz = 30000;
				#else		// TS2022
				if(delta_freq_KHz > 24000)
				{
					delta_freq_KHz -= 12000;
				}
				if(delta_freq_KHz > 20000)
					delta_freq_KHz = 20000;
				#endif
			}
			else
			{
				#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0)||(MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
				delta_freq_KHz = 12000;
				#else		// TS2022
				delta_freq_KHz = 8000;
				#endif
			}

			if(delta_freq_KHz < 8000)
				delta_freq_KHz = 8000;

			delta_freq_KHz -= 4000;

			freq_KHz += delta_freq_KHz;
		}


		if(end_flag == 1)
		{
			end_flag = 0;
			mt_fe_bs_notify(MtFeMsg_BSOneWinFinish, &(freq_KHz));
			break;
		}
		if(freq_KHz >= 2149000)
		{
			freq_KHz = 2149000;
			end_flag = 1;
		}

		mt_fe_bs_notify(MtFeMsg_BSOneWinFinish, &(freq_KHz));
	}


	_mt_fe_dmd_ds3k_bs_connect(p_bs_info, 0, g_scanned_tp_cnt);

	return MtFeErr_Ok;
}



/************************************************************************************
** Function: _mt_fe_dmd_ds3k_bs_connect
**
**
** Description:	try to lock the scanned TPs according to the start TP index and the
**				TP numbers need to lock
**
**
** Inputs:
**
**		Parameter		type				description
**	--------------------------------------------------------------------
**		start_index		U16					the start TP index in memory
**		scanned_tp		U16					TP num need to try lock
**
** Outputs:
**
**		Parameter		type				description
**	---------------------------------------------------------------------------------
**		p_bs_info		MT_FE_BS_TP_INFO *	the blindscan TP pointer
**
**
*************************************************************************************/
MT_FE_RET _mt_fe_dmd_ds3k_bs_connect(MT_FE_BS_TP_INFO *p_bs_info,
									U16 start_index,
									U16 scanned_tp)
{
	U16		index, timing_thr, crl_thr;
	U8		j, count, timing_fail, crl_fail, tmp, same_locked_TP_flag;
	U32		sym_rate_KSs, centerfreq_KHz, truefreq_KHz;
	S16		lpf_offset_KHz, i;
	S32		carrieroffset_KHz, delta_freq_KHz, delta_smy_rate_KSs, deviceoffset_KHz;

	MT_FE_RET		ret = MtFeErr_Ok;

	U32 real_freq_KHz;

	MT_FE_TYPE			current_mode;
	MT_FE_LOCK_STATE	lockstate;
	MT_FE_TP_INFO 		*p_scanned_tp_info = (MT_FE_TP_INFO *)(p_bs_info->p_tp_info);

	if((mt_fe_tn_set_tuner == NULL)||(mt_fe_tn_get_offset == NULL))
		return MtFeErr_NullPointer;

	for(index = start_index; index < scanned_tp; index++)
	{
		if(g_blindscan_cancel) break;

		if(p_scanned_tp_info[index].dvb_type != MtFeType_DvbS_Unknown)
			continue;

		centerfreq_KHz	= p_scanned_tp_info[index].freq_KHz;
		sym_rate_KSs	= p_scanned_tp_info[index].sym_rate_KSs;

		lpf_offset_KHz = 0;
		truefreq_KHz   = 0;

	#if ((MT_FE_TN_SUPPORT_TS2020 != 0)||(MT_FE_TN_SUPPORT_TS2022 != 0) || (MT_FE_TN_SUPPORT_SHARP6306 != 0)|| (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
		if ((sym_rate_KSs < 5000) && (use_unicable_device == 0))
		{
			lpf_offset_KHz	  = BLINDSCAN_LPF_OFFSET_KHz;
			centerfreq_KHz	 += BLINDSCAN_LPF_OFFSET_KHz;
		}
	#endif

		for(j = 0; j < 2; j++)
		{
			if(g_blindscan_cancel) break;

			if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
			{
				mt_fe_dmd_ds3k_global_reset();

				_mt_fe_dmd_ds3k_set_carrier_offset(g_carrier_offset);
			}

			g_eq_coef_check = 0;

			current_mode = (j == 0) ? MtFeType_DvbS : MtFeType_DvbS2;
			g_current_type = current_mode;

			if(use_unicable_device)
			{
				ret = mt_fe_unicable_set_tuner(centerfreq_KHz, sym_rate_KSs, &real_freq_KHz, g_ub_select, g_bank_select, g_ub_freq_MHz);
				deviceoffset_KHz = real_freq_KHz - centerfreq_KHz;
			}
			else
			{
				ret = mt_fe_tn_set_tuner(centerfreq_KHz, sym_rate_KSs, lpf_offset_KHz);
				real_freq_KHz = centerfreq_KHz;
				deviceoffset_KHz = 0;
			}

			if(ret != MtFeErr_Ok)		// Error parameter
			{
				lockstate = MtFeLockState_Unlocked;
				break;
			}

			_mt_fe_dmd_set_reg(0xb2, 0x01);

			_mt_fe_dmd_ds3k_bs_set_demod(sym_rate_KSs, current_mode);

			if((sym_rate_KSs <= 5000)&&(current_mode == MtFeType_DvbS2))
			{
				_mt_fe_dmd_set_reg(0xc0, 0x04);
				_mt_fe_dmd_set_reg(0x8a, 0x09);
				_mt_fe_dmd_set_reg(0x8b, 0x22);
				_mt_fe_dmd_set_reg(0x8c, 0x88);
			}

		#if ((MT_FE_TN_SUPPORT_SHARP6306 != 0) || (MT_FE_TN_SUPPORT_SHARP7803 != 0) || (MT_FE_TN_SUPPORT_SHARP7903 != 0))
			if(sym_rate_KSs >= 6500)		//if(sym_rate_KSs >= 10000)
			{
				_mt_fe_dmd_get_reg(0x0c, &tmp);
				tmp |= 0x04;
				_mt_fe_dmd_set_reg(0x0c, tmp);
			}
		#endif


			if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
			{
				_mt_fe_dmd_ds3k_set_carrier_offset(-(S32)mt_fe_tn_get_offset() - lpf_offset_KHz - deviceoffset_KHz);
			}
			else
			{
				_mt_fe_dmd_ds3k_set_carrier_offset(+(S32)mt_fe_tn_get_offset() + lpf_offset_KHz + deviceoffset_KHz);
			}

			_mt_fe_dmd_set_reg(0xb2, 0x00);

			lockstate = MtFeLockState_Waiting;
			_mt_fe_dmd_ds3k_check_CCI(&lockstate);

			if(lockstate == MtFeLockState_Unlocked)
			{
				mt_fe_bs_notify(MtFeMsg_BSTpUnlock, &(p_bs_info->p_tp_info[index]));
				p_bs_info->p_tp_info[index].dvb_type = MtFeType_DvbS_Checked;

				break;
			}

		#if 0
			if(sym_rate_KSs >= 10000)
			{
				count = 30;
				timing_thr = 2;
				crl_thr = 1;
			}
			else if(sym_rate_KSs >= 4000)
			{
				count = 50;
				timing_thr = 2;
				crl_thr = 1;
			}
			else/*only care the time to lock DVBS signal*/
			{
				count = 40;
				timing_thr = 2;
				crl_thr = 1;
			}
		#else
			if(sym_rate_KSs >= 10000)
			{
				count = 25;
				timing_thr = 2;
				crl_thr = 1;
			}
			else if(sym_rate_KSs >= 4000)
			{
				count = 28;
				timing_thr = 2;
				crl_thr = 1;
			}
			else if(sym_rate_KSs >= 2000)
			{
				count = 35;
				timing_thr = 2;
				crl_thr = 1;
			}
			else
			{
				count = 40;
				timing_thr = 2;
				crl_thr = 1;
			}
		#endif

			timing_fail = 0;
			crl_fail 	= 0;
			lockstate 	= MtFeLockState_Unlocked;

			while((count > 0)&&(!g_blindscan_cancel))
			{
				if (current_mode == MtFeType_DvbS2)
				{
					if(g_dmd_id == MtFeDmdId_DS300X)
					{
						_mt_fe_dmd_get_reg(0x0d, &tmp);
						if((tmp & 0x08) == 0x08)		// bit3 = 1
						{
							_mt_fe_dmd_get_reg(0x7e, &tmp);
							if((tmp & 0xc0) == 0x40)			// 8PSK
							{
								if((tmp & 0x0f) > 8)					// 8/9 & 9/10
								{
									_mt_fe_dmd_set_reg(0x7c, 0x01);
									_mt_fe_dmd_set_reg(0x80, 0x88);
								}
								else if((tmp & 0x0f) == 8)				// 5/6
								{
									_mt_fe_dmd_set_reg(0x7c, 0x01);
									_mt_fe_dmd_set_reg(0x80, 0x98);
								}
								else if((tmp & 0x20) == 0x20)			// pilot on
								{
									_mt_fe_dmd_set_reg(0x7c, 0x01);
									_mt_fe_dmd_set_reg(0xae, 0x0f);
									_mt_fe_dmd_set_reg(0x80, 0x54);
									_mt_fe_dmd_set_reg(0x81, 0x85);
								}
								else if((tmp & 0x0f) == 6)				// 3/4
								{
									_mt_fe_dmd_set_reg(0x7c, 0x01);
									_mt_fe_dmd_set_reg(0x80, 0x98);
								}
								else if((tmp & 0x0f) == 5)				// 2/3
								{
									_mt_fe_dmd_set_reg(0x7c, 0x01);
									_mt_fe_dmd_set_reg(0x80, 0xa8);
								}
								else if((tmp & 0x0f) == 4)				// 3/5
								{
									_mt_fe_dmd_set_reg(0x7c, 0x01);
									_mt_fe_dmd_set_reg(0x80, 0xc9);
								}
							}
							else if((tmp & 0xc0) == 0x00)		// QPSK
							{
								_mt_fe_dmd_set_reg(0x7c, 0x00);
								_mt_fe_dmd_set_reg(0x80, 0x88);
								_mt_fe_dmd_set_reg(0x81, 0x88);
							}
						}
					}
					else
					{
						_mt_fe_dmd_get_reg(0x76, &tmp);
						if(((tmp & 0x40) == 0) && (g_eq_coef_check == 0))		// 21 taps and first time
						{
							_mt_fe_dmd_get_reg(0xb3, &tmp);
							if((tmp & 0x3f) >= 0x21)
							{
								_mt_fe_dmd_get_reg(0x7e, &tmp);
								if((tmp & 0x20) == 0)	// pilot off
								{
									U32 tap_val[21], main_tap_value, other_tap_value, ratio;
									U8 i, bit01_00;
									U16 bit09_00, bit09_02;
									S32 real, image;

									for(i = 0; i < 21; i ++)
									{

										_mt_fe_dmd_set_reg(0x7A, (U8)(i << 2));
										_mt_fe_dmd_get_reg(0x7A, &tmp);
										bit01_00 = tmp & 0x03;
										_mt_fe_dmd_get_reg(0x7B, &tmp);
										bit09_02 = tmp << 2;
										bit09_00 = bit09_02 | bit01_00;
										real = (S32)bit09_00;
										if(real >= 512)
										{
											real -= 1024;
										}

										_mt_fe_dmd_set_reg(0x7A, (U8)((i + 21) << 2));
										_mt_fe_dmd_get_reg(0x7A, &tmp);
										bit01_00 = tmp & 0x03;
										_mt_fe_dmd_get_reg(0x7B, &tmp);
										bit09_02 = tmp << 2;
										bit09_00 = bit09_02 | bit01_00;
										image = (int)bit09_00;
										if(image >= 512)
										{
											image -= 1024;
										}

										tap_val[i] = real * real + image * image;
									}

									main_tap_value = tap_val[10];

									other_tap_value = 0;

									for(i = 0; i < 10; i ++)
									{
										other_tap_value += tap_val[i];
										other_tap_value += tap_val[i + 11];
									}

									if(main_tap_value != 0)
									{
										ratio = (other_tap_value * 100 + main_tap_value - 1) / main_tap_value;
									}
									else
									{
										ratio = 0;
									}

									if(ratio > MT_FE_S2_EQ_COEF_THRESHOLD)
									{
										_mt_fe_dmd_set_reg(0xbd, 0x03);

										_mt_fe_dmd_set_reg(0xb2, 0x01);
										_mt_fe_dmd_set_reg(0x00, 0x01);
										_mt_sleep(1);
										_mt_fe_dmd_set_reg(0x00, 0x00);
										_mt_fe_dmd_set_reg(0xb2, 0x00);

										g_eq_coef_check = 1;
									}
								}
							}
						}
					}

					_mt_fe_dmd_get_reg(0x0d, &tmp);
					if ((tmp & 0x8f) == 0x8f)
					{
						lockstate = MtFeLockState_Locked;
					}
				}
				else
				{
					_mt_fe_dmd_get_reg(0xd1, &tmp);
					if ((tmp & 0x07) == 0x07)
					{
						_mt_fe_dmd_get_reg(0x0d, &tmp);
						if((tmp & 0x07) == 0x07)
							lockstate = MtFeLockState_Locked;
					}
				}


				if(lockstate == MtFeLockState_Locked)
				{
					S32 carrier_offset;
					U8 reg_0x0c;

					_mt_fe_dmd_get_reg(0x0c, &reg_0x0c);
					if(((reg_0x0c & 0x04) == 0x00) && (use_unicable_device == 0))
					{
						// AFC on
						_mt_fe_dmd_ds3k_get_total_carrier_offset(&carrier_offset);
						if((carrier_offset > ((S32)sym_rate_KSs)) || (carrier_offset < -((S32)sym_rate_KSs)))
						{
							mt_fe_tn_set_tuner(centerfreq_KHz, sym_rate_KSs, lpf_offset_KHz);
							real_freq_KHz = centerfreq_KHz;
							deviceoffset_KHz = 0;

							_mt_fe_dmd_set_reg(0xb2, 0x01);

							_mt_fe_dmd_ds3k_bs_set_demod(sym_rate_KSs, current_mode);

							if((sym_rate_KSs <= 5000)&&(current_mode == MtFeType_DvbS2))
							{
								_mt_fe_dmd_set_reg(0xc0, 0x04);
								_mt_fe_dmd_set_reg(0x8a, 0x09);
								_mt_fe_dmd_set_reg(0x8b, 0x22);
								_mt_fe_dmd_set_reg(0x8c, 0x88);
							}

							_mt_fe_dmd_get_reg(0x0c, &reg_0x0c);
							reg_0x0c |= 0x04;
							_mt_fe_dmd_set_reg(0x0c, reg_0x0c);


							if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
							{
								_mt_fe_dmd_ds3k_set_carrier_offset(-(S32)mt_fe_tn_get_offset() - lpf_offset_KHz - deviceoffset_KHz);
							}
							else
							{
								_mt_fe_dmd_ds3k_set_carrier_offset(+(S32)mt_fe_tn_get_offset() + lpf_offset_KHz + deviceoffset_KHz);
							}

							_mt_fe_dmd_set_reg(0xb2, 0x00);

							count = 40;
							lockstate = MtFeLockState_Waiting;
							mt_fe_print(("\tCarrier offset out of range! Frequency = %d, CarrierOffset = %d, SymbolRate = %d\n", centerfreq_KHz, carrier_offset, sym_rate_KSs));
						}
						else
						{
							// normal carrier offset
							break;
						}
					}
					else
					{
						// AFC off
						break;
					}
				}
				else
				{
					_mt_fe_dmd_get_reg(0xb3, &tmp);
					if((tmp & 0x80) == 0x80)
					{
						timing_fail++;
						if(timing_fail >= timing_thr)
						{
							timing_fail = 1;
							break;
						}
						mt_fe_dmd_ds3k_soft_reset();
						count = 30;
					}
					else if(((tmp & 0x40) == 0x40)&&((tmp & 0x80) != 0x80))
					{
						crl_fail++;
						if(crl_fail >= crl_thr)
						{
							crl_fail = 1;
							break;
						}
						mt_fe_dmd_ds3k_soft_reset();
						count = 50;
					}
				}

				_mt_sleep(50);
				count--;
			}

			if(count == 0)
			{
				mt_fe_bs_notify(MtFeMsg_BSTpUnlock, &(p_bs_info->p_tp_info[index]));
				p_bs_info->p_tp_info[index].dvb_type = MtFeType_DvbS_Checked;
				break;
			}


			if(timing_fail == 1)
			{
				mt_fe_bs_notify(MtFeMsg_BSTpUnlock, &(p_bs_info->p_tp_info[index]));
				p_bs_info->p_tp_info[index].dvb_type = MtFeType_DvbS_Checked;
				break;
			}
			else if(crl_fail == 1)
			{
				if(current_mode == MtFeType_DvbS)
				{
					continue;
				}
				else
				{
					mt_fe_bs_notify(MtFeMsg_BSTpUnlock, &(p_bs_info->p_tp_info[index]));
					p_bs_info->p_tp_info[index].dvb_type = MtFeType_DvbS_Checked;
					break;
				}
			}

			if(lockstate == MtFeLockState_Locked)
			{
				_mt_fe_dmd_ds3k_get_carrier_offset(&carrieroffset_KHz);


				if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
				{
					truefreq_KHz = real_freq_KHz - (S32)mt_fe_tn_get_offset() + carrieroffset_KHz;
				}
				else
				{
					truefreq_KHz = real_freq_KHz + (S32)mt_fe_tn_get_offset() - carrieroffset_KHz;
				}

				_mt_fe_dmd_ds3k_get_sym_rate(&sym_rate_KSs);

				same_locked_TP_flag = 0;
				for(i = (S16)(g_scanned_tp_cnt - 1); i >= 0; i--)
				{
					if(g_blindscan_cancel) break;

					if(p_bs_info->p_tp_info[i].freq_KHz > truefreq_KHz)
						delta_freq_KHz = p_bs_info->p_tp_info[i].freq_KHz - truefreq_KHz;
					else
						delta_freq_KHz = truefreq_KHz - p_bs_info->p_tp_info[i].freq_KHz;

					if(p_bs_info->p_tp_info[i].sym_rate_KSs > sym_rate_KSs)
						delta_smy_rate_KSs =  p_bs_info->p_tp_info[i].sym_rate_KSs - sym_rate_KSs;
					else
						delta_smy_rate_KSs =  sym_rate_KSs - p_bs_info->p_tp_info[i].sym_rate_KSs;


					if(((delta_smy_rate_KSs < 50) && (delta_freq_KHz < (p_bs_info->p_tp_info[i].sym_rate_KSs / 2)))&&
					   (p_bs_info->p_tp_info[i].dvb_type == current_mode))
					{
						same_locked_TP_flag = 1;
						p_scanned_tp_info[index].dvb_type = MtFeType_DvbS_Checked;

						break;
					}
				}

				if(same_locked_TP_flag == 1)
					break;

				g_locked_tp_cnt++;
				p_bs_info->p_tp_info[index].freq_KHz 	 = truefreq_KHz;
				p_bs_info->p_tp_info[index].sym_rate_KSs = (U16)sym_rate_KSs;
				p_bs_info->p_tp_info[index].dvb_type 	 = current_mode;
				_mt_fe_dmd_ds3k_get_fec(&(p_bs_info->p_tp_info[index]), current_mode);

				p_bs_info->tp_num = g_locked_tp_cnt;

				if(spectrum_inverted == 1)
				{
					g_first_TP_locked			 = 1;
					g_first_TP_freq_KHz			 = truefreq_KHz;
					g_first_TP_symbol_rate_KSs	 = sym_rate_KSs;
				}


			#if ((MT_FE_TS_CLOCK_AUTO_SET_FOR_CI_MODE != 0) || (MT_FE_TS_CLOCK_AUTO_SET_FOR_SERIAL_MODE != 0))
				_mt_fe_dmd_ds3k_set_clock_ratio();
			#endif

				mt_fe_bs_notify(MtFeMsg_BSTpLocked, &(p_bs_info->p_tp_info[index]));

				break;
			}
			else
			{
				if(current_mode == MtFeType_DvbS2)
					p_bs_info->p_tp_info[index].dvb_type = MtFeType_DvbS_Checked;

				mt_fe_bs_notify(MtFeMsg_BSTpUnlock, &(p_bs_info->p_tp_info[index]));
			}
		}
	}


	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_unicable_set_tuner(U32 freq_KHz, U32 symbol_rate_KSs, U32* real_freq_KHz, U8 ub_select, U8 bank_select, U32 ub_freq_MHz)
{
	U32 tmp_freq_MHz;
	MT_FE_DiSEqC_MSG msg;
	U16 T_chan;

	tmp_freq_MHz = freq_KHz / 1000;

	T_chan = (U16)((tmp_freq_MHz + ub_freq_MHz)/4 - 350);

	msg.data_send[0] = 0xe0;
	msg.data_send[1] = 0x00;
	msg.data_send[2] = 0x5a;

	msg.data_send[3] = (U8)((ub_select << 5) & 0xE0);
	msg.data_send[3] = (U8)(msg.data_send[3] + ((bank_select << 2) & 0x1C));
	msg.data_send[3] = (U8)(msg.data_send[3] + ((T_chan >> 8) & 0x03));

	msg.data_send[4] = (U8)(T_chan & 0xff);

	msg.size_send = 5;
	msg.is_enable_receive = FALSE;
	msg.is_envelop_mode = FALSE;

	mt_fe_dmd_ds3k_DiSEqC_send_receive_msg(&msg);


	//_mt_sleep(500);


	tmp_freq_MHz += ub_freq_MHz;
	tmp_freq_MHz /= 4;
	tmp_freq_MHz *= 4;
	tmp_freq_MHz -= ub_freq_MHz;

	*real_freq_KHz = tmp_freq_MHz * 1000;

	mt_fe_tn_set_tuner(ub_freq_MHz * 1000, symbol_rate_KSs, 0);

	return MtFeErr_Ok;
}


MT_FE_RET _mt_fe_dmd_ds3k_get_CCI(S32 *pCCIFreq, S32 *pCCIAmp)
{
	U32	bit07_00 = 0;
	U32	bit15_08 = 0;
	U32	bit15_00 = 0;
	U8	val;
	S32	x = 0;

	_mt_fe_dmd_get_reg(0x5C, &val);
	bit07_00 = val;

	_mt_fe_dmd_get_reg(0x5B, &val);
	bit15_08 = val;

	bit15_00 = bit15_08<<8 | bit07_00;
	x = bit15_00;
	if(x >= 32768)
	{
		x -= 65536;
	}
	*pCCIFreq = x;

	_mt_fe_dmd_get_reg(0x5A, &val);
	bit07_00 = val;

	_mt_fe_dmd_get_reg(0x59, &val);
	bit15_08 = val;

	bit15_00 = bit15_08<<8 | bit07_00;
	x = bit15_00;
	if(x >= 32768)
	{
		x -= 65536;
	}
	*pCCIAmp = x / 128;

	return MtFeErr_Ok;
}


MT_FE_BOOL mt_fe_ds3k_unicable_scan_UB(U32 startFreq, U32 *result_freq, S32 *pCCIAmp, U32 *pCCIGain)	// Unit: KHz
{
	U8 i, cnt, reg_0xb3, reg_0x59, CCI_on_flag_cnt;
	S8 val_cci;

	U8 val;

	U8 Readval = 0, DeciMode = 0;
	S32 x = 0;
	U32 bit15_08 = 0, bit15_00 = 0;
	DB fCCIFreq = 0.0, fTotalFO = 0.0;
	S32 CCIFreq = 0, CCIAmp = 0;

	S32 MaxFreq, MinFreq;
	S32 MaxAmp, MinAmp;

	*result_freq = 0;
	*pCCIAmp = 0;
	*pCCIGain = 0;

	spectrum_inverted = (U8)(iq_inverted_status ^ use_unicable_device);		// XOR

	mt_fe_tn_set_tuner(startFreq, 40000, 0);

	_mt_fe_dmd_set_reg(0xb2, 0x01);

	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_set_reg(0x00, 0x01);
	}


	_mt_fe_dmd_ds3k_init_reg(cci_detect_reg_tbl_def, sizeof(cci_detect_reg_tbl_def)/2);


	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		if(spectrum_inverted == 1)			// 0x4d, bit 1
		{
			_mt_fe_dmd_get_reg(0x4d, &val);
			val |= 0x02;
			_mt_fe_dmd_set_reg(0x4d, val);
		}
		else
		{
			_mt_fe_dmd_get_reg(0x4d, &val);
			val &= ~0x02;
			_mt_fe_dmd_set_reg(0x4d, val);
		}

		if(agc_polarization == 1)			// 0x30, bit 4
		{
			_mt_fe_dmd_get_reg(0x30, &val);
			val |= 0x10;
			_mt_fe_dmd_set_reg(0x30, val);
		}
		else
		{
			_mt_fe_dmd_get_reg(0x30, &val);
			val &= ~0x10;
			_mt_fe_dmd_set_reg(0x30, val);
		}
	}


	_mt_fe_dmd_ds3k_set_sym_rate(40000);

	if((g_dmd_id == MtFeDmdId_DS3002B) || (g_dmd_id == MtFeDmdId_DS3103))
	{
		_mt_fe_dmd_set_reg(0x00, 0x00);
	}
	_mt_fe_dmd_set_reg(0xb2, 0x00);


	_mt_sleep(100);

	cnt = 0;

	_mt_fe_dmd_get_reg(0xb3, &reg_0xb3);

	while((reg_0xb3 == 0x38) || (reg_0xb3 <= 3))
	{
		cnt ++;

		if(cnt > 10)
		{
			mt_fe_print(("\tFailed to get CCI 1!\n"));
			return MtFe_False;
		}

		_mt_sleep(10);

		_mt_fe_dmd_get_reg(0xb3, &reg_0xb3);
	}

	CCI_on_flag_cnt = 0;
	for(i = 0; i < 15; i++)
	{
		_mt_fe_dmd_get_reg(0x59, &reg_0x59);
		val_cci = (S8)reg_0x59;

		_mt_fe_dmd_get_reg(0xb3, &reg_0xb3);

		if(val_cci < 0)
		{
			CCI_on_flag_cnt = 0;
			break;
		}

		if(val_cci > 2)		// 2				//MT_FE_CCI_THRESHOLD
			CCI_on_flag_cnt++;
	}


	if (CCI_on_flag_cnt < 10)
	{
		mt_fe_print(("\tFailed to get CCI 2!\n"));
		return MtFe_False;
	}

	_mt_sleep(1);


	_mt_fe_dmd_get_reg(0x5D, &Readval);
	Readval = (U8)(Readval & 0xF8);
	_mt_fe_dmd_set_reg(0x5D, Readval);

	_mt_fe_dmd_get_reg(0x5F, &Readval);
	bit15_08 = Readval << 8;

	_mt_fe_dmd_get_reg(0x5E, &Readval);

	bit15_00 = bit15_08 | Readval;
	x = (int)bit15_00;
	if(x >= 32768)
	{
		x -= 65536;
	}

	fTotalFO = x / 65536.0 * MT_FE_MCLK_KHZ;


	_mt_fe_dmd_get_reg(0xC9, &Readval);
	DeciMode = (U8)((Readval& 0x70) >> 4);


	MaxAmp = 0x80000000;
	MinAmp = 0x7FFFFFFF;
	MaxFreq = 0x80000000;
	MinFreq = 0x7FFFFFFF;

	for(i = 0; i < 5; i ++)
	{
		_mt_fe_dmd_ds3k_get_CCI(&CCIFreq, &CCIAmp);

		mt_fe_print(("\tFreq = %d, Amp = %d\n", CCIFreq, CCIAmp));

		if(MaxFreq < CCIFreq)	MaxFreq = CCIFreq;
		if(MinFreq > CCIFreq)	MinFreq = CCIFreq;
		if(MaxAmp < CCIAmp)		MaxAmp = CCIAmp;
		if(MinAmp > CCIAmp)		MinAmp = CCIAmp;

		if((MaxAmp - MinAmp) >= 0x10)
		{
			mt_fe_print(("\tMaxAmp = %d, MinAmp = %d\n", MaxAmp, MinAmp));
			return MtFe_False;
		}

		if((MaxFreq - MinFreq) >= 0x100)
		{
			mt_fe_print(("\tMaxFreq = %d, MinFreq = %d\n", MaxFreq, MinFreq));
			return MtFe_False;
		}

		_mt_sleep(10);
	}

	*pCCIAmp = CCIAmp;

	fCCIFreq = (CCIFreq / 65536.0 * MT_FE_MCLK_KHZ) / (1 << DeciMode) - fTotalFO;
	mt_fe_print(("\tCCI Offset = [%d], TN_Offset = [%d]\n", (S32)fCCIFreq, mt_fe_tn_get_offset()));

	if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
	{
		//#if (MT_FE_TN_SUPPORT_SHARP6306 != 0)
		//fCCIFreq -= mt_fe_tn_get_offset();
		//#else
		fCCIFreq += mt_fe_tn_get_offset();
		//#endif
	}
	else
	{
		//#if (MT_FE_TN_SUPPORT_SHARP6306 != 0)
		//fCCIFreq += mt_fe_tn_get_offset();
		//#else
		fCCIFreq -= mt_fe_tn_get_offset();
		//#endif
	}

	if((spectrum_inverted == 1) && (g_dmd_id == MtFeDmdId_DS300X))
	{
		*result_freq = startFreq + (S32)fCCIFreq;
	}
	else
	{
		*result_freq = startFreq - (S32)fCCIFreq;
	}

	mt_fe_print(("\tAmp=[%d],Freq=[%d],CciFo=[%d],TnFo=[%d],Result=[%d]\n", *pCCIAmp, startFreq, (S32)fCCIFreq, mt_fe_tn_get_offset(), *result_freq));

	*result_freq += 500;
	*result_freq /= 1000;

	mt_fe_tn_get_gain(pCCIGain);

	if(*pCCIGain >= 0x80000000)
	{
		*pCCIGain = 0;
	}

	if(*pCCIGain < 1151)
	{
		*pCCIGain = 1151;
	}

	return MtFe_True;
}


MT_FE_BOOL _mt_fe_dmd_ds3k_unicable_check_result_duplicate(MT_FE_UNICABLE_DEVICE *p_ub_list, U16 count, U32 target_freq_MHz)
{
	U16 i;

	for(i = 0; i < count; i ++)
	{
		if(target_freq_MHz < (p_ub_list[i].freq_MHz - 1))
		{
			continue;
		}

		if(target_freq_MHz > (p_ub_list[i].freq_MHz + 1))
		{
			continue;
		}

		return MtFe_True;
	}

	return MtFe_False;
}

void mt_fe_dmd_ds3k_unicable_blind_detect(U16 start_freq_MHz, U16 stop_freq_MHz, MT_FE_UNICABLE_DEVICE *p_ub_list, BOOL need_init)
{
	U16 cur_freq_MHz = 0;

	U32 CCI_result_MHz = 0;
	S32 CCI_Amp = 0;
	U32 CCI_Gain = 0;

	U16 i, ub_count = 0;

	U32 fft_result = 0;
	U32 fft_freq = 0;
	U32 fft_gain = 0;

	U32 min_gain = 0xFFFFFFFF, max_gain = 0;

	U32 total_gain = 0;

	g_ub_count = 0;

	spectrum_inverted = (U8)(iq_inverted_status ^ use_unicable_device);		// XOR

	if(need_init != FALSE)
	{
		MT_FE_DiSEqC_MSG	msg;

		msg.data_send[0] = 0xE0;
		msg.data_send[1] = 0x10;
		msg.data_send[2] = 0x5B;
		msg.data_send[3] = 0x00;
		msg.data_send[4] = 0x00;

		msg.size_send = 5;
		msg.is_enable_receive = FALSE;
		msg.is_envelop_mode = FALSE;

		mt_fe_dmd_ds3k_set_LNB(MtFe_True, MtFe_False, MtFeLNB_18V, MtFe_False);
		mt_fe_dmd_ds3k_DiSEqC_send_receive_msg(&msg);
		_mt_sleep(10);
		mt_fe_print(("\tDiSEqC messages have been sent out!\n"));
	}

	for(cur_freq_MHz = start_freq_MHz + 15; cur_freq_MHz < stop_freq_MHz; cur_freq_MHz += 30)
	{
		mt_fe_print(("\n\t--------[%4d]MHz--------\n", cur_freq_MHz));

		if(mt_fe_ds3k_unicable_scan_UB(cur_freq_MHz * 1000, &CCI_result_MHz, &CCI_Amp, &CCI_Gain) == MtFe_True)
		{
			if(_mt_fe_dmd_ds3k_unicable_check_result_duplicate(p_ub_list, g_ub_count, CCI_result_MHz) == MtFe_False)
			{
				U32 Verify_MHz = 0;
				S32 Verify_Amp = 0;
				U32 Verify_Gain = 0;

				mt_fe_print(("\t[CCI -- %4d]MHz----[AMP -- %4d]----[Gain -- %d]\n", CCI_result_MHz, CCI_Amp, CCI_Gain));

				if(mt_fe_ds3k_unicable_scan_UB((CCI_result_MHz + 5) * 1000, &Verify_MHz, &Verify_Amp, &Verify_Gain) == MtFe_True)
				{
					mt_fe_print(("\t[VFY -- %4d]MHz----[AMP -- %4d]----[Gain -- %d]\n", Verify_MHz, Verify_Amp, Verify_Gain));

					if(Verify_MHz == CCI_result_MHz)
					{
						p_ub_list[g_ub_count].freq_MHz = CCI_result_MHz;
						p_ub_list[g_ub_count].result_MHz = CCI_result_MHz;
						p_ub_list[g_ub_count].result_gain = CCI_Gain;
						p_ub_list[g_ub_count].UB_ok = TRUE;

						g_ub_count ++;

						if(CCI_Gain > max_gain)		max_gain = CCI_Gain;
						if(CCI_Gain < min_gain)		min_gain = CCI_Gain;
					}
				}
			}
		}
	}

	mt_fe_print(("\n\tTotal %d %s found!\n\n", g_ub_count, (g_ub_count == 1) ? "UB is" : "UBs are"));

	if(min_gain > max_gain)
	{
		min_gain = max_gain;
	}

	ub_count = g_ub_count;
	if(g_ub_count > 0)
	{
		U32 average_gain = 0;
		U32 gain_ratio = 0;
		U32 valid_ub_num = 0;

		total_gain = 0;

		for(i = 0; i < g_ub_count; i ++)
		{
			if(p_ub_list[i].UB_ok == TRUE)
			{
				gain_ratio = p_ub_list[i].result_gain * 10 / min_gain;

				if(gain_ratio < 15)
				{
					total_gain += p_ub_list[i].result_gain;
					valid_ub_num ++;
				}
			}
		}

		if(valid_ub_num > 0)
		{
			average_gain = total_gain / valid_ub_num;

			for(i = 0; i < g_ub_count; i ++)
			{
				if(p_ub_list[i].UB_ok == TRUE)
				{
					gain_ratio = p_ub_list[i].result_gain * 100 / average_gain;

					mt_fe_print(("\t[%02d]--Freq = [%4d] Gain = [%d], Avg = [%d], Ratio = [%d]--%s!\n", i, p_ub_list[i].result_MHz, p_ub_list[i].result_gain, average_gain, gain_ratio, (gain_ratio > 150) ? "FAIL" : "OK"));

					if(gain_ratio > 150)
					{
						p_ub_list[i].UB_ok = FALSE;

						ub_count --;

						continue;
					}
				}
			}
		}
	}

	g_ub_count = ub_count;

	return;
}


MT_FE_RET mt_fe_ds3k_get_quality(MT_FE_TYPE dtv_type, U32* quality)
{
	U8 npow1, npow2, spow1, tmp_val;
	U32 tmp;

	S16 cnt = 10;		// calculate total 10 times to smooth the result

	double npow = 0.0, spow = 0.0, snr_val = 0.0, val = 0.0;


	switch(dtv_type)
	{
		case MtFeType_DvbS2:
			while(cnt > 0)
			{
				_mt_fe_dmd_get_reg(0x8C, &npow1);
				_mt_fe_dmd_get_reg(0x8D, &npow2);
				npow += (((npow1 & 0x3F) + (npow2 << 6)) >> 2);

				_mt_fe_dmd_get_reg(0x8E, &spow1);
				spow += ((spow1 * spow1) >> 1);

				cnt --;
			}

			npow /= 10.0;
			spow /= 10.0;

			if(spow == 0)
			{
				snr_val = 0;
			}
			else if(npow == 0)
			{
				snr_val = 22;
			}
			else			//	limit the snr_val valid range to [-3.0, 22.0]
			{
				snr_val = (10 * log10((spow / npow)));

				if(snr_val > 22.0)
					snr_val = 22.0;

				if(snr_val < -3.0)
					snr_val = -3.0;
			}

			*quality = (U32)((snr_val + 3) * 100 / (22 + 3));

			break;

		case MtFeType_DvbS:
			tmp = 0;
			while(cnt > 0)
			{
				_mt_fe_dmd_get_reg(0xFF, &tmp_val);
				tmp += tmp_val;
				cnt--;
			}

			val = tmp / 80.0;

			if(val > 0)
			{
				snr_val = log((double)(val)) / log((double)(10)) * 10;

				if(snr_val < 4.0)
					snr_val = 4.0;

				if(snr_val > 12.0)
					snr_val = 12.0;


				*quality = (U32)((snr_val - 4) * 100 / 8);
			}
			else
			{
				*quality = 0;
			}
			break;

		default:
			break;
	}

	return MtFeErr_Ok;
}

