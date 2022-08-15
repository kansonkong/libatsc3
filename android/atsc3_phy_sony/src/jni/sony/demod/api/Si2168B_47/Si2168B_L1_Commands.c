/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168B Layer 1 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   API commands definitions
   FILE: Si2168B_L1_Commands.c
   Supported IC : Si2168B
   Compiled for ROM 0 firmware 3_A_build_3
   Revision: REVISION_NAME
   Tag:  V0.3.8_FW_4_4b17
   Date: May 22 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
/* Change log:

  As from V0.3.4:
   <improvement> [POWER_DOWN/fast I2C] In Si2168B_L1_POWER_DOWN: adding a 2 ms delay before issuing POWER_DOWN, to make sure that the FW 'main' function
     has completed at least 1 cycle, and applied any pending command.
   <improvement> [code_checkers] In Si2168B_L1_GetCommandResponseString: using snprintf and strncat with a max string size of 1000
                   This is considered safe by code checkers
                   CAUTION: the input string needs to be declared by the caller with a minimum size of 1000.
   <correction/Si2167B & Si2166B> [Freq offset] In Si2168B_L1_DVBS2_STATUS and Si2168B_L1_DVBS_STATUS:
                   Reading Si2167B register to report the correct afc_freq value.
                   Other parts will have this corrected by FW.

  As from V0.2.8:
   <code_checkers> In Si2168B_L1_DD_GET_REG / Si2168B_L1_DD_SSI_SQI / Si2168B_L1_DVBT2_PLP_INFO / Si2168B_L1_SET_PROPERTY
                      Si2168B_L1_DOWNLOAD_DATASET_CONTINUE: removing 'dead code' (when no field can be out of range)

  As from V0.2.5:
  <new feature> In Si2168B_pollForResponse:
    For debug purpose, read and trace 7 bytes in case the error bit is set

 As from V0.2.2:
  API change: new definition of DVB_C2_STATUS GUARD_INT field (3 bits, using '0' for 1/64, '5' for 1_128) to match other standards.
  In Si2168B_L1_DD_SET_REG/Si2168B_L1_DVBC2_CTRL/Si2168B_L1_SCAN_CTRL: changing '%d' to '%ld' for freq values
    in SiTRACE lines to avoid warnings.

 As from V0.0.0:
  Initial version (based on Si2169 code V2.6.1)


****************************************************************************************/
#define   Si2168B_COMMAND_PROTOTYPES
#define   DEBUG_RANGE_CHECK
#include "Si2168B_Platform_Definition.h"

//Edit by ITE
//snprintf -> sprintf 
//strncat -> strcat 

/***********************************************************************************************************************
  Si2168B_CurrentResponseStatus function
  Use:        status checking function
              Used to fill the Si2168B_COMMON_REPLY_struct members with the ptDataBuffer byte's bits
  Comments:   The status byte definition being identical for all commands,
              using this function to fill the status structure helps reducing the code size
  Parameter: ptDataBuffer  a single byte received when reading a command's response (the first byte)
  Returns:   0 if the err bit (bit 6) is unset, 1 otherwise
 ***********************************************************************************************************************/
unsigned char Si2168B_CurrentResponseStatus (L1_Si2168B_Context *api, unsigned char ptDataBuffer)
{
    api->status->ddint   = ((ptDataBuffer >> 0 ) & 0x01);
    api->status->scanint = ((ptDataBuffer >> 1 ) & 0x01);
    api->status->err     = ((ptDataBuffer >> 6 ) & 0x01);
    api->status->cts     = ((ptDataBuffer >> 7 ) & 0x01);
  return (api->status->err ? ERROR_Si2168B_ERR : NO_Si2168B_ERROR);
}

/***********************************************************************************************************************
  Si2168B_pollForCTS function
  Use:        CTS checking function
              Used to check the CTS bit until it is set before sending the next command
  Comments:   The status byte definition being identical for all commands,
              using this function to fill the status structure helps reducing the code size
              max timeout = 1000 ms

  Returns:   1 if the CTS bit is set, 0 otherwise
 ***********************************************************************************************************************/
unsigned char Si2168B_pollForCTS (L1_Si2168B_Context *api)
{
  unsigned char rspByteBuffer[1];
  unsigned int start_time;

  start_time = system_time();

  while (system_time() - start_time <1000)  { /* wait a maximum of 1000ms */
    if (L0_ReadCommandBytes(api->i2c, 1, rspByteBuffer) != 1) {
      SiTRACE("Si2168B_pollForCTS ERROR reading byte 0!\n");
      return ERROR_Si2168B_POLLING_CTS;
    }
    /* return OK if CTS set */
    if (rspByteBuffer[0] & 0x80) {
      return NO_Si2168B_ERROR;
    }
  }

  SiTRACE("Si2168B_pollForCTS ERROR CTS Timeout!\n");
  return ERROR_Si2168B_CTS_TIMEOUT;
}

/***********************************************************************************************************************
  Si2168B_pollForResponse function
  Use:        command response retrieval function
              Used to retrieve the command response in the provided buffer
  Comments:   The status byte definition being identical for all commands,
              using this function to fill the status structure helps reducing the code size
              max timeout = 1000 ms

  Parameter:  nbBytes          the number of response bytes to read
  Parameter:  pByteBuffer      a buffer into which bytes will be stored
  Returns:    0 if no error, an error code otherwise
 ***********************************************************************************************************************/
unsigned char Si2168B_pollForResponse (L1_Si2168B_Context *api, unsigned int nbBytes, unsigned char *pByteBuffer)
{
  unsigned char debugBuffer[7];
  unsigned int start_time;

  start_time = system_time();

  while (system_time() - start_time <1000)  { /* wait a maximum of 1000ms */
    if ((unsigned int)L0_ReadCommandBytes(api->i2c, nbBytes, pByteBuffer) != nbBytes) {
      SiTRACE("Si2168B_pollForResponse ERROR reading byte 0!\n");
      return ERROR_Si2168B_POLLING_RESPONSE;
    }
    /* return response err flag if CTS set */
    if (pByteBuffer[0] & 0x80)  {
      /* for debug purpose, read and trace 2 bytes in case the error bit is set */
      if (pByteBuffer[0] & 0x40)  {
        L0_ReadCommandBytes(api->i2c, 7, &(debugBuffer[0]) );
        SiTRACE("Si2168B debug bytes 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", debugBuffer[0], debugBuffer[1], debugBuffer[2], debugBuffer[3], debugBuffer[4], debugBuffer[5], debugBuffer[6]);
      }
      return Si2168B_CurrentResponseStatus(api, pByteBuffer[0]);
    }
  }

  SiTRACE("Si2168B_pollForResponse ERROR CTS Timeout!\n");
  return ERROR_Si2168B_CTS_TIMEOUT;
}

#ifdef    Si2168B_CONFIG_CLKIO_CMD
 /*---------------------------------------------------*/
/* Si2168B_CONFIG_CLKIO COMMAND                     */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_CONFIG_CLKIO              (L1_Si2168B_Context *api,
                                                   unsigned char   output,
                                                   unsigned char   pre_driver_str,
                                                   unsigned char   driver_str)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[4];
    api->rsp->config_clkio.STATUS = api->status;

    SiTRACE("Si2168B CONFIG_CLKIO ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((output         > Si2168B_CONFIG_CLKIO_CMD_OUTPUT_MAX        ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("OUTPUT %d "        , output         );
    if ((pre_driver_str > Si2168B_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("PRE_DRIVER_STR %d ", pre_driver_str );
    if ((driver_str     > Si2168B_CONFIG_CLKIO_CMD_DRIVER_STR_MAX    ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("DRIVER_STR %d "    , driver_str     );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_CONFIG_CLKIO_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( output         & Si2168B_CONFIG_CLKIO_CMD_OUTPUT_MASK         ) << Si2168B_CONFIG_CLKIO_CMD_OUTPUT_LSB        |
                                         ( pre_driver_str & Si2168B_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_MASK ) << Si2168B_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_LSB|
                                         ( driver_str     & Si2168B_CONFIG_CLKIO_CMD_DRIVER_STR_MASK     ) << Si2168B_CONFIG_CLKIO_CMD_DRIVER_STR_LSB    );

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing CONFIG_CLKIO bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 4, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling CONFIG_CLKIO response\n");
      return error_code;
    }

    api->rsp->config_clkio.mode           =   (( ( (rspByteBuffer[1]  )) >> Si2168B_CONFIG_CLKIO_RESPONSE_MODE_LSB           ) & Si2168B_CONFIG_CLKIO_RESPONSE_MODE_MASK           );
    api->rsp->config_clkio.pre_driver_str =   (( ( (rspByteBuffer[2]  )) >> Si2168B_CONFIG_CLKIO_RESPONSE_PRE_DRIVER_STR_LSB ) & Si2168B_CONFIG_CLKIO_RESPONSE_PRE_DRIVER_STR_MASK );
    api->rsp->config_clkio.driver_str     =   (( ( (rspByteBuffer[3]  )) >> Si2168B_CONFIG_CLKIO_RESPONSE_DRIVER_STR_LSB     ) & Si2168B_CONFIG_CLKIO_RESPONSE_DRIVER_STR_MASK     );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_CONFIG_CLKIO_CMD */
#ifdef    Si2168B_CONFIG_PINS_CMD
 /*---------------------------------------------------*/
/* Si2168B_CONFIG_PINS COMMAND                      */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_CONFIG_PINS               (L1_Si2168B_Context *api,
                                                   unsigned char   gpio0_mode,
                                                   unsigned char   gpio0_read,
                                                   unsigned char   gpio1_mode,
                                                   unsigned char   gpio1_read)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[3];
    unsigned char rspByteBuffer[3];
    api->rsp->config_pins.STATUS = api->status;

    SiTRACE("Si2168B CONFIG_PINS ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((gpio0_mode > Si2168B_CONFIG_PINS_CMD_GPIO0_MODE_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("GPIO0_MODE %d ", gpio0_mode );
    if ((gpio0_read > Si2168B_CONFIG_PINS_CMD_GPIO0_READ_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("GPIO0_READ %d ", gpio0_read );
    if ((gpio1_mode > Si2168B_CONFIG_PINS_CMD_GPIO1_MODE_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("GPIO1_MODE %d ", gpio1_mode );
    if ((gpio1_read > Si2168B_CONFIG_PINS_CMD_GPIO1_READ_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("GPIO1_READ %d ", gpio1_read );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_CONFIG_PINS_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( gpio0_mode & Si2168B_CONFIG_PINS_CMD_GPIO0_MODE_MASK ) << Si2168B_CONFIG_PINS_CMD_GPIO0_MODE_LSB|
                                         ( gpio0_read & Si2168B_CONFIG_PINS_CMD_GPIO0_READ_MASK ) << Si2168B_CONFIG_PINS_CMD_GPIO0_READ_LSB);
    cmdByteBuffer[2] = (unsigned char) ( ( gpio1_mode & Si2168B_CONFIG_PINS_CMD_GPIO1_MODE_MASK ) << Si2168B_CONFIG_PINS_CMD_GPIO1_MODE_LSB|
                                         ( gpio1_read & Si2168B_CONFIG_PINS_CMD_GPIO1_READ_MASK ) << Si2168B_CONFIG_PINS_CMD_GPIO1_READ_LSB);

    if (L0_WriteCommandBytes(api->i2c, 3, cmdByteBuffer) != 3) {
      SiTRACE("Error writing CONFIG_PINS bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 3, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling CONFIG_PINS response\n");
      return error_code;
    }

    api->rsp->config_pins.gpio0_mode  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_CONFIG_PINS_RESPONSE_GPIO0_MODE_LSB  ) & Si2168B_CONFIG_PINS_RESPONSE_GPIO0_MODE_MASK  );
    api->rsp->config_pins.gpio0_state =   (( ( (rspByteBuffer[1]  )) >> Si2168B_CONFIG_PINS_RESPONSE_GPIO0_STATE_LSB ) & Si2168B_CONFIG_PINS_RESPONSE_GPIO0_STATE_MASK );
    api->rsp->config_pins.gpio1_mode  =   (( ( (rspByteBuffer[2]  )) >> Si2168B_CONFIG_PINS_RESPONSE_GPIO1_MODE_LSB  ) & Si2168B_CONFIG_PINS_RESPONSE_GPIO1_MODE_MASK  );
    api->rsp->config_pins.gpio1_state =   (( ( (rspByteBuffer[2]  )) >> Si2168B_CONFIG_PINS_RESPONSE_GPIO1_STATE_LSB ) & Si2168B_CONFIG_PINS_RESPONSE_GPIO1_STATE_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_CONFIG_PINS_CMD */
#ifdef    Si2168B_DD_BER_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_BER COMMAND                           */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_BER                    (L1_Si2168B_Context *api,
                                                   unsigned char   rst)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[3];
    api->rsp->dd_ber.STATUS = api->status;

    SiTRACE("Si2168B DD_BER ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((rst > Si2168B_DD_BER_CMD_RST_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RST %d ", rst );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_BER_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( rst & Si2168B_DD_BER_CMD_RST_MASK ) << Si2168B_DD_BER_CMD_RST_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DD_BER bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 3, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_BER response\n");
      return error_code;
    }

    api->rsp->dd_ber.exp  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_BER_RESPONSE_EXP_LSB  ) & Si2168B_DD_BER_RESPONSE_EXP_MASK  );
    api->rsp->dd_ber.mant =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_BER_RESPONSE_MANT_LSB ) & Si2168B_DD_BER_RESPONSE_MANT_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_BER_CMD */
#ifdef    Si2168B_DD_CBER_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_CBER COMMAND                          */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_CBER                   (L1_Si2168B_Context *api,
                                                   unsigned char   rst)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[3];
    api->rsp->dd_cber.STATUS = api->status;

    SiTRACE("Si2168B DD_CBER ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((rst > Si2168B_DD_CBER_CMD_RST_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RST %d ", rst );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_CBER_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( rst & Si2168B_DD_CBER_CMD_RST_MASK ) << Si2168B_DD_CBER_CMD_RST_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DD_CBER bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 3, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_CBER response\n");
      return error_code;
    }

    api->rsp->dd_cber.exp  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_CBER_RESPONSE_EXP_LSB  ) & Si2168B_DD_CBER_RESPONSE_EXP_MASK  );
    api->rsp->dd_cber.mant =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_CBER_RESPONSE_MANT_LSB ) & Si2168B_DD_CBER_RESPONSE_MANT_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_CBER_CMD */

#ifdef    Si2168B_DD_EXT_AGC_TER_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_EXT_AGC_TER COMMAND                   */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_EXT_AGC_TER            (L1_Si2168B_Context *api,
                                                   unsigned char   agc_1_mode,
                                                   unsigned char   agc_1_inv,
                                                   unsigned char   agc_2_mode,
                                                   unsigned char   agc_2_inv,
                                                   unsigned char   agc_1_kloop,
                                                   unsigned char   agc_2_kloop,
                                                   unsigned char   agc_1_min,
                                                   unsigned char   agc_2_min)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[6];
    unsigned char rspByteBuffer[3];
    api->rsp->dd_ext_agc_ter.STATUS = api->status;

    SiTRACE("Si2168B DD_EXT_AGC_TER ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((agc_1_mode  > Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("AGC_1_MODE %d " , agc_1_mode  );
    if ((agc_1_inv   > Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_INV_MAX  ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("AGC_1_INV %d "  , agc_1_inv   );
    if ((agc_2_mode  > Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("AGC_2_MODE %d " , agc_2_mode  );
    if ((agc_2_inv   > Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_INV_MAX  ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("AGC_2_INV %d "  , agc_2_inv   );
    if ((agc_1_kloop > Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_MAX)  || (agc_1_kloop < Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_MIN) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("AGC_1_KLOOP %d ", agc_1_kloop );
    if ((agc_2_kloop > Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_MAX)  || (agc_2_kloop < Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_MIN) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("AGC_2_KLOOP %d ", agc_2_kloop );
    SiTRACE("AGC_1_MIN %d "  , agc_1_min   );
    SiTRACE("AGC_2_MIN %d "  , agc_2_min   );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_EXT_AGC_TER_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( agc_1_mode  & Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MASK  ) << Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MODE_LSB |
                                         ( agc_1_inv   & Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_INV_MASK   ) << Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_INV_LSB  |
                                         ( agc_2_mode  & Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MASK  ) << Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MODE_LSB |
                                         ( agc_2_inv   & Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_INV_MASK   ) << Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_INV_LSB  );
    cmdByteBuffer[2] = (unsigned char) ( ( agc_1_kloop & Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_MASK ) << Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_LSB);
    cmdByteBuffer[3] = (unsigned char) ( ( agc_2_kloop & Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_MASK ) << Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_LSB);
    cmdByteBuffer[4] = (unsigned char) ( ( agc_1_min   & Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MIN_MASK   ) << Si2168B_DD_EXT_AGC_TER_CMD_AGC_1_MIN_LSB  );
    cmdByteBuffer[5] = (unsigned char) ( ( agc_2_min   & Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MIN_MASK   ) << Si2168B_DD_EXT_AGC_TER_CMD_AGC_2_MIN_LSB  );

    if (L0_WriteCommandBytes(api->i2c, 6, cmdByteBuffer) != 6) {
      SiTRACE("Error writing DD_EXT_AGC_TER bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 3, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_EXT_AGC_TER response\n");
      return error_code;
    }

    api->rsp->dd_ext_agc_ter.agc_1_level =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_EXT_AGC_TER_RESPONSE_AGC_1_LEVEL_LSB ) & Si2168B_DD_EXT_AGC_TER_RESPONSE_AGC_1_LEVEL_MASK );
    api->rsp->dd_ext_agc_ter.agc_2_level =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_EXT_AGC_TER_RESPONSE_AGC_2_LEVEL_LSB ) & Si2168B_DD_EXT_AGC_TER_RESPONSE_AGC_2_LEVEL_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_EXT_AGC_TER_CMD */

#ifdef    Si2168B_DD_FER_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_FER COMMAND                           */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_FER                    (L1_Si2168B_Context *api,
                                                   unsigned char   rst)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[3];
    api->rsp->dd_fer.STATUS = api->status;

    SiTRACE("Si2168B DD_FER ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((rst > Si2168B_DD_FER_CMD_RST_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RST %d ", rst );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_FER_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( rst & Si2168B_DD_FER_CMD_RST_MASK ) << Si2168B_DD_FER_CMD_RST_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DD_FER bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 3, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_FER response\n");
      return error_code;
    }

    api->rsp->dd_fer.exp  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_FER_RESPONSE_EXP_LSB  ) & Si2168B_DD_FER_RESPONSE_EXP_MASK  );
    api->rsp->dd_fer.mant =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_FER_RESPONSE_MANT_LSB ) & Si2168B_DD_FER_RESPONSE_MANT_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_FER_CMD */
#ifdef    Si2168B_DD_GET_REG_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_GET_REG COMMAND                       */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_GET_REG                (L1_Si2168B_Context *api,
                                                   unsigned char   reg_code_lsb,
                                                   unsigned char   reg_code_mid,
                                                   unsigned char   reg_code_msb)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[4];
    unsigned char rspByteBuffer[5];
    api->rsp->dd_get_reg.STATUS = api->status;

    SiTRACE("Si2168B DD_GET_REG ");
  #ifdef   DEBUG_RANGE_CHECK
    SiTRACE("REG_CODE_LSB %d ", reg_code_lsb );
    SiTRACE("REG_CODE_MID %d ", reg_code_mid );
    SiTRACE("REG_CODE_MSB %d ", reg_code_msb );
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_GET_REG_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( reg_code_lsb & Si2168B_DD_GET_REG_CMD_REG_CODE_LSB_MASK ) << Si2168B_DD_GET_REG_CMD_REG_CODE_LSB_LSB);
    cmdByteBuffer[2] = (unsigned char) ( ( reg_code_mid & Si2168B_DD_GET_REG_CMD_REG_CODE_MID_MASK ) << Si2168B_DD_GET_REG_CMD_REG_CODE_MID_LSB);
    cmdByteBuffer[3] = (unsigned char) ( ( reg_code_msb & Si2168B_DD_GET_REG_CMD_REG_CODE_MSB_MASK ) << Si2168B_DD_GET_REG_CMD_REG_CODE_MSB_LSB);

    if (L0_WriteCommandBytes(api->i2c, 4, cmdByteBuffer) != 4) {
      SiTRACE("Error writing DD_GET_REG bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 5, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_GET_REG response\n");
      return error_code;
    }

    api->rsp->dd_get_reg.data1 =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_GET_REG_RESPONSE_DATA1_LSB ) & Si2168B_DD_GET_REG_RESPONSE_DATA1_MASK );
    api->rsp->dd_get_reg.data2 =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_GET_REG_RESPONSE_DATA2_LSB ) & Si2168B_DD_GET_REG_RESPONSE_DATA2_MASK );
    api->rsp->dd_get_reg.data3 =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DD_GET_REG_RESPONSE_DATA3_LSB ) & Si2168B_DD_GET_REG_RESPONSE_DATA3_MASK );
    api->rsp->dd_get_reg.data4 =   (( ( (rspByteBuffer[4]  )) >> Si2168B_DD_GET_REG_RESPONSE_DATA4_LSB ) & Si2168B_DD_GET_REG_RESPONSE_DATA4_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_GET_REG_CMD */
#ifdef    Si2168B_DD_MP_DEFAULTS_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_MP_DEFAULTS COMMAND                   */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_MP_DEFAULTS            (L1_Si2168B_Context *api,
                                                   unsigned char   mp_a_mode,
                                                   unsigned char   mp_b_mode,
                                                   unsigned char   mp_c_mode,
                                                   unsigned char   mp_d_mode)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[5];
    unsigned char rspByteBuffer[5];
    api->rsp->dd_mp_defaults.STATUS = api->status;

    SiTRACE("Si2168B DD_MP_DEFAULTS ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((mp_a_mode > Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("MP_A_MODE %d ", mp_a_mode );
    if ((mp_b_mode > Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("MP_B_MODE %d ", mp_b_mode );
    if ((mp_c_mode > Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("MP_C_MODE %d ", mp_c_mode );
    if ((mp_d_mode > Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("MP_D_MODE %d ", mp_d_mode );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_MP_DEFAULTS_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( mp_a_mode & Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_MASK ) << Si2168B_DD_MP_DEFAULTS_CMD_MP_A_MODE_LSB);
    cmdByteBuffer[2] = (unsigned char) ( ( mp_b_mode & Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_MASK ) << Si2168B_DD_MP_DEFAULTS_CMD_MP_B_MODE_LSB);
    cmdByteBuffer[3] = (unsigned char) ( ( mp_c_mode & Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_MASK ) << Si2168B_DD_MP_DEFAULTS_CMD_MP_C_MODE_LSB);
    cmdByteBuffer[4] = (unsigned char) ( ( mp_d_mode & Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_MASK ) << Si2168B_DD_MP_DEFAULTS_CMD_MP_D_MODE_LSB);

    if (L0_WriteCommandBytes(api->i2c, 5, cmdByteBuffer) != 5) {
      SiTRACE("Error writing DD_MP_DEFAULTS bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 5, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_MP_DEFAULTS response\n");
      return error_code;
    }

    api->rsp->dd_mp_defaults.mp_a_mode =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_LSB ) & Si2168B_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_MASK );
    api->rsp->dd_mp_defaults.mp_b_mode =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_LSB ) & Si2168B_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_MASK );
    api->rsp->dd_mp_defaults.mp_c_mode =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_LSB ) & Si2168B_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_MASK );
    api->rsp->dd_mp_defaults.mp_d_mode =   (( ( (rspByteBuffer[4]  )) >> Si2168B_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_LSB ) & Si2168B_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_MP_DEFAULTS_CMD */
#ifdef    Si2168B_DD_PER_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_PER COMMAND                           */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_PER                    (L1_Si2168B_Context *api,
                                                   unsigned char   rst)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[3];
    api->rsp->dd_per.STATUS = api->status;

    SiTRACE("Si2168B DD_PER ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((rst > Si2168B_DD_PER_CMD_RST_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RST %d ", rst );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_PER_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( rst & Si2168B_DD_PER_CMD_RST_MASK ) << Si2168B_DD_PER_CMD_RST_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DD_PER bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 3, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_PER response\n");
      return error_code;
    }

    api->rsp->dd_per.exp  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_PER_RESPONSE_EXP_LSB  ) & Si2168B_DD_PER_RESPONSE_EXP_MASK  );
    api->rsp->dd_per.mant =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_PER_RESPONSE_MANT_LSB ) & Si2168B_DD_PER_RESPONSE_MANT_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_PER_CMD */
#ifdef    Si2168B_DD_RESTART_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_RESTART COMMAND                       */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_RESTART                (L1_Si2168B_Context *api)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[1];
    unsigned char rspByteBuffer[1];
    api->rsp->dd_restart.STATUS = api->status;

    SiTRACE("Si2168B DD_RESTART ");
    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_RESTART_CMD;

    if (L0_WriteCommandBytes(api->i2c, 1, cmdByteBuffer) != 1) {
      SiTRACE("Error writing DD_RESTART bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 1, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_RESTART response\n");
      return error_code;
    }


    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_RESTART_CMD */
#ifdef    Si2168B_DD_SET_REG_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_SET_REG COMMAND                       */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_SET_REG                (L1_Si2168B_Context *api,
                                                   unsigned char   reg_code_lsb,
                                                   unsigned char   reg_code_mid,
                                                   unsigned char   reg_code_msb,
                                                   unsigned long   value)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[8];
    unsigned char rspByteBuffer[1];
    api->rsp->dd_set_reg.STATUS = api->status;

    SiTRACE("Si2168B DD_SET_REG ");
  #ifdef   DEBUG_RANGE_CHECK
    SiTRACE("REG_CODE_LSB %d ", reg_code_lsb );
    SiTRACE("REG_CODE_MID %d ", reg_code_mid );
    SiTRACE("REG_CODE_MSB %d ", reg_code_msb );
    SiTRACE("VALUE %ld "      , value        );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_SET_REG_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( reg_code_lsb & Si2168B_DD_SET_REG_CMD_REG_CODE_LSB_MASK ) << Si2168B_DD_SET_REG_CMD_REG_CODE_LSB_LSB);
    cmdByteBuffer[2] = (unsigned char) ( ( reg_code_mid & Si2168B_DD_SET_REG_CMD_REG_CODE_MID_MASK ) << Si2168B_DD_SET_REG_CMD_REG_CODE_MID_LSB);
    cmdByteBuffer[3] = (unsigned char) ( ( reg_code_msb & Si2168B_DD_SET_REG_CMD_REG_CODE_MSB_MASK ) << Si2168B_DD_SET_REG_CMD_REG_CODE_MSB_LSB);
    cmdByteBuffer[4] = (unsigned char) ( ( value        & Si2168B_DD_SET_REG_CMD_VALUE_MASK        ) << Si2168B_DD_SET_REG_CMD_VALUE_LSB       );
    cmdByteBuffer[5] = (unsigned char) ((( value        & Si2168B_DD_SET_REG_CMD_VALUE_MASK        ) << Si2168B_DD_SET_REG_CMD_VALUE_LSB       )>>8);
    cmdByteBuffer[6] = (unsigned char) ((( value        & Si2168B_DD_SET_REG_CMD_VALUE_MASK        ) << Si2168B_DD_SET_REG_CMD_VALUE_LSB       )>>16);
    cmdByteBuffer[7] = (unsigned char) ((( value        & Si2168B_DD_SET_REG_CMD_VALUE_MASK        ) << Si2168B_DD_SET_REG_CMD_VALUE_LSB       )>>24);

    if (L0_WriteCommandBytes(api->i2c, 8, cmdByteBuffer) != 8) {
      SiTRACE("Error writing DD_SET_REG bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 1, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_SET_REG response\n");
      return error_code;
    }


    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_SET_REG_CMD */
#ifdef    Si2168B_DD_SSI_SQI_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_SSI_SQI COMMAND                       */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_SSI_SQI                (L1_Si2168B_Context *api,
                                                             char  tuner_rssi)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[3];
    api->rsp->dd_ssi_sqi.STATUS = api->status;

    SiTRACE("Si2168B DD_SSI_SQI ");
  #ifdef   DEBUG_RANGE_CHECK
    SiTRACE("TUNER_RSSI %d ", tuner_rssi );
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_SSI_SQI_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( tuner_rssi & Si2168B_DD_SSI_SQI_CMD_TUNER_RSSI_MASK ) << Si2168B_DD_SSI_SQI_CMD_TUNER_RSSI_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DD_SSI_SQI bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 3, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_SSI_SQI response\n");
      return error_code;
    }

    api->rsp->dd_ssi_sqi.ssi =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_SSI_SQI_RESPONSE_SSI_LSB ) & Si2168B_DD_SSI_SQI_RESPONSE_SSI_MASK );
    api->rsp->dd_ssi_sqi.sqi = (((( ( (rspByteBuffer[2]  )) >> Si2168B_DD_SSI_SQI_RESPONSE_SQI_LSB ) & Si2168B_DD_SSI_SQI_RESPONSE_SQI_MASK) <<Si2168B_DD_SSI_SQI_RESPONSE_SQI_SHIFT ) >>Si2168B_DD_SSI_SQI_RESPONSE_SQI_SHIFT );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_SSI_SQI_CMD */
#ifdef    Si2168B_DD_STATUS_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_STATUS COMMAND                        */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_STATUS                 (L1_Si2168B_Context *api,
                                                   unsigned char   intack)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[8];
    api->rsp->dd_status.STATUS = api->status;

    SiTRACE("Si2168B DD_STATUS ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((intack > Si2168B_DD_STATUS_CMD_INTACK_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("INTACK %d ", intack );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_STATUS_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( intack & Si2168B_DD_STATUS_CMD_INTACK_MASK ) << Si2168B_DD_STATUS_CMD_INTACK_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DD_STATUS bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 8, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_STATUS response\n");
      return error_code;
    }

    api->rsp->dd_status.pclint       =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_STATUS_RESPONSE_PCLINT_LSB       ) & Si2168B_DD_STATUS_RESPONSE_PCLINT_MASK       );
    api->rsp->dd_status.dlint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_STATUS_RESPONSE_DLINT_LSB        ) & Si2168B_DD_STATUS_RESPONSE_DLINT_MASK        );
    api->rsp->dd_status.berint       =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_STATUS_RESPONSE_BERINT_LSB       ) & Si2168B_DD_STATUS_RESPONSE_BERINT_MASK       );
    api->rsp->dd_status.uncorint     =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_STATUS_RESPONSE_UNCORINT_LSB     ) & Si2168B_DD_STATUS_RESPONSE_UNCORINT_MASK     );
    api->rsp->dd_status.rsqint_bit5  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT5_LSB  ) & Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT5_MASK  );
    api->rsp->dd_status.rsqint_bit6  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT6_LSB  ) & Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT6_MASK  );
    api->rsp->dd_status.rsqint_bit7  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT7_LSB  ) & Si2168B_DD_STATUS_RESPONSE_RSQINT_BIT7_MASK  );
    api->rsp->dd_status.pcl          =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_STATUS_RESPONSE_PCL_LSB          ) & Si2168B_DD_STATUS_RESPONSE_PCL_MASK          );
    api->rsp->dd_status.dl           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_STATUS_RESPONSE_DL_LSB           ) & Si2168B_DD_STATUS_RESPONSE_DL_MASK           );
    api->rsp->dd_status.ber          =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_STATUS_RESPONSE_BER_LSB          ) & Si2168B_DD_STATUS_RESPONSE_BER_MASK          );
    api->rsp->dd_status.uncor        =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_STATUS_RESPONSE_UNCOR_LSB        ) & Si2168B_DD_STATUS_RESPONSE_UNCOR_MASK        );
    api->rsp->dd_status.rsqstat_bit5 =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_STATUS_RESPONSE_RSQSTAT_BIT5_LSB ) & Si2168B_DD_STATUS_RESPONSE_RSQSTAT_BIT5_MASK );
    api->rsp->dd_status.rsqstat_bit6 =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_STATUS_RESPONSE_RSQSTAT_BIT6_LSB ) & Si2168B_DD_STATUS_RESPONSE_RSQSTAT_BIT6_MASK );
    api->rsp->dd_status.rsqstat_bit7 =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_STATUS_RESPONSE_RSQSTAT_BIT7_LSB ) & Si2168B_DD_STATUS_RESPONSE_RSQSTAT_BIT7_MASK );
    api->rsp->dd_status.modulation   =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DD_STATUS_RESPONSE_MODULATION_LSB   ) & Si2168B_DD_STATUS_RESPONSE_MODULATION_MASK   );
    api->rsp->dd_status.ts_bit_rate  =   (( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 )) >> Si2168B_DD_STATUS_RESPONSE_TS_BIT_RATE_LSB  ) & Si2168B_DD_STATUS_RESPONSE_TS_BIT_RATE_MASK  );
    api->rsp->dd_status.ts_clk_freq  =   (( ( (rspByteBuffer[6]  ) | (rspByteBuffer[7]  << 8 )) >> Si2168B_DD_STATUS_RESPONSE_TS_CLK_FREQ_LSB  ) & Si2168B_DD_STATUS_RESPONSE_TS_CLK_FREQ_MASK  );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_STATUS_CMD */
#ifdef    Si2168B_DD_UNCOR_CMD
 /*---------------------------------------------------*/
/* Si2168B_DD_UNCOR COMMAND                         */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DD_UNCOR                  (L1_Si2168B_Context *api,
                                                   unsigned char   rst)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[3];
    api->rsp->dd_uncor.STATUS = api->status;

    SiTRACE("Si2168B DD_UNCOR ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((rst > Si2168B_DD_UNCOR_CMD_RST_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RST %d ", rst );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DD_UNCOR_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( rst & Si2168B_DD_UNCOR_CMD_RST_MASK ) << Si2168B_DD_UNCOR_CMD_RST_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DD_UNCOR bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 3, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DD_UNCOR response\n");
      return error_code;
    }

    api->rsp->dd_uncor.uncor_lsb =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DD_UNCOR_RESPONSE_UNCOR_LSB_LSB ) & Si2168B_DD_UNCOR_RESPONSE_UNCOR_LSB_MASK );
    api->rsp->dd_uncor.uncor_msb =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DD_UNCOR_RESPONSE_UNCOR_MSB_LSB ) & Si2168B_DD_UNCOR_RESPONSE_UNCOR_MSB_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DD_UNCOR_CMD */
#ifdef    Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD
 /*---------------------------------------------------*/
/* Si2168B_DOWNLOAD_DATASET_CONTINUE COMMAND        */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DOWNLOAD_DATASET_CONTINUE (L1_Si2168B_Context *api,
                                                   unsigned char   data0,
                                                   unsigned char   data1,
                                                   unsigned char   data2,
                                                   unsigned char   data3,
                                                   unsigned char   data4,
                                                   unsigned char   data5,
                                                   unsigned char   data6)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[8];
    unsigned char rspByteBuffer[1];
    api->rsp->download_dataset_continue.STATUS = api->status;

    SiTRACE("Si2168B DOWNLOAD_DATASET_CONTINUE ");
  #ifdef   DEBUG_RANGE_CHECK
    SiTRACE("DATA0 %d ", data0 );
    SiTRACE("DATA1 %d ", data1 );
    SiTRACE("DATA2 %d ", data2 );
    SiTRACE("DATA3 %d ", data3 );
    SiTRACE("DATA4 %d ", data4 );
    SiTRACE("DATA5 %d ", data5 );
    SiTRACE("DATA6 %d ", data6 );
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( data0 & Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA0_MASK ) << Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA0_LSB);
    cmdByteBuffer[2] = (unsigned char) ( ( data1 & Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA1_MASK ) << Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA1_LSB);
    cmdByteBuffer[3] = (unsigned char) ( ( data2 & Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA2_MASK ) << Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA2_LSB);
    cmdByteBuffer[4] = (unsigned char) ( ( data3 & Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA3_MASK ) << Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA3_LSB);
    cmdByteBuffer[5] = (unsigned char) ( ( data4 & Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA4_MASK ) << Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA4_LSB);
    cmdByteBuffer[6] = (unsigned char) ( ( data5 & Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA5_MASK ) << Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA5_LSB);
    cmdByteBuffer[7] = (unsigned char) ( ( data6 & Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA6_MASK ) << Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_DATA6_LSB);

    if (L0_WriteCommandBytes(api->i2c, 8, cmdByteBuffer) != 8) {
      SiTRACE("Error writing DOWNLOAD_DATASET_CONTINUE bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 1, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DOWNLOAD_DATASET_CONTINUE response\n");
      return error_code;
    }


    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD */
#ifdef    Si2168B_DOWNLOAD_DATASET_START_CMD
 /*---------------------------------------------------*/
/* Si2168B_DOWNLOAD_DATASET_START COMMAND           */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DOWNLOAD_DATASET_START    (L1_Si2168B_Context *api,
                                                   unsigned char   dataset_id,
                                                   unsigned char   dataset_checksum,
                                                   unsigned char   data0,
                                                   unsigned char   data1,
                                                   unsigned char   data2,
                                                   unsigned char   data3,
                                                   unsigned char   data4)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[8];
    unsigned char rspByteBuffer[1];
    api->rsp->download_dataset_start.STATUS = api->status;

    SiTRACE("Si2168B DOWNLOAD_DATASET_START ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((dataset_id       > Si2168B_DOWNLOAD_DATASET_START_CMD_DATASET_ID_MAX      ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("DATASET_ID %d "      , dataset_id       );
    SiTRACE("DATASET_CHECKSUM %d ", dataset_checksum );
    SiTRACE("DATA0 %d "           , data0            );
    SiTRACE("DATA1 %d "           , data1            );
    SiTRACE("DATA2 %d "           , data2            );
    SiTRACE("DATA3 %d "           , data3            );
    SiTRACE("DATA4 %d "           , data4            );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DOWNLOAD_DATASET_START_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( dataset_id       & Si2168B_DOWNLOAD_DATASET_START_CMD_DATASET_ID_MASK       ) << Si2168B_DOWNLOAD_DATASET_START_CMD_DATASET_ID_LSB      );
    cmdByteBuffer[2] = (unsigned char) ( ( dataset_checksum & Si2168B_DOWNLOAD_DATASET_START_CMD_DATASET_CHECKSUM_MASK ) << Si2168B_DOWNLOAD_DATASET_START_CMD_DATASET_CHECKSUM_LSB);
    cmdByteBuffer[3] = (unsigned char) ( ( data0            & Si2168B_DOWNLOAD_DATASET_START_CMD_DATA0_MASK            ) << Si2168B_DOWNLOAD_DATASET_START_CMD_DATA0_LSB           );
    cmdByteBuffer[4] = (unsigned char) ( ( data1            & Si2168B_DOWNLOAD_DATASET_START_CMD_DATA1_MASK            ) << Si2168B_DOWNLOAD_DATASET_START_CMD_DATA1_LSB           );
    cmdByteBuffer[5] = (unsigned char) ( ( data2            & Si2168B_DOWNLOAD_DATASET_START_CMD_DATA2_MASK            ) << Si2168B_DOWNLOAD_DATASET_START_CMD_DATA2_LSB           );
    cmdByteBuffer[6] = (unsigned char) ( ( data3            & Si2168B_DOWNLOAD_DATASET_START_CMD_DATA3_MASK            ) << Si2168B_DOWNLOAD_DATASET_START_CMD_DATA3_LSB           );
    cmdByteBuffer[7] = (unsigned char) ( ( data4            & Si2168B_DOWNLOAD_DATASET_START_CMD_DATA4_MASK            ) << Si2168B_DOWNLOAD_DATASET_START_CMD_DATA4_LSB           );

    if (L0_WriteCommandBytes(api->i2c, 8, cmdByteBuffer) != 8) {
      SiTRACE("Error writing DOWNLOAD_DATASET_START bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 1, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DOWNLOAD_DATASET_START response\n");
      return error_code;
    }


    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DOWNLOAD_DATASET_START_CMD */

#ifdef    Si2168B_DVBC_STATUS_CMD
 /*---------------------------------------------------*/
/* Si2168B_DVBC_STATUS COMMAND                      */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DVBC_STATUS               (L1_Si2168B_Context *api,
                                                   unsigned char   intack)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[9];
    api->rsp->dvbc_status.STATUS = api->status;

    SiTRACE("Si2168B DVBC_STATUS ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((intack > Si2168B_DVBC_STATUS_CMD_INTACK_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("INTACK %d ", intack );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DVBC_STATUS_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( intack & Si2168B_DVBC_STATUS_CMD_INTACK_MASK ) << Si2168B_DVBC_STATUS_CMD_INTACK_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DVBC_STATUS bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 9, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DVBC_STATUS response\n");
      return error_code;
    }

    api->rsp->dvbc_status.pclint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBC_STATUS_RESPONSE_PCLINT_LSB        ) & Si2168B_DVBC_STATUS_RESPONSE_PCLINT_MASK        );
    api->rsp->dvbc_status.dlint         =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBC_STATUS_RESPONSE_DLINT_LSB         ) & Si2168B_DVBC_STATUS_RESPONSE_DLINT_MASK         );
    api->rsp->dvbc_status.berint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBC_STATUS_RESPONSE_BERINT_LSB        ) & Si2168B_DVBC_STATUS_RESPONSE_BERINT_MASK        );
    api->rsp->dvbc_status.uncorint      =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBC_STATUS_RESPONSE_UNCORINT_LSB      ) & Si2168B_DVBC_STATUS_RESPONSE_UNCORINT_MASK      );
    api->rsp->dvbc_status.pcl           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBC_STATUS_RESPONSE_PCL_LSB           ) & Si2168B_DVBC_STATUS_RESPONSE_PCL_MASK           );
    api->rsp->dvbc_status.dl            =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBC_STATUS_RESPONSE_DL_LSB            ) & Si2168B_DVBC_STATUS_RESPONSE_DL_MASK            );
    api->rsp->dvbc_status.ber           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBC_STATUS_RESPONSE_BER_LSB           ) & Si2168B_DVBC_STATUS_RESPONSE_BER_MASK           );
    api->rsp->dvbc_status.uncor         =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBC_STATUS_RESPONSE_UNCOR_LSB         ) & Si2168B_DVBC_STATUS_RESPONSE_UNCOR_MASK         );
    api->rsp->dvbc_status.cnr           =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DVBC_STATUS_RESPONSE_CNR_LSB           ) & Si2168B_DVBC_STATUS_RESPONSE_CNR_MASK           );
    api->rsp->dvbc_status.afc_freq      = (((( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 )) >> Si2168B_DVBC_STATUS_RESPONSE_AFC_FREQ_LSB      ) & Si2168B_DVBC_STATUS_RESPONSE_AFC_FREQ_MASK) <<Si2168B_DVBC_STATUS_RESPONSE_AFC_FREQ_SHIFT ) >>Si2168B_DVBC_STATUS_RESPONSE_AFC_FREQ_SHIFT      );
    api->rsp->dvbc_status.timing_offset = (((( ( (rspByteBuffer[6]  ) | (rspByteBuffer[7]  << 8 )) >> Si2168B_DVBC_STATUS_RESPONSE_TIMING_OFFSET_LSB ) & Si2168B_DVBC_STATUS_RESPONSE_TIMING_OFFSET_MASK) <<Si2168B_DVBC_STATUS_RESPONSE_TIMING_OFFSET_SHIFT ) >>Si2168B_DVBC_STATUS_RESPONSE_TIMING_OFFSET_SHIFT );
    api->rsp->dvbc_status.constellation =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBC_STATUS_RESPONSE_CONSTELLATION_LSB ) & Si2168B_DVBC_STATUS_RESPONSE_CONSTELLATION_MASK );
    api->rsp->dvbc_status.sp_inv        =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBC_STATUS_RESPONSE_SP_INV_LSB        ) & Si2168B_DVBC_STATUS_RESPONSE_SP_INV_MASK        );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DVBC_STATUS_CMD */


#ifdef    Si2168B_DVBT2_FEF_CMD
 /*---------------------------------------------------*/
/* Si2168B_DVBT2_FEF COMMAND                        */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DVBT2_FEF                 (L1_Si2168B_Context *api,
                                                   unsigned char   fef_tuner_flag,
                                                   unsigned char   fef_tuner_flag_inv)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[12];
    api->rsp->dvbt2_fef.STATUS = api->status;

    SiTRACE("Si2168B DVBT2_FEF ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((fef_tuner_flag     > Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MAX    ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("FEF_TUNER_FLAG %d "    , fef_tuner_flag     );
    if ((fef_tuner_flag_inv > Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("FEF_TUNER_FLAG_INV %d ", fef_tuner_flag_inv );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DVBT2_FEF_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( fef_tuner_flag     & Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MASK     ) << Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_LSB    |
                                         ( fef_tuner_flag_inv & Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_MASK ) << Si2168B_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DVBT2_FEF bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 12, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DVBT2_FEF response\n");
      return error_code;
    }

    api->rsp->dvbt2_fef.fef_type       =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT2_FEF_RESPONSE_FEF_TYPE_LSB       ) & Si2168B_DVBT2_FEF_RESPONSE_FEF_TYPE_MASK       );
    api->rsp->dvbt2_fef.fef_length     =   (( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 ) | (rspByteBuffer[6]  << 16 ) | (rspByteBuffer[7]  << 24 )) >> Si2168B_DVBT2_FEF_RESPONSE_FEF_LENGTH_LSB     ) & Si2168B_DVBT2_FEF_RESPONSE_FEF_LENGTH_MASK     );
    api->rsp->dvbt2_fef.fef_repetition =   (( ( (rspByteBuffer[8]  ) | (rspByteBuffer[9]  << 8 ) | (rspByteBuffer[10] << 16 ) | (rspByteBuffer[11] << 24 )) >> Si2168B_DVBT2_FEF_RESPONSE_FEF_REPETITION_LSB ) & Si2168B_DVBT2_FEF_RESPONSE_FEF_REPETITION_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DVBT2_FEF_CMD */
#ifdef    Si2168B_DVBT2_PLP_INFO_CMD
 /*---------------------------------------------------*/
/* Si2168B_DVBT2_PLP_INFO COMMAND                   */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DVBT2_PLP_INFO            (L1_Si2168B_Context *api,
                                                   unsigned char   plp_index)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[13];
    api->rsp->dvbt2_plp_info.STATUS = api->status;

    SiTRACE("Si2168B DVBT2_PLP_INFO ");
  #ifdef   DEBUG_RANGE_CHECK
    SiTRACE("PLP_INDEX %d ", plp_index );
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DVBT2_PLP_INFO_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( plp_index & Si2168B_DVBT2_PLP_INFO_CMD_PLP_INDEX_MASK ) << Si2168B_DVBT2_PLP_INFO_CMD_PLP_INDEX_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DVBT2_PLP_INFO bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 13, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DVBT2_PLP_INFO response\n");
      return error_code;
    }

    api->rsp->dvbt2_plp_info.plp_id                 =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_ID_LSB                 ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_ID_MASK                 );
    api->rsp->dvbt2_plp_info.plp_payload_type       =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_PAYLOAD_TYPE_LSB       ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_PAYLOAD_TYPE_MASK       );
    api->rsp->dvbt2_plp_info.plp_type               =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_LSB               ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_MASK               );
    api->rsp->dvbt2_plp_info.first_frame_idx_msb    =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_FIRST_FRAME_IDX_MSB_LSB    ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_FIRST_FRAME_IDX_MSB_MASK    );
    api->rsp->dvbt2_plp_info.first_rf_idx           =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_FIRST_RF_IDX_LSB           ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_FIRST_RF_IDX_MASK           );
    api->rsp->dvbt2_plp_info.ff_flag                =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_FF_FLAG_LSB                ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_FF_FLAG_MASK                );
    api->rsp->dvbt2_plp_info.plp_group_id_msb       =   (( ( (rspByteBuffer[4]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_GROUP_ID_MSB_LSB       ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_GROUP_ID_MSB_MASK       );
    api->rsp->dvbt2_plp_info.first_frame_idx_lsb    =   (( ( (rspByteBuffer[4]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_FIRST_FRAME_IDX_LSB_LSB    ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_FIRST_FRAME_IDX_LSB_MASK    );
    api->rsp->dvbt2_plp_info.plp_mod_msb            =   (( ( (rspByteBuffer[5]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_MOD_MSB_LSB            ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_MOD_MSB_MASK            );
    api->rsp->dvbt2_plp_info.plp_cod                =   (( ( (rspByteBuffer[5]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_COD_LSB                ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_COD_MASK                );
    api->rsp->dvbt2_plp_info.plp_group_id_lsb       =   (( ( (rspByteBuffer[5]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_GROUP_ID_LSB_LSB       ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_GROUP_ID_LSB_MASK       );
    api->rsp->dvbt2_plp_info.plp_num_blocks_max_msb =   (( ( (rspByteBuffer[6]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_NUM_BLOCKS_MAX_MSB_LSB ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_NUM_BLOCKS_MAX_MSB_MASK );
    api->rsp->dvbt2_plp_info.plp_fec_type           =   (( ( (rspByteBuffer[6]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_FEC_TYPE_LSB           ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_FEC_TYPE_MASK           );
    api->rsp->dvbt2_plp_info.plp_rot                =   (( ( (rspByteBuffer[6]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_ROT_LSB                ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_ROT_MASK                );
    api->rsp->dvbt2_plp_info.plp_mod_lsb            =   (( ( (rspByteBuffer[6]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_MOD_LSB_LSB            ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_MOD_LSB_MASK            );
    api->rsp->dvbt2_plp_info.frame_interval_msb     =   (( ( (rspByteBuffer[7]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_FRAME_INTERVAL_MSB_LSB     ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_FRAME_INTERVAL_MSB_MASK     );
    api->rsp->dvbt2_plp_info.plp_num_blocks_max_lsb =   (( ( (rspByteBuffer[7]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_NUM_BLOCKS_MAX_LSB_LSB ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_NUM_BLOCKS_MAX_LSB_MASK );
    api->rsp->dvbt2_plp_info.time_il_length_msb     =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_TIME_IL_LENGTH_MSB_LSB     ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_TIME_IL_LENGTH_MSB_MASK     );
    api->rsp->dvbt2_plp_info.frame_interval_lsb     =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_FRAME_INTERVAL_LSB_LSB     ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_FRAME_INTERVAL_LSB_MASK     );
    api->rsp->dvbt2_plp_info.time_il_type           =   (( ( (rspByteBuffer[9]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_TIME_IL_TYPE_LSB           ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_TIME_IL_TYPE_MASK           );
    api->rsp->dvbt2_plp_info.time_il_length_lsb     =   (( ( (rspByteBuffer[9]  )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_TIME_IL_LENGTH_LSB_LSB     ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_TIME_IL_LENGTH_LSB_MASK     );
    api->rsp->dvbt2_plp_info.reserved_1_1           =   (( ( (rspByteBuffer[10] )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_RESERVED_1_1_LSB           ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_RESERVED_1_1_MASK           );
    api->rsp->dvbt2_plp_info.in_band_b_flag         =   (( ( (rspByteBuffer[10] )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_IN_BAND_B_FLAG_LSB         ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_IN_BAND_B_FLAG_MASK         );
    api->rsp->dvbt2_plp_info.in_band_a_flag         =   (( ( (rspByteBuffer[10] )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_IN_BAND_A_FLAG_LSB         ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_IN_BAND_A_FLAG_MASK         );
    api->rsp->dvbt2_plp_info.static_flag            =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_STATIC_FLAG_LSB            ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_STATIC_FLAG_MASK            );
    api->rsp->dvbt2_plp_info.plp_mode               =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_MODE_LSB               ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_PLP_MODE_MASK               );
    api->rsp->dvbt2_plp_info.reserved_1_2           =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_RESERVED_1_2_LSB           ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_RESERVED_1_2_MASK           );
    api->rsp->dvbt2_plp_info.static_padding_flag    =   (( ( (rspByteBuffer[12] )) >> Si2168B_DVBT2_PLP_INFO_RESPONSE_STATIC_PADDING_FLAG_LSB    ) & Si2168B_DVBT2_PLP_INFO_RESPONSE_STATIC_PADDING_FLAG_MASK    );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DVBT2_PLP_INFO_CMD */
#ifdef    Si2168B_DVBT2_PLP_SELECT_CMD
 /*---------------------------------------------------*/
/* Si2168B_DVBT2_PLP_SELECT COMMAND                 */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DVBT2_PLP_SELECT          (L1_Si2168B_Context *api,
                                                   unsigned char   plp_id,
                                                   unsigned char   plp_id_sel_mode)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[3];
    unsigned char rspByteBuffer[1];
    api->rsp->dvbt2_plp_select.STATUS = api->status;

    SiTRACE("Si2168B DVBT2_PLP_SELECT ");
  #ifdef   DEBUG_RANGE_CHECK
    SiTRACE("PLP_ID %d "         , plp_id          );
    if ((plp_id_sel_mode > Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("PLP_ID_SEL_MODE %d ", plp_id_sel_mode );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DVBT2_PLP_SELECT_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( plp_id          & Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_MASK          ) << Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_LSB         );
    cmdByteBuffer[2] = (unsigned char) ( ( plp_id_sel_mode & Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MASK ) << Si2168B_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_LSB);

    if (L0_WriteCommandBytes(api->i2c, 3, cmdByteBuffer) != 3) {
      SiTRACE("Error writing DVBT2_PLP_SELECT bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 1, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DVBT2_PLP_SELECT response\n");
      return error_code;
    }


    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DVBT2_PLP_SELECT_CMD */
#ifdef    Si2168B_DVBT2_STATUS_CMD
 /*---------------------------------------------------*/
/* Si2168B_DVBT2_STATUS COMMAND                     */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DVBT2_STATUS              (L1_Si2168B_Context *api,
                                                   unsigned char   intack)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[14];
    api->rsp->dvbt2_status.STATUS = api->status;

    SiTRACE("Si2168B DVBT2_STATUS ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((intack > Si2168B_DVBT2_STATUS_CMD_INTACK_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("INTACK %d ", intack );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DVBT2_STATUS_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( intack & Si2168B_DVBT2_STATUS_CMD_INTACK_MASK ) << Si2168B_DVBT2_STATUS_CMD_INTACK_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DVBT2_STATUS bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 14, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DVBT2_STATUS response\n");
      return error_code;
    }

    api->rsp->dvbt2_status.pclint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_PCLINT_LSB        ) & Si2168B_DVBT2_STATUS_RESPONSE_PCLINT_MASK        );
    api->rsp->dvbt2_status.dlint         =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_DLINT_LSB         ) & Si2168B_DVBT2_STATUS_RESPONSE_DLINT_MASK         );
    api->rsp->dvbt2_status.berint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_BERINT_LSB        ) & Si2168B_DVBT2_STATUS_RESPONSE_BERINT_MASK        );
    api->rsp->dvbt2_status.uncorint      =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_UNCORINT_LSB      ) & Si2168B_DVBT2_STATUS_RESPONSE_UNCORINT_MASK      );
    api->rsp->dvbt2_status.notdvbt2int   =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_NOTDVBT2INT_LSB   ) & Si2168B_DVBT2_STATUS_RESPONSE_NOTDVBT2INT_MASK   );
    api->rsp->dvbt2_status.pcl           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_PCL_LSB           ) & Si2168B_DVBT2_STATUS_RESPONSE_PCL_MASK           );
    api->rsp->dvbt2_status.dl            =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_DL_LSB            ) & Si2168B_DVBT2_STATUS_RESPONSE_DL_MASK            );
    api->rsp->dvbt2_status.ber           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_BER_LSB           ) & Si2168B_DVBT2_STATUS_RESPONSE_BER_MASK           );
    api->rsp->dvbt2_status.uncor         =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_UNCOR_LSB         ) & Si2168B_DVBT2_STATUS_RESPONSE_UNCOR_MASK         );
    api->rsp->dvbt2_status.notdvbt2      =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_NOTDVBT2_LSB      ) & Si2168B_DVBT2_STATUS_RESPONSE_NOTDVBT2_MASK      );
    api->rsp->dvbt2_status.cnr           =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_CNR_LSB           ) & Si2168B_DVBT2_STATUS_RESPONSE_CNR_MASK           );
    api->rsp->dvbt2_status.afc_freq      = (((( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 )) >> Si2168B_DVBT2_STATUS_RESPONSE_AFC_FREQ_LSB      ) & Si2168B_DVBT2_STATUS_RESPONSE_AFC_FREQ_MASK) <<Si2168B_DVBT2_STATUS_RESPONSE_AFC_FREQ_SHIFT ) >>Si2168B_DVBT2_STATUS_RESPONSE_AFC_FREQ_SHIFT      );
    api->rsp->dvbt2_status.timing_offset = (((( ( (rspByteBuffer[6]  ) | (rspByteBuffer[7]  << 8 )) >> Si2168B_DVBT2_STATUS_RESPONSE_TIMING_OFFSET_LSB ) & Si2168B_DVBT2_STATUS_RESPONSE_TIMING_OFFSET_MASK) <<Si2168B_DVBT2_STATUS_RESPONSE_TIMING_OFFSET_SHIFT ) >>Si2168B_DVBT2_STATUS_RESPONSE_TIMING_OFFSET_SHIFT );
    api->rsp->dvbt2_status.constellation =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_CONSTELLATION_LSB ) & Si2168B_DVBT2_STATUS_RESPONSE_CONSTELLATION_MASK );
    api->rsp->dvbt2_status.sp_inv        =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_SP_INV_LSB        ) & Si2168B_DVBT2_STATUS_RESPONSE_SP_INV_MASK        );
    api->rsp->dvbt2_status.fef           =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_FEF_LSB           ) & Si2168B_DVBT2_STATUS_RESPONSE_FEF_MASK           );
    api->rsp->dvbt2_status.fft_mode      =   (( ( (rspByteBuffer[9]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_FFT_MODE_LSB      ) & Si2168B_DVBT2_STATUS_RESPONSE_FFT_MODE_MASK      );
    api->rsp->dvbt2_status.guard_int     =   (( ( (rspByteBuffer[9]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_GUARD_INT_LSB     ) & Si2168B_DVBT2_STATUS_RESPONSE_GUARD_INT_MASK     );
    api->rsp->dvbt2_status.bw_ext        =   (( ( (rspByteBuffer[9]  )) >> Si2168B_DVBT2_STATUS_RESPONSE_BW_EXT_LSB        ) & Si2168B_DVBT2_STATUS_RESPONSE_BW_EXT_MASK        );
    api->rsp->dvbt2_status.num_plp       =   (( ( (rspByteBuffer[10] )) >> Si2168B_DVBT2_STATUS_RESPONSE_NUM_PLP_LSB       ) & Si2168B_DVBT2_STATUS_RESPONSE_NUM_PLP_MASK       );
    api->rsp->dvbt2_status.pilot_pattern =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_LSB ) & Si2168B_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_MASK );
    api->rsp->dvbt2_status.tx_mode       =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT2_STATUS_RESPONSE_TX_MODE_LSB       ) & Si2168B_DVBT2_STATUS_RESPONSE_TX_MODE_MASK       );
    api->rsp->dvbt2_status.rotated       =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT2_STATUS_RESPONSE_ROTATED_LSB       ) & Si2168B_DVBT2_STATUS_RESPONSE_ROTATED_MASK       );
    api->rsp->dvbt2_status.short_frame   =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT2_STATUS_RESPONSE_SHORT_FRAME_LSB   ) & Si2168B_DVBT2_STATUS_RESPONSE_SHORT_FRAME_MASK   );
    api->rsp->dvbt2_status.t2_mode       =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT2_STATUS_RESPONSE_T2_MODE_LSB       ) & Si2168B_DVBT2_STATUS_RESPONSE_T2_MODE_MASK       );
    api->rsp->dvbt2_status.code_rate     =   (( ( (rspByteBuffer[12] )) >> Si2168B_DVBT2_STATUS_RESPONSE_CODE_RATE_LSB     ) & Si2168B_DVBT2_STATUS_RESPONSE_CODE_RATE_MASK     );
    api->rsp->dvbt2_status.t2_version    =   (( ( (rspByteBuffer[12] )) >> Si2168B_DVBT2_STATUS_RESPONSE_T2_VERSION_LSB    ) & Si2168B_DVBT2_STATUS_RESPONSE_T2_VERSION_MASK    );
    api->rsp->dvbt2_status.plp_id        =   (( ( (rspByteBuffer[13] )) >> Si2168B_DVBT2_STATUS_RESPONSE_PLP_ID_LSB        ) & Si2168B_DVBT2_STATUS_RESPONSE_PLP_ID_MASK        );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DVBT2_STATUS_CMD */
#ifdef    Si2168B_DVBT2_TX_ID_CMD
 /*---------------------------------------------------*/
/* Si2168B_DVBT2_TX_ID COMMAND                      */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DVBT2_TX_ID               (L1_Si2168B_Context *api)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[1];
    unsigned char rspByteBuffer[8];
    api->rsp->dvbt2_tx_id.STATUS = api->status;

    SiTRACE("Si2168B DVBT2_TX_ID ");
    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DVBT2_TX_ID_CMD;

    if (L0_WriteCommandBytes(api->i2c, 1, cmdByteBuffer) != 1) {
      SiTRACE("Error writing DVBT2_TX_ID bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 8, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DVBT2_TX_ID response\n");
      return error_code;
    }

    api->rsp->dvbt2_tx_id.tx_id_availability =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT2_TX_ID_RESPONSE_TX_ID_AVAILABILITY_LSB ) & Si2168B_DVBT2_TX_ID_RESPONSE_TX_ID_AVAILABILITY_MASK );
    api->rsp->dvbt2_tx_id.cell_id            =   (( ( (rspByteBuffer[2]  ) | (rspByteBuffer[3]  << 8 )) >> Si2168B_DVBT2_TX_ID_RESPONSE_CELL_ID_LSB            ) & Si2168B_DVBT2_TX_ID_RESPONSE_CELL_ID_MASK            );
    api->rsp->dvbt2_tx_id.network_id         =   (( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 )) >> Si2168B_DVBT2_TX_ID_RESPONSE_NETWORK_ID_LSB         ) & Si2168B_DVBT2_TX_ID_RESPONSE_NETWORK_ID_MASK         );
    api->rsp->dvbt2_tx_id.t2_system_id       =   (( ( (rspByteBuffer[6]  ) | (rspByteBuffer[7]  << 8 )) >> Si2168B_DVBT2_TX_ID_RESPONSE_T2_SYSTEM_ID_LSB       ) & Si2168B_DVBT2_TX_ID_RESPONSE_T2_SYSTEM_ID_MASK       );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DVBT2_TX_ID_CMD */

#ifdef    Si2168B_DVBT_STATUS_CMD
 /*---------------------------------------------------*/
/* Si2168B_DVBT_STATUS COMMAND                      */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DVBT_STATUS               (L1_Si2168B_Context *api,
                                                   unsigned char   intack)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[13];
    api->rsp->dvbt_status.STATUS = api->status;

    SiTRACE("Si2168B DVBT_STATUS ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((intack > Si2168B_DVBT_STATUS_CMD_INTACK_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("INTACK %d ", intack );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DVBT_STATUS_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( intack & Si2168B_DVBT_STATUS_CMD_INTACK_MASK ) << Si2168B_DVBT_STATUS_CMD_INTACK_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing DVBT_STATUS bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 13, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DVBT_STATUS response\n");
      return error_code;
    }

    api->rsp->dvbt_status.pclint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_STATUS_RESPONSE_PCLINT_LSB        ) & Si2168B_DVBT_STATUS_RESPONSE_PCLINT_MASK        );
    api->rsp->dvbt_status.dlint         =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_STATUS_RESPONSE_DLINT_LSB         ) & Si2168B_DVBT_STATUS_RESPONSE_DLINT_MASK         );
    api->rsp->dvbt_status.berint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_STATUS_RESPONSE_BERINT_LSB        ) & Si2168B_DVBT_STATUS_RESPONSE_BERINT_MASK        );
    api->rsp->dvbt_status.uncorint      =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_STATUS_RESPONSE_UNCORINT_LSB      ) & Si2168B_DVBT_STATUS_RESPONSE_UNCORINT_MASK      );
    api->rsp->dvbt_status.notdvbtint    =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_STATUS_RESPONSE_NOTDVBTINT_LSB    ) & Si2168B_DVBT_STATUS_RESPONSE_NOTDVBTINT_MASK    );
    api->rsp->dvbt_status.pcl           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT_STATUS_RESPONSE_PCL_LSB           ) & Si2168B_DVBT_STATUS_RESPONSE_PCL_MASK           );
    api->rsp->dvbt_status.dl            =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT_STATUS_RESPONSE_DL_LSB            ) & Si2168B_DVBT_STATUS_RESPONSE_DL_MASK            );
    api->rsp->dvbt_status.ber           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT_STATUS_RESPONSE_BER_LSB           ) & Si2168B_DVBT_STATUS_RESPONSE_BER_MASK           );
    api->rsp->dvbt_status.uncor         =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT_STATUS_RESPONSE_UNCOR_LSB         ) & Si2168B_DVBT_STATUS_RESPONSE_UNCOR_MASK         );
    api->rsp->dvbt_status.notdvbt       =   (( ( (rspByteBuffer[2]  )) >> Si2168B_DVBT_STATUS_RESPONSE_NOTDVBT_LSB       ) & Si2168B_DVBT_STATUS_RESPONSE_NOTDVBT_MASK       );
    api->rsp->dvbt_status.cnr           =   (( ( (rspByteBuffer[3]  )) >> Si2168B_DVBT_STATUS_RESPONSE_CNR_LSB           ) & Si2168B_DVBT_STATUS_RESPONSE_CNR_MASK           );
    api->rsp->dvbt_status.afc_freq      = (((( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 )) >> Si2168B_DVBT_STATUS_RESPONSE_AFC_FREQ_LSB      ) & Si2168B_DVBT_STATUS_RESPONSE_AFC_FREQ_MASK) <<Si2168B_DVBT_STATUS_RESPONSE_AFC_FREQ_SHIFT ) >>Si2168B_DVBT_STATUS_RESPONSE_AFC_FREQ_SHIFT      );
    api->rsp->dvbt_status.timing_offset = (((( ( (rspByteBuffer[6]  ) | (rspByteBuffer[7]  << 8 )) >> Si2168B_DVBT_STATUS_RESPONSE_TIMING_OFFSET_LSB ) & Si2168B_DVBT_STATUS_RESPONSE_TIMING_OFFSET_MASK) <<Si2168B_DVBT_STATUS_RESPONSE_TIMING_OFFSET_SHIFT ) >>Si2168B_DVBT_STATUS_RESPONSE_TIMING_OFFSET_SHIFT );
    api->rsp->dvbt_status.constellation =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBT_STATUS_RESPONSE_CONSTELLATION_LSB ) & Si2168B_DVBT_STATUS_RESPONSE_CONSTELLATION_MASK );
    api->rsp->dvbt_status.sp_inv        =   (( ( (rspByteBuffer[8]  )) >> Si2168B_DVBT_STATUS_RESPONSE_SP_INV_LSB        ) & Si2168B_DVBT_STATUS_RESPONSE_SP_INV_MASK        );
    api->rsp->dvbt_status.rate_hp       =   (( ( (rspByteBuffer[9]  )) >> Si2168B_DVBT_STATUS_RESPONSE_RATE_HP_LSB       ) & Si2168B_DVBT_STATUS_RESPONSE_RATE_HP_MASK       );
    api->rsp->dvbt_status.rate_lp       =   (( ( (rspByteBuffer[9]  )) >> Si2168B_DVBT_STATUS_RESPONSE_RATE_LP_LSB       ) & Si2168B_DVBT_STATUS_RESPONSE_RATE_LP_MASK       );
    api->rsp->dvbt_status.fft_mode      =   (( ( (rspByteBuffer[10] )) >> Si2168B_DVBT_STATUS_RESPONSE_FFT_MODE_LSB      ) & Si2168B_DVBT_STATUS_RESPONSE_FFT_MODE_MASK      );
    api->rsp->dvbt_status.guard_int     =   (( ( (rspByteBuffer[10] )) >> Si2168B_DVBT_STATUS_RESPONSE_GUARD_INT_LSB     ) & Si2168B_DVBT_STATUS_RESPONSE_GUARD_INT_MASK     );
    api->rsp->dvbt_status.hierarchy     =   (( ( (rspByteBuffer[11] )) >> Si2168B_DVBT_STATUS_RESPONSE_HIERARCHY_LSB     ) & Si2168B_DVBT_STATUS_RESPONSE_HIERARCHY_MASK     );
    api->rsp->dvbt_status.tps_length    = (((( ( (rspByteBuffer[12] )) >> Si2168B_DVBT_STATUS_RESPONSE_TPS_LENGTH_LSB    ) & Si2168B_DVBT_STATUS_RESPONSE_TPS_LENGTH_MASK) <<Si2168B_DVBT_STATUS_RESPONSE_TPS_LENGTH_SHIFT ) >>Si2168B_DVBT_STATUS_RESPONSE_TPS_LENGTH_SHIFT    );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DVBT_STATUS_CMD */
#ifdef    Si2168B_DVBT_TPS_EXTRA_CMD
 /*---------------------------------------------------*/
/* Si2168B_DVBT_TPS_EXTRA COMMAND                   */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_DVBT_TPS_EXTRA            (L1_Si2168B_Context *api)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[1];
    unsigned char rspByteBuffer[6];
    api->rsp->dvbt_tps_extra.STATUS = api->status;

    SiTRACE("Si2168B DVBT_TPS_EXTRA ");
    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_DVBT_TPS_EXTRA_CMD;

    if (L0_WriteCommandBytes(api->i2c, 1, cmdByteBuffer) != 1) {
      SiTRACE("Error writing DVBT_TPS_EXTRA bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 6, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling DVBT_TPS_EXTRA response\n");
      return error_code;
    }

    api->rsp->dvbt_tps_extra.lptimeslice =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_LPTIMESLICE_LSB ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_LPTIMESLICE_MASK );
    api->rsp->dvbt_tps_extra.hptimeslice =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_HPTIMESLICE_LSB ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_HPTIMESLICE_MASK );
    api->rsp->dvbt_tps_extra.lpmpefec    =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_LPMPEFEC_LSB    ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_LPMPEFEC_MASK    );
    api->rsp->dvbt_tps_extra.hpmpefec    =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_HPMPEFEC_LSB    ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_HPMPEFEC_MASK    );
    api->rsp->dvbt_tps_extra.dvbhinter   =   (( ( (rspByteBuffer[1]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_DVBHINTER_LSB   ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_DVBHINTER_MASK   );
    api->rsp->dvbt_tps_extra.cell_id     = (((( ( (rspByteBuffer[2]  ) | (rspByteBuffer[3]  << 8 )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_CELL_ID_LSB     ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_CELL_ID_MASK) <<Si2168B_DVBT_TPS_EXTRA_RESPONSE_CELL_ID_SHIFT ) >>Si2168B_DVBT_TPS_EXTRA_RESPONSE_CELL_ID_SHIFT     );
    api->rsp->dvbt_tps_extra.tps_res1    =   (( ( (rspByteBuffer[4]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_TPS_RES1_LSB    ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_TPS_RES1_MASK    );
    api->rsp->dvbt_tps_extra.tps_res2    =   (( ( (rspByteBuffer[4]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_TPS_RES2_LSB    ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_TPS_RES2_MASK    );
    api->rsp->dvbt_tps_extra.tps_res3    =   (( ( (rspByteBuffer[5]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_TPS_RES3_LSB    ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_TPS_RES3_MASK    );
    api->rsp->dvbt_tps_extra.tps_res4    =   (( ( (rspByteBuffer[5]  )) >> Si2168B_DVBT_TPS_EXTRA_RESPONSE_TPS_RES4_LSB    ) & Si2168B_DVBT_TPS_EXTRA_RESPONSE_TPS_RES4_MASK    );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_DVBT_TPS_EXTRA_CMD */

#ifdef    Si2168B_EXIT_BOOTLOADER_CMD
 /*---------------------------------------------------*/
/* Si2168B_EXIT_BOOTLOADER COMMAND                  */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_EXIT_BOOTLOADER           (L1_Si2168B_Context *api,
                                                   unsigned char   func,
                                                   unsigned char   ctsien)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[1];
    api->rsp->exit_bootloader.STATUS = api->status;

    SiTRACE("Si2168B EXIT_BOOTLOADER ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((func   > Si2168B_EXIT_BOOTLOADER_CMD_FUNC_MAX  ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("FUNC %d "  , func   );
    if ((ctsien > Si2168B_EXIT_BOOTLOADER_CMD_CTSIEN_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("CTSIEN %d ", ctsien );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_EXIT_BOOTLOADER_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( func   & Si2168B_EXIT_BOOTLOADER_CMD_FUNC_MASK   ) << Si2168B_EXIT_BOOTLOADER_CMD_FUNC_LSB  |
                                         ( ctsien & Si2168B_EXIT_BOOTLOADER_CMD_CTSIEN_MASK ) << Si2168B_EXIT_BOOTLOADER_CMD_CTSIEN_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing EXIT_BOOTLOADER bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 1, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling EXIT_BOOTLOADER response\n");
      return error_code;
    }


    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_EXIT_BOOTLOADER_CMD */
#ifdef    Si2168B_GET_PROPERTY_CMD
 /*---------------------------------------------------*/
/* Si2168B_GET_PROPERTY COMMAND                     */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_GET_PROPERTY              (L1_Si2168B_Context *api,
                                                   unsigned char   reserved,
                                                   unsigned int    prop)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[4];
    unsigned char rspByteBuffer[4];
    api->rsp->get_property.STATUS = api->status;

    SiTRACE("Si2168B GET_PROPERTY ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((reserved > Si2168B_GET_PROPERTY_CMD_RESERVED_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED %d ", reserved );
    SiTRACE("PROP %d "    , prop     );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_GET_PROPERTY_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( reserved & Si2168B_GET_PROPERTY_CMD_RESERVED_MASK ) << Si2168B_GET_PROPERTY_CMD_RESERVED_LSB);
    cmdByteBuffer[2] = (unsigned char) ( ( prop     & Si2168B_GET_PROPERTY_CMD_PROP_MASK     ) << Si2168B_GET_PROPERTY_CMD_PROP_LSB    );
    cmdByteBuffer[3] = (unsigned char) ((( prop     & Si2168B_GET_PROPERTY_CMD_PROP_MASK     ) << Si2168B_GET_PROPERTY_CMD_PROP_LSB    )>>8);

    if (L0_WriteCommandBytes(api->i2c, 4, cmdByteBuffer) != 4) {
      SiTRACE("Error writing GET_PROPERTY bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 4, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling GET_PROPERTY response\n");
      return error_code;
    }

    api->rsp->get_property.reserved =   (( ( (rspByteBuffer[1]  )) >> Si2168B_GET_PROPERTY_RESPONSE_RESERVED_LSB ) & Si2168B_GET_PROPERTY_RESPONSE_RESERVED_MASK );
    api->rsp->get_property.data     =   (( ( (rspByteBuffer[2]  ) | (rspByteBuffer[3]  << 8 )) >> Si2168B_GET_PROPERTY_RESPONSE_DATA_LSB     ) & Si2168B_GET_PROPERTY_RESPONSE_DATA_MASK     );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_GET_PROPERTY_CMD */
#ifdef    Si2168B_GET_REV_CMD
 /*---------------------------------------------------*/
/* Si2168B_GET_REV COMMAND                          */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_GET_REV                   (L1_Si2168B_Context *api)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[1];
    unsigned char rspByteBuffer[10];
    api->rsp->get_rev.STATUS = api->status;

    SiTRACE("Si2168B GET_REV ");
    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_GET_REV_CMD;

    if (L0_WriteCommandBytes(api->i2c, 1, cmdByteBuffer) != 1) {
      SiTRACE("Error writing GET_REV bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 10, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling GET_REV response\n");
      return error_code;
    }

    api->rsp->get_rev.pn       =   (( ( (rspByteBuffer[1]  )) >> Si2168B_GET_REV_RESPONSE_PN_LSB       ) & Si2168B_GET_REV_RESPONSE_PN_MASK       );
    api->rsp->get_rev.fwmajor  =   (( ( (rspByteBuffer[2]  )) >> Si2168B_GET_REV_RESPONSE_FWMAJOR_LSB  ) & Si2168B_GET_REV_RESPONSE_FWMAJOR_MASK  );
    api->rsp->get_rev.fwminor  =   (( ( (rspByteBuffer[3]  )) >> Si2168B_GET_REV_RESPONSE_FWMINOR_LSB  ) & Si2168B_GET_REV_RESPONSE_FWMINOR_MASK  );
    api->rsp->get_rev.patch    =   (( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 )) >> Si2168B_GET_REV_RESPONSE_PATCH_LSB    ) & Si2168B_GET_REV_RESPONSE_PATCH_MASK    );
    api->rsp->get_rev.cmpmajor =   (( ( (rspByteBuffer[6]  )) >> Si2168B_GET_REV_RESPONSE_CMPMAJOR_LSB ) & Si2168B_GET_REV_RESPONSE_CMPMAJOR_MASK );
    api->rsp->get_rev.cmpminor =   (( ( (rspByteBuffer[7]  )) >> Si2168B_GET_REV_RESPONSE_CMPMINOR_LSB ) & Si2168B_GET_REV_RESPONSE_CMPMINOR_MASK );
    api->rsp->get_rev.cmpbuild =   (( ( (rspByteBuffer[8]  )) >> Si2168B_GET_REV_RESPONSE_CMPBUILD_LSB ) & Si2168B_GET_REV_RESPONSE_CMPBUILD_MASK );
    api->rsp->get_rev.chiprev  =   (( ( (rspByteBuffer[9]  )) >> Si2168B_GET_REV_RESPONSE_CHIPREV_LSB  ) & Si2168B_GET_REV_RESPONSE_CHIPREV_MASK  );
    api->rsp->get_rev.mcm_die  =   (( ( (rspByteBuffer[9]  )) >> Si2168B_GET_REV_RESPONSE_MCM_DIE_LSB  ) & Si2168B_GET_REV_RESPONSE_MCM_DIE_MASK  );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_GET_REV_CMD */
#ifdef    Si2168B_I2C_PASSTHROUGH_CMD
 /*---------------------------------------------------*/
/* Si2168B_I2C_PASSTHROUGH COMMAND                  */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_I2C_PASSTHROUGH           (L1_Si2168B_Context *api,
                                                   unsigned char   subcode,
                                                   unsigned char   i2c_passthru,
                                                   unsigned char   reserved)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[3];
    api->rsp->i2c_passthrough.STATUS = api->status;

    SiTRACE("Si2168B I2C_PASSTHROUGH ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((subcode      > Si2168B_I2C_PASSTHROUGH_CMD_SUBCODE_MAX     )  || (subcode      < Si2168B_I2C_PASSTHROUGH_CMD_SUBCODE_MIN     ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SUBCODE %d "     , subcode      );
    if ((i2c_passthru > Si2168B_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("I2C_PASSTHRU %d ", i2c_passthru );
    if ((reserved     > Si2168B_I2C_PASSTHROUGH_CMD_RESERVED_MAX    ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED %d "    , reserved     );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_I2C_PASSTHROUGH_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( subcode      & Si2168B_I2C_PASSTHROUGH_CMD_SUBCODE_MASK      ) << Si2168B_I2C_PASSTHROUGH_CMD_SUBCODE_LSB     );
    cmdByteBuffer[2] = (unsigned char) ( ( i2c_passthru & Si2168B_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_MASK ) << Si2168B_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_LSB|
                                         ( reserved     & Si2168B_I2C_PASSTHROUGH_CMD_RESERVED_MASK     ) << Si2168B_I2C_PASSTHROUGH_CMD_RESERVED_LSB    );

    if (L0_WriteCommandBytes(api->i2c, 3, cmdByteBuffer) != 3) {
      SiTRACE("Error writing I2C_PASSTHROUGH bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_I2C_PASSTHROUGH_CMD */
#ifdef    Si2168B_MCNS_STATUS_CMD
 /*---------------------------------------------------*/
/* Si2168B_MCNS_STATUS COMMAND                      */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_MCNS_STATUS               (L1_Si2168B_Context *api,
                                                   unsigned char   intack)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[10];
    api->rsp->mcns_status.STATUS = api->status;

    SiTRACE("Si2168B MCNS_STATUS ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((intack > Si2168B_MCNS_STATUS_CMD_INTACK_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("INTACK %d ", intack );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_MCNS_STATUS_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( intack & Si2168B_MCNS_STATUS_CMD_INTACK_MASK ) << Si2168B_MCNS_STATUS_CMD_INTACK_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing MCNS_STATUS bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 10, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling MCNS_STATUS response\n");
      return error_code;
    }

    api->rsp->mcns_status.pclint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_MCNS_STATUS_RESPONSE_PCLINT_LSB        ) & Si2168B_MCNS_STATUS_RESPONSE_PCLINT_MASK        );
    api->rsp->mcns_status.dlint         =   (( ( (rspByteBuffer[1]  )) >> Si2168B_MCNS_STATUS_RESPONSE_DLINT_LSB         ) & Si2168B_MCNS_STATUS_RESPONSE_DLINT_MASK         );
    api->rsp->mcns_status.berint        =   (( ( (rspByteBuffer[1]  )) >> Si2168B_MCNS_STATUS_RESPONSE_BERINT_LSB        ) & Si2168B_MCNS_STATUS_RESPONSE_BERINT_MASK        );
    api->rsp->mcns_status.uncorint      =   (( ( (rspByteBuffer[1]  )) >> Si2168B_MCNS_STATUS_RESPONSE_UNCORINT_LSB      ) & Si2168B_MCNS_STATUS_RESPONSE_UNCORINT_MASK      );
    api->rsp->mcns_status.pcl           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_MCNS_STATUS_RESPONSE_PCL_LSB           ) & Si2168B_MCNS_STATUS_RESPONSE_PCL_MASK           );
    api->rsp->mcns_status.dl            =   (( ( (rspByteBuffer[2]  )) >> Si2168B_MCNS_STATUS_RESPONSE_DL_LSB            ) & Si2168B_MCNS_STATUS_RESPONSE_DL_MASK            );
    api->rsp->mcns_status.ber           =   (( ( (rspByteBuffer[2]  )) >> Si2168B_MCNS_STATUS_RESPONSE_BER_LSB           ) & Si2168B_MCNS_STATUS_RESPONSE_BER_MASK           );
    api->rsp->mcns_status.uncor         =   (( ( (rspByteBuffer[2]  )) >> Si2168B_MCNS_STATUS_RESPONSE_UNCOR_LSB         ) & Si2168B_MCNS_STATUS_RESPONSE_UNCOR_MASK         );
    api->rsp->mcns_status.cnr           =   (( ( (rspByteBuffer[3]  )) >> Si2168B_MCNS_STATUS_RESPONSE_CNR_LSB           ) & Si2168B_MCNS_STATUS_RESPONSE_CNR_MASK           );
    api->rsp->mcns_status.afc_freq      = (((( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 )) >> Si2168B_MCNS_STATUS_RESPONSE_AFC_FREQ_LSB      ) & Si2168B_MCNS_STATUS_RESPONSE_AFC_FREQ_MASK) <<Si2168B_MCNS_STATUS_RESPONSE_AFC_FREQ_SHIFT ) >>Si2168B_MCNS_STATUS_RESPONSE_AFC_FREQ_SHIFT      );
    api->rsp->mcns_status.timing_offset = (((( ( (rspByteBuffer[6]  ) | (rspByteBuffer[7]  << 8 )) >> Si2168B_MCNS_STATUS_RESPONSE_TIMING_OFFSET_LSB ) & Si2168B_MCNS_STATUS_RESPONSE_TIMING_OFFSET_MASK) <<Si2168B_MCNS_STATUS_RESPONSE_TIMING_OFFSET_SHIFT ) >>Si2168B_MCNS_STATUS_RESPONSE_TIMING_OFFSET_SHIFT );
    api->rsp->mcns_status.constellation =   (( ( (rspByteBuffer[8]  )) >> Si2168B_MCNS_STATUS_RESPONSE_CONSTELLATION_LSB ) & Si2168B_MCNS_STATUS_RESPONSE_CONSTELLATION_MASK );
    api->rsp->mcns_status.sp_inv        =   (( ( (rspByteBuffer[8]  )) >> Si2168B_MCNS_STATUS_RESPONSE_SP_INV_LSB        ) & Si2168B_MCNS_STATUS_RESPONSE_SP_INV_MASK        );
    api->rsp->mcns_status.interleaving  =   (( ( (rspByteBuffer[9]  )) >> Si2168B_MCNS_STATUS_RESPONSE_INTERLEAVING_LSB  ) & Si2168B_MCNS_STATUS_RESPONSE_INTERLEAVING_MASK  );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_MCNS_STATUS_CMD */

#ifdef    Si2168B_PART_INFO_CMD
 /*---------------------------------------------------*/
/* Si2168B_PART_INFO COMMAND                        */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_PART_INFO                 (L1_Si2168B_Context *api)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[1];
    unsigned char rspByteBuffer[13];
    api->rsp->part_info.STATUS = api->status;

    SiTRACE("Si2168B PART_INFO ");
    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_PART_INFO_CMD;

    if (L0_WriteCommandBytes(api->i2c, 1, cmdByteBuffer) != 1) {
      SiTRACE("Error writing PART_INFO bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 13, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling PART_INFO response\n");
      return error_code;
    }

    api->rsp->part_info.chiprev  =   (( ( (rspByteBuffer[1]  )) >> Si2168B_PART_INFO_RESPONSE_CHIPREV_LSB  ) & Si2168B_PART_INFO_RESPONSE_CHIPREV_MASK  );
    api->rsp->part_info.part     =   (( ( (rspByteBuffer[2]  )) >> Si2168B_PART_INFO_RESPONSE_PART_LSB     ) & Si2168B_PART_INFO_RESPONSE_PART_MASK     );
    api->rsp->part_info.pmajor   =   (( ( (rspByteBuffer[3]  )) >> Si2168B_PART_INFO_RESPONSE_PMAJOR_LSB   ) & Si2168B_PART_INFO_RESPONSE_PMAJOR_MASK   );
    api->rsp->part_info.pminor   =   (( ( (rspByteBuffer[4]  )) >> Si2168B_PART_INFO_RESPONSE_PMINOR_LSB   ) & Si2168B_PART_INFO_RESPONSE_PMINOR_MASK   );
    api->rsp->part_info.pbuild   =   (( ( (rspByteBuffer[5]  )) >> Si2168B_PART_INFO_RESPONSE_PBUILD_LSB   ) & Si2168B_PART_INFO_RESPONSE_PBUILD_MASK   );
    api->rsp->part_info.reserved =   (( ( (rspByteBuffer[6]  ) | (rspByteBuffer[7]  << 8 )) >> Si2168B_PART_INFO_RESPONSE_RESERVED_LSB ) & Si2168B_PART_INFO_RESPONSE_RESERVED_MASK );
    api->rsp->part_info.serial   =   (( ( (rspByteBuffer[8]  ) | (rspByteBuffer[9]  << 8 ) | (rspByteBuffer[10] << 16 ) | (rspByteBuffer[11] << 24 )) >> Si2168B_PART_INFO_RESPONSE_SERIAL_LSB   ) & Si2168B_PART_INFO_RESPONSE_SERIAL_MASK   );
    api->rsp->part_info.romid    =   (( ( (rspByteBuffer[12] )) >> Si2168B_PART_INFO_RESPONSE_ROMID_LSB    ) & Si2168B_PART_INFO_RESPONSE_ROMID_MASK    );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_PART_INFO_CMD */
#ifdef    Si2168B_POWER_DOWN_CMD
 /*---------------------------------------------------*/
/* Si2168B_POWER_DOWN COMMAND                       */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_POWER_DOWN                (L1_Si2168B_Context *api)
{
    unsigned char cmdByteBuffer[1];
    api->rsp->power_down.STATUS = api->status;

    SiTRACE("Si2168B POWER_DOWN ");
    SiTRACE("\n");

    system_wait(2); /* Make sure that the FW 'main' function has applied any previous settings before going to standby */

    cmdByteBuffer[0] = Si2168B_POWER_DOWN_CMD;

    if (L0_WriteCommandBytes(api->i2c, 1, cmdByteBuffer) != 1) {
      SiTRACE("Error writing POWER_DOWN bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_POWER_DOWN_CMD */
#ifdef    Si2168B_POWER_UP_CMD
 /*---------------------------------------------------*/
/* Si2168B_POWER_UP COMMAND                         */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_POWER_UP                  (L1_Si2168B_Context *api,
                                                   unsigned char   subcode,
                                                   unsigned char   reset,
                                                   unsigned char   reserved2,
                                                   unsigned char   reserved4,
                                                   unsigned char   reserved1,
                                                   unsigned char   addr_mode,
                                                   unsigned char   reserved5,
                                                   unsigned char   func,
                                                   unsigned char   clock_freq,
                                                   unsigned char   ctsien,
                                                   unsigned char   wake_up)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[8];
    unsigned char rspByteBuffer[1];
    api->rsp->power_up.STATUS = api->status;

    SiTRACE("Si2168B POWER_UP ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((subcode    > Si2168B_POWER_UP_CMD_SUBCODE_MAX   )  || (subcode    < Si2168B_POWER_UP_CMD_SUBCODE_MIN   ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SUBCODE %d "   , subcode    );
    if ((reset      > Si2168B_POWER_UP_CMD_RESET_MAX     )  || (reset      < Si2168B_POWER_UP_CMD_RESET_MIN     ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESET %d "     , reset      );
    if ((reserved2  > Si2168B_POWER_UP_CMD_RESERVED2_MAX )  || (reserved2  < Si2168B_POWER_UP_CMD_RESERVED2_MIN ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED2 %d " , reserved2  );
    if ((reserved4  > Si2168B_POWER_UP_CMD_RESERVED4_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED4 %d " , reserved4  );
    if ((reserved1  > Si2168B_POWER_UP_CMD_RESERVED1_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED1 %d " , reserved1  );
    if ((addr_mode  > Si2168B_POWER_UP_CMD_ADDR_MODE_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("ADDR_MODE %d " , addr_mode  );
    if ((reserved5  > Si2168B_POWER_UP_CMD_RESERVED5_MAX )  || (reserved5  < Si2168B_POWER_UP_CMD_RESERVED5_MIN ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED5 %d " , reserved5  );
    if ((func       > Si2168B_POWER_UP_CMD_FUNC_MAX      ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("FUNC %d "      , func       );
    if ((clock_freq > Si2168B_POWER_UP_CMD_CLOCK_FREQ_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("CLOCK_FREQ %d ", clock_freq );
    if ((ctsien     > Si2168B_POWER_UP_CMD_CTSIEN_MAX    ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("CTSIEN %d "    , ctsien     );
    if ((wake_up    > Si2168B_POWER_UP_CMD_WAKE_UP_MAX   )  || (wake_up    < Si2168B_POWER_UP_CMD_WAKE_UP_MIN   ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("WAKE_UP %d "   , wake_up    );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_POWER_UP_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( subcode    & Si2168B_POWER_UP_CMD_SUBCODE_MASK    ) << Si2168B_POWER_UP_CMD_SUBCODE_LSB   );
    cmdByteBuffer[2] = (unsigned char) ( ( reset      & Si2168B_POWER_UP_CMD_RESET_MASK      ) << Si2168B_POWER_UP_CMD_RESET_LSB     );
    cmdByteBuffer[3] = (unsigned char) ( ( reserved2  & Si2168B_POWER_UP_CMD_RESERVED2_MASK  ) << Si2168B_POWER_UP_CMD_RESERVED2_LSB );
    cmdByteBuffer[4] = (unsigned char) ( ( reserved4  & Si2168B_POWER_UP_CMD_RESERVED4_MASK  ) << Si2168B_POWER_UP_CMD_RESERVED4_LSB );
    cmdByteBuffer[5] = (unsigned char) ( ( reserved1  & Si2168B_POWER_UP_CMD_RESERVED1_MASK  ) << Si2168B_POWER_UP_CMD_RESERVED1_LSB |
                                         ( addr_mode  & Si2168B_POWER_UP_CMD_ADDR_MODE_MASK  ) << Si2168B_POWER_UP_CMD_ADDR_MODE_LSB |
                                         ( reserved5  & Si2168B_POWER_UP_CMD_RESERVED5_MASK  ) << Si2168B_POWER_UP_CMD_RESERVED5_LSB );
    cmdByteBuffer[6] = (unsigned char) ( ( func       & Si2168B_POWER_UP_CMD_FUNC_MASK       ) << Si2168B_POWER_UP_CMD_FUNC_LSB      |
                                         ( clock_freq & Si2168B_POWER_UP_CMD_CLOCK_FREQ_MASK ) << Si2168B_POWER_UP_CMD_CLOCK_FREQ_LSB|
                                         ( ctsien     & Si2168B_POWER_UP_CMD_CTSIEN_MASK     ) << Si2168B_POWER_UP_CMD_CTSIEN_LSB    );
    cmdByteBuffer[7] = (unsigned char) ( ( wake_up    & Si2168B_POWER_UP_CMD_WAKE_UP_MASK    ) << Si2168B_POWER_UP_CMD_WAKE_UP_LSB   );

    if (L0_WriteCommandBytes(api->i2c, 8, cmdByteBuffer) != 8) {
      SiTRACE("Error writing POWER_UP bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 1, rspByteBuffer);

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_POWER_UP_CMD */
#ifdef    Si2168B_RSSI_ADC_CMD
 /*---------------------------------------------------*/
/* Si2168B_RSSI_ADC COMMAND                         */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_RSSI_ADC                  (L1_Si2168B_Context *api,
                                                   unsigned char   on_off)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[2];
    api->rsp->rssi_adc.STATUS = api->status;

    SiTRACE("Si2168B RSSI_ADC ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((on_off > Si2168B_RSSI_ADC_CMD_ON_OFF_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("ON_OFF %d ", on_off );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_RSSI_ADC_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( on_off & Si2168B_RSSI_ADC_CMD_ON_OFF_MASK ) << Si2168B_RSSI_ADC_CMD_ON_OFF_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing RSSI_ADC bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 2, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling RSSI_ADC response\n");
      return error_code;
    }

    api->rsp->rssi_adc.level =   (( ( (rspByteBuffer[1]  )) >> Si2168B_RSSI_ADC_RESPONSE_LEVEL_LSB ) & Si2168B_RSSI_ADC_RESPONSE_LEVEL_MASK );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_RSSI_ADC_CMD */
#ifdef    Si2168B_SCAN_CTRL_CMD
 /*---------------------------------------------------*/
/* Si2168B_SCAN_CTRL COMMAND                        */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_SCAN_CTRL                 (L1_Si2168B_Context *api,
                                                   unsigned char   action,
                                                   unsigned long   tuned_rf_freq)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[8];
    unsigned char rspByteBuffer[1];
    api->rsp->scan_ctrl.STATUS = api->status;

    SiTRACE("Si2168B SCAN_CTRL ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((action        > Si2168B_SCAN_CTRL_CMD_ACTION_MAX       )  || (action        < Si2168B_SCAN_CTRL_CMD_ACTION_MIN       ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("ACTION %d "       , action        );
    SiTRACE("TUNED_RF_FREQ %ld ", tuned_rf_freq );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_SCAN_CTRL_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( action        & Si2168B_SCAN_CTRL_CMD_ACTION_MASK        ) << Si2168B_SCAN_CTRL_CMD_ACTION_LSB       );
    cmdByteBuffer[2] = (unsigned char)0x00;
    cmdByteBuffer[3] = (unsigned char)0x00;
    cmdByteBuffer[4] = (unsigned char) ( ( tuned_rf_freq & Si2168B_SCAN_CTRL_CMD_TUNED_RF_FREQ_MASK ) << Si2168B_SCAN_CTRL_CMD_TUNED_RF_FREQ_LSB);
    cmdByteBuffer[5] = (unsigned char) ((( tuned_rf_freq & Si2168B_SCAN_CTRL_CMD_TUNED_RF_FREQ_MASK ) << Si2168B_SCAN_CTRL_CMD_TUNED_RF_FREQ_LSB)>>8);
    cmdByteBuffer[6] = (unsigned char) ((( tuned_rf_freq & Si2168B_SCAN_CTRL_CMD_TUNED_RF_FREQ_MASK ) << Si2168B_SCAN_CTRL_CMD_TUNED_RF_FREQ_LSB)>>16);
    cmdByteBuffer[7] = (unsigned char) ((( tuned_rf_freq & Si2168B_SCAN_CTRL_CMD_TUNED_RF_FREQ_MASK ) << Si2168B_SCAN_CTRL_CMD_TUNED_RF_FREQ_LSB)>>24);

    if (L0_WriteCommandBytes(api->i2c, 8, cmdByteBuffer) != 8) {
      SiTRACE("Error writing SCAN_CTRL bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 1, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling SCAN_CTRL response\n");
      return error_code;
    }


    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_SCAN_CTRL_CMD */
#ifdef    Si2168B_SCAN_STATUS_CMD
 /*---------------------------------------------------*/
/* Si2168B_SCAN_STATUS COMMAND                      */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_SCAN_STATUS               (L1_Si2168B_Context *api,
                                                   unsigned char   intack)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[2];
    unsigned char rspByteBuffer[11];
    api->rsp->scan_status.STATUS = api->status;

    SiTRACE("Si2168B SCAN_STATUS ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((intack > Si2168B_SCAN_STATUS_CMD_INTACK_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("INTACK %d ", intack );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_SCAN_STATUS_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( intack & Si2168B_SCAN_STATUS_CMD_INTACK_MASK ) << Si2168B_SCAN_STATUS_CMD_INTACK_LSB);

    if (L0_WriteCommandBytes(api->i2c, 2, cmdByteBuffer) != 2) {
      SiTRACE("Error writing SCAN_STATUS bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 11, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling SCAN_STATUS response\n");
      return error_code;
    }

    api->rsp->scan_status.buzint      =   (( ( (rspByteBuffer[1]  )) >> Si2168B_SCAN_STATUS_RESPONSE_BUZINT_LSB      ) & Si2168B_SCAN_STATUS_RESPONSE_BUZINT_MASK      );
    api->rsp->scan_status.reqint      =   (( ( (rspByteBuffer[1]  )) >> Si2168B_SCAN_STATUS_RESPONSE_REQINT_LSB      ) & Si2168B_SCAN_STATUS_RESPONSE_REQINT_MASK      );
    api->rsp->scan_status.buz         =   (( ( (rspByteBuffer[2]  )) >> Si2168B_SCAN_STATUS_RESPONSE_BUZ_LSB         ) & Si2168B_SCAN_STATUS_RESPONSE_BUZ_MASK         );
    api->rsp->scan_status.req         =   (( ( (rspByteBuffer[2]  )) >> Si2168B_SCAN_STATUS_RESPONSE_REQ_LSB         ) & Si2168B_SCAN_STATUS_RESPONSE_REQ_MASK         );
    api->rsp->scan_status.scan_status =   (( ( (rspByteBuffer[3]  )) >> Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_LSB ) & Si2168B_SCAN_STATUS_RESPONSE_SCAN_STATUS_MASK );
    api->rsp->scan_status.rf_freq     =   (( ( (rspByteBuffer[4]  ) | (rspByteBuffer[5]  << 8 ) | (rspByteBuffer[6]  << 16 ) | (rspByteBuffer[7]  << 24 )) >> Si2168B_SCAN_STATUS_RESPONSE_RF_FREQ_LSB     ) & Si2168B_SCAN_STATUS_RESPONSE_RF_FREQ_MASK     );
    api->rsp->scan_status.symb_rate   =   (( ( (rspByteBuffer[8]  ) | (rspByteBuffer[9]  << 8 )) >> Si2168B_SCAN_STATUS_RESPONSE_SYMB_RATE_LSB   ) & Si2168B_SCAN_STATUS_RESPONSE_SYMB_RATE_MASK   );
    api->rsp->scan_status.modulation  =   (( ( (rspByteBuffer[10] )) >> Si2168B_SCAN_STATUS_RESPONSE_MODULATION_LSB  ) & Si2168B_SCAN_STATUS_RESPONSE_MODULATION_MASK  );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_SCAN_STATUS_CMD */
#ifdef    Si2168B_SET_PROPERTY_CMD
 /*---------------------------------------------------*/
/* Si2168B_SET_PROPERTY COMMAND                     */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_SET_PROPERTY              (L1_Si2168B_Context *api,
                                                   unsigned char   reserved,
                                                   unsigned int    prop,
                                                   unsigned int    data)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[6];
    unsigned char rspByteBuffer[4];
    api->rsp->set_property.STATUS = api->status;

    SiTRACE("Si2168B SET_PROPERTY ");
  #ifdef   DEBUG_RANGE_CHECK
    SiTRACE("RESERVED %d ", reserved );
    SiTRACE("PROP 0x%04x ", prop     );
    SiTRACE("DATA %d "    , data     );
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_SET_PROPERTY_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( reserved & Si2168B_SET_PROPERTY_CMD_RESERVED_MASK ) << Si2168B_SET_PROPERTY_CMD_RESERVED_LSB);
    cmdByteBuffer[2] = (unsigned char) ( ( prop     & Si2168B_SET_PROPERTY_CMD_PROP_MASK     ) << Si2168B_SET_PROPERTY_CMD_PROP_LSB    );
    cmdByteBuffer[3] = (unsigned char) ((( prop     & Si2168B_SET_PROPERTY_CMD_PROP_MASK     ) << Si2168B_SET_PROPERTY_CMD_PROP_LSB    )>>8);
    cmdByteBuffer[4] = (unsigned char) ( ( data     & Si2168B_SET_PROPERTY_CMD_DATA_MASK     ) << Si2168B_SET_PROPERTY_CMD_DATA_LSB    );
    cmdByteBuffer[5] = (unsigned char) ((( data     & Si2168B_SET_PROPERTY_CMD_DATA_MASK     ) << Si2168B_SET_PROPERTY_CMD_DATA_LSB    )>>8);

    if (L0_WriteCommandBytes(api->i2c, 6, cmdByteBuffer) != 6) {
      SiTRACE("Error writing SET_PROPERTY bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    error_code = Si2168B_pollForResponse(api, 4, rspByteBuffer);
    if (error_code) {
      SiTRACE("Error polling SET_PROPERTY response\n");
      return error_code;
    }

    api->rsp->set_property.reserved =   (( ( (rspByteBuffer[1]  )) >> Si2168B_SET_PROPERTY_RESPONSE_RESERVED_LSB ) & Si2168B_SET_PROPERTY_RESPONSE_RESERVED_MASK );
    api->rsp->set_property.data     =   (( ( (rspByteBuffer[2]  ) | (rspByteBuffer[3]  << 8 )) >> Si2168B_SET_PROPERTY_RESPONSE_DATA_LSB     ) & Si2168B_SET_PROPERTY_RESPONSE_DATA_MASK     );

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_SET_PROPERTY_CMD */
#ifdef    Si2168B_SPI_LINK_CMD
 /*---------------------------------------------------*/
/* Si2168B_SPI_LINK COMMAND                         */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_SPI_LINK                  (L1_Si2168B_Context *api,
                                                   unsigned char   subcode,
                                                   unsigned char   spi_pbl_key,
                                                   unsigned char   spi_pbl_num,
                                                   unsigned char   spi_conf_clk,
                                                   unsigned char   spi_clk_pola,
                                                   unsigned char   spi_conf_data,
                                                   unsigned char   spi_data_dir,
                                                   unsigned char   spi_enable)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[7];
    api->rsp->spi_link.STATUS = api->status;

    SiTRACE("Si2168B SPI_LINK ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((subcode       > Si2168B_SPI_LINK_CMD_SUBCODE_MAX      )  || (subcode       < Si2168B_SPI_LINK_CMD_SUBCODE_MIN      ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SUBCODE %d "      , subcode       );
    SiTRACE("SPI_PBL_KEY %d "  , spi_pbl_key   );
    if ((spi_pbl_num   > Si2168B_SPI_LINK_CMD_SPI_PBL_NUM_MAX  ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SPI_PBL_NUM %d "  , spi_pbl_num   );
    if ((spi_conf_clk  > Si2168B_SPI_LINK_CMD_SPI_CONF_CLK_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SPI_CONF_CLK %d " , spi_conf_clk  );
    if ((spi_clk_pola  > Si2168B_SPI_LINK_CMD_SPI_CLK_POLA_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SPI_CLK_POLA %d " , spi_clk_pola  );
    if ((spi_conf_data > Si2168B_SPI_LINK_CMD_SPI_CONF_DATA_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SPI_CONF_DATA %d ", spi_conf_data );
    if ((spi_data_dir  > Si2168B_SPI_LINK_CMD_SPI_DATA_DIR_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SPI_DATA_DIR %d " , spi_data_dir  );
    if ((spi_enable    > Si2168B_SPI_LINK_CMD_SPI_ENABLE_MAX   ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SPI_ENABLE %d "   , spi_enable    );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_SPI_LINK_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( subcode       & Si2168B_SPI_LINK_CMD_SUBCODE_MASK       ) << Si2168B_SPI_LINK_CMD_SUBCODE_LSB      );
    cmdByteBuffer[2] = (unsigned char) ( ( spi_pbl_key   & Si2168B_SPI_LINK_CMD_SPI_PBL_KEY_MASK   ) << Si2168B_SPI_LINK_CMD_SPI_PBL_KEY_LSB  );
    cmdByteBuffer[3] = (unsigned char) ( ( spi_pbl_num   & Si2168B_SPI_LINK_CMD_SPI_PBL_NUM_MASK   ) << Si2168B_SPI_LINK_CMD_SPI_PBL_NUM_LSB  );
    cmdByteBuffer[4] = (unsigned char) ( ( spi_conf_clk  & Si2168B_SPI_LINK_CMD_SPI_CONF_CLK_MASK  ) << Si2168B_SPI_LINK_CMD_SPI_CONF_CLK_LSB |
                                         ( spi_clk_pola  & Si2168B_SPI_LINK_CMD_SPI_CLK_POLA_MASK  ) << Si2168B_SPI_LINK_CMD_SPI_CLK_POLA_LSB );
    cmdByteBuffer[5] = (unsigned char) ( ( spi_conf_data & Si2168B_SPI_LINK_CMD_SPI_CONF_DATA_MASK ) << Si2168B_SPI_LINK_CMD_SPI_CONF_DATA_LSB|
                                         ( spi_data_dir  & Si2168B_SPI_LINK_CMD_SPI_DATA_DIR_MASK  ) << Si2168B_SPI_LINK_CMD_SPI_DATA_DIR_LSB );
    cmdByteBuffer[6] = (unsigned char) ( ( spi_enable    & Si2168B_SPI_LINK_CMD_SPI_ENABLE_MASK    ) << Si2168B_SPI_LINK_CMD_SPI_ENABLE_LSB   );

    if (L0_WriteCommandBytes(api->i2c, 7, cmdByteBuffer) != 7) {
      SiTRACE("Error writing SPI_LINK bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_SPI_LINK_CMD */
#ifdef    Si2168B_SPI_PASSTHROUGH_CMD
 /*---------------------------------------------------*/
/* Si2168B_SPI_PASSTHROUGH COMMAND                  */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_SPI_PASSTHROUGH           (L1_Si2168B_Context *api,
                                                   unsigned char   subcode,
                                                   unsigned char   spi_passthr_clk,
                                                   unsigned char   spi_passth_data)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[4];
    api->rsp->spi_passthrough.STATUS = api->status;

    SiTRACE("Si2168B SPI_PASSTHROUGH ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((subcode         > Si2168B_SPI_PASSTHROUGH_CMD_SUBCODE_MAX        )  || (subcode         < Si2168B_SPI_PASSTHROUGH_CMD_SUBCODE_MIN        ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SUBCODE %d "        , subcode         );
    if ((spi_passthr_clk > Si2168B_SPI_PASSTHROUGH_CMD_SPI_PASSTHR_CLK_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SPI_PASSTHR_CLK %d ", spi_passthr_clk );
    if ((spi_passth_data > Si2168B_SPI_PASSTHROUGH_CMD_SPI_PASSTH_DATA_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SPI_PASSTH_DATA %d ", spi_passth_data );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_SPI_PASSTHROUGH_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( subcode         & Si2168B_SPI_PASSTHROUGH_CMD_SUBCODE_MASK         ) << Si2168B_SPI_PASSTHROUGH_CMD_SUBCODE_LSB        );
    cmdByteBuffer[2] = (unsigned char) ( ( spi_passthr_clk & Si2168B_SPI_PASSTHROUGH_CMD_SPI_PASSTHR_CLK_MASK ) << Si2168B_SPI_PASSTHROUGH_CMD_SPI_PASSTHR_CLK_LSB);
    cmdByteBuffer[3] = (unsigned char) ( ( spi_passth_data & Si2168B_SPI_PASSTHROUGH_CMD_SPI_PASSTH_DATA_MASK ) << Si2168B_SPI_PASSTHROUGH_CMD_SPI_PASSTH_DATA_LSB);

    if (L0_WriteCommandBytes(api->i2c, 4, cmdByteBuffer) != 4) {
      SiTRACE("Error writing SPI_PASSTHROUGH bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_SPI_PASSTHROUGH_CMD */
#ifdef    Si2168B_START_CLK_CMD
 /*---------------------------------------------------*/
/* Si2168B_START_CLK COMMAND                        */
/*---------------------------------------------------*/
unsigned char Si2168B_L1_START_CLK                 (L1_Si2168B_Context *api,
                                                   unsigned char   subcode,
                                                   unsigned char   reserved1,
                                                   unsigned char   tune_cap,
                                                   unsigned char   reserved2,
                                                   unsigned int    clk_mode,
                                                   unsigned char   reserved3,
                                                   unsigned char   reserved4,
                                                   unsigned char   start_clk)
{
    unsigned char error_code = 0;
    unsigned char cmdByteBuffer[13];
    api->rsp->start_clk.STATUS = api->status;

    SiTRACE("Si2168B START_CLK ");
  #ifdef   DEBUG_RANGE_CHECK
    if ((subcode   > Si2168B_START_CLK_CMD_SUBCODE_MAX  )  || (subcode   < Si2168B_START_CLK_CMD_SUBCODE_MIN  ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("SUBCODE %d "  , subcode   );
    if ((reserved1 > Si2168B_START_CLK_CMD_RESERVED1_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED1 %d ", reserved1 );
    if ((tune_cap  > Si2168B_START_CLK_CMD_TUNE_CAP_MAX ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("TUNE_CAP %d " , tune_cap  );
    if ((reserved2 > Si2168B_START_CLK_CMD_RESERVED2_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED2 %d ", reserved2 );
    if ((clk_mode  > Si2168B_START_CLK_CMD_CLK_MODE_MAX )  || (clk_mode  < Si2168B_START_CLK_CMD_CLK_MODE_MIN ) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("CLK_MODE %d " , clk_mode  );
    if ((reserved3 > Si2168B_START_CLK_CMD_RESERVED3_MAX)  || (reserved3 < Si2168B_START_CLK_CMD_RESERVED3_MIN) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED3 %d ", reserved3 );
    if ((reserved4 > Si2168B_START_CLK_CMD_RESERVED4_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("RESERVED4 %d ", reserved4 );
    if ((start_clk > Si2168B_START_CLK_CMD_START_CLK_MAX) ) {error_code++; SiTRACE("\nOut of range: ");}; SiTRACE("START_CLK %d ", start_clk );
    if (error_code) {
      SiTRACE("%d out of range parameters\n", error_code);
      return ERROR_Si2168B_PARAMETER_OUT_OF_RANGE;
    }
  #endif /* DEBUG_RANGE_CHECK */

    SiTRACE("\n");
    cmdByteBuffer[0] = Si2168B_START_CLK_CMD;
    cmdByteBuffer[1] = (unsigned char) ( ( subcode   & Si2168B_START_CLK_CMD_SUBCODE_MASK   ) << Si2168B_START_CLK_CMD_SUBCODE_LSB  );
    cmdByteBuffer[2] = (unsigned char) ( ( reserved1 & Si2168B_START_CLK_CMD_RESERVED1_MASK ) << Si2168B_START_CLK_CMD_RESERVED1_LSB);
    cmdByteBuffer[3] = (unsigned char) ( ( tune_cap  & Si2168B_START_CLK_CMD_TUNE_CAP_MASK  ) << Si2168B_START_CLK_CMD_TUNE_CAP_LSB |
                                         ( reserved2 & Si2168B_START_CLK_CMD_RESERVED2_MASK ) << Si2168B_START_CLK_CMD_RESERVED2_LSB);
    cmdByteBuffer[4] = (unsigned char) ( ( clk_mode  & Si2168B_START_CLK_CMD_CLK_MODE_MASK  ) << Si2168B_START_CLK_CMD_CLK_MODE_LSB );
    cmdByteBuffer[5] = (unsigned char) ((( clk_mode  & Si2168B_START_CLK_CMD_CLK_MODE_MASK  ) << Si2168B_START_CLK_CMD_CLK_MODE_LSB )>>8);
    cmdByteBuffer[6] = (unsigned char) ( ( reserved3 & Si2168B_START_CLK_CMD_RESERVED3_MASK ) << Si2168B_START_CLK_CMD_RESERVED3_LSB);
    cmdByteBuffer[7] = (unsigned char) ( ( reserved4 & Si2168B_START_CLK_CMD_RESERVED4_MASK ) << Si2168B_START_CLK_CMD_RESERVED4_LSB);
    cmdByteBuffer[8] = (unsigned char)0x00;
    cmdByteBuffer[9] = (unsigned char)0x00;
    cmdByteBuffer[10] = (unsigned char)0x00;
    cmdByteBuffer[11] = (unsigned char)0x00;
    cmdByteBuffer[12] = (unsigned char) ( ( start_clk & Si2168B_START_CLK_CMD_START_CLK_MASK ) << Si2168B_START_CLK_CMD_START_CLK_LSB);

    if (L0_WriteCommandBytes(api->i2c, 13, cmdByteBuffer) != 13) {
      SiTRACE("Error writing START_CLK bytes!\n");
      return ERROR_Si2168B_SENDING_COMMAND;
    }

    return NO_Si2168B_ERROR;
}
#endif /* Si2168B_START_CLK_CMD */


  /* --------------------------------------------*/
  /* SEND_COMMAND2 FUNCTION                      */
  /* --------------------------------------------*/
unsigned char   Si2168B_L1_SendCommand2(L1_Si2168B_Context *api, unsigned int cmd_code) {
    switch (cmd_code) {
    #ifdef        Si2168B_CONFIG_CLKIO_CMD
     case         Si2168B_CONFIG_CLKIO_CMD_CODE:
       return Si2168B_L1_CONFIG_CLKIO (api, api->cmd->config_clkio.output, api->cmd->config_clkio.pre_driver_str, api->cmd->config_clkio.driver_str );
     break;
    #endif /*     Si2168B_CONFIG_CLKIO_CMD */
    #ifdef        Si2168B_CONFIG_PINS_CMD
     case         Si2168B_CONFIG_PINS_CMD_CODE:
       return Si2168B_L1_CONFIG_PINS (api, api->cmd->config_pins.gpio0_mode, api->cmd->config_pins.gpio0_read, api->cmd->config_pins.gpio1_mode, api->cmd->config_pins.gpio1_read );
     break;
    #endif /*     Si2168B_CONFIG_PINS_CMD */
    #ifdef        Si2168B_DD_BER_CMD
     case         Si2168B_DD_BER_CMD_CODE:
       return Si2168B_L1_DD_BER (api, api->cmd->dd_ber.rst );
     break;
    #endif /*     Si2168B_DD_BER_CMD */
    #ifdef        Si2168B_DD_CBER_CMD
     case         Si2168B_DD_CBER_CMD_CODE:
       return Si2168B_L1_DD_CBER (api, api->cmd->dd_cber.rst );
     break;
    #endif /*     Si2168B_DD_CBER_CMD */

    #ifdef        Si2168B_DD_EXT_AGC_TER_CMD
     case         Si2168B_DD_EXT_AGC_TER_CMD_CODE:
       return Si2168B_L1_DD_EXT_AGC_TER (api, api->cmd->dd_ext_agc_ter.agc_1_mode, api->cmd->dd_ext_agc_ter.agc_1_inv, api->cmd->dd_ext_agc_ter.agc_2_mode, api->cmd->dd_ext_agc_ter.agc_2_inv, api->cmd->dd_ext_agc_ter.agc_1_kloop, api->cmd->dd_ext_agc_ter.agc_2_kloop, api->cmd->dd_ext_agc_ter.agc_1_min, api->cmd->dd_ext_agc_ter.agc_2_min );
     break;
    #endif /*     Si2168B_DD_EXT_AGC_TER_CMD */

    #ifdef        Si2168B_DD_FER_CMD
     case         Si2168B_DD_FER_CMD_CODE:
       return Si2168B_L1_DD_FER (api, api->cmd->dd_fer.rst );
     break;
    #endif /*     Si2168B_DD_FER_CMD */
    #ifdef        Si2168B_DD_GET_REG_CMD
     case         Si2168B_DD_GET_REG_CMD_CODE:
       return Si2168B_L1_DD_GET_REG (api, api->cmd->dd_get_reg.reg_code_lsb, api->cmd->dd_get_reg.reg_code_mid, api->cmd->dd_get_reg.reg_code_msb );
     break;
    #endif /*     Si2168B_DD_GET_REG_CMD */
    #ifdef        Si2168B_DD_MP_DEFAULTS_CMD
     case         Si2168B_DD_MP_DEFAULTS_CMD_CODE:
       return Si2168B_L1_DD_MP_DEFAULTS (api, api->cmd->dd_mp_defaults.mp_a_mode, api->cmd->dd_mp_defaults.mp_b_mode, api->cmd->dd_mp_defaults.mp_c_mode, api->cmd->dd_mp_defaults.mp_d_mode );
     break;
    #endif /*     Si2168B_DD_MP_DEFAULTS_CMD */
    #ifdef        Si2168B_DD_PER_CMD
     case         Si2168B_DD_PER_CMD_CODE:
       return Si2168B_L1_DD_PER (api, api->cmd->dd_per.rst );
     break;
    #endif /*     Si2168B_DD_PER_CMD */
    #ifdef        Si2168B_DD_RESTART_CMD
     case         Si2168B_DD_RESTART_CMD_CODE:
       return Si2168B_L1_DD_RESTART (api );
     break;
    #endif /*     Si2168B_DD_RESTART_CMD */
    #ifdef        Si2168B_DD_SET_REG_CMD
     case         Si2168B_DD_SET_REG_CMD_CODE:
       return Si2168B_L1_DD_SET_REG (api, api->cmd->dd_set_reg.reg_code_lsb, api->cmd->dd_set_reg.reg_code_mid, api->cmd->dd_set_reg.reg_code_msb, api->cmd->dd_set_reg.value );
     break;
    #endif /*     Si2168B_DD_SET_REG_CMD */
    #ifdef        Si2168B_DD_SSI_SQI_CMD
     case         Si2168B_DD_SSI_SQI_CMD_CODE:
       return Si2168B_L1_DD_SSI_SQI (api, api->cmd->dd_ssi_sqi.tuner_rssi );
     break;
    #endif /*     Si2168B_DD_SSI_SQI_CMD */
    #ifdef        Si2168B_DD_STATUS_CMD
     case         Si2168B_DD_STATUS_CMD_CODE:
       return Si2168B_L1_DD_STATUS (api, api->cmd->dd_status.intack );
     break;
    #endif /*     Si2168B_DD_STATUS_CMD */
    #ifdef        Si2168B_DD_UNCOR_CMD
     case         Si2168B_DD_UNCOR_CMD_CODE:
       return Si2168B_L1_DD_UNCOR (api, api->cmd->dd_uncor.rst );
     break;
    #endif /*     Si2168B_DD_UNCOR_CMD */
    #ifdef        Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD
     case         Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_CODE:
       return Si2168B_L1_DOWNLOAD_DATASET_CONTINUE (api, api->cmd->download_dataset_continue.data0, api->cmd->download_dataset_continue.data1, api->cmd->download_dataset_continue.data2, api->cmd->download_dataset_continue.data3, api->cmd->download_dataset_continue.data4, api->cmd->download_dataset_continue.data5, api->cmd->download_dataset_continue.data6 );
     break;
    #endif /*     Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD */
    #ifdef        Si2168B_DOWNLOAD_DATASET_START_CMD
     case         Si2168B_DOWNLOAD_DATASET_START_CMD_CODE:
       return Si2168B_L1_DOWNLOAD_DATASET_START (api, api->cmd->download_dataset_start.dataset_id, api->cmd->download_dataset_start.dataset_checksum, api->cmd->download_dataset_start.data0, api->cmd->download_dataset_start.data1, api->cmd->download_dataset_start.data2, api->cmd->download_dataset_start.data3, api->cmd->download_dataset_start.data4 );
     break;
    #endif /*     Si2168B_DOWNLOAD_DATASET_START_CMD */

    #ifdef        Si2168B_DVBC_STATUS_CMD
     case         Si2168B_DVBC_STATUS_CMD_CODE:
       return Si2168B_L1_DVBC_STATUS (api, api->cmd->dvbc_status.intack );
     break;
    #endif /*     Si2168B_DVBC_STATUS_CMD */


    #ifdef        Si2168B_DVBT2_FEF_CMD
     case         Si2168B_DVBT2_FEF_CMD_CODE:
       return Si2168B_L1_DVBT2_FEF (api, api->cmd->dvbt2_fef.fef_tuner_flag, api->cmd->dvbt2_fef.fef_tuner_flag_inv );
     break;
    #endif /*     Si2168B_DVBT2_FEF_CMD */
    #ifdef        Si2168B_DVBT2_PLP_INFO_CMD
     case         Si2168B_DVBT2_PLP_INFO_CMD_CODE:
       return Si2168B_L1_DVBT2_PLP_INFO (api, api->cmd->dvbt2_plp_info.plp_index );
     break;
    #endif /*     Si2168B_DVBT2_PLP_INFO_CMD */
    #ifdef        Si2168B_DVBT2_PLP_SELECT_CMD
     case         Si2168B_DVBT2_PLP_SELECT_CMD_CODE:
       return Si2168B_L1_DVBT2_PLP_SELECT (api, api->cmd->dvbt2_plp_select.plp_id, api->cmd->dvbt2_plp_select.plp_id_sel_mode );
     break;
    #endif /*     Si2168B_DVBT2_PLP_SELECT_CMD */
    #ifdef        Si2168B_DVBT2_STATUS_CMD
     case         Si2168B_DVBT2_STATUS_CMD_CODE:
       return Si2168B_L1_DVBT2_STATUS (api, api->cmd->dvbt2_status.intack );
     break;
    #endif /*     Si2168B_DVBT2_STATUS_CMD */
    #ifdef        Si2168B_DVBT2_TX_ID_CMD
     case         Si2168B_DVBT2_TX_ID_CMD_CODE:
       return Si2168B_L1_DVBT2_TX_ID (api );
     break;
    #endif /*     Si2168B_DVBT2_TX_ID_CMD */

    #ifdef        Si2168B_DVBT_STATUS_CMD
     case         Si2168B_DVBT_STATUS_CMD_CODE:
       return Si2168B_L1_DVBT_STATUS (api, api->cmd->dvbt_status.intack );
     break;
    #endif /*     Si2168B_DVBT_STATUS_CMD */
    #ifdef        Si2168B_DVBT_TPS_EXTRA_CMD
     case         Si2168B_DVBT_TPS_EXTRA_CMD_CODE:
       return Si2168B_L1_DVBT_TPS_EXTRA (api );
     break;
    #endif /*     Si2168B_DVBT_TPS_EXTRA_CMD */

    #ifdef        Si2168B_EXIT_BOOTLOADER_CMD
     case         Si2168B_EXIT_BOOTLOADER_CMD_CODE:
       return Si2168B_L1_EXIT_BOOTLOADER (api, api->cmd->exit_bootloader.func, api->cmd->exit_bootloader.ctsien );
     break;
    #endif /*     Si2168B_EXIT_BOOTLOADER_CMD */
    #ifdef        Si2168B_GET_PROPERTY_CMD
     case         Si2168B_GET_PROPERTY_CMD_CODE:
       return Si2168B_L1_GET_PROPERTY (api, api->cmd->get_property.reserved, api->cmd->get_property.prop );
     break;
    #endif /*     Si2168B_GET_PROPERTY_CMD */
    #ifdef        Si2168B_GET_REV_CMD
     case         Si2168B_GET_REV_CMD_CODE:
       return Si2168B_L1_GET_REV (api );
     break;
    #endif /*     Si2168B_GET_REV_CMD */
    #ifdef        Si2168B_I2C_PASSTHROUGH_CMD
     case         Si2168B_I2C_PASSTHROUGH_CMD_CODE:
       return Si2168B_L1_I2C_PASSTHROUGH (api, api->cmd->i2c_passthrough.subcode, api->cmd->i2c_passthrough.i2c_passthru, api->cmd->i2c_passthrough.reserved );
     break;
    #endif /*     Si2168B_I2C_PASSTHROUGH_CMD */
    #ifdef        Si2168B_MCNS_STATUS_CMD
     case         Si2168B_MCNS_STATUS_CMD_CODE:
       return Si2168B_L1_MCNS_STATUS (api, api->cmd->mcns_status.intack );
     break;
    #endif /*     Si2168B_MCNS_STATUS_CMD */

    #ifdef        Si2168B_PART_INFO_CMD
     case         Si2168B_PART_INFO_CMD_CODE:
       return Si2168B_L1_PART_INFO (api );
     break;
    #endif /*     Si2168B_PART_INFO_CMD */
    #ifdef        Si2168B_POWER_DOWN_CMD
     case         Si2168B_POWER_DOWN_CMD_CODE:
       return Si2168B_L1_POWER_DOWN (api );
     break;
    #endif /*     Si2168B_POWER_DOWN_CMD */
    #ifdef        Si2168B_POWER_UP_CMD
     case         Si2168B_POWER_UP_CMD_CODE:
       return Si2168B_L1_POWER_UP (api, api->cmd->power_up.subcode, api->cmd->power_up.reset, api->cmd->power_up.reserved2, api->cmd->power_up.reserved4, api->cmd->power_up.reserved1, api->cmd->power_up.addr_mode, api->cmd->power_up.reserved5, api->cmd->power_up.func, api->cmd->power_up.clock_freq, api->cmd->power_up.ctsien, api->cmd->power_up.wake_up );
     break;
    #endif /*     Si2168B_POWER_UP_CMD */
    #ifdef        Si2168B_RSSI_ADC_CMD
     case         Si2168B_RSSI_ADC_CMD_CODE:
       return Si2168B_L1_RSSI_ADC (api, api->cmd->rssi_adc.on_off );
     break;
    #endif /*     Si2168B_RSSI_ADC_CMD */
    #ifdef        Si2168B_SCAN_CTRL_CMD
     case         Si2168B_SCAN_CTRL_CMD_CODE:
       return Si2168B_L1_SCAN_CTRL (api, api->cmd->scan_ctrl.action, api->cmd->scan_ctrl.tuned_rf_freq );
     break;
    #endif /*     Si2168B_SCAN_CTRL_CMD */
    #ifdef        Si2168B_SCAN_STATUS_CMD
     case         Si2168B_SCAN_STATUS_CMD_CODE:
       return Si2168B_L1_SCAN_STATUS (api, api->cmd->scan_status.intack );
     break;
    #endif /*     Si2168B_SCAN_STATUS_CMD */
    #ifdef        Si2168B_SET_PROPERTY_CMD
     case         Si2168B_SET_PROPERTY_CMD_CODE:
       return Si2168B_L1_SET_PROPERTY (api, api->cmd->set_property.reserved, api->cmd->set_property.prop, api->cmd->set_property.data );
     break;
    #endif /*     Si2168B_SET_PROPERTY_CMD */
    #ifdef        Si2168B_SPI_LINK_CMD
     case         Si2168B_SPI_LINK_CMD_CODE:
       return Si2168B_L1_SPI_LINK (api, api->cmd->spi_link.subcode, api->cmd->spi_link.spi_pbl_key, api->cmd->spi_link.spi_pbl_num, api->cmd->spi_link.spi_conf_clk, api->cmd->spi_link.spi_clk_pola, api->cmd->spi_link.spi_conf_data, api->cmd->spi_link.spi_data_dir, api->cmd->spi_link.spi_enable );
     break;
    #endif /*     Si2168B_SPI_LINK_CMD */
    #ifdef        Si2168B_SPI_PASSTHROUGH_CMD
     case         Si2168B_SPI_PASSTHROUGH_CMD_CODE:
       return Si2168B_L1_SPI_PASSTHROUGH (api, api->cmd->spi_passthrough.subcode, api->cmd->spi_passthrough.spi_passthr_clk, api->cmd->spi_passthrough.spi_passth_data );
     break;
    #endif /*     Si2168B_SPI_PASSTHROUGH_CMD */
    #ifdef        Si2168B_START_CLK_CMD
     case         Si2168B_START_CLK_CMD_CODE:
       return Si2168B_L1_START_CLK (api, api->cmd->start_clk.subcode, api->cmd->start_clk.reserved1, api->cmd->start_clk.tune_cap, api->cmd->start_clk.reserved2, api->cmd->start_clk.clk_mode, api->cmd->start_clk.reserved3, api->cmd->start_clk.reserved4, api->cmd->start_clk.start_clk );
     break;
    #endif /*     Si2168B_START_CLK_CMD */
   default : break;
    }
     return 0;
  }

#ifdef    Si2168B_GET_COMMAND_STRINGS

  /* --------------------------------------------*/
  /* GET_COMMAND_RESPONSE_STRING FUNCTION        */
  /* --------------------------------------------*/
unsigned char   Si2168B_L1_GetCommandResponseString(L1_Si2168B_Context *api, unsigned int cmd_code, const char *separator, char *msg) {
    switch (cmd_code) {
    #ifdef        Si2168B_CONFIG_CLKIO_CMD
     case         Si2168B_CONFIG_CLKIO_CMD_CODE:
      sprintf(msg,"CONFIG_CLKIO ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT          ",1000);
           if  (api->rsp->config_clkio.STATUS->ddint          ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->config_clkio.STATUS->ddint          ==     1) strcat(msg,"TRIGGERED    ");
      else                                                             sprintf(msg,1000,"%s%d", msg, api->rsp->config_clkio.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT        ",1000);
           if  (api->rsp->config_clkio.STATUS->scanint        ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->config_clkio.STATUS->scanint        ==     1) strcat(msg,"TRIGGERED    ");
      else                                                             sprintf(msg,1000,"%s%d", msg, api->rsp->config_clkio.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR            ",1000);
           if  (api->rsp->config_clkio.STATUS->err            ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->config_clkio.STATUS->err            ==     0) strcat(msg,"NO_ERROR");
      else                                                             sprintf(msg,1000,"%s%d", msg, api->rsp->config_clkio.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS            ",1000);
           if  (api->rsp->config_clkio.STATUS->cts            ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->config_clkio.STATUS->cts            ==     0) strcat(msg,"WAIT     ");
      else                                                             sprintf(msg,1000,"%s%d", msg, api->rsp->config_clkio.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MODE           ",1000);
           if  (api->rsp->config_clkio.mode           ==     2) strcat(msg,"CLK_INPUT ");
      else if  (api->rsp->config_clkio.mode           ==     1) strcat(msg,"CLK_OUTPUT");
      else if  (api->rsp->config_clkio.mode           ==     0) strcat(msg,"UNUSED    ");
      else                                                     sprintf(msg,1000,"%s%d", msg, api->rsp->config_clkio.mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PRE_DRIVER_STR ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->config_clkio.pre_driver_str);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DRIVER_STR     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->config_clkio.driver_str);
     break;
    #endif /*     Si2168B_CONFIG_CLKIO_CMD */

    #ifdef        Si2168B_CONFIG_PINS_CMD
     case         Si2168B_CONFIG_PINS_CMD_CODE:
      sprintf(msg,"CONFIG_PINS ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT       ",1000);
           if  (api->rsp->config_pins.STATUS->ddint       ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->config_pins.STATUS->ddint       ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->config_pins.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT     ",1000);
           if  (api->rsp->config_pins.STATUS->scanint     ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->config_pins.STATUS->scanint     ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->config_pins.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR         ",1000);
           if  (api->rsp->config_pins.STATUS->err         ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->config_pins.STATUS->err         ==     0) strcat(msg,"NO_ERROR");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->config_pins.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS         ",1000);
           if  (api->rsp->config_pins.STATUS->cts         ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->config_pins.STATUS->cts         ==     0) strcat(msg,"WAIT     ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->config_pins.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-GPIO0_MODE  ",1000);
           if  (api->rsp->config_pins.gpio0_mode  ==     1) strcat(msg,"DISABLE ");
      else if  (api->rsp->config_pins.gpio0_mode  ==     2) strcat(msg,"DRIVE_0 ");
      else if  (api->rsp->config_pins.gpio0_mode  ==     3) strcat(msg,"DRIVE_1 ");
      else if  (api->rsp->config_pins.gpio0_mode  ==     8) strcat(msg,"HW_LOCK ");
      else if  (api->rsp->config_pins.gpio0_mode  ==     7) strcat(msg,"INT_FLAG");
      else if  (api->rsp->config_pins.gpio0_mode  ==     4) strcat(msg,"TS_ERR  ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->config_pins.gpio0_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-GPIO0_STATE ",1000);
           if  (api->rsp->config_pins.gpio0_state ==     0) strcat(msg,"READ_0");
      else if  (api->rsp->config_pins.gpio0_state ==     1) strcat(msg,"READ_1");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->config_pins.gpio0_state);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-GPIO1_MODE  ",1000);
           if  (api->rsp->config_pins.gpio1_mode  ==     1) strcat(msg,"DISABLE ");
      else if  (api->rsp->config_pins.gpio1_mode  ==     2) strcat(msg,"DRIVE_0 ");
      else if  (api->rsp->config_pins.gpio1_mode  ==     3) strcat(msg,"DRIVE_1 ");
      else if  (api->rsp->config_pins.gpio1_mode  ==     8) strcat(msg,"HW_LOCK ");
      else if  (api->rsp->config_pins.gpio1_mode  ==     7) strcat(msg,"INT_FLAG");
      else if  (api->rsp->config_pins.gpio1_mode  ==     4) strcat(msg,"TS_ERR  ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->config_pins.gpio1_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-GPIO1_STATE ",1000);
           if  (api->rsp->config_pins.gpio1_state ==     0) strcat(msg,"READ_0");
      else if  (api->rsp->config_pins.gpio1_state ==     1) strcat(msg,"READ_1");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->config_pins.gpio1_state);
     break;
    #endif /*     Si2168B_CONFIG_PINS_CMD */

    #ifdef        Si2168B_DD_BER_CMD
     case         Si2168B_DD_BER_CMD_CODE:
      sprintf(msg,"DD_BER ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dd_ber.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_ber.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ber.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dd_ber.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_ber.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ber.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dd_ber.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_ber.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ber.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dd_ber.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_ber.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ber.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-EXP     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ber.exp);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MANT    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ber.mant);
     break;
    #endif /*     Si2168B_DD_BER_CMD */

    #ifdef        Si2168B_DD_CBER_CMD
     case         Si2168B_DD_CBER_CMD_CODE:
      sprintf(msg,"DD_CBER ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dd_cber.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_cber.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->dd_cber.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dd_cber.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_cber.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->dd_cber.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dd_cber.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_cber.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->dd_cber.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dd_cber.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_cber.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->dd_cber.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-EXP     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_cber.exp);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MANT    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_cber.mant);
     break;
    #endif /*     Si2168B_DD_CBER_CMD */


    #ifdef        Si2168B_DD_EXT_AGC_TER_CMD
     case         Si2168B_DD_EXT_AGC_TER_CMD_CODE:
      sprintf(msg,"DD_EXT_AGC_TER ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT       ",1000);
           if  (api->rsp->dd_ext_agc_ter.STATUS->ddint       ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_ext_agc_ter.STATUS->ddint       ==     1) strcat(msg,"TRIGGERED    ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ext_agc_ter.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT     ",1000);
           if  (api->rsp->dd_ext_agc_ter.STATUS->scanint     ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_ext_agc_ter.STATUS->scanint     ==     1) strcat(msg,"TRIGGERED    ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ext_agc_ter.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR         ",1000);
           if  (api->rsp->dd_ext_agc_ter.STATUS->err         ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_ext_agc_ter.STATUS->err         ==     0) strcat(msg,"NO_ERROR");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ext_agc_ter.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS         ",1000);
           if  (api->rsp->dd_ext_agc_ter.STATUS->cts         ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_ext_agc_ter.STATUS->cts         ==     0) strcat(msg,"WAIT     ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ext_agc_ter.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-AGC_1_LEVEL ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ext_agc_ter.agc_1_level);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-AGC_2_LEVEL ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ext_agc_ter.agc_2_level);
     break;
    #endif /*     Si2168B_DD_EXT_AGC_TER_CMD */


    #ifdef        Si2168B_DD_FER_CMD
     case         Si2168B_DD_FER_CMD_CODE:
      sprintf(msg,"DD_FER ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dd_fer.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_fer.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_fer.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dd_fer.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_fer.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_fer.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dd_fer.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_fer.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_fer.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dd_fer.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_fer.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_fer.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-EXP     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_fer.exp);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MANT    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_fer.mant);
     break;
    #endif /*     Si2168B_DD_FER_CMD */

    #ifdef        Si2168B_DD_GET_REG_CMD
     case         Si2168B_DD_GET_REG_CMD_CODE:
      sprintf(msg,"DD_GET_REG ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dd_get_reg.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_get_reg.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_get_reg.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dd_get_reg.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_get_reg.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_get_reg.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dd_get_reg.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_get_reg.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_get_reg.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dd_get_reg.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_get_reg.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_get_reg.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DATA1   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_get_reg.data1);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DATA2   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_get_reg.data2);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DATA3   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_get_reg.data3);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DATA4   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_get_reg.data4);
     break;
    #endif /*     Si2168B_DD_GET_REG_CMD */

    #ifdef        Si2168B_DD_MP_DEFAULTS_CMD
     case         Si2168B_DD_MP_DEFAULTS_CMD_CODE:
      sprintf(msg,"DD_MP_DEFAULTS ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT     ",1000);
           if  (api->rsp->dd_mp_defaults.STATUS->ddint     ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_mp_defaults.STATUS->ddint     ==     1) strcat(msg,"TRIGGERED    ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dd_mp_defaults.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT   ",1000);
           if  (api->rsp->dd_mp_defaults.STATUS->scanint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_mp_defaults.STATUS->scanint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dd_mp_defaults.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR       ",1000);
           if  (api->rsp->dd_mp_defaults.STATUS->err       ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_mp_defaults.STATUS->err       ==     0) strcat(msg,"NO_ERROR");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dd_mp_defaults.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS       ",1000);
           if  (api->rsp->dd_mp_defaults.STATUS->cts       ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_mp_defaults.STATUS->cts       ==     0) strcat(msg,"WAIT     ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dd_mp_defaults.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MP_A_MODE ",1000);
           if  (api->rsp->dd_mp_defaults.mp_a_mode ==     3) strcat(msg,"AGC_1         ");
      else if  (api->rsp->dd_mp_defaults.mp_a_mode ==     4) strcat(msg,"AGC_1_INVERTED");
      else if  (api->rsp->dd_mp_defaults.mp_a_mode ==     5) strcat(msg,"AGC_2         ");
      else if  (api->rsp->dd_mp_defaults.mp_a_mode ==     6) strcat(msg,"AGC_2_INVERTED");
      else if  (api->rsp->dd_mp_defaults.mp_a_mode ==     0) strcat(msg,"DISABLE       ");
      else if  (api->rsp->dd_mp_defaults.mp_a_mode ==     1) strcat(msg,"DRIVE_0       ");
      else if  (api->rsp->dd_mp_defaults.mp_a_mode ==     2) strcat(msg,"DRIVE_1       ");
      else if  (api->rsp->dd_mp_defaults.mp_a_mode ==     7) strcat(msg,"FEF           ");
      else if  (api->rsp->dd_mp_defaults.mp_a_mode ==     8) strcat(msg,"FEF_INVERTED  ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->dd_mp_defaults.mp_a_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MP_B_MODE ",1000);
           if  (api->rsp->dd_mp_defaults.mp_b_mode ==     3) strcat(msg,"AGC_1         ");
      else if  (api->rsp->dd_mp_defaults.mp_b_mode ==     4) strcat(msg,"AGC_1_INVERTED");
      else if  (api->rsp->dd_mp_defaults.mp_b_mode ==     5) strcat(msg,"AGC_2         ");
      else if  (api->rsp->dd_mp_defaults.mp_b_mode ==     6) strcat(msg,"AGC_2_INVERTED");
      else if  (api->rsp->dd_mp_defaults.mp_b_mode ==     0) strcat(msg,"DISABLE       ");
      else if  (api->rsp->dd_mp_defaults.mp_b_mode ==     1) strcat(msg,"DRIVE_0       ");
      else if  (api->rsp->dd_mp_defaults.mp_b_mode ==     2) strcat(msg,"DRIVE_1       ");
      else if  (api->rsp->dd_mp_defaults.mp_b_mode ==     7) strcat(msg,"FEF           ");
      else if  (api->rsp->dd_mp_defaults.mp_b_mode ==     8) strcat(msg,"FEF_INVERTED  ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->dd_mp_defaults.mp_b_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MP_C_MODE ",1000);
           if  (api->rsp->dd_mp_defaults.mp_c_mode ==     3) strcat(msg,"AGC_1         ");
      else if  (api->rsp->dd_mp_defaults.mp_c_mode ==     4) strcat(msg,"AGC_1_INVERTED");
      else if  (api->rsp->dd_mp_defaults.mp_c_mode ==     5) strcat(msg,"AGC_2         ");
      else if  (api->rsp->dd_mp_defaults.mp_c_mode ==     6) strcat(msg,"AGC_2_INVERTED");
      else if  (api->rsp->dd_mp_defaults.mp_c_mode ==     0) strcat(msg,"DISABLE       ");
      else if  (api->rsp->dd_mp_defaults.mp_c_mode ==     1) strcat(msg,"DRIVE_0       ");
      else if  (api->rsp->dd_mp_defaults.mp_c_mode ==     2) strcat(msg,"DRIVE_1       ");
      else if  (api->rsp->dd_mp_defaults.mp_c_mode ==     7) strcat(msg,"FEF           ");
      else if  (api->rsp->dd_mp_defaults.mp_c_mode ==     8) strcat(msg,"FEF_INVERTED  ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->dd_mp_defaults.mp_c_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MP_D_MODE ",1000);
           if  (api->rsp->dd_mp_defaults.mp_d_mode ==     3) strcat(msg,"AGC_1         ");
      else if  (api->rsp->dd_mp_defaults.mp_d_mode ==     4) strcat(msg,"AGC_1_INVERTED");
      else if  (api->rsp->dd_mp_defaults.mp_d_mode ==     5) strcat(msg,"AGC_2         ");
      else if  (api->rsp->dd_mp_defaults.mp_d_mode ==     6) strcat(msg,"AGC_2_INVERTED");
      else if  (api->rsp->dd_mp_defaults.mp_d_mode ==     0) strcat(msg,"DISABLE       ");
      else if  (api->rsp->dd_mp_defaults.mp_d_mode ==     1) strcat(msg,"DRIVE_0       ");
      else if  (api->rsp->dd_mp_defaults.mp_d_mode ==     2) strcat(msg,"DRIVE_1       ");
      else if  (api->rsp->dd_mp_defaults.mp_d_mode ==     7) strcat(msg,"FEF           ");
      else if  (api->rsp->dd_mp_defaults.mp_d_mode ==     8) strcat(msg,"FEF_INVERTED  ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->dd_mp_defaults.mp_d_mode);
     break;
    #endif /*     Si2168B_DD_MP_DEFAULTS_CMD */

    #ifdef        Si2168B_DD_PER_CMD
     case         Si2168B_DD_PER_CMD_CODE:
      sprintf(msg,"DD_PER ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dd_per.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_per.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_per.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dd_per.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_per.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_per.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dd_per.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_per.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_per.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dd_per.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_per.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_per.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-EXP     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_per.exp);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MANT    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_per.mant);
     break;
    #endif /*     Si2168B_DD_PER_CMD */

    #ifdef        Si2168B_DD_RESTART_CMD
     case         Si2168B_DD_RESTART_CMD_CODE:
      sprintf(msg,"DD_RESTART ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dd_restart.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_restart.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_restart.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dd_restart.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_restart.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_restart.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dd_restart.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_restart.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_restart.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dd_restart.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_restart.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_restart.STATUS->cts);
     break;
    #endif /*     Si2168B_DD_RESTART_CMD */

    #ifdef        Si2168B_DD_SET_REG_CMD
     case         Si2168B_DD_SET_REG_CMD_CODE:
      sprintf(msg,"DD_SET_REG ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dd_set_reg.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_set_reg.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_set_reg.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dd_set_reg.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_set_reg.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_set_reg.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dd_set_reg.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_set_reg.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_set_reg.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dd_set_reg.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_set_reg.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_set_reg.STATUS->cts);
     break;
    #endif /*     Si2168B_DD_SET_REG_CMD */

    #ifdef        Si2168B_DD_SSI_SQI_CMD
     case         Si2168B_DD_SSI_SQI_CMD_CODE:
      sprintf(msg,"DD_SSI_SQI ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dd_ssi_sqi.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_ssi_sqi.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ssi_sqi.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dd_ssi_sqi.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_ssi_sqi.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ssi_sqi.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dd_ssi_sqi.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_ssi_sqi.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ssi_sqi.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dd_ssi_sqi.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_ssi_sqi.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ssi_sqi.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SSI     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ssi_sqi.ssi);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SQI     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_ssi_sqi.sqi);
     break;
    #endif /*     Si2168B_DD_SSI_SQI_CMD */

    #ifdef        Si2168B_DD_STATUS_CMD
     case         Si2168B_DD_STATUS_CMD_CODE:
      sprintf(msg,"DD_STATUS ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT        ",1000);
           if  (api->rsp->dd_status.STATUS->ddint        ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_status.STATUS->ddint        ==     1) strcat(msg,"TRIGGERED    ");
      else                                                        sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT      ",1000);
           if  (api->rsp->dd_status.STATUS->scanint      ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_status.STATUS->scanint      ==     1) strcat(msg,"TRIGGERED    ");
      else                                                        sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR          ",1000);
           if  (api->rsp->dd_status.STATUS->err          ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_status.STATUS->err          ==     0) strcat(msg,"NO_ERROR");
      else                                                        sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS          ",1000);
           if  (api->rsp->dd_status.STATUS->cts          ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_status.STATUS->cts          ==     0) strcat(msg,"WAIT     ");
      else                                                        sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCLINT       ",1000);
           if  (api->rsp->dd_status.pclint       ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dd_status.pclint       ==     0) strcat(msg,"NO_CHANGE");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.pclint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DLINT        ",1000);
           if  (api->rsp->dd_status.dlint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dd_status.dlint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.dlint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BERINT       ",1000);
           if  (api->rsp->dd_status.berint       ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dd_status.berint       ==     0) strcat(msg,"NO_CHANGE");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.berint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCORINT     ",1000);
           if  (api->rsp->dd_status.uncorint     ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dd_status.uncorint     ==     0) strcat(msg,"NO_CHANGE");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.uncorint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RSQINT_BIT5  ",1000);
           if  (api->rsp->dd_status.rsqint_bit5  ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dd_status.rsqint_bit5  ==     0) strcat(msg,"NO_CHANGE");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.rsqint_bit5);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RSQINT_BIT6  ",1000);
           if  (api->rsp->dd_status.rsqint_bit6  ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dd_status.rsqint_bit6  ==     0) strcat(msg,"NO_CHANGE");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.rsqint_bit6);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RSQINT_BIT7  ",1000);
           if  (api->rsp->dd_status.rsqint_bit7  ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dd_status.rsqint_bit7  ==     0) strcat(msg,"NO_CHANGE");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.rsqint_bit7);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCL          ",1000);
           if  (api->rsp->dd_status.pcl          ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->dd_status.pcl          ==     0) strcat(msg,"NO_LOCK");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.pcl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DL           ",1000);
           if  (api->rsp->dd_status.dl           ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->dd_status.dl           ==     0) strcat(msg,"NO_LOCK");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.dl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BER          ",1000);
           if  (api->rsp->dd_status.ber          ==     1) strcat(msg,"BER_ABOVE");
      else if  (api->rsp->dd_status.ber          ==     0) strcat(msg,"BER_BELOW");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.ber);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCOR        ",1000);
           if  (api->rsp->dd_status.uncor        ==     0) strcat(msg,"NO_UNCOR_FOUND");
      else if  (api->rsp->dd_status.uncor        ==     1) strcat(msg,"UNCOR_FOUND   ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.uncor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RSQSTAT_BIT5 ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.rsqstat_bit5);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RSQSTAT_BIT6 ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.rsqstat_bit6);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RSQSTAT_BIT7 ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.rsqstat_bit7);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MODULATION   ",1000);
           if  (api->rsp->dd_status.modulation   ==    10) strcat(msg,"DSS  ");
      else if  (api->rsp->dd_status.modulation   ==     3) strcat(msg,"DVBC ");
      else if  (api->rsp->dd_status.modulation   ==    11) strcat(msg,"DVBC2");
      else if  (api->rsp->dd_status.modulation   ==     8) strcat(msg,"DVBS ");
      else if  (api->rsp->dd_status.modulation   ==     9) strcat(msg,"DVBS2");
      else if  (api->rsp->dd_status.modulation   ==     2) strcat(msg,"DVBT ");
      else if  (api->rsp->dd_status.modulation   ==     7) strcat(msg,"DVBT2");
      else if  (api->rsp->dd_status.modulation   ==     1) strcat(msg,"MCNS ");
      else                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.modulation);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TS_BIT_RATE  ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.ts_bit_rate);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TS_CLK_FREQ  ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_status.ts_clk_freq);
     break;
    #endif /*     Si2168B_DD_STATUS_CMD */

    #ifdef        Si2168B_DD_UNCOR_CMD
     case         Si2168B_DD_UNCOR_CMD_CODE:
      sprintf(msg,"DD_UNCOR ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT     ",1000);
           if  (api->rsp->dd_uncor.STATUS->ddint     ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_uncor.STATUS->ddint     ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_uncor.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT   ",1000);
           if  (api->rsp->dd_uncor.STATUS->scanint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dd_uncor.STATUS->scanint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_uncor.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR       ",1000);
           if  (api->rsp->dd_uncor.STATUS->err       ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dd_uncor.STATUS->err       ==     0) strcat(msg,"NO_ERROR");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_uncor.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS       ",1000);
           if  (api->rsp->dd_uncor.STATUS->cts       ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dd_uncor.STATUS->cts       ==     0) strcat(msg,"WAIT     ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dd_uncor.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCOR_LSB ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_uncor.uncor_lsb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCOR_MSB ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dd_uncor.uncor_msb);
     break;
    #endif /*     Si2168B_DD_UNCOR_CMD */

    #ifdef        Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD
     case         Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD_CODE:
      sprintf(msg,"DOWNLOAD_DATASET_CONTINUE ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->download_dataset_continue.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->download_dataset_continue.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->download_dataset_continue.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->download_dataset_continue.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->download_dataset_continue.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->download_dataset_continue.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->download_dataset_continue.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->download_dataset_continue.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->download_dataset_continue.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->download_dataset_continue.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->download_dataset_continue.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->download_dataset_continue.STATUS->cts);
     break;
    #endif /*     Si2168B_DOWNLOAD_DATASET_CONTINUE_CMD */

    #ifdef        Si2168B_DOWNLOAD_DATASET_START_CMD
     case         Si2168B_DOWNLOAD_DATASET_START_CMD_CODE:
      sprintf(msg,"DOWNLOAD_DATASET_START ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->download_dataset_start.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->download_dataset_start.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                                sprintf(msg,1000,"%s%d", msg, api->rsp->download_dataset_start.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->download_dataset_start.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->download_dataset_start.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                                sprintf(msg,1000,"%s%d", msg, api->rsp->download_dataset_start.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->download_dataset_start.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->download_dataset_start.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                                sprintf(msg,1000,"%s%d", msg, api->rsp->download_dataset_start.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->download_dataset_start.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->download_dataset_start.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                                sprintf(msg,1000,"%s%d", msg, api->rsp->download_dataset_start.STATUS->cts);
     break;
    #endif /*     Si2168B_DOWNLOAD_DATASET_START_CMD */


    #ifdef        Si2168B_DVBC_STATUS_CMD
     case         Si2168B_DVBC_STATUS_CMD_CODE:
      sprintf(msg,"DVBC_STATUS ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT         ",1000);
           if  (api->rsp->dvbc_status.STATUS->ddint         ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbc_status.STATUS->ddint         ==     1) strcat(msg,"TRIGGERED    ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT       ",1000);
           if  (api->rsp->dvbc_status.STATUS->scanint       ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbc_status.STATUS->scanint       ==     1) strcat(msg,"TRIGGERED    ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR           ",1000);
           if  (api->rsp->dvbc_status.STATUS->err           ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dvbc_status.STATUS->err           ==     0) strcat(msg,"NO_ERROR");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS           ",1000);
           if  (api->rsp->dvbc_status.STATUS->cts           ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dvbc_status.STATUS->cts           ==     0) strcat(msg,"WAIT     ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCLINT        ",1000);
           if  (api->rsp->dvbc_status.pclint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbc_status.pclint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.pclint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DLINT         ",1000);
           if  (api->rsp->dvbc_status.dlint         ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbc_status.dlint         ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.dlint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BERINT        ",1000);
           if  (api->rsp->dvbc_status.berint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbc_status.berint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.berint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCORINT      ",1000);
           if  (api->rsp->dvbc_status.uncorint      ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbc_status.uncorint      ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.uncorint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCL           ",1000);
           if  (api->rsp->dvbc_status.pcl           ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->dvbc_status.pcl           ==     0) strcat(msg,"NO_LOCK");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.pcl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DL            ",1000);
           if  (api->rsp->dvbc_status.dl            ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->dvbc_status.dl            ==     0) strcat(msg,"NO_LOCK");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.dl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BER           ",1000);
           if  (api->rsp->dvbc_status.ber           ==     1) strcat(msg,"BER_ABOVE");
      else if  (api->rsp->dvbc_status.ber           ==     0) strcat(msg,"BER_BELOW");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.ber);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCOR         ",1000);
           if  (api->rsp->dvbc_status.uncor         ==     0) strcat(msg,"NO_UNCOR_FOUND");
      else if  (api->rsp->dvbc_status.uncor         ==     1) strcat(msg,"UNCOR_FOUND   ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.uncor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CNR           ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.cnr);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-AFC_FREQ      ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.afc_freq);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TIMING_OFFSET ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.timing_offset);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CONSTELLATION ",1000);
           if  (api->rsp->dvbc_status.constellation ==    10) strcat(msg,"QAM128");
      else if  (api->rsp->dvbc_status.constellation ==     7) strcat(msg,"QAM16 ");
      else if  (api->rsp->dvbc_status.constellation ==    11) strcat(msg,"QAM256");
      else if  (api->rsp->dvbc_status.constellation ==     8) strcat(msg,"QAM32 ");
      else if  (api->rsp->dvbc_status.constellation ==     9) strcat(msg,"QAM64 ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.constellation);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SP_INV        ",1000);
           if  (api->rsp->dvbc_status.sp_inv        ==     1) strcat(msg,"INVERTED");
      else if  (api->rsp->dvbc_status.sp_inv        ==     0) strcat(msg,"NORMAL  ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbc_status.sp_inv);
     break;
    #endif /*     Si2168B_DVBC_STATUS_CMD */



    #ifdef        Si2168B_DVBT2_FEF_CMD
     case         Si2168B_DVBT2_FEF_CMD_CODE:
      sprintf(msg,"DVBT2_FEF ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT          ",1000);
           if  (api->rsp->dvbt2_fef.STATUS->ddint          ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_fef.STATUS->ddint          ==     1) strcat(msg,"TRIGGERED    ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_fef.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT        ",1000);
           if  (api->rsp->dvbt2_fef.STATUS->scanint        ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_fef.STATUS->scanint        ==     1) strcat(msg,"TRIGGERED    ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_fef.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR            ",1000);
           if  (api->rsp->dvbt2_fef.STATUS->err            ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dvbt2_fef.STATUS->err            ==     0) strcat(msg,"NO_ERROR");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_fef.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS            ",1000);
           if  (api->rsp->dvbt2_fef.STATUS->cts            ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dvbt2_fef.STATUS->cts            ==     0) strcat(msg,"WAIT     ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_fef.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FEF_TYPE       ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_fef.fef_type);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FEF_LENGTH     ",1000); sprintf(msg,1000,"%s%ld", msg, api->rsp->dvbt2_fef.fef_length);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FEF_REPETITION ",1000); sprintf(msg,1000,"%s%ld", msg, api->rsp->dvbt2_fef.fef_repetition);
     break;
    #endif /*     Si2168B_DVBT2_FEF_CMD */

    #ifdef        Si2168B_DVBT2_PLP_INFO_CMD
     case         Si2168B_DVBT2_PLP_INFO_CMD_CODE:
      sprintf(msg,"DVBT2_PLP_INFO ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT                  ",1000);
           if  (api->rsp->dvbt2_plp_info.STATUS->ddint                  ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_plp_info.STATUS->ddint                  ==     1) strcat(msg,"TRIGGERED    ");
      else                                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT                ",1000);
           if  (api->rsp->dvbt2_plp_info.STATUS->scanint                ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_plp_info.STATUS->scanint                ==     1) strcat(msg,"TRIGGERED    ");
      else                                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR                    ",1000);
           if  (api->rsp->dvbt2_plp_info.STATUS->err                    ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dvbt2_plp_info.STATUS->err                    ==     0) strcat(msg,"NO_ERROR");
      else                                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS                    ",1000);
           if  (api->rsp->dvbt2_plp_info.STATUS->cts                    ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dvbt2_plp_info.STATUS->cts                    ==     0) strcat(msg,"WAIT     ");
      else                                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_ID                 ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_id);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_PAYLOAD_TYPE       ",1000);
           if  (api->rsp->dvbt2_plp_info.plp_payload_type       ==     1) strcat(msg,"GCS ");
      else if  (api->rsp->dvbt2_plp_info.plp_payload_type       ==     0) strcat(msg,"GFPS");
      else if  (api->rsp->dvbt2_plp_info.plp_payload_type       ==     2) strcat(msg,"GSE ");
      else if  (api->rsp->dvbt2_plp_info.plp_payload_type       ==     3) strcat(msg,"TS  ");
      else                                                               sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_payload_type);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_TYPE               ",1000);
           if  (api->rsp->dvbt2_plp_info.plp_type               ==     0) strcat(msg,"COMMON    ");
      else if  (api->rsp->dvbt2_plp_info.plp_type               ==     1) strcat(msg,"Data_Type1");
      else if  (api->rsp->dvbt2_plp_info.plp_type               ==     2) strcat(msg,"Data_Type2");
      else                                                               sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_type);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FIRST_FRAME_IDX_MSB    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.first_frame_idx_msb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FIRST_RF_IDX           ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.first_rf_idx);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FF_FLAG                ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.ff_flag);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_GROUP_ID_MSB       ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_group_id_msb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FIRST_FRAME_IDX_LSB    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.first_frame_idx_lsb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_MOD_MSB            ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_mod_msb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_COD                ",1000);
           if  (api->rsp->dvbt2_plp_info.plp_cod                ==     0) strcat(msg,"1_2");
      else if  (api->rsp->dvbt2_plp_info.plp_cod                ==     2) strcat(msg,"2_3");
      else if  (api->rsp->dvbt2_plp_info.plp_cod                ==     3) strcat(msg,"3_4");
      else if  (api->rsp->dvbt2_plp_info.plp_cod                ==     1) strcat(msg,"3_5");
      else if  (api->rsp->dvbt2_plp_info.plp_cod                ==     4) strcat(msg,"4_5");
      else if  (api->rsp->dvbt2_plp_info.plp_cod                ==     5) strcat(msg,"5_6");
      else                                                               sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_cod);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_GROUP_ID_LSB       ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_group_id_lsb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_NUM_BLOCKS_MAX_MSB ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_num_blocks_max_msb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_FEC_TYPE           ",1000);
           if  (api->rsp->dvbt2_plp_info.plp_fec_type           ==     0) strcat(msg,"16K_LDPC");
      else if  (api->rsp->dvbt2_plp_info.plp_fec_type           ==     1) strcat(msg,"64K_LDPC");
      else                                                               sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_fec_type);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_ROT                ",1000);
           if  (api->rsp->dvbt2_plp_info.plp_rot                ==     0) strcat(msg,"NOT_ROTATED");
      else if  (api->rsp->dvbt2_plp_info.plp_rot                ==     1) strcat(msg,"ROTATED    ");
      else                                                               sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_rot);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_MOD_LSB            ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_mod_lsb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FRAME_INTERVAL_MSB     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.frame_interval_msb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_NUM_BLOCKS_MAX_LSB ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_num_blocks_max_lsb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TIME_IL_LENGTH_MSB     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.time_il_length_msb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FRAME_INTERVAL_LSB     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.frame_interval_lsb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TIME_IL_TYPE           ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.time_il_type);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TIME_IL_LENGTH_LSB     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.time_il_length_lsb);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RESERVED_1_1           ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.reserved_1_1);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-IN_BAND_B_FLAG         ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.in_band_b_flag);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-IN_BAND_A_FLAG         ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.in_band_a_flag);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-STATIC_FLAG            ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.static_flag);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_MODE               ",1000);
           if  (api->rsp->dvbt2_plp_info.plp_mode               ==     2) strcat(msg,"HIGH_EFFICIENCY_MODE");
      else if  (api->rsp->dvbt2_plp_info.plp_mode               ==     1) strcat(msg,"NORMAL_MODE         ");
      else if  (api->rsp->dvbt2_plp_info.plp_mode               ==     0) strcat(msg,"NOT_SPECIFIED       ");
      else if  (api->rsp->dvbt2_plp_info.plp_mode               ==     3) strcat(msg,"RESERVED            ");
      else                                                               sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.plp_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RESERVED_1_2           ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.reserved_1_2);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-STATIC_PADDING_FLAG    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_info.static_padding_flag);
     break;
    #endif /*     Si2168B_DVBT2_PLP_INFO_CMD */

    #ifdef        Si2168B_DVBT2_PLP_SELECT_CMD
     case         Si2168B_DVBT2_PLP_SELECT_CMD_CODE:
      sprintf(msg,"DVBT2_PLP_SELECT ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->dvbt2_plp_select.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_plp_select.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_select.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->dvbt2_plp_select.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_plp_select.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_select.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->dvbt2_plp_select.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dvbt2_plp_select.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_select.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->dvbt2_plp_select.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dvbt2_plp_select.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                          sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_plp_select.STATUS->cts);
     break;
    #endif /*     Si2168B_DVBT2_PLP_SELECT_CMD */

    #ifdef        Si2168B_DVBT2_STATUS_CMD
     case         Si2168B_DVBT2_STATUS_CMD_CODE:
      sprintf(msg,"DVBT2_STATUS ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT         ",1000);
           if  (api->rsp->dvbt2_status.STATUS->ddint         ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_status.STATUS->ddint         ==     1) strcat(msg,"TRIGGERED    ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT       ",1000);
           if  (api->rsp->dvbt2_status.STATUS->scanint       ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_status.STATUS->scanint       ==     1) strcat(msg,"TRIGGERED    ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR           ",1000);
           if  (api->rsp->dvbt2_status.STATUS->err           ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dvbt2_status.STATUS->err           ==     0) strcat(msg,"NO_ERROR");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS           ",1000);
           if  (api->rsp->dvbt2_status.STATUS->cts           ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dvbt2_status.STATUS->cts           ==     0) strcat(msg,"WAIT     ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCLINT        ",1000);
           if  (api->rsp->dvbt2_status.pclint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt2_status.pclint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.pclint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DLINT         ",1000);
           if  (api->rsp->dvbt2_status.dlint         ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt2_status.dlint         ==     0) strcat(msg,"NO_CHANGE");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.dlint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BERINT        ",1000);
           if  (api->rsp->dvbt2_status.berint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt2_status.berint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.berint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCORINT      ",1000);
           if  (api->rsp->dvbt2_status.uncorint      ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt2_status.uncorint      ==     0) strcat(msg,"NO_CHANGE");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.uncorint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-NOTDVBT2INT   ",1000);
           if  (api->rsp->dvbt2_status.notdvbt2int   ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt2_status.notdvbt2int   ==     0) strcat(msg,"NO_CHANGE");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.notdvbt2int);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCL           ",1000);
           if  (api->rsp->dvbt2_status.pcl           ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->dvbt2_status.pcl           ==     0) strcat(msg,"NO_LOCK");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.pcl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DL            ",1000);
           if  (api->rsp->dvbt2_status.dl            ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->dvbt2_status.dl            ==     0) strcat(msg,"NO_LOCK");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.dl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BER           ",1000);
           if  (api->rsp->dvbt2_status.ber           ==     1) strcat(msg,"BER_ABOVE");
      else if  (api->rsp->dvbt2_status.ber           ==     0) strcat(msg,"BER_BELOW");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.ber);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCOR         ",1000);
           if  (api->rsp->dvbt2_status.uncor         ==     0) strcat(msg,"NO_UNCOR_FOUND");
      else if  (api->rsp->dvbt2_status.uncor         ==     1) strcat(msg,"UNCOR_FOUND   ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.uncor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-NOTDVBT2      ",1000);
           if  (api->rsp->dvbt2_status.notdvbt2      ==     0) strcat(msg,"DVBT2    ");
      else if  (api->rsp->dvbt2_status.notdvbt2      ==     1) strcat(msg,"NOT_DVBT2");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.notdvbt2);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CNR           ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.cnr);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-AFC_FREQ      ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.afc_freq);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TIMING_OFFSET ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.timing_offset);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CONSTELLATION ",1000);
           if  (api->rsp->dvbt2_status.constellation ==    10) strcat(msg,"QAM128");
      else if  (api->rsp->dvbt2_status.constellation ==     7) strcat(msg,"QAM16 ");
      else if  (api->rsp->dvbt2_status.constellation ==    11) strcat(msg,"QAM256");
      else if  (api->rsp->dvbt2_status.constellation ==     8) strcat(msg,"QAM32 ");
      else if  (api->rsp->dvbt2_status.constellation ==     9) strcat(msg,"QAM64 ");
      else if  (api->rsp->dvbt2_status.constellation ==     3) strcat(msg,"QPSK  ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.constellation);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SP_INV        ",1000);
           if  (api->rsp->dvbt2_status.sp_inv        ==     1) strcat(msg,"INVERTED");
      else if  (api->rsp->dvbt2_status.sp_inv        ==     0) strcat(msg,"NORMAL  ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.sp_inv);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FEF           ",1000);
           if  (api->rsp->dvbt2_status.fef           ==     1) strcat(msg,"FEF   ");
      else if  (api->rsp->dvbt2_status.fef           ==     0) strcat(msg,"NO_FEF");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.fef);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FFT_MODE      ",1000);
           if  (api->rsp->dvbt2_status.fft_mode      ==    14) strcat(msg,"16K");
      else if  (api->rsp->dvbt2_status.fft_mode      ==    10) strcat(msg,"1K ");
      else if  (api->rsp->dvbt2_status.fft_mode      ==    11) strcat(msg,"2K ");
      else if  (api->rsp->dvbt2_status.fft_mode      ==    15) strcat(msg,"32K");
      else if  (api->rsp->dvbt2_status.fft_mode      ==    12) strcat(msg,"4K ");
      else if  (api->rsp->dvbt2_status.fft_mode      ==    13) strcat(msg,"8K ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.fft_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-GUARD_INT     ",1000);
           if  (api->rsp->dvbt2_status.guard_int     ==     6) strcat(msg,"19_128");
      else if  (api->rsp->dvbt2_status.guard_int     ==     7) strcat(msg,"19_256");
      else if  (api->rsp->dvbt2_status.guard_int     ==     5) strcat(msg,"1_128 ");
      else if  (api->rsp->dvbt2_status.guard_int     ==     2) strcat(msg,"1_16  ");
      else if  (api->rsp->dvbt2_status.guard_int     ==     1) strcat(msg,"1_32  ");
      else if  (api->rsp->dvbt2_status.guard_int     ==     4) strcat(msg,"1_4   ");
      else if  (api->rsp->dvbt2_status.guard_int     ==     3) strcat(msg,"1_8   ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.guard_int);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BW_EXT        ",1000);
           if  (api->rsp->dvbt2_status.bw_ext        ==     1) strcat(msg,"EXTENDED");
      else if  (api->rsp->dvbt2_status.bw_ext        ==     0) strcat(msg,"NORMAL  ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.bw_ext);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-NUM_PLP       ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.num_plp);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PILOT_PATTERN ",1000);
           if  (api->rsp->dvbt2_status.pilot_pattern ==     0) strcat(msg,"PP1");
      else if  (api->rsp->dvbt2_status.pilot_pattern ==     1) strcat(msg,"PP2");
      else if  (api->rsp->dvbt2_status.pilot_pattern ==     2) strcat(msg,"PP3");
      else if  (api->rsp->dvbt2_status.pilot_pattern ==     3) strcat(msg,"PP4");
      else if  (api->rsp->dvbt2_status.pilot_pattern ==     4) strcat(msg,"PP5");
      else if  (api->rsp->dvbt2_status.pilot_pattern ==     5) strcat(msg,"PP6");
      else if  (api->rsp->dvbt2_status.pilot_pattern ==     6) strcat(msg,"PP7");
      else if  (api->rsp->dvbt2_status.pilot_pattern ==     7) strcat(msg,"PP8");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.pilot_pattern);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TX_MODE       ",1000);
           if  (api->rsp->dvbt2_status.tx_mode       ==     1) strcat(msg,"MISO");
      else if  (api->rsp->dvbt2_status.tx_mode       ==     0) strcat(msg,"SISO");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.tx_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ROTATED       ",1000);
           if  (api->rsp->dvbt2_status.rotated       ==     0) strcat(msg,"NORMAL ");
      else if  (api->rsp->dvbt2_status.rotated       ==     1) strcat(msg,"ROTATED");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.rotated);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SHORT_FRAME   ",1000);
           if  (api->rsp->dvbt2_status.short_frame   ==     0) strcat(msg,"16K_LDPC");
      else if  (api->rsp->dvbt2_status.short_frame   ==     1) strcat(msg,"64K_LDPC");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.short_frame);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-T2_MODE       ",1000);
           if  (api->rsp->dvbt2_status.t2_mode       ==     0) strcat(msg,"BASE");
      else if  (api->rsp->dvbt2_status.t2_mode       ==     1) strcat(msg,"LITE");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.t2_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CODE_RATE     ",1000);
           if  (api->rsp->dvbt2_status.code_rate     ==     1) strcat(msg,"1_2");
      else if  (api->rsp->dvbt2_status.code_rate     ==    10) strcat(msg,"1_3");
      else if  (api->rsp->dvbt2_status.code_rate     ==     2) strcat(msg,"2_3");
      else if  (api->rsp->dvbt2_status.code_rate     ==    12) strcat(msg,"2_5");
      else if  (api->rsp->dvbt2_status.code_rate     ==     3) strcat(msg,"3_4");
      else if  (api->rsp->dvbt2_status.code_rate     ==    13) strcat(msg,"3_5");
      else if  (api->rsp->dvbt2_status.code_rate     ==     4) strcat(msg,"4_5");
      else if  (api->rsp->dvbt2_status.code_rate     ==     5) strcat(msg,"5_6");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.code_rate);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-T2_VERSION    ",1000);
           if  (api->rsp->dvbt2_status.t2_version    ==     0) strcat(msg,"1_1_1");
      else if  (api->rsp->dvbt2_status.t2_version    ==     1) strcat(msg,"1_2_1");
      else if  (api->rsp->dvbt2_status.t2_version    ==     2) strcat(msg,"1_3_1");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.t2_version);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PLP_ID        ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_status.plp_id);
     break;
    #endif /*     Si2168B_DVBT2_STATUS_CMD */

    #ifdef        Si2168B_DVBT2_TX_ID_CMD
     case         Si2168B_DVBT2_TX_ID_CMD_CODE:
      sprintf(msg,"DVBT2_TX_ID ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT              ",1000);
           if  (api->rsp->dvbt2_tx_id.STATUS->ddint              ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_tx_id.STATUS->ddint              ==     1) strcat(msg,"TRIGGERED    ");
      else                                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_tx_id.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT            ",1000);
           if  (api->rsp->dvbt2_tx_id.STATUS->scanint            ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt2_tx_id.STATUS->scanint            ==     1) strcat(msg,"TRIGGERED    ");
      else                                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_tx_id.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR                ",1000);
           if  (api->rsp->dvbt2_tx_id.STATUS->err                ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dvbt2_tx_id.STATUS->err                ==     0) strcat(msg,"NO_ERROR");
      else                                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_tx_id.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS                ",1000);
           if  (api->rsp->dvbt2_tx_id.STATUS->cts                ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dvbt2_tx_id.STATUS->cts                ==     0) strcat(msg,"WAIT     ");
      else                                                                sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_tx_id.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TX_ID_AVAILABILITY ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_tx_id.tx_id_availability);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CELL_ID            ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_tx_id.cell_id);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-NETWORK_ID         ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_tx_id.network_id);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-T2_SYSTEM_ID       ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt2_tx_id.t2_system_id);
     break;
    #endif /*     Si2168B_DVBT2_TX_ID_CMD */


    #ifdef        Si2168B_DVBT_STATUS_CMD
     case         Si2168B_DVBT_STATUS_CMD_CODE:
      sprintf(msg,"DVBT_STATUS ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT         ",1000);
           if  (api->rsp->dvbt_status.STATUS->ddint         ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt_status.STATUS->ddint         ==     1) strcat(msg,"TRIGGERED    ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT       ",1000);
           if  (api->rsp->dvbt_status.STATUS->scanint       ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt_status.STATUS->scanint       ==     1) strcat(msg,"TRIGGERED    ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR           ",1000);
           if  (api->rsp->dvbt_status.STATUS->err           ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dvbt_status.STATUS->err           ==     0) strcat(msg,"NO_ERROR");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS           ",1000);
           if  (api->rsp->dvbt_status.STATUS->cts           ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dvbt_status.STATUS->cts           ==     0) strcat(msg,"WAIT     ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCLINT        ",1000);
           if  (api->rsp->dvbt_status.pclint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt_status.pclint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.pclint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DLINT         ",1000);
           if  (api->rsp->dvbt_status.dlint         ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt_status.dlint         ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.dlint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BERINT        ",1000);
           if  (api->rsp->dvbt_status.berint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt_status.berint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.berint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCORINT      ",1000);
           if  (api->rsp->dvbt_status.uncorint      ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt_status.uncorint      ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.uncorint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-NOTDVBTINT    ",1000);
           if  (api->rsp->dvbt_status.notdvbtint    ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->dvbt_status.notdvbtint    ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.notdvbtint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCL           ",1000);
           if  (api->rsp->dvbt_status.pcl           ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->dvbt_status.pcl           ==     0) strcat(msg,"NO_LOCK");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.pcl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DL            ",1000);
           if  (api->rsp->dvbt_status.dl            ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->dvbt_status.dl            ==     0) strcat(msg,"NO_LOCK");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.dl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BER           ",1000);
           if  (api->rsp->dvbt_status.ber           ==     1) strcat(msg,"BER_ABOVE");
      else if  (api->rsp->dvbt_status.ber           ==     0) strcat(msg,"BER_BELOW");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.ber);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCOR         ",1000);
           if  (api->rsp->dvbt_status.uncor         ==     0) strcat(msg,"NO_UNCOR_FOUND");
      else if  (api->rsp->dvbt_status.uncor         ==     1) strcat(msg,"UNCOR_FOUND   ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.uncor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-NOTDVBT       ",1000);
           if  (api->rsp->dvbt_status.notdvbt       ==     0) strcat(msg,"DVBT    ");
      else if  (api->rsp->dvbt_status.notdvbt       ==     1) strcat(msg,"NOT_DVBT");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.notdvbt);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CNR           ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.cnr);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-AFC_FREQ      ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.afc_freq);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TIMING_OFFSET ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.timing_offset);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CONSTELLATION ",1000);
           if  (api->rsp->dvbt_status.constellation ==     7) strcat(msg,"QAM16");
      else if  (api->rsp->dvbt_status.constellation ==     9) strcat(msg,"QAM64");
      else if  (api->rsp->dvbt_status.constellation ==     3) strcat(msg,"QPSK ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.constellation);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SP_INV        ",1000);
           if  (api->rsp->dvbt_status.sp_inv        ==     1) strcat(msg,"INVERTED");
      else if  (api->rsp->dvbt_status.sp_inv        ==     0) strcat(msg,"NORMAL  ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.sp_inv);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RATE_HP       ",1000);
           if  (api->rsp->dvbt_status.rate_hp       ==     1) strcat(msg,"1_2");
      else if  (api->rsp->dvbt_status.rate_hp       ==     2) strcat(msg,"2_3");
      else if  (api->rsp->dvbt_status.rate_hp       ==     3) strcat(msg,"3_4");
      else if  (api->rsp->dvbt_status.rate_hp       ==     5) strcat(msg,"5_6");
      else if  (api->rsp->dvbt_status.rate_hp       ==     7) strcat(msg,"7_8");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.rate_hp);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RATE_LP       ",1000);
           if  (api->rsp->dvbt_status.rate_lp       ==     1) strcat(msg,"1_2");
      else if  (api->rsp->dvbt_status.rate_lp       ==     2) strcat(msg,"2_3");
      else if  (api->rsp->dvbt_status.rate_lp       ==     3) strcat(msg,"3_4");
      else if  (api->rsp->dvbt_status.rate_lp       ==     5) strcat(msg,"5_6");
      else if  (api->rsp->dvbt_status.rate_lp       ==     7) strcat(msg,"7_8");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.rate_lp);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FFT_MODE      ",1000);
           if  (api->rsp->dvbt_status.fft_mode      ==    11) strcat(msg,"2K");
      else if  (api->rsp->dvbt_status.fft_mode      ==    12) strcat(msg,"4K");
      else if  (api->rsp->dvbt_status.fft_mode      ==    13) strcat(msg,"8K");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.fft_mode);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-GUARD_INT     ",1000);
           if  (api->rsp->dvbt_status.guard_int     ==     2) strcat(msg,"1_16");
      else if  (api->rsp->dvbt_status.guard_int     ==     1) strcat(msg,"1_32");
      else if  (api->rsp->dvbt_status.guard_int     ==     4) strcat(msg,"1_4 ");
      else if  (api->rsp->dvbt_status.guard_int     ==     3) strcat(msg,"1_8 ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.guard_int);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-HIERARCHY     ",1000);
           if  (api->rsp->dvbt_status.hierarchy     ==     2) strcat(msg,"ALFA1");
      else if  (api->rsp->dvbt_status.hierarchy     ==     3) strcat(msg,"ALFA2");
      else if  (api->rsp->dvbt_status.hierarchy     ==     5) strcat(msg,"ALFA4");
      else if  (api->rsp->dvbt_status.hierarchy     ==     1) strcat(msg,"NONE ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.hierarchy);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TPS_LENGTH    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_status.tps_length);
     break;
    #endif /*     Si2168B_DVBT_STATUS_CMD */

    #ifdef        Si2168B_DVBT_TPS_EXTRA_CMD
     case         Si2168B_DVBT_TPS_EXTRA_CMD_CODE:
      sprintf(msg,"DVBT_TPS_EXTRA ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT       ",1000);
           if  (api->rsp->dvbt_tps_extra.STATUS->ddint       ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt_tps_extra.STATUS->ddint       ==     1) strcat(msg,"TRIGGERED    ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT     ",1000);
           if  (api->rsp->dvbt_tps_extra.STATUS->scanint     ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->dvbt_tps_extra.STATUS->scanint     ==     1) strcat(msg,"TRIGGERED    ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR         ",1000);
           if  (api->rsp->dvbt_tps_extra.STATUS->err         ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->dvbt_tps_extra.STATUS->err         ==     0) strcat(msg,"NO_ERROR");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS         ",1000);
           if  (api->rsp->dvbt_tps_extra.STATUS->cts         ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->dvbt_tps_extra.STATUS->cts         ==     0) strcat(msg,"WAIT     ");
      else                                                            sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-LPTIMESLICE ",1000);
           if  (api->rsp->dvbt_tps_extra.lptimeslice ==     0) strcat(msg,"off");
      else if  (api->rsp->dvbt_tps_extra.lptimeslice ==     1) strcat(msg,"on ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.lptimeslice);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-HPTIMESLICE ",1000);
           if  (api->rsp->dvbt_tps_extra.hptimeslice ==     0) strcat(msg,"off");
      else if  (api->rsp->dvbt_tps_extra.hptimeslice ==     1) strcat(msg,"on ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.hptimeslice);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-LPMPEFEC    ",1000);
           if  (api->rsp->dvbt_tps_extra.lpmpefec    ==     0) strcat(msg,"off");
      else if  (api->rsp->dvbt_tps_extra.lpmpefec    ==     1) strcat(msg,"on ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.lpmpefec);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-HPMPEFEC    ",1000);
           if  (api->rsp->dvbt_tps_extra.hpmpefec    ==     0) strcat(msg,"off");
      else if  (api->rsp->dvbt_tps_extra.hpmpefec    ==     1) strcat(msg,"on ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.hpmpefec);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DVBHINTER   ",1000);
           if  (api->rsp->dvbt_tps_extra.dvbhinter   ==     1) strcat(msg,"in_depth");
      else if  (api->rsp->dvbt_tps_extra.dvbhinter   ==     0) strcat(msg,"native  ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.dvbhinter);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CELL_ID     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.cell_id);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TPS_RES1    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.tps_res1);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TPS_RES2    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.tps_res2);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TPS_RES3    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.tps_res3);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TPS_RES4    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->dvbt_tps_extra.tps_res4);
     break;
    #endif /*     Si2168B_DVBT_TPS_EXTRA_CMD */


    #ifdef        Si2168B_EXIT_BOOTLOADER_CMD
     case         Si2168B_EXIT_BOOTLOADER_CMD_CODE:
      sprintf(msg,"EXIT_BOOTLOADER ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->exit_bootloader.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->exit_bootloader.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->exit_bootloader.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->exit_bootloader.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->exit_bootloader.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->exit_bootloader.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->exit_bootloader.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->exit_bootloader.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->exit_bootloader.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->exit_bootloader.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->exit_bootloader.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->exit_bootloader.STATUS->cts);
     break;
    #endif /*     Si2168B_EXIT_BOOTLOADER_CMD */

    #ifdef        Si2168B_GET_PROPERTY_CMD
     case         Si2168B_GET_PROPERTY_CMD_CODE:
      sprintf(msg,"GET_PROPERTY ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT    ",1000);
           if  (api->rsp->get_property.STATUS->ddint    ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->get_property.STATUS->ddint    ==     1) strcat(msg,"TRIGGERED    ");
      else                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->get_property.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT  ",1000);
           if  (api->rsp->get_property.STATUS->scanint  ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->get_property.STATUS->scanint  ==     1) strcat(msg,"TRIGGERED    ");
      else                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->get_property.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR      ",1000);
           if  (api->rsp->get_property.STATUS->err      ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->get_property.STATUS->err      ==     0) strcat(msg,"NO_ERROR");
      else                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->get_property.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS      ",1000);
           if  (api->rsp->get_property.STATUS->cts      ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->get_property.STATUS->cts      ==     0) strcat(msg,"WAIT     ");
      else                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->get_property.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RESERVED ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_property.reserved);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DATA     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_property.data);
     break;
    #endif /*     Si2168B_GET_PROPERTY_CMD */

    #ifdef        Si2168B_GET_REV_CMD
     case         Si2168B_GET_REV_CMD_CODE:
      sprintf(msg,"GET_REV ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT    ",1000);
           if  (api->rsp->get_rev.STATUS->ddint    ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->get_rev.STATUS->ddint    ==     1) strcat(msg,"TRIGGERED    ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT  ",1000);
           if  (api->rsp->get_rev.STATUS->scanint  ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->get_rev.STATUS->scanint  ==     1) strcat(msg,"TRIGGERED    ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR      ",1000);
           if  (api->rsp->get_rev.STATUS->err      ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->get_rev.STATUS->err      ==     0) strcat(msg,"NO_ERROR");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS      ",1000);
           if  (api->rsp->get_rev.STATUS->cts      ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->get_rev.STATUS->cts      ==     0) strcat(msg,"WAIT     ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PN       ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.pn);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FWMAJOR  ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.fwmajor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-FWMINOR  ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.fwminor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PATCH    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.patch);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CMPMAJOR ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.cmpmajor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CMPMINOR ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.cmpminor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CMPBUILD ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.cmpbuild);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CHIPREV  ",1000);
           if  (api->rsp->get_rev.chiprev  ==     1) strcat(msg,"A");
      else if  (api->rsp->get_rev.chiprev  ==     2) strcat(msg,"B");
      else if  (api->rsp->get_rev.chiprev  ==     2) strcat(msg,"C");
      else                                          sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.chiprev);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MCM_DIE  ",1000);
           if  (api->rsp->get_rev.mcm_die  ==     1) strcat(msg,"DIE_A ");
      else if  (api->rsp->get_rev.mcm_die  ==     2) strcat(msg,"DIE_B ");
      else if  (api->rsp->get_rev.mcm_die  ==     0) strcat(msg,"SINGLE");
      else                                          sprintf(msg,1000,"%s%d", msg, api->rsp->get_rev.mcm_die);
     break;
    #endif /*     Si2168B_GET_REV_CMD */

    #ifdef        Si2168B_I2C_PASSTHROUGH_CMD
     case         Si2168B_I2C_PASSTHROUGH_CMD_CODE:
      sprintf(msg,"I2C_PASSTHROUGH ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->i2c_passthrough.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->i2c_passthrough.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->i2c_passthrough.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->i2c_passthrough.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->i2c_passthrough.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->i2c_passthrough.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->i2c_passthrough.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->i2c_passthrough.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->i2c_passthrough.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->i2c_passthrough.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->i2c_passthrough.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->i2c_passthrough.STATUS->cts);
     break;
    #endif /*     Si2168B_I2C_PASSTHROUGH_CMD */

    #ifdef        Si2168B_MCNS_STATUS_CMD
     case         Si2168B_MCNS_STATUS_CMD_CODE:
      sprintf(msg,"MCNS_STATUS ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT         ",1000);
           if  (api->rsp->mcns_status.STATUS->ddint         ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->mcns_status.STATUS->ddint         ==     1) strcat(msg,"TRIGGERED    ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT       ",1000);
           if  (api->rsp->mcns_status.STATUS->scanint       ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->mcns_status.STATUS->scanint       ==     1) strcat(msg,"TRIGGERED    ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR           ",1000);
           if  (api->rsp->mcns_status.STATUS->err           ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->mcns_status.STATUS->err           ==     0) strcat(msg,"NO_ERROR");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS           ",1000);
           if  (api->rsp->mcns_status.STATUS->cts           ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->mcns_status.STATUS->cts           ==     0) strcat(msg,"WAIT     ");
      else                                                           sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCLINT        ",1000);
           if  (api->rsp->mcns_status.pclint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->mcns_status.pclint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.pclint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DLINT         ",1000);
           if  (api->rsp->mcns_status.dlint         ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->mcns_status.dlint         ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.dlint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BERINT        ",1000);
           if  (api->rsp->mcns_status.berint        ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->mcns_status.berint        ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.berint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCORINT      ",1000);
           if  (api->rsp->mcns_status.uncorint      ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->mcns_status.uncorint      ==     0) strcat(msg,"NO_CHANGE");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.uncorint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PCL           ",1000);
           if  (api->rsp->mcns_status.pcl           ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->mcns_status.pcl           ==     0) strcat(msg,"NO_LOCK");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.pcl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DL            ",1000);
           if  (api->rsp->mcns_status.dl            ==     1) strcat(msg,"LOCKED ");
      else if  (api->rsp->mcns_status.dl            ==     0) strcat(msg,"NO_LOCK");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.dl);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BER           ",1000);
           if  (api->rsp->mcns_status.ber           ==     1) strcat(msg,"BER_ABOVE");
      else if  (api->rsp->mcns_status.ber           ==     0) strcat(msg,"BER_BELOW");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.ber);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-UNCOR         ",1000);
           if  (api->rsp->mcns_status.uncor         ==     0) strcat(msg,"NO_UNCOR_FOUND");
      else if  (api->rsp->mcns_status.uncor         ==     1) strcat(msg,"UNCOR_FOUND   ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.uncor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CNR           ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.cnr);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-AFC_FREQ      ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.afc_freq);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-TIMING_OFFSET ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.timing_offset);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CONSTELLATION ",1000);
           if  (api->rsp->mcns_status.constellation ==    11) strcat(msg,"QAM256");
      else if  (api->rsp->mcns_status.constellation ==     9) strcat(msg,"QAM64 ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.constellation);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SP_INV        ",1000);
           if  (api->rsp->mcns_status.sp_inv        ==     1) strcat(msg,"INVERTED");
      else if  (api->rsp->mcns_status.sp_inv        ==     0) strcat(msg,"NORMAL  ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.sp_inv);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-INTERLEAVING  ",1000);
           if  (api->rsp->mcns_status.interleaving  ==     0) strcat(msg,"0__128_1    ");
      else if  (api->rsp->mcns_status.interleaving  ==    10) strcat(msg,"10__128_6   ");
      else if  (api->rsp->mcns_status.interleaving  ==    11) strcat(msg,"11__RESERVED");
      else if  (api->rsp->mcns_status.interleaving  ==    12) strcat(msg,"12__128_7   ");
      else if  (api->rsp->mcns_status.interleaving  ==    13) strcat(msg,"13__RESERVED");
      else if  (api->rsp->mcns_status.interleaving  ==    14) strcat(msg,"14__128_8   ");
      else if  (api->rsp->mcns_status.interleaving  ==    15) strcat(msg,"15__RESERVED");
      else if  (api->rsp->mcns_status.interleaving  ==     1) strcat(msg,"1__128_1    ");
      else if  (api->rsp->mcns_status.interleaving  ==     2) strcat(msg,"2__128_2    ");
      else if  (api->rsp->mcns_status.interleaving  ==     3) strcat(msg,"3__64_2     ");
      else if  (api->rsp->mcns_status.interleaving  ==     4) strcat(msg,"4__128_3    ");
      else if  (api->rsp->mcns_status.interleaving  ==     5) strcat(msg,"5__32_4     ");
      else if  (api->rsp->mcns_status.interleaving  ==     6) strcat(msg,"6__128_4    ");
      else if  (api->rsp->mcns_status.interleaving  ==     7) strcat(msg,"7__16_8     ");
      else if  (api->rsp->mcns_status.interleaving  ==     8) strcat(msg,"8__128_5    ");
      else if  (api->rsp->mcns_status.interleaving  ==     9) strcat(msg,"9__8_16     ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->mcns_status.interleaving);
     break;
    #endif /*     Si2168B_MCNS_STATUS_CMD */


    #ifdef        Si2168B_PART_INFO_CMD
     case         Si2168B_PART_INFO_CMD_CODE:
      sprintf(msg,"PART_INFO ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT    ",1000);
           if  (api->rsp->part_info.STATUS->ddint    ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->part_info.STATUS->ddint    ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT  ",1000);
           if  (api->rsp->part_info.STATUS->scanint  ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->part_info.STATUS->scanint  ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR      ",1000);
           if  (api->rsp->part_info.STATUS->err      ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->part_info.STATUS->err      ==     0) strcat(msg,"NO_ERROR");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS      ",1000);
           if  (api->rsp->part_info.STATUS->cts      ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->part_info.STATUS->cts      ==     0) strcat(msg,"WAIT     ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CHIPREV  ",1000);
           if  (api->rsp->part_info.chiprev  ==     1) strcat(msg,"A");
      else if  (api->rsp->part_info.chiprev  ==     2) strcat(msg,"B");
      else                                            sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.chiprev);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PART     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.part);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PMAJOR   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.pmajor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PMINOR   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.pminor);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-PBUILD   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.pbuild);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RESERVED ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.reserved);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SERIAL   ",1000); sprintf(msg,1000,"%s%ld", msg, api->rsp->part_info.serial);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ROMID    ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->part_info.romid);
     break;
    #endif /*     Si2168B_PART_INFO_CMD */

    #ifdef        Si2168B_POWER_DOWN_CMD
     case         Si2168B_POWER_DOWN_CMD_CODE:
      sprintf(msg,"POWER_DOWN ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->power_down.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->power_down.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->power_down.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->power_down.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->power_down.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->power_down.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->power_down.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->power_down.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->power_down.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->power_down.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->power_down.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                    sprintf(msg,1000,"%s%d", msg, api->rsp->power_down.STATUS->cts);
     break;
    #endif /*     Si2168B_POWER_DOWN_CMD */

    #ifdef        Si2168B_POWER_UP_CMD
     case         Si2168B_POWER_UP_CMD_CODE:
      sprintf(msg,"POWER_UP ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->power_up.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->power_up.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->power_up.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->power_up.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->power_up.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->power_up.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->power_up.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->power_up.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->power_up.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->power_up.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->power_up.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->power_up.STATUS->cts);
     break;
    #endif /*     Si2168B_POWER_UP_CMD */

    #ifdef        Si2168B_RSSI_ADC_CMD
     case         Si2168B_RSSI_ADC_CMD_CODE:
      sprintf(msg,"RSSI_ADC ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->rssi_adc.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->rssi_adc.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->rssi_adc.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->rssi_adc.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->rssi_adc.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->rssi_adc.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->rssi_adc.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->rssi_adc.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->rssi_adc.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->rssi_adc.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->rssi_adc.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->rssi_adc.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-LEVEL   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->rssi_adc.level);
     break;
    #endif /*     Si2168B_RSSI_ADC_CMD */

    #ifdef        Si2168B_SCAN_CTRL_CMD
     case         Si2168B_SCAN_CTRL_CMD_CODE:
      sprintf(msg,"SCAN_CTRL ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->scan_ctrl.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->scan_ctrl.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->scan_ctrl.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->scan_ctrl.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->scan_ctrl.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->scan_ctrl.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->scan_ctrl.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->scan_ctrl.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->scan_ctrl.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->scan_ctrl.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->scan_ctrl.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->scan_ctrl.STATUS->cts);
     break;
    #endif /*     Si2168B_SCAN_CTRL_CMD */

    #ifdef        Si2168B_SCAN_STATUS_CMD
     case         Si2168B_SCAN_STATUS_CMD_CODE:
      sprintf(msg,"SCAN_STATUS ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT       ",1000);
           if  (api->rsp->scan_status.STATUS->ddint       ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->scan_status.STATUS->ddint       ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT     ",1000);
           if  (api->rsp->scan_status.STATUS->scanint     ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->scan_status.STATUS->scanint     ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR         ",1000);
           if  (api->rsp->scan_status.STATUS->err         ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->scan_status.STATUS->err         ==     0) strcat(msg,"NO_ERROR");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS         ",1000);
           if  (api->rsp->scan_status.STATUS->cts         ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->scan_status.STATUS->cts         ==     0) strcat(msg,"WAIT     ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BUZINT      ",1000);
           if  (api->rsp->scan_status.buzint      ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->scan_status.buzint      ==     0) strcat(msg,"NO_CHANGE");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.buzint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-REQINT      ",1000);
           if  (api->rsp->scan_status.reqint      ==     1) strcat(msg,"CHANGED  ");
      else if  (api->rsp->scan_status.reqint      ==     0) strcat(msg,"NO_CHANGE");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.reqint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-BUZ         ",1000);
           if  (api->rsp->scan_status.buz         ==     1) strcat(msg,"BUSY");
      else if  (api->rsp->scan_status.buz         ==     0) strcat(msg,"CTS ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.buz);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-REQ         ",1000);
           if  (api->rsp->scan_status.req         ==     0) strcat(msg,"NO_REQUEST");
      else if  (api->rsp->scan_status.req         ==     1) strcat(msg,"REQUEST   ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.req);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCAN_STATUS ",1000);
           if  (api->rsp->scan_status.scan_status ==     6) strcat(msg,"ANALOG_CHANNEL_FOUND ");
      else if  (api->rsp->scan_status.scan_status ==    63) strcat(msg,"DEBUG                ");
      else if  (api->rsp->scan_status.scan_status ==     5) strcat(msg,"DIGITAL_CHANNEL_FOUND");
      else if  (api->rsp->scan_status.scan_status ==     2) strcat(msg,"ENDED                ");
      else if  (api->rsp->scan_status.scan_status ==     3) strcat(msg,"ERROR                ");
      else if  (api->rsp->scan_status.scan_status ==     0) strcat(msg,"IDLE                 ");
      else if  (api->rsp->scan_status.scan_status ==     1) strcat(msg,"SEARCHING            ");
      else if  (api->rsp->scan_status.scan_status ==     4) strcat(msg,"TUNE_REQUEST         ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.scan_status);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RF_FREQ     ",1000); sprintf(msg,1000,"%s%ld", msg, api->rsp->scan_status.rf_freq);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SYMB_RATE   ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.symb_rate);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-MODULATION  ",1000);
           if  (api->rsp->scan_status.modulation  ==    10) strcat(msg,"DSS  ");
      else if  (api->rsp->scan_status.modulation  ==     3) strcat(msg,"DVBC ");
      else if  (api->rsp->scan_status.modulation  ==    11) strcat(msg,"DVBC2");
      else if  (api->rsp->scan_status.modulation  ==     8) strcat(msg,"DVBS ");
      else if  (api->rsp->scan_status.modulation  ==     9) strcat(msg,"DVBS2");
      else if  (api->rsp->scan_status.modulation  ==     2) strcat(msg,"DVBT ");
      else if  (api->rsp->scan_status.modulation  ==     7) strcat(msg,"DVBT2");
      else if  (api->rsp->scan_status.modulation  ==     1) strcat(msg,"MCNS ");
      else                                                 sprintf(msg,1000,"%s%d", msg, api->rsp->scan_status.modulation);
     break;
    #endif /*     Si2168B_SCAN_STATUS_CMD */

    #ifdef        Si2168B_SET_PROPERTY_CMD
     case         Si2168B_SET_PROPERTY_CMD_CODE:
      sprintf(msg,"SET_PROPERTY ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT    ",1000);
           if  (api->rsp->set_property.STATUS->ddint    ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->set_property.STATUS->ddint    ==     1) strcat(msg,"TRIGGERED    ");
      else                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->set_property.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT  ",1000);
           if  (api->rsp->set_property.STATUS->scanint  ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->set_property.STATUS->scanint  ==     1) strcat(msg,"TRIGGERED    ");
      else                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->set_property.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR      ",1000);
           if  (api->rsp->set_property.STATUS->err      ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->set_property.STATUS->err      ==     0) strcat(msg,"NO_ERROR");
      else                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->set_property.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS      ",1000);
           if  (api->rsp->set_property.STATUS->cts      ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->set_property.STATUS->cts      ==     0) strcat(msg,"WAIT     ");
      else                                                       sprintf(msg,1000,"%s%d", msg, api->rsp->set_property.STATUS->cts);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-RESERVED ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->set_property.reserved);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DATA     ",1000); sprintf(msg,1000,"%s%d", msg, api->rsp->set_property.data);
     break;
    #endif /*     Si2168B_SET_PROPERTY_CMD */

    #ifdef        Si2168B_SPI_LINK_CMD
     case         Si2168B_SPI_LINK_CMD_CODE:
      sprintf(msg,"SPI_LINK ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->spi_link.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->spi_link.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->spi_link.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->spi_link.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->spi_link.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->spi_link.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->spi_link.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->spi_link.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->spi_link.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->spi_link.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->spi_link.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                  sprintf(msg,1000,"%s%d", msg, api->rsp->spi_link.STATUS->cts);
     break;
    #endif /*     Si2168B_SPI_LINK_CMD */

    #ifdef        Si2168B_SPI_PASSTHROUGH_CMD
     case         Si2168B_SPI_PASSTHROUGH_CMD_CODE:
      sprintf(msg,"SPI_PASSTHROUGH ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->spi_passthrough.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->spi_passthrough.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->spi_passthrough.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->spi_passthrough.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->spi_passthrough.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->spi_passthrough.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->spi_passthrough.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->spi_passthrough.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->spi_passthrough.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->spi_passthrough.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->spi_passthrough.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                         sprintf(msg,1000,"%s%d", msg, api->rsp->spi_passthrough.STATUS->cts);
     break;
    #endif /*     Si2168B_SPI_PASSTHROUGH_CMD */

    #ifdef        Si2168B_START_CLK_CMD
     case         Si2168B_START_CLK_CMD_CODE:
      sprintf(msg,"START_CLK ");
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-DDINT   ",1000);
           if  (api->rsp->start_clk.STATUS->ddint   ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->start_clk.STATUS->ddint   ==     1) strcat(msg,"TRIGGERED    ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->start_clk.STATUS->ddint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-SCANINT ",1000);
           if  (api->rsp->start_clk.STATUS->scanint ==     0) strcat(msg,"NOT_TRIGGERED");
      else if  (api->rsp->start_clk.STATUS->scanint ==     1) strcat(msg,"TRIGGERED    ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->start_clk.STATUS->scanint);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-ERR     ",1000);
           if  (api->rsp->start_clk.STATUS->err     ==     1) strcat(msg,"ERROR   ");
      else if  (api->rsp->start_clk.STATUS->err     ==     0) strcat(msg,"NO_ERROR");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->start_clk.STATUS->err);
       sprintf(msg,1000,"%s%s",msg,separator); strcat(msg,"-CTS     ",1000);
           if  (api->rsp->start_clk.STATUS->cts     ==     1) strcat(msg,"COMPLETED");
      else if  (api->rsp->start_clk.STATUS->cts     ==     0) strcat(msg,"WAIT     ");
      else                                                   sprintf(msg,1000,"%s%d", msg, api->rsp->start_clk.STATUS->cts);
     break;
    #endif /*     Si2168B_START_CLK_CMD */

     default : break;
    }
    return 0;
  }
#endif /* Si2168B_GET_COMMAND_STRINGS */

