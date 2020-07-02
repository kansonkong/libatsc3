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

