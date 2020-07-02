Content-Type:multipart/related;
 boundary="boundary-content";
 type=application/mbms-envelope+xml

--boundary-content
Content-Type:application/mbms-envelope+xml
Content-Location:envelope.xml

<?xml version="1.0" encoding="UTF-8"?>
<metadataEnvelope xmlns="urn:3gpp:metadata:2005:MBMS:envelope">
    <item metadataURI="usbd.rusd" contentType="application/route-usd+xml" version="38"/>
    <item metadataURI="stsid.sls" contentType="application/route-s-tsid+xml" version="122"/>
    <item metadataURI="mpd.mpd" contentType="application/dash+xml" version="145"/>
    <item metadataURI="held.held" contentType="application/atsc-held+xml" version="1"/>
</metadataEnvelope>

--boundary-content
Content-Type:application/route-usd+xml; charset=utf-8
Content-Location:usbd.rusd

<?xml version="1.0" encoding="UTF-8"?>
<BundleDescriptionROUTE xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ROUTEUSD/1.0/">
    <UserServiceDescription serviceId="5004">
        <DeliveryMethod>
            <BroadcastAppService>
                <BasePattern>a0-a02_2-</BasePattern>
                <BasePattern>a1-a13_3-</BasePattern>
                <BasePattern>d4_4-</BasePattern>
                <BasePattern>video-</BasePattern>
            </BroadcastAppService>
        </DeliveryMethod>
    </UserServiceDescription>
</BundleDescriptionROUTE>

--boundary-content
Content-Type:application/route-s-tsid+xml; charset=utf-8
Content-Location:stsid.sls

<?xml version="1.0" encoding="UTF-8"?>
<S-TSID xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/S-TSID/1.0/" xmlns:afdt="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ATSC-FDT/1.0/" xmlns:fdt="urn:ietf:params:xml:ns:fdt">
    <RS sIpAddr="172.16.200.1" dIpAddr="239.255.50.4" dPort="5004">
        <LS tsi="100">
            <SrcFlow rt="true">
                <EFDT>
                    <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="5900000" afdt:fileTemplate="video-$TOI$.mp4v">
                        <fdt:File TOI="1" Content-Location="video-init.mp4v"/>
                    </FDT-Instance>
                </EFDT>
                <ContentInfo>
                    <MediaInfo contentType="video" repId="Video1_1">
                        <ContentRating value="1,'TV-PG',{0 'TV-PG'}"/>
                    </MediaInfo>
                </ContentInfo>
                <Payload codePoint="8" formatId="1" frag="0" order="true"/>
            </SrcFlow>
        </LS>
        <LS tsi="200">
            <SrcFlow rt="true">
                <EFDT>
                    <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="96000" afdt:fileTemplate="a0-a02_2-$TOI$.m4s">
                        <fdt:File TOI="1" Content-Location="a0-a02_2-init.mp4"/>
                    </FDT-Instance>
                </EFDT>
                <ContentInfo>
                    <MediaInfo contentType="audio" repId="a02_2"/>
                </ContentInfo>
                <Payload codePoint="8" formatId="1" frag="0" order="true"/>
            </SrcFlow>
        </LS>
        <LS tsi="201">
            <SrcFlow rt="true">
                <EFDT>
                    <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="32000" afdt:fileTemplate="a1-a13_3-$TOI$.m4s">
                        <fdt:File TOI="1" Content-Location="a1-a13_3-init.mp4"/>
                    </FDT-Instance>
                </EFDT>
                <ContentInfo>
                    <MediaInfo contentType="audio" repId="a13_3"/>
                </ContentInfo>
                <Payload codePoint="8" formatId="1" frag="0" order="true"/>
            </SrcFlow>
        </LS>
        <LS tsi="300">
            <SrcFlow rt="true">
                <EFDT>
                    <FDT-Instance Expires="4294967295" afdt:efdtVersion="1" afdt:maxTransportSize="140000" afdt:fileTemplate="d4_4-$TOI$.m4s">
                        <fdt:File TOI="1" Content-Location="d4_4-init.mp4"/>
                    </FDT-Instance>
                </EFDT>
                <ContentInfo>
                    <MediaInfo contentType="subtitles" repId="d4_4"/>
                </ContentInfo>
                <Payload codePoint="8" formatId="1" frag="0" order="true"/>
            </SrcFlow>
        </LS>
        <LS tsi="1166">
            <SrcFlow>
                <EFDT>
                    <FDT-Instance Expires="4294967295">
                        <fdt:File TOI="3" Content-Location="Alert.pkg" Content-Type="multipart/related" afdt:appContextIdList="tag:sinclairplatform.com,2020:KSNV:2089 tag:sinclairplatform.com,2020:KLAS:2081 tag:sinclairplatform.com,2020:KTNV:2085 tag:sinclairplatform.com,2020:KVCW:2091"/>
                    </FDT-Instance>
                </EFDT>
                <Payload codePoint="3" formatId="3" frag="0" order="true"/>
            </SrcFlow>
        </LS>
        <LS tsi="1174">
            <SrcFlow>
                <EFDT>
                    <FDT-Instance Expires="4294967295">
                        <fdt:File TOI="3" Content-Location="App.pkg" Content-Type="multipart/related" afdt:appContextIdList="tag:sinclairplatform.com,2020:KSNV:2089 tag:sinclairplatform.com,2020:KLAS:2081 tag:sinclairplatform.com,2020:KTNV:2085 tag:sinclairplatform.com,2020:KVCW:2091"/>
                    </FDT-Instance>
                </EFDT>
                <Payload codePoint="3" formatId="3" frag="0" order="true"/>
            </SrcFlow>
        </LS>
    </RS>
</S-TSID>

--boundary-content
Content-Type:application/dash+xml; charset=utf-8
Content-Location:mpd.mpd

<?xml version="1.0" encoding="UTF-8"?>
<MPD xmlns="urn:mpeg:dash:schema:mpd:2011" availabilityStartTime="1970-01-01T00:00:00Z" maxSegmentDuration="PT2S" minBufferTime="PT2.1S" minimumUpdatePeriod="PT30S" profiles="urn:mpeg:dash:profile:isoff-broadcast:2015" publishTime="2020-06-29T14:44:02Z" timeShiftBufferDepth="PT20S" type="dynamic" xmlns:cenc="urn:mpeg:cenc:2013" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="urn:mpeg:dash:schema:mpd:2011 DASH-MPD.xsd">
   <Period id="P0" start="PT0S">
      <AdaptationSet contentType="video" id="0" maxFrameRate="60000/1001" maxHeight="1080" maxWidth="1920" mimeType="video/mp4" minFrameRate="60000/1001" minHeight="1080" minWidth="1920" par="16:9" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
         <Representation bandwidth="5900000" codecs="hev1.2.4.L123.90" frameRate="60000/1001" height="1080" id="Video1_1" sar="1:1" width="1920">
            <SegmentTemplate duration="2002000" initialization="video-init.mp4v" media="video-$Number$.mp4v" startNumber="0" timescale="1000000"/>
         </Representation>

      </AdaptationSet>
      <AdaptationSet contentType="audio" id="1" lang="eng" mimeType="audio/mp4" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="main"/>
         <Representation audioSamplingRate="48000" bandwidth="96000" codecs="ac-4.02.00.00" id="a02_2">
            <AudioChannelConfiguration schemeIdUri="tag:dolby.com,2015:dash:audio_channel_configuration:2015" value="000047"/>
            <SegmentTemplate duration="2002000" initialization="a0-$RepresentationID$-init.mp4" media="a0-$RepresentationID$-$Number$.m4s" startNumber="0" timescale="1000000"/>
         </Representation>

      </AdaptationSet>
      <AdaptationSet contentType="audio" id="2" lang="eng" mimeType="audio/mp4" segmentAlignment="true" startWithSAP="1">
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="alternate"/>
         <Representation audioSamplingRate="48000" bandwidth="32000" codecs="ac-4.02.00.00" id="a13_3">
            <AudioChannelConfiguration schemeIdUri="tag:dolby.com,2015:dash:audio_channel_configuration:2015" value="000001"/>
            <SegmentTemplate duration="2002000" initialization="a1-$RepresentationID$-init.mp4" media="a1-$RepresentationID$-$Number$.m4s" startNumber="0" timescale="1000000"/>
         </Representation>

      </AdaptationSet>
      <AdaptationSet contentType="text" id="3" lang="eng" mimeType="application/mp4" segmentAlignment="true" startWithSAP="1">
         <SupplementalProperty schemeIdUri="http://dashif.org/guidelines/dash-atsc-closedcaption" value="ar:16-9,er:0,profile:0,3d:0"/>
         <Role schemeIdUri="urn:mpeg:dash:role:2011" value="caption"/>
         <Representation bandwidth="140000" codecs="stpp.ttml.im1i" id="d4_4">
            <SegmentTemplate duration="2002000" initialization="$RepresentationID$-init.mp4" media="$RepresentationID$-$Number$.m4s" startNumber="0" timescale="1000000"/>
         </Representation>

      </AdaptationSet>
   </Period>
</MPD>

--boundary-content
Content-Type:application/atsc-held+xml; charset=utf-8
Content-Location:held.held

<?xml version="1.0" encoding="UTF-8"?>
<HELD xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/AppSignaling/HELD/1.0/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
<HTMLEntryPackage appContextId="tag:sinclairplatform.com,2020:KSNV:2089" appRendering="true" bcastEntryPackageUrl="App.pkg" bcastEntryPageUrl="index.html" coupledServices="5004"/>
</HELD>

--boundary-content--
