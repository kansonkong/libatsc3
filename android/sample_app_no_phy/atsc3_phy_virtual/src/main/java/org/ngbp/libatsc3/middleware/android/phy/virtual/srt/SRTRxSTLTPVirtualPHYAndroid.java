package org.ngbp.libatsc3.middleware.android.phy.virtual.srt;

import android.content.res.AssetManager;

import org.ngbp.libatsc3.middleware.android.phy.virtual.Atsc3NdkPHYVirtualStaticJniLoader;

public class SRTRxSTLTPVirtualPHYAndroid extends Atsc3NdkPHYVirtualStaticJniLoader  {

    public native int init();
    public native int run();
    public native boolean is_running();
    public native int stop();
    public native int deinit();

    public native void setSrtSourceConnectionString(String srtSourceConnectionString);

}
