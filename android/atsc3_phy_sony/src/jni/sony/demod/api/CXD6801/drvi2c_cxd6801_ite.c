#include "drvi2c_cxd6801_ite.h"


sony_cxd6801_result_t drvi2c_cxd6801_ite_Read(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode);
sony_cxd6801_result_t drvi2c_cxd6801_ite_Write(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t* pData, uint32_t size, uint8_t mode);


void drvi2c_cxd6801_ite_Initialize(drvi2c_cxd6801_ite_t* pDrvI2c, Endeavour* endeavour, uint8_t i2cbus)
{
	pDrvI2c->endeavour_sony = endeavour;
	pDrvI2c->i2cbus = i2cbus;
}


void drvi2c_cxd6801_ite_CreateI2c(sony_cxd6801_i2c_t* pI2c, drvi2c_cxd6801_ite_t* pDrvI2c)
{	
    pI2c->Read = drvi2c_cxd6801_ite_Read;
    pI2c->Write = drvi2c_cxd6801_ite_Write;
    pI2c->ReadRegister = sony_cxd6801_i2c_CommonReadRegister;
    pI2c->WriteRegister = sony_cxd6801_i2c_CommonWriteRegister;
    pI2c->WriteOneRegister = sony_cxd6801_i2c_CommonWriteOneRegister;
    pI2c->gwAddress = 0;
    pI2c->gwSub = 0;
	pI2c->user = pDrvI2c;
}


sony_cxd6801_result_t drvi2c_cxd6801_ite_Read(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, uint8_t* pData, uint32_t size, uint8_t mode)
{
	Dword error;
	Byte buf[4];
	drvi2c_cxd6801_ite_t *pDrvI2c = NULL;
	

	pDrvI2c = (drvi2c_cxd6801_ite_t*)(pI2c->user);


	if(size <= 40)
	{
		error = IT9300_readGenericRegisters(pDrvI2c->endeavour_sony, 0, pDrvI2c->i2cbus, deviceAddress, (Byte)size, pData);
		if (error) return SONY_CXD6801_RESULT_ERROR_I2C;
	}
	else
	{
		buf[0] = 0xF5;
		buf[1] = pDrvI2c->i2cbus;
		buf[2] = deviceAddress;
		buf[3] = (Byte)size;
		error = IT9300_writeRegisters(pDrvI2c->endeavour_sony, 0, 0x4900, 4, buf);
		if (error) return SONY_CXD6801_RESULT_ERROR_I2C;

		error = IT9300_readRegisters(pDrvI2c->endeavour_sony, 0, 0xF000, (Byte)size, pData);
		if (error) return SONY_CXD6801_RESULT_ERROR_I2C;
	}


	return SONY_CXD6801_RESULT_OK;
}


sony_cxd6801_result_t drvi2c_cxd6801_ite_Write(sony_cxd6801_i2c_t* pI2c, uint8_t deviceAddress, const uint8_t* pData, uint32_t size, uint8_t mode)
{
	Dword error;
	Byte buf[4];
	drvi2c_cxd6801_ite_t *pDrvI2c = NULL;
	Byte target_size;
	Byte target_offset;

	pDrvI2c = (drvi2c_cxd6801_ite_t*)(pI2c->user);


	if ((mode & 0x02) == 0)
	{
		error = IT9300_writeRegister(pDrvI2c->endeavour_sony, 0, p_br_reg_repeat_start, 1);
		if (error) return SONY_CXD6801_RESULT_ERROR_I2C;
	}

	if(size <= 40)
	{
		error = IT9300_writeGenericRegisters(pDrvI2c->endeavour_sony, 0, pDrvI2c->i2cbus, deviceAddress, (Byte)size, (Byte *)pData);
		if (error) return SONY_CXD6801_RESULT_ERROR_I2C;
	}
	else
	{
		target_size = (Byte)size;
		target_offset = 0;
		while(target_size >= 40)
		{
			error = IT9300_writeRegisters(pDrvI2c->endeavour_sony, 0, 0xF001 + target_offset, 40, (Byte *)(pData + target_offset));
			if (error) return SONY_CXD6801_RESULT_ERROR_I2C;
			target_offset += 40;
			target_size -= 40;
		}
		if(target_size > 0)
		{
			error = IT9300_writeRegisters(pDrvI2c->endeavour_sony, 0, 0xF001 + target_offset, target_size, (Byte *)(pData + target_offset));
			if (error) return SONY_CXD6801_RESULT_ERROR_I2C;
		}

		buf[0] = 0xF4;
		buf[1] = pDrvI2c->i2cbus;
		buf[2] = deviceAddress;
		buf[3] = (Byte)size;
		error = IT9300_writeRegisters(pDrvI2c->endeavour_sony, 0, 0x4900, 4, buf);
		if (error) return SONY_CXD6801_RESULT_ERROR_I2C;
	}


	if ((mode & 0x02) == 0)
	{
		error = IT9300_writeRegister(pDrvI2c->endeavour_sony, 0, p_br_reg_repeat_start, 0);
		if (error) return SONY_CXD6801_RESULT_ERROR_I2C;
	}

	return SONY_CXD6801_RESULT_OK;
}



