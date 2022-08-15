#include "Si_ITE_L0_API.h"

unsigned int system_time (void)
{
	Dword time;
	BrUser_time(&time);
	return (unsigned int)time;
}

void system_wait (int time_ms)
{
	void* dummy = NULL;
	BrUser_delay((Bridge*)dummy, time_ms);
}

void L0_SetAddress (L0_Context* i2c, int add, Endeavour* endeavour, unsigned char chip, unsigned char i2cbus)
{
	i2c->endeavour_si = (Endeavour*)endeavour;
	i2c->address = add;
	i2c->bus = i2cbus;
	i2c->bridx = chip;
}

unsigned char L0_ReadCommandBytes (L0_Context* i2c, unsigned char iNbBytes, unsigned char *pucDataBuffer)
{
	Dword error = 0;

	error = IT9300_readGenericRegisters((Endeavour*)i2c->endeavour_si, i2c->bridx, i2c->bus, (unsigned char) i2c->address, iNbBytes, pucDataBuffer);
	if(error)
		return 0;
	else
		return iNbBytes;
}

unsigned char L0_WriteCommandBytes (L0_Context* i2c, unsigned char iNbBytes, unsigned char *pucDataBuffer)
{
	Dword error = 0;

	error = IT9300_writeGenericRegisters((Endeavour*)i2c->endeavour_si, i2c->bridx, i2c->bus, (unsigned char) i2c->address, iNbBytes, pucDataBuffer);
	if(error)
		return 0;
	else
		return iNbBytes;
}

