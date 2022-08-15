/*************************************************************************************
                  Silicon Laboratories Broadcast Si2148 Layer 1 API
   API functions prototypes used by commands and properties
   FILE: Si2148_L1_API.h
   Supported IC : Si2148
   Compiled for ROM 33 firmware 2_1_build_6
   Revision: 0.1
   Date: November 12 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef    _Si2148_L1_API_H_
#define    _Si2148_L1_API_H_

//#include "Silabs_L0_API.h"
#include "Si_ITE_L0_API.h"

/* _demod_defines_insertion_start */
/* _demod_defines_insertion_point */

#include "Si2148_Commands.h"
#include "Si2148_Properties.h"
#include "Si2148_typedefs.h"
#include "Si2148_Commands_Prototypes.h"

#define NO_Si2148_ERROR                     0x00
#define ERROR_Si2148_PARAMETER_OUT_OF_RANGE 0x01
#define ERROR_Si2148_ALLOCATING_CONTEXT     0x02
#define ERROR_Si2148_SENDING_COMMAND        0x03
#define ERROR_Si2148_CTS_TIMEOUT            0x04
#define ERROR_Si2148_ERR                    0x05
#define ERROR_Si2148_POLLING_CTS            0x06
#define ERROR_Si2148_POLLING_RESPONSE       0x07
#define ERROR_Si2148_LOADING_FIRMWARE       0x08
#define ERROR_Si2148_LOADING_BOOTBLOCK      0x09
#define ERROR_Si2148_STARTING_FIRMWARE      0x0a
#define ERROR_Si2148_SW_RESET               0x0b
#define ERROR_Si2148_INCOMPATIBLE_PART      0x0c
/* _specific_error_value_insertion_start */
#define ERROR_Si2148_TUNINT_TIMEOUT         0x0d
#define ERROR_Si2148_xTVINT_TIMEOUT         0x0e
#define ERROR_Si2148_CRC_CHECK_ERROR        0x0f
/* _specific_error_value_insertion_point */

/* define PART_INTEGRITY_CHECKS (here) and the expected part number, chip rev, pmajor, pminor, and romid (in Si2148_L1_API.c) to use integrity checking */
#undef PART_INTEGRITY_CHECKS
/* chip rev constants for integrity checking */
extern const char Si2148_chiprev;
extern const char Si2148_part;
extern const char Si2148_partMajorVersion;
extern const char Si2148_partMinorVersion;
extern const char Si2148_partRomid;

unsigned char Si2148_L1_API_Init    (L1_Si2148_Context *api, int add, Endeavour* endeavour, unsigned char chip, unsigned char i2cbus);
unsigned char Si2148_L1_API_Patch   (L1_Si2148_Context *api,  unsigned char iNbBytes, unsigned char *pucDataBuffer);
unsigned char Si2148_L1_CheckStatus (L1_Si2148_Context *api);
char*         Si2148_L1_API_ERROR_TEXT(int  error_code);
char*         Si2148_L1_API_TAG_TEXT  (void);

#define Si2148_TAG Si2148_L1_API_TAG_TEXT


#ifdef    Si2148_GET_PROPERTY_STRING
void Si2148_L1_FillPropertyStringText(L1_Si2148_Context *api, unsigned int prop_code, const char *separator, char *msg);
#endif /* Si2148_GET_PROPERTY_STRING */

#endif /* _Si2148_L1_API_H_ */






