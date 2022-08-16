/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 1 API
   API commands strings functions definitions
   FILE: Si2168_Commands_Strings.c
   Supported IC : Si2168
   Compiled for ROM 0 firmware 0_A_build_7
   Revision: REVISION_NAME
   Tag:  V2.6.0
   Date: November 07 2011
  (C) Copyright 2011, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifdef    __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef    Si2168_COMMAND_LINE_APPLICATION

#define   Si2168_COMMAND_PROTOTYPES

#include "Si2168_Platform_Definition.h"
/*#include "Si2168_Commands_Strings.h"*/

/* _command_string_functions_insertion_start */
void  Si2168_GetCommandString (SILABS_FE_Context *front_end, char *msg)
{
  char  separatorString[10];
  char  userEntry[100];
  char *separator;
  char *newline;
  int code;

  code = 0;

  if (msg == NULL) return;

  separator = separatorString;
  sprintf(separator,"%s","\n ");
  sprintf(msg,      "%s","");

  printf ("CONFIG_PINS ");
  printf ("DD_STATUS ");
  printf ("DD_BER ");
  printf ("DD_CBER ");
  printf ("DD_PER ");
  printf ("DD_UNCOR\n");
  printf ("DVBC_STATUS ");
  printf ("DVBT_STATUS ");
  printf ("DVBT2_STATUS ");
  printf ("DVBT2_FEF ");

  if ( (newline = fgets(userEntry, MAX_ENTRY_LENGTH, stdin)) != NULL ) {
    newline = strchr(userEntry, '\n');        /* search for newline character        */
    if ( newline != NULL ) { *newline = '\0'; /* overwrite trailing newline (if any) */ }
  }
       if (strcmp_nocase(userEntry, ""        ) ==0) {return;}
  else if (strcmp_nocase(userEntry, "DD_UNCOR") ==0) {
    code = Si2168_DD_UNCOR_CMD_CODE;
    Si2168_L1_DD_UNCOR                 (front_end->Si2168_FE->demod, Si2168_DD_UNCOR_CMD_RST_RUN);
  }

  if (code !=0 ) {
    Si2168_L1_GetCommandResponseString (front_end->Si2168_FE->demod, code, separator, msg);
  }

  return code;
}
/* _command_string_functions_insertion_point */
#endif /* Si2168_COMMAND_LINE_APPLICATION */

#ifdef    __cplusplus
}
#endif /* __cplusplus */

