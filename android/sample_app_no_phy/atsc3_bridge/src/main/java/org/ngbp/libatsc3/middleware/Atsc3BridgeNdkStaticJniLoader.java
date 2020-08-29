package org.ngbp.libatsc3.middleware;

import android.util.Log;

public abstract class Atsc3BridgeNdkStaticJniLoader {

    public native int init();

    static {
        Log.w("Atsc3BridgeNdkStaticJniLoader", "loading libatsc3_bridge");
        System.loadLibrary("atsc3_bridge");
    }
}
