/*************************************************************************************
                  Silicon Laboratories Broadcast Si2168 Layer 1 API
   API commands structures and functions definitions
   FILE: Si2168_Commands.h
   Supported IC : Si2168
   Compiled for ROM 2 firmware 2_0_build_11
   Revision: REVISION_NAME
   Date: September 24 2012
  (C) Copyright 2012, Silicon Laboratories, Inc. All rights reserved.
**************************************************************************************/
#ifndef    Si2168_COMMANDS_H
#define    Si2168_COMMANDS_H

/* _status_defines_insertion_start */

/* STATUS structure definition */
  typedef struct { /* Si2168_COMMON_REPLY_struct */
    unsigned char   ddint;
    unsigned char   scanint;
    unsigned char   err;
    unsigned char   cts;
 }  Si2168_COMMON_REPLY_struct;

/* STATUS fields definition */
  /* STATUS, DDINT field definition (address 0, size 1, lsb 0, unsigned)*/
  #define  Si2168_STATUS_DDINT_LSB         0
  #define  Si2168_STATUS_DDINT_MASK        0x01
   #define Si2168_STATUS_DDINT_NOT_TRIGGERED  0
   #define Si2168_STATUS_DDINT_TRIGGERED      1
  /* STATUS, SCANINT field definition (address 0, size 1, lsb 1, unsigned)*/
  #define  Si2168_STATUS_SCANINT_LSB         1
  #define  Si2168_STATUS_SCANINT_MASK        0x01
   #define Si2168_STATUS_SCANINT_NOT_TRIGGERED  0
   #define Si2168_STATUS_SCANINT_TRIGGERED      1
  /* STATUS, ERR field definition (address 0, size 1, lsb 6, unsigned)*/
  #define  Si2168_STATUS_ERR_LSB         6
  #define  Si2168_STATUS_ERR_MASK        0x01
   #define Si2168_STATUS_ERR_ERROR     1
   #define Si2168_STATUS_ERR_NO_ERROR  0
  /* STATUS, CTS field definition (address 0, size 1, lsb 7, unsigned)*/
  #define  Si2168_STATUS_CTS_LSB         7
  #define  Si2168_STATUS_CTS_MASK        0x01
   #define Si2168_STATUS_CTS_COMPLETED  1
   #define Si2168_STATUS_CTS_WAIT       0

/* _status_defines_insertion_point */

/* _commands_defines_insertion_start */
/* Si2168_CONFIG_CLKIO command definition */
#define   Si2168_CONFIG_CLKIO_CMD 0x18

#ifdef    Si2168_CONFIG_CLKIO_CMD
  #define Si2168_CONFIG_CLKIO_CMD_CODE 0x010018

    typedef struct { /* Si2168_CONFIG_CLKIO_CMD_struct */
     unsigned char   output;
     unsigned char   pre_driver_str;
     unsigned char   driver_str;
   } Si2168_CONFIG_CLKIO_CMD_struct;


    typedef struct { /* Si2168_CONFIG_CLKIO_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   mode;
      unsigned char   pre_driver_str;
      unsigned char   driver_str;
   }  Si2168_CONFIG_CLKIO_CMD_REPLY_struct;

   /* CONFIG_CLKIO command, OUTPUT field definition (address 1,size 2, lsb 0, unsigned) */
   #define  Si2168_CONFIG_CLKIO_CMD_OUTPUT_LSB         0
   #define  Si2168_CONFIG_CLKIO_CMD_OUTPUT_MASK        0x03
   #define  Si2168_CONFIG_CLKIO_CMD_OUTPUT_MIN         0
   #define  Si2168_CONFIG_CLKIO_CMD_OUTPUT_MAX         2
    #define Si2168_CONFIG_CLKIO_CMD_OUTPUT_NO_CHANGE  0
    #define Si2168_CONFIG_CLKIO_CMD_OUTPUT_OFF        2
    #define Si2168_CONFIG_CLKIO_CMD_OUTPUT_ON         1
   /* CONFIG_CLKIO command, PRE_DRIVER_STR field definition (address 1,size 2, lsb 2, unsigned) */
   #define  Si2168_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_LSB         2
   #define  Si2168_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_MASK        0x03
   #define  Si2168_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_MIN         0
   #define  Si2168_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_MAX         3
    #define Si2168_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_PRE_DRIVER_MIN  0
    #define Si2168_CONFIG_CLKIO_CMD_PRE_DRIVER_STR_PRE_DRIVER_MAX  3
   /* CONFIG_CLKIO command, DRIVER_STR field definition (address 1,size 4, lsb 4, unsigned) */
   #define  Si2168_CONFIG_CLKIO_CMD_DRIVER_STR_LSB         4
   #define  Si2168_CONFIG_CLKIO_CMD_DRIVER_STR_MASK        0x0f
   #define  Si2168_CONFIG_CLKIO_CMD_DRIVER_STR_MIN         0
   #define  Si2168_CONFIG_CLKIO_CMD_DRIVER_STR_MAX         15
    #define Si2168_CONFIG_CLKIO_CMD_DRIVER_STR_DRIVER_MIN  0
    #define Si2168_CONFIG_CLKIO_CMD_DRIVER_STR_DRIVER_MAX  15
   /* CONFIG_CLKIO command, MODE field definition (address 1, size 2, lsb 0, unsigned)*/
   #define  Si2168_CONFIG_CLKIO_RESPONSE_MODE_LSB         0
   #define  Si2168_CONFIG_CLKIO_RESPONSE_MODE_MASK        0x03
    #define Si2168_CONFIG_CLKIO_RESPONSE_MODE_CLK_INPUT   2
    #define Si2168_CONFIG_CLKIO_RESPONSE_MODE_CLK_OUTPUT  1
    #define Si2168_CONFIG_CLKIO_RESPONSE_MODE_UNUSED      0
   /* CONFIG_CLKIO command, PRE_DRIVER_STR field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_CONFIG_CLKIO_RESPONSE_PRE_DRIVER_STR_LSB         0
   #define  Si2168_CONFIG_CLKIO_RESPONSE_PRE_DRIVER_STR_MASK        0xff
   /* CONFIG_CLKIO command, DRIVER_STR field definition (address 3, size 8, lsb 0, unsigned)*/
   #define  Si2168_CONFIG_CLKIO_RESPONSE_DRIVER_STR_LSB         0
   #define  Si2168_CONFIG_CLKIO_RESPONSE_DRIVER_STR_MASK        0xff

#endif /* Si2168_CONFIG_CLKIO_CMD */

/* Si2168_CONFIG_PINS command definition */
#define   Si2168_CONFIG_PINS_CMD 0x12

#ifdef    Si2168_CONFIG_PINS_CMD
  #define Si2168_CONFIG_PINS_CMD_CODE 0x010012

    typedef struct { /* Si2168_CONFIG_PINS_CMD_struct */
     unsigned char   gpio0_mode;
     unsigned char   gpio0_read;
     unsigned char   gpio1_mode;
     unsigned char   gpio1_read;
   } Si2168_CONFIG_PINS_CMD_struct;


    typedef struct { /* Si2168_CONFIG_PINS_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   gpio0_mode;
      unsigned char   gpio0_state;
      unsigned char   gpio1_mode;
      unsigned char   gpio1_state;
   }  Si2168_CONFIG_PINS_CMD_REPLY_struct;

   /* CONFIG_PINS command, GPIO0_MODE field definition (address 1,size 7, lsb 0, unsigned) */
   #define  Si2168_CONFIG_PINS_CMD_GPIO0_MODE_LSB         0
   #define  Si2168_CONFIG_PINS_CMD_GPIO0_MODE_MASK        0x7f
   #define  Si2168_CONFIG_PINS_CMD_GPIO0_MODE_MIN         0
   #define  Si2168_CONFIG_PINS_CMD_GPIO0_MODE_MAX         8
    #define Si2168_CONFIG_PINS_CMD_GPIO0_MODE_DISABLE    1
    #define Si2168_CONFIG_PINS_CMD_GPIO0_MODE_DRIVE_0    2
    #define Si2168_CONFIG_PINS_CMD_GPIO0_MODE_DRIVE_1    3
    #define Si2168_CONFIG_PINS_CMD_GPIO0_MODE_HW_LOCK    8
    #define Si2168_CONFIG_PINS_CMD_GPIO0_MODE_INT_FLAG   7
    #define Si2168_CONFIG_PINS_CMD_GPIO0_MODE_NO_CHANGE  0
    #define Si2168_CONFIG_PINS_CMD_GPIO0_MODE_TS_ERR     4
   /* CONFIG_PINS command, GPIO0_READ field definition (address 1,size 1, lsb 7, unsigned) */
   #define  Si2168_CONFIG_PINS_CMD_GPIO0_READ_LSB         7
   #define  Si2168_CONFIG_PINS_CMD_GPIO0_READ_MASK        0x01
   #define  Si2168_CONFIG_PINS_CMD_GPIO0_READ_MIN         0
   #define  Si2168_CONFIG_PINS_CMD_GPIO0_READ_MAX         1
    #define Si2168_CONFIG_PINS_CMD_GPIO0_READ_DO_NOT_READ  0
    #define Si2168_CONFIG_PINS_CMD_GPIO0_READ_READ         1
   /* CONFIG_PINS command, GPIO1_MODE field definition (address 2,size 7, lsb 0, unsigned) */
   #define  Si2168_CONFIG_PINS_CMD_GPIO1_MODE_LSB         0
   #define  Si2168_CONFIG_PINS_CMD_GPIO1_MODE_MASK        0x7f
   #define  Si2168_CONFIG_PINS_CMD_GPIO1_MODE_MIN         0
   #define  Si2168_CONFIG_PINS_CMD_GPIO1_MODE_MAX         8
    #define Si2168_CONFIG_PINS_CMD_GPIO1_MODE_DISABLE    1
    #define Si2168_CONFIG_PINS_CMD_GPIO1_MODE_DRIVE_0    2
    #define Si2168_CONFIG_PINS_CMD_GPIO1_MODE_DRIVE_1    3
    #define Si2168_CONFIG_PINS_CMD_GPIO1_MODE_HW_LOCK    8
    #define Si2168_CONFIG_PINS_CMD_GPIO1_MODE_INT_FLAG   7
    #define Si2168_CONFIG_PINS_CMD_GPIO1_MODE_NO_CHANGE  0
    #define Si2168_CONFIG_PINS_CMD_GPIO1_MODE_TS_ERR     4
   /* CONFIG_PINS command, GPIO1_READ field definition (address 2,size 1, lsb 7, unsigned) */
   #define  Si2168_CONFIG_PINS_CMD_GPIO1_READ_LSB         7
   #define  Si2168_CONFIG_PINS_CMD_GPIO1_READ_MASK        0x01
   #define  Si2168_CONFIG_PINS_CMD_GPIO1_READ_MIN         0
   #define  Si2168_CONFIG_PINS_CMD_GPIO1_READ_MAX         1
    #define Si2168_CONFIG_PINS_CMD_GPIO1_READ_DO_NOT_READ  0
    #define Si2168_CONFIG_PINS_CMD_GPIO1_READ_READ         1
   /* CONFIG_PINS command, GPIO0_MODE field definition (address 1, size 7, lsb 0, unsigned)*/
   #define  Si2168_CONFIG_PINS_RESPONSE_GPIO0_MODE_LSB         0
   #define  Si2168_CONFIG_PINS_RESPONSE_GPIO0_MODE_MASK        0x7f
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO0_MODE_DISABLE   1
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO0_MODE_DRIVE_0   2
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO0_MODE_DRIVE_1   3
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO0_MODE_HW_LOCK   8
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO0_MODE_INT_FLAG  7
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO0_MODE_TS_ERR    4
   /* CONFIG_PINS command, GPIO0_STATE field definition (address 1, size 1, lsb 7, unsigned)*/
   #define  Si2168_CONFIG_PINS_RESPONSE_GPIO0_STATE_LSB         7
   #define  Si2168_CONFIG_PINS_RESPONSE_GPIO0_STATE_MASK        0x01
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO0_STATE_READ_0  0
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO0_STATE_READ_1  1
   /* CONFIG_PINS command, GPIO1_MODE field definition (address 2, size 7, lsb 0, unsigned)*/
   #define  Si2168_CONFIG_PINS_RESPONSE_GPIO1_MODE_LSB         0
   #define  Si2168_CONFIG_PINS_RESPONSE_GPIO1_MODE_MASK        0x7f
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO1_MODE_DISABLE   1
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO1_MODE_DRIVE_0   2
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO1_MODE_DRIVE_1   3
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO1_MODE_HW_LOCK   8
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO1_MODE_INT_FLAG  7
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO1_MODE_TS_ERR    4
   /* CONFIG_PINS command, GPIO1_STATE field definition (address 2, size 1, lsb 7, unsigned)*/
   #define  Si2168_CONFIG_PINS_RESPONSE_GPIO1_STATE_LSB         7
   #define  Si2168_CONFIG_PINS_RESPONSE_GPIO1_STATE_MASK        0x01
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO1_STATE_READ_0  0
    #define Si2168_CONFIG_PINS_RESPONSE_GPIO1_STATE_READ_1  1

#endif /* Si2168_CONFIG_PINS_CMD */

/* Si2168_DD_BER command definition */
#define   Si2168_DD_BER_CMD 0x82

#ifdef    Si2168_DD_BER_CMD
  #define Si2168_DD_BER_CMD_CODE 0x010082

    typedef struct { /* Si2168_DD_BER_CMD_struct */
     unsigned char   rst;
   } Si2168_DD_BER_CMD_struct;


    typedef struct { /* Si2168_DD_BER_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   exp;
      unsigned char   mant;
   }  Si2168_DD_BER_CMD_REPLY_struct;

   /* DD_BER command, RST field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DD_BER_CMD_RST_LSB         0
   #define  Si2168_DD_BER_CMD_RST_MASK        0x01
   #define  Si2168_DD_BER_CMD_RST_MIN         0
   #define  Si2168_DD_BER_CMD_RST_MAX         1
    #define Si2168_DD_BER_CMD_RST_CLEAR  1
    #define Si2168_DD_BER_CMD_RST_RUN    0
   /* DD_BER command, EXP field definition (address 1, size 4, lsb 0, unsigned)*/
   #define  Si2168_DD_BER_RESPONSE_EXP_LSB         0
   #define  Si2168_DD_BER_RESPONSE_EXP_MASK        0x0f
    #define Si2168_DD_BER_RESPONSE_EXP_EXP_MIN  0
    #define Si2168_DD_BER_RESPONSE_EXP_EXP_MAX  8
   /* DD_BER command, MANT field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_BER_RESPONSE_MANT_LSB         0
   #define  Si2168_DD_BER_RESPONSE_MANT_MASK        0xff
    #define Si2168_DD_BER_RESPONSE_MANT_MANT_MIN  0
    #define Si2168_DD_BER_RESPONSE_MANT_MANT_MAX  99

#endif /* Si2168_DD_BER_CMD */

/* Si2168_DD_CBER command definition */
#define   Si2168_DD_CBER_CMD 0x81

#ifdef    Si2168_DD_CBER_CMD
  #define Si2168_DD_CBER_CMD_CODE 0x010081

    typedef struct { /* Si2168_DD_CBER_CMD_struct */
     unsigned char   rst;
   } Si2168_DD_CBER_CMD_struct;


    typedef struct { /* Si2168_DD_CBER_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   exp;
      unsigned char   mant;
   }  Si2168_DD_CBER_CMD_REPLY_struct;

   /* DD_CBER command, RST field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DD_CBER_CMD_RST_LSB         0
   #define  Si2168_DD_CBER_CMD_RST_MASK        0x01
   #define  Si2168_DD_CBER_CMD_RST_MIN         0
   #define  Si2168_DD_CBER_CMD_RST_MAX         1
    #define Si2168_DD_CBER_CMD_RST_CLEAR  1
    #define Si2168_DD_CBER_CMD_RST_RUN    0
   /* DD_CBER command, EXP field definition (address 1, size 4, lsb 0, unsigned)*/
   #define  Si2168_DD_CBER_RESPONSE_EXP_LSB         0
   #define  Si2168_DD_CBER_RESPONSE_EXP_MASK        0x0f
    #define Si2168_DD_CBER_RESPONSE_EXP_EXP_MIN  0
    #define Si2168_DD_CBER_RESPONSE_EXP_EXP_MAX  8
   /* DD_CBER command, MANT field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_CBER_RESPONSE_MANT_LSB         0
   #define  Si2168_DD_CBER_RESPONSE_MANT_MASK        0xff
    #define Si2168_DD_CBER_RESPONSE_MANT_MANT_MIN  0
    #define Si2168_DD_CBER_RESPONSE_MANT_MANT_MAX  99

#endif /* Si2168_DD_CBER_CMD */


/* Si2168_DD_EXT_AGC_TER command definition */
#define   Si2168_DD_EXT_AGC_TER_CMD 0x89

#ifdef    Si2168_DD_EXT_AGC_TER_CMD
  #define Si2168_DD_EXT_AGC_TER_CMD_CODE 0x010089

    typedef struct { /* Si2168_DD_EXT_AGC_TER_CMD_struct */
     unsigned char   agc_1_mode;
     unsigned char   agc_1_inv;
     unsigned char   agc_2_mode;
     unsigned char   agc_2_inv;
     unsigned char   agc_1_kloop;
     unsigned char   agc_2_kloop;
     unsigned char   agc_1_min;
     unsigned char   agc_2_min;
   } Si2168_DD_EXT_AGC_TER_CMD_struct;


    typedef struct { /* Si2168_DD_EXT_AGC_TER_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   agc_1_level;
      unsigned char   agc_2_level;
   }  Si2168_DD_EXT_AGC_TER_CMD_REPLY_struct;

   /* DD_EXT_AGC_TER command, AGC_1_MODE field definition (address 1,size 3, lsb 0, unsigned) */
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_LSB         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MASK        0x07
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MIN         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MAX         5
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_A       2
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_B       3
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_C       4
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_MP_D       5
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_NOT_USED   1
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MODE_NO_CHANGE  0
   /* DD_EXT_AGC_TER command, AGC_1_INV field definition (address 1,size 1, lsb 3, unsigned) */
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_INV_LSB         3
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_INV_MASK        0x01
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_INV_MIN         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_INV_MAX         1
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_INV_INVERTED      1
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_INV_NOT_INVERTED  0
   /* DD_EXT_AGC_TER command, AGC_2_MODE field definition (address 1,size 3, lsb 4, unsigned) */
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_LSB         4
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MASK        0x07
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MIN         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MAX         5
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_A       2
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_B       3
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_C       4
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_MP_D       5
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_NOT_USED   1
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MODE_NO_CHANGE  0
   /* DD_EXT_AGC_TER command, AGC_2_INV field definition (address 1,size 1, lsb 7, unsigned) */
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_INV_LSB         7
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_INV_MASK        0x01
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_INV_MIN         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_INV_MAX         1
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_INV_INVERTED      1
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_INV_NOT_INVERTED  0
   /* DD_EXT_AGC_TER command, AGC_1_KLOOP field definition (address 2,size 5, lsb 0, unsigned) */
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_LSB         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_MASK        0x1f
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_MIN         6
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_MAX         20
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_AGC_1_KLOOP_MIN  6
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_KLOOP_AGC_1_KLOOP_MAX  20
   /* DD_EXT_AGC_TER command, AGC_2_KLOOP field definition (address 3,size 5, lsb 0, unsigned) */
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_LSB         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_MASK        0x1f
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_MIN         6
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_MAX         20
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_AGC_2_KLOOP_MIN  6
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_KLOOP_AGC_2_KLOOP_MAX  20
   /* DD_EXT_AGC_TER command, AGC_1_MIN field definition (address 4,size 8, lsb 0, unsigned) */
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MIN_LSB         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MIN_MASK        0xff
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MIN_MIN         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MIN_MAX         255
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MIN_AGC_1_MIN_MIN  0
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_1_MIN_AGC_1_MIN_MAX  255
   /* DD_EXT_AGC_TER command, AGC_2_MIN field definition (address 5,size 8, lsb 0, unsigned) */
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MIN_LSB         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MIN_MASK        0xff
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MIN_MIN         0
   #define  Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MIN_MAX         255
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MIN_AGC_2_MIN_MIN  0
    #define Si2168_DD_EXT_AGC_TER_CMD_AGC_2_MIN_AGC_2_MIN_MAX  255
   /* DD_EXT_AGC_TER command, AGC_1_LEVEL field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_EXT_AGC_TER_RESPONSE_AGC_1_LEVEL_LSB         0
   #define  Si2168_DD_EXT_AGC_TER_RESPONSE_AGC_1_LEVEL_MASK        0xff
   /* DD_EXT_AGC_TER command, AGC_2_LEVEL field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_EXT_AGC_TER_RESPONSE_AGC_2_LEVEL_LSB         0
   #define  Si2168_DD_EXT_AGC_TER_RESPONSE_AGC_2_LEVEL_MASK        0xff

#endif /* Si2168_DD_EXT_AGC_TER_CMD */


/* Si2168_DD_FER command definition */
#define   Si2168_DD_FER_CMD 0x86

#ifdef    Si2168_DD_FER_CMD
  #define Si2168_DD_FER_CMD_CODE 0x010086

    typedef struct { /* Si2168_DD_FER_CMD_struct */
     unsigned char   rst;
   } Si2168_DD_FER_CMD_struct;


    typedef struct { /* Si2168_DD_FER_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   exp;
      unsigned char   mant;
   }  Si2168_DD_FER_CMD_REPLY_struct;

   /* DD_FER command, RST field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DD_FER_CMD_RST_LSB         0
   #define  Si2168_DD_FER_CMD_RST_MASK        0x01
   #define  Si2168_DD_FER_CMD_RST_MIN         0
   #define  Si2168_DD_FER_CMD_RST_MAX         1
    #define Si2168_DD_FER_CMD_RST_CLEAR  1
    #define Si2168_DD_FER_CMD_RST_RUN    0
   /* DD_FER command, EXP field definition (address 1, size 4, lsb 0, unsigned)*/
   #define  Si2168_DD_FER_RESPONSE_EXP_LSB         0
   #define  Si2168_DD_FER_RESPONSE_EXP_MASK        0x0f
    #define Si2168_DD_FER_RESPONSE_EXP_EXP_MIN  0
    #define Si2168_DD_FER_RESPONSE_EXP_EXP_MAX  8
   /* DD_FER command, MANT field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_FER_RESPONSE_MANT_LSB         0
   #define  Si2168_DD_FER_RESPONSE_MANT_MASK        0xff
    #define Si2168_DD_FER_RESPONSE_MANT_MANT_MIN  0
    #define Si2168_DD_FER_RESPONSE_MANT_MANT_MAX  99

#endif /* Si2168_DD_FER_CMD */

/* Si2168_DD_GET_REG command definition */
#define   Si2168_DD_GET_REG_CMD 0x8f

#ifdef    Si2168_DD_GET_REG_CMD
  #define Si2168_DD_GET_REG_CMD_CODE 0x01008f

    typedef struct { /* Si2168_DD_GET_REG_CMD_struct */
     unsigned char   reg_code_lsb;
     unsigned char   reg_code_mid;
     unsigned char   reg_code_msb;
   } Si2168_DD_GET_REG_CMD_struct;


    typedef struct { /* Si2168_DD_GET_REG_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   data1;
      unsigned char   data2;
      unsigned char   data3;
      unsigned char   data4;
   }  Si2168_DD_GET_REG_CMD_REPLY_struct;

   /* DD_GET_REG command, REG_CODE_LSB field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_LSB_LSB         0
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_LSB_MASK        0xff
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_LSB_MIN         0
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_LSB_MAX         255
    #define Si2168_DD_GET_REG_CMD_REG_CODE_LSB_REG_CODE_LSB_MIN  0
    #define Si2168_DD_GET_REG_CMD_REG_CODE_LSB_REG_CODE_LSB_MAX  255
   /* DD_GET_REG command, REG_CODE_MID field definition (address 2,size 8, lsb 0, unsigned) */
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_MID_LSB         0
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_MID_MASK        0xff
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_MID_MIN         0
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_MID_MAX         255
    #define Si2168_DD_GET_REG_CMD_REG_CODE_MID_REG_CODE_MID_MIN  0
    #define Si2168_DD_GET_REG_CMD_REG_CODE_MID_REG_CODE_MID_MAX  255
   /* DD_GET_REG command, REG_CODE_MSB field definition (address 3,size 8, lsb 0, unsigned) */
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_MSB_LSB         0
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_MSB_MASK        0xff
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_MSB_MIN         0
   #define  Si2168_DD_GET_REG_CMD_REG_CODE_MSB_MAX         255
    #define Si2168_DD_GET_REG_CMD_REG_CODE_MSB_REG_CODE_MSB_MIN  0
    #define Si2168_DD_GET_REG_CMD_REG_CODE_MSB_REG_CODE_MSB_MAX  255
   /* DD_GET_REG command, DATA1 field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_GET_REG_RESPONSE_DATA1_LSB         0
   #define  Si2168_DD_GET_REG_RESPONSE_DATA1_MASK        0xff
    #define Si2168_DD_GET_REG_RESPONSE_DATA1_DATA1_MIN  0
    #define Si2168_DD_GET_REG_RESPONSE_DATA1_DATA1_MAX  255
   /* DD_GET_REG command, DATA2 field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_GET_REG_RESPONSE_DATA2_LSB         0
   #define  Si2168_DD_GET_REG_RESPONSE_DATA2_MASK        0xff
    #define Si2168_DD_GET_REG_RESPONSE_DATA2_DATA2_MIN  0
    #define Si2168_DD_GET_REG_RESPONSE_DATA2_DATA2_MAX  255
   /* DD_GET_REG command, DATA3 field definition (address 3, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_GET_REG_RESPONSE_DATA3_LSB         0
   #define  Si2168_DD_GET_REG_RESPONSE_DATA3_MASK        0xff
    #define Si2168_DD_GET_REG_RESPONSE_DATA3_DATA3_MIN  0
    #define Si2168_DD_GET_REG_RESPONSE_DATA3_DATA3_MAX  255
   /* DD_GET_REG command, DATA4 field definition (address 4, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_GET_REG_RESPONSE_DATA4_LSB         0
   #define  Si2168_DD_GET_REG_RESPONSE_DATA4_MASK        0xff
    #define Si2168_DD_GET_REG_RESPONSE_DATA4_DATA4_MIN  0
    #define Si2168_DD_GET_REG_RESPONSE_DATA4_DATA4_MAX  255

#endif /* Si2168_DD_GET_REG_CMD */

/* Si2168_DD_MP_DEFAULTS command definition */
#define   Si2168_DD_MP_DEFAULTS_CMD 0x88

#ifdef    Si2168_DD_MP_DEFAULTS_CMD
  #define Si2168_DD_MP_DEFAULTS_CMD_CODE 0x010088

    typedef struct { /* Si2168_DD_MP_DEFAULTS_CMD_struct */
     unsigned char   mp_a_mode;
     unsigned char   mp_b_mode;
     unsigned char   mp_c_mode;
     unsigned char   mp_d_mode;
   } Si2168_DD_MP_DEFAULTS_CMD_struct;


    typedef struct { /* Si2168_DD_MP_DEFAULTS_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   mp_a_mode;
      unsigned char   mp_b_mode;
      unsigned char   mp_c_mode;
      unsigned char   mp_d_mode;
   }  Si2168_DD_MP_DEFAULTS_CMD_REPLY_struct;

   /* DD_MP_DEFAULTS command, MP_A_MODE field definition (address 1,size 7, lsb 0, unsigned) */
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_LSB         0
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_MASK        0x7f
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_MIN         0
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_MAX         3
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_DISABLE    1
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_DRIVE_0    2
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_DRIVE_1    3
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_A_MODE_NO_CHANGE  0
   /* DD_MP_DEFAULTS command, MP_B_MODE field definition (address 2,size 7, lsb 0, unsigned) */
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_LSB         0
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_MASK        0x7f
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_MIN         0
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_MAX         3
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_DISABLE    1
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_DRIVE_0    2
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_DRIVE_1    3
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_B_MODE_NO_CHANGE  0
   /* DD_MP_DEFAULTS command, MP_C_MODE field definition (address 3,size 7, lsb 0, unsigned) */
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_LSB         0
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_MASK        0x7f
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_MIN         0
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_MAX         3
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_DISABLE    1
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_DRIVE_0    2
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_DRIVE_1    3
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_C_MODE_NO_CHANGE  0
   /* DD_MP_DEFAULTS command, MP_D_MODE field definition (address 4,size 7, lsb 0, unsigned) */
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_LSB         0
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_MASK        0x7f
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_MIN         0
   #define  Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_MAX         3
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_DISABLE    1
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_DRIVE_0    2
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_DRIVE_1    3
    #define Si2168_DD_MP_DEFAULTS_CMD_MP_D_MODE_NO_CHANGE  0
   /* DD_MP_DEFAULTS command, MP_A_MODE field definition (address 1, size 7, lsb 0, unsigned)*/
   #define  Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_LSB         0
   #define  Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_MASK        0x7f
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_AGC_1           3
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_AGC_1_INVERTED  4
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_AGC_2           5
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_AGC_2_INVERTED  6
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_DISABLE         0
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_DRIVE_0         1
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_DRIVE_1         2
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_FEF             7
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_A_MODE_FEF_INVERTED    8
   /* DD_MP_DEFAULTS command, MP_B_MODE field definition (address 2, size 7, lsb 0, unsigned)*/
   #define  Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_LSB         0
   #define  Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_MASK        0x7f
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_AGC_1           3
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_AGC_1_INVERTED  4
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_AGC_2           5
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_AGC_2_INVERTED  6
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_DISABLE         0
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_DRIVE_0         1
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_DRIVE_1         2
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_FEF             7
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_B_MODE_FEF_INVERTED    8
   /* DD_MP_DEFAULTS command, MP_C_MODE field definition (address 3, size 7, lsb 0, unsigned)*/
   #define  Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_LSB         0
   #define  Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_MASK        0x7f
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_AGC_1           3
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_AGC_1_INVERTED  4
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_AGC_2           5
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_AGC_2_INVERTED  6
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_DISABLE         0
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_DRIVE_0         1
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_DRIVE_1         2
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_FEF             7
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_C_MODE_FEF_INVERTED    8
   /* DD_MP_DEFAULTS command, MP_D_MODE field definition (address 4, size 7, lsb 0, unsigned)*/
   #define  Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_LSB         0
   #define  Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_MASK        0x7f
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_AGC_1           3
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_AGC_1_INVERTED  4
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_AGC_2           5
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_AGC_2_INVERTED  6
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_DISABLE         0
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_DRIVE_0         1
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_DRIVE_1         2
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_FEF             7
    #define Si2168_DD_MP_DEFAULTS_RESPONSE_MP_D_MODE_FEF_INVERTED    8

#endif /* Si2168_DD_MP_DEFAULTS_CMD */

/* Si2168_DD_PER command definition */
#define   Si2168_DD_PER_CMD 0x83

#ifdef    Si2168_DD_PER_CMD
  #define Si2168_DD_PER_CMD_CODE 0x010083

    typedef struct { /* Si2168_DD_PER_CMD_struct */
     unsigned char   rst;
   } Si2168_DD_PER_CMD_struct;


    typedef struct { /* Si2168_DD_PER_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   exp;
      unsigned char   mant;
   }  Si2168_DD_PER_CMD_REPLY_struct;

   /* DD_PER command, RST field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DD_PER_CMD_RST_LSB         0
   #define  Si2168_DD_PER_CMD_RST_MASK        0x01
   #define  Si2168_DD_PER_CMD_RST_MIN         0
   #define  Si2168_DD_PER_CMD_RST_MAX         1
    #define Si2168_DD_PER_CMD_RST_CLEAR  1
    #define Si2168_DD_PER_CMD_RST_RUN    0
   /* DD_PER command, EXP field definition (address 1, size 4, lsb 0, unsigned)*/
   #define  Si2168_DD_PER_RESPONSE_EXP_LSB         0
   #define  Si2168_DD_PER_RESPONSE_EXP_MASK        0x0f
    #define Si2168_DD_PER_RESPONSE_EXP_EXP_MIN  0
    #define Si2168_DD_PER_RESPONSE_EXP_EXP_MAX  8
   /* DD_PER command, MANT field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_PER_RESPONSE_MANT_LSB         0
   #define  Si2168_DD_PER_RESPONSE_MANT_MASK        0xff
    #define Si2168_DD_PER_RESPONSE_MANT_MANT_MIN  0
    #define Si2168_DD_PER_RESPONSE_MANT_MANT_MAX  99

#endif /* Si2168_DD_PER_CMD */

/* Si2168_DD_RESTART command definition */
#define   Si2168_DD_RESTART_CMD 0x85

#ifdef    Si2168_DD_RESTART_CMD
  #define Si2168_DD_RESTART_CMD_CODE 0x010085

    typedef struct { /* Si2168_DD_RESTART_CMD_struct */
         unsigned char   nothing;   } Si2168_DD_RESTART_CMD_struct;


    typedef struct { /* Si2168_DD_RESTART_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_DD_RESTART_CMD_REPLY_struct;

#endif /* Si2168_DD_RESTART_CMD */

/* Si2168_DD_SET_REG command definition */
#define   Si2168_DD_SET_REG_CMD 0x8e

#ifdef    Si2168_DD_SET_REG_CMD
  #define Si2168_DD_SET_REG_CMD_CODE 0x01008e

    typedef struct { /* Si2168_DD_SET_REG_CMD_struct */
     unsigned char   reg_code_lsb;
     unsigned char   reg_code_mid;
     unsigned char   reg_code_msb;
     unsigned long   value;
   } Si2168_DD_SET_REG_CMD_struct;


    typedef struct { /* Si2168_DD_SET_REG_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_DD_SET_REG_CMD_REPLY_struct;

   /* DD_SET_REG command, REG_CODE_LSB field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_LSB_LSB         0
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_LSB_MASK        0xff
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_LSB_MIN         0
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_LSB_MAX         255
    #define Si2168_DD_SET_REG_CMD_REG_CODE_LSB_REG_CODE_LSB_MIN  0
    #define Si2168_DD_SET_REG_CMD_REG_CODE_LSB_REG_CODE_LSB_MAX  255
   /* DD_SET_REG command, REG_CODE_MID field definition (address 2,size 8, lsb 0, unsigned) */
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_MID_LSB         0
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_MID_MASK        0xff
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_MID_MIN         0
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_MID_MAX         255
    #define Si2168_DD_SET_REG_CMD_REG_CODE_MID_REG_CODE_MID_MIN  0
    #define Si2168_DD_SET_REG_CMD_REG_CODE_MID_REG_CODE_MID_MAX  255
   /* DD_SET_REG command, REG_CODE_MSB field definition (address 3,size 8, lsb 0, unsigned) */
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_MSB_LSB         0
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_MSB_MASK        0xff
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_MSB_MIN         0
   #define  Si2168_DD_SET_REG_CMD_REG_CODE_MSB_MAX         255
    #define Si2168_DD_SET_REG_CMD_REG_CODE_MSB_REG_CODE_MSB_MIN  0
    #define Si2168_DD_SET_REG_CMD_REG_CODE_MSB_REG_CODE_MSB_MAX  255
   /* DD_SET_REG command, VALUE field definition (address 4,size 32, lsb 0, unsigned) */
   #define  Si2168_DD_SET_REG_CMD_VALUE_LSB         0
   #define  Si2168_DD_SET_REG_CMD_VALUE_MASK        0xffffffff
   #define  Si2168_DD_SET_REG_CMD_VALUE_MIN         0
   #define  Si2168_DD_SET_REG_CMD_VALUE_MAX         4294967295
    #define Si2168_DD_SET_REG_CMD_VALUE_VALUE_MIN  0
    #define Si2168_DD_SET_REG_CMD_VALUE_VALUE_MAX  4294967295
#endif /* Si2168_DD_SET_REG_CMD */

/* Si2168_DD_SSI_SQI command definition */
#define   Si2168_DD_SSI_SQI_CMD 0x8b

#ifdef    Si2168_DD_SSI_SQI_CMD
  #define Si2168_DD_SSI_SQI_CMD_CODE 0x01008b

    typedef struct { /* Si2168_DD_SSI_SQI_CMD_struct */
              char   tuner_rssi;
   } Si2168_DD_SSI_SQI_CMD_struct;


    typedef struct { /* Si2168_DD_SSI_SQI_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
               char   ssi;
               char   sqi;
   }  Si2168_DD_SSI_SQI_CMD_REPLY_struct;

   /* DD_SSI_SQI command, TUNER_RSSI field definition (address 1,size 8, lsb 0, signed) */
   #define  Si2168_DD_SSI_SQI_CMD_TUNER_RSSI_LSB         0
   #define  Si2168_DD_SSI_SQI_CMD_TUNER_RSSI_MASK        0xff
   #define  Si2168_DD_SSI_SQI_CMD_TUNER_RSSI_SHIFT       24
   #define  Si2168_DD_SSI_SQI_CMD_TUNER_RSSI_MIN         -128.0
   #define  Si2168_DD_SSI_SQI_CMD_TUNER_RSSI_MAX         127.0
   /* DD_SSI_SQI command, SSI field definition (address 1, size 8, lsb 0, signed)*/
   #define  Si2168_DD_SSI_SQI_RESPONSE_SSI_LSB         0
   #define  Si2168_DD_SSI_SQI_RESPONSE_SSI_MASK        0xff
   #define  Si2168_DD_SSI_SQI_RESPONSE_SSI_SHIFT       24
    #define Si2168_DD_SSI_SQI_RESPONSE_SSI_SSI_MIN  -1
    #define Si2168_DD_SSI_SQI_RESPONSE_SSI_SSI_MAX  100
   /* DD_SSI_SQI command, SQI field definition (address 2, size 8, lsb 0, signed)*/
   #define  Si2168_DD_SSI_SQI_RESPONSE_SQI_LSB         0
   #define  Si2168_DD_SSI_SQI_RESPONSE_SQI_MASK        0xff
   #define  Si2168_DD_SSI_SQI_RESPONSE_SQI_SHIFT       24
    #define Si2168_DD_SSI_SQI_RESPONSE_SQI_SQI_MIN  -1
    #define Si2168_DD_SSI_SQI_RESPONSE_SQI_SQI_MAX  100

#endif /* Si2168_DD_SSI_SQI_CMD */

/* Si2168_DD_STATUS command definition */
#define   Si2168_DD_STATUS_CMD 0x87

#ifdef    Si2168_DD_STATUS_CMD
  #define Si2168_DD_STATUS_CMD_CODE 0x010087

    typedef struct { /* Si2168_DD_STATUS_CMD_struct */
     unsigned char   intack;
   } Si2168_DD_STATUS_CMD_struct;


    typedef struct { /* Si2168_DD_STATUS_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   pclint;
      unsigned char   dlint;
      unsigned char   berint;
      unsigned char   uncorint;
      unsigned char   rsqint_bit5;
      unsigned char   rsqint_bit6;
      unsigned char   rsqint_bit7;
      unsigned char   pcl;
      unsigned char   dl;
      unsigned char   ber;
      unsigned char   uncor;
      unsigned char   rsqstat_bit5;
      unsigned char   rsqstat_bit6;
      unsigned char   rsqstat_bit7;
      unsigned char   modulation;
      unsigned int    ts_bit_rate;
      unsigned int    ts_clk_freq;
   }  Si2168_DD_STATUS_CMD_REPLY_struct;

   /* DD_STATUS command, INTACK field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DD_STATUS_CMD_INTACK_LSB         0
   #define  Si2168_DD_STATUS_CMD_INTACK_MASK        0x01
   #define  Si2168_DD_STATUS_CMD_INTACK_MIN         0
   #define  Si2168_DD_STATUS_CMD_INTACK_MAX         1
    #define Si2168_DD_STATUS_CMD_INTACK_CLEAR  1
    #define Si2168_DD_STATUS_CMD_INTACK_OK     0
   /* DD_STATUS command, PCLINT field definition (address 1, size 1, lsb 1, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_PCLINT_LSB         1
   #define  Si2168_DD_STATUS_RESPONSE_PCLINT_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_PCLINT_CHANGED    1
    #define Si2168_DD_STATUS_RESPONSE_PCLINT_NO_CHANGE  0
   /* DD_STATUS command, DLINT field definition (address 1, size 1, lsb 2, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_DLINT_LSB         2
   #define  Si2168_DD_STATUS_RESPONSE_DLINT_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_DLINT_CHANGED    1
    #define Si2168_DD_STATUS_RESPONSE_DLINT_NO_CHANGE  0
   /* DD_STATUS command, BERINT field definition (address 1, size 1, lsb 3, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_BERINT_LSB         3
   #define  Si2168_DD_STATUS_RESPONSE_BERINT_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_BERINT_CHANGED    1
    #define Si2168_DD_STATUS_RESPONSE_BERINT_NO_CHANGE  0
   /* DD_STATUS command, UNCORINT field definition (address 1, size 1, lsb 4, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_UNCORINT_LSB         4
   #define  Si2168_DD_STATUS_RESPONSE_UNCORINT_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_UNCORINT_CHANGED    1
    #define Si2168_DD_STATUS_RESPONSE_UNCORINT_NO_CHANGE  0
   /* DD_STATUS command, RSQINT_BIT5 field definition (address 1, size 1, lsb 5, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_RSQINT_BIT5_LSB         5
   #define  Si2168_DD_STATUS_RESPONSE_RSQINT_BIT5_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_RSQINT_BIT5_CHANGED    1
    #define Si2168_DD_STATUS_RESPONSE_RSQINT_BIT5_NO_CHANGE  0
   /* DD_STATUS command, RSQINT_BIT6 field definition (address 1, size 1, lsb 6, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_RSQINT_BIT6_LSB         6
   #define  Si2168_DD_STATUS_RESPONSE_RSQINT_BIT6_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_RSQINT_BIT6_CHANGED    1
    #define Si2168_DD_STATUS_RESPONSE_RSQINT_BIT6_NO_CHANGE  0
   /* DD_STATUS command, RSQINT_BIT7 field definition (address 1, size 1, lsb 7, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_RSQINT_BIT7_LSB         7
   #define  Si2168_DD_STATUS_RESPONSE_RSQINT_BIT7_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_RSQINT_BIT7_CHANGED    1
    #define Si2168_DD_STATUS_RESPONSE_RSQINT_BIT7_NO_CHANGE  0
   /* DD_STATUS command, PCL field definition (address 2, size 1, lsb 1, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_PCL_LSB         1
   #define  Si2168_DD_STATUS_RESPONSE_PCL_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_PCL_LOCKED   1
    #define Si2168_DD_STATUS_RESPONSE_PCL_NO_LOCK  0
   /* DD_STATUS command, DL field definition (address 2, size 1, lsb 2, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_DL_LSB         2
   #define  Si2168_DD_STATUS_RESPONSE_DL_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_DL_LOCKED   1
    #define Si2168_DD_STATUS_RESPONSE_DL_NO_LOCK  0
   /* DD_STATUS command, BER field definition (address 2, size 1, lsb 3, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_BER_LSB         3
   #define  Si2168_DD_STATUS_RESPONSE_BER_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_BER_BER_ABOVE  1
    #define Si2168_DD_STATUS_RESPONSE_BER_BER_BELOW  0
   /* DD_STATUS command, UNCOR field definition (address 2, size 1, lsb 4, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_UNCOR_LSB         4
   #define  Si2168_DD_STATUS_RESPONSE_UNCOR_MASK        0x01
    #define Si2168_DD_STATUS_RESPONSE_UNCOR_NO_UNCOR_FOUND  0
    #define Si2168_DD_STATUS_RESPONSE_UNCOR_UNCOR_FOUND     1
   /* DD_STATUS command, RSQSTAT_BIT5 field definition (address 2, size 1, lsb 5, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_RSQSTAT_BIT5_LSB         5
   #define  Si2168_DD_STATUS_RESPONSE_RSQSTAT_BIT5_MASK        0x01
    #define  Si2168_DD_STATUS_RESPONSE_RSQSTAT_BIT5_NO_CHANGE 0
    #define  Si2168_DD_STATUS_RESPONSE_RSQSTAT_BIT5_CHANGE    1
   /* DD_STATUS command, RSQSTAT_BIT6 field definition (address 2, size 1, lsb 6, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_RSQSTAT_BIT6_LSB         6
   #define  Si2168_DD_STATUS_RESPONSE_RSQSTAT_BIT6_MASK        0x01
   /* DD_STATUS command, RSQSTAT_BIT7 field definition (address 2, size 1, lsb 7, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_RSQSTAT_BIT7_LSB         7
   #define  Si2168_DD_STATUS_RESPONSE_RSQSTAT_BIT7_MASK        0x01
   /* DD_STATUS command, MODULATION field definition (address 3, size 4, lsb 0, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_MODULATION_LSB         0
   #define  Si2168_DD_STATUS_RESPONSE_MODULATION_MASK        0x0f
    #define Si2168_DD_STATUS_RESPONSE_MODULATION_DSS     10
    #define Si2168_DD_STATUS_RESPONSE_MODULATION_DVBC    3
    #define Si2168_DD_STATUS_RESPONSE_MODULATION_DVBS    8
    #define Si2168_DD_STATUS_RESPONSE_MODULATION_DVBS2   9
    #define Si2168_DD_STATUS_RESPONSE_MODULATION_DVBT    2
    #define Si2168_DD_STATUS_RESPONSE_MODULATION_DVBT2   7
   /* DD_STATUS command, TS_BIT_RATE field definition (address 4, size 16, lsb 0, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_TS_BIT_RATE_LSB         0
   #define  Si2168_DD_STATUS_RESPONSE_TS_BIT_RATE_MASK        0xffff
   /* DD_STATUS command, TS_CLK_FREQ field definition (address 6, size 16, lsb 0, unsigned)*/
   #define  Si2168_DD_STATUS_RESPONSE_TS_CLK_FREQ_LSB         0
   #define  Si2168_DD_STATUS_RESPONSE_TS_CLK_FREQ_MASK        0xffff

#endif /* Si2168_DD_STATUS_CMD */

/* Si2168_DD_UNCOR command definition */
#define   Si2168_DD_UNCOR_CMD 0x84

#ifdef    Si2168_DD_UNCOR_CMD
  #define Si2168_DD_UNCOR_CMD_CODE 0x010084

    typedef struct { /* Si2168_DD_UNCOR_CMD_struct */
     unsigned char   rst;
   } Si2168_DD_UNCOR_CMD_struct;


    typedef struct { /* Si2168_DD_UNCOR_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   uncor_lsb;
      unsigned char   uncor_msb;
   }  Si2168_DD_UNCOR_CMD_REPLY_struct;

   /* DD_UNCOR command, RST field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DD_UNCOR_CMD_RST_LSB         0
   #define  Si2168_DD_UNCOR_CMD_RST_MASK        0x01
   #define  Si2168_DD_UNCOR_CMD_RST_MIN         0
   #define  Si2168_DD_UNCOR_CMD_RST_MAX         1
    #define Si2168_DD_UNCOR_CMD_RST_CLEAR  1
    #define Si2168_DD_UNCOR_CMD_RST_RUN    0
   /* DD_UNCOR command, UNCOR_LSB field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_UNCOR_RESPONSE_UNCOR_LSB_LSB         0
   #define  Si2168_DD_UNCOR_RESPONSE_UNCOR_LSB_MASK        0xff
   /* DD_UNCOR command, UNCOR_MSB field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_DD_UNCOR_RESPONSE_UNCOR_MSB_LSB         0
   #define  Si2168_DD_UNCOR_RESPONSE_UNCOR_MSB_MASK        0xff

#endif /* Si2168_DD_UNCOR_CMD */

/* Si2168_DOWNLOAD_DATASET_CONTINUE command definition */
#define   Si2168_DOWNLOAD_DATASET_CONTINUE_CMD 0xb9

#ifdef    Si2168_DOWNLOAD_DATASET_CONTINUE_CMD
  #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_CODE 0x0100b9

    typedef struct { /* Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_struct */
     unsigned char   data0;
     unsigned char   data1;
     unsigned char   data2;
     unsigned char   data3;
     unsigned char   data4;
     unsigned char   data5;
     unsigned char   data6;
   } Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_struct;


    typedef struct { /* Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_REPLY_struct;

   /* DOWNLOAD_DATASET_CONTINUE command, DATA0 field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA0_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA0_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA0_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA0_MAX         255
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA0_DATA0_MIN  0
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA0_DATA0_MAX  255
   /* DOWNLOAD_DATASET_CONTINUE command, DATA1 field definition (address 2,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA1_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA1_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA1_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA1_MAX         255
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA1_DATA1_MIN  0
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA1_DATA1_MAX  255
   /* DOWNLOAD_DATASET_CONTINUE command, DATA2 field definition (address 3,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA2_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA2_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA2_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA2_MAX         255
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA2_DATA2_MIN  0
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA2_DATA2_MAX  255
   /* DOWNLOAD_DATASET_CONTINUE command, DATA3 field definition (address 4,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA3_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA3_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA3_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA3_MAX         255
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA3_DATA3_MIN  0
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA3_DATA3_MAX  255
   /* DOWNLOAD_DATASET_CONTINUE command, DATA4 field definition (address 5,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA4_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA4_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA4_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA4_MAX         255
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA4_DATA4_MIN  0
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA4_DATA4_MAX  255
   /* DOWNLOAD_DATASET_CONTINUE command, DATA5 field definition (address 6,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA5_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA5_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA5_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA5_MAX         255
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA5_DATA5_MIN  0
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA5_DATA5_MAX  255
   /* DOWNLOAD_DATASET_CONTINUE command, DATA6 field definition (address 7,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA6_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA6_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA6_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA6_MAX         255
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA6_DATA6_MIN  0
    #define Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_DATA6_DATA6_MAX  255
#endif /* Si2168_DOWNLOAD_DATASET_CONTINUE_CMD */

/* Si2168_DOWNLOAD_DATASET_START command definition */
#define   Si2168_DOWNLOAD_DATASET_START_CMD 0xb8

#ifdef    Si2168_DOWNLOAD_DATASET_START_CMD
  #define Si2168_DOWNLOAD_DATASET_START_CMD_CODE 0x0100b8

    typedef struct { /* Si2168_DOWNLOAD_DATASET_START_CMD_struct */
     unsigned char   dataset_id;
     unsigned char   dataset_checksum;
     unsigned char   data0;
     unsigned char   data1;
     unsigned char   data2;
     unsigned char   data3;
     unsigned char   data4;
   } Si2168_DOWNLOAD_DATASET_START_CMD_struct;


    typedef struct { /* Si2168_DOWNLOAD_DATASET_START_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_DOWNLOAD_DATASET_START_CMD_REPLY_struct;

   /* DOWNLOAD_DATASET_START command, DATASET_ID field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_ID_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_ID_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_ID_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_ID_MAX         0
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_ID_RFU  0
   /* DOWNLOAD_DATASET_START command, DATASET_CHECKSUM field definition (address 2,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_CHECKSUM_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_CHECKSUM_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_CHECKSUM_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_CHECKSUM_MAX         255
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_CHECKSUM_DATASET_CHECKSUM_MIN  0
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATASET_CHECKSUM_DATASET_CHECKSUM_MAX  255
   /* DOWNLOAD_DATASET_START command, DATA0 field definition (address 3,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA0_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA0_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA0_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA0_MAX         255
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA0_DATA0_MIN  0
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA0_DATA0_MAX  255
   /* DOWNLOAD_DATASET_START command, DATA1 field definition (address 4,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA1_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA1_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA1_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA1_MAX         255
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA1_DATA1_MIN  0
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA1_DATA1_MAX  255
   /* DOWNLOAD_DATASET_START command, DATA2 field definition (address 5,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA2_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA2_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA2_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA2_MAX         255
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA2_DATA2_MIN  0
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA2_DATA2_MAX  255
   /* DOWNLOAD_DATASET_START command, DATA3 field definition (address 6,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA3_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA3_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA3_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA3_MAX         255
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA3_DATA3_MIN  0
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA3_DATA3_MAX  255
   /* DOWNLOAD_DATASET_START command, DATA4 field definition (address 7,size 8, lsb 0, unsigned) */
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA4_LSB         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA4_MASK        0xff
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA4_MIN         0
   #define  Si2168_DOWNLOAD_DATASET_START_CMD_DATA4_MAX         255
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA4_DATA4_MIN  0
    #define Si2168_DOWNLOAD_DATASET_START_CMD_DATA4_DATA4_MAX  255
#endif /* Si2168_DOWNLOAD_DATASET_START_CMD */

/* Si2168_DVBC_STATUS command definition */
#define   Si2168_DVBC_STATUS_CMD 0x90

#ifdef    Si2168_DVBC_STATUS_CMD
  #define Si2168_DVBC_STATUS_CMD_CODE 0x010090

    typedef struct { /* Si2168_DVBC_STATUS_CMD_struct */
     unsigned char   intack;
   } Si2168_DVBC_STATUS_CMD_struct;


    typedef struct { /* Si2168_DVBC_STATUS_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   pclint;
      unsigned char   dlint;
      unsigned char   berint;
      unsigned char   uncorint;
      unsigned char   pcl;
      unsigned char   dl;
      unsigned char   ber;
      unsigned char   uncor;
      unsigned char   cnr;
               int    afc_freq;
               int    timing_offset;
      unsigned char   constellation;
      unsigned char   sp_inv;
   }  Si2168_DVBC_STATUS_CMD_REPLY_struct;

   /* DVBC_STATUS command, INTACK field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DVBC_STATUS_CMD_INTACK_LSB         0
   #define  Si2168_DVBC_STATUS_CMD_INTACK_MASK        0x01
   #define  Si2168_DVBC_STATUS_CMD_INTACK_MIN         0
   #define  Si2168_DVBC_STATUS_CMD_INTACK_MAX         1
    #define Si2168_DVBC_STATUS_CMD_INTACK_CLEAR  1
    #define Si2168_DVBC_STATUS_CMD_INTACK_OK     0
   /* DVBC_STATUS command, PCLINT field definition (address 1, size 1, lsb 1, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_PCLINT_LSB         1
   #define  Si2168_DVBC_STATUS_RESPONSE_PCLINT_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_PCLINT_CHANGED    1
    #define Si2168_DVBC_STATUS_RESPONSE_PCLINT_NO_CHANGE  0
   /* DVBC_STATUS command, DLINT field definition (address 1, size 1, lsb 2, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_DLINT_LSB         2
   #define  Si2168_DVBC_STATUS_RESPONSE_DLINT_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_DLINT_CHANGED    1
    #define Si2168_DVBC_STATUS_RESPONSE_DLINT_NO_CHANGE  0
   /* DVBC_STATUS command, BERINT field definition (address 1, size 1, lsb 3, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_BERINT_LSB         3
   #define  Si2168_DVBC_STATUS_RESPONSE_BERINT_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_BERINT_CHANGED    1
    #define Si2168_DVBC_STATUS_RESPONSE_BERINT_NO_CHANGE  0
   /* DVBC_STATUS command, UNCORINT field definition (address 1, size 1, lsb 4, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_UNCORINT_LSB         4
   #define  Si2168_DVBC_STATUS_RESPONSE_UNCORINT_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_UNCORINT_CHANGED    1
    #define Si2168_DVBC_STATUS_RESPONSE_UNCORINT_NO_CHANGE  0
   /* DVBC_STATUS command, PCL field definition (address 2, size 1, lsb 1, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_PCL_LSB         1
   #define  Si2168_DVBC_STATUS_RESPONSE_PCL_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_PCL_LOCKED   1
    #define Si2168_DVBC_STATUS_RESPONSE_PCL_NO_LOCK  0
   /* DVBC_STATUS command, DL field definition (address 2, size 1, lsb 2, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_DL_LSB         2
   #define  Si2168_DVBC_STATUS_RESPONSE_DL_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_DL_LOCKED   1
    #define Si2168_DVBC_STATUS_RESPONSE_DL_NO_LOCK  0
   /* DVBC_STATUS command, BER field definition (address 2, size 1, lsb 3, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_BER_LSB         3
   #define  Si2168_DVBC_STATUS_RESPONSE_BER_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_BER_BER_ABOVE  1
    #define Si2168_DVBC_STATUS_RESPONSE_BER_BER_BELOW  0
   /* DVBC_STATUS command, UNCOR field definition (address 2, size 1, lsb 4, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_UNCOR_LSB         4
   #define  Si2168_DVBC_STATUS_RESPONSE_UNCOR_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_UNCOR_NO_UNCOR_FOUND  0
    #define Si2168_DVBC_STATUS_RESPONSE_UNCOR_UNCOR_FOUND     1
   /* DVBC_STATUS command, CNR field definition (address 3, size 8, lsb 0, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_CNR_LSB         0
   #define  Si2168_DVBC_STATUS_RESPONSE_CNR_MASK        0xff
   /* DVBC_STATUS command, AFC_FREQ field definition (address 4, size 16, lsb 0, signed)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_AFC_FREQ_LSB         0
   #define  Si2168_DVBC_STATUS_RESPONSE_AFC_FREQ_MASK        0xffff
   #define  Si2168_DVBC_STATUS_RESPONSE_AFC_FREQ_SHIFT       16
   /* DVBC_STATUS command, TIMING_OFFSET field definition (address 6, size 16, lsb 0, signed)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_TIMING_OFFSET_LSB         0
   #define  Si2168_DVBC_STATUS_RESPONSE_TIMING_OFFSET_MASK        0xffff
   #define  Si2168_DVBC_STATUS_RESPONSE_TIMING_OFFSET_SHIFT       16
   /* DVBC_STATUS command, CONSTELLATION field definition (address 8, size 6, lsb 0, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_CONSTELLATION_LSB         0
   #define  Si2168_DVBC_STATUS_RESPONSE_CONSTELLATION_MASK        0x3f
    #define Si2168_DVBC_STATUS_RESPONSE_CONSTELLATION_QAM128  10
    #define Si2168_DVBC_STATUS_RESPONSE_CONSTELLATION_QAM16   7
    #define Si2168_DVBC_STATUS_RESPONSE_CONSTELLATION_QAM256  11
    #define Si2168_DVBC_STATUS_RESPONSE_CONSTELLATION_QAM32   8
    #define Si2168_DVBC_STATUS_RESPONSE_CONSTELLATION_QAM64   9
   /* DVBC_STATUS command, SP_INV field definition (address 8, size 1, lsb 6, unsigned)*/
   #define  Si2168_DVBC_STATUS_RESPONSE_SP_INV_LSB         6
   #define  Si2168_DVBC_STATUS_RESPONSE_SP_INV_MASK        0x01
    #define Si2168_DVBC_STATUS_RESPONSE_SP_INV_INVERTED  1
    #define Si2168_DVBC_STATUS_RESPONSE_SP_INV_NORMAL    0

#endif /* Si2168_DVBC_STATUS_CMD */



/* Si2168_DVBT2_FEF command definition */
#define   Si2168_DVBT2_FEF_CMD 0x51

#ifdef    Si2168_DVBT2_FEF_CMD
  #define Si2168_DVBT2_FEF_CMD_CODE 0x010051

    typedef struct { /* Si2168_DVBT2_FEF_CMD_struct */
     unsigned char   fef_tuner_flag;
     unsigned char   fef_tuner_flag_inv;
   } Si2168_DVBT2_FEF_CMD_struct;


    typedef struct { /* Si2168_DVBT2_FEF_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   fef_type;
      unsigned long   fef_length;
      unsigned long   fef_repetition;
   }  Si2168_DVBT2_FEF_CMD_REPLY_struct;

   /* DVBT2_FEF command, FEF_TUNER_FLAG field definition (address 1,size 3, lsb 0, unsigned) */
   #define  Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_LSB         0
   #define  Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MASK        0x07
   #define  Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MIN         0
   #define  Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MAX         5
    #define Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_A       2
    #define Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_B       3
    #define Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_C       4
    #define Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_MP_D       5
    #define Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_NOT_USED   1
    #define Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_NO_CHANGE  0
   /* DVBT2_FEF command, FEF_TUNER_FLAG_INV field definition (address 1,size 1, lsb 3, unsigned) */
   #define  Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_LSB         3
   #define  Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_MASK        0x01
   #define  Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_MIN         0
   #define  Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_MAX         1
    #define Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_FEF_HIGH  0
    #define Si2168_DVBT2_FEF_CMD_FEF_TUNER_FLAG_INV_FEF_LOW   1
   /* DVBT2_FEF command, FEF_TYPE field definition (address 1, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_FEF_RESPONSE_FEF_TYPE_LSB         0
   #define  Si2168_DVBT2_FEF_RESPONSE_FEF_TYPE_MASK        0x0f
   /* DVBT2_FEF command, FEF_LENGTH field definition (address 4, size 32, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_FEF_RESPONSE_FEF_LENGTH_LSB         0
   #define  Si2168_DVBT2_FEF_RESPONSE_FEF_LENGTH_MASK        0xffffffff
   /* DVBT2_FEF command, FEF_REPETITION field definition (address 8, size 32, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_FEF_RESPONSE_FEF_REPETITION_LSB         0
   #define  Si2168_DVBT2_FEF_RESPONSE_FEF_REPETITION_MASK        0xffffffff

#endif /* Si2168_DVBT2_FEF_CMD */

/* Si2168_DVBT2_PLP_INFO command definition */
#define   Si2168_DVBT2_PLP_INFO_CMD 0x53

#ifdef    Si2168_DVBT2_PLP_INFO_CMD
  #define Si2168_DVBT2_PLP_INFO_CMD_CODE 0x010053

    typedef struct { /* Si2168_DVBT2_PLP_INFO_CMD_struct */
     unsigned char   plp_index;
   } Si2168_DVBT2_PLP_INFO_CMD_struct;


    typedef struct { /* Si2168_DVBT2_PLP_INFO_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   plp_id;
      unsigned char   reserved_1_1;
      unsigned char   in_band_b_flag;
      unsigned char   in_band_a_flag;
      unsigned char   static_flag;
      unsigned char   plp_mode;
      unsigned char   reserved_1_2;
      unsigned char   static_padding_flag;
      unsigned char   plp_payload_type;
      unsigned char   plp_type;
      unsigned char   first_frame_idx_msb;
      unsigned char   first_rf_idx;
      unsigned char   ff_flag;
      unsigned char   plp_group_id_msb;
      unsigned char   first_frame_idx_lsb;
      unsigned char   plp_mod_msb;
      unsigned char   plp_cod;
      unsigned char   plp_group_id_lsb;
      unsigned char   plp_num_blocks_max_msb;
      unsigned char   plp_fec_type;
      unsigned char   plp_rot;
      unsigned char   plp_mod_lsb;
      unsigned char   frame_interval_msb;
      unsigned char   plp_num_blocks_max_lsb;
      unsigned char   time_il_length_msb;
      unsigned char   frame_interval_lsb;
      unsigned char   time_il_type;
      unsigned char   time_il_length_lsb;
   }  Si2168_DVBT2_PLP_INFO_CMD_REPLY_struct;

   /* DVBT2_PLP_INFO command, PLP_INDEX field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_DVBT2_PLP_INFO_CMD_PLP_INDEX_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_CMD_PLP_INDEX_MASK        0xff
   #define  Si2168_DVBT2_PLP_INFO_CMD_PLP_INDEX_MIN         0
   #define  Si2168_DVBT2_PLP_INFO_CMD_PLP_INDEX_MAX         255.0
   /* DVBT2_PLP_INFO command, PLP_ID field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_ID_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_ID_MASK        0xff
   /* DVBT2_PLP_INFO command, RESERVED_1_1 field definition (address 10, size 6, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_RESERVED_1_1_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_RESERVED_1_1_MASK        0x3f
   /* DVBT2_PLP_INFO command, IN_BAND_B_FLAG field definition (address 10, size 1, lsb 6, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_IN_BAND_B_FLAG_LSB         6
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_IN_BAND_B_FLAG_MASK        0x01
   /* DVBT2_PLP_INFO command, IN_BAND_A_FLAG field definition (address 10, size 1, lsb 7, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_IN_BAND_A_FLAG_LSB         7
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_IN_BAND_A_FLAG_MASK        0x01
   /* DVBT2_PLP_INFO command, STATIC_FLAG field definition (address 11, size 1, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_STATIC_FLAG_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_STATIC_FLAG_MASK        0x01
   /* DVBT2_PLP_INFO command, PLP_MODE field definition (address 11, size 2, lsb 1, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MODE_LSB         1
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MODE_MASK        0x03
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MODE_HIGH_EFFICIENCY_MODE  2
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MODE_NORMAL_MODE           1
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MODE_NOT_SPECIFIED         0
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MODE_RESERVED              3
   /* DVBT2_PLP_INFO command, RESERVED_1_2 field definition (address 11, size 5, lsb 3, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_RESERVED_1_2_LSB         3
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_RESERVED_1_2_MASK        0x1f
   /* DVBT2_PLP_INFO command, STATIC_PADDING_FLAG field definition (address 12, size 1, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_STATIC_PADDING_FLAG_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_STATIC_PADDING_FLAG_MASK        0x01
   /* DVBT2_PLP_INFO command, PLP_PAYLOAD_TYPE field definition (address 2, size 5, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_PAYLOAD_TYPE_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_PAYLOAD_TYPE_MASK        0x1f
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_PAYLOAD_TYPE_GCS   1
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_PAYLOAD_TYPE_GFPS  0
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_PAYLOAD_TYPE_GSE   2
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_PAYLOAD_TYPE_TS    3
   /* DVBT2_PLP_INFO command, PLP_TYPE field definition (address 2, size 3, lsb 5, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_LSB         5
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_MASK        0x07
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_COMMON      0
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_DATA_TYPE1  1
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_TYPE_DATA_TYPE2  2
   /* DVBT2_PLP_INFO command, FIRST_FRAME_IDX_MSB field definition (address 3, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FIRST_FRAME_IDX_MSB_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FIRST_FRAME_IDX_MSB_MASK        0x0f
   /* DVBT2_PLP_INFO command, FIRST_RF_IDX field definition (address 3, size 3, lsb 4, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FIRST_RF_IDX_LSB         4
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FIRST_RF_IDX_MASK        0x07
   /* DVBT2_PLP_INFO command, FF_FLAG field definition (address 3, size 1, lsb 7, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FF_FLAG_LSB         7
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FF_FLAG_MASK        0x01
   /* DVBT2_PLP_INFO command, PLP_GROUP_ID_MSB field definition (address 4, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_GROUP_ID_MSB_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_GROUP_ID_MSB_MASK        0x0f
   /* DVBT2_PLP_INFO command, FIRST_FRAME_IDX_LSB field definition (address 4, size 4, lsb 4, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FIRST_FRAME_IDX_LSB_LSB         4
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FIRST_FRAME_IDX_LSB_MASK        0x0f
   /* DVBT2_PLP_INFO command, PLP_MOD_MSB field definition (address 5, size 1, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MOD_MSB_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MOD_MSB_MASK        0x01
   /* DVBT2_PLP_INFO command, PLP_COD field definition (address 5, size 3, lsb 1, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_COD_LSB         1
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_COD_MASK        0x07
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_COD_1_2  0
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_COD_2_3  2
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_COD_3_4  3
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_COD_3_5  1
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_COD_4_5  4
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_COD_5_6  5
   /* DVBT2_PLP_INFO command, PLP_GROUP_ID_LSB field definition (address 5, size 4, lsb 4, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_GROUP_ID_LSB_LSB         4
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_GROUP_ID_LSB_MASK        0x0f
   /* DVBT2_PLP_INFO command, PLP_NUM_BLOCKS_MAX_MSB field definition (address 6, size 3, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_NUM_BLOCKS_MAX_MSB_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_NUM_BLOCKS_MAX_MSB_MASK        0x07
   /* DVBT2_PLP_INFO command, PLP_FEC_TYPE field definition (address 6, size 2, lsb 3, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_FEC_TYPE_LSB         3
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_FEC_TYPE_MASK        0x03
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_FEC_TYPE_16K_LDPC  0
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_FEC_TYPE_64K_LDPC  1
   /* DVBT2_PLP_INFO command, PLP_ROT field definition (address 6, size 1, lsb 5, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_ROT_LSB         5
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_ROT_MASK        0x01
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_ROT_NOT_ROTATED  0
    #define Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_ROT_ROTATED      1
   /* DVBT2_PLP_INFO command, PLP_MOD_LSB field definition (address 6, size 2, lsb 6, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MOD_LSB_LSB         6
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_MOD_LSB_MASK        0x03
   /* DVBT2_PLP_INFO command, FRAME_INTERVAL_MSB field definition (address 7, size 1, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FRAME_INTERVAL_MSB_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FRAME_INTERVAL_MSB_MASK        0x01
   /* DVBT2_PLP_INFO command, PLP_NUM_BLOCKS_MAX_LSB field definition (address 7, size 7, lsb 1, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_NUM_BLOCKS_MAX_LSB_LSB         1
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_PLP_NUM_BLOCKS_MAX_LSB_MASK        0x7f
   /* DVBT2_PLP_INFO command, TIME_IL_LENGTH_MSB field definition (address 8, size 1, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_TIME_IL_LENGTH_MSB_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_TIME_IL_LENGTH_MSB_MASK        0x01
   /* DVBT2_PLP_INFO command, FRAME_INTERVAL_LSB field definition (address 8, size 7, lsb 1, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FRAME_INTERVAL_LSB_LSB         1
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_FRAME_INTERVAL_LSB_MASK        0x7f
   /* DVBT2_PLP_INFO command, TIME_IL_TYPE field definition (address 9, size 1, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_TIME_IL_TYPE_LSB         0
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_TIME_IL_TYPE_MASK        0x01
   /* DVBT2_PLP_INFO command, TIME_IL_LENGTH_LSB field definition (address 9, size 7, lsb 1, unsigned)*/
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_TIME_IL_LENGTH_LSB_LSB         1
   #define  Si2168_DVBT2_PLP_INFO_RESPONSE_TIME_IL_LENGTH_LSB_MASK        0x7f

#endif /* Si2168_DVBT2_PLP_INFO_CMD */

/* Si2168_DVBT2_PLP_SELECT command definition */
#define   Si2168_DVBT2_PLP_SELECT_CMD 0x52

#ifdef    Si2168_DVBT2_PLP_SELECT_CMD
  #define Si2168_DVBT2_PLP_SELECT_CMD_CODE 0x010052

    typedef struct { /* Si2168_DVBT2_PLP_SELECT_CMD_struct */
     unsigned char   plp_id;
     unsigned char   plp_id_sel_mode;
   } Si2168_DVBT2_PLP_SELECT_CMD_struct;


    typedef struct { /* Si2168_DVBT2_PLP_SELECT_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_DVBT2_PLP_SELECT_CMD_REPLY_struct;

   /* DVBT2_PLP_SELECT command, PLP_ID field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_LSB         0
   #define  Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_MASK        0xff
   #define  Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_MIN         0
   #define  Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_MAX         255.0
   /* DVBT2_PLP_SELECT command, PLP_ID_SEL_MODE field definition (address 2,size 1, lsb 0, unsigned) */
   #define  Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_LSB         0
   #define  Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MASK        0x01
   #define  Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MIN         0
   #define  Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MAX         1
    #define Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_AUTO    0
    #define Si2168_DVBT2_PLP_SELECT_CMD_PLP_ID_SEL_MODE_MANUAL  1
#endif /* Si2168_DVBT2_PLP_SELECT_CMD */

/* Si2168_DVBT2_STATUS command definition */
#define   Si2168_DVBT2_STATUS_CMD 0x50

#ifdef    Si2168_DVBT2_STATUS_CMD
  #define Si2168_DVBT2_STATUS_CMD_CODE 0x010050

    typedef struct { /* Si2168_DVBT2_STATUS_CMD_struct */
     unsigned char   intack;
   } Si2168_DVBT2_STATUS_CMD_struct;


    typedef struct { /* Si2168_DVBT2_STATUS_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   pclint;
      unsigned char   dlint;
      unsigned char   berint;
      unsigned char   uncorint;
      unsigned char   notdvbt2int;
      unsigned char   num_plp;
      unsigned char   pilot_pattern;
      unsigned char   tx_mode;
      unsigned char   rotated;
      unsigned char   short_frame;
      unsigned char   code_rate;
      unsigned char   plp_id;
      unsigned char   pcl;
      unsigned char   dl;
      unsigned char   ber;
      unsigned char   uncor;
      unsigned char   notdvbt2;
      unsigned char   cnr;
               int    afc_freq;
               int    timing_offset;
      unsigned char   constellation;
      unsigned char   sp_inv;
      unsigned char   fef;
      unsigned char   fft_mode;
      unsigned char   guard_int;
      unsigned char   bw_ext;
   }  Si2168_DVBT2_STATUS_CMD_REPLY_struct;

   /* DVBT2_STATUS command, INTACK field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DVBT2_STATUS_CMD_INTACK_LSB         0
   #define  Si2168_DVBT2_STATUS_CMD_INTACK_MASK        0x01
   #define  Si2168_DVBT2_STATUS_CMD_INTACK_MIN         0
   #define  Si2168_DVBT2_STATUS_CMD_INTACK_MAX         1
    #define Si2168_DVBT2_STATUS_CMD_INTACK_CLEAR  1
    #define Si2168_DVBT2_STATUS_CMD_INTACK_OK     0
   /* DVBT2_STATUS command, PCLINT field definition (address 1, size 1, lsb 1, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_PCLINT_LSB         1
   #define  Si2168_DVBT2_STATUS_RESPONSE_PCLINT_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_PCLINT_CHANGED    1
    #define Si2168_DVBT2_STATUS_RESPONSE_PCLINT_NO_CHANGE  0
   /* DVBT2_STATUS command, DLINT field definition (address 1, size 1, lsb 2, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_DLINT_LSB         2
   #define  Si2168_DVBT2_STATUS_RESPONSE_DLINT_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_DLINT_CHANGED    1
    #define Si2168_DVBT2_STATUS_RESPONSE_DLINT_NO_CHANGE  0
   /* DVBT2_STATUS command, BERINT field definition (address 1, size 1, lsb 3, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_BERINT_LSB         3
   #define  Si2168_DVBT2_STATUS_RESPONSE_BERINT_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_BERINT_CHANGED    1
    #define Si2168_DVBT2_STATUS_RESPONSE_BERINT_NO_CHANGE  0
   /* DVBT2_STATUS command, UNCORINT field definition (address 1, size 1, lsb 4, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_UNCORINT_LSB         4
   #define  Si2168_DVBT2_STATUS_RESPONSE_UNCORINT_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_UNCORINT_CHANGED    1
    #define Si2168_DVBT2_STATUS_RESPONSE_UNCORINT_NO_CHANGE  0
   /* DVBT2_STATUS command, NOTDVBT2INT field definition (address 1, size 1, lsb 5, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_NOTDVBT2INT_LSB         5
   #define  Si2168_DVBT2_STATUS_RESPONSE_NOTDVBT2INT_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_NOTDVBT2INT_CHANGED    1
    #define Si2168_DVBT2_STATUS_RESPONSE_NOTDVBT2INT_NO_CHANGE  0
   /* DVBT2_STATUS command, NUM_PLP field definition (address 10, size 8, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_NUM_PLP_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_NUM_PLP_MASK        0xff
   /* DVBT2_STATUS command, PILOT_PATTERN field definition (address 11, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_MASK        0x0f
    #define Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_PP1  0
    #define Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_PP2  1
    #define Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_PP3  2
    #define Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_PP4  3
    #define Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_PP5  4
    #define Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_PP6  5
    #define Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_PP7  6
    #define Si2168_DVBT2_STATUS_RESPONSE_PILOT_PATTERN_PP8  7
   /* DVBT2_STATUS command, TX_MODE field definition (address 11, size 1, lsb 4, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_TX_MODE_LSB         4
   #define  Si2168_DVBT2_STATUS_RESPONSE_TX_MODE_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_TX_MODE_MISO  1
    #define Si2168_DVBT2_STATUS_RESPONSE_TX_MODE_SISO  0
   /* DVBT2_STATUS command, ROTATED field definition (address 11, size 1, lsb 5, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_ROTATED_LSB         5
   #define  Si2168_DVBT2_STATUS_RESPONSE_ROTATED_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_ROTATED_NORMAL   0
    #define Si2168_DVBT2_STATUS_RESPONSE_ROTATED_ROTATED  1
   /* DVBT2_STATUS command, SHORT_FRAME field definition (address 11, size 1, lsb 6, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_SHORT_FRAME_LSB         6
   #define  Si2168_DVBT2_STATUS_RESPONSE_SHORT_FRAME_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_SHORT_FRAME_16K_LDPC  0
    #define Si2168_DVBT2_STATUS_RESPONSE_SHORT_FRAME_64K_LDPC  1
   /* DVBT2_STATUS command, CODE_RATE field definition (address 12, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_CODE_RATE_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_CODE_RATE_MASK        0x0f
    #define Si2168_DVBT2_STATUS_RESPONSE_CODE_RATE_1_2  1
    #define Si2168_DVBT2_STATUS_RESPONSE_CODE_RATE_2_3  2
    #define Si2168_DVBT2_STATUS_RESPONSE_CODE_RATE_3_4  3
    #define Si2168_DVBT2_STATUS_RESPONSE_CODE_RATE_3_5  13
    #define Si2168_DVBT2_STATUS_RESPONSE_CODE_RATE_4_5  4
    #define Si2168_DVBT2_STATUS_RESPONSE_CODE_RATE_5_6  5
   /* DVBT2_STATUS command, PLP_ID field definition (address 13, size 8, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_PLP_ID_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_PLP_ID_MASK        0xff
   /* DVBT2_STATUS command, PCL field definition (address 2, size 1, lsb 1, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_PCL_LSB         1
   #define  Si2168_DVBT2_STATUS_RESPONSE_PCL_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_PCL_LOCKED   1
    #define Si2168_DVBT2_STATUS_RESPONSE_PCL_NO_LOCK  0
   /* DVBT2_STATUS command, DL field definition (address 2, size 1, lsb 2, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_DL_LSB         2
   #define  Si2168_DVBT2_STATUS_RESPONSE_DL_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_DL_LOCKED   1
    #define Si2168_DVBT2_STATUS_RESPONSE_DL_NO_LOCK  0
   /* DVBT2_STATUS command, BER field definition (address 2, size 1, lsb 3, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_BER_LSB         3
   #define  Si2168_DVBT2_STATUS_RESPONSE_BER_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_BER_BER_ABOVE  1
    #define Si2168_DVBT2_STATUS_RESPONSE_BER_BER_BELOW  0
   /* DVBT2_STATUS command, UNCOR field definition (address 2, size 1, lsb 4, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_UNCOR_LSB         4
   #define  Si2168_DVBT2_STATUS_RESPONSE_UNCOR_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_UNCOR_NO_UNCOR_FOUND  0
    #define Si2168_DVBT2_STATUS_RESPONSE_UNCOR_UNCOR_FOUND     1
   /* DVBT2_STATUS command, NOTDVBT2 field definition (address 2, size 1, lsb 5, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_NOTDVBT2_LSB         5
   #define  Si2168_DVBT2_STATUS_RESPONSE_NOTDVBT2_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_NOTDVBT2_DVBT2      0
    #define Si2168_DVBT2_STATUS_RESPONSE_NOTDVBT2_NOT_DVBT2  1
   /* DVBT2_STATUS command, CNR field definition (address 3, size 8, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_CNR_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_CNR_MASK        0xff
   /* DVBT2_STATUS command, AFC_FREQ field definition (address 4, size 16, lsb 0, signed)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_AFC_FREQ_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_AFC_FREQ_MASK        0xffff
   #define  Si2168_DVBT2_STATUS_RESPONSE_AFC_FREQ_SHIFT       16
   /* DVBT2_STATUS command, TIMING_OFFSET field definition (address 6, size 16, lsb 0, signed)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_TIMING_OFFSET_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_TIMING_OFFSET_MASK        0xffff
   #define  Si2168_DVBT2_STATUS_RESPONSE_TIMING_OFFSET_SHIFT       16
   /* DVBT2_STATUS command, CONSTELLATION field definition (address 8, size 6, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_CONSTELLATION_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_CONSTELLATION_MASK        0x3f
    #define Si2168_DVBT2_STATUS_RESPONSE_CONSTELLATION_QAM128  10
    #define Si2168_DVBT2_STATUS_RESPONSE_CONSTELLATION_QAM16   7
    #define Si2168_DVBT2_STATUS_RESPONSE_CONSTELLATION_QAM256  11
    #define Si2168_DVBT2_STATUS_RESPONSE_CONSTELLATION_QAM32   8
    #define Si2168_DVBT2_STATUS_RESPONSE_CONSTELLATION_QAM64   9
    #define Si2168_DVBT2_STATUS_RESPONSE_CONSTELLATION_QPSK    3
   /* DVBT2_STATUS command, SP_INV field definition (address 8, size 1, lsb 6, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_SP_INV_LSB         6
   #define  Si2168_DVBT2_STATUS_RESPONSE_SP_INV_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_SP_INV_INVERTED  1
    #define Si2168_DVBT2_STATUS_RESPONSE_SP_INV_NORMAL    0
   /* DVBT2_STATUS command, FEF field definition (address 8, size 1, lsb 7, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_FEF_LSB         7
   #define  Si2168_DVBT2_STATUS_RESPONSE_FEF_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_FEF_FEF     1
    #define Si2168_DVBT2_STATUS_RESPONSE_FEF_NO_FEF  0
   /* DVBT2_STATUS command, FFT_MODE field definition (address 9, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_FFT_MODE_LSB         0
   #define  Si2168_DVBT2_STATUS_RESPONSE_FFT_MODE_MASK        0x0f
    #define Si2168_DVBT2_STATUS_RESPONSE_FFT_MODE_16K  14
    #define Si2168_DVBT2_STATUS_RESPONSE_FFT_MODE_1K   10
    #define Si2168_DVBT2_STATUS_RESPONSE_FFT_MODE_2K   11
    #define Si2168_DVBT2_STATUS_RESPONSE_FFT_MODE_32K  15
    #define Si2168_DVBT2_STATUS_RESPONSE_FFT_MODE_4K   12
    #define Si2168_DVBT2_STATUS_RESPONSE_FFT_MODE_8K   13
   /* DVBT2_STATUS command, GUARD_INT field definition (address 9, size 3, lsb 4, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_LSB         4
   #define  Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_MASK        0x07
    #define Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_19_128  6
    #define Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_19_256  7
    #define Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_1_128   5
    #define Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_1_16    2
    #define Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_1_32    1
    #define Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_1_4     4
    #define Si2168_DVBT2_STATUS_RESPONSE_GUARD_INT_1_8     3
   /* DVBT2_STATUS command, BW_EXT field definition (address 9, size 1, lsb 7, unsigned)*/
   #define  Si2168_DVBT2_STATUS_RESPONSE_BW_EXT_LSB         7
   #define  Si2168_DVBT2_STATUS_RESPONSE_BW_EXT_MASK        0x01
    #define Si2168_DVBT2_STATUS_RESPONSE_BW_EXT_EXTENDED  1
    #define Si2168_DVBT2_STATUS_RESPONSE_BW_EXT_NORMAL    0

#endif /* Si2168_DVBT2_STATUS_CMD */

/* Si2168_DVBT2_TX_ID command definition */
#define   Si2168_DVBT2_TX_ID_CMD 0x54

#ifdef    Si2168_DVBT2_TX_ID_CMD
  #define Si2168_DVBT2_TX_ID_CMD_CODE 0x010054

    typedef struct { /* Si2168_DVBT2_TX_ID_CMD_struct */
         unsigned char   nothing;   } Si2168_DVBT2_TX_ID_CMD_struct;


    typedef struct { /* Si2168_DVBT2_TX_ID_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   tx_id_availability;
      unsigned int    cell_id;
      unsigned int    network_id;
      unsigned int    t2_system_id;
   }  Si2168_DVBT2_TX_ID_CMD_REPLY_struct;

   /* DVBT2_TX_ID command, TX_ID_AVAILABILITY field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_TX_ID_RESPONSE_TX_ID_AVAILABILITY_LSB         0
   #define  Si2168_DVBT2_TX_ID_RESPONSE_TX_ID_AVAILABILITY_MASK        0xff
   /* DVBT2_TX_ID command, CELL_ID field definition (address 2, size 16, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_TX_ID_RESPONSE_CELL_ID_LSB         0
   #define  Si2168_DVBT2_TX_ID_RESPONSE_CELL_ID_MASK        0xffff
   /* DVBT2_TX_ID command, NETWORK_ID field definition (address 4, size 16, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_TX_ID_RESPONSE_NETWORK_ID_LSB         0
   #define  Si2168_DVBT2_TX_ID_RESPONSE_NETWORK_ID_MASK        0xffff
   /* DVBT2_TX_ID command, T2_SYSTEM_ID field definition (address 6, size 16, lsb 0, unsigned)*/
   #define  Si2168_DVBT2_TX_ID_RESPONSE_T2_SYSTEM_ID_LSB         0
   #define  Si2168_DVBT2_TX_ID_RESPONSE_T2_SYSTEM_ID_MASK        0xffff

#endif /* Si2168_DVBT2_TX_ID_CMD */


/* Si2168_DVBT_STATUS command definition */
#define   Si2168_DVBT_STATUS_CMD 0xa0

#ifdef    Si2168_DVBT_STATUS_CMD
  #define Si2168_DVBT_STATUS_CMD_CODE 0x0100a0

    typedef struct { /* Si2168_DVBT_STATUS_CMD_struct */
     unsigned char   intack;
   } Si2168_DVBT_STATUS_CMD_struct;


    typedef struct { /* Si2168_DVBT_STATUS_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   pclint;
      unsigned char   dlint;
      unsigned char   berint;
      unsigned char   uncorint;
      unsigned char   notdvbtint;
      unsigned char   fft_mode;
      unsigned char   guard_int;
      unsigned char   hierarchy;
               char   tps_length;
      unsigned char   pcl;
      unsigned char   dl;
      unsigned char   ber;
      unsigned char   uncor;
      unsigned char   notdvbt;
      unsigned char   cnr;
               int    afc_freq;
               int    timing_offset;
      unsigned char   constellation;
      unsigned char   sp_inv;
      unsigned char   rate_hp;
      unsigned char   rate_lp;
   }  Si2168_DVBT_STATUS_CMD_REPLY_struct;

   /* DVBT_STATUS command, INTACK field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_DVBT_STATUS_CMD_INTACK_LSB         0
   #define  Si2168_DVBT_STATUS_CMD_INTACK_MASK        0x01
   #define  Si2168_DVBT_STATUS_CMD_INTACK_MIN         0
   #define  Si2168_DVBT_STATUS_CMD_INTACK_MAX         1
    #define Si2168_DVBT_STATUS_CMD_INTACK_CLEAR  1
    #define Si2168_DVBT_STATUS_CMD_INTACK_OK     0
   /* DVBT_STATUS command, PCLINT field definition (address 1, size 1, lsb 1, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_PCLINT_LSB         1
   #define  Si2168_DVBT_STATUS_RESPONSE_PCLINT_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_PCLINT_CHANGED    1
    #define Si2168_DVBT_STATUS_RESPONSE_PCLINT_NO_CHANGE  0
   /* DVBT_STATUS command, DLINT field definition (address 1, size 1, lsb 2, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_DLINT_LSB         2
   #define  Si2168_DVBT_STATUS_RESPONSE_DLINT_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_DLINT_CHANGED    1
    #define Si2168_DVBT_STATUS_RESPONSE_DLINT_NO_CHANGE  0
   /* DVBT_STATUS command, BERINT field definition (address 1, size 1, lsb 3, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_BERINT_LSB         3
   #define  Si2168_DVBT_STATUS_RESPONSE_BERINT_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_BERINT_CHANGED    1
    #define Si2168_DVBT_STATUS_RESPONSE_BERINT_NO_CHANGE  0
   /* DVBT_STATUS command, UNCORINT field definition (address 1, size 1, lsb 4, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_UNCORINT_LSB         4
   #define  Si2168_DVBT_STATUS_RESPONSE_UNCORINT_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_UNCORINT_CHANGED    1
    #define Si2168_DVBT_STATUS_RESPONSE_UNCORINT_NO_CHANGE  0
   /* DVBT_STATUS command, NOTDVBTINT field definition (address 1, size 1, lsb 5, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_NOTDVBTINT_LSB         5
   #define  Si2168_DVBT_STATUS_RESPONSE_NOTDVBTINT_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_NOTDVBTINT_CHANGED    1
    #define Si2168_DVBT_STATUS_RESPONSE_NOTDVBTINT_NO_CHANGE  0
   /* DVBT_STATUS command, FFT_MODE field definition (address 10, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_FFT_MODE_LSB         0
   #define  Si2168_DVBT_STATUS_RESPONSE_FFT_MODE_MASK        0x0f
    #define Si2168_DVBT_STATUS_RESPONSE_FFT_MODE_2K  11
    #define Si2168_DVBT_STATUS_RESPONSE_FFT_MODE_4K  12
    #define Si2168_DVBT_STATUS_RESPONSE_FFT_MODE_8K  13
   /* DVBT_STATUS command, GUARD_INT field definition (address 10, size 3, lsb 4, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_GUARD_INT_LSB         4
   #define  Si2168_DVBT_STATUS_RESPONSE_GUARD_INT_MASK        0x07
    #define Si2168_DVBT_STATUS_RESPONSE_GUARD_INT_1_16  2
    #define Si2168_DVBT_STATUS_RESPONSE_GUARD_INT_1_32  1
    #define Si2168_DVBT_STATUS_RESPONSE_GUARD_INT_1_4   4
    #define Si2168_DVBT_STATUS_RESPONSE_GUARD_INT_1_8   3
   /* DVBT_STATUS command, HIERARCHY field definition (address 11, size 3, lsb 0, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_HIERARCHY_LSB         0
   #define  Si2168_DVBT_STATUS_RESPONSE_HIERARCHY_MASK        0x07
    #define Si2168_DVBT_STATUS_RESPONSE_HIERARCHY_ALFA1  2
    #define Si2168_DVBT_STATUS_RESPONSE_HIERARCHY_ALFA2  3
    #define Si2168_DVBT_STATUS_RESPONSE_HIERARCHY_ALFA4  5
    #define Si2168_DVBT_STATUS_RESPONSE_HIERARCHY_NONE   1
   /* DVBT_STATUS command, TPS_LENGTH field definition (address 12, size 7, lsb 0, signed)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_TPS_LENGTH_LSB         0
   #define  Si2168_DVBT_STATUS_RESPONSE_TPS_LENGTH_MASK        0x7f
   #define  Si2168_DVBT_STATUS_RESPONSE_TPS_LENGTH_SHIFT       25
   /* DVBT_STATUS command, PCL field definition (address 2, size 1, lsb 1, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_PCL_LSB         1
   #define  Si2168_DVBT_STATUS_RESPONSE_PCL_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_PCL_LOCKED   1
    #define Si2168_DVBT_STATUS_RESPONSE_PCL_NO_LOCK  0
   /* DVBT_STATUS command, DL field definition (address 2, size 1, lsb 2, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_DL_LSB         2
   #define  Si2168_DVBT_STATUS_RESPONSE_DL_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_DL_LOCKED   1
    #define Si2168_DVBT_STATUS_RESPONSE_DL_NO_LOCK  0
   /* DVBT_STATUS command, BER field definition (address 2, size 1, lsb 3, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_BER_LSB         3
   #define  Si2168_DVBT_STATUS_RESPONSE_BER_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_BER_BER_ABOVE  1
    #define Si2168_DVBT_STATUS_RESPONSE_BER_BER_BELOW  0
   /* DVBT_STATUS command, UNCOR field definition (address 2, size 1, lsb 4, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_UNCOR_LSB         4
   #define  Si2168_DVBT_STATUS_RESPONSE_UNCOR_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_UNCOR_NO_UNCOR_FOUND  0
    #define Si2168_DVBT_STATUS_RESPONSE_UNCOR_UNCOR_FOUND     1
   /* DVBT_STATUS command, NOTDVBT field definition (address 2, size 1, lsb 5, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_NOTDVBT_LSB         5
   #define  Si2168_DVBT_STATUS_RESPONSE_NOTDVBT_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_NOTDVBT_DVBT      0
    #define Si2168_DVBT_STATUS_RESPONSE_NOTDVBT_NOT_DVBT  1
   /* DVBT_STATUS command, CNR field definition (address 3, size 8, lsb 0, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_CNR_LSB         0
   #define  Si2168_DVBT_STATUS_RESPONSE_CNR_MASK        0xff
   /* DVBT_STATUS command, AFC_FREQ field definition (address 4, size 16, lsb 0, signed)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_AFC_FREQ_LSB         0
   #define  Si2168_DVBT_STATUS_RESPONSE_AFC_FREQ_MASK        0xffff
   #define  Si2168_DVBT_STATUS_RESPONSE_AFC_FREQ_SHIFT       16
   /* DVBT_STATUS command, TIMING_OFFSET field definition (address 6, size 16, lsb 0, signed)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_TIMING_OFFSET_LSB         0
   #define  Si2168_DVBT_STATUS_RESPONSE_TIMING_OFFSET_MASK        0xffff
   #define  Si2168_DVBT_STATUS_RESPONSE_TIMING_OFFSET_SHIFT       16
   /* DVBT_STATUS command, CONSTELLATION field definition (address 8, size 6, lsb 0, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_CONSTELLATION_LSB         0
   #define  Si2168_DVBT_STATUS_RESPONSE_CONSTELLATION_MASK        0x3f
    #define Si2168_DVBT_STATUS_RESPONSE_CONSTELLATION_QAM16  7
    #define Si2168_DVBT_STATUS_RESPONSE_CONSTELLATION_QAM64  9
    #define Si2168_DVBT_STATUS_RESPONSE_CONSTELLATION_QPSK   3
   /* DVBT_STATUS command, SP_INV field definition (address 8, size 1, lsb 6, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_SP_INV_LSB         6
   #define  Si2168_DVBT_STATUS_RESPONSE_SP_INV_MASK        0x01
    #define Si2168_DVBT_STATUS_RESPONSE_SP_INV_INVERTED  1
    #define Si2168_DVBT_STATUS_RESPONSE_SP_INV_NORMAL    0
   /* DVBT_STATUS command, RATE_HP field definition (address 9, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_RATE_HP_LSB         0
   #define  Si2168_DVBT_STATUS_RESPONSE_RATE_HP_MASK        0x0f
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_HP_1_2  1
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_HP_2_3  2
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_HP_3_4  3
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_HP_5_6  5
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_HP_7_8  7
   /* DVBT_STATUS command, RATE_LP field definition (address 9, size 4, lsb 4, unsigned)*/
   #define  Si2168_DVBT_STATUS_RESPONSE_RATE_LP_LSB         4
   #define  Si2168_DVBT_STATUS_RESPONSE_RATE_LP_MASK        0x0f
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_LP_1_2  1
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_LP_2_3  2
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_LP_3_4  3
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_LP_5_6  5
    #define Si2168_DVBT_STATUS_RESPONSE_RATE_LP_7_8  7

#endif /* Si2168_DVBT_STATUS_CMD */

/* Si2168_DVBT_TPS_EXTRA command definition */
#define   Si2168_DVBT_TPS_EXTRA_CMD 0xa1

#ifdef    Si2168_DVBT_TPS_EXTRA_CMD
  #define Si2168_DVBT_TPS_EXTRA_CMD_CODE 0x0100a1

    typedef struct { /* Si2168_DVBT_TPS_EXTRA_CMD_struct */
         unsigned char   nothing;   } Si2168_DVBT_TPS_EXTRA_CMD_struct;


    typedef struct { /* Si2168_DVBT_TPS_EXTRA_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   lptimeslice;
      unsigned char   hptimeslice;
      unsigned char   lpmpefec;
      unsigned char   hpmpefec;
      unsigned char   dvbhinter;
               int    cell_id;
      unsigned char   tps_res1;
      unsigned char   tps_res2;
      unsigned char   tps_res3;
      unsigned char   tps_res4;
   }  Si2168_DVBT_TPS_EXTRA_CMD_REPLY_struct;

   /* DVBT_TPS_EXTRA command, LPTIMESLICE field definition (address 1, size 1, lsb 0, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_LPTIMESLICE_LSB         0
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_LPTIMESLICE_MASK        0x01
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_LPTIMESLICE_OFF  0
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_LPTIMESLICE_ON   1
   /* DVBT_TPS_EXTRA command, HPTIMESLICE field definition (address 1, size 1, lsb 1, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_HPTIMESLICE_LSB         1
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_HPTIMESLICE_MASK        0x01
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_HPTIMESLICE_OFF  0
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_HPTIMESLICE_ON   1
   /* DVBT_TPS_EXTRA command, LPMPEFEC field definition (address 1, size 1, lsb 2, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_LPMPEFEC_LSB         2
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_LPMPEFEC_MASK        0x01
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_LPMPEFEC_OFF  0
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_LPMPEFEC_ON   1
   /* DVBT_TPS_EXTRA command, HPMPEFEC field definition (address 1, size 1, lsb 3, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_HPMPEFEC_LSB         3
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_HPMPEFEC_MASK        0x01
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_HPMPEFEC_OFF  0
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_HPMPEFEC_ON   1
   /* DVBT_TPS_EXTRA command, DVBHINTER field definition (address 1, size 1, lsb 4, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_DVBHINTER_LSB         4
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_DVBHINTER_MASK        0x01
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_DVBHINTER_IN_DEPTH  1
    #define Si2168_DVBT_TPS_EXTRA_RESPONSE_DVBHINTER_NATIVE    0
   /* DVBT_TPS_EXTRA command, CELL_ID field definition (address 2, size 16, lsb 0, signed)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_CELL_ID_LSB         0
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_CELL_ID_MASK        0xffff
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_CELL_ID_SHIFT       16
   /* DVBT_TPS_EXTRA command, TPS_RES1 field definition (address 4, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_TPS_RES1_LSB         0
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_TPS_RES1_MASK        0x0f
   /* DVBT_TPS_EXTRA command, TPS_RES2 field definition (address 4, size 4, lsb 4, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_TPS_RES2_LSB         4
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_TPS_RES2_MASK        0x0f
   /* DVBT_TPS_EXTRA command, TPS_RES3 field definition (address 5, size 4, lsb 0, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_TPS_RES3_LSB         0
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_TPS_RES3_MASK        0x0f
   /* DVBT_TPS_EXTRA command, TPS_RES4 field definition (address 5, size 4, lsb 4, unsigned)*/
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_TPS_RES4_LSB         4
   #define  Si2168_DVBT_TPS_EXTRA_RESPONSE_TPS_RES4_MASK        0x0f

#endif /* Si2168_DVBT_TPS_EXTRA_CMD */


/* Si2168_EXIT_BOOTLOADER command definition */
#define   Si2168_EXIT_BOOTLOADER_CMD 0x01

#ifdef    Si2168_EXIT_BOOTLOADER_CMD
  #define Si2168_EXIT_BOOTLOADER_CMD_CODE 0x010001

    typedef struct { /* Si2168_EXIT_BOOTLOADER_CMD_struct */
     unsigned char   func;
     unsigned char   ctsien;
   } Si2168_EXIT_BOOTLOADER_CMD_struct;


    typedef struct { /* Si2168_EXIT_BOOTLOADER_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_EXIT_BOOTLOADER_CMD_REPLY_struct;

   /* EXIT_BOOTLOADER command, FUNC field definition (address 1,size 4, lsb 0, unsigned) */
   #define  Si2168_EXIT_BOOTLOADER_CMD_FUNC_LSB         0
   #define  Si2168_EXIT_BOOTLOADER_CMD_FUNC_MASK        0x0f
   #define  Si2168_EXIT_BOOTLOADER_CMD_FUNC_MIN         0
   #define  Si2168_EXIT_BOOTLOADER_CMD_FUNC_MAX         1
    #define Si2168_EXIT_BOOTLOADER_CMD_FUNC_BOOTLOADER  0
    #define Si2168_EXIT_BOOTLOADER_CMD_FUNC_NORMAL      1
   /* EXIT_BOOTLOADER command, CTSIEN field definition (address 1,size 1, lsb 7, unsigned) */
   #define  Si2168_EXIT_BOOTLOADER_CMD_CTSIEN_LSB         7
   #define  Si2168_EXIT_BOOTLOADER_CMD_CTSIEN_MASK        0x01
   #define  Si2168_EXIT_BOOTLOADER_CMD_CTSIEN_MIN         0
   #define  Si2168_EXIT_BOOTLOADER_CMD_CTSIEN_MAX         1
    #define Si2168_EXIT_BOOTLOADER_CMD_CTSIEN_OFF  0
    #define Si2168_EXIT_BOOTLOADER_CMD_CTSIEN_ON   1
#endif /* Si2168_EXIT_BOOTLOADER_CMD */

/* Si2168_GET_PROPERTY command definition */
#define   Si2168_GET_PROPERTY_CMD 0x15

#ifdef    Si2168_GET_PROPERTY_CMD
  #define Si2168_GET_PROPERTY_CMD_CODE 0x010015

    typedef struct { /* Si2168_GET_PROPERTY_CMD_struct */
     unsigned char   reserved;
     unsigned int    prop;
   } Si2168_GET_PROPERTY_CMD_struct;


    typedef struct { /* Si2168_GET_PROPERTY_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   reserved;
      unsigned int    data;
   }  Si2168_GET_PROPERTY_CMD_REPLY_struct;

   /* GET_PROPERTY command, RESERVED field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_GET_PROPERTY_CMD_RESERVED_LSB         0
   #define  Si2168_GET_PROPERTY_CMD_RESERVED_MASK        0xff
   #define  Si2168_GET_PROPERTY_CMD_RESERVED_MIN         0
   #define  Si2168_GET_PROPERTY_CMD_RESERVED_MAX         0
    #define Si2168_GET_PROPERTY_CMD_RESERVED_RESERVED_MIN  0
    #define Si2168_GET_PROPERTY_CMD_RESERVED_RESERVED_MAX  0
   /* GET_PROPERTY command, PROP field definition (address 2,size 16, lsb 0, unsigned) */
   #define  Si2168_GET_PROPERTY_CMD_PROP_LSB         0
   #define  Si2168_GET_PROPERTY_CMD_PROP_MASK        0xffff
   #define  Si2168_GET_PROPERTY_CMD_PROP_MIN         0
   #define  Si2168_GET_PROPERTY_CMD_PROP_MAX         65535
    #define Si2168_GET_PROPERTY_CMD_PROP_PROP_MIN  0
    #define Si2168_GET_PROPERTY_CMD_PROP_PROP_MAX  65535
   /* GET_PROPERTY command, RESERVED field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_GET_PROPERTY_RESPONSE_RESERVED_LSB         0
   #define  Si2168_GET_PROPERTY_RESPONSE_RESERVED_MASK        0xff
   /* GET_PROPERTY command, DATA field definition (address 2, size 16, lsb 0, unsigned)*/
   #define  Si2168_GET_PROPERTY_RESPONSE_DATA_LSB         0
   #define  Si2168_GET_PROPERTY_RESPONSE_DATA_MASK        0xffff

#endif /* Si2168_GET_PROPERTY_CMD */

/* Si2168_GET_REV command definition */
#define   Si2168_GET_REV_CMD 0x11

#ifdef    Si2168_GET_REV_CMD
  #define Si2168_GET_REV_CMD_CODE 0x010011

    typedef struct { /* Si2168_GET_REV_CMD_struct */
         unsigned char   nothing;   } Si2168_GET_REV_CMD_struct;


    typedef struct { /* Si2168_GET_REV_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   pn;
      unsigned char   fwmajor;
      unsigned char   fwminor;
      unsigned int    patch;
      unsigned char   cmpmajor;
      unsigned char   cmpminor;
      unsigned char   cmpbuild;
      unsigned char   chiprev;
   }  Si2168_GET_REV_CMD_REPLY_struct;

   /* GET_REV command, PN field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_GET_REV_RESPONSE_PN_LSB         0
   #define  Si2168_GET_REV_RESPONSE_PN_MASK        0xff
   /* GET_REV command, FWMAJOR field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_GET_REV_RESPONSE_FWMAJOR_LSB         0
   #define  Si2168_GET_REV_RESPONSE_FWMAJOR_MASK        0xff
   /* GET_REV command, FWMINOR field definition (address 3, size 8, lsb 0, unsigned)*/
   #define  Si2168_GET_REV_RESPONSE_FWMINOR_LSB         0
   #define  Si2168_GET_REV_RESPONSE_FWMINOR_MASK        0xff
   /* GET_REV command, PATCH field definition (address 4, size 16, lsb 0, unsigned)*/
   #define  Si2168_GET_REV_RESPONSE_PATCH_LSB         0
   #define  Si2168_GET_REV_RESPONSE_PATCH_MASK        0xffff
   /* GET_REV command, CMPMAJOR field definition (address 6, size 8, lsb 0, unsigned)*/
   #define  Si2168_GET_REV_RESPONSE_CMPMAJOR_LSB         0
   #define  Si2168_GET_REV_RESPONSE_CMPMAJOR_MASK        0xff
   /* GET_REV command, CMPMINOR field definition (address 7, size 8, lsb 0, unsigned)*/
   #define  Si2168_GET_REV_RESPONSE_CMPMINOR_LSB         0
   #define  Si2168_GET_REV_RESPONSE_CMPMINOR_MASK        0xff
   /* GET_REV command, CMPBUILD field definition (address 8, size 8, lsb 0, unsigned)*/
   #define  Si2168_GET_REV_RESPONSE_CMPBUILD_LSB         0
   #define  Si2168_GET_REV_RESPONSE_CMPBUILD_MASK        0xff
    #define Si2168_GET_REV_RESPONSE_CMPBUILD_CMPBUILD_MIN  0
    #define Si2168_GET_REV_RESPONSE_CMPBUILD_CMPBUILD_MAX  255
   /* GET_REV command, CHIPREV field definition (address 9, size 4, lsb 0, unsigned)*/
   #define  Si2168_GET_REV_RESPONSE_CHIPREV_LSB         0
   #define  Si2168_GET_REV_RESPONSE_CHIPREV_MASK        0x0f
    #define Si2168_GET_REV_RESPONSE_CHIPREV_A  1
    #define Si2168_GET_REV_RESPONSE_CHIPREV_B  2

#endif /* Si2168_GET_REV_CMD */

/* Si2168_I2C_PASSTHROUGH command definition */
#define   Si2168_I2C_PASSTHROUGH_CMD 0xc0

#ifdef    Si2168_I2C_PASSTHROUGH_CMD
  #define Si2168_I2C_PASSTHROUGH_CMD_CODE 0x0100c0

    typedef struct { /* Si2168_I2C_PASSTHROUGH_CMD_struct */
     unsigned char   subcode;
     unsigned char   i2c_passthru;
     unsigned char   reserved;
   } Si2168_I2C_PASSTHROUGH_CMD_struct;


    typedef struct { /* Si2168_I2C_PASSTHROUGH_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_I2C_PASSTHROUGH_CMD_REPLY_struct;

   /* I2C_PASSTHROUGH command, SUBCODE field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_I2C_PASSTHROUGH_CMD_SUBCODE_LSB         0
   #define  Si2168_I2C_PASSTHROUGH_CMD_SUBCODE_MASK        0xff
   #define  Si2168_I2C_PASSTHROUGH_CMD_SUBCODE_MIN         13
   #define  Si2168_I2C_PASSTHROUGH_CMD_SUBCODE_MAX         13
    #define Si2168_I2C_PASSTHROUGH_CMD_SUBCODE_CODE  13
   /* I2C_PASSTHROUGH command, I2C_PASSTHRU field definition (address 2,size 1, lsb 0, unsigned) */
   #define  Si2168_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_LSB         0
   #define  Si2168_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_MASK        0x01
   #define  Si2168_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_MIN         0
   #define  Si2168_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_MAX         1
    #define Si2168_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_CLOSE  1
    #define Si2168_I2C_PASSTHROUGH_CMD_I2C_PASSTHRU_OPEN   0
   /* I2C_PASSTHROUGH command, RESERVED field definition (address 2,size 7, lsb 1, unsigned) */
   #define  Si2168_I2C_PASSTHROUGH_CMD_RESERVED_LSB         1
   #define  Si2168_I2C_PASSTHROUGH_CMD_RESERVED_MASK        0x7f
   #define  Si2168_I2C_PASSTHROUGH_CMD_RESERVED_MIN         0
   #define  Si2168_I2C_PASSTHROUGH_CMD_RESERVED_MAX         0
    #define Si2168_I2C_PASSTHROUGH_CMD_RESERVED_RESERVED  0
#endif /* Si2168_I2C_PASSTHROUGH_CMD */

/* Si2168_PART_INFO command definition */
#define   Si2168_PART_INFO_CMD 0x02

#ifdef    Si2168_PART_INFO_CMD
  #define Si2168_PART_INFO_CMD_CODE 0x010002

    typedef struct { /* Si2168_PART_INFO_CMD_struct */
         unsigned char   nothing;   } Si2168_PART_INFO_CMD_struct;


    typedef struct { /* Si2168_PART_INFO_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   chiprev;
      unsigned char   romid;
      unsigned char   part;
      unsigned char   pmajor;
      unsigned char   pminor;
      unsigned char   pbuild;
      unsigned int    reserved;
      unsigned long   serial;
   }  Si2168_PART_INFO_CMD_REPLY_struct;

   /* PART_INFO command, CHIPREV field definition (address 1, size 4, lsb 0, unsigned)*/
   #define  Si2168_PART_INFO_RESPONSE_CHIPREV_LSB         0
   #define  Si2168_PART_INFO_RESPONSE_CHIPREV_MASK        0x0f
    #define Si2168_PART_INFO_RESPONSE_CHIPREV_A  1
    #define Si2168_PART_INFO_RESPONSE_CHIPREV_B  2
   /* PART_INFO command, ROMID field definition (address 12, size 8, lsb 0, unsigned)*/
   #define  Si2168_PART_INFO_RESPONSE_ROMID_LSB         0
   #define  Si2168_PART_INFO_RESPONSE_ROMID_MASK        0xff
   /* PART_INFO command, PART field definition (address 2, size 8, lsb 0, unsigned)*/
   #define  Si2168_PART_INFO_RESPONSE_PART_LSB         0
   #define  Si2168_PART_INFO_RESPONSE_PART_MASK        0xff
   /* PART_INFO command, PMAJOR field definition (address 3, size 8, lsb 0, unsigned)*/
   #define  Si2168_PART_INFO_RESPONSE_PMAJOR_LSB         0
   #define  Si2168_PART_INFO_RESPONSE_PMAJOR_MASK        0xff
   /* PART_INFO command, PMINOR field definition (address 4, size 8, lsb 0, unsigned)*/
   #define  Si2168_PART_INFO_RESPONSE_PMINOR_LSB         0
   #define  Si2168_PART_INFO_RESPONSE_PMINOR_MASK        0xff
   /* PART_INFO command, PBUILD field definition (address 5, size 8, lsb 0, unsigned)*/
   #define  Si2168_PART_INFO_RESPONSE_PBUILD_LSB         0
   #define  Si2168_PART_INFO_RESPONSE_PBUILD_MASK        0xff
   /* PART_INFO command, RESERVED field definition (address 6, size 16, lsb 0, unsigned)*/
   #define  Si2168_PART_INFO_RESPONSE_RESERVED_LSB         0
   #define  Si2168_PART_INFO_RESPONSE_RESERVED_MASK        0xffff
   /* PART_INFO command, SERIAL field definition (address 8, size 32, lsb 0, unsigned)*/
   #define  Si2168_PART_INFO_RESPONSE_SERIAL_LSB         0
   #define  Si2168_PART_INFO_RESPONSE_SERIAL_MASK        0xffffffff

#endif /* Si2168_PART_INFO_CMD */

/* Si2168_POWER_DOWN command definition */
#define   Si2168_POWER_DOWN_CMD 0x13

#ifdef    Si2168_POWER_DOWN_CMD
  #define Si2168_POWER_DOWN_CMD_CODE 0x010013

    typedef struct { /* Si2168_POWER_DOWN_CMD_struct */
         unsigned char   nothing;   } Si2168_POWER_DOWN_CMD_struct;


    typedef struct { /* Si2168_POWER_DOWN_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_POWER_DOWN_CMD_REPLY_struct;

#endif /* Si2168_POWER_DOWN_CMD */

/* Si2168_POWER_UP command definition */
#define   Si2168_POWER_UP_CMD 0xc0

#ifdef    Si2168_POWER_UP_CMD
  #define Si2168_POWER_UP_CMD_CODE 0x0200c0

    typedef struct { /* Si2168_POWER_UP_CMD_struct */
     unsigned char   subcode;
     unsigned char   reset;
     unsigned char   reserved2;
     unsigned char   reserved4;
     unsigned char   reserved1;
     unsigned char   addr_mode;
     unsigned char   reserved5;
     unsigned char   func;
     unsigned char   clock_freq;
     unsigned char   ctsien;
     unsigned char   wake_up;
   } Si2168_POWER_UP_CMD_struct;


    typedef struct { /* Si2168_POWER_UP_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_POWER_UP_CMD_REPLY_struct;

   /* POWER_UP command, SUBCODE field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_POWER_UP_CMD_SUBCODE_LSB         0
   #define  Si2168_POWER_UP_CMD_SUBCODE_MASK        0xff
   #define  Si2168_POWER_UP_CMD_SUBCODE_MIN         6
   #define  Si2168_POWER_UP_CMD_SUBCODE_MAX         6
    #define Si2168_POWER_UP_CMD_SUBCODE_CODE  6
   /* POWER_UP command, RESET field definition (address 2,size 8, lsb 0, unsigned) */
   #define  Si2168_POWER_UP_CMD_RESET_LSB         0
   #define  Si2168_POWER_UP_CMD_RESET_MASK        0xff
   #define  Si2168_POWER_UP_CMD_RESET_MIN         1
   #define  Si2168_POWER_UP_CMD_RESET_MAX         8
    #define Si2168_POWER_UP_CMD_RESET_RESET   1
    #define Si2168_POWER_UP_CMD_RESET_RESUME  8
   /* POWER_UP command, RESERVED2 field definition (address 3,size 8, lsb 0, unsigned) */
   #define  Si2168_POWER_UP_CMD_RESERVED2_LSB         0
   #define  Si2168_POWER_UP_CMD_RESERVED2_MASK        0xff
   #define  Si2168_POWER_UP_CMD_RESERVED2_MIN         15
   #define  Si2168_POWER_UP_CMD_RESERVED2_MAX         15
    #define Si2168_POWER_UP_CMD_RESERVED2_RESERVED  15
   /* POWER_UP command, RESERVED4 field definition (address 4,size 8, lsb 0, unsigned) */
   #define  Si2168_POWER_UP_CMD_RESERVED4_LSB         0
   #define  Si2168_POWER_UP_CMD_RESERVED4_MASK        0xff
   #define  Si2168_POWER_UP_CMD_RESERVED4_MIN         0
   #define  Si2168_POWER_UP_CMD_RESERVED4_MAX         0
    #define Si2168_POWER_UP_CMD_RESERVED4_RESERVED  0
   /* POWER_UP command, RESERVED1 field definition (address 5,size 4, lsb 0, unsigned) */
   #define  Si2168_POWER_UP_CMD_RESERVED1_LSB         0
   #define  Si2168_POWER_UP_CMD_RESERVED1_MASK        0x0f
   #define  Si2168_POWER_UP_CMD_RESERVED1_MIN         0
   #define  Si2168_POWER_UP_CMD_RESERVED1_MAX         0
    #define Si2168_POWER_UP_CMD_RESERVED1_RESERVED  0
   /* POWER_UP command, ADDR_MODE field definition (address 5,size 1, lsb 4, unsigned) */
   #define  Si2168_POWER_UP_CMD_ADDR_MODE_LSB         4
   #define  Si2168_POWER_UP_CMD_ADDR_MODE_MASK        0x01
   #define  Si2168_POWER_UP_CMD_ADDR_MODE_MIN         0
   #define  Si2168_POWER_UP_CMD_ADDR_MODE_MAX         1
    #define Si2168_POWER_UP_CMD_ADDR_MODE_CAPTURE  1
    #define Si2168_POWER_UP_CMD_ADDR_MODE_CURRENT  0
   /* POWER_UP command, RESERVED5 field definition (address 5,size 1, lsb 5, unsigned) */
   #define  Si2168_POWER_UP_CMD_RESERVED5_LSB         5
   #define  Si2168_POWER_UP_CMD_RESERVED5_MASK        0x01
   #define  Si2168_POWER_UP_CMD_RESERVED5_MIN         1
   #define  Si2168_POWER_UP_CMD_RESERVED5_MAX         1
    #define Si2168_POWER_UP_CMD_RESERVED5_RESERVED  1
   /* POWER_UP command, FUNC field definition (address 6,size 4, lsb 0, unsigned) */
   #define  Si2168_POWER_UP_CMD_FUNC_LSB         0
   #define  Si2168_POWER_UP_CMD_FUNC_MASK        0x0f
   #define  Si2168_POWER_UP_CMD_FUNC_MIN         0
   #define  Si2168_POWER_UP_CMD_FUNC_MAX         1
    #define Si2168_POWER_UP_CMD_FUNC_BOOTLOADER  0
    #define Si2168_POWER_UP_CMD_FUNC_NORMAL      1
   /* POWER_UP command, CLOCK_FREQ field definition (address 6,size 3, lsb 4, unsigned) */
   #define  Si2168_POWER_UP_CMD_CLOCK_FREQ_LSB         4
   #define  Si2168_POWER_UP_CMD_CLOCK_FREQ_MASK        0x07
   #define  Si2168_POWER_UP_CMD_CLOCK_FREQ_MIN         0
   #define  Si2168_POWER_UP_CMD_CLOCK_FREQ_MAX         4
    #define Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_16MHZ  0
    #define Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_24MHZ  2
    #define Si2168_POWER_UP_CMD_CLOCK_FREQ_CLK_27MHZ  3
   /* POWER_UP command, CTSIEN field definition (address 6,size 1, lsb 7, unsigned) */
   #define  Si2168_POWER_UP_CMD_CTSIEN_LSB         7
   #define  Si2168_POWER_UP_CMD_CTSIEN_MASK        0x01
   #define  Si2168_POWER_UP_CMD_CTSIEN_MIN         0
   #define  Si2168_POWER_UP_CMD_CTSIEN_MAX         1
    #define Si2168_POWER_UP_CMD_CTSIEN_DISABLE  0
    #define Si2168_POWER_UP_CMD_CTSIEN_ENABLE   1
   /* POWER_UP command, WAKE_UP field definition (address 7,size 1, lsb 0, unsigned) */
   #define  Si2168_POWER_UP_CMD_WAKE_UP_LSB         0
   #define  Si2168_POWER_UP_CMD_WAKE_UP_MASK        0x01
   #define  Si2168_POWER_UP_CMD_WAKE_UP_MIN         1
   #define  Si2168_POWER_UP_CMD_WAKE_UP_MAX         1
    #define Si2168_POWER_UP_CMD_WAKE_UP_WAKE_UP  1
#endif /* Si2168_POWER_UP_CMD */

/* Si2168_RSSI_ADC command definition */
#define   Si2168_RSSI_ADC_CMD 0x17

#ifdef    Si2168_RSSI_ADC_CMD
  #define Si2168_RSSI_ADC_CMD_CODE 0x010017

    typedef struct { /* Si2168_RSSI_ADC_CMD_struct */
     unsigned char   on_off;
   } Si2168_RSSI_ADC_CMD_struct;


    typedef struct { /* Si2168_RSSI_ADC_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   level;
   }  Si2168_RSSI_ADC_CMD_REPLY_struct;

   /* RSSI_ADC command, ON_OFF field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_RSSI_ADC_CMD_ON_OFF_LSB         0
   #define  Si2168_RSSI_ADC_CMD_ON_OFF_MASK        0x01
   #define  Si2168_RSSI_ADC_CMD_ON_OFF_MIN         0
   #define  Si2168_RSSI_ADC_CMD_ON_OFF_MAX         1
    #define Si2168_RSSI_ADC_CMD_ON_OFF_OFF  0
    #define Si2168_RSSI_ADC_CMD_ON_OFF_ON   1
   /* RSSI_ADC command, LEVEL field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_RSSI_ADC_RESPONSE_LEVEL_LSB         0
   #define  Si2168_RSSI_ADC_RESPONSE_LEVEL_MASK        0xff

#endif /* Si2168_RSSI_ADC_CMD */

/* Si2168_SCAN_CTRL command definition */
#define   Si2168_SCAN_CTRL_CMD 0x31

#ifdef    Si2168_SCAN_CTRL_CMD
  #define Si2168_SCAN_CTRL_CMD_CODE 0x010031

    typedef struct { /* Si2168_SCAN_CTRL_CMD_struct */
     unsigned char   action;
     unsigned long   tuned_rf_freq;
   } Si2168_SCAN_CTRL_CMD_struct;


    typedef struct { /* Si2168_SCAN_CTRL_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_SCAN_CTRL_CMD_REPLY_struct;

   /* SCAN_CTRL command, ACTION field definition (address 1,size 4, lsb 0, unsigned) */
   #define  Si2168_SCAN_CTRL_CMD_ACTION_LSB         0
   #define  Si2168_SCAN_CTRL_CMD_ACTION_MASK        0x0f
   #define  Si2168_SCAN_CTRL_CMD_ACTION_MIN         1
   #define  Si2168_SCAN_CTRL_CMD_ACTION_MAX         3
    #define Si2168_SCAN_CTRL_CMD_ACTION_ABORT   3
    #define Si2168_SCAN_CTRL_CMD_ACTION_RESUME  2
    #define Si2168_SCAN_CTRL_CMD_ACTION_START   1
   /* SCAN_CTRL command, TUNED_RF_FREQ field definition (address 4,size 32, lsb 0, unsigned) */
   #define  Si2168_SCAN_CTRL_CMD_TUNED_RF_FREQ_LSB         0
   #define  Si2168_SCAN_CTRL_CMD_TUNED_RF_FREQ_MASK        0xffffffff
   #define  Si2168_SCAN_CTRL_CMD_TUNED_RF_FREQ_MIN         0
   #define  Si2168_SCAN_CTRL_CMD_TUNED_RF_FREQ_MAX         4294967
    #define Si2168_SCAN_CTRL_CMD_TUNED_RF_FREQ_TUNED_RF_FREQ_MIN  0
    #define Si2168_SCAN_CTRL_CMD_TUNED_RF_FREQ_TUNED_RF_FREQ_MAX  4294967
#endif /* Si2168_SCAN_CTRL_CMD */

/* Si2168_SCAN_STATUS command definition */
#define   Si2168_SCAN_STATUS_CMD 0x30

#ifdef    Si2168_SCAN_STATUS_CMD
  #define Si2168_SCAN_STATUS_CMD_CODE 0x010030

    typedef struct { /* Si2168_SCAN_STATUS_CMD_struct */
     unsigned char   intack;
   } Si2168_SCAN_STATUS_CMD_struct;


    typedef struct { /* Si2168_SCAN_STATUS_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   buzint;
      unsigned char   reqint;
      unsigned char   modulation;
      unsigned char   buz;
      unsigned char   req;
      unsigned char   scan_status;
      unsigned long   rf_freq;
      unsigned int    symb_rate;
   }  Si2168_SCAN_STATUS_CMD_REPLY_struct;

   /* SCAN_STATUS command, INTACK field definition (address 1,size 1, lsb 0, unsigned) */
   #define  Si2168_SCAN_STATUS_CMD_INTACK_LSB         0
   #define  Si2168_SCAN_STATUS_CMD_INTACK_MASK        0x01
   #define  Si2168_SCAN_STATUS_CMD_INTACK_MIN         0
   #define  Si2168_SCAN_STATUS_CMD_INTACK_MAX         1
    #define Si2168_SCAN_STATUS_CMD_INTACK_CLEAR  1
    #define Si2168_SCAN_STATUS_CMD_INTACK_OK     0
   /* SCAN_STATUS command, BUZINT field definition (address 1, size 1, lsb 0, unsigned)*/
   #define  Si2168_SCAN_STATUS_RESPONSE_BUZINT_LSB         0
   #define  Si2168_SCAN_STATUS_RESPONSE_BUZINT_MASK        0x01
    #define Si2168_SCAN_STATUS_RESPONSE_BUZINT_CHANGED    1
    #define Si2168_SCAN_STATUS_RESPONSE_BUZINT_NO_CHANGE  0
   /* SCAN_STATUS command, REQINT field definition (address 1, size 1, lsb 1, unsigned)*/
   #define  Si2168_SCAN_STATUS_RESPONSE_REQINT_LSB         1
   #define  Si2168_SCAN_STATUS_RESPONSE_REQINT_MASK        0x01
    #define Si2168_SCAN_STATUS_RESPONSE_REQINT_CHANGED    1
    #define Si2168_SCAN_STATUS_RESPONSE_REQINT_NO_CHANGE  0
   /* SCAN_STATUS command, MODULATION field definition (address 10, size 4, lsb 0, unsigned)*/
   #define  Si2168_SCAN_STATUS_RESPONSE_MODULATION_LSB         0
   #define  Si2168_SCAN_STATUS_RESPONSE_MODULATION_MASK        0x0f
    #define Si2168_SCAN_STATUS_RESPONSE_MODULATION_DSS     10
    #define Si2168_SCAN_STATUS_RESPONSE_MODULATION_DVBC    3
    #define Si2168_SCAN_STATUS_RESPONSE_MODULATION_DVBS    8
    #define Si2168_SCAN_STATUS_RESPONSE_MODULATION_DVBS2   9
    #define Si2168_SCAN_STATUS_RESPONSE_MODULATION_DVBT    2
    #define Si2168_SCAN_STATUS_RESPONSE_MODULATION_DVBT2   7
   /* SCAN_STATUS command, BUZ field definition (address 2, size 1, lsb 0, unsigned)*/
   #define  Si2168_SCAN_STATUS_RESPONSE_BUZ_LSB         0
   #define  Si2168_SCAN_STATUS_RESPONSE_BUZ_MASK        0x01
    #define Si2168_SCAN_STATUS_RESPONSE_BUZ_BUSY  1
    #define Si2168_SCAN_STATUS_RESPONSE_BUZ_CTS   0
   /* SCAN_STATUS command, REQ field definition (address 2, size 1, lsb 1, unsigned)*/
   #define  Si2168_SCAN_STATUS_RESPONSE_REQ_LSB         1
   #define  Si2168_SCAN_STATUS_RESPONSE_REQ_MASK        0x01
    #define Si2168_SCAN_STATUS_RESPONSE_REQ_NO_REQUEST  0
    #define Si2168_SCAN_STATUS_RESPONSE_REQ_REQUEST     1
   /* SCAN_STATUS command, SCAN_STATUS field definition (address 3, size 6, lsb 0, unsigned)*/
   #define  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_LSB         0
   #define  Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_MASK        0x3f
    #define Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_ANALOG_CHANNEL_FOUND   6
    #define Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_DEBUG                  63
    #define Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_DIGITAL_CHANNEL_FOUND  5
    #define Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_ENDED                  2
    #define Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_ERROR                  3
    #define Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_IDLE                   0
    #define Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_SEARCHING              1
    #define Si2168_SCAN_STATUS_RESPONSE_SCAN_STATUS_TUNE_REQUEST           4
   /* SCAN_STATUS command, RF_FREQ field definition (address 4, size 32, lsb 0, unsigned)*/
   #define  Si2168_SCAN_STATUS_RESPONSE_RF_FREQ_LSB         0
   #define  Si2168_SCAN_STATUS_RESPONSE_RF_FREQ_MASK        0xffffffff
   /* SCAN_STATUS command, SYMB_RATE field definition (address 8, size 16, lsb 0, unsigned)*/
   #define  Si2168_SCAN_STATUS_RESPONSE_SYMB_RATE_LSB         0
   #define  Si2168_SCAN_STATUS_RESPONSE_SYMB_RATE_MASK        0xffff

#endif /* Si2168_SCAN_STATUS_CMD */

/* Si2168_SET_PROPERTY command definition */
#define   Si2168_SET_PROPERTY_CMD 0x14

#ifdef    Si2168_SET_PROPERTY_CMD
  #define Si2168_SET_PROPERTY_CMD_CODE 0x010014

    typedef struct { /* Si2168_SET_PROPERTY_CMD_struct */
     unsigned char   reserved;
     unsigned int    prop;
     unsigned int    data;
   } Si2168_SET_PROPERTY_CMD_struct;


    typedef struct { /* Si2168_SET_PROPERTY_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
      unsigned char   reserved;
      unsigned int    data;
   }  Si2168_SET_PROPERTY_CMD_REPLY_struct;

   /* SET_PROPERTY command, RESERVED field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_SET_PROPERTY_CMD_RESERVED_LSB         0
   #define  Si2168_SET_PROPERTY_CMD_RESERVED_MASK        0xff
   #define  Si2168_SET_PROPERTY_CMD_RESERVED_MIN         0
   #define  Si2168_SET_PROPERTY_CMD_RESERVED_MAX         255.0
   /* SET_PROPERTY command, PROP field definition (address 2,size 16, lsb 0, unsigned) */
   #define  Si2168_SET_PROPERTY_CMD_PROP_LSB         0
   #define  Si2168_SET_PROPERTY_CMD_PROP_MASK        0xffff
   #define  Si2168_SET_PROPERTY_CMD_PROP_MIN         0
   #define  Si2168_SET_PROPERTY_CMD_PROP_MAX         65535
    #define Si2168_SET_PROPERTY_CMD_PROP_PROP_MIN  0
    #define Si2168_SET_PROPERTY_CMD_PROP_PROP_MAX  65535
   /* SET_PROPERTY command, DATA field definition (address 4,size 16, lsb 0, unsigned) */
   #define  Si2168_SET_PROPERTY_CMD_DATA_LSB         0
   #define  Si2168_SET_PROPERTY_CMD_DATA_MASK        0xffff
   #define  Si2168_SET_PROPERTY_CMD_DATA_MIN         0
   #define  Si2168_SET_PROPERTY_CMD_DATA_MAX         65535
    #define Si2168_SET_PROPERTY_CMD_DATA_DATA_MIN  0
    #define Si2168_SET_PROPERTY_CMD_DATA_DATA_MAX  65535
   /* SET_PROPERTY command, RESERVED field definition (address 1, size 8, lsb 0, unsigned)*/
   #define  Si2168_SET_PROPERTY_RESPONSE_RESERVED_LSB         0
   #define  Si2168_SET_PROPERTY_RESPONSE_RESERVED_MASK        0xff
    #define Si2168_SET_PROPERTY_RESPONSE_RESERVED_RESERVED_MIN  0
    #define Si2168_SET_PROPERTY_RESPONSE_RESERVED_RESERVED_MAX  0
   /* SET_PROPERTY command, DATA field definition (address 2, size 16, lsb 0, unsigned)*/
   #define  Si2168_SET_PROPERTY_RESPONSE_DATA_LSB         0
   #define  Si2168_SET_PROPERTY_RESPONSE_DATA_MASK        0xffff

#endif /* Si2168_SET_PROPERTY_CMD */

/* Si2168_START_CLK command definition */
#define   Si2168_START_CLK_CMD 0xc0

#ifdef    Si2168_START_CLK_CMD
  #define Si2168_START_CLK_CMD_CODE 0x0300c0

    typedef struct { /* Si2168_START_CLK_CMD_struct */
     unsigned char   subcode;
     unsigned char   reserved1;
     unsigned char   tune_cap;
     unsigned char   reserved2;
     unsigned int    clk_mode;
     unsigned char   reserved3;
     unsigned char   reserved4;
     unsigned char   start_clk;
   } Si2168_START_CLK_CMD_struct;


    typedef struct { /* Si2168_START_CLK_CMD_REPLY_struct */
       Si2168_COMMON_REPLY_struct * STATUS;
   }  Si2168_START_CLK_CMD_REPLY_struct;

   /* START_CLK command, SUBCODE field definition (address 1,size 8, lsb 0, unsigned) */
   #define  Si2168_START_CLK_CMD_SUBCODE_LSB         0
   #define  Si2168_START_CLK_CMD_SUBCODE_MASK        0xff
   #define  Si2168_START_CLK_CMD_SUBCODE_MIN         18
   #define  Si2168_START_CLK_CMD_SUBCODE_MAX         18
    #define Si2168_START_CLK_CMD_SUBCODE_CODE  18
   /* START_CLK command, RESERVED1 field definition (address 2,size 8, lsb 0, unsigned) */
   #define  Si2168_START_CLK_CMD_RESERVED1_LSB         0
   #define  Si2168_START_CLK_CMD_RESERVED1_MASK        0xff
   #define  Si2168_START_CLK_CMD_RESERVED1_MIN         0
   #define  Si2168_START_CLK_CMD_RESERVED1_MAX         0
    #define Si2168_START_CLK_CMD_RESERVED1_RESERVED  0
   /* START_CLK command, TUNE_CAP field definition (address 3,size 4, lsb 0, unsigned) */
   #define  Si2168_START_CLK_CMD_TUNE_CAP_LSB         0
   #define  Si2168_START_CLK_CMD_TUNE_CAP_MASK        0x0f
   #define  Si2168_START_CLK_CMD_TUNE_CAP_MIN         0
   #define  Si2168_START_CLK_CMD_TUNE_CAP_MAX         15
    #define Si2168_START_CLK_CMD_TUNE_CAP_10P4     8
    #define Si2168_START_CLK_CMD_TUNE_CAP_11P7     9
    #define Si2168_START_CLK_CMD_TUNE_CAP_13P0     10
    #define Si2168_START_CLK_CMD_TUNE_CAP_14P3     11
    #define Si2168_START_CLK_CMD_TUNE_CAP_15P6     12
    #define Si2168_START_CLK_CMD_TUNE_CAP_16P9     13
    #define Si2168_START_CLK_CMD_TUNE_CAP_18P2     14
    #define Si2168_START_CLK_CMD_TUNE_CAP_19P5     15
    #define Si2168_START_CLK_CMD_TUNE_CAP_1P3      1
    #define Si2168_START_CLK_CMD_TUNE_CAP_2P6      2
    #define Si2168_START_CLK_CMD_TUNE_CAP_3P9      3
    #define Si2168_START_CLK_CMD_TUNE_CAP_5P2      4
    #define Si2168_START_CLK_CMD_TUNE_CAP_6P5      5
    #define Si2168_START_CLK_CMD_TUNE_CAP_7P8      6
    #define Si2168_START_CLK_CMD_TUNE_CAP_9P1      7
    #define Si2168_START_CLK_CMD_TUNE_CAP_EXT_CLK  0
   /* START_CLK command, RESERVED2 field definition (address 3,size 4, lsb 4, unsigned) */
   #define  Si2168_START_CLK_CMD_RESERVED2_LSB         4
   #define  Si2168_START_CLK_CMD_RESERVED2_MASK        0x0f
   #define  Si2168_START_CLK_CMD_RESERVED2_MIN         0
   #define  Si2168_START_CLK_CMD_RESERVED2_MAX         0
    #define Si2168_START_CLK_CMD_RESERVED2_RESERVED  0
   /* START_CLK command, CLK_MODE field definition (address 4,size 12, lsb 0, unsigned) */
   #define  Si2168_START_CLK_CMD_CLK_MODE_LSB         0
   #define  Si2168_START_CLK_CMD_CLK_MODE_MASK        0xfff
   #define  Si2168_START_CLK_CMD_CLK_MODE_MIN         575
   #define  Si2168_START_CLK_CMD_CLK_MODE_MAX         3328
    #define Si2168_START_CLK_CMD_CLK_MODE_CLK_CLKIO    3328
    #define Si2168_START_CLK_CMD_CLK_MODE_CLK_XTAL_IN  1536
    #define Si2168_START_CLK_CMD_CLK_MODE_XTAL         575
   /* START_CLK command, RESERVED3 field definition (address 6,size 8, lsb 0, unsigned) */
   #define  Si2168_START_CLK_CMD_RESERVED3_LSB         0
   #define  Si2168_START_CLK_CMD_RESERVED3_MASK        0xff
   #define  Si2168_START_CLK_CMD_RESERVED3_MIN         22
   #define  Si2168_START_CLK_CMD_RESERVED3_MAX         22
    #define Si2168_START_CLK_CMD_RESERVED3_RESERVED  22
   /* START_CLK command, RESERVED4 field definition (address 7,size 1, lsb 0, unsigned) */
   #define  Si2168_START_CLK_CMD_RESERVED4_LSB         0
   #define  Si2168_START_CLK_CMD_RESERVED4_MASK        0x01
   #define  Si2168_START_CLK_CMD_RESERVED4_MIN         0
   #define  Si2168_START_CLK_CMD_RESERVED4_MAX         0
    #define Si2168_START_CLK_CMD_RESERVED4_RESERVED  0
   /* START_CLK command, START_CLK field definition (address 12,size 1, lsb 0, unsigned) */
   #define  Si2168_START_CLK_CMD_START_CLK_LSB         0
   #define  Si2168_START_CLK_CMD_START_CLK_MASK        0x01
   #define  Si2168_START_CLK_CMD_START_CLK_MIN         0
   #define  Si2168_START_CLK_CMD_START_CLK_MAX         0
    #define Si2168_START_CLK_CMD_START_CLK_START_CLK  0
#endif /* Si2168_START_CLK_CMD */

/* _commands_defines_insertion_point */

/* _commands_struct_insertion_start */

  /* --------------------------------------------*/
  /* COMMANDS STRUCT                             */
  /* This is used to store all command fields    */
  /* --------------------------------------------*/
  typedef struct { /* Si2168_CmdObj struct */
    #ifdef    Si2168_CONFIG_CLKIO_CMD
              Si2168_CONFIG_CLKIO_CMD_struct               config_clkio;
    #endif /* Si2168_CONFIG_CLKIO_CMD */
    #ifdef    Si2168_CONFIG_PINS_CMD
              Si2168_CONFIG_PINS_CMD_struct                config_pins;
    #endif /* Si2168_CONFIG_PINS_CMD */
    #ifdef    Si2168_DD_BER_CMD
              Si2168_DD_BER_CMD_struct                     dd_ber;
    #endif /* Si2168_DD_BER_CMD */
    #ifdef    Si2168_DD_CBER_CMD
              Si2168_DD_CBER_CMD_struct                    dd_cber;
    #endif /* Si2168_DD_CBER_CMD */

    #ifdef    Si2168_DD_EXT_AGC_TER_CMD
              Si2168_DD_EXT_AGC_TER_CMD_struct             dd_ext_agc_ter;
    #endif /* Si2168_DD_EXT_AGC_TER_CMD */

    #ifdef    Si2168_DD_FER_CMD
              Si2168_DD_FER_CMD_struct                     dd_fer;
    #endif /* Si2168_DD_FER_CMD */
    #ifdef    Si2168_DD_GET_REG_CMD
              Si2168_DD_GET_REG_CMD_struct                 dd_get_reg;
    #endif /* Si2168_DD_GET_REG_CMD */
    #ifdef    Si2168_DD_MP_DEFAULTS_CMD
              Si2168_DD_MP_DEFAULTS_CMD_struct             dd_mp_defaults;
    #endif /* Si2168_DD_MP_DEFAULTS_CMD */
    #ifdef    Si2168_DD_PER_CMD
              Si2168_DD_PER_CMD_struct                     dd_per;
    #endif /* Si2168_DD_PER_CMD */
    #ifdef    Si2168_DD_RESTART_CMD
              Si2168_DD_RESTART_CMD_struct                 dd_restart;
    #endif /* Si2168_DD_RESTART_CMD */
    #ifdef    Si2168_DD_SET_REG_CMD
              Si2168_DD_SET_REG_CMD_struct                 dd_set_reg;
    #endif /* Si2168_DD_SET_REG_CMD */
    #ifdef    Si2168_DD_SSI_SQI_CMD
              Si2168_DD_SSI_SQI_CMD_struct                 dd_ssi_sqi;
    #endif /* Si2168_DD_SSI_SQI_CMD */
    #ifdef    Si2168_DD_STATUS_CMD
              Si2168_DD_STATUS_CMD_struct                  dd_status;
    #endif /* Si2168_DD_STATUS_CMD */
    #ifdef    Si2168_DD_UNCOR_CMD
              Si2168_DD_UNCOR_CMD_struct                   dd_uncor;
    #endif /* Si2168_DD_UNCOR_CMD */
    #ifdef    Si2168_DOWNLOAD_DATASET_CONTINUE_CMD
              Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_struct  download_dataset_continue;
    #endif /* Si2168_DOWNLOAD_DATASET_CONTINUE_CMD */
    #ifdef    Si2168_DOWNLOAD_DATASET_START_CMD
              Si2168_DOWNLOAD_DATASET_START_CMD_struct     download_dataset_start;
    #endif /* Si2168_DOWNLOAD_DATASET_START_CMD */
    #ifdef    Si2168_DVBC_STATUS_CMD
              Si2168_DVBC_STATUS_CMD_struct                dvbc_status;
    #endif /* Si2168_DVBC_STATUS_CMD */


    #ifdef    Si2168_DVBT2_FEF_CMD
              Si2168_DVBT2_FEF_CMD_struct                  dvbt2_fef;
    #endif /* Si2168_DVBT2_FEF_CMD */
    #ifdef    Si2168_DVBT2_PLP_INFO_CMD
              Si2168_DVBT2_PLP_INFO_CMD_struct             dvbt2_plp_info;
    #endif /* Si2168_DVBT2_PLP_INFO_CMD */
    #ifdef    Si2168_DVBT2_PLP_SELECT_CMD
              Si2168_DVBT2_PLP_SELECT_CMD_struct           dvbt2_plp_select;
    #endif /* Si2168_DVBT2_PLP_SELECT_CMD */
    #ifdef    Si2168_DVBT2_STATUS_CMD
              Si2168_DVBT2_STATUS_CMD_struct               dvbt2_status;
    #endif /* Si2168_DVBT2_STATUS_CMD */
    #ifdef    Si2168_DVBT2_TX_ID_CMD
              Si2168_DVBT2_TX_ID_CMD_struct                dvbt2_tx_id;
    #endif /* Si2168_DVBT2_TX_ID_CMD */

    #ifdef    Si2168_DVBT_STATUS_CMD
              Si2168_DVBT_STATUS_CMD_struct                dvbt_status;
    #endif /* Si2168_DVBT_STATUS_CMD */
    #ifdef    Si2168_DVBT_TPS_EXTRA_CMD
              Si2168_DVBT_TPS_EXTRA_CMD_struct             dvbt_tps_extra;
    #endif /* Si2168_DVBT_TPS_EXTRA_CMD */

    #ifdef    Si2168_EXIT_BOOTLOADER_CMD
              Si2168_EXIT_BOOTLOADER_CMD_struct            exit_bootloader;
    #endif /* Si2168_EXIT_BOOTLOADER_CMD */
    #ifdef    Si2168_GET_PROPERTY_CMD
              Si2168_GET_PROPERTY_CMD_struct               get_property;
    #endif /* Si2168_GET_PROPERTY_CMD */
    #ifdef    Si2168_GET_REV_CMD
              Si2168_GET_REV_CMD_struct                    get_rev;
    #endif /* Si2168_GET_REV_CMD */
    #ifdef    Si2168_I2C_PASSTHROUGH_CMD
              Si2168_I2C_PASSTHROUGH_CMD_struct            i2c_passthrough;
    #endif /* Si2168_I2C_PASSTHROUGH_CMD */
    #ifdef    Si2168_PART_INFO_CMD
              Si2168_PART_INFO_CMD_struct                  part_info;
    #endif /* Si2168_PART_INFO_CMD */
    #ifdef    Si2168_POWER_DOWN_CMD
              Si2168_POWER_DOWN_CMD_struct                 power_down;
    #endif /* Si2168_POWER_DOWN_CMD */
    #ifdef    Si2168_POWER_UP_CMD
              Si2168_POWER_UP_CMD_struct                   power_up;
    #endif /* Si2168_POWER_UP_CMD */
    #ifdef    Si2168_RSSI_ADC_CMD
              Si2168_RSSI_ADC_CMD_struct                   rssi_adc;
    #endif /* Si2168_RSSI_ADC_CMD */
    #ifdef    Si2168_SCAN_CTRL_CMD
              Si2168_SCAN_CTRL_CMD_struct                  scan_ctrl;
    #endif /* Si2168_SCAN_CTRL_CMD */
    #ifdef    Si2168_SCAN_STATUS_CMD
              Si2168_SCAN_STATUS_CMD_struct                scan_status;
    #endif /* Si2168_SCAN_STATUS_CMD */
    #ifdef    Si2168_SET_PROPERTY_CMD
              Si2168_SET_PROPERTY_CMD_struct               set_property;
    #endif /* Si2168_SET_PROPERTY_CMD */
    #ifdef    Si2168_START_CLK_CMD
              Si2168_START_CLK_CMD_struct                  start_clk;
    #endif /* Si2168_START_CLK_CMD */
  } Si2168_CmdObj;
/* _commands_struct_insertion_point */

/* _commands_reply_struct_insertion_start */

  /* --------------------------------------------*/
  /* COMMANDS REPLY STRUCT                       */
  /* This stores all command reply fields        */
  /* --------------------------------------------*/
  typedef struct { /* Si2168_CmdReplyObj struct */
    #ifdef    Si2168_CONFIG_CLKIO_CMD
              Si2168_CONFIG_CLKIO_CMD_REPLY_struct               config_clkio;
    #endif /* Si2168_CONFIG_CLKIO_CMD */
    #ifdef    Si2168_CONFIG_PINS_CMD
              Si2168_CONFIG_PINS_CMD_REPLY_struct                config_pins;
    #endif /* Si2168_CONFIG_PINS_CMD */
    #ifdef    Si2168_DD_BER_CMD
              Si2168_DD_BER_CMD_REPLY_struct                     dd_ber;
    #endif /* Si2168_DD_BER_CMD */
    #ifdef    Si2168_DD_CBER_CMD
              Si2168_DD_CBER_CMD_REPLY_struct                    dd_cber;
    #endif /* Si2168_DD_CBER_CMD */

    #ifdef    Si2168_DD_EXT_AGC_TER_CMD
              Si2168_DD_EXT_AGC_TER_CMD_REPLY_struct             dd_ext_agc_ter;
    #endif /* Si2168_DD_EXT_AGC_TER_CMD */

    #ifdef    Si2168_DD_FER_CMD
              Si2168_DD_FER_CMD_REPLY_struct                     dd_fer;
    #endif /* Si2168_DD_FER_CMD */
    #ifdef    Si2168_DD_GET_REG_CMD
              Si2168_DD_GET_REG_CMD_REPLY_struct                 dd_get_reg;
    #endif /* Si2168_DD_GET_REG_CMD */
    #ifdef    Si2168_DD_MP_DEFAULTS_CMD
              Si2168_DD_MP_DEFAULTS_CMD_REPLY_struct             dd_mp_defaults;
    #endif /* Si2168_DD_MP_DEFAULTS_CMD */
    #ifdef    Si2168_DD_PER_CMD
              Si2168_DD_PER_CMD_REPLY_struct                     dd_per;
    #endif /* Si2168_DD_PER_CMD */
    #ifdef    Si2168_DD_RESTART_CMD
              Si2168_DD_RESTART_CMD_REPLY_struct                 dd_restart;
    #endif /* Si2168_DD_RESTART_CMD */
    #ifdef    Si2168_DD_SET_REG_CMD
              Si2168_DD_SET_REG_CMD_REPLY_struct                 dd_set_reg;
    #endif /* Si2168_DD_SET_REG_CMD */
    #ifdef    Si2168_DD_SSI_SQI_CMD
              Si2168_DD_SSI_SQI_CMD_REPLY_struct                 dd_ssi_sqi;
    #endif /* Si2168_DD_SSI_SQI_CMD */
    #ifdef    Si2168_DD_STATUS_CMD
              Si2168_DD_STATUS_CMD_REPLY_struct                  dd_status;
    #endif /* Si2168_DD_STATUS_CMD */
    #ifdef    Si2168_DD_UNCOR_CMD
              Si2168_DD_UNCOR_CMD_REPLY_struct                   dd_uncor;
    #endif /* Si2168_DD_UNCOR_CMD */
    #ifdef    Si2168_DOWNLOAD_DATASET_CONTINUE_CMD
              Si2168_DOWNLOAD_DATASET_CONTINUE_CMD_REPLY_struct  download_dataset_continue;
    #endif /* Si2168_DOWNLOAD_DATASET_CONTINUE_CMD */
    #ifdef    Si2168_DOWNLOAD_DATASET_START_CMD
              Si2168_DOWNLOAD_DATASET_START_CMD_REPLY_struct     download_dataset_start;
    #endif /* Si2168_DOWNLOAD_DATASET_START_CMD */
    #ifdef    Si2168_DVBC_STATUS_CMD
              Si2168_DVBC_STATUS_CMD_REPLY_struct                dvbc_status;
    #endif /* Si2168_DVBC_STATUS_CMD */


    #ifdef    Si2168_DVBT2_FEF_CMD
              Si2168_DVBT2_FEF_CMD_REPLY_struct                  dvbt2_fef;
    #endif /* Si2168_DVBT2_FEF_CMD */
    #ifdef    Si2168_DVBT2_PLP_INFO_CMD
              Si2168_DVBT2_PLP_INFO_CMD_REPLY_struct             dvbt2_plp_info;
    #endif /* Si2168_DVBT2_PLP_INFO_CMD */
    #ifdef    Si2168_DVBT2_PLP_SELECT_CMD
              Si2168_DVBT2_PLP_SELECT_CMD_REPLY_struct           dvbt2_plp_select;
    #endif /* Si2168_DVBT2_PLP_SELECT_CMD */
    #ifdef    Si2168_DVBT2_STATUS_CMD
              Si2168_DVBT2_STATUS_CMD_REPLY_struct               dvbt2_status;
    #endif /* Si2168_DVBT2_STATUS_CMD */
    #ifdef    Si2168_DVBT2_TX_ID_CMD
              Si2168_DVBT2_TX_ID_CMD_REPLY_struct                dvbt2_tx_id;
    #endif /* Si2168_DVBT2_TX_ID_CMD */

    #ifdef    Si2168_DVBT_STATUS_CMD
              Si2168_DVBT_STATUS_CMD_REPLY_struct                dvbt_status;
    #endif /* Si2168_DVBT_STATUS_CMD */
    #ifdef    Si2168_DVBT_TPS_EXTRA_CMD
              Si2168_DVBT_TPS_EXTRA_CMD_REPLY_struct             dvbt_tps_extra;
    #endif /* Si2168_DVBT_TPS_EXTRA_CMD */

    #ifdef    Si2168_EXIT_BOOTLOADER_CMD
              Si2168_EXIT_BOOTLOADER_CMD_REPLY_struct            exit_bootloader;
    #endif /* Si2168_EXIT_BOOTLOADER_CMD */
    #ifdef    Si2168_GET_PROPERTY_CMD
              Si2168_GET_PROPERTY_CMD_REPLY_struct               get_property;
    #endif /* Si2168_GET_PROPERTY_CMD */
    #ifdef    Si2168_GET_REV_CMD
              Si2168_GET_REV_CMD_REPLY_struct                    get_rev;
    #endif /* Si2168_GET_REV_CMD */
    #ifdef    Si2168_I2C_PASSTHROUGH_CMD
              Si2168_I2C_PASSTHROUGH_CMD_REPLY_struct            i2c_passthrough;
    #endif /* Si2168_I2C_PASSTHROUGH_CMD */
    #ifdef    Si2168_PART_INFO_CMD
              Si2168_PART_INFO_CMD_REPLY_struct                  part_info;
    #endif /* Si2168_PART_INFO_CMD */
    #ifdef    Si2168_POWER_DOWN_CMD
              Si2168_POWER_DOWN_CMD_REPLY_struct                 power_down;
    #endif /* Si2168_POWER_DOWN_CMD */
    #ifdef    Si2168_POWER_UP_CMD
              Si2168_POWER_UP_CMD_REPLY_struct                   power_up;
    #endif /* Si2168_POWER_UP_CMD */
    #ifdef    Si2168_RSSI_ADC_CMD
              Si2168_RSSI_ADC_CMD_REPLY_struct                   rssi_adc;
    #endif /* Si2168_RSSI_ADC_CMD */
    #ifdef    Si2168_SCAN_CTRL_CMD
              Si2168_SCAN_CTRL_CMD_REPLY_struct                  scan_ctrl;
    #endif /* Si2168_SCAN_CTRL_CMD */
    #ifdef    Si2168_SCAN_STATUS_CMD
              Si2168_SCAN_STATUS_CMD_REPLY_struct                scan_status;
    #endif /* Si2168_SCAN_STATUS_CMD */
    #ifdef    Si2168_SET_PROPERTY_CMD
              Si2168_SET_PROPERTY_CMD_REPLY_struct               set_property;
    #endif /* Si2168_SET_PROPERTY_CMD */
    #ifdef    Si2168_START_CLK_CMD
              Si2168_START_CLK_CMD_REPLY_struct                  start_clk;
    #endif /* Si2168_START_CLK_CMD */
  } Si2168_CmdReplyObj;
/* _commands_reply_struct_insertion_point */

#ifdef    Si2168_COMMAND_PROTOTYPES
#define   Si2168_GET_COMMAND_STRINGS
#endif /* Si2168_COMMAND_PROTOTYPES */

#endif /* Si2168_COMMANDS_H */





