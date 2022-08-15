/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 1 API
   API functions prototypes used by commands and properties
   FILE: Si2168_L1_API.h
   Supported IC : Si2168
   Compiled for ROM 2 firmware 2_0_build_11
   Revision: REVISION_NAME
   Date: September 24 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef    _Si2168_L1_API_H_
#define    _Si2168_L1_API_H_

//#include "Silabs_L0_API.h"
#include "Si_ITE_L0_API.h"

/* _demod_defines_insertion_start */
#ifdef    DEMOD_Si2168
#ifndef   NO_SAT
  #define DEMOD_DVB_S_S2_DSS
#endif /* NO_SAT */
#endif /* DEMOD_Si2168 */
/* _demod_defines_insertion_point */

#include "Si2168_Commands.h"
#include "Si2168_Properties.h"
#include "Si2168_typedefs.h"
#include "Si2168_Commands_Prototypes.h"

#define NO_Si2168_ERROR                     0x00
#define ERROR_Si2168_PARAMETER_OUT_OF_RANGE 0x01
#define ERROR_Si2168_ALLOCATING_CONTEXT     0x02
#define ERROR_Si2168_SENDING_COMMAND        0x03
#define ERROR_Si2168_CTS_TIMEOUT            0x04
#define ERROR_Si2168_ERR                    0x05
#define ERROR_Si2168_POLLING_CTS            0x06
#define ERROR_Si2168_POLLING_RESPONSE       0x07
#define ERROR_Si2168_LOADING_FIRMWARE       0x08
#define ERROR_Si2168_LOADING_BOOTBLOCK      0x09
#define ERROR_Si2168_STARTING_FIRMWARE      0x0a
#define ERROR_Si2168_SW_RESET               0x0b
#define ERROR_Si2168_INCOMPATIBLE_PART      0x0c
#define ERROR_Si2168_DISEQC_BUS_NOT_READY   0x0d

unsigned char Si2168_L1_API_Init    (L1_Si2168_Context *api, int add, Endeavour* endeavour, unsigned char chip, unsigned char i2cbus);
unsigned char Si2168_L1_API_Patch   (L1_Si2168_Context *api, int iNbBytes, unsigned char *pucDataBuffer);
unsigned char Si2168_L1_CheckStatus (L1_Si2168_Context *api);
char*         Si2168_L1_API_ERROR_TEXT(unsigned char  error_code);
char*         Si2168_L1_API_TAG_TEXT  (void);

int Si2168_L1_Status            (L1_Si2168_Context *api);

#define Si2168_TAG Si2168_L1_API_TAG_TEXT

#ifdef    Si2168_GET_PROPERTY_STRING
void Si2168_L1_FillPropertyStringText(L1_Si2168_Context *api, unsigned int prop_code, const char *separator, char *msg);
#endif /* Si2168_GET_PROPERTY_STRING */

#endif /* _Si2168_L1_API_H_ */






