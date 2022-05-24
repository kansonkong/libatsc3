package org.ngbp.libatsc3.middleware.android.phy;

public abstract class Atsc3NdkPHYTolkaStaticJniLoader extends Atsc3NdkPHYClientBase {

    static {
        System.loadLibrary("atsc3_phy_tolka");
    }
}
