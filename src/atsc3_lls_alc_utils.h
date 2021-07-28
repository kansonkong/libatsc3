/*
 * atsc3_lls_alc_tools.h
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */


#ifndef __LLS_SESSION_RELAX_SOURCE_IP_CHECK__
#define __LLS_SESSION_RELAX_SOURCE_IP_CHECK__ true
#endif

#ifndef ATSC3_LLS_ALC_TOOLS_H_
#define ATSC3_LLS_ALC_TOOLS_H_
#include <assert.h>
#include <stdbool.h>

#include "atsc3_lls.h"
#include "atsc3_lls_types.h"
#include "atsc3_alc_session.h"
#include "atsc3_lls_slt_parser.h"
#include "atsc3_lls_sls_parser.h"
#include "atsc3_logging_externs.h"

#if defined (__cplusplus)
extern "C" {
#endif

lls_sls_alc_monitor_t* lls_sls_alc_monitor_create(void);

void lls_slt_alc_session_and_monitor_mark_all_atsc3_lls_slt_service_as_transient_stale(lls_slt_monitor_t* lls_slt_monitor);
void lls_slt_alc_session_and_monitor_remove_all_atsc3_lls_slt_service_with_matching_transient_stale(lls_slt_monitor_t* lls_slt_monitor);

/*
 * jjustman-2021-06-22:
 *  SLT: invoked from atsc3_lls_slt_parser.c:lls_slt_table_perform_update
 *  MMT: mmt-related ROUTE handling for NRT <ROUTEComponent>... elements
 *   - AND -
 *  ROUTE: SLS:  added into atsc3_route_sls_processor.c:atsc3_route_sls_process_from_alc_packet_and_file for when may have an additional  have
 *      atsc3_sls_metadata_fragments_pending->atsc3_route_s_tsid
 *
 *      Example:
 *
 *      <?xml version="1.0" encoding="UTF-8"?>
<S-TSID xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/S-TSID/1.0/" xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/" xmlns:fdt="urn:ietf:params:xml:ns:fdt">
   <RS sIpAddr="172.16.200.1" dIpAddr="239.255.3.2" dPort="5002">
      <LS tsi="100">
         <SrcFlow rt="true">
            <EFDT>
               <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="1025000" afdt:fileTemplate="video-$TOI$.mp4v">
                  <fdt:File TOI="1" Content-Location="video-init.mp4" />
               </FDT-Instance>
            </EFDT>
            <ContentInfo>
               <MediaInfo contentType="video" repId="Video1_1" />
            </ContentInfo>
            <Payload codePoint="8" formatId="1" frag="0" order="true" />
         </SrcFlow>
      </LS>
      <LS tsi="200">
         <SrcFlow rt="true">
            <EFDT>
               <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="96000" afdt:fileTemplate="a2_2-$TOI$.m4s">
                  <fdt:File TOI="1" Content-Location="a2_2-init.mp4" />
               </FDT-Instance>
            </EFDT>
            <ContentInfo>
               <MediaInfo contentType="audio" repId="a2_2" />
            </ContentInfo>
            <Payload codePoint="8" formatId="1" frag="0" order="true" />
         </SrcFlow>
      </LS>
      <LS tsi="300">
         <SrcFlow rt="true">
            <EFDT>
               <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="50000" afdt:fileTemplate="d4_3-$TOI$.m4s">
                  <fdt:File TOI="1" Content-Location="d4_3-init.mp4" />
               </FDT-Instance>
            </EFDT>
            <ContentInfo>
               <MediaInfo contentType="subtitles" repId="d4_3" />
            </ContentInfo>
            <Payload codePoint="8" formatId="1" frag="0" order="true" />
         </SrcFlow>
      </LS>
      <LS tsi="21840">
         <SrcFlow>
            <EFDT>
               <FDT-Instance Expires="4294967295">
                  <fdt:File TOI="343" Content-Location="single-alert.pkg" Content-Length="3150" Content-Type="multipart/related" afdt:appContextIdList="app:sinclairplatform.com:dma:839" />
               </FDT-Instance>
            </EFDT>
            <Payload codePoint="3" formatId="3" frag="0" order="true" />
         </SrcFlow>
      </LS>
      <LS tsi="23165">
         <SrcFlow>
            <EFDT>
               <FDT-Instance Expires="4294967295">
                  <fdt:File TOI="3" Content-Location="weather-latest.pkg" Content-Length="5322842" Content-Type="multipart/related" afdt:appContextIdList="app:sinclairplatform.com:dma:839" />
               </FDT-Instance>
            </EFDT>
            <Payload codePoint="3" formatId="3" frag="0" order="true" />
         </SrcFlow>
      </LS>
      <LS tsi="23192">
         <SrcFlow>
            <EFDT>
               <FDT-Instance Expires="4294967295">
                  <fdt:File TOI="3" Content-Location="App.pkg" Content-Length="1589633" Content-Type="multipart/related" afdt:appContextIdList="app:sinclairplatform.com:dma:839" />
               </FDT-Instance>
            </EFDT>
            <Payload codePoint="3" formatId="3" frag="0" order="true" />
         </SrcFlow>
      </LS>
      <LS tsi="23233">
         <SrcFlow>
            <EFDT>
               <FDT-Instance Expires="4294967295">
                  <fdt:File TOI="3" Content-Location="Alert.pkg" Content-Length="2412" Content-Type="multipart/related" afdt:appContextIdList="app:sinclairplatform.com:dma:839" />
               </FDT-Instance>
            </EFDT>
            <Payload codePoint="3" formatId="3" frag="0" order="true" />
         </SrcFlow>
      </LS>
      <LS tsi="23236">
         <SrcFlow>
            <EFDT>
               <FDT-Instance Expires="4294967295">
                  <fdt:File TOI="3" Content-Location="weather-7day.pkg" Content-Length="42426" Content-Type="multipart/related" afdt:appContextIdList="app:sinclairplatform.com:dma:839" />
               </FDT-Instance>
            </EFDT>
            <Payload codePoint="3" formatId="3" frag="0" order="true" />
         </SrcFlow>
      </LS>
   </RS>
   &gt;
   ***
   <RS dIpAddr="239.255.50.69" dPort="41337">
      <LS tsi="0">
         <SrcFlow>
            <EFDT>
               <FDT-Instance Expires="4294967295">
                  <fdt:File TOI="1" Content-Location="package-lock.json" Content-Length="383556" afdt:appContextIdList="app:sinclairplatform.com:dma:839" />
               </FDT-Instance>
            </EFDT>
            <Payload codePoint="1" formatId="1" frag="0" order="true" />
         </SrcFlow>
      </LS>
   </RS>
</S-TSID>

 */
lls_sls_alc_session_t* lls_slt_alc_session_find_or_create(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service);
lls_sls_alc_session_t* lls_slt_alc_session_find_or_create_from_ip_udp_values(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service, uint32_t sls_destination_ip_address, uint16_t sls_destination_udp_port, uint32_t sls_source_ip_address);
lls_sls_alc_monitor_t* atsc3_lls_sls_alc_monitor_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port);

//jjustman-202-03-28 - internal methods for lls_sls_alc_session creation without adding to a monitor
lls_sls_alc_session_t* lls_slt_alc_session_create(atsc3_lls_slt_service_t* atsc3_lls_slt_service);
lls_sls_alc_session_t* lls_slt_alc_session_create_from_ip_and_port_values(atsc3_lls_slt_service_t* atsc3_lls_slt_service, uint32_t sls_destination_ip_address, uint16_t sls_destination_udp_port, uint32_t sls_source_ip_address);

//jjustman-2020-03-25: TODO - deprecate single lls_sls_alc_session resolution, instead use lls_sls_alc_montitor_t for alc session extraction
lls_sls_alc_session_t* lls_slt_alc_session_find(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service);
lls_sls_alc_session_t* lls_slt_alc_session_find_from_udp_packet(lls_slt_monitor_t* lls_slt_monitor, uint32_t src_ip_addr, uint32_t dst_ip_addr, uint16_t dst_port);
lls_sls_alc_session_t* lls_slt_alc_session_find_from_service_id(lls_slt_monitor_t* lls_slt_monitor, uint16_t service_id);

void lls_sls_alc_update_s_tsid_RS_dIpAddr_dPort_if_missing(udp_flow_t* udp_flow, lls_sls_alc_monitor_t* lls_sls_alc_monitor, atsc3_route_s_tsid_t* atsc3_route_s_tsid);

void lls_sls_alc_update_all_mediainfo_flow_v_from_route_s_tsid(lls_sls_alc_monitor_t* lls_sls_alc_monitor, atsc3_route_s_tsid_t* atsc3_route_s_tsid);
int lls_sls_alc_add_additional_ip_flows_from_route_s_tsid(lls_slt_monitor_t* lls_slt_monitor, lls_sls_alc_monitor_t* lls_sls_alc_monitor, atsc3_route_s_tsid_t* atsc3_route_s_tsid);

void lls_slt_alc_session_remove(lls_slt_monitor_t* lls_slt_monitor, atsc3_lls_slt_service_t* atsc3_lls_slt_service);

lls_sls_alc_monitor_t* lls_monitor_sls_alc_session_create(atsc3_lls_slt_service_t* atsc3_lls_slt_service);

void lls_sls_alc_session_free(lls_sls_alc_session_t** lls_session_ptr);


#if defined (__cplusplus)
}
#endif



#define _ATSC3_LLS_ALC_UTILS_ERROR(...)  __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define _ATSC3_LLS_ALC_UTILS_WARN(...)   __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define _ATSC3_LLS_ALC_UTILS_INFO(...)   if(_LLS_ALC_UTILS_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__);  }
#define _ATSC3_LLS_ALC_UTILS_DEBUG(...)  if(_LLS_ALC_UTILS_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define _ATSC3_LLS_ALC_UTILS_TRACE(...)  if(_LLS_ALC_UTILS_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_TRACE(__VA_ARGS__); }

#endif /* ATSC3_LLS_ALC_TOOLS_H_ */
