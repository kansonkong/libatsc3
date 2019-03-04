/** \file lct_hdr.h \brief LCT header
 *
 *  $Author: peltotal $ $Date: 2007/02/26 13:48:19 $ $Revision: 1.23 $
 *
 *  MAD-ALCLIB: Implementation of ALC/LCT protocols, Compact No-Code FEC,
 *  Simple XOR FEC, Reed-Solomon FEC, and RLC Congestion Control protocol.
 *  Copyright (c) 2003-2007 TUT - Tampere University of Technology
 *  main authors/contacts: jani.peltotalo@tut.fi and sami.peltotalo@tut.fi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  In addition, as a special exception, TUT - Tampere University of Technology
 *  gives permission to link the code of this program with the OpenSSL library (or
 *  with modified versions of OpenSSL that use the same license as OpenSSL), and
 *  distribute linked combinations including the two. You must obey the GNU
 *  General Public License in all respects for all of the code used other than
 *  OpenSSL. If you modify this file, you may extend this exception to your version
 *  of the file, but you are not obligated to do so. If you do not wish to do so,
 *  delete this exception statement from your version.
 */

#ifndef _LCT_HDR_H_
#define _LCT_HDR_H_

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure for the default part of LCT header.
 * @struct def_lct_hdr
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   V   | C | r |S| O |H|T|R|A|B|   HDR_LEN     |       CP      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    Congestion Control Information (CCI, length = 32 bits)     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 *
 *
 *
 *A.3.5 Packet Format
The packet format used by ROUTE follows the ALC packet format per RFC 5775 [27],
	i.e. the UDP header followed by the LCT header and the source FEC Payload ID followed by the packet payload.
	The LCT header shall be as defined in the LCT building block in RFC 5651 [26].
	The source FEC Payload ID in ROUTE is represented by the start_offset field, provided either directly or provided by any FEC scheme.
	The start_offset field value represents the starting byte position, relative to the first byte of the delivery object,
	 of the subsequent and contiguous portion of the delivery object carried in the present ROUTE packet.


	 A.3.5.1 FEC Payload ID for Source Flows
The syntax of the FEC Payload ID for the Compact No-Code FEC Scheme used in ROUTE source flows shall be a 32-bit unsigned integer value that expresses the start_offset of the fragment. Figure A.3.3 diagrams the 32-bit start_offset field.

0-31 bits
0 1 2 3 4 5 6 7 8 9 0 12345678901234567890                      1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   start_offset											    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


The Congestion Control Information (CCI) field in the LCT header contains the required congestion control information. The packet payload contains data bytes belonging to the delivery object. If more than one object is carried in the session, then the TOI within the LCT header shall be used to identify the object to which the packet payload data pertains.
The version number field of the LCT header shall be interpreted as the ROUTE version number field. This version of ROUTE implicitly makes use of version 1 of the LCT building block defined in RFC 5651 [26].
The overall ROUTE packet format shall be as depicted in Figure A.3.2. The packet is an IP packet, either IPv4 or IPv6, and the IP header precedes the UDP header. The ROUTE packet format has no dependencies on the IP version number.

 *
 *
 *                          Note for ATSC3 compat: A/331
 *
 *                          A.3.6 LCT Building Block
The LCT packet header fields shall be used as defined by the LCT building block in RFC 5651 [26]. The semantics and usage of the following LCT header fields shall be further constrained in ROUTE as follows:
123
ATSC A/331:2017 A/331, Annex A 6 December 2017

* Version number (V) – This 4-bit field indicates the protocol version number.
 	 The version number for this specification is ‘0001’.

 * Congestion Control flag (C) field – This 2-bit field, as defined in RFC 5651 [26], shall be set to ‘00’.

 * Protocol-Specific Indication (PSI) – This 2-bit field indicates whether the current packet is a source packet
			or an FEC repair packet. As the ROUTE source protocol only delivers source packets,
			this field shall be set to ‘10’.

 * Transport Session Identifier flag (S) – This 1-bit field shall be set to ‘1’ to indicate a 32-bit word in the TSI field.

 * Transport Object Identifier flag (O) – This 2-bit field shall be set to ‘01’ to indicate the number of full 32-bit words in the TOI field.

 * Half-word flag (H) – This 1-bit field shall be set to ‘0’ to indicate that no half-word field sizes are used.

 * Codepoint (CP) – This 8-bit field is used to indicate the type of the payload that is carried by this packet,
					and for ROUTE, is defined as shown below in Table A.3.6 to indicate the type of
					delivery object carried in the payload of the associated ROUTE packet.
					Depending on the type of the payload, additional payload header(s) may be added
					to prefix the payload data.

 * Transport Session Identifier (TSI) – This 32-bit field shall identify the Transport Session in ROUTE.
 	 The context of the Transport Session is provided by signaling metadata.
 	 The TSI field is constrained to a length of 32 bits because the Transport Session Identifier flag (S) must be set to ‘1’
 	 and the Half-word flag (H) must be set to ‘0’.

 * Transport Object Identifier (TOI) – This 32-bit field shall identify the object within this session
 	 to which the payload of the current packet belongs. The mapping of the TOI field to the object is
   	   provided by the Extended FDT. The TOI field is constrained to a length of 32 bits because the
   	   Transport Object Identifier flag (O) must be set to ‘01’ and the Half-word flag (H) must be set to ‘0’.


The main changes that ROUTE introduces to the usage of the LCT building block are the following:

	• ROUTE limits the usage of the LCT building block to a single channel per session.
		Congestion control is thus sender-driven in ROUTE.
	The functionality of receiver-driven layered multicast may still be offered by the application,
		allowing the receiver application to select the appropriate delivery session based on the bandwidth
		requirement of that session.
 *
 *
 *
 *EFDT – An optional element that, when present, shall contain a single instance of an FDT-Instance element per RFC 6726 [30] FLUTE, which may contain FDT extensions as defined in Sections A.3.3.2.3, A.3.3.2.4 and A.3.3.2.5. The EFDT element shall be present if this source flow transports streaming media segments. The presence of the SrcFlow.EFDT element and an EFDT transported in TOI=0 of the same LCT channel carrying this source flow shall be mutually exclusive.

ROUTE SLS is delivered with TSI=0

Service Layer Signaling provides detailed technical information to the ATSC 3.0 receiver to enable the discovery and access of ATSC 3.0 user services and their content components. It comprises a set of XML-encoded metadata fragments carried over a dedicated LCT channel. That LCT channel can be acquired using the bootstrap information contained in the SLT as described in Section 6.3. The SLS is defined on a per-service level, and it describes the characteristics and access information of the service, such as a list of its content components and how to acquire them, and the receiver capabilities required to make a meaningful presentation of the service, and the means to recover partially-received objects.. In the ROUTE/DASH system, for linear services delivery, the SLS consists of the following metadata fragments: (ROUTE-specific) USBD, S-TSID, DASH MPD, DWD (see A/337 [7]) and the HELD (see A/337 [7]).

The SLS fragments shall be delivered on a dedicated LCT transport channel with TSI = 0.

 */

//manually parse into struct for endianness
/**
 * https://tools.ietf.org/html/rfc5651
 */
typedef struct atsc3_def_lct_hdr {

  unsigned short version:4;		/**< LCT version number */
  unsigned short flag_c:2;		/**< congestion control flag */
  unsigned short psi:2;			/**jdj-2019-01-07  -    Protocol-Specific Indication (PSI): 2 bits **/

  unsigned short flag_s:1;		/**< transport session identifier flag */
  unsigned short flag_o:2;		/**< transport object identifier flag */
  unsigned short flag_h:1;		/**< half word flag */
  unsigned short reserved:2;	/** reserved as per rfc5651 */

  unsigned short flag_a:1;		/**< close session flag */
  unsigned short flag_b:1;		/**< close object flag */

  unsigned char	hdr_len_raw;		/**< total length of LCT header,
  	  Total length of the LCT header in units of 32-bit words.  The
      length of the LCT header MUST be a multiple of 32 bits.  This
      field can be used to directly access the portion of the packet
      beyond the LCT header, i.e., to the first other header if it
      exists, or to the packet payload if it exists and there is no
      other header, or to the end of the packet if there are no other
      headers or packet payload.
   */
  uint8_t hdr_len;

  uint8_t codepoint;
    /**
     A.3.6 LCT Building Block
     The LCT packet header fields shall be used as defined by the LCT building block in RFC 5651 [26].
     The semantics and usage of the following LCT header fields shall be further constrained in ROUTE as follows:
     
     Codepoint (CP) – This 8-bit field is used to indicate the type of the payload that is carried by this packet,
     and for ROUTE, is defined as shown below in Table A.3.6 to indicate the type of delivery object carried in
     the payload of the associated ROUTE packet.
     
     Depending on the type of the payload, additional payload header(s) may be added to prefix the payload data.
     
     Table A.3.6 Defined Values of Codepoint Field of LCT Header
     Codepoint value (CP)
     Semantics
     
     @formatId
     
     @frag
     
     @order
     0
     ATSC Reserved (not used)
     1
     NRT- File Mode
     1 (File Mode)
     0 (arbitrary)
     true
     2
     NRT – Entity Mode
     2 (Entity Mode)
     0
     true
     3
     NRT – Unsigned Package Mode
     3 (Unsigned Package Mode)
     0
     true
     4
     NRT – Signed Package Mode
     4 (Signed Package Mode)
     0
     true
     5
     New IS, timeline changed
     1 (File Mode)
     0
     true
     6
     New IS, timeline continued
     1
     0
     true
     7
     Redundant IS
     1
     0
     true
     8
     Media Segment, File Mode
     1
     1 (sample)
     true
     9
     Media Segment, Entity Mode
     2 (Entity Mode)
     1
     true
     10 – 127
     ATSC Reserved
     128 – 255
     Attributes of this type of packet are signalled by attributes given in the SrcFlow.Payload element associated with the CodePoint vlaue
     Per Payload element
     Per Payload element
     Per Payload element
     
     Detailed semantics for each of the defined values of the Codepoint (CP) field, which represents the type of delivery object carried in the associated ROUTE packet, shall be as follows:
     CP=1: The delivery object is an NRT file or a byte-range portion of such file delivered in ROUTE
     File Mode, as described by the EFDT element of the source flow delivering this object. CP=2: The delivery object is an NRT file or a byte-range portion of such file delivered in ROUTE
     Entity Mode.
     CP=3: The delivery object is a file of type multipart/related or a byte-range portion of such file
     delivered in ROUTE Unsigned Package Mode.
     CP=4: The delivery object is a file of type multipart/signed or a byte-range portion of such file
     delivered in ROUTE Signed Package Mode.
     CP=5: The delivery object is a DASH Initialization Segment (IS) delivered in ROUTE File Mode
     which differs from the previously delivered IS and also indicates a new presentation timeline. CP=6: The delivery object is a DASH Initialization Segment delivered in ROUTE File Mode
     which differs from the previously delivered IS, but maintains the same presentation timeline. CP=7: The delivery object is a DASH Initialization Segment (IS) delivered in ROUTE File Mode
     that is identical to the previously delivered IS.
     CP=8: The delivery object is a DASH Media Segment delivered in ROUTE File Mode as described
     by the EFDT of the source flow delivering its parent object flow.
     CP=9: The delivery object is a DASH Media Segment delivered in ROUTE Entity Mode as
     described by the EFDT of the source flow delivering its parent object flow.
     CP values from 10 to 127: These CP values are reserved for future ATSC use.
     CP values from 128 to 255: These CP values are used for packets in which the corresponding
     Payload element in the SrcFlow specifies the attributes @formatId, @frag, and @order. For example, a SrcFlow.Payload element in which @codePoint = 128 will specify the @formatId, @frag, and @order attributes of LCT packets with CP=128.
     
     
     */

  uint32_t cci;	/**< congestion control header Congestion Control Information (CCI): 32 bits only in ATSC3 **/

  uint32_t tsi;  //(TSI is always 32bits length = 32*S+16*H bits)
  uint32_t toi;  //(TOI, length = 32*O+16*H bits)


} atsc3_def_lct_hdr_t;

/**
 * This function adds FDT LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param fdt_instance_id FDT instance id
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3 
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 192   |   V   |           FDT Instance ID             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

int add_fdt_lct_he(atsc3_def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned int fdt_instance_id);

/**
 * This function adds CENC LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param content_enc_algo content encoding algorith used with the FDT instance payload
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 193   |     CENC      |          Reserved             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

int add_cenc_lct_he(atsc3_def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned char content_enc_algo);

/**
 * This function adds FTI (FEC Encoding IDs 0, 2, 128, 130) LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param transferlen length of the transport object
 * @param fec_inst_id FEC instance id (or reserved/zeroed with ID 0)
 * @param eslen encoding symbol length
 * @param max_sblen maximum source block length
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 64    |     HEL       |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * |                       Transfer Length                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   Reserved/FEC Instance ID    |     Encoding Symbol Length    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                  Maximum Source Block Length                  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 */

int add_fti_0_2_128_130_lct_he(atsc3_def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned long long transferlen,
								unsigned short fec_inst_id, unsigned short eslen, unsigned int max_sblen);
 
/**
 * This function adds FTI (FEC Encoding ID 3) LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param transferlen length of the transport object
 * @param m finite field parameter
 * @param G number of encoding symbols per FLUTE packet
 * @param eslen encoding symbol length
 * @param max_sblen maximum source block length
 * @param mxnbofes maximum number of encoding symbols per block
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 64    |     HEL       |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * |                       Transfer Length                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |       m       |       G       |     Encoding Symbol Length    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  Maximum Source Block Length  | Max. Num. of Encoding Symbols |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 */

int add_fti_3_lct_he(atsc3_def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned long long transferlen,
					unsigned char m, unsigned char G, unsigned short eslen, unsigned short max_sblen,
					unsigned short mxnbofes);

/*
 * This function adds FTI (FEC Encoding ID 129) LCT extension header to FLUTE's header.
 *
 * @param def_lct_hdr pointer to the default LCT header
 * @param hdrlen current length of FLUTE header
 * @param transferlen length of the transport object
 * @param fec_inst_id FEC instance id (or reserved/zeroed with ID 0)
 * @param eslen encoding symbol length
 * @param max_sblen maximum source block length
 * @param mxnbofes maximum number of encoding symbols per block
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   HET = 64    |     HEL       |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * |                       Transfer Length                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    FEC Instance ID            |     Encoding Symbol Length    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  Maximum Source Block Length  | Max. Num. of Encoding Symbols |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 */

int add_fti_129_lct_he(atsc3_def_lct_hdr_t *def_lct_hdr, int hdrlen, unsigned long long transferlen,
						unsigned short fec_inst_id, unsigned short eslen, unsigned short max_sblen,
						unsigned short mxnbofes);

/**
 * This function adds NOP LCT extension header to FLUTE's header.
 * Not yet implemented.
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |      HET      |      HEL      |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * .                                                               . 
 * .           Header Extension Content (HEC)                      . 
 * .                                                               .
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

int add_nop_lct_he(void);

/*
 * This function adds AUTH LCT extension header to FLUTE's header.
 * Not yet implemented.
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    HET = 0    |      HEL      |                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
 * .                                                               . 
 * .           Header Extension Content (HEC)                      . 
 * .                                                               .
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

int add_auth_lct_he(void);

/**
 * This function adds TIME LCT extension header to FLUTE's header.
 * Not yet implemented.
 *
 * @return number of bytes added to the FLUTE's header
 *
 */

/*
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    HET = 2    |      HEL      |         Use (bit field)       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       first time value                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * ...            (other time values (optional)                  ...
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *                 Use (bit field)
 *
 *                 2                                       3
 *   6   7   8   9   0   1   2   3   4   5   6   7   8   9   0   1
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |SCT|SCT|ERT|SLC|   reserved    |          PI-specific          |
 * |Hi |Low|   |   |    by LCT     |              use              |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 */

int add_time_lct_he(void);

#ifdef __cplusplus
}; //extern "C"
#endif

#endif

