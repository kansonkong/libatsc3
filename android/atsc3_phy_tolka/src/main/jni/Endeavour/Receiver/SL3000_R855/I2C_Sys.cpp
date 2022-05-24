#include "I2C_Sys.h"
Endeavour*	endeavour_r855;
Byte		endeavour_r855_i2cBus;
Byte		r855_i2cAddr;


void I2C_Init(Endeavour* endeavour, Byte i2cBus,Byte i2cAddr)
{
	endeavour_r855 = endeavour;
	endeavour_r855_i2cBus = i2cBus;
	r855_i2cAddr=i2cAddr;

}

int Close_I2C(void)
{
	//implement your I2C code here//

	return TRUE;
}

//int I2C_Write_Len(I2C_LEN_TYPE *I2C_Info)
R855_ErrCode	I2C_Write_Len(I2C_LEN_TYPE *I2C_Info)
{
	Dword	error;

	Byte		buf[255];
	int 		len=0;

	if (I2C_Info->Len >= 254)
	{
		printf("I2C read length >= 254 bytes!!!!\n");
		return R855_Fail;
	}
	
	buf[0]=I2C_Info->RegAddr;
	len=I2C_Info->Len+1;
	
	for (int i = 0; i < I2C_Info->Len; i++) 
		  buf[1 + i] = I2C_Info->Data[i]; 

	//for(int i=0;i<len;i++)
		//R855_PRINT("[I2C_Write_Len]buf[%d]=0x%x\n",i, buf[i]);


	error = IT9300_ExtI2C_write(endeavour_r855, 0, endeavour_r855_i2cBus, r855_i2cAddr, len, buf, False);


	if(error)
		return R855_Fail;
	else
		return R855_Success;

}



R855_ErrCode I2C_Read_Len(I2C_LEN_TYPE *I2C_Info)
{
	Dword	error;

	if (I2C_Info->Len >= 255)
	{
		printf("I2C read length >= 255 bytes!!!!\n");
		return R855_Fail;
	}
	I2C_Info->Data[0]=I2C_Info->RegAddr;
	I2C_Info->Len=I2C_Info->Len+1;	


	if((I2C_Info->Len)>0 &&I2C_Info->Len<50 )
	{
	
		//error = IT9300_ExtI2C_read(endeavour_r855, 0, endeavour_r855_i2cBus, r855_i2cAddr, len, buf);
		error = IT9300_ExtI2C_read(endeavour_r855, 0, endeavour_r855_i2cBus, r855_i2cAddr, I2C_Info->Len, I2C_Info->Data);
		for(int i=0;i<I2C_Info->Len;i++)
			I2C_Info->Data[i]= R855_Convert(I2C_Info->Data[i]);
	}
	else
	{	

		printf("[I2C_Read_Len]Len is over limited\n");
		return R855_Fail;
	}
#if 0
	printf("[I2C_Read_Len]error=0x%x\n",error);
	for (int i = 0; i < I2C_Info->Len; i++) 
		printf("[I2C_Read_Len]I2C_Info->Data[%d]=0x%x\n",i,I2C_Info->Data[i]);

#endif
	
	if(error)
		return R855_Fail;
	else
		return R855_Success;


}



R855_ErrCode I2C_Write(I2C_TYPE_INFO *I2C_Info)
{
	Dword	error;

	Byte		buf[2];
	int 		len=0;

	
	buf[0]=I2C_Info->RegAddr;
	buf[1] = I2C_Info->Data; 

#if 0
	for(int i=0;i<len;i++)
		R855_PRINT("[I2C_Write_Len]buf[%d]=0x%x\n",i, buf[i]);
#endif

	error = IT9300_ExtI2C_write(endeavour_r855, 0, endeavour_r855_i2cBus, r855_i2cAddr, 2, buf, False);



	if(error)
		return R855_Fail;
	else
		return R855_Success;


}


int R855_Convert(int InvertNum)
{
	int ReturnNum = 0;
	int AddNum    = 0x80;
	int BitNum    = 0x01;
	int CuntNum   = 0;

	for(CuntNum = 0;CuntNum < 8;CuntNum ++)
	{
		if(BitNum & InvertNum)
			ReturnNum += AddNum;

		AddNum /= 2;
		BitNum *= 2;
	}

	return ReturnNum;
}
