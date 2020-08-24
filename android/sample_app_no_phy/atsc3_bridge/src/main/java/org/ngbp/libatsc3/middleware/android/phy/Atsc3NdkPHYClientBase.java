package org.ngbp.libatsc3.middleware.android.phy;

import android.hardware.usb.UsbDevice;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/*
    state management overview

    init -> (optional phy specific configuration parameters)
          -> run
            -> stop
                -> deinit
 */

public abstract class Atsc3NdkPHYClientBase {

    public static class USBVendorIDProductIDSupportedPHY {
        public USBVendorIDProductIDSupportedPHY(int vendorID, int productID, String phyName, boolean isBootloader, Class<? extends Atsc3NdkPHYClientBase> candidatePHYImplementation) {
            this.vendorID = vendorID;
            this.productID = productID;
            this.phyName = phyName;
            this.isBootloader = isBootloader;
            this.candidatePHYImplementation = candidatePHYImplementation;
        }

        public int vendorID;
        public int productID;
        public String phyName;
        public boolean isBootloader;
        public Class<? extends Atsc3NdkPHYClientBase> candidatePHYImplementation;
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
    //
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

    public native int     download_bootloader_firmware(int fd);
    public native int     open(int fd);
    public native int     tune(int freqKhz, int single_plp);
    public native int     listen_plps(List<Byte> plps);

}
