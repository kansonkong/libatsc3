package com.api;

public class Variable {
	// ----- LL variables -----



	public static final int OVA_BASE = 0x4C00;

	//#define OVA_PRECHIP_VERSION_7_0
	public static final int OVA_LINK_VERSION = (OVA_BASE-4);
	public static final int OVA_LINK_VERSION_31_24 = (OVA_LINK_VERSION+0);
	public static final int OVA_LINK_VERSION_23_16 = (OVA_LINK_VERSION+1);
	public static final int OVA_LINK_VERSION_15_8 = (OVA_LINK_VERSION+2);
	public static final int OVA_LINK_VERSION_7_0 = (OVA_LINK_VERSION+3);
	public static final int OVA_SECOND_DEMOD_I2C_ADDR = (OVA_BASE-5);

	public static final int OVA_IR_TABLE = (OVA_BASE-361);
	public static final int OVA_HID_TABLE = OVA_IR_TABLE;
	public static final int OVA_IR_TOGGLE_MASK = (OVA_HID_TABLE+7*50);
	public static final int OVA_IR_nKEYS = (OVA_IR_TOGGLE_MASK+2);
	public static final int OVA_IR_FN_EXPIRE_TIME = (OVA_IR_nKEYS+1);
	public static final int OVA_IR_REPEAT_PERIOD = (OVA_IR_FN_EXPIRE_TIME+1);
	public static final int OVA_IR_RESERVED_PARAM = (OVA_IR_REPEAT_PERIOD+1);

	public static final int OVA_IR_TABLE_ADDR = (OVA_BASE-363);
	public static final int OVA_IR_TABLE_ADDR_15_18 = (OVA_IR_TABLE_ADDR+0);
	public static final int OVA_IR_TABLE_ADDR_7_0 = (OVA_IR_TABLE_ADDR+1);
	public static final int OVA_HOST_REQ_IR_MODE = (OVA_BASE-364);
	public static final int OVA_EEPROM_CFG = (OVA_BASE-620);
	public static final int OVA_XC4000_PKTCNT = (OVA_BASE-621);
	public static final int OVA_XC4000_CLKCNT1 = (OVA_BASE-623);
	public static final int OVA_XC4000_CLKCNT2 = (OVA_BASE-625);
	public static final int OVA_I2C_NO_STOPBIT_PKTCNT = (OVA_BASE-626);
	public static final int OVA_CLK_STRETCH = (OVA_BASE-643);
	public static final int OVA_DUMMY0XX = (OVA_BASE-644);
	public static final int OVA_HW_VERSION = (OVA_BASE-645);
	public static final int OVA_TMP_HW_VERSION = (OVA_BASE-646);
	public static final int OVA_EEPROM_CFG_VALID = (OVA_BASE-647);
	public static final int OVA_THIRD_DEMOD_I2C_ADDR = (OVA_BASE-648);
	public static final int OVA_FOURTH_DEMOD_I2C_ADDR = (OVA_BASE-649);
	public static final int OVA_SECOND_DEMOD_I2C_BUS = (OVA_BASE-650);
	public static final int OVA_NEXT_LEVEL_FIRST_I2C_ADDR = (OVA_BASE-651);
	public static final int OVA_NEXT_LEVEL_SECOND_I2C_ADDR = (OVA_BASE-652);
	public static final int OVA_NEXT_LEVEL_THIRD_I2C_ADDR = (OVA_BASE-653);
	public static final int OVA_NEXT_LEVEL_FOURTH_I2C_ADDR = (OVA_BASE-654);
	public static final int OVA_NEXT_LEVEL_FIRST_I2C_BUS = (OVA_BASE-655);
	public static final int OVA_NEXT_LEVEL_SECOND_I2C_BUS = (OVA_BASE-656);
	public static final int OVA_NEXT_LEVEL_THIRD_I2C_BUS = (OVA_BASE-657);
	public static final int OVA_NEXT_LEVEL_FOURTH_I2C_BUS = (OVA_BASE-658);
	public static final int OVA_EEPROM_I2C_ADD = (OVA_BASE-659);
	public static final int OVA_EEPROM_TYPE = (OVA_BASE-660);
	public static final int OVA_UART_RX_LENGTH = (OVA_BASE-661);
	public static final int OVA_UART_RX_READY = (OVA_BASE-662);
	public static final int OVA_NEXT_LEVEL_FIRST_FW_INDEX = (OVA_BASE-663);
	public static final int OVA_NEXT_LEVEL_SECOND_FW_INDEX = (OVA_BASE-664);
	public static final int OVA_NEXT_LEVEL_THIRD_FW_INDEX = (OVA_BASE-665);
	public static final int OVA_NEXT_LEVEL_FOURTH_FW_INDEX = (OVA_BASE-666);
	public static final int OVA_UART_REALSEND = (OVA_BASE-667);
	//////////////////////////////////////////////////////////////////////////////////
	public static final int OVA_NEXT_LEVEL_FIFTH_I2C_ADDR = (OVA_BASE-668);
	public static final int OVA_NEXT_LEVEL_FIFTH_I2C_BUS = (OVA_BASE-669);
	public static final int OVA_UART_MODE = (OVA_BASE-670);

		
	public static final int OVA_EEPROM_BOOT_FW_OFFSET = (OVA_BASE-763);
	public static final int OVA_EEPROM_BOOT_FW_SIZE = (OVA_BASE-765);
	public static final int OVA_EEPROM_BOOT_FW_SEGMENT_NUM = (OVA_BASE-766);
	public static final int OVA_EEPROM_BOOT_ERROR_CODE = (OVA_BASE-767);
	public static final int OVA_TEST = (OVA_BASE-768);


	// For API: just renaming
	public static final int second_i2c_address = OVA_SECOND_DEMOD_I2C_ADDR;
	public static final int third_i2c_address = OVA_THIRD_DEMOD_I2C_ADDR;
	public static final int fourth_i2c_address = OVA_FOURTH_DEMOD_I2C_ADDR;
	public static final int second_i2c_bus = OVA_SECOND_DEMOD_I2C_BUS;
	public static final int next_level_first_i2c_address = OVA_NEXT_LEVEL_FIRST_I2C_ADDR;
	public static final int next_level_second_i2c_address = OVA_NEXT_LEVEL_SECOND_I2C_ADDR;
	public static final int next_level_third_i2c_address = OVA_NEXT_LEVEL_THIRD_I2C_ADDR;
	public static final int next_level_fourth_i2c_address = OVA_NEXT_LEVEL_FOURTH_I2C_ADDR;
	public static final int next_level_first_i2c_bus = OVA_NEXT_LEVEL_FIRST_I2C_BUS;
	public static final int next_level_second_i2c_bus = OVA_NEXT_LEVEL_SECOND_I2C_BUS;
	public static final int next_level_third_i2c_bus = OVA_NEXT_LEVEL_THIRD_I2C_BUS;
	public static final int next_level_fourth_i2c_bus = OVA_NEXT_LEVEL_FOURTH_I2C_BUS;

	public static final int ir_table_start_15_8 = OVA_IR_TABLE_ADDR_15_18;
	public static final int ir_table_start_7_0 = OVA_IR_TABLE_ADDR_7_0;
	public static final int link_version_31_24 = OVA_LINK_VERSION_31_24;
	public static final int link_version_23_16 = OVA_LINK_VERSION_23_16;
	public static final int link_version_15_8 = OVA_LINK_VERSION_15_8;
	public static final int link_version_7_0 = OVA_LINK_VERSION_7_0;

	// More for API...
	public static final int prechip_version_7_0 = 0x384F;
	public static final int chip_version_7_0 = 0x1222;




	// ----- OFDM variables -----
	//this file define variable which initialized by AP
	//CFOE------------------------------------------

	//These variables are initialized by API.
	//Don't change the order of the definition of these variables.



	//2k
	//BASE Address 0x418B
	public static final int var_addr_base = 0x418b;
	public static final int log_addr_base = 0x418d;
	public static final int log_data_base = 0x418f;
	public static final int LowerLocalRetrain = 0x43bb;

	//BASE Address 0x0000
	public static final int trigger_ofsm = 0x0000;
	public static final int Training_Mode = 0x0001;
	public static final int RESET_STATE = 0x0002;
	public static final int RelayCommandWrite = 0x0003;
	public static final int ECO_ASIC = 0x0004;
	public static final int var_FW_init_ready = 0x0005;

	public static final int var_end = 0x0006;

	//BASE Address 0xFFFF
}

