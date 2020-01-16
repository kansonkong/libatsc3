package org.ngbp.libatsc3.a331.models;

import java.util.ArrayList;

public class Service {

    public int serviceId;
    public String globalServiceId;
    public int majorChannelNo;
    public int minorChannelNo;
    public int serviceCategory;

    public String shortServiceName;

    public ArrayList<BroadcastSvcSignaling> broadcastSvcSignalingCollection = new ArrayList<>();

    public String toString() {
        return String.format("%d.%d %s", majorChannelNo, minorChannelNo, shortServiceName);
    }

    public static class BroadcastSvcSignaling {

        public int slsProtocol;
        public int slsMajorProtocolVersion;
        public int slsMinorProtocolVersion;
        public String slsDestinationIpAddress;
        public String slsDestinationUdpPort;
        public String slsSourceIpAddress;

    }
}
