/*
 * atsc3_a344_receiver_query_api.h
 *
 *  Created on: Sep 7, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_A344_RECEIVER_QUERY_API_H_
#define ATSC3_A344_RECEIVER_QUERY_API_H_

#if defined (__cplusplus)
extern "C" {
#endif


/*	9.2.1 Query Content Advisory Rating API
 *	atsc3_a344_receiver_query_rating_level
 *
 * 	org.atsc.query.ratingLevel
 * 	sample BA to receiver query:
 *
{
    "jsonrpc": "2.0",
    "method": "org.atsc.query.ratingLevel",
    "id": 37
}
 *
 * sample receiver to BA response payload
{
    "jsonrpc": "2.0",
    "result": {
        "rating": "1,'TV-PG-D-L', {0 'TV PG'}{1 'D'}{2 'L'}",
        "contentRating": "1,'TV-G', {0 'TV G'}",
        "blocked": false
	},
	"id": 37
*/

void atsc3_a344_receiver_query_rating_level();

/*
 * 9.2.2 Query Closed Captions Enabled/Disabled API
 *
 * org.atsc.query.cc
 * sample BA to receiver query:
 *
 {
    "jsonrpc": "2.0",
    "method": "org.atsc.query.cc",
    "id": 49
}

 * sample receiver to BA response payload
{
    "jsonrpc": "2.0",
    "result": {"ccEnabled": true},
    "id": 49
}
 */

void atsc3_a344_receiver_query_closed_captions_enabled();

/*
 * 9.2.2 Query Closed Captions Enabled/Disabled API
 *
 * org.atsc.query.cc
 * sample BA to receiver query:
 *
 {
    "jsonrpc": "2.0",
    "method": "org.atsc.query.cc",
    "id": 49
}
 * sample receiver to BA response payload
{
    "jsonrpc": "2.0",
    "result": {"ccEnabled": true},
    "id": 49
}
 */

void atsc3_a344_receiver_query_closed_captions_enabled();

/*
 * 9.2.3 Query Service ID API
 *
 * org.atsc.query.service
 * {
    "jsonrpc": "2.0",
    "method": "org.atsc.query.service",
 *
 * {
    "jsonrpc": "2.0",
    "result": {"service": "https://doi.org/10.5239/8A23-2B0"},
    "id": 55
}
 */

void atsc3_a344_receiver_query_service_id();

/*
 * 9.2.4 Query Language Preferences API
 * org.atsc.query.languages
 *
 * {
    "jsonrpc": "2.0",
    "method": "org.atsc.query.languages",
    "id": 95
}

 * sample receiver to BA response payload
 *
{
    "jsonrpc": "2.0",
    "result": {
        "preferredAudioLang": "es",
        "preferredUiLang": "en",
        "preferredCaptionSubtitleLang": "es"
	},
	"id": 95 }
 */

void atsc3_a344_receiver_query_language_preferences();

/*
 * 9.2.5 Query Caption Display Preferences API
 * org.atsc.query.captionDisplay
 *
 * sample receiver to BA response payload

 {
	"type": "object",
	"properties": {
		"msgType": {
			"enum": ["captionDisplayPrefs"]
		},
		"cta708": {
			"type": "object",
			"properties": {
				"characterColor": {
					"type": "string"
				},
				"characterOpacity": {
					"type": "number"
				},
				"characterSize": {
					"type": "integer"
				},
				"fontStyle": {
					"enum": [
						"Default", "MonospacedSerifs", "ProportionalSerifs", "MonospacedNoSerifs", "ProportionalNoSerifs", "Casual", "Cursive", "SmallCaps"
					]
				},
				"backgroundColor": {
					"type": "string"
				},
				"backgroundOpacity": {
					"type": "number"
				},
				"characterEdge": {
					"enum": [
						"None",
						"Raised", "Depressed", "Uniform", "LeftDropShadow", "RightDropShadow"
					]
				},

				"characterEdgeColor": {
					"type": "string"
				},
				"windowColor": {
					"type": "string"
				},
				"windowOpacity": {
					"type": "number"
				}
			}
		},
		"imsc1": {
			"type": "object",
			"properties": {}
		}
	}
}
 */

void atsc3_a344_receiver_query_caption_display();

/*
 * 9.2.6 Query Audio Accessibility Preferences API
 * org.atsc.query.audioAccessibilityPref"
 *
 * e.g.:
 *
 *  "jsonrpc": "2.0",
    "result": {
        "videoDescriptionService": {
            "enabled": true
        }
},
"id": 90
 */
void atsc3_a344_receiver_query_audio_accessibility_preferences();


/*
 * 9.2.7 Query MPD URL API
 *
 * method: "org.atsc.query.MPDUrl"
 *
 * response:
 * "jsonrpc": "2.0",
    "result": {"MPDUrl": "http://127.0.0.1:8080/10.4/MPD.mpd"},
    "id": 913
}
 */
void atsc3_a344_receiver_query_mvpd_url();

/*
 * 9.2.8 Query Receiver Web Server URI API
 * method: "org.atsc.query.baseURI"
 *
 * responses:
 * {
    "jsonrpc": "2.0",
    "result": {
        "baseURI": "http://localhost:8080/contextA"
    },
"id": 90 }
 */
void atsc3_a344_receiver_query_web_server_url();

/*
 * 9.2.9 Query Alerting Signaling API
 *
 * "method": "org.atsc.query.alertingUrl",
 *
 * {
    "jsonrpc": "2.0",
    "method": "org.atsc.query.alertingUrl",
    "params": {
        "alertingTypes": ["AEAT", "OSN"]
    },
"id": 913

{
    "jsonrpc": "2.0",
    "result": {
        "alertList": [
          { "alertingType": "AEAT",
            "alertingFragment": "<AEAT>...</AEAT>" },
          { "alertingType": "OSN",
            "alertingFragment": "<OSN>...</OSN>",
            "receiveTime": "2017-01-01T23:54:59.590Z" }
        ]
},
"id": 913 }
 */

void atsc3_a344_receiver_query_alerting_signalling();


/*
 * 9.2.10 Query Service Guide URLs API
 *
 *     "method": "org.atsc.query.serviceGuideUrls",
 */

void atsc3_a344_receiver_query_service_guide_url();


/***
 *
 * async notifications:
 * ATSC A/344:2019 ATSC 3.0 Interactive Content 02 May 2019
 *
 *
 * msgType Event Description Reference
 *
 *
ratingChange	Content Advisory Rating Settings Change – a notification that is provided whenever the user changes the Content Advisory Rating settings in the Receiver.  Sec. 9.3.1

ratingBlock		Content Advisory Rating Block Change – a notification that is provided whenever the user changes the content advisory rating settings in the Receiver such that the currently decoding program goes from blocked to unblocked or unblocked to blocked. Sec. 9.3.2
serviceChange 	Service Change – a notification that is provided if a different service is acquired due to user action, and the new service signals the URL of the same application. Sec. 9.3.3
captionState	Caption State – a notification that is provided whenever the user changes the state of closed caption display (either off to on, or on to off). Sec. 9.3.4
langPref 		Language Preference – a notification that is provided whenever the user changes the preferred language.	Sec. 9.3.5
captionDisplayPrefs	Closed Caption display properties preferences.	Sec. 9.3.6
audioAccessibilityPref	Audio Accessibilities preferences.	Sec. 9.3.7
MPDChange	Notification of a change to the broadcast MPD.	Sec. 9.3.8

alertingChange	Alerting Change – a notification that a new version of either the AEAT or OSN messages have been received or if alert filtering preferences have been changed resulting in events becoming unfiltered.	Sec. 9.3.9
contentChange	Content Change – a notification that new content has been placed in the Application Context Cache and may be accessed by the Broadcaster Application.	Sec. 9.3.10
serviceGuideChange	Service Guide Change – a notification that is provided when new ESG fragments have been received.	Sec. 9.3.11
contentRecoveryStateChange	Content Recovery State Change – a notification that is provided whenever use of audio watermark, video watermark, audio fingerprint, and/or video fingerprint for content recovery changes.	Sec. 9.10.4

displayOverrideChange	Display Override Change – a notification that is provided if the display override state or the state of blocked application access to certain resource changes.	Sec. 9.10.5
recoveredComponentInfoChange	Recovered Component Info Change – a notification that is provided if a component of the service being received by the Receiver changes at the upstream.	Sec. 9.10.6
rmpMediaTimeChange	RMP Media Time Change – a notification that is provided periodically during playback.	Sec. 9.14.5
rmpPlaybackStateChange	RMP Playback State Change – a notification that is provided if the playback state changes.	Sec. 9.14.6
rmpPlaybackRateChange	RMP Playback Rate Change – a notification that is provided if playback speed changes.	Sec. 9.14.7
xlinkResolution		Xlink Resolution – a notification that is provided when the RMP encounters a period with an XLink attribute.	Sec. 9.16.1
 */



#if defined (__cplusplus)
}
#endif

#define __A344_RECEIVER_QUERY_ERROR(...) __LIBATSC3_TIMESTAMP_ERROR(__VA_ARGS__);
#define __A344_RECEIVER_QUERY_WARN(...)  __LIBATSC3_TIMESTAMP_WARN(__VA_ARGS__);
#define __A344_RECEIVER_QUERY_INFO(...)  if(_A344_RECEIVER_QUERY_INFO_ENABLED)  { __LIBATSC3_TIMESTAMP_INFO(__VA_ARGS__); }
#define __A344_RECEIVER_QUERY_DEBUG(...) if(_A344_RECEIVER_QUERY_DEBUG_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }
#define __A344_RECEIVER_QUERY_TRACE(...) if(_A344_RECEIVER_QUERY_TRACE_ENABLED) { __LIBATSC3_TIMESTAMP_DEBUG(__VA_ARGS__); }

#endif /* ATSC3_A344_RECEIVER_QUERY_API_H_ */
