#ifndef __SI_ITE_L0_API_H
#define __SI_ITE_L0_API_H

#include "IT9300.h"

#include <linux/kernel.h>
#include <linux/module.h>
#define SiTRACE(...) 
//#define SiTRACE	printk
#define SiTraceConfiguration(...) 
#define sprintf(...)
#define strcat(...)
#define SiERROR(...)


typedef struct L0_Context
{
	Handle			endeavour_si;
    int				address;
	unsigned char	bus;
	unsigned char	bridx;
    int             indexSize;
    int             mustReadWithoutStop;
} L0_Context;


unsigned int system_time (void);
void system_wait (int time_ms);

void L0_SetAddress (L0_Context* i2c, int add, Endeavour* endeavour, unsigned char chip, unsigned char i2cbus);
unsigned char L0_ReadCommandBytes (L0_Context* i2c, unsigned char iNbBytes, unsigned char *pucDataBuffer);
unsigned char L0_WriteCommandBytes (L0_Context* i2c, unsigned char iNbBytes, unsigned char *pucDataBuffer);












#endif
