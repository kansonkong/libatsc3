/*************************************************************************************
                  Silicon Laboratories Broadcast Si2148 Layer 1 API
   API properties strings definitions
   FILE: Si2148_Properties_Strings.h
   Supported IC : Si2148
   Compiled for ROM 33 firmware 2_1_build_6
   Revision: 0.1
   Date: November 12 2012
   Tag: ROM33_2_1_build_6_V0.1
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef   _Si2148_PROPERTIES_STRINGS_H_
#define   _Si2148_PROPERTIES_STRINGS_H_

#ifdef Si2148_COMMAND_LINE_APPLICATION

extern Si2148_PropObj Si2148_prop;

typedef enum   {
  Si2148_UNSIGNED_CHAR,
  Si2148_SIGNED_CHAR,
  Si2148_UNSIGNED_INT,
  Si2148_SIGNED_INT
} Si2148_datatypeEnum;

typedef struct {
  const char *name;
  int   value;
} Si2148_optionStruct;

/* _properties_features_enum_insertion_start */
typedef enum {
  Si2148_ATV,
  Si2148_COMMON,
  Si2148_DTV,
  Si2148_TUNER
} Si2148_featureEnum;
/* _properties_features_enum_insertion_point */

typedef struct {
  const char          *name;
  Si2148_datatypeEnum  datatype;
  void                *pField;
  int                  nbOptions;
  Si2148_optionStruct *option;
} Si2148_fieldDicoStruct;

typedef struct {
  Si2148_featureEnum      feature;
  const char             *name;
  int                     nbFields;
  Si2148_fieldDicoStruct *field;
  unsigned int            propertyCode;
} Si2148_propertyInfoStruct;

/* _properties_strings_extern_insertion_start */

extern Si2148_optionStruct    Si2148_ATV_ARTIFICIAL_SNOW_PROP_GAIN[];
extern Si2148_optionStruct    Si2148_ATV_ARTIFICIAL_SNOW_PROP_OFFSET[];
extern Si2148_fieldDicoStruct Si2148_ATV_ARTIFICIAL_SNOW_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_ATV_EXT_AGC_PROP_MIN_10MV[];
extern Si2148_optionStruct    Si2148_ATV_EXT_AGC_PROP_MAX_10MV[];
extern Si2148_fieldDicoStruct Si2148_ATV_EXT_AGC_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_ATV_LIF_FREQ_PROP_OFFSET[];
extern Si2148_fieldDicoStruct Si2148_ATV_LIF_FREQ_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_ATV_LIF_OUT_PROP_OFFSET[];
extern Si2148_optionStruct    Si2148_ATV_LIF_OUT_PROP_AMP[];
extern Si2148_fieldDicoStruct Si2148_ATV_LIF_OUT_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_ATV_PGA_TARGET_PROP_PGA_TARGET[];
extern Si2148_optionStruct    Si2148_ATV_PGA_TARGET_PROP_OVERRIDE_ENABLE[];
extern Si2148_fieldDicoStruct Si2148_ATV_PGA_TARGET_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_CRYSTAL_TRIM_PROP_XO_CAP[];
extern Si2148_fieldDicoStruct Si2148_CRYSTAL_TRIM_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_AGC_FREEZE_INPUT_PROP_LEVEL[];
extern Si2148_optionStruct    Si2148_DTV_AGC_FREEZE_INPUT_PROP_PIN[];
extern Si2148_fieldDicoStruct Si2148_DTV_AGC_FREEZE_INPUT_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_AGC_SPEED_PROP_IF_AGC_SPEED[];
extern Si2148_optionStruct    Si2148_DTV_AGC_SPEED_PROP_AGC_DECIM[];
extern Si2148_fieldDicoStruct Si2148_DTV_AGC_SPEED_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_CONFIG_IF_PORT_PROP_DTV_OUT_TYPE[];
extern Si2148_optionStruct    Si2148_DTV_CONFIG_IF_PORT_PROP_DTV_AGC_SOURCE[];
extern Si2148_fieldDicoStruct Si2148_DTV_CONFIG_IF_PORT_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_EXT_AGC_PROP_MIN_10MV[];
extern Si2148_optionStruct    Si2148_DTV_EXT_AGC_PROP_MAX_10MV[];
extern Si2148_fieldDicoStruct Si2148_DTV_EXT_AGC_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_FILTER_SELECT_PROP_FILTER[];
extern Si2148_fieldDicoStruct Si2148_DTV_FILTER_SELECT_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_IEN_PROP_CHLIEN[];
extern Si2148_fieldDicoStruct Si2148_DTV_IEN_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_INITIAL_AGC_SPEED_PROP_IF_AGC_SPEED[];
extern Si2148_optionStruct    Si2148_DTV_INITIAL_AGC_SPEED_PROP_AGC_DECIM[];
extern Si2148_fieldDicoStruct Si2148_DTV_INITIAL_AGC_SPEED_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_PERIOD[];
extern Si2148_fieldDicoStruct Si2148_DTV_INITIAL_AGC_SPEED_PERIOD_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_INTERNAL_ZIF_PROP_ATSC[];
extern Si2148_optionStruct    Si2148_DTV_INTERNAL_ZIF_PROP_QAM_US[];
extern Si2148_optionStruct    Si2148_DTV_INTERNAL_ZIF_PROP_DVBT[];
extern Si2148_optionStruct    Si2148_DTV_INTERNAL_ZIF_PROP_DVBC[];
extern Si2148_optionStruct    Si2148_DTV_INTERNAL_ZIF_PROP_ISDBT[];
extern Si2148_optionStruct    Si2148_DTV_INTERNAL_ZIF_PROP_ISDBC[];
extern Si2148_optionStruct    Si2148_DTV_INTERNAL_ZIF_PROP_DTMB[];
extern Si2148_fieldDicoStruct Si2148_DTV_INTERNAL_ZIF_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_INT_SENSE_PROP_CHLNEGEN[];
extern Si2148_optionStruct    Si2148_DTV_INT_SENSE_PROP_CHLPOSEN[];
extern Si2148_fieldDicoStruct Si2148_DTV_INT_SENSE_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_LIF_FREQ_PROP_OFFSET[];
extern Si2148_fieldDicoStruct Si2148_DTV_LIF_FREQ_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_LIF_OUT_PROP_OFFSET[];
extern Si2148_optionStruct    Si2148_DTV_LIF_OUT_PROP_AMP[];
extern Si2148_fieldDicoStruct Si2148_DTV_LIF_OUT_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_MODE_PROP_BW[];
extern Si2148_optionStruct    Si2148_DTV_MODE_PROP_MODULATION[];
extern Si2148_optionStruct    Si2148_DTV_MODE_PROP_INVERT_SPECTRUM[];
extern Si2148_fieldDicoStruct Si2148_DTV_MODE_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_PGA_LIMITS_PROP_MIN[];
extern Si2148_optionStruct    Si2148_DTV_PGA_LIMITS_PROP_MAX[];
extern Si2148_fieldDicoStruct Si2148_DTV_PGA_LIMITS_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_PGA_TARGET_PROP_PGA_TARGET[];
extern Si2148_optionStruct    Si2148_DTV_PGA_TARGET_PROP_OVERRIDE_ENABLE[];
extern Si2148_fieldDicoStruct Si2148_DTV_PGA_TARGET_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_RF_TOP_PROP_DTV_RF_TOP[];
extern Si2148_fieldDicoStruct Si2148_DTV_RF_TOP_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_RSQ_RSSI_THRESHOLD_PROP_LO[];
extern Si2148_optionStruct    Si2148_DTV_RSQ_RSSI_THRESHOLD_PROP_HI[];
extern Si2148_fieldDicoStruct Si2148_DTV_RSQ_RSSI_THRESHOLD_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_DTV_ZIF_DC_CANCELLER_BW_PROP_BANDWIDTH[];
extern Si2148_fieldDicoStruct Si2148_DTV_ZIF_DC_CANCELLER_BW_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_MASTER_IEN_PROP_TUNIEN[];
extern Si2148_optionStruct    Si2148_MASTER_IEN_PROP_ATVIEN[];
extern Si2148_optionStruct    Si2148_MASTER_IEN_PROP_DTVIEN[];
extern Si2148_optionStruct    Si2148_MASTER_IEN_PROP_ERRIEN[];
extern Si2148_optionStruct    Si2148_MASTER_IEN_PROP_CTSIEN[];
extern Si2148_fieldDicoStruct Si2148_MASTER_IEN_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_TUNER_BLOCKED_VCO_PROP_VCO_CODE[];
extern Si2148_fieldDicoStruct Si2148_TUNER_BLOCKED_VCO_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_TUNER_IEN_PROP_TCIEN[];
extern Si2148_optionStruct    Si2148_TUNER_IEN_PROP_RSSILIEN[];
extern Si2148_optionStruct    Si2148_TUNER_IEN_PROP_RSSIHIEN[];
extern Si2148_fieldDicoStruct Si2148_TUNER_IEN_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_TUNER_INT_SENSE_PROP_TCNEGEN[];
extern Si2148_optionStruct    Si2148_TUNER_INT_SENSE_PROP_RSSILNEGEN[];
extern Si2148_optionStruct    Si2148_TUNER_INT_SENSE_PROP_RSSIHNEGEN[];
extern Si2148_optionStruct    Si2148_TUNER_INT_SENSE_PROP_TCPOSEN[];
extern Si2148_optionStruct    Si2148_TUNER_INT_SENSE_PROP_RSSILPOSEN[];
extern Si2148_optionStruct    Si2148_TUNER_INT_SENSE_PROP_RSSIHPOSEN[];
extern Si2148_fieldDicoStruct Si2148_TUNER_INT_SENSE_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_TUNER_LO_INJECTION_PROP_BAND_1[];
extern Si2148_optionStruct    Si2148_TUNER_LO_INJECTION_PROP_BAND_2[];
extern Si2148_optionStruct    Si2148_TUNER_LO_INJECTION_PROP_BAND_3[];
extern Si2148_fieldDicoStruct Si2148_TUNER_LO_INJECTION_PROP_FIELDS[];

extern Si2148_optionStruct    Si2148_TUNER_RETURN_LOSS_PROP_CONFIG[];
extern Si2148_optionStruct    Si2148_TUNER_RETURN_LOSS_PROP_MODE[];
extern Si2148_fieldDicoStruct Si2148_TUNER_RETURN_LOSS_PROP_FIELDS[];

/* _properties_strings_extern_insertion_point */
extern Si2148_propertyInfoStruct Si2148_propertyDictionary[];

int   Si2148_PropertyNames       (L1_Si2148_Context *api, char *msg);
int   Si2148_PropertyIndex       (L1_Si2148_Context *api, const char *property);
int   Si2148_PropertyFields      (L1_Si2148_Context *api, int propIndex, char *msg);
int   Si2148_PropertyFieldIndex  (L1_Si2148_Context *api, int propIndex, char *field);
int   Si2148_PropertyFieldEnums  (L1_Si2148_Context *api, int propIndex, int fieldIndex, char *msg);
int   Si2148_GetValueFromEntry   (L1_Si2148_Context *api, Si2148_fieldDicoStruct field, char *entry, int *value);
char *Si2148_GetEnumFromValue    (L1_Si2148_Context *api, Si2148_fieldDicoStruct field, int iValue);
char *Si2148_GetEnumFromField    (L1_Si2148_Context *api, Si2148_fieldDicoStruct field);
int   Si2148_SetFieldFromEntry   (L1_Si2148_Context *api, Si2148_fieldDicoStruct field, char *entry);
void  Si2148_showProperty        (L1_Si2148_Context *api, int propertyIndex);
void  Si2148_showPropertyNamed   (L1_Si2148_Context *api, char *property);
int   Si2148_showProperties      (L1_Si2148_Context *api, Si2148_featureEnum feature);
void  Si2148_setupProperty       (L1_Si2148_Context *api, int propIndex);
int   Si2148_setupProperties     (L1_Si2148_Context *api, Si2148_featureEnum feature);

#endif /* Si2148_COMMAND_LINE_APPLICATION */

/* _properties_strings_protos_insertion_start */

#ifdef    Si2148_COMMAND_LINE_APPLICATION
int  Si2148_showATVProperties   (L1_Si2148_Context *api);
int  Si2148_showCOMMONProperties(L1_Si2148_Context *api);
int  Si2148_showDTVProperties   (L1_Si2148_Context *api);
int  Si2148_showTUNERProperties (L1_Si2148_Context *api);


int  Si2148_showAllProperties    (L1_Si2148_Context *api);
#endif /* Si2148_COMMAND_LINE_APPLICATION */
void  Si2148_setupATVDefaults   (L1_Si2148_Context *api);
void  Si2148_setupCOMMONDefaults(L1_Si2148_Context *api);
void  Si2148_setupDTVDefaults   (L1_Si2148_Context *api);
void  Si2148_setupTUNERDefaults (L1_Si2148_Context *api);

void  Si2148_setupAllDefaults   (L1_Si2148_Context *api);

int  Si2148_setupATVProperties   (L1_Si2148_Context *api);
int  Si2148_setupCOMMONProperties(L1_Si2148_Context *api);
int  Si2148_setupDTVProperties   (L1_Si2148_Context *api);
int  Si2148_setupTUNERProperties (L1_Si2148_Context *api);
int  Si2148_setupAllProperties   (L1_Si2148_Context *api);

int  Si2148_downloadATVProperties   (L1_Si2148_Context *api);
int  Si2148_downloadCOMMONProperties(L1_Si2148_Context *api);
int  Si2148_downloadDTVProperties   (L1_Si2148_Context *api);
int  Si2148_downloadTUNERProperties (L1_Si2148_Context *api);
int  Si2148_downloadAllProperties   (L1_Si2148_Context *api);
/* _properties_strings_protos_insertion_point */

#endif /* _Si2148_PROPERTIES_STRINGS_H_ */





