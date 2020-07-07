/*
 * atsc3_monitor_events_sls.h
 *
 *  Created on: Jul 7, 2020
 *      Author: jjustman
 *
 *  Purpose:
 *
 *  	monitor event dispatcher callback signatures for relevant SLS (service layer signalling)
 *
 */

#ifndef ATSC3_MONITOR_EVENTS_SLS_H
#define ATSC3_MONITOR_EVENTS_SLS_H

#include "atsc3_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

//jjustman-2020-07-01 #WI - todo: dispatch HELD block_t* payload to application callback

//see atsc3_monitor_events_alc.h for package extraction methods and definitions

//<?xml version="1.0" encoding="UTF-8"?>
//<HELD xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/AppSignaling/HELD/1.0/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
//<HTMLEntryPackage appContextId="tag:sinclairplatform.com,2020:KSNV:2089" appRendering="false" bcastEntryPackageUrl="App.pkg" bcastEntryPageUrl="index.html" coupledServices="5004"/>
//</HELD>

typedef void (*atsc3_sls_on_held_trigger_received_f)(uint16_t service_id, block_t* held_payload);

//jjustman-2020-07-07: TODO: remaining SLS metadata object types dispatched as event
// 							per A/331:2020 - Table 6.17 Metadata Object Types

#ifdef __cplusplus
}
#endif
    

#endif /* ATSC3_MONITOR_EVENTS_SLS_H */
