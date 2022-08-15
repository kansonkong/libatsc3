#include "it930x-core.h"

#include "dibhost.h"
#include "dibfrontend_tune.h"
#include "dibsip.h"
#include "dibpmu.h"
#include "dibx09x.h"
#include "dib7000m.h"
#include "dibosiris.h"
#include "dibcomponent_monitor.h"
#include "dibmonitor.h"

static const struct dib9090_config nim9090md_config[2] = 
{
    {
        NULL, // update_lna
        0,     // dib0090_freq_offset_khz_uhf
        0,     // dib0090_freq_offset_khz_vhf
        30000, // clock_khz
        1,     // clockouttobamse
        (0 << 15) | (1 << 13) | (0 << 12) | (3 << 10) | (0 << 9) | (1 << 7) | (0 << 6) | (0 << 4) | (1 << 3) | (1 << 1) | (0), // drives
        1,    // output_mpeg2_in_188_bytes
    }, 
	{
        NULL, // update_lna
        0,     // dib0090_freq_offset_khz_uhf
        0,     // dib0090_freq_offset_khz_vhf
        30000, // clock_khz
        0,     // clockouttobamse
        (0 << 15) | (1 << 13) | (0 << 12) | (3 << 10) | (0 << 9) | (1 << 7) | (0 << 6) | (0 << 4) | (1 << 3) | (1 << 1) | (0), // drives
        1,    // output_mpeg2_in_188_bytes
        {
            { BOARD_GPIO_COMPONENT_DEMOD, BOARD_GPIO_FUNCTION_COMPONENT_ON,  0x0010, 0x0000, 0x0010 }, // LED is on GPIO 4
            { BOARD_GPIO_COMPONENT_DEMOD, BOARD_GPIO_FUNCTION_COMPONENT_OFF, 0x0010, 0x0000, 0x0000 },
        },

        { /* subband */
            2,
            {
                { 240, { BOARD_GPIO_COMPONENT_DEMOD, BOARD_GPIO_FUNCTION_SUBBAND_GPIO, 0x0006, 0x0000, 0x0006 } }, /* GPIO 1 and 2 to 1 for VHF */
                { 890, { BOARD_GPIO_COMPONENT_DEMOD, BOARD_GPIO_FUNCTION_SUBBAND_GPIO, 0x0006, 0x0000, 0x0000 } }, /* GPIO 1 and 2 to 0 for UHF */
                { 0 },
            },
        },
    },
};

static const struct dib9090_config nim9090md_config_4CH[4] = 
{
    {
        NULL, // update_lna
        0,     // dib0090_freq_offset_khz_uhf
        0,     // dib0090_freq_offset_khz_vhf
        30000, // clock_khz
       1,     // clockouttobamse
        (0 << 15) | (1 << 13) | (0 << 12) | (3 << 10) | (0 << 9) | (1 << 7) | (0 << 6) | (0 << 4) | (1 << 3) | (1 << 1) | (0), // drives
        1,    // output_mpeg2_in_188_bytes
    },
        {
        NULL, // update_lna
        0,     // dib0090_freq_offset_khz_uhf
        0,     // dib0090_freq_offset_khz_vhf
        30000, // clock_khz
        1,     // clockouttobamse
        (0 << 15) | (1 << 13) | (0 << 12) | (3 << 10) | (0 << 9) | (1 << 7) | (0 << 6) | (0 << 4) | (1 << 3) | (1 << 1) | (0), // drives
        1,    // output_mpeg2_in_188_bytes
    },
    {
        NULL, // update_lna
        0,     // dib0090_freq_offset_khz_uhf
        0,     // dib0090_freq_offset_khz_vhf
        30000, // clock_khz
        1,     // clockouttobamse
        (0 << 15) | (1 << 13) | (0 << 12) | (3 << 10) | (0 << 9) | (1 << 7) | (0 << 6) | (0 << 4) | (1 << 3) | (1 << 1) | (0), // drives
        1,    // output_mpeg2_in_188_bytes
    }, 
        {
        NULL, // update_lna
        0,     // dib0090_freq_offset_khz_uhf
        0,     // dib0090_freq_offset_khz_vhf
        30000, // clock_khz
        0,     // clockouttobamse
        (0 << 15) | (1 << 13) | (0 << 12) | (3 << 10) | (0 << 9) | (1 << 7) | (0 << 6) | (0 << 4) | (1 << 3) | (1 << 1) | (0), // drives
        1,    // output_mpeg2_in_188_bytes
        {
            { BOARD_GPIO_COMPONENT_DEMOD, BOARD_GPIO_FUNCTION_COMPONENT_ON,  0x0010, 0x0000, 0x0010 }, // LED is on GPIO 4
            { BOARD_GPIO_COMPONENT_DEMOD, BOARD_GPIO_FUNCTION_COMPONENT_OFF, 0x0010, 0x0000, 0x0000 },
        },
        { /* subband */
            2,
            {
                { 240, { BOARD_GPIO_COMPONENT_DEMOD, BOARD_GPIO_FUNCTION_SUBBAND_GPIO, 0x0006, 0x0000, 0x0006 } }, /* GPIO 1 and 2 to 1 for VHF */
                { 890, { BOARD_GPIO_COMPONENT_DEMOD, BOARD_GPIO_FUNCTION_SUBBAND_GPIO, 0x0006, 0x0000, 0x0000 } }, /* GPIO 1 and 2 to 0 for UHF */
                { 0 },
            },
        },
    },
};


static const struct osiris_config nim9090md_osiris_config = 
{
    1000,     /* DCDC1                             */
    2000,     /* DCDC2                             */
    1800,     /* LDO_SRAM                          */
    1800,     /* LDO_TUNER                         */
    1800,     /* LDO_ADC1                          */
    1020,     /* LDO_CORE_MAC                      */
    3000,     /* LDO_ADC2                          */
    (1 << 1), /* Enable LDO_ADC2, Disable LDO_ADC1 */
    0,        /* Disable DCDC2 current sense       */
    0,        /* Divide DCDC2 clock by 2           */
};

Dword DRV_Dib9090_Initialize(Device_Context *DC, Byte byChipType)
{
	struct dibDataBusHost *dibBus;
	struct dibPMU *pmu;
	int i = 0;
	Dword error = Error_NO_ERROR;
	
	DC->dib9090.i2c = host_i2c_interface_attach(&DC->it9300, 3, 0, NULL);
	if(DC->dib9090.i2c == NULL) {
		error = Error_WRITE_TUNER_FAIL;
		goto exit;
	}
	
	for(i = 0; i < 4; i++) {
		memset(&DC->dib9090.monitor[i], 0, sizeof(struct dibDemodMonitor));		
	}
	
	switch(byChipType)
	{
		case EEPROM_Dibcom9090:
			{
				deb_data("[DiB9090] EEPROM_Dibcom9090 init\n");
				frontend_init(&DC->dib9090.frontend[0]); // initializing the frontend-structure
				frontend_init(&DC->dib9090.frontend[1]); // initializing the frontend-structure
				frontend_set_id(&DC->dib9090.frontend[0], 0); // assign an absolute ID to the frontend
				frontend_set_id(&DC->dib9090.frontend[1], 1); // assign an absolute ID to the frontend

				if (dib9090_firmware_sip_register(&DC->dib9090.frontend[0], 0x80, DC->dib9090.i2c, &nim9090md_config[0]) == NULL) {
					deb_data("[DiB9090] frontend[0] attaching demod and tuners failed.\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}

				if (dib9090_firmware_sip_register(&DC->dib9090.frontend[1], 0x82, DC->dib9090.i2c, &nim9090md_config[1]) == NULL) { 
					deb_data("[DiB9090] frontend[1] attaching demod and tuners failed.\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}

				if(dib7000m_i2c_enumeration(DC->dib9090.i2c, 1, 0x22, 0x80) != DIB_RETURN_SUCCESS) {// non-standard i2c-enumeration, because of INT_SELECT-fixation 
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}
				if(dib7000m_i2c_enumeration(DC->dib9090.i2c, 1, DEFAULT_DIB9000_I2C_ADDRESS, 0x82) != DIB_RETURN_SUCCESS) {
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}

				dibBus = dib7000m_get_i2c_master(&DC->dib9090.frontend[0], DIBX000_I2C_INTERFACE_GPIO_1_2, 0);
				if(dibBus == NULL) return Error_WRITE_TUNER_FAIL;
				if ((pmu = osiris_create(dibBus, &nim9090md_osiris_config)) == NULL) {
					deb_data("[DiB9090] osiris_create fail\n");
					error = Error_READ_TUNER_FAIL;
					goto exit;
				}

				dib9090_set_pmu(&DC->dib9090.frontend[0], pmu); // workaround because we cannot access the PMU from the host after downloading the firmware - for now
				if (frontend_reset(&DC->dib9090.frontend[0]) != DIB_RETURN_SUCCESS)	{
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}

				if (frontend_reset(&DC->dib9090.frontend[1]) != DIB_RETURN_SUCCESS) {
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}
					
				
			}
			break;
		case EEPROM_Dibcom9090_4chan:
			{
				deb_data("[DiB9090] EEPROM_Dibcom9090_4chan init\n");
				frontend_init(&DC->dib9090.frontend[0]); // initializing the frontend-structure 
				frontend_init(&DC->dib9090.frontend[1]); // initializing the frontend-structure 
				frontend_init(&DC->dib9090.frontend[2]); // initializing the frontend-structure 
				frontend_init(&DC->dib9090.frontend[3]); // initializing the frontend-structure 
				frontend_set_id(&DC->dib9090.frontend[0], 0);  // assign an absolute ID to the frontend 
				frontend_set_id(&DC->dib9090.frontend[1], 1);  // assign an absolute ID to the frontend 
				frontend_set_id(&DC->dib9090.frontend[2], 2);  // assign an absolute ID to the frontend 
				frontend_set_id(&DC->dib9090.frontend[3], 3);  // assign an absolute ID to the frontend 

				if (dib9090_firmware_sip_register(&DC->dib9090.frontend[0], 0x80, DC->dib9090.i2c, &nim9090md_config_4CH[0]) == NULL) {
					printk("[DiB9090] - frontend[0] attaching demod and tuners failed.\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}

				DC->dib9090.i2c = dib7000m_get_i2c_master(&DC->dib9090.frontend[0], DIBX000_I2C_INTERFACE_GPIO_3_4, 0);
				if(DC->dib9090.i2c == NULL)
					return  Error_WRITE_TUNER_FAIL;

				if (dib9090_firmware_sip_register(&DC->dib9090.frontend[1], 0x82, DC->dib9090.i2c, &nim9090md_config_4CH[1]) == NULL) { 
					printk("[DiB9090] - frontend[1] attaching demod and tuners failed.\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}

				if (dib9090_firmware_sip_register(&DC->dib9090.frontend[2], 0x84, DC->dib9090.i2c, &nim9090md_config_4CH[2]) == NULL) { 
					printk("[DiB9090] - frontend[2] attaching demod and tuners failed.\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}

				if (dib9090_firmware_sip_register(&DC->dib9090.frontend[3], 0x86, DC->dib9090.i2c, &nim9090md_config_4CH[3]) == NULL) { 
					printk("[DiB9090] - frontend[3] attaching demod and tuners failed.\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}

				DC->dib9090.i2c = data_bus_client_get_data_bus(demod_get_data_bus_client(&DC->dib9090.frontend[0]));
				if(DC->dib9090.i2c == NULL) return Error_WRITE_TUNER_FAIL;
				if((dib7000m_i2c_enumeration(DC->dib9090.i2c, 1, 0x22, 0x80))!= DIB_RETURN_SUCCESS) // non-standard i2c-enumeration, because of INT_SELECT-fixation 
					return Error_WRITE_TUNER_FAIL;
				DC->dib9090.i2c = data_bus_client_get_data_bus(demod_get_data_bus_client(&DC->dib9090.frontend[1]));
				if(DC->dib9090.i2c == NULL) return Error_WRITE_TUNER_FAIL;
				if((dib7000m_i2c_enumeration(DC->dib9090.i2c, 1, 0x20, 0x82))!= DIB_RETURN_SUCCESS)
					return Error_WRITE_TUNER_FAIL;
				DC->dib9090.i2c = data_bus_client_get_data_bus(demod_get_data_bus_client(&DC->dib9090.frontend[2]));
				if(DC->dib9090.i2c == NULL) return Error_WRITE_TUNER_FAIL;
				
				if((dib7000m_i2c_enumeration(DC->dib9090.i2c, 1, 0x12, 0x84))!= DIB_RETURN_SUCCESS)
					return Error_WRITE_TUNER_FAIL;
				DC->dib9090.i2c = data_bus_client_get_data_bus(demod_get_data_bus_client(&DC->dib9090.frontend[3]));
				if(DC->dib9090.i2c == NULL) return Error_WRITE_TUNER_FAIL;
				if((dib7000m_i2c_enumeration(DC->dib9090.i2c, 1, 0x10, 0x86))!=DIB_RETURN_SUCCESS)
					return Error_WRITE_TUNER_FAIL;
				dibBus = dib7000m_get_i2c_master(&DC->dib9090.frontend[0], DIBX000_I2C_INTERFACE_GPIO_1_2, 0);
				if(dibBus == NULL) return Error_WRITE_TUNER_FAIL;
				if ((pmu = osiris_create(dibBus, &nim9090md_osiris_config)) == NULL) {
					printk("[DiB9090] - osiris_create fail\n");
					error = Error_READ_TUNER_FAIL;
					goto exit;
				}
				dib9090_set_pmu(&DC->dib9090.frontend[0], pmu); // workaround because we cannot access the PMU from the host after downloading the firmware - for now 

				if(frontend_reset(&DC->dib9090.frontend[0]) != DIB_RETURN_SUCCESS) {
					printk("[DiB9090] - frontend[0] frontend_reset fail\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}
				if(frontend_reset(&DC->dib9090.frontend[1]) != DIB_RETURN_SUCCESS) {
					printk("[DiB9090] - frontend[1] frontend_reset fail\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}
				if(frontend_reset(&DC->dib9090.frontend[2]) != DIB_RETURN_SUCCESS) {
					printk("[DiB9090] - frontend[2] frontend_reset fail\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}
		
				if(frontend_reset(&DC->dib9090.frontend[3]) != DIB_RETURN_SUCCESS) {
					printk("[DiB9090] - frontend[3] frontend_reset fail\n");
					error = Error_WRITE_TUNER_FAIL;
					goto exit;
				}				
			}
			break;
		default:
			break;
	}
	
	


	deb_data("[DiB9090] init success\n");
	return 0;

exit:
	deb_data("[DiB9090] init fail\n");
	return error;
}

Dword DRV_Dib9090_acquireChannel(Device_Context *DC, Word bandwidth, Dword frequency, Byte byChipType)
{
	struct dibChannel ch;
	//struct dibFrontend frontend[2];
	
	//memcpy(&frontend[0], DC->dib9090.frontend[0], sizeof(struct dibFrontend));
	//memcpy(&frontend[1], DC->dib9090.frontend[1], sizeof(struct dibFrontend));
	//frontend[0] = (struct dibFrontend *)DC->dib9090.frontend[0];
	//frontend[1] = (struct dibFrontend *)DC->dib9090.frontend[1];

	INIT_CHANNEL(&ch, STANDARD_DVBT);

	DC->dib9090.frequency = ch.RF_kHz = frequency;
	DC->dib9090.bandwidth = ch.bandwidth_kHz = bandwidth;

	//tune_diversity(frontend[0], 2, &ch);
	//tune_diversity(frontend[1], 2, &ch);
	switch(byChipType)
	{
		case EEPROM_Dibcom9090:
			{
				tune_diversity(DC->dib9090.frontend, 2, &ch);
			}
			break;
		case EEPROM_Dibcom9090_4chan:
			{
				tune_diversity(DC->dib9090.frontend, 4, &ch);
			}
			break;
		default:
			break;
		
	}

	return 0;
}

static Byte GetDib0090BER(Device_Context *DC, int iIndex)
{
	Dword invBER = 0;
    Byte BER_SQI = 0;
    int i = 0;
	
	if((iIndex < 0) || (iIndex > 4)) {
		BER_SQI = 0;
		goto exit;
	}
	
	//SQ (count from BER)
    if (DC->dib9090.monitor[iIndex].locks.fec_mpeg == 0) {
    	BER_SQI = 0;
    }
    else {
				
    	if(DC->dib9090.monitor[iIndex].ber_raw == 0)
    		invBER = 10000001;
    	else
    		invBER = 100000000 / DC->dib9090.monitor[iIndex].ber_raw;
    	if(invBER < 1000)
    		BER_SQI = 0;
    	else if(invBER >= 1000 && invBER < 10000000)
    	{
    		if(invBER < 1245) //3.0
    		    BER_SQI = 20 ;
    		else if (invBER >= 1245 && invBER < 1567) //3.1
    		    BER_SQI = 22 ;
    		else if (invBER >= 1567 && invBER < 1973) //3.2
    		    BER_SQI = 24 ;
    		else if (invBER >= 1973 && invBER < 2484) //3.3
    		    BER_SQI = 26 ;
    		else if (invBER >= 2484 && invBER < 3127) //3.4
    		    BER_SQI = 28 ;
    		else if (invBER >= 3127 && invBER < 3936) //3.5
    		    BER_SQI = 30 ;
    		else if (invBER >= 3936 && invBER < 4955) //3.6
    		    BER_SQI = 32 ;
    		else if (invBER >= 4955 && invBER < 6238) //3.7
    		    BER_SQI = 34 ;
    		else if (invBER >= 6238 && invBER < 7853) //3.8
    		    BER_SQI = 36 ;
    		else if (invBER >= 7853 && invBER < 9886) //3.9
    		    BER_SQI = 38 ;
    		else if (invBER >= 9886 && invBER < 12446) //4.0
    		    BER_SQI = 40 ;
    		else if (invBER >= 12446 && invBER < 15668) //4.1
    		    BER_SQI = 42 ;
    		else if (invBER >= 15668 && invBER < 19725) //4.2
    		    BER_SQI = 44 ;
    		else if (invBER >= 19725 && invBER < 24832) //4.3
    		    BER_SQI = 46 ;
    		else if (invBER >= 24832 && invBER < 31261) //4.4
    		    BER_SQI = 48 ;
    		else if (invBER >= 31261 && invBER < 39356) //4.5
    		    BER_SQI = 50 ;
    		else if (invBER >= 39356 && invBER < 49546) //4.6
    		    BER_SQI = 52 ;
    		else if (invBER >= 49546 && invBER < 62374) //4.7
    		    BER_SQI = 54 ;
    		else if (invBER >= 62374 && invBER < 78524) //4.8
    		    BER_SQI = 56 ;
    		else if (invBER >= 78524 && invBER < 98856) //4.9
    		    BER_SQI = 58 ;
    		else if (invBER >= 98856 && invBER < 124452) //5.0
    		    BER_SQI = 60 ;
    		else if (invBER >= 124452 && invBER < 156676) //5.1
    		    BER_SQI = 62 ;
    		else if (invBER >= 156676 && invBER < 197243) //5.2
    		    BER_SQI = 64 ;
    		else if (invBER >= 197243 && invBER < 248314) //5.3
    		    BER_SQI = 66 ;
    		else if (invBER >= 248314 && invBER < 312609) //5.4
    		    BER_SQI = 68 ;
    		else if (invBER >= 312609 && invBER < 393551) //5.5
    		    BER_SQI = 70 ;
    		else if (invBER >= 393551 && invBER < 495451) //5.6
    		    BER_SQI = 72 ;
    		else if (invBER >= 495451 && invBER < 623735) //5.7
    		    BER_SQI = 74 ;
    		else if (invBER >= 623735 && invBER < 785237) //5.8
    		    BER_SQI = 76 ;
    		else if (invBER >= 785237 && invBER < 988554) //5.9
    		    BER_SQI = 78 ;
    		else if (invBER >= 988554 && invBER < 1244515) //6.0
    		    BER_SQI = 80 ;
    		else if (invBER >= 1244515 && invBER < 1566751) //6.1
    		    BER_SQI = 82 ;
    		else if (invBER >= 1566751 && invBER < 1972422) //6.2
    		    BER_SQI = 84 ;
    		else if (invBER >= 1972422 && invBER < 2483135) //6.3
    		    BER_SQI = 86 ;
    		else if (invBER >= 2483135 && invBER < 3126081) //6.5
    		    BER_SQI = 88 ;
    		else if (invBER >= 3126081 && invBER < 3935502) //6.6
    		    BER_SQI = 90 ;
    		else if (invBER >= 3935502 && invBER < 4954502) //6.7
    		    BER_SQI = 92 ;                
    		else if (invBER >= 4954502 && invBER < 6237347) //6.8
    		    BER_SQI = 94 ;
    		else if (invBER >= 6237347 && invBER < 7852361) //6.9
    		    BER_SQI = 96 ;
    		else if (invBER >= 7852361 && invBER < 9885534) //7.0
    		    BER_SQI = 98 ;
    		else
    		    BER_SQI = 100 ;
    	}       
    	else if(invBER > 10000000) {
    	     BER_SQI = 100;
    	 }
    }
exit:
    return BER_SQI;
}

Dword DRV_Dib9090_getStatistic (Device_Context *DC, GetStatisticRequest *getstatisticrequest, Byte byChipType, Bool bIsRescan)
{
	Dword error = Error_NO_ERROR;
	struct dibDVBSignalStatus status[4];
    Byte BER_SQI = 0;
    Bool bIsLocked = 0;
    Bool bIsTPSLocked = 0;
    int i = 0, iSize = 0;
    uint8_t u8MaxStrength = 0;
    int iMaxStrengthIndex = -1;
    
    
    for(i = 0; i < 4; i ++) {
		memset(&status[i] , 0, sizeof(struct dibDVBSignalStatus));
	}
    
    switch(byChipType)
    {
		case EEPROM_Dibcom9090:
			iSize = 2;
			break;
		case EEPROM_Dibcom9090_4chan:
			iSize = 4;
			break;	
		default:
			iSize = 0;
			break;		
	}
	
	for(i = 0; i < iSize; i++) {
		demod_get_monitoring(&DC->dib9090.frontend[i], &DC->dib9090.monitor[i]);	
	}
	
	for(i = 0; i < iSize; i++) {
		DC->dib9090.frontend[i].demod_info->ops.get_signal_status(&DC->dib9090.frontend[i], &status[i]);
	}

   
	for(i = 0; i < iSize; i++) {
		//signalLocked[i] = (Bool)DC->dib9090.monitor[i].locks.tps_sync & DC->dib9090.monitor[i].locks.fec_mpeg;
		bIsLocked |= DC->dib9090.monitor[i].locks.tps_sync & DC->dib9090.monitor[i].locks.fec_mpeg;
		bIsTPSLocked |= DC->dib9090.monitor[i].locks.tps_sync;	
		
		if( status[i].signal_strength_dib > u8MaxStrength) {
			
			u8MaxStrength = status[i].signal_strength_dib;
			iMaxStrengthIndex = i;
		}
	}
		
	BER_SQI = GetDib0090BER(DC, iMaxStrengthIndex);		
	getstatisticrequest->statistic.signalLocked = bIsLocked;	//demod_lock
	getstatisticrequest->statistic.signalPresented = bIsTPSLocked; //fec_lock		
	getstatisticrequest->statistic.signalQuality = (Dword)BER_SQI;
	getstatisticrequest->statistic.signalStrength = (Dword)u8MaxStrength;
	getstatisticrequest->snr = 0;
	getstatisticrequest->rf_power = 0;
	// Re-train
	if ((!getstatisticrequest->statistic.signalPresented) && (!bIsRescan)) {
		struct dibChannel ch;	
		memset(&ch , 0, sizeof(struct dibChannel));
		

		deb_data("Re-train F: %d B: %d start\n", (int)DC->dib9090.frequency, DC->dib9090.bandwidth);
		INIT_CHANNEL(&ch, STANDARD_DVBT);
		ch.RF_kHz = DC->dib9090.frequency;
		ch.bandwidth_kHz = DC->dib9090.bandwidth;			
		tune_diversity(DC->dib9090.frontend, iSize, &ch);
		deb_data("Re-train done\n");
		
		DRV_Dib9090_getStatistic(DC, getstatisticrequest, byChipType, True); //Rescan
		
	}
	
	return error;
}

Dword DRV_Dib9090_getChannelStatistic (Device_Context *DC, ChannelStatistic* channelStatistic, int br_idx, int ts_idx)
{
	Dword error = Error_NO_ERROR;

	return error;
}

Dword DRV_Dib9090_isLocked(Device_Context *DC, Bool* locked, Byte byChipType)
{
	Dword error = Error_NO_ERROR;
	Bool signalLocked[4];
	int i = 0;
	int iSize = 0;
	
	
	switch(byChipType)
	{
		case EEPROM_Dibcom9090:
			iSize = 2;
			break;
		case EEPROM_Dibcom9090_4chan:
			iSize = 4;
			break;
		default:
			break;
	}
	
	for(i = 0; i < iSize; i++) {
		demod_get_monitoring(&DC->dib9090.frontend[i], &DC->dib9090.monitor[i]);	
		signalLocked[i] = (Bool)DC->dib9090.monitor[i].locks.tps_sync & DC->dib9090.monitor[i].locks.fec_mpeg;
	}
	
	*locked = False;
	for(i = 0; i < iSize; i++) {
		if(signalLocked[i]) {
			*locked = True;
		}
	}
		
		
	return error;
}

Dword DRV_Dib9090_DeInitialize(Device_Context *DC, Byte byChipType)
{
	Dword error = Error_NO_ERROR;
	
	deb_data("DRV_Dib9090_DeInitialize\n");

	if (DC->dib9090.i2c) {
		if(byChipType == EEPROM_Dibcom9090_4chan)
		{
		}
		frontend_unregister_components(&DC->dib9090.frontend[1]);
		frontend_unregister_components(&DC->dib9090.frontend[0]);
		host_i2c_release(DC->dib9090.i2c);
		deb_data("host_i2c_release\n");
	}

	return 0;
}

/**********************************************************
*
*   DRV_Dib9090_getDiversityDetailData 
*   
*   purpose: This function could get much more signal 
*            information for DIBCOM diversity dongle.
*            By using ioctl command 
*            "IOCTL_ITE_DIVERSITY_GETDETAILDATA", could
*            call this function.
*
***********************************************************/
Dword DRV_Dib9090_getDiversityDetailData(Device_Context *DC, DibcomDiversityData* data, Byte byChipType)
{
	int i = 0, lock_idx = 0, iSize = 0;
	Byte byIsLock = 0;
 	Dword result = Error_NO_ERROR;
	struct dibDVBSignalStatus status[4];
	
    for(i = 0; i < 4; i ++) {
		memset(&status[i] , 0, sizeof(struct dibDVBSignalStatus));
	}
		
	switch(byChipType)
	{
		case EEPROM_Dibcom9090:
			iSize = 2;
			break;
		case EEPROM_Dibcom9090_4chan:
			iSize = 4;
			break;
		default:
			break;
	}
	
	for(i = 0; i < iSize; i++) {
		demod_get_monitoring(&DC->dib9090.frontend[i], &DC->dib9090.monitor[i]);	
	}
	
	for(i = 0; i < iSize; i++) {
		DC->dib9090.frontend[i].demod_info->ops.get_signal_status(&DC->dib9090.frontend[i], &status[i]);
	}
	
	for(i = 0 ; i < iSize ; i++) {
		byIsLock |= DC->dib9090.monitor[i].locks.fec_mpeg &	DC->dib9090.monitor[i].locks.tps_sync;
	}
	
	if(!byIsLock) {
		return DIB_RETURN_ERROR;
	}else {
		for(i = 0 ; i < iSize ; i++) {
			memcpy(&data->DibDetailData[i] , &DC->dib9090.monitor[i], sizeof(struct dibDemodMonitor) );
			memcpy(&data->status[i], &status[i], sizeof(struct dibDVBSignalStatus));
		}
		
	}
	return 0;
}

/**********************************************************
*
*   DL_Dib9090_getDiversityDetailData 
*   
*   purpose: This function will call DRV_Dib9090_getDiversityData
*            to get much more signal information, including, 
*            Constellation, CodeRate, GuardInterval 
*            and so forth.
*
***********************************************************/
Dword DL_Dib9090_getDiversityDetailData(Device_Context *DC, DibcomDiversityData* data, Byte byChipType)
{
	Dword result = Error_NO_ERROR;

	mutex_lock(&DC->dev_mutex);
		result = DRV_Dib9090_getDiversityDetailData(DC, data, byChipType);
	mutex_unlock(&DC->dev_mutex);

	return result;                                                                 
}


