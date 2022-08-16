#include "it930x-core.h"
#include "Si2168B_L2_API.h"

//Si2168B_L2_Context g_Si2168B_FE[4][5];

Dword DRV_Si2168B_Initialize (Device_Context *DC, int br_idx, int tsSrcIdx) {
	Dword error = Error_NO_ERROR;
	Byte demod_i2cadd = 0xCE;
	Byte tuner_i2cadd = 0xC6;
	Byte dvb_t2_brIdx, dvb_t2_i2cbus;
	int lock = 0;
	int	standard;
	int dvb_t_stream;
	Byte dvb_t2_plp_id = 0, t2_lock_mode = 0;
	Si2168B_L2_Context* Si2168B_FE;
	
	DC->Si2168B_FE[tsSrcIdx] = kzalloc(sizeof(Si2168B_L2_Context), GFP_KERNEL);	
	
	Si2168B_FE = (Si2168B_L2_Context *)DC->Si2168B_FE[tsSrcIdx];

	standard = Si2168B_DD_MODE_PROP_MODULATION_DVBT2;
	dvb_t_stream = Si2168B_DVBT_HIERARCHY_PROP_STREAM_HP;
	t2_lock_mode = 0;

	dvb_t2_brIdx = (Byte)br_idx;
	dvb_t2_i2cbus = DC->it9300.tsSource[br_idx][tsSrcIdx].i2cBus;

	error = Si2168B_L2_SW_Init (Si2168B_FE, demod_i2cadd, tuner_i2cadd, &DC->it9300, dvb_t2_brIdx, dvb_t2_i2cbus, NULL);
    if(!error) {
			printk("Si2168B_L2_SW_Init fail!  \n");
			return -1;
		}

    DC->it9300.tsSource[br_idx][tsSrcIdx].initialized=True;

	error = Si2168B_L2_switch_to_standard (Si2168B_FE, standard, 0);
	if(!error) {
		printk("DVB_T switch fail...!!!\n");
		return -1;
	}

	return 0;
}

Dword DRV_Si2168B_acquireChannel (Device_Context *DC, Word bandwidth, Dword frequency, int br_idx, int ts_idx)
{
	Dword error = Error_NO_ERROR;
	Si2168B_L2_Context* Si2168B_FE;
	int lock = 0;

	Si2168B_FE = (Si2168B_L2_Context *)DC->Si2168B_FE[ts_idx];
	
	lock = Si2168B_L2_lock_to_carrier(Si2168B_FE, Si2168B_DD_MODE_PROP_MODULATION_DVBT2, frequency * 1000, bandwidth * 1000, 0, 0, 0, 0, 0);
	if(lock) {
		printk("Si2168B_drv - Si2168B is lock\n");

		 //Get Lock & standard
		error = Si2168B_L1_DD_STATUS(Si2168B_FE->demod, Si2168B_DD_STATUS_CMD_INTACK_OK);
		if(error != NO_Si2168B_ERROR) printk("Si2168B_drv - Si2168B_L1_DD_STATUS fail...!!\n");

		if(Si2168B_FE->demod->rsp->dd_status.modulation == 2) {
			 printk("Si2168B_drv - modulation : DVB_T\n");
		} else if(Si2168B_FE->demod->rsp->dd_status.modulation == 7) {
			printk("Si2168B_drv - modulation : DVB_T2\n");
		}

	} else {
		printk("Si2168B_drv - Si2168B is unlock\n");
	}

	return !lock;
}

Dword DRV_Si2168B_getStatistic (Device_Context *DC, GetStatisticRequest *getstatisticrequest, int br_idx, int ts_idx)
{
	Dword error = Error_NO_ERROR;
	Si2168B_L2_Context* Si2168B_FE;
	int standard;
	
	Si2168B_FE = (Si2168B_L2_Context *)DC->Si2168B_FE[ts_idx];
	
	error = Si2168B_L1_DD_STATUS(Si2168B_FE->demod, Si2168B_DD_STATUS_CMD_INTACK_OK);
	if(error != NO_Si2168B_ERROR)  printk("[Si2168B] - Get standard fail...!!!\n");
				
	standard = Si2168B_FE->demod->rsp->dd_status.modulation;	
	
	error = Si2168B_L1_DD_STATUS(Si2168B_FE->demod, Si2168B_DD_STATUS_CMD_INTACK_OK);	
	if(error != NO_Si2168B_ERROR) printk("Si2168B_drv - Si2168B_L1_DD_STATUS fail...!!\n");
	
	//=========================================================
	error = Si2168B_L2_Tuner_I2C_Enable(Si2168B_FE);
	if(error != NO_Si2168B_ERROR) printk("Si2168B_drv - Si2168B_L2_Tuner_I2C_Enable fail...!!\n");
	
	if(SiLabs_TER_Tuner_DTV_Text_STATUS(Si2168B_FE->tuner_ter) != NO_Si2147_ERROR)
	{
		printk("Error reading TUNER_STATUS\n");
		error = Si2168B_L2_Tuner_I2C_Disable(Si2168B_FE);
		return error;
	}	
	
	error = Si2147_L1_TUNER_STATUS(Si2168B_FE->tuner_ter->Si2147_Tuner[0], Si2147_TUNER_STATUS_CMD_INTACK_OK);
	if(error != NO_Si2147_ERROR) {
		printk("Si2168B_drv - Si2147_L1_TUNER_STATUS fail...!!\n");
		return error;
	}
	
	error = Si2168B_L2_Tuner_I2C_Disable(Si2168B_FE);
	if(error != NO_Si2168B_ERROR) {
		printk("Si2168B_drv - Si2168B_L2_Tuner_I2C_Disable fail...!!\n");
		return error;//goto exit;
	}
	
	Si2168B_FE->demod->cmd->dd_ext_agc_ter.agc_1_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_NO_CHANGE;
	Si2168B_FE->demod->cmd->dd_ext_agc_ter.agc_2_mode  = Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_NO_CHANGE;
	
	error = Si2168B_L1_SendCommand2(Si2168B_FE->demod, Si2168B_DD_EXT_AGC_TER_CMD_CODE);
	if(error != NO_Si2147_ERROR) {
		printk("Si2168B_drv - Si2168B_L1_SendCommand2 fail...!!\n");
		return error;
	}
	
	if(standard == 2) {
		error = Si2168B_L1_DVBT_STATUS(Si2168B_FE->demod, Si2168B_DVBT_STATUS_CMD_INTACK_OK);
		if(error != NO_Si2168B_ERROR) {
			printk("Si2168B_drv - Si2168B_L1_DVBT_STATUS fail...!!\n");
			return error;
		}
		
		error = Si2168B_L1_DVBT_TPS_EXTRA(Si2168B_FE->demod);
		if(error != NO_Si2168B_ERROR) {
			printk("Si2168B_drv - Si2168B_L1_DVBT_TPS_EXTRA fail...!!\n");
			return error;
		}
		
		error = Si2168B_L1_DD_SSI_SQI(Si2168B_FE->demod, Si2168B_FE->tuner_ter->Si2147_Tuner[0]->rsp->tuner_status.rssi);
		if(error != NO_Si2168B_ERROR) {
			printk("Si2168B_drv - Si2168B_L1_DD_SSI_SQI fail...!!\n");
			return error;
		}
	}
	
	if(standard == 7) {		
		error = Si2168B_L1_DVBT2_STATUS(Si2168B_FE->demod, Si2168B_DVBT_STATUS_CMD_INTACK_OK);
		if(error != NO_Si2168B_ERROR) {
			printk("Si2168B_drv - Si2168B_L1_DVBT2_STATUS fail...!!\n");
			return error;
		}
		
		error = Si2168B_L1_DD_SSI_SQI(Si2168B_FE->demod, Si2168B_FE->tuner_ter->rssi);
		if(error != NO_Si2168B_ERROR) {
			printk("Si2168B_drv - Si2168B_L1_DD_SSI_SQI fail...!!\n");
			return error;
		}
	}
	
/**	
	error = Si2168B_L2_Tuner_I2C_Enable(Si2168B_FE);	//
	if(error != NO_Si2168B_ERROR) return error;//goto exit;	
	error = Si2147_L1_TUNER_STATUS(Si2168B_FE->tuner_ter->Si2147_Tuner[0], Si2147_TUNER_STATUS_CMD_INTACK_OK);
	if(error != NO_Si2147_ERROR) return error;//goto exit;	
	error = Si2168B_L2_Tuner_I2C_Disable(Si2168B_FE);
	if(error != NO_Si2168B_ERROR) //goto exit;
	
	error = Si2168B_L1_DD_SSI_SQI(Si2168B_FE->demod, Si2168B_FE->tuner_ter->Si2147_Tuner[0]->rsp->tuner_status.rssi);
	if(error != NO_Si2168B_ERROR) return error;//goto exit;
	//printf("RSSI : %d\n", Si2168B_FE->tuner_ter->Si2147_Tuner[0]->rsp->tuner_status.rssi);
	//printf("SSI : %d\n", Si2168B_FE->demod->rsp->dd_ssi_sqi.ssi);
	//printf("SQI : %d\n", Si2168B_FE->demod->rsp->dd_ssi_sqi.sqi);   //-1 : unlock
*/

	getstatisticrequest->statistic.signalLocked = Si2168B_FE->demod->rsp->dd_status.dl;	//demod_lock
	getstatisticrequest->statistic.signalPresented = Si2168B_FE->demod->rsp->dd_status.dl; //fec_lock
	
	getstatisticrequest->statistic.signalQuality = (Dword)Si2168B_FE->demod->rsp->dd_ssi_sqi.sqi;
	if(Si2168B_FE->demod->rsp->dd_ssi_sqi.sqi < 0) //(if no signal, quality is -1)
		getstatisticrequest->statistic.signalQuality = 0;

	//printk("ssi: %d\n", Si2168B_FE->demod->rsp->dd_ssi_sqi.ssi);
	//printk("rssi: %d\n", Si2168B_FE->tuner_ter->Si2147_Tuner[0]->rsp->tuner_status.rssi);
	
	if(Si2168B_FE->demod->rsp->dd_status.dl)
		getstatisticrequest->statistic.signalStrength = (Dword)Si2168B_FE->demod->rsp->dd_ssi_sqi.ssi;
	else 
		getstatisticrequest->statistic.signalStrength = 0;
		
	getstatisticrequest->snr = 0;
	getstatisticrequest->rf_power = 0;

	return error;
}

Dword DRV_Si2168B_getChannelStatistic (Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int ts_idx)
{
	Dword error = Error_NO_ERROR;
	Si2168B_L2_Context* Si2168B_FE;
	
	Si2168B_FE = (Si2168B_L2_Context *)DC->Si2168B_FE[ts_idx];
	
	error = Si2168B_L1_DD_STATUS(Si2168B_FE->demod, Si2168B_DD_STATUS_CMD_INTACK_OK);
	if(error != NO_Si2168B_ERROR) {
		printk("Si2168B_drv - Si2168B_L1_DD_STATUS fail...!!\n");
		return error;
	}
	
	channelStatistic->postVitBitCount = (Dword)Si2168B_FE->demod->rsp->dd_status.ber;
	channelStatistic->postVitErrorCount = (channelStatistic->postVitBitCount * channelStatistic->postVitBitCount);
	
	error = Si2168B_L1_DD_UNCOR (Si2168B_FE->demod, Si2168B_DD_UNCOR_CMD_RST_CLEAR);
	if(error != NO_Si2168B_ERROR) {
		printk("Si2168B_drv - Si2168B_L1_DD_UNCOR fail...!!\n");
		return error;
	}
	
	channelStatistic->abortCount = (Dword)(Si2168B_FE->demod->rsp->dd_uncor.uncor_msb<<16) + Si2168B_FE->demod->rsp->dd_uncor.uncor_lsb;

/***	
	//API: error = Si2168B_L1_DD_STATUS(Si2168B_FE->demod, Si2168B_DD_STATUS_CMD_INTACK_OK);
	//GET:
	//Ber = Si2168_FE->demod->rsp->dd_status.ber;
	//API: error = Si2168B_L1_DD_UNCOR (Si2168B_FE->demod, Si2168B_DD_UNCOR_CMD_RST_CLEAR);
	//GET:
	//abortCount = (Si2168_FE->demod->rsp->dd_uncor.uncor_msb<<16) + Si2168_FE->demod->rsp->dd_uncor.uncor_lsb;
**/

	return error;
}

Dword DRV_Si2168B_isLocked(Device_Context *DC, Bool* locked, int br_idx, int ts_idx)
{
	Dword error = Error_NO_ERROR;
	Si2168B_L2_Context* Si2168B_FE;
	
	Si2168B_FE = (Si2168B_L2_Context *)DC->Si2168B_FE[ts_idx];
	
	error = Si2168B_L1_DD_STATUS(Si2168B_FE->demod, Si2168B_DD_STATUS_CMD_INTACK_OK);	
	if(error != NO_Si2168B_ERROR) printk("Si2168B_drv - Get status fail...!!\n");
	
	if(Si2168B_FE->demod->rsp->dd_status.dl)
	{
		*locked = True;
		//printk("Si2168B %d, %d is lock \n", br_idx, ts_idx);
	}
	else
	{
		*locked = False;
	}

	return error;
}


