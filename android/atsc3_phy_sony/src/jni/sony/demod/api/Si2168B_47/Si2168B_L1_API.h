/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168B Layer 1 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   API functions prototypes used by commands and properties
   FILE: Si2168B_L1_API.h
   Supported IC : Si2168B
   Compiled for ROM 0 firmware 3_A_build_3
   Revision: REVISION_NAME
   Date: May 22 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef    _Si2168B_L1_API_H_
#define    _Si2168B_L1_API_H_

//Edit by ITE
//#include "Silabs_L0_API.h"
#include "Si_ITE_L0_API.h"

#ifdef    DEMOD_Si2168B
#ifndef   NO_SAT
  #define DEMOD_DVB_S_S2_DSS
#endif /* NO_SAT */
#endif /* DEMOD_Si2168B */

#include "Si2168B_Commands.h"
#include "Si2168B_Properties.h"
#include "Si2168B_typedefs.h"
#include "Si2168B_Commands_Prototypes.h"
#include "Si2168B_Properties_Functions.h"

#define NO_Si2168B_ERROR                     0x00
#define ERROR_Si2168B_PARAMETER_OUT_OF_RANGE 0x01
#define ERROR_Si2168B_ALLOCATING_CONTEXT     0x02
#define ERROR_Si2168B_SENDING_COMMAND        0x03
#define ERROR_Si2168B_CTS_TIMEOUT            0x04
#define ERROR_Si2168B_ERR                    0x05
#define ERROR_Si2168B_POLLING_CTS            0x06
#define ERROR_Si2168B_POLLING_RESPONSE       0x07
#define ERROR_Si2168B_LOADING_FIRMWARE       0x08
#define ERROR_Si2168B_LOADING_BOOTBLOCK      0x09
#define ERROR_Si2168B_STARTING_FIRMWARE      0x0a
#define ERROR_Si2168B_SW_RESET               0x0b
#define ERROR_Si2168B_INCOMPATIBLE_PART      0x0c
#define ERROR_Si2168B_DISEQC_BUS_NOT_READY   0x0d
#define ERROR_Si2168B_UNKNOWN_COMMAND        0xf0
#define ERROR_Si2168B_UNKNOWN_PROPERTY       0xf1

//Edit by ITE
//unsigned char Si2168B_L1_API_Init      (L1_Si2168B_Context *api, int add);
unsigned char Si2168B_L1_API_Init      (L1_Si2168B_Context *api, int add, Endeavour* endeavour, unsigned char chip, unsigned char i2cbus);

unsigned char Si2168B_L1_API_Patch     (L1_Si2168B_Context *api, int iNbBytes, unsigned char *pucDataBuffer);
unsigned char Si2168B_L1_CheckStatus   (L1_Si2168B_Context *api);
char*         Si2168B_L1_API_ERROR_TEXT(int error_code);
char*         Si2168B_L1_API_TAG_TEXT  (void);

#define Si2168B_TAG Si2168B_L1_API_TAG_TEXT

#ifdef    Si2168B_GET_PROPERTY_STRING
void Si2168B_L1_FillPropertyStringText(L1_Si2168B_Context *api, unsigned int prop_code, const char *separator, char *msg);
#endif /* Si2168B_GET_PROPERTY_STRING */

#endif /* _Si2168B_L1_API_H_ */






