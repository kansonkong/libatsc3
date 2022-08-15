/****************************************************************************/
/*                   MONTAGE PROPRIETARY AND CONFIDENTIAL                   */
/*                   Montage Technology (Shanghai) Inc.                     */
/*                          All Rights Reserved                             */
/****************************************************************************/
/*
 * Filename:      mt_fe_tn_montage_ts2022.c
 *
 * Description:   Montage M88TS2022 Digital Satellite Tuner IC driver.
 *
 * Author:        Daniel.Zhu
 * Version:       1.12.01
 * Date:          2011-09-19
/****************************************************************************/

#include "mt_fe_def.h"

#if (MT_FE_TN_SUPPORT_TS2022 > 0)

#include "mt_fe_i2c.h"


#define MT_FE_CRYSTAL_KHZ				27000		/* Crystal Frequency of M88TS2022 used, unit: KHz , range: 16000 - 32000 */

#define AVG_NUM							16
#define SIGNAL_STRENGTH_RATIO			100

#define TUNER_UNDEF						0
#define TUNER_M88TS2020					1			//define the tuner Version
#define TUNER_M88TS2022					2

#define DEMOD_TYPE_ALI					0			// define the demodulator type, if it is Ali chip, please set it to 1, else set it to 0

extern void _mt_sleep(U32 ms);



static S32	g_tuner_freq_offset_KHz = 0;
static U8	gvTunerVersion = TUNER_M88TS2022;
/*****************************************************************************/
/* FUNCTIONS:
/*
/* Place holders for user to define M88TS2022 register read and write routines.
/* These should be defined elsewhere by the user.
/*
/*****************************************************************************/
void WaitTime(U16 ms)
{					//Wait time , unit : ms
	_mt_sleep(ms);
}

/***************************************************************************/
/* Function to Check the tuner version: 0: Error  1: M88TS2020  2: M88TS2022
/***************************************************************************/

U8 CheckTunerVersion(void)
{
	U8 buf;

	// Wake Up the tuner

	_mt_fe_tn_get_reg(0x00, &buf);
	buf &= 0x03;

	if(buf == 0x00)
	{
		_mt_fe_tn_set_reg(0x00, 0x01);
		WaitTime(2);
	}
	_mt_fe_tn_set_reg(0x00, 0x03);
	WaitTime(2);

	//Check the tuner version

	_mt_fe_tn_get_reg(0x00, &buf);

	if((buf == 0x01) || (buf == 0x41) || (buf == 0x81))
	{
		return TUNER_M88TS2020;
	}
	else if((buf == 0xc3) || (buf == 0x83))
	{
		return TUNER_M88TS2022;
	}
	else
	{
		return TUNER_UNDEF;
	}
}

/***************************************************************************/
/* Function to Initialize the M88TS2022 */
/***************************************************************************/

void InitialTuner(void)
{
	U8 buf;

	gvTunerVersion = CheckTunerVersion();	// check the tuner version and power on the tuner

	if(gvTunerVersion == TUNER_UNDEF)
	{
		return;
	}

	if(gvTunerVersion == TUNER_M88TS2020)
	{					//For M88TS2020
		if(TUNER_RFBYPASS_ON)
		{
			_mt_fe_tn_set_reg(0x62, 0xfd);
		}
		else
		{
			_mt_fe_tn_set_reg(0x62, 0xbd);
		}

		WaitTime(2);

		if(TUNER_CKOUT_ON)
		{
			if(TUNER_CKOUT_DIV)
			{
				buf = TUNER_CKOUT_DIV;
				buf &= 0x1f;
				_mt_fe_tn_set_reg(0x05, buf);
				WaitTime(2);
			}
			_mt_fe_tn_set_reg(0x42, 0x73);
			WaitTime(2);
		}
		else
		{
			_mt_fe_tn_set_reg(0x42, 0x63);
		}

		_mt_fe_tn_set_reg(0x07, 0x02);
		_mt_fe_tn_set_reg(0x08, 0x01);
	}
	else if(gvTunerVersion == TUNER_M88TS2022)
	{								//For M88TS2022
		if(TUNER_RFBYPASS_ON)
		{
			_mt_fe_tn_set_reg(0x62, 0xec);
		}
		else
		{
			_mt_fe_tn_set_reg(0x62, 0x6c);
		}

		WaitTime(2);

		if(TUNER_CKOUT_XTAL)
		{
			_mt_fe_tn_set_reg(0x42, 0x6c);
			WaitTime(2);
		}
		else if(TUNER_CKOUT_ON)
		{
			if(TUNER_CKOUT_DIV)
			{
				buf = TUNER_CKOUT_DIV;
				buf &= 0x1f;
				_mt_fe_tn_set_reg(0x05, buf);
				WaitTime(2);
			}
			_mt_fe_tn_set_reg(0x42, 0x70);
			WaitTime(2);
		}
		else
		{
			_mt_fe_tn_set_reg(0x42, 0x60);
		}

		_mt_fe_tn_set_reg(0x7d, 0x9d);
		_mt_fe_tn_set_reg(0x7c, 0x9a);
		_mt_fe_tn_set_reg(0x7a, 0x76);

		_mt_fe_tn_set_reg(0x3b, 0x01);
		_mt_fe_tn_set_reg(0x63, 0x88);

		_mt_fe_tn_set_reg(0x61, 0x85);
		_mt_fe_tn_set_reg(0x22, 0x30);
		_mt_fe_tn_set_reg(0x30, 0x40);
		_mt_fe_tn_set_reg(0x20, 0x23);
		_mt_fe_tn_set_reg(0x24, 0x02);
		_mt_fe_tn_set_reg(0x12, 0xa0);

		if(DEMOD_TYPE_ALI)
		{
			_mt_fe_tn_set_reg(0x07, 0x33);
			_mt_fe_tn_set_reg(0x24, 0x01);
		}
	}
}

/**************************************************************************/
/* Function to Set the M88TS2022
/*fPLL:    Frequency         			unit: MHz	from 950 to 2150
/*fSym:    SymbolRate         			unit: KS/s  from 1000 to 45000
/*lpfOffset: Set the low pass filter offset when the demodulator set the PLL offset at low symbolrate  unit: KHz
/*gainHold:  The flag of AGC gain hold, the tuner gain is hold when gainHold == 1 , default please set gainHold = 0
/*return:	 Frequency offset of PLL 	 	unit: KHz
/**************************************************************************/

S32 ts2022_SetTuner(U32 fPLL, U32 fSym, U16 lpfOffset, U8 gainHold)
{
	U8		buf, capCode, div4, changePLL, K, lpf_mxdiv, divMax, divMin, RFgain;
	U32		gdiv28;
	U32		N, lpf_gm, f3dB, fREF, divN, lpf_coeff;
	S32		freqOffset;

	//Initialize the tuner
	//InitialTuner();

	if(gvTunerVersion == TUNER_UNDEF)
	{
		return 0;
	}

	//Set the PLL

	if(gvTunerVersion == TUNER_M88TS2020)
	{
		_mt_fe_tn_set_reg(0x10, 0x00);
	}
	else if(gvTunerVersion == TUNER_M88TS2022)
	{
		_mt_fe_tn_set_reg(0x10, 0x0b);
		_mt_fe_tn_set_reg(0x11, 0x40);
	}

	div4 = 0;
	changePLL = 0;
	K = 0;
	divN = 0;
	N = 0;
	fREF = 2;

	if(gvTunerVersion == TUNER_M88TS2020)
	{
		K = (MT_FE_CRYSTAL_KHZ / 1000 + 1)/2 - 8;

		if (fPLL < 1146 )
		{
			_mt_fe_tn_set_reg(0x10, 0x11);
			div4 = 1;
			divN = fPLL * (K + 8) * 4000 / MT_FE_CRYSTAL_KHZ;
		}
		else
		{
			_mt_fe_tn_set_reg(0x10, 0x01);
			divN = fPLL * (K + 8) * 2000 / MT_FE_CRYSTAL_KHZ;
		}

		divN =divN + divN % 2;

		N = divN - 1024;

		buf = (N >> 8) & 0x0f;
		_mt_fe_tn_set_reg(0x01, buf);

		buf = N & 0xff;
		_mt_fe_tn_set_reg(0x02, buf);

		buf = K;
		_mt_fe_tn_set_reg(0x03, buf);
	}
	else if(gvTunerVersion == TUNER_M88TS2022)
	{
		if(fREF == 1)
			K = MT_FE_CRYSTAL_KHZ / 1000 - 8;
		else
			K = (MT_FE_CRYSTAL_KHZ / 1000 + 1) / 2 - 8;

		if (fPLL < 1103)
		{
			_mt_fe_tn_set_reg(0x10, 0x1b);
			div4 = 1;
			divN = fPLL * (K+8) * 4000 / MT_FE_CRYSTAL_KHZ;
		}
		else
		{
			divN = fPLL * (K+8) * 2000 / MT_FE_CRYSTAL_KHZ;
		}

		divN = divN + divN % 2;

		if (divN < 4095)
		{
			N = divN - 1024;
		}
		else if (divN < 6143)
		{
			N = divN + 1024;
		}
		else
		{
			N = divN + 3072;
		}

		buf = (N >> 8) & 0x3f;
		_mt_fe_tn_set_reg(0x01, buf);

		buf = N & 0xff;
		_mt_fe_tn_set_reg(0x02, buf);

		buf = K;
		_mt_fe_tn_set_reg(0x03, buf);
	}

	_mt_fe_tn_set_reg(0x51, 0x0f);
	_mt_fe_tn_set_reg(0x51, 0x1f);
	_mt_fe_tn_set_reg(0x50, 0x10);
	_mt_fe_tn_set_reg(0x50, 0x00);
	WaitTime(5);

	_mt_fe_tn_get_reg(0x15, &buf);
	if((buf & 0x40) != 0x40)
	{
		_mt_fe_tn_set_reg(0x51, 0x0f);
		_mt_fe_tn_set_reg(0x51, 0x1f);
		_mt_fe_tn_set_reg(0x50, 0x10);
		_mt_fe_tn_set_reg(0x50, 0x00);
		WaitTime(5);
	}

	if(gvTunerVersion == TUNER_M88TS2020)
	{
		_mt_fe_tn_get_reg(0x66, &buf);
		changePLL = (((buf & 0x80) >> 7) != div4);

		if(changePLL)
		{
			_mt_fe_tn_set_reg(0x10, 0x11);

			div4 = 1;

			divN = fPLL * (K + 8) * 4000 / MT_FE_CRYSTAL_KHZ;
			divN = divN + divN % 2;
			N = divN - 1024;

			buf = (N >>8) & 0x0f;
			_mt_fe_tn_set_reg(0x01, buf);

			buf = N & 0xff;
			_mt_fe_tn_set_reg(0x02, buf);

			_mt_fe_tn_set_reg(0x51, 0x0f);
			_mt_fe_tn_set_reg(0x51, 0x1f);
			_mt_fe_tn_set_reg(0x50, 0x10);
			_mt_fe_tn_set_reg(0x50, 0x00);
			WaitTime(5);
		}
	}
	else if(gvTunerVersion == TUNER_M88TS2022)
	{
		_mt_fe_tn_get_reg(0x14, &buf);
		buf &= 0x7f;

		if(buf < 64)
		{
			_mt_fe_tn_get_reg(0x10, &buf);
			buf |= 0x80;
			_mt_fe_tn_set_reg(0x10, buf);
			_mt_fe_tn_set_reg(0x11, 0x6f);

			_mt_fe_tn_set_reg(0x51, 0x0f);
			_mt_fe_tn_set_reg(0x51, 0x1f);
			_mt_fe_tn_set_reg(0x50, 0x10);
			_mt_fe_tn_set_reg(0x50, 0x00);
			WaitTime(5);

			_mt_fe_tn_get_reg(0x15, &buf);
			if((buf & 0x40) != 0x40)
			{
				_mt_fe_tn_set_reg(0x51, 0x0f);
				_mt_fe_tn_set_reg(0x51, 0x1f);
				_mt_fe_tn_set_reg(0x50, 0x10);
				_mt_fe_tn_set_reg(0x50, 0x00);
				WaitTime(5);
			}
		}

		_mt_fe_tn_get_reg(0x14, &buf);
		buf &= 0x1f;

		if(buf > 19)
		{
			_mt_fe_tn_get_reg(0x10, &buf);
			buf &= 0xfd;
			_mt_fe_tn_set_reg(0x10, buf);
		}
	}

	freqOffset = (S32)(divN * MT_FE_CRYSTAL_KHZ / (K + 8)/(div4 + 1) / 2 - fPLL * 1000);

	// set the RF gain
	if(gvTunerVersion == TUNER_M88TS2020)
	{
		_mt_fe_tn_set_reg(0x60, 0x79);
	}

	_mt_fe_tn_set_reg(0x51, 0x17);
	_mt_fe_tn_set_reg(0x51, 0x1f);
	_mt_fe_tn_set_reg(0x50, 0x08);
	_mt_fe_tn_set_reg(0x50, 0x00);
	WaitTime(5);

	_mt_fe_tn_get_reg(0x3c, &buf);
	if(buf == 0)
	{
		_mt_fe_tn_set_reg(0x51, 0x17);
		_mt_fe_tn_set_reg(0x51, 0x1f);
		_mt_fe_tn_set_reg(0x50, 0x08);
		_mt_fe_tn_set_reg(0x50, 0x00);
		WaitTime(5);
	}

	if(gvTunerVersion == TUNER_M88TS2020)
	{
		_mt_fe_tn_get_reg(0x3d, &buf);
		RFgain = buf & 0x0f;

		if(RFgain < 15)
		{
			if(RFgain < 4)	RFgain = 0;
			else			RFgain = RFgain - 3;

			buf = ((RFgain << 3) | 0x01) & 0x79;
			_mt_fe_tn_set_reg(0x60, buf);

			_mt_fe_tn_set_reg(0x51, 0x17);
			_mt_fe_tn_set_reg(0x51, 0x1f);
			_mt_fe_tn_set_reg(0x50, 0x08);
			_mt_fe_tn_set_reg(0x50, 0x00);
			WaitTime(5);
		}
	}

	// set the LPF

	if(gvTunerVersion == TUNER_M88TS2022)
	{
		_mt_fe_tn_set_reg(0x25, 0x00);
		_mt_fe_tn_set_reg(0x27, 0x70);
		_mt_fe_tn_set_reg(0x41, 0x09);

		_mt_fe_tn_set_reg(0x08, 0x0b);
	}

	f3dB = fSym * 135 / 200 + 2000;

	f3dB += lpfOffset;

	if(f3dB < 7000)		f3dB = 7000;
	if(f3dB > 40000)	f3dB = 40000;

	gdiv28 = (MT_FE_CRYSTAL_KHZ / 1000 * 1694 + 500) / 1000;

	buf = (U8)gdiv28;
	_mt_fe_tn_set_reg(0x04, buf);

	_mt_fe_tn_set_reg(0x51, 0x1b);
	_mt_fe_tn_set_reg(0x51, 0x1f);
	_mt_fe_tn_set_reg(0x50, 0x04);
	_mt_fe_tn_set_reg(0x50, 0x00);
	WaitTime(2);

	_mt_fe_tn_get_reg(0x26, &buf);
	if(buf == 0x00)
	{
		_mt_fe_tn_set_reg(0x51, 0x1b);
		_mt_fe_tn_set_reg(0x51, 0x1f);
		_mt_fe_tn_set_reg(0x50, 0x04);
		_mt_fe_tn_set_reg(0x50, 0x00);
		WaitTime(2);

		_mt_fe_tn_get_reg(0x26, &buf);
	}

	capCode = buf & 0x3f;

	if(gvTunerVersion == TUNER_M88TS2022)
	{
		_mt_fe_tn_set_reg(0x41, 0x0d);

		_mt_fe_tn_set_reg(0x51, 0x1b);
		_mt_fe_tn_set_reg(0x51, 0x1f);
		_mt_fe_tn_set_reg(0x50, 0x04);
		_mt_fe_tn_set_reg(0x50, 0x00);
		WaitTime(2);

		_mt_fe_tn_get_reg(0x26, &buf);
		if(buf == 0x00)
		{
			_mt_fe_tn_set_reg(0x51, 0x1b);
			_mt_fe_tn_set_reg(0x51, 0x1f);
			_mt_fe_tn_set_reg(0x50, 0x04);
			_mt_fe_tn_set_reg(0x50, 0x00);
			WaitTime(2);

			_mt_fe_tn_get_reg(0x26, &buf);
		}

		buf &= 0x3f;
		capCode = (capCode + buf) / 2;
	}

	gdiv28 = gdiv28 * 207 / (capCode * 2 + 151);

	divMax = gdiv28 * 135 / 100;
	divMin = gdiv28 * 78 / 100;

	if(divMax > 63)	divMax = 63;

	if(gvTunerVersion == TUNER_M88TS2020)
	{
		lpf_coeff = 2766;
	}
	else if(gvTunerVersion == TUNER_M88TS2022)
	{
		lpf_coeff = 3200;
	}

	lpf_gm = (f3dB * gdiv28 * 2 / lpf_coeff / (MT_FE_CRYSTAL_KHZ / 1000) + 1) / 2;

	if (lpf_gm > 23)	lpf_gm = 23;
	if (lpf_gm < 1)		lpf_gm = 1;

	lpf_mxdiv = (lpf_gm * (MT_FE_CRYSTAL_KHZ / 1000) * lpf_coeff * 2 / f3dB + 1) / 2;

	if (lpf_mxdiv < divMin)
	{
		lpf_gm++;
		lpf_mxdiv = (lpf_gm * (MT_FE_CRYSTAL_KHZ / 1000) * lpf_coeff * 2 / f3dB + 1) / 2;
	}

	if (lpf_mxdiv > divMax)
	{
		lpf_mxdiv = divMax;
	}

	buf = lpf_mxdiv;
	_mt_fe_tn_set_reg(0x04, buf);

	buf = lpf_gm;
	_mt_fe_tn_set_reg(0x06, buf);

	_mt_fe_tn_set_reg(0x51, 0x1b);
	_mt_fe_tn_set_reg(0x51, 0x1f);
	_mt_fe_tn_set_reg(0x50, 0x04);
	_mt_fe_tn_set_reg(0x50, 0x00);
	WaitTime(2);

	_mt_fe_tn_get_reg(0x26, &buf);
	if(buf == 0x00)
	{
		_mt_fe_tn_set_reg(0x51, 0x1b);
		_mt_fe_tn_set_reg(0x51, 0x1f);
		_mt_fe_tn_set_reg(0x50, 0x04);
		_mt_fe_tn_set_reg(0x50, 0x00);
		WaitTime(2);

		_mt_fe_tn_get_reg(0x26, &buf);
	}

	if(gvTunerVersion == TUNER_M88TS2022)
	{
		capCode = buf & 0x3f;

		_mt_fe_tn_set_reg(0x41, 0x09);

		_mt_fe_tn_set_reg(0x51, 0x1b);
		_mt_fe_tn_set_reg(0x51, 0x1f);
		_mt_fe_tn_set_reg(0x50, 0x04);
		_mt_fe_tn_set_reg(0x50, 0x00);
		WaitTime(2);

		_mt_fe_tn_get_reg(0x26, &buf);
		if(buf == 0x00)
		{
			_mt_fe_tn_set_reg(0x51, 0x1b);
			_mt_fe_tn_set_reg(0x51, 0x1f);
			_mt_fe_tn_set_reg(0x50, 0x04);
			_mt_fe_tn_set_reg(0x50, 0x00);
			WaitTime(2);

			_mt_fe_tn_get_reg(0x26, &buf);
		}

		buf &= 0x3f;
		capCode = (capCode + buf) / 2;

		buf = capCode | 0x80;
		_mt_fe_tn_set_reg(0x25, buf);
		_mt_fe_tn_set_reg(0x27, 0x30);

		_mt_fe_tn_set_reg(0x08, 0x09);
	}

	// Set the BB gain
	// default should set gainHold = 0;
	// except when the AGC of demodulator is hold, for example application at blind scan use Haier demodulator

	if(gainHold == 0)
	{
		_mt_fe_tn_set_reg(0x51, 0x1e);
		_mt_fe_tn_set_reg(0x51, 0x1f);
		_mt_fe_tn_set_reg(0x50, 0x01);
		_mt_fe_tn_set_reg(0x50, 0x00);
		WaitTime(20);

		_mt_fe_tn_get_reg(0x21, &buf);
		if(buf == 0x00)
		{
			_mt_fe_tn_set_reg(0x51, 0x1e);
			_mt_fe_tn_set_reg(0x51, 0x1f);
			_mt_fe_tn_set_reg(0x50, 0x01);
			_mt_fe_tn_set_reg(0x50, 0x00);
			WaitTime(20);
		}

		if(gvTunerVersion == TUNER_M88TS2020)
		{
			if(RFgain == 15)
			{
				WaitTime(20);
				_mt_fe_tn_get_reg(0x21, &buf);
				buf &= 0x0f;

				if(buf < 3)
				{
					_mt_fe_tn_set_reg(0x60, 0x61);

					_mt_fe_tn_set_reg(0x51, 0x17);
					_mt_fe_tn_set_reg(0x51, 0x1f);
					_mt_fe_tn_set_reg(0x50, 0x08);
					_mt_fe_tn_set_reg(0x50, 0x00);
					WaitTime(20);
				}
			}
		}

		//User should delay 100ms here to wait the Tuner gain stable before checking the chip lock status
		//If there have delay time at the function of setting demodulator, you can take out it to reduce the lock time;
		WaitTime(40);
	}

	return freqOffset;	// return the frequency offset : KHz
}


/***************************************************************************/
/* Function to set the M88TS2022 into Sleep mode
/***************************************************************************/

void SleepTuner(void)
{
	_mt_fe_tn_set_reg(0x00, 0x00);
}

/*************************************************************************/
/*  Function to get the tuner gain
/*  Vagc:the voltage of the AGC from the demodulator;	unit: mV  from 0 to 2600
/*  Return: total gain of tuner	in 0.01dB
/*  How to calculate the signal strength use this function please refer to the driver user's manual
/************************************************************************/

S32	GetTunerGain(U16 Vagc)
{
	U8  buf, gain1, gain2, gain3;
	S32 gain;

	_mt_fe_tn_get_reg(0x3d, &buf);
	gain1 = buf & 0x1f;
	_mt_fe_tn_get_reg(0x21, &buf);
	gain2 = buf & 0x1f;

	if(gvTunerVersion == TUNER_M88TS2020)
	{
		if(gain1 < 0)		gain1 = 0;
		if(gain1 > 15)		gain1 = 15;
		if(gain2 < 0 )		gain2 = 0;
		if(gain2 > 13)		gain2 = 13;

		if(Vagc < 400)		Vagc = 400;
		if(Vagc > 1100)		Vagc = 1100;

		gain = (U16) gain1 * 233 + (U16) gain2 * 350 + Vagc * 24 / 10 + 1000;
	}
	else if(gvTunerVersion == TUNER_M88TS2022)
	{
		_mt_fe_tn_get_reg(0x66, &buf);
		gain3 = (buf >> 3) & 0x07;

		if(gain1 < 0)	gain1 = 0;
		if(gain1 > 15)	gain1 = 15;
		if(gain2 < 2)	gain2 = 2;
		if(gain2 > 16)	gain2 = 16;
		if(gain3 < 0)	gain3 = 0;
		if(gain3 > 6)	gain3 = 6;

		if(Vagc < 600)	Vagc = 600;
		if(Vagc > 1600)	Vagc = 1600;

		gain = (U16) gain1 * 265 + (U16) gain2 * 338 + (U16) gain3 * 285 + Vagc * 176 / 100 - 3000;
	}

	return gain;
}


/*************************************************************************/
/*  This is a demo function to calculate the signal strength use M88TS2022GetTunerGain
/*  This is defined for applications using Montage DVB-S2 demodulator
/*  This function should be defined by users if they use a different type of demodulator
/*  Return: the signal strength	0% to 100%
/************************************************************************/
U8	GetSignalStrength(void)
{
	U8  strength;
	S32  Vagc;
	S32 gain;
	MT_FE_LOCK_STATE   	lockstate;

	U8 AgcPWM;

	U8		i;
	U16		sum;
	static U8 ts2022_signal_strength[AVG_NUM];
	static U8 signal_index = 0;

	// Step 1: read the AGC PWM rate from the demodulator register

	_mt_fe_dmd_get_reg(0x3f, &AgcPWM);

	// Step 2: Calculate the AGC voltage based on the AGC PWM rate, unit: mV

	if(gvTunerVersion == TUNER_M88TS2020)
	{
		Vagc = AgcPWM * 20 - 1166;
	}
	else if(gvTunerVersion == TUNER_M88TS2022)
	{
		Vagc = AgcPWM * 16 - 670;
	}

	if(Vagc < 0)  Vagc = 0;

	// Step 3: Calculate the total gain of the tuner

	gain = GetTunerGain((U16)Vagc);			//Get the total tuner gain in 0.01dB

	// Step 4: Calculate the signal strength based on the total gain of the tuner

	if(gain > 8500)			strength = 0;								//0%           no signal or weak signal
	else if(gain > 6500)	strength = 0 + (8500 - gain) * 3/100;		//0% - 60%     weak signal
	else if(gain > 4500)	strength = 60 + (6500 - gain) * 3 / 200;	//60% - 90%    normal signal
	else					strength = 90 + (4500 - gain) / 500;		//90% - 99%    strong signal


	// Step 5: Adjust the display of the signal strength according to your requirements

	mt_fe_dmd_ds3k_get_lock_state(&lockstate);		// Get the lock status of the demodulator M88DS3002;

	// when the channel is locked, set the signal strength bigger

	if((strength < 40) && (lockstate == MtFeLockState_Locked))
		strength = 20 + strength / 2;

	//smooth the display of signal strength when the signal fluctuates, average AVG_NUM times

	ts2022_signal_strength[signal_index] = strength;
	signal_index++;
	if (signal_index == AVG_NUM)
		signal_index = 0;

	sum = 0;
	for (i = 0; i < AVG_NUM; i++)
		sum += ts2022_signal_strength[i];

	strength = sum / AVG_NUM;

	//Scale the display by multiplying the signal strength with a coefficient

	strength = (U16)strength * SIGNAL_STRENGTH_RATIO / 100;

	//Limit the display within 0% to 100% to avoid errors

	if(strength > 100)  strength = 100;
	if(strength < 0)    strength = 0;

	//Return the signal strength
	return (U8) strength;
}


/***************************************************************************/
/* Function to Initialize the M88TS2022 */
/***************************************************************************/

MT_FE_RET mt_fe_tn_init_ts2022(void)
{
	InitialTuner();

	return MtFeErr_Ok;
}

/*************************************************************************
** Function: mt_fe_tn_get_tuner_freq_offset_ts2022
**
**
** Description:	return the tuner freq. offset, unit: KHz
**
**
** Inputs: none
**
**
** Outputs:	none
**
**
*************************************************************************/
S32 mt_fe_tn_get_tuner_freq_offset_ts2022(void)
{
	return g_tuner_freq_offset_KHz;
}



/*************************************************************************/
/*	Function to Set the M88TS2022 */

/*	freq_MHz:			U32	Frequency					unit: MHz  from 950 to 2150 */
/*	sym_rate_KSs:		U32	SymbolRate					unit: KS/s from 1000 to 45000 */
/*	lpf_offset_KHz:		S16	low pass filter offset		unit: KHz*/
/************************************************************************/

MT_FE_RET mt_fe_tn_set_freq_ts2022(U32 freq_KHz, U32 sym_rate_KSs, S16 lpf_offset_KHz)
{
	U32 freq_MHz = (freq_KHz + 500) / 1000;

	g_tuner_freq_offset_KHz = ts2022_SetTuner(freq_MHz, sym_rate_KSs, lpf_offset_KHz, 0);

	g_tuner_freq_offset_KHz += (S32)(freq_MHz * 1000 - freq_KHz);

	return MtFeErr_Ok;
}

MT_FE_RET mt_fe_tn_get_gain_ts2022(U32 *p_gain)
{
	S16 Vagc;
	U32 gain;

	U8 AgcPWM;

	_mt_fe_dmd_get_reg(0x3f, &AgcPWM);

	if(gvTunerVersion == TUNER_M88TS2020)
	{
		Vagc = AgcPWM * 20 - 1166;
	}
	else if(gvTunerVersion == TUNER_M88TS2022)
	{
		Vagc = AgcPWM * 16 - 670;
	}

	if(Vagc < 0)  Vagc = 0;

	gain = GetTunerGain((U16)Vagc);

	*p_gain = gain;

	return MtFeErr_Ok;
}

/***************************************************************************/
/* Function to set the M88TS2022 into Sleep mode
/***************************************************************************/

MT_FE_RET	mt_fe_tn_sleep_ts2022(void)
{
	SleepTuner();
	WaitTime(50);
	return MtFeErr_Ok;
}

MT_FE_RET	mt_fe_tn_wake_up_ts2022(void)
{
	U8 val;

	_mt_fe_tn_get_reg(0x00, &val);
	if((val&0x01) == 0x00)
	{
		_mt_fe_tn_set_reg(0x00, 0x01);
		WaitTime(50);
	}

	_mt_fe_tn_set_reg(0x00, 0x03);
	WaitTime(50);

	return MtFeErr_Ok;
}

/*************************************************************************/
/*	This is a demo function to calculate the signal strength use mt_fe_tn_get_tuner_gain
/*  This is defined for applications using Montage ABS demodulator ES-ABS
/*  This function should be defined by users if they use a different type of demodulator
/*	Return: the signal strength			 0% to 100%
/************************************************************************/


MT_FE_RET	mt_fe_tn_get_strength_ts2022(S8 *p_strength)
{
	*p_strength = GetSignalStrength();

	return MtFeErr_Ok;
}


#endif/*#if (MT_FE_TN_SUPPORT_TS2022 > 0)*/
