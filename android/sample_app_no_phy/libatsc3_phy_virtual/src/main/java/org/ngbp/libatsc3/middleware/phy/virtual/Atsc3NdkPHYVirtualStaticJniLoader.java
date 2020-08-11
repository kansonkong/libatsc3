package org.ngbp.libatsc3.middleware.phy.virtual;

public class Atsc3NdkPHYVirtualStaticJniLoader {

    static {
        System.loadLibrary("libatsc3_phy_virtual");
    }
}
