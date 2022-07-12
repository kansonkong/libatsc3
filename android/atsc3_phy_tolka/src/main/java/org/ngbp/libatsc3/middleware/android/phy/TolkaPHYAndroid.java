package org.ngbp.libatsc3.middleware.android.phy;

import android.util.Log;

import java.util.List;

public class TolkaPHYAndroid extends Atsc3NdkPHYTolkaStaticJniLoader  {

    //jjustman-2022-05-24 -     <usb-device vendor-id="1165" product-id="37638" />
    public static final int TOLKA_VENDOR_ID = 1165;
    public static final int REVB            = 37638;

    static {
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new Atsc3NdkPHYClientBase.USBVendorIDProductIDSupportedPHY(TOLKA_VENDOR_ID, REVB, "Tolka REVB", false, TolkaPHYAndroid.class));
        Log.w("TolkaPHYAndroid", String.format("static constructor, allRegisteredPHYImplementations is now %d elements: ",Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.size()));
    }

    @Override public native int init();
    @Override public native int run();
    @Override public native int stop();
    @Override public native int deinit();

    @Override public native int download_bootloader_firmware(int fd, int deviceType, String devicePath);
    @Override public native int open(int fd, int deviceType, String devicePath);
    @Override public native int tune(int freqKhz, int single_plp);
    @Override public native int listen_plps(List<Byte> plps);

    @Override public native String get_sdk_version();
    @Override public native String get_firmware_version();
}
