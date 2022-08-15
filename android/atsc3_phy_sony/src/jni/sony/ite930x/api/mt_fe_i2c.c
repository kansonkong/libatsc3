/****************************************************************************
* MONTAGE PROPRIETARY AND CONFIDENTIAL
* Montage Technology (Shanghai) Inc.
* All Rights Reserved
* --------------------------------------------------------------------------
*
* File:				mt_fe_i2c.c
*
* Current version:	1.00.00
*
* Description:		Define all i2c function for FE module.
*
* Log:	Description			Version		Date		Author
*		---------------------------------------------------------------------
*		Create				1.00.00		2010.09.15	YZ.Huang
*		Modify				1.00.00		2010.09.15	YZ.Huang
****************************************************************************/
#include "mt_fe_def.h"
#include "IT9300.h"

Endeavour* endeavour_mt_fe;
U8 endeavour_mt_fe_I2C_bus;
U8 endeavour_mt_fe_chip_idx;


void _mt_fe_dmd_set_endeavour(Endeavour* endeavour, U8 endeavourChipidx, U8 i2cbus)
{
	endeavour_mt_fe = endeavour;
	endeavour_mt_fe_chip_idx = endeavourChipidx;
	endeavour_mt_fe_I2C_bus = i2cbus;	
}


/*****************************************************************
** Function: _mt_fe_dmd_set_reg
**
**
** Description:	write data to demod register
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	reg_index		U8			register index
**	data			U8			value to write
**
**
** Outputs:
**
**
*****************************************************************/
MT_FE_RET _mt_fe_dmd_set_reg(U8 reg_index, U8 data)
{

	Dword error = 0;
	U8 buffer[2];
	buffer[0] = reg_index;
	buffer[1] = data;

	error = IT9300_writeGenericRegisters(endeavour_mt_fe, endeavour_mt_fe_chip_idx, endeavour_mt_fe_I2C_bus, MT_FE_DMD_I2C_ADDR_DS3K, 2, buffer);

	if(error) return MtFeErr_I2cErr;
		
	return MtFeErr_Ok;
}



/*****************************************************************
** Function: _mt_fe_dmd_get_reg
**
**
** Description:	read data from demod register
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	reg_index		U8			register index
**
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	p_buf			U8*			register data
**
**
*****************************************************************/
MT_FE_RET _mt_fe_dmd_get_reg(U8 reg_index, U8 *p_buf)
{
	Dword error = 0;
	U8 wBuffer;

	wBuffer = reg_index;

	error = IT9300_writeGenericRegisters(endeavour_mt_fe, endeavour_mt_fe_chip_idx, endeavour_mt_fe_I2C_bus, MT_FE_DMD_I2C_ADDR_DS3K, 1, &wBuffer);
	if(error) return MtFeErr_I2cErr;
	error = IT9300_readGenericRegisters(endeavour_mt_fe, endeavour_mt_fe_chip_idx, endeavour_mt_fe_I2C_bus, MT_FE_DMD_I2C_ADDR_DS3K, 1, p_buf);
	if(error) return MtFeErr_I2cErr;

	return MtFeErr_Ok;
}





/*****************************************************************
** Function: _mt_fe_tn_set_reg
**
**
** Description:	write data to tuner register
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	reg_index		U8			register index
**	data			U8			value to write
**
**
** Outputs:
**
**
*****************************************************************/
MT_FE_RET _mt_fe_tn_set_reg(U8 reg_index, U8 data)
{
	MT_FE_RET ret;
	U8 val;
	Dword error = 0;
	U8 buffer[2];

	/*open I2C repeater*/
	/*Do not care to close the I2C repeater, it will close by itself*/
	val = 0x11;
	ret = _mt_fe_dmd_set_reg(0x03, val);
	if (ret != MtFeErr_Ok)
		return ret;

	buffer[0] = reg_index;
	buffer[1] = data;
	
	error = IT9300_writeGenericRegisters(endeavour_mt_fe, endeavour_mt_fe_chip_idx, endeavour_mt_fe_I2C_bus, MT_FE_TN_I2C_ADDR, 2, buffer);

	if(error) return MtFeErr_I2cErr;	
	
	return MtFeErr_Ok;
}




/*****************************************************************
** Function: _mt_fe_tn_get_reg
**
**
** Description:	get tuner register data
**
**
** Inputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	register		U8			register address
**
**
** Outputs:
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	p_buf			U8*			register data
**
**
*****************************************************************/
MT_FE_RET _mt_fe_tn_get_reg(U8 reg_index, U8 *p_buf)
{
	MT_FE_RET ret;
	U8 val;
	Dword error = 0;
	U8 wBuffer;

	/*open I2C repeater*/
	/*Do not care to close the I2C repeater, it will close by itself*/
	val = 0x12;
		// IMPORTANT:
		// This value can be 0x11 or 0x12.
		// It depends on the sum of I2C_STOP flags in a whole I2CRead operation flow.
		// 0x11 means there's only ONE I2C_STOP flag.
		// 0x12 means there're two I2C_STOP flags.
		// Please refer to the application notes for detail descriptions
	ret = _mt_fe_dmd_set_reg(0x03, val);
	if (ret != MtFeErr_Ok)
		return ret;

	/*Do not sleep any time after I2C repeater is opened.*/
	/*please read tuner register at once.*/


	wBuffer = reg_index;

	error = IT9300_writeGenericRegisters(endeavour_mt_fe, endeavour_mt_fe_chip_idx, endeavour_mt_fe_I2C_bus, MT_FE_TN_I2C_ADDR, 1, &wBuffer);
	if(error) return MtFeErr_I2cErr;
	error = IT9300_readGenericRegisters(endeavour_mt_fe, endeavour_mt_fe_chip_idx, endeavour_mt_fe_I2C_bus, MT_FE_TN_I2C_ADDR, 1, p_buf);
	if(error) return MtFeErr_I2cErr;

	return MtFeErr_Ok;
}




MT_FE_RET _mt_fe_tn_write(U8 *p_buf, U16 n_byte)
{
	MT_FE_RET ret;
	U8	val;

	/*open I2C repeater*/
	/*Do not care to close the I2C repeater, it will close by itself*/
	val = 0x11;
	ret = _mt_fe_dmd_set_reg(0x03, val);
	if (ret != MtFeErr_Ok)
		return ret;


	/*Do not sleep any time after I2C repeater is opened.*/
	/*please write N bytes to register at once.*/

	return MtFeErr_Ok;
}




/*****************************************************************
** Function: _mt_fe_iic_write
**
**
** Description:	write n bytes via iic to device
**
**
** Inputs:
**
**
**	Parameter		Type		Description
**	----------------------------------------------------------
**	p_buf			U8*			pointer of the tuner data
**	n_byte			U16			the data length
**
**
** Outputs:		none
**
**
**
*****************************************************************/
MT_FE_RET _mt_fe_write_fw(U8 reg_index, U8 *p_buf, U16 n_byte)
{
	U8 i;
	U8 wBuffer[33];
	Dword error = 0;

	wBuffer[0] = reg_index;
	for(i = 0; i < 32; i++)
		wBuffer[i + 1] = p_buf[i]; 		

	error = IT9300_writeGenericRegisters(endeavour_mt_fe, endeavour_mt_fe_chip_idx, endeavour_mt_fe_I2C_bus, MT_FE_DMD_I2C_ADDR_DS3K, 33, wBuffer);

	if(error) return MtFeErr_I2cErr;

	return MtFeErr_Ok;
}

void _mt_sleep(U32 ms)
{
	BrUser_delay((Bridge*)endeavour_mt_fe, ms);	
}



