/***************************************************************************************
                  Silicon Laboratories Broadcast Si2148 Layer 2 API
   L2 API for commands and properties
   FILE: Si2148_L2_API.c
   Supported IC : Si2148
   Compiled for ROM 33 firmware 2_1_build_6
   Revision: 0.1
   Tag:  ROM33_2_1_build_6_V0.1
   Date: November 12 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
****************************************************************************************/
#include <string.h>
/* Si2148 API Defines */
/* define this if using the DTV video filter */
#undef USING_DLIF_FILTER
/************************************************************************************************************************/
/* Si2148 API Specific Includes */
#include "Si2148_L2_API.h"               /* Include file for this code */
#include "Si2148_Firmware_2_1_build_6.h"    /* dummy firmware compatible with Si2148-marking */
#include "Si2148_Firmware_0_E_build_15.h"       /* firmware compatible with Si2148-marking */
#define Si2148_BYTES_PER_LINE 8
#ifdef USING_DLIF_FILTER
#include "write_DLIF_video_coeffs.h"   /* .h file from custom Video filter Tool output */
#endif
/************************************************************************************************************************
  NAME: Si2148_Configure
  DESCRIPTION: Setup Si2148 video filters, GPIOs/clocks, Common Properties startup, etc.
  Parameter:  Pointer to Si2148 Context
  Returns:    I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_Configure           (L1_Si2148_Context *api)
{
    int return_code;
    return_code = NO_Si2148_ERROR;

/* _specific_configure_insertion_start */
  /* load DTV video filter file */
  #ifdef USING_DLIF_FILTER
     if ((return_code = Si2148_LoadVideofilter(api,DLIF_VIDFILT_TABLE,DLIF_VIDFILT_LINES)) != NO_Si2148_ERROR)
       return return_code;
  #endif
    /* _specific_configure_insertion_point */

    /* Set All Properties startup configuration */
    Si2148_setupAllDefaults     (api);
    Si2148_downloadAllProperties(api);

    return return_code;
}
/************************************************************************************************************************
  NAME: Si2148_PowerUpWithPatch
  DESCRIPTION: Send Si2148 API PowerUp Command with PowerUp to bootloader,
  Check the Chip rev and part, and ROMID are compared to expected values.
  Load the Firmware Patch then Start the Firmware.
  Programming Guide Reference:    Flowchart A.2 (POWER_UP with patch flowchart)

  Parameter:  pointer to Si2148 Context
  Returns:    Si2148/I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_PowerUpWithPatch    (L1_Si2148_Context *api)
{
    int return_code;
    return_code = NO_Si2148_ERROR;

    /* always wait for CTS prior to POWER_UP command */
    if ((return_code = Si2148_pollForCTS  (api)) != NO_Si2148_ERROR) {
        SiTRACE ("Si2148_pollForCTS error 0x%02x\n", return_code);
        return return_code;
    }

    if ((return_code = Si2148_L1_POWER_UP (api,
                            Si2148_POWER_UP_CMD_SUBCODE_CODE,
                            Si2148_POWER_UP_CMD_CLOCK_MODE_XTAL,
                            Si2148_POWER_UP_CMD_EN_XOUT_DIS_XOUT,
                            Si2148_POWER_UP_CMD_PD_LDO_LDO_POWER_UP,
                            Si2148_POWER_UP_CMD_RESERVED2_RESERVED,
                            Si2148_POWER_UP_CMD_RESERVED3_RESERVED,
                            Si2148_POWER_UP_CMD_RESERVED4_RESERVED,
                            Si2148_POWER_UP_CMD_RESERVED5_RESERVED,
                            Si2148_POWER_UP_CMD_RESERVED6_RESERVED,
                            Si2148_POWER_UP_CMD_RESERVED7_RESERVED,
                            Si2148_POWER_UP_CMD_RESET_RESET,
                            Si2148_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ,
                            Si2148_POWER_UP_CMD_RESERVED8_RESERVED,
                            Si2148_POWER_UP_CMD_FUNC_BOOTLOADER,
                            Si2148_POWER_UP_CMD_CTSIEN_DISABLE,
                            Si2148_POWER_UP_CMD_WAKE_UP_WAKE_UP
                            )) != NO_Si2148_ERROR)
    {
        SiTRACE ("Si2148_L1_POWER_UP error 0x%02x: %s\n", return_code, Si2148_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }

    /* Get the Part Info from the chip.   This command is only valid in Bootloader mode */
    if ((return_code = Si2148_L1_PART_INFO(api)) != NO_Si2148_ERROR) {
        SiTRACE ("Si2148_L1_PART_INFO error 0x%02x: %s\n", return_code, Si2148_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }
    SiTRACE("chiprev %d\n",        api->rsp->part_info.chiprev);
    SiTRACE("part    Si21%d\n",    api->rsp->part_info.part   );
    SiTRACE("pmajor  %d\n",        api->rsp->part_info.pmajor );
    if (api->rsp->part_info.pmajor >= 0x30) {
    SiTRACE("pmajor '%c'\n",       api->rsp->part_info.pmajor );
    }
    SiTRACE("pminor  %d\n",        api->rsp->part_info.pminor );
    if (api->rsp->part_info.pminor >= 0x30) {
    SiTRACE("pminor '%c'\n",       api->rsp->part_info.pminor );
    }
    SiTRACE("romid %3d/0x%02x\n",  api->rsp->part_info.romid,  api->rsp->part_info.romid );
#ifdef    PART_INTEGRITY_CHECKS
    /* Check the Chip revision, part and ROMID against expected values and report an error if incompatible */
    if (api->rsp->part_info.chiprev != api->chiprev) {
        SiTRACE ("INCOMPATIBLE PART error chiprev %d (expected %d)\n", api->rsp->part_info.chiprev, api->chiprev);
        return_code = ERROR_Si2148_INCOMPATIBLE_PART;
    }
    /* Part Number is represented as the last 2 digits */
    if (api->rsp->part_info.part != api->part) {
        SiTRACE ("INCOMPATIBLE PART error part   %d (expected %d)\n", api->rsp->part_info.part, api->part);
        return_code = ERROR_Si2148_INCOMPATIBLE_PART;
    }
    /* Part Major Version in ASCII */
    if (api->rsp->part_info.pmajor != api->partMajorVersion) {
        SiTRACE ("INCOMPATIBLE PART error pmajor %d (expected %d)\n", api->rsp->part_info.pmajor, api->partMajorVersion);
        return_code = ERROR_Si2148_INCOMPATIBLE_PART;
    }
    /* Part Minor Version in ASCII */
    if (api->rsp->part_info.pminor != api->partMinorVersion) {
        SiTRACE ("INCOMPATIBLE PART error pminor %d (expected %d)\n", api->rsp->part_info.pminor, api->partMinorVersion);
        return_code = ERROR_Si2148_INCOMPATIBLE_PART;
    }
    /* ROMID in byte representation */
    if (api->rsp->part_info.romid != api->partRomid) {
        SiTRACE ("INCOMPATIBLE PART error romid %3d (expected %2d)\n", api->rsp->part_info.romid, api->partRomid);
        return_code = ERROR_Si2148_INCOMPATIBLE_PART;
    }
    if (return_code != NO_Si2148_ERROR) return return_code;
#endif /* PART_INTEGRITY_CHECKS */
      /* Load the Firmware */

    if (api->rsp->part_info.romid == 0x32)
    {
      /* Load the Firmware */
       if ((return_code = Si2148_LoadFirmware(api, Si2148_FW_0_Eb15, FIRMWARE_LINES_0_Eb15)) != NO_Si2148_ERROR) {
         SiTRACE ("Si2148_LoadFirmware error 0x%02x: %s\n", return_code, Si2148_L1_API_ERROR_TEXT(return_code) );
         return return_code;
       }
    }
    else if (api->rsp->part_info.romid == 0x33) /* if Si2148-A20 part load firmware patch (currently a dummy patch , 20bx) */
    {
        /* This dummy patch (20bx) will skip the loadFirmware function and boot from NVM only.
           When a new patch is available for the Si2148-A20, include the patch file and replace the firmware array and size in the function below */
        if ((return_code = Si2148_LoadFirmware_16(api, Si2148_FW_2_1b6, FIRMWARE_LINES_2_1b6)) != NO_Si2148_ERROR)
        {
          SiTRACE ("Si2148_LoadFirmware_16 error 0x%02x: %s\n", return_code, Si2148_L1_API_ERROR_TEXT(return_code) );
          return return_code;
        }
    }
    else
    {
        SiTRACE ("INCOMPATIBLE PART error ROMID 0x%02x (expected 0x%02x)\n", api->rsp->part_info.romid, api->partRomid);
        return ERROR_Si2148_INCOMPATIBLE_PART;
    }
   /* Check the RAM_CRC value and compare with the expected value */
    /* If they match then move on,  otherwise indicate error */
    /* This check is bypassed by default to speed boot time. */

#ifdef   RAM_CRC_CHECK
    if ((return_code = Si2148_L1_RAM_CRC(api)) != NO_Si2148_ERROR)
    {
         SiTRACE ("Si2148_L1_RAM_CRC error 0x%02x: %s\n", return_code, Si2148_L1_API_ERROR_TEXT(return_code) );
         return return_code;
    }
    SiTRACE("RAM CRC = 0x%X\n",api->rsp->ram_crc.crc);
    if (api->rsp->ram_crc.crc != RAM_CRC_2_1b6)
    {
        return_code = ERROR_Si2148_CRC_CHECK_ERROR;
        SiTRACE ("RAM_CRC (0x%X) does not match expected (0x%X) error 0x%02x: %s\n",api->rsp->ram_crc.crc,RAM_CRC_2_1b6,return_code, Si2148_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }
#endif /* RAM_CRC_CHECK */
    /*Start the Firmware */
    if ((return_code = Si2148_StartFirmware(api)) != NO_Si2148_ERROR) { /* Start firmware */
        SiTRACE ("Si2148_StartFirmware error 0x%02x: %s\n", return_code, Si2148_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }

    return NO_Si2148_ERROR;
}
/************************************************************************************************************************
  NAME: Si2148_LoadFirmware_16
  DESCRIPTON: Load firmware from firmware_struct array in Si2148_Firmware_x_y_build_z.h file into Si2148
              Requires Si2148 to be in bootloader mode after PowerUp
  Programming Guide Reference:    Flowchart A.3 (Download FW PATCH flowchart)

  Parameter:  Si2148 Context (I2C address)
  Parameter:  pointer to firmware_struct array
  Parameter:  number of lines in firmware table array (size in bytes / firmware_struct)
  Returns:    Si2148/I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_LoadFirmware_16        (L1_Si2148_Context *api, firmware_struct fw_table[], int nbLines)
{
    int return_code;
    int line;
    return_code = NO_Si2148_ERROR;

    SiTRACE ("Si2148_LoadFirmware_16 starting...\n");
    SiTRACE ("Si2148_LoadFirmware_16 nbLines %d\n", nbLines);

    /* for each line in fw_table */
    for (line = 0; line < nbLines; line++)
    {
		if (fw_table[line].firmware_len > 0)  /* don't download if length is 0 , e.g. dummy firmware */
		{
			/* send firmware_len bytes (up to 16) to Si2148 */
			if ((return_code = Si2148_L1_API_Patch(api, fw_table[line].firmware_len, fw_table[line].firmware_table)) != NO_Si2148_ERROR)
			{
			  SiTRACE("Si2148_LoadFirmware_16 error 0x%02x patching line %d: %s\n", return_code, line, Si2148_L1_API_ERROR_TEXT(return_code) );
			  if (line == 0) {
			  SiTRACE("The firmware is incompatible with the part!\n");
			  }
			  return ERROR_Si2148_LOADING_FIRMWARE;
			}
			if (line == 3) {SiTraceConfiguration("traces suspend");}
		}
    }
    SiTraceConfiguration("traces resume");
    SiTRACE ("Si2148_LoadFirmware_16 complete...\n");
    return NO_Si2148_ERROR;
}
/************************************************************************************************************************
  NAME: Si2148_LoadFirmware
  DESCRIPTON: Load firmware from FIRMWARE_TABLE array in Si2148_Firmware_x_y_build_z.h file into Si2148
              Requires Si2148 to be in bootloader mode after PowerUp
  Programming Guide Reference:    Flowchart A.3 (Download FW PATCH flowchart)

  Parameter:  Si2148 Context (I2C address)
  Parameter:  pointer to firmware table array
  Parameter:  number of lines in firmware table array (size in bytes / BYTES_PER_LINE)
  Returns:    Si2148/I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_LoadFirmware        (L1_Si2148_Context *api, unsigned char fw_table[], int nbLines)
{
    int return_code;
    int line;
    return_code = NO_Si2148_ERROR;

    SiTRACE ("Si2148_LoadFirmware starting...\n");
    SiTRACE ("Si2148_LoadFirmware nbLines %d\n", nbLines);

    /* for each line in fw_table */
    for (line = 0; line < nbLines; line++)
    {
        /* send Si2148_BYTES_PER_LINE fw bytes to Si2148 */
        if ((return_code = Si2148_L1_API_Patch(api, Si2148_BYTES_PER_LINE, fw_table + Si2148_BYTES_PER_LINE*line)) != NO_Si2148_ERROR)
        {
          SiTRACE("Si2148_LoadFirmware error 0x%02x patching line %d: %s\n", return_code, line, Si2148_L1_API_ERROR_TEXT(return_code) );
          if (line == 0) {
          SiTRACE("The firmware is incompatible with the part!\n");
          }
          return ERROR_Si2148_LOADING_FIRMWARE;
        }
		if (line==3) {SiTraceConfiguration("traces suspend");}
    }
    SiTraceConfiguration("traces resume");
    SiTRACE ("Si2148_LoadFirmware complete...\n");
    return NO_Si2148_ERROR;
}
/************************************************************************************************************************
  NAME: Si2148_StartFirmware
  DESCRIPTION: Start Si2148 firmware (put the Si2148 into run mode)
  Parameter:   Si2148 Context (I2C address)
  Parameter (passed by Reference):   ExitBootloadeer Response Status byte : tunint, atvint, dtvint, err, cts
  Returns:     I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_StartFirmware       (L1_Si2148_Context *api)
{

    if (Si2148_L1_EXIT_BOOTLOADER(api, Si2148_EXIT_BOOTLOADER_CMD_FUNC_TUNER, Si2148_EXIT_BOOTLOADER_CMD_CTSIEN_OFF) != NO_Si2148_ERROR)
    {
        return ERROR_Si2148_STARTING_FIRMWARE;
    }

    return NO_Si2148_ERROR;
}
/************************************************************************************************************************
  NAME: Si2148_Init
  DESCRIPTION:Reset and Initialize Si2148
  Parameter:  Si2148 Context (I2C address)
  Returns:    I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_Init                (L1_Si2148_Context *api)
{
    int return_code;
    SiTRACE("Si2148_Init starting...\n");

    if ((return_code = Si2148_PowerUpWithPatch(api)) != NO_Si2148_ERROR) {   /* PowerUp into bootloader */
        SiTRACE ("Si2148_PowerUpWithPatch error 0x%02x: %s\n", return_code, Si2148_L1_API_ERROR_TEXT(return_code) );
        return return_code;
    }
    /* At this point, FW is loaded and started.  */
    Si2148_Configure(api);
    SiTRACE("Si2148_Init complete...\n");
    return NO_Si2148_ERROR;
}
/* _specific_code_insertion_start */
 /************************************************************************************************************************
  NAME: Si2148_Tune
  DESCRIPTIION: Tune Si2148 in specified mode (ATV/DTV) at center frequency, wait for TUNINT and xTVINT with timeout

  Parameter:  Pointer to Si2148 Context (I2C address)
  Parameter:  Mode (ATV or DTV) use Si2148_TUNER_TUNE_FREQ_CMD_MODE_ATV or Si2148_TUNER_TUNE_FREQ_CMD_MODE_DTV constants
  Parameter:  frequency (Hz) as a unsigned long integer
  Returns:    0 if channel found.  A nonzero value means either an error occurred or channel not locked.
  Programming Guide Reference:    Flowchart A.7 (Tune flowchart)
************************************************************************************************************************/
 int  Si2148_Tune              (L1_Si2148_Context *api, unsigned char mode, unsigned long freq)
{
    unsigned int start_time  = 0;
    int return_code = 0;
    unsigned int timeout     = 36;


    if (Si2148_L1_TUNER_TUNE_FREQ (api,
                                   mode,
                                   freq) != NO_Si2148_ERROR)
    {
        return ERROR_Si2148_SENDING_COMMAND;
    }
    start_time = system_time();
    /* wait for TUNINT, timeout is 36 ms */
    while ( (system_time() - start_time) < timeout )
    {
        if ((return_code = Si2148_L1_CheckStatus(api)) != 0)
            return return_code;
        if (api->status->tunint)
            break;
    }
    if (!api->status->tunint)
    {
      SiTRACE("Timeout waiting for TUNINT\n");
      return ERROR_Si2148_TUNINT_TIMEOUT;
    }

    /* wait for xTVINT, timeout is  10 ms for DTVINT */
    start_time = system_time();
    timeout    =  10;
    while ( (system_time() - start_time) < timeout )
    {
        if ((return_code = Si2148_L1_CheckStatus(api)) != 0)
            return return_code;

         if (api->status->dtvint)
            break;
    }

    if (api->status->dtvint)
    {
      SiTRACE("DTV Tune Successful\n");
      return NO_Si2148_ERROR;
    }
    else
      SiTRACE("Timeout waiting for DTVINT\n");


    return ERROR_Si2148_xTVINT_TIMEOUT;
}
 /************************************************************************************************************************
  NAME: Si2148_DTVTune
  DESCRIPTION: Update DTV_MODE and tune DTV mode at center frequency
  Parameter:  Pointer to Si2148 Context (I2C address)
  Parameter:  frequency (Hz)
  Parameter:  bandwidth , 6,7 or 8 MHz
  Parameter:  modulation,  e.g. use constant Si2148_DTV_MODE_PROP_MODULATION_DVBT for DVBT mode
  Parameter:  rsp - commandResp structure to returns tune status info.
  Returns:    I2C transaction error code, 0 if successful
  Programming Guide Reference:    Flowchart A.7 (Tune flowchart)
************************************************************************************************************************/
int  Si2148_DTVTune           (L1_Si2148_Context *api, unsigned long freq, unsigned char bw, unsigned char modulation, unsigned char invert_spectrum)
{
    int return_code;
    return_code = NO_Si2148_ERROR;

    /* update DTV_MODE_PROP property */
    api->prop->dtv_mode.bw = bw;
    api->prop->dtv_mode.invert_spectrum = invert_spectrum;
    api->prop->dtv_mode.modulation = modulation;
    if (Si2148_L1_SetProperty2(api, Si2148_DTV_MODE_PROP) != NO_Si2148_ERROR)
    {
      return ERROR_Si2148_SENDING_COMMAND;
    }

    return_code = Si2148_Tune (api, Si2148_TUNER_TUNE_FREQ_CMD_MODE_DTV, freq);

    return return_code;
}
/************************************************************************************************************************
  NAME: Si2148_LoadVideofilter
  DESCRIPTION:        Load video filters from vidfiltTable in Si2148_write_xTV_video_coeffs.h file into Si2148
  Programming Guide Reference:    Flowchart A.4 (Download Video Filters flowchart)

  Parameter:  Si2148 Context (I2C address)
  Parameter:  pointer to video filter table array
  Parameter:  number of lines in video filter table array (size in bytes / atv_vid_filt_struct)
  Returns:    Si2148/I2C transaction error code, NO_Si2178_ERROR if successful
************************************************************************************************************************/
int Si2148_LoadVideofilter     (L1_Si2148_Context *api, vid_filt_struct vidFiltTable[], int lines)
{
    int line;
     /* for each line in VIDFILT_TABLE (max 16 bytes)  */
    for (line = 0; line < lines ; line++)
    {
        /* send up to 16 byte I2C command to Si2178 */
        if (Si2148_L1_API_Patch(api, vidFiltTable[line].vid_filt_len, vidFiltTable[line].vid_filt_table) != NO_Si2148_ERROR)
        {
            return ERROR_Si2148_SENDING_COMMAND;
        }
    }
    return NO_Si2148_ERROR;
}
 /************************************************************************************************************************
  NAME: Si2148_XoutOn
  Parameter:  Pointer to Si2148 Context (I2C address)
  Returns:    I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_XoutOn             (L1_Si2148_Context *api)
{
     int return_code;
    SiTRACE("Si2148_XoutOn:  Turning Xout ON\n");

     if ((return_code = Si2148_L1_CONFIG_CLOCKS(api,
                                              Si2148_CONFIG_CLOCKS_CMD_SUBCODE_CODE,
                                              Si2148_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL,
                                              Si2148_CONFIG_CLOCKS_CMD_EN_XOUT_EN_XOUT)) != NO_Si2148_ERROR)
        return return_code;

    return NO_Si2148_ERROR;
}
 /************************************************************************************************************************
  NAME: Si2148_XoutOff
  Parameter:  Pointer to Si2148 Context (I2C address)
  Returns:    I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_XoutOff            (L1_Si2148_Context *api)
{
    int return_code;
    SiTRACE("Si2148_XoutOff:  Turning Xout OFF\n");
    if ((return_code = Si2148_L1_CONFIG_CLOCKS(api,
                                              Si2148_CONFIG_CLOCKS_CMD_SUBCODE_CODE,
                                              Si2148_CONFIG_CLOCKS_CMD_CLOCK_MODE_XTAL,
                                              Si2148_CONFIG_CLOCKS_CMD_EN_XOUT_DIS_XOUT)) != NO_Si2148_ERROR)

        return return_code;

    return NO_Si2148_ERROR;
}
 /************************************************************************************************************************
  NAME: Si2148_Standby
  Parameter:  Pointer to Si2148 Context (I2C address)
  Returns:    I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_Standby         (L1_Si2148_Context *api)
{
    int return_code;
    SiTRACE("Si2148_CLOCK_OFF: Turning clock OFF\n");

    if ((return_code = Si2148_L1_STANDBY(api, Si2148_STANDBY_CMD_TYPE_LNA_ON)) != NO_Si2148_ERROR)
    return return_code;

    return NO_Si2148_ERROR;
}
 /************************************************************************************************************************
  NAME: Si2148_Powerdown
  Parameter:  Pointer to Si2148 Context (I2C address)
  Returns:    I2C transaction error code, NO_Si2148_ERROR if successful
************************************************************************************************************************/
int Si2148_Powerdown         (L1_Si2148_Context *api)
{
    int return_code;
    SiTRACE("Si2148_CLOCK_OFF: Turning clock OFF\n");

    if ((return_code = Si2148_L1_POWER_DOWN(api)) != NO_Si2148_ERROR)
    return return_code;

    return NO_Si2148_ERROR;
}
/************************************************************************************************************************
  NAME: Si2148_AGC_Override
  Parameter:  Pointer to Si2148 Context (I2C address)
  Parameter:  Mode 0=Normal, 1= Max, 2=TOP
  Returns:    I2C transaction error code, 0 if successful
************************************************************************************************************************/
int Si2148_AGC_Override    (L1_Si2148_Context *Si2148, unsigned char mode )
{
    int return_code;
    SiTRACE("Si2148_AGC_Override: mode = %d\n",mode);
    switch (mode)
    {
        case  Si2148_FORCE_NORMAL_AGC:
            if ((return_code = Si2148_L1_AGC_OVERRIDE (Si2148, Si2148_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2148_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE)) != NO_Si2148_ERROR)
                return return_code;
            break;
        case  Si2148_FORCE_MAX_AGC:
            if ((return_code = Si2148_L1_AGC_OVERRIDE (Si2148, Si2148_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_ENABLE,Si2148_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_DISABLE)) != NO_Si2148_ERROR)
                return return_code;
            break;
        case  Si2148_FORCE_TOP_AGC:
            if ((return_code = Si2148_L1_AGC_OVERRIDE (Si2148, Si2148_AGC_OVERRIDE_CMD_FORCE_MAX_GAIN_DISABLE,Si2148_AGC_OVERRIDE_CMD_FORCE_TOP_GAIN_ENABLE)) != NO_Si2148_ERROR)
                return return_code;
            break;

        default:
            return ERROR_Si2148_PARAMETER_OUT_OF_RANGE;

    }

    return NO_Si2148_ERROR;
}
/* _specific_code_insertion_point */





