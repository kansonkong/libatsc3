#ifndef __SMSAPI_STRUCTS_H__
#define __SMSAPI_STRUCTS_H__

#include "smsapi_platdefs.h"
#include "smsapi_const.h"

#include "IT9300.h"


//#include "osw.h"

/***************************************************************/
/* Struct - SmsApiHandle                                       */
/***************************************************************/
typedef struct SmsApi_Handle_S
{
	unsigned char numSmsDev;
    // ADR
//    Event               adrEvent;
//    Event               reponseEvent;
	unsigned int        reponseType;
	unsigned char		pReponseData[2048];
    
    // Statistics
	unsigned char       IsRfLocked;
	unsigned char       IsDemodLocked;
	int					InBandPwr;
	int					SNR;
	int					RSSI;
	int					CarrierOffset;
	unsigned int        BER;
	unsigned int        TotalTsPackets;
	unsigned int        ErrorTsPackets;    
	unsigned int        Frequency;
	unsigned int        Bandwidth;
	unsigned int        ReceptionQuality;

	bool 				isDvbT2Mode;

//#ifdef SMS_DEMOD_DVBT2
	unsigned int		numOfPlps;
	unsigned int        plpId[8];
//#endif

} SmsApi_Handle_ST;

/***************************************************************/
/* Struct - SmsMsg                                             */
/***************************************************************/
typedef struct SmsMsgHdr_S
{
	unsigned short		msgType;
	unsigned char       msgSrcId;
	unsigned char       msgDstId;
	unsigned short      msgLength;  // Length is of the entire message, including header
	unsigned short      msgFlags;
} SmsMsgHdr_ST;

typedef struct SmsMsgData_S
{
    SmsMsgHdr_ST        xMsgHeader;
	unsigned int        msgData[1];
} SmsMsgData_ST;

typedef struct SmsMsgData2Args_S
{
    SmsMsgHdr_ST        xMsgHeader;
	unsigned int        msgData[2];
} SmsMsgData2Args_ST;

typedef struct SmsMsgData3Args_S
{
    SmsMsgHdr_ST        xMsgHeader;
	unsigned int        msgData[3];
} SmsMsgData3Args_ST;

typedef struct SmsMsgData5Args_S
{
    SmsMsgHdr_ST		xMsgHeader;
	unsigned int		msgData[5];
} SmsMsgData5Args_ST;

typedef struct SmsIntLine_S
{
    SmsMsgHdr_ST		xMsgHeader;
	unsigned int		Controler;
	unsigned int        GpioNum;
	unsigned int	    PulseWidth;
} SmsIntLine_ST;

typedef struct SmsDataDownload_S
{
    SmsMsgHdr_ST		xMsgHeader;
	unsigned int		MemAddr;
	unsigned int		Payload[SMS_MAX_PAYLOAD_SIZE / 4];
} SmsDataDownload_ST;

////////////////////////////////////////////////
/// Transprt Stream
typedef struct SMSHOSTLIB_VERSIONING_S
{
	unsigned char		Major;
	unsigned char		Minor;
	unsigned char		Patch;
	unsigned char		FieldPatch;
} SMSHOSTLIB_VERSIONING_ST;

/// Version
typedef struct SMSHOSTLIB_VERSION_S
{
	unsigned short              ChipModel;              //!< e.g. 0x1102 for SMS-1102 "Nova"
	unsigned char               Step;                   //!< 0 - Step A
	unsigned char               MetalFix;               //!< 0 - Metal 0
	unsigned char               FirmwareId;             //!< 0xFF - ROM or see @SMSHOSTLIB_DEVICE_MODES_E
	unsigned char               SupportedProtocols;     /*!< Bitwise OR combination of supported
                                                                protocols, see @SMSHOSTLIB_DEVICE_MODES_E */
    SMSHOSTLIB_VERSIONING_ST    FwVer;                  //!< Firmware version
    SMSHOSTLIB_VERSIONING_ST    RomVer;                 //!< ROM version
	unsigned char               TextLabel[34];          //!< Text label
    SMSHOSTLIB_VERSIONING_ST    RFVer;                  //!< RF tuner version
	unsigned int                PkgVer;                 //!< SMS11xx Package Version
	unsigned int                Reserved[9];            //!< Reserved for future use
} SMSHOSTLIB_VERSION_ST;

/////////////////
/// Version
typedef struct _VERSION_MSG_S
{
    SmsMsgHdr_ST			xMsgHeader;
    SMSHOSTLIB_VERSION_ST	Ver;
} VERSION_MSG_ST;

////////////////////////////////////////////////
/// Transprt Stream
typedef struct SmsMsgTsEnable_S
{
    SmsMsgHdr_ST		xMsgHeader;
	unsigned int		TsClock;                // 0 - TS Clock Speed in Hz
	unsigned int		eTsiMode;               // 1 - TS Mode of operation Serial (on SDIO or HIF Pins), or Parallel
	unsigned int	    eTsiSignals;            // 2 - Level of Valid, Sync and Error signals when active
	unsigned int	    nTsiPcktDelay;          // 3 - number of delay bytes between TS packets (for 204bytes mode set to 16)
	unsigned int	    eTsiClkSensePolar;      // 4 - Clock edge to sample data
	unsigned int	    TsBitOrder;             // 5 - Bit order in TS output
	unsigned int		EnableControlOverTs;    // 6 - Enable Control messages over TS interface
	unsigned int		TsiEncapsulationFormat; // 7 - TS encapsulation method
	unsigned int		TsiPaddingPackets;      // 8 - Number of TS padding packets appended to control messages
	unsigned int	    eTsiElectrical;         // 9 - Set slew rate
	unsigned int		IoVoltage;              // 10 - Set IO voltage
	unsigned int		eTsiErrActive;          // 11 - Set ErrActive status
	unsigned int		eTsiClkKeepGo;          // 12 - Set TS clock keep go with no packet or not
} SmsMsgTsEnable_ST;


//! DVBT Statistics
typedef struct TRANSMISSION_STATISTICS_S
{
	unsigned int	 Frequency;               //!< Frequency in Hz
	unsigned int	Bandwidth;               //!< Bandwidth in MHz
	unsigned int	TransmissionMode;        //!< FFT mode carriers in Kilos
	unsigned int	GuardInterval;           //!< Guard Interval from SMSHOSTLIB_GUARD_INTERVALS_ET
	unsigned int	CodeRate;                //!< Code Rate from SMSHOSTLIB_CODE_RATE_ET
	unsigned int	LPCodeRate;              //!< Low Priority Code Rate from SMSHOSTLIB_CODE_RATE_ET
	unsigned int	Hierarchy;               //!< Hierarchy from SMSHOSTLIB_HIERARCHY_ET
	unsigned int	Constellation;           //!< Constellation from SMSHOSTLIB_CONSTELLATION_ET

    // DVB-H TPS parameters
	unsigned int	CellId;                  //!< TPS Cell ID in bits 15..0, bits 31..16 zero; if set to 0xFFFFFFFF cell_id not yet recovered
	unsigned int	DvbhSrvIndHP;            //!< DVB-H service indication info, bit 1 - Time Slicing indicator, bit 0 - MPE-FEC indicator
	unsigned int	DvbhSrvIndLP;            //!< DVB-H service indication info, bit 1 - Time Slicing indicator, bit 0 - MPE-FEC indicator
	unsigned int	IsDemodLocked;           //!< 0 - not locked, 1 - locked

}TRANSMISSION_STATISTICS_ST;

typedef struct RECEPTION_STATISTICS_S
{
	unsigned int	IsRfLocked;              //!< 0 - not locked, 1 - locked
	unsigned int	IsDemodLocked;           //!< 0 - not locked, 1 - locked
	unsigned int	IsExternalLNAOn;         //!< 0 - external LNA off, 1 - external LNA on
	unsigned int	ModemState;              //!< from SMSHOSTLIB_DVB_MODEM_STATE_ET
    int				SNR;                     //!< dB
	unsigned int	BER;                     //!< Post Viterbi BER [1E-5]
	unsigned int	BERErrorCount;           //!< Number of erroneous SYNC bits.
	unsigned int	BERBitCount;             //!< Total number of SYNC bits.
	unsigned int	TS_PER;                  //!< Transport stream PER, 0xFFFFFFFF indicate N/A
	unsigned int	MFER;                    //!< DVB-H frame error rate in percentage, 0xFFFFFFFF indicate N/A, valid only for DVB-H
    int				RSSI;                    //!< dBm
	int				InBandPwr;               //!< In band power in dBM
	int				CarrierOffset;           //!< Carrier Offset in bin/1024
	unsigned int	ErrorTSPackets;          //!< Number of erroneous transport-stream packets
	unsigned int	TotalTSPackets;          //!< Total number of transport-stream packets
	int				RefDevPPM;
	int				FreqDevHz;
	int				MRC_SNR;                 //!< dB, in Non MRC application: buffer_overflow flag. Should be 0
	int				MRC_RSSI;                //!< dBm, 
	int				MRC_InBandPwr;           //!< In band power in dBM, in Non MRC application: dvbt buffer max count. Should be less than 345*188
	unsigned int	ErrorTSPacketsAfterReset; //!< Number of erroneous transport-stream packets from the last reset
	unsigned int	TotalTSPacketsAfterReset; //!< Total number of transport-stream packets from the last reset

}RECEPTION_STATISTICS_ST;

typedef struct SMSHOSTLIB_STATISTICS_DVBT_S
{
    // Reception
    RECEPTION_STATISTICS_ST ReceptionData;

    // Transmission parameters
    TRANSMISSION_STATISTICS_ST TransmissionData;

	unsigned int ReceptionQuality;
} SMSHOSTLIB_STATISTICS_DVBT_ST;

//! DVBT2 Statistics

#define DVBT2_MAX_PLPS_LITE                                             (8)
#define DVBT2_ACTIVE_PLPS_LITE                                      (2)

typedef struct TRANSMISSION_STATISTICS_DVBT2_S
{
	unsigned int Frequency;             //!< Frequency in Hz
	unsigned int Bandwidth;             //!< Bandwidth in MHz
	unsigned int res[3];
}TRANSMISSION_STATISTICS_DVBT2_ST;

typedef struct RECEPTION_STATISTICS_DVBT2_S
{
	unsigned int	IsModemLocked;              //!< 0 - not locked, 1 - locked
    TRANSMISSION_STATISTICS_DVBT2_ST  txStatistics;
    int				carrierOffset;
	int				inbandPower;
	unsigned int	extLna;
	unsigned int	totalFrames;
	int				SNR;                     //!< dB     
	int				RSSI;                    //!< dBm         
	unsigned int	FER;
	unsigned int	CellId;
	unsigned int	netId;
	unsigned int	receptionQuality;
	unsigned int	bwt_ext;
	unsigned int	fftMode;
	unsigned int	guardInterval;
	unsigned int	pilotPattern;
	unsigned int	bitRate;
	unsigned int	extended;
	unsigned int	toneReservation;
	unsigned int	l1PostSize;
	unsigned int	numOfAuxs;
	unsigned int	numOfPlps;
	unsigned int	liteMode;

    int				MRC_SNR;                 // !< dB
	unsigned int	SNRFullRes;                // !< dB x 65536    
	int				MRC_InBandPwr;           // !< In band power in dBM
	int				MRC_Rssi;
	unsigned char	commonPlpNotSupported;
	unsigned char	l1modulation;
	unsigned short	numdatasymbols;
	unsigned int	res[2];
} RECEPTION_STATISTICS_DVBT2_ST;

typedef struct DVBT2_GENERAL_INFO_S
{
	unsigned int smoothing;
	unsigned int res[3];
} DVBT2_GENERAL_INFO_ST;

typedef struct DVBT2_PLP_STATISTICS_DATA_S
{
	unsigned int plpId;
	unsigned int plpType;
	unsigned int plpPayloadType;
	unsigned int ffFlag;
	unsigned int firstRfIdx;
	unsigned int firstFrameIdx;
	unsigned int plpGroupId;
	unsigned int plpCod;
	unsigned int plpMod;
	unsigned int plpRotation;
	unsigned int plpFecType;
	unsigned int plpNumBlocksMax;
	unsigned int frameInterval;
	unsigned int timeIlLength;
	unsigned int timeIlType;
	unsigned int inbandA_Flag;
	unsigned int inbandB_Flag;
	unsigned int plpMode;
	unsigned int staticFlag;
	unsigned int staticPaddingFlag;
	unsigned int res[3];
} DVBT2_PLP_STATISTICS_DATA_ST;

typedef struct PLP_DATA_S
{
    DVBT2_PLP_STATISTICS_DATA_ST plpStatistics;
} PLP_DATA_ST;

typedef struct
{
	unsigned int plpId;
	unsigned int plpType;
	unsigned int plpEfficiencyMode;
	unsigned int dnp;
	unsigned int issyi;
	unsigned int crcErrors;
	unsigned int numOfLdpcIters;
	unsigned int totalNumBBFramesReceived;  // Total number of BB frames received.
	unsigned int totalNumErrBBFramesReceived; // Total number of error BB frames received.
	unsigned int totalNumTsPktsReceived;  // Total number of TS packets received.
	unsigned int totalNumTsPktsTransmitted;  // Total number of TS packets transmitted to the TSI.
	unsigned int totalNumErrTsPktsReceived;  // Total number of error TS packets received.
	unsigned int numOfOverflow;
	unsigned int numOfUnderflow;
	unsigned int dejitterBufferSize;
	unsigned int totalNumOfPktsInserted;
	unsigned int totalNumTsPktsForwarded;
	unsigned int totalPostLdpcErr;
	unsigned int numTsPktsReceivedAfterReset;           //!< Total number of transport-stream packets from the last reset
	unsigned int numErrTsPktsReceivedAfterReset;        //!< Number of erroneous transport-stream packets from the last reset
	unsigned int res[1];
} ACTIVE_PLP_STATISTICS_ST;

// Statistics information returned as response for SmsLiteMsGetStatistics_Req for DVB applications, SMS1100 and up
typedef struct SMSHOSTLIB_STATISTICS_DVBT2_S
{
    // Reception
    RECEPTION_STATISTICS_DVBT2_ST ReceptionData;
    // Transmission parameters
    TRANSMISSION_STATISTICS_DVBT2_ST TransmissionData;
    DVBT2_GENERAL_INFO_ST generalInfo;
    // Burst parameters, valid only for DVBT2
    PLP_DATA_ST PlpData[DVBT2_MAX_PLPS_LITE];
    ACTIVE_PLP_STATISTICS_ST activePlps[DVBT2_ACTIVE_PLPS_LITE];
} SMSHOSTLIB_STATISTICS_DVBT2_ST;

#endif





