//#include "stdafx.h"
//#include "windows.h"
#include <stdlib.h>
#include <unistd.h>
#include "brType.h"

#ifndef  _R855_H_ 
#define _R855_H_


 //When the R855 API fails, 
 //the user needs to print the API process message to find the error.
 //This flag needs to be set to 1.
#define R855_DEBUG  0  

#if R855_DEBUG
	#define R855_PRINT(...) printf(__VA_ARGS__)
#else
	#define R855_PRINT(...)
#endif


#define SPECTIAL_SETTING_FOR_TEST FALSE

#define R855_VERSION     "R855_MP_MT1_APIv1.2B_UIvAAB"
#define R855_VER_NUM     9
#define R855_CHIP_ID     0x97


//----------------------------------------------------------//
//                   Spectial setting for debug use         //
// if R855_SPECTIAL_SETTING = TRUE , other setting is effective.
//----------------------------------------------------------//
#define R855_SPECTIAL_SETTING FALSE //for rafael internal debug use
//pulse flag = 1,  R855_LNASTEP_AUTO sel "manual" and R855_LNASTEP_MANUAL sel 7
#define R855_LNASTEP_AUTO 1 //R41[1][0:manual, 1:ctrl by flag]
#define R855_LNASTEP_MANUAL 0 //R34[2:0][0~7]
#define R855_LPF_ACI_ENB 0   //[0:disable, 1:enable]
//ClkOut disable, CP_Offset off, Xtal PW = Highest, Xtal gm = 2*gm(24MHz), CP_Ix1, CP cur 0.7mA	
#define R855_SPUR_TEST 0
//----------------------------------------------------------//
//                   Type Define                            //
//----------------------------------------------------------//

#define UINT8  unsigned char
#define UINT16 unsigned short
#define UINT32 unsigned long
#define INT8   signed char
#define INT16  signed short
#define INT32  signed int

#define TRUE   1
#define FALSE  0
//----------------------------------------------------------//
//                   Define                                 //
//----------------------------------------------------------//
#define R855_REG_NUM          48  //R0~R7: read only
#define R855_TF_HIGH_NUM      8
#define R855_TF_MID_NUM       11
#define R855_TF_LOW_NUM       9
#define R855_RING_POWER_FREQ  115000
//#define R855_IMR_IF           5300
#define R855_IMR_TRIAL        7

//Mixer amp > 7
#define R855_IMR_GAIN_REG_G7     20		//R20[3:0]
#define R855_IMR_PHASE_REG_G7    21		//R21[3:0]
#define R855_IMR_IQCAP_REG_G7    21		//R21[6:5]

//Mixer amp < 7
#define R855_IMR_GAIN_REG_L7	 42		//R42[3:0]
#define R855_IMR_PHASE_REG_L7    42		//R42[7:4]

#define R855_IMR_POINT_NUM    8
//----------------------------------------------------------//
//                   Internal Structure                     //
//----------------------------------------------------------//
typedef struct _R855_Sys_Info_Type
{
	UINT16      IF_KHz;            
	UINT16      FILT_CAL_IF; 
	UINT8       BW;
	UINT8       HPF_COR;
	UINT8       FILT_EXT_ENA_R17_7_1BT;
	UINT8		FILT_EXT_OPT_R43_1_3BT;
	UINT8       FILT_EXT_CONDI_R34_4_1BT;
	UINT8		HPF_NOTCH_R24_7_1BT;    
	UINT8		FILT3_COMP_R36_3_2BT; 
	UINT8		FILT5_FORCEQ_R43_0_1BT;   
	UINT8		FILT5_AUTO_COMP_R36_5_2BT;   
	UINT8       FILT5_MAN_COMP_R20_4_1BT;  
	UINT8		NA_PWR_DET_R9_7_1BT;      
	UINT8       POLY_CUR_R12_3_1BT; 
	//AGC/RF
	UINT8		RF_CLASSB_CHARGE_R36_1_1BT;
	UINT8		RF_RES_CAP_R24_5_2BT;
	UINT8		BB_CLASSB_CHARGE_R17_1_1BT;
	UINT8		BB_RES_CAP_R23_6_1BT;
	//Discharge
	UINT8		LNA_PEAK_AVG_R46_5_1BT;
	UINT8		RF_PEAK_AVG_R39_7_1BT;
	UINT8		BB_PEAK_AVG_R46_2_1BT;
}R855_Sys_Info_Type;

typedef struct _R855_Freq_Info_Type
{
	UINT8		RF_POLY;
	UINT8		LNA_BAND;
	UINT8		LPF_CAP;
	UINT8		LPF_NOTCH;
	UINT8		IMR_MEM_NOR;
	UINT8		IMR_MEM_REV;
	UINT8       BYP_LPF;
	UINT8       TEMP;    
}R855_Freq_Info_Type;

typedef struct _R855_SysFreq_Info_Type
{
	UINT8		TF_MODE_R13_7_1BT;
	UINT8       Q_CTRL_R14_7_1BT;
	UINT8		LNA_MAX_GAIN_R17_2_1BT;
	UINT8       ENB_ATT_R17_0_1BT;
	UINT8		LNA_TOP_R37_0_4BT;
	UINT8		LNA_VTH_R38_0_4BT;
	UINT8		LNA_VTL_R13_1_4BT;
	UINT8       RF_TOP_R37_4_3BT;
	UINT8       RF_VTH_R38_4_4BT;
	UINT8       RF_VTL_R25_0_4BT;
	UINT8       NRB_TOP_R40_4_4BT;
	UINT8       NRB_BW_HPF_R34_5_2BT;
	UINT8       NRB_BW_LPF_R41_2_2BT;
	UINT8		MIXER_TOP_R40_0_2BT;
	UINT8		MIXER_VTH_R18_1_4BT;
	UINT8		MIXER_VTL_R39_0_3BT;
	UINT8       MIXER_GAIN_LIMIT_R20_5_2BT;
	UINT8		POLY_GAIN_R21_7_1BT;
	UINT8       FILTER_TOP_R40_2_2BT;
	UINT8       FILTER_VTH_R25_4_4BT;
	UINT8       FILTER_VTL_R39_4_3BT;
	UINT8	    MIXER_AMP_LPF_R35_2_3BT;
	UINT8		LPF_ACI_ENB_R34_7_1BT;
	UINT8		RBG_MIN_RFBUF_R13_6_1BT;
	UINT8       LNA_RF_DIS_MODE_R44_4_3BT;
	UINT8       LNA_DIS_CURR_R43_4_1BT;
	UINT8       RF_DIS_CURR_R43_5_1BT;
	UINT8       LNA_RF_DIS_SLOW_R44_0_2BT;
	UINT8       LNA_RF_DIS_FAST_R44_2_2BT;
	UINT8		SLOW_DIS_ENABLE_R41_0_1BT;
	UINT8       BB_DIS_CURR_R18_7_1BT;
	UINT8       MIXER_FILTER_DIS_R43_6_2BT;
	UINT8		FAGC_2_SAGC_R47_7_1BT;
	UINT8		CLK_FAST_R47_2_2BT;
	UINT8		CLK_SLOW_R24_1_R13_5_2BT;
	UINT8		LEVEL_SW_R45_4_1BT;
	UINT8		MODE_SEL_R45_5_1BT;
	UINT8		LEVEL_SW_VTHH_R37_7_1BT;
	UINT8		LEVEL_SW_VTLL_R46_1_1BT;
	UINT8		IMG_ACI_R9_4_1BT;
	UINT8		FILTER_G_CONTROL_R21_4_1BT;
	UINT8		WIDEN_VTH_VTL_R45_0_2BT;
	UINT8       IMG_NRB_ADDER_R45_2_2BT;
	UINT8       FILT_3TH_GAIN_MAN_R24_2_1BT;
	UINT8       HPF_COMP_R35_6_2BT;    
	UINT8       FB1_RES_R35_5_1BT;
	UINT8       VGA_PIN_LVL_R23_3_1BT;
	UINT8		VGA_ATV_MODEL_R23_1_1BT;
	UINT8       VGA_OUT_ATT_R36_2_1BT;
	UINT8       LNA_RF_CHARGE_R15_5_1BT;
	UINT8       TEMP;
}R855_SysFreq_Info_Type;

typedef struct _R855_Cal_Info_Type
{
	UINT8		FILTER_6DB;   //no use
	UINT8       TF_PATH;
	UINT8		MIXER_AMP_GAIN;
	UINT8		MIXER_BUFFER_GAIN;
	UINT8		LNA_GAIN;
	UINT8		LNA_POWER;
	UINT8		RFBUF_POWER;
	UINT8		RFBUF_OUT;
}R855_Cal_Info_Type;

typedef struct _R855_Sect_Type
{
	UINT8   Phase_Y;
	UINT8   Gain_X;
	UINT8   Iqcap;
	UINT8   Value;
}R855_Sect_Type;

typedef struct _R855_TF_Result
{
	UINT8   TF_Set;
	UINT8   TF_Value;
}R855_TF_Result;

typedef enum _R855_TF_Type
{	
	R855_TF_270N_39N = 0,  //270n/39n   (DTV Terr) 
	R855_TF_BEAD_68N,      //Bead/68n   (China DVB-C, DTMB, ATV) 
	R855_TF_270N_27N,
	R855_TF_BEAD_27N,
	R855_TF_390N_27N,
	R855_TF_390N_39N,        //only for LGIT
	R855_TF_SIZE
}R855_TF_Type;

typedef enum _R855_UL_TF_Type
{
	R855_UL_USING_BEAD = 0,            
    R855_UL_USING_270NH,                      
	R855_UL_USING_390NH,  
}R855_UL_TF_Type;

typedef enum _R855_MID_TF_Type
{
	R855_MID_USING_27NH = 0,            
	R855_MID_USING_39NH,      //39n
	R855_MID_USING_68NH,      //68n
}R855_MID_TF_Type;

typedef enum _R855_Cal_Type
{
	R855_IMR_CAL = 0,
	R855_IMR_LNA_CAL,
	R855_TF_MID_LNA_CAL,
	R855_TF_LNA_CAL,
	R855_LPF_CAL,
	R855_LPF_LNA_CAL
}R855_Cal_Type;

typedef enum _R855_Xtal_Pwr_Type
{
	//lowest, 1.5 (0)
	//lowest, 2.0 (1)
	//low, 1.5 (2)
    //low, 2.0 (3)
	//high, 1.5 (4)
	//high, 2.0 (5)
	//highest, 1.5 (6)
	//highest, 2.0 (7)
    R855_XTAL_HIGHEST = 7,
	R855_XTAL_CHECK_SIZE = 8
}R855_Xtal_Pwr_Type;

typedef enum _R855_Share_Xtal_Type
{
	R855_NO_SHARE_XTAL = 0,                     //normal 
	R855_MASTER_TO_SLAVE_XTAL_IN,     //power max, cap min     
    R855_MASTER_TO_SLAVE_XTAL_OUT,  //power max, cap min
    R855_SLAVE_XTAL_OUT,                         //xtal_gm off (R32[4:3]=2'b11), cap min
    R855_SLAVE_XTAL_IN,                            //no use. power max? cap?
}R855_Share_Xtal_Type;

typedef enum _R855_IMR_Type  
{
	R855_IMR_NOR=0,
	R855_IMR_REV,
}R855_IMR_Type;

typedef enum _R855_IMR_MIXER_GAIN_Type  
{
	R855_IMR_MIX_GAIN_G7=0,    //IMR Mixer Gain > 7
	R855_IMR_MIX_GAIN_L7,	   //IMR Mixer Gain < 7
}R855_IMR_MIXER_GAIN_Type;

typedef enum _R855_TF_Mode_Type  
{
	R855_TF_AUTO=0,
	R855_TF_PLAIN,
	R855_TF_SHARP,
}R855_TF_Mode_Type;

typedef enum _R855_Poly_Type  
{
	R855_Poly_0=0,
	R855_Poly_3,
	R855_Poly_5,	
}R855_Poly_Type;


//for LNA¡BRFbuf¡BMixer use
typedef enum _R855_VTH_Type  
{
	R855_VTH_0_16=0,
	R855_VTH_0_26,
	R855_VTH_0_36,
	R855_VTH_0_46,
	R855_VTH_0_56,
	R855_VTH_0_66,
	R855_VTH_0_76,
	R855_VTH_0_86,
	R855_VTH_0_96,
	R855_VTH_1_06,
	R855_VTH_1_16,
	R855_VTH_1_26,
	R855_VTH_1_36,
	R855_VTH_1_46,
	R855_VTH_1_56,
	R855_VTH_1_66,
}R855_VTH_Type;

//only for RFBUF VTH use
typedef enum _R855_RFBUF_VTH_Type  
{
	R855_RFBUF_VTH_0_01=0,
	R855_RFBUF_VTH_0_21,
	R855_RFBUF_VTH_0_41,
	R855_RFBUF_VTH_0_61,
	R855_RFBUF_VTH_0_81,
	R855_RFBUF_VTH_1_01,
	R855_RFBUF_VTH_1_21,
	R855_RFBUF_VTH_1_41,
}R855_RFBUF_VTH_Type;

//LNA VTL usable [R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
//RFbuf VTL usable all
//Mixer and Filter VTL usable R855_VTL_0_43 ~ R855_VTL_1_13
typedef enum _R855_VTL_Type  
{
	R855_VTL_0_43=0,
	R855_VTL_0_53,
	R855_VTL_0_63,
	R855_VTL_0_73,
	R855_VTL_0_83,
	R855_VTL_0_93,
	R855_VTL_1_03,
	R855_VTL_1_13,
	R855_VTL_1_23,
	R855_VTL_1_33,
	R855_VTL_1_43,
	R855_VTL_1_53,
	R855_VTL_1_63,
	R855_VTL_1_73,
	R855_VTL_1_83,
	R855_VTL_1_93,
}R855_VTL_Type;

//----------------------------------------------------------//
//                   R855 Public Parameter                     //
//----------------------------------------------------------//
typedef enum _R855_ErrCode
{
	R855_Success = TRUE,
	R855_Fail = FALSE
}R855_ErrCode;

typedef enum _R855_Standard_Type  //Don't remove standand list!!
{
	//DTV
	R855_DVB_T_6M = 0,  
	R855_DVB_T_7M,
	R855_DVB_T_8M, 
    R855_DVB_T2_6M,       //IF=4.57M
	R855_DVB_T2_7M,       //IF=4.57M
	R855_DVB_T2_8M,       //IF=4.57M
	R855_DVB_T2_1_7M,
	R855_DVB_T2_10M,
	R855_DVB_C_8M,
	R855_DVB_C_6M, 
	R855_J83B,
	R855_ISDB_T_4063,       //IF=4.063M
	R855_ISDB_T_4570,       //IF=4.57M
	R855_DTMB_8M_4570,      //IF=4.57M
	R855_DTMB_6M_4500,      //IF=4.5M, BW=6M
	R855_ATSC,  
	R855_ATSC3, 
	//DTV IF=5M
	R855_DVB_T_6M_IF_5M,
	R855_DVB_T_7M_IF_5M,
	R855_DVB_T_8M_IF_5M,
	R855_DVB_T2_6M_IF_5M,
	R855_DVB_T2_7M_IF_5M,
	R855_DVB_T2_8M_IF_5M,
	R855_DVB_T2_1_7M_IF_5M,
	R855_DVB_C_8M_IF_5M,
	R855_DVB_C_6M_IF_5M, 
	R855_J83B_IF_5M,
	R855_ISDB_T_IF_5M,            
	R855_DTMB_8M_IF_5M,     
	R855_DTMB_6M_IF_5M,     
	R855_ATSC_IF_5M,  
	R855_ATSC3_IF_5M, 
	R855_FM,
	R855_STD_SIZE,
}R855_Standard_Type;

typedef enum _R855_RF_Gain_Type
{
	
	R855_RF_AUTO = 0,
	R855_RF_MANUAL
}R855_RF_Gain_Type;

typedef enum _R855_IF_Gain_Type
{
	
	R855_IF_AUTO = 0,
	R855_IF_MANUAL
}R855_IF_Gain_Type;

typedef enum _R855_Xtal_Div_Type
{
	R855_XTAL_DIV1_11 = 0,
	R855_XTAL_DIV2_12,
	R855_XTAL_DIV2_21,
	R855_XTAL_DIV4_22
}R855_Xtal_Div_Type;

typedef enum _R855_IfAgc_Type
{
	R855_IF_AGC1 = 0,
	R855_IF_AGC2
}R855_IfAgc_Type;

typedef struct _R855_Set_Info
{
	UINT32                  RF_KHz;
	R855_Standard_Type     R855_Standard;
	R855_IfAgc_Type        R855_IfAgc_Select; 
}R855_Set_Info;

typedef struct _R855_RF_Gain_Info
{
	UINT16  RF_gain_comb;
	UINT8   RF_gain1;
	UINT8   RF_gain2;
	UINT8   RF_gain3;
	UINT8   RF_gain4;
}R855_RF_Gain_Info;

typedef enum _R855_TuneMode_Type
{
	R855_AUTO_SCAN = 0,
	R855_CHANNEL_CHANGE
}R855_TuneMode_Type;

typedef enum _R855_IMR_CAL_TYPE
{
	R855_IMR_AUTO_HW = 0,
	R855_IMR_MANUAL_SW
}R855_IMR_CAL_TYPE;

//----------------------------------------------------------//
//                   R855 Public Function                       //
//----------------------------------------------------------//
#define R855_Delay_MS	usleep	// by ITE

R855_ErrCode R855_Init(R855_Standard_Type R855_Standard);
R855_ErrCode R855_SetPllData(R855_Set_Info R855_INFO);               //old set frequency function, replaced by below function
R855_ErrCode R855_SetPllData_Mode(R855_Set_Info R855_INFO, R855_TuneMode_Type R855_TuningMode);  //New tuning function that can specify Channel_Change or Auto_Scan by R855_TuningMode
R855_ErrCode R855_Standby(void);
R855_ErrCode R855_WakeUp(void);
R855_ErrCode R855_GetTotalRssi(UINT32 RF_Freq_Khz, R855_Standard_Type RT_Standard, INT32 *RssiDbm);   //*RssiDbm is dBm*1
R855_ErrCode R855_GetTotalRssi2(UINT32 RF_Freq_Khz, R855_Standard_Type RT_Standard, INT32 *RssiDbm);  //*RssiDbm is dBm*10
R855_ErrCode R855_SetXtalCap(UINT8 u8XtalCap); 
UINT8 R855_PLL_Lock(void);
R855_ErrCode R855_AGC_Slow(void);
//--------------------for GUI test--------------------------//
R855_ErrCode R855_GetRfGain(R855_RF_Gain_Info *pR855_rf_gain);
R855_ErrCode R855_RfGainMode(R855_RF_Gain_Type R855_RfGainType);
R855_ErrCode R855_SetIfFreq(UINT16 IF_KHz, UINT32 RF_KHz, R855_Standard_Type u1SystemStd);
R855_ErrCode R855_SetLpfBw(UINT8 LPF_BW);
R855_ErrCode R855_SetLpfOffset(UINT8 LPF_Offset);
R855_ErrCode R855_SetHpfOffset(UINT8 HPF_Offset);
R855_ErrCode R855_SetIfLpf(UINT32 LPF_Cor_Freq);
R855_ErrCode R855_SetLnaTop(UINT8 LNA_TOP);
R855_ErrCode R855_CheckMaxGain(void);
//--------------------Test function-----------------------//
R855_ErrCode R855_Dump_Data(void);
R855_ErrCode R855_RfGainCtrl(R855_RF_Gain_Type R855_RfGainType, UINT8 LnaGain, UINT8 RfGain, UINT8 MixerGain, UINT8 FilterGain);
R855_ErrCode R855_IfGainCtrl(R855_IF_Gain_Type R855_IfGainType, UINT8 VgaGain);
R855_ErrCode R855_GetRfRssi(UINT32 RF_Freq_Khz, R855_Standard_Type RT_Standard, INT32 *RfGain);
UINT8 R855_Read_ChipID(void) ; //CD OK;
R855_ErrCode Init_R855(Endeavour  *pEndeavour, R855_Set_Info *pR855_Info);

R855_ErrCode Tune_R855(R855_Set_Info *pR855_Info);
typedef enum _R855_Vga_Mode_TYPE
{
	VGA_AUTO = 0,
	VGA_MANUAL
}R855_Vga_Mode_TYPE;


extern R855_ErrCode R855_VgaCtrl(R855_Vga_Mode_TYPE R855_VgaModeType, UINT8 u1ManualCode);

#endif