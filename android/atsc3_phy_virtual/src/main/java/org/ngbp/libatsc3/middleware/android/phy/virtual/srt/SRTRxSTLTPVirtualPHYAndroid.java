package org.ngbp.libatsc3.middleware.android.phy.virtual.srt;

import android.content.res.AssetManager;

import org.ngbp.libatsc3.middleware.android.phy.virtual.Atsc3NdkVirtualPHYStaticJniLoader;

public class SRTRxSTLTPVirtualPHYAndroid extends Atsc3NdkVirtualPHYStaticJniLoader {

    @Override public native int init();
    @Override public native int run();
    @Override public native boolean is_running();
    @Override public native int stop();
    @Override public native int deinit();

    public native void setSrtSourceConnectionString(String srtSourceConnectionString);

}
