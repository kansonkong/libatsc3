/*
 *
 * atsc3_isobmff-test.c:  test parser for isobmff boxes
 *
 *
 [moof] size=8+1092
  [mfhd] size=12+4
    sequence number = 2
  [traf] size=8+1016
    [tfhd] size=12+4, flags=20000
      track ID = 1
    [tfdt] size=12+8, version=1
      base media decode time = 0
    [trun] size=12+968, flags=f01
      sample count = 60
      data offset = 1108
      entry 0000 = sample_duration:16683, sample_size:110958, sample_flags:2000000, sample_composition_time_offset:50050
      entry 0001 = sample_duration:16683, sample_size:17007, sample_flags:10000, sample_composition_time_offset:166833
      entry 0002 = sample_duration:16683, sample_size:2859, sample_flags:10000, sample_composition_time_offset:83417
      entry 0003 = sample_duration:16683, sample_size:898, sample_flags:10000, sample_composition_time_offset:33367
      entry 0004 = sample_duration:16683, sample_size:309, sample_flags:10000, sample_composition_time_offset:0
      entry 0005 = sample_duration:16683, sample_size:301, sample_flags:10000, sample_composition_time_offset:16683
      entry 0006 = sample_duration:16683, sample_size:900, sample_flags:10000, sample_composition_time_offset:50050
      entry 0007 = sample_duration:16683, sample_size:324, sample_flags:10000, sample_composition_time_offset:16683
      entry 0008 = sample_duration:16683, sample_size:318, sample_flags:10000, sample_composition_time_offset:33367
      entry 0009 = sample_duration:16683, sample_size:19276, sample_flags:10000, sample_composition_time_offset:166833
      entry 0010 = sample_duration:16683, sample_size:2925, sample_flags:10000, sample_composition_time_offset:83417
      entry 0011 = sample_duration:16683, sample_size:1031, sample_flags:10000, sample_composition_time_offset:33367
      entry 0012 = sample_duration:16683, sample_size:318, sample_flags:10000, sample_composition_time_offset:0
      entry 0013 = sample_duration:16683, sample_size:312, sample_flags:10000, sample_composition_time_offset:16683
      entry 0014 = sample_duration:16683, sample_size:1091, sample_flags:10000, sample_composition_time_offset:50050
      entry 0015 = sample_duration:16683, sample_size:345, sample_flags:10000, sample_composition_time_offset:16683
      entry 0016 = sample_duration:16683, sample_size:340, sample_flags:10000, sample_composition_time_offset:33367
      entry 0017 = sample_duration:16683, sample_size:17461, sample_flags:10000, sample_composition_time_offset:150150
      entry 0018 = sample_duration:16683, sample_size:2846, sample_flags:10000, sample_composition_time_offset:83417
      entry 0019 = sample_duration:16683, sample_size:1251, sample_flags:10000, sample_composition_time_offset:33367
      entry 0020 = sample_duration:16683, sample_size:414, sample_flags:10000, sample_composition_time_offset:0
      entry 0021 = sample_duration:16683, sample_size:567, sample_flags:10000, sample_composition_time_offset:16683
      entry 0022 = sample_duration:16683, sample_size:1002, sample_flags:10000, sample_composition_time_offset:50050
      entry 0023 = sample_duration:16683, sample_size:633, sample_flags:10000, sample_composition_time_offset:16683
      entry 0024 = sample_duration:16683, sample_size:24385, sample_flags:10000, sample_composition_time_offset:150150
      entry 0025 = sample_duration:16683, sample_size:2825, sample_flags:10000, sample_composition_time_offset:83417
      entry 0026 = sample_duration:16683, sample_size:1140, sample_flags:10000, sample_composition_time_offset:33367
      entry 0027 = sample_duration:16683, sample_size:547, sample_flags:10000, sample_composition_time_offset:0
      entry 0028 = sample_duration:16683, sample_size:496, sample_flags:10000, sample_composition_time_offset:16683
      entry 0029 = sample_duration:16683, sample_size:821, sample_flags:10000, sample_composition_time_offset:50050
      entry 0030 = sample_duration:16683, sample_size:527, sample_flags:10000, sample_composition_time_offset:16683
      entry 0031 = sample_duration:16683, sample_size:20770, sample_flags:10000, sample_composition_time_offset:150150
      entry 0032 = sample_duration:16683, sample_size:2904, sample_flags:10000, sample_composition_time_offset:83417
      entry 0033 = sample_duration:16683, sample_size:1445, sample_flags:10000, sample_composition_time_offset:33367
      entry 0034 = sample_duration:16683, sample_size:474, sample_flags:10000, sample_composition_time_offset:0
      entry 0035 = sample_duration:16683, sample_size:504, sample_flags:10000, sample_composition_time_offset:16683
      entry 0036 = sample_duration:16683, sample_size:946, sample_flags:10000, sample_composition_time_offset:50050
      entry 0037 = sample_duration:16683, sample_size:509, sample_flags:10000, sample_composition_time_offset:16683
      entry 0038 = sample_duration:16683, sample_size:20638, sample_flags:10000, sample_composition_time_offset:150150
      entry 0039 = sample_duration:16683, sample_size:2328, sample_flags:10000, sample_composition_time_offset:83417
      entry 0040 = sample_duration:16683, sample_size:778, sample_flags:10000, sample_composition_time_offset:33367
      entry 0041 = sample_duration:16683, sample_size:409, sample_flags:10000, sample_composition_time_offset:0
      entry 0042 = sample_duration:16683, sample_size:393, sample_flags:10000, sample_composition_time_offset:16683
      entry 0043 = sample_duration:16683, sample_size:721, sample_flags:10000, sample_composition_time_offset:50050
      entry 0044 = sample_duration:16683, sample_size:418, sample_flags:10000, sample_composition_time_offset:16683
      entry 0045 = sample_duration:16683, sample_size:21161, sample_flags:10000, sample_composition_time_offset:166833
      entry 0046 = sample_duration:16683, sample_size:2906, sample_flags:10000, sample_composition_time_offset:83417
      entry 0047 = sample_duration:16683, sample_size:1012, sample_flags:10000, sample_composition_time_offset:33367
      entry 0048 = sample_duration:16683, sample_size:331, sample_flags:10000, sample_composition_time_offset:0
      entry 0049 = sample_duration:16683, sample_size:352, sample_flags:10000, sample_composition_time_offset:16683
      entry 0050 = sample_duration:16683, sample_size:1130, sample_flags:10000, sample_composition_time_offset:50050
      entry 0051 = sample_duration:16683, sample_size:402, sample_flags:10000, sample_composition_time_offset:16683
      entry 0052 = sample_duration:16683, sample_size:387, sample_flags:10000, sample_composition_time_offset:33367
      entry 0053 = sample_duration:16683, sample_size:11764, sample_flags:10000, sample_composition_time_offset:150150
      entry 0054 = sample_duration:16683, sample_size:2604, sample_flags:10000, sample_composition_time_offset:83417
      entry 0055 = sample_duration:16683, sample_size:1075, sample_flags:10000, sample_composition_time_offset:33367
      entry 0056 = sample_duration:16683, sample_size:348, sample_flags:10000, sample_composition_time_offset:0
      entry 0057 = sample_duration:16683, sample_size:418, sample_flags:10000, sample_composition_time_offset:16683
      entry 0058 = sample_duration:16683, sample_size:921, sample_flags:10000, sample_composition_time_offset:50050
      entry 0059 = sample_duration:16683, sample_size:441, sample_flags:10000, sample_composition_time_offset:16683
  [traf] size=8+44
    [tfhd] size=12+12, flags=20018
      track ID = 2
      default sample duration = 1
      default sample size = 34
    [trun] size=12+8, flags=1
      sample count = 60
      data offset = 313300
      entry 0000 =
      entry 0001 =
      entry 0002 =
      entry 0003 =
      entry 0004 =
      entry 0005 =
      entry 0006 =
      entry 0007 =
      entry 0008 =
      entry 0009 =
      entry 0010 =
      entry 0011 =
      entry 0012 =
      entry 0013 =
      entry 0014 =
      entry 0015 =
      entry 0016 =
      entry 0017 =
      entry 0018 =
      entry 0019 =
      entry 0020 =
      entry 0021 =
      entry 0022 =
      entry 0023 =
      entry 0024 =
      entry 0025 =
      entry 0026 =
      entry 0027 =
      entry 0028 =
      entry 0029 =
      entry 0030 =
      entry 0031 =
      entry 0032 =
      entry 0033 =
      entry 0034 =
      entry 0035 =
      entry 0036 =
      entry 0037 =
      entry 0038 =
      entry 0039 =
      entry 0040 =
      entry 0041 =
      entry 0042 =
      entry 0043 =
      entry 0044 =
      entry 0045 =
      entry 0046 =
      entry 0047 =
      entry 0048 =
      entry 0049 =
      entry 0050 =
      entry 0051 =
      entry 0052 =
      entry 0053 =
      entry 0054 =
      entry 0055 =
      entry 0056 =
      entry 0057 =
      entry 0058 =
      entry 0059 =
[mdat] size=8+312916
 *
0000   00 00 04 4c 6d 6f 6f 66 00 00 00 10 6d 66 68 64   ...Lmoof....mfhd
0010   00 00 00 00 00 00 00 01 00 00 04 00 74 72 61 66   ............traf
0020   00 00 00 10 74 66 68 64 00 02 00 00 00 00 00 01   ....tfhd........
0030   00 00 00 14 74 66 64 74 01 00 00 00 00 00 00 00   ....tfdt........
0040   00 00 00 00 00 00 03 d4 74 72 75 6e 00 00 0f 01   .......Ôtrun....
0050   00 00 00 3c 00 00 04 54 00 00 41 2b 00 01 a1 33   ...<...T..A+..¡3
0060   02 00 00 00 00 00 c3 82 00 00 41 2b 00 00 0c 46   ......Ã...A+...F
0070   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 17 32   ......Ã...A+...2
0080   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 1d 2f   ......Ã...A+.../
0090   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 35 ba   ......Ã...A+..5º
00a0   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 22 f3   ......Ã...A+.."ó
00b0   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 2d 5d   ......Ã...A+..-]
00c0   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 76 82   ......Ã...A+..v.
00d0   00 01 00 00 00 01 04 ad 00 00 41 2b 00 00 00 4c   ..........A+...L
00e0   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 12 3c   .......W..A+...<
00f0   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 22 31   ......Ã...A+.."1
0100   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 2c 9c   ......Ã...A+..,.
0110   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 2d b2   ......Ã...A+..-²
0120   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 39 ad   ......Ã...A+..9.
0130   00 01 00 00 00 02 8b b1 00 00 41 2b 00 00 01 5a   .......±..A+...Z
0140   00 01 00 00 00 01 45 d9 00 00 41 2b 00 00 00 c0   ......EÙ..A+...À
0150   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 00 79   .......W..A+...y
0160   00 01 00 00 00 00 00 00 00 00 41 2b 00 00 00 48   ..........A+...H
0170   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 00 c6   ......A+..A+...Æ
0180   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 00 60   ......Ã...A+...`
0190   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 00 53   ......A+..A+...S
01a0   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 2b 00   .......W..A+..+.
01b0   00 01 00 00 00 02 8b b1 00 00 41 2b 00 00 02 3e   .......±..A+...>
01c0   00 01 00 00 00 01 45 d9 00 00 41 2b 00 00 00 ba   ......EÙ..A+...º
01d0   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 00 4e   .......W..A+...N
01e0   00 01 00 00 00 00 00 00 00 00 41 2b 00 00 00 42   ..........A+...B
01f0   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 01 2e   ......A+..A+....
0200   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 00 5f   ......Ã...A+..._
0210   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 00 84   ......A+..A+....
0220   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 2d 16   .......W..A+..-.
0230   00 01 00 00 00 02 8b b1 00 00 41 2b 00 00 03 23   .......±..A+...#
0240   00 01 00 00 00 01 45 d9 00 00 41 2b 00 00 01 29   ......EÙ..A+...)
0250   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 00 db   .......W..A+...Û
0260   00 01 00 00 00 00 00 00 00 00 41 2b 00 00 00 94   ..........A+....
0270   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 01 43   ......A+..A+...C
0280   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 00 d4   ......Ã...A+...Ô
0290   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 00 97   ......A+..A+....
02a0   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 18 e3   .......W..A+...ã
02b0   00 01 00 00 00 02 8b b1 00 00 41 2b 00 00 02 e0   .......±..A+...à
02c0   00 01 00 00 00 01 45 d9 00 00 41 2b 00 00 01 a7   ......EÙ..A+...§
02d0   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 00 b9   .......W..A+...¹
02e0   00 01 00 00 00 00 00 00 00 00 41 2b 00 00 00 df   ..........A+...ß
02f0   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 01 00   ......A+..A+....
0300   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 00 46   ......Ã...A+...F
0310   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 00 7d   ......A+..A+...}
0320   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 4c e6   .......W..A+..Læ
0330   00 01 00 00 00 02 8b b1 00 00 41 2b 00 00 02 c0   .......±..A+...À
0340   00 01 00 00 00 01 45 d9 00 00 41 2b 00 00 01 0e   ......EÙ..A+....
0350   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 01 c7   .......W..A+...Ç
0360   00 01 00 00 00 00 00 00 00 00 41 2b 00 00 01 c8   ..........A+...È
0370   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 05 69   ......A+..A+...i
0380   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 01 51   ......Ã...A+...Q
0390   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 01 2e   ......A+..A+....
03a0   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 6e cf   .......W..A+..nÏ
03b0   00 01 00 00 00 02 4a 86 00 00 41 2b 00 00 07 55   ......J...A+...U
03c0   00 01 00 00 00 01 45 d9 00 00 41 2b 00 00 03 5c   ......EÙ..A+...\
03d0   00 01 00 00 00 00 82 57 00 00 41 2b 00 00 02 6f   .......W..A+...o
03e0   00 01 00 00 00 00 00 00 00 00 41 2b 00 00 01 ee   ..........A+...î
03f0   00 01 00 00 00 00 41 2b 00 00 41 2b 00 00 01 ef   ......A+..A+...ï
0400   00 01 00 00 00 00 c3 82 00 00 41 2b 00 00 01 78   ......Ã...A+...x
0410   00 01 00 00 00 00 41 2b 00 00 00 34 74 72 61 66   ......A+...4traf
0420   00 00 00 18 74 66 68 64 00 02 00 18 00 00 00 02   ....tfhd........
0430   00 00 00 01 00 00 00 22 00 00 00 14 74 72 75 6e   ......."....trun
0440   00 00 00 01 00 00 00 3c 00 05 11 3a 00 05 17 ba   .......<...:...º
0450   6d 64 61 74                                       mdat


[moof] size=8+1092
  [mfhd] size=12+4
    sequence number = 2
  [traf] size=8+1016
    [tfhd] size=12+4, flags=20000
      track ID = 1
    [tfdt] size=12+8, version=1
      base media decode time = 0
    [trun] size=12+968, flags=f01
      sample count = 60
      data offset = 1108

 */
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdbool.h>

#define __UNIT_TEST 1
#ifdef __UNIT_TEST

//forward declare our testcases

static char* _get_movie_fragment_metadata()	{ return "0000044c6d6f6f66000000106d66686400000000000000010000040074726166000000107466686400020000000000010000001474666474010000000000000000000000000003d47472756e00000f010000003c000004540000412b0001a133020000000000c3820000412b00000c46000100000000c3820000412b00001732000100000000c3820000412b00001d2f000100000000c3820000412b000035ba000100000000c3820000412b000022f3000100000000c3820000412b00002d5d000100000000c3820000412b0000768200010000000104ad0000412b0000004c00010000000082570000412b0000123c000100000000c3820000412b00002231000100000000c3820000412b00002c9c000100000000c3820000412b00002db2000100000000c3820000412b000039ad0001000000028bb10000412b0000015a00010000000145d90000412b000000c000010000000082570000412b0000007900010000000000000000412b00000048000100000000412b0000412b000000c6000100000000c3820000412b00000060000100000000412b0000412b0000005300010000000082570000412b00002b000001000000028bb10000412b0000023e00010000000145d90000412b000000ba00010000000082570000412b0000004e00010000000000000000412b00000042000100000000412b0000412b0000012e000100000000c3820000412b0000005f000100000000412b0000412b0000008400010000000082570000412b00002d160001000000028bb10000412b0000032300010000000145d90000412b0000012900010000000082570000412b000000db00010000000000000000412b00000094000100000000412b0000412b00000143000100000000c3820000412b000000d4000100000000412b0000412b0000009700010000000082570000412b000018e30001000000028bb10000412b000002e000010000000145d90000412b000001a700010000000082570000412b000000b900010000000000000000412b000000df000100000000412b0000412b00000100000100000000c3820000412b00000046000100000000412b0000412b0000007d00010000000082570000412b00004ce60001000000028bb10000412b000002c000010000000145d90000412b0000010e00010000000082570000412b000001c700010000000000000000412b000001c8000100000000412b0000412b00000569000100000000c3820000412b00000151000100000000412b0000412b0000012e00010000000082570000412b00006ecf0001000000024a860000412b0000075500010000000145d90000412b0000035c00010000000082570000412b0000026f00010000000000000000412b000001ee000100000000412b0000412b000001ef000100000000c3820000412b00000178000100000000412b0000003474726166000000187466686400020018000000020000000100000022000000147472756e000000010000003c0005113a000517ba6d646174"; }
void __create_binary_payload(char *test_payload_base64, uint8_t **binary_payload, int * binary_payload_size);

typedef struct box {
	uint32_t size;
	uint32_t type;
} box_t;


typedef struct fullbox {
	box_t 	box;
	uint8_t version;
	uint8_t flags[3];
} fullbox_t;

typedef struct mfhd {
	fullbox_t fullbox;

	uint32_t sequence_number;
} mfhd_t;


/*
aligned(8) class TrackRunBox
extends FullBox(‘trun’, version, tr_flags) {
	unsigned int(32) sample_count;
// the following are optional fields
	signed int(32) data_offset;
	unsigned int(32) first_sample_flags;
// all fields in the following array are optional {
      unsigned int(32)  sample_duration;
      unsigned int(32)  sample_size;
      unsigned int(32)  sample_flags
      if (version == 0)
         { unsigned int(32) sample_composition_time_offset; }

      else
         { signed int(32) sample_composition_time_offset; }

   }[ sample_count ]
}
*/
typedef struct trun_sample {
	uint32_t sample_duration;
	uint32_t sample_size;
	uint32_t sample_flags;
	uint32_t sample_composition_time_offset; //v=1 signed
} trun_sample_t;

typedef struct trun {
	fullbox_t fullbox;
	uint32_t sample_count;
	int32_t data_offset;
	uint32_t first_sample_flags;
	int sample_n;
	trun_sample_t** trun_sample;
}trun_t;

/*
 aligned(8) class TrackFragmentBaseMediaDecodeTimeBox extends FullBox(‘tfdt’, version, 0) {
if (version==1) {
unsigned int(64) baseMediaDecodeTime; } else { // version==0
unsigned int(32) baseMediaDecodeTime; }
}

 */
typedef struct tfdt_v0 {
	fullbox_t fullbox;
	uint32_t base_media_decode_time;
} tfdt_v0_t;

typedef struct tfdt_v1 {
	fullbox_t fullbox;
	uint64_t base_media_decode_time;
} tfdt_v1_t;

/**
 * aligned(8) class TrackFragmentHeaderBox extends FullBox(‘tfhd’, 0, tf_flags){
unsigned int(32) track_ID;
// all the following are optional fields
 unsigned int(64) base_data_offset;
 unsigned int(32) sample_description_index;
 unsigned int(32) default_sample_duration;
 unsigned int(32) default_sample_size;
 unsigned int(32) default_sample_flags
}
 *
 */
typedef struct tfhd {
	box_t* box;
	uint32_t track_id;
	uint64_t base_data_offset;
	uint32_t sample_description_index;
	uint32_t default_sample_duration;
	uint32_t default_sample_size;
	uint32_t default_sample_flags;

} tfhd_t;
/*
 * aligned(8) class TrackFragmentBox extends Box(‘traf’){
 */
typedef struct traf {
	box_t* box;
	tfhd_t* tfhd;
	tfdt_v0_t* tfdt_v0;
	tfdt_v1_t* tfdt_v1;

	trun_t* trun;

} traf_t;

/**
 * aligned(8) class MovieFragmentBox extends Box(‘moof’){ }
 *
 */
typedef struct moof {
	box_t box;
	mfhd_t* mfhd;
	int traf_n;
	traf_t** traf;
} moof_t;

/*
 * unsigned int(8) version = v;
bit(24) flags = f;
 */



bool type_matches(box_t* box, char* type) {
	if((((box->type >> 24) & 0xFF) == type[0]) && (((box->type >> 16) & 0xFF) == type[1]) && (((box->type >> 8) & 0xFF) == type[2]) && (((box->type) & 0xFF) == type[3])) {
		return true;
	}
	return false;
}

void add_box(box_t* parent_box, box_t* child_box) {
//	if(parent_box->can_have_children) {
//		if(!parent_box->child_box_n) {
//			parent_box->child_box = calloc(1, sizeof(box_t*));
//			parent_box->child_box[0] = child_box;
//			parent_box->child_box_n = 1;
//		} else {
//			parent_box->child_box = realloc(parent_box->child_box, parent_box->child_box_n+1);
//			parent_box->child_box[parent_box->child_box_n++] = child_box;
//		}
//		child_box->parent_box = parent_box;
//	} else {
//		//walk up the chain and add it accordingly
//
//	}
//




}



void read_box_full_header(uint8_t** buffer, box_t* box, fullbox_t* fullbox) {
	uint8_t scratch;
	fullbox->box.size = box->size;
	fullbox->box.type = box->type;

	//version
	memcpy(&scratch, *buffer, 1);
	fullbox->version = ntohl(scratch);
	*buffer+=1;

	//flags
	memcpy(&scratch, *buffer, 1);
	fullbox->flags[0] = ntohl(scratch);
	*buffer+=1;
	//flags
	memcpy(&scratch, *buffer, 1);
	fullbox->flags[1] = ntohl(scratch);
	*buffer+=1;
	//flags
	memcpy(&scratch, *buffer, 1);
	fullbox->flags[2] = ntohl(scratch);
	*buffer+=1;

}


void read_uint32_t(uint8_t** buffer, void** addr) {
	//size
	uint32_t scratch;
	memcpy(&scratch, *buffer, 4);
	*addr = ntohl(scratch);
	*buffer+=4;


}




void read_box_header(uint8_t** buffer, box_t* box) {
	uint32_t scratch;
	memcpy(&scratch, *buffer, 4);
	box->size = ntohl(scratch);
	*buffer+=4;

	memcpy(&scratch, *buffer, 4);
	box->type = ntohl(scratch);
	*buffer+=4;

//	if(type_matches(box, "mfhd")) {
//
//		box->interior_size = sizeof(mfhd_t);
//		box->can_have_children = false;
//	}
//	if(type_matches(box, "traf")) {
//		box->interior_size = sizeof(traf_t);
//	}
//
//	if(box->interior_size) {
//		buffer += box->interior_size;
//	}

}


void* parse_box_from_header(uint8_t** binary_payload, uint8_t* binary_payload_start, uint32_t binary_payload_size,  box_t** box) {
	void* current_box = NULL;
	box_t temp_box;
	read_box_header(binary_payload, &temp_box);

	if(type_matches(&temp_box, "moof")) {
		current_box = calloc(1, sizeof(moof_t));
		//copy first uint32_t size, uint32_t type into our final box
		memcpy(current_box, &temp_box, 8);

		((moof_t*)current_box)->mfhd = calloc(1, sizeof(mfhd_t));
		parse_box_from_header(binary_payload, binary_payload_start, binary_payload_size, &(((moof_t*)current_box)->mfhd));

		int traf_n = 0;
		((moof_t*)current_box)->traf = calloc(1, sizeof(traf_t*));

		while(*binary_payload - binary_payload_start < binary_payload_size) {
			traf_n = ((moof_t*)current_box)->traf_n++;
			if(traf_n > 1) {
				//resize
				((moof_t*)current_box)->traf = realloc(((moof_t*)current_box)->traf, traf_n+1);
			}

			((moof_t*)current_box)->traf[traf_n] = calloc(1, sizeof(traf_t));
			parse_box_from_header(binary_payload, binary_payload_start, binary_payload_size, &((((moof_t*)current_box)->traf[traf_n])));
		}


		*box = current_box;
	} else if(type_matches(&temp_box, "mfhd")) {
		current_box = box;
		read_box_full_header(binary_payload, &temp_box, &box);
		mfhd_t* mfhd = *box;
		//copy first uint32_t size, uint32_t type into our final box
		//memcpy(&current_box, &temp_box, 12);
		read_uint32_t(binary_payload, &mfhd->sequence_number);
	}



	return current_box;

}
void print_hex(uint8_t* ptr, uint8_t size) {
	for(int i=0; i < size; i++) {
		printf("0x%0x ", ptr[i]);
	}
	printf("\n");
}


bool read_header_for_chars(uint8_t** buffer, char* type) {
	//printf("checking 0x%0x, 0x%0x\n", (*buffer)[0], type[0]);
	if((*buffer)[0] == type[0] && (*buffer)[1] == type[1] && (*buffer)[2] == type[2] && (*buffer)[3] == type[3]) {
		return true;
	}
	(*buffer)++;
	return false;

}
typedef struct truncate_box {
	uint32_t box_size;
	uint8_t box_start;
	uint8_t box_end;
} truncate_box_t;

void dump_box(box_t* box) {
	printf("box size: %u, label: %c%c%c%c\n", box->size, box->type>>24, box->type>>16, box->type>>8, box->type & 0xFF);
}
//
//int main() {
//
//	char* base64_payload = _get_movie_fragment_metadata();
//	uint8_t* binary_payload;
//	int binary_payload_size;
//
//	__create_binary_payload(base64_payload, &binary_payload, &binary_payload_size);
//
//	uint8_t* binary_payload_start = binary_payload;
//	int matching_box_count = 0;
//	truncate_box_t truncate_box;
//	printf("starting at: %p", binary_payload_start);
//
//	while(binary_payload - binary_payload_start < binary_payload_size) {
//		if(read_header_for_chars(&binary_payload, "traf")) {
//			printf("found instance %d, at %p\n", matching_box_count, binary_payload);
//			binary_payload -=4;
//			memcpy(&truncate_box.box_size, binary_payload, 4);
//			truncate_box.box_size = ntohl(truncate_box.box_size);
//			truncate_box.box_start = binary_payload - binary_payload_start;
//			truncate_box.box_end = 8 + truncate_box.box_size;
//			matching_box_count++;
//			binary_payload += 8;
//		}
//	}
//	if(matching_box_count == 2) {
//		printf("to truncate: %u, starting at %d, ending at %d\n", truncate_box.box_size, truncate_box.box_start, truncate_box.box_end);
//		//our first box should be moof
//
//		uint32_t moof_box_size;
//		memcpy(&moof_box_size, binary_payload_start, 4);
//		moof_box_size = ntohl(moof_box_size);
//		uint32_t adjusted_moof_box_size = moof_box_size;
//		adjusted_moof_box_size -= truncate_box.box_size;
//
//		printf("original moof box size is: %u, adjusted size is: %u\n", moof_box_size, adjusted_moof_box_size);
//		//update the moof box size
//		uint32_t hl_adjusted_moof_box_size = htonl(adjusted_moof_box_size);
//		memcpy(binary_payload_start, &hl_adjusted_moof_box_size, 4);
//
//		uint8_t* final_payload = calloc(binary_payload_size, sizeof(char));
//		printf("first segment len: %p - %p\n", truncate_box.box_start, binary_payload_start);
//
//
////
////		memcpy(final_payload, binary_payload_start, truncate_box.box_start);
////		uint8_t last_segment_len = binary_payload_size - first_segment_len - truncate_box.box_size;
////
////		memcpy(&final_payload[first_segment_len], truncate_box.box_end, last_segment_len);
////		uint8_t total_len = first_segment_len + last_segment_len;
////		printf("old length: %u, new length: %u", binary_payload_size, total_len);
//		FILE* fp = fopen("patch.moof", "w");
////		fwrite(binary_payload_start, total_len, 1, fp);
//		fclose(fp);
//
//	}
//	return 0;
//}



int main() {

	char* base64_payload = _get_movie_fragment_metadata();
	uint8_t* binary_payload;
	int binary_payload_size;

	__create_binary_payload(base64_payload, &binary_payload, &binary_payload_size);

	uint8_t* binary_payload_start = binary_payload;
	int matching_box_count = 0;
	printf("starting at: %p", binary_payload_start);

	box_t* box = NULL;

	while(binary_payload - binary_payload_start < binary_payload_size) {
		//start with a moof box
		void* current_box = parse_box_from_header(&binary_payload, binary_payload_start, binary_payload_size, &box);

//		if(read_header_for_chars)
//		if(read_header_for_chars(&binary_payload, "traf")) {
//			printf("found instance %d, at %p\n", matching_box_count, binary_payload);
//			binary_payload -=4;
//			memcpy(&truncate_box.box_size, binary_payload, 4);
//			truncate_box.box_size = ntohl(truncate_box.box_size);
//			truncate_box.box_start = binary_payload - binary_payload_start;
//			truncate_box.box_end = 8 + truncate_box.box_size;
//			matching_box_count++;
//			binary_payload += 8;
//		}
	}
//	if(matching_box_count == 2) {
//		printf("to truncate: %u, starting at %d, ending at %d\n", truncate_box.box_size, truncate_box.box_start, truncate_box.box_end);
//		//our first box should be moof
//
//		uint32_t moof_box_size;
//		memcpy(&moof_box_size, binary_payload_start, 4);
//		moof_box_size = ntohl(moof_box_size);
//		uint32_t adjusted_moof_box_size = moof_box_size;
//		adjusted_moof_box_size -= truncate_box.box_size;
//
//		printf("original moof box size is: %u, adjusted size is: %u\n", moof_box_size, adjusted_moof_box_size);
//		//update the moof box size
//		uint32_t hl_adjusted_moof_box_size = htonl(adjusted_moof_box_size);
//		memcpy(binary_payload_start, &hl_adjusted_moof_box_size, 4);
//
//		uint8_t* final_payload = calloc(binary_payload_size, sizeof(char));
//		printf("first segment len: %p - %p\n", truncate_box.box_start, binary_payload_start);


//
//		memcpy(final_payload, binary_payload_start, truncate_box.box_start);
//		uint8_t last_segment_len = binary_payload_size - first_segment_len - truncate_box.box_size;
//
//		memcpy(&final_payload[first_segment_len], truncate_box.box_end, last_segment_len);
//		uint8_t total_len = first_segment_len + last_segment_len;
//		printf("old length: %u, new length: %u", binary_payload_size, total_len);
	//	FILE* fp = fopen("patch.moof", "w");
//		fwrite(binary_payload_start, total_len, 1, fp);
	//	fclose(fp);

	//}
	return 0;
}


/**
 *
int main() {

	char* base64_payload = _get_movie_fragment_metadata();
	uint8_t* binary_payload;
	int binary_payload_size;

	__create_binary_payload(base64_payload, &binary_payload, &binary_payload_size);

	box_t* root_box = calloc(1, sizeof(box_t));
	uint8_t* binary_payload_start = binary_payload;

	read_header(&binary_payload, root_box);
	print_hex((uint8_t*)root_box, 8);
	dump_box(root_box);

	box_t* parent_box = root_box;
	while(binary_payload - binary_payload_start < binary_payload_size) {
		printf("binary payload is at: %p\n", binary_payload);

		box_t* current_box = calloc(1, sizeof(box_t));
		read_header(&binary_payload, current_box);
		dump_box(current_box);

		add_box(parent_box, current_box);

		print_hex((uint8_t*)current_box, 8);

		parent_box = current_box;



	}


	return 0;
}

 *
 */


void __create_binary_payload(char *test_payload_base64, uint8_t **binary_payload, int * binary_payload_size) {
	int test_payload_base64_length = strlen(test_payload_base64);
	int test_payload_binary_size = test_payload_base64_length/2;

	uint8_t *test_payload_binary = calloc(test_payload_binary_size, sizeof(uint8_t));

	for (size_t count = 0; count < test_payload_binary_size; count++) {
	        sscanf(test_payload_base64, "%2hhx", &test_payload_binary[count]);
	        test_payload_base64 += 2;
	}

	*binary_payload = test_payload_binary;
	*binary_payload_size = test_payload_binary_size;
}



#endif


