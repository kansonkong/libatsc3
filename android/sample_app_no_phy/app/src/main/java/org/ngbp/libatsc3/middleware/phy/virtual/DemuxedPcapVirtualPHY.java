package org.ngbp.libatsc3.middleware.phy.virtual;

import android.content.res.AssetManager;

public class DemuxedPcapVirtualPHY {

    //libatsc3 methods here...
    public native int atsc3_pcap_open_for_replay(String filename);
    public native int atsc3_pcap_open_for_replay_from_assetManager(String filename, AssetManager assetManager);
    public native int atsc3_pcap_thread_run();
    public native int atsc3_pcap_thread_stop();

    static {
        System.loadLibrary("libatsc3_DemuxedPcapVirtualPHY");
    }

}
