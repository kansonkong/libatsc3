/*++
Copyright (c) 2006-2009 ITEtech Corporation. All rights reserved.

Module Name:
    iocontrol.cpp

Abstract:
    Demodulator IOCTL Query and Set functions
--*/
#include "it930x-core.h"
#include <linux/string.h>
#include <asm/ioctl.h>
#include <linux/stddef.h>
/*****************************************************************************
*
*  Function:   ioctrol_IT9300_GPIO
*
*  Arguments:  pdev             	- The device struct for get the handle.
* 			   iGPIOH				- The index of GPIOH 
*              byInOutput			- Input (0)/ Output (1) setting 
*              byValue				- Output Value Low (0) / High (1)
*
*  Returns:    Error_NO_ERROR: successful, non-zero error code otherwise.
*
*  Notes:
*
*****************************************************************************/
Dword ioctrol_IT9300_GPIO(IT930x_Device* pdev, int iGPIOH, Byte byInOutput, Byte byValue)
{
	Dword dwError = Error_NO_ERROR;
	Dword p_br_reg_top_gpiohx_en = 0;
	Dword p_br_reg_top_gpiohx_on = 0;
	Dword p_br_reg_top_gpiohx_o = 0;
	int chip = 0;
	
	switch (iGPIOH) {
	case 1:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh1_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh1_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh1_o;
		break;
	case 2:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh2_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh2_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh2_o;
		break;
	case 3:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh3_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh3_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh3_o;
		break;
	case 4:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh4_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh4_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh4_o;
		break;
	case 5:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh5_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh5_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh5_o;
		break;
	case 6:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh6_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh6_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh6_o;
		break;
	case 7:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh7_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh7_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh7_o;
		break;
	case 8:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh8_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh8_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh8_o;
		break;
	case 9:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh9_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh9_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh9_o;
		break;
	case 10:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh10_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh10_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh10_o;
		break;	
	case 11:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh11_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh11_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh11_o;
		break;	
	case 12:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh12_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh12_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh12_o;
		break;	
	case 13:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh13_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh13_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh13_o;
		break;	
	case 14:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh14_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh14_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh14_o;
		break;	
	case 15:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh15_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh15_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh15_o;
		break;	
	case 16:
		p_br_reg_top_gpiohx_en = p_br_reg_top_gpioh16_en;
		p_br_reg_top_gpiohx_on = p_br_reg_top_gpioh16_on;
		p_br_reg_top_gpiohx_o = p_br_reg_top_gpioh16_o;
		break;		
	default:
		dwError = Error_NOT_SUPPORT;
		goto GPIOFAIL;					
	}
	
	dwError = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpiohx_en, 0x01);
	if (dwError) 
		goto GPIOFAIL;
	dwError = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpiohx_on, byInOutput);
	if (dwError) 
		goto GPIOFAIL;
	dwError = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpiohx_o, byValue);
	if (dwError) 
		goto GPIOFAIL;

GPIOFAIL:	
	return dwError;
}

void Ioctl_16bit_Assign(Byte* buf,Word var,Byte Index)
{
	buf[Index+0] = (Byte)(var);
	buf[Index+1] = (Byte)(var>>8);
}

void Ioctl_32bit_Assign(Byte* buf,Dword var,Byte Index)
{
	buf[Index+0] = (Byte)(var);
	buf[Index+1] = (Byte)(var>>8);
	buf[Index+2] = (Byte)(var>>16);
	buf[Index+3] = (Byte)(var>>24);
}

Dword compat_it930x_null_function(
	void* dev, 
	unsigned long cmd, 
	unsigned long arg, 
	unsigned char number)
{
	unsigned int nr =_IOC_NR(cmd);
	printk("This callback function not support, nr = %d\n",nr);
	return 0;
}

Dword compat_it930x_getchannelstatic(
	void* dev, 
	unsigned long cmd, 
	unsigned long arg, 
	unsigned char number)
{
	int retcode = Error_NO_ERROR;
//	unsigned int nr = _IOC_NR(cmd);
//	char stack_kdata[128];
//	char *kdata = NULL;
//	unsigned int in_size, out_size, drv_size, ksize;
//	unsigned char i;
//	GetChannelStatisticRequest Request;
//	IT930x_Device* pdev = (IT930x_Device*) dev;
//	int br_idx = 0, ts_idx = 0;
//
//	printk("- Enter %s Function line %d\n", __func__,__LINE__);
//
//	drv_size = _IOC_SIZE(IOCTL_ITE_DEMOD_GETCHANNELSTATISTIC);
//	out_size = in_size =_IOC_SIZE(cmd);
//
//	//check io directioc is read or write
//	if ((cmd & IOCTL_ITE_DEMOD_GETCHANNELSTATISTIC & IOC_IN) == 0)
//		in_size = 0; //read
//	if ((cmd & IOCTL_ITE_DEMOD_GETCHANNELSTATISTIC & IOC_OUT) == 0)
//		out_size = 0; //write
//
//	ksize = max(max(in_size, out_size), drv_size);
//
//	if (ksize <= sizeof(stack_kdata)) {
//		kdata = stack_kdata;
//	} else {
//		kdata = kmalloc(ksize, GFP_KERNEL);
//		if (!kdata) {
//			retcode = -ENOMEM;
//			printk("kmalloc fail\n");
//			goto err_i1;
//		}
//	}
//
//	if (copy_from_user(kdata, (unsigned char *)arg, in_size) != 0) {
//		retcode = -EFAULT;
//		printk("copy_from_user fail\n");
//		goto err_i1;
//	}
//
//	if (ksize > in_size)
//		memset(kdata + in_size, 0, ksize - in_size);
//
//	br_idx = number/4;
//	ts_idx = number % 4;
//
//	Request.error = DL_Demodulator_getChannelStatistic(pdev->DC, ts_idx, &Request.channelStatistic, br_idx, ts_idx);
//
//	if (Request.error == 0) {
//		if (in_size == 0)
//			memset(kdata, 0, out_size);
//
//		Ioctl_16bit_Assign(kdata,Request.channelStatistic.abortCount,4);
//		Ioctl_32bit_Assign(kdata,Request.channelStatistic.postVitBitCount,8);
//		Ioctl_32bit_Assign(kdata,Request.channelStatistic.postVitErrorCount,12);
//		Ioctl_32bit_Assign(kdata,Request.channelStatistic.softBitCount,16);
//		Ioctl_32bit_Assign(kdata,Request.channelStatistic.softErrorCount,20);
//		Ioctl_32bit_Assign(kdata,Request.channelStatistic.preVitBitCount,24);
//		Ioctl_32bit_Assign(kdata,Request.channelStatistic.preVitErrorCount,28);
//		Ioctl_32bit_Assign(kdata,Request.error,32);
//	}
//
//	if (copy_to_user((unsigned char *)arg, kdata, out_size) != 0) {
//		printk("copy_to_user fail\n");
//		retcode = -EFAULT;
//	}
//	err_i1:
//	if (kdata != stack_kdata)
//		kfree(kdata);
		
	return retcode;
}

struct {
	it930x_ioctl_compat_t *fn;
	char *name;
} it930x_compat_ioctls[] = {
#define IT930X_IOCTL32_DEF(n, f)[_IOC_NR(IOCTL_ITE_DEMOD_##n)] = {.fn = f, .name = #n}
	IT930X_IOCTL32_DEF(WRITEREGISTERS, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(WRITEEEPROMVALUES, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(READREGISTERS, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(READEEPROMVALUES, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(GETCHANNELMODULATION, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(ACQUIRECHANNEL, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(ISLOCKED, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(SETSTATISTICRANGE, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(GETSTATISTIC, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(GETCHANNELSTATISTIC, compat_it930x_getchannelstatic),
	IT930X_IOCTL32_DEF(CONTROLPOWERSAVING, compat_it930x_null_function),
	IT930X_IOCTL32_DEF(GETBOARDINPUTPOWER, compat_it930x_null_function),
};

/*****************************************************************************
*
*  Function:   DemodIOCTLFun
*
*  Arguments:  dev             			- The device struct for get the handle.
*              IOCTLCode               - Device IO control code
*              pIOBuffer               - buffer containing data for the IOCTL
* 				 number					- Chip_Index
*
*  Returns:    Error_NO_ERROR: successful, non-zero error code otherwise.
*
*  Notes:
*
*****************************************************************************/
// IRQL:DISPATCH_LEVEL
Dword DemodIOCTLFun(
    IN void *			dev,
    IN Dword        IOCTLCode,
    IN unsigned long        pIOBuffer,
    IN Byte 		number)
{
    Dword error = Error_NO_ERROR;
//    int br_idx = 0, ts_idx = 0;
//    int i = 0;
//    IT930x_Device* pdev = (IT930x_Device*) dev;
//    unsigned int nr = _IOC_NR(IOCTLCode);
//	it930x_ioctl_compat_t *fn = NULL;
//
//    switch (IOCTLCode) {
//	case IOCTL_ITE_DEMOD_ACQUIRECHANNEL:
//	{
//		AcquireChannelRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, ts_idx = 0;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		//deb_data("br_idx: %d   tsSrcIdx: %u\n", br_idx, ts_idx);
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(AcquireChannelRequest)))
//			return -EFAULT;
//
//		if (Request.frequency == 0)
//			break;
//
//		//deb_data("[iocontrol] bandwith = %d\n", Request.bandwidth);
//
//		if ((pdev->DC->chip_Type[br_idx][ts_idx] != EEPROM_MXL691) &&
//			(pdev->DC->chip_Type[br_idx][ts_idx] != EEPROM_MXL691_DUALP) &&
//			(pdev->DC->chip_Type[br_idx][ts_idx] != EEPROM_MXL692) &&
//			(pdev->DC->chip_Type[br_idx][ts_idx] != EEPROM_MXL248) &&
//			(pdev->DC->chip_Type[br_idx][ts_idx] != EEPROM_CXD6801_NUVYYO) &&
//			(pdev->DC->chip_Type[br_idx][ts_idx] != EEPROM_CXD6801_EVB) &&
//			(pdev->DC->chip_Type[br_idx][ts_idx] != EEPROM_CXD6801_TS7) &&
//			(pdev->DC->chip_Type[br_idx][ts_idx] != EEPROM_MXL541C)) {
//			if (Request.bandwidth == 0)
//				break;
//		}
//
//		//if (!pdev->chip[number]->if_chip_start_capture) {
//		Request.error = DL_Demodulator_acquireChannel(pdev->DC, &Request, br_idx, ts_idx);
//
//		if (!Request.error)
//			printk("set Mode =%d, Freq = %d, BW = %d, success\n", Request.mode, Request.frequency, Request.bandwidth);
//		else
//			printk("set Mode =%d, Freq = %d, BW = %d, fail = 0x%x\n", Request.mode, Request.frequency, Request.bandwidth, Request.error);
//		//}
//		//else {
//		//	Request.error = 2;
//		//}
//		atomic_set(&pdev->clean_urb_flag, 0);
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(AcquireChannelRequest)))
//			return -EFAULT;
//
//		return Request.error;
//	}
//#if 1
//	case IOCTL_ITE_DEMOD_ISLOCKED:
//	{
//		IsLockedRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, ts_idx = 0;
//		Bool islocked = 0;
//
//		br_idx = number/4;
//		ts_idx = number % 4;
//
//		Request.error = DL_Demodulator_isLocked(pdev->DC, &islocked, br_idx, ts_idx);
//		Request.locked = islocked;
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(IsLockedRequest)))
//			return -EFAULT;
//
//    	break;
//	}
//#endif
//	case IOCTL_ITE_DEMOD_GETCHANNELSTATISTIC:
//	{
//		GetChannelStatisticRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, ts_idx = 0;
//
//		br_idx = number/4;
//		ts_idx = number % 4;
//
//		Request.error = DL_Demodulator_getChannelStatistic (pdev->DC, ts_idx, &Request.channelStatistic, br_idx, ts_idx);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(GetChannelStatisticRequest)))
//			return -EFAULT;
//
//    	break;
//	}
//	case IOCTL_ITE_DEMOD_GETSTATISTIC:
//    {
//		GetStatisticRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, ts_idx = 0;
//
//		br_idx = number/4;
//		ts_idx = number % 4;
//
//		Request.error = DL_Demodulator_getStatistic (pdev->DC, &Request, br_idx, ts_idx);
//		if (Request.error)
//			printk("[IOCTL] - Get statistic fail...!!!\n");
//#ifdef	PATCH_FOR_NX
//		if (Request.statistic.signalStrength < 100) {
//				Request.statistic.signalStrength = 100;
//				//deb_data(">>>>>>>>>> Strength change to  : %d\n\n", Request.statistic.signalStrength);
//		}
//#endif
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(GetStatisticRequest)))
//			return -EFAULT;
//
//    	break;
//	}
//    case IOCTL_ITE_DEMOD_GETDRIVERINFO:
//	{
//		DemodDriverInfo DriverInfo;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		Dword dwFWVersionLink = 0;
//		Dword dwFWVersionOFDM = 0;
//		int br_idx = 0, ts_idx = 0;
//
//		br_idx = number/4;
//		ts_idx = number % 4;
//
//        strcpy((char *)DriverInfo.DriverVersion, DRIVER_RELEASE_VERSION);
//		sprintf((char *)DriverInfo.APIVersion, "%x.%x.%x.%x", (Byte)(Version_NUMBER>>8), (Byte)(Version_NUMBER), Version_DATE, Version_BUILD);
//		sprintf((char *)DriverInfo.FWVersionLink, "%d.%d.%d.%d", DVB_LL_VERSION1, DVB_LL_VERSION2, DVB_LL_VERSION3, DVB_LL_VERSION4);
//		sprintf((char *)DriverInfo.FWVersionOFDM, "%d.%d.%d.%d", DVB_OFDM_VERSION1, DVB_OFDM_VERSION2, DVB_OFDM_VERSION3, DVB_OFDM_VERSION4);
//		strcpy((char *)DriverInfo.Company, "ITEtech");
//		strcpy((char *)DriverInfo.SupportHWInfo, "Endeavour");
//		DriverInfo.error = Error_NO_ERROR;
//		DriverInfo.BoardId = pdev->DC->board_id;
//		DriverInfo.chip_Type = pdev->DC->chip_Type[br_idx][ts_idx];
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&DriverInfo, sizeof(DemodDriverInfo)))
//			return -EFAULT;
//
//    	break;
//	}
//	case IOCTL_ITE_DEMOD_WRITEREGISTERS:
//	{
//		RegistersRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, ts_idx = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(RegistersRequest)))
//			return -EFAULT;
//
//		Request.chip = number;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		Request.error = DL_Demodulator_writeRegisters(pdev->DC, Request.processor, Request.option, Request.registerAddress, Request.bufferLength, Request.buffer, br_idx, ts_idx);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(RegistersRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_DEMOD_READREGISTERS:
//    {
//		RegistersRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, ts_idx = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(RegistersRequest)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		Request.error = DL_Demodulator_readRegisters(pdev->DC, Request.processor, Request.option, Request.registerAddress, Request.bufferLength, Request.buffer, br_idx, ts_idx);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(RegistersRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_WRITEREGISTERS:
//	{
//		RegistersRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(RegistersRequest)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		Request.error = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, br_idx, Request.registerAddress, *Request.buffer);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(RegistersRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_READREGISTERS:
//	{
//		RegistersRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(RegistersRequest)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		Request.error = IT9300_readRegister((Endeavour*) &pdev->DC->it9300, br_idx, Request.registerAddress, Request.buffer);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(RegistersRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_SETPIDFILTER:
//	{
//		PIDFilter Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		Word tsIndex[5];
//		int br_idx = 0, ts_idx = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(PIDFilter)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		tsIndex[1] = p_br_mp2if_ts1_pid_index;
//		tsIndex[2] = p_br_mp2if_ts2_pid_index;
//		tsIndex[3] = p_br_mp2if_ts3_pid_index;
//		tsIndex[4] = p_br_mp2if_ts4_pid_index;
//
//		if (Request.tableLen != 0) {
//			pdev->DC->it9300.tsSource[br_idx][ts_idx].tableLen    = Request.tableLen;
//			pdev->DC->it9300.tsSource[br_idx][ts_idx].orgPidTable = Request.oldPID;
//			pdev->DC->it9300.tsSource[br_idx][ts_idx].newPidTable = Request.newPID;
//			//pdev->DC->endeavour.tsSource[0][number].tsPort      = (TsPort)(number+1);
//
//			//deb_data("handle_index = %d\n",  number);
//
//			mutex_lock(&pdev->DC->dev_mutex);
//			Request.error = IT9300_enPidFilter((Endeavour*) &pdev->DC->it9300, br_idx, (Byte)ts_idx);
//			mutex_unlock(&pdev->DC->dev_mutex);
//		} else {
//			mutex_lock(&pdev->DC->dev_mutex);
//			Request.error = IT9300_writeRegister ((Endeavour*) &pdev->DC->it9300, br_idx, p_br_mp2if_pid_index_en, 0);
//			Request.error = IT9300_writeRegister ((Endeavour*) &pdev->DC->it9300, br_idx, tsIndex[ts_idx+1], 1);
//			mutex_unlock(&pdev->DC->dev_mutex);
//		}
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(PIDFilter)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_SETUARTBAUDRATE:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		SetUartBaudrate Request;
//		Dword dwBaudRate = 9600; //Default Baud Rate
//		UartBaudRate baudrate = UART_BAUDRATE_9600; //Default Baud Rate
//
//		if (copy_from_user((void *)&dwBaudRate, (void *)pIOBuffer, sizeof(unsigned long))) {
//			dwBaudRate = *((unsigned long *)pIOBuffer);
//			//return -EFAULT;
//		}
//
//		switch (dwBaudRate) {
//		case 9600:
//			baudrate = UART_BAUDRATE_9600;
//			break;
//		case 19200:
//			baudrate = UART_BAUDRATE_19200;
//			break;
//		case 38400:
//			baudrate = UART_BAUDRATE_38400;
//			break;
//		case 57600:
//			baudrate = UART_BAUDRATE_57600;
//			break;
//		default:
//			baudrate = UART_BAUDRATE_9600;
//			break;
//		}
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = IT9300_setUARTBaudrate(&pdev->DC->it9300, 0, baudrate);	//0 = which endeavour
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			printk("IT9300_setUARTBaudrate error!\n");
//		} else {
//			printk("IT9300_setUARTBaudrate OK!\n");
//			//copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(RegistersRequest));
//
//		}
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_SENTUARTDATA:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		Byte ReturnChannelCommand[RETURNCHANNEL_PACKETLENGTH];
//		Word RXDeviceID = 0xFFFF;
//		Word TXDeviceID = 0xFFFF;
//		SentUartData* pRequest;
//		SentUartData Request;
//		Dword dwLength = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(SentUartData))) {
//			pRequest = (SentUartData*) pIOBuffer;
//			//return -EFAULT;
//		} else
//			pRequest = &Request;
//
//		if (pRequest->len > RETURNCHANNEL_PACKETLENGTH)
//			dwLength = RETURNCHANNEL_PACKETLENGTH;
//		else
//			dwLength = pRequest->len;
//
//		if (copy_from_user((void*)ReturnChannelCommand, (void*)pRequest->cmd, dwLength)) {
//			memcpy(ReturnChannelCommand, (Byte*)pRequest->cmd, dwLength);
//			//return -EFAULT;
//		}
//		RXDeviceID = ((ReturnChannelCommand[1] & 0xFF) << 8) + (ReturnChannelCommand[2] & 0xFF);
//		TXDeviceID = ((ReturnChannelCommand[3] & 0xFF) << 8) + (ReturnChannelCommand[4] & 0xFF);
//
//		if (ReturnChannelCommand[0] != '#') {
//				printk("IOCTL_ITE_ENDEAVOUR_SENTUARTDATA: NO leading TAG !!!!\n");
//				break;
//		}
//
//		if(pdev->DC->board_id == 0x54) { //Multi-thread virtual com
//			if (pdev->DC->rx_device_id[number] != RXDeviceID) {
//				printk("IOCTL_ITE_ENDEAVOUR_SENTUARTDATA: RX DEVICE ERROR, SKIP !!!!\n");
//				break;
//			}
//		} else {
//			if ((pdev->DC->rx_device_id[number] != RXDeviceID) && (TXDeviceID != 0xFFFF)) {
//				printk("IOCTL_ITE_ENDEAVOUR_SENTUARTDATA: RX DEVICE ERROR, SKIP !!!!\n");
//				break;
//			}
//
//			if ((TXDeviceID == 0xFFFF) && (pdev->DC->rx_device_id[number] != pdev->DC->rx_device_id[0])) {
//				printk("IOCTL_ITE_ENDEAVOUR_SENTUARTDATA: Command Re-sent !!!! handle_number = %d\n", pdev->DC->rx_device_id[number]);
//				break;
//			}
//		}
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		if (pdev->DC->board_id == 0x54) { //Multi-thread virtual com
//				//printk("IT9300_sentUARTData switch to [%d]!\n", number);
//
//			switch (number) {
//			case 0:
//				error = ioctrol_IT9300_GPIO(pdev, 11, 1, 1);
//				if (error)
//					goto SWITCHFAIL;
//
//				error = ioctrol_IT9300_GPIO(pdev, 10, 1, 1);
//				if (error)
//					goto SWITCHFAIL;
//
//				break;
//			case 1:
//				error = ioctrol_IT9300_GPIO(pdev, 10, 1, 0);
//				if (error)
//					goto SWITCHFAIL;
//				error = ioctrol_IT9300_GPIO(pdev, 11, 1, 1);
//				if (error)
//					goto SWITCHFAIL;
//				break;
//			case 2:
//				error = ioctrol_IT9300_GPIO(pdev, 10, 1, 1);
//				if (error)
//					goto SWITCHFAIL;
//
//				error = ioctrol_IT9300_GPIO(pdev, 11, 1, 0);
//				if (error)
//					goto SWITCHFAIL;
//				break;
//			case 3:
//				error = ioctrol_IT9300_GPIO(pdev, 10, 1, 0);
//				if (error)
//					goto SWITCHFAIL;
//				error = ioctrol_IT9300_GPIO(pdev, 11, 1, 0);
//				if (error)
//					goto SWITCHFAIL;
//				break;
//			default:
//				goto SWITCHFAIL;
//			}
//			mdelay(10);
//		}
//		error = IT9300_sentUARTData(&pdev->DC->it9300, 0, dwLength, (Byte *)ReturnChannelCommand);
//SWITCHFAIL:
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error)
//			printk("IT9300_sentUARTData error!\n");
//		else
//			printk("IT9300_sentUARTData OK!\n");
//
//		break;
//	}
//	case IOCTL_ITE_DEMOD_DEALWITH_RETURNCHANNELPID: // From virtual com
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//
//		if (copy_from_user((void *)&pdev->DealWith_ReturnChannelPacket, (void *)pIOBuffer, sizeof(unsigned long))) {
//			pdev->DealWith_ReturnChannelPacket = *((unsigned long *)pIOBuffer);
//			//return -EFAULT;
//		}
//
//		if (pdev->DealWith_ReturnChannelPacket)
//			deb_data("- Enter %s Function - dev->DealWith_ReturnChannelPacket == Open\n", __func__);
//		else
//			deb_data("- Enter %s Function - dev->DealWith_ReturnChannelPacket == Close\n", __func__);
//
//		return 0;
//	}
//	case IOCTL_ITE_DEMOD_RESETPID:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		ResetPidRequest Request;
//		SimplePidTable Request_test;
//		int br_idx = 0, ts_idx = 0;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		Request.chip = number;
//		mutex_lock(&pdev->DC->dev_mutex);
//		Request.error = IT9300_simplePidFilter_ResetPidTable((Endeavour*) &pdev->DC->it9300, br_idx, ts_idx);
//		Request.error = IT9300_simplePidFilter_SetMode((Endeavour*) &pdev->DC->it9300, br_idx, ts_idx, 0);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(ResetPidRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_DEMOD_CONTROLPIDFILTER:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		ControlPidFilterRequest Request;
//		SimplePidTable Request_test;
//		int i = 0;
//		int br_idx = 0, ts_idx = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(ControlPidFilterRequest)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		Request.chip = number;
//		mutex_lock(&pdev->DC->dev_mutex);
//		if (Request.control == ENABLE_PID_MODE) {
//			Request.error = IT9300_simplePidFilter_SetMode((Endeavour*) &pdev->DC->it9300, br_idx, ts_idx, Request.mode);
//		} else
//			Request.error = IT9300_simplePidFilter_SetMode((Endeavour*) &pdev->DC->it9300, br_idx, ts_idx, 0);
//
//		mutex_unlock(&pdev->DC->dev_mutex);
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(ControlPidFilterRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_DEMOD_ADDPIDAT:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		AddPidAtRequest Request;
//		SimplePidTable Request_test;
//		int br_idx = 0, ts_idx = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(AddPidAtRequest)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		Request.chip = number;
//		mutex_lock(&pdev->DC->dev_mutex);
//		Request.error = IT9300_simplePidFilter_AddPid((Endeavour*) &pdev->DC->it9300, br_idx, ts_idx ,Request.pid.value, Request.index);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(AddPidAtRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_DEMOD_REMOVEPIDAT:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		RemovePidAtRequest Request;
//		SimplePidTable Request_test;
//		int br_idx = 0, ts_idx = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(RemovePidAtRequest)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		Request.chip = number;
//		mutex_lock(&pdev->DC->dev_mutex);
//		Request.error = IT9300_simplePidFilter_RemovePid((Endeavour*) &pdev->DC->it9300, br_idx, ts_idx , Request.index);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(RemovePidAtRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_DEMOD_READEEPROMVALUES:
//	{
//		ReadEepromValuesRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, i, checksum = 0;
//		Byte wBuffer[2];
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(ReadEepromValuesRequest)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//
//		//printk("addr = %d \n", pRequest->registerAddress);
//#if 1
//		/**********read 256 value(byte) from eeprom***********/
//		wBuffer[0] = (Byte)Request.registerAddress;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		Request.error = IT9300_writeGenericRegisters((Endeavour*) &pdev->DC->it9300, br_idx, 2, 0xA0, 1, wBuffer);
//		Request.error = IT9300_readGenericRegisters((Endeavour*) &pdev->DC->it9300, br_idx, 2, 0xA0, 1, Request.buffer);
//		mutex_unlock(&pdev->DC->dev_mutex);
//#endif
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(ReadEepromValuesRequest)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_DEMOD_WRITEEEPROMVALUES:
//	{
//		WriteEepromValuesRequest Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, i, checksum = 0;
//		Byte wBuffer[2];
//		Byte rBuffer[256];
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(WriteEepromValuesRequest)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//
//		for (i = 0; i < 256; i++) {
//			//deb_data ("[0x%02x] : 0x%02x\n", i, pRequest->buffer[i]);
//			wBuffer[0] = (Byte)i;
//			wBuffer[1] = Request.buffer[i];
//
//			mutex_lock(&pdev->DC->dev_mutex);
//			Request.error = IT9300_writeGenericRegisters((Endeavour*) &pdev->DC->it9300, br_idx, 2, 0xA0, 2, wBuffer);
//			mutex_unlock(&pdev->DC->dev_mutex);
//
//			if (Request.error)
//				deb_data("Write EEPROM fail!!!\n");
//
//			msleep(1);
//		}
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(WriteEepromValuesRequest)))
//			return -EFAULT;
//
//		deb_data("Write EEPROM finesh!!!\n");
//
//		break;
//	}
//	case IOCTL_ITE_DEMOD_GETBOARDINPUTPOWER:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		Word Request;
//		Byte value = 0, loop_flag;
//		int br_idx = 0, ts_idx = 0;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		//error = DL_Demodulator_getBoardInputPower((Demodulator *)pdev->DC->it9300.tsSource[br_idx][ts_idx].htsDev, &value);
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = DL_Demodulator_getBoardInputPower(pdev->DC, &value, br_idx, ts_idx);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			deb_data("Get Board Input Power fail!!!\n");
//			Request = 0;
//			return error;
//		}
//
//		Request = value;
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(Word)))
//			return -EFAULT;
//
//		return error;
//	}
//	case IOCTL_ITE_ENDEAVOUR_RXRESET:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		Word Request;
//		Byte value = 0;
//		int br_idx = 0, ts_idx = 0;
//		int chip = 0; //= br_chip
//		int i = 0, j = 0;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(Word)))
//			return -EFAULT;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		//error = DL_Demodulator_getBoardInputPower((Demodulator *)pdev->DC->it9300.tsSource[br_idx][ts_idx].htsDev, &value);
//		//error = DL_Demodulator_getBoardInputPower(pdev->DC, &value, br_idx, ts_idx);
//
//		//deb_data("---------RESET RX..!!    %d\n", *pRequest);
//		//return 0;
//
//		//Reset
//		mutex_lock(&pdev->DC->dev_mutex);
//
//		error = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpioh2_en, 0x01);
//		if (error)
//			goto resetfail;
//		error = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpioh2_on, 0x01);
//		if (error)
//			goto resetfail;
//		//error = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpioh2_o, 0x1);
//		//if (error) goto exit;
//		error = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpioh2_o, 0x0);
//		if (error)
//			goto resetfail;
//
//		//BrUser_delay((Endeavour*) &pdev->DC->it9300, 200);
//		mdelay(200);
//		error = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpioh2_o, 0x1);
//		if (error)
//			goto resetfail;
//		//BrUser_delay((Endeavour*) &pdev->DC->it9300, 200);
//		mdelay(200);
//
//		//BrUser_delay((Endeavour*) &pdev->DC->it9300, 100);
//		mdelay(200);
//		error = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpioh2_o, 0x0);
//		if (error)
//			goto resetfail;
//		//BrUser_delay((Endeavour*) &pdev->DC->it9300, 100);
//		mdelay(200);
//		error = IT9300_writeRegister((Endeavour*) &pdev->DC->it9300, chip, p_br_reg_top_gpioh2_o, 0x1);
//		if (error)
//			goto resetfail;
//
//		for (i = 0; i < pdev->DC->it9300_Number; i++) {
//			for (j = 0; j < pdev->DC->it9300.receiver_chip_number[i]; j++) {
//				switch (pdev->DC->chip_Type[i][j]) {
//				case EEPROM_IT913X:
//					error = DRV_IT913x_Initialize(pdev->DC, i, j);
//					break;
//				case EEPROM_Si2168B:
//					error = DRV_Si2168B_Initialize(pdev->DC, i, j);
//					break;
//				case EEPROM_Dibcom9090:
//					error = DRV_Dib9090_Initialize(pdev->DC, EEPROM_Dibcom9090);
//					break;
//				case EEPROM_Dibcom9090_4chan:
//					error = DRV_Dib9090_Initialize(pdev->DC, EEPROM_Dibcom9090_4chan);
//					break;
//				case EEPROM_MXL691: //MXL69x ATSC, Sean
//					error = DRV_MXL69X_Initialize(pdev->DC, i, j, EEPROM_MXL691);
//					break;
//				case EEPROM_MXL691_DUALP:
//					error = DRV_MXL69X_Initialize(pdev->DC, i, j, EEPROM_MXL691_DUALP);
//					break;
//				case EEPROM_MXL692:
//					error = DRV_MXL69X_Initialize(pdev->DC, i, j, EEPROM_MXL692);
//					break;
//				case EEPROM_MXL248:
//					error = DRV_MXL69X_Initialize(pdev->DC, i, j, EEPROM_MXL248);
//					break;
//				case EEPROM_CXD285X:
//					error = DRV_CXD285X_Initialize(pdev->DC, i, j, EEPROM_CXD285X);
//					break;
//				case EEPROM_CXD6801_NUVYYO: //Sony CXD6801
//					error = DRV_CXD6801_initialize(pdev->DC, i, j, EEPROM_CXD6801_NUVYYO);
//					break;
//				case EEPROM_CXD6801_EVB: //Sony CXD6801
//					error = DRV_CXD6801_initialize(pdev->DC, i, j, EEPROM_CXD6801_EVB);
//					break;
//				case EEPROM_CXD6801_TS7: //Sony CXD6801
//					error = DRV_CXD6801_initialize(pdev->DC, i, j, EEPROM_CXD6801_TS7);
//					break;
//				case EEPROM_EW100: //Sony CXD2880
//					error = DRV_CXD2880_initialize(pdev->DC, i, j, EEPROM_EW100);
//					break;
//				case EEPROM_CXD2880: //Sony CXD2880
//					error = DRV_CXD2880_initialize(pdev->DC, i, j, EEPROM_CXD2880);
//					break;
//				case EEPROM_CXD2880_33: //Sony CXD2880
//					error = DRV_CXD2880_initialize(pdev->DC, i, j, EEPROM_CXD2880_33);
//					break;
//				case EEPROM_MXL541C:
//					error = DRV_MXL541C_Initialize(pdev->DC, i, j, EEPROM_MXL541C);
//					break;
//				default:
//					deb_data("Unknown chip type [%d]\n", pdev->DC->chip_Type[i][j]);
//					break;
//				}
//			}
//		}
//
//		mutex_unlock(&pdev->DC->dev_mutex);
//resetfail:
//		if (error)
//			deb_data("--------  RESET FAIL....[0x%lx]\n", error);
//		else
//			deb_data("--------  RESET SUCCESS...!\n");
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(Word)))
//			return -EFAULT;
//
//		return error;
//	}
//	case IOCTL_ITE_ENDEAVOUR_GETBOARDID:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		Byte byRequest = 0;
//		mutex_lock(&pdev->DC->dev_mutex);
//		byRequest = (Byte) pdev->DC->board_id;
//		mutex_unlock(&pdev->DC->dev_mutex);
//		if (copy_to_user((void *)pIOBuffer, (void *)&byRequest, sizeof(Byte)))
//			return -EFAULT;
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_GETBOARDID_KERNEL:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		Byte byRequest = 0;
//		Byte* pRequest;
//
//		pRequest = (Byte *)pIOBuffer;
//		mutex_lock(&pdev->DC->dev_mutex);
//		byRequest = (Byte) pdev->DC->board_id;
//		mutex_unlock(&pdev->DC->dev_mutex);
//		deb_data("IOCTL_ITE_ENDEAVOUR_GETBOARDID_KERNEL = 0x%02x\n", byRequest);
//
//		*pRequest = byRequest;
//
//		break;
//	}
//	case IOCTL_ITE_DIVERSITY_GETDETAILDATA:
//	{
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//		int br_idx = 0, ts_idx = 0;
//		DibcomDiversityData* pRequest;
//
//		pRequest = (DibcomDiversityData*) kmalloc(sizeof(DibcomDiversityData), GFP_KERNEL);
//		if (pRequest == NULL) {
//			printk("[IOCTL: IOCTL_ITE_DIVERSITY_GETDETAILDATA] - kmalloc Fail!!\n");
//			return -EFAULT;
//		}
//
//		memset(pRequest, 0, sizeof(DibcomDiversityData));
//
//		pRequest->chip = number;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		pRequest->error = DL_Dib9090_getDiversityDetailData(pdev->DC, pRequest, pdev->DC->chip_Type[br_idx][ts_idx]);
//		mutex_unlock(&pdev->DC->dev_mutex);
//		if (pRequest->error) {
//			printk("DL_Dib9090_getDiversityDetailData fail!!!\n");
//			return -EFAULT;
//		}
//		if (copy_to_user((void *)pIOBuffer, (void *)pRequest, sizeof(DibcomDiversityData))) {
//			printk("[IOCTL: IOCTL_ITE_DIVERSITY_GETDETAILDATA] - copy_to_user Fail!!\n");
//			return -EFAULT;
//		}
//		kfree(pRequest);
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_BCAS_RESETCARD:
//	{
//		int br_idx = 0;
//		br_idx = number / 4;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = IT9300_bcasResetCard(&pdev->DC->it9300, br_idx);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			deb_data("IT9300_bcasResetCard chip = %d fail!!!\n", br_idx);
//			return -EFAULT;
//		}
//
//		deb_data("IT9300_bcasResetCard chip = %d OK!!!\n", br_idx);
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_BCAS_SENDDATA:
//	{
//		BCASData data;
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		if (copy_from_user((void *)&data, (void *)pIOBuffer, sizeof(BCASData)))
//			return -EFAULT;
//
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = IT9300_bcasSendData(&pdev->DC->it9300, br_idx, data.byLength, data.abyBuffer);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			deb_data("IT9300_bcasSendData chip = %d fail!!!\n", br_idx);
//			return -EFAULT;
//		}
//
//		deb_data("IT9300_bcasSendData chip = %d Length = %d OK!!!\n", br_idx, data.byLength);
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_BCAS_SETBAUDRATE:
//	{
//		Dword dwBaudRate = UART_BAUDRATE_9600;
//		UartBaudRate baudrate = UART_BAUDRATE_9600; //Default Baud Rate
//
//		br_idx = number / 4;
//
//		if (copy_from_user((void *)&dwBaudRate, (void *)pIOBuffer, sizeof(Dword)))
//			return -EFAULT;
//
//		deb_data("Set BaudRate = %lu !!!\n", dwBaudRate);
//		switch (dwBaudRate) {
//		case 9600:
//			baudrate = UART_BAUDRATE_9600;
//			break;
//		case 19200:
//			baudrate = UART_BAUDRATE_19200;
//			break;
//		default:
//			deb_data("Baudrate not supporting!!!\n");
//			baudrate = UART_BAUDRATE_9600;
//			break;
//		}
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = IT9300_bcasSetBaudrate(&pdev->DC->it9300, br_idx, baudrate);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			deb_data("IT9300_bcasSetBaudrate chip = %d fail!!!\n", br_idx);
//			return -EFAULT;
//		}
//
//		deb_data("IT9300_bcasSetBaudrate chip = %d OK!!!\n", br_idx);
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_BCAS_GETDATA:
//	{
//		BCASData data;
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		memset(&data, 0, sizeof(data));
//
//		if (copy_from_user((void *)&data, (void *)pIOBuffer, sizeof(BCASData)))
//			return -EFAULT;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = IT9300_bcasGetData(&pdev->DC->it9300, br_idx, &data.byLength, data.abyBuffer);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			deb_data("IT9300_bcasGetData chip = %d fail!!! error = 0x%lx\n", br_idx, error);
//			return -EFAULT;
//		}
//
//		deb_data("IT9300_bcasGetData chip = %d Length = %d OK!!!\n", br_idx, data.byLength);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&data, sizeof(BCASData)))
//		{
//			printk("[IOCTL: IOCTL_ITE_ENDEAVOUR_BCAS_GETDATA] - copy_to_user Fail!!\n");
//			return -EFAULT;
//		}
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_BCAS_GETISREADY:
//	{
//		Byte byResult = 0;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = IT9300_readRegister(&pdev->DC->it9300, br_idx, OVA_UART_RX_READY, &byResult);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			deb_data("IOCTL_ITE_ENDEAVOUR_BCAS_GETISREADY chip = %d fail!!!\n", br_idx);
//			return -EFAULT;
//		}
//
//		deb_data("IT9300_bcasGetIsReady chip = %d IsReady = %d OK!!!\n", br_idx, byResult);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&byResult, sizeof(Byte))) {
//			printk("[IOCTL: IOCTL_ITE_ENDEAVOUR_BCAS_GETISREADY] - copy_to_user Fail!!\n");
//			return -EFAULT;
//		}
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_BCAS_DETECTCARD:
//	{
//		Bool bResult = False;
//
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = IT9300_bcasDetectCard(&pdev->DC->it9300, br_idx, &bResult);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			deb_data("IT9300_bcasDetectCard chip = %d fail!!!\n", br_idx);
//			return -EFAULT;
//		}
//
//		deb_data("IT9300_bcasDetectCard chip = %d bResult = %d OK!!!\n", br_idx, bResult);
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&bResult, sizeof(Bool))) {
//			printk("[IOCTL: IOCTL_ITE_ENDEAVOUR_BCAS_DETECTCARD] - copy_to_user Fail!!\n");
//			return -EFAULT;
//		}
//
//		break;
//	}
//	case IOCTL_ITE_ENDEAVOUR_BCAS_GETATR:
//	{
//		Byte byResult = 0;
//		BCASData data;
//
//		memset(&data, 0, sizeof(data));
//		br_idx = number / 4;
//		ts_idx = number % 4;
//
//		mutex_lock(&pdev->DC->dev_mutex);
//		error = IT9300_readRegister(&pdev->DC->it9300, br_idx, OVA_UART_RX_LENGTH, &byResult);
//		mutex_unlock(&pdev->DC->dev_mutex);
//
//		if (error) {
//			deb_data("IOCTL_ITE_ENDEAVOUR_BCAS_GETATR chip = %d fail!!!\n", br_idx);
//			return -EFAULT;
//		}
//		if (byResult == 13)	{
//			mutex_lock(&pdev->DC->dev_mutex);
//			error = IT9300_bcasGetData(&pdev->DC->it9300, br_idx, &data.byLength, data.abyBuffer);
//			mutex_unlock(&pdev->DC->dev_mutex);
//		} else {
//			deb_data("IOCTL_ITE_ENDEAVOUR_BCAS_GETATR chip = %d  Wrong Length!!!\n", br_idx);
//			return -EFAULT;
//		}
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&data, sizeof(BCASData))) {
//			printk("[IOCTL: IOCTL_ITE_ENDEAVOUR_BCAS_GETRXLENGTH] - copy_to_user Fail!!\n");
//			return -EFAULT;
//		}
//
//		break;
//	}
//	case IOCTL_ITE_DVBT2_GETMPLPS:
//	{
//		MPLPData Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(MPLPData))) {
//			printk("[IOCTL: IOCTL_ITE_DVBT2_GETMPLPS] - copy_from_user Fail!!\n");
//			return -EFAULT;
//		}
//
//		switch (pdev->DC->chip_Type[br_idx][ts_idx]) {
//		case EEPROM_EW100:
//		case EEPROM_CXD2880:
//		case EEPROM_CXD2880_33:
//		{
//			Request.error = DRV_CXD2880_getMPLPID(pdev->DC, &Request, br_idx, ts_idx);
//			if (Request.error) {
//				printk("DL_ATBM_MXL_getMPLPID failed!!!\n");
//				return -EFAULT;
//			}
//			break;
//		}
//		default:
//			break;
//		}
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(MPLPData))) {
//			printk("[IOCTL: IOCTL_ITE_DVBT2_GETMPLP] - copy_to_user Fail!!\n");
//			return -EFAULT;
//		}
//		break;
//	}
//	case IOCTL_ITE_DVBT2_SETPLPID:
//	{
//		MPLPData Request;
//		IT930x_Device* pdev = (IT930x_Device*) dev;
//
//		if (copy_from_user((void *)&Request, (void *)pIOBuffer, sizeof(MPLPData))) {
//			printk("[IOCTL: IOCTL_ITE_DVBT2_SETPLPID] - copy_from_user Fail!!\n");
//			return -EFAULT;
//		}
//
//		switch (pdev->DC->chip_Type[br_idx][ts_idx]) {
//		case EEPROM_EW100:
//		case EEPROM_CXD2880:
//		case EEPROM_CXD2880_33:
//		{
//			Request.error = DRV_CXD2880_setPLPID(pdev->DC, &Request, br_idx, ts_idx);
//			if (Request.error) {
//				printk("DL_ATBM_MXL_setPLPID failed!!!\n");
//				return -EFAULT;
//			}
//			break;
//		}
//		default:
//			break;
//		}
//
//		if (copy_to_user((void *)pIOBuffer, (void *)&Request, sizeof(MPLPData))) {
//			printk("[IOCTL: IOCTL_ITE_DVBT2_SETPLPID] - copy_to_user Fail!!\n");
//			return -EFAULT;
//		}
//		break;
//	}
//	default:
//	{
//		fn = it930x_compat_ioctls[nr].fn;
//		if (fn) {
//			error = (*fn) ((void*)dev, IOCTLCode, pIOBuffer, number);
//			if (error)
//				printk(" === %s === fail\n",it930x_compat_ioctls[nr].name);
//		} else
//			deb_data("Error: command[0x%lx] not support\n", IOCTLCode);
//	}
//    }
    return error;
}
