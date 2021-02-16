package org.ngbp.libatsc3.middleware;

import android.util.Log;

public abstract class Atsc3NdkMediaMMTBridgeStaticJniLoader {

    //jjustman-2021-01-19 - add in explicit finalizer invocation to native release()
    @Override
    protected void finalize() throws Throwable {
        release();
        super.finalize();
    }

    public native int init();
    public native void release();

    static {
        Log.w("Atsc3NdkMediaMMTBridgeStaticJniLoader", "loading libatsc3_bridge_media_mmt");
        System.loadLibrary("atsc3_bridge_media_mmt");
    }
}
