/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 1 API
   API functions definitions used by commands and properties
   FILE: Si2168_L1_API.c
   Supported IC : Si2168
   Compiled for ROM 2 firmware 2_0_build_11
   Revision: REVISION_NAME
   Tag:  V2.6.0
   Date: September 24 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
/* Change log:

 As from V2.6.0:
  In Si2168_L1_API_Patch:
  Correction on 'write' error handling (incorrect traces and return value)

 As from V2.4.5:
   In Si2168_L1_API_Init: Adding NO_SAT tags to allow using Si2168 code without SAT features

 As from 2.4.2:
  Removed 'old' part integrity checking variables and code (not used anymore)

 As from 2.1.1:
  In Si2168_L1_Status: using DD_STATUS_RESPONSE fields (previously DD_MODE_PROP fields)

**************************************************************************************/
#define   Si2168_COMMAND_PROTOTYPES

#include "Si2168_Platform_Definition.h"

/***********************************************************************************************************************
  Si2168_L1_API_Init function
  Use:        software initialisation function
              Used to initialize the software context
  Returns:    0 if no error
  Comments:   It should be called first and once only when starting the application
  Parameter:   **ppapi         a pointer to the api context to initialize
  Parameter:  add            the Si2168 I2C address
  Porting:    Allocation errors need to be properly managed.
  Porting:    I2C initialization needs to be adapted to use the available I2C functions
 ***********************************************************************************************************************/
unsigned char    Si2168_L1_API_Init      (L1_Si2168_Context *api, int add, Endeavour* endeavour, unsigned char chip, unsigned char i2cbus) {
    api->i2c = &(api->i2cObj);

    L0_SetAddress(api->i2c, add, endeavour, chip, i2cbus);

    api->cmd    = &(api->cmdObj);
    api->rsp    = &(api->rspObj);
    api->prop   = &(api->propObj);
    api->status = &(api->statusObj);

    api->tuner_ter_chip = 0;
    api->tuner_ter_clock_input = Si2168_CLOCK_MODE_TER;
    api->tuner_ter_clock_freq  = Si2168_REF_FREQUENCY_TER;

    return NO_Si2168_ERROR;
}
/***********************************************************************************************************************
  Si2168_L1_API_Patch function
  Use:        Patch information function
              Used to send a number of bytes to the Si2168. Useful to download the firmware.
  Returns:    0 if no error
  Parameter:  error_code the error code.
  Porting:    Useful for application development for debug purposes.
  Porting:    May not be required for the final application, can be removed if not used.
 ***********************************************************************************************************************/
unsigned char    Si2168_L1_API_Patch     (L1_Si2168_Context *api, int iNbBytes, unsigned char *pucDataBuffer) {
    unsigned char res;
    unsigned char rspByteBuffer[1];

//    SiTRACE("Si2168 Patch %d bytes, ",iNbBytes);

    res = L0_WriteCommandBytes(api->i2c, (unsigned char) iNbBytes, pucDataBuffer);
    if (res!=iNbBytes) {
      SiTRACE("Si2168_L1_API_Patch error writing bytes: %s\n", Si2168_L1_API_ERROR_TEXT(ERROR_Si2168_LOADING_FIRMWARE) );
      return ERROR_Si2168_LOADING_FIRMWARE;
    }

    res = Si2168_pollForResponse(api, 1, rspByteBuffer);
    if (res != NO_Si2168_ERROR) {
      SiTRACE("Si2168_L1_API_Patch error 0x%02x polling response: %s\n", res, Si2168_L1_API_ERROR_TEXT(res) );
      return ERROR_Si2168_POLLING_RESPONSE;
    }

    return NO_Si2168_ERROR;
}
/***********************************************************************************************************************
  Si2168_L1_CheckStatus function
  Use:        Status information function
              Used to retrieve the status byte
  Returns:    0 if no error
  Parameter:  error_code the error code.
 ***********************************************************************************************************************/
unsigned char    Si2168_L1_CheckStatus   (L1_Si2168_Context *api) {
    unsigned char rspByteBuffer[1];
    return Si2168_pollForResponse(api, 1, rspByteBuffer);
}
/***********************************************************************************************************************
  Si2168_L1_API_ERROR_TEXT function
  Use:        Error information function
              Used to retrieve a text based on an error code
  Returns:    the error text
  Parameter:  error_code the error code.
  Porting:    Useful for application development for debug purposes.
  Porting:    May not be required for the final application, can be removed if not used.
 ***********************************************************************************************************************/
char*            Si2168_L1_API_ERROR_TEXT(unsigned char error_code) {
    switch (error_code) {
        case NO_Si2168_ERROR                     : return (char *)"No Si2168 error";
        case ERROR_Si2168_ALLOCATING_CONTEXT     : return (char *)"Error while allocating Si2168 context";
        case ERROR_Si2168_PARAMETER_OUT_OF_RANGE : return (char *)"Si2168 parameter(s) out of range";
        case ERROR_Si2168_SENDING_COMMAND        : return (char *)"Error while sending Si2168 command";
        case ERROR_Si2168_CTS_TIMEOUT            : return (char *)"Si2168 CTS timeout";
        case ERROR_Si2168_ERR                    : return (char *)"Si2168 Error (status 'err' bit 1)";
        case ERROR_Si2168_POLLING_CTS            : return (char *)"Si2168 Error while polling CTS";
        case ERROR_Si2168_POLLING_RESPONSE       : return (char *)"Si2168 Error while polling response";
        case ERROR_Si2168_LOADING_FIRMWARE       : return (char *)"Si2168 Error while loading firmware";
        case ERROR_Si2168_LOADING_BOOTBLOCK      : return (char *)"Si2168 Error while loading bootblock";
        case ERROR_Si2168_STARTING_FIRMWARE      : return (char *)"Si2168 Error while starting firmware";
        case ERROR_Si2168_SW_RESET               : return (char *)"Si2168 Error during software reset";
        case ERROR_Si2168_INCOMPATIBLE_PART      : return (char *)"Si2168 Error Incompatible part";
/* _specific_error_text_string_insertion_start */
/* _specific_error_text_string_insertion_point */
        default                                  : return (char *)"Unknown Si2168 error code";
    }
}
/***********************************************************************************************************************
  Si2168_L1_API_TAG_TEXT function
  Use:        Error information function
              Used to retrieve a text containing the TAG information (related to the code version)
  Returns:    the TAG text
  Porting:    May not be required for the final application, can be removed if not used.
 ***********************************************************************************************************************/
char*            Si2168_L1_API_TAG_TEXT(void) { return (char *)"V2.6.0";}

/* _specific_code_insertion_start */
 /************************************************************************************************************************
  NAME: Si2168_L1_Status
  DESCRIPTION: Calls the Si2168 global status function (DD_STATUS) and then the standard-specific status functions
  Porting:    Remove the un-necessary functions calls, if any. (Checking the TPS status may not be required)

  Parameter:  Pointer to Si2168 Context
  Returns:    1 if the current modulation is valid, 0 otherwise
************************************************************************************************************************/
int Si2168_L1_Status            (L1_Si2168_Context *api)
{
    /* Call the demod global status function */
    Si2168_L1_DD_STATUS (api, Si2168_DD_STATUS_CMD_INTACK_OK);

    /* Call the standard-specific status function */
    switch (api->rsp->dd_status.modulation)
    {
      case Si2168_DD_STATUS_RESPONSE_MODULATION_DVBT : { Si2168_L1_DVBT_STATUS  (api, Si2168_DVBT_STATUS_CMD_INTACK_OK );
        break;
      }

      case Si2168_DD_STATUS_RESPONSE_MODULATION_DVBT2: { Si2168_L1_DVBT2_STATUS (api, Si2168_DVBT2_STATUS_CMD_INTACK_OK);
        break;
      }

      case Si2168_DD_STATUS_RESPONSE_MODULATION_DVBC : { Si2168_L1_DVBC_STATUS  (api, Si2168_DVBC_STATUS_CMD_INTACK_OK );
        break;
      }


      default                                  : { return 0; break; }
    }

    switch (api->rsp->dd_status.modulation)
    {
      case Si2168_DD_STATUS_RESPONSE_MODULATION_DVBT2:


      default                                  : { Si2168_L1_DD_PER (api, Si2168_DD_PER_CMD_RST_RUN);
        break;
      }
    }
    switch (api->rsp->dd_status.modulation)
    {
      case Si2168_DD_STATUS_RESPONSE_MODULATION_DVBT : {
                                                   Si2168_L1_DVBT_TPS_EXTRA (api);
        break;
      }
      default                                  : { break; }
    }

 return 1;
}
/* _specific_code_insertion_point */





