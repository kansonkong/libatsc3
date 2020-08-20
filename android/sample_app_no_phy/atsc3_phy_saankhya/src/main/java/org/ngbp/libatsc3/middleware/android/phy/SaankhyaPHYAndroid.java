package org.ngbp.libatsc3.middleware.android.phy;

import java.util.List;

public class SaankhyaPHYAndroid extends Atsc3NdkPHYSaankhyaStaticJniLoader  {

    static {
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new USBVendorIDProductIDSupportedPHY(1204, 243, "SL-FX3-Preboot", true, SaankhyaPHYAndroid.class));
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new USBVendorIDProductIDSupportedPHY(1204, 240, "SL-KAILASH", false, SaankhyaPHYAndroid.class));
    }

    @Override public native int init();
    @Override public native int run();
    @Override public native int stop();
    @Override public native int deinit();

    @Override public native int download_bootloader_firmware(int fd);
    @Override public native int open(int fd);
    @Override public native int tune(int freqKhz, int single_plp);
    @Override public native int listen_plps(List<Byte> plps);

}
