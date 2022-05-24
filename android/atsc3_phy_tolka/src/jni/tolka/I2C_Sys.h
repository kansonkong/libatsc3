#ifndef _I2C_SYS_H_
#define _I2C_SYS_H_

#include "R855.h"
#include "IT9300.h"
typedef struct _I2C_LEN_TYPE
{
	UINT8 RegAddr;
	UINT8 Data[50];
	UINT8 Len;
}I2C_LEN_TYPE;

typedef struct _I2C_TYPE
{
	UINT8 RegAddr;
	UINT8 Data;
}I2C_TYPE_INFO;

void I2C_Init(Endeavour* endeavour, Byte i2cBus,Byte i2cAddr);
int Close_I2C();
R855_ErrCode I2C_Write_Len(I2C_LEN_TYPE *I2C_Info);
R855_ErrCode I2C_Read_Len(I2C_LEN_TYPE *I2C_Info);
R855_ErrCode I2C_Write(I2C_TYPE_INFO *I2C_Info);
int R855_Convert(int InvertNum);


#endif
