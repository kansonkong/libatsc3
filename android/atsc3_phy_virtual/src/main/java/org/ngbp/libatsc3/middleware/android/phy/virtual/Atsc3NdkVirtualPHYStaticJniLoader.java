package org.ngbp.libatsc3.middleware.android.phy.virtual;

import org.ngbp.libatsc3.middleware.android.phy.Atsc3NdkPHYClientBase;

public abstract class Atsc3NdkVirtualPHYStaticJniLoader extends Atsc3NdkPHYClientBase {

    static {
        System.loadLibrary("atsc3_phy_virtual");
    }
}
