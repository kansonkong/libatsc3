/*
	at3_atscstd.h

	for Project Atlas

	Define common ATSC/ATSC3 structs used in AT3DRV driver.

	Copyright © 2018, 2019 LowaSIS, Inc.
*/

#ifndef __AT3_ATSC_STD_H__
#define __AT3_ATSC_STD_H__

//============================================================================

#include "at3_common.h"

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//============================================================================
// ATSC3

//------------------------------------
// A/322 Physical Layer

// They have same numeric value as L1D_plp_fec_type.
typedef enum {
	eLPFT_BchLdpc16K = 0,
	eLPFT_BchLdpc64K = 1,
	eLPFT_CrcLdpc16K = 2,
	eLPFT_CrcLdpc64K = 3,
	eLPFT_Ldpc16K = 4,
	eLPFT_Ldpc64K = 5,
	eLPFT_Max,
	eLPFT_Unknown = 0xff,
} E_L1d_PlpFecType;

// same as L1D_plp_mod
typedef enum {
	eLPM_Qpsk = 0,
	eLPM_16Qam = 1,
	eLPM_64Qam = 2,
	eLPM_256Qam = 3,
	eLPM_1024Qam = 4,
	eLPM_4096Qam = 5,
	eLPM_Max,
	eLPM_Unknown = 0xff,
} E_L1d_PlpMod;

// same as L1D_plp_cod
typedef enum {
	eLPC_2_15 = 0,
	eLPC_3_15 = 1,
	eLPC_4_15 = 2,
	eLPC_5_15 = 3,
	eLPC_6_15 = 4,
	eLPC_7_15 = 5,
	eLPC_8_15 = 6,
	eLPC_9_15 = 7,
	eLPC_10_15 = 8,
	eLPC_11_15 = 9,
	eLPC_12_15 = 10,
	eLPC_13_15 = 11,
	eLPC_Max,
	eLPC_Unknown = 0xff,
} E_L1d_PlpCod;

AT3STDAPI const char *AT3_L1dPlpFecTypeString(E_L1d_PlpFecType t);
AT3STDAPI const char *AT3_L1dPlpModString(E_L1d_PlpMod m);
AT3STDAPI const char *AT3_L1dPlpCodString(E_L1d_PlpCod c);

/*	S_AT3_L1TIME

	L1dTime info.
*/
typedef struct 
{
	uint8_t flag; // same meaning as L1B_time_info_flag
		// 0: below time info is not valid.
		// 1/2/3: time info is valid, with prevision of ms/us/ns.
	uint32_t sec; // same meaning as L1D_time_sec
#if 1
	uint16_t msec;
	uint16_t usec;
	uint16_t nsec;
#else
	uint32_t msec: 10;
	uint32_t usec: 10;
	uint32_t nsec: 10;
#endif
} S_AT3_L1TIME;

/*
	L1TIME to char string conversion.
	minimum buf size is (1 + 10 + 1 + 9 + 1). recommend 32 for safe margin.
*/
AT3STDAPI const char *AT3_L1TimeString(S_AT3_L1TIME *tm, char *buf, uint32_t ulSecondBase);


/*	Baseband packet header
*/
typedef struct 
{
#if 0
	base field
	+-+-----------+---------+---+
	|m| ptr_lsb   | ptr_msb |ofi| (mode 1 example)
	|1|    7      |    6    | 2 |
	+-+-----------+---------+---+
	opt. field
		+----------------+----------------+
		|xtype| xlen_lsb |  extlen_msb    | (short or long ext)
		|  3  |     5    |        8       |
		+----------------+----------------+
		+-----+----------+----------------+
		|n_ext| xlen_lsb |    xlen_msb    | (mixed ext)
		|  3  |     5    |        8       | N: 2 ~ 7
		+-----+----------+----------------+
	mixed ext data
			+-----+------------+-----+------------+----
			|xtyp1| ext_len1   |xtyp2| ext_len2   |..
			|  3  |     13     |  3  |     13     |
			+-----+------------+-----+------------+----
#endif
	uint8_t  mode;
		// base field mode, 0 or 1
		// mode 0: base field is 1 byte
		// mode 1: base field is 2 bytes, ofi field exist.
	uint16_t ptr;
		// pointer, range 0 ~ 8191
		// offset from bbp payload start to first ALP packet header.
	uint8_t  ofi;
		// 0 No Ext: Absence of both optional and extension fields.
		// 1 Short Ext: Presence of the opt. field with length 1 byte.
		// 2 Long Ext: Presence of the opt. field with length 2 bytes.
		// 3 Mixed Ext: Presence of the opt. field with length 2 bytes.

	// uint8_t  ext_type;
	// 	// 3 bits: 
	// 	// counter (000b) or padding (111b)
	uint16_t ext_len;
	// 	// max 13 bits: 
	// 	// actual length in bytes of any Extension field. can be zero.

	// parsed result
	int hdr_len;
		// bbp header length
		// base field + optinal fielad + extension field

	int counter;
		// -1 if no counter value exist.

} S_AT3_BBP_HDR;


//============================================================================
// A/322 L1 signaling

/*
	Below informations is necessary:
		System bandwidth 
		Preamble fft size
		Preamble pilot dx
		Preamble guard interval
		L1b fec type
*/
typedef struct S_AT3_PHY_BSP // Bootstrap Parameters
{
	// at this time, bootstrap signaling of major version zero is defined
	uint8_t   bs_num_symbol_m1;  // Number of bootstrap symbols - 1

	// below data is preamble structure parameters
	uint8_t   pb_fft_size;  // Preamble FFT size, 0: 8K, 1: 16K, 2: 32K
	uint8_t   pb_pilot;     // Preamble pilot pattern Dx, 3, 4, 6, 8, 12, 16, 24, 32

	uint8_t   pb_gi;        // Preamble guard interval
		// same encoding as L1D_guard_interval or L1B_first_sub_guard_interval
		// Refer A/322-2018 Table 9.11
	uint8_t   l1b_fec_type;  // L1-Basic fec type, (0 - 6, meaning Mode 1 - Mode 7)
		// Same encoding as L1B_L1_Detail_fec_type
		// Refer A/322-2018, Table 9.6

	// some copy of bootstrap symbol data
	uint8_t   system_bw;     // System bandwidth. 0:6MHz, 1:7MHz, 2:8MHz, 3:>8MHz
		// same as S_FE_DETAIL::sBootstrap::systemBw

} S_AT3_PHY_BSP;

AT3STDAPI const char *AT3_BootstrapString(const S_AT3_PHY_BSP *p, char *buf, unsigned bufsz, unsigned indent);


// forward decl.
struct S_AT3_PHY_L1D_COMMON;
struct S_AT3_PHY_L1D_SUBFRAME;
struct S_AT3_PHY_L1D_PLP;

typedef struct S_AT3_PHY_L1B
{
	// system and frame parameters
	uint8_t      version;             // L1-Basic structure version. (currently 0)
	uint8_t      mimo_sp_enc;         // MIMO pilot encoding scheme (0 or 1)
	uint8_t      lls_flg;             // Presence or absence of Low Level Signaling (LLS)
	uint8_t      time_info_flg;       // Presence or absence, and precision of timing information
	uint8_t      return_ch_flg;       // Dedicated return channel (DRC) is present or not
	uint8_t      papr;                // PAPR reduction (0 - 3)
	uint8_t      frame_length_mode;   // Frame is time-aligned (0) or symbol-aligned (1)
	// valid if frame_length_mode == 0 (time-aligned frame)
	  uint16_t   frame_length;        // Frame length (5ms unit)
	  uint16_t   excess_smp_per_sym;  // Additional number of excess samples included in the guard interval
	// valid if frame_length_mode == 1 (symbol-aligned frame)
	  uint16_t   time_offset;         // Number of sample periods
	  uint8_t    additional_smp;      // Number of additional samples at the end of frames
	uint8_t      num_subframe_minus_1;// Number of subframe - 1

	// parameters for L1-detail
	uint8_t      pb_num_symbol;       // Number of preamble symbol - 1
	uint8_t      pb_reduced_carriers; // Cred_coeff indicator for preamble (0 - 4)
	uint8_t      l1d_content_tag;     // Incremented if L1-Detail content is modified
	uint16_t     l1d_size;            // L1-Detail information size (byte)
	uint8_t      l1d_fec_type;        // L1-Detail fec type (0 - 6, meaning Mode 1 - Mode 7)
	uint8_t      l1d_add_parity_mode; // L1-Detail Additional parity mode (0 - 2)
	uint32_t     l1d_total_cells;     // The total size (specified in OFDM cells) of L1-Detail signaling

	// parameters for first subframe
	uint8_t      sf0_mimo;            // Subframe #0 MIMO or not
	uint8_t      sf0_miso;            // Subframe #0 MISO option (0 - 2, No/64c/256c)
	uint8_t      sf0_fft_size;        // Subframe #0 FFT size (0 - 2, 8K/16K/32K)
	uint8_t      sf0_reduced_carriers;// Cred_coeff indicator for subframe #0 (0 - 4)
	uint8_t      sf0_gi;              // Subframe #0 guard interval (1 - 12)
	uint16_t     sf0_num_ofdm_symbol; // Subframe #0 number of data payload OFDM symbols - 1
	uint8_t      sf0_sp;              // Subframe #0 scattered pilot pattern
	uint8_t      sf0_sp_boost;        // Subframe #0 scattered pilot boost (0 - 4)
	uint8_t      sf0_sbs_first;       // Subframe #0 subframe boundary symbol first flag
	uint8_t      sf0_sbs_last;        // Subframe #0 subframe boundary symbol last flag

	// miscellaneous parameters
	uint8_t      reserved[6];         // Reserved for future use
	
} S_AT3_PHY_L1B;

typedef struct S_AT3_PHY_L1D_COMMON
{
	uint8_t              version;             // L1-Detail structure version
	uint8_t              num_rf;              // Number of other frequencies in channel bonding (0 or 1)

	// valid if num_rf == 1
	  uint16_t           bonded_bsid;         // BSID of separate RF channel

	// valid if S_AT3_PHY_L1B::time_info_flg == 1 or 2 or 3
	  uint32_t           time_sec;            // Time information (sec)
	  uint16_t           time_msec;           // Time information (msec)
	  // valid if S_AT3_PHY_L1B::time_info_flg == 2 or 3 (US or NS)
	    uint16_t         time_usec;           // Time information (usec)
	    // valid if S_AT3_PHY_L1B::time_info_flg == 3 (NS)
	      uint16_t       time_nsec;           // Time information (nsec)

	// valid if version >= 1
	uint16_t             bsid;                // BSID of the current channel
	uint16_t             reserved_bitlen;     // Size of reserved data (bit)

} S_AT3_PHY_L1D_COMMON;

typedef struct S_AT3_PHY_L1D_SUBFRAME
{
	uint8_t              index;               // Subframe index
	uint8_t              mimo;                // MIMO or not
	uint8_t              miso;                // MISO option (0 - 2, No/64c/256c)
	uint8_t              fft_size;            // FFT size (0 - 2, 8K/16K/32K)
	uint8_t              reduced_carriers;    // Cred_coeff indicator (0 - 4)
	uint8_t              gi;                  // Guard interval (1 - 12)
	uint16_t             num_ofdm_symbol;     // Number of data payload OFDM symbols - 1
	uint8_t              sp;                  // Scattered pilot pattern
	uint8_t              sp_boost;            // Scattered pilot boost (0 - 4)
	uint8_t              sbs_first;           // Subframe boundary symbol first flag
	uint8_t              sbs_last;            // Subframe boundary symbol last flag
	// valid if S_AT3_PHY_L1B::num_subframe_minus_1 > 0
	  uint8_t            subframe_mux;        // Time-division multiplexed or concatenated in time
	uint8_t              freq_interleaver;    // Frequency interleaver is enabled or not
	// valid if sbs_first || sbs_last
	  uint16_t           sbs_null_cells;      // Number of null cells
	uint8_t              num_plp_minus_1;     // Number of PLPs in the current subframe

	// valid after full parse
	struct S_AT3_PHY_L1D_PLP *plps;           // array of detail plp info. size is num_plp_minus_1 + 1

} S_AT3_PHY_L1D_SUBFRAME;

typedef struct S_AT3_PHY_L1D_PLP
{
	uint8_t              id;                 // PIP ID (0 - 63) assigned uniquely within each RF channel
	uint8_t              lls_flg;            // Current PLP contains LLS information or not
	uint8_t              layer;              // Layer index of current PLP (0 - 1, Core/Enhanced)
	uint32_t             start;              // First data cell index
	uint32_t             size;               // Number of data cells allocated to the current PLP
	uint8_t              scrambler_type;     // Choice of scrambler (only 0 in current version)
	E_L1d_PlpFecType     fec_type;           // PLP FEC method
	E_L1d_PlpMod         mod;                // PLP modulation
	E_L1d_PlpCod         cod;                // PLP code rate

	uint8_t              ti_mode;            // PLP time interleaving mode (0 - 2, none/cti/hti)
	// valid if ti_mode == 0 none
	  uint16_t           fec_block_start;    // Start position of the first FEC Block
	// valid if ti_mode == 1 cti
	  uint32_t           cti_fec_block_start; // Position for CTI. (See spec)

	// valid if S_AT3_PHY_L1D_COMMON::num_rf > 0
	  uint8_t            num_ch_bonded;      // Number of other frequencies in channel bonding for this PLP (0 or 1)
	  // valid if num_ch_bonded > 0
	    uint8_t          ch_bonding_format;  // Channel bonding format for the current PLP
	    uint8_t          bonded_rf_id_0;     // Bonded RF ID #0
	    uint8_t          bonded_rf_id_1;     // Bonded RF ID #1

	// valid if S_AT3_PHY_L1D_SUBFRAME::mimo == 1
	  uint8_t            mimo_stream_combine;  // Stream combining option is used or not
	  uint8_t            mimo_iq_interleave;   // IQ polarization interleaving option is used or not
	  uint8_t            mimo_ph;              // Phase hopping option is used nor not

	// valid if layer == 0
	  uint8_t            plp_type;             // Current PLP is non-dispersed (0) or dispersed (1)
	  // valid if plp_type == 1
	    uint16_t         num_subslice;         // Number of subslice - 1 (1 - 16383)
	    uint32_t         subslice_interval;    // Subslice interval (data cell unit)

	  // valid if (ti_mode == 1/cti or ti_mode == 2/hti) and mod == 0/eLPM_Qpsk
	    uint8_t          ti_ext_interleave;    // Extended interleaving is used or not

	  // valid if ti_mode == 1/cti
	    uint8_t          cti_depth;      // Number of rows used in the CTI (0 - 3)
	    uint16_t         cti_start_row;  // Position of the interleaver selector at the start of the subframe

	  // valid if ti_mode == 2/hti
	    uint8_t          hti_inter_subframe;       // HTI is used or not in this subframe
	    uint8_t          hti_num_ti_blocks_m1;     // Number of TI blocks - 1 (0 - 15)
	    uint16_t         hti_num_fec_block_max_m1; // Maximum number of FEC Blocks - 1
	    uint16_t         hti_num_fec_block_m1[16]; // Number of FEC Blocks - 1
	    uint8_t          hti_cell_interleave;      // Cell Interleaver is used or not

	// valid if layer != 0
	  uint8_t            ldm_inj_level;      // Enhanced PLP's injection level

} S_AT3_PHY_L1D_PLP;

/*
	
*/
typedef struct S_AT3_PHY_L1PREAMBLE
{
	//---- L1Basic part
	S_AT3_PHY_L1B          l1b;      // L1Basic

	//---- L1Detail part
	S_AT3_PHY_L1D_COMMON   l1dc;     // L1Detail common part
	                                 // it is valid only when l1dsfs is non-null.
	S_AT3_PHY_L1D_SUBFRAME *l1dsfs;  // array of L1Detail subframe part
	                                 // size is l1b.num_subframe_minus_1 + 1
	                                 // it can be null if this struct has l1basic part only.

	//---- Common part (both basic and detail)
	void (*free)(struct S_AT3_PHY_L1PREAMBLE *preamble);
	                                 // function pointer that should be used 
	                                 // for freeing this structure

} S_AT3_PHY_L1PREAMBLE;


AT3STDAPI const char *AT3_L1PreambleString(const S_AT3_PHY_L1PREAMBLE *p, char *buf, unsigned bufsz, unsigned indent);


//============================================================================
// A/330 Link layer


typedef enum
{
	eAlpStd_Unknown = -1,      // Note: it cannot be used for Set api
	eAlpStd_Auto = 0,          // Note: it is not used in Get api
	eAlpStd_Korea = 1, // A/330:2016
	eAlpStd_Usa = 2,   // A/330:2018 (draft)

} E_AT3_ALP_STD;



/*	S_AT3_ALP_HEADER

	Parsed ALP header.
*/
typedef struct 
{
	uint8_t  packet_type;
		// 0: IPv4
		// 2: Compressed IP
		// 4: Link layer signaling
		// 6: Packet type extension
		// 7: TS

	int length; // alp payload length. alp header size not included. 
		// note! 
		// this length does not include signaling info bytes (5 bytes) 

	// uint8_t  payload_conf; // 0: single, 1: seg/concat
	enum E_ALP_PC {
		eSINGLE_PACKET = 1,
		eSEGMENTATION = 2,
		eCONCATENATION = 3,
	} payload_cfg;

	uint8_t  sid_valid : 1; // same as SIF
	uint8_t  hext_valid : 1; // same as HEF

	// uint8_t  seg_concat; // 0: segment, 1: concatetation, 2: single

	// segmentation mode. only valid when payload_cfg is eSEGMEN.
	uint8_t  seg_sn;  // sequence number: 0 ~ 31
	uint8_t  seg_lsi; // 1 if it is last segment.

	// concatenation mode. only valid when payload_cfg is eCONCAT.
	uint8_t  con_count;  // number of packets in this ALP. range: 2 ~ 9.
	uint16_t con_len[9]; // length of each component.

	// bool     header_mode;  // 0: single, 1: additional

	// sub stream id
	uint8_t  sid; // only valid when sid_valid is true.

	// header extension. only valid when hext_valid is true.
	uint8_t  hext_type;
	uint16_t hext_len;  // max 256. bitstream's ext_len_minus1 + 1.
	uint8_t  hext_byte[256+4]; // +4 is safe margin..

	// signaling info. only valid when packet type is 4.
	// uint8_t  sig_valid;
	uint8_t  sig_type;     // 8 bits, 1:LMT, 2:ROHC-U desc.
	uint16_t sig_type_ext;
	uint8_t  sig_ver;
	uint8_t  sig_format : 2;   // 2 bits, 0:Binary, 1:XML, 2:JSON
	uint8_t  sig_encoding : 2; // 2 bits, 0:Plain, 1:Deflate

	// packet type extension. only valid when packet type is 6.
	// uint8_t  pte_valid;
	uint16_t pte_ext_type;

	// alp header length.
	//   base header + additional header + header extension + signaling info header
	// alp payload will start after this header len.
	// this should not be larger than MAX_ALP_HDR_LEN
	uint16_t header_len;

} S_AT3_ALP_HEADER;


//============================================================================

// A/331 Signaling

/*	S_AT3_LMT

	Parsed Link Mapping Table
*/
typedef struct 
{
	int num_mc;  // number of multicasts

	struct S_LMT_MC // Multicasts info
	{
		uint8_t plp_id;

		uint32_t src_ip_add, dst_ip_add;
			// ip address uses host-endian.
			// ex: 1.2.3.4 => 0x01020304
		uint16_t src_udp_port, dst_udp_port;
		uint8_t  sid_flag: 1;
		uint8_t  compressed_flag: 1;
		uint8_t  sid, context_id;

	} *mc; // pointer of multicast array. number of element is num_mc.

} S_AT3_LMT;

AT3STDAPI void AT3_ATSC_PrintLmt(S_AT3_LMT *pLmt, int dbglevel);

//============================================================================

#ifdef __cplusplus
};
#endif

#endif // __AT3_ATSC_STD_H__

