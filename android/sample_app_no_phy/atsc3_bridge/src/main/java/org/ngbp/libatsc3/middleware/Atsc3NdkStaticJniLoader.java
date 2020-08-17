package org.ngbp.libatsc3.middleware;

import android.util.Log;

public class Atsc3NdkStaticJniLoader {
    static {
        //jjustman:2019-11-24: cross reference and circular dependency with NXP_Tuner_Lib and SL API methods
        //System.loadLibrary("NXP_Tuner_Lib");
        //System.loadLibrary("SiTune_Tuner_Libs");
        Log.w("Atsc3NdkStaticJniLoader", "loading libatsc3_bridge");
        System.loadLibrary("atsc3_bridge");
    }
}
