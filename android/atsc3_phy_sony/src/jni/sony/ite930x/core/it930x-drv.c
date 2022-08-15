#include "it930x-core.h"

int dvb_usb_it930x_debug;
//module_param_named(debug,dvb_usb_it930x_debug, int, 0644);
extern Word g_def_device_id;

/**************************** DRV ****************************/
Dword DRV_IrTblDownload(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;
//	struct file *filp;
//	unsigned char b_buf[512] ;
//	int fileSize;
//	mm_segment_t oldfs;
//
//	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);
//
//	oldfs = get_fs();
//	set_fs(KERNEL_DS);
//	filp = filp_open("/lib/firmware/af35irtbl.bin", O_RDWR, 0644);
//	if (IS_ERR(filp)) {
//		deb_data("\t Error: open af35irtbl.bin fail\n");
//		goto exit;
//	}
//
//	if ((filp->f_op) == NULL) {
//		deb_data("\t Error: file operation method error\n");
//		goto exit;
//	}
//
//	filp->f_pos = 0x00;
//	fileSize = filp->f_op->read(filp, b_buf, sizeof(b_buf), &filp->f_pos);
//
//	//result = Demodulator_loadIrTable((Demodulator*)&DC->Demodulator, (Word)fileSize, b_buf);
//	if (result) {
//		deb_data("\t Error: Demodulator_loadIrTable [0x%08lx]\n", result);
//		goto exit;
//	}
//
//	filp_close(filp, NULL);
//	set_fs(oldfs);
//
//	deb_data("- %s success -\n",  __func__);
//	return result;
//
//	exit:
//	deb_data("- %s fail -\n",  __func__);
	return result;
}

/* Set tuner Frequency and BandWidth */
static Dword DRV_SetFreqBw(
		Device_Context *DC,
		Byte chip_index,
		Dword dwFreq,
		Word ucBw)
{
	Dword result = Error_NO_ERROR;

	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);
	//deb_data("\t chip[%d] desire to set Freq = %ld, BW = %d\n", chip_index, dwFreq, ucBw * 1000);

	if (DC->fc[chip_index].bEnPID) {
		//Demodulator_resetPid((Demodulator*)&DC->Demodulator, chip_index);
		/* Disable PID filter */
		//Demodulator_writeRegisterBits((Demodulator*)&DC->Demodulator, chip_index, Processor_OFDM, p_mp2if_pid_en, mp2if_pid_en_pos, mp2if_pid_en_len, 0);
	}

	DC->fc[chip_index].tunerinfo.bSettingFreq = true; //before acquireChannel, it is ture; otherwise, it is false

	if (!DC->fc[chip_index].tunerinfo.bTunerInited) {
		deb_data("\t Error: tuner power is off\n");
		goto exit;
	}

	if (dwFreq)
		DC->fc[chip_index].ulDesiredFrequency = dwFreq;

	if (ucBw)
		DC->fc[chip_index].ucDesiredBandWidth = ucBw * 1000;

	DC->fc[chip_index].tunerinfo.bTunerOK = false;
	if (DC->fc[chip_index].ulDesiredFrequency != 0 && DC->fc[chip_index].ucDesiredBandWidth != 0) {
		//result = Demodulator_acquireChannel((Demodulator*)&DC->Demodulator, chip_index, DC->fc[chip_index].ucDesiredBandWidth, DC->fc[chip_index].ulDesiredFrequency);
		if (result) {
			deb_data("\t Error: chip[%d] Demodulator_acquireChannel fail [0x%08lx]\n", chip_index, result);
			goto exit;
		} else {
			/* When success acquireChannel, record currentFreq/currentBW */
			DC->fc[chip_index].ulCurrentFrequency = DC->fc[chip_index].ulDesiredFrequency;
			DC->fc[chip_index].ucCurrentBandWidth = DC->fc[chip_index].ucDesiredBandWidth;
		}
	} else {
		deb_data("\t Warning: parameter not correct\n");
		goto exit;
	}

	if (DC->StreamType == StreamType_DVBT_DATAGRAM)
		DC->fc[chip_index].OvrFlwChk = 5;

	DC->fc[chip_index].tunerinfo.bTunerOK = true;

	/* Not the same as V4L, don't set the others chip to the same feq*/

	DC->fc[chip_index].tunerinfo.bSettingFreq = false;

	deb_data("- chip[%d] set Freq = %ld, BW = %d, success -\n", chip_index, DC->fc[chip_index].ulCurrentFrequency, DC->fc[chip_index].ucCurrentBandWidth);
	return result;

	exit:
	DC->fc[chip_index].tunerinfo.bSettingFreq = false;

	deb_data("- chip[%d] set Freq = %ld, BW = %d, fail -\n", chip_index, DC->fc[chip_index].ulDesiredFrequency, DC->fc[chip_index].ucDesiredBandWidth);
	return result;
}

/* Check tuner lock signal or not */
static Dword DRV_isLocked(
		Device_Context *DC,
		Byte chip_index,
		Bool *bLock)
{
	Dword result = Error_NO_ERROR;
	*bLock = true;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Demodulator_isLocked((Demodulator*)&DC->Demodulator, chip_index, bLock);
	if (result)
			deb_data("\t Error: Demodulator_isLocked fail [0x%08lx]\n", result);

	return result;
}

/* Get signal strength */
static Dword DRV_getSignalStrength(
		Device_Context *DC,
		Byte chip_index,
		Byte *strength)
{
	Dword result = Error_NO_ERROR;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Demodulator_getSignalStrength((Demodulator*)&DC->Demodulator, chip_index, strength);

	return result;
}

/* Get signal strengthDbm */
static Dword DRV_getSignalStrengthDbm(
		Device_Context *DC,
		Byte chip_index,
		Long *strengthDbm)
{
	Dword result = Error_NO_ERROR;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Demodulator_getSignalStrengthDbm((Demodulator*)&DC->Demodulator, chip_index, strengthDbm);

	return result;
}

/* Get Channel Statistic */
static Dword DRV_getChannelStatistic(
		Device_Context *DC,
		Byte chip_index,
		ChannelStatistic *channelStatistic)
{
	Dword result = Error_NO_ERROR;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Demodulator_getChannelStatistic((Demodulator*)&DC->Demodulator, chip_index, channelStatistic);

	return result;
}

/* Get Channel Modulation */
static Dword DRV_getChannelModulation(
		Device_Context *DC,
		Byte chip_index,
		ChannelModulation *channelModulation)
{
	Dword result = Error_NO_ERROR;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Standard_getChannelModulation((Demodulator*)&DC->Demodulator, chip_index, channelModulation);

	return result;
}

#if 0
/* Get SNR value */
static Dword DRV_getSNRValue(
	Device_Context *DC,
	Dword *snr_value)
{
	Dword result = Error_NO_ERROR;
	Byte snr_reg_23_16, snr_reg_15_8, snr_reg_7_0;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Demodulator_readRegister((Demodulator*)&DC->Demodulator, 0, Processor_OFDM, 0x2e, (unsigned char *)&snr_reg_23_16);
	if (result) {
		deb_data("\t Error: Demodulator_getSNR snr_reg_23_16 is failed\n");
		goto exit;
	}

	//result = Demodulator_readRegister((Demodulator*)&DC->Demodulator, 0, Processor_OFDM, 0x2d, (unsigned char *)&snr_reg_15_8);
	if (result) {
		deb_data("\t Error: Demodulator_getSNR snr_reg_15_8 is failed\n");
		goto exit;
	}

	//result = Demodulator_readRegister((Demodulator*)&DC->Demodulator, 0, Processor_OFDM, 0x2c, (unsigned char *)&snr_reg_7_0);
	if (result) {
		deb_data("\t Error: Demodulator_getSNR snr_reg_7_0 is failed\n");
		goto exit;
	}

	*snr_value = ((snr_reg_23_16 & 0xff) * 256 * 256) + ((snr_reg_15_8 & 0xff) * 256) + (snr_reg_7_0 & 0xff);

	deb_data("- %s : success -\n", __func__);
	return result;

exit:
	deb_data("- %s : fail -\n", __func__);
	return result;
}
#endif
/* Get OFDM /LINK Firmware version */
Dword DRV_getFirmwareVersionFromFile(
		Device_Context *DC,
		Dword* version)
{
	Byte chip_version = 0;
	Dword chip_Type;
	Byte var[2];
	Dword result = Error_NO_ERROR;

	Dword OFDM_VER1, OFDM_VER2, OFDM_VER3, OFDM_VER4;
	Dword LINK_VER1, LINK_VER2, LINK_VER3, LINK_VER4;

	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Demodulator_readRegister((Demodulator*) &DC->Demodulator, 0, Processor_LINK, chip_version_7_0, &chip_version);
	result = IT9300_readRegister(&DC->it9300, 0, chip_version_7_0, &chip_version);
	if (result) {
		deb_data("\t Error: IT9300_readRegister [chip_version_7_0] is failed\n");
		goto exit;
	}

	//result = Demodulator_readRegisters((Demodulator*) &DC->Demodulator, 0, Processor_LINK, (chip_version_7_0 + 1), 2, var);
	result = IT9300_readRegisters(&DC->it9300, 0, (chip_version_7_0 + 1), 2, var);
	if (result) {
		deb_data("\t Error: IT9300_readRegisters [chip_version_7_0 + 1] is failed\n");
		goto exit;
	}

	chip_Type = var[1]<<8 | var[0];

	//if(processor == Processor_OFDM)
	//	*version = (Dword)((OFDM_VER1 << 24) + (OFDM_VER2 << 16) + (OFDM_VER3 << 8) + OFDM_VER4);
	//else //LINK
	*version = (Dword)((DVB_LL_VERSION1 << 24) + (DVB_LL_VERSION2 << 16) + (DVB_LL_VERSION3 << 8) + DVB_LL_VERSION4);

	deb_data("- %s : success -\n", __func__);
	return result;

	exit:
	deb_data("- %s : fail -\n", __func__);
	return result;
}

Dword DRV_Initialize(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;
	Dword fileVersion = 0, cmdVersion = 0;
	Byte usb_dma_reg = 0, chip_index = 0;

	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	if (DC->it9300.booted) {
		/* From Standard_setBusTuner() -> Standard_getFirmwareVersion() */
		//result = DRV_getFirmwareVersionFromFile(DC, Processor_OFDM, &fileVersion);
		if (result) {
			deb_data("\t Error: getFirmwarDRV_getSNRValueeVersionFromFile fail [0x%08lx]\n", result);
			goto exit;
		}

		/* Use "Command_QUERYINFO" to get firmware version */
		//result = Demodulator_getFirmwareVersion((Demodulator*)&DC->Demodulator, Processor_OFDM, &cmdVersion);
		if (result) {
			deb_data("\t Error: getFirmwareVersion fail [0x%08lx]\n", result);
			goto exit;
		}

		if (cmdVersion != fileVersion) {
			deb_data("\t Warning: Outside_Fw = 0x%lX, Inside_Fw = 0x%lX, Reboot\n", fileVersion, cmdVersion);

			/* Not the same as V4L, no patch for 2 chips reboot */
			//result = Demodulator_reboot((Demodulator*)&DC->Demodulator);
			DC->bBootCode = true;

			if (result) {
				deb_data("\t Error: Reboot fail [0x%08lx]\n", result);
				goto exit;
			} else {
				result = Error_NOT_READY;
				goto exit;
			}
		} else {
			deb_data("\t Firmware version is the same\n");
			result = Error_NO_ERROR;
		}
	}

	/* ReInit: Patch for NIM fail or disappear, Maggie */
	//result = Demodulator_initialize((Demodulator*)&DC->Demodulator, DC->Demodulator.chipNumber, 8000, DC->StreamType, DC->architecture);
	if (result) {
		deb_data("\t Error: ReInit fail [0x%08lx]\n", result);
		goto exit;
	}

	//Demodulator_getFirmwareVersion((Demodulator*)&DC->Demodulator, Processor_OFDM, &cmdVersion);
	deb_data("\t FwVer OFDM = 0x%lX\n", cmdVersion);
	//Demodulator_getFirmwareVersion((Demodulator*)&DC->Demodulator, Processor_LINK, &cmdVersion);
	deb_data("\t FwVer LINK = 0x%lX\n", cmdVersion);

	/* Patch for 0-Byte USB packet error. write Link 0xDD8D[3] = 1 */
	for (chip_index = 0 ; chip_index < DC->it9300_Number; chip_index++) {
		//result = Demodulator_readRegister((Demodulator*)&DC->Demodulator, chip_index, Processor_LINK, 0xdd8d, &usb_dma_reg);
		usb_dma_reg |= 0x08; //reg_usb_min_len
		//result = Demodulator_writeRegister((Demodulator*)&DC->Demodulator, chip_index, Processor_LINK, 0xdd8d, usb_dma_reg);
	}

	deb_data("- %s success -\n",  __func__);
	return result;

	exit:
	deb_data("- %s fail -\n",  __func__);
	return result;
}

void DRV_InitDevInfo(
		Device_Context *DC,
		Byte chip_index)
{
	DC->fc[chip_index].ulCurrentFrequency = 0;
	DC->fc[chip_index].ucCurrentBandWidth = 0;
	DC->fc[chip_index].ulDesiredFrequency = 0;
	DC->fc[chip_index].ucDesiredBandWidth = 6000;

	/* For PID Filter Setting */
	DC->fc[chip_index].bEnPID = false;
	DC->fc[chip_index].bApOn = false;
	DC->fc[chip_index].bResetTs = false;
	DC->fc[chip_index].tunerinfo.bTunerOK = false;
	DC->fc[chip_index].tunerinfo.bSettingFreq = false;
}

static Dword DRV_GET_RX_ID (Device_Context *DC)
{
	Dword result = Error_NO_ERROR;
	Byte read_register = 0;
	Word map_enalbe_reg_addr = (OVA_EEPROM_CFG + 0x50);
	Word rxid_h_reg_addr = (OVA_EEPROM_CFG + 0x51);
	Word rxid_l_reg_addr = (OVA_EEPROM_CFG + 0x52);
	Dword temp = 0, temp2 = 0, i = 0;

	result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, map_enalbe_reg_addr, 1, &read_register);
	if (result) {
		deb_data("\t Error: Read EEPROM fail [0x%08lx]\n", result);
		goto exit;
	}

	if (read_register) {
		DC->map_enalbe = true;

		result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, rxid_h_reg_addr, 1, &read_register);
		temp = read_register;
		//deb_data("EEPROM - high value = %d \n", read_register);

		result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, rxid_l_reg_addr, 1, &read_register);
		temp2 = read_register;
		//deb_data("EEPROM - low value = %d \n", read_register);

		DC->device_id = (temp << 8) + temp2;
	} else {
		//deb_data("EEPROM - Mapp disalbe, Rx device ID using default\n");
		DC->device_id = g_def_device_id;
	}

	//deb_data("EEPROM - Device_ID = %ld \n", DC->device_id);

	//deb_data("- %s success -\n",  __func__);
	return result;

	exit:
	deb_data("EEPROM - Rx device ID using default\n");
	DC->device_id = g_def_device_id;

	deb_data("- %s fail -\n",  __func__);
	return result;
}

/* Get EEPROM_IRMODE/bIrTblDownload/bRAWIr/architecture config from EEPROM */
static Dword DRV_GetEEPROMConfig(Device_Context *DC)
{
	Dword	result = Error_NO_ERROR;
	Byte	chip_version = 0;
	Dword	chip_Type = 0;
	Byte	var[2];
	Byte	btmp = 0;
	Byte	eeprom_exit = 0;
	int i = 0, j = 0, br_idx = 0, ts_idx = 0;
	Word rxid_reg_addr = 0x49EC, Serial_No = -1;

	DC->it9300_Number = 1;
	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
		DC->it9300.receiver_chip_number[br_idx] = RX_CHIP_NUMBER;
	}
	DC->board_id = RX_BOARD_ID;
	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++)
		for(ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++)
			DC->chip_Type[br_idx][ts_idx] = RX_CHIP_TYPE;

	DC->device_id = g_def_device_id;
	DC->it9300.chipCount = 1;
	DC->it9300.bypassScript = True;
	DC->it9300.bypassBoot = False;
	DC->it9300.maxBusTxSize = 63;

	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
		DC->it9300.gator[br_idx].existed = True;
		DC->it9300.gator[br_idx].outTsPktLen = PKT_LEN_188;
		DC->it9300.gator[br_idx].booted = False;
		DC->it9300.gator[br_idx].initialized = False;

		if (br_idx == 0)
			DC->it9300.gator[br_idx].outDataType = OUT_DATA_USB_DATAGRAM;
		else
			DC->it9300.gator[br_idx].outDataType = OUT_DATA_TS_PARALLEL;

		for (ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {

			DC->it9300.tsSource[br_idx][ts_idx].existed = True;
			DC->it9300.tsSource[br_idx][ts_idx].tsType = TS_SERIAL;
			DC->it9300.tsSource[br_idx][ts_idx].tsPktLen = 188;	///Unknow
			DC->it9300.tsSource[br_idx][ts_idx].initialized = False; ///Unknow
			DC->it9300.tsSource[br_idx][ts_idx].htsDev = &DC->it9133[br_idx][ts_idx];
			DC->it9133[br_idx][ts_idx].userData = &DC->it9300;
			DC->it9133[br_idx][ts_idx].pendeavour = &DC->it9300;
			DC->it9133[br_idx][ts_idx].br_idx = br_idx;
			DC->it9133[br_idx][ts_idx].tsSrcIdx = ts_idx;

			if (ts_idx >= 2)
				DC->it9300.tsSource[br_idx][ts_idx].i2cBus = 2;
			else
				DC->it9300.tsSource[br_idx][ts_idx].i2cBus = 3;

			if ((ts_idx == 0)||(ts_idx == 2))
				DC->it9300.tsSource[br_idx][ts_idx].i2cAddr  = 0x38;
			else
				DC->it9300.tsSource[br_idx][ts_idx].i2cAddr = 0x3C;

			DC->it9300.tsSource[br_idx][ts_idx].tsPort = ts_idx + 1;

			DC->rx_device_id[((br_idx * 4) + ts_idx)] = DC->device_id;
			//deb_data("EEPROM - %d, %d, RxDevice_ID = 0x%lx \n", br_idx, ts_idx, DC->device_id);
			DC->device_id--;
		}
	}

	///check eeprom valid or not
	result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, OVA_EEPROM_CFG_VALID, 1, &eeprom_exit);
	if (result) {
		deb_data("EEPROM - 0x461D eeprom valid bit read fail!");
		goto exit;
	}

	//deb_data("EEPROM - Valid bit = 0x%02X \n", eeprom_exit);
	//printk("eeprom_exit = %d\n",eeprom_exit);
	if (eeprom_exit != 0) {
		//deb_data("EEPROM - ========== Can read eeprom \n");
		///Read Board ID

		result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_BOARDID, 1, &btmp);
		if (result) {
			deb_data("EEPROM - read Board ID fail \n");
			goto exit;
		}

		DC->board_id = btmp;

		//deb_data("EEPROM - Board ID: %ld \n", DC->board_id);
		///read IT930x NUM

		result	= IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_EVBNUM, 1, &btmp);
		if (result) {
			deb_data("EEPROM - read IT9300 NUM fail \n");
			goto exit;
		}

		//deb_data("EEPROM - Number of IT9300 is %d \n", btmp);

		DC->it9300_Number = btmp;

		///read chip type
		/*patch for read eeprom valid bit*/

		result = IT9300_readRegister ((Endeavour*) &DC->it9300, 0, chip_version_7_0, &chip_version); //chip =0
		result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, chip_version_7_0+1, 2, var);
		if (result) {
			deb_data("EEPROM - patch fail \n");
			goto exit;
		}

		chip_Type = var[1]<<8 | var[0];

		//deb_data("EEPROM - BR Chip Version is %d, BR Chip Type is 0x%lx \n", chip_version , chip_Type);

		for (i = 0; i < 4; i++)
			DC->it9300.gator[i].i2cAddr = 0x68 + (Byte)(i<<1);

		DC->it9300.gator[1].i2cAddr = 0x6a;//0x6a;

		///read NLC(Receiver) NUM
		for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
			DC->it9300.gator[br_idx].existed = True;

			DC->it9300.gator[br_idx].i2cAddr = 0x68 + (Byte)((i+1)<<1);

			if (br_idx == 0)
				DC->it9300.gator[0].outDataType = OUT_DATA_USB_DATAGRAM;
			else
				DC->it9300.gator[br_idx].outDataType = OUT_DATA_TS_PARALLEL;

			DC->it9300.gator[br_idx].outTsPktLen = PKT_LEN_188;
			DC->it9300.gator[br_idx].initialized = False;
			DC->it9300.gator[br_idx].booted = False;

			result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_EVBNUM + 1 + br_idx, 1, &btmp);
			if (result) {
				deb_data("EEPROM - read NLC NUM fail \n");
				goto exit;
			}

			//deb_data("EEPROM - %d NLC(Receiver) NUM: %d \n", br_idx, btmp);

			DC->it9300.receiver_chip_number[br_idx] = btmp;

			for (ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
				//Receiver chip type
				if (br_idx == 0) {
					if (ts_idx == 0)
						result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_1stNLC_1, 1, &btmp);
					else if (ts_idx == 1)
						result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_1stNLC_2, 1, &btmp);
					else if (ts_idx == 2)
						result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_1stNLC_3, 1, &btmp);
					else if (ts_idx == 3)
						result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_1stNLC_4, 1, &btmp);

					//deb_data("EEPROM - %d Receiver Chip Type is 0x%x \n", ts_idx , btmp);

					DC->chip_Type[br_idx][ts_idx] = btmp;
				} else {
					result = IT9300_readRegisters((Endeavour*) &DC->it9300, 0, EEPROM_CHIPTYPE_otherNLC, 1, &btmp);
					DC->chip_Type[br_idx][ts_idx] = btmp;
				}

				if (result) {
					deb_data("EEPROM - read NLC Chip Type: fail...!!\n");
					goto exit;
				}

				DC->it9300.tsSource[br_idx][ts_idx].tsPktLen = 188;	///Unknow
				DC->it9300.tsSource[br_idx][ts_idx].initialized = False; ///Unknow
				DC->it9300.tsSource[br_idx][ts_idx].existed = True;
				DC->it9300.tsSource[br_idx][ts_idx].htsDev = &DC->it9133[br_idx][ts_idx];
				DC->it9133[br_idx][ts_idx].userData = &DC->it9300;
				DC->it9133[br_idx][ts_idx].pendeavour = &DC->it9300;
				DC->it9133[br_idx][ts_idx].br_idx = br_idx;
				DC->it9133[br_idx][ts_idx].tsSrcIdx = ts_idx;
			}
		}

#if 1 //4 btye version
///Rx DEVICE ID
		result = DRV_GET_RX_ID(DC);

		if (DC->device_id == g_def_device_id) {
			for (br_idx = 0; br_idx <  DC->it9300_Number; br_idx++) {
				for (ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
					//DC->it9133[br_idx][ts_idx].returnchannel_rx_dev_id = (DC->device_id - ts_idx - (br_idx * 4));
					DC->rx_device_id[((br_idx * 4) + ts_idx)] = (DC->device_id - ts_idx - (br_idx * 4));
					//deb_data("EEPROM - %d, %d, RxDevice_ID = %ld \n", br_idx, ts_idx, (DC->device_id - ts_idx - (br_idx * 4)));
					g_def_device_id--;
				}
			}
		} else {
			for (br_idx = 0; br_idx <  DC->it9300_Number; br_idx++) {
				for (ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
					//DC->it9133[br_idx][ts_idx].returnchannel_rx_dev_id = (DC->device_id + ts_idx + (br_idx * 4));
					DC->rx_device_id[((br_idx * 4) + ts_idx)] = (DC->device_id + ts_idx + (br_idx * 4));
					//deb_data("EEPROM - %d, %d, RxDevice_ID = %ld \n", br_idx, ts_idx, (DC->device_id + ts_idx + (br_idx * 4)));
				}
			}
		}
#endif
	}

	if ((DC->board_id == 0x50) || (DC->board_id == 0x54)) { /* 4/8/12/16 */
		///4
		if (DC->it9300_Number == 1) {
		}
		///8
		if (DC->it9300_Number == 2) {
			/* 1st */
			DC->it9300.tsSource[0][4].tsType   = TS_PARALLEL;	/* 1st Port_0 */
			DC->it9300.tsSource[0][4].tsPort   = TS_PORT_0;		/* 1st Port_0 */

			/* 2st */
			DC->it9300.tsSource[1][0].i2cAddr = 0x38;
			DC->it9300.tsSource[1][1].i2cAddr = 0x3C;
			DC->it9300.tsSource[1][2].i2cAddr = 0x38;
			DC->it9300.tsSource[1][3].i2cAddr = 0x3C;
			DC->it9300.tsSource[1][0].i2cBus = 3;
			DC->it9300.tsSource[1][1].i2cBus = 3;
			DC->it9300.tsSource[1][2].i2cBus = 2;
			DC->it9300.tsSource[1][3].i2cBus = 2;
			DC->it9300.tsSource[1][0].tsType = TS_SERIAL;
			DC->it9300.tsSource[1][1].tsType = TS_SERIAL;
			DC->it9300.tsSource[1][2].tsType = TS_SERIAL;
			DC->it9300.tsSource[1][3].tsType = TS_SERIAL;
			DC->it9300.tsSource[1][0].tsPort = TS_PORT_1;
			DC->it9300.tsSource[1][1].tsPort = TS_PORT_2;
			DC->it9300.tsSource[1][2].tsPort = TS_PORT_3;
			DC->it9300.tsSource[1][3].tsPort = TS_PORT_4;
		}

		///12
		if (DC->it9300_Number == 3) {
			/* Temp CASE */
		}

		///16
		if (DC->it9300_Number == 4)	 {
			/* Temp CASE */
		}

	} else if (DC->board_id == 0x51) {	///5's
		/* Temp CASE */
	} else if (DC->board_id == 0x52) { // MXL69X ATSC device
		//MAX_NUMBER_OF_RX_FILTER
		DC->it9300.tsSource[0][0].i2cAddr = 0xC6;
		DC->it9300.tsSource[0][1].i2cAddr = 0xC0;
		DC->it9300.tsSource[0][2].i2cAddr = 0xC6;
		DC->it9300.tsSource[0][3].i2cAddr = 0xC0;
		DC->it9300.tsSource[0][0].i2cBus = 2;
		DC->it9300.tsSource[0][1].i2cBus = 2;
		DC->it9300.tsSource[0][2].i2cBus = 3;
		DC->it9300.tsSource[0][3].i2cBus = 3;
		DC->it9300.tsSource[0][4].i2cBus = 3;
		DC->it9300.tsSource[0][4].i2cAddr = 0x3E;
		DC->it9300.tsSource[0][4].tsType = TS_SERIAL;
		DC->it9300.tsSource[0][4].tsPort = TS_PORT_0;
	} else if (DC->board_id == 0x30) { ///2's
		if (DC->chip_Type[0][0] == EEPROM_MXL692) {
			DC->it9300.tsSource[0][0].i2cAddr = 0xC6;
			DC->it9300.tsSource[0][1].i2cAddr = 0xC0;
		} else {
			DC->it9300.tsSource[0][0].i2cAddr = 0x38;
			DC->it9300.tsSource[0][1].i2cAddr = 0x38;
		}

		if (DC->it9300.receiver_chip_number[0] == 2 && (DC->chip_Type[0][0]) == 0x00) { //check diversity
			DC->it9300.tsSource[0][0].i2cBus = 2;
			DC->it9300.tsSource[0][1].i2cBus = 3;
			DC->it9300.tsSource[0][0].tsPort = TS_PORT_4;
			DC->it9300.tsSource[0][1].tsPort = TS_PORT_0;
			DC->chip_Type[0][0] =  DC->chip_Type[0][1];
			DC->chip_Type[0][1] = 0x00;
			DC->it9300.receiver_chip_number[0] = 1;
		} else {
			DC->it9300.tsSource[0][0].i2cBus = 3;
			DC->it9300.tsSource[0][1].i2cBus = 2;
			DC->it9300.tsSource[0][0].tsPort = TS_PORT_0;
			DC->it9300.tsSource[0][1].tsPort = TS_PORT_4;
		}

		DC->it9300.tsSource[0][0].tag[0] = 0x00 + ((1)<<4);
		DC->it9300.tsSource[0][0].tag[1] = 0x01 + ((1)<<4);
		DC->it9300.tsSource[0][0].tag[2] = 0x02 + ((1)<<4);
		DC->it9300.tsSource[0][0].tag[3] = 0x03 + ((1)<<4);
		DC->it9300.tsSource[0][1].tag[0] = 0x00 + ((2)<<4);
		DC->it9300.tsSource[0][1].tag[1] = 0x01 + ((2)<<4);
		DC->it9300.tsSource[0][1].tag[2] = 0x02 + ((2)<<4);
		DC->it9300.tsSource[0][1].tag[3] = 0x03 + ((2)<<4);
	} else if (DC->board_id == 0x33) {
		DC->it9300.tsSource[0][0].i2cAddr = 0xC8;
		DC->it9300.tsSource[0][0].i2cBus = 2;
		DC->it9300.tsSource[0][0].tsType = TS_SERIAL;
		DC->it9300.tsSource[0][0].tsPort = TS_PORT_0;
	}

	if (eeprom_exit != 0)
			deb_data("============ Set Data From EEPROM Value ============\n");
	else
			deb_data("============ Set Data From Default Value ============\n");

	deb_data("SETTING INFO - Number of IT9300 is %d \n", DC->it9300_Number);

	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++)
			deb_data("SETTING INFO - %d NLC(Receiver) NUM: %d \n", br_idx, DC->it9300.receiver_chip_number[br_idx]);

	deb_data("SETTING INFO - Board ID: %ld \n", DC->board_id);
	deb_data("SETTING INFO - Device ID: %ld \n", DC->device_id);

	if (eeprom_exit != 0)
			deb_data("SETTING INFO - BR Chip Version is %d, BR Chip Type is 0x%lx \n", chip_version , chip_Type);

	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
		for(ts_idx = 0; ts_idx <  DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
			deb_data("SETTING INFO - %d, %d Receiver Chip Type is 0x%x \n", br_idx, ts_idx , DC->chip_Type[br_idx][ts_idx]);
			deb_data("SETTING INFO - %d, %d Receiver i2cAddr is 0x%x \n", br_idx, ts_idx , DC->it9300.tsSource[br_idx][ts_idx].i2cAddr);
			deb_data("SETTING INFO - %d, %d Receiver i2cBus is 0x%x \n", br_idx, ts_idx , DC->it9300.tsSource[br_idx][ts_idx].i2cBus);
			deb_data("SETTING INFO - %d, %d Receiver tsType is 0x%x \n", br_idx, ts_idx , DC->it9300.tsSource[br_idx][ts_idx].tsType);
			deb_data("SETTING INFO - %d, %d Receiver tsPort is 0x%x \n", br_idx, ts_idx , DC->it9300.tsSource[br_idx][ts_idx].tsPort);
			deb_data("SETTING INFO - %d, %d RxDevice_ID = %d \n", br_idx, ts_idx, DC->rx_device_id[((br_idx * 4) + ts_idx)]);
		}
	}

	deb_data("=====================================================\n");
	deb_data("- %s success -\n",  __func__);
	return result;

	exit:
	deb_data("- %s fail -\n",  __func__);
	return result;
}

/* Set Bus Tuner */
static Dword DRV_SetBusTuner(
		Device_Context *DC,
		Word busId,
		Word tunerId)
{
	Dword result = Error_NO_ERROR;
	Dword version = 0;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);
	//deb_data("\t Set busId = 0x%x, tunerId = 0x%x\n", busId, tunerId);

	if ((DC->UsbMode == 0x0110) && (busId == BUS_USB))
		busId = BUS_USB11;

	//result = Demodulator_setBusTuner((Demodulator*)&DC->Demodulator, busId, tunerId);
	if (result){
		deb_data("\t Error: Demodulator_setBusTuner fail [0x%08lx]\n", result);
		goto exit;
	}

	//result = Demodulator_getFirmwareVersion((Demodulator*)&DC->Demodulator, Processor_LINK, &version);
	if (version != 0)
		DC->it9300.booted = True;
	else
		DC->it9300.booted = False;

	if (result){
		deb_data("\t Error: Demodulator_getFirmwareVersion fail [0x%08lx]\n", result);
		goto exit;
	}

	exit:
	return result;
}

/* Set tuner power control */
static Dword DRV_TunerPowerCtrl(
		Device_Context *DC,
		Byte chip_index,
		Bool bPowerOn)
{
	Dword result = Error_NO_ERROR;

	deb_data("- Enter %s Function, chip[%d] -\n", __func__, chip_index);

	/* Omega has no GPIOH7 */

	if (bPowerOn)
		msleep(50);

	return result;
}

Dword DRV_TunerWakeup(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	/* Tuner power on */
	//result = Demodulator_writeRegisterBits((Demodulator*)&DC->Demodulator, 0, Processor_LINK,  p_reg_top_gpioh7_o, reg_top_gpioh7_o_pos, reg_top_gpioh7_o_len, 1);

	return result;
}

static Dword DRV_Reboot(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Demodulator_reboot((Demodulator*)&DC->Demodulator);

	return result;
}

Dword DRV_USBSetup(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;
	Byte chip_index = 0;

	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	if (DC->it9300.receiver_chip_number[0] == 2) {
		//timing
		for (chip_index = 0; chip_index < 2; chip_index++) {
			//result = Demodulator_writeRegisterBits((Demodulator*)&DC->Demodulator, chip_index, Processor_OFDM, p_reg_dca_fpga_latch, reg_dca_fpga_latch_pos, reg_dca_fpga_latch_len, 0x66);
			if(result)
				goto exit;

			//result = Demodulator_writeRegisterBits((Demodulator*)&DC->Demodulator, chip_index, Processor_OFDM, p_reg_dca_platch, reg_dca_platch_pos, reg_dca_platch_len, 1);
			if(result)
				goto exit;
		}
	}

	deb_data("- %s success -\n",  __func__);
	return result;

	exit:
	deb_data("- %s fail -\n",  __func__);
	return result;
}

Dword DRV_NIMSuspend(
		Device_Context *DC,
		bool bSuspend)
{
	Dword result = Error_NO_ERROR;

	/* Not the same as V4L, no return Error_NO_ERROR if only one chip */

	deb_data("- Enter %s Function : Suspend = %s -\n", __func__, bSuspend ? "ON" : "OFF");

	if (bSuspend) {
		//sleep
		//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 1);
		if(result)
			return result;

		msleep(50);
	} else {
		//resume
		//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
		if(result)
			return result;

		msleep(100);
	}

	return result;
}

Dword DRV_InitNIMSuspendRegs(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_en, reg_top_gpioh5_en_pos, reg_top_gpioh5_en_len, 1);
	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_on, reg_top_gpioh5_on_pos, reg_top_gpioh5_on_len, 1);
	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
	msleep(10);

	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 1, Processor_LINK, p_reg_top_pwrdw, reg_top_pwrdw_pos, reg_top_pwrdw_len, 1);
	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 1, Processor_LINK, p_reg_top_pwrdw_hwen, reg_top_pwrdw_hwen_pos, reg_top_pwrdw_hwen_len, 1);

	return result;
}

Dword DRV_NIMReset(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	/* Set AF0350 GPIOH1 to 0 to reset AF0351 */
	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK,  p_reg_top_gpioh1_en, reg_top_gpioh1_en_pos, reg_top_gpioh1_en_len, 1);
	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK,  p_reg_top_gpioh1_on, reg_top_gpioh1_on_pos, reg_top_gpioh1_on_len, 1);
	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK,  p_reg_top_gpioh1_o, reg_top_gpioh1_o_pos, reg_top_gpioh1_o_len, 0);

	//mdelay(50);
	msleep(50);

	//result = Demodulator_writeRegisterBits((Demodulator*) &DC->Demodulator, 0, Processor_LINK,  p_reg_top_gpioh1_o, reg_top_gpioh1_o_pos, reg_top_gpioh1_o_len, 1);

	return result;
}

Dword DRV_DummyCmd(Device_Context *DC)
{
	int count = 0;
	Dword result = Error_NO_ERROR;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	for (count = 0; count < 5 ; count++) {
		//deb_data("%s: patch for KJS/EEPC = %d\n", __func__, count);
		//result = DRV_SetBusTuner(DC, BUS_USB, Tuner_Afatech_OMEGA);
		msleep(1);
	}

	return result;
}

Dword NIM_ResetSeq(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;
	Byte bootbuffer[6];
	bootbuffer[0] = 0x05;
	bootbuffer[1] = 0x00;
	bootbuffer[2] = 0x23;
	bootbuffer[3] = 0x01;
	bootbuffer[4] = 0xFE;
	bootbuffer[5] = 0xDC;
	//checksum = 0xFEDC

	/* Reset 9300 -> boot -> demod init */

	//GPIOH5 init
	//result = Demodulator_writeRegisterBits((Demodulator*)&DC->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_en, reg_top_gpioh5_en_pos, reg_top_gpioh5_en_len, 0);
	//result = Demodulator_writeRegisterBits((Demodulator*)&DC->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_on, reg_top_gpioh5_on_pos, reg_top_gpioh5_on_len, 0);

	//result = Demodulator_writeRegisterBits((Demodulator*)&DC->Demodulator, 0, Processor_LINK, p_reg_top_gpioh5_o, reg_top_gpioh5_o_pos, reg_top_gpioh5_o_len, 0);
	//mdelay(100);

	result = DRV_NIMReset(DC);
	result = DRV_InitNIMSuspendRegs(DC);
	//result = Demodulator_writeGenericRegisters ((Demodulator*)&DC->Demodulator, 0, 0x01, 0x3a, 0x06, bootbuffer);
	//result = Demodulator_readGenericRegisters ((Demodulator*)&DC->Demodulator, 0, 0x01, 0x3a, 0x05, bootbuffer);

	msleep(50); //Delay for Fw boot

	//Demodulator_readRegister((Demodulator*)&DC->Demodulator, 1, Processor_LINK, 0x4100, &ucValue);

	/* Demod & Tuner init */
	result = DRV_Initialize(DC);

	return result;
}

/* Set tuner power saving and control */
static Dword DRV_ApCtrl(
		Device_Context *DC,
		Byte chip_index,
		Bool bOn)
{
	Dword result = Error_NO_ERROR;
	int count;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	/* clock from tuner, so close sequence demod->tuner, and 9133->9137. vice versa. */
	/* BUT! demod->tuner:57mADC, tuner->demod:37mADC */

	if (bOn) {
		/* Power on */
		if (chip_index == 1) {
			/* chip[1] */
			switch (DC->chip_version) {
				case 1:
					/* Omega 1 */
					result = NIM_ResetSeq(DC);
					if(result)
						NIM_ResetSeq(DC);
					break;
				case 2:
					/* Omega 2 */
					result = DRV_NIMSuspend(DC, false);
					if(result){
						deb_data("\t Error: GPIOH5 ON fail [0x%08lx]\n", result);
						goto exit;
					}
					break;
				default:
					deb_data("\t Error: chip version not defined [%d]\n", DC->chip_version);
					goto exit;
					break;
			}
		} else {
			/* chip[0] */
			/* DummyCmd */
			DC->UsbCtrlTimeOut = 1;
			for (count = 0; count < 5 ;count++) {
				//result = Demodulator_getFirmwareVersion((Demodulator*)&DC->Demodulator, Processor_LINK, &version);
				//mdelay(1);
				msleep(1);
			}
			DC->UsbCtrlTimeOut = 5;
		}

		//result = Demodulator_controlTunerPowerSaving((Demodulator*)&DC->Demodulator, chip_index, bOn);
		if (result) {
			deb_data("\t Error: chip[%d] Demodulator_controlTunerPowerSaving fail [0x%08lx]\n", chip_index, result);
			goto exit;
		}

		//result = Demodulator_controlPowerSaving((Demodulator*)&DC->Demodulator, chip_index, bOn);
		if (result) {
			deb_data("\t Error: chip[%d] Demodulator_controlPowerSaving fail [0x%08lx]\n", chip_index, result);
			goto exit;
		}
	} else {
		/* Power down */
		/* DCA DUT: 36(all) -> 47-159(no GPIOH5, sequence change) */
		//if((chip_index == 0) && (DC->endeavour.chipNumber == 2)){
		if ((chip_index == 0) && (DC->it9300.receiver_chip_number[0] == 2)) {
			//result = Demodulator_controlTunerLeakage((Demodulator*)&DC->Demodulator, 1, bOn);
			if (result) {
				deb_data("\t chip[1] Demodulator_controlTunerLeakage fail [0x%08lx]\n", result);
				goto exit;
			}

			result = DRV_NIMSuspend(DC, true);
			if (result) {
				deb_data("\t Error: GPIOH5 OFF fail [0x%08lx]\n", result);
				goto exit;
			}
		}

		//result = Demodulator_controlPowerSaving((Demodulator*)&DC->Demodulator, chip_index, bOn);
		if (result) {
			deb_data("\t Error: chip[%d] Demodulator_controlPowerSaving fail [0x%08lx]\n", chip_index, result);
			goto exit;
		}

		//result = Demodulator_controlTunerPowerSaving((Demodulator*)&DC->Demodulator, chip_index, bOn);
		if (result) {
			deb_data("\t Error: chip[%d] Demodulator_controlTunerPowerSaving fail [0x%08lx]\n", chip_index, result);
			goto exit;
		}
	}

	deb_data("- chip[%d] set POWER %s success -\n",chip_index, bOn ? "ON" : "DOWN");
	return result;

	exit:
	deb_data("- chip[%d] set POWER %s fail -\n",chip_index, bOn ? "ON" : "DOWN");
	return result;
}
#if 0
Dword DRV_getSNR(
	Device_Context *DC,
	Byte chip_index,
	Constellation *constellation,
	Byte *snr)
{
	Dword result = Error_NO_ERROR;
	ChannelModulation channelModulation;
	Dword snr_value;

	//deb_data("- Enter %s Function line %d\n", __func__,__LINE__);

	result = DRV_getChannelModulation(DC, chip_index, &channelModulation);
	if (result) {
		deb_data("\t Error: DRV_getChannelModulation fail [0x%08lx]\n", result);
		goto exit;
	}

	result = DRV_getSNRValue(DC, &snr_value);
	if (result) {
		deb_data("\t Error: DRV_getSNRValue fail [0x%08lx]\n", result);
		goto exit;
	}

	*constellation = channelModulation.constellation;

	switch (channelModulation.transmissionMode) {
	case TransmissionMode_2K:
		snr_value = snr_value * 4;
		break;
	case TransmissionMode_4K:
		snr_value = snr_value * 2;
		break;
	default:
		snr_value = snr_value * 1;
		break;
	}

	switch (channelModulation.constellation) {
	case 0:
		// Constellation_QPSK
		if (snr_value < 0xB4771)
			*snr = 0;
		else if (snr_value < 0xC1AED)
			*snr = 1;
		else if (snr_value < 0xD0D27)
			*snr = 2;
		else if(snr_value < 0xE4D19)
			*snr = 3;
		else if (snr_value < 0xE5DA8)
			*snr = 4;
		else if (snr_value < 0x107097)
			*snr = 5;
		else if (snr_value < 0x116975)
			*snr = 6;
		else if (snr_value < 0x1252D9)
			*snr = 7;
		else if (snr_value < 0x131FA4)
			*snr = 8;
		else if (snr_value < 0x13D5E1)
			*snr = 9;
		else if (snr_value < 0x148E53)
			*snr = 10;
		else if (snr_value < 0x15358B)
			*snr = 11;
		else if (snr_value < 0x15DD29)
			*snr = 12;
		else if (snr_value < 0x168112)
			*snr = 13;
		else if (snr_value < 0x170B61)
			*snr = 14;
		else if (snr_value < 0x17A532)
			*snr = 15;
		else if (snr_value < 0x180F94)
			*snr = 16;
		else if (snr_value < 0x186ED2)
			*snr = 17;
		else if (snr_value < 0x18B271)
			*snr = 18;
		else if (snr_value < 0x18E118)
			*snr = 19;
		else if (snr_value < 0x18FF4B)
			*snr = 20;
		else if (snr_value < 0x190AF1)
			*snr = 21;
		else if (snr_value < 0x191451)
			*snr = 22;
		else
			*snr = 23;
		break;
	case 1:
		// Constellation_16QAM
		if (snr_value < 0x4F0D5)
			*snr = 0;
		else if (snr_value < 0x5387A)
			*snr = 1;
		else if (snr_value < 0x573A4)
			*snr = 2;
		else if (snr_value < 0x5A99E)
			*snr = 3;
		else if (snr_value < 0x5CC80)
			*snr = 4;
		else if (snr_value < 0x5EB62)
			*snr = 5;
		else if (snr_value < 0x5FECF)
			*snr = 6;
		else if (snr_value < 0x60B80)
			*snr = 7;
		else if (snr_value < 0x62501)
			*snr = 8;
		else if (snr_value < 0x64865)
			*snr = 9;
		else if (snr_value < 0x69604)
			*snr = 10;
		else if (snr_value < 0x6F356)
			*snr = 11;
		else if (snr_value < 0x7706A)
			*snr = 12;
		else if (snr_value < 0x804D3)
			*snr = 13;
		else if (snr_value < 0x89D1A)
			*snr = 14;
		else if (snr_value < 0x93E3D)
			*snr = 15;
		else if (snr_value < 0x9E35D)
			*snr = 16;
		else if (snr_value < 0xA7C3C)
			*snr = 17;
		else if (snr_value < 0xAFAF8)
			*snr = 18;
		else if (snr_value < 0xB719D)
			*snr = 19;
		else if (snr_value < 0xBDA6A)
			*snr = 20;
		else if (snr_value < 0xC0C75)
			*snr = 21;
		else if (snr_value < 0xC3F7D)
			*snr = 22;
		else if (snr_value < 0xC5E62)
			*snr = 23;
		else if (snr_value < 0xC6C31)
			*snr = 24;
		else if (snr_value < 0xC7925)
			*snr = 25;
		else
			*snr = 26;
		break;
	case 2:
		// Constellation_64QAM
		if (snr_value < 0x256D0)
			*snr = 0;
		else if (snr_value < 0x27A65)
			*snr = 1;
		else if (snr_value < 0x29873)
			*snr = 2;
		else if (snr_value < 0x2B7FE)
			*snr = 3;
		else if (snr_value < 0x2CF1E)
			*snr = 4;
		else if (snr_value < 0x2E234)
			*snr = 5;
		else if (snr_value < 0x2F409)
			*snr = 6;
		else if (snr_value < 0x30046)
			*snr = 7;
		else if (snr_value < 0x30844)
			*snr = 8;
		else if (snr_value < 0x30A02)
			*snr = 9;
		else if (snr_value < 0x30CDE)
			*snr = 10;
		else if (snr_value < 0x31031)
			*snr = 11;
		else if (snr_value < 0x3144C)
			*snr = 12;
		else if (snr_value < 0x315DD)
			*snr = 13;
		else if (snr_value < 0x31920)
			*snr = 14;
		else if (snr_value < 0x322D0)
			*snr = 15;
		else if (snr_value < 0x339FC)
			*snr = 16;
		else if (snr_value < 0x364A1)
			*snr = 17;
		else if (snr_value < 0x38BCC)
			*snr = 18;
		else if (snr_value < 0x3C7D3)
			*snr = 19;
		else if (snr_value < 0x408CC)
			*snr = 20;
		else if (snr_value < 0x43BED)
			*snr = 21;
		else if (snr_value < 0x48061)
			*snr = 22;
		else if (snr_value < 0x4BE95)
			*snr = 23;
		else if (snr_value < 0x4FA7D)
			*snr = 24;
		else if (snr_value < 0x52405)
			*snr = 25;
		else if (snr_value < 0x5570D)
			*snr = 26;
		else if (snr_value < 0x59FEB)
			*snr = 27;
		else if (snr_value < 0x5BF38)
			*snr = 28;
		else
			*snr = 29;
		break;
	default:
		deb_data("\t Error: Get constellation is failed\n");
		goto exit;
		break;
	}

   	deb_data("- %s success -\n",  __func__);
	return result;

exit:
	deb_data("- %s fail -\n",  __func__);
    return result;
}
#endif

Dword DRV_ResetRxPort(Device_Context *DC, int is_open)
{
	Dword result = Error_NO_ERROR;
	Byte chip_index = 0, port_index = 0;

	for (chip_index = 0; chip_index < DC->it9300_Number; chip_index++) {

		for (port_index = 0; port_index < DC->it9300.receiver_chip_number[chip_index]; port_index++) {
			if (is_open == 1)
				result = IT9300_enableTsPort(&DC->it9300, chip_index, port_index);
			else
				result = IT9300_disableTsPort(&DC->it9300, chip_index, port_index);

			if (result) {
				deb_data("\t Error: Set chip[%d] port[%d] fail\n", chip_index, port_index);
				goto exit;
			}
		}

		if (is_open == 0) {
			mdelay(10);
			result = IT9300_writeRegister(&DC->it9300, chip_index, 0xda1d, 0x1);
			result = IT9300_writeRegister(&DC->it9300, chip_index, 0xda1d, 0x0);
		}
	}

	deb_data("- %s %s port success -\n",  __func__, (is_open == 1) ? "Enable" : "Disable");
	return result;

	exit:
	deb_data("- %s %s port fail -\n",  __func__, (is_open == 1) ? "Enable" : "Disable");
	return result;
}

//**************************** DL ****************************//

Dword DL_NIMReset(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_NIMReset(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_USBSetup(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_USBSetup(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_NIMSuspend(
		Device_Context *DC,
		bool bSuspend)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_NIMSuspend(DC, bSuspend);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_InitNIMSuspendRegs(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_InitNIMSuspendRegs(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_Initialize(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_Initialize(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_SetBusTuner(
		Device_Context *DC,
		Word busId,
		Word tunerId)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_SetBusTuner(DC, busId, tunerId);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_GetEEPROMConfig(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_GetEEPROMConfig(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_TunerWakeup(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_TunerWakeup(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_IrTblDownload(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_IrTblDownload(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_TunerPowerCtrl(
		Device_Context *DC,
		u8 bPowerOn)
{
	Dword result = Error_NO_ERROR;
	Byte chip_index = 0;
	int br_idx = 0;

	deb_data("- Enter %s Function, POWER %s -\n", __func__, bPowerOn ? "ON" : "OFF");

	//for(chip_index = 0; chip_index < DC->endeavour.chipNumber; chip_index++){
	for (chip_index = 0; chip_index < DC->it9300.receiver_chip_number[br_idx]; chip_index++) {
		mutex_lock(&DC->dev_mutex);
		result = DRV_TunerPowerCtrl(DC, chip_index, bPowerOn);
		mutex_unlock(&DC->dev_mutex);

		if (result) {
			deb_data("\t Error: chip[%d] DRV_TunerPowerCtrl fail [0x%08lx]\n", chip_index, result);
			goto exit;
		}
	}

	deb_data("- %s success -\n",  __func__);
	return result;

	exit:
	deb_data("- %s fail -\n",  __func__);
	return result;
}

Dword DL_Tuner_SetFreqBw(
		Device_Context *DC,
		Byte chip_index,
		u32 dwFreq,
		u8 ucBw)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	if (DC->fc[chip_index].ulDesiredFrequency != dwFreq || DC->fc[chip_index].ucDesiredBandWidth != (ucBw * 1000))
		result = DRV_SetFreqBw(DC, chip_index, dwFreq, ucBw);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_isLocked(
		Device_Context *DC,
		Byte chip_index,
		Bool *bLock)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_isLocked(DC, chip_index, bLock);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_getSignalStrength(
		Device_Context *DC,
		Byte chip_index,
		Byte *strength)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_getSignalStrength(DC, chip_index, strength);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_getSignalStrengthDbm(
		Device_Context *DC,
		Byte chip_index,
		Long *strengthDbm)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_getSignalStrengthDbm(DC, chip_index, strengthDbm);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_getChannelStatistic(
		Device_Context *DC,
		Byte chip_index,
		ChannelStatistic *channelStatistic)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_getChannelStatistic(DC, chip_index, channelStatistic);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_getChannelModulation(
		Device_Context *DC,
		Byte chip_index,
		ChannelModulation *channelModulation)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_getChannelModulation(DC, chip_index, channelModulation);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_Reboot(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_Reboot(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_CheckTunerInited(
		Device_Context *DC,
		Byte chip_index,
		bool *Tuner_inited)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	*Tuner_inited = DC->fc[chip_index].tunerinfo.bTunerInited;
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_DemodIOCTLFun(
		void *dev,
		Dword IOCTLCode,
		unsigned long pIOBuffer,
		Byte chip_index)
{
	Dword result = Error_NO_ERROR;

	result = DemodIOCTLFun(dev, IOCTLCode, pIOBuffer, chip_index);

	return result;
}

Dword DL_DummyCmd(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_DummyCmd(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

/* Set tuner power saving and control */
Dword DL_ApCtrl(
		Device_Context *DC,
		Byte chip_index,
		Bool bOn)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_ApCtrl(DC, chip_index, bOn);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

void UsbBus(Device_Context *DC)
{
	Dword temp;
	int i;
	temp = 2;

	DC->it9300.ctrlBus = (CtrlBus)temp;

	if ((DC->it9300.ctrlBus == BUS_USB) || (DC->it9300.ctrlBus == BUS_USB11)) {
		DC->it9300.maxBusTxSize = 63;
		DC->it9300.bypassScript = True;
		DC->it9300.bypassBoot = False;

		for (i = 0; i < 4; i++) {
			//if(i<DC->endeavour.br_chipNumber)
			if (i < DC->it9300_Number)
				DC->it9300.gator[i].existed = True;
			else
				DC->it9300.gator[i].existed = False;
		}

		DC->it9300.gator[0].outDataType = OUT_DATA_TS_PARALLEL;
		DC->it9300.gator[0].outTsPktLen = PKT_LEN_188;
		DC->it9300.gator[0].booted = False;

#if 0	///Multi Endeavour
		DC->it9300.tsSource[0][0].i2cBus 	= 0x03;
        DC->it9300.tsSource[0][0].i2cAddr	= 0x3E;
		DC->it9300.tsSource[0][0].tsPort	= TS_PORT_0;
        DC->it9300.tsSource[0][1].i2cBus 	= 0x03;
        DC->it9300.tsSource[0][1].i2cAddr	= 0x38;
		DC->it9300.tsSource[0][1].tsPort	= TS_PORT_1
        DC->it9300.tsSource[0][2].i2cBus 	= 0x03;
        DC->it9300.tsSource[0][2].i2cAddr	= 0x3c;
		DC->it9300.tsSource[0][2].tsPort	= TS_PORT_2;
        DC->it9300.tsSource[0][3].i2cBus 	= 0x02;
        DC->it9300.tsSource[0][3].i2cAddr	= 0x38;
		DC->it9300.tsSource[0][3].tsPort	= TS_PORT_3;
        DC->it9300.tsSource[0][4].i2cBus 	= 0x02;
        DC->it9300.tsSource[0][4].i2cAddr	= 0x3c;
		DC->it9300.tsSource[0][4].tsPort	= TS_PORT_4;
#endif

#if 1 ///Single Endeavour
		// settings for 1st TS device
		DC->it9300.tsSource[0][0].existed  = True;
		DC->it9300.tsSource[0][0].tsType   = TS_SERIAL;//TS_PARALLEL;
		DC->it9300.tsSource[0][0].i2cAddr  = 0x38;
		DC->it9300.tsSource[0][0].i2cBus   = 3;
		DC->it9300.tsSource[0][0].tsPort   = TS_PORT_1;
		// settings for 2nd TS device
		DC->it9300.tsSource[0][1].existed  = True;
		DC->it9300.tsSource[0][1].tsType   = TS_SERIAL;
		DC->it9300.tsSource[0][1].i2cAddr  = 0x3C;
		DC->it9300.tsSource[0][1].i2cBus   = 3;
		DC->it9300.tsSource[0][1].tsPort   = TS_PORT_2;
		// settings for 3st TS device
		DC->it9300.tsSource[0][2].existed  = True;
		DC->it9300.tsSource[0][2].tsType   = TS_SERIAL;
		DC->it9300.tsSource[0][2].i2cAddr  = 0x38;
		DC->it9300.tsSource[0][2].i2cBus   = 2;
		DC->it9300.tsSource[0][2].tsPort   = TS_PORT_3;
		// settings for 4st TS device
		DC->it9300.tsSource[0][3].existed  = True;
		DC->it9300.tsSource[0][3].tsType   = TS_SERIAL;
		DC->it9300.tsSource[0][3].i2cAddr  = 0x3C;
		DC->it9300.tsSource[0][3].i2cBus   = 2;
		DC->it9300.tsSource[0][3].tsPort   = TS_PORT_4;
#endif
	} else {
		DC->it9300.ctrlBus = BUS_NULL;
		DC->it9300.maxBusTxSize = 54;
	}
}

Dword DRV_IT930x_Initialize (Device_Context *DC, Byte br_idx) {

	Dword error = BR_ERR_NO_ERROR;
	Dword fileVersion = 0xFFFFFFFF, cmdVersion = 0xFFFFFFFF, tmpVersion = 0;
	Byte usb_dma_reg = 0, chip_index = 0, var1 = 0, var2 = 0, var3 = 0;

	error = IT9300_readRegisters(&DC->it9300, 0, 0xF53A, 1, &var1);
	error = IT9300_readRegisters(&DC->it9300, 0, 0xDA98, 1, &var2);
	error = IT9300_readRegisters(&DC->it9300, 0, 0xDA99, 1, &var3);

	deb_data("0xF53A = %d,  0xDA98 = 0x%X,  0xDA99 = 0x%X\n", var1, var2, var3);

	error = IT9300_getFirmwareVersion (&DC->it9300, br_idx);
	if (error) {
		deb_data("IT9300_getFirmwareVersion fail! \n");
		goto exit;
	}

	if (DC->it9300.firmwareVersion != 0) {
		DC->it9300.gator[br_idx].booted = True;
	} else
		DC->it9300.gator[br_idx].booted = False;

	if (DC->it9300.gator[br_idx].booted) {
		/* getFirmwareVersion() */
		error = DRV_getFirmwareVersionFromFile(DC, &tmpVersion);
		if (error) {
			deb_data("\t getFirmwareVersionFromFile fail [0x%08lx]\n", error);
			goto exit;
		}

		fileVersion = (Dword)(tmpVersion&0xFFFFFFFF);

		/* Use "Command_QUERYINFO" to get firmware version */
		error = IT9300_getFirmwareVersion(&DC->it9300, br_idx);
		if (error) {
			deb_data("\t getFirmwareVersion fail [0x%08lx]\n", error);
			goto exit;
		}
		cmdVersion = DC->it9300.firmwareVersion;

		deb_data("\t cmdVersion = 0x%lX\n", cmdVersion);
		deb_data("\t fileVersion = 0x%lX\n", fileVersion);

		if (cmdVersion != fileVersion) {
			deb_data("\t Outside_Fw = 0x%lX, Inside_Fw = 0x%lX, Reboot\n", fileVersion, cmdVersion);

			error = IT9300_reboot(&DC->it9300, br_idx);
			DC->bBootCode = true;
			if (error) {
				deb_data("\t Reboot fail [0x%08lx]\n", error);
				goto exit;
			} else
				return Error_NOT_READY;
		} else
				deb_data("\t FW version is the same\n");
	}

	error = IT9300_initialize (&DC->it9300, br_idx);
	if (error) {
		deb_data("IT9300_initialize fail \n");
		goto exit;
	}

	if ((DC->chip_Type[0][0] == EEPROM_EW100) || (DC->chip_Type[0][0] == EEPROM_CXD2880) || (DC->chip_Type[0][0] == EEPROM_CXD2880_33)) {
		error = IT9300_writeRegister(&DC->it9300, 0, p_br_reg_ts_fail_ignore, 0x1F);
		if (error)
			goto exit;
		error = IT9300_writeRegister(&DC->it9300, 0, p_br_reg_ts0_clk_sel, 0x01);
		if (error)
			goto exit;
	}

	deb_data("IT9300_initialize ok \n");
#if BCAS_ENABLE
	error = IT9300_bcasInit(&DC->it9300, br_idx);
    if (error) {
		deb_data("IT9300_bcasInit fail \n");
		goto exit;
	}
	deb_data("IT9300_bcasInit OK \n");
#endif
	exit:
	return error;
}

Dword DRV_IT930x_device_init(
		Device_Context *DC)
{
	int	br_idx = 0, ts_idx = 0, i = 0;
	Dword	error = Error_NO_ERROR;

	deb_data("- Enter %s Function -\n",  __func__);

	///==============it930x Initial==============
	if (DC->it9300_Number == 0)
		DC->it9300_Number = 1;


	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
		error =  DRV_IT930x_Initialize(DC, (Byte)br_idx);
		if (error) {
			deb_data("930x Initialize Error = 0x%08lx\n", error);
			return error;
		}
	}

	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
		for (ts_idx = 0; ts_idx < DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
			switch (DC->chip_Type[br_idx][ts_idx]) {
				case EEPROM_IT913X: //IT913x
					error = DRV_IT913x_Initialize(DC, br_idx, ts_idx);
					break;
				case EEPROM_Si2168B: //Si2168B
					error = DRV_Si2168B_Initialize(DC, br_idx, ts_idx);
					break;
				case EEPROM_Dibcom9090: //DIB9090
					error = DRV_Dib9090_Initialize(DC, EEPROM_Dibcom9090);
					break;
				case EEPROM_Dibcom9090_4chan: //DIB9090_4ch
					error = DRV_Dib9090_Initialize(DC, EEPROM_Dibcom9090_4chan);
					break;
				case EEPROM_MXL691: //MXL69x ATSC, Sean
					error = DRV_MXL69X_Initialize(DC, br_idx, ts_idx, EEPROM_MXL691);
					break;
				case EEPROM_MXL691_DUALP:
					error = DRV_MXL69X_Initialize(DC, br_idx, ts_idx, EEPROM_MXL691_DUALP);
					break;
				case EEPROM_MXL692:
					error = DRV_MXL69X_Initialize(DC, br_idx, ts_idx, EEPROM_MXL692);
					break;
				case EEPROM_MXL248:
					error = DRV_MXL69X_Initialize(DC, br_idx, ts_idx, EEPROM_MXL248);
					break;
				case EEPROM_CXD285X: //Sony CXD285X
					error = DRV_CXD285X_Initialize(DC, br_idx, ts_idx, EEPROM_CXD285X);
					break;
				case EEPROM_CXD6801_NUVYYO: //Sony CXD6801
					error = DRV_CXD6801_initialize(DC, br_idx, ts_idx, EEPROM_CXD6801_NUVYYO);
					break;
				case EEPROM_CXD6801_EVB: //Sony CXD6801
					error = DRV_CXD6801_initialize(DC, br_idx, ts_idx, EEPROM_CXD6801_EVB);
					break;
				case EEPROM_CXD6801_TS7: //Sony CXD6801
					error = DRV_CXD6801_initialize(DC, br_idx, ts_idx, EEPROM_CXD6801_TS7);
					break;
				case EEPROM_EW100: //Sony CXD2880
					error = DRV_CXD2880_initialize(DC, br_idx, ts_idx, EEPROM_EW100);
					break;
				case EEPROM_CXD2880: //Sony CXD2880
					error = DRV_CXD2880_initialize(DC, br_idx, ts_idx, EEPROM_CXD2880);
					break;
				case EEPROM_CXD2880_33: //Sony CXD2880
					error = DRV_CXD2880_initialize(DC, br_idx, ts_idx, EEPROM_CXD2880_33);
					break;
				case EEPROM_MXL541C:
					error = DRV_MXL541C_Initialize(DC, br_idx, ts_idx, EEPROM_MXL541C);
					break;
				default:
					deb_data("Unknown chip type [%d]\n", DC->chip_Type[br_idx][ts_idx]);
					break;
			}

			if (error) {
				deb_data("Tuner Initialize Fail [0x%08lx]\n", error);
				DC->is_rx_init[br_idx][ts_idx] = 0;
				if (DC->chip_Type[br_idx][ts_idx] != EEPROM_IT913X)
					return error;
			} else
				DC->is_rx_init[br_idx][ts_idx] = 1;
		}
	}

	///====== Set In SyncByte & TsType ======

	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
		for (ts_idx = 0; ts_idx < DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
			///Set SyncByte (0x17, 0x27, 0x37, 0x47, 0x57, 0x67, 0x77, 0x87)
			//DC->it9300.tsSource[br_idx][ts_idx].syncByte = 0x17 + ((br_idx * 4) + ts_idx) * 16;
			///Set SyncByte (0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47)
			DC->it9300.tsSource[br_idx][ts_idx].syncByte = 0x40 + ((br_idx * 4) + ts_idx);

//#ifdef PROBE_DEBUG_MESSAGE
			deb_data("syncByte = 0x%x \n", DC->it9300.tsSource[br_idx][ts_idx].syncByte);
//#endif
			error = IT9300_setInTsType((Endeavour*) &DC->it9300, br_idx, ts_idx);
			if (error) {
				deb_data("%d, %d Ts Source Set in TsType error!\n", br_idx, ts_idx);
				return (error);
			} else {
//#ifdef PROBE_DEBUG_MESSAGE
				deb_data("%d, %d Ts Source Set in TsType OK!\n", br_idx, ts_idx);
//#endif
			}

			error = IT9300_setSyncByteMode(&DC->it9300, br_idx, ts_idx);
			if (error)  {
				deb_data("%d it9300, %d Ts Source set syncByte error!\n", br_idx, ts_idx);
				return (error);
			} else {
#ifdef PROBE_DEBUG_MESSAGE
				deb_data("%d it9300, %d Ts Source set syncByte OK!\n", br_idx, ts_idx);
#endif
			}
		}
	}
	return (error);
}

Dword DL_IT930x_device_init(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_IT930x_device_init(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword Device_init(
		Device_Context *DC,
		Bool bBoot)
{
	Dword result = Error_NO_ERROR;
	Byte chip_index = 0;
	int TS_PACKET_COUNT_HI = 0;

	deb_data("- Enter %s Function -\n",  __func__);

#ifdef QuantaMID
	deb_data("\t === AfaDTV on Quanta  ===\n");
#endif

#ifdef EEEPC
	deb_data("\t === AfaDTV on EEEPC ===\n");
#endif

#ifdef DRIVER_RELEASE_VERSION
	deb_data("\t DRIVER_RELEASE_VERSION\t\t: %s\n", DRIVER_RELEASE_VERSION);
#else
	deb_data("\t DRIVER_RELEASE_VERSION\t\t: v0.0.0\n");
#endif

#ifdef FW_RELEASE_VERSION
	deb_data("\t FW_RELEASE_VERSION\t\t: %s\n", FW_RELEASE_VERSION);
#else
	deb_data("\t FW_RELEASE_VERSION\t\t: v0_0_0_0\n");
#endif

#ifdef OMEGA2_FW_RELEASE_VERSION
	deb_data("\t OMEGA2_FW_RELEASE_VERSION\t: %s\n", OMEGA2_FW_RELEASE_VERSION);
#else
	deb_data("\t OMEGA2_FW_RELEASE_VERSION\t: v0_0_0_0\n");
#endif

#ifdef OMEGA2I_FW_RELEASE_VERSION
	deb_data("\t OMEGA2I_FW_RELEASE_VERSION\t: %s\n", OMEGA2I_FW_RELEASE_VERSION);
#else
	deb_data("\t OMEGA2I_FW_RELEASE_VERSION\t: v0_0_0_0\n");
#endif

#ifdef Version_NUMBER
	deb_data("\t API_RELEASE_VERSION\t\t: %X.%X.%X\n", Version_NUMBER, Version_DATE, Version_BUILD);
#else
	deb_data("\t API_RELEASE_VERSION\t\t:000.00000000.0\n");
#endif

	/* Set Device init Info */
	DC->bEnterSuspend = false;
	DC->bSurpriseRemoval = false;
	DC->bDevNotResp = false;
	DC->bSelectiveSuspend = false;
	DC->bTunerPowerOff = false;

	if (bBoot) {
		DC->bSupportSelSuspend = false;
		DC->it9300.userData = (Handle)DC;
		//DC->endeavour.chipNumber = 1;
		DC->it9300.receiver_chip_number[0] = 1;
		DC->architecture = Architecture_DCA;
		DC->bIrTblDownload = false;
		DC->fc[0].tunerinfo.TunerId = 0;
		DC->fc[1].tunerinfo.TunerId = 0;
		DC->fc[0].bTimerOn = false;
		DC->fc[1].bTimerOn = false;
		DC->bDualTs = false;
		DC->StreamType = StreamType_DVBT_DATAGRAM;
		DC->UsbCtrlTimeOut = 1;
	} else
		DC->UsbCtrlTimeOut = 5;

	result = DL_DummyCmd(DC);
	if (result) {
		deb_data("\t Error: DummyCmd fail [0x%08lx]\n", result);
		goto exit;
	} else
			deb_data("\t DummyCmd success\n");

	if (bBoot) {
		/* Set USB Info */
		DC->MaxPacketSize = 0x0200; //default USB2.0
		DC->UsbMode = (DC->MaxPacketSize == 0x200) ? 0x0200 : 0x0110;
		deb_data("\t USB mode = 0x%x\n", DC->UsbMode);

		TS_PACKET_COUNT_HI = DC->it9300.it9300user_usb20_frame_size_dw * 4 / 188;
		DC->TsPacketCount = (DC->UsbMode == 0x200) ? TS_PACKET_COUNT_HI : TS_PACKET_COUNT_FU;
		DC->TsFrames = (DC->UsbMode == 0x200) ? TS_FRAMES_HI : TS_FRAMES_FU;
		DC->TsFrameSize = TS_PACKET_SIZE * DC->TsPacketCount;
		DC->TsFrameSizeDw = DC->TsFrameSize / 4;
	}

	DC->bEP12Error = false;
	DC->bEP45Error = false;
	DC->ForceWrite = false;
	DC->ulActiveFilter = 0;

	if (bBoot) {
		/* Patch for eeepc */
		result = DL_SetBusTuner(DC, BUS_USB, 0x38);
		if (result) {
			deb_data("\t Error: SetBusTuner fail [0x%08lx]\n", result);
			goto exit;
		}

		result = DL_GetEEPROMConfig(DC);// read eeprom from here, mark by sean
		if (result) {
			deb_data("\t Error: GetEEPROMConfig fail [0x%08lx]\n", result);
			goto exit;
		}
	}

	deb_data("\t Set busId = 0x%x, tunerId = 0x%x\n", BUS_USB, DC->fc[0].tunerinfo.TunerId);
	result = DL_SetBusTuner(DC, BUS_USB, DC->fc[0].tunerinfo.TunerId);
	//result = Demodulator_readRegister((Demodulator*)&DC->Demodulator, 0, Processor_LINK, EEPROM_TUNERID, &read_register);
	if (result) {
		deb_data("\t Error: SetBusTuner fail [0x%08lx]\n", result);
		goto exit;
	}

	//if(DC->endeavour.chipNumber == 2 && !DC->endeavour.booted){
	if (DC->it9300.receiver_chip_number[0] == 2 && !DC->it9300.booted) {
		//plug/cold-boot/S4
		result = DL_NIMReset(DC);
	} else if (DC->it9300.receiver_chip_number[0] == 2 && DC->it9300.booted) {
		//else if(DC->endeavour.chipNumber == 2 && DC->endeavour.booted){
		//warm-boot/(S1)
		result = DL_NIMSuspend(DC, false);
		result = DL_TunerWakeup(DC); //actually for mt2266
	} else if (DC->it9300.receiver_chip_number[0] == 1 && DC->it9300.booted) {
		//else if(DC->endeavour.chipNumber == 1 && DC->endeavour.booted){
		//warm-boot/(S1)
		result = DL_TunerWakeup(DC);
	}

	if (result) {
		deb_data("\t Error: NIMReset or NIMSuspend or TunerWakeup fail [0x%08lx]\n", result);
		goto exit;
	}

	result = DL_Initialize(DC);
	if (result) {
		deb_data("\t Error: Initialize fail [0x%08lx]\n", result);
		goto exit;
	}

	if  (DC->bIrTblDownload){
		result = DL_IrTblDownload(DC);
		if (result) {
			deb_data("\t Error: IrTblDownload fail [0x%08lx]\n", result);
			goto exit;
		}
	}

	//if(DC->endeavour.chipNumber == 2){
	if (DC->it9300.receiver_chip_number[0] == 2) {
		result = DL_USBSetup(DC);
		if (result) {
			deb_data("\t Error: USBSetup fail [0x%08lx]\n", result);
			goto exit;
		}

		result = DL_InitNIMSuspendRegs(DC);
		if (result) {
			deb_data("\t Error: InitNIMSuspendRegs fail [0x%08lx]\n", result);
			goto exit;
		}
	}

	//for(chip_index = DC->endeavour.chipNumber; chip_index > 0; chip_index--){
	for (chip_index = DC->it9300.receiver_chip_number[0]; chip_index > 0; chip_index--) {
		result = DL_ApCtrl(DC, (chip_index - 1), false);
		if (result) {
			deb_data("\t Error: ApCtrl[%d] fail [0x%08lx]\n", chip_index, result);
			goto exit;
		}
		DC->fc[chip_index - 1].bTimerOn = false;
		DC->fc[chip_index - 1].tunerinfo.bTunerInited = false;
	}
	/* AirHD need to init some regs */
	/*
	result = DRV_WriteRegister(dev, 0, Processor_OFDM, 0xF714, 0x0);
	result = DRV_WriteRegister(dev, 0, Processor_LINK, 0xDD88, 0x40);
	result = DRV_WriteRegister(dev, 0, Processor_LINK, 0xDD89, 0x0);
	result = DRV_WriteRegister(dev, 0, Processor_LINK, 0xDD0E, 0x80);
	result = DRV_WriteRegister(dev, 0, Processor_LINK, 0xDD0C, 0x80);

	result = DRV_WriteRegisterBits(dev, 0, Processor_LINK, 0xDD11, 0x7, 0x1, 0x0);
	result = DRV_WriteRegisterBits(dev, 0, Processor_LINK, 0xDD11, 0x5, 0x1, 0x0);
	result = DRV_WriteRegisterBits(dev, 0, Processor_LINK, 0xDD13, 0x7, 0x1, 0x0);
	result = DRV_WriteRegisterBits(dev, 0, Processor_LINK, 0xDD13, 0x5, 0x1, 0x0);
	result = DRV_WriteRegisterBits(dev, 0, Processor_LINK, 0xDD11, 0x7, 0x1, 0x1);
	result = DRV_WriteRegisterBits(dev, 0, Processor_LINK, 0xDD11, 0x5, 0x1, 0x1);

	result = DRV_WriteRegister(dev, 0, Processor_LINK, 0xDDA9, 0x04);
	*/
	deb_data("- %s success -\n",  __func__);
	return result;

	exit:
	deb_data("- %s fail -\n",  __func__);
	return result;
}

Dword DRV_device_communication_test (Device_Context *DC) {
	Dword error = Error_NO_ERROR;
	Dword version = 0;

	error = IT9300_getFirmwareVersion (&DC->it9300, 0);

	if (error)
			deb_data("Device test Error\n");
	else
			deb_data("Device test ok\n");

	return error;
}

Dword DL_device_communication_test (Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_device_communication_test(DC);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DL_ResetRxPort(Device_Context *DC, int is_open)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
	result = DRV_ResetRxPort(DC, is_open);
	mutex_unlock(&DC->dev_mutex);

	return result;
}

Dword DRV_IT930x_device_deinit(
		Device_Context *DC)
{
	int	br_idx = 0, ts_idx = 0, i = 0;
	Dword	error = Error_NO_ERROR;

	deb_data("- Enter %s Function -\n",  __func__);

	///==============it930x DeInitial==============

	for (br_idx = 0; br_idx < DC->it9300_Number; br_idx++) {
		for (ts_idx = 0; ts_idx < DC->it9300.receiver_chip_number[br_idx]; ts_idx++) {
			if (!DC->is_rx_init[br_idx][ts_idx]) { //init fail
				deb_data("\tSkip uninitializaion of the fail receiver \n");
				continue;
			} else
					deb_data("\tuninitializaion br_idx = %u ts_idx = %u\n", br_idx, ts_idx);

			switch (DC->chip_Type[br_idx][ts_idx]) {
				case EEPROM_IT913X: //IT913x
					//error = DRV_IT913x_DeInitialize(DC, (Byte)br_idx, ts_idx);
					break;
				case EEPROM_Si2168B: //Si2168B
					//error = DRV_Si2168B_DeInitialize(DC, (Byte)br_idx, ts_idx);
					break;
				case EEPROM_Dibcom9090: //DIB9090
					error = DRV_Dib9090_DeInitialize(DC, EEPROM_Dibcom9090);
					break;
				case EEPROM_Dibcom9090_4chan: //DIB9090_4ch
					error = DRV_Dib9090_DeInitialize(DC, EEPROM_Dibcom9090_4chan);
					break;
				case EEPROM_MXL691: //MXL69X ATSC
				case EEPROM_MXL691_DUALP:
				case EEPROM_MXL692:
				case EEPROM_MXL248:
					error = DRV_MXL69X_DeInitialize(DC, br_idx, ts_idx);
					break;
				case EEPROM_CXD6801_NUVYYO: //Sony CXD6801
				case EEPROM_CXD6801_EVB:
				case EEPROM_CXD6801_TS7:
					break;
				case EEPROM_MXL541C:
					error = DRV_MXL541C_DeInitialize(DC, br_idx, ts_idx);
					break;
				case EEPROM_EW100: //Sony CXD2880
				case EEPROM_CXD2880:
				case EEPROM_CXD2880_33:
					break;
				default:
					deb_data("Unknown chip type [%d]\n", DC->chip_Type[br_idx][ts_idx]);
					break;
			}

			if (error)
					deb_data("Tuner DeInitialize Fail [0x%08lx]\n", error);
		}
	}

	return (error);
}

Dword DL_IT930x_device_deinit(Device_Context *DC)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);

	result = DRV_IT930x_device_deinit(DC);

	mutex_unlock(&DC->dev_mutex);

	return result;
}
