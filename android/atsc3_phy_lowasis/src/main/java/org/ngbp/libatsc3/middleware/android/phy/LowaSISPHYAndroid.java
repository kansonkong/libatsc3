package org.ngbp.libatsc3.middleware.android.phy;

import org.ngbp.libatsc3.middleware.android.phy.Atsc3NdkPHYClientBase;

import android.hardware.usb.UsbConfiguration;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbInterface;
import android.util.Log;

import java.util.List;

public class LowaSISPHYAndroid extends Atsc3NdkPHYLowaSISStaticJniLoader  {

    private static final String TAG = "LowaSISPHYAndroid";

    /*
          <!-- LowaSIS: VID:0xf055, PID:0x1e1b f055 ATLAS
                            bootloader:             device release: 0x15B
                            full firmware running:  device release: 0x0200 ~ 0xFFFF -->
            <usb-device vendor-id="61525" product-id="7707" />

            LowaSIS has a special 'workaround' for determining if this instance is a bootloader
         */
    static {
        Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.add(new USBVendorIDProductIDSupportedPHY(61525, 7707, "LowaSIS", LowaSISPHYAndroid::RunIsBootloaderCallback, LowaSISPHYAndroid.class));
        Log.w("LowaSISPHYAndroid", String.format("static constructor, allRegisteredPHYImplementations is now %d elements: ", Atsc3NdkPHYClientBase.AllRegisteredPHYImplementations.size()));

    }

    public static boolean RunIsBootloaderCallback(UsbDevice device) {
        // full-function (fwloaded) atlas device has 2 or 3 EPs.
        // preboot (not fwloaded) atlas device has (currently) zero EP.

        int vid = device.getVendorId(); int pid = device.getProductId();
        if (vid != 0xf055 || pid != 0x1e1b) return false; // not atlas device

        UsbConfiguration conf = device.getConfiguration(0);
        UsbInterface intf = conf.getInterface(0);
        int numEp = intf.getEndpointCount();
        Log.d(TAG, "atlas device: num ep = " + numEp);

        if (numEp < 2) return true;

        return false;
    }

    @Override public native int init();
    @Override public native int run();
    @Override public native int stop();
    @Override public native int deinit();

    @Override public native int download_bootloader_firmware(int fd, String devicePath);
    @Override public native int open(int fd, String devicePath);
    @Override public native int tune(int freqKhz, int single_plp);
    @Override public native int listen_plps(List<Byte> plps);




}
