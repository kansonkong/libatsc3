#ifndef __SMSAPI_CONST_H__
#define __SMSAPI_CONST_H__

/*************************************************************************
* Defines
*************************************************************************/
#define SMS_IIC_RX_BUFFER_SIZE      256*5             // T2 stats message size is 1,088 bytes
#define SMS_MAX_PAYLOAD_SIZE        240
#define MAX_CHUNK_SIZE              (10*1024)
#define DVBT_USER_CRISTAL           12000000L 

//******************************
// Log messages definitions
// Definitions for the flags field in the message header
// bits 13,14,15 of msgFlags allocated to SmsCommIntf
// The definition is on a per-bit basis
#define MSG_HDR_DEFAULT_DYNAMIC_MSG 0x0000  // Message is dynamic
#define MSG_HDR_FLAG_STATIC_MSG     0x0001  // Message is dynamic when this bit is '0'
#define MSG_HDR_FLAG_RX_DIR         0x0002  // Direction is RX when this bit is '1'
#define MSG_HDR_FLAG_SPLIT_MSG_HDR  0x0004  // Message format is SmsMessage_ST, with pMsg pointing to remainder of message
#define MSG_HDR_FLAG_TS_PAYLOAD     0x0008  // Message payload is in the form of TS packets
#define MSG_HDR_FLAG_ALIGN_MASK     0x0300  // Two bits denoting number of padding bytes inserted at the
// start of the data in split messages. Used for alignment
#define MSG_HDR_FLAG_EXT_LEN_HDR    0xF000  // Extended msg len (MS nibble).


/*************************************************************************
SMS Host Library IDs
*************************************************************************/
#define SMS_HOST_ID_BASE            100
#define SMS_HOST_LIB                (SMS_HOST_ID_BASE + 50)
#define SMS_HOST_LIB_INTERNAL       (SMS_HOST_ID_BASE + 51)
#define SMS_HOST_LIB_INTERNAL2      (SMS_HOST_ID_BASE + 52)
#define SMS_HOST_LIB_ADR            (SMS_HOST_ID_BASE + 60)

#define HIF_TASK                    11      // Firmware messages processor task IS
#define HIF_TASK_SLAVE              22
#define HIF_TASK_SLAVE2             33
#define HIF_TASK_SLAVE3             44


/*************************************************************************
* Enums
*************************************************************************/
typedef enum
{
    SMSAPI_ERR_OK = 0,
    SMSAPI_ERR_ERROR,
    SMSAPI_ERR_TIMEOUT,
} SMSAPI_ERR_CODES_E;

typedef enum
{
    SMSHOSTLIB_DEVMD_DVBT,  
    SMSHOSTLIB_DEVMD_DVBH,
    SMSHOSTLIB_DEVMD_DAB_TDMB,
    SMSHOSTLIB_DEVMD_DAB_TDMB_DABIP,
    SMSHOSTLIB_DEVMD_DVBT_BDA,
    SMSHOSTLIB_DEVMD_ISDBT,
    SMSHOSTLIB_DEVMD_ISDBT_BDA,
    SMSHOSTLIB_DEVMD_CMMB,
    SMSHOSTLIB_DEVMD_RAW_TUNER,
    SMSHOSTLIB_DEVMD_FM_RADIO,
    SMSHOSTLIB_DEVMD_FM_RADIO_BDA,
    SMSHOSTLIB_DEVMD_ATSC,
    SMSHOSTLIB_DEVMD_ATV,
    SMSHOSTLIB_DEVMD_DVBT2,
    SMSHOSTLIB_DEVMD_DRM,
    SMSHOSTLIB_DEVMD_DVBT2_BDA,
    SMSHOSTLIB_DEVMD_MAX,
    SMSHOSTLIB_DEVMD_NONE = -1
} SMSHOSTLIB_DEVICE_MODES_E;

typedef enum
{
    BW_8_MHZ        = 0,
    BW_7_MHZ        = 1,
    BW_6_MHZ        = 2,
    BW_5_MHZ        = 3,
    BW_ISDBT_1SEG   = 4,
    BW_ISDBT_3SEG   = 5,
    BW_2_MHZ        = 6,
    BW_FM_RADIO     = 7,
    BW_1_5_MHZ      = 15,
    BW_UNKNOWN      = 0xFFFF
} SMSHOSTLIB_FREQ_BANDWIDTH_E;

/* TS configuration parameters */
typedef enum SmsTsiMode_E
{
    TSI_SERIAL_MAIN,            //  TSI_SERIAL_ON_SDIO
    TSI_SERIAL_SECONDARY,       //  TSI_SERIAL_ON_HIF
    TSI_PARALLEL_MAIN,          //  TSI_PARALLEL_ON_HIF
    TSI_PARALLEL_SECONDARY,
    TSI_SERIAL_MAIN_WITH_SYNC_EXTEND = 16,
    TSI_MAX_MODE
} SmsTsiMode_ET;

typedef enum SmsTsiFormat_E
{
    TSI_TRANSPARENT,
    TSI_ENCAPSULATED,
    TSI_MAX_FORMAT
} SmsTsiFormat_ET;

typedef enum SmsTsiErrActive_E
{
    TSI_ERR_NOT_ACTIVE,
    TSI_ERR_ACTIVE,
    TSI_MAX_ERR_ACTIVE
} SmsTsiErrActive_ET;

typedef enum SmsTsiSigActive_E
{
    TSI_SIGNALS_ACTIVE_LOW,
    TSI_SIGNALS_ACTIVE_HIGH,
    TSI_MAX_SIG_ACTIVE
} SmsTsiSigActive_ET;

typedef enum SmsTsiClockKeepGo_E
{
    TSI_CLK_STAY_LOW_GO_NO_PKT,
    TSI_CLK_KEEP_GO_NO_PKT,
    TSI_MAX_CLK_ON
} SmsTsiClockKeepGo_ET;

typedef enum SmsTsiSensPolar_E
{
    TSI_SIG_OUT_FALL_EDGE,
    TSI_SIG_OUT_RISE_EDGE,
    TSI_MAX_CLK_POLAR
}SmsTsiSensPolar_ET;

typedef enum SmsTsiBitOrder_E
{
    TSI_BIT7_IS_MSB,
    TSI_BIT0_IS_MSB,
    TSI_MAX_BIT_ORDER
} SmsTsiBitOrder_ET;

typedef enum SmsTsiElectrical_E
{
    TSI_ELEC_LOW,       // slew rate 0.45 V/ns, drive 2.8 mA 
    TSI_ELEC_NORMAL,    // slew rate 1.7 V/ns, drive 7 mA
    TSI_ELEC_HIGH       // slew rate 3.3 V/ns, drive 10 mA
} SmsTsiElectrical_ET;

typedef enum IoVoltage_E
{
    IOC_VOLTAGE_0,
    IOC_VOLTAGE_1_8,
    IOC_VOLTAGE_3_3
} IoVoltage_ET;

// Definitions of the message types.
// For each type, the format used (excluding the header) is specified
// The message direction is also specified
typedef enum MsgTypes_E
{
    MSG_TYPE_BASE_VAL               = 500,
    MSG_SMS_RF_TUNE_REQ             = 561,      // Application: CMMB, DVBT/H
                                                // A request to tune to a new frequency
                                                // Format:  32-bit - Frequency in Hz
                                                //          32-bit - Bandwidth (in CMMB always use BW_8_MHZ)
                                                //          32-bit - Crystal (Use 0 for default, always 0 in CMMB)
                                                // Direction: Host->SMS
    MSG_SMS_RF_TUNE_RES             = 562,      // Application: CMMB, DVBT/H
                                                // A response to MSG_SMS_RF_TUNE_REQ
                                                // In DVBT/H this only indicates that the tune request
                                                // was received.
                                                // In CMMB, the response returns after the demod has determined
                                                // if there is a valid CMMB transmission on the frequency
                                                //
                                                // Format:
                                                //  DVBT/H:
                                                //      32-bit Return status. Should be SMSHOSTLIB_ERR_OK.
                                                //  CMMB:
                                                //      32-bit CMMB signal status - SMSHOSTLIB_ERR_OK means that the
                                                //                  frequency has a valid CMMB signal
                                                //
                                                // Direction: SMS->Host
    MSG_SMS_INIT_DEVICE_REQ         = 578,      // A request to init device
                                                // Format: 32-bit - device mode (DVBT,DVBH,TDMB,DAB, DRM)
                                                //         32-bit - Crystal
                                                //         32-bit - Clk Division
                                                //         32-bit - Ref Division
                                                // Direction: Host->SMS
    MSG_SMS_INIT_DEVICE_RES         = 579,      // The response to MSG_SMS_INIT_DEVICE_REQ
                                                // Format:  32-bit - status
    MSG_SMS_ADD_PID_FILTER_REQ      = 601,      // Application: DVB-T/DVB-H
                                                // Add PID to filter list
                                                // Format: 32-bit PID
                                                // Direction: Host->SMS
    MSG_SMS_ADD_PID_FILTER_RES      = 602,      // Application: DVB-T/DVB-H
                                                // The response to MSG_SMS_ADD_PID_FILTER_REQ
                                                // Format:  32-bit - Status
                                                // Direction: SMS->Host
    MSG_SMS_REMOVE_PID_FILTER_REQ   = 603,      // Application: DVB-T/DVB-H
                                                // Remove PID from filter list
                                                // Format: 32-bit PID
                                                // Direction: Host->SMS
    MSG_SMS_REMOVE_PID_FILTER_RES   = 604,      // Application: DVB-T/DVB-H
                                                // The response to MSG_SMS_REMOVE_PID_FILTER_REQ
                                                // Format:  32-bit - Status
                                                // Direction: SMS->Host
    MSG_SMS_HO_PER_SLICES_IND       = 630,      // Application: DVB-H 
                                                // Direction: FW-->Host
    MSG_SMS_GET_STATISTICS_EX_REQ   = 653,      // Application: ISDBT / FM
                                                // Request for statistics 
                                                // Direction: Host-->FW
    MSG_SMS_GET_STATISTICS_EX_RES   = 654,      // Application: ISDBT / FM
                                                // Format:
                                                // 32 bit ErrCode
                                                // The rest: A mode-specific statistics struct starting
                                                // with a 32 bits type field.
                                                // Direction: FW-->Host
    MSG_SMS_DATA_DOWNLOAD_REQ       = 660,      // Application: All
                                                // Direction: Host-->FW
    MSG_SMS_DATA_DOWNLOAD_RES       = 661,      // Application: All
                                                // Direction: FW-->Host
    MSG_SMS_DATA_VALIDITY_REQ       = 662,      // Application: All
                                                // Direction: Host-->FW
    MSG_SMS_DATA_VALIDITY_RES       = 663,      // Application: All
                                                // Direction: FW-->Host
    MSG_SMS_SWDOWNLOAD_TRIGGER_REQ  = 664,      // Application: All
                                                // Direction: Host-->FW
    MSG_SMS_SWDOWNLOAD_TRIGGER_RES  = 665,      // Application: All
                                                // Direction: FW-->Host
    MSG_SMS_SWDOWNLOAD_BACKDOOR_REQ = 666,      // Application: All
                                                // Direction: Host-->FW
    MSG_SMS_SWDOWNLOAD_BACKDOOR_RES = 667,      // Application: All
                                                // Direction: FW-->Host
    MSG_SMS_GET_VERSION_EX_REQ      = 668,      // Application: All Except CMMB
                                                // Direction: Host-->FW
    MSG_SMS_GET_VERSION_EX_RES      = 669,      // Application: All Except CMMB
                                                // Direction: FW-->Host
    MSG_SMS_SPI_INT_LINE_SET_REQ    = 710,      //
    MSG_SMS_SPI_INT_LINE_SET_RES    = 711,      //
    MSG_SMS_ENBALE_TS_INTERFACE_REQ = 736,      // A request set TS interface as the DATA(!) output interface
                                                // Format:  32-bit - Requested Clock speed in Hz(0-disable)
                                                //          32-bit - transmission mode (Serial or Parallel)
                                                // Direction: Host->SMS
    MSG_SMS_ENBALE_TS_INTERFACE_RES = 737,      //
    MSG_SMS_I2C_SHORT_STAT_IND      = 798,      // Application Type: DVB-T/ISDB-T 
                                                // Format: ShortStatMsg_ST
                                                //      Data[0] = uint16_t msgType
                                                //      Data[1] = uint8_t msgSrcId
                                                //      Data[2] = uint8_t msgDstId
                                                //      Data[3] = uint16_t    msgLength   
                                                //      Data[4] = uint16_t    msgFlags
                                                //  The following parameters relevant in DVB-T only - in isdb-t should be Zero
                                                //      Data[5] = uint32_t IsDemodLocked;
                                                //      Data[6] = uint32_t InBandPwr;
                                                //      Data[7] = uint32_t BER;
                                                //      Data[8] = uint32_t SNR;
                                                //      Data[9] = uint32_t TotalTsPackets;
                                                //      Data[10]= uint32_t ErrorTSPackets;
                                                // Direction: FW-->Host
    MSG_SMS_SLAVE_DEVICE_DETECTED   = 804,      // Application: DVB-T MRC
                                                // Description: FW indicate that Slave exist in MRC - DVB-T application
                                                // Direction: FW->Host
    MSG_LAST_MSG_TYPE               = 1000      // Note: Stellar ROM limits this number to 700, other chip sets to 900, Siena sets to 1000
} MsgTypes_ET;

#endif // __SMSAPI_CONST_H__
