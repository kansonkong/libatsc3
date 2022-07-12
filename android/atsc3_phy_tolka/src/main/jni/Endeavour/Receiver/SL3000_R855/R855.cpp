//-----------------------------------------------------//
//                                                     //
// Filename: R855.c                                    //
//                                                     //
// This file is R855 tuner driver                      //
// Copyright 2015 by Rafaelmicro., Inc.                //
// Author: Jason Wang                                  //
//-----------------------------------------------------//

//#include "stdafx.h"
#include "R855.h"
#include "I2C_Sys.h"    // "I2C_Sys" is only for SW porting reference.




#define  R855_FILTER_GAIN_DELAY   2    //3 or 2
#define  R855_FILTER_CODE_DELAY   2    //2
#define  R855_XTAL_CHK_DELAY      10
#define  R855_PLL_LOCK_DELAY      10
UINT32 R855_ADC_READ_DELAY = 2;        //3 or 2
UINT8  R855_ADC_READ_COUNT = 1;
UINT32 R855_IMR_IF = 5300;
UINT8 R855_IMR_CAL_MANUAL = 0;
R855_IMR_CAL_TYPE	R855_IMR_TYPE = R855_IMR_AUTO_HW; //[R855_IMR_AUTO_HW or R855_IMR_MANUAL_SW]
UINT8 R855_HW_CAL_DELAY = 55;

//----------------- User Parameter (set by user) ---------------//
//*** share Xtal options ***//
//1. R855_NO_SHARE_XTAL
//2. R855_MASTER_TO_SLAVE_XTAL_IN              //R855+RT710
//3. R855_MASTER_TO_SLAVE_XTAL_OUT             //R855 to RT500/RT534(or others)
//4. R855_SLAVE_XTAL_OUT                       //pure ATV
UINT8   R855_SHARE_XTAL = R855_NO_SHARE_XTAL;

//*** Internal Xtal cap ***//
//Xtal CL*2 = (R855_XTAL_CAP+External Cap)
//for share Xtal, set to 0. use external cap to tune freq. deviation.
UINT8   R855_XTAL_CAP_VALUE = 38;    //range 0~41
UINT8   R855_XTAL_CAP = R855_XTAL_CAP_VALUE;

//*** Mid Band TF select ***//
//1. L19=27nH: R855_MID_USING_27NH
//2. L19=39nH: R855_MID_USING_39NH
//3. L19=68nH: R855_MID_USING_68NH
UINT8 R855_DetectMidTfType_def = R855_MID_USING_39NH;   

//MT4: 39nH(Others) / 68nH(DTMB)
//MT2: 27nH
//------------------------------------------------------------------------//
UINT16 R855_Xtal = 24000;	   //only support 24MHz X'tal

UINT8 R855_iniArray[3][R855_REG_NUM] = {                
/*
	{     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //24M
	  //   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
           0xC0, 0x90, 0x20, 0xBC, 0x07, 0x6A, 0x00, 0x88, 
	//     0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F                                                   	   													  						  
		   0x00, 0xF2, 0x19, 0xD5, 0x60, 0x51, 0x8C, 0x2A, 
	//     0x10  0x11  0x12  0x13  0x14  0x15  0x16  0x17                                                     	   
           0x0E, 0xA5, 0x8A, 0xAA, 0xEA, 0x84, 0x28, 0x00, 
	//     0x18  0x19  0x1A  0x1B  0x1C  0x1D  0x1E  0x1F  
		   0x05, 0x0F, 0x80, 0x03, 0x06, 0x99, 0xBB, 0xC6,   
	//     0x20  0x21  0x22  0x23  0x24  0x25  0x26  0x27 
		   0xBB, 0x3A, 0x00, 0xF0, 0x74, 0x04, 0xD0, 0x3B },    //R46[1] pulse flag = (0:normal, 1:force flag=1)
	//     0x28  0x29  0x2A  0x2B  0x2C  0x2D  0x2E  0x2F 
*/
	{     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //24M
	  //   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
           0xC0, 0x10, 0x20, 0xBC, 0x07, 0x68, 0x02, 0x8C, 
	//     0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F                                                   	   													  						  
		   0x00, 0x45, 0x19, 0x75, 0x62, 0x18, 0x89, 0x6A, 
	//     0x10  0x11  0x12  0x13  0x14  0x15  0x16  0x17                                                     	   
           0x84, 0xB3, 0x8A, 0xAA, 0xEA, 0x84, 0x33, 0x00, 
	//     0x18  0x19  0x1A  0x1B  0x1C  0x1D  0x1E  0x1F  
		   0x04, 0x1F, 0x90, 0x03, 0x04, 0xBA, 0xEE, 0xB4,   
	//     0x20  0x21  0x22  0x23  0x24  0x25  0x26  0x27 
		   0x99, 0x4E, 0x92, 0xF0, 0x74, 0x04, 0x58, 0xA9 },    //R46[1] pulse flag = (0:normal, 1:force flag=1)
	//     0x28  0x29  0x2A  0x2B  0x2C  0x2D  0x2E  0x2F 

	{     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //16M
	  //   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
           0x84, 0xFE, 0x1C, 0x90, 0x60, 0x26, 0x00, 0xAC, 
	//     0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F                                                   	   													  						  
		   0x00, 0x7F, 0x22, 0x00, 0x70, 0x80, 0x91, 0x79, 
	//     0x10  0x11  0x12  0x13  0x14  0x15  0x16  0x17                                                     	   
           0x3A, 0x07, 0x95, 0x00, 0x00, 0x1F, 0x2C, 0x1C, 
	//     0x18  0x19  0x1A  0x1B  0x1C  0x1D  0x1E  0x1F 
		   0x01, 0x1E, 0x00, 0xCD, 0xDC, 0x3B, 0x59, 0x59,
	//     0x20  0x21  0x22  0x23  0x24  0x25  0x26  0x27 
		   0xB6, 0xAB, 0xA2, 0x55, 0x15, 0xF7, 0xED, 0x01 },
	//     0x28  0x29  0x2A  0x2B  0x2C  0x2D  0x2E  0x2F		

	{     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //27M
	  //   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
           0x84, 0xFE, 0x1C, 0x90, 0x60, 0x26, 0x00, 0xAC, 
	//     0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F                                                   	   													  						  
		   0x00, 0x7F, 0x22, 0x00, 0x70, 0x80, 0x91, 0x79, 
	//     0x10  0x11  0x12  0x13  0x14  0x15  0x16  0x17                                                     	   
           0x3A, 0x07, 0x8B, 0x5E, 0x42, 0x1C, 0x2C, 0x1C, 
	//     0x18  0x19  0x1A  0x1B  0x1C  0x1D  0x1E  0x1F 
		   0x01, 0x1E, 0x00, 0x8D, 0xDC, 0x3B, 0x59, 0x59,
	//     0x20  0x21  0x22  0x23  0x24  0x25  0x26  0x27 
		   0xB6, 0xAB, 0xA2, 0x55, 0x15, 0xF7, 0xED, 0x01 },
	//     0x28  0x29  0x2A  0x2B  0x2C  0x2D  0x2E  0x2F
};

UINT8 R855_XtalDiv = 0 ; //R855_XTAL_DIV1_11;	// by ITE
UINT8 R855_ADDRESS = 0xF6;	
UINT8 R855_Poly = R855_Poly_5;
UINT8 R855_XTAL_DIV_MANUAL = 0;
UINT8 R855_XtalDiv_Man= 0; //R855_XtalDiv;	// by ITE
UINT8 R855_MIXER_MODE_MANUAL = 0;          
UINT8 R855_Mixer_Mode_Man=0;               
UINT8 R855_SetTfType_UL_MID = R855_TF_BEAD_27N;   //default: DTV
UINT8 R855_DetectTfType_Cal = R855_UL_USING_BEAD;  //Bead, 270n, 390n
UINT8 R855_Fil_Cal_Gap = 16;  
UINT32 R855_IF_HIGH = 8500;  
UINT8 R855_Xtal_Pwr = R855_XTAL_HIGHEST;         
UINT8 R855_Xtal_Pwr_tmp = R855_XTAL_HIGHEST; 
UINT8 R855_Mixer_Mode = R855_IMR_NOR;
//RSSI ADC
UINT8 R855_IF_RSSI_ADC_BEFORE = 0;  
UINT8 R855_IF_RSSI_ADC_AFTER = 0;  

//TF Cal Array
UINT8 R855_TF_CAL_Array[3][R855_REG_NUM] = {   //24M/16M/27M
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only					
	   0x04, 0x10, 0x00, 0xA6, 0x8F, 0x6A, 0x23, 0xBB,                                                   	   													  						 
	   0x00, 0x38, 0xC0, 0xF6, 0x68, 0x98, 0x12, 0x6B,                                                 	   
	   0x12, 0x51, 0x89, 0x55, 0x55, 0x90, 0x28, 0x00, 				
	   0x01, 0x0F, 0x72, 0x03, 0x04, 0x99, 0xBB, 0x46,					
	   0xBB, 0x3A, 0x00, 0xF1, 0x74, 0x84, 0x20, 0x3B },

	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only					
	   0x1C, 0xF2, 0x31, 0x90, 0xB0, 0xC1, 0x20, 0xBF,                                                   	   													  						 
	   0x00, 0x47, 0x86, 0x30, 0x60, 0x00, 0x00, 0x69,                                                 	   
	   0x00, 0x30, 0x90, 0x00, 0x00, 0x5F, 0x9E, 0x42, 				
	   0x29, 0x0F, 0x5B, 0x23, 0xC6, 0x38, 0x38, 0x08,					
	   0x63, 0x00, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x01 },

	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only					
	   0x0C, 0xFA, 0x10, 0x90, 0xB0, 0xC1, 0x20, 0xBF,                                                   	   													  						 
	   0x00, 0x57, 0x86, 0x30, 0x60, 0x00, 0x7F, 0x69,                                                 	   
	   0x00, 0x34, 0x88, 0x2F, 0xA1, 0x58, 0xBE, 0x5C, 				
	   0x21, 0x3F, 0x50, 0x23, 0xE6, 0x38, 0x38, 0x08,					
	   0x63, 0x00, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x01 },
};  
//TF Mid Cal Array (no use)
UINT8 R855_TF_MID_CAL_Array[3][R855_REG_NUM] = {   //24M/16M/27M
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only					
	   0x0C, 0xF2, 0x70, 0x90, 0xB0, 0xC1, 0x1B, 0xBE,             
	   0x00, 0x57, 0x86, 0x30, 0x60, 0x00, 0x7F, 0x69,                                                 	   
	   0x00, 0x34, 0xA4, 0xAB, 0xAA, 0x50, 0xBA, 0x40, 				
	   0x61, 0x1E, 0x71, 0x59, 0xE6, 0x38, 0x38, 0x08,		//R32 div?
	   0x63, 0x70, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x00 },

	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only					
	   0x0C, 0xF2, 0x70, 0x90, 0xB0, 0xC1, 0x07, 0xBE,     
	   0x00, 0x57, 0x86, 0x30, 0x60, 0x00, 0x7F, 0x69,   
	   0x00, 0x34, 0xB9, 0x00, 0x00, 0x53, 0xBA, 0x40,		
	   0x61, 0x1E, 0x7A, 0x59, 0xE6, 0x38, 0x38, 0x08,					
	   0x63, 0x70, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x00 },

	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only					
	   0x0C, 0xF2, 0x70, 0x90, 0xB0, 0xC1, 0x1B, 0xBE,             
	   0x00, 0x57, 0x86, 0x30, 0x60, 0x00, 0x7F, 0x69,                                                 	   
	   0x00, 0x34, 0xA0, 0x7B, 0x09, 0x50, 0xBA, 0x40, 				
	   0x61, 0x1E, 0x6F, 0x19, 0xE6, 0x38, 0x38, 0x08,					
	   0x63, 0x70, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x00 }
};  
//LPF Cal Array
UINT8 R855_LPF_CAL_Array[3][R855_REG_NUM] = {   //24M/16M/27M

	 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only			
	   0x06, 0x10, 0x00, 0xA6, 0xCF, 0x6A, 0x00, 0x88,                                                   	   													  						 
	   0x00, 0x38, 0xCC, 0x00, 0x68, 0x98, 0x02, 0x7B,                                                 	   
	   0x00, 0x2F, 0x8A, 0x55, 0x55, 0x10, 0x28, 0x00, 				
	   0x01, 0x0F, 0x72, 0x03, 0x04, 0x99, 0xBB, 0x46,					
	   0xBB, 0x3A, 0x00, 0xF1, 0x74, 0x84, 0x20, 0x3B },

	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only					
	   0x1E, 0xF2, 0x31, 0x90, 0xB0, 0x01, 0x20, 0x8F,                                                   	   													  						 
	   0x00, 0x57, 0x86, 0x30, 0x60, 0x00, 0x00, 0x69,                                                 	   
	   0x00, 0xB4, 0x90, 0xCD, 0x4C, 0x5C, 0x8E, 0x42, 				
	   0x29, 0x1E, 0x5B, 0xE3, 0xC6, 0x38, 0x38, 0x08,					
	   0x63, 0x00, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x01 },

	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only					
	   0x1E, 0xF2, 0x31, 0x90, 0xB0, 0x01, 0x20, 0x8F,                                                   	   													  						 
	   0x00, 0x57, 0x86, 0x30, 0x60, 0x00, 0x00, 0x69,                                                 	   
	   0x00, 0xB4, 0x88, 0xCE, 0x95, 0x5C, 0xAE, 0x42, 				
	   0x21, 0x1E, 0x50, 0xA3, 0xC6, 0x38, 0x38, 0x08,					
	   0x63, 0x00, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x01 }
};  
//IMR Cal array
UINT8 R855_IMR_CAL_Array[3][R855_REG_NUM] = {   //24M/16M/27M
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only						   
	   0x06, 0x10, 0x00, 0xA6, 0x0F, 0x6A, 0x00, 0x88,    
	   0x00, 0x08, 0xD0, 0xF6, 0x60, 0x90, 0x12, 0x77,
	   0xFC, 0x5F, 0x93, 0x55, 0xD5, 0x08, 0x28, 0x00, 				
	   0x01, 0x0F, 0x11, 0x0F, 0x18, 0x99, 0xBB, 0x46,					
	   0xBB, 0x3A, 0x00, 0xF1, 0x74, 0x84, 0x20, 0x3B },  

	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only	
	   0x0E, 0xF2, 0x35, 0x80, 0xB0, 0x39, 0x28, 0x8F,  
	   0x00, 0x57, 0x86, 0x20, 0x60, 0x00, 0x77, 0x69,
	   0x41, 0xEC, 0x9B, 0xEB, 0x51, 0x5C, 0x8E, 0x42,
	   0x31, 0x3F, 0x59, 0xAB, 0xDE, 0x38, 0x38, 0xF8,					
	   0x63, 0xF8, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x01 },

	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //read only
	   0x0E, 0xF2, 0x35, 0x80, 0xB0, 0x39, 0x27, 0x8F,
	   0x00, 0x57, 0x86, 0x20, 0x60, 0x00, 0x77, 0x69,  
	   0x41, 0xEC, 0x91, 0x00, 0x00, 0x5F, 0xAE, 0x42, 	
	   0x21, 0x3F, 0x51, 0xEB, 0xDE, 0x38, 0x38, 0xF8,					
	   0x63, 0xF8, 0xC2, 0x45, 0x55, 0xFB, 0x6C, 0x01 }
};  
//----------------------------------------------------------//
//                   Internal Parameters                    //
//----------------------------------------------------------//


UINT8 R855_Array[R855_REG_NUM]={0};
R855_Sect_Type R855_IMR_Data_G7_MIXAMP[R855_IMR_POINT_NUM] = 
{
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}
};

R855_Sect_Type R855_IMR_Data_L7_MIXAMP[R855_IMR_POINT_NUM] = 
{
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}
};

UINT8 R855_IMR_MEM_GLOBAL=0;
UINT8 R855_IMR_Q0_Value[R855_IMR_POINT_NUM]={0}; //store (Q0,Q0) value to compare Q7

I2C_TYPE_INFO  R855_I2C;
I2C_LEN_TYPE R855_I2C_Len;

UINT8  R855_IMR_point_num;
UINT8  R855_Initial_done_flag = FALSE;
UINT8  R855_IMR_done_flag = FALSE;
UINT8  R855_Bandwidth = 0;   //8M
UINT8  R855_Lpf_Lsb = 0;   
UINT8  R855_Fil_Cal_flag[R855_STD_SIZE];
static UINT8 R855_Fil_Cal_BW[R855_STD_SIZE];
static UINT8 R855_Fil_Cal_code[R855_STD_SIZE];
static UINT8 R855_Fil_Cal_LpfLsb[R855_STD_SIZE];    
static R855_Standard_Type R855_pre_standard = R855_STD_SIZE;
static UINT8 R855_SBY[R855_REG_NUM];
static UINT8 R855_Standby_flag = FALSE;
//0(8M), 2(7M), 3(6M)

static UINT8 R855_Fil_Cal_BW_def[R855_STD_SIZE]={
	   3, 2, 2, 3, 2, 2, 2, 0,           //DVB-T, DVB-T2
	   0, 2, 2, 3, 2, 2, 3, 2, 2,           //DVB-C, J83B, ISDBT, DTMB, ATSC, ATSC3
	   2, 2, 0, 2, 2, 0, 3,              //DVB-T, DVB-T2 (IF 5M)
	   0, 2, 2, 2, 0, 2, 2, 2, 2            //DVB-C, J83B, ISDBT, DTMB, ATSC (IF 5M), ATSC3 (IF 5M), FM
       };
static UINT8 R855_Fil_Cal_code_def[R855_STD_SIZE]={
        3,  8,  2,  3,  8,  2,  8,  0,             //DVB-T, DVB-T2
	    8,  4,  7,  5, 16,  1,  5,  6, 6,             //DVB-C, J83B, ISDBT, DTMB, ATSC, ATSC3
	   13,  1, 13, 13,  1, 13, 23,                 //DVB-T, DVB-T2 (IF 5M)
	   11,  3, 11, 10, 11,  9,  8,  8, 17              //DVB-C, J83B, ISDBT, DTMB, ATSC (IF 5M), ATSC3 (IF 5M), FM
       };

static UINT8 R855_IMR_Cal_Result = 0; //1: fail, 0: ok
static UINT8 R855_TF_Check_Result = 0; //1: fail, 0: ok

//0: L270n/39n  (DTV, ATV/Cable) *
//1: Bead/68n  (China DTMB, DVB-C, ATV) *
//2: L270n/27n (all except DTMB) 
//3: Bead/27n  (China DTMB, DVB-C, ATV) 
//4: L390n/27n  (China DTMB) 
//5: L390n/39n  (for LGIT: China DTMB, DVB-C, ATV)
UINT8  R855_TF = 0;

UINT32 R855_LNA_MID_LOW[R855_TF_SIZE] = { 236000, 164000, 292000, 236000, 236000, 220000};
UINT32 R855_LNA_HIGH_MID[R855_TF_SIZE] = { 508000, 484000, 620000, 620000, 620000, 508000}; 

UINT32 R855_TF_Freq_High[R855_TF_SIZE][R855_TF_HIGH_NUM] = 
{  
	 { 720000, 696000, 664000, 624000, 608000, 560000, 544000, 512000},	 
     { 720000, 664000, 624000, 608000, 560000, 528000, 512000, 488000},
	 { 720000, 704000, 688000, 672000, 656000, 528000, 640000, 624000},
	 { 720000, 704000, 688000, 672000, 656000, 528000, 640000, 624000},
	 { 720000, 704000, 688000, 672000, 656000, 528000, 640000, 624000},
	 { 720000, 696000, 664000, 624000, 608000, 560000, 544000, 512000}
};
UINT32 R855_TF_Freq_Mid[R855_TF_SIZE][R855_TF_MID_NUM] = 
{	 
	  {400000, 384000, 368000, 352000, 336000, 320000, 304000, 288000, 272000, 256000, 240000}, //270n/39n
	  {320000, 304000, 272000, 264000, 240000, 233000, 216000, 200000, 192000, 184000, 168000}, //Bead/68n 
	  {496000, 464000, 448000, 416000, 400000, 384000, 368000, 352000, 336000, 320000, 296000}, //270n/27n  
	  {496000, 464000, 448000, 416000, 400000, 368000, 336000, 304000, 288000, 272000, 240000}, //Bead/27n
	  {496000, 464000, 448000, 416000, 400000, 368000, 336000, 304000, 288000, 272000, 240000}, //390n/27n
	  {400000, 384000, 368000, 352000, 320000, 304000, 288000, 272000, 256000, 240000, 224000}  //390n/39n
};
UINT32 R855_TF_Freq_Low[R855_TF_SIZE][R855_TF_LOW_NUM] = 
{    
	  {195000, 176000, 144000, 128000, 112000, 96000, 80000, 64000, 48000},  //270n/39n
      {168000, 160000, 144000, 128000, 112000, 96000, 64000, 56000, 48000},  //Bead/68n 
	  {200000, 176000, 144000, 128000, 112000, 96000, 80000, 64000, 48000},  //270n/27n 
	  {168000, 160000, 144000, 128000, 112000, 96000, 64000, 56000, 48000},  //Bead/27n 
	  {144000, 136000, 128000, 104000, 96000, 80000, 64000, 56000, 48000},   //390n/27n
	  {144000, 136000, 128000, 104000, 96000, 80000, 64000, 56000, 48000}    //390n/39n
};


UINT8 R855_TF_Result_High[R855_TF_SIZE][R855_TF_HIGH_NUM] = 
{    
	  //{0x00, 0x01, 0x02, 0x04, 0x04, 0x07, 0x09, 0x0B},
	  {0x00, 0x01, 0x02, 0x04, 0x09, 0x0C, 0x0E, 0x0F},
	  {0x00, 0x02, 0x04, 0x04, 0x07, 0x0A, 0x0D, 0x0E},
	  {0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04},
	  {0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04},
	  {0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04},
	  {0x00, 0x01, 0x02, 0x04, 0x04, 0x07, 0x09, 0x0B}
};
UINT8 R855_TF_Result_Mid[R855_TF_SIZE][R855_TF_MID_NUM] = 
{    
	  {0x00, 0x01, 0x03, 0x03, 0x04, 0x05, 0x07, 0x08, 0x0B, 0x0D, 0x0F},
      {0x00, 0x02, 0x04, 0x04, 0x07, 0x0A, 0x0D, 0x0E, 0x12, 0x12, 0x16},
	  {0x00, 0x02, 0x02, 0x04, 0x04, 0x06, 0x07, 0x09, 0x0B, 0x0C, 0x0F},
      {0x00, 0x02, 0x02, 0x04, 0x04, 0x07, 0x0B, 0x0E, 0x11, 0x13, 0x18},
	  {0x00, 0x02, 0x02, 0x04, 0x04, 0x07, 0x0B, 0x0E, 0x11, 0x13, 0x18},
	  {0x00, 0x01, 0x03, 0x03, 0x05, 0x07, 0x08, 0x0B, 0x0D, 0x0F, 0x13}
};
UINT8 R855_TF_Result_Low[R855_TF_SIZE][R855_TF_LOW_NUM] = 
{  
	  {0x00, 0x01, 0x03, 0x07, 0x0C, 0x11, 0x1B, 0x2F, 0x6D},  //270n/39n
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x08},  //Bead/68n
	  {0x00, 0x01, 0x03, 0x07, 0x0C, 0x11, 0x1B, 0x2F, 0x6D},  //270n/27n   
	  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x08},  //Bead/27n
	  {0x00, 0x02, 0x03, 0x03, 0x05, 0x0C, 0x19, 0x26, 0x37},  //390n/27n
	  {0x00, 0x02, 0x03, 0x03, 0x05, 0x0C, 0x19, 0x26, 0x37}   //390n/39n
};

//270n/39n, low/mid Q-on, high Q-off
/*static UINT16 R855_Lna_Plain_Acc_Gain[7][30] = 
{
 {0,16,30,30,44,58,73,90,106,123,139,157,175,193,214,230,248,261,276,287,296,302,312,330,348,366,382,366,384,399},   //<190
 {0,14,28,28,41,55,71,87,104,120,137,154,172,190,210,225,243,256,271,283,292,299,308,325,341,356,368,358,374,386},   //190~215
 {0,12,25,25,37,51,65,80,96,110,124,139,154,168,182,195,209,225,243,258,269,279,293,305,315,324,329,342,351,356},    //215~236
 {0,13,27,27,41,57,73,90,106,123,138,154,170,185,200,211,223,239,257,273,285,294,309,320,329,338,345,356,364,370},   //236~508
 {0, 0,17,17,52,84,112,138,161,183,202,221,240,256,274,284,295,311,328,344,356,366,381,391,401,410,418,431,440,448}, //508~590
 {0, 0,17,17,37,58,79,100,121,140,157,175,193,208,225,236,247,263,281,296,308,318,333,344,355,365,374,380,391,400},  //590~674
 {0, 0,17,17,31,47,64,81,99,117,133,152,170,188,209,221,234,247,262,274,283,291,301,313,326,339,351,339,351,363}     //>674
};*/


static UINT16 R850_Lna_Acc_Gain[5][32] = //dB*100
{
	{0, 119, 240, 381, 522, 679, 849, 945, 1049, 1163, 1295, 1431, 1567, 1696, 1834, 1978, 2123, 2266, 2401, 2518, 2648, 2754, 2867, 2975, 3068, 3172, 3360, 3548, 3548, 3550, 3736, 3735},//50~205MHz
	{0, 106, 209, 323, 428, 535, 636, 750,  870,  999, 1144, 1289, 1431, 1564, 1702, 1842, 1980, 2112, 2233, 2336, 2479, 2600, 2731, 2859, 2970, 3097, 3183, 3252, 3253, 3365, 3448, 3507},//205~425MHz
	{0, 112, 263, 429, 588, 759, 935,1049, 1168, 1297, 1441, 1585, 1724, 1852, 1986, 2119, 2249, 2372, 2483, 2576, 2731, 2863, 3009, 3154, 3284, 3435, 3596, 3756, 3756, 3749, 3913, 4074},//425~535MHz
	{0,   0, 113, 244, 366, 491, 614, 722,  838,  965, 1108, 1251, 1391, 1519, 1652, 1784, 1912, 2032, 2141, 2231, 2387, 2519, 2667, 2815, 2948, 3105, 3196, 3279, 3279, 3389, 3478, 3558},//535~655MHz
	{0,   0, 116, 247, 371, 499, 631, 717,  810,  915, 1034, 1157, 1281, 1398, 1523, 1651, 1779, 1902, 2017, 2115, 2256, 2375, 2506, 2634, 2747, 2878, 2993, 3099, 3099, 3093, 3206, 3311},//>655MHz
};


static INT8 Lna_Acc_Gain_offset_more_than_10[86]= //dB*10
												{-15,	-10, -5, -3, -6, -6, -2, 7,	14,	16,	//45~145
												14,	9,	5,	-6,	-4,	-9,	1,	-2,	-7,	-14,	//145~245
												5,	3,	-4,	-7,	-1,	-6,	1,	5,	6,	13,		//245~345
												-7,	-4,	2,	-10,	0,	1,	10,	17,	-11, -6,//345~445
												-1,	3,	6,	9,	10,	10,	-2,	-6,	-12, 17,	//445~545
												10,	5,	3,	3,	5,	5,	4,	-4,	-10, -17,	//545~645
												-20,	-5,	-1,	-1,	3,	3,	-1,	-2,	-5,	-6,	//645~745
												-5,	-2,	1,	5,	6,	6,	5,	1,	-2,	-3,		//745~845
												-2,	1,	3,	3,	1,	-3};					//845~905


static INT8 Lna_Acc_Gain_offset_less_than_10[86]= //dB*10
												{-13,	-10, -5, -2, -1, 0,	1, 3, 5, 5,		//45~145
												6,	5,	5,	2,	2,	-1,	7,	3,	-1,	-11,	//145~245
												-4,	-3,	-4,	-4,	-1,	-5,	-3,	-2,	-1,	5,		//245~345
												-4,	0,	5,	-1,	3,	3,	7,	11,	-3,	1,		//345~445
												6,	9,	12,	14,	15,	15,	-23, -22, -22, 3,	//445~545
												2,	2,	2,	1,	2,	1,	1,	-2,	-3,	-4,		//545~645
												-4,	-4,	-3,	-5,	-3,	-3,	-4,	-3,	-3,	-2,		//645~745
												-1,	0,	0,	1,	2,	2,	3,	3,	3,	3,		//745~845
												3,	3,	3,	2,	2,	1};						//845~905


static INT16 rfrssi_offset_by_freq[86]={177, 142, 28, 180, 257, 177, 55, 85, 196, 285,//45~145
                                           315, 291, 263, 75, 386, 293, 613, 664, 757, 721,//145~245
                                           745, 731, 727, 586, 498, 403, 356, 306, 279, -93,//245~345
                                           -57, -68, -110, -184, -216, -197, -151, -85, 17, 68,//345~445
                                           113, 124, 107, 47, -114, -149, -175, -129, -75, 19,//445~545
                                           79, 44, 4, -32, 47, -34, -61, -41, -4, 83,//545~645
                                           123, 156, 252, 203, 126, 7, -117, -130, -115, -55,//645~745
                                           25, 77, 56, -12, 164, 42, -17, -49, -41, 0,//745~845
										   53, 109, 122, 114, 71, -11};//845~905


/*
static UINT16 R855_Lna_Sharp_Acc_Gain[7][30] = 
{
 {130,130,130,130,143,157,172,188,205,222,238,256,274,293,314,314,314,327,342,355,364,371,381,381,381,381,381,365,383,400},
 {117,117,117,117,130,144,160,176,192,209,225,243,261,279,300,300,300,312,328,340,350,357,366,366,366,366,366,356,372,384},
 { 86, 86, 86, 86, 99,113,127,142,157,172,186,201,216,230,244,244,244,260,279,294,306,315,329,329,329,329,329,342,351,356},
 {104,104,104,104,115,129,143,157,172,186,200,215,229,243,257,257,257,273,292,308,320,329,344,344,344,344,344,355,363,369},
 {114,114,114,114,136,156,179,201,222,241,259,278,295,312,329,329,329,345,363,379,391,402,418,418,418,418,418,430,439,448},
 {117,117,117,117,130,145,160,177,193,209,224,240,257,272,287,287,288,304,322,337,350,360,375,375,375,375,375,381,391,400},
 {119,119,119,119,129,141,154,168,183,198,212,229,246,262,281,281,282,295,311,323,332,340,350,350,350,350,350,338,350,362}
};
*/

/*  //Bead/68nH, Q-off
static UINT16 R855_Lna_Plain_Acc_Gain[7][30] = 
{
 {0,13,26,25,41,58,76,94,111,129,146,166,185,205,227,241,257,270,283,294,302,308,317,332,347,363,378,364,380,396},   //<190
 {0,13,27,27,42,58,75,92,110,127,144,162,181,198,218,233,250,264,279,291,300,307,317,333,351,369,387,369,387,405},   //190~215
 {0,11,23,23,38,53,69,85,101,116,130,146,161,175,191,205,221,236,254,269,280,290,304,319,334,348,362,361,376,390},   //215~380
 {0,12,22,22,32,45,58,73, 87,102,115,129,143,156,170,176,182,198,217,233,245,255,271,275,277,279,280,305,306,306},   //380~484
 {0, 0,16,22,51,83,115,140,162,185,204,224,242,259,277,286,297,313,332,347,359,369,384,395,405,414,422,431,441,449}, //484~590
 {0, 0,15,20,32,52,72,92,112,131,149,166,183,199,215,225,237,253,272,287,299,309,324,335,345,356,365,370,380,389},   //590~674
 {0, 0,15,16,30,45,59,77,93,110,126,145,163,181,201,213,226,240,255,267,277,284,295,307,320,333,345,333,345,357}     //>674
};

static UINT16 R855_Lna_Sharp_Acc_Gain[7][30] = 
{
 {116,116,116,116,131,147,165,183,201,219,236,255,275,295,317,317,318,330,344,355,363,369,378,378,378,378,378,364,380,396},
 {127,127,127,127,142,158,175,192,210,228,244,263,281,299,319,319,320,333,349,361,370,377,387,387,387,387,387,369,387,405},
 {110,110,110,110,123,138,154,170,186,202,216,232,247,261,276,276,277,293,312,327,339,348,362,362,362,362,362,361,376,389},
 { 60, 60, 60, 60, 69, 81, 92,104,117,129,141,154,166,178,191,191,191,208,227,242,254,264,280,280,280,280,280,304,306,306},
 {118,118,118,118,137,158,181,204,225,244,262,280,298,315,332,332,333,349,367,383,396,406,422,422,422,422,422,431,440,449},
 {111,111,111,111,125,138,154,169,185,200,215,230,246,260,276,276,277,293,311,326,339,349,365,365,365,365,365,370,380,389},
 {116,116,116,116,126,137,150,164,179,193,208,224,241,257,275,275,277,290,306,318,328,335,345,345,345,345,345,332,345,357} 
};
*/
static UINT8 R855_Lna_Pulse_Comp[7][30] = 
{
 {126,126,126,126,112,95,77,58,69,69,69,69,69,74,56,56,56,56,74,74,47,66,54,54,54,54,54,30,30,30},
 {136,136,136,136,120,103,85,67,77,77,77,77,77,79,56,56,56,56,78,78,78,72,64,64,64,64,64,47,47,47},
 {123,123,123,123,108,92,75,57,68,68,68,68,68,73,55,55,55,55,72,72,72,64,52,52,52,52,52,30,30,30},
 {103,103,103,103,92,79,65,51,60,60,60,60,60,66,51,51,51,51,64,64,64,55,41,41,41,41,41,18,18,18},
 {117,117,117,117,106,93,79,65,73,73,73,73,73,76,57,57,57,57,76,76,76,69,59,59,59,59,59,44,44,44},
 {118,118,118,118,105,91,76,59,66,66,66,66,66,72,54,54,54,54,70,70,70,61,48,48,48,48,48,28,28,28},
 {116,116,116,116,106,94,81,67,73,73,73,73,73,76,59,59,59,59,76,76,76,69,58,58,58,58,58,44,44,44}
};

static UINT8   R855_RfFlag, R855_PulseFlag;
static UINT8   R855_TF_Mode1, R855_TF_Mode2;
static UINT16  R855_Rf_Acc_Gain[16]={0,138,275,413,529,529,529,665,800,938,1077,1216,1326,1424,1561,1694}; //dB*100
static UINT16  R855_Mixer_Acc_Gain[16]={0,0,0,0,114,241,375,504,627,759,886,1008,1113,1243,1377,1511}; //dB*100
static UINT16  R855_Filter_Acc_Gain[16]={0,160,311,462,611,763,911,1061,1210,1360,1506,1644,1790,1936,2087,2231}; //dB*100
//----------------------------------------------------------//
//                   Internal static struct                 //
//----------------------------------------------------------//
static R855_SysFreq_Info_Type  R855_SysFreq_Info1;
static R855_Sys_Info_Type      R855_Sys_Info1;
static R855_Freq_Info_Type     R855_Freq_Info1;
//----------------------------------------------------------//
//                   Internal Functions                     //
//----------------------------------------------------------//
R855_ErrCode R855_TF_Check(void);
R855_ErrCode R855_Xtal_Check(void);
R855_ErrCode R855_InitReg(void);
R855_ErrCode R855_Cal_Prepare(UINT8 u1CalFlag);
R855_ErrCode R855_IMR(UINT8 IMR_MEM, UINT8 IM_Flag, UINT8 Rev_Mode, UINT8 Mix_Gain_Mode);
R855_ErrCode R855_PLL(UINT32 LO_Freq, UINT16 IF_Freq, R855_Standard_Type R855_Standard);
R855_ErrCode R855_MUX(UINT32 LO_KHz, UINT32 RF_KHz, R855_Standard_Type R855_Standard);
R855_ErrCode R855_IQ_G7_MIXAMP(R855_Sect_Type* IQ_Pont);
R855_ErrCode R855_IQ_L7_MIXAMP(R855_Sect_Type* IQ_Pont);
R855_ErrCode R855_IQ_Tree_G7_MIXAMP(UINT8 FixPot, UINT8 FlucPot, UINT8 PotReg, R855_Sect_Type* CompareTree);
R855_ErrCode R855_IQ_Tree_L7_MIXAMP(UINT8 FixPot, UINT8 FlucPot, UINT8 PotReg, R855_Sect_Type* CompareTree);
R855_ErrCode R855_IQ_Tree5_G7_MIXAMP(UINT8 FixPot, UINT8 FlucPot, UINT8 PotReg, R855_Sect_Type* CompareTree);
R855_ErrCode R855_IQ_Tree5_L7_MIXAMP(UINT8 FixPot, UINT8 FlucPot, UINT8 PotReg, R855_Sect_Type* CompareTree);
R855_ErrCode R855_CompreCor(R855_Sect_Type* CorArry);
R855_ErrCode R855_CompreStep_G7_MIXAMP(R855_Sect_Type* StepArry, UINT8 Pace);
R855_ErrCode R855_CompreStep_L7_MIXAMP(R855_Sect_Type* StepArry, UINT8 Pace);
R855_ErrCode R855_Muti_Read(UINT8* IMR_Result_Data);
R855_ErrCode R855_Section_G7_MIXAMP(R855_Sect_Type* SectionArry);
R855_ErrCode R855_Section_L7_MIXAMP(R855_Sect_Type* SectionArry);
R855_ErrCode R855_F_IMR_G7_MIXAMP(R855_Sect_Type* IQ_Pont);
R855_ErrCode R855_F_IMR_L7_MIXAMP(R855_Sect_Type* IQ_Pont);
R855_ErrCode R855_IMR_Cross_G7_MIXAMP(R855_Sect_Type* IQ_Pont, UINT8* X_Direct);
R855_ErrCode R855_IMR_Cross_L7_MIXAMP(R855_Sect_Type* IQ_Pont, UINT8* X_Direct);
R855_ErrCode R855_IMR_Iqcap(R855_Sect_Type* IQ_Point);   
R855_ErrCode R855_SetTF(UINT32 u4FreqKHz, UINT8 u1TfType);
R855_ErrCode R855_SetStandard(R855_Standard_Type RT_Standard);
R855_ErrCode R855_SetFrequency(R855_Set_Info R855_INFO);

R855_Sys_Info_Type R855_Sys_Sel(R855_Standard_Type R855_Standard);
R855_Freq_Info_Type R855_Freq_Sel(UINT32 LO_freq, UINT32 RF_freq, R855_Standard_Type R855_Standard);
R855_SysFreq_Info_Type R855_SysFreq_Sel(R855_Standard_Type R855_Standard,UINT32 RF_freq);

UINT8 R855_Filt_Cal_ADC(UINT32 IF_Freq, UINT8 R855_BW, UINT8 FilCal_Gap);

R855_ErrCode R855_Printf_Sys_Parameter(R855_Sys_Info_Type R855_Sys_Info);
R855_ErrCode R855_Printf_SysFreq_Parameter(R855_SysFreq_Info_Type R855_SysFreq_Info);

R855_Sys_Info_Type R855_Sys_Sel(R855_Standard_Type R855_Standard)
{
	R855_Sys_Info_Type R855_Sys_Info;

	switch (R855_Standard)
	{
	case R855_DVB_T_6M: 
	case R855_DVB_T2_6M: 
		R855_Sys_Info.IF_KHz=4570;           //IF
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7300;      //CAL IF
		R855_Sys_Info.HPF_COR=7;	         //R24[3:0]=7
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_T_7M:  
	case R855_DVB_T2_7M:  
		R855_Sys_Info.IF_KHz=4570;           //IF
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7920;      //CAL IF
		R855_Sys_Info.HPF_COR=9;	         //R24[3:0]=9
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_T_8M: 
	case R855_DVB_T2_8M: 
		R855_Sys_Info.IF_KHz=4570;           //IF
		R855_Sys_Info.BW=0;                  //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8280;      //CAL IF
		R855_Sys_Info.HPF_COR=11;            //R24[3:0]=12 
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_T2_1_7M: 
		R855_Sys_Info.IF_KHz=1900;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7920;      //CAL IF
		R855_Sys_Info.HPF_COR=11;	         //R24[3:0]=11
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_T2_10M: 
		R855_Sys_Info.IF_KHz=5600;
		R855_Sys_Info.BW=0;                  //BW=8M
		R855_Sys_Info.FILT_CAL_IF=10400;     //CAL IF
		R855_Sys_Info.HPF_COR=12;            //R24[3:0]=12
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_C_8M:   
		R855_Sys_Info.IF_KHz=5070;
		R855_Sys_Info.BW=0;                  //BW=8M
		R855_Sys_Info.FILT_CAL_IF=9100;      //CAL IF
		R855_Sys_Info.HPF_COR=12;            //R24[3:0]=11
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=5;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_C_6M:  
		R855_Sys_Info.IF_KHz=5070;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=8100;      //CAL IF   
		R855_Sys_Info.HPF_COR=5;             //R24[3:0]=5
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=5;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_J83B:      //always normal for J83B
		R855_Sys_Info.IF_KHz=5070;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=8000;      //CAL IF  
		R855_Sys_Info.HPF_COR=5;             //R24[3:0]=5
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_ISDB_T_4063: 
		R855_Sys_Info.IF_KHz=4063;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7120;      //CAL IF  //7200
		R855_Sys_Info.HPF_COR=9;             //R24[3:0]=9
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_ISDB_T_4570:
		R855_Sys_Info.IF_KHz=4570;           //IF
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7350;      //CAL IF
		R855_Sys_Info.HPF_COR=8;             //R24[3:0]=8
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=0;           //[0:notch high, 1:notch low] 
		break;

	case R855_DTMB_8M_4570: 
		R855_Sys_Info.IF_KHz=4570;
		R855_Sys_Info.BW=0;                  //BW=8M		 
		R855_Sys_Info.FILT_CAL_IF=8450;      //CAL IF
		R855_Sys_Info.HPF_COR=12;            //R24[3:0]=12
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DTMB_6M_4500:
		R855_Sys_Info.IF_KHz=4500;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7120;      //CAL IF  
		R855_Sys_Info.HPF_COR=7;             //R24[3:0]=7
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_ATSC:  
		R855_Sys_Info.IF_KHz=5070;
		//R855_Sys_Info.BW=1;                  //BW=7M  
		R855_Sys_Info.BW=2;                  //BW=6M  
		R855_Sys_Info.FILT_CAL_IF=7850;      //CAL IF   
		if(R855_Mixer_Mode == R855_IMR_NOR)
		{
			R855_Sys_Info.HPF_COR=6;             //R24[3:0]=6
		}
		else
		{
			R855_Sys_Info.HPF_COR=7;             //R24[3:0]=7
		}
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;		 //[0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=5;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=0;           //[0:notch high, 1:notch low] 
		break;
	case R855_ATSC3:  
		R855_Sys_Info.IF_KHz=5070;
		R855_Sys_Info.BW=1;                  //BW=7M  
		R855_Sys_Info.FILT_CAL_IF=7850;      //CAL IF   
		if(R855_Mixer_Mode == R855_IMR_NOR)
		{
			R855_Sys_Info.HPF_COR=6;             //R24[3:0]=6
		}
		else
		{
			R855_Sys_Info.HPF_COR=7;             //R24[3:0]=7
		}
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;		 //[0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=5;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=0;           //[0:notch high, 1:notch low] 
		break;
    case R855_DVB_T_6M_IF_5M: 
	case R855_DVB_T2_6M_IF_5M: 
		R855_Sys_Info.IF_KHz=5000;           //IF
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7750;      //CAL IF
		R855_Sys_Info.HPF_COR=6;             //R24[3:0]=6
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_T_7M_IF_5M:  
	case R855_DVB_T2_7M_IF_5M:  
		R855_Sys_Info.IF_KHz=5000;           //IF
		R855_Sys_Info.BW=0;                  //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8450;      //CAL IF
		R855_Sys_Info.HPF_COR=8;             //R24[3:0]=8
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_T_8M_IF_5M: 
	case R855_DVB_T2_8M_IF_5M: 
		R855_Sys_Info.IF_KHz=5000;           //IF
		R855_Sys_Info.BW=0;                  //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8650;      //CAL IF
		R855_Sys_Info.HPF_COR=9;             //R24[3:0]=9
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_T2_1_7M_IF_5M: 
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=2;                  //BW=6M
		R855_Sys_Info.FILT_CAL_IF=6000;      //CAL IF
		R855_Sys_Info.HPF_COR=1;             //R24[3:0]=1
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_C_8M_IF_5M:  
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=0;                  //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8800;      //CAL IF 
		R855_Sys_Info.HPF_COR=12;            //R24[3:0]=12
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=6;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DVB_C_6M_IF_5M:  
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=8200;      //CAL IF   
		R855_Sys_Info.HPF_COR=6;             //R24[3:0]=6
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=6;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_J83B_IF_5M:  //always use normal for J83B
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7600;      //CAL IF  
		R855_Sys_Info.HPF_COR=5;             //R24[3:0]=5 
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_ISDB_T_IF_5M: 
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7650;      //CAL IF  
		R855_Sys_Info.HPF_COR=6;             //R24[3:0]=6
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=0;           //[0:notch high, 1:notch low] 
		break;

	case R855_DTMB_8M_IF_5M: 
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=0;                  //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8950;      //CAL IF  
		R855_Sys_Info.HPF_COR=14;            //		
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	case R855_DTMB_6M_IF_5M:
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7750;      //CAL IF  
		R855_Sys_Info.HPF_COR=6;             //R24[3:0]=6
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;
	
	case R855_ATSC_IF_5M:  
#if 0	 //3.25M
		R855_Sys_Info.IF_KHz=3250;
		R855_Sys_Info.BW=1;//1;                  //BW=7M  
		R855_Sys_Info.FILT_CAL_IF=5970;      //CAL IF   //7900
		if(R855_Mixer_Mode == R855_IMR_NOR)
		{
			R855_Sys_Info.HPF_COR=15;             //R24[3:0]=6
		}
		else
		{
			R855_Sys_Info.HPF_COR=15;             //R24[3:0]=7
		}
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=5;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=0;           //[0:notch high, 1:notch low] 
#else  //IF 5M
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=1;//1; 				 //BW=7M  
		R855_Sys_Info.FILT_CAL_IF = 8020;// 7720; 	 //CAL IF	//7900
		if(R855_Mixer_Mode == R855_IMR_NOR)
		{
			R855_Sys_Info.HPF_COR=6;			 //R24[3:0]=6
		}
		else
		{
			R855_Sys_Info.HPF_COR=7;			 //R24[3:0]=7
		}
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1; 	   //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=5; 		//[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=0;		   //[0:notch high, 1:notch low] 

#endif 
		break;
	case R855_ATSC3_IF_5M:  
 #if 0
		R855_Sys_Info.IF_KHz = 3250;
		R855_Sys_Info.BW=1;                  //BW=7M  
		R855_Sys_Info.FILT_CAL_IF = 5970;    //CAL IF   //7900
		if(R855_Mixer_Mode == R855_IMR_NOR)
		{
			R855_Sys_Info.HPF_COR = 15;             //R24[3:0]=6
		}
		else
		{
			R855_Sys_Info.HPF_COR = 15;           //R24[3:0]=7
		}
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=5;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=0;           //[0:notch high, 1:notch low] 		
 #else
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=1;                  //BW=7M  
		R855_Sys_Info.FILT_CAL_IF=7720;      //CAL IF   //7900
		if(R855_Mixer_Mode == R855_IMR_NOR)
		{
			R855_Sys_Info.HPF_COR=6;             //R24[3:0]=6
		}
		else
		{
			R855_Sys_Info.HPF_COR=7;             //R24[3:0]=7
		}
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=1;        //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=5;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=0;           //[0:notch high, 1:notch low] 
#endif
		break;
	case R855_FM:  
		R855_Sys_Info.IF_KHz=5000;
		R855_Sys_Info.BW=1;                  //BW=7M
		R855_Sys_Info.FILT_CAL_IF=7200;      //CAL IF
		R855_Sys_Info.HPF_COR=5;             //R24[3:0]=5
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;

	default:  //R855_DVB_T_8M
		R855_Sys_Info.IF_KHz=4570;           //IF
		R855_Sys_Info.BW=0;                  //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8280;      //CAL IF
		R855_Sys_Info.HPF_COR=12;            //R24[3:0]=12
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;		 //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		R855_Sys_Info.HPF_NOTCH_R24_7_1BT=1;           //[0:notch high, 1:notch low] 
		break;
	}

	//Set By DTV/ATV
	R855_Sys_Info.FILT3_COMP_R36_3_2BT=0;         //0         (R36[4:3]=0)
	R855_Sys_Info.FILT5_AUTO_COMP_R36_5_2BT=0;    //0         (R36[6:5]=0)
	R855_Sys_Info.FILT5_MAN_COMP_R20_4_1BT=0;     //0         (R20[4]=0)
	R855_Sys_Info.FILT5_FORCEQ_R43_0_1BT=1;       //manual    (R36[7]=1)

	//AGC/RF
	R855_Sys_Info.RF_CLASSB_CHARGE_R36_1_1BT=0;	//[0:largest, 1:smallest]
	R855_Sys_Info.RF_RES_CAP_R24_5_2BT=0;			//[0:short, 1:300K, 2:500K, 3:945K]
	R855_Sys_Info.BB_CLASSB_CHARGE_R17_1_1BT=0;	//[0:largest, 1:smallest]
	R855_Sys_Info.BB_RES_CAP_R23_6_1BT=1;			//[0:normal, 1:small ripple]	
	
	R855_Sys_Info.LNA_PEAK_AVG_R46_5_1BT=0;		//[0:peak, 1:avg]
	R855_Sys_Info.RF_PEAK_AVG_R39_7_1BT=1;			//[0:avg, 1:peak]
	R855_Sys_Info.BB_PEAK_AVG_R46_2_1BT=0;			//[0:peak, 1:avg]

	switch (R855_Standard)
	{
	case R855_ATSC:
	case R855_ATSC3:
	case R855_ATSC_IF_5M:
	case R855_ATSC3_IF_5M:
		//Discharge
		R855_Sys_Info.LNA_PEAK_AVG_R46_5_1BT=0;		//[0:peak, 1:avg]
		R855_Sys_Info.RF_PEAK_AVG_R39_7_1BT=1;			//[0:avg, 1:peak]
		R855_Sys_Info.BB_PEAK_AVG_R46_2_1BT=1;			//[0:peak, 1:avg]
		break;
	default:
		//Discharge
		R855_Sys_Info.LNA_PEAK_AVG_R46_5_1BT=0;		//[0:peak, 1:avg]
		R855_Sys_Info.RF_PEAK_AVG_R39_7_1BT=1;			//[0:avg, 1:peak]
		R855_Sys_Info.BB_PEAK_AVG_R46_2_1BT=0;			//[0:peak, 1:avg]
		break;
	}


	//NA
	switch(R855_Standard)
	{
		case R855_DTMB_8M_4570:
		case R855_DTMB_6M_4500:
		case R855_DTMB_8M_IF_5M:
		case R855_DTMB_6M_IF_5M:
		case R855_DVB_C_8M:
		case R855_DVB_C_6M:
        case R855_DVB_C_8M_IF_5M:
		case R855_DVB_C_6M_IF_5M:
		case R855_J83B:
		case R855_J83B_IF_5M:
			R855_Sys_Info.NA_PWR_DET_R9_7_1BT = 1;       //on       (R9[7]=0)
			break;

		default:
			R855_Sys_Info.NA_PWR_DET_R9_7_1BT = 0;       //off       (R9[7]=0)
			break;
	}

	//polyphase current (--> apply to IMR?)
	if((R855_Standard==R855_ATSC) || (R855_Standard==R855_ATSC_IF_5M) || (R855_Standard==R855_ATSC3) || (R855_Standard==R855_ATSC3_IF_5M))
		R855_Sys_Info.POLY_CUR_R12_3_1BT=0;    //high
	else
		R855_Sys_Info.POLY_CUR_R12_3_1BT=1;    //low	


	//Mixer PLL mode
if(R855_MIXER_MODE_MANUAL==0) 
{
	if((R855_Standard==R855_ATSC) || (R855_Standard==R855_ATSC_IF_5M) || (R855_Standard==R855_ATSC3) || (R855_Standard==R855_ATSC3_IF_5M)) //ATSC
	{
		R855_Mixer_Mode = R855_IMR_NOR;  //mixer up-side tune
	}
	else if((R855_Standard==R855_J83B) || (R855_Standard==R855_J83B_IF_5M))  //J83B
	{
		R855_Mixer_Mode = R855_IMR_NOR;  //mixer up-side tune
	}
	else  //other DTV
	{
		R855_Mixer_Mode = R855_IMR_NOR;   //mixer up-side tune
	}
}
else //manual mode
{
	R855_Mixer_Mode = R855_Mixer_Mode_Man;
}

	switch(R855_Standard)
	{
		//DVB-C
		case R855_DVB_C_8M:
		case R855_DVB_C_6M:
        case R855_DVB_C_8M_IF_5M:
		case R855_DVB_C_6M_IF_5M:

			if(R855_TF_Check_Result==1)  //fail
			{
				R855_DetectTfType_Cal = R855_UL_USING_270NH;
			}
	 
			 if(R855_DetectTfType_Cal==R855_UL_USING_BEAD)  //Low=Bead
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_BEAD_27N;		
				 else  //Mid=68
					 R855_SetTfType_UL_MID = R855_TF_BEAD_68N;		
			 }
			 else  //Low=270n
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_270N_27N;		
				 else  //Mid=39
					 R855_SetTfType_UL_MID = R855_TF_270N_39N;						
			 }
			 break;	

		//J83B
		case R855_J83B:
		case R855_J83B_IF_5M:

			if(R855_TF_Check_Result==1)  //fail
			{
				R855_DetectTfType_Cal = R855_UL_USING_270NH;
			}

			 if(R855_DetectTfType_Cal==R855_UL_USING_BEAD)  //Low=Bead
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_BEAD_27N;		
				 else  //Mid=68
					 R855_SetTfType_UL_MID = R855_TF_BEAD_68N;		
			 }
			 else  //Low=270n
			 {
				 R855_SetTfType_UL_MID = R855_TF_270N_39N;		
				 /*
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_270N_27N;		
				 else  //Mid=39
					 R855_SetTfType_UL_MID = R855_TF_270N_39N;		
				*/					
			 }
			break;

		//ATSC
		case R855_ATSC:  
		case R855_ATSC3:  
		case R855_ATSC_IF_5M:  
		case R855_ATSC3_IF_5M:  

			if(R855_TF_Check_Result==1)  //fail
			{
				R855_DetectTfType_Cal = R855_UL_USING_270NH;
			}

			 if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) //Low=Bead
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_BEAD_27N;		
				 else  //Mid=68
					 R855_SetTfType_UL_MID = R855_TF_BEAD_68N;			
			 }
			 else   //Low=270n
			 {
				 R855_SetTfType_UL_MID = R855_TF_270N_39N;		
				 /*
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_270N_27N;		
				 else  //Mid=39
					 R855_SetTfType_UL_MID = R855_TF_270N_39N;		
				*/	
			 }
			 break;

		//DTMB
		case R855_DTMB_8M_4570:
		case R855_DTMB_6M_4500:
		case R855_DTMB_8M_IF_5M:
		case R855_DTMB_6M_IF_5M:

			if(R855_TF_Check_Result==1)  //fail
			{
				R855_DetectTfType_Cal = R855_UL_USING_BEAD;
			}

			 if(R855_DetectTfType_Cal==R855_UL_USING_BEAD)
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_BEAD_27N;		
				 else  //Mid=68
					 R855_SetTfType_UL_MID = R855_TF_BEAD_68N;		
			 }
			 else    //Low=270n
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_270N_27N;		
				 else  //Mid=39
					 R855_SetTfType_UL_MID = R855_TF_270N_39N;					 
			 }
			 break;

		//ISDB-T
		case R855_ISDB_T_4063:		
		case R855_ISDB_T_4570:		
		case R855_ISDB_T_IF_5M:

			if(R855_TF_Check_Result==1)  //fail
			{
				R855_DetectTfType_Cal = R855_UL_USING_270NH;
			}
				 
			 if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) //Low=Bead
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_BEAD_27N;		
				 else  //Mid=68
					 R855_SetTfType_UL_MID = R855_TF_BEAD_68N;	      	
			 }
			 else    //Low=270n
			 {
				 R855_SetTfType_UL_MID = R855_TF_270N_39N;		
				 /*
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_270N_27N;		
				 else  //Mid=39
					 R855_SetTfType_UL_MID = R855_TF_270N_39N;		
				*/
			 }
			 break;

		default:		//DVB-T/T2

			if(R855_TF_Check_Result==1)  //fail
			{
				R855_DetectTfType_Cal = R855_UL_USING_270NH;
			}
			 
			 if(R855_DetectTfType_Cal==R855_UL_USING_BEAD)  //Low=Bead
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_BEAD_27N;		
				 else  //Mid=68
					 R855_SetTfType_UL_MID = R855_TF_BEAD_68N;		
			 }
			 else   //Low=270n
			 {
				 if(R855_DetectMidTfType_def==R855_MID_USING_27NH) //Mid=27
					 R855_SetTfType_UL_MID = R855_TF_270N_27N;		
				 else  //Mid=39
					 R855_SetTfType_UL_MID = R855_TF_270N_39N;			
			 }
			 break;
	}

#if(R855_DEBUG==1)
	R855_Printf_Sys_Parameter(R855_Sys_Info);
#endif
	return R855_Sys_Info;
}


R855_Freq_Info_Type R855_Freq_Sel(UINT32 LO_freq, UINT32 RF_freq, R855_Standard_Type R855_Standard) //CD OK
{
	R855_Freq_Info_Type R855_Freq_Info;

	//----- RF dependent parameter --------
	//LNA band 
	if(RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID])  
		 R855_Freq_Info.LNA_BAND = 3;   //R15[1:0]=2'b11; low
	else if((RF_freq>=R855_LNA_MID_LOW[R855_SetTfType_UL_MID]) && (RF_freq<R855_LNA_HIGH_MID[R855_SetTfType_UL_MID]))  //388~612
		 R855_Freq_Info.LNA_BAND = 2;   //R15[1:0]=2'b10; mid
	else     // >612
		 R855_Freq_Info.LNA_BAND = 1;   //R15[1:0]=00 or 01; high 

	//----- LO dependent parameter --------
	//IMR point 
	if((LO_freq>0) && (LO_freq<205000))  
	{
         R855_Freq_Info.IMR_MEM_NOR = 0;   
		 R855_Freq_Info.IMR_MEM_REV = 4;   
	}
	else if((LO_freq>=205000) && (LO_freq<400000))  
	{
         R855_Freq_Info.IMR_MEM_NOR = 1;   
		 R855_Freq_Info.IMR_MEM_REV = 5;   
	}
	else if((LO_freq>=400000) && (LO_freq<760000))  
	{
		 R855_Freq_Info.IMR_MEM_NOR = 2;  
		 R855_Freq_Info.IMR_MEM_REV = 6;   
	}
	else 
	{
		 R855_Freq_Info.IMR_MEM_NOR = 3;
		 R855_Freq_Info.IMR_MEM_REV = 7;   
	}

	//RF polyfilter band
	//if((LO_freq>0) && (LO_freq<133000))  
	if((LO_freq>0) && (LO_freq<130000))  
         R855_Freq_Info.RF_POLY = 2;   //R17[6:5]=2; low   =>R12[7;6]
	else if((LO_freq>=130000) && (LO_freq<205000))  
         R855_Freq_Info.RF_POLY = 1;   //R17[6:5]=1; mid  =>R12[7;6]
	else if((LO_freq>=205000) && (LO_freq<400000))  
		 R855_Freq_Info.RF_POLY = 0;   //R17[6:5]=0; highest      =>R12[7;6]
	else
		 R855_Freq_Info.RF_POLY = 3;   //R17[6:5]=3; ultra high  =>R12[7;6]

	
	//LPF Cap, Notch
	switch(R855_Standard)
	{
		case R855_DVB_C_8M:                  //Cable
		case R855_DVB_C_6M:
		case R855_J83B:
        case R855_DVB_C_8M_IF_5M:
		case R855_DVB_C_6M_IF_5M:
		case R855_J83B_IF_5M:
			if(LO_freq<77000)  
			{
				R855_Freq_Info.LPF_CAP = 15;
				R855_Freq_Info.LPF_NOTCH = 10;
			}
			else if((LO_freq>=77000) && (LO_freq<85000))
			{
				R855_Freq_Info.LPF_CAP = 15;
				R855_Freq_Info.LPF_NOTCH = 4;
			}
			else if((LO_freq>=85000) && (LO_freq<115000))
			{
				R855_Freq_Info.LPF_CAP = 13;
				R855_Freq_Info.LPF_NOTCH = 3;
			}
			else if((LO_freq>=115000) && (LO_freq<125000))
			{
				R855_Freq_Info.LPF_CAP = 11;
				R855_Freq_Info.LPF_NOTCH = 1;
			}
			else if((LO_freq>=125000) && (LO_freq<141000))
			{
				R855_Freq_Info.LPF_CAP = 9;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			else if((LO_freq>=141000) && (LO_freq<157000))
			{
				R855_Freq_Info.LPF_CAP = 8;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			else if((LO_freq>=157000) && (LO_freq<181000))
			{
				R855_Freq_Info.LPF_CAP = 6;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			else if((LO_freq>=181000) && (LO_freq<205000))
			{
				R855_Freq_Info.LPF_CAP = 3;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			else //LO>=201M
			{
				R855_Freq_Info.LPF_CAP = 0;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			break;


		default: 

			if((LO_freq>0) && (LO_freq<73000))  
			{
				R855_Freq_Info.LPF_CAP = 8;
				R855_Freq_Info.LPF_NOTCH = 10;
			}
			else if((LO_freq>=73000) && (LO_freq<81000))
			{
				R855_Freq_Info.LPF_CAP = 8;
				R855_Freq_Info.LPF_NOTCH = 4;
			}
			else if((LO_freq>=81000) && (LO_freq<89000))
			{
				R855_Freq_Info.LPF_CAP = 8;
				R855_Freq_Info.LPF_NOTCH = 3;
			}
			else if((LO_freq>=89000) && (LO_freq<121000))
			{
				R855_Freq_Info.LPF_CAP = 6;
				R855_Freq_Info.LPF_NOTCH = 1;
			}
			else if((LO_freq>=121000) && (LO_freq<145000))
			{
				R855_Freq_Info.LPF_CAP = 4;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			else if((LO_freq>=145000) && (LO_freq<153000))
			{
				R855_Freq_Info.LPF_CAP = 3;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			else if((LO_freq>=153000) && (LO_freq<177000))
			{
				R855_Freq_Info.LPF_CAP = 2;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			else if((LO_freq>=177000) && (LO_freq<201000))
			{
				R855_Freq_Info.LPF_CAP = 1;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			else //LO>=201M
			{
				R855_Freq_Info.LPF_CAP = 0;
				R855_Freq_Info.LPF_NOTCH = 0;
			}
			break;

	}//end switch(standard)


	//TF LPF setting
	switch(R855_Standard)
	{
		case R855_DTMB_8M_4570:
		case R855_DTMB_6M_4500:
		case R855_DTMB_8M_IF_5M:
		case R855_DTMB_6M_IF_5M:
			if(R855_SetTfType_UL_MID==R855_TF_BEAD_68N)
			{
				 if(LO_freq<=205000)  
 					 R855_Freq_Info.BYP_LPF = 1;      //low pass  (R15[6]=1)
				 else
					 R855_Freq_Info.BYP_LPF = 0;      //bypass  (R15[6]=0)
			}
			else  //R855_TF_BEAD_27N
			{
				 if(LO_freq<=157000)  
 					 R855_Freq_Info.BYP_LPF = 1;      //low pass  (R15[6]=1)
				 else
					 R855_Freq_Info.BYP_LPF = 0;      //bypass  (R15[6]=0)			
			}
			      
		break;

		default:  //other standard
			 if(LO_freq<=245000)  
 	             R855_Freq_Info.BYP_LPF = 1;      //low pass  (R15[6]=1)
			 else
				 R855_Freq_Info.BYP_LPF = 0;      //bypass  (R15[6]=0)

        break;
	}//end switch

	R855_Freq_Info.TEMP = 0;

	return R855_Freq_Info;

}



R855_SysFreq_Info_Type R855_SysFreq_Sel(R855_Standard_Type R855_Standard,UINT32 RF_freq) //CD OK
{
	R855_SysFreq_Info_Type R855_SysFreq_Info;
	
	switch(R855_Standard)
	{
	case R855_DVB_T_6M:
	case R855_DVB_T_7M:
	case R855_DVB_T_8M:
	case R855_DVB_T_6M_IF_5M:
	case R855_DVB_T_7M_IF_5M:
	case R855_DVB_T_8M_IF_5M:
	case R855_DVB_T2_6M:
	case R855_DVB_T2_7M: 
	case R855_DVB_T2_8M:
	case R855_DVB_T2_1_7M:
	case R855_DVB_T2_10M:
    case R855_DVB_T2_6M_IF_5M:
	case R855_DVB_T2_7M_IF_5M:
	case R855_DVB_T2_8M_IF_5M:
	case R855_DVB_T2_1_7M_IF_5M:

		//default for All
		R855_SysFreq_Info.TF_MODE_R13_7_1BT=0;				//[0:plain, 1:sharp]		***
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
		R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=1;         //LNA MAX Gain[0:30, 1:31]
		R855_SysFreq_Info.ENB_ATT_R17_0_1BT=1;              // [0:disable 0~5, 1:enb att]
		R855_SysFreq_Info.LNA_TOP_R37_0_4BT=5;
		R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_56;
		R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
		R855_SysFreq_Info.RF_TOP_R37_4_3BT=4;
		R855_SysFreq_Info.RF_VTH_R38_4_4BT=R855_RFBUF_VTH_1_41;
		R855_SysFreq_Info.RF_VTL_R25_0_4BT=R855_VTL_0_93;
		R855_SysFreq_Info.NRB_TOP_R40_4_4BT=6;
		R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=3;			// Nrb LPF BW 	[0:widest, 1:wide, 2:low, 3:lowest]
		R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=0;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
		R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=2;
		R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=R855_VTH_1_36;       
		R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=R855_VTL_0_83;   
		R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=1;         
		R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=R855_VTH_1_26;   
		R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=R855_VTL_0_63;    
		R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=1;        //[0:normal(widest) ~ 7(narrowest)]	 
		R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=1;			//[0:disable, 1:enable]		***
		R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=1;			//[0:min=1, 1:min=0]
		R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=3;		//[0:max=6, 1:max=8, 2:max=10, 3:max=12]	
		R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=1;			//[0:normal, 1:13~14 (by mixamp)]
		R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=7;		//[0:cont_Both(small), 1:cont_RF(small), 2:cont_lna(small), 3:cont_Both(large+small) , 4:sw_Both(small), 5:sw_RF(small), 6:sw_lna(small), 7:sw_Both (large+small)]
		R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=0;		//[0:0.15u, 1:0.3u, 2:0.45u, 3:0.6u]
		R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=1;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=0;		//[0:disable, 1:enable]		***
		R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=0;          //[0:x1, 1:x1/2]
		R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=3;     //[0:highest, 1:high, 2:low, 3:lowest]  
		R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=0;			//[0:slow, 1:fast]
		R855_SysFreq_Info.CLK_FAST_R47_2_2BT=2;			//[0:250 Hz, 1:500 Hz, 2:1kHz, 3:2kHz]
		R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=3;			//[0:4Hz, 1:16Hz, 2:1Hz, 3:64Hz]
		R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=1;				//[0:disable, 1:enable]
		R855_SysFreq_Info.MODE_SEL_R45_5_1BT=0;				//[0:terrestial mode, 1:Cable mode]
		R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=0;			//[0:1.84V, 1:1.94V]
		R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=1;			//[0:0.34V, 1:0.43V]
		R855_SysFreq_Info.IMG_ACI_R9_4_1BT=0;				//[0:ctrl by img&aci, 1:diable] ***
		R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=1;		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
		R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=2;		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
		R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=2;		//[0: normal, 1:top+6, 2:top+9, 3:top+11]
		R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=1;    //[0:1.3dB, 1:4.3dB]
		R855_SysFreq_Info.HPF_COMP_R35_6_2BT=0;				//[0:normal, 1: +1.5dB, 2: +3dB, 3: +4dB]
		R855_SysFreq_Info.FB1_RES_R35_5_1BT=0;              //[0:2K, 1:8K]
		R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=1;			//[0:+4dB, 1:normal]
		R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=1;			//[0:10dB large (dvbt), 1:normal (analog)]
		R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=1;			//[0:normal, 1:-5dB]


		//exception(overwrite)
		if(RF_freq>=782000 && RF_freq<790000) //LTE
		{
			R855_SysFreq_Info.LNA_TOP_R37_0_4BT=4;
			R855_SysFreq_Info.RF_TOP_R37_4_3BT=5;
			R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=3;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		}

		if(R855_DetectTfType_Cal==R855_UL_USING_270NH) //270n
		{
			if(RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID])  //low band
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         // [0:30, 1:31]
				R855_SysFreq_Info.LNA_TOP_R37_0_4BT=6;
				R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=3;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
			}  //end low band
		}
		else if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) //bead
		{
			if(RF_freq<=100000) //<100MHz
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>100000) && (RF_freq<340000)) //100M~340M
			{
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>=340000) && (RF_freq<=380000)) //340M~380M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}
		break;

    case R855_DVB_C_8M:
	case R855_DVB_C_6M:	
	case R855_DVB_C_8M_IF_5M:
	case R855_DVB_C_6M_IF_5M:	
		//default value [off,31]
		R855_SysFreq_Info.TF_MODE_R13_7_1BT=0;				//[0:plain, 1:sharp]		***
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
		R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=1;         //LNA MAX Gain[0:30, 1:31]
		R855_SysFreq_Info.ENB_ATT_R17_0_1BT=1;              // [0:disable 0~5, 1:enb att]
		R855_SysFreq_Info.LNA_TOP_R37_0_4BT=4;
		R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_46;
		R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
		R855_SysFreq_Info.RF_TOP_R37_4_3BT=2;  
		R855_SysFreq_Info.RF_VTH_R38_4_4BT=R855_RFBUF_VTH_1_41;
		R855_SysFreq_Info.RF_VTL_R25_0_4BT=R855_VTL_0_83;
		R855_SysFreq_Info.NRB_TOP_R40_4_4BT=6;
		R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=3;			// Nrb LPF BW 	[0:widest, 1:wide, 2:low, 3:lowest]
		R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=3;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
		R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=0;
		R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=R855_VTH_1_36;       
		R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=R855_VTL_0_83;   
		R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=1;         
		R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=R855_VTH_1_26;   
		R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=R855_VTL_0_63;    
		R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=1;        //[0:normal(widest) ~ 7(narrowest)]	 
		R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=0;			//[0:disable, 1:enable]		***
		R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=0;			//[0:min=1, 1:min=0]
		R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=3;		//[0:max=6, 1:max=8, 2:max=10, 3:max=12]	
		R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=1;			//[0:normal, 1:13~14 (by mixamp)]
		R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=7;		//[0:cont_Both(small), 1:cont_RF(small), 2:cont_lna(small), 3:cont_Both(large+small) , 4:sw_Both(small), 5:sw_RF(small), 6:sw_lna(small), 7:sw_Both (large+small)]
		R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=0;		//[0:0.15u, 1:0.3u, 2:0.45u, 3:0.6u]
		R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=1;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=0;		//[0:disable, 1:enable]		***
		R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=0;          //[0:x1, 1:x1/2]
		R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=3;     //[0:highest, 1:high, 2:low, 3:lowest]
		R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=0;			//[0:slow, 1:fast]
		R855_SysFreq_Info.CLK_FAST_R47_2_2BT=2;			//[0:250 Hz, 1:500 Hz, 2:1kHz, 3:2kHz]
		R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=3;			//[0:4Hz, 1:16Hz, 2:1Hz, 3:64Hz]
		R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=1;				//[0:disable, 1:enable]
		R855_SysFreq_Info.MODE_SEL_R45_5_1BT=0;				//[0:terrestial mode, 1:Cable mode]
		R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=0;			//[0:1.84V, 1:1.94V]
		R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=1;			//[0:0.34V, 1:0.43V]
		R855_SysFreq_Info.IMG_ACI_R9_4_1BT=1;				//[0:ctrl by img&aci, 1:diable] ***
		R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=1;		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
		R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=0;		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
		R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=2;		//[0: normal, 1:top+6, 2:top+9, 3:top+11]
		R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=1;    //[0:1.3dB, 1:4.3dB]
		R855_SysFreq_Info.HPF_COMP_R35_6_2BT=0;				//[0:normal, 1: +1.5dB, 2: +3dB, 3: +4dB]
		R855_SysFreq_Info.FB1_RES_R35_5_1BT=0;              //[0:2K, 1:8K]
		R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=1;			//[0:+4dB, 1:normal]
		R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=1;			//[0:10dB large (dvbt), 1:normal (analog)]
		R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=1;			//[0:normal, 1:-5dB]

		//exception(overwrite)
		if(R855_DetectTfType_Cal==R855_UL_USING_270NH) //270n
		{
			if((RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID]))  //low band
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}
		else if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) 
		{
			if(RF_freq<=100000) //<100MHz
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>100000) && (RF_freq<340000)) //100M~340M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>=340000) && (RF_freq<=380000)) //340M~380M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}


		break;

	case R855_J83B:
	case R855_J83B_IF_5M:
		//default value [off,31]
		R855_SysFreq_Info.TF_MODE_R13_7_1BT=0;				//[0:plain, 1:sharp]		***
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
		R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=1;         //LNA MAX Gain[0:30, 1:31]
		R855_SysFreq_Info.ENB_ATT_R17_0_1BT=1;              // [0:disable 0~5, 1:enb att]
		R855_SysFreq_Info.LNA_TOP_R37_0_4BT=4;
		R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_46;
		R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
		R855_SysFreq_Info.RF_TOP_R37_4_3BT=2;  
		R855_SysFreq_Info.RF_VTH_R38_4_4BT=R855_RFBUF_VTH_1_41;
		R855_SysFreq_Info.RF_VTL_R25_0_4BT=R855_VTL_0_83;
		R855_SysFreq_Info.NRB_TOP_R40_4_4BT=6;
		R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=3;			// Nrb LPF BW 	[0:widest, 1:wide, 2:low, 3:lowest]
		R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=0;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
		R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=2;
		R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=R855_VTH_1_36;       
		R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=R855_VTL_0_83;   
		R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=1;         
		R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=R855_VTH_1_26;   
		R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=R855_VTL_0_63;    
		R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=1;        //[0:normal(widest) ~ 7(narrowest)]	 
		R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=0;			//[0:disable, 1:enable]		***
		R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=0;			//[0:min=1, 1:min=0]
		R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=3;		//[0:max=6, 1:max=8, 2:max=10, 3:max=12]
		R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=1;			//[0:normal, 1:13~14 (by mixamp)]
		R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=7;		////[0:cont_Both(small), 1:cont_RF(small), 2:cont_lna(small), 3:cont_Both(large+small) , 4:sw_Both(small), 5:sw_RF(small), 6:sw_lna(small), 7:sw_Both (large+small)]
		R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=0;		//[0:0.15u, 1:0.3u, 2:0.45u, 3:0.6u]
		R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=1;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=0;		//[0:disable, 1:enable]		***
		R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=0;          //[0:x1, 1:x1/2]
		R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=3;     //[0:highest, 1:high, 2:low, 3:lowest]  
		R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=0;			//[0:slow, 1:fast]
		R855_SysFreq_Info.CLK_FAST_R47_2_2BT=2;			//[0:250 Hz, 1:500 Hz, 2:1kHz, 3:2kHz]
		R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=3;			//[0:4Hz, 1:16Hz, 2:1Hz, 3:64Hz]
		R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=1;				//[0:disable, 1:enable]
		R855_SysFreq_Info.MODE_SEL_R45_5_1BT=0;				//[0:terrestial mode, 1:Cable mode]
		R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=0;			//[0:1.84V, 1:1.94V]
		R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=1;			//[0:0.34V, 1:0.43V]
		R855_SysFreq_Info.IMG_ACI_R9_4_1BT=1;				//[0:ctrl by img&aci, 1:diable] ***
		R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=1;		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
		R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=0;		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
		R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=2;		//[0: normal, 1:top+6, 2:top+9, 3:top+11]
		R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=1;    //[0:1.3dB, 1:4.3dB]
		R855_SysFreq_Info.HPF_COMP_R35_6_2BT=0;				//[0:normal, 1: +1.5dB, 2: +3dB, 3: +4dB]
		R855_SysFreq_Info.FB1_RES_R35_5_1BT=0;              //[0:2K, 1:8K]
		R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=1;			//[0:+4dB, 1:normal]
		R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=1;			//[0:10dB large (dvbt), 1:normal (analog)]
		R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=1;			//[0:normal, 1:-5dB]

		//exception(overwrite)
		if(R855_DetectTfType_Cal==R855_UL_USING_270NH) //270n
		{
			if((RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID]))  //low band
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}
		else if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) 
		{
			if(RF_freq<=100000) //<100MHz
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>100000) && (RF_freq<340000)) //100M~340M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>=340000) && (RF_freq<=380000)) //340M~380M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}

		break;

	case R855_ISDB_T_4063:	
	case R855_ISDB_T_4570:	
	case R855_ISDB_T_IF_5M:	
		//default for All
		R855_SysFreq_Info.TF_MODE_R13_7_1BT=0;				//[0:plain, 1:sharp]		***
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
		R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=1;         //LNA MAX Gain[0:30, 1:31]
		R855_SysFreq_Info.ENB_ATT_R17_0_1BT=1;              // [0:disable 0~5, 1:enb att]
		R855_SysFreq_Info.LNA_TOP_R37_0_4BT=5;
		R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_56;
		R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
		R855_SysFreq_Info.RF_TOP_R37_4_3BT=5;
		R855_SysFreq_Info.RF_VTH_R38_4_4BT=R855_RFBUF_VTH_1_41;
		R855_SysFreq_Info.RF_VTL_R25_0_4BT=R855_VTL_0_93;
		R855_SysFreq_Info.NRB_TOP_R40_4_4BT=6;
		R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=3;			// Nrb LPF BW 	[0:widest, 1:wide, 2:low, 3:lowest]
		R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=0;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
		R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=3;
		R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=R855_VTH_1_36;       
		R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=R855_VTL_0_83;   
		R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=1;         
		R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=R855_VTH_1_26;   
		R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=R855_VTL_0_63;    
		R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=1;        //[0:normal(widest) ~ 7(narrowest)]	 
		R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=1;			//[0:disable, 1:enable]		***
		R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=1;			//[0:min=1, 1:min=0]
		R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=3;		//[0:max=6, 1:max=8, 2:max=10, 3:max=12]	
		R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=0;			//[0:normal, 1:13~14 (by mixamp)]
		R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=7;		//[0:cont_Both(small), 1:cont_RF(small), 2:cont_lna(small), 3:cont_Both(large+small) , 4:sw_Both(small), 5:sw_RF(small), 6:sw_lna(small), 7:sw_Both (large+small)]
		R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=0;		//[0:0.15u, 1:0.3u, 2:0.45u, 3:0.6u]
		R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=1;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=0;		//[0:disable, 1:enable]		***
		R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=0;          //[0:x1, 1:x1/2]
		R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=3;     //[0:highest, 1:high, 2:low, 3:lowest]  
		R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=0;			//[0:slow, 1:fast]
		R855_SysFreq_Info.CLK_FAST_R47_2_2BT=2;			//[0:250 Hz, 1:500 Hz, 2:1kHz, 3:2kHz]
		R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=3;			//[0:4Hz, 1:16Hz, 2:1Hz, 3:64Hz]
		R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=1;				//[0:disable, 1:enable]
		R855_SysFreq_Info.MODE_SEL_R45_5_1BT=0;				//[0:terrestial mode, 1:Cable mode]
		R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=0;			//[0:1.84V, 1:1.94V]
		R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=1;			//[0:0.34V, 1:0.43V]
		R855_SysFreq_Info.IMG_ACI_R9_4_1BT=0;				//[0:ctrl by img&aci, 1:diable] ***
		R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=1;		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
		R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=2;		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
		R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=2;		//[0: normal, 1:top+6, 2:top+9, 3:top+11]
		R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=1;    //[0:1.3dB, 1:4.3dB]
		R855_SysFreq_Info.HPF_COMP_R35_6_2BT=0;				//[0:normal, 1: +1.5dB, 2: +3dB, 3: +4dB]
		R855_SysFreq_Info.FB1_RES_R35_5_1BT=0;              //[0:2K, 1:8K]
		R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=1;			//[0:+4dB, 1:normal]
		R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=1;			//[0:10dB large (dvbt), 1:normal (analog)]
		R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=1;			//[0:normal, 1:-5dB]


		//exception(overwrite)
		if(R855_DetectTfType_Cal==R855_UL_USING_270NH) //270n
		{
			if(RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID])  //low band
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         // [0:30, 1:31]
				R855_SysFreq_Info.NRB_TOP_R40_4_4BT=15;
			}  //end low band
		}
		else if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) //bead
		{
			if(RF_freq<=100000) //<100MHz
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>100000) && (RF_freq<340000)) //100M~340M
			{
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>=340000) && (RF_freq<=380000)) //340M~380M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}

		break;

	case R855_DTMB_8M_4570:
	case R855_DTMB_6M_4500:
	case R855_DTMB_8M_IF_5M:
	case R855_DTMB_6M_IF_5M:
	//default for All
		R855_SysFreq_Info.TF_MODE_R13_7_1BT=0;				//[0:plain, 1:sharp]		***
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
		R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=1;         //LNA MAX Gain[0:30, 1:31]
		R855_SysFreq_Info.ENB_ATT_R17_0_1BT=1;              // [0:disable 0~5, 1:enb att]
		R855_SysFreq_Info.LNA_TOP_R37_0_4BT=5;
		R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_56;
		R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
		R855_SysFreq_Info.RF_TOP_R37_4_3BT=4;
		R855_SysFreq_Info.RF_VTH_R38_4_4BT=R855_RFBUF_VTH_1_41;
		R855_SysFreq_Info.RF_VTL_R25_0_4BT=R855_VTL_0_93;
		R855_SysFreq_Info.NRB_TOP_R40_4_4BT=6;
		R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=3;			// Nrb LPF BW 	[0:widest, 1:wide, 2:low, 3:lowest]
		R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=0;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
		R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=1;
		R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=R855_VTH_1_36;       
		R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=R855_VTL_0_83;   
		R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=1;         
		R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=R855_VTH_1_26;   
		R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=R855_VTL_0_63;    
		R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=1;        //[0:normal(widest) ~ 7(narrowest)]	 
		R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=1;			//[0:disable, 1:enable]		***
		R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=1;			//[0:min=1, 1:min=0]
		R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=3;		//[0:max=6, 1:max=8, 2:max=10, 3:max=12]	
		R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=0;			//[0:normal, 1:13~14 (by mixamp)]
		R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=7;		//[0:cont_Both(small), 1:cont_RF(small), 2:cont_lna(small), 3:cont_Both(large+small) , 4:sw_Both(small), 5:sw_RF(small), 6:sw_lna(small), 7:sw_Both (large+small)]
		R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=0;		//[0:0.15u, 1:0.3u, 2:0.45u, 3:0.6u]
		R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=1;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=0;		//[0:disable, 1:enable]		***
		R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=0;          //[0:x1, 1:x1/2]
		R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=3;     //[0:highest, 1:high, 2:low, 3:lowest]  
		R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=0;			//[0:slow, 1:fast]
		R855_SysFreq_Info.CLK_FAST_R47_2_2BT=2;			//[0:250 Hz, 1:500 Hz, 2:1kHz, 3:2kHz]
		R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=3;			//[0:4Hz, 1:16Hz, 2:1Hz, 3:64Hz]
		R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=1;				//[0:disable, 1:enable]
		R855_SysFreq_Info.MODE_SEL_R45_5_1BT=0;				//[0:terrestial mode, 1:Cable mode]
		R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=0;			//[0:1.84V, 1:1.94V]
		R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=1;			//[0:0.34V, 1:0.43V]
		R855_SysFreq_Info.IMG_ACI_R9_4_1BT=0;				//[0:ctrl by img&aci, 1:diable] ***
		R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=1;		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
		R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=2;		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
		R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=2;		//[0: normal, 1:top+6, 2:top+9, 3:top+11]
		R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=1;    //[0:1.3dB, 1:4.3dB]
		R855_SysFreq_Info.HPF_COMP_R35_6_2BT=0;				//[0:normal, 1: +1.5dB, 2: +3dB, 3: +4dB]
		R855_SysFreq_Info.FB1_RES_R35_5_1BT=0;              //[0:2K, 1:8K]
		R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=1;			//[0:+4dB, 1:normal]
		R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=1;			//[0:10dB large (dvbt), 1:normal (analog)]
		R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=1;			//[0:normal, 1:-5dB]


		//exception(overwrite)
		if(R855_DetectTfType_Cal==R855_UL_USING_270NH) //270n
		{
			if(RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID])  //low band
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         // [0:30, 1:31]
			}  //end low band
		}
		else if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) //bead
		{
			if(RF_freq<=100000) //<100MHz
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>100000) && (RF_freq<340000)) //100M~340M
			{
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>=340000) && (RF_freq<=380000)) //340M~380M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}
		break;

	case R855_ATSC:
	case R855_ATSC3:
	case R855_ATSC_IF_5M:
	case R855_ATSC3_IF_5M:
		//default value [off,31]
		R855_SysFreq_Info.TF_MODE_R13_7_1BT=0;				//[0:plain, 1:sharp]		***
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
		R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=1;         //LNA MAX Gain[0:30, 1:31]
		R855_SysFreq_Info.ENB_ATT_R17_0_1BT=1;              // [0:disable 0~5, 1:enb att]
		R855_SysFreq_Info.LNA_TOP_R37_0_4BT=6;
		R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_66;
		R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_1_13;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
		R855_SysFreq_Info.RF_TOP_R37_4_3BT=5;   
		R855_SysFreq_Info.RF_VTH_R38_4_4BT=R855_RFBUF_VTH_1_41;
		R855_SysFreq_Info.RF_VTL_R25_0_4BT=R855_VTL_0_93;
		R855_SysFreq_Info.NRB_TOP_R40_4_4BT=3;
		R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=3;			// Nrb LPF BW 	[0:widest, 1:wide, 2:low, 3:lowest]
		R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=3;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
		R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=0;
		R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=R855_VTH_1_36;       
		R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=R855_VTL_0_73;   
		R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=3;         
		R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=R855_VTH_1_26;   
		R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=R855_VTL_0_73;    
		R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=1;        //[0:normal(widest) ~ 7(narrowest)]	 
		R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=1;			//[0:disable, 1:enable]		***
		R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=0;			//[0:min=1, 1:min=0]
		R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=2;		//[0:max=6, 1:max=8, 2:max=10, 3:max=12]
		R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=0;			//[0:normal, 1:13~14 (by mixamp)]
		R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=7;		////[0:cont_Both(small), 1:cont_RF(small), 2:cont_lna(small), 3:cont_Both(large+small) , 4:sw_Both(small), 5:sw_RF(small), 6:sw_lna(small), 7:sw_Both (large+small)]
		R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=0;		//[0:0.15u, 1:0.3u, 2:0.45u, 3:0.6u]
		R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=2;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=0;		//[0:disable, 1:enable]		***
		R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=0;          //[0:x1, 1:x1/2]
		R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=0;     //[0:highest, 1:high, 2:low, 3:lowest]  
		R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=0;			//[0:slow, 1:fast]
		R855_SysFreq_Info.CLK_FAST_R47_2_2BT=3;			//[0:250 Hz, 1:500 Hz, 2:1kHz, 3:2kHz]
		R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=3;			//[0:4Hz, 1:16Hz, 2:1Hz, 3:64Hz]
		R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=1;				//[0:disable, 1:enable]
		R855_SysFreq_Info.MODE_SEL_R45_5_1BT=1;				//[0:terrestial mode, 1:Cable mode]
		R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=0;			//[0:1.84V, 1:1.94V]
		R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=1;			//[0:0.34V, 1:0.43V]
		R855_SysFreq_Info.IMG_ACI_R9_4_1BT=0;				//[0:ctrl by img&aci, 1:diable] ***
		R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=0;		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
		R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=3;		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
		R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=3;		//[0: normal, 1:top+6, 2:top+9, 3:top+11]
		R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=1;    //[0:1.3dB, 1:4.3dB]
		R855_SysFreq_Info.HPF_COMP_R35_6_2BT=0;				// [0:normal, 1: +1.5dB, 2: +3dB, 3: +4dB]
		R855_SysFreq_Info.FB1_RES_R35_5_1BT=0;              //[0:2K, 1:8K]
		R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=1;			//[0:+4dB, 1:normal]
		R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=1;			//[0:10dB large (dvbt), 1:normal (analog)]
		R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=1;			//[0:normal, 1:-5dB]


		//exception
		if(R855_DetectTfType_Cal==R855_UL_USING_270NH) //270n
		{
			if((RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID]))  //low band
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}
		else if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) 
		{
			if(RF_freq<=100000) //<100MHz
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
				R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_46;
				R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
			}
			else if((RF_freq>100000) && (RF_freq<340000)) //100M~340M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
				R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_46;
				R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
			}
			else if((RF_freq>=340000) && (RF_freq<=380000)) //340M~380M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
				R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_46;
				R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
			}
		}
		break;

	case R855_FM:
		//default for All
		R855_SysFreq_Info.TF_MODE_R13_7_1BT=0;				//[0:plain, 1:sharp]		***
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
		R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=1;         //LNA MAX Gain[0:30, 1:31]
		R855_SysFreq_Info.ENB_ATT_R17_0_1BT=1;              // [0:disable 0~5, 1:enb att]
		R855_SysFreq_Info.LNA_TOP_R37_0_4BT=5;
		R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_56;
		R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
		R855_SysFreq_Info.RF_TOP_R37_4_3BT=4;
		R855_SysFreq_Info.RF_VTH_R38_4_4BT=R855_RFBUF_VTH_1_41;
		R855_SysFreq_Info.RF_VTL_R25_0_4BT=R855_VTL_0_93;
		R855_SysFreq_Info.NRB_TOP_R40_4_4BT=6;
		R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=3;			// Nrb LPF BW 	[0:widest, 1:wide, 2:low, 3:lowest]
		R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=0;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
		R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=2;
		R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=R855_VTH_1_36;       
		R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=R855_VTL_0_83;   
		R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=1;         
		R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=R855_VTH_1_26;   
		R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=R855_VTL_0_63;    
		R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=1;        //[0:normal(widest) ~ 7(narrowest)]	 
		R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=1;			//[0:disable, 1:enable]		***
		R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=1;			//[0:min=1, 1:min=0]
		R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=3;		//[0:max=6, 1:max=8, 2:max=10, 3:max=12]	
		R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=1;			//[0:normal, 1:13~14 (by mixamp)]
		R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=7;		//[0:cont_Both(small), 1:cont_RF(small), 2:cont_lna(small), 3:cont_Both(large+small) , 4:sw_Both(small), 5:sw_RF(small), 6:sw_lna(small), 7:sw_Both (large+small)]
		R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=0;		//[0:0.15u, 1:0.3u, 2:0.45u, 3:0.6u]
		R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=1;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=0;		//[0:disable, 1:enable]		***
		R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=0;          //[0:x1, 1:x1/2]
		R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=3;     //[0:highest, 1:high, 2:low, 3:lowest]  
		R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=0;			//[0:slow, 1:fast]
		R855_SysFreq_Info.CLK_FAST_R47_2_2BT=2;			//[0:250 Hz, 1:500 Hz, 2:1kHz, 3:2kHz]
		R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=3;			//[0:4Hz, 1:16Hz, 2:1Hz, 3:64Hz]
		R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=1;				//[0:disable, 1:enable]
		R855_SysFreq_Info.MODE_SEL_R45_5_1BT=0;				//[0:terrestial mode, 1:Cable mode]
		R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=0;			//[0:1.84V, 1:1.94V]
		R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=1;			//[0:0.34V, 1:0.43V]
		R855_SysFreq_Info.IMG_ACI_R9_4_1BT=0;				//[0:ctrl by img&aci, 1:diable] ***
		R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=1;		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
		R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=2;		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
		R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=2;		//[0: normal, 1:top+6, 2:top+9, 3:top+11]
		R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=1;    //[0:1.3dB, 1:4.3dB]
		R855_SysFreq_Info.HPF_COMP_R35_6_2BT=0;				//[0:normal, 1: +1.5dB, 2: +3dB, 3: +4dB]
		R855_SysFreq_Info.FB1_RES_R35_5_1BT=0;              //[0:2K, 1:8K]
		R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=1;			//[0:+4dB, 1:normal]
		R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=1;			//[0:10dB large (dvbt), 1:normal (analog)]
		R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=1;			//[0:normal, 1:-5dB]


		//exception(overwrite)
		if(RF_freq>=782000 && RF_freq<790000) //LTE
		{
			R855_SysFreq_Info.LNA_TOP_R37_0_4BT=4;
			R855_SysFreq_Info.RF_TOP_R37_4_3BT=5;
			R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=3;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		}

		if(R855_DetectTfType_Cal==R855_UL_USING_270NH) //270n
		{
			if(RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID])  //low band
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         // [0:30, 1:31]
				R855_SysFreq_Info.LNA_TOP_R37_0_4BT=6;
				R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=3;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
			}  //end low band
		}
		else if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) //bead
		{
			if(RF_freq<=100000) //<100MHz
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>100000) && (RF_freq<340000)) //100M~340M
			{
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>=340000) && (RF_freq<=380000)) //340M~380M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}
		break;

	default: //DVB-T
		//default for All
		R855_SysFreq_Info.TF_MODE_R13_7_1BT=0;				//[0:plain, 1:sharp]		***
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]   
		R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=1;         //LNA MAX Gain[0:30, 1:31]
		R855_SysFreq_Info.ENB_ATT_R17_0_1BT=1;              // [0:disable 0~5, 1:enb att]
		R855_SysFreq_Info.LNA_TOP_R37_0_4BT=5;
		R855_SysFreq_Info.LNA_VTH_R38_0_4BT=R855_VTH_1_56;
		R855_SysFreq_Info.LNA_VTL_R13_1_4BT=R855_VTL_0_93;  //usable=>[R855_VTL_0_53, R855_VTL_0_73, R855_VTL_0_93, R855_VTL_1_13, R855_VTL_1_33, R855_VTL_1_53, R855_VTL_1_73, R855_VTL_1_93]
		R855_SysFreq_Info.RF_TOP_R37_4_3BT=4;
		R855_SysFreq_Info.RF_VTH_R38_4_4BT=R855_RFBUF_VTH_1_41;
		R855_SysFreq_Info.RF_VTL_R25_0_4BT=R855_VTL_0_93;
		R855_SysFreq_Info.NRB_TOP_R40_4_4BT=6;
		R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=3;			// Nrb LPF BW 	[0:widest, 1:wide, 2:low, 3:lowest]
		R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=0;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
		R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=2;
		R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=R855_VTH_1_36;       
		R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=R855_VTL_0_83;   
		R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=1;         
		R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=R855_VTH_1_26;   
		R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=R855_VTL_0_63;    
		R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=1;        //[0:normal(widest) ~ 7(narrowest)]	 
		R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=1;			//[0:disable, 1:enable]		***
		R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=1;			//[0:min=1, 1:min=0]
		R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=3;		//[0:max=6, 1:max=8, 2:max=10, 3:max=12]	
		R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=1;			//[0:normal, 1:13~14 (by mixamp)]
		R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=7;		//[0:cont_Both(small), 1:cont_RF(small), 2:cont_lna(small), 3:cont_Both(large+small) , 4:sw_Both(small), 5:sw_RF(small), 6:sw_lna(small), 7:sw_Both (large+small)]
		R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=1;			//[0:1/3 dis current, 1:normal]
		R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=0;		//[0:0.15u, 1:0.3u, 2:0.45u, 3:0.6u]
		R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=1;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=0;		//[0:disable, 1:enable]		***
		R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=0;          //[0:x1, 1:x1/2]
		R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=3;     //[0:highest, 1:high, 2:low, 3:lowest]  
		R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=0;			//[0:slow, 1:fast]
		R855_SysFreq_Info.CLK_FAST_R47_2_2BT=2;			//[0:250 Hz, 1:500 Hz, 2:1kHz, 3:2kHz]
		R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=3;			//[0:4Hz, 1:16Hz, 2:1Hz, 3:64Hz]
		R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=1;				//[0:disable, 1:enable]
		R855_SysFreq_Info.MODE_SEL_R45_5_1BT=0;				//[0:terrestial mode, 1:Cable mode]
		R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=0;			//[0:1.84V, 1:1.94V]
		R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=1;			//[0:0.34V, 1:0.43V]
		R855_SysFreq_Info.IMG_ACI_R9_4_1BT=0;				//[0:ctrl by img&aci, 1:diable] ***
		R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=1;		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
		R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=2;		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
		R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=2;		//[0: normal, 1:top+6, 2:top+9, 3:top+11]
		R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=1;    //[0:1.3dB, 1:4.3dB]
		R855_SysFreq_Info.HPF_COMP_R35_6_2BT=0;				//[0:normal, 1: +1.5dB, 2: +3dB, 3: +4dB]
		R855_SysFreq_Info.FB1_RES_R35_5_1BT=0;              //[0:2K, 1:8K]
		R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=1;			//[0:+4dB, 1:normal]
		R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=1;			//[0:10dB large (dvbt), 1:normal (analog)]
		R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=1;			//[0:normal, 1:-5dB]


		//exception(overwrite)
		if(RF_freq>=782000 && RF_freq<790000) //LTE
		{
			R855_SysFreq_Info.LNA_TOP_R37_0_4BT=4;
			R855_SysFreq_Info.RF_TOP_R37_4_3BT=5;
			R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=3;		//[0:0.9u, 1:1.5u, 2:2.4u, 3:3u]
		}

		if(R855_DetectTfType_Cal==R855_UL_USING_270NH) //270n
		{
			if(RF_freq<R855_LNA_MID_LOW[R855_SetTfType_UL_MID])  //low band
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         // [0:30, 1:31]
				R855_SysFreq_Info.LNA_TOP_R37_0_4BT=6;
				R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=3;			// Nrb HPF BW	[0:lowest, 1:low, 2:high, 3:highest]
			}  //end low band
		}
		else if(R855_DetectTfType_Cal==R855_UL_USING_BEAD) //bead
		{
			if(RF_freq<=100000) //<100MHz
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>100000) && (RF_freq<340000)) //100M~340M
			{
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
			else if((RF_freq>=340000) && (RF_freq<=380000)) //340M~380M
			{
				R855_SysFreq_Info.Q_CTRL_R14_7_1BT=1;               // [0:off, 1:1.5k]   
				R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=0;         //LNA MAX Gain[0:30, 1:31]
			}
		}
		break;
	
	} //end switch


	//exception(overwrite)
	if(RF_freq<=64000) //Q Ctrl OFF
	{
		R855_SysFreq_Info.Q_CTRL_R14_7_1BT=0;               // [0:off, 1:1.5k]
	}


	R855_SysFreq_Info.TEMP=0;

#if(R855_DEBUG==1)
	R855_Printf_SysFreq_Parameter(R855_SysFreq_Info);
#endif
	return R855_SysFreq_Info;
}
R855_ErrCode Init_R855(Endeavour  *pEndeavour, R855_Set_Info *pR855_Info)
{
	uint8_t i2c_bus = 3;
	uint8_t i2c_r855_w_addr = 0xF6;//read chip id is 0xF7
	R855_ErrCode ret=R855_Success;

	I2C_Init(pEndeavour, i2c_bus,i2c_r855_w_addr);//R855

#if 0		
	R855_Read_ChipID();
#endif
	//Initial R855
	if (R855_Init(pR855_Info->R855_Standard) != R855_Success)
	{
		printf("\n R855_Init failed! \n");
		ret=R855_Fail;

	}

	return ret;

}
R855_ErrCode Tune_R855(R855_Set_Info *pR855_Info)
{
	R855_ErrCode ret=R855_Success;
	if(pR855_Info)
	{
		if (R855_SetPllData(*pR855_Info) != R855_Success)
		{
			printf("\n R855_SetPllData failed! \n");
			ret=R855_Fail;
		}
		else
		{
			printf("\n R855_tune ok \n");
			ret=R855_Success;
		}
		
	}
	else
	{
		printf("\n pR855_Info is Null \n");
		ret=R855_Fail;
	}
	return ret;
}

R855_ErrCode R855_Init(R855_Standard_Type R855_Standard)
{
    UINT8 i;
	R855_Sys_Info_Type R855_Sys_Init;
#if(R855_DEBUG==1)
	R855_PRINT("Start Initial process...\n");
#endif

	R855_Sys_Init = R855_Sys_Sel(R855_Standard);	//Get R850_Mixer_Mode by different standard

	//Xtal cap (??)
	if(R855_SHARE_XTAL==R855_SLAVE_XTAL_OUT)
	{
		R855_XTAL_CAP = 0;
	}
	else
	{
		R855_XTAL_CAP = R855_XTAL_CAP_VALUE;
	}

	if(R855_Initial_done_flag==FALSE)
	{
		  if(R855_InitReg() != R855_Success)        
			  return R855_Fail;

		  R855_IMR_Cal_Result = 0; 
		  R855_TF_Check_Result = 0;

		  //reset filter cal. data
		  for (i=0; i<R855_STD_SIZE; i++)
		  {	  
			  R855_Fil_Cal_flag[i] = FALSE;
			  R855_Fil_Cal_code[i] = 0;      //R22[5:1]
			  R855_Fil_Cal_BW[i] = 0x00;
			  R855_Fil_Cal_LpfLsb[i] = 0;  //R22[0]
		  }

          if(R855_IMR_done_flag==FALSE)
		  {
			  if(R855_TF_Check() != R855_Success)        
				 return R855_Fail;

			  //start IMR calibration
			  if(R855_Cal_Prepare(R855_IMR_CAL) != R855_Success)     
				  return R855_Fail;

#if(R855_DEBUG==1)
	R855_PRINT("IM_Flag[TRUE:Full K, FALSE: No Full K]\n");
	R855_PRINT("Rev_Mode[0:R855_IMR_NOR, 1:R855_IMR_REV]\n");
	R855_PRINT("Mix_Gain_Mode[0:R855_IMR_MIX_GAIN_G7, 1:R855_IMR_MIX_GAIN_L7]\n");
#endif	

				if(R855_Mixer_Mode == R855_IMR_NOR)
				{

					  if(R855_IMR(2, TRUE, R855_IMR_NOR, R855_IMR_MIX_GAIN_G7) != R855_Success)  //Full K node 2
						return R855_Fail;

					  if(R855_IMR(1, TRUE, R855_IMR_NOR, R855_IMR_MIX_GAIN_G7) != R855_Success)	 //Full K node 1
						return R855_Fail;

					  if(R855_IMR(0, FALSE, R855_IMR_NOR, R855_IMR_MIX_GAIN_G7) != R855_Success)
						return R855_Fail;

					  if(R855_IMR(3, FALSE, R855_IMR_NOR, R855_IMR_MIX_GAIN_G7) != R855_Success)   
						return R855_Fail;

					  if(R855_IMR(2, FALSE, R855_IMR_NOR, R855_IMR_MIX_GAIN_L7) != R855_Success)      
						return R855_Fail;

					  if(R855_IMR(1, FALSE, R855_IMR_NOR, R855_IMR_MIX_GAIN_L7) != R855_Success)
						return R855_Fail;

					  if(R855_IMR(0, FALSE, R855_IMR_NOR, R855_IMR_MIX_GAIN_L7) != R855_Success)
						return R855_Fail;

					  if(R855_IMR(3, FALSE, R855_IMR_NOR, R855_IMR_MIX_GAIN_L7) != R855_Success)   
						return R855_Fail;
				}
				else
				{
					  //Reverse
					  if(R855_IMR(6, TRUE, R855_IMR_REV, R855_IMR_MIX_GAIN_G7) != R855_Success)   //Full K node 6, Rev
						return R855_Fail;

					  if(R855_IMR(5, TRUE, R855_IMR_REV, R855_IMR_MIX_GAIN_G7) != R855_Success) 
						return R855_Fail;

					  if(R855_IMR(4, FALSE, R855_IMR_REV, R855_IMR_MIX_GAIN_G7) != R855_Success)   
						return R855_Fail;

					  if(R855_IMR(7, TRUE, R855_IMR_REV, R855_IMR_MIX_GAIN_G7) != R855_Success)   
						return R855_Fail;

					  if(R855_IMR(6, FALSE, R855_IMR_REV, R855_IMR_MIX_GAIN_L7) != R855_Success)   //Full K node 6, Rev
						return R855_Fail;

					  if(R855_IMR(5, FALSE, R855_IMR_REV, R855_IMR_MIX_GAIN_L7) != R855_Success) 
						return R855_Fail;

					  if(R855_IMR(4, FALSE, R855_IMR_REV, R855_IMR_MIX_GAIN_L7) != R855_Success)   
						return R855_Fail;

					  if(R855_IMR(7, FALSE, R855_IMR_REV, R855_IMR_MIX_GAIN_L7) != R855_Success)   
						return R855_Fail;
				}
			  R855_IMR_done_flag = TRUE;
		  }

		  //do Xtal check
		  if(R855_SHARE_XTAL==R855_SLAVE_XTAL_OUT)
		  {
			  R855_Xtal_Pwr = R855_XTAL_HIGHEST;
		  }
		  else if(R855_SHARE_XTAL==R855_MASTER_TO_SLAVE_XTAL_OUT)
		  {
			  R855_Xtal_Pwr = R855_XTAL_HIGHEST;
		  }
		  else
		  {
			  if(R855_InitReg() != R855_Success)        
				 return R855_Fail;

			  if(R855_Xtal_Check() != R855_Success)        
				 return R855_Fail;

			  if(R855_Xtal_Pwr_tmp==R855_XTAL_HIGHEST)
				  R855_Xtal_Pwr = R855_XTAL_HIGHEST;
			  else if(R855_Xtal_Pwr_tmp<=2)  //<=low, gm=1.5
				  R855_Xtal_Pwr = 2;
			  else
				  R855_Xtal_Pwr = R855_Xtal_Pwr_tmp+1;	

		  }

#if(R855_DEBUG==1)
	R855_PRINT("Xtal Power = %d\n", R855_Xtal_Pwr);
#endif
		  R855_Initial_done_flag = TRUE;

	} //end if(check init flag)

	//write initial reg
	if(R855_InitReg() != R855_Success)        
		return R855_Fail;

	R855_pre_standard = R855_STD_SIZE;

	return R855_Success;
}



R855_ErrCode R855_InitReg(void) //CD OK
{
	UINT8 InitArrayCunt = 0;
	UINT8 XtalCap, CapTot, Cap_x;
	
	//Write Full Table, Set Xtal Power = highest at initial
	R855_I2C_Len.RegAddr = 0;
	R855_I2C_Len.Len = R855_REG_NUM;

	if(R855_XTAL_CAP>30)
	{
		CapTot = R855_XTAL_CAP-10;
		XtalCap = 1;  //10
	}
	else
	{
		CapTot = R855_XTAL_CAP;
		XtalCap = 0;  //0
	}
	Cap_x = CapTot >> 1;

	R855_iniArray[0][33]=(R855_iniArray[0][33] & 0xD0) |  (XtalCap<<5) | Cap_x;
	R855_iniArray[1][33]=(R855_iniArray[1][33] & 0xD0) |  (XtalCap<<5) | Cap_x;  //24M
	R855_iniArray[2][33]=(R855_iniArray[2][33] & 0xD0) |  (XtalCap<<5) | Cap_x;
/*     CD??
	//DLDO1 off
	R855_iniArray[0][8]=(R855_iniArray[0][8] & 0xCF) | 0x30;
	R855_iniArray[1][8]=(R855_iniArray[1][8] & 0xCF) | 0x30;
	R855_iniArray[2][8]=(R855_iniArray[2][8] & 0xCF) | 0x30;

	//DLDO2, ALDO, Mixer, IQ_Gen off
	R855_iniArray[0][11]=(R855_iniArray[0][11] & 0xC0) | 0x3F;
	R855_iniArray[1][11]=(R855_iniArray[1][11] & 0xC0) | 0x3F;
	R855_iniArray[2][11]=(R855_iniArray[2][11] & 0xC0) | 0x3F;

	//gm=off, xtal power=highest
	if(R855_SHARE_XTAL==R855_SLAVE_XTAL_OUT)
	{
		R855_iniArray[0][32]=(R855_iniArray[0][32] & 0xE1) | 0x18;
		R855_iniArray[1][32]=(R855_iniArray[1][32] & 0xE1) | 0x18;
		R855_iniArray[2][32]=(R855_iniArray[2][32] & 0xE1) | 0x18;
	}
	else if(R855_SHARE_XTAL==R855_MASTER_TO_SLAVE_XTAL_OUT) //gm=2.0; xtal power=highest
	{
		R855_iniArray[0][32]=(R855_iniArray[0][32] & 0xE1) | 0x00;
		R855_iniArray[1][32]=(R855_iniArray[1][32] & 0xE1) | 0x00;
		R855_iniArray[2][32]=(R855_iniArray[2][32] & 0xE1) | 0x00;
	}
*/
	//update reg
	if(R855_Xtal == 24000)
	{
		for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
		{
			R855_I2C_Len.Data[InitArrayCunt] = R855_iniArray[0][InitArrayCunt];
			R855_Array[InitArrayCunt] = R855_iniArray[0][InitArrayCunt];
		}
	}
	else if(R855_Xtal == 16000)
	{
		for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
		{
			R855_I2C_Len.Data[InitArrayCunt] = R855_iniArray[1][InitArrayCunt];
			R855_Array[InitArrayCunt] = R855_iniArray[1][InitArrayCunt];
		}	
	}
	else if(R855_Xtal == 27000)
	{
		for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
		{
			R855_I2C_Len.Data[InitArrayCunt] = R855_iniArray[2][InitArrayCunt];
			R855_Array[InitArrayCunt] = R855_iniArray[2][InitArrayCunt];
		}	
	}
	else
	{
		//no support now
		R855_PRINT("[R855_InitReg]no support now\n");
		return R855_Fail;
	}


	if(I2C_Write_Len(&R855_I2C_Len) != R855_Success)
	{
		R855_PRINT("[R855_InitReg]I2C_Write_Len failed!\n");
		return R855_Fail;
	}
/*    CD??
	//Mixer ON
	R855_I2C.RegAddr = 11;
	R855_Array[11] = R855_Array[11] & 0xFD;  //R11[1]=0
	R855_I2C.Data = R855_Array[11];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//IQ Gen ON
	R855_I2C.RegAddr = 11;
	R855_Array[11] = R855_Array[11] & 0xFE;  //R11[0]=0
	R855_I2C.Data = R855_Array[11];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//DLDO1 ON
	R855_I2C.RegAddr = 8;
	R855_Array[8] = (R855_Array[8] & 0xCF) | 0x00;  //R8[5:4]=00
	R855_I2C.Data = R855_Array[8];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//ALDO, DLDO2 ON
	R855_I2C.RegAddr = 11;
	R855_Array[11] = (R855_Array[11] & 0xC3) | 0x10;  //R11[5:4]=01, R11[3:2]=00
	R855_I2C.Data = R855_Array[11];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//LNA manual 0
	R855_I2C.RegAddr = 13;
	R855_Array[13] = (R855_Array[13] & 0xC0) | 0x01;  
	R855_I2C.Data = R855_Array[13];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//LNA Auto
	R855_I2C.RegAddr = 13;
	R855_Array[13] = (R855_Array[13] & 0xFE) | 0x00;  
	R855_I2C.Data = R855_Array[13];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
*/
	return R855_Success;
}


R855_ErrCode R855_TF_Check(void)  //CD  OK
{

	UINT8   ADC_Read_Value0 = 60;
	UINT8   ADC_Read_Value32 = 0;

/*
	R855_Delay_MS(R855_XTAL_CHK_DELAY);

	R855_I2C_Len.RegAddr = 0;
	R855_I2C_Len.Len = 2;    
	if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
	{
		I2C_Read_Len(&R855_I2C_Len);
	}
*/
	//------- Low Band TF detect --------//
	if(R855_Cal_Prepare(R855_TF_LNA_CAL) != R855_Success)     
		return R855_Fail;

	//Set LPF Gain = 0
	R855_I2C.RegAddr = 25;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x0F) | (0<<4);
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];  
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	 //Set LNA TF=(0,0)
	 R855_I2C.RegAddr = 14;
     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x80);  	
     R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
     if(I2C_Write(&R855_I2C) != R855_Success)
          return R855_Fail;

	 R855_Delay_MS(10 * 1000); 	// by ITE

	 if(R855_Muti_Read(&ADC_Read_Value0) != R855_Success)
		  return R855_Fail;

	 //Set LNA TF=32
	 R855_I2C.RegAddr = 14;
     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x80) | 32 ;
     R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
     if(I2C_Write(&R855_I2C) != R855_Success)
          return R855_Fail;

	 R855_Delay_MS(10 * 1000); 	// by ITE

	 if(R855_Muti_Read(&ADC_Read_Value32) != R855_Success)
		 return R855_Fail;

	 //if ADC_Read=0, TF_Check_Fail
	 if((ADC_Read_Value0==0) && (ADC_Read_Value32==0))
		 R855_TF_Check_Result = 1;  //fail
	 else
		 R855_TF_Check_Result = 0;

	 if(ADC_Read_Value0 > ADC_Read_Value32)
        R855_DetectTfType_Cal = R855_UL_USING_BEAD; //>60
	 else
	    R855_DetectTfType_Cal = R855_UL_USING_270NH; //<20


#if(R855_DEBUG==1)
	 R855_PRINT("R855_DetectTfType_Cal result = %d [0:R855_UL_USING_BEAD, 1:R855_UL_USING_270NH]\n", R855_DetectTfType_Cal);
#endif
	return R855_Success;
}

R855_ErrCode R855_Xtal_Check(void)  //CD OK
{
	UINT8 i = 0;
	//UINT8 lock_bank = 45;  //LO=474
	UINT8 lock_bank = 28;  //LO=800   //CD ??

	UINT8	save_bnak_oth_set=0; 
	//set pll autotune = 64kHz (fast)
	R855_I2C.RegAddr = 47;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFD) | 0x02;  //R47[1]=1
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//Set Ni, Si, DivNum    
	if (R855_Xtal ==16000)  //LO=474MHz, VCO=3792   //CD ?? 16MHz no match parameter
	{
		R855_Array[26] = (R855_Array[26] & 0x80) | 21;               //Ni=21
		R855_Array[35] = (R855_Array[35] & 0x3F) | (3<<6);           //Si=3
		R855_Array[35] = (R855_Array[35] & 0xC7) | 0x08;             //div 4
		R855_Array[27] = 0x00;                                       //SDM_L
		R855_Array[28] = 0x00;                                       //SDM_H
		R855_Array[32] = (R855_Array[32] & 0x27) | (2<<3) | (0<<6);  //gm=1; Xtal div1
		R855_Array[32] = (R855_Array[32] & 0xE1) | (3<<1) | (1<<3);  //gm=1.5;lowest 
		//R855_Array[30] = (R855_Array[30] & 0xDF) | 0x00;           //AGC ref clk (16) => share with VCO manual code, /2
	}
	else if (R855_Xtal ==24000) //LO=800MHz, VCO=3200
	{
		R855_Array[26] = (R855_Array[26] & 0x80) | 13;               //Ni=13
		R855_Array[29] = (R855_Array[29] & 0x3F) | (1<<6);           //Si=1
		R855_Array[29] = (R855_Array[29] & 0xE3) | 0x04;             //div 4
		R855_Array[27] = 0xAA;                                       //SDM_L
		R855_Array[28] = 0xAA;                                       //SDM_H
		R855_Array[32] = (R855_Array[32] & 0x27) | (0<<3) | (0<<6);  //gm=2; Xtal div1
		R855_Array[32] = (R855_Array[32] & 0xE1) | (3<<1) | (1<<3);  //gm=1.5; lowest
		//R855_Array[30] = (R855_Array[30] & 0xDF) | 0x20;           //AGC ref clk (24) => share with as VCO manual code, /2
	}
	else if (R855_Xtal ==27000) //LO=800MHz, VCO=3200
	{
		R855_Array[26] = (R855_Array[26] & 0x80) | 11;               //Ni=11
		R855_Array[29] = (R855_Array[29] & 0x3F) | (2<<6);           //Si=2
		R855_Array[29] = (R855_Array[29] & 0xE3) | 0x04;             //div 4
		R855_Array[27] = 0x5E;                                       //SDM_L
		R855_Array[28] = 0x42;                                       //SDM_H
		R855_Array[32] = (R855_Array[32] & 0x27) | (0<<3) | (0<<6);  //gm=2; Xtal div1
		R855_Array[32] = (R855_Array[32] & 0xE1) | (3<<1) | (1<<3);  //gm=1.5; lowest
		//R855_Array[30] = (R855_Array[30] & 0xDF) | 0x20;           //AGC ref clk (24) => share with as VCO manual code, /2
	}
	else  //no support 
	{
		return R855_Fail;
	}
	R855_I2C.RegAddr = 26;
	R855_I2C.Data = R855_Array[26];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = 29;
	R855_I2C.Data = R855_Array[29];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = 32;
	R855_I2C.Data = R855_Array[32];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = 27;
	R855_I2C.Data = R855_Array[27];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = 28;
	R855_I2C.Data = R855_Array[28];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//set cap = 40p
	R855_I2C.RegAddr = 33;
	R855_Array[33] = (R855_Array[33] & 0xD0) | 0x2F;  
	R855_I2C.Data = R855_Array[33];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//set manual initial reg = 1 000000; 
	R855_I2C.RegAddr = 30;
	save_bnak_oth_set = R855_Array[30];
	R855_Array[30] = (R855_Array[30] & 0x80) | 0x40;  //manual 
	R855_I2C.Data = R855_Array[30];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//set auto
	R855_I2C.RegAddr = 30;
	R855_Array[30] = (R855_Array[30] & 0xBF);
	R855_I2C.Data = R855_Array[30];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//Restore setting(co-use bank)
	R855_I2C.RegAddr = 30;
	R855_Array[30] = save_bnak_oth_set;
	R855_I2C.Data = R855_Array[30];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;


	for(i=0; i<R855_XTAL_CHECK_SIZE; i++) //0~6
	{
	    // from weak to strong
		if(i==0)  //gm=1.5, lowest
			R855_Array[32] = (R855_Array[32] & 0xE1) | (3<<1) | (1<<3);  
		else if(i==1)  //gm=2, lowest
			R855_Array[32] = (R855_Array[32] & 0xE1) | (3<<1) | (0<<3);  
		else if(i==2) //gm=1.5, low
			R855_Array[32] = (R855_Array[32] & 0xE1) | (2<<1) | (1<<3);  
		else if(i==3) //gm=2, low
			R855_Array[32] = (R855_Array[32] & 0xE1) | (2<<1) | (0<<3);  
		else if(i==4) //gm=1.5, high
			R855_Array[32] = (R855_Array[32] & 0xE1) | (1<<1) | (1<<3);  
		else if(i==5) //gm=2, high
			R855_Array[32] = (R855_Array[32] & 0xE1) | (1<<1) | (0<<3);  
		else if(i==6) //gm=1.5, highest
			R855_Array[32] = (R855_Array[32] & 0xE1) | (0<<1) | (1<<3);
		else  //gm=2, highest
			R855_Array[32] = (R855_Array[32] & 0xE1) | (0<<1) | (0<<3);			

		//set gm and xtal power
		R855_I2C.RegAddr = 32;		
		R855_I2C.Data = R855_Array[32];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_Delay_MS(R855_XTAL_CHK_DELAY * 1000);  //10ms for div1 /1, agc_ref /2	// by ITE

		R855_I2C_Len.RegAddr = 0x00;
		R855_I2C_Len.Len = 4;
		I2C_Read_Len(&R855_I2C_Len);

        if(((R855_I2C_Len.Data[1] & 0x80) == 0x80) && ((R855_I2C_Len.Data[3] & 0x3F) <= (lock_bank+6)) && ((R855_I2C_Len.Data[3] & 0x3F) >= (lock_bank-6))) 
		{
		    R855_Xtal_Pwr_tmp = i;
			break;
		}

	    if(i==(R855_XTAL_CHECK_SIZE-1))  //5
		{
	        R855_Xtal_Pwr_tmp = i;
		}
	}

#if(R855_DEBUG==1)
	R855_PRINT("defined lock_bank = %d, Xtal lock bank = %d\n", lock_bank, (R855_I2C_Len.Data[3] & 0x3F));
#endif

    return R855_Success;
}	

R855_ErrCode R855_Cal_Prepare(UINT8 u1CalFlag) //CD OK
{
	 UINT8   InitArrayCunt = 0;
	 UINT8 XtalCap, CapTot, Cap_x;
	
    //set Xtal cap
	if(R855_XTAL_CAP>30)
	{
		CapTot = R855_XTAL_CAP-10;
		XtalCap = 1;  //10
	}
	else
	{
		CapTot = R855_XTAL_CAP;
		XtalCap = 0;  //0
	}
	Cap_x = CapTot >> 1;

	 //Write Full Table, include PLL & RingPLL all settings
	 R855_I2C_Len.RegAddr = 0;
	 R855_I2C_Len.Len = R855_REG_NUM;

	 switch(u1CalFlag)
	 {
	    case R855_IMR_CAL:
				R855_IMR_CAL_Array[0][33]=(R855_IMR_CAL_Array[0][33] & 0xD0) |  (XtalCap<<5) | Cap_x;
				R855_IMR_CAL_Array[1][33]=(R855_IMR_CAL_Array[1][33] & 0xD0) |  (XtalCap<<5) | Cap_x;  //24M
				R855_IMR_CAL_Array[2][33]=(R855_IMR_CAL_Array[2][33] & 0xD0) |  (XtalCap<<5) | Cap_x;

				//gm=off, xtal power=highest
				if(R855_SHARE_XTAL==R855_SLAVE_XTAL_OUT)
				{
					R855_IMR_CAL_Array[0][32]=(R855_IMR_CAL_Array[0][32] & 0xE1) | 0x18;
					R855_IMR_CAL_Array[1][32]=(R855_IMR_CAL_Array[1][32] & 0xE1) | 0x18;
					R855_IMR_CAL_Array[2][32]=(R855_IMR_CAL_Array[2][32] & 0xE1) | 0x18;
				}
				else if(R855_SHARE_XTAL==R855_MASTER_TO_SLAVE_XTAL_OUT) //gm=2.0,xtal power=highest
				{
					R855_IMR_CAL_Array[0][32]=(R855_IMR_CAL_Array[0][32] & 0xE1) | 0x00;
					R855_IMR_CAL_Array[1][32]=(R855_IMR_CAL_Array[1][32] & 0xE1) | 0x00;
					R855_IMR_CAL_Array[2][32]=(R855_IMR_CAL_Array[2][32] & 0xE1) | 0x00;
				}

				if(R855_Xtal == 24000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_IMR_CAL_Array[0][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_IMR_CAL_Array[0][InitArrayCunt];
					}
				}
				else if(R855_Xtal == 16000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_IMR_CAL_Array[1][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_IMR_CAL_Array[1][InitArrayCunt];
					}	
				}
				else if(R855_Xtal == 27000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_IMR_CAL_Array[2][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_IMR_CAL_Array[2][InitArrayCunt];
					}
				}
				else
				{
				   //no support  now
					return R855_Fail;
				}

			break;

		case R855_IMR_LNA_CAL:						    

			break;

		case R855_TF_MID_LNA_CAL:

				R855_TF_MID_CAL_Array[0][33]=(R855_TF_MID_CAL_Array[0][33] & 0xD0) | (XtalCap<<5) | Cap_x;
				R855_TF_MID_CAL_Array[1][33]=(R855_TF_MID_CAL_Array[1][33] & 0xD0) | (XtalCap<<5) | Cap_x;  //24M
				R855_TF_MID_CAL_Array[2][33]=(R855_TF_MID_CAL_Array[2][33] & 0xD0) | (XtalCap<<5) | Cap_x;
                            

				//gm=off, xtal power=highest
				if(R855_SHARE_XTAL==R855_SLAVE_XTAL_OUT)
				{
					R855_TF_MID_CAL_Array[0][32]=(R855_TF_MID_CAL_Array[0][32] & 0xE1) | 0x18;
					R855_TF_MID_CAL_Array[1][32]=(R855_TF_MID_CAL_Array[1][32] & 0xE1) | 0x18;
					R855_TF_MID_CAL_Array[2][32]=(R855_TF_MID_CAL_Array[2][32] & 0xE1) | 0x18;
				}
				else if(R855_SHARE_XTAL==R855_MASTER_TO_SLAVE_XTAL_OUT) //gm=2.0,xtal power=highest
				{
					R855_TF_MID_CAL_Array[0][32]=(R855_TF_MID_CAL_Array[0][32] & 0xE1) | 0x00;
					R855_TF_MID_CAL_Array[1][32]=(R855_TF_MID_CAL_Array[1][32] & 0xE1) | 0x00;
					R855_TF_MID_CAL_Array[2][32]=(R855_TF_MID_CAL_Array[2][32] & 0xE1) | 0x00;
				}

				if(R855_Xtal == 24000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_TF_MID_CAL_Array[0][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_TF_MID_CAL_Array[0][InitArrayCunt];
					}
				}
				else if(R855_Xtal == 16000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_TF_MID_CAL_Array[1][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_TF_MID_CAL_Array[1][InitArrayCunt];
					}	
				}
				else if(R855_Xtal == 27000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_TF_MID_CAL_Array[2][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_TF_MID_CAL_Array[2][InitArrayCunt];
					}
				}
				else
				{
				   //no support  now
					return R855_Fail;
				}
			break;

        case R855_TF_LNA_CAL:

				R855_TF_CAL_Array[0][33]=(R855_TF_CAL_Array[0][33] & 0xD0) | (XtalCap<<5) | Cap_x;
				R855_TF_CAL_Array[1][33]=(R855_TF_CAL_Array[1][33] & 0xD0) | (XtalCap<<5) | Cap_x;  //24M
				R855_TF_CAL_Array[2][33]=(R855_TF_CAL_Array[2][33] & 0xD0) | (XtalCap<<5) | Cap_x;
				//gm=off, xtal power=highest
				if(R855_SHARE_XTAL==R855_SLAVE_XTAL_OUT)
				{
					R855_TF_CAL_Array[0][32]=(R855_TF_CAL_Array[0][32] & 0xE1) | 0x18;
					R855_TF_CAL_Array[1][32]=(R855_TF_CAL_Array[1][32] & 0xE1) | 0x18;
					R855_TF_CAL_Array[2][32]=(R855_TF_CAL_Array[2][32] & 0xE1) | 0x18;
				}
				else if(R855_SHARE_XTAL==R855_MASTER_TO_SLAVE_XTAL_OUT) //gm=2.0,xtal power=highest
				{
					R855_TF_CAL_Array[0][32]=(R855_TF_CAL_Array[0][32] & 0xE1) | 0x00;
					R855_TF_CAL_Array[1][32]=(R855_TF_CAL_Array[1][32] & 0xE1) | 0x00;
					R855_TF_CAL_Array[2][32]=(R855_TF_CAL_Array[2][32] & 0xE1) | 0x00;
				}

				if(R855_Xtal == 24000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_TF_CAL_Array[0][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_TF_CAL_Array[0][InitArrayCunt];
					}
				}
				else if(R855_Xtal == 16000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_TF_CAL_Array[1][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_TF_CAL_Array[1][InitArrayCunt];
					}	
				}
				else if(R855_Xtal == 27000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_TF_CAL_Array[2][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_TF_CAL_Array[2][InitArrayCunt];
					}	
				}
				else
				{
				   //no support  now
					return R855_Fail;
				}
			break;

		case R855_LPF_CAL: 

				R855_LPF_CAL_Array[0][33]=(R855_LPF_CAL_Array[0][33] & 0xD0) | (XtalCap<<5) | Cap_x;
				R855_LPF_CAL_Array[1][33]=(R855_LPF_CAL_Array[1][33] & 0xD0) | (XtalCap<<5) | Cap_x;  //24M
				R855_LPF_CAL_Array[2][33]=(R855_LPF_CAL_Array[2][33] & 0xD0) | (XtalCap<<5) | Cap_x;

				//gm=off, xtal power=highest
				if(R855_SHARE_XTAL==R855_SLAVE_XTAL_OUT)
				{
					R855_LPF_CAL_Array[0][32]=(R855_LPF_CAL_Array[0][32] & 0xE1) | 0x18;
					R855_LPF_CAL_Array[1][32]=(R855_LPF_CAL_Array[1][32] & 0xE1) | 0x18;
					R855_LPF_CAL_Array[2][32]=(R855_LPF_CAL_Array[2][32] & 0xE1) | 0x18;
				}
				else if(R855_SHARE_XTAL==R855_MASTER_TO_SLAVE_XTAL_OUT) //gm=2.0,xtal power=highest
				{
					R855_LPF_CAL_Array[0][32]=(R855_LPF_CAL_Array[0][32] & 0xE1) | 0x00;
					R855_LPF_CAL_Array[1][32]=(R855_LPF_CAL_Array[1][32] & 0xE1) | 0x00;
					R855_LPF_CAL_Array[2][32]=(R855_LPF_CAL_Array[2][32] & 0xE1) | 0x00;
				}

				if(R855_Xtal == 24000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_LPF_CAL_Array[0][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_LPF_CAL_Array[0][InitArrayCunt];
					}
				}
				else if(R855_Xtal == 16000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_LPF_CAL_Array[1][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_LPF_CAL_Array[1][InitArrayCunt];
					}	
				}
				else if(R855_Xtal == 27000)
				{
					for(InitArrayCunt = 8; InitArrayCunt<R855_REG_NUM; InitArrayCunt ++)
					{
						R855_I2C_Len.Data[InitArrayCunt] = R855_LPF_CAL_Array[2][InitArrayCunt];
						R855_Array[InitArrayCunt] = R855_LPF_CAL_Array[2][InitArrayCunt];
					}
				}
				else
				{
				   //no support 27MHz now
					return R855_Fail;
				}
			break;		

		default:

			break;

	 }

	 if(I2C_Write_Len(&R855_I2C_Len) != R855_Success)
		 return R855_Fail;


	//set pll autotune = 64kHz (fast)
	R855_I2C.RegAddr = 47;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFD) | 0x02;  //R47[1]=1
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
	
	
      return R855_Success;
}




R855_ErrCode R855_IMR(UINT8 IMR_MEM, UINT8 IM_Flag, UINT8 Rev_Mode, UINT8 Mix_Gain_Mode) //CD OK
{
	UINT32 RingVCO = 0;
	UINT32 RingFreq = 0;
	UINT32 RingRef = R855_Xtal;
	UINT8  divnum_ring = 0;

	UINT8  IMR_Gain = 0;
	UINT8  IMR_Phase = 0;

	UINT8  LPF_Count = 0;
	UINT8  ADC_Read_Value = 0;

	UINT8 IMR_HW_done_flag = 0;
	UINT8 IMR_counter = 0;
	UINT8 IMR_auto_result = 0;
	UINT8 HW_ReCal_num = 0;
	
	UINT8	HWCal_init_loca = 0x00;

	R855_Sect_Type IMR_POINT;

	R855_Sect_Type Compare_Bet[3];

#if(R855_DEBUG==1)
	R855_PRINT("IMR parameter IMR_MEM=%d, IM_Flag=%d, Rev_Mode=%d, Mix_Gain_Mode=%d\n",IMR_MEM, IM_Flag, Rev_Mode, Mix_Gain_Mode);
#endif


	if(R855_Xtal==16000)  //16M	
         divnum_ring = 25;                    //3200/8/16.  32>divnum>7
	else if(R855_Xtal==24000) //24M
		 divnum_ring = 17;
	else if(R855_Xtal==27000) //27M
		 divnum_ring = 15;
	else //not support
		 return R855_Fail;
	
	 //RingVCO = (divnum_ring)* 8 * RingRef;  //VCO=3264/3200/3240 for 24M/16M/27M
	 RingVCO = (divnum_ring)* 4 * RingRef;  //VCO=1632 for 24M
	 RingFreq = RingVCO/24;


	 R855_IMR_IF = 5300;

	 R855_IMR_MEM_GLOBAL = IMR_MEM;

	switch(IMR_MEM)
	{
	case 0: // RingFreq = 136.0M
		//RingFreq = RingVCO/24;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | (2<<5);  // ring_div1 /6 (2)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | (2<<0);  // ring_div2 /4 (2)	
		RingFreq = RingVCO/12;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (0<<7) | (2<<5));  // mod23 = /3   R34[7]=0  ,     ring_div[/3, /6, /12, /24] R34[6:5]=2
		break;
	case 1: // RingFreq = 326.4M
		//RingFreq = RingVCO/10;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | (1<<5);  // ring_div1 /5 (1)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | (1<<0);  // ring_div2 /2 (1)

         //RingFreq = 320MHz
         if(R855_Xtal==24000) //24M
         {
			divnum_ring = 20;
            RingVCO = (divnum_ring)* 4 * RingRef;  //VCO=1632 for 24M
        }
		RingFreq = RingVCO/6;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (0<<7) | (1<<5));  // mod23 = /3   R34[7]=0  ,     ring_div[/3, /6, /12, /24] R34[6:5] =1
		break;
	case 2: // RingFreq = 544M
		//RingFreq = RingVCO/6;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | (2<<5);  // ring_div1 /6 (2)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | (0);       // ring_div2 /1 (0)		
		RingFreq = RingVCO/3;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (0<<7) | (0<<5));  // mod23 = /3   R34[7]=0  ,     ring_div[/3, /6, /12, /24] R34[6:5]
		break;
	case 3: // RingFreq = 816M
		//RingFreq = RingVCO/4;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | 0;          // ring_div1 /4 (0)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | 0;          // ring_div2 /1 (0)		
		RingFreq = RingVCO/2;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (1<<7) | (0<<5));  // mod23 = /2   R34[7]=1  ,     ring_div[/2, /4, /8, /16] R34[6:5]
		break;
	case 4: // RingFreq = 136.0M
		//RingFreq = RingVCO/24;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | (2<<5);  // ring_div1 /6 (2)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | (2<<0);  // ring_div2 /4 (2)		
		RingFreq = RingVCO/12;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (0<<7) | (2<<5));  // mod23 = /3   R34[7]=0  ,     ring_div[/3, /6, /12, /24] R34[6:5]=2
		break;
	case 5: // RingFreq = 326.4M
		//RingFreq = RingVCO/10;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | (1<<5);  // ring_div1 /5 (1)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | (1<<0);  // ring_div2 /2 (1)		
		//RingFreq = 320MHz
         if(R855_Xtal==24000) //24M
         {
			divnum_ring = 20;
            RingVCO = (divnum_ring)* 4 * RingRef;  //VCO=1632 for 24M
        }
		RingFreq = RingVCO/6;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (0<<7) | (1<<5));  // mod23 = /3   R34[7]=0  ,     ring_div[/3, /6, /12, /24] R34[6:5] =1
		break;
	case 6: // RingFreq = 544M
		//RingFreq = RingVCO/6;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | (2<<5);  // ring_div1 /6 (2)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | (0);       // ring_div2 /1 (0)		
		RingFreq = RingVCO/3;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (0<<7) | (0<<5));  // mod23 = /3   R34[7]=0  ,     ring_div[/3, /6, /12, /24] R34[6:5]
		break;
	case 7: // RingFreq = 816M
		//RingFreq = RingVCO/4;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | 0;          // ring_div1 /4 (0)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | 0;          // ring_div2 /1 (0)	
		RingFreq = RingVCO/2;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (1<<7) | (0<<5));  // mod23 = /2   R34[7]=1  ,     ring_div[/2, /4, /8, /16] R34[6:5]
		break;
	default:
		//RingFreq = RingVCO/6;
		//R855_Array[34] = (R855_Array[34] & 0x9F) | (2<<5);  // ring_div1 /6 (2)
		//R855_Array[35] = (R855_Array[35] & 0xFC) | (0);       // ring_div2 /1 (0)		
		RingFreq = RingVCO/3;
		R855_Array[34] = ((R855_Array[34] & 0x1F) | (0<<7) | (0<<5));  // mod23 = /3   R34[7]=0  ,     ring_div[/3, /6, /12, /24] R34[6:5]	
		break;
	}
#if(R855_DEBUG==1)
	R855_PRINT("IMR RingFreq = %d KHz \n", RingFreq);
#endif

	//write RingPLL setting, R34
	R855_Array[34] = (R855_Array[34] & 0xE0) | divnum_ring;   //ring_div_num, R34[4:0]
/* CD ??
	if(RingVCO>=3200000)
	    R855_Array[34] = (R855_Array[34] & 0x7F);   //vco_band=high, R34[7]=0
	 else
        R855_Array[34] = (R855_Array[34] | 0x80);      //vco_band=low, R34[7]=1
*/

	//write RingPLL setting, R34
	R855_I2C.RegAddr = 34;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
	

	if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
	{
		R855_I2C.RegAddr = 18;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr]&0xE1)|(8<<1);
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
	}
	else   //R855_IMR_MIX_GAIN_L7, mixer gain = 6
	{
		R855_I2C.RegAddr = 18;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr]&0xE1)|(6<<1);
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
	}

	if(Rev_Mode==R855_IMR_NOR)  //normal
	{
		if(R855_MUX(RingFreq - R855_IMR_IF, RingFreq, R855_STD_SIZE) != R855_Success)      //IMR MUX (LO, RF)
			return R855_Fail;

		if(R855_PLL((RingFreq - R855_IMR_IF), (UINT16)R855_IMR_IF, R855_STD_SIZE) != R855_Success)  //IMR PLL
			return R855_Fail;

		 //Img_R = normal
		 R855_I2C.RegAddr = 20;
		 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F);  //R20[7]=0
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		 if(I2C_Write(&R855_I2C) != R855_Success)
			 return R855_Fail;

		// Mixer Amp LPF=1 (0 is widest, 7 is narrowest)
		R855_I2C.RegAddr = 35;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE3) | (1<<2); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 	 //Set TF, place after R855_MUX( )
		 //TF is dependent to LNA/Mixer Gain setting
		if(R855_SetTF(RingFreq, (UINT8)R855_SetTfType_UL_MID) != R855_Success)
			return R855_Fail;

		//clear IQ_cap
		IMR_POINT.Iqcap = 0;
	}
	else //Reverse
	{
		if(R855_MUX(RingFreq + R855_IMR_IF, RingFreq, R855_STD_SIZE) != R855_Success)      //IMR MUX (LO, RF)
			return R855_Fail;

		if(R855_PLL((RingFreq + R855_IMR_IF), (UINT16)R855_IMR_IF, R855_STD_SIZE) != R855_Success)  //IMR PLL
			return R855_Fail;

		 //Img_R = reverse
		 R855_I2C.RegAddr = 20;
		 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] | 0x80);  //R20[7]=1
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		 if(I2C_Write(&R855_I2C) != R855_Success)
			 return R855_Fail;

		// Mixer Amp LPF=1 (0 is widest, 7 is narrowest)
		R855_I2C.RegAddr = 35;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE3) | (1<<2); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		if(R855_SetTF(RingFreq, (UINT8)R855_SetTfType_UL_MID) != R855_Success)
			return R855_Fail;

		//clear IQ_cap
		IMR_POINT.Iqcap = 0;
	}


	if(R855_IMR_CAL_MANUAL == 0)
	{
		if(IM_Flag == TRUE) //Full K by SW
		{
			R855_IMR_TYPE = R855_IMR_MANUAL_SW;
		}
		else
		{
			R855_IMR_TYPE = R855_IMR_AUTO_HW;
		}
	}



	if(R855_IMR_TYPE == R855_IMR_AUTO_HW) //HW Calibration
	{
		//------- increase Filter gain to let ADC read value significant ---------//
		 for(LPF_Count=5; LPF_Count < 16; LPF_Count=LPF_Count+2)  //start from 5
		 {
			R855_I2C.RegAddr = 25;
			R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x0F) | (LPF_Count<<4);
			R855_I2C.Data = R855_Array[R855_I2C.RegAddr];  
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;

			R855_Delay_MS(R855_FILTER_GAIN_DELAY * 1000); 	// by ITE
			
			if(R855_Muti_Read(&ADC_Read_Value) != R855_Success)
				return R855_Fail;

			if(ADC_Read_Value > 40*R855_ADC_READ_COUNT)
				break;
		 }

		//image auto R18[5]=1   [0:i2c, 1:auto]
		R855_I2C.RegAddr = 18;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xDF) | (1<<5); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
	
		//clk en R23[7]=1  [0:off, 1:on]
		R855_I2C.RegAddr = 23;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | (1<<7); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		if(IM_Flag == TRUE) //Full K
		{
			//DSP mode R24[1]=0 [0:normal, 1:1 round]
			R855_I2C.RegAddr = 24;
			R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFD) | (0<<1); 
			R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;
		}
		else
		{
			//DSP mode R24[1]=0 [0:normal, 1:1 round]
			R855_I2C.RegAddr = 24;
			R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFD) | (1<<1); 
			R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;

		}

		//tpp time R24[4:2] = 7 [0:0.25ms, 1:0.5ms, 2:1ms, 3: 1.5ms, 4:2ms, 5:2.5ms, 6:3ms, 7:3.5ms]
		R855_I2C.RegAddr = 24;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE3) | (7<<2);
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		//ts time R24[6:5] = 3 [0:0.5ms, 1:1ms, 2:2ms, 3:3ms]
		R855_I2C.RegAddr = 24;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x9F) | (3<<5); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		if(Rev_Mode==R855_IMR_NOR)  //normal 2(true)->1(false)->0(false)->3(false)
		{
			if(IM_Flag == FALSE) //DSP 1 round
			{
				if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
				{
					if((IMR_MEM==3)||(IMR_MEM==0))
					{
						HWCal_init_loca = (R855_IMR_Data_G7_MIXAMP[1].Gain_X & 0x0F) | ((R855_IMR_Data_G7_MIXAMP[1].Phase_Y & 0x0F)<<4);
					}
				}
				else   //R855_IMR_MIX_GAIN_L7, mixer gain = 6
				{
					//L7 reference to G7
					HWCal_init_loca = (R855_IMR_Data_G7_MIXAMP[IMR_MEM].Gain_X & 0x0F) | ((R855_IMR_Data_G7_MIXAMP[IMR_MEM].Phase_Y & 0x0F)<<4);
				}
				R855_I2C.RegAddr = 42;
				R855_Array[R855_I2C.RegAddr] = HWCal_init_loca; 
				R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
				if(I2C_Write(&R855_I2C) != R855_Success)
					return R855_Fail;
			}
			else
			{
				HWCal_init_loca = 0x00;
				R855_I2C.RegAddr = 42;
				R855_Array[R855_I2C.RegAddr] = HWCal_init_loca; 
				R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
				if(I2C_Write(&R855_I2C) != R855_Success)
					return R855_Fail;
			}
		}
		else //Reverse 6(true)->5(false)->4(false)->7(true)
		{
			if(IM_Flag == FALSE) //DSP 1 round
			{
				if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
				{
					if((IMR_MEM==4) || IMR_MEM==7)
					{
						HWCal_init_loca = (R855_IMR_Data_G7_MIXAMP[5].Gain_X & 0x0F) | (R855_IMR_Data_G7_MIXAMP[5].Phase_Y & 0xF0);
					}
				}
				else   //R855_IMR_MIX_GAIN_L7, mixer gain = 6
				{
					//L7 reference to G7
					HWCal_init_loca = (R855_IMR_Data_G7_MIXAMP[IMR_MEM].Gain_X & 0x0F) | ((R855_IMR_Data_G7_MIXAMP[IMR_MEM].Phase_Y & 0x0F)<<4);

				}
				
				R855_I2C.RegAddr = 42;
				R855_Array[R855_I2C.RegAddr] = HWCal_init_loca; 
				R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
				if(I2C_Write(&R855_I2C) != R855_Success)
					return R855_Fail;
			}
			else
			{
				HWCal_init_loca = 0x00;
				R855_I2C.RegAddr = 42;
				R855_Array[R855_I2C.RegAddr] = HWCal_init_loca; 
				R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
				if(I2C_Write(&R855_I2C) != R855_Success)
					return R855_Fail;
			}
		}
		


		do
		{
			if(HW_ReCal_num == 1) //initial value is 0
			{
				//HW Re-Calibration and result is initial location
				//initial Gain R42[3:0]  Phase R42[7:4] 
				R855_I2C.RegAddr = 42;
				R855_Array[R855_I2C.RegAddr] = IMR_auto_result; 
				R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
				if(I2C_Write(&R855_I2C) != R855_Success)
					return R855_Fail;
			}

			//dsp unable R24[0]=0 [0:unable, 1:enable]
			R855_I2C.RegAddr = 24;
			R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE); 
			R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
			if(I2C_Write(&R855_I2C) != R855_Success)	
				return R855_Fail;

			R855_Delay_MS(10 * 1000); //need optimize	// by ITE

			//dsp enable R24[0]=1 [0:unable, 1:enable]
			R855_I2C.RegAddr = 24;
			R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE) | (1); 
			R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;
			
			R855_Delay_MS(R855_HW_CAL_DELAY * 1000); //need optimize	// by ITE

			R855_I2C_Len.RegAddr = 0;
			R855_I2C_Len.Len = 3;              // read R2 ymin and xmin	
			if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
			{
				I2C_Read_Len(&R855_I2C_Len);
			}

			IMR_HW_done_flag = ((R855_I2C_Len.Data[1] & 0x40)>>6) ;

			for(IMR_counter =0; IMR_counter<30; IMR_counter++)
			{
				if(IMR_HW_done_flag == 0x01)
				{	
					break;
				}
				else
				{
					R855_I2C_Len.RegAddr = 0;
					R855_I2C_Len.Len = 3;              // read R2 ymin and xmin	
					R855_Delay_MS(1 * 1000);	// by ITE
					if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
					{
						I2C_Read_Len(&R855_I2C_Len);
					}
					IMR_HW_done_flag = ((R855_I2C_Len.Data[1] & 0x40)>>6);
				}
			}

			R855_I2C_Len.RegAddr = 0;
			R855_I2C_Len.Len = 8;              // read R2 ymin and xmin	and min value
			R855_Delay_MS(1 * 1000);	// by ITE
			if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
			{
				I2C_Read_Len(&R855_I2C_Len);
			}
			IMR_auto_result = R855_I2C_Len.Data[2];
			IMR_POINT.Value = ((R855_I2C_Len.Data[7] & 0x3F));
			
			if((((IMR_auto_result & 0x07)>=3) || (((IMR_auto_result >> 4) & 0x07)>=3)) && (IM_Flag == TRUE))
			{
				HW_ReCal_num += 1; //Re-Calibration if HW_ReCal_num =1
			}

		}while(HW_ReCal_num == 1);  //Re-Calibration

		//clear initial value 
		HW_ReCal_num = 0;

		R855_I2C.RegAddr = 42;
		R855_Array[R855_I2C.RegAddr] = 0x00;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		//dsp unable R24[0]=0 [0:unable, 1:enable]
		R855_I2C.RegAddr = 24;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE) | (0); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
		{
			IMR_POINT.Gain_X  =0x00;
			IMR_POINT.Phase_Y = 0x00 ;
			IMR_POINT.Iqcap = 0;
			IMR_POINT.Gain_X  |= (IMR_auto_result & 0x0F);    //R20[3:0]
			IMR_POINT.Phase_Y |= ((IMR_auto_result & 0xF0) >> 4); //R21[3:0]
		}
		else //R855_IMR_MIX_GAIN_L7, mixer gain = 6
		{
			IMR_POINT.Gain_X  =0x00;
			IMR_POINT.Phase_Y = 0x00;
			IMR_POINT.Iqcap = 0;
			IMR_POINT.Gain_X  |= (IMR_auto_result & 0x0F);   //R42[3:0]
			IMR_POINT.Phase_Y |= (IMR_auto_result & 0xF0);  //R42[7:4]
		}

		//image auto R18[5]=0   [0:i2c, 1:auto]
		R855_I2C.RegAddr = 18;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xDF) | (0<<5); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;


		if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
		{
			Compare_Bet[0].Gain_X = IMR_POINT.Gain_X;
			Compare_Bet[0].Phase_Y =IMR_POINT.Phase_Y;

			Compare_Bet[0].Iqcap = 0;
			Compare_Bet[0].Value = IMR_POINT.Value;

			if(R855_IMR_Iqcap(&Compare_Bet[0]) != R855_Success)
				return R855_Fail;

			IMR_POINT = Compare_Bet[0];
		}
	}
	else //SW calibration
	{
		//Must do MUX before PLL()
		if(Rev_Mode==R855_IMR_NOR)  //normal
		{
			if(IM_Flag == TRUE)
			{
				if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
				{
					 if(R855_IQ_G7_MIXAMP(&IMR_POINT) != R855_Success)
						return R855_Fail;
				}
				else   //R855_IMR_MIX_GAIN_L7, mixer gain = 6
				{
					 if(R855_IQ_L7_MIXAMP(&IMR_POINT) != R855_Success)
						return R855_Fail;
				}
			}
			else  //IMR_MEM 1, 3
			{
				if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
				{
					if((IMR_MEM==0) || (IMR_MEM==3))
					{
						IMR_POINT.Gain_X = R855_IMR_Data_G7_MIXAMP[1].Gain_X;  
						IMR_POINT.Phase_Y = R855_IMR_Data_G7_MIXAMP[1].Phase_Y;
						IMR_POINT.Value = R855_IMR_Data_G7_MIXAMP[1].Value;
					}
					if(R855_F_IMR_G7_MIXAMP(&IMR_POINT) != R855_Success)
						return R855_Fail;
				}
				else   //R855_IMR_MIX_GAIN_L7, mixer gain = 6
				{
					//L7 Reference to G7
					IMR_POINT.Gain_X = R855_IMR_Data_G7_MIXAMP[IMR_MEM].Gain_X;   //node 3
					IMR_POINT.Phase_Y = R855_IMR_Data_G7_MIXAMP[IMR_MEM].Phase_Y;
					IMR_POINT.Value = R855_IMR_Data_G7_MIXAMP[IMR_MEM].Value;

					if(R855_F_IMR_L7_MIXAMP(&IMR_POINT) != R855_Success)
						return R855_Fail;
				}
				
			}
		}
		else  //Reverse Mode
		{

			if(IM_Flag == TRUE)
			{
				if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
				{
					 if(R855_IQ_G7_MIXAMP(&IMR_POINT) != R855_Success)
						return R855_Fail;
				}
				else   //R855_IMR_MIX_GAIN_L7, mixer gain = 6
				{
					 if(R855_IQ_L7_MIXAMP(&IMR_POINT) != R855_Success)
						return R855_Fail;
				}
			}
			else  //IMR_MEM 5, 4, 7
			{
				if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
				{
					if(IMR_MEM==4 || IMR_MEM==7)
					{
						IMR_POINT.Gain_X = R855_IMR_Data_G7_MIXAMP[5].Gain_X;    
						IMR_POINT.Phase_Y = R855_IMR_Data_G7_MIXAMP[5].Phase_Y;
						IMR_POINT.Value = R855_IMR_Data_G7_MIXAMP[5].Value;
					}

					if(R855_F_IMR_G7_MIXAMP(&IMR_POINT) != R855_Success)
						return R855_Fail;
				}
				else  //R855_IMR_MIX_GAIN_L7, mixer gain = 6
				{
					IMR_POINT.Gain_X = R855_IMR_Data_G7_MIXAMP[IMR_MEM].Gain_X;
					IMR_POINT.Phase_Y = R855_IMR_Data_G7_MIXAMP[IMR_MEM].Phase_Y;
					IMR_POINT.Value = R855_IMR_Data_G7_MIXAMP[IMR_MEM].Value;
					if(R855_F_IMR_L7_MIXAMP(&IMR_POINT) != R855_Success)
						return R855_Fail;
				}
			}
		} //end if(Rev_Mode==R855_IMR_NOR) //normal
	} //end if(R855_IMR_TYPE == R855_IMR_AUTO)

#if(R855_DEBUG==1)
	R855_PRINT("IMR_POINT.Gain_X = 0x%02X\n", IMR_POINT.Gain_X);
	R855_PRINT("IMR_POINT.Phase_Y = 0x%02X\n", IMR_POINT.Phase_Y);
	R855_PRINT("IMR_POINT.Value = %d\n", IMR_POINT.Value);
	R855_PRINT("IMR_POINT.Iqcap = %d\n", IMR_POINT.Iqcap);
#endif

	if(Mix_Gain_Mode==R855_IMR_MIX_GAIN_G7) //mixer gain = 8
	{
		R855_IMR_Data_G7_MIXAMP[IMR_MEM].Gain_X  = IMR_POINT.Gain_X;
		R855_IMR_Data_G7_MIXAMP[IMR_MEM].Phase_Y = IMR_POINT.Phase_Y;
		R855_IMR_Data_G7_MIXAMP[IMR_MEM].Value = IMR_POINT.Value;
		R855_IMR_Data_G7_MIXAMP[IMR_MEM].Iqcap = IMR_POINT.Iqcap;	

		IMR_Gain = R855_IMR_Data_G7_MIXAMP[IMR_MEM].Gain_X & 0x0F;   //R20[4:0]
		IMR_Phase = R855_IMR_Data_G7_MIXAMP[IMR_MEM].Phase_Y & 0x0F; //R21[4:0]

		if((IM_Flag == TRUE) && (((IMR_Gain & 0x07)>6) || ((IMR_Phase & 0x07)>6))) //Full K
		{
			if((R855_IMR_Data_G7_MIXAMP[IMR_MEM].Value+15) >= (R855_IMR_Q0_Value[IMR_MEM]))
			{
				R855_IMR_Cal_Result = 1; //fail
			}

		}
	}
	else //R855_IMR_MIX_GAIN_L7, mixer gain = 6
	{
		R855_IMR_Data_L7_MIXAMP[IMR_MEM].Gain_X  = IMR_POINT.Gain_X;
		R855_IMR_Data_L7_MIXAMP[IMR_MEM].Phase_Y = IMR_POINT.Phase_Y;
		R855_IMR_Data_L7_MIXAMP[IMR_MEM].Value = IMR_POINT.Value;
		R855_IMR_Data_L7_MIXAMP[IMR_MEM].Iqcap = IMR_POINT.Iqcap;	

		IMR_Gain = R855_IMR_Data_L7_MIXAMP[IMR_MEM].Gain_X & 0x0F;   //R20[4:0]
		IMR_Phase = R855_IMR_Data_L7_MIXAMP[IMR_MEM].Phase_Y & 0xF0; //R21[4:0]
		if((IM_Flag == TRUE) && (((IMR_Gain & 0x07)>6) || ((IMR_Phase & 0x07)>6))) //Full K
		{
			if((R855_IMR_Data_L7_MIXAMP[IMR_MEM].Value+15) >= (R855_IMR_Q0_Value[IMR_MEM]))
			{
				R855_IMR_Cal_Result = 1; //fail
			}
		}
	}

	return R855_Success;
}


R855_ErrCode R855_PLL(UINT32 LO_Freq, UINT16 IF_Freq, R855_Standard_Type R855_Standard)//CD OK
{
	UINT8   MixDiv = 2;
	UINT8   DivBuf = 0;
	UINT8   Ni = 0;
	UINT8   Si = 0;
	UINT8   DivNum = 0;
	UINT16  Nint = 0;
	UINT32  VCO_Min = 2270000;   //2270 in MT2
	UINT32  VCO_Max = VCO_Min*2;
	UINT32  VCO_Freq = 0;
	UINT32  PLL_Ref	= R855_Xtal;		
	UINT32  VCO_Fra	= 0;		
	UINT16  Nsdm = 2;
	UINT16  SDM = 0;
	UINT16  SDM16to9 = 0;
	UINT16  SDM8to1 = 0;
	UINT8   XTAL_POW = 0;     //highest
	UINT8   XTAL_GM = 0;      //gm=2
	UINT16  u2XalDivJudge;
	UINT8   u1XtalDivRemain;
	UINT8   SDM_RES = 0x00;
	UINT8   IQGen_Cur = 0;    //DminDmin
	UINT8   IQBias = 1;       //BiasI
	UINT8   IQLoad = 0;       //3.2k/2
	UINT8   OutBuf_Bias = 0;  //max
	UINT8   BiasHf = 0;       //135u 
	UINT8	CP_Current = 0;
	UINT8   CP_Offset = 0;  
	UINT8   XtalDivQ = 0;
	UINT8   ClkOut = 0;
	UINT8	CP_ix2=0;
	UINT8   XtalPwrTemp=0;
	UINT32  Spur_pos;
	UINT32  Spur_neg;
	UINT32  RF_Freq = LO_Freq + IF_Freq;

	UINT8   vco_bank_read = 0;
	UINT8   vco_bank_lock = 0;
	UINT8   BoundDiv = 1;
	UINT8   CentBoundDiv = 1;

	UINT8	save_bnak_oth_set=0; 

	/*
	Spur_24mhz_1_op
	bit[7:6], Xtal PW  [0:highest, 1:high, 2:low, 3:lowest],
	bit[5:4], Xtal GM  [0:2*gm (24MHz), 1:gm (20MHz), 2:gm (16MHz), 3:off]
	bit[3], none
	bit[2:0], cp current[0:0.7mA, 1:0.6mA, 2:0.5mA, 3:0.4mA, 4:0.3mA, 5:0.2mA, 6:0.1mA, 7:Auto]
	*/

	UINT8   Spur_24mhz_1_op_dtv[42] = {             
		0x1,	0x1,	0x57,	0x93,	0x87,	0x97,	0x80,	0x7,	0x7,	0x57,
	//					48,     72,     96,     120,    144,    168,    192,    216,   
		0x53,	0x7,	0x87,	0x7,	0x47,	0x7,	0x7,	0x7,	0x7,	0x92,
	//	 240,    264,	288,	312,	336,	360,	384,	408,	432,	456,	
		0x7,	0x47,	0x92,	0x0,	0x40,	0x7,	0x43,	0x7,	0x43,	0x3,
	//	480,	504,	528,	552,	576,	600,	624,	648,	672,	696,	
		0x7,	0x3,	0x7,	0x7,	0x7,	0x7,	0x7,	0x7,	0x7,	0x7,
	//	720,	744,	768,	792,	816,	840,	864,	888,	912,	936,	
		0x7,	0x7};
	//	960,	984



	/*
	Spur_24mhz_2_op
	bit[7:5], none
	bit[4], xtal/2(PLL_AGC) [0:/1, 1:/2]
	bit[3], xtal/2(PLL) [0:/1, 1:/2]
	bit[2], CP offset [0:no, 1:30uA]
	bit[1], cp_ix2 [0-cp_I x1, 1:cp_I x2]
	bit[0], clkout [0:enable, 1:disable]
	*/

	UINT8   Spur_24mhz_2_op_dtv[42] = {             
		0x1,	0x1,	0x19,	0x16,	0x9,	0xD,	0xB,	0x1,	0x1,	0x9,
	//					48,     72,     96,     120,    144,    168,    192,    216,   
		0xD,	0x1,	0x8,	0x1,	0xA,	0x1,	0x9,	0x1,	0x9,	0xF,
	//	 240,    264,	288,	312,	336,	360,	384,	408,	432,	456,
		0x9,	0xC,	0x5,	0x2,	0x0,	0x9,	0x8,	0x9,	0x1,	0x9,
	//	480,	504,	528,	552,	576,	600,	624,	648,	672,	696,
		0x1,	0x9,	0x1,	0x9,	0x1,	0x1,	0x1,	0x1,	0x1,	0x1,
	//	720,	744,	768,	792,	816,	840,	864,	888,	912,	936,	
		0x1,	0x1};
	//	960,	984

	UINT8 Spur_apply_1_op;
	UINT8 Spur_apply_2_op;
	UINT8 Xtal_Div2_AGC_PLL; //[0:ref/1, 1:ref/2]
	UINT8 Xtal_Div2_PLL; //[0:ref/1, 1:ref/2]

	VCO_Min = 2270000; 
	VCO_Max = VCO_Min*2;


	//set pll autotune = 64kHz (fast)
	R855_I2C.RegAddr = 47;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFD) | 0x02;  //R47[1]=1
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	// CP current: 0.7 (R31[4:2]=000, largest BW for analog circuit); VCO current=0 (R31[1:0]=00, max) ; HfDiv Buf = 6dB buffer (R31[7]=0)
	R855_I2C.RegAddr = 31;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x60) | 0x00; 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	// VCO power = auto
	R855_I2C.RegAddr = 26;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | 0x80; // R26[7]=1
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	// PFD DLDO=4mA
	R855_I2C.RegAddr = 10;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFC) | 0x00; // R10[1:0]=00
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

       //PLL LDOA=2.2V(R8[5:4]=00)
	R855_I2C.RegAddr = 8;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xCF) | 0x00; 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	// DLDO2=3mA(R9[1:0]=01); 
	R855_I2C.RegAddr = 9;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFC) | 0x01; 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	// HF Fiv LDO=7mA (new bonding set this off)
	R855_I2C.RegAddr = 10;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF3) | 0x00; // R10[3:2]=00
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	//Divider
	while(MixDiv <= 64)
	{
		if(((LO_Freq * MixDiv) >= VCO_Min) && ((LO_Freq * MixDiv) < VCO_Max))
		{
			DivBuf = MixDiv;
			while(DivBuf > 2)
			{
				DivBuf = DivBuf >> 1;
				DivNum ++;
			}			
			break;
		}
		MixDiv = MixDiv << 1;
	}

	//IQ Gen block & BiasHF & NS_RES & SDM_Res
	if(MixDiv <= 4)  //Div=2,4
	{
		IQGen_Cur = 0;    //DminDmin
		IQBias = 1;       //BiasI   
		IQLoad = 0;       //3.2k/2 
		OutBuf_Bias = 0;  //0 (max)     
		BiasHf = 0;       //135u
		//SDM_RES = 0;      //short  CD??
		//NS_RES = 0;       //0R  CD??
	}
	else if(MixDiv == 8) 
	{
		IQGen_Cur = 0;    //DminDmin
		IQBias = 0;       //BiasI/2   
		IQLoad = 1;       //3.2k
		OutBuf_Bias = 1;  //1 
		BiasHf = 1;       //110u
		//SDM_RES = 0;      //short  CD??
		//NS_RES = 1;       //800R CD??
	}
	else if(MixDiv == 16) 
	{
		IQGen_Cur = 0;    //DminDmin
		IQBias = 0;       //BiasI/2   
		IQLoad = 1;       //3.2k
		OutBuf_Bias = 2;  //2 
		BiasHf = 1;       //110u
		//SDM_RES = 1;      //790R CD??
		//NS_RES = 0;       //0R CD??
	}
	else if(MixDiv >= 32) //32, 64
	{
		IQGen_Cur = 0;    //DminDmin
		IQBias = 0;       //BiasI/2   
		IQLoad = 1;       //3.2k
		OutBuf_Bias = 3;  //3 (min)
		BiasHf = 1;       //110u
		//SDM_RES = 1;      //790R CD??
		//NS_RES = 0;       //0R CD??
	}
	else
	{
		return R855_Fail;
	}

	//Xtal Div setting
	if(R855_Standard == R855_STD_SIZE) //for cal, RingPLL not support xtal div2
	{
		    R855_XtalDiv = R855_XTAL_DIV1_11; //div1 for calibration
			ClkOut = 1;  //off
			XtalPwrTemp = R855_XTAL_HIGHEST;
			CP_Offset = 0;  //off
			CP_ix2 = 0; // x1
	}   
	else  
	{
		if(R855_Mixer_Mode == R855_IMR_NOR) 	
			RF_Freq = LO_Freq - IF_Freq;  		
		else  
			RF_Freq = LO_Freq + IF_Freq;  
		
        if(R855_Xtal==16000)
		   u2XalDivJudge = (UINT16) (RF_Freq/1000/8);
		else if(R855_Xtal==24000)
           u2XalDivJudge = (UINT16) (RF_Freq/1000/12);
		else if(R855_Xtal==27000)
           u2XalDivJudge = (UINT16) (RF_Freq/13500);
		else
           return R855_Fail;

		u1XtalDivRemain = (UINT8) (u2XalDivJudge % 2);
		XtalDivQ = (UINT8) (u2XalDivJudge / 2);

		//fine tune for 24MHz Xtal

		//DTV 
		Spur_pos = (UINT32)(R855_Xtal*XtalDivQ + 4000);  //+4M
		Spur_neg = (UINT32)(R855_Xtal*XtalDivQ + R855_Xtal - 4000);   //-4M
		

		if( (RF_Freq>Spur_pos) && (RF_Freq<Spur_neg) )  //spur-free		
		{
			R855_XtalDiv = R855_XTAL_DIV1_11;	
			SDM_RES = 0;     //off, since no spur in this area
			ClkOut = 1;      //off
			CP_Offset = 0;   //off
			CP_ix2 = 0; // x1
			
			if(R855_Xtal_Pwr==2)  //low, gm=1.5
				XtalPwrTemp = 3;
			else
				XtalPwrTemp = R855_Xtal_Pwr;			
			
		}
		else //spur-area
		{
			if(u1XtalDivRemain==1)
				XtalDivQ = XtalDivQ+1;  //rounding
			else
				XtalDivQ = XtalDivQ;

			Spur_apply_1_op = Spur_24mhz_1_op_dtv[XtalDivQ];  //24MHz
			Spur_apply_2_op = Spur_24mhz_2_op_dtv[XtalDivQ];  //24MHz

/*
	Spur_24mhz_1_op_dtv 
	bit[7:6], Xtal PW  [0:highest, 1:high, 2:low, 3:lowest],
	bit[5:4], Xtal GM  [0:2*gm (24MHz), 1:gm (20MHz), 2:gm (16MHz), 3:off]
	bit[3], none
	bit[2:0], cp current[0:0.7mA, 1:0.6mA, 2:0.5mA, 3:0.4mA, 4:0.3mA, 5:0.2mA, 6:0.1mA, 7:Auto]

	Spur_24mhz_2_op_dtv 
	bit[7:5], none
	bit[4], xtal/2(PLL_AGC) [0:/1, 1:/2]
	bit[3], xtal/2(PLL) [0:/1, 1:/2]
	bit[2], CP offset [0:no, 1:30uA]
	bit[1], cp_ix2 [0-cp_I x1, 1:cp_I x2]
	bit[0], clkout [0:enable, 1:disable]
*/

			Xtal_Div2_AGC_PLL = (Spur_apply_2_op & 0x10)>>4; //[0:ref/1, 1:ref/2]
			Xtal_Div2_PLL = (Spur_apply_2_op & 0x08)>>3; //[0:ref/1, 1:ref/2]
			R855_XtalDiv = (Xtal_Div2_AGC_PLL<<1) + Xtal_Div2_PLL;  //[0:R855_XTAL_DIV1_11, 1:R855_XTAL_DIV2_12, 2:R855_XTAL_DIV2_21, 3:R855_XTAL_DIV4_22]
			XtalPwrTemp = ((3-((Spur_apply_1_op & 0xC0)>>6))<<1)+(1-((Spur_apply_1_op & 0x30)>>4));//[0:lowest/1.5, 1:lowest/2.0, 2:low/1.5, 3:low/2.0, 4:high/1.5, 5:high/2.0, 6:highest/1.5, 7:highest/2.0]
			CP_Current = (Spur_apply_1_op & 0x07); //[0:0.7mA, 1:0.6mA, 2:0.5mA, 3:0.4mA, 4:0.3mA, 5:0.2mA, 6:0.1mA, 7:Auto]
			CP_Offset = (Spur_apply_2_op & 0x04)>>2; //[0:no, 1:30uA]
			CP_ix2 = (Spur_apply_2_op & 0x02)>>1; //[0:cp_I x1, 1:cp_I x2]
			ClkOut = (Spur_apply_2_op & 0x01);  //[0:enable, 1:disable]
		} //end of spur area
	}

//manual Xtal Div,
if(R855_XTAL_DIV_MANUAL==1) 
{
	R855_XtalDiv = R855_XtalDiv_Man;
} 

#if(R855_SPUR_TEST == 1)
	ClkOut = 1;      //off
	CP_Offset = 0;   //off
	XtalPwrTemp = 7;	//Xtal PW = Highest, Xtal gm = 2*gm(24MHz)
	CP_Current = 7;  //auto
	CP_ix2 = 0;
#endif

    //------ Xtal freq depend setting: Xtal Gm --------//
	if(R855_SHARE_XTAL==R855_SLAVE_XTAL_OUT)
	{	
			XTAL_POW = 0; //highest
			XTAL_GM = 3;  //off
	}
	else if(R855_SHARE_XTAL==R855_MASTER_TO_SLAVE_XTAL_OUT)
	{	
			XTAL_POW = 0; //highest
			XTAL_GM = 0;  //2.0 gm
	}
	else 
	{
		if(R855_Initial_done_flag==TRUE)
		{		
			if(XtalPwrTemp<R855_Xtal_Pwr)	
				XtalPwrTemp = R855_Xtal_Pwr;

			//read VCO and check again when next channel is gm=1.5, low

			//read PLL lock
			R855_I2C_Len.RegAddr = 0x00;
			R855_I2C_Len.Len = 4;
			if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
				return R855_Fail;

			vco_bank_read = R855_I2C_Len.Data[3] & 0x3F;

			if((XtalPwrTemp<=2) && (R855_Standard!=R855_STD_SIZE)) //gm=1.5, low
			{
				while((vco_bank_lock==0) && (XtalPwrTemp<7))
				{
					if(XtalPwrTemp==0)  //gm=1.5, lowest
						R855_Array[32] = (R855_Array[32] & 0xE1) | (3<<1) | (1<<3);  
					else if(XtalPwrTemp==1)  //gm=2, lowest
						R855_Array[32] = (R855_Array[32] & 0xE1) | (3<<1) | (0<<3);  
					else if(XtalPwrTemp==2) //gm=1.5, low
						R855_Array[32] = (R855_Array[32] & 0xE1) | (2<<1) | (1<<3);  
					else if(XtalPwrTemp==3) //gm=2, low
						R855_Array[32] = (R855_Array[32] & 0xE1) | (2<<1) | (0<<3);  
					else if(XtalPwrTemp==4) //gm=1.5, high
						R855_Array[32] = (R855_Array[32] & 0xE1) | (1<<1) | (1<<3);  
					else if(XtalPwrTemp==5) //gm=2, high
						R855_Array[32] = (R855_Array[32] & 0xE1) | (1<<1) | (0<<3);  
					else if(XtalPwrTemp==6) //gm=1.5, highest
						R855_Array[32] = (R855_Array[32] & 0xE1) | (0<<1) | (1<<3);
					else  //gm=2, highest
						R855_Array[32] = (R855_Array[32] & 0xE1) | (0<<1) | (0<<3);			
				
					//set gm & xtal power
					R855_I2C.RegAddr = 32;		
					R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
					if(I2C_Write(&R855_I2C) != R855_Success)
						return R855_Fail;	

					//set manual VCO bank; 
					R855_I2C.RegAddr = 30;
					save_bnak_oth_set = R855_Array[R855_I2C.RegAddr];
					if(vco_bank_read<32)
						R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x80) | 0x40;  //manual 0
					else
						R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x80) | 0x60;  //manual 32
					R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
					if(I2C_Write(&R855_I2C) != R855_Success)
						return R855_Fail;

					//set auto
					R855_I2C.RegAddr = 30;
					R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xBF);
					R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
					if(I2C_Write(&R855_I2C) != R855_Success)
						return R855_Fail;

					//Restore setting(co-use bank)
					R855_I2C.RegAddr = 30;
					R855_Array[R855_I2C.RegAddr] = save_bnak_oth_set;
					R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
					if(I2C_Write(&R855_I2C) != R855_Success)
						return R855_Fail;

					R855_Delay_MS(R855_XTAL_CHK_DELAY*2 * 1000);  //10ms for div1 /1, agc_ref /2	// by ITE

					R855_I2C_Len.RegAddr = 0x00;
					R855_I2C_Len.Len = 4;
					if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
						return R855_Fail;

					if(((R855_I2C_Len.Data[1] & 0x80) == 0x80) && ((R855_I2C_Len.Data[3] & 0x3F) <= (vco_bank_read+5)) && ((R855_I2C_Len.Data[3] & 0x3F) >= (vco_bank_read-5))) 			
					{
						vco_bank_lock = 1;		
						break;
					}
					else  //unlock
					{
						XtalPwrTemp++;
					}
				}//end while loop
			}

				//for 16/24/27M
				if(XtalPwrTemp==0)	
				{
					XTAL_GM = 1;    //gm=1.5
					XTAL_POW = 3;   //lowest				
				}
				else if(XtalPwrTemp==1)	
				{
					XTAL_GM = 0;    //gm=2
					XTAL_POW = 3;   //lowest				
				}
				else if(XtalPwrTemp==2)	
				{
					XTAL_GM = 1;    //gm=1.5
					XTAL_POW = 2;   //low			
				}
				else if(XtalPwrTemp==3)	
				{
					XTAL_GM = 0;    //gm=2
					XTAL_POW = 2;   //low				
				}
				else if(XtalPwrTemp==4)	
				{
					XTAL_GM = 1;    //gm=1.5
					XTAL_POW = 1;   //high				
				}
				else if(XtalPwrTemp==5)	
				{
					XTAL_GM = 0;    //gm=2
					XTAL_POW = 1;   //high				
				}
				else if(XtalPwrTemp==6)	
				{
					XTAL_GM = 1;    //gm=1.5
					XTAL_POW = 0;   //highest				
				}
				else  //XtalPwrTemp=7
				{
					XTAL_GM = 0;    //gm=2
					XTAL_POW = 0;   //highest
				}
			
		}  //end of if(R855_Initial_done_flag==TRUE)
		else   //init fail
		{
				XTAL_POW = 0;      //highest,       R32[2:1]=0	
				XTAL_GM = 0;       //gm=2
		}
	}
    //Xtal power & gm 
	R855_I2C.RegAddr = 32;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE1) | (XTAL_POW<<1) | (XTAL_GM<<3); 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

    //clk_out
    R855_I2C.RegAddr = 35;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFD) | (ClkOut<<1); 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

    //Set CP offset
	R855_I2C.RegAddr = 36;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | (CP_Offset<<7); // R36[7]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	//Set CP_ix2
	R855_I2C.RegAddr = 44;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | (CP_ix2<<7); // R44[7]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	//Set CP_Current
	R855_I2C.RegAddr = 31;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE3) | (CP_Current<<2); // R31[4:2]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	//------ Xtal freq depend setting: AGC ref clk --------//
	if(R855_Xtal==16000)
	{		
		R855_Array[32] = (R855_Array[32] & 0xDF) | 0x00;  //clk(16)/1 /2  = 8M 
	}
	else if(R855_Xtal==24000)
	{
		if((R855_XtalDiv==R855_XTAL_DIV1_11) || (R855_XtalDiv==R855_XTAL_DIV2_12))
			R855_Array[32] = (R855_Array[32] & 0xDF) | 0x20;  //clk(24)/1 /3 = 8M
		else
			R855_Array[32] = (R855_Array[32] & 0xDF) | 0x20;  //clk(24)/2 /3 = 4M   //related to AGC clk speed 
	}
	else if(R855_Xtal==27000)
	{
		if((R855_XtalDiv==R855_XTAL_DIV1_11) || (R855_XtalDiv==R855_XTAL_DIV2_12))
			R855_Array[32] = (R855_Array[32] & 0xDF) | 0x20;  //clk(24)/1 /3 = 8M
		else
			R855_Array[32] = (R855_Array[32] & 0xDF) | 0x20;  //clk(24)/2 /3 = 4M   //related to AGC clk speed 
	}
	else  //not support Xtal freq
	{
		return R855_Fail;
	}
	//Set AGC ref clk
	R855_I2C.RegAddr = 32;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//------ Xtal divider 1 & 2 --------//
	if(R855_XtalDiv==R855_XTAL_DIV1_11)
	{
		PLL_Ref = R855_Xtal;	         
		R855_Array[32] = (R855_Array[32] & 0x3F) | 0x00; //b7:2nd_div2=0, b6:1st_div2=0
	}
	else if(R855_XtalDiv==R855_XTAL_DIV2_12)
	{
		PLL_Ref = R855_Xtal/2;	         
		R855_Array[32] = (R855_Array[32] & 0x3F) | 0x80; //b7:2nd_div2=1, b6:1st_div2=0
	}
	else if(R855_XtalDiv==R855_XTAL_DIV2_21)
	{
		PLL_Ref = R855_Xtal/2;	         
		R855_Array[32] = (R855_Array[32] & 0x3F) | 0x40; //b7:2nd_div2=0, b6:1st_div2=1
	}
	else if(R855_XtalDiv==R855_XTAL_DIV4_22)  //not use
	{
		PLL_Ref = R855_Xtal/4;	         
		R855_Array[32] = (R855_Array[32] & 0x3F) | 0xC0; //b7:2nd_div2=1, b6:1st_div2=1
	}
	//Xtal divider setting
	R855_I2C.RegAddr = 32;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
    if(I2C_Write(&R855_I2C) != R855_Success)
	   return R855_Fail;

	//Out Buf Bias R10[6:5]
	R855_I2C.RegAddr = 10;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x9F) | (OutBuf_Bias<<5); 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;


        //IQ gen current R11[0]
	R855_I2C.RegAddr = 11;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE) | (IQGen_Cur);
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

       //Divider HfDiv current=135u(R31[6:5]=00)
	R855_I2C.RegAddr = 31;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x9F) | 0x00; 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

    

	//BiasI(R12[4])
	R855_I2C.RegAddr = 12;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xEF) | (IQBias<<4); 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

       //BiasHF(R33[7:6])
	R855_I2C.RegAddr = 33;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x3B) | (BiasHf<<6); 
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;

	//Divider num
	R855_I2C.RegAddr = 29;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE3) | (DivNum << 2);  //R29[4:2]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	VCO_Freq = LO_Freq * MixDiv;
	Nint = (UINT16) (VCO_Freq / 2 / PLL_Ref);
	VCO_Fra = (UINT16) (VCO_Freq - 2 * PLL_Ref * Nint);

	//max offset = (2*PLL_Ref)/MixDiv

      // IQLoadv R12[5]
	R855_I2C.RegAddr = 12;
	R855_Array[12] = (R855_Array[12] & 0xDF) | (IQLoad<<5); 
	R855_I2C.Data = R855_Array[12];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;
    
/*
        //NS_RES CD?? 
	if(VCO_Freq>4200000)
		NS_RES = 0; 
	R855_I2C.RegAddr = 36;
	R855_Array[36] = (R855_Array[36] & 0xDE) | (NS_RES) | (IQLoad<<5); 
	R855_I2C.Data = R855_Array[36];
	if(I2C_Write(&R855_I2C) != R855_Success)
	    return R855_Fail;
*/
	if(PLL_Ref==16000)  
	{
		if(MixDiv==2)
		{
			BoundDiv = 4;
			CentBoundDiv = 2;
		}
		else if(MixDiv==4)
		{
			BoundDiv = 2;
			CentBoundDiv = 1;
		}
		else
		{
			BoundDiv = 1;
			CentBoundDiv = 1;
		}
	}
	else if(PLL_Ref==8000)
	{
		if(MixDiv==2)
		{
			BoundDiv = 2;
			CentBoundDiv = 1;
		}
		else if(MixDiv==4)
		{
			BoundDiv = 1;
			CentBoundDiv = 1;
		}
		else
		{
			BoundDiv = 1;
			CentBoundDiv = 1;
		}
	}
	else if((PLL_Ref==24000) || (PLL_Ref==27000))
	{
		if(MixDiv==2)
		{
			BoundDiv = 6;
			CentBoundDiv = 3;
		}
		else if(MixDiv==4)
		{
			BoundDiv = 3;
			CentBoundDiv = 2;
		}
		else if(MixDiv==8)
		{
			BoundDiv = 2;
			CentBoundDiv = 1;
		}
		else
		{
			BoundDiv = 1;
			CentBoundDiv = 1;
		}
	}		
	else if((PLL_Ref==12000) || (PLL_Ref==13500))
	{
		if(MixDiv==2)
		{
			BoundDiv = 3;
			CentBoundDiv = 2;
		}
		else if(MixDiv==4)
		{
			BoundDiv = 2;
			CentBoundDiv = 1;
		}
		else if(MixDiv==8)
		{
			BoundDiv = 1;
			CentBoundDiv = 1;
		}
		else
		{
			BoundDiv = 1;
			CentBoundDiv = 1;
		}
	}
	else
	{
		BoundDiv = 1;
		CentBoundDiv = 1;
	}

	//Boundary spur prevention
	if (VCO_Fra < PLL_Ref/64/BoundDiv)  //2*PLL_Ref/128/BoundDiv
	{
		VCO_Fra = 0;
	}
	else if (VCO_Fra > (2*PLL_Ref - PLL_Ref/64/BoundDiv))  //2*PLL_Ref*(1-1/128/BoundDiv)
	{
		VCO_Fra = 0;
		Nint ++;
	}
	else if((VCO_Fra > (PLL_Ref - PLL_Ref/128/CentBoundDiv)) && (VCO_Fra < PLL_Ref)) //> 2*PLL_Ref*(1-1/128/CentBoundDiv)*0.5,  < 2*PLL_Ref*128/256
	{
		VCO_Fra = (PLL_Ref - PLL_Ref/128/CentBoundDiv);      // VCO_Fra = 2*PLL_Ref*(1-1/128/CentBoundDiv)*0.5
	}
	else if((VCO_Fra > PLL_Ref) && (VCO_Fra < (PLL_Ref + PLL_Ref/128/CentBoundDiv))) //> 2*PLL_Ref*128/256,  < 2*PLL_Ref*(1+1/128/CentBoundDiv)*0.5
	{
		VCO_Fra = (PLL_Ref + PLL_Ref/128/CentBoundDiv);      // VCO_Fra = 2*PLL_Ref*(1+1/128/CentBoundDiv)*0.5
	}
	else
	{
		VCO_Fra = VCO_Fra;
	}
	

	//Ni & Si	
	Ni = (UINT8) ((Nint - 13) / 4);
	Si = (UINT8) (Nint - 4 *Ni - 13);

	R855_I2C.RegAddr = 26;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x80) | Ni;     //R26[6:0]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	  return R855_Fail;

	R855_I2C.RegAddr = 29;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x3F) | (Si<<6);  //R29[7:6]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
	  return R855_Fail;
         	
	//pw_sdm
	R855_I2C.RegAddr = 30;
	R855_Array[R855_I2C.RegAddr] &= 0x7F;    //R30[7]
	if(VCO_Fra == 0)
	{
		R855_Array[R855_I2C.RegAddr] |= 0x80;
	}


	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//SDM calculator
	while(VCO_Fra > 1)
	{			
		if (VCO_Fra > (2*PLL_Ref / Nsdm))
		{		
			SDM = SDM + 32768 / (Nsdm/2);
			VCO_Fra = VCO_Fra - 2*PLL_Ref / Nsdm;
			if (Nsdm >= 0x8000)
				break;
		}
		Nsdm = Nsdm << 1;
	}

	SDM16to9 = SDM >> 8;
	SDM8to1 =  SDM - (SDM16to9 << 8);

	R855_I2C.RegAddr = 28;
	R855_Array[R855_I2C.RegAddr] = (UINT8) SDM16to9;  //R28
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = 27;
	R855_Array[R855_I2C.RegAddr] = (UINT8) SDM8to1;    //R27
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

 //move to end of R855_SetFreq( )??
	//PLL_lock delay time & AGC clk
	//Xtal freq/div1/agc_ref/pll_atune(2^10)*64
	if(R855_Xtal==24000 || R855_Xtal==27000)  //24M, 27M
	{
		if((R855_XtalDiv == R855_XTAL_DIV4_22) || (R855_XtalDiv == R855_XTAL_DIV2_21))
		{
			R855_Delay_MS(R855_PLL_LOCK_DELAY*2 * 1000);  //agc_ref: /3  (4M)	// by ITE
			R855_Array[47] = (R855_Array[47] & 0xF3) | 0x04;  //R47[3:2]=01 (1KHz)
		}
		else
		{
			R855_Delay_MS(R855_PLL_LOCK_DELAY * 1000);      //agc_ref: /3 (8M)	// by ITE
			R855_Array[47] = (R855_Array[47] & 0xF3) | 0x00;  //R47[3:2]=01 (512Hz)   //CD?? no 512Hz
		}
	}
	else  //16M
	{
		if((R855_XtalDiv == R855_XTAL_DIV4_22) || (R855_XtalDiv == R855_XTAL_DIV2_21))
		{
			R855_Delay_MS(R855_PLL_LOCK_DELAY*2 * 1000);	//agc_ref: /2  (4M)	// by ITE
			R855_Array[47] = (R855_Array[47] & 0xF3) | 0x04;  //R47[3:2]=01 (1KHz)
		}
		else 
		{
			R855_Delay_MS(R855_PLL_LOCK_DELAY * 1000);     //agc_ref: /2  (8M)	// by ITE
			R855_Array[47] = (R855_Array[47] & 0xF3) | 0x00;  //R47[3:2]=01 (512Hz)   //CD?? no 512Hz
		}
	}

	//Set AGC clk
	R855_I2C.RegAddr = 47;	
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	if(R855_SHARE_XTAL!=R855_SLAVE_XTAL_OUT)
	{
		//read PLL lock
		R855_I2C_Len.RegAddr = 0x00;
		R855_I2C_Len.Len = 3;
		if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
			return R855_Fail;

		if( (R855_I2C_Len.Data[1] & 0x80) == 0x00 ) 
		{
			R855_I2C.RegAddr = 32;
			R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE7) | 0x00;  //gm2
			R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;
		}
	}

	//set pll autotune = 1khz (1)
	R855_I2C.RegAddr = 47;
	R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] & 0xFD;  //R47[1]=0
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;

}


R855_ErrCode R855_MUX(UINT32 LO_KHz, UINT32 RF_KHz, R855_Standard_Type R855_Standard) //CD OK
{	
	UINT8 Reg_IMR_Gain_G7   = 0;
	UINT8 Reg_IMR_Phase_G7  = 0;
	UINT8 Reg_IMR_Iqcap_G7  = 0;

	UINT8 Reg_IMR_Gain_L7   = 0;
	UINT8 Reg_IMR_Phase_L7  = 0;

	//Freq_Info_Type R855_Freq_Info1;
	R855_Freq_Info1 = R855_Freq_Sel(LO_KHz, RF_KHz, R855_Standard);

	// LNA band, TF LPF
	R855_I2C.RegAddr = 15;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xBC) | (R855_Freq_Info1.LNA_BAND) | (R855_Freq_Info1.BYP_LPF<<6);  //R15[1:0], R15[6]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	// RF Polyfilter
	R855_I2C.RegAddr = 12;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x3F) | (R855_Freq_Info1.RF_POLY<<6);  //R12[7:6]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	// LNA Cap, Notch
	R855_I2C.RegAddr = 16;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x00) | (R855_Freq_Info1.LPF_CAP)  | (R855_Freq_Info1.LPF_NOTCH<<4);	 //R16
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
/*
	// LNA Notch
	R855_I2C.RegAddr = 16;
	R855_Array[16] = (R855_Array[16] & 0x0F) | (R855_Freq_Info1.LPF_NOTCH<<4);  //R16[7:4]
	R855_I2C.Data = R855_Array[16];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	 // Set TF LPF
	 R855_I2C.RegAddr = 15;
	 R855_Array[15] = (R855_Array[15] & 0xBF) | (R855_Freq_Info1.BYP_LPF<<6);  //R15[6]
     R855_I2C.Data = R855_Array[15];
     if(I2C_Write(&R855_I2C) != R855_Success)
        return R855_Fail;
*/
	//Set_IMR
	if(R855_IMR_done_flag == TRUE)
	{
		if(R855_Mixer_Mode==R855_IMR_NOR)
		{
			Reg_IMR_Gain_G7 = R855_IMR_Data_G7_MIXAMP[R855_Freq_Info1.IMR_MEM_NOR].Gain_X & 0x0F;   //R20[3:0]
			Reg_IMR_Phase_G7 = R855_IMR_Data_G7_MIXAMP[R855_Freq_Info1.IMR_MEM_NOR].Phase_Y & 0x0F; //R21[3:0]
			Reg_IMR_Iqcap_G7 = R855_IMR_Data_G7_MIXAMP[R855_Freq_Info1.IMR_MEM_NOR].Iqcap;          //0,1,2

			Reg_IMR_Gain_L7 = R855_IMR_Data_L7_MIXAMP[R855_Freq_Info1.IMR_MEM_NOR].Gain_X & 0x0F;   //R42[3:0]
			Reg_IMR_Phase_L7 = (R855_IMR_Data_L7_MIXAMP[R855_Freq_Info1.IMR_MEM_NOR].Phase_Y & 0xF0)>>4; //R42[7:4]
		}
		else
		{
			Reg_IMR_Gain_G7 = R855_IMR_Data_G7_MIXAMP[R855_Freq_Info1.IMR_MEM_REV].Gain_X & 0x0F;   //R20[3:0]
			Reg_IMR_Phase_G7 = R855_IMR_Data_G7_MIXAMP[R855_Freq_Info1.IMR_MEM_REV].Phase_Y & 0x0F; //R21[3:0]
			Reg_IMR_Iqcap_G7 = R855_IMR_Data_G7_MIXAMP[R855_Freq_Info1.IMR_MEM_REV].Iqcap;          //0,1,2	

			Reg_IMR_Gain_L7 = R855_IMR_Data_L7_MIXAMP[R855_Freq_Info1.IMR_MEM_REV].Gain_X & 0x0F;   //R42[3:0]
			Reg_IMR_Phase_L7 = (R855_IMR_Data_L7_MIXAMP[R855_Freq_Info1.IMR_MEM_REV].Phase_Y & 0xF0)>>4; //R42[7:4]
		}
/*
		if(((Reg_IMR_Gain & 0x07)>6) || ((Reg_IMR_Phase & 0x07)>6))
		{
			Reg_IMR_Gain = 0;
			Reg_IMR_Phase = 0;
			Reg_IMR_Iqcap = 0; 		
			R855_IMR_Cal_Result = 1; //fail
		}
		else
		{
			R855_IMR_Cal_Result = 0; //ok
		}
*/		
		if(R855_IMR_Cal_Result==1)  //fail
		{
			Reg_IMR_Gain_G7 = 0;
			Reg_IMR_Phase_G7 = 0;
			Reg_IMR_Iqcap_G7 = 0;

			Reg_IMR_Gain_L7 = 0;
			Reg_IMR_Phase_L7 = 0;
		}
	}
	else
	{
		Reg_IMR_Gain_G7 = 0;
	    Reg_IMR_Phase_G7 = 0;
		Reg_IMR_Iqcap_G7 = 0;

		Reg_IMR_Gain_L7 = 0;
		Reg_IMR_Phase_L7 = 0;
	}

	//Gain, R20[4:0]
	R855_I2C.RegAddr = R855_IMR_GAIN_REG_G7;                  
	R855_Array[R855_IMR_GAIN_REG_G7] = (R855_Array[R855_IMR_GAIN_REG_G7] & 0xF0) | (Reg_IMR_Gain_G7 & 0x0F);
	R855_I2C.Data = R855_Array[R855_IMR_GAIN_REG_G7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

    //Phase, R21[4:0]
	R855_I2C.RegAddr = R855_IMR_PHASE_REG_G7;                  
	R855_Array[R855_IMR_PHASE_REG_G7] = (R855_Array[R855_IMR_PHASE_REG_G7] & 0xF0) | (Reg_IMR_Phase_G7 & 0x0F);
	R855_I2C.Data = R855_Array[R855_IMR_PHASE_REG_G7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//Iqcap, R21[6:5]
	R855_I2C.RegAddr = R855_IMR_IQCAP_REG_G7;
	R855_Array[R855_IMR_IQCAP_REG_G7] = (R855_Array[R855_IMR_IQCAP_REG_G7] & 0x9F) | (Reg_IMR_Iqcap_G7<<5);
	R855_I2C.Data =R855_Array[R855_IMR_IQCAP_REG_G7]  ;
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//Gain,R42[3:0]
	R855_I2C.RegAddr = R855_IMR_GAIN_REG_L7;                  
	R855_Array[R855_IMR_GAIN_REG_L7] = (R855_Array[R855_IMR_GAIN_REG_L7] & 0xF0) | (Reg_IMR_Gain_L7 & 0x0F);
	R855_I2C.Data = R855_Array[R855_IMR_GAIN_REG_L7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

    //Phase, R42[7:4]
	R855_I2C.RegAddr = R855_IMR_PHASE_REG_L7;                  
	R855_Array[R855_IMR_PHASE_REG_L7] = (R855_Array[R855_IMR_PHASE_REG_L7] & 0x0F) | ((Reg_IMR_Phase_L7 & 0x0F)<<4);
	R855_I2C.Data = R855_Array[R855_IMR_PHASE_REG_L7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}


R855_ErrCode R855_SetTF(UINT32 u4FreqKHz, UINT8 u1TfType)   //CD OK
{
    UINT8    u1FreqCount = 0;
	UINT32   u4Freq1 = 0;
	UINT32   u4Freq2 = 0;
	UINT32   u4Ratio;
	UINT8    u1TF_Set_Result1 = 0;
	UINT8    u1TF_Set_Result2 = 0;
	UINT8    u1TF_tmp1, u1TF_tmp2;
	UINT16   u2Int;
	
	if((u4FreqKHz>0) && (u4FreqKHz<R855_LNA_MID_LOW[R855_SetTfType_UL_MID]))  //Low
	{

		if(u4FreqKHz<R855_TF_Freq_Low[u1TfType][R855_TF_LOW_NUM-1])
		{
			u1FreqCount = R855_TF_LOW_NUM;
		}
		else
		{
			while((u4FreqKHz < R855_TF_Freq_Low[u1TfType][u1FreqCount]) && (u1FreqCount<R855_TF_LOW_NUM))
			{
			   u1FreqCount++;
			}
		}

		if(u1FreqCount==0)
		{
			R855_TF = R855_TF_Result_Low[u1TfType][0];
		}
		else if(u1FreqCount==R855_TF_LOW_NUM)
        {
			R855_TF = R855_TF_Result_Low[u1TfType][R855_TF_LOW_NUM-1];
		}
		else
		{
			u1TF_Set_Result1 = R855_TF_Result_Low[u1TfType][u1FreqCount-1]; 
			u1TF_Set_Result2 = R855_TF_Result_Low[u1TfType][u1FreqCount]; 
			u4Freq1 = R855_TF_Freq_Low[u1TfType][u1FreqCount-1];
			u4Freq2 = R855_TF_Freq_Low[u1TfType][u1FreqCount];

			u1TF_tmp1 = ((u1TF_Set_Result1 & 0x40)>>2)*3 + (u1TF_Set_Result1 & 0x3F);  //b6 is 3xb4
			u1TF_tmp2 = ((u1TF_Set_Result2 & 0x40)>>2)*3 + (u1TF_Set_Result2 & 0x3F);		
			 
			u4Ratio = (u4Freq1- u4FreqKHz)*100/(u4Freq1 - u4Freq2);

			u2Int = (UINT16)((u1TF_tmp2 - u1TF_tmp1)*u4Ratio/100);
			R855_TF = u1TF_tmp1 + (UINT8)u2Int;
			if(((u1TF_tmp2 - u1TF_tmp1)*u4Ratio - u2Int*100) > 50)			 
			 R855_TF = R855_TF + 1;	

			if(R855_TF>=0x40)
			{
				R855_TF = (R855_TF + 0x10);
			}
		 }
	}
	else if((u4FreqKHz>=R855_LNA_MID_LOW[R855_SetTfType_UL_MID]) && (u4FreqKHz<R855_LNA_HIGH_MID[R855_SetTfType_UL_MID]))  //Mid
    {

		if((u4FreqKHz < R855_TF_Freq_Mid[u1TfType][R855_TF_MID_NUM-1]))
		{
			u1FreqCount = R855_TF_MID_NUM;
		}
		else
		{
			while((u4FreqKHz < R855_TF_Freq_Mid[u1TfType][u1FreqCount]) && (u1FreqCount<R855_TF_MID_NUM))
			{
				u1FreqCount++;
			}
		}

		if(u1FreqCount==0)
		{
			R855_TF = R855_TF_Result_Mid[u1TfType][0];
		}
		else if(u1FreqCount==R855_TF_MID_NUM)
		{
			R855_TF = R855_TF_Result_Mid[u1TfType][R855_TF_MID_NUM-1];
		}
		else
		{
			u1TF_Set_Result1 = R855_TF_Result_Mid[u1TfType][u1FreqCount-1]; 
			u1TF_Set_Result2 = R855_TF_Result_Mid[u1TfType][u1FreqCount]; 
			u4Freq1 = R855_TF_Freq_Mid[u1TfType][u1FreqCount-1];
			u4Freq2 = R855_TF_Freq_Mid[u1TfType][u1FreqCount]; 

			u1TF_tmp1 = ((u1TF_Set_Result1 & 0x40)>>2) + (u1TF_Set_Result1 & 0x3F);  //b6 is 1xb4
			u1TF_tmp2 = ((u1TF_Set_Result2 & 0x40)>>2) + (u1TF_Set_Result2 & 0x3F);	

			u4Ratio = (u4Freq1- u4FreqKHz)*100/(u4Freq1 - u4Freq2);

			u2Int = (UINT16)((u1TF_tmp2 - u1TF_tmp1)*u4Ratio/100);
			R855_TF = u1TF_tmp1 + (UINT8)u2Int;
			if(((u1TF_tmp2 - u1TF_tmp1)*u4Ratio - u2Int*100) > 50)			 
				R855_TF = R855_TF + 1;			 

			if(R855_TF>=0x40)
			{
				R855_TF = (R855_TF + 0x30);
			}
		}
	}
	else  //HIGH
	{
		if(u4FreqKHz < R855_TF_Freq_High[u1TfType][R855_TF_HIGH_NUM-1])
		{
			u1FreqCount = R855_TF_HIGH_NUM;
		}
		else
		{
			while((u4FreqKHz < R855_TF_Freq_High[u1TfType][u1FreqCount]) && (u1FreqCount<R855_TF_HIGH_NUM))
			{
			   u1FreqCount++;
			}
		}
        
		if(u1FreqCount==0)
		{
			R855_TF = R855_TF_Result_High[u1TfType][0];
		}
		else if(u1FreqCount==R855_TF_HIGH_NUM)
        {
			R855_TF = R855_TF_Result_High[u1TfType][R855_TF_HIGH_NUM-1];
		}
		else
		{
			u1TF_Set_Result1 = R855_TF_Result_High[u1TfType][u1FreqCount-1]; 
		    u1TF_Set_Result2 = R855_TF_Result_High[u1TfType][u1FreqCount]; 
		    u4Freq1 = R855_TF_Freq_High[u1TfType][u1FreqCount-1];
		    u4Freq2 = R855_TF_Freq_High[u1TfType][u1FreqCount]; 
			u4Ratio = (u4Freq1- u4FreqKHz)*100/(u4Freq1 - u4Freq2);

			u2Int = (UINT16)((u1TF_Set_Result2 - u1TF_Set_Result1)*u4Ratio/100);
			R855_TF = u1TF_Set_Result1 + (UINT8)u2Int;
			if(((u1TF_Set_Result2 - u1TF_Set_Result1)*u4Ratio - u2Int*100) > 50)			 
			    R855_TF = R855_TF + 1;	             
		}
	}
  
    //Set TF: R14[6:0]
	R855_I2C.RegAddr = 14;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x80) | R855_TF;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr]  ;
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

    return R855_Success;
}

R855_ErrCode R855_IQ_G7_MIXAMP(R855_Sect_Type* IQ_Pont) //CD OK  Mixer > 7
{
	R855_Sect_Type Compare_IQ[3];
	UINT8   X_Direction;  // 1:X, 0:Y

	 //------- increase Filter gain to let ADC read value significant ---------//
	UINT8   LPF_Count = 0;
	UINT8   ADC_Read_Value = 0;

	 for(LPF_Count=5; LPF_Count < 16; LPF_Count=LPF_Count+2)  //start from 5
	 {
		R855_I2C.RegAddr = 25;
		R855_Array[25] = (R855_Array[25] & 0x0F) | (LPF_Count<<4);
		R855_I2C.Data = R855_Array[25];  
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_Delay_MS(R855_FILTER_GAIN_DELAY * 1000); 	// by ITE
		
		if(R855_Muti_Read(&ADC_Read_Value) != R855_Success)
			return R855_Fail;

		if(ADC_Read_Value > 40*R855_ADC_READ_COUNT)
			break;
	 }

/*
	//Filter Gain 15
	R855_I2C.RegAddr = 41;
	R855_Array[41] = (R855_Array[41] & 0x0F) | (15<<4);
	R855_I2C.Data = R855_Array[41];  
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
*/
	//give a initial value, no useful
	Compare_IQ[0].Gain_X  = R855_Array[R855_IMR_GAIN_REG_G7] & 0xF0;
	Compare_IQ[0].Phase_Y = R855_Array[R855_IMR_PHASE_REG_G7] & 0xF0;

	    // Determine X or Y
	    if(R855_IMR_Cross_G7_MIXAMP(&Compare_IQ[0], &X_Direction) != R855_Success)
			return R855_Fail;

		if(X_Direction==1)
		{
			//compare and find min of 3 points. determine I/Q direction
		    if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
			  return R855_Fail;

		    //increase step to find min value of this direction
		    if(R855_CompreStep_G7_MIXAMP(&Compare_IQ[0], R855_IMR_GAIN_REG_G7) != R855_Success)  //X
			  return R855_Fail;
		}
		else
		{
		   //compare and find min of 3 points. determine I/Q direction
		   if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		   	 return R855_Fail;

		   //increase step to find min value of this direction
		   if(R855_CompreStep_G7_MIXAMP(&Compare_IQ[0], R855_IMR_PHASE_REG_G7) != R855_Success)  //Y
			 return R855_Fail;
		}

		//Another direction
		if(X_Direction==1) //Y-direct
		{	    
           if(R855_IQ_Tree5_G7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_G7, &Compare_IQ[0]) != R855_Success) //Y
		     return R855_Fail;

		   //compare and find min of 3 points. determine I/Q direction
		   if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		   	 return R855_Fail;

		   //increase step to find min value of this direction
		   if(R855_CompreStep_G7_MIXAMP(&Compare_IQ[0], R855_IMR_PHASE_REG_G7) != R855_Success)  //Y
			 return R855_Fail;
		}
		else //X-direct
		{
		   if(R855_IQ_Tree5_G7_MIXAMP(Compare_IQ[0].Phase_Y, Compare_IQ[0].Gain_X, R855_IMR_PHASE_REG_G7, &Compare_IQ[0]) != R855_Success) //X
		     return R855_Fail;
        
		   //compare and find min of 3 points. determine I/Q direction
		   if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		     return R855_Fail;

	       //increase step to find min value of this direction
		   if(R855_CompreStep_G7_MIXAMP(&Compare_IQ[0], R855_IMR_GAIN_REG_G7) != R855_Success) //X
		     return R855_Fail;
		}
		

		//--- Check 3 points again---//
		if(X_Direction==1)  //X-direct
		{
		    if(R855_IQ_Tree_G7_MIXAMP(Compare_IQ[0].Phase_Y, Compare_IQ[0].Gain_X, R855_IMR_PHASE_REG_G7, &Compare_IQ[0]) != R855_Success) //X
			  return R855_Fail;
		}
		else  //Y-direct
		{
		   if(R855_IQ_Tree_G7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_G7, &Compare_IQ[0]) != R855_Success) //Y
			return R855_Fail;
		}

		if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
			return R855_Fail;

    //Section-9 check
	if(R855_Section_G7_MIXAMP(&Compare_IQ[0]) != R855_Success)
			return R855_Fail;

	//clear IQ_Cap = 0
	//Compare_IQ[0].Iqcap = R855_Array[R855_IMR_IQCAP_REG_G7] & 0x3F;
	Compare_IQ[0].Iqcap = 0;

	if(R855_IMR_Iqcap(&Compare_IQ[0]) != R855_Success)
			return R855_Fail;

	*IQ_Pont = Compare_IQ[0];

	//reset gain/phase/iqcap control setting
	R855_I2C.RegAddr = R855_IMR_GAIN_REG_G7;
	R855_Array[R855_IMR_GAIN_REG_G7] = R855_Array[R855_IMR_GAIN_REG_G7] & 0xF0;
	R855_I2C.Data = R855_Array[R855_IMR_GAIN_REG_G7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = R855_IMR_PHASE_REG_G7;
	R855_Array[R855_IMR_PHASE_REG_G7] = R855_Array[R855_IMR_PHASE_REG_G7] & 0xF0;
	R855_I2C.Data = R855_Array[R855_IMR_PHASE_REG_G7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = R855_IMR_IQCAP_REG_G7;
	R855_Array[R855_IMR_IQCAP_REG_G7] = R855_Array[R855_IMR_IQCAP_REG_G7] & 0x9F;
	R855_I2C.Data = R855_Array[R855_IMR_IQCAP_REG_G7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}
R855_ErrCode R855_IQ_L7_MIXAMP(R855_Sect_Type* IQ_Pont) //CD OK  Mixer gain < 7
{
	R855_Sect_Type Compare_IQ[3];
	UINT8   X_Direction;  // 1:X, 0:Y

	 //------- increase Filter gain to let ADC read value significant ---------//
	UINT8   LPF_Count = 0;
	UINT8   ADC_Read_Value = 0;

	 for(LPF_Count=5; LPF_Count < 16; LPF_Count=LPF_Count+2)  //start from 5
	 {
		R855_I2C.RegAddr = 25;
		R855_Array[25] = (R855_Array[25] & 0x0F) | (LPF_Count<<4);
		R855_I2C.Data = R855_Array[25];  
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_Delay_MS(R855_FILTER_GAIN_DELAY * 1000); 	// by ITE
		
		if(R855_Muti_Read(&ADC_Read_Value) != R855_Success)
			return R855_Fail;

		if(ADC_Read_Value > 40*R855_ADC_READ_COUNT)
			break;
	 }

/*
	//Filter Gain 15
	R855_I2C.RegAddr = 41;
	R855_Array[41] = (R855_Array[41] & 0x0F) | (15<<4);
	R855_I2C.Data = R855_Array[41];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
*/
	//give a initial value, no useful
	Compare_IQ[0].Gain_X  = R855_Array[R855_IMR_GAIN_REG_L7] & 0xF0;
	Compare_IQ[0].Phase_Y = R855_Array[R855_IMR_PHASE_REG_L7] & 0x0F;

	    // Determine X or Y
	    if(R855_IMR_Cross_L7_MIXAMP(&Compare_IQ[0], &X_Direction) != R855_Success)
			return R855_Fail;

		if(X_Direction==1)
		{
			//compare and find min of 3 points. determine I/Q direction
		    if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
			  return R855_Fail;

		    //increase step to find min value of this direction
		    //if(R855_CompreStep_L7_MIXAMP(&Compare_IQ[0], R855_IMR_GAIN_REG_L7) != R855_Success)  //X
			if(R855_CompreStep_L7_MIXAMP(&Compare_IQ[0], X_Direction) != R855_Success)  //X
			  return R855_Fail;
		}
		else
		{
		   //compare and find min of 3 points. determine I/Q direction
		   if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		   	 return R855_Fail;

		   //increase step to find min value of this direction
		   //if(R855_CompreStep_L7_MIXAMP(&Compare_IQ[0], R855_IMR_PHASE_REG_L7) != R855_Success)  //Y
		   if(R855_CompreStep_L7_MIXAMP(&Compare_IQ[0], X_Direction) != R855_Success)  //Y
			 return R855_Fail;
		}

		//Another direction
		if(X_Direction==1) //Y-direct
		{	    
           //if(R855_IQ_Tree5_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_L7, &Compare_IQ[0]) != R855_Success) //Y
			if(R855_IQ_Tree5_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, X_Direction, &Compare_IQ[0]) != R855_Success) //Y
		     return R855_Fail;

		   //compare and find min of 3 points. determine I/Q direction
		   if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		   	 return R855_Fail;

		   //increase step to find min value of this direction
		   //if(R855_CompreStep_L7_MIXAMP(&Compare_IQ[0], R855_IMR_PHASE_REG_L7) != R855_Success)  //Y
		   if(R855_CompreStep_L7_MIXAMP(&Compare_IQ[0], 1-X_Direction) != R855_Success)  //Y
			 return R855_Fail;
		}
		else //X-direct
		{
		   //if(R855_IQ_Tree5_L7_MIXAMP(Compare_IQ[0].Phase_Y, Compare_IQ[0].Gain_X, R855_IMR_PHASE_REG_L7, &Compare_IQ[0]) != R855_Success) //X
			if(R855_IQ_Tree5_L7_MIXAMP(Compare_IQ[0].Phase_Y, Compare_IQ[0].Gain_X, X_Direction, &Compare_IQ[0]) != R855_Success) //X
		     return R855_Fail;
        
		   //compare and find min of 3 points. determine I/Q direction
		   if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		     return R855_Fail;

	       //increase step to find min value of this direction
		   //if(R855_CompreStep_L7_MIXAMP(&Compare_IQ[0], R855_IMR_GAIN_REG_L7) != R855_Success) //X
		   if(R855_CompreStep_L7_MIXAMP(&Compare_IQ[0], 1-X_Direction) != R855_Success) //X
		     return R855_Fail;
		}
		

		//--- Check 3 points again---//
		if(X_Direction==1)  //X-direct
		{
		   //if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Phase_Y, Compare_IQ[0].Gain_X, R855_IMR_PHASE_REG_L7, &Compare_IQ[0]) != R855_Success) //X
			if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Phase_Y, Compare_IQ[0].Gain_X, 1-X_Direction, &Compare_IQ[0]) != R855_Success) //X
			  return R855_Fail;
		}
		else  //Y-direct
		{
		   //if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_L7, &Compare_IQ[0]) != R855_Success) //Y
			if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, 1-X_Direction, &Compare_IQ[0]) != R855_Success) //Y
			return R855_Fail;
		}

		if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
			return R855_Fail;

    //Section-9 check
	if(R855_Section_L7_MIXAMP(&Compare_IQ[0]) != R855_Success)
			return R855_Fail;

	//clear IQ_Cap = 0
	//Compare_IQ[0].Iqcap = R855_Array[R855_IMR_IQCAP_REG_G7] & 0x3F;
	Compare_IQ[0].Iqcap = 0;

	if(R855_IMR_Iqcap(&Compare_IQ[0]) != R855_Success)
			return R855_Fail;

	*IQ_Pont = Compare_IQ[0];

	//reset gain/phase/iqcap control setting
	R855_I2C.RegAddr = R855_IMR_GAIN_REG_L7;
	R855_Array[R855_IMR_GAIN_REG_L7] = R855_Array[R855_IMR_GAIN_REG_L7] & 0xF0;
	R855_I2C.Data = R855_Array[R855_IMR_GAIN_REG_L7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = R855_IMR_PHASE_REG_L7;
	R855_Array[R855_IMR_PHASE_REG_L7] = R855_Array[R855_IMR_PHASE_REG_L7] & 0x0F;
	R855_I2C.Data = R855_Array[R855_IMR_PHASE_REG_L7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = R855_IMR_IQCAP_REG_G7;
	R855_Array[R855_IMR_IQCAP_REG_G7] = R855_Array[R855_IMR_IQCAP_REG_G7] & 0x9F;
	R855_I2C.Data = R855_Array[R855_IMR_IQCAP_REG_G7];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}
//--------------------------------------------------------------------------------------------
// Purpose: record IMR results by input gain/phase location
//          then adjust gain or phase positive 1 step and negtive 1 step, both record results
// input: FixPot: phase or gain
//        FlucPot phase or gain
//        PotReg: Reg20 or Reg21
//        CompareTree: 3 IMR trace and results
// output: TREU or FALSE
//--------------------------------------------------------------------------------------------
R855_ErrCode R855_IQ_Tree_G7_MIXAMP(UINT8 FixPot, UINT8 FlucPot, UINT8 PotReg, R855_Sect_Type* CompareTree) //CD OK
{
	UINT8 TreeCunt  = 0;
	UINT8 PntReg = 0;
		
	//PntReg is reg to change; FlucPot is change value
	if(PotReg == R855_IMR_GAIN_REG_G7)
		PntReg = R855_IMR_PHASE_REG_G7; //phase control
	else
		PntReg = R855_IMR_GAIN_REG_G7; //gain control

	for(TreeCunt = 0; TreeCunt<3; TreeCunt ++)
	{
		R855_I2C.RegAddr = PotReg;
		R855_I2C.Data = FixPot;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = PntReg;
		R855_I2C.Data = FlucPot;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		if(R855_Muti_Read(&CompareTree[TreeCunt].Value) != R855_Success)
			return R855_Fail;
	
		if(PotReg == R855_IMR_GAIN_REG_G7)
		{
			CompareTree[TreeCunt].Gain_X  = FixPot;
			CompareTree[TreeCunt].Phase_Y = FlucPot;
		}
		else
		{
			CompareTree[TreeCunt].Phase_Y  = FixPot;
			CompareTree[TreeCunt].Gain_X = FlucPot;
		}
		
		if(TreeCunt == 0)   //try right-side point
			FlucPot ++; 
		else if(TreeCunt == 1) //try left-side point
		{
			if((FlucPot & 0x07) == 1) //if absolute location is 1, change I/Q direction (CD 0x0F->0x07)
			{
				if(FlucPot & 0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path (CD 0x10->0x08)
				{
					//FlucPot = (FlucPot & 0xE0) | 0x01;		
					FlucPot = (FlucPot & 0xF0) | 0x01;	//CD	
				}
				else
				{
					//FlucPot = (FlucPot & 0xE0) | 0x11;
					FlucPot = (FlucPot & 0xF0) | 0x09; //CD
					
				}
			}
			else
			{
				FlucPot = FlucPot - 2;
			}
				
		}
	}

	return R855_Success;
}




R855_ErrCode R855_IQ_Tree_L7_MIXAMP(UINT8 FixPot, UINT8 FlucPot, UINT8 PotReg, R855_Sect_Type* CompareTree) //CD OK
{
	UINT8 TreeCunt  = 0;
	UINT8 PntReg = 0;
		
	//PntReg is reg to change; FlucPot is change value
	/* Register is same, so do not to judge
	if(PotReg == R855_IMR_GAIN_REG_L7)
		PntReg = R855_IMR_PHASE_REG_L7; //phase control
	else
		PntReg = R855_IMR_GAIN_REG_L7; //gain control
*/
	PntReg = R855_IMR_GAIN_REG_L7; //phase control register = gain control register

	for(TreeCunt = 0; TreeCunt<3; TreeCunt ++)
	{
/*
		R855_I2C.RegAddr = PotReg;
		R855_I2C.Data = FixPot;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = PntReg;
		R855_I2C.Data = FlucPot;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
*/

		//if(PntReg == R855_IMR_GAIN_REG_L7)   //FlucPot is gain, FixPot is phase
		if(PotReg == 0)   //FlucPot is gain, FixPot is phase
		{
			R855_I2C.RegAddr = PntReg;
			R855_I2C.Data = (FlucPot & 0x0F) | (FixPot & 0xF0);
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;
		}
		else //FlucPot is phase, FixPot is gain
		{
			R855_I2C.RegAddr = PntReg;
			R855_I2C.Data = (FixPot & 0x0F) | (FlucPot & 0xF0);
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;
		}


		if(R855_Muti_Read(&CompareTree[TreeCunt].Value) != R855_Success)
			return R855_Fail;
	
		//if(PotReg == R855_IMR_GAIN_REG_L7)
		if(PotReg == 1)
		{
			CompareTree[TreeCunt].Gain_X  = FixPot;
			CompareTree[TreeCunt].Phase_Y = FlucPot;
		}
		else
		{
			CompareTree[TreeCunt].Phase_Y  = FixPot;
			CompareTree[TreeCunt].Gain_X = FlucPot;
		}
		
		if(TreeCunt == 0)   //try right-side point
		{
			//FlucPot ++; //CD Mixamp >7
			//if(PotReg == R855_IMR_GAIN_REG_L7)  //CD Mixamp <7
			if(PotReg == 1)
			{
				FlucPot = FlucPot + (1<<4);     //+1
			}
			else //PotReg == R855_IMR_PHASE_REG_L7
			{
				FlucPot ++; //CD Mixamp <7
			}
		}
		else if(TreeCunt == 1) //try left-side point
		{
			/* //CD Mixamp >7
			if((FlucPot & 0x07) == 1) //if absolute location is 1, change I/Q direction (CD 0x0F->0x07)
			{
				if(FlucPot & 0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path (CD 0x10->0x08)
				{
					//FlucPot = (FlucPot & 0xE0) | 0x01;		
					FlucPot = (FlucPot & 0xF0) | 0x01;	//CD	
				}
				else
				{
					//FlucPot = (FlucPot & 0xE0) | 0x11;
					FlucPot = (FlucPot & 0xF0) | 0x09; //CD
					
				}
			}
			else
			{
				FlucPot = FlucPot - 2;
			}*/
			//if(PotReg == R855_IMR_GAIN_REG_L7)  //CD Mixamp <7
			if(PotReg == 1)
			{
				//if((FlucPot & 0x07) == 1) //if absolute location is 1, change I/Q direction (CD 0x0F->0x07)
				if((FlucPot & 0x70) == 0x10) //if absolute location is 1, change I/Q direction (CD 0x0F->0x07)
				{
					//if(FlucPot & 0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path (CD 0x10->0x08)
					if(FlucPot & 0x80) //b[4]:I/Q selection. 0:Q-path, 1:I-path (CD 0x10->0x08)
					{
						//FlucPot = (FlucPot & 0xE0) | 0x01;		
						//FlucPot = (FlucPot & 0xF0) | 0x01;	//CD	Mixamp >7
						FlucPot = (FlucPot & 0x0F) | 0x10;	//CD	 Mixamp <7
					}
					else
					{
						//FlucPot = (FlucPot & 0xE0) | 0x11;
						//FlucPot = (FlucPot & 0xF0) | 0x09; //CD Mixamp >7
						FlucPot = (FlucPot & 0x0F) | 0x90; //CD Mixamp <7
						
					}
				}
				else
				{
					//FlucPot = FlucPot - 2; //CD Mixamp >7
					FlucPot = FlucPot-(2<<4); //CD Mixamp <7
				}
			}
			else //PotReg == R855_IMR_PHASE_REG_L7
			{
				if((FlucPot & 0x07) == 1) //if absolute location is 1, change I/Q direction (CD 0x0F->0x07)
				{
					if(FlucPot & 0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path (CD 0x10->0x08)
					{
						//FlucPot = (FlucPot & 0xE0) | 0x01;		
						FlucPot = (FlucPot & 0xF0) | 0x01;	//CD	
					}
					else
					{
						//FlucPot = (FlucPot & 0xE0) | 0x11;
						FlucPot = (FlucPot & 0xF0) | 0x09; //CD
					}
				}
				else
				{
					FlucPot = FlucPot - 2;
				}
			}
				
		}
	}

	return R855_Success;
}




R855_ErrCode R855_IQ_Tree5_G7_MIXAMP(UINT8 FixPot, UINT8 FlucPot, UINT8 PotReg, R855_Sect_Type* CompareTree) //CD OK
{
	UINT8 TreeCunt  = 0;
	UINT8 TreeTimes = 5;
	UINT8 TempPot   = 0;
	UINT8 PntReg = 0;
	UINT8 CompCunt = 0;
	R855_Sect_Type CorTemp[5];
    R855_Sect_Type Compare_Temp;
	UINT8 CuntTemp = 0;

	//memset(&Compare_Temp,0, sizeof(R855_Sect_Type));
	Compare_Temp.Gain_X = 0;
	Compare_Temp.Phase_Y = 0;
	Compare_Temp.Iqcap = 0;
	Compare_Temp.Value = 255;
		
	for(CompCunt=0; CompCunt<3; CompCunt++)
	{
		CorTemp[CompCunt].Gain_X = CompareTree[CompCunt].Gain_X;
		CorTemp[CompCunt].Phase_Y = CompareTree[CompCunt].Phase_Y;
		CorTemp[CompCunt].Value = CompareTree[CompCunt].Value;
	}

	//PntReg is reg to change; FlucPot is change value
	if(PotReg == R855_IMR_GAIN_REG_G7)
		PntReg = R855_IMR_PHASE_REG_G7; //phase control
	else
		PntReg = R855_IMR_GAIN_REG_G7; //gain control

	for(TreeCunt = 0; TreeCunt<TreeTimes; TreeCunt ++)
	{
		R855_I2C.RegAddr = PotReg;
		R855_I2C.Data = FixPot;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = PntReg;
		R855_I2C.Data = FlucPot;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		if(R855_Muti_Read(&CorTemp[TreeCunt].Value) != R855_Success)
			return R855_Fail;
	
		if(PotReg == R855_IMR_GAIN_REG_G7)
		{
			CorTemp[TreeCunt].Gain_X  = FixPot;
			CorTemp[TreeCunt].Phase_Y = FlucPot;
		}
		else
		{
			CorTemp[TreeCunt].Phase_Y  = FixPot;
			CorTemp[TreeCunt].Gain_X = FlucPot;
		}
		
		if(TreeCunt == 0)   //next try right-side 1 point
			FlucPot ++;     //+1
		else if(TreeCunt == 1)   //next try right-side 2 point
			FlucPot ++;     //1+1=2
		else if(TreeCunt == 2)   //next try left-side 1 point
		{
			//if((FlucPot & 0x0F) == 0x02) //if absolute location is 2, change I/Q direction and set to 1
			if((FlucPot & 0x07) == 0x02) //if absolute location is 2, change I/Q direction and set to 1    //CD
			{
				TempPot = 1;
				//if((FlucPot & 0x10)==0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path    
				if((FlucPot & 0x08)==0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path   //CD
				{
					//FlucPot = (FlucPot & 0xE0) | 0x01;  //Q1
					FlucPot = (FlucPot & 0xF0) | 0x01;  //Q1  CD
				}
				else
				{
					//FlucPot = (FlucPot & 0xE0) | 0x11;  //I1   
					FlucPot = (FlucPot & 0xF0) | 0x09;  //I1   CD
				}
			}
			else
				FlucPot -= 3;  //+2-3=-1
		}
		else if(TreeCunt == 3) //next try left-side 2 point
		{
			if(TempPot==1)  //been chnaged I/Q
			{
				FlucPot += 1;
			}
			//else if((FlucPot & 0x0F) == 0x00) //if absolute location is 0, change I/Q direction
			else if((FlucPot & 0x07) == 0x00) //if absolute location is 0, change I/Q direction   CD
			{
				TempPot = 1;
				//if((FlucPot & 0x10)==0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path
				if((FlucPot & 0x08)==0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path   CD
				{
					//FlucPot = (FlucPot & 0xE0) | 0x01;  //Q1
					FlucPot = (FlucPot & 0xF0) | 0x01;  //Q1  CD
				}
				else
				{
					//FlucPot = (FlucPot & 0xE0) | 0x11;  //I1
					FlucPot = (FlucPot & 0xF0) | 0x09;  //I1  CD
				}
			}
			else
				FlucPot -= 1;  //-1-1=-2
		}

		if(CorTemp[TreeCunt].Value < Compare_Temp.Value)
		{
		  Compare_Temp.Value = CorTemp[TreeCunt].Value;
		  Compare_Temp.Gain_X = CorTemp[TreeCunt].Gain_X;
		  Compare_Temp.Phase_Y = CorTemp[TreeCunt].Phase_Y;		
		  CuntTemp = TreeCunt; 
		}
/*
		if(TreeCunt == 0)   //try right-side point
			FlucPot ++; 
		else if(TreeCunt == 1) //try left-side point
		{
			if((FlucPot & 0x0F) == 1) //if absolute location is 1, change I/Q direction
			{
				if(FlucPot & 0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path
				{
					FlucPot = (FlucPot & 0xE0) | 0x01;			
				}
				else
				{
					FlucPot = (FlucPot & 0xE0) | 0x11;
				}
			}
			else
				FlucPot = FlucPot - 2;  
				
		}
*/
	}

	for(CompCunt=0; CompCunt<3; CompCunt++)
	{
		if(CuntTemp==3 || CuntTemp==4)
		{
			CompareTree[CompCunt].Gain_X = CorTemp[2+CompCunt].Gain_X;  //2,3,4
			CompareTree[CompCunt].Phase_Y = CorTemp[2+CompCunt].Phase_Y;
			CompareTree[CompCunt].Value = CorTemp[2+CompCunt].Value;
		}
		else
		{
			CompareTree[CompCunt].Gain_X = CorTemp[CompCunt].Gain_X;    //0,1,2
			CompareTree[CompCunt].Phase_Y = CorTemp[CompCunt].Phase_Y;
			CompareTree[CompCunt].Value = CorTemp[CompCunt].Value;
		}
		
	}

	return R855_Success;
}




R855_ErrCode R855_IQ_Tree5_L7_MIXAMP(UINT8 FixPot, UINT8 FlucPot, UINT8 PotReg, R855_Sect_Type* CompareTree) //CD OK
{
	UINT8 TreeCunt  = 0;
	UINT8 TreeTimes = 5;
	UINT8 TempPot   = 0;
	UINT8 PntReg = 0;
	UINT8 CompCunt = 0;
	R855_Sect_Type CorTemp[5];
    R855_Sect_Type Compare_Temp;
	UINT8 CuntTemp = 0;

	//memset(&Compare_Temp,0, sizeof(R855_Sect_Type));
	Compare_Temp.Gain_X = 0;
	Compare_Temp.Phase_Y = 0;
	Compare_Temp.Iqcap = 0;
	Compare_Temp.Value = 255;
		
	for(CompCunt=0; CompCunt<3; CompCunt++)
	{
		CorTemp[CompCunt].Gain_X = CompareTree[CompCunt].Gain_X;
		CorTemp[CompCunt].Phase_Y = CompareTree[CompCunt].Phase_Y;
		CorTemp[CompCunt].Value = CompareTree[CompCunt].Value;
	}

	//PntReg is reg to change; FlucPot is change value
/* Register is same, so do not to judge
	//if(PotReg == R855_IMR_GAIN_REG_L7)
	if(PotReg == 1)  //1:X(R855_IMR_GAIN_REG_L7), 0:Y(R855_IMR_PHASE_REG_L7)
		PntReg = R855_IMR_PHASE_REG_L7; //phase control
	else
		PntReg = R855_IMR_GAIN_REG_L7; //gain control
*/
	PntReg = R855_IMR_GAIN_REG_L7; //phase control register = gain control register

	for(TreeCunt = 0; TreeCunt<TreeTimes; TreeCunt ++)
	{
/*
		R855_I2C.RegAddr = PotReg;
		R855_I2C.Data = FixPot;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = PntReg;
		R855_I2C.Data = FlucPot;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
*/
		//if(PntReg == R855_IMR_GAIN_REG_L7)   //FlucPot is gain, FixPot is phase
		if(PotReg == 0)   //FlucPot is gain, FixPot is phase
		{
			R855_I2C.RegAddr = PntReg;
			R855_I2C.Data = (FlucPot & 0x0F) | (FixPot & 0xF0);
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;
		}
		else //FlucPot is phase, FixPot is gain
		{
			R855_I2C.RegAddr = PntReg;
			R855_I2C.Data = (FixPot & 0x0F) | (FlucPot & 0xF0);
			if(I2C_Write(&R855_I2C) != R855_Success)
				return R855_Fail;
		}


		if(R855_Muti_Read(&CorTemp[TreeCunt].Value) != R855_Success)
			return R855_Fail;
	
		//if(PotReg == R855_IMR_GAIN_REG_L7)
		if(PotReg == 1)
		{
			CorTemp[TreeCunt].Gain_X  = FixPot;
			CorTemp[TreeCunt].Phase_Y = FlucPot;
		}
		else
		{
			CorTemp[TreeCunt].Phase_Y  = FixPot;
			CorTemp[TreeCunt].Gain_X = FlucPot;
		}
		
		if(TreeCunt == 0)   //next try right-side 1 point
		{
			//FlucPot ++;     //+1   CD  Mixamp >7
			//if(PotReg == R855_IMR_GAIN_REG_L7)  //CD Mixamp <7
			if(PotReg == 1)  //CD Mixamp <7
				FlucPot = FlucPot + (1<<4);     //+1
			else   //PotReg == R855_IMR_PHASE_REG_L7
				FlucPot ++;

		}
		else if(TreeCunt == 1)   //next try right-side 2 point
		{
			//FlucPot ++;     //1+1=2  CD  Mixamp >7
			//if(PotReg == R855_IMR_GAIN_REG_L7)  //CD Mixamp <7
			if(PotReg == 1)  //CD Mixamp <7
				FlucPot = FlucPot + (1<<4);     //+1
			else   //R855_IMR_PHASE_REG_L7
				FlucPot ++;
		}
		else if(TreeCunt == 2)   //next try left-side 1 point
		{
			/*
			//if((FlucPot & 0x0F) == 0x02) //if absolute location is 2, change I/Q direction and set to 1
			if((FlucPot & 0x07) == 0x02) //if absolute location is 2, change I/Q direction and set to 1    //CD Mixamp >7
			{
				TempPot = 1;
				//if((FlucPot & 0x10)==0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path    
				if((FlucPot & 0x08)==0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path   //CD  Mixamp >7
				{
					//FlucPot = (FlucPot & 0xE0) | 0x01;  //Q1 
					FlucPot = (FlucPot & 0xF0) | 0x01;  //Q1  CD  Mixamp >7
				}
				else
				{
					//FlucPot = (FlucPot & 0xE0) | 0x11;  //I1   
					FlucPot = (FlucPot & 0xF0) | 0x09;  //I1   CD  Mixamp >7
				}
			}
			else
			{
				FlucPot -= 3;  //+2-3=-1
			}
			*/
			//if(PotReg == R855_IMR_GAIN_REG_L7)  //CD Mixamp <7
			if(PotReg == 1)  //CD Mixamp <7
			{
				//if((FlucPot & 0x07) == 0x02) //if absolute location is 2, change I/Q direction and set to 1    //CD Mixamp >7
				if((FlucPot & 0x70) == 0x20) //if absolute location is 2, change I/Q direction and set to 1    //CD Mixamp <7
				{
					TempPot = 1;
					//if((FlucPot & 0x10)==0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path    
					//if((FlucPot & 0x08)==0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path   //CD  Mixamp >7
					if((FlucPot & 0x80)==0x80) //b[4]:I/Q selection. 0:Q-path, 1:I-path   //CD  Mixamp <7
					{
						//FlucPot = (FlucPot & 0xE0) | 0x01;  //Q1 
						//FlucPot = (FlucPot & 0xF0) | 0x01;  //Q1  CD  Mixamp >7
						FlucPot = (FlucPot & 0x0F) | 0x10;  //Q1  CD  Mixamp <7
					}
					else
					{
						//FlucPot = (FlucPot & 0xE0) | 0x11;  //I1   
						//FlucPot = (FlucPot & 0xF0) | 0x09;  //I1   CD  Mixamp >7
						FlucPot = (FlucPot & 0x0F) | 0x90;  //I1   CD  Mixamp <7
					}
				}
				else
				{
					//FlucPot -= 3;  //+2-3=-1    CD  Mixamp >7
					FlucPot = FlucPot - (3<<4);  //+2-3=-1    CD  Mixamp <7
				}
			}
			else   //PotReg == R855_IMR_PHASE_REG_L7
			{
				if((FlucPot & 0x07) == 0x02) //if absolute location is 2, change I/Q direction and set to 1   
				{
					TempPot = 1;
					//if((FlucPot & 0x10)==0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path    
					if((FlucPot & 0x08)==0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path   //CD  Mixamp >7
					{
						//FlucPot = (FlucPot & 0xE0) | 0x01;  //Q1 
						FlucPot = (FlucPot & 0xF0) | 0x01;  //Q1  CD  Mixamp >7
					}
					else
					{
						//FlucPot = (FlucPot & 0xE0) | 0x11;  //I1   
						FlucPot = (FlucPot & 0xF0) | 0x09;  //I1   CD  Mixamp >7
					}
				}
				else
				{
					FlucPot -= 3;  //+2-3=-1
				}
			}
		}
		else if(TreeCunt == 3) //next try left-side 2 point
		{
			/*if(TempPot==1)  //been chnaged I/Q
			{
				FlucPot += 1;
			}
			//else if((FlucPot & 0x0F) == 0x00) //if absolute location is 0, change I/Q direction
			else if((FlucPot & 0x07) == 0x00) //if absolute location is 0, change I/Q direction   CD
			{
				TempPot = 1;
				//if((FlucPot & 0x10)==0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path
				if((FlucPot & 0x08)==0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path   CD
				{
					//FlucPot = (FlucPot & 0xE0) | 0x01;  //Q1
					FlucPot = (FlucPot & 0xF0) | 0x01;  //Q1  CD
				}
				else
				{
					//FlucPot = (FlucPot & 0xE0) | 0x11;  //I1
					FlucPot = (FlucPot & 0xF0) | 0x09;  //I1  CD
				}
			}
			else
			{
				FlucPot -= 1;  //-1-1=-2
			}*/

			//if(PotReg == R855_IMR_GAIN_REG_L7)  //CD Mixamp <7
			if(PotReg == 1)  //CD Mixamp <7
			{
				if(TempPot==1)  //been chnaged I/Q
				{
					//FlucPot += 1;  //CD Mixamp >7
					FlucPot = FlucPot + (1<<4);  //CD Mixamp <7

				}
				//else if((FlucPot & 0x0F) == 0x00) //if absolute location is 0, change I/Q direction
				//else if((FlucPot & 0x07) == 0x00) //if absolute location is 0, change I/Q direction   CD  //CD Mixamp >7
				else if((FlucPot & 0x70) == 0x00) //if absolute location is 0, change I/Q direction   CD  //CD Mixamp <7
				{
					TempPot = 1;
					//if((FlucPot & 0x10)==0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path
					//if((FlucPot & 0x08)==0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path   CD  Mixamp >7
					if((FlucPot & 0x80)==0x80) //b[4]:I/Q selection. 0:Q-path, 1:I-path   CD  Mixamp <7
					{
						//FlucPot = (FlucPot & 0xE0) | 0x01;  //Q1
						//FlucPot = (FlucPot & 0xF0) | 0x01;  //Q1  CD Mixamp >7
						FlucPot = (FlucPot & 0x0F) | 0x10;  //Q1  CD Mixamp >7
					}
					else
					{
						//FlucPot = (FlucPot & 0xE0) | 0x11;  //I1
						//FlucPot = (FlucPot & 0xF0) | 0x09;  //I1  CD Mixamp >7
						FlucPot = (FlucPot & 0x0F) | 0x90;  //I1  CD Mixamp <7
					}
				}
				else
				{
					//FlucPot -= 1;  //-1-1=-2  CD Mixamp >7
					FlucPot = FlucPot-(1<<4);  //-1-1=-2  CD Mixamp >7
				}
			}
			else //PotReg == R855_IMR_PHASE_REG_L7
			{
				if(TempPot==1)  //been chnaged I/Q
				{
					FlucPot += 1;
				}
				//else if((FlucPot & 0x0F) == 0x00) //if absolute location is 0, change I/Q direction
				else if((FlucPot & 0x07) == 0x00) //if absolute location is 0, change I/Q direction   CD
				{
					TempPot = 1;
					//if((FlucPot & 0x10)==0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path
					if((FlucPot & 0x08)==0x08) //b[4]:I/Q selection. 0:Q-path, 1:I-path   CD
					{
						//FlucPot = (FlucPot & 0xE0) | 0x01;  //Q1
						FlucPot = (FlucPot & 0xF0) | 0x01;  //Q1  CD
					}
					else
					{
						//FlucPot = (FlucPot & 0xE0) | 0x11;  //I1
						FlucPot = (FlucPot & 0xF0) | 0x09;  //I1  CD
					}
				}
				else
				{
					FlucPot -= 1;  //-1-1=-2
				}
			}
		}

		if(CorTemp[TreeCunt].Value < Compare_Temp.Value)
		{
		  Compare_Temp.Value = CorTemp[TreeCunt].Value;
		  Compare_Temp.Gain_X = CorTemp[TreeCunt].Gain_X;
		  Compare_Temp.Phase_Y = CorTemp[TreeCunt].Phase_Y;		
		  CuntTemp = TreeCunt; 
		}
/*
		if(TreeCunt == 0)   //try right-side point
			FlucPot ++; 
		else if(TreeCunt == 1) //try left-side point
		{
			if((FlucPot & 0x0F) == 1) //if absolute location is 1, change I/Q direction
			{
				if(FlucPot & 0x10) //b[4]:I/Q selection. 0:Q-path, 1:I-path
				{
					FlucPot = (FlucPot & 0xE0) | 0x01;			
				}
				else
				{
					FlucPot = (FlucPot & 0xE0) | 0x11;
				}
			}
			else
				FlucPot = FlucPot - 2;  
				
		}
*/
	}

	for(CompCunt=0; CompCunt<3; CompCunt++)
	{
		if(CuntTemp==3 || CuntTemp==4)
		{
			CompareTree[CompCunt].Gain_X = CorTemp[2+CompCunt].Gain_X;  //2,3,4
			CompareTree[CompCunt].Phase_Y = CorTemp[2+CompCunt].Phase_Y;
			CompareTree[CompCunt].Value = CorTemp[2+CompCunt].Value;
		}
		else
		{
			CompareTree[CompCunt].Gain_X = CorTemp[CompCunt].Gain_X;    //0,1,2
			CompareTree[CompCunt].Phase_Y = CorTemp[CompCunt].Phase_Y;
			CompareTree[CompCunt].Value = CorTemp[CompCunt].Value;
		}
		
	}

	return R855_Success;
}




//-----------------------------------------------------------------------------------/ 
// Purpose: compare IMR result aray [0][1][2], find min value and store to CorArry[0]
// input: CorArry: three IMR data array
// output: TRUE or FALSE
//-----------------------------------------------------------------------------------/
R855_ErrCode R855_CompreCor(R855_Sect_Type* CorArry)    //CD OK  mixamp>8 and <8 is same
{
	UINT8 CompCunt = 0;
	R855_Sect_Type CorTemp;

	for(CompCunt=3; CompCunt > 0; CompCunt --)
	{
		if(CorArry[0].Value > CorArry[CompCunt - 1].Value) //compare IMR result [0][1][2], find min value
		{
			CorTemp = CorArry[0];
			CorArry[0] = CorArry[CompCunt - 1];
			CorArry[CompCunt - 1] = CorTemp;
		}
	}

	return R855_Success;
}


//-------------------------------------------------------------------------------------//
// Purpose: if (Gain<9 or Phase<9), Gain+1 or Phase+1 and compare with min value
//          new < min => update to min and continue
//          new > min => Exit
// input: StepArry: three IMR data array
//        Pace: gain or phase register
// output: TRUE or FALSE 
//-------------------------------------------------------------------------------------//
R855_ErrCode R855_CompreStep_G7_MIXAMP(R855_Sect_Type* StepArry, UINT8 Pace)  //CD OK Mixamp>7
{
	R855_Sect_Type StepTemp;
	
	//min value already saved in StepArry[0]
	StepTemp.Phase_Y = StepArry[0].Phase_Y;  //whole byte data
	StepTemp.Gain_X = StepArry[0].Gain_X;
	//StepTemp.Iqcap  = StepArry[0].Iqcap;

	//while(((StepTemp.Gain_X & 0x0F) < R855_IMR_TRIAL) && ((StepTemp.Phase_Y & 0x0F) < R855_IMR_TRIAL))  
	while(((StepTemp.Gain_X & 0x07) < R855_IMR_TRIAL) && ((StepTemp.Phase_Y & 0x07) < R855_IMR_TRIAL))     //CD
	{
		if(Pace == R855_IMR_GAIN_REG_G7)
			StepTemp.Gain_X ++;
		else
			StepTemp.Phase_Y ++;
	
		R855_I2C.RegAddr = R855_IMR_GAIN_REG_G7;
		R855_I2C.Data = StepTemp.Gain_X ;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = R855_IMR_PHASE_REG_G7;
		R855_I2C.Data = StepTemp.Phase_Y;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		if(R855_Muti_Read(&StepTemp.Value) != R855_Success)
			return R855_Fail;

		if(StepTemp.Value <= StepArry[0].Value)
		{
			StepArry[0].Gain_X  = StepTemp.Gain_X;
			StepArry[0].Phase_Y = StepTemp.Phase_Y;
			//StepArry[0].Iqcap = StepTemp.Iqcap;
			StepArry[0].Value   = StepTemp.Value;
		}
		else if((StepTemp.Value - 2*R855_ADC_READ_COUNT) > StepArry[0].Value)
		{
			break;		
		}
		
	} //end of while()
	
	return R855_Success;
}
//-------------------------------------------------------------------------------------//
// Purpose: if (Gain<9 or Phase<9), Gain+1 or Phase+1 and compare with min value
//          new < min => update to min and continue
//          new > min => Exit
// input: StepArry: three IMR data array
//        Pace: gain or phase register
// output: TRUE or FALSE 
//-------------------------------------------------------------------------------------//
R855_ErrCode R855_CompreStep_L7_MIXAMP(R855_Sect_Type* StepArry, UINT8 Pace)  //CD OK Mixamp<7
{
	R855_Sect_Type StepTemp;
	
	//min value already saved in StepArry[0]
	StepTemp.Phase_Y = StepArry[0].Phase_Y;  //whole byte data
	StepTemp.Gain_X = StepArry[0].Gain_X;
	//StepTemp.Iqcap  = StepArry[0].Iqcap;

	//while(((StepTemp.Gain_X & 0x0F) < R855_IMR_TRIAL) && ((StepTemp.Phase_Y & 0x0F) < R855_IMR_TRIAL))  
	//while(((StepTemp.Gain_X & 0x07) < R855_IMR_TRIAL) && ((StepTemp.Phase_Y & 0x07) < R855_IMR_TRIAL))     //CD   >7
	while(((StepTemp.Gain_X & 0x07) < R855_IMR_TRIAL) && (((StepTemp.Phase_Y & 0x70)>>4) < R855_IMR_TRIAL))     //CD   <7
	{
		//if(Pace == R855_IMR_GAIN_REG_L7)
		if(Pace == 1)   //1:X(R855_IMR_GAIN_REG_L7), 0:Y(R855_IMR_PHASE_REG_L7)
			StepTemp.Gain_X ++;
		else
			//StepTemp.Phase_Y ++;   //CD Mixeramp >7
			StepTemp.Phase_Y = StepTemp.Phase_Y + (1<<4);   //CD Mixeramp >7
/*
		R855_I2C.RegAddr = R855_IMR_GAIN_REG_L7;
		R855_I2C.Data = StepTemp.Gain_X ;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = R855_IMR_PHASE_REG_L7;
		R855_I2C.Data = StepTemp.Phase_Y;
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
*/
		R855_I2C.RegAddr = R855_IMR_GAIN_REG_L7;
	    R855_I2C.Data = (StepTemp.Gain_X & 0x0F) | (StepTemp.Phase_Y & 0xF0);
	    if(I2C_Write(&R855_I2C) != R855_Success)
		  return R855_Fail;

		if(R855_Muti_Read(&StepTemp.Value) != R855_Success)
			return R855_Fail;

		if(StepTemp.Value <= StepArry[0].Value)
		{
			StepArry[0].Gain_X  = StepTemp.Gain_X;
			StepArry[0].Phase_Y = StepTemp.Phase_Y;
			//StepArry[0].Iqcap = StepTemp.Iqcap;
			StepArry[0].Value   = StepTemp.Value;
		}
		else if((StepTemp.Value - 2*R855_ADC_READ_COUNT) > StepArry[0].Value)
		{
			break;		
		}
		
	} //end of while()
	
	return R855_Success;
}

//-----------------------------------------------------------------------------------/ 
// Purpose: read multiple IMR results for stability
// input: IMR_Reg: IMR result address
//        IMR_Result_Data: result 
// output: TRUE or FALSE
//-----------------------------------------------------------------------------------/
R855_ErrCode R855_Muti_Read(UINT8* IMR_Result_Data) //CD OK
{
    R855_Delay_MS(R855_ADC_READ_DELAY * 1000);//2	// by ITE

	R855_I2C_Len.RegAddr = 0;
	R855_I2C_Len.Len = 2;              // read 0x01
	if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
	{
		I2C_Read_Len(&R855_I2C_Len);
	}

	*IMR_Result_Data = (R855_I2C_Len.Data[1] & 0x3F);

	return R855_Success;
}

 
R855_ErrCode R855_Section_G7_MIXAMP(R855_Sect_Type* IQ_Pont)  //CD OK
{
	R855_Sect_Type Compare_IQ[3];
	R855_Sect_Type Compare_Bet[3];

	//Try X-1 column and save min result to Compare_Bet[0]
	//if((IQ_Pont->Gain_X & 0x0F) == 0x00)
	if((IQ_Pont->Gain_X & 0x07) == 0x00) //CD
	{
		//Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) & 0xEF) + 1;  //Q-path, Gain=1
		Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) & 0xF7) + 1;  //Q-path, Gain=1   CD
	}
	else
	{
		Compare_IQ[0].Gain_X  = IQ_Pont->Gain_X - 1;  //left point
	}
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	if(R855_IQ_Tree_G7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_G7, &Compare_IQ[0]) != R855_Success)  // y-direction
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[0].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[0].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[0].Value = Compare_IQ[0].Value;

	//Try X column and save min result to Compare_Bet[1]
	Compare_IQ[0].Gain_X = IQ_Pont->Gain_X;
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	if(R855_IQ_Tree_G7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_G7, &Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[1].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[1].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[1].Value = Compare_IQ[0].Value;

	//Try X+1 column and save min result to Compare_Bet[2]
	//if((IQ_Pont->Gain_X & 0x0F) == 0x00)		
	if((IQ_Pont->Gain_X & 0x07) == 0x00)	//CD	
		//Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) | 0x10) + 1;  //I-path, Gain=1
		Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) | 0x80) + 1;  //I-path, Gain=1   CD
	else
	    Compare_IQ[0].Gain_X = IQ_Pont->Gain_X + 1;
    
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	if(R855_IQ_Tree_G7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_G7, &Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[2].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[2].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[2].Value = Compare_IQ[0].Value;

	if(R855_CompreCor(&Compare_Bet[0]) != R855_Success)
		return R855_Fail;

	*IQ_Pont = Compare_Bet[0];
	
	return R855_Success;
}
R855_ErrCode R855_Section_L7_MIXAMP(R855_Sect_Type* IQ_Pont)  //CD OK
{
	R855_Sect_Type Compare_IQ[3];
	R855_Sect_Type Compare_Bet[3];

	//Try X-1 column and save min result to Compare_Bet[0]
	//if((IQ_Pont->Gain_X & 0x0F) == 0x00)
	if((IQ_Pont->Gain_X & 0x07) == 0x00) //CD
	{
		//Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) & 0xEF) + 1;  //Q-path, Gain=1
		Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) & 0xF7) + 1;  //Q-path, Gain=1   CD
	}
	else
	{
		Compare_IQ[0].Gain_X  = IQ_Pont->Gain_X - 1;  //left point
	}
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	//if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_L7, &Compare_IQ[0]) != R855_Success)  // y-direction
	if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, 1, &Compare_IQ[0]) != R855_Success)  // y-direction
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[0].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[0].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[0].Value = Compare_IQ[0].Value;

	//Try X column and save min result to Compare_Bet[1]
	Compare_IQ[0].Gain_X = IQ_Pont->Gain_X;
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	//if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_L7, &Compare_IQ[0]) != R855_Success)
	if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, 1, &Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[1].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[1].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[1].Value = Compare_IQ[0].Value;

	//Try X+1 column and save min result to Compare_Bet[2]
	//if((IQ_Pont->Gain_X & 0x0F) == 0x00)		
	if((IQ_Pont->Gain_X & 0x07) == 0x00)	//CD	
		//Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) | 0x10) + 1;  //I-path, Gain=1
		Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) | 0x80) + 1;  //I-path, Gain=1   CD
	else
	    Compare_IQ[0].Gain_X = IQ_Pont->Gain_X + 1;
    
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	//if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_L7, &Compare_IQ[0]) != R855_Success)
	if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, 1, &Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[2].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[2].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[2].Value = Compare_IQ[0].Value;

	if(R855_CompreCor(&Compare_Bet[0]) != R855_Success)
		return R855_Fail;

	*IQ_Pont = Compare_Bet[0];
	
	return R855_Success;
}

R855_ErrCode R855_IMR_Cross_G7_MIXAMP(R855_Sect_Type* IQ_Pont, UINT8* X_Direct)   //CD OK Mixer gain >7
{
	R855_Sect_Type Compare_Cross[9]; //(0,0)(0,Q-1)(0,I-1)(Q-1,0)(I-1,0)+(0,Q-2)(0,I-2)(Q-2,0)(I-2,0)
	R855_Sect_Type Compare_Temp;
	UINT8 CrossCount = 0;
	//UINT8 RegGain = R855_Array[R855_IMR_GAIN_REG_G7] & 0xE0;
	//UINT8 RegPhase = R855_Array[R855_IMR_PHASE_REG_G7] & 0xE0;
	UINT8 RegGain = R855_Array[R855_IMR_GAIN_REG_G7] & 0xF0;
	UINT8 RegPhase = R855_Array[R855_IMR_PHASE_REG_G7] & 0xF0;

	Compare_Temp.Gain_X = 0;
	Compare_Temp.Phase_Y = 0;
	Compare_Temp.Iqcap = 0;
	Compare_Temp.Value = 255;

	for(CrossCount=0; CrossCount<9; CrossCount++)
	{
		if(CrossCount==0)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
		else if(CrossCount==1)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;       //0
		  Compare_Cross[CrossCount].Phase_Y = RegPhase + 1;  //Q-1
		}
		else if(CrossCount==2)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;               //0
		  //Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x10) + 1; //I-1
		  Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x08) + 1; //I-1   CD
		}
		else if(CrossCount==3)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain + 1; //Q-1
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
		else if(CrossCount==4)
		{
		  //Compare_Cross[CrossCount].Gain_X = (RegGain | 0x10) + 1; //I-1
		  Compare_Cross[CrossCount].Gain_X = (RegGain | 0x08) + 1; //I-1   CD
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
		else if(CrossCount==5)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;       //0
		  Compare_Cross[CrossCount].Phase_Y = RegPhase + 2;  //Q-2
		}
		else if(CrossCount==6)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;               //0
		  //Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x10) + 2; //I-2
		  Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x08) + 2; //I-2    CD
		}
		else if(CrossCount==7)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain + 2; //Q-2
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
		else if(CrossCount==8)
		{
		  //Compare_Cross[CrossCount].Gain_X = (RegGain | 0x10) + 2; //I-2
		  Compare_Cross[CrossCount].Gain_X = (RegGain | 0x08) + 2; //I-2   CD
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}

    	R855_I2C.RegAddr = R855_IMR_GAIN_REG_G7;
	    R855_I2C.Data = Compare_Cross[CrossCount].Gain_X;
	    if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

	    R855_I2C.RegAddr = R855_IMR_PHASE_REG_G7;
	    R855_I2C.Data = Compare_Cross[CrossCount].Phase_Y;
	    if(I2C_Write(&R855_I2C) != R855_Success)
		  return R855_Fail;
	
        if(R855_Muti_Read(&Compare_Cross[CrossCount].Value) != R855_Success)
		  return R855_Fail;

		//store (0,0) value
		if(CrossCount==0)
		{
			R855_IMR_Q0_Value[R855_IMR_MEM_GLOBAL] = Compare_Cross[CrossCount].Value;
		}
		if( Compare_Cross[CrossCount].Value < Compare_Temp.Value)
		{
		  Compare_Temp.Value = Compare_Cross[CrossCount].Value;
		  Compare_Temp.Gain_X = Compare_Cross[CrossCount].Gain_X;
		  Compare_Temp.Phase_Y = Compare_Cross[CrossCount].Phase_Y;		
		}
	} //end for loop


    //if(((Compare_Temp.Phase_Y & 0x1F)==0x01) || (Compare_Temp.Phase_Y & 0x1F)==0x02)  //phase Q1 or Q2
    if(((Compare_Temp.Phase_Y & 0x0F)==0x01) || (Compare_Temp.Phase_Y & 0x0F)==0x02)  //phase Q1 or Q2    CD
	{
      *X_Direct = (UINT8) 0;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;    //0
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y; //0
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[1].Gain_X;    //0
	  IQ_Pont[1].Phase_Y = Compare_Cross[1].Phase_Y; //Q1
	  IQ_Pont[1].Value = Compare_Cross[1].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[5].Gain_X;   //0
	  IQ_Pont[2].Phase_Y = Compare_Cross[5].Phase_Y;//Q2
	  IQ_Pont[2].Value = Compare_Cross[5].Value;
	}
	//else if(((Compare_Temp.Phase_Y & 0x1F)==0x11) || (Compare_Temp.Phase_Y & 0x1F)==0x12)  //phase I1 or I2
	else if(((Compare_Temp.Phase_Y & 0x0F)==0x09) || (Compare_Temp.Phase_Y & 0x0F)==0x0A)  //phase I1 or I2    CD
	{
      *X_Direct = (UINT8) 0;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;    //0
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y; //0
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[2].Gain_X;    //0
	  IQ_Pont[1].Phase_Y = Compare_Cross[2].Phase_Y; //Q1
	  IQ_Pont[1].Value = Compare_Cross[2].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[6].Gain_X;   //0
	  IQ_Pont[2].Phase_Y = Compare_Cross[6].Phase_Y;//Q2
	  IQ_Pont[2].Value = Compare_Cross[6].Value;
	}
	//else if(((Compare_Temp.Gain_X & 0x1F)==0x01) || (Compare_Temp.Gain_X & 0x1F)==0x02)  //gain Q1 or Q2
	else if(((Compare_Temp.Gain_X & 0x0F)==0x01) || (Compare_Temp.Gain_X & 0x0F)==0x02)  //gain Q1 or Q2   CD
	{
      *X_Direct = (UINT8) 1;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;    //0
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y; //0
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[3].Gain_X;    //Q1
	  IQ_Pont[1].Phase_Y = Compare_Cross[3].Phase_Y; //0
	  IQ_Pont[1].Value = Compare_Cross[3].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[7].Gain_X;   //Q2
	  IQ_Pont[2].Phase_Y = Compare_Cross[7].Phase_Y;//0
	  IQ_Pont[2].Value = Compare_Cross[7].Value;
	}
	//else if(((Compare_Temp.Gain_X & 0x1F)==0x11) || (Compare_Temp.Gain_X & 0x1F)==0x12)  //gain I1 or I2
	else if(((Compare_Temp.Gain_X & 0x0F)==0x09) || (Compare_Temp.Gain_X & 0x0F)==0x0A)  //gain I1 or I2
	{
      *X_Direct = (UINT8) 1;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;    //0
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y; //0
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[4].Gain_X;    //I1
	  IQ_Pont[1].Phase_Y = Compare_Cross[4].Phase_Y; //0
	  IQ_Pont[1].Value = Compare_Cross[4].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[8].Gain_X;   //I2
	  IQ_Pont[2].Phase_Y = Compare_Cross[8].Phase_Y;//0
	  IQ_Pont[2].Value = Compare_Cross[8].Value;
	}
	else //(0,0) 
	{	
	  *X_Direct = (UINT8) 1;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y;
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[3].Gain_X;    //Q1
	  IQ_Pont[1].Phase_Y = Compare_Cross[3].Phase_Y; //0
	  IQ_Pont[1].Value = Compare_Cross[3].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[4].Gain_X;   //I1
	  IQ_Pont[2].Phase_Y = Compare_Cross[4].Phase_Y; //0
	  IQ_Pont[2].Value = Compare_Cross[4].Value;
	}
	return R855_Success;
}


R855_ErrCode R855_IMR_Cross_L7_MIXAMP(R855_Sect_Type* IQ_Pont, UINT8* X_Direct)   //CD OK  Mixer gain < 7
{

	R855_Sect_Type Compare_Cross[9]; //(0,0)(0,Q-1)(0,I-1)(Q-1,0)(I-1,0)+(0,Q-2)(0,I-2)(Q-2,0)(I-2,0)
	R855_Sect_Type Compare_Temp;
	UINT8 CrossCount = 0;
	//UINT8 RegGain = R855_Array[R855_IMR_GAIN_REG_G7] & 0xE0;
	//UINT8 RegPhase = R855_Array[R855_IMR_PHASE_REG_G7] & 0xE0;
	UINT8 RegGain = R855_Array[R855_IMR_GAIN_REG_L7] & 0xF0;
	UINT8 RegPhase = R855_Array[R855_IMR_PHASE_REG_L7] & 0x0F;

	Compare_Temp.Gain_X = 0;
	Compare_Temp.Phase_Y = 0;
	Compare_Temp.Iqcap = 0;
	Compare_Temp.Value = 255;

	for(CrossCount=0; CrossCount<9; CrossCount++)
	{
		if(CrossCount==0)//(0,0) 
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
		else if(CrossCount==1)//(0,Q-1)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;       //0
		  //Compare_Cross[CrossCount].Phase_Y = RegPhase + 1;  //Q-1    CD mix>7
		  Compare_Cross[CrossCount].Phase_Y = RegPhase + (1<<4);  //Q-1  CD mix<7
		}
		else if(CrossCount==2)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;               //0
		  //Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x10) + 1; //I-1  CD >7
		  Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x80) + (1<<4); //I-1   CD <7

		}
		else if(CrossCount==3)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain + 1; //Q-1
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
		else if(CrossCount==4)
		{
		  //Compare_Cross[CrossCount].Gain_X = (RegGain | 0x10) + 1; //I-1
		  Compare_Cross[CrossCount].Gain_X = (RegGain | 0x08) + 1; //I-1   CD >7
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
		else if(CrossCount==5)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;       //0
		  //Compare_Cross[CrossCount].Phase_Y = RegPhase + 2;  //Q-2   CD >7
		  Compare_Cross[CrossCount].Phase_Y = RegPhase + (2<<4);  //Q-2   CD <7

		}
		else if(CrossCount==6)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain;               //0
		  //Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x10) + 2; //I-2
		  //Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x08) + 2; //I-2    CD  >7
		  Compare_Cross[CrossCount].Phase_Y = (RegPhase | 0x80) + (2<<4); //I-2    CD  <7
		}
		else if(CrossCount==7)
		{
		  Compare_Cross[CrossCount].Gain_X = RegGain + 2; //Q-2
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
		else if(CrossCount==8)
		{
		  //Compare_Cross[CrossCount].Gain_X = (RegGain | 0x10) + 2; //I-2
		  Compare_Cross[CrossCount].Gain_X = (RegGain | 0x08) + 2; //I-2   CD
		  Compare_Cross[CrossCount].Phase_Y = RegPhase;
		}
/*
    	R855_I2C.RegAddr = R855_IMR_GAIN_REG_L7;
	    R855_I2C.Data = Compare_Cross[CrossCount].Gain_X;
	    if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

	    R855_I2C.RegAddr = R855_IMR_PHASE_REG_L7;
	    R855_I2C.Data = Compare_Cross[CrossCount].Phase_Y;
	    if(I2C_Write(&R855_I2C) != R855_Success)
		  return R855_Fail;
*/
		R855_I2C.RegAddr = R855_IMR_GAIN_REG_L7;
	    R855_I2C.Data = (Compare_Cross[CrossCount].Gain_X & 0x0F) | (Compare_Cross[CrossCount].Phase_Y & 0xF0);
	    if(I2C_Write(&R855_I2C) != R855_Success)
		  return R855_Fail;


        if(R855_Muti_Read(&Compare_Cross[CrossCount].Value) != R855_Success)
		  return R855_Fail;

		//store (0,0) value
		if(CrossCount==0)
		{
			R855_IMR_Q0_Value[R855_IMR_MEM_GLOBAL] = Compare_Cross[CrossCount].Value;
		}

		if( Compare_Cross[CrossCount].Value < Compare_Temp.Value)
		{
		  Compare_Temp.Value = Compare_Cross[CrossCount].Value;
		  Compare_Temp.Gain_X = Compare_Cross[CrossCount].Gain_X;
		  Compare_Temp.Phase_Y = Compare_Cross[CrossCount].Phase_Y;		
		}
	} //end for loop


    //if(((Compare_Temp.Phase_Y & 0x1F)==0x01) || (Compare_Temp.Phase_Y & 0x1F)==0x02)  //phase Q1 or Q2
    //if(((Compare_Temp.Phase_Y & 0x0F)==0x01) || (Compare_Temp.Phase_Y & 0x0F)==0x02)  //phase Q1 or Q2    CD  >7
	if(((Compare_Temp.Phase_Y & 0xF0)==0x10) || (Compare_Temp.Phase_Y & 0xF0)==0x20)  //phase Q1 or Q2    CD  <7
	{
      *X_Direct = (UINT8) 0;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;    //0
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y; //0
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[1].Gain_X;    //0
	  IQ_Pont[1].Phase_Y = Compare_Cross[1].Phase_Y; //Q1
	  IQ_Pont[1].Value = Compare_Cross[1].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[5].Gain_X;   //0
	  IQ_Pont[2].Phase_Y = Compare_Cross[5].Phase_Y;//Q2
	  IQ_Pont[2].Value = Compare_Cross[5].Value;
	}
	//else if(((Compare_Temp.Phase_Y & 0x1F)==0x11) || (Compare_Temp.Phase_Y & 0x1F)==0x12)  //phase I1 or I2
	//else if(((Compare_Temp.Phase_Y & 0x0F)==0x09) || (Compare_Temp.Phase_Y & 0x0F)==0x0A)  //phase I1 or I2    CD >7
	else if(((Compare_Temp.Phase_Y & 0xF0)==0x90) || (Compare_Temp.Phase_Y & 0xF0)==0xA0)  //phase I1 or I2    CD <7
	{
      *X_Direct = (UINT8) 0;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;    //0
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y; //0
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[2].Gain_X;    //0
	  IQ_Pont[1].Phase_Y = Compare_Cross[2].Phase_Y; //Q1
	  IQ_Pont[1].Value = Compare_Cross[2].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[6].Gain_X;   //0
	  IQ_Pont[2].Phase_Y = Compare_Cross[6].Phase_Y;//Q2
	  IQ_Pont[2].Value = Compare_Cross[6].Value;
	}
	//else if(((Compare_Temp.Gain_X & 0x1F)==0x01) || (Compare_Temp.Gain_X & 0x1F)==0x02)  //gain Q1 or Q2
	//else if(((Compare_Temp.Gain_X & 0x0F)==0x01) || (Compare_Temp.Gain_X & 0x0F)==0x02)  //gain Q1 or Q2   CD >7
	else if(((Compare_Temp.Gain_X & 0x0F)==0x01) || (Compare_Temp.Gain_X & 0x0F)==0x02)  //gain Q1 or Q2   CD <7
	{
      *X_Direct = (UINT8) 1;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;    //0
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y; //0
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[3].Gain_X;    //Q1
	  IQ_Pont[1].Phase_Y = Compare_Cross[3].Phase_Y; //0
	  IQ_Pont[1].Value = Compare_Cross[3].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[7].Gain_X;   //Q2
	  IQ_Pont[2].Phase_Y = Compare_Cross[7].Phase_Y;//0
	  IQ_Pont[2].Value = Compare_Cross[7].Value;
	}
	//else if(((Compare_Temp.Gain_X & 0x1F)==0x11) || (Compare_Temp.Gain_X & 0x1F)==0x12)  //gain I1 or I2
	//else if(((Compare_Temp.Gain_X & 0x0F)==0x09) || (Compare_Temp.Gain_X & 0x0F)==0x0A)  //gain I1 or I2 CD>7
	else if(((Compare_Temp.Gain_X & 0x0F)==0x09) || (Compare_Temp.Gain_X & 0x0F)==0x0A)  //gain I1 or I2 CD<7
	{
      *X_Direct = (UINT8) 1;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;    //0
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y; //0
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[4].Gain_X;    //I1
	  IQ_Pont[1].Phase_Y = Compare_Cross[4].Phase_Y; //0
	  IQ_Pont[1].Value = Compare_Cross[4].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[8].Gain_X;   //I2
	  IQ_Pont[2].Phase_Y = Compare_Cross[8].Phase_Y;//0
	  IQ_Pont[2].Value = Compare_Cross[8].Value;
	}
	else //(0,0) 
	{	
	  *X_Direct = (UINT8) 1;
	  IQ_Pont[0].Gain_X = Compare_Cross[0].Gain_X;
	  IQ_Pont[0].Phase_Y = Compare_Cross[0].Phase_Y;
	  IQ_Pont[0].Value = Compare_Cross[0].Value;

	  IQ_Pont[1].Gain_X = Compare_Cross[3].Gain_X;    //Q1
	  IQ_Pont[1].Phase_Y = Compare_Cross[3].Phase_Y; //0
	  IQ_Pont[1].Value = Compare_Cross[3].Value;

	  IQ_Pont[2].Gain_X = Compare_Cross[4].Gain_X;   //I1
	  IQ_Pont[2].Phase_Y = Compare_Cross[4].Phase_Y; //0
	  IQ_Pont[2].Value = Compare_Cross[4].Value;
	}
	return R855_Success;
}


//----------------------------------------------------------------------------------------//
// purpose: search surrounding points from previous point 
//          try (x-1), (x), (x+1) columns, and find min IMR result point
// input: IQ_Pont: previous point data(IMR Gain, Phase, ADC Result, RefRreq)
//                 will be updated to final best point                 
// output: TRUE or FALSE
//----------------------------------------------------------------------------------------//
R855_ErrCode R855_F_IMR_G7_MIXAMP(R855_Sect_Type* IQ_Pont)  //CD OK
{
	R855_Sect_Type Compare_IQ[3];
	R855_Sect_Type Compare_Bet[3];

	 //------- increase Filter gain to let ADC read value significant ---------//
	UINT8   LPF_Count = 0;
	UINT8   ADC_Read_Value = 0;

	 for(LPF_Count=6; LPF_Count < 16; LPF_Count=LPF_Count+3)  //start from 6
	 {
		R855_I2C.RegAddr = 25;
		R855_Array[25] = (R855_Array[25] & 0x0F) | (LPF_Count<<4);
		R855_I2C.Data = R855_Array[25];  
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_Delay_MS(R855_FILTER_GAIN_DELAY * 1000); 	// by ITE
		
		if(R855_Muti_Read(&ADC_Read_Value) != R855_Success)
			return R855_Fail;

		if(ADC_Read_Value > 40*R855_ADC_READ_COUNT)
			break;
	 }
/*
	//Filter Gain 15
	R855_I2C.RegAddr = 41;
	R855_Array[41] = (R855_Array[41] & 0x0F) | (15<<4);
	R855_I2C.Data = R855_Array[41];  
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
*/
	//Try X-1 column and save min result to Compare_Bet[0]
	//if((IQ_Pont->Gain_X & 0x0F) == 0x00)
	if((IQ_Pont->Gain_X & 0x07) == 0x00)   //CD
	{
		//Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) & 0xEF) + 1;  //Q-path, Gain=1
		Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) & 0xF7) + 1;  //Q-path, Gain=1    CD
	}
	else
	{
		Compare_IQ[0].Gain_X  = IQ_Pont->Gain_X - 1;  //left point
	}
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	if(R855_IQ_Tree_G7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_G7, &Compare_IQ[0]) != R855_Success)  // y-direction
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[0].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[0].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[0].Value = Compare_IQ[0].Value;

	//Try X column and save min result to Compare_Bet[1]
	Compare_IQ[0].Gain_X = IQ_Pont->Gain_X;
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	if(R855_IQ_Tree_G7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_G7, &Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[1].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[1].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[1].Value = Compare_IQ[0].Value;

	//Try X+1 column and save min result to Compare_Bet[2]
	//if((IQ_Pont->Gain_X & 0x0F) == 0x00)	
	if((IQ_Pont->Gain_X & 0x07) == 0x00)	//CD
		//Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) | 0x10) + 1;  //I-path, Gain=1
		Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) | 0x08) + 1;  //I-path, Gain=1   CD
	else
	    Compare_IQ[0].Gain_X = IQ_Pont->Gain_X + 1;
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	if(R855_IQ_Tree_G7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_G7, &Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[2].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[2].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[2].Value = Compare_IQ[0].Value;

	if(R855_CompreCor(&Compare_Bet[0]) != R855_Success)
		return R855_Fail;

	//clear IQ_Cap = 0
	//Compare_Bet[0].Iqcap = R855_Array[3] & 0xFC;
	Compare_Bet[0].Iqcap = 0;

	if(R855_IMR_Iqcap(&Compare_Bet[0]) != R855_Success)
		return R855_Fail;

	*IQ_Pont = Compare_Bet[0];
	
	return R855_Success;
}


R855_ErrCode R855_F_IMR_L7_MIXAMP(R855_Sect_Type* IQ_Pont)  //CD OK
{
	R855_Sect_Type Compare_IQ[3];
	R855_Sect_Type Compare_Bet[3];

	 //------- increase Filter gain to let ADC read value significant ---------//
	UINT8   LPF_Count = 0;
	UINT8   ADC_Read_Value = 0;

	 for(LPF_Count=6; LPF_Count < 16; LPF_Count=LPF_Count+3)  //start from 6
	 {
		R855_I2C.RegAddr = 25;
		R855_Array[25] = (R855_Array[25] & 0x0F) | (LPF_Count<<4);
		R855_I2C.Data = R855_Array[25];  
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_Delay_MS(R855_FILTER_GAIN_DELAY * 1000); 	// by ITE
		
		if(R855_Muti_Read(&ADC_Read_Value) != R855_Success)
			return R855_Fail;

		if(ADC_Read_Value > 40*R855_ADC_READ_COUNT)
			break;
	 }
/*
	//Filter Gain 15
	R855_I2C.RegAddr = 41;
	R855_Array[41] = (R855_Array[41] & 0x0F) | (15<<4);
	R855_I2C.Data = R855_Array[41];  
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
*/
	//Try X-1 column and save min result to Compare_Bet[0]
	//if((IQ_Pont->Gain_X & 0x0F) == 0x00)
	if((IQ_Pont->Gain_X & 0x07) == 0x00)   //CD
	{
		//Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) & 0xEF) + 1;  //Q-path, Gain=1
		Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) & 0xF7) + 1;  //Q-path, Gain=1    CD
	}
	else
	{
		Compare_IQ[0].Gain_X  = IQ_Pont->Gain_X - 1;  //left point
	}
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	//if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_L7, &Compare_IQ[0]) != R855_Success)  // y-direction
	if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, 1, &Compare_IQ[0]) != R855_Success)  // y-direction
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[0].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[0].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[0].Value = Compare_IQ[0].Value;

	//Try X column and save min result to Compare_Bet[1]
	Compare_IQ[0].Gain_X = IQ_Pont->Gain_X;
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	//if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_L7, &Compare_IQ[0]) != R855_Success)
	if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, 1, &Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[1].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[1].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[1].Value = Compare_IQ[0].Value;

	//Try X+1 column and save min result to Compare_Bet[2]
	//if((IQ_Pont->Gain_X & 0x0F) == 0x00)	
	if((IQ_Pont->Gain_X & 0x07) == 0x00)	//CD
		//Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) | 0x10) + 1;  //I-path, Gain=1
		Compare_IQ[0].Gain_X = ((IQ_Pont->Gain_X) | 0x08) + 1;  //I-path, Gain=1   CD
	else
	    Compare_IQ[0].Gain_X = IQ_Pont->Gain_X + 1;
	Compare_IQ[0].Phase_Y = IQ_Pont->Phase_Y;

	//if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, R855_IMR_GAIN_REG_L7, &Compare_IQ[0]) != R855_Success)
	if(R855_IQ_Tree_L7_MIXAMP(Compare_IQ[0].Gain_X, Compare_IQ[0].Phase_Y, 1, &Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	if(R855_CompreCor(&Compare_IQ[0]) != R855_Success)
		return R855_Fail;

	Compare_Bet[2].Gain_X = Compare_IQ[0].Gain_X;
	Compare_Bet[2].Phase_Y = Compare_IQ[0].Phase_Y;
	Compare_Bet[2].Value = Compare_IQ[0].Value;

	if(R855_CompreCor(&Compare_Bet[0]) != R855_Success)
		return R855_Fail;

	//clear IQ_Cap = 0
	//Compare_Bet[0].Iqcap = R855_Array[3] & 0xFC;
	Compare_Bet[0].Iqcap = 0;

	if(R855_IMR_Iqcap(&Compare_Bet[0]) != R855_Success)
		return R855_Fail;

	*IQ_Pont = Compare_Bet[0];
	
	return R855_Success;
}


R855_ErrCode R855_IMR_Iqcap(R855_Sect_Type* IQ_Point)       //CO OK
{
    R855_Sect_Type Compare_Temp;
	int i = 0;

	//Set Gain/Phase to right setting
	R855_I2C.RegAddr = R855_IMR_GAIN_REG_G7;
	R855_I2C.Data = IQ_Point->Gain_X; 
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_I2C.RegAddr = R855_IMR_PHASE_REG_G7;
	R855_I2C.Data = IQ_Point->Phase_Y;
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//try iqcap
	for(i=0; i<3; i++)
	{
		Compare_Temp.Iqcap = (UINT8) i;  
		R855_I2C.RegAddr = R855_IMR_IQCAP_REG_G7;
		R855_Array[R855_IMR_IQCAP_REG_G7] = (R855_Array[R855_IMR_IQCAP_REG_G7] & 0x9F) | (Compare_Temp.Iqcap<<5);  
		R855_I2C.Data = R855_Array[R855_IMR_IQCAP_REG_G7];  
		if(I2C_Write(&R855_I2C) != R855_Success)
			   return R855_Fail;

		if(R855_Muti_Read(&(Compare_Temp.Value)) != R855_Success)
			   return R855_Fail;

		if(Compare_Temp.Value < IQ_Point->Value)
		{
			IQ_Point->Value = Compare_Temp.Value; 
			IQ_Point->Iqcap = Compare_Temp.Iqcap;  //0, 1, 2
		}
	}

    return R855_Success;
}




R855_ErrCode R855_SetStandard(R855_Standard_Type RT_Standard)  //CD OK
{	 
	UINT8 u1FilCalGap = R855_Fil_Cal_Gap;

#if(R855_DEBUG==1)
	R855_PRINT("Start R855_SetStandard process...\n");
#endif

    R855_Sys_Info1 = R855_Sys_Sel(RT_Standard);

	// Filter Calibration	
    if(R855_Fil_Cal_flag[RT_Standard] == FALSE)
	{
		R855_Fil_Cal_code[RT_Standard] = R855_Filt_Cal_ADC(R855_Sys_Info1.FILT_CAL_IF, R855_Sys_Info1.BW, u1FilCalGap);
		R855_Fil_Cal_BW[RT_Standard] = R855_Bandwidth;
        R855_Fil_Cal_flag[RT_Standard] = TRUE;
        R855_Fil_Cal_LpfLsb[RT_Standard] = R855_Lpf_Lsb;  

		//protection
		if(R855_IMR_Cal_Result==1) //fail
		{
			if((R855_Fil_Cal_BW[RT_Standard]==7) && (R855_Fil_Cal_code[RT_Standard]==15))  //6M/15
			{
				R855_Fil_Cal_BW[RT_Standard] = R855_Fil_Cal_BW_def[RT_Standard];
				R855_Fil_Cal_code[RT_Standard] = (R855_Fil_Cal_code_def[RT_Standard]>>1);
				R855_Fil_Cal_LpfLsb[RT_Standard] = (R855_Fil_Cal_code_def[RT_Standard] & 0x01);
			}
		}

	    //Reset register and Array 
	    if(R855_InitReg() != R855_Success)        
		   return R855_Fail;
	}

	//Re-set Registers when change standard
	if(RT_Standard != R855_pre_standard)
	{
		 if(R855_InitReg() != R855_Success)      
		     return R855_Fail;

		//Filter_Ext_Ena R17[7]  {0:man, 1:auto}
        R855_I2C.RegAddr = 17;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr ] & 0x7F) | (R855_Sys_Info1.FILT_EXT_ENA_R17_7_1BT<<7) ;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr ];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

        //FILT_EXT_OPT_R43_1_3BT R43[3:1]
        R855_I2C.RegAddr = 43;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr ] & 0xF1) | (R855_Sys_Info1.FILT_EXT_OPT_R43_1_3BT<<1) ;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr ];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		//FILT_EXT_CONDI_R34_4_1BT
		R855_I2C.RegAddr = 34;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr ] & 0xEF) | (R855_Sys_Info1.FILT_EXT_CONDI_R34_4_1BT<<4) ;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr ];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
        
        //FILT5_MAN_COMP_R20_4_1BT R20[4]
        R855_I2C.RegAddr = 20;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr ] & 0xEF) | (R855_Sys_Info1.FILT5_MAN_COMP_R20_4_1BT<<4) ;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr ];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		// Set HPF notch 
		R855_I2C.RegAddr = 24;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | (R855_Sys_Info1.HPF_NOTCH_R24_7_1BT<<7);  
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		// Set LPF coarse BW(R22[4] R19[7:6])
        switch(R855_Fil_Cal_BW[RT_Standard])
        {
        case 0: //9M  000
			R855_Array[22] = (R855_Array[22] & 0xEF) | 0x00;
		    R855_Array[19] = (R855_Array[19] & 0x3F) | 0x00;
			break;
		case 1: //8M  001
		    R855_Array[22] = (R855_Array[22] & 0xEF) | 0x00;
		    R855_Array[19] = (R855_Array[19] & 0x3F) | 0x40;
            break;
		case 3: //7M  011
		    R855_Array[22] = (R855_Array[22] & 0xEF) | 0x00;
		    R855_Array[19] = (R855_Array[19] & 0x3F) | 0xC0;
            break;
		case 7: //6M  111
		    R855_Array[22] = (R855_Array[22] & 0xEF) | 0x10;
		    R855_Array[19] = (R855_Array[19] & 0x3F) | 0xC0;
            break;
		}
        R855_I2C.RegAddr = 19;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
        R855_I2C.RegAddr = 22;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

        // Set LPF coarse BW(R19[5:1])
		R855_I2C.RegAddr = 19;
		R855_Array[R855_I2C.RegAddr ] = (R855_Array[R855_I2C.RegAddr ] & 0xC1) | (R855_Fil_Cal_code[RT_Standard]<<2) | (R855_Fil_Cal_LpfLsb[RT_Standard]<<1);
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr ];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
        
		// Set HPF corner R22[3:0]
		R855_I2C.RegAddr = 22;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF0) | R855_Sys_Info1.HPF_COR;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		// Set NA det power  R9[7]
		R855_I2C.RegAddr = 9;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | (R855_Sys_Info1.NA_PWR_DET_R9_7_1BT<<7); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		// Polyphase current(R12[3]);
		R855_I2C.RegAddr = 12;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF7) | (R855_Sys_Info1.POLY_CUR_R12_3_1BT<<3);
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;	

             
		// 5th Filter Auto Comp  R36[6:5]
		R855_I2C.RegAddr = 36;
		R855_Array[R855_I2C.RegAddr ] = (R855_Array[R855_I2C.RegAddr ] & 0x9F) | (R855_Sys_Info1.FILT5_AUTO_COMP_R36_5_2BT<<5); 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr ];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

        // 5th Filter Force Q(bit7); R43[0]
        R855_I2C.RegAddr = 43;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE) | (R855_Sys_Info1.FILT5_FORCEQ_R43_0_1BT) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

        // 3th Filter Comp(bit4:3)
		R855_I2C.RegAddr = 36;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE7) | (R855_Sys_Info1.FILT3_COMP_R36_3_2BT<<3) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		//AGC/RF
		R855_I2C.RegAddr = 36;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFD) | (R855_Sys_Info1.RF_CLASSB_CHARGE_R36_1_1BT<<1) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = 24;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x9F) | (R855_Sys_Info1.RF_RES_CAP_R24_5_2BT<<5) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		//AGC_BB
		R855_I2C.RegAddr = 17;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFD) | (R855_Sys_Info1.BB_CLASSB_CHARGE_R17_1_1BT<<1) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
	
		R855_I2C.RegAddr = 23;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xBF) | (R855_Sys_Info1.BB_RES_CAP_R23_6_1BT<<6) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		//Discharge
		R855_I2C.RegAddr = 46;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xDF) | (R855_Sys_Info1.LNA_PEAK_AVG_R46_5_1BT<<5) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = 39;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | (R855_Sys_Info1.RF_PEAK_AVG_R39_7_1BT<<7) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = 46;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFB) | (R855_Sys_Info1.BB_PEAK_AVG_R46_2_1BT<<2) ; 
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
		

	}
    R855_pre_standard = RT_Standard;

	return R855_Success;
}

UINT8  R855_Filt_Cal_ADC(UINT32 IF_Freq, UINT8 R855_BW, UINT8 FilCal_Gap)  //cd ok
{
	UINT8     u1FilterCodeResult = 0;
	UINT8     u1FilterCode = 0;
	UINT8     u1FilterCalValue = 0;
	UINT8     u1FilterCalValuePre = 0;
	UINT8     initial_cnt = 0;
	UINT8     i = 0;
	UINT32    RingFreq = 72000;
	UINT8     ADC_Read_Value = 0;
	UINT8     LPF_Count = 0;
	


	 if(R855_Cal_Prepare(R855_LPF_CAL) != R855_Success)      
	      return R855_Fail;
/*
//test ADC
		R855_I2C.RegAddr = 25;
		R855_Array[25] = (R855_Array[25] & 0xFE) | 1; //1: IF RSSI; 0:image
		R855_I2C.Data = R855_Array[25];  
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_I2C.RegAddr = 35;
		R855_Array[35] = (R855_Array[35] & 0xFB) | (1<<2);  //1:2.2V(IF_RSSI)  0:2.0V(image)
		R855_I2C.Data = R855_Array[35];  
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
*/
	 //Set PLL (normal)
	 if(R855_PLL((RingFreq + IF_Freq), (UINT16)IF_Freq, R855_STD_SIZE) != R855_Success)   //FilCal PLL
	       return R855_Fail;

	 for(LPF_Count=5; LPF_Count < 16; LPF_Count ++)  //start from 5
	 {
		R855_I2C.RegAddr = 25;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x0F) | (LPF_Count<<4);
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];  
		if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		R855_Delay_MS(R855_FILTER_GAIN_DELAY * 1000); 	// by ITE
		
		if(R855_Muti_Read(&ADC_Read_Value) != R855_Success)
			return R855_Fail;

		if(ADC_Read_Value > 40*R855_ADC_READ_COUNT)
			break;
	 }

	 //------- Try suitable BW --------//
	 if(R855_BW==2) //6M
         initial_cnt = 2;  //try 7M first
	 else
		 initial_cnt = 0;  //try 9M first

	 for(i=initial_cnt; i<4; i++)
	 {
         if(i==0)
		 {
			R855_Bandwidth = 0; //9M
		 }
		 else if(i==1)
		 {
             //R855_Bandwidth = 0; //8M
			 R855_Bandwidth = 1; //8M    R22[4] R19[7:6]
		 }
		 else if(i==2)
		 {
			// R855_Bandwidth = 2; //7M
			 R855_Bandwidth = 3; //7M   R22[4] R19[7:6]
		 }
		 else
		 {
			 //R855_Bandwidth = 3; //6M, (not 2!!!!!)
			 R855_Bandwidth = 7; //6M	R22[4] R19[7:6]
		 }
/*
		 //Set BW
		 R855_I2C.RegAddr = 22;	
		 R855_Array[22] = (R855_Array[22] & 0x9F) | (R855_Bandwidth<<5);   
		 R855_I2C.Data = R855_Array[22];		
		 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
*/

		 //Set BW  R22[4] R19[7:6]
		 R855_I2C.RegAddr = 22;	
		 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xEF) | ((R855_Bandwidth & 0x04)<<2);   
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];		
		 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		 R855_I2C.RegAddr = 19;	
		 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x3F) | ((R855_Bandwidth & 0x03)<<6);   
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];		
		 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

		 // read code 0
		 R855_I2C.RegAddr = 19;
		 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xC3);  //code 0
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		 if(I2C_Write(&R855_I2C) != R855_Success)
			  return R855_Fail;

		 R855_Delay_MS(R855_FILTER_CODE_DELAY * 1000); //delay ms	// by ITE
	     
		 if(R855_Muti_Read(&u1FilterCalValuePre) != R855_Success)
			  return R855_Fail;

#if(R855_DEBUG==1)
	if(i==initial_cnt)
	{
		R855_PRINT("=============Filter Calibration Try suitable BW...===============\n");
	}
	R855_PRINT("R855_Bandwidth[0:9M, 1:8M, 3:7M, 7:6M]=%d, filter code=0\n", R855_Bandwidth);
	R855_PRINT("u1FilterCalValuePre=%d\n", u1FilterCalValuePre);
#endif


		 //read code 13
		 R855_I2C.RegAddr = 19;
		 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xC3)| (13<<2);  //code 13
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		 if(I2C_Write(&R855_I2C) != R855_Success)
			  return R855_Fail;


		 R855_Delay_MS(R855_FILTER_CODE_DELAY * 1000); //delay ms	// by ITE
	     
		 if(R855_Muti_Read(&u1FilterCalValue) != R855_Success)
			  return R855_Fail;

#if(R855_DEBUG==1)
	R855_PRINT("R855_Bandwidth[0:9M, 1:8M, 3:7M, 7:6M]=%d, filter code=13\n", R855_Bandwidth);
	R855_PRINT("u1FilterCalValue=%d\n", u1FilterCalValue);
#endif
		 if(u1FilterCalValuePre > (u1FilterCalValue+8))  //suitable BW found
			 break;
	 }
#if(R855_DEBUG==1)
	 R855_PRINT("R855_Bandwidth[0:9M, 1:8M, 3:7M, 7:6M] result=%d \n", R855_Bandwidth);
	R855_PRINT("=============Filter Calibration Try LPF code...===============\n");
	R855_PRINT("FilCal_Gap*R855_ADC_READ_COUNT = %d (condition)\n", FilCal_Gap*R855_ADC_READ_COUNT);
#endif
     //-------- Try LPF filter code ---------//
	 u1FilterCalValuePre = 0;
	 for(u1FilterCode=0; u1FilterCode<16; u1FilterCode++)
	 {
		 /*
         R855_I2C.RegAddr = 22;
         R855_Array[22] = (R855_Array[22] & 0xE1) | (u1FilterCode<<1);
         R855_I2C.Data = R855_Array[22];
         if(I2C_Write(&R855_I2C) != R855_Success)
              return R855_Fail;
		 */
		 if(u1FilterCode==0)  //R855S
		 {
			R855_Delay_MS(R855_FILTER_CODE_DELAY * 1000); //delay ms	// by ITE
		 }

		 R855_I2C.RegAddr = 19;
		 R855_Array[19] = (R855_Array[19] & 0xC3)| (u1FilterCode<<2); 
		 R855_I2C.Data = R855_Array[19];
		 if(I2C_Write(&R855_I2C) != R855_Success)
			  return R855_Fail;

		 R855_Delay_MS(R855_FILTER_CODE_DELAY * 1000); //delay ms	// by ITE

		 //First read
		 if(R855_Muti_Read(&u1FilterCalValue) != R855_Success)
		      return R855_Fail;

		 //second read 
		 if(R855_Muti_Read(&u1FilterCalValue) != R855_Success)
		      return R855_Fail;
#if(R855_DEBUG==1)
		 R855_PRINT("R855_Bandwidth[0:9M, 1:8M, 3:7M, 7:6M]=%d, filter code=%d, u1FilterCalValue=%d\n", R855_Bandwidth, (u1FilterCode*2), u1FilterCalValue);
#endif
		 if(u1FilterCode==0)
             u1FilterCalValuePre = u1FilterCalValue;


		 if((u1FilterCalValue+FilCal_Gap*R855_ADC_READ_COUNT) < u1FilterCalValuePre)
		 {
			 u1FilterCodeResult = u1FilterCode;
			  break;
		 }
	 }

     //Try LSB bit
	 if(u1FilterCodeResult>0)   //try code-1 & lsb=1
	 {		 
		 R855_I2C.RegAddr = 19;
		 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xC1) | ((u1FilterCodeResult-1)<<2) | 0x02; 
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		 if(I2C_Write(&R855_I2C) != R855_Success)
			  return R855_Fail;

		 R855_Delay_MS(R855_FILTER_CODE_DELAY * 1000); //delay ms	// by ITE

		 //First read
		 if(R855_Muti_Read(&u1FilterCalValue) != R855_Success)
			  return R855_Fail;
		//second read 
		 if(R855_Muti_Read(&u1FilterCalValue) != R855_Success)
			  return R855_Fail;
#if(R855_DEBUG==1)
		 R855_PRINT("R855_Bandwidth[0:9M, 1:8M, 3:7M, 7:6M]=%d, filter code=%d, u1FilterCalValue=%d\n", R855_Bandwidth, ((u1FilterCodeResult*2)-1), u1FilterCalValue);
#endif
		 if((u1FilterCalValue+FilCal_Gap*R855_ADC_READ_COUNT) < u1FilterCalValuePre)
		 {
			 u1FilterCodeResult = u1FilterCodeResult - 1;
			 R855_Lpf_Lsb = 1;
		 }
		 else
		 {
		 	 //u1FilterCodeResult = u1FilterCodeResult;
			 R855_Lpf_Lsb = 0;
		 }
	 }
     
	 if(u1FilterCode==16)
	 {
          u1FilterCodeResult = 15;
		  R855_Lpf_Lsb = 1;
	 }

#if(R855_DEBUG==1)
	 R855_PRINT("Filter Result R855_Bandwidth[0:9M, 1:8M, 3:7M, 7:6M]=%d, filter code=%d\n", R855_Bandwidth, (u1FilterCodeResult*2)+R855_Lpf_Lsb);
	R855_PRINT("==============================================================\n");
#endif
	  return u1FilterCodeResult;

}

R855_ErrCode R855_SetFrequency(R855_Set_Info R855_INFO) //CD OK
{
	 UINT32	  LO_KHz;
	 UINT8    Img_R;
	 UINT8    i = 0;
	 UINT8    vco_bank_read = 0;
	 UINT8    Reg_Num_Index;

#if(R855_DEBUG==1)
	R855_PRINT("Start R855_SetFrequency process...\n");
#endif
	 //Get Sys-Freq parameter
     R855_SysFreq_Info1 = R855_SysFreq_Sel(R855_INFO.R855_Standard, R855_INFO.RF_KHz);

	 R855_IMR_point_num = R855_Freq_Info1.IMR_MEM_NOR;


	 //AGC Ctrl (clk,R855_DTV_AGC_SLOW=FALSE; ctrl_by_slow)
	 //AGC Ctrl reset   0->1->0
	 R855_I2C.RegAddr = 47;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] | 0x80);
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
		 return R855_Fail;

     // Check Input Frequency Range
	 if((R855_INFO.RF_KHz<40000) || (R855_INFO.RF_KHz>1002000))
	 {
		 return R855_Fail;
	 }

	 if(R855_Mixer_Mode==R855_IMR_REV)
	 {
		 LO_KHz = R855_INFO.RF_KHz - R855_Sys_Info1.IF_KHz;
		 Img_R = 1;
	 }
	 else
	 {
		 LO_KHz = R855_INFO.RF_KHz + R855_Sys_Info1.IF_KHz;
		 Img_R = 0;
	 }
	  R855_I2C.RegAddr = 20;
	  R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | (Img_R<<7);  //R20[7]
	  R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	  if(I2C_Write(&R855_I2C) != R855_Success)
		 return R855_Fail;

	 //Set MUX dependent var. Must do before PLL( ) 
     if(R855_MUX(LO_KHz, R855_INFO.RF_KHz, R855_INFO.R855_Standard) != R855_Success)   //normal MUX
        return R855_Fail;

     //Set PLL
     if(R855_PLL(LO_KHz, R855_Sys_Info1.IF_KHz, R855_INFO.R855_Standard) != R855_Success) //noraml PLL
        return R855_Fail;

	 //Set TF
	 if(R855_SetTF(R855_INFO.RF_KHz, R855_SetTfType_UL_MID) != R855_Success)
		return R855_Fail;


	 //R855_SysFreq_Info.TF_MODE_R13_7_1BT=1;
	 Reg_Num_Index = 13;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x7F) | (R855_SysFreq_Info1.TF_MODE_R13_7_1BT<<7);  //R13[7]

	 // LNA TF Q-Ctrl (depend on RF_KHz)
	 Reg_Num_Index = 14;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x7F) | (R855_SysFreq_Info1.Q_CTRL_R14_7_1BT<<7);  //R14[7]

	 //LNA Max Gain
	 Reg_Num_Index = 17;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFB) | (R855_SysFreq_Info1.LNA_MAX_GAIN_R17_2_1BT<<2);
	 //Set LNA enable atten.
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFE) | (R855_SysFreq_Info1.ENB_ATT_R17_0_1BT);

     // LNA_TOP_R37_0_4BT
	 Reg_Num_Index = 37;
     R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF0) | (15 - R855_SysFreq_Info1.LNA_TOP_R37_0_4BT);
	 //LNA VTH/VTL
	 Reg_Num_Index = 38;
     R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF0) | R855_SysFreq_Info1.LNA_VTH_R38_0_4BT;  //R38[3:0]
	 Reg_Num_Index = 13;
     R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xE1) | (R855_SysFreq_Info1.LNA_VTL_R13_1_4BT<<1); // R13[4:1]

	 // RF_TOP_R37_4_3BT
	 Reg_Num_Index = 37;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | ((7 - R855_SysFreq_Info1.RF_TOP_R37_4_3BT)<<4);
	 // RF VTL/H
	 Reg_Num_Index = 38;
     //R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x0F) | (R855_SysFreq_Info1.RF_VTH_R38_4_4BT<<4); //R38[7:4]
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x0F) | (R855_SysFreq_Info1.RF_VTH_R38_4_4BT<<5); //R38[7:5], R38[4] don't care this bit and fixed this bit=0
	 Reg_Num_Index = 25;
     R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF0) | R855_SysFreq_Info1.RF_VTL_R25_0_4BT; //R25[3:0]
    
	 // NRB TOP
	 Reg_Num_Index = 40;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x0F) | ((15 - R855_SysFreq_Info1.NRB_TOP_R40_4_4BT)<<4); //R40[7:4]

	 // NRB HPF & LPF BW
	 Reg_Num_Index = 34;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x9F) | (R855_SysFreq_Info1.NRB_BW_HPF_R34_5_2BT<<5);
	 Reg_Num_Index = 41;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF3) | (R855_SysFreq_Info1.NRB_BW_LPF_R41_2_2BT<<2);

     // MIXER TOP
	 Reg_Num_Index = 40;
     R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFC) | (3 - R855_SysFreq_Info1.MIXER_TOP_R40_0_2BT);   //R40[1:0]

     // MIXER VTH 
	 Reg_Num_Index = 18;
     R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xE1) | (R855_SysFreq_Info1.MIXER_VTH_R18_1_4BT<<1); //R18[4:1]

     // MIXER VTL 
	 Reg_Num_Index = 39;
     R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF8) | R855_SysFreq_Info1.MIXER_VTL_R39_0_3BT;  //R39[2:0]
     
     // Filter VTH
	 Reg_Num_Index = 25;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x0F) | (R855_SysFreq_Info1.FILTER_VTH_R25_4_4BT<<4); //R25[7:4]

     // Filter VTL
	 Reg_Num_Index = 39;
     R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | (R855_SysFreq_Info1.FILTER_VTL_R39_4_3BT<<4); //R39[6:4]

	 // Filter TOP
	 Reg_Num_Index = 40;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF3) | ((3-R855_SysFreq_Info1.FILTER_TOP_R40_2_2BT)<<2); //R40[3:2]

	 // Mixer Amp LPF R35[4:2]
	 Reg_Num_Index = 35;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xE3) | (R855_SysFreq_Info1.MIXER_AMP_LPF_R35_2_3BT<<2); 

	 //LPF_ACI_ENB_R34_7_1BT R34[7]
	 Reg_Num_Index = 34;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x7F) | (R855_SysFreq_Info1.LPF_ACI_ENB_R34_7_1BT<<7); 

	 //RBG_MIN_RFBUF_R13_6_1BT
	 Reg_Num_Index = 13;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xBF) | (R855_SysFreq_Info1.RBG_MIN_RFBUF_R13_6_1BT<<6);

	 // Mixer Gain Limt
	 Reg_Num_Index = 20;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x9F) | (R855_SysFreq_Info1.MIXER_GAIN_LIMIT_R20_5_2BT<<5);   //R20[6:5]

	 //POLY_GAIN_R21_7_1BT		
	 Reg_Num_Index = 21;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x7F) | (R855_SysFreq_Info1.POLY_GAIN_R21_7_1BT<<7); 

	 //LNA_RF Discharge Mode
	 Reg_Num_Index = 44;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | (R855_SysFreq_Info1.LNA_RF_DIS_MODE_R44_4_3BT<<4); //R44[6:4]  
/*	 
	 switch(R855_SysFreq_Info1.LNA_RF_DIS_MODE_R44_4_3BT)
	 {
	 case 0:  //auto R44[6:4] = 011
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | 0x30;  
		 break;
	 case 1: //both(fast+slow)  R44[6:4] = 111
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | 0x70;  
		 break;
	 case 2: //both(slow)  R44[6:4] = 100
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | 0x40;  
		 break;
	 case 3: //LNA(slow)  R44[6:4] = 110
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | 0x60;  
		 break;
	 case 4: //RF(slow)  R44[6:4] = 101
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | 0x50;  
		 break;
	 default:
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x8F) | 0x30; 
		 break;
	 }
*/


	 //LNA_dis current   R43[4]
	 Reg_Num_Index = 43;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xEF) | (R855_SysFreq_Info1.LNA_DIS_CURR_R43_4_1BT<<4); 
     //RF_dis current   R43[5]
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (R855_SysFreq_Info1.RF_DIS_CURR_R43_5_1BT<<5); 

	 //LNA and RF slow disch(1:0) 
	 Reg_Num_Index = 44;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFC) | R855_SysFreq_Info1.LNA_RF_DIS_SLOW_R44_0_2BT;  
	 //LNA and RF fast disch(3:2)
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF3) | (R855_SysFreq_Info1.LNA_RF_DIS_FAST_R44_2_2BT<<2);  

	 //SLOW_DIS_ENABLE_R41_0_1BT
	 Reg_Num_Index = 41;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFE) | (R855_SysFreq_Info1.SLOW_DIS_ENABLE_R41_0_1BT);  

	 //BB disch current R18
	 Reg_Num_Index = 18;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x7F) | (R855_SysFreq_Info1.BB_DIS_CURR_R18_7_1BT<<7);  

	 //Mixer/Filter disch R43[7:6]
	 Reg_Num_Index = 43;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x3F) | (R855_SysFreq_Info1.MIXER_FILTER_DIS_R43_6_2BT<<6);  


	//FAGC_2_SAGC_R47_7_1BT;
	// Reg_Num_Index = 47;
	// R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x7F) | (R855_SysFreq_Info1.FAGC_2_SAGC_R47_7_1BT<<7);  
	//CLK_FAST_R47_2_2BT;
	 Reg_Num_Index = 47;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF3) | (R855_SysFreq_Info1.CLK_FAST_R47_2_2BT<<2);  
	//CLK_SLOW_R24_1_R13_5_2BT;
	 switch(R855_SysFreq_Info1.CLK_SLOW_R24_1_R13_5_2BT)
	 {
	 case 0: //00
		 Reg_Num_Index = 24;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFD) | (0<<1);  
		 Reg_Num_Index = 13;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (0<<5);  
		 break;
	 case 1://01
		 Reg_Num_Index = 24;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFD) | (0<<1);  
		 Reg_Num_Index = 13;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (1<<5);  
		 break;
	 case 2://10
		 Reg_Num_Index = 24;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFD) | (1<<1);  
		 Reg_Num_Index = 13;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (0<<5);  
		 break;
	 case 3://11
		 Reg_Num_Index = 24;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFD) | (1<<1);  
		 Reg_Num_Index = 13;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (1<<5);  
		 break;
	 default://00
		 Reg_Num_Index = 24;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFD) | (0<<1);  
		 Reg_Num_Index = 13;
		 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (0<<5);  
		 break;
	 }
	//LEVEL_SW_R45_4_1BT;
	 Reg_Num_Index = 45;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xEF) | (R855_SysFreq_Info1.LEVEL_SW_R45_4_1BT<<4);  
	//MODE_SEL_R45_5_1BT;
	 Reg_Num_Index = 45;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (R855_SysFreq_Info1.MODE_SEL_R45_5_1BT<<5);  
	//LEVEL_SW_VTHH_R37_7_1BT;
	 Reg_Num_Index = 37;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x7F) | (R855_SysFreq_Info1.LEVEL_SW_VTHH_R37_7_1BT<<7);  
	//LEVEL_SW_VTLL_R46_1_1BT;
	 Reg_Num_Index = 46;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFD) | (R855_SysFreq_Info1.LEVEL_SW_VTLL_R46_1_1BT<<1);  

	 //IMG_ACI_R9_4_1BT R9[4]
	 Reg_Num_Index = 9;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xEF) | (R855_SysFreq_Info1.IMG_ACI_R9_4_1BT<<4);  
     
	 //FILTER_G_CONTROL_R21_4_1BT		//[0:ctrl by filtg (>9=0, <6=1), 1:diable] ***
	 Reg_Num_Index = 21;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xEF) | (R855_SysFreq_Info1.FILTER_G_CONTROL_R21_4_1BT<<4);  

	 //WIDEN_VTH_VTL_R45_0_2BT		//[0:debug mode(no change), 1:small, 2:mId, 3:biggest] ***
	 Reg_Num_Index = 45;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFC) | (R855_SysFreq_Info1.WIDEN_VTH_VTL_R45_0_2BT);  
	 //NBR Image TOP adder R45[3:2]  
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF3) | (R855_SysFreq_Info1.IMG_NRB_ADDER_R45_2_2BT<<2);  

	 // 3th LPF gain (man)  R24[2]
	 Reg_Num_Index = 24;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFB) | (R855_SysFreq_Info1.FILT_3TH_GAIN_MAN_R24_2_1BT<<2); 

	 // VGA HPF Comp R35[7:6]
	 Reg_Num_Index = 35;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0x3F) | (R855_SysFreq_Info1.HPF_COMP_R35_6_2BT<<6); 
	 // VGA 1st FB res  R35[5]
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (R855_SysFreq_Info1.FB1_RES_R35_5_1BT<<5); 

	 //VGA Pin level; delta; R23[3]
	 Reg_Num_Index = 23;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xF7) | (R855_SysFreq_Info1.VGA_PIN_LVL_R23_3_1BT<<3) ;  

	 //VGA_ATV_MODEL_R23_1_1BT
	 Reg_Num_Index = 23;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFD) | (R855_SysFreq_Info1.VGA_ATV_MODEL_R23_1_1BT<<1) ;  

	 //VGA Out att. R36[2]
	 Reg_Num_Index = 36;
	 R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xFB) | (R855_SysFreq_Info1.VGA_OUT_ATT_R36_2_1BT<<2);  

	 //LNA_RF charge  R15[5]
	 //Reg_Num_Index = 15;
	 //R855_Array[Reg_Num_Index] = (R855_Array[Reg_Num_Index] & 0xDF) | (R855_SysFreq_Info1.LNA_RF_CHARGE_R15_5_1BT<<5);  


	 //IF AGC 1/2
	 Reg_Num_Index = 36;
	 if(R855_INFO.R855_IfAgc_Select==R855_IF_AGC2)
         R855_Array[Reg_Num_Index] = R855_Array[Reg_Num_Index] | 0x01;
     else
         R855_Array[Reg_Num_Index] = R855_Array[Reg_Num_Index] & 0xFE;



#if(R855_SPECTIAL_SETTING == TRUE)

	//LNAstep_autoR41[1][0:manual, 1:ctrl by flag], LNAstep_manualR37[0:2][0~7]
	//pulse flag force mode, "ctrl by flag" and  manial "7"
	R855_Array[41] = (R855_Array[41] & 0xFD) | (R855_LNASTEP_AUTO<<1);
	R855_Array[37] = (R855_Array[37] & 0xF8) | (R855_LNASTEP_MANUAL);


	//lpf_ACI_enb R34[7] [0:disable, 1:enable]
	R855_Array[34] = (R855_Array[34] & 0x7F) | (R855_LPF_ACI_ENB<<7);

#endif


	//Write I2C R8~R47
	R855_I2C_Len.RegAddr = 8;
	R855_I2C_Len.Len = R855_REG_NUM - 8;
	for(i=8; i<R855_REG_NUM; i++)
	{
		R855_I2C_Len.Data[i-8] = R855_Array[i];	
	}	
	if(I2C_Write_Len(&R855_I2C_Len) != R855_Success)
		return R855_Fail;

	R855_I2C_Len.RegAddr = 0x00;
	R855_I2C_Len.Len = 4;
	if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
		return R855_Fail;

	vco_bank_read = R855_I2C_Len.Data[3] & 0x3F;


	 //TF LPF_Q enhance
	 R855_I2C.RegAddr = 15;			
	 R855_Array[15] = (R855_Array[15] | 0x80);  //normal
	 R855_I2C.Data = R855_Array[15];
	 if(I2C_Write(&R855_I2C) != R855_Success)
		 return R855_Fail;	

	 //------- goodm for 4th DTMB s11 ----------//
	 if(R855_INFO.R855_Standard==R855_DTMB_8M_4570 || R855_INFO.R855_Standard==R855_DTMB_8M_IF_5M)
	 {
		if((R855_INFO.RF_KHz==482000) || (R855_INFO.RF_KHz==658000) || (R855_INFO.RF_KHz==786000))
			R855_Array[15] = (R855_Array[15] & 0xF7) | 0x00;				
		else
			R855_Array[15] = (R855_Array[15] & 0xF7) | 0x08;				
	 }
	 else
	 {
		 R855_Array[15] = (R855_Array[15] & 0xF7) | 0x08;
	 }
	 R855_I2C.RegAddr = 15;			
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr]  ;
	 if(I2C_Write(&R855_I2C) != R855_Success)
	 	return R855_Fail;


	 //FAGC_2_SAGC_R47_7_1BT //[0:slow, 1:fast]
	 //AGC Ctrl control by excel
	 R855_I2C.RegAddr = 47;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7F) | (R855_SysFreq_Info1.FAGC_2_SAGC_R47_7_1BT<<7); 
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
		 return R855_Fail;

	 return R855_Success;
}

R855_ErrCode R855_SetPllData(R855_Set_Info R855_INFO)  //CD OK
{	  

#if(R855_DEBUG==1)


	R855_PRINT("Start R855_SetPllData process...\n");
	R855_PRINT("R855_INFO.R855_IfAgc_Select=%d\n", R855_INFO.R855_IfAgc_Select);
	R855_PRINT("R855_INFO.R855_Standard=%d\n", R855_INFO.R855_Standard);
	R855_PRINT("R855_INFO.RF_KHz=%d\n", R855_INFO.RF_KHz);	
#endif
	  if(R855_Initial_done_flag==FALSE)
	  {
	      R855_Init(R855_INFO.R855_Standard);
	  }
	  R855_PRINT("R855_SetStandard\n");

      if(R855_SetStandard(R855_INFO.R855_Standard) != R855_Success)
		  return R855_Fail;
	  R855_PRINT("R855_SetFrequency\n");

      if(R855_SetFrequency(R855_INFO) != R855_Success)
          return R855_Fail;


#if(R855_DEBUG==1)
	R855_Dump_Data();
	INT32 RfGain=0;
	R855_GetRfRssi(R855_INFO.RF_KHz,R855_INFO.R855_Standard,&RfGain);
	printf("RfGain=%d \n",RfGain);

#endif
      return R855_Success;
}

R855_ErrCode R855_SetPllData_Mode(R855_Set_Info R855_INFO, R855_TuneMode_Type R855_TuningMode)   //CD OK
{
	UINT8 count = 0;
	UINT8 LNA_gain;

	LNA_gain = 31;

    if(R855_SetPllData(R855_INFO) != R855_Success)
        return R855_Fail;

	if(R855_TuningMode==R855_CHANNEL_CHANGE) //adjust to slow clk
	{	  

	  R855_I2C_Len.RegAddr = 0x00;
	  R855_I2C_Len.Len = 7;
	  if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
		  return R855_Fail;		

	  do
	  {
		  LNA_gain = (R855_I2C_Len.Data[6] & 0x1F);
		  R855_Delay_MS(10 * 1000);	// by ITE
		  if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
			  return R855_Fail;	
		  count++;
	  
	  }while(((R855_I2C_Len.Data[6] & 0x1F)!=LNA_gain) && (count<5));

	  R855_AGC_Slow();

	}

	return R855_Success;
}

R855_ErrCode R855_Standby(void)
{
	UINT8 i;

	if(R855_Initial_done_flag==FALSE)
	{
		for(i=0; i<R855_REG_NUM; i++)
		{		
			if(R855_Xtal==24000)
				R855_Array[i]=R855_iniArray[0][i];		//24M
			else if(R855_Xtal==16000)
				R855_Array[i]=R855_iniArray[1][i];		//16M
			else
				R855_Array[i]=R855_iniArray[2][i];		//27M

		}
	}
	
	for(i=0; i<R855_REG_NUM; i++)
	{		
		R855_SBY[i]=R855_Array[i];		
	}

	 //All[bit0], RF_Buf[bit1], LNA[bit2], MIXER[bit3], PLL_LDOA[bit5:4], ADC[bit6], Ring[bit7]
	 R855_I2C.RegAddr = 8;
	 R855_Array[R855_I2C.RegAddr] = 0xFF;  
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //LDO, TF_buf_cur=low, mpcr_cur=low
	 //dldo2[bit1:0], Xtal LDO(option)[bit2], AGC PW[bit3], LNA/RF det[bit5], HPF pw[bit6], NAT pw[bit7]
	 R855_I2C.RegAddr = 9;
	 if(R855_SHARE_XTAL==R855_NO_SHARE_XTAL)  //not share xtal
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x10) | 0x6F;  
	 else //share xtal case
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x10) | 0x6B;  
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //PFD_ldo[bit1:0], Hfdiv_ldo[bit3:2], Nrd det[bit4], IQGen[bit7]
	 R855_I2C.RegAddr = 10;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x60) | 0x9F;    
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //TF Buf PW[bit3]= low, Mixer cur[bit5] = low
	 R855_I2C.RegAddr = 11;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xD7) | 0x00;    
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //mpcr current[bit2]= low
	 R855_I2C.RegAddr = 12;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFF) | 0x04;    
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;
	
	 //LNA manual 31[bit5:1]; TF sharp[bit7]
	 R855_I2C.RegAddr = 13;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x40) | 0xBF; 
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //Q1.5 off[bit7]; TF code=0[bit6:0]
	 R855_I2C.RegAddr = 14;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x00); 
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //Byp-LPF = bypass[bit6]
	 R855_I2C.RegAddr = 15;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xBF) | 0x00; 
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //VGA code mode[bit0]
	 R855_I2C.RegAddr = 23;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE) | 0x01; 
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //Rf Buf manual[bit3]
	 R855_I2C.RegAddr = 17;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF7) | 0x08; 
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 //RFbuf gain[bit3:0]
	 R855_I2C.RegAddr = 25;
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF0) | 0x00; 
	 R855_I2C.Data = R855_Array[39];
	 if(I2C_Write(&R855_I2C) != R855_Success)
			return R855_Fail;

	 R855_Standby_flag = TRUE;

	return R855_Success;
}

//-----------------------------------------------------------------------//
//  This function is used to wake up tuner from Standby mode             //
//-----------------------------------------------------------------------//
R855_ErrCode R855_WakeUp(void)
{
	UINT8 i;
	UINT8 PFD_Dldo, Pll_Aldo, Pll_dldo2;

	Pll_Aldo = R855_SBY[8] & 0x30;
	Pll_dldo2 = R855_SBY[9] & 0x03;
	PFD_Dldo = R855_SBY[10] & 0x03;


	if(R855_Standby_flag == FALSE)
	{
		return R855_Success;
	}

	R855_SBY[8] = (R855_SBY[8] & 0xC7) | 0x38;   //Pll_Aldo off R8[5:4]=11, Mixer off R8[bit3]=1
	R855_SBY[9] = (R855_SBY[9] & 0xFC) | 0x03;  //Pll_dldo2 off R9[1:0]=11
	R855_SBY[10] = (R855_SBY[10] & 0x7C) | 0x83;   //PFD_Dldo off, R10[1:0]=11, IQ_Gen off R10[7]=1


	R855_I2C_Len.RegAddr = 0;
	R855_I2C_Len.Len = R855_REG_NUM;
	for(i = 0; i<R855_REG_NUM; i ++)
	{
		R855_I2C_Len.Data[i] = R855_SBY[i];
		R855_Array[i] = R855_SBY[i];
	}
	if(I2C_Write_Len(&R855_I2C_Len) != R855_Success)
		return R855_Fail;

	//Mixer ON[bit3], //DLDO1 ON [bit5:4]
	R855_I2C.RegAddr = 8;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xC7) | Pll_Aldo;  //R8[3]=0
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//Pll_dldo2 ON[bit1:0]
	R855_I2C.RegAddr = 9;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFC) | Pll_dldo2;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//IQ Gen ON[bit7], PFD_Dldo ON[bit1:0]
	R855_I2C.RegAddr = 10;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x7C) | PFD_Dldo;  //R10[7]=0
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;	

	R855_Standby_flag = FALSE;

	return R855_Success;
}

R855_ErrCode R855_GetRfGain(R855_RF_Gain_Info *pR855_rf_gain)  //CD OK
{
	R855_I2C_Len.RegAddr = 0x00;
	R855_I2C_Len.Len = 7;
	if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
	{
		I2C_Read_Len(&R855_I2C_Len);
	}
	pR855_rf_gain->RF_gain1 = (R855_I2C_Len.Data[6] & 0x1F);          //lna
	pR855_rf_gain->RF_gain2 = (R855_I2C_Len.Data[4] & 0xF0)>>4;          //rf
	pR855_rf_gain->RF_gain3 = (R855_I2C_Len.Data[5] & 0xF0)>>4;       //mixer
	pR855_rf_gain->RF_gain4 = (R855_I2C_Len.Data[5] & 0x0F);          //filter

	pR855_rf_gain->RF_gain_comb = pR855_rf_gain->RF_gain1*15
		                                              + pR855_rf_gain->RF_gain2*7 
		                                              + pR855_rf_gain->RF_gain3*8
													  + pR855_rf_gain->RF_gain4*15;

    return R855_Success;
}


R855_ErrCode R855_RfGainMode(R855_RF_Gain_Type R855_RfGainType)
{
    UINT8 MixerGain = 0;
	UINT8 RfGain = 0;
	UINT8 LnaGain = 0;
	UINT8 FilterGain = 0;

	if(R855_RfGainType==R855_RF_MANUAL)
	{
		R855_I2C_Len.RegAddr = 0;
		R855_I2C_Len.Len = 8; 
		if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
		{
		    I2C_Read_Len(&R855_I2C_Len);
		}

		LnaGain = R855_I2C_Len.Data[6] & 0x1F;
		RfGain = ((R855_I2C_Len.Data[4] & 0xF0) >> 4);
		MixerGain = ((R855_I2C_Len.Data[5] & 0xF0) >> 4); 
		FilterGain = ((R855_I2C_Len.Data[5] & 0x0F) >> 0);

		//LNA auto off R13[0]=1
		R855_I2C.RegAddr = 13;
		R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] | 0x01;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

		//RFbuf auto off R17[3]=1
		R855_I2C.RegAddr = 17;
		R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] | 0x08;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

		// Mixer off R18[0]=0
		R855_I2C.RegAddr = 18;
		R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] & 0xFE;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;


		//Filter auto off R19[0]=0
		R855_I2C.RegAddr = 19;
		R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] & 0xFE;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

		//set LNA gain R13[5:1]
		R855_I2C.RegAddr = 13;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xC1) | LnaGain<<1;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

		//set Mixer Buffer gain R25[3:0] & Filter gain R25[7:4]
		R855_I2C.RegAddr = 25;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x00) | RfGain | (FilterGain<<4);
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

		//set Mixer R18[4:1]
		R855_I2C.RegAddr = 18;
		R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x00) | (MixerGain<<1);
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;
	}
	else
	{
	    //LNA auto off R13[0]=0
		R855_I2C.RegAddr = 13;
		R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] & 0xFE;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

		//RFbuf auto off R17[3]=0
		R855_I2C.RegAddr = 17;
		R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] & 0xF7;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

		// Mixer off R18[0]=1
		R855_I2C.RegAddr = 18;
		R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] | 0x01;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;

		//Filter auto off R19[0]=1
		R855_I2C.RegAddr = 19;
		R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] | 0x01;
		R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
		if(I2C_Write(&R855_I2C) != R855_Success)
		   return R855_Fail;
	}

    return R855_Success;
}

//------------------------------------------------------------------//
//  R855_PLL_Lock( ): Read PLL lock status (R3[6])                  //
//  Return: 1: PLL lock                                             //
//          0: PLL unlock                                           //
//------------------------------------------------------------------//
UINT8 R855_PLL_Lock(void)  //CD OK
{
	UINT8 fg_lock = 0;

	R855_I2C_Len.RegAddr = 0x00;
	R855_I2C_Len.Len = 2;
	if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
	{
	    I2C_Read_Len(&R855_I2C_Len);
	}

	if( (R855_I2C_Len.Data[1] & 0x80) == 0x00 ) 		
		fg_lock = 0;		
	else
        fg_lock = 1;

	return fg_lock;
}
UINT8 R855_Read_ChipID(void)  //CD OK
{
	UINT8 fg_lock = 0;


	R855_I2C_Len.RegAddr = 0x00;
	R855_I2C_Len.Len = 1;
	if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
	{
	    I2C_Read_Len(&R855_I2C_Len);
	}

	R855_PRINT("R855 Chip ID is 0x%x\n",R855_I2C_Len.Data[0]);
	return fg_lock;
}

R855_ErrCode R855_AGC_Slow(void)
{
	 //Set AGC ref clk	
	 if(R855_Xtal==16000)			
		 R855_Array[30] = (R855_Array[30] & 0xDF) | 0x00;  //clk(16)/Xtaldiv/2  
	 else
		 R855_Array[30] = (R855_Array[30] & 0xDF) | 0x20;  //clk(24)/Xtaldiv/3 

	 R855_I2C.RegAddr = 30;
	 R855_I2C.Data = R855_Array[30];
	 if(I2C_Write(&R855_I2C) != R855_Success)
		 return R855_Fail;

	 //AGC CLK slow
	 R855_I2C.RegAddr = 47;			
	 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF3) | 0x0C;  //R47[3:2]=11
	 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	 if(I2C_Write(&R855_I2C) != R855_Success)
		 return R855_Fail;

	 return R855_Success;
}
//------------------------------------------------------------------//
//  R855_SetIfFreq( ): Update tuner IF parameter                    //
//  Input para1: desired IF output freq                             //
//  Input para2: RF freq                                            //
//  Input para3: TV system number                                   //
//------------------------------------------------------------------//
R855_ErrCode R855_SetIfFreq(UINT16 IF_KHz, UINT32 RF_KHz, R855_Standard_Type u1SystemStd)
{
	R855_Sys_Info_Type     Sys_Info_temp;
	R855_Set_Info     R855_Info;

	R855_Info.R855_IfAgc_Select = R855_IF_AGC1;
	R855_Info.R855_Standard = u1SystemStd;

	Sys_Info_temp = R855_Sys_Sel(u1SystemStd);
	R855_Info.RF_KHz = RF_KHz + IF_KHz - Sys_Info_temp.IF_KHz;

	R855_SetFrequency(R855_Info);


	return R855_Success;
}

//-------------------------------------------------------------------------------//
//  R855_SetLpfBw( ): Set LPF coarse tune parameter: LPF_BW                      //
//  cover range if LPF_BW is:                                                    //
//		0: 9.45M~15.80MHz                                                        //
//		1: 7.75M~ 11.50MHz                                                        //
//		2: 6.55M~ 9.05MHz                                                        // 
//		3: 5.45M~ 7.05MHz                                                        // 
//-------------------------------------------------------------------------------//
R855_ErrCode R855_SetLpfBw(UINT8 LPF_BW)  
{
	UINT8 u1LpfBw_Msb;
	UINT8 u1LpfBw;

	//R22[4]R19[7:6]
	switch(LPF_BW)
	{
	case 0: //9M, R22[4]R19[7:6] = 000
		u1LpfBw_Msb=0;
		u1LpfBw=0x00;
		break;
	case 1: //8M, R22[4]R19[7:6] = 001
		u1LpfBw_Msb=0;
		u1LpfBw=0x40;
		break;
	case 2: //7M, R22[4]R19[7:6] = 011
		u1LpfBw_Msb=0;
		u1LpfBw=0xC0;
		break;
	case 3: //6M, R22[4]R19[7:6] = 111
		u1LpfBw_Msb=1;
		u1LpfBw=0xC0;
		break;
	default:
		u1LpfBw_Msb=0;
		u1LpfBw=0x00;
		break;
	}

	// Set LPF coarse BW MSB, R22[4] 
	R855_I2C.RegAddr = 22;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xEF) | (u1LpfBw_Msb<<4);
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	// Set LPF coarse BW, R19[7:6]
	R855_I2C.RegAddr = 19;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x3F) | u1LpfBw;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}

//------------------------------------------------------------------------------- //
//  R855_SetLpfOffset( ): Set LPF fine tune parameter: LPF_Offset                 //
//  range: 0x00~0x1F    (R19[4:0])                                                //
//  31 is narrowest; 0 is widest                                                  //
//--------------------------------------------------------------------------------//
R855_ErrCode R855_SetLpfOffset(UINT8 LPF_Offset)  
{
	// Set LPF fine code
	R855_I2C.RegAddr = 19;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE0) | (LPF_Offset);  //R19[4:0]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}

//---------------------------------------------------------------------//
//  R855_SetHpfOffset( ): Set HPF parameter: HPF_Offset                //
//  range is: 0x00~0x0F    (R24[3:0])                                  //
//  0 is narrowest; 15 is widest                                       //
//	0: 4.80M		4: 2.25M		8: 1.45M		12: 0.67M          //
//	1: 2.97M		5: 2.00M		9: 1.15M		13: 0.54M          //
//	2: 2.65M		6: 1.78M		10: 0.90M		14: 0.41M          //
//	3: 2.50M		7: 1.67M		11: 0.79M		15: 0.33M          //
//---------------------------------------------------------------------//
R855_ErrCode R855_SetHpfOffset(UINT8 HPF_Offset)  
{

	// Set HPF corner 
	R855_I2C.RegAddr = 24;
	R855_Array[24] = (R855_Array[24] & 0xF0) | HPF_Offset;
	R855_I2C.Data = R855_Array[24];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}

//------------------------------------------------------------------//
//  R855_SetIfLpf( ): Set LPF                                       //
//  Input parameter: LPF_Cor_Freq    (unit: KHz)                    //
//------------------------------------------------------------------//
R855_ErrCode R855_SetIfLpf(UINT32 LPF_Cor_Freq) //TBD
{
	UINT8     u1FilterCode;

	u1FilterCode = R855_Filt_Cal_ADC(LPF_Cor_Freq, 1, R855_Fil_Cal_Gap);

	// Set LPF coarse BW(R22[4] R19[7:6])
    switch(R855_Bandwidth)
    {
    case 0: //9M  000
		R855_Array[22] = (R855_Array[22] & 0xEF) | 0x00;
	    R855_Array[19] = (R855_Array[19] & 0x3F) | 0x00;
		break;
	case 1: //8M  001
	    R855_Array[22] = (R855_Array[22] & 0xEF) | 0x00;
	    R855_Array[19] = (R855_Array[19] & 0x3F) | 0x40;
        break;
	case 3: //7M  011
	    R855_Array[22] = (R855_Array[22] & 0xEF) | 0x00;
	    R855_Array[19] = (R855_Array[19] & 0x3F) | 0xC0;
        break;
	case 7: //6M  111
	    R855_Array[22] = (R855_Array[22] & 0xEF) | 0x10;
	    R855_Array[19] = (R855_Array[19] & 0x3F) | 0xC0;
        break;
	}
    R855_I2C.RegAddr = 19;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;
    R855_I2C.RegAddr = 22;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

    // Set LPF coarse BW(R19[5:1])
	R855_I2C.RegAddr = 19;
	R855_Array[R855_I2C.RegAddr ] = (R855_Array[R855_I2C.RegAddr ] & 0xC1) | (u1FilterCode<<2) | (R855_Lpf_Lsb<<1);
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr ];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}

//----------------------------------------------------------------------//
//  R855_GetRfRssi( ): Get RF RSSI                                      //
//  1st parameter: input RF Freq    (KHz)                               //
//  2nd parameter: input Standard                                       //
//  3rd parameter: output RF gain (dB*100)                              //
//----------------------------------------------------------------------//
R855_ErrCode R855_GetRfRssi(UINT32 RF_Freq_Khz, R855_Standard_Type RT_Standard, INT32 *RfGain)
{ 
	UINT8   lna_max=30; 
	UINT8   lna_min=0;
	UINT8   rf_max=15;
	UINT8   mixer_max=14;
	UINT8	mixer_1315_gain = 0;
	UINT8   RF_gain1, RF_gain2, RF_gain3, RF_gain4;
	UINT8   u1FreqIndex = 0;
	INT16   u2FreqFactor;
	UINT16  acc_lna_gain;
	INT16  acc_lna_gain_temp;
	INT16   rf_total_gain;
	UINT8  u1LnaGainqFactorIdx;

	RT_Standard = RT_Standard;

	R855_I2C_Len.RegAddr = 0x00;
	R855_I2C_Len.Len = 8;
	if(I2C_Read_Len(&R855_I2C_Len) != R855_Success)
	{
		I2C_Read_Len(&R855_I2C_Len);
	}
	RF_gain1 = (R855_I2C_Len.Data[6] & 0x1F);          //lna
	RF_gain2 = (R855_I2C_Len.Data[4] & 0xF0)>>4;          //rf
	RF_gain3 = (R855_I2C_Len.Data[5] & 0xF0)>>4;       //mixer
	RF_gain4 = (R855_I2C_Len.Data[5] & 0x0F);          //filter
	//R855_RfFlag = ((R855_I2C_Len.Data[5] & 0x10) >> 4);
	R855_PulseFlag = ((R855_I2C_Len.Data[3] & 0x40) >> 6);
	//R855_TF_Mode1 = (R855_Array[13] & 0x40) >> 6;          //R13[6], 0:auto    1:manuel
	R855_TF_Mode2 = (R855_Array[13] & 0x80) >> 7;          //R13[7], 0:plain   1:sharp

	//max LNA gain R17[2]
	if(R855_Array[17]&0x04)
		lna_max = 31;
	else
		lna_max = 30;

	//min LNA gain (enb_att) R17[0]
	if((R855_Array[17]&0x01)==0x01)
		lna_min=0;
	else//disable 0~5
		lna_min=6;



	//max RF buffer gain limit R17[6]
	if((R855_Array[17]&0x40)==0x40)
		rf_max = 15;
	else
		rf_max = 12;

	//max Mixer gain limit
	if((R855_Array[20]&0x60)==0x60)  //gain_limit=12
		mixer_max = 12;
	else if((R855_Array[20]&0x60)==0x40)  //gain_limit=10
		mixer_max = 10;
	else if((R855_Array[20]&0x60)==0x20)  //gain_limit=8
		mixer_max = 8;
	else
		mixer_max = 6;

	mixer_1315_gain = ((R855_Array[21]&0x80)>>7);			//R21[7]: 0:original, 1:ctrl by mixamp (>10)
	if(RF_gain1>lna_max)
		RF_gain1=lna_max;

	//if(RF_gain1<lna_min)
	//	RF_gain1=lna_min;

	if(RF_gain2>rf_max)
		RF_gain2=rf_max;


	if((RF_gain3 > 12)&&(mixer_1315_gain==1))
		mixer_1315_gain = RF_gain3;		//save 0 or 13 or 14 or 15

	if(RF_gain3>mixer_max)
		RF_gain3=mixer_max;
	


	//Select LNA freq table
	if(RF_Freq_Khz<=205000)   //<205M
	{
		u1FreqIndex = 0;
	}
	else if(RF_Freq_Khz>205000 && RF_Freq_Khz<=425000)   // 190~215M
	{
		u1FreqIndex = 1;
	}
	else if(RF_Freq_Khz>425000 && RF_Freq_Khz<=535000)   // 215~236M
	{
		u1FreqIndex = 2;
	}
	else if(RF_Freq_Khz>535000 && RF_Freq_Khz<=655000)   // 236~508M
	{
		u1FreqIndex = 3;
	}
	else   //>655M
	{
		u1FreqIndex = 4;
	}

	//LNA Gain
	acc_lna_gain = R850_Lna_Acc_Gain[u1FreqIndex][RF_gain1];

	u1LnaGainqFactorIdx = (UINT8) ((RF_Freq_Khz-50000) / 10000);
	if( ((RF_Freq_Khz-50000)  - (u1LnaGainqFactorIdx * 10000))>=5000)
		u1LnaGainqFactorIdx +=1;

	if(RF_gain1 >= 10) //10~31
	{
		acc_lna_gain_temp = (INT16)acc_lna_gain + (INT16)(Lna_Acc_Gain_offset_more_than_10[u1LnaGainqFactorIdx]*10);
	}
	else //0~9
	{
		acc_lna_gain_temp = (INT16)acc_lna_gain + (INT16)(Lna_Acc_Gain_offset_less_than_10[u1LnaGainqFactorIdx]*10);
	}
	acc_lna_gain = (UINT16)acc_lna_gain_temp;

	u2FreqFactor = 0; //rfrssi_offset_by_freq[u1LnaGainqFactorIdx];

 
	if((mixer_1315_gain!=0) && (mixer_1315_gain!=1))	//Gain 13 or 14 or 15
	{
		rf_total_gain = acc_lna_gain + R855_Rf_Acc_Gain[RF_gain2] + R855_Mixer_Acc_Gain[RF_gain3] + R855_Filter_Acc_Gain[RF_gain4] + (R855_Mixer_Acc_Gain[mixer_1315_gain] - R855_Mixer_Acc_Gain[12]);
	}
	else
	{
		rf_total_gain = acc_lna_gain + R855_Rf_Acc_Gain[RF_gain2] + R855_Mixer_Acc_Gain[RF_gain3] + R855_Filter_Acc_Gain[RF_gain4];
	}


	rf_total_gain = rf_total_gain - (INT16)u2FreqFactor;


    //3rd LPF gain(man)  R24[2]=0, when R24[2]=1 the rssi need add 3dB
	if((R855_Array[24]&0x04)==0x04)
	{
		rf_total_gain = rf_total_gain + 300;  //normal (for DTMB, <100M)
	}

	*RfGain = rf_total_gain;

    return R855_Success;
}



//----------------------------------------------------------------------//
//  R855_GetRssiADC( ): Get Output Power                                  //
//  1st parameter: return Output Power     (dBm*100)                      //
//----------------------------------------------------------------------//
R855_ErrCode R855_GetRssiADC(INT32 *OutputPower)
{
	UINT8   adc_read;	
	UINT8	rssi_mode_sel; //R45[7:6] [0:normal, 1:RSSI_log, 2:RSSI_filt, 3:RSSI_ifagc]
	UINT8	sel_det2;//R18[7]=[0:Pulse test, 1:IMR]
	UINT8	rssi_g; //R39[6:4]=[0:-10dB, 1:-5dB, 2:0dB, 3:5dB, 4:10dB, 5:15dB, 6:20dB, 7:25dB]
	INT32	rssi_g_offset;
	I2C_LEN_TYPE Dlg_I2C_Len;
	INT32   power_table[64] = {                        //*100
		-1130, -1130, -1130, -1130, -1130, -1130, -1130, -1130, -1130, -940,   //0~9
		-740, -490, -400, -270, -250, -230, -160, -130, -100, 0,   //10~19
		30, 110, 150, 190, 210, 230, 250, 290, 340, 410,   //20~29
		430, 450, 480, 490, 510, 570, 610, 640, 660, 680,   //30~39
		700, 720, 740, 800, 860, 920, 960, 1000, 1020, 1040,   //40~49
		1060, 1190, 1230, 1280, 1310, 1350, 1370, 1420, 1460, 1500,   //50~59
		1530, 1560, 1590, 1580              //60~63
	};


	//Recode original setting
	sel_det2 = R855_Array[18] & 0x80;
	rssi_g = R855_Array[39] & 0x70; //co-use with Filter vtl R39[6:4]
	rssi_mode_sel = R855_Array[45] & 0xC0;


	//1. agc fix on R23[7] = 1
	R855_I2C.RegAddr = 23;
	R855_Array[R855_I2C.RegAddr] |= 0x80;  
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;


	//2. select det2 R18[6] = 1
	//3. sel_det2 = IMR R18[7] = 1
	R855_I2C.RegAddr = 18;
	R855_Array[R855_I2C.RegAddr] |= 0xC0;  
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//4. RSSI mode sel RSSI_filt R45[7:6]=10
	R855_I2C.RegAddr = 45;
	R855_Array[R855_I2C.RegAddr] &= 0x3F;  
	R855_Array[R855_I2C.RegAddr] |= 0x80;  
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//5.rssi_g = 0dB R39[6:4] = 010
	R855_I2C.RegAddr = 39;
	R855_Array[R855_I2C.RegAddr] &= 0x8F;  
	R855_Array[R855_I2C.RegAddr] |= 0x20;  
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//ADC PW ON R8[6]=0
	R855_I2C.RegAddr = 8;
	R855_Array[R855_I2C.RegAddr] &= 0xBF;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//6. read ADC
	//wait 10ms
	R855_Delay_MS(10 * 1000);	// by ITE
	//good adc sensitivity is 30 to 56 (about 5 ~ 12dB).
	//if adc not 30~56, need to modify rssi_g R39[6:4]=>[0:-10dB, 1:-5dB, 2:0dB, 3:5dB, 4:10dB, 5:15dB, 6:20dB, 7:25dB]
	//read adc value
	Dlg_I2C_Len.RegAddr = 0x00;
	Dlg_I2C_Len.Len = 2;
	if(I2C_Read_Len(&Dlg_I2C_Len) != R855_Success) // read data length
	{
		I2C_Read_Len(&Dlg_I2C_Len);
	}
	adc_read = (Dlg_I2C_Len.Data[1] & 0x3F);
	R855_IF_RSSI_ADC_BEFORE = adc_read;  

	//check adc range, we wanted adc value is 30~56(5~12dB).
	//rssi_g: R39[6:4]=[0:-10dB, 1:-5dB, 2:0dB, 3:5dB, 4:10dB, 5:15dB, 6:20dB, 7:25dB]
	R855_I2C.RegAddr = 39;
	R855_Array[R855_I2C.RegAddr] &= 0x8F;
	if(power_table[adc_read]<-500)//+15dB
	{
		rssi_g_offset = 1500;
		R855_Array[R855_I2C.RegAddr] |= (5<<4);  
	}
	else if(power_table[adc_read]<0)//+10dB
	{
		rssi_g_offset = 1000;
		R855_Array[R855_I2C.RegAddr] |= (4<<4);
	}
	else if(power_table[adc_read]<500)//+5dB
	{
		rssi_g_offset = 500;
		R855_Array[R855_I2C.RegAddr] |= (3<<4);  
	}
	else if(power_table[adc_read]<1000)//+0dB
	{
		rssi_g_offset = 0;
		R855_Array[R855_I2C.RegAddr] |= (2<<4);  
	}
	else if(power_table[adc_read]<1500)//-5dB
	{
		rssi_g_offset = -500;
		R855_Array[R855_I2C.RegAddr] |= (1<<4);  
	}
	else if(power_table[adc_read]<2000)//-10dB
	{
		rssi_g_offset = -1000;
		R855_Array[R855_I2C.RegAddr] |= (0<4);
	}	
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	R855_Delay_MS(10 * 1000);	// by ITE

	//read adc value again
	Dlg_I2C_Len.RegAddr = 0x00;
	Dlg_I2C_Len.Len = 2;
	if(I2C_Read_Len(&Dlg_I2C_Len) != R855_Success) // read data length
	{
		I2C_Read_Len(&Dlg_I2C_Len);
	}
	adc_read = (Dlg_I2C_Len.Data[1] & 0x3F);
	R855_IF_RSSI_ADC_AFTER = adc_read;


	//ADC PW OFF R8[6]=1
	R855_I2C.RegAddr = 8;
	R855_Array[R855_I2C.RegAddr] |= 0x40;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//7. select RF_det2 R18[6] = 0
	R855_I2C.RegAddr = 18;
	R855_Array[R855_I2C.RegAddr] &= 0xBF;  
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//8.Restore setting

	// RSSI mode sel normal R45[7:6] [0:normal, 1:RSSI_log, 2:RSSI_filt, 3:RSSI_ifagc]
	R855_I2C.RegAddr = 45;
	R855_Array[R855_I2C.RegAddr] &= 0x3F;
	R855_Array[R855_I2C.RegAddr] |= rssi_mode_sel;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;


	//sel_det2 R18[7]=[0:Pulse test, 1:IMR]
	
	R855_I2C.RegAddr = 18;
	R855_Array[R855_I2C.RegAddr] &= 0x7F;
	R855_Array[R855_I2C.RegAddr] |= sel_det2;
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//rssi_g R39[6:4]=[0:-10dB, 1:-5dB, 2:0dB, 3:5dB, 4:10dB, 5:15dB, 6:20dB, 7:25dB]
	R855_I2C.RegAddr = 39;
	R855_Array[R855_I2C.RegAddr] &= 0x8F;
	R855_Array[R855_I2C.RegAddr] |= rssi_g; //co-use with Filter vtl R39[6:4]
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	//wait 750ms
	R855_Delay_MS(750 * 1000);	// by ITE

	//9. agc fix off R23[7] = 0
	R855_I2C.RegAddr = 23;
	R855_Array[R855_I2C.RegAddr] &= 0x7F;  
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	*OutputPower = (INT32)(power_table[adc_read] - (INT32)rssi_g_offset);

	return R855_Success;
}



//----------------------------------------------------------------------//
//  R855_GetTotalRssi( ): Get Total RSSI. Resolution 1dB                //
//  1st parameter: input RF Freq    (KHz)                               //
//  2nd parameter: input Standard                                       //
//  3rd parameter: return signal level indicator (dBm*100)                  //
//----------------------------------------------------------------------//
R855_ErrCode R855_GetTotalRssi(UINT32 RF_Freq_Khz, R855_Standard_Type RT_Standard, INT32 *RssiDbm)
{
	INT32   rf_rssi;
	//UINT32  if_rssi;
	INT32   output_power_rssi;
	INT32   total_rssi;
	INT32   ssi_offset = 30;   //need to fine tune by platform(Unit is dB*100)
	INT32   total_rssi_dbm=0;

	R855_GetRfRssi(RF_Freq_Khz, RT_Standard, &rf_rssi);

   
	R855_GetRssiADC(&output_power_rssi);

    total_rssi = (INT32) (output_power_rssi - rf_rssi);


	//for different platform, need to fine tune offset value
	*RssiDbm = total_rssi/100 + ssi_offset;
#if 0
	printf("\n[R855_GetTotalRssi]output_power_rssi=%d\n",output_power_rssi);
	printf("\n[R855_GetTotalRssi]rf_rssi=%d\n",rf_rssi);
	printf("\n[R855_GetTotalRssi]total_rssi=%d\n",total_rssi);
#endif


	return R855_Success;
}


//----------------------------------------------------------------------//
//  R855_SetXtalCap( ): Set R855 Internal Xtal Cap                      //
//  1st parameter: Xtal Cap value; range 0~41(pF)                       //
//----------------------------------------------------------------------//
R855_ErrCode R855_SetXtalCap(UINT8 u8XtalCap) //CD OK
{
	UINT8 XtalCap;
	UINT8 Capx;
	UINT8 Capx_3_0;

	if(u8XtalCap>31)
	{
		XtalCap = 1;  //10
		Capx = u8XtalCap-10;
	}
	else
	{
		XtalCap = 0; //0
        Capx = u8XtalCap;
	}

	//Capxx = Capx & 0x01;
    Capx_3_0 = Capx >> 1;
		
	// Set Xtal Cap
	R855_I2C.RegAddr = 33;
	R855_Array[33] = (R855_Array[33] & 0xD0)  | (XtalCap<<5) | Capx_3_0;  
	R855_I2C.Data = R855_Array[33];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}

R855_ErrCode R855_CheckMaxGain(void)
{
/*
	 //Enable Poly Gain (13~14), MT6 no use
	 R855_I2C.RegAddr = 19;
	 R855_Array[19] = (R855_Array[19] & 0xEF) | (1<<4);  
     R855_I2C.Data = R855_Array[19];
     if(I2C_Write(&R855_I2C) != R855_Success)
        return R855_Fail;
*/
	 //VGA Pin level(+4); delta(20dBc); MT6 change to 24dBc
	 R855_I2C.RegAddr = 25;
	 R855_Array[25] = (R855_Array[25] & 0xAF) | (0<<4) | (1<<6);  
     R855_I2C.Data = R855_Array[25];
     if(I2C_Write(&R855_I2C) != R855_Success)
        return R855_Fail;

	 //3th LPF gain (man)(+4)
	 R855_I2C.RegAddr = 24;
	 R855_Array[24] = (R855_Array[24] & 0xBF) | (1<<6);  
     R855_I2C.Data = R855_Array[24];
     if(I2C_Write(&R855_I2C) != R855_Success)
        return R855_Fail;

	 // Mixer Gain Limt (12)
	 R855_I2C.RegAddr = 20;
	 R855_Array[20] = (R855_Array[20] & 0x9F) | 0x60;
     R855_I2C.Data = R855_Array[20];
     if(I2C_Write(&R855_I2C) != R855_Success)
        return R855_Fail;

	return R855_Success;
}
R855_ErrCode R855_RfGainCtrl(R855_RF_Gain_Type R855_RfGainType, UINT8 LnaGain, UINT8 RfGain, UINT8 MixerGain, UINT8 FilterGain)
{
	if(R855_RfGainType==R855_RF_MANUAL)
	{
		//LNA auto off R13[0]=1
	     R855_I2C.RegAddr = 13;
	     R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] | 0x01;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

		 //Mixer buffer R17[3]=1
		 R855_I2C.RegAddr = 17;
	     R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] | 0x08;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;
		 
		 //Mixer off R18[0]=0
	     R855_I2C.RegAddr = 18;
	     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE);
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

		 //Filter auto off=0 R19[0]
	     R855_I2C.RegAddr = 19;
	     R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] & 0xFE;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

		//set LNA gain R13[5:1]
	     R855_I2C.RegAddr = 13;
	     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xC1) | LnaGain<<1;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

		 //set Mixer Buffer gain  R25[3:0]
	     R855_I2C.RegAddr = 25;
	     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF0) | RfGain;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

		 //set Mixer R18[4:1]
		 R855_I2C.RegAddr = 18; 
	     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xE1) | (MixerGain<<1);
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;
		 
		 //Filter gain R25[7:4]
	     R855_I2C.RegAddr = 25; 
	     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0x0F) | (FilterGain<<4);
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;
	}
	else
	{
	    //LNA auto on R13[0]=0
	     R855_I2C.RegAddr = 13;
	     R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] & 0xFE;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

		 //Mixer buffer R17[3]=0
		 R855_I2C.RegAddr = 17;
	     R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] & 0xF7;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;
		 
		 //Mixer on R18[0]=1
	     R855_I2C.RegAddr = 18;
	     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] | 0x01);
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

		 //Filter auto off=0 R19[0]
	     R855_I2C.RegAddr = 19;
	     R855_Array[R855_I2C.RegAddr] = R855_Array[R855_I2C.RegAddr] | 0x01;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

	}

    return R855_Success;
}
R855_ErrCode R855_IfGainCtrl(R855_IF_Gain_Type R855_IfGainType, UINT8 VgaGain)
{
	if(R855_IfGainType==R855_IF_MANUAL)
	{
		 //VGA code mode R23[0] = 1
	     R855_I2C.RegAddr = 23;
	     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE) | 0x01;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;

		 if(VgaGain==0)  //normal R23[3]=1
		 {
		 	 //VGA gain
			 R855_I2C.RegAddr = 23;
			 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF7) | 0x08;
			 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
			 if(I2C_Write(&R855_I2C) != R855_Success)
				   return R855_Fail;
		 }
		 else  //+4
		 {
		 	 //VGA gain
			 R855_I2C.RegAddr = 23;
			 R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF7) | 0x00;
			 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
			 if(I2C_Write(&R855_I2C) != R855_Success)
				   return R855_Fail;		 
		 }
	}
	else  //VGA auto
	{
		 //VGA auto mode  R23[0] = 0
	     R855_I2C.RegAddr = 23;
	     R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xFE) | 0x00;
		 R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	     if(I2C_Write(&R855_I2C) != R855_Success)
		       return R855_Fail;	
	}

	return R855_Success;
}
R855_ErrCode R855_Dump_Data(void) 
{
	UINT8 i ;
 	UINT8   R855_RfFlag, R855_PulseFlag;
	//UINT8   R855_TF_Mode1, R855_TF_Mode2;
	UINT8   RF_gain1, RF_gain2, RF_gain3, RF_gain4;

	R855_Delay_MS(10 * 1000);	// by ITE

	R855_I2C_Len.RegAddr = 0;
	R855_I2C_Len.Len = R855_REG_NUM;
	I2C_Read_Len(&R855_I2C_Len);

	RF_gain1 = (R855_I2C_Len.Data[3] & 0x1F);          //lna
	RF_gain2 = (R855_I2C_Len.Data[4] & 0x0F);          //rf
	RF_gain3 = (R855_I2C_Len.Data[4] & 0xF0)>>4;       //mixer
	RF_gain4 = (R855_I2C_Len.Data[5] & 0x0F);          //filter
	R855_RfFlag = ((R855_I2C_Len.Data[5] & 0x10) >> 4);
	R855_PulseFlag = ((R855_I2C_Len.Data[5] & 0x40) >> 6);
	//R855_TF_Mode1 = (R855_Array[13] & 0x40) >> 6;          //R13[6], 0:auto    1:manuel
	//R855_TF_Mode2 = (R855_Array[13] & 0x80) >> 7;          //R13[7], 0:plain   1:sharp

	//for customer to dump data
	

#if(R855_DEBUG==1)
	R855_PRINT("===================Start Tuner Log...=======================\n");
	R855_PRINT("LNA=%d, RF=%d, Mixer=%d, Filter=%d\n", RF_gain1, RF_gain2, RF_gain3, RF_gain4);
	R855_PRINT("RF_flag=%d, Pulse_flag=%d\n", R855_RfFlag, R855_PulseFlag);
	R855_PRINT("R855_PLL_Lock=%d,\n", R855_PLL_Lock());
	

	for(i=0;i<R855_REG_NUM;i++)
	{
		R855_PRINT("R%d=0x%X\n",i,R855_I2C_Len.Data[i]);
	}
	R855_PRINT("=============================================================\n");
#endif
	return R855_Success;
}


R855_ErrCode R855_Printf_Sys_Parameter(R855_Sys_Info_Type R855_Sys_Info)
{
#if(R855_DEBUG==1)
	R855_PRINT("======================R855_Sys_Info List...=========================\n");
	R855_PRINT("R855_Sys_Info.IF_KHz=%d\n", R855_Sys_Info.IF_KHz);
	R855_PRINT("R855_Sys_Info.FILT_CAL_IF=%d\n", R855_Sys_Info.FILT_CAL_IF);
	R855_PRINT("R855_Sys_Info.BW=%d\n", R855_Sys_Info.BW);
	R855_PRINT("R855_Sys_Info.HPF_COR=%d\n", R855_Sys_Info.HPF_COR);
	R855_PRINT("R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=%d\n", R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT);
	R855_PRINT("R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=%d\n", R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT);
	R855_PRINT("R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=%d\n", R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT);
	R855_PRINT("R855_Sys_Info.HPF_NOTCH_R24_7_1BT=%d\n", R855_Sys_Info.HPF_NOTCH_R24_7_1BT);
	R855_PRINT("R855_Sys_Info.FILT3_COMP_R36_3_2BT=%d\n", R855_Sys_Info.FILT3_COMP_R36_3_2BT);
	R855_PRINT("R855_Sys_Info.FILT5_FORCEQ_R43_0_1BT=%d\n", R855_Sys_Info.FILT5_FORCEQ_R43_0_1BT);
	R855_PRINT("R855_Sys_Info.FILT5_AUTO_COMP_R36_5_2BT=%d\n", R855_Sys_Info.FILT5_AUTO_COMP_R36_5_2BT);
	R855_PRINT("R855_Sys_Info.FILT5_MAN_COMP_R20_4_1BT=%d\n", R855_Sys_Info.FILT5_MAN_COMP_R20_4_1BT);
	R855_PRINT("R855_Sys_Info.NA_PWR_DET_R9_7_1BT=%d\n", R855_Sys_Info.NA_PWR_DET_R9_7_1BT);
	R855_PRINT("R855_Sys_Info.POLY_CUR_R12_3_1BT=%d\n", R855_Sys_Info.POLY_CUR_R12_3_1BT);
	R855_PRINT("R855_Sys_Info.RF_CLASSB_CHARGE_R36_1_1BT=%d\n", R855_Sys_Info.RF_CLASSB_CHARGE_R36_1_1BT);
	R855_PRINT("R855_Sys_Info.RF_RES_CAP_R24_5_2BT=%d\n", R855_Sys_Info.RF_RES_CAP_R24_5_2BT);
	R855_PRINT("R855_Sys_Info.BB_CLASSB_CHARGE_R17_1_1BT=%d\n", R855_Sys_Info.BB_CLASSB_CHARGE_R17_1_1BT);
	R855_PRINT("R855_Sys_Info.BB_RES_CAP_R23_6_1BT=%d\n", R855_Sys_Info.BB_RES_CAP_R23_6_1BT);
	R855_PRINT("R855_Sys_Info.LNA_PEAK_AVG_R46_5_1BT=%d\n", R855_Sys_Info.LNA_PEAK_AVG_R46_5_1BT);
	R855_PRINT("R855_Sys_Info.RF_PEAK_AVG_R39_7_1BT=%d\n", R855_Sys_Info.RF_PEAK_AVG_R39_7_1BT);
	R855_PRINT("R855_Sys_Info.BB_PEAK_AVG_R46_2_1BT=%d\n", R855_Sys_Info.BB_PEAK_AVG_R46_2_1BT);
	
	R855_PRINT("==========================================================\n");
#endif
	return R855_Success;
}
R855_ErrCode R855_Printf_SysFreq_Parameter(R855_SysFreq_Info_Type R855_SysFreq_Info)
{
#if(R855_DEBUG==1)
	

	R855_PRINT("======================R855_SysFreq_Info List...=========================\n");
	R855_PRINT("R855_SysFreq_Info.TF_MODE_R13_7_1BT=%d\n", R855_SysFreq_Info.TF_MODE_R13_7_1BT);
	R855_PRINT("R855_SysFreq_Info.Q_CTRL_R14_7_1BT=%d\n", R855_SysFreq_Info.Q_CTRL_R14_7_1BT);
	R855_PRINT("R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT=%d\n", R855_SysFreq_Info.LNA_MAX_GAIN_R17_2_1BT);
	R855_PRINT("R855_SysFreq_Info.ENB_ATT_R17_0_1BT=%d\n", R855_SysFreq_Info.ENB_ATT_R17_0_1BT);
	R855_PRINT("R855_SysFreq_Info.LNA_TOP_R37_0_4BT=%d\n", R855_SysFreq_Info.LNA_TOP_R37_0_4BT);
	R855_PRINT("R855_SysFreq_Info.LNA_VTH_R38_0_4BT=%d\n", R855_SysFreq_Info.LNA_VTH_R38_0_4BT);
	R855_PRINT("R855_SysFreq_Info.LNA_VTL_R13_1_4BT=%d\n", R855_SysFreq_Info.LNA_VTL_R13_1_4BT);
	R855_PRINT("R855_SysFreq_Info.RF_TOP_R37_4_3BT=%d\n", R855_SysFreq_Info.RF_TOP_R37_4_3BT);
	R855_PRINT("R855_SysFreq_Info.RF_VTH_R38_4_4BT=%d\n", R855_SysFreq_Info.RF_VTH_R38_4_4BT);
	R855_PRINT("R855_SysFreq_Info.RF_VTL_R25_0_4BT=%d\n", R855_SysFreq_Info.RF_VTL_R25_0_4BT);
	R855_PRINT("R855_SysFreq_Info.NRB_TOP_R40_4_4BT=%d\n", R855_SysFreq_Info.NRB_TOP_R40_4_4BT);
	R855_PRINT("R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT=%d\n", R855_SysFreq_Info.NRB_BW_LPF_R41_2_2BT);
	R855_PRINT("R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT=%d\n", R855_SysFreq_Info.NRB_BW_HPF_R34_5_2BT);
	R855_PRINT("R855_SysFreq_Info.MIXER_TOP_R40_0_2BT=%d\n", R855_SysFreq_Info.MIXER_TOP_R40_0_2BT);
	R855_PRINT("R855_SysFreq_Info.MIXER_VTH_R18_1_4BT=%d\n", R855_SysFreq_Info.MIXER_VTH_R18_1_4BT);
	R855_PRINT("R855_SysFreq_Info.MIXER_VTL_R39_0_3BT=%d\n", R855_SysFreq_Info.MIXER_VTL_R39_0_3BT);
	R855_PRINT("R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT=%d\n", R855_SysFreq_Info.MIXER_GAIN_LIMIT_R20_5_2BT);
	R855_PRINT("R855_SysFreq_Info.POLY_GAIN_R21_7_1BT=%d\n", R855_SysFreq_Info.POLY_GAIN_R21_7_1BT);
	R855_PRINT("R855_SysFreq_Info.FILTER_TOP_R40_2_2BT=%d\n", R855_SysFreq_Info.FILTER_TOP_R40_2_2BT);
	R855_PRINT("R855_SysFreq_Info.FILTER_VTH_R25_4_4BT=%d\n", R855_SysFreq_Info.FILTER_VTH_R25_4_4BT);
	R855_PRINT("R855_SysFreq_Info.FILTER_VTL_R39_4_3BT=%d\n", R855_SysFreq_Info.FILTER_VTL_R39_4_3BT);
	R855_PRINT("R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT=%d\n", R855_SysFreq_Info.MIXER_AMP_LPF_R35_2_3BT);
	R855_PRINT("R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT=%d\n", R855_SysFreq_Info.LPF_ACI_ENB_R34_7_1BT);
	R855_PRINT("R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT=%d\n", R855_SysFreq_Info.RBG_MIN_RFBUF_R13_6_1BT);
	R855_PRINT("R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT=%d\n", R855_SysFreq_Info.LNA_RF_DIS_MODE_R44_4_3BT);
	R855_PRINT("R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT=%d\n", R855_SysFreq_Info.LNA_DIS_CURR_R43_4_1BT);
	R855_PRINT("R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT=%d\n", R855_SysFreq_Info.RF_DIS_CURR_R43_5_1BT);
	R855_PRINT("R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT=%d\n", R855_SysFreq_Info.LNA_RF_DIS_SLOW_R44_0_2BT);
	R855_PRINT("R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT=%d\n", R855_SysFreq_Info.LNA_RF_DIS_FAST_R44_2_2BT);
	R855_PRINT("R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT=%d\n", R855_SysFreq_Info.SLOW_DIS_ENABLE_R41_0_1BT);
	R855_PRINT("R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT=%d\n", R855_SysFreq_Info.BB_DIS_CURR_R18_7_1BT);
	R855_PRINT("R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT=%d\n", R855_SysFreq_Info.MIXER_FILTER_DIS_R43_6_2BT);
	R855_PRINT("R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT=%d\n", R855_SysFreq_Info.FAGC_2_SAGC_R47_7_1BT);
	R855_PRINT("R855_SysFreq_Info.CLK_FAST_R47_2_2BT=%d\n", R855_SysFreq_Info.CLK_FAST_R47_2_2BT);
	R855_PRINT("R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT=%d\n", R855_SysFreq_Info.CLK_SLOW_R24_1_R13_5_2BT);
	R855_PRINT("R855_SysFreq_Info.LEVEL_SW_R45_4_1BT=%d\n", R855_SysFreq_Info.LEVEL_SW_R45_4_1BT);
	R855_PRINT("R855_SysFreq_Info.MODE_SEL_R45_5_1BT=%d\n", R855_SysFreq_Info.MODE_SEL_R45_5_1BT);
	R855_PRINT("R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT=%d\n", R855_SysFreq_Info.LEVEL_SW_VTHH_R37_7_1BT);
	R855_PRINT("R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT=%d\n", R855_SysFreq_Info.LEVEL_SW_VTLL_R46_1_1BT);
	R855_PRINT("R855_SysFreq_Info.IMG_ACI_R9_4_1BT=%d\n", R855_SysFreq_Info.IMG_ACI_R9_4_1BT);
	R855_PRINT("R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT=%d\n", R855_SysFreq_Info.FILTER_G_CONTROL_R21_4_1BT);
	R855_PRINT("R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT=%d\n", R855_SysFreq_Info.WIDEN_VTH_VTL_R45_0_2BT);
	R855_PRINT("R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT=%d\n", R855_SysFreq_Info.IMG_NRB_ADDER_R45_2_2BT);
	R855_PRINT("R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT=%d\n", R855_SysFreq_Info.FILT_3TH_GAIN_MAN_R24_2_1BT);
	R855_PRINT("R855_SysFreq_Info.HPF_COMP_R35_6_2BT=%d\n", R855_SysFreq_Info.HPF_COMP_R35_6_2BT);
	R855_PRINT("R855_SysFreq_Info.FB1_RES_R35_5_1BT=%d\n", R855_SysFreq_Info.FB1_RES_R35_5_1BT);
	R855_PRINT("R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT=%d\n", R855_SysFreq_Info.VGA_PIN_LVL_R23_3_1BT);
	R855_PRINT("R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT=%d\n", R855_SysFreq_Info.VGA_ATV_MODEL_R23_1_1BT);
	R855_PRINT("R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT=%d\n", R855_SysFreq_Info.VGA_OUT_ATT_R36_2_1BT);
	R855_PRINT("R855_SysFreq_Info.LNA_RF_CHARGE_R15_5_1BT=%d\n", R855_SysFreq_Info.LNA_RF_CHARGE_R15_5_1BT);
	R855_PRINT("R855_SysFreq_Info.TEMP=%d\n", R855_SysFreq_Info.TEMP);
	R855_PRINT("==========================================================\n");
#endif
	return R855_Success;
}
//-------------------------------------------------------------------------------//
//  R855_SetLnaTop( ): Set LNA TOP                                               //
//  Input parameter: LNA_TOP_R37_0_4BT, range 0~15                                         //
//    set LNA_TOP_R37_0_4BT higher can raise LNA gain;                                     //
//-------------------------------------------------------------------------------//
R855_ErrCode R855_SetLnaTop(UINT8 LNA_TOP_R37_0_4BT)  
{
	// Set LNA TOP
	R855_I2C.RegAddr = 37;
	R855_Array[R855_I2C.RegAddr] = (R855_Array[R855_I2C.RegAddr] & 0xF0) | (15 - LNA_TOP_R37_0_4BT);
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}

//-------------------------------------------------------------------------------//
//  R855_VgaCtrl( ): Set VGA mode                                               //
//  Input parameter:                                                             //  
//    -R855_VgaModeType: VGA_MANUAL or VGA_AUTO                                  // 
//    -u1ManualCode:     1:normal;   0:+4dB                                      //
//-------------------------------------------------------------------------------//
R855_ErrCode R855_VgaCtrl(R855_Vga_Mode_TYPE R855_VgaModeType, UINT8 u1ManualCode)
{
	if(R855_VgaModeType==VGA_MANUAL)  
		R855_Array[23] = (R855_Array[23] & 0xF6) | (1) | (u1ManualCode<<3);
	else    //VGA_AUTO
		R855_Array[23] = (R855_Array[23] & 0xF6) | 0x08;

	// Set VGA mode
	R855_I2C.RegAddr = 23;	
	R855_I2C.Data = R855_Array[R855_I2C.RegAddr];
	if(I2C_Write(&R855_I2C) != R855_Success)
		return R855_Fail;

	return R855_Success;
}




/*
R855_Sys_Info_Type R855_Sys_Sel_CIF_5M(R855_Standard_Type R855_Standard)
{
	R855_Sys_Info_Type R855_Sys_Info;

	 R855_Sys_Info = R855_Sys_Sel(R855_Standard);

	switch (R855_Standard)
	{

    case R855_MN_5800: 
	case R855_MN_5100:   
#if(R855_ATV_CIF_5M==TRUE)
	case R855_MN_CIF_5M: 
#endif
		R855_Sys_Info.IF_KHz=6750;                    //IF
		R855_Sys_Info.BW=0x00;                          //BW=8M
		R855_Sys_Info.FILT_CAL_IF=7750;          //CAL IF  (%)
		R855_Sys_Info.HPF_COR=8;	             
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;            //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		break;

	case R855_PAL_I:      
#if(R855_ATV_CIF_5M==TRUE)
	case R855_PAL_I_CIF_5M: 
#endif
		R855_Sys_Info.IF_KHz=7750;                    //IF
		R855_Sys_Info.BW=0x00;                          //BW=8M
		R855_Sys_Info.FILT_CAL_IF=9100;          //CAL IF  
		R855_Sys_Info.HPF_COR=10;	           
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;            //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		break;

	case R855_PAL_DK: 
#if(R855_ATV_CIF_5M==TRUE)
	case R855_PAL_DK_CIF_5M: 
#endif
		R855_Sys_Info.IF_KHz=7750;                    //IF
		R855_Sys_Info.BW=0x00;                          //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8950;          //CAL IF     
		R855_Sys_Info.HPF_COR=11;	          
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;      //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		break;

	case R855_PAL_B_7M:  
#if(R855_ATV_CIF_5M==TRUE)
	case R855_PAL_B_7M_6800:  
	case R855_PAL_B_7M_CIF_5M:  
#endif
		R855_Sys_Info.IF_KHz=7250;                     //IF
		R855_Sys_Info.BW=0x00;                           //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8450;           //CAL IF
		R855_Sys_Info.HPF_COR=10;	           
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;            //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		break;

    case R855_PAL_BGH_8M: 
#if(R855_ATV_CIF_5M==TRUE)
	case R855_PAL_BGH_8M_CIF_5M:
#endif
		R855_Sys_Info.IF_KHz=7750;                    //IF
		R855_Sys_Info.BW=0x00;                          //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8950;          //CAL IF
		R855_Sys_Info.HPF_COR=8;	            
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;            //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		break;

	case R855_SECAM_L:
#if(R855_ATV_CIF_5M==TRUE)
	case R855_SECAM_L_CIF_5M:
#endif
		R855_Sys_Info.IF_KHz=7750;                    //IF
		R855_Sys_Info.BW=0x00;                          //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8950;          //CAL IF
		R855_Sys_Info.HPF_COR=11;	           
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;            //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		break;

    case R855_SECAM_L1:   
#if(R855_ATV_CIF_5M==TRUE)
	case R855_SECAM_L1_CIF_5M:
#endif
        R855_Sys_Info.IF_KHz=2250;                    //IF
		R855_Sys_Info.BW=0x00;                          //BW=8M
		R855_Sys_Info.FILT_CAL_IF=9500;          //CAL IF
		R855_Sys_Info.HPF_COR=10;	             
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;            //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		break;

    case R855_SECAM_L1_INV: 
#if(R855_ATV_CIF_5M==TRUE)
	case R855_SECAM_L1_INV_CIF_5M:
#endif
		R855_Sys_Info.IF_KHz=7750;                    //IF
		R855_Sys_Info.BW=0x00;                          //BW=8M
		R855_Sys_Info.FILT_CAL_IF=8950;          //CAL IF
		R855_Sys_Info.HPF_COR=11;	          
		R855_Sys_Info.FILT_EXT_ENA_R17_7_1BT=0;            //ext disable [0:man, 1:auto]
		R855_Sys_Info.FILT_EXT_OPT_R43_1_3BT=0;         //[0:normal, 1:hp+1, 2:hp+2, 3:hp+3, 4:hp+3, 5:hp+3&lp-2, 6:hp+3&lp-4, 7:hp+3&lp-6]
		R855_Sys_Info.FILT_EXT_CONDI_R34_4_1BT=1;	//[0:& image=0, 1:only wanted]
		break;

	default:
		break;
	}

	return R855_Sys_Info;
}

*/
