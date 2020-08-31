package org.ngbp.libatsc3.middleware.android.phy;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.util.Log;

import java.util.HashMap;
import java.util.Map;

public class Atsc3UsbDevice {
    /*
       from res/xml/device_filter.xml:

       <!-- Cypress preboot loader - NOTE: may be one of multiple different PHY implementations using FX2/FX3 -->
        <usb-device vendor-id="1204" product-id="243" />

        <!-- LowaSIS: VID:0xf055, PID:0x1e1b f055 ATLAS -->
        <usb-device vendor-id="61525" product-id="7707" />

        <!-- Saankhya Labs - KAILASH -->
        <usb-device vendor-id="1204" product-id="243" />
     */
    public static Map<UsbDevice, Atsc3UsbDevice> AllAtsc3UsbDevices = new HashMap<>();

    public static Atsc3UsbDevice FindFromUsbDevice(UsbDevice toSearchDevice) {
        return AllAtsc3UsbDevices.get(toSearchDevice);
    }
    public static void DumpAllAtsc3UsbDevices() {
//        Log.d("AllAtsc3UsbDevices", String.format("AllAtsc3UsbDevices has %d entries", AllAtsc3UsbDevices.size()));
//
//        for(Map.Entry<UsbDevice, Atsc3UsbDevice> entries : AllAtsc3UsbDevices.entrySet()) {
//            Log.d("AllAtsc3UsbDevices", String.format("key: %s (%s), val: %s", entries.getKey(), entries.getKey().getDeviceName(), entries.getValue()));
//        }
    }

    public void destroy() {
        AllAtsc3UsbDevices.remove(this.usbDevice);
//        Log.d("Atsc3UsbDevice", String.format("removing this: %s (usbDevice: %s), entries remaining: %d", this, this.usbDevice, AllAtsc3UsbDevices.size()));

        this.disconnect();
        this.usbDevice = null;
        this.conn = null;
    }

    public UsbDevice usbDevice;
    public UsbDeviceConnection conn;
    public Atsc3NdkPHYClientBase atsc3NdkPHYClientBase;

    public Atsc3UsbDevice(UsbDevice usbDevice, UsbDeviceConnection c) {
        this.usbDevice = usbDevice;
        this.conn = c;

        AllAtsc3UsbDevices.put(usbDevice, this);
    }
    public void setAtsc3NdkPHYClientBase(Atsc3NdkPHYClientBase atsc3NdkPHYClientBase) {
        this.atsc3NdkPHYClientBase = atsc3NdkPHYClientBase;
    }

    public int getFd() {
        if(usbDevice != null && conn != null) {
            return conn.getFileDescriptor();
        }
        return 0;
    }
    //returns back the relevant /dev/bus filesystem path..
    public String getDeviceName() {
        if(usbDevice != null) {
            return usbDevice.getDeviceName();
        } else {
            return "";
        }
    }

    public void disconnect() {
        if (conn != null)
            conn.close();
        conn = null;
        usbDevice = null;
    }
    
    public String toString() {
        if(usbDevice == null) {
            return "";
        }

        return String.format("%s:%s", getFd(), usbDevice.getDeviceName());
    }
};