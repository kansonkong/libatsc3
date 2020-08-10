/*
 * astc3_lls.h
 *
 *  Created on: Jan 5, 2019
 *      Author: jjustman
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "atsc3_utils.h"
#include "atsc3_lls_types.h"
#include "atsc3_gzip.h"
#include "xml.h"

#ifndef MODULES_DEMUX_MMT_ASTC3_LLS_H_
#define MODULES_DEMUX_MMT_ASTC3_LLS_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int _LLS_INFO_ENABLED;
extern int _LLS_DEBUG_ENABLED;
extern int _LLS_TRACE_ENABLED;


#define LLS_DST_ADDR 3758102332
#define LLS_DST_PORT 4937

lls_table_t* lls_create_base_table(block_t* lls_packet_block);
lls_table_t* lls_create_xml_table(block_t* lls_packet_block);

lls_table_t* __lls_table_create(block_t* lls_packet_block);

lls_table_t* atsc3_lls_table_parse_raw_xml(atsc3_lls_table_t* lls_table);

lls_table_t* lls_table_create_or_update_from_lls_slt_monitor(lls_slt_monitor_t* lls_slt_monitor, block_t* lls_packet_block);
lls_table_t* lls_table_create_or_update_from_lls_slt_monitor_with_metrics(lls_slt_monitor_t* lls_slt_monitor, block_t* lls_packet_block, uint32_t* parsed, uint32_t* parsed_update, uint32_t* parsed_error);
lls_table_t* atsc3_lls_table_create_or_update_from_lls_slt_monitor_with_metrics_single_table(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_table_t* lls_table_new, uint32_t* parsed, uint32_t* parsed_update, uint32_t* parsed_error);
lls_table_t* atsc3_lls_table_create_or_update_from_lls_slt_monitor_dispatcher(lls_slt_monitor_t* lls_slt_monitor, lls_table_t* lls_table);

void lls_table_free(lls_table_t** lls_table_p);
int  lls_create_table_type_instance(lls_table_t* lls_table, xml_node_t* xml_node);
void lls_dump_instance_table(lls_table_t *base_table);

//xml parsing methods
xml_document_t* xml_payload_document_parse(uint8_t *xml, int xml_size);
xml_node_t* xml_payload_document_extract_root_node(xml_document_t*);

int build_rrt_table(lls_table_t* lls_table, xml_node_t* xml_root);
int build_system_time_table(lls_table_t* lls_table, xml_node_t* xml_root);
int build_aeat_table(lls_table_t* lls_table, xml_node_t* xml_root);
int build_onscreen_message_notification_table(lls_table_t* lls_table, xml_node_t* xml_root);

#define _LLS_PRINTLN(...) printf(__VA_ARGS__);printf("%s%s","\r","\n")
#define _LLS_PRINTF(...)  printf(__VA_ARGS__);

#define _LLS_ERROR(...)   printf("%s:%d:ERROR:%.4f: ",__FILE__,__LINE__, gt());_LLS_PRINTLN(__VA_ARGS__);
#define _LLS_WARN(...)    printf("%s:%d:WARN:%.4f: ",__FILE__,__LINE__, gt());_LLS_PRINTLN(__VA_ARGS__);
#define _LLS_INFO(...)    if(_LLS_INFO_ENABLED) { printf("%s:%d:INFO:%.4f: ",__FILE__,__LINE__, gt());_LLS_PRINTLN(__VA_ARGS__); }
#define _LLS_INFO_I(...)  if(_LLS_INFO_ENABLED) { printf(" "); _LLS_PRINTLN(__VA_ARGS__); }

#define _LLS_DEBUG(...)   if(_LLS_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt());_LLS_PRINTLN(__VA_ARGS__); }
#define _LLS_DEBUGF(...)  if(_LLS_DEBUG_ENABLED) { printf("%s:%d:DEBUG:%.4f: ",__FILE__,__LINE__, gt());_LLS_PRINTF(__VA_ARGS__); }
#define _LLS_DEBUGA(...)  if(_LLS_DEBUG_ENABLED) { _LLS_PRINTF(__VA_ARGS__); }
#define _LLS_DEBUGN(...)  if(_LLS_DEBUG_ENABLED) { _LLS_PRINTLN(__VA_ARGS__); }
#define _LLS_DEBUGNT(...) if(_LLS_DEBUG_ENABLED) { _LLS_PRINTF(" ");_LLS_PRINTLN(__VA_ARGS__); }

#define _LLS_TRACE(...)   if(_LLS_TRACE_ENABLED) { printf("%s:%d:TRACE:%.4f: ",__FILE__,__LINE__, gt());_LLS_PRINTLN(__VA_ARGS__); }
#define _LLS_TRACEF(...)  if(_LLS_TRACE_ENABLED) {  printf("%s:%d:TRACE:%.4f: ",__FILE__,__LINE__, gt());_LLS_PRINTF(__VA_ARGS__); }
#define _LLS_TRACEA(...)  if(_LLS_TRACE_ENABLED) { _LLS_PRINTF(__VA_ARGS__); }
#define _LLS_TRACEN(...)  if(_LLS_TRACE_ENABLED) { _LLS_PRINTLN(__VA_ARGS__); }

/**
 *
 * Raw SLT example:

0000   01 01 00 02 1f 8b 08 08 92 17 18 5c 00 03 53 4c   ...........\..SL
0010   54 00 b5 d5 5b 6f 82 30 14 00 e0 f7 fd 0a d2 e7   T.µÕ[o.0..à÷ý.Òç
0020   0d 4a 41 37 0d 60 9c 9a c5 44 8d 09 2e d9 9b a9   .JA7.`..ÅD...Ù.©
0030   d0 61 17 68 5d 5b cd fc f7 3b a8 cb e2 bc 44 16   Ða.h][Íü÷;¨Ëâ¼D.
0040   7d 22 9c 4b cf e9 f7 00 41 eb ab c8 ad 15 53 9a   }".KÏé÷.Aë«È..S.
0050   4b 11 22 d7 c6 c8 62 22 91 29 17 59 88 96 e6 fd   K."×ÆÈb".).Y..æý
0060   e1 09 b5 a2 bb 20 1e 4c 2c a8 14 3a 44 86 66 4d   á.µ¢» .L,¨.:D.fM
0070   6a 74 62 4b 95 dd 13 ec d6 9b 6f c3 41 9c cc 59   jtbK.Ý.ìÖ.oÃA.ÌY
0080   41 b5 d3 9e c4 1d cf e9 b2 9c c3 99 6b 07 da 1c   AµÓ.Ä.Ïé².Ã.k.Ú.
0090   38 d3 41 d6 4c f3 34 44 35 8c a2 20 66 6a c5 13   8ÓAÖLó4D5.¢ fjÅ.
00a0   66 e9 ed b3 0f 71 17 63 17 59 59 2e 67 34 df a5   féí³.q.c.YY.g4ß¥
00b0   fb 5d 98 af c4 66 54 73 57 ca 53 78 65 05 9b 16   û].¯ÄfTsWÊSxe...
00c0   85 99 42 43 41 3f a4 ea cc a9 10 2c 1f c9 f2 18   ..BCA?¤êÌ©.,.Éò.
00d0   88 71 b1 1f 43 3f 83 3a d0 9b 49 b5 de c6 e6 52   .q±.C?.:Ð.IµÞÆæR
00e0   99 dd a8 11 2d 58 88 da 93 de b0 67 0d 87 13 ab   .Ý¨.-X.Ú.Þ°g...«
00f0   4c e7 26 5e 25 31 fb 1c 2d 8b 10 95 5b 3f 2b 49   Lç&^%1û.-...[?+I
0100   d3 84 ea 4d 9c 67 82 e6 40 04 75 7a ac a4 91 89   Ó.êM.g.æ@.uz¬¤..
0110   cc 43 44 ca 3e dd 65 da 70 41 0d 80 f6 17 ed 34   ÌCDÊ>ÝeÚpA..ö.í4
0120   55 4c 83 1a f1 1a 36 a9 d5 6c 17 db ee df b2 d7   UL..ñ.6©Õl.Ûîß²×
0130   74 31 86 6d 80 67 eb 00 d9 58 2e 15 18 fc f6 bb   t1.m.gë.ÙX...üö»
0140   8f c4 76 eb 36 c1 65 bf 13 05 ce 6e f7 53 9c a4   .Ävë6Áe¿..În÷S.¤
0150   2a 27 b9 8c 93 54 e7 24 b7 e5 3c 28 db e3 24 d7   *'¹..Tç$·å<(Ûã$×
0160   e1 f4 aa 72 7a 97 71 7a d5 39 bd db 72 7a 67 39   áôªrz.qzÕ9½Ûrzg9
0170   bd eb 70 fa 55 39 fd cb 38 fd ea 9c fe 6d 39 fd   ½ëpúU9ýË8ýê.þm9ý
0180   b3 9c fe 15 38 6b 18 37 2e e3 64 3a 3b e2 e3 1f   ³.þ.8k.7.ãd:;âã.
0190   f3 e9 c5 2f ff 74 39 f8 ba 1d 71 21 d8 6e 9c 76   óéÅ/ÿt9øº.q!Øn.v
01a0   21 9b 0b 55 73 29 ff 34 d1 dd 37 2e 0e fb 8f ce   !..Us)ÿ4ÑÝ7..û.Î
01b0   06 00 00                                          ...

Raw SystemTime message:
0000   01 00 5e 00 17 3c 00 1c 42 22 fa 9f 08 00 45 00   ..^..<..B"ú...E.
0010   00 dd 01 00 40 00 01 11 c0 27 c0 a8 00 04 e0 00   .Ý..@...À'À¨..à.
0020   17 3c 90 8b 13 49 00 c9 2d 76 03 01 00 01 1f 8b   .<...I.É-v......
0030   08 08 97 17 18 5c 00 03 53 79 73 74 65 6d 54 69   .....\..SystemTi
0040   6d 65 00 35 8d cb 0a 82 40 14 40 f7 7e c5 70 f7   me.5.Ë..@.@÷~Åp÷
0050   7a 0b 89 22 7c 10 15 14 28 05 63 50 cb 61 bc 3e   z.."|...(.cPËa¼>
0060   60 1c c3 b9 66 fe 7d 6e da 1e 38 e7 44 e9 b7 33   `.Ã¹fþ}nÚ.8çDé·3
0070   e2 43 83 6b 7b 1b c3 3a 58 81 20 ab fb b2 b5 75   âC.k{.Ã:X. «û²µu
0080   0c 23 57 fe 0e d2 c4 8b e4 ec 98 ba a2 ed 48 2c   .#Wþ.ÒÄ.äì.º¢íH,
0090   82 75 31 34 cc ef 3d e2 34 4d 81 62 a7 83 7e a8   .u14Ìï=â4M.b§.~¨
00a0   f1 99 67 52 37 d4 29 87 87 42 1e 43 3c 91 69 97   ñ.gR7Ô)..B.C<.i.
00b0   f8 8c f2 25 8b 6b 7e c6 65 80 20 f4 38 0c 64 f9   ø.ò%.k~Æe. ô8.dù
00c0   c1 fa 56 55 8e 38 86 70 0b 62 64 9d f5 5a 99 3f   ÁúVU.8.p.bd.õZ.?
00d0   f3 ef c5 e6 02 a2 74 92 15 8f cb b2 52 c6 11 60   óïÅæ.¢t...Ë²RÆ.`
00e0   e2 fd 00 35 18 c1 1f b6 00 00 00                  âý.5.Á.¶...

see atsc3_lls_test.c for base64 string getters of test payloads
 *
 */


#ifdef __cplusplus
}
#endif
#endif /* MODULES_DEMUX_MMT_ASTC3_LLS_H_ */
