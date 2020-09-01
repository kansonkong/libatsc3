package org.ngbp.libatsc3.middleware.android.phy.virtual;

import android.content.res.AssetManager;

public class PcapDemuxedVirtualPHYAndroid extends Atsc3NdkVirtualPHYStaticJniLoader  {
    @Override public native int init();
    @Override public native int run();
    @Override public native int stop();
    @Override public native int deinit();

    @Override public native int open_from_capture(String filename);
    
}
