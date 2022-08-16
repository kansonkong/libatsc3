/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168B Layer 1 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   API functions definitions used by commands and properties
   FILE: Si2168B_L1_API.c
   Supported IC : Si2168B
   Compiled for ROM 0 firmware 3_A_build_3
   Revision: REVISION_NAME
   Tag:  V0.3.8_FW_4_4b17
   Date: May 22 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
/* Change log:

 As from V0.3.1:
  <new_feature> [SPI] in Si2168B_L1_API_Init:
    SPI download default values
     <porting> Set these values to match your SPI HW, or use 'SiLabs_API_SPI_Setup' to set them later on

 As from V0.2.4:
  <new_feature> In Si2168B_L1_API_Init:
    Default values for tuner clocks control set to match previous behavior:
      api->tuner_ter_clock_control = Si2168B_CLOCK_MANAGED;
      api->tuner_sat_clock_control = Si2168B_CLOCK_MANAGED;
  <cleanup>     In Si2168B_L1_API_Init: moving SiERROR message inside '#ifndef     Si2168B_SAT_CLOCK_SOURCE area'

 As from V0.2.2:
  In Si2168B_L1_API_Patch:
   Changing 'res' type to int, to avoid warnings when compiled with '-pedantic'
  In Si2168B_L1_API_ERROR_TEXT:
   Changing 'error_code' type to int, to avoid warnings when compiled with '-pedantic'

  In Si2168B_L1_API_Init:
   Setting dvbt2_fef.fef_tuner_flag  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_NOT_USED;
    in case Si2168B_L2_TER_FEF_CONFIG is not called during SW init

 As from V0.1.2:
  Adding Si2168B_L1_API_TER_Clock and Si2168B_L1_API_SAT_Clock to help configure clocks for multi-frontend cases

 As from V0.1.0:
  Version on the day we received the silicon

 As from V0.0.0:
  Initial version (based on Si2169 code V2.6.1)
**************************************************************************************/
#define   Si2168B_COMMAND_PROTOTYPES

#define   SILABS_API_LAYER 1
#include "Si2168B_Platform_Definition.h"

/***********************************************************************************************************************
  Si2168B_L1_API_Init function
  Use:        software initialisation function
              Used to initialize the software context
  Returns:    0 if no error
  Comments:   It should be called first and once only when starting the application
  Parameter:   **ppapi         a pointer to the api context to initialize
  Parameter:  add            the Si2168B I2C address
  Porting:    Allocation errors need to be properly managed.
  Porting:    I2C initialization needs to be adapted to use the available I2C functions
 ***********************************************************************************************************************/
//Edit by ITE
unsigned char    Si2168B_L1_API_Init      (L1_Si2168B_Context *api, int add, Endeavour* endeavour, unsigned char chip, unsigned char i2cbus) {
    api->i2c = &(api->i2cObj);
    
//Edit by ITE                  
//    L0_Init(api->i2c);
//    L0_SetAddress(api->i2c, add, 0);
    L0_SetAddress(api->i2c, add, endeavour, chip, i2cbus);

    api->cmd      = &(api->cmdObj);
    api->rsp      = &(api->rspObj);
    api->prop     = &(api->propObj);
    api->status   = &(api->statusObj);
    api->propShadow        = &(api->propShadowObj);
    /* Set the propertyWriteMode to Si2168B_DOWNLOAD_ON_CHANGE to only download property settings on change (recommended) */
    /*      if propertyWriteMode is set to Si2168B_DOWNLOAD_ALWAYS the properties will be downloaded regardless of change */
    api->propertyWriteMode = Si2168B_DOWNLOAD_ON_CHANGE;
    /* SPI download default values */
    api->spi_download    = 0;
    /* <porting> Set these values to match your SPI HW, or use 'SiLabs_API_SPI_Setup' to set them later on */
    api->spi_send_option = 0x01;
    api->spi_clk_pin     = Si2168B_SPI_LINK_CMD_SPI_CONF_CLK_DISEQC_IN;
    api->spi_clk_pola    = Si2168B_SPI_LINK_CMD_SPI_CLK_POLA_RISING;
    api->spi_data_pin    = Si2168B_SPI_LINK_CMD_SPI_CONF_DATA_DISEQC_CMD;
    api->spi_data_order  = Si2168B_SPI_LINK_CMD_SPI_DATA_DIR_MSB_FIRST;
    /* Clock settings as per compilation flags                     */
    /*  For multi-frontend HW, these may be adapted later on,      */
    /*   using Si2168B_L1_API_TER_Clock and Si2168B_L1_API_SAT_Clock */
  api->cmd->dvbt2_fef.fef_tuner_flag  = Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_NOT_USED;
 #ifndef     Si2168B_TER_CLOCK_SOURCE
  #define    Si2168B_TER_CLOCK_SOURCE            Si2168B_TER_Tuner_clock
    SiERROR("Si2168B_TER_CLOCK_SOURCE not defined at project level. Defaulting to 'TER_Tuner_clock' (can be changed using Si2168B_L2_TER_Clock)\n");
 #endif   /* Si2168B_TER_CLOCK_SOURCE */
    api->tuner_ter_clock_source  = Si2168B_TER_CLOCK_SOURCE;
    api->tuner_ter_clock_control = Si2168B_CLOCK_MANAGED;
    api->tuner_ter_agc_control   = 1;

 #ifndef     Si2168B_CLOCK_MODE_TER
  #define    Si2168B_CLOCK_MODE_TER                Si2168B_START_CLK_CMD_CLK_MODE_CLK_CLKIO
    SiERROR("Si2168B_CLOCK_MODE_TER   not defined at project level. Defaulting to 'CLK_CLKIO' (can be changed using Si2168B_L2_TER_Clock)\n");
 #endif   /* Si2168B_CLOCK_MODE_TER */
    api->tuner_ter_clock_input = Si2168B_CLOCK_MODE_TER;

 #ifndef     Si2168B_REF_FREQUENCY_TER
  #define    Si2168B_REF_FREQUENCY_TER                                             24
    SiERROR("Si2168B_REF_FREQUENCY_TER not defined at project level. Defaulting to 24 MHz (can be changed using Si2168B_L2_TER_Clock)\n");
 #endif   /* Si2168B_REF_FREQUENCY_TER */
    api->tuner_ter_clock_freq  = Si2168B_REF_FREQUENCY_TER;

    return NO_Si2168B_ERROR;
}
/***********************************************************************************************************************
  Si2168B_L1_API_Patch function
  Use:        Patch information function
              Used to send a number of bytes to the Si2168B. Useful to download the firmware.
  Returns:    0 if no error
  Parameter:  error_code the error code.
  Porting:    Useful for application development for debug purposes.
  Porting:    May not be required for the final application, can be removed if not used.
 ***********************************************************************************************************************/
unsigned char    Si2168B_L1_API_Patch     (L1_Si2168B_Context *api, int iNbBytes, unsigned char *pucDataBuffer) {
    int res;
    unsigned char rspByteBuffer[1];

    SiTRACE("Si2168B Patch %d bytes\n",iNbBytes);

    res = L0_WriteCommandBytes(api->i2c, iNbBytes, pucDataBuffer);
    if (res!=iNbBytes) {
      SiTRACE("Si2168B_L1_API_Patch error writing bytes: %s\n", Si2168B_L1_API_ERROR_TEXT(ERROR_Si2168B_LOADING_FIRMWARE) );
      return ERROR_Si2168B_LOADING_FIRMWARE;
    }

    res = Si2168B_pollForResponse(api, 1, rspByteBuffer);
    if (res != NO_Si2168B_ERROR) {
      SiTRACE("Si2168B_L1_API_Patch error 0x%02x polling response: %s\n", res, Si2168B_L1_API_ERROR_TEXT(res) );
      return ERROR_Si2168B_POLLING_RESPONSE;
    }

    return NO_Si2168B_ERROR;
}
/***********************************************************************************************************************
  Si2168B_L1_CheckStatus function
  Use:        Status information function
              Used to retrieve the status byte
  Returns:    0 if no error
  Parameter:  error_code the error code.
 ***********************************************************************************************************************/
unsigned char    Si2168B_L1_CheckStatus   (L1_Si2168B_Context *api) {
    unsigned char rspByteBuffer[1];
    return Si2168B_pollForResponse(api, 1, rspByteBuffer);
}
/***********************************************************************************************************************
  Si2168B_L1_API_ERROR_TEXT function
  Use:        Error information function
              Used to retrieve a text based on an error code
  Returns:    the error text
  Parameter:  error_code the error code.
  Porting:    Useful for application development for debug purposes.
  Porting:    May not be required for the final application, can be removed if not used.
 ***********************************************************************************************************************/
char*            Si2168B_L1_API_ERROR_TEXT(int error_code) {
    switch (error_code) {
        case NO_Si2168B_ERROR                     : return (char *)"No Si2168B error";
        case ERROR_Si2168B_ALLOCATING_CONTEXT     : return (char *)"Error while allocating Si2168B context";
        case ERROR_Si2168B_PARAMETER_OUT_OF_RANGE : return (char *)"Si2168B parameter(s) out of range";
        case ERROR_Si2168B_SENDING_COMMAND        : return (char *)"Error while sending Si2168B command";
        case ERROR_Si2168B_CTS_TIMEOUT            : return (char *)"Si2168B CTS timeout";
        case ERROR_Si2168B_ERR                    : return (char *)"Si2168B Error (status 'err' bit 1)";
        case ERROR_Si2168B_POLLING_CTS            : return (char *)"Si2168B Error while polling CTS";
        case ERROR_Si2168B_POLLING_RESPONSE       : return (char *)"Si2168B Error while polling response";
        case ERROR_Si2168B_LOADING_FIRMWARE       : return (char *)"Si2168B Error while loading firmware";
        case ERROR_Si2168B_LOADING_BOOTBLOCK      : return (char *)"Si2168B Error while loading bootblock";
        case ERROR_Si2168B_STARTING_FIRMWARE      : return (char *)"Si2168B Error while starting firmware";
        case ERROR_Si2168B_SW_RESET               : return (char *)"Si2168B Error during software reset";
        case ERROR_Si2168B_INCOMPATIBLE_PART      : return (char *)"Si2168B Error Incompatible part";
        case ERROR_Si2168B_UNKNOWN_COMMAND        : return (char *)"Si2168B Error unknown command";
        case ERROR_Si2168B_UNKNOWN_PROPERTY       : return (char *)"Si2168B Error unknown property";
        default                                  : return (char *)"Unknown Si2168B error code";
    }
}
/***********************************************************************************************************************
  Si2168B_L1_API_TAG_TEXT function
  Use:        Error information function
              Used to retrieve a text containing the TAG information (related to the code version)
  Returns:    the TAG text
  Porting:    May not be required for the final application, can be removed if not used.
 ***********************************************************************************************************************/
char*            Si2168B_L1_API_TAG_TEXT(void) { return (char *)"V0.3.8_FW_4_4b17";}

 /************************************************************************************************************************
  NAME: Si2168B_L1_Status
  DESCRIPTION: Calls the Si2168B global status function (DD_STATUS) and then the standard-specific status functions
  Porting:    Remove the un-necessary functions calls, if any. (Checking the TPS status may not be required)

  Parameter:  Pointer to Si2168B Context
  Returns:    1 if the current modulation is valid, 0 otherwise
************************************************************************************************************************/
int Si2168B_L1_Status            (L1_Si2168B_Context *api)
{
    /* Call the demod global status function */
    Si2168B_L1_DD_STATUS (api, Si2168B_DD_STATUS_CMD_INTACK_OK);

    /* Call the standard-specific status function */
    switch (api->rsp->dd_status.modulation)
    {
      case Si2168B_DD_STATUS_RESPONSE_MODULATION_DVBT : { Si2168B_L1_DVBT_STATUS  (api, Si2168B_DVBT_STATUS_CMD_INTACK_OK );
        break;
      }

      case Si2168B_DD_STATUS_RESPONSE_MODULATION_DVBT2: { Si2168B_L1_DVBT2_STATUS (api, Si2168B_DVBT2_STATUS_CMD_INTACK_OK);
        break;
      }

      case Si2168B_DD_STATUS_RESPONSE_MODULATION_DVBC : { Si2168B_L1_DVBC_STATUS  (api, Si2168B_DVBC_STATUS_CMD_INTACK_OK );
        break;
      }



      default                                  : { return 0; break; }
    }

    switch (api->rsp->dd_status.modulation)
    {
      case Si2168B_DD_STATUS_RESPONSE_MODULATION_DVBT2:


      default                                  : { Si2168B_L1_DD_PER (api, Si2168B_DD_PER_CMD_RST_RUN);
        break;
      }
    }
    switch (api->rsp->dd_status.modulation)
    {
      case Si2168B_DD_STATUS_RESPONSE_MODULATION_DVBT : {
                                                   Si2168B_L1_DVBT_TPS_EXTRA (api);
        break;
      }
      default                                  : { break; }
    }

 return 1;
}

