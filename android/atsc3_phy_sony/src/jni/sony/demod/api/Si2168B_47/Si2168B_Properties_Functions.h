/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168B Layer 1 API

   EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND CONDITIONS OF
     THE SOFTWARE LICENSE AGREEMENT IN THE DOCUMENTATION FILE CORRESPONDING
     TO THIS SOURCE FILE.
   IF YOU DO NOT AGREE TO THE LIMITED LICENSE AND CONDITIONS OF SUCH AGREEMENT,
     PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.

   API properties functions definitions
   FILE: Si2168B_Properties_Functions.h
   Supported IC : Si2168B
   Compiled for ROM 0 firmware 3_A_build_3
   Revision: REVISION_NAME
   Date: May 22 2013
  (C) Copyright 2013, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef   _Si2168B_PROPERTIES_FUNCTIONS_H_
#define   _Si2168B_PROPERTIES_FUNCTIONS_H_

void          Si2168B_storeUserProperties           (Si2168B_PropObj   *prop);
unsigned char Si2168B_PackProperty                  (Si2168B_PropObj   *prop, unsigned int prop_code, int *data);
unsigned char Si2168B_UnpackProperty                (Si2168B_PropObj   *prop, unsigned int prop_code, int  data);
void          Si2168B_storePropertiesDefaults       (Si2168B_PropObj   *prop);

int  Si2168B_downloadCOMMONProperties(L1_Si2168B_Context *api);
int  Si2168B_downloadDDProperties    (L1_Si2168B_Context *api);
int  Si2168B_downloadDVBCProperties  (L1_Si2168B_Context *api);



int  Si2168B_downloadDVBTProperties  (L1_Si2168B_Context *api);

int  Si2168B_downloadDVBT2Properties (L1_Si2168B_Context *api);

int  Si2168B_downloadMCNSProperties  (L1_Si2168B_Context *api);

int  Si2168B_downloadSCANProperties  (L1_Si2168B_Context *api);
int  Si2168B_downloadAllProperties   (L1_Si2168B_Context *api);

#endif /* _Si2168B_PROPERTIES_FUNCTIONS_H_ */







