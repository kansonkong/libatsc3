package org.ngbp.libatsc3.middleware.android.phy;

public abstract class Atsc3NdkPHYSonyStaticJniLoader extends Atsc3NdkPHYClientBase {

    static {
        System.loadLibrary("atsc3_phy_sony");
    }
}
