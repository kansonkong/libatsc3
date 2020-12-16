package org.ngbp.libatsc3.middleware;

import android.util.Log;

public abstract class Atsc3NdkMediaMMTBridgeStaticJniLoader {

    public native int init();
    public native void release();

    static {
        Log.w("Atsc3NdkMediaMMTBridgeStaticJniLoader", "loading libatsc3_bridge_media_mmt");
        System.loadLibrary("atsc3_bridge_media_mmt");
    }
}
