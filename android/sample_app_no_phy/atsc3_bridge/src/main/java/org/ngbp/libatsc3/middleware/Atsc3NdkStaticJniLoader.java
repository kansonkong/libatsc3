package org.ngbp.libatsc3.middleware;

public class Atsc3NdkStaticJniLoader {


    static {
        //jjustman:2019-11-24: cross reference and circular dependency with NXP_Tuner_Lib and SL API methods
        //System.loadLibrary("NXP_Tuner_Lib");
        //System.loadLibrary("SiTune_Tuner_Libs");

        System.loadLibrary("libatsc3_bridge");
    }
}
