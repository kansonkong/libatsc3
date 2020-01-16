package org.ngbp.libatsc3.sampleapp;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;

public class atsc3UsbDevice {
    /*
        TODO:

        capture VID/PID's for:
            CypressFX3 bootloader   PID=1204, VID=240
            LowaSIS post-bootloader
            SL post-bootloader      PID=1204, VID=243
     */

    public UsbDevice dev;
    public UsbDeviceConnection conn;
    public int fd;

    //  /dev/bus/usb/
    // jjustman-2019-12-05: TODO: remove me
    //  //long key = ((bus & 0xff) << 8) + (addr & 0xff);

    public long key;

    public atsc3UsbDevice(UsbDevice d, UsbDeviceConnection c, int fd, long key) {
        this.dev = d;
        this.conn = c;
        this.fd = fd;
        this.key = key;
    }

    public void disconnect() {
        if (conn != null)
            conn.close();
        conn = null;
        dev = null;
        fd = -1;
        key = -1;
    }
    public String toString() {
        if(dev == null) {
            return "";
        }

        return String.format("%s:%s", fd, dev.getDeviceName());
    }
};