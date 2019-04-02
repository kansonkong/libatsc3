/*
 * atsc3_route_mpd.h
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 *
 *      <?xml version="1.0" encoding="UTF-8"?>
<MPD availabilityStartTime="2019-02-19T21:40:50Z" maxSegmentDuration="PT2S" minBufferTime="PT2S" minimumUpdatePeriod="PT10S" profiles="urn:mpeg:dash:profile:isoff-live:2011" publishTime="2019-02-19T21:40:50Z" timeShiftBufferDepth="PT20S" type="dynamic" xmlns="urn:mpeg:dash:schema:mpd:2011" xmlns:cenc="urn:mpeg:cenc:2013" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemalocation="urn:mpeg:dash:schema:mpd:2011 DASH-MPD.xsd">
   <Period id="P21" start="PT2155S" xlink:actuate="onLoad" xlink:href="tag:atsc.org,2016:xlinkhttps://atsc3vod.npgco.com/crengine?clientid=%clientid%&amp;uniqueid=775&amp;thisname=SNPR55030&amp;nextid=774&amp;nextname=SKPR60825" xmlns:xlink="http://www.w3.org/1999/xlink">
      <AdaptationSet contentType="video" id="0" maxFrameRate="30000/1001" maxHeight="1080" maxWidth="1920" mimeType="video/mp4" minFrameRate="30000/1001" minHeight="1080" minWidth="1920" par="16:9" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
         <Representation bandwidth="7000000" codecs="hev1.2.4.L120.9" frameRate="30000/1001" height="1080" id="0" sar="1:1" width="1920">
            <SegmentTemplate duration="2002000" initialization="test-$RepresentationID$-init.mp4v" media="test-$RepresentationID$-$Number$.mp4v" presentationTimeOffset="2155019000" startNumber="1088" timescale="1000000"/>
         </Representation>
      </AdaptationSet>
      <AdaptationSet contentType="audio" id="1" lang="eng" mimeType="audio/mp4" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
         <Representation audioSamplingRate="48000" bandwidth="387000" codecs="mp4a.40.2" id="1">
            <AudioChannelConfiguration schemeIdUri="urn:mpeg:dash:23003:3:audio_channel_configuration:2011" value="2"/>
            <SegmentTemplate duration="2002000" initialization="test-$RepresentationID$-init.mp4a" media="test-$RepresentationID$-$Number$.mp4a" presentationTimeOffset="2155019000" startNumber="1088" timescale="1000000"/>
         </Representation>
      </AdaptationSet>
   </Period>
   <Period id="P22" start="PT2185S" xlink:actuate="onLoad" xlink:href="tag:atsc.org,2016:xlinkhttps://atsc3vod.npgco.com/crengine?clientid=%clientid%&amp;uniqueid=776&amp;thisname=SKPR60825&amp;nextid=775&amp;nextname=SNPR55030" xmlns:xlink="http://www.w3.org/1999/xlink">
      <AdaptationSet contentType="video" id="0" maxFrameRate="30000/1001" maxHeight="1080" maxWidth="1920" mimeType="video/mp4" minFrameRate="30000/1001" minHeight="1080" minWidth="1920" par="16:9" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
         <Representation bandwidth="7000000" codecs="hev1.2.4.L120.9" frameRate="30000/1001" height="1080" id="0" sar="1:1" width="1920">
            <SegmentTemplate duration="2002000" initialization="test-$RepresentationID$-init.mp4v" media="test-$RepresentationID$-$Number$.mp4v" presentationTimeOffset="2185049000" startNumber="1103" timescale="1000000"/>
         </Representation>
      </AdaptationSet>
      <AdaptationSet contentType="audio" id="1" lang="eng" mimeType="audio/mp4" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
         <Representation audioSamplingRate="48000" bandwidth="387000" codecs="mp4a.40.2" id="1">
            <AudioChannelConfiguration schemeIdUri="urn:mpeg:dash:23003:3:audio_channel_configuration:2011" value="2"/>
            <SegmentTemplate duration="2002000" initialization="test-$RepresentationID$-init.mp4a" media="test-$RepresentationID$-$Number$.mp4a" presentationTimeOffset="2185049000" startNumber="1103" timescale="1000000"/>
         </Representation>
      </AdaptationSet>
   </Period>
   <Period id="P23" start="PT2215S" xlink:actuate="onLoad" xlink:href="tag:atsc.org,2016:xlinkhttps://atsc3vod.npgco.com/crengine?clientid=%clientid%&amp;uniqueid=777&amp;thisname=SNPR60093&amp;nextid=776&amp;nextname=SKPR60825" xmlns:xlink="http://www.w3.org/1999/xlink">
      <AdaptationSet contentType="video" id="0" maxFrameRate="30000/1001" maxHeight="1080" maxWidth="1920" mimeType="video/mp4" minFrameRate="30000/1001" minHeight="1080" minWidth="1920" par="16:9" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
         <Representation bandwidth="7000000" codecs="hev1.2.4.L120.9" frameRate="30000/1001" height="1080" id="0" sar="1:1" width="1920">
            <SegmentTemplate duration="2002000" initialization="test-$RepresentationID$-init.mp4v" media="test-$RepresentationID$-$Number$.mp4v" presentationTimeOffset="2215112000" startNumber="1119" timescale="1000000"/>
         </Representation>
      </AdaptationSet>
      <AdaptationSet contentType="audio" id="1" lang="eng" mimeType="audio/mp4" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
         <Representation audioSamplingRate="48000" bandwidth="387000" codecs="mp4a.40.2" id="1">
            <AudioChannelConfiguration schemeIdUri="urn:mpeg:dash:23003:3:audio_channel_configuration:2011" value="2"/>
            <SegmentTemplate duration="2002000" initialization="test-$RepresentationID$-init.mp4a" media="test-$RepresentationID$-$Number$.mp4a" presentationTimeOffset="2215112000" startNumber="1119" timescale="1000000"/>
         </Representation>
      </AdaptationSet>
   </Period>
</MPD>

ATSC3_VECTOR_BUILDER_STRUCT
 *
 */

#ifndef ATSC3_ROUTE_MPD_H_
#define ATSC3_ROUTE_MPD_H_

/**
 *  xlink:href="tag:atsc.org,2016:xlinkhttps://atsc3vod.npgco.com/crengine?clientid=%clientid%&amp;uniqueid=775&amp;thisname=SNPR55030&amp;nextid=774&amp;nextname=SKPR60825"
 *    xmlns:xlink="http://www.w3.org/1999/xlink">
 */
typedef struct atsc3_xlink {
	char* actuate;
	char* href;
	char* xlink;

} atsc3_xlink_t;

typedef struct atsc3_frame_rate {
	uint32_t numerator;
	uint32_t denom;

} atsc3_frame_rate_t;


/*
 *  <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
 */

typedef struct atsc3_route_role {
	char* scheme_id_uri;
	char* value;
} atsc3_route_role_t;

/*
<Representation audioSamplingRate="48000" bandwidth="387000" codecs="mp4a.40.2" id="1">
*/

typedef struct atsc3_route_representation {
	uint32_t		audio_sampling_rate;
	uint32_t		bandwidth;
	char*			codecs;
	uint32_t 		id;
} atsc3_route_representation_t;

/*
<AudioChannelConfiguration schemeIdUri="urn:mpeg:dash:23003:3:audio_channel_configuration:2011" value="2"/>
*/

typedef struct atsc3_route_audio_channel_configuration {
	char* scheme_id_uri;
	uint32_t value;
} atsc3_route_audio_channel_configuration_t;

/*
<SegmentTemplate duration="2002000" initialization="test-$RepresentationID$-init.mp4a"
media="test-$RepresentationID$-$Number$.mp4a" presentationTimeOffset="2185049000" startNumber="1103" timescale="1000000"/>
 */

typedef struct atsc3_route_segment_template {
	uint32_t 	duration;
	char*		initialization;
	char*		media;
	uint32_t	presentation_time_offset;
	uint32_t	start_number;
	uint32_t	timescale;

} atsc3_route_segment_template_t;
/**
 *       <AdaptationSet contentType="video" id="0"
 *       maxFrameRate="30000/1001" maxHeight="1080"
 *       maxWidth="1920" mimeType="video/mp4"
 *       minFrameRate="30000/1001" minHeight="1080" minWidth="1920"
 *       par="16:9" segmentAlignment="true" startWithSAP="1">
 *
 */

typedef struct atsc3_route_adaptation_set {
	char*										content_type;
	uint32_t									id;
	atsc3_frame_rate_t 							max_frame_rate;
	uint32_t									max_height;
	uint32_t									max_width;
	char*										mime_type;
	atsc3_frame_rate_t							min_frame_rate;
	uint32_t									min_height;
	uint32_t									min_width;
	char*										par;
	bool										segment_alignment;
	bool										start_with_sap;
	atsc3_route_role_t							atsc3_route_role;
	atsc3_route_representation_t 				atsc3_route_representation;
	atsc3_route_audio_channel_configuration_t	atsc3_route_audio_channel_configuration;
	atsc3_route_segment_template_t				atsc3_route_segment_template;

} atsc3_route_adaptation_set_t;

/*
 *    <Period id="P21" start="PT2155S" xlink:actuate="onLoad"
 *    xlink:href="tag:atsc.org,2016:xlinkhttps://atsc3vod.npgco.com/crengine?clientid=%clientid%&amp;uniqueid=775&amp;thisname=SNPR55030&amp;nextid=774&amp;nextname=SKPR60825"
 *    xmlns:xlink="http://www.w3.org/1999/xlink">
 *
 */
typedef struct atsc3_route_period {
	char* 			id;
	char*			start;
	atsc3_xlink_t* 	atsc3_xlink;
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_period, atsc3_route_adaptation_set)

} atsc3_route_period_t;

//<MPD availabilityStartTime="2019-02-19T21:40:50Z" maxSegmentDuration="PT2S" minBufferTime="PT2S" minimumUpdatePeriod="PT10S"
//profiles="urn:mpeg:dash:profile:isoff-live:2011" publishTime="2019-02-19T21:40:50Z" timeShiftBufferDepth="PT20S" type="dynamic"
//xmlns="urn:mpeg:dash:schema:mpd:2011" xmlns:cenc="urn:mpeg:cenc:2013" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
//xsi:schemalocation="urn:mpeg:dash:schema:mpd:2011 DASH-MPD.xsd">

typedef struct atsc3_route_mpd {
	char* availability_start_time;
	char* max_segment_duration;
	char* min_buffer_time;
	char* minimum_update_period;
	char* profiles;
	char* publish_time;
	char* time_shift_buffer_depth;
	char* type;

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_route_mpd, atsc3_route_period)

} atsc3_route_mpd_t;


#include "atsc3_utils.h"
#include "atsc3_vector_builder.h"




#endif /* ATSC3_ROUTE_MPD_H_ */
