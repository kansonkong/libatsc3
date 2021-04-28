package org.ngbp.libatsc3.middleware.android.phy;

import android.util.Log;

import java.util.List;

public class SaankhyaPHYAndroid extends Atsc3NdkPHYSaankhyaStaticJniLoader  {

    //jjustman-2021-04-28 - <1204, 243> is the generic VID/PID for the FX3 bootloader
    public static final int CYPRESS_VENDOR_ID = 1204;
    public static final int FX3_PREBOOT_PRODUCT_ID = 243;

    //this PID is only re-enumerated after our FX3 preboot firmware has been loaded based upon DeviceTypeSelectionDialog disambiguation
    public static final int KAILASH_PRODUCT_ID  = 240;

    // The same values as in SaankhyaPHYAndroid.h
    public static final int DEVICE_TYPE_MARKONE     = 0;
    public static final int DEVICE_TYPE_FX3_KAILASH = 1;
    public static final int DEVICE_TYPE_FX3_YOGA    = 3;

    static {
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new USBVendorIDProductIDSupportedPHY(CYPRESS_VENDOR_ID, 243, "SL-FX3-Preboot", true, SaankhyaPHYAndroid.class));
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new USBVendorIDProductIDSupportedPHY(CYPRESS_VENDOR_ID, KAILASH_PRODUCT_ID, "SL-KAILASH", false, SaankhyaPHYAndroid.class));
        Log.w("SaankhyaPHYAndroid", String.format("static constructor, allRegisteredPHYImplementations is now %d elements: ",Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.size()));

    }

    @Override public native int init();
    @Override public native int run();
    @Override public native int stop();
    @Override public native int deinit();

    @Override public native int download_bootloader_firmware(int fd, int deviceType, String devicePath);
    @Override public native int open(int fd, int deviceType, String devicePath);
    @Override public native int tune(int freqKhz, int single_plp);
    @Override public native int listen_plps(List<Byte> plps);

}
