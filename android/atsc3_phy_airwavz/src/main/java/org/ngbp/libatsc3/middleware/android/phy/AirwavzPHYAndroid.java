package org.ngbp.libatsc3.middleware.android.phy;

import org.ngbp.libatsc3.middleware.android.phy.Atsc3NdkPHYClientBase;

import android.hardware.usb.UsbConfiguration;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbInterface;
import android.util.Log;

import java.util.List;

public class AirwavzPHYAndroid extends Atsc3NdkPHYAirwavzStaticJniLoader  {

    private static final String TAG = "AirwavzPHYAndroid";

    /*
            <usb-device vendor-id="5840" product-id="3500" />  <!-- Airwavz.tv RZR-1200 -->
            <usb-device vendor-id="5840" product-id="3737" />  <!-- Airwavz.tv RZR-1400 -->
     */

    static {
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new USBVendorIDProductIDSupportedPHY(5840, 3500, "Airwavz.tz RZR-1200",false, AirwavzPHYAndroid.class));
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new USBVendorIDProductIDSupportedPHY(5840, 3737, "Airwavz.tz RZR-1400",false, AirwavzPHYAndroid.class));

        Log.w("AirwavzPHYAndroid", String.format("static constructor, allRegisteredPHYImplementations is now %d elements: ", Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.size()));
    }

    @Override public native int init();
    @Override public native int run();
    @Override public native int stop();
    @Override public native int deinit();

    @Override public native int open(int fd, String devicePath);
    @Override public native int tune(int freqKhz, int single_plp);
    @Override public native int listen_plps(List<Byte> plps);
}
