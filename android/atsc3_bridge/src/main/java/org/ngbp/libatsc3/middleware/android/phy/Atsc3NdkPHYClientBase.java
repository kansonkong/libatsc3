package org.ngbp.libatsc3.middleware.android.phy;

import android.hardware.usb.UsbDevice;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

import androidx.annotation.Keep;

/*
    state management overview

    init -> (optional phy specific configuration parameters)
          -> run
            -> stop
                -> deinit
 */

@Keep
public abstract class Atsc3NdkPHYClientBase {

    @Keep interface Atsc3NdkPHYClientBaseBootloaderCallback {
        boolean RunIsBootloaderCallback(UsbDevice usbDevice);
    }

    @Keep public static class USBVendorIDProductIDSupportedPHY {
        public USBVendorIDProductIDSupportedPHY(int vendorID, int productID, String phyName, boolean isBootloaderFlag, Class<? extends Atsc3NdkPHYClientBase> candidatePHYImplementation) {
            this.vendorID = vendorID;
            this.productID = productID;
            this.phyName = phyName;
            this.isBootloaderFlag = isBootloaderFlag;
            this.candidatePHYImplementation = candidatePHYImplementation;
        }

        public USBVendorIDProductIDSupportedPHY(int vendorID, int productID, String phyName, Atsc3NdkPHYClientBaseBootloaderCallback bootloaderCallback, Class<? extends Atsc3NdkPHYClientBase> candidatePHYImplementation) {
            this.vendorID = vendorID;
            this.productID = productID;
            this.phyName = phyName;
            this.bootloaderCallback = bootloaderCallback;
            this.candidatePHYImplementation = candidatePHYImplementation;
        }

        public int vendorID;
        public int productID;
        public String phyName;
        public boolean isBootloaderFlag;
        public Atsc3NdkPHYClientBaseBootloaderCallback bootloaderCallback;
        public Class<? extends Atsc3NdkPHYClientBase> candidatePHYImplementation;

        public boolean getIsBootloader(UsbDevice usbDevice) {
            if(isBootloaderFlag || (bootloaderCallback != null && bootloaderCallback.RunIsBootloaderCallback(usbDevice))) {
                return true;
            } else {
                return false;
            }
        }

    }

    synchronized public static Atsc3NdkPHYClientBase CreateInstanceFromUSBVendorIDProductIDSupportedPHY(USBVendorIDProductIDSupportedPHY usbVendorIDProductIDSupportedPHY) {
        Log.w("Atsc3NdkPHYClientBase", "CreateInstanceFromUSBVendorIDProductIDSupportedPHY");

        Atsc3NdkPHYClientBase newInstance = null;
        try {
            newInstance = usbVendorIDProductIDSupportedPHY.candidatePHYImplementation.newInstance();
            newInstance.init();

        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InstantiationException e) {
            e.printStackTrace();
        }

        return newInstance;
    }

    static ArrayList<USBVendorIDProductIDSupportedPHY> AllRegisteredPHYImplementations = new ArrayList<>();
    public static ArrayList<Atsc3NdkPHYClientBase.USBVendorIDProductIDSupportedPHY> GetCandidatePHYImplementations(UsbDevice usbDevice) {
        ArrayList<USBVendorIDProductIDSupportedPHY> matchingRegisteredPHYImplementations = new ArrayList<>();
        for(USBVendorIDProductIDSupportedPHY usbVendorIDProductIDSupportedPHY : AllRegisteredPHYImplementations) {
            if(usbVendorIDProductIDSupportedPHY.vendorID == usbDevice.getVendorId() && usbVendorIDProductIDSupportedPHY.productID == usbDevice.getProductId()) {
                matchingRegisteredPHYImplementations.add(usbVendorIDProductIDSupportedPHY);
            }
        }
        return matchingRegisteredPHYImplementations.size() > 0 ? matchingRegisteredPHYImplementations : null;
    }

    public Atsc3UsbDevice atsc3UsbDevice = null;
    public void setAtsc3UsbDevice(Atsc3UsbDevice atsc3UsbDevice) {
        this.atsc3UsbDevice = atsc3UsbDevice;
    }

    //required jni methods for implementation
    public native int     init();
    public native int     run();
    public native boolean is_running();
    public native int     stop();
    public native int     deinit();

    //optional jni methods, to enable per-PHY use-cases,
    //but un-safe for non context-aware invocation

    public native int     download_bootloader_firmware(int fd, String devicePath);
    public native int     open(int fd, String devicePath);
    public native int     open_from_capture(String filename);
    public native int     tune(int freqKhz, int single_plp);
    public native int     listen_plps(List<Byte> plps);

}
