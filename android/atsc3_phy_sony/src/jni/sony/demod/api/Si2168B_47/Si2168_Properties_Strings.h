/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 1 API
   API properties strings definitions
   FILE: Si2168_Properties_Strings.h
   Supported IC : Si2168
   Compiled for ROM 2 firmware 2_0_build_11
   Revision: REVISION_NAME
   Date: September 24 2012
   Tag: TAGNAME
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef   _Si2168_PROPERTIES_STRINGS_H_
#define   _Si2168_PROPERTIES_STRINGS_H_

#ifdef Si2168_COMMAND_LINE_APPLICATION

extern Si2168_PropObj Si2168_prop;

typedef enum   {
  Si2168_UNSIGNED_CHAR,
  Si2168_SIGNED_CHAR,
  Si2168_UNSIGNED_INT,
  Si2168_SIGNED_INT
} Si2168_datatypeEnum;

typedef struct {
  const char *name;
  int   value;
} Si2168_optionStruct;

/* _properties_features_enum_insertion_start */
typedef enum {
  Si2168_COMMON,
  Si2168_DD,
  Si2168_DVBC,


  Si2168_DVBT,

  Si2168_DVBT2,

  Si2168_SCAN
} Si2168_featureEnum;
/* _properties_features_enum_insertion_point */

typedef struct {
  const char          *name;
  Si2168_datatypeEnum  datatype;
  void                *pField;
  int                  nbOptions;
  Si2168_optionStruct *option;
} Si2168_fieldDicoStruct;

typedef struct {
  Si2168_featureEnum      feature;
  const char             *name;
  int                     nbFields;
  Si2168_fieldDicoStruct *field;
  unsigned int            propertyCode;
} Si2168_propertyInfoStruct;

/* _properties_strings_extern_insertion_start */

extern Si2168_optionStruct    Si2168_DD_BER_RESOL_PROP_EXP[];
extern Si2168_optionStruct    Si2168_DD_BER_RESOL_PROP_MANT[];
extern Si2168_fieldDicoStruct Si2168_DD_BER_RESOL_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_CBER_RESOL_PROP_EXP[];
extern Si2168_optionStruct    Si2168_DD_CBER_RESOL_PROP_MANT[];
extern Si2168_fieldDicoStruct Si2168_DD_CBER_RESOL_PROP_FIELDS[];


extern Si2168_optionStruct    Si2168_DD_FER_RESOL_PROP_EXP[];
extern Si2168_optionStruct    Si2168_DD_FER_RESOL_PROP_MANT[];
extern Si2168_fieldDicoStruct Si2168_DD_FER_RESOL_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT0[];
extern Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT1[];
extern Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT2[];
extern Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT3[];
extern Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT4[];
extern Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT5[];
extern Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT6[];
extern Si2168_optionStruct    Si2168_DD_IEN_PROP_IEN_BIT7[];
extern Si2168_fieldDicoStruct Si2168_DD_IEN_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_IF_INPUT_FREQ_PROP_OFFSET[];
extern Si2168_fieldDicoStruct Si2168_DD_IF_INPUT_FREQ_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT0[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT1[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT2[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT3[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT4[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT5[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT6[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_NEG_BIT7[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT0[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT1[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT2[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT3[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT4[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT5[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT6[];
extern Si2168_optionStruct    Si2168_DD_INT_SENSE_PROP_POS_BIT7[];
extern Si2168_fieldDicoStruct Si2168_DD_INT_SENSE_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_MODE_PROP_BW[];
extern Si2168_optionStruct    Si2168_DD_MODE_PROP_MODULATION[];
extern Si2168_optionStruct    Si2168_DD_MODE_PROP_INVERT_SPECTRUM[];
extern Si2168_optionStruct    Si2168_DD_MODE_PROP_AUTO_DETECT[];
extern Si2168_fieldDicoStruct Si2168_DD_MODE_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_PER_RESOL_PROP_EXP[];
extern Si2168_optionStruct    Si2168_DD_PER_RESOL_PROP_MANT[];
extern Si2168_fieldDicoStruct Si2168_DD_PER_RESOL_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_RSQ_BER_THRESHOLD_PROP_EXP[];
extern Si2168_optionStruct    Si2168_DD_RSQ_BER_THRESHOLD_PROP_MANT[];
extern Si2168_fieldDicoStruct Si2168_DD_RSQ_BER_THRESHOLD_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_SSI_SQI_PARAM_PROP_SQI_AVERAGE[];
extern Si2168_fieldDicoStruct Si2168_DD_SSI_SQI_PARAM_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_TS_FREQ_PROP_REQ_FREQ_10KHZ[];
extern Si2168_fieldDicoStruct Si2168_DD_TS_FREQ_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_MODE[];
extern Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_CLOCK[];
extern Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_CLK_GAPPED_EN[];
extern Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_TS_ERR_POLARITY[];
extern Si2168_optionStruct    Si2168_DD_TS_MODE_PROP_SPECIAL[];
extern Si2168_fieldDicoStruct Si2168_DD_TS_MODE_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_STRENGTH[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_DATA_SHAPE[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_STRENGTH[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHAPE[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_INVERT[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_PAR_PROP_TS_CLK_SHIFT[];
extern Si2168_fieldDicoStruct Si2168_DD_TS_SETUP_PAR_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_STRENGTH[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_DATA_SHAPE[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_STRENGTH[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_SHAPE[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_CLK_INVERT[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_SYNC_DURATION[];
extern Si2168_optionStruct    Si2168_DD_TS_SETUP_SER_PROP_TS_BYTE_ORDER[];
extern Si2168_fieldDicoStruct Si2168_DD_TS_SETUP_SER_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DVBC_ADC_CREST_FACTOR_PROP_CREST_FACTOR[];
extern Si2168_fieldDicoStruct Si2168_DVBC_ADC_CREST_FACTOR_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DVBC_AFC_RANGE_PROP_RANGE_KHZ[];
extern Si2168_fieldDicoStruct Si2168_DVBC_AFC_RANGE_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DVBC_CONSTELLATION_PROP_CONSTELLATION[];
extern Si2168_fieldDicoStruct Si2168_DVBC_CONSTELLATION_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DVBC_SYMBOL_RATE_PROP_RATE[];
extern Si2168_fieldDicoStruct Si2168_DVBC_SYMBOL_RATE_PROP_FIELDS[];



extern Si2168_optionStruct    Si2168_DVBT2_ADC_CREST_FACTOR_PROP_CREST_FACTOR[];
extern Si2168_fieldDicoStruct Si2168_DVBT2_ADC_CREST_FACTOR_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DVBT2_AFC_RANGE_PROP_RANGE_KHZ[];
extern Si2168_fieldDicoStruct Si2168_DVBT2_AFC_RANGE_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DVBT2_FEF_TUNER_PROP_TUNER_DELAY[];
extern Si2168_optionStruct    Si2168_DVBT2_FEF_TUNER_PROP_TUNER_FREEZE_TIME[];
extern Si2168_optionStruct    Si2168_DVBT2_FEF_TUNER_PROP_TUNER_UNFREEZE_TIME[];
extern Si2168_fieldDicoStruct Si2168_DVBT2_FEF_TUNER_PROP_FIELDS[];


extern Si2168_optionStruct    Si2168_DVBT_ADC_CREST_FACTOR_PROP_CREST_FACTOR[];
extern Si2168_fieldDicoStruct Si2168_DVBT_ADC_CREST_FACTOR_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DVBT_AFC_RANGE_PROP_RANGE_KHZ[];
extern Si2168_fieldDicoStruct Si2168_DVBT_AFC_RANGE_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_DVBT_HIERARCHY_PROP_STREAM[];
extern Si2168_fieldDicoStruct Si2168_DVBT_HIERARCHY_PROP_FIELDS[];


extern Si2168_optionStruct    Si2168_MASTER_IEN_PROP_DDIEN[];
extern Si2168_optionStruct    Si2168_MASTER_IEN_PROP_SCANIEN[];
extern Si2168_optionStruct    Si2168_MASTER_IEN_PROP_ERRIEN[];
extern Si2168_optionStruct    Si2168_MASTER_IEN_PROP_CTSIEN[];
extern Si2168_fieldDicoStruct Si2168_MASTER_IEN_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_SCAN_FMAX_PROP_SCAN_FMAX[];
extern Si2168_fieldDicoStruct Si2168_SCAN_FMAX_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_SCAN_FMIN_PROP_SCAN_FMIN[];
extern Si2168_fieldDicoStruct Si2168_SCAN_FMIN_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_SCAN_IEN_PROP_BUZIEN[];
extern Si2168_optionStruct    Si2168_SCAN_IEN_PROP_REQIEN[];
extern Si2168_fieldDicoStruct Si2168_SCAN_IEN_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_SCAN_INT_SENSE_PROP_BUZNEGEN[];
extern Si2168_optionStruct    Si2168_SCAN_INT_SENSE_PROP_REQNEGEN[];
extern Si2168_optionStruct    Si2168_SCAN_INT_SENSE_PROP_BUZPOSEN[];
extern Si2168_optionStruct    Si2168_SCAN_INT_SENSE_PROP_REQPOSEN[];
extern Si2168_fieldDicoStruct Si2168_SCAN_INT_SENSE_PROP_FIELDS[];


extern Si2168_optionStruct    Si2168_SCAN_SYMB_RATE_MAX_PROP_SCAN_SYMB_RATE_MAX[];
extern Si2168_fieldDicoStruct Si2168_SCAN_SYMB_RATE_MAX_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_SCAN_SYMB_RATE_MIN_PROP_SCAN_SYMB_RATE_MIN[];
extern Si2168_fieldDicoStruct Si2168_SCAN_SYMB_RATE_MIN_PROP_FIELDS[];

extern Si2168_optionStruct    Si2168_SCAN_TER_CONFIG_PROP_MODE[];
extern Si2168_optionStruct    Si2168_SCAN_TER_CONFIG_PROP_ANALOG_BW[];
extern Si2168_optionStruct    Si2168_SCAN_TER_CONFIG_PROP_SEARCH_ANALOG[];
extern Si2168_fieldDicoStruct Si2168_SCAN_TER_CONFIG_PROP_FIELDS[];


/* _properties_strings_extern_insertion_point */
extern Si2168_propertyInfoStruct Si2168_propertyDictionary[];

int   Si2168_PropertyNames       (L1_Si2168_Context *api, char *msg);
int   Si2168_PropertyIndex       (L1_Si2168_Context *api, const char *property);
int   Si2168_PropertyFields      (L1_Si2168_Context *api, int propIndex, char *msg);
int   Si2168_PropertyFieldIndex  (L1_Si2168_Context *api, int propIndex, char *field);
int   Si2168_PropertyFieldEnums  (L1_Si2168_Context *api, int propIndex, int fieldIndex, char *msg);
int   Si2168_GetValueFromEntry   (L1_Si2168_Context *api, Si2168_fieldDicoStruct field, char *entry, int *value);
char *Si2168_GetEnumFromValue    (L1_Si2168_Context *api, Si2168_fieldDicoStruct field, int iValue);
char *Si2168_GetEnumFromField    (L1_Si2168_Context *api, Si2168_fieldDicoStruct field);
int   Si2168_SetFieldFromEntry   (L1_Si2168_Context *api, Si2168_fieldDicoStruct field, char *entry);
void  Si2168_showProperty        (L1_Si2168_Context *api, int propertyIndex);
void  Si2168_showPropertyNamed   (L1_Si2168_Context *api, char *property);
int   Si2168_showProperties      (L1_Si2168_Context *api, Si2168_featureEnum feature);
void  Si2168_setupProperty       (L1_Si2168_Context *api, int propIndex);
int   Si2168_setupProperties     (L1_Si2168_Context *api, Si2168_featureEnum feature);

#endif /* Si2168_COMMAND_LINE_APPLICATION */

/* _properties_strings_protos_insertion_start */

#ifdef    Si2168_COMMAND_LINE_APPLICATION
int  Si2168_showCOMMONProperties(L1_Si2168_Context *api);
int  Si2168_showDDProperties    (L1_Si2168_Context *api);
int  Si2168_showDVBCProperties  (L1_Si2168_Context *api);


int  Si2168_showDVBTProperties  (L1_Si2168_Context *api);

int  Si2168_showDVBT2Properties (L1_Si2168_Context *api);

int  Si2168_showSCANProperties  (L1_Si2168_Context *api);


int  Si2168_showAllProperties    (L1_Si2168_Context *api);
#endif /* Si2168_COMMAND_LINE_APPLICATION */
void  Si2168_setupCOMMONDefaults(L1_Si2168_Context *api);
void  Si2168_setupDDDefaults    (L1_Si2168_Context *api);
void  Si2168_setupDVBCDefaults  (L1_Si2168_Context *api);


void  Si2168_setupDVBTDefaults  (L1_Si2168_Context *api);

void  Si2168_setupDVBT2Defaults (L1_Si2168_Context *api);

void  Si2168_setupSCANDefaults  (L1_Si2168_Context *api);

void  Si2168_setupAllDefaults   (L1_Si2168_Context *api);

int  Si2168_setupCOMMONProperties(L1_Si2168_Context *api);
int  Si2168_setupDDProperties    (L1_Si2168_Context *api);
int  Si2168_setupDVBCProperties  (L1_Si2168_Context *api);


int  Si2168_setupDVBTProperties  (L1_Si2168_Context *api);

int  Si2168_setupDVBT2Properties (L1_Si2168_Context *api);

int  Si2168_setupSCANProperties  (L1_Si2168_Context *api);
int  Si2168_setupAllProperties   (L1_Si2168_Context *api);

int  Si2168_downloadCOMMONProperties(L1_Si2168_Context *api);
int  Si2168_downloadDDProperties    (L1_Si2168_Context *api);
int  Si2168_downloadDVBCProperties  (L1_Si2168_Context *api);


int  Si2168_downloadDVBTProperties  (L1_Si2168_Context *api);

int  Si2168_downloadDVBT2Properties (L1_Si2168_Context *api);

int  Si2168_downloadSCANProperties  (L1_Si2168_Context *api);
int  Si2168_downloadAllProperties   (L1_Si2168_Context *api);
/* _properties_strings_protos_insertion_point */

#endif /* _Si2168_PROPERTIES_STRINGS_H_ */





