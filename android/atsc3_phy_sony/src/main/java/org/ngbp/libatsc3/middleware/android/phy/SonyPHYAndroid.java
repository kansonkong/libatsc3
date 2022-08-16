package org.ngbp.libatsc3.middleware.android.phy;

import android.util.Log;

import java.util.List;

public class SonyPHYAndroid extends Atsc3NdkPHYSonyStaticJniLoader  {

    //jjustman-2022-05-24 -     <usb-device vendor-id="1165" product-id="37638" />
    public static final int SONY_VENDOR_ID    = 1165;
    public static final int ITE930X_ENDEAVOUR = 37638;

    static {
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new Atsc3NdkPHYClientBase.USBVendorIDProductIDSupportedPHY(SONY_VENDOR_ID, ITE930X_ENDEAVOUR, "Sony ENDEAVOUR", false, SonyPHYAndroid.class));
        Log.w("SonyPHYAndroid", String.format("static constructor, allRegisteredPHYImplementations is now %d elements: ",Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.size()));
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
