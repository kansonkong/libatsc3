package com.api;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.TimeoutException;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import android.content.Context;
import android.content.res.XmlResourceParser;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbRequest;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Xml;

public class Bus {
    private static String TAG = "Endeavour.Bus";
    
    public static UsbDeviceConnection connection;
    private static UsbEndpoint ep4TSRead;
    //private static UsbEndpoint ep5TSRead;
    private static UsbEndpoint epRead;
    private static UsbEndpoint epWrite;
    private static final int TIMEOUT = 500;
    private static final int TIMEOUT2 = 0;
    private static UsbInterface iface;

    private static class ID {
        public int vid;
        public int pid;

        public ID(int vid, int pid) {
            this.vid = vid;
            this.pid = pid;
        }
    }

    public static long openDevice (UsbManager manager, UsbDevice device) {
        connection = manager.openDevice(device);
        Log.d(TAG, "connection: " + connection.toString());
        if(connection != null) {
            Log.d(TAG, "iface: "+ iface.toString());
            if(connection.claimInterface(iface, true)) {
                Log.d(TAG, "claimInterface is successful");
                Datagram.reset();
                return Error.Error_NO_ERROR;
            }
        }
        return Error.Error_USB_OPEN_FAIL;
    }

    public static boolean checkDevice (UsbDevice device, Context context, int device_filter) {
        ArrayList<ID> support = new ArrayList<ID>();
        XmlResourceParser xml = context.getResources().getXml(device_filter);
        try {
            xml.next();
            
            int eventType = 0;
            while((eventType = xml.getEventType()) != XmlPullParser.END_DOCUMENT) {
                
                switch(eventType) {
                case XmlPullParser.START_TAG:
                    if(xml.getName().equals("usb-device")) {
                        AttributeSet as = Xml.asAttributeSet(xml);
                        int supportVid = Integer.parseInt(as.getAttributeValue(null, "vendor-id"));
                        int supportPid = Integer.parseInt(as.getAttributeValue(null, "product-id"));
                        support.add(new ID(supportVid, supportPid));
                        Log.v(TAG, "support vendorId = 0x" + Integer.toHexString(supportVid) +
                                ", productId = 0x" + Integer.toHexString(supportPid));
                    }
                    break;
                }
                xml.next();
            }
        } catch (XmlPullParserException e) {
            // TODO Auto-generated catch block
            Log.v(TAG, e.toString());
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            Log.v(TAG, e.toString());
            e.printStackTrace();
        }
            
        if(device == null) {
            Log.v(TAG, "Device is null");
            return false;
        } else {
            int deviceVid = device.getVendorId();
            int devicePid = device.getProductId();
            Log.v(TAG, "device vendorId = 0x" + Integer.toHexString(deviceVid) +
                       ", productId = 0x" + Integer.toHexString(devicePid));

            for(int i = 0; i < support.size(); i++) {
                if(deviceVid == support.get(i).vid && devicePid == support.get(i).pid) {
                    Log.v(TAG, "Device is found");
                    iface = device.getInterface(0);
                    for(int j = 0; j < iface.getEndpointCount(); j++) {
                            UsbEndpoint ep = iface.getEndpoint(j);
                            Log.v(TAG, "EP(" + j + "), addr = 0x" + Integer.toHexString(ep.getAddress()) +
                                    ", attr = " + ep.getAttributes() +
                                    ", dir = " + ep.getDirection() +
                                    ", num = " + ep.getEndpointNumber() +
                                    ", intval = " + ep.getInterval() +
                                    ", maxSize =" + ep.getMaxPacketSize());

                            if(j == 0)
                                epRead = iface.getEndpoint(j);
                            else if(j == 1)
                                epWrite = iface.getEndpoint(j);
                            else if(j == 2)
                                ep4TSRead = iface.getEndpoint(j);
                    }

                    return true;                        
                }
            }
        }

        return false;
    }
    
    public static long initial (UsbManager manager, Context context, int device_filter) {
        ArrayList<ID> support = new ArrayList<ID>();
        XmlResourceParser xml = context.getResources().getXml(device_filter);
        try {
            xml.next();
            
            int eventType = 0;
            while((eventType = xml.getEventType()) != XmlPullParser.END_DOCUMENT) {
                
                switch(eventType) {
                case XmlPullParser.START_TAG:
                    if(xml.getName().equals("usb-device")) {
                        AttributeSet as = Xml.asAttributeSet(xml);
                        int supportVid = Integer.parseInt(as.getAttributeValue(null, "vendor-id"));
                        int supportPid = Integer.parseInt(as.getAttributeValue(null, "product-id"));
                        support.add(new ID(supportVid, supportPid));
                        Log.v(TAG, "support vendorId = 0x" + Integer.toHexString(supportVid) +
                                ", productId = 0x" + Integer.toHexString(supportPid));
                    }
                    break;
                }
                xml.next();
            }
        } catch (XmlPullParserException e) {
            // TODO Auto-generated catch block
            Log.v(TAG, e.toString());
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            Log.v(TAG, e.toString());
            e.printStackTrace();
        }
        
        HashMap<String, UsbDevice> deviceList = manager.getDeviceList();
        Iterator<UsbDevice> deviceIterator = deviceList.values().iterator();
        
        if(!deviceIterator.hasNext()) {
            Log.v(TAG, "This Android device does not meet the system requirements");
            return Error.Error_USB_DEVICE_NOT_FOUND;
        }
        /*
        int testShow = 0;
        Log.v(TAG, "test show pid vid");
        while(deviceIterator.hasNext()) {
            UsbDevice device = deviceIterator.next();
            Log.v(TAG, testShow + ", vid = " + Integer.toHexString(device.getVendorId()) + ", pid = " + Integer.toHexString(device.getProductId()));
            testShow++;
        }*/

        while(deviceIterator.hasNext()) {
            UsbDevice device = deviceIterator.next();
            
            if(device == null) {
                Log.v(TAG, "Device is null");
                return Error.Error_USB_DEVICE_NOT_FOUND;
            } else {
                int deviceVid = device.getVendorId();
                int devicePid = device.getProductId();
                Log.v(TAG, "device vendorId = 0x" + Integer.toHexString(deviceVid) +
                        ", productId = 0x" + Integer.toHexString(devicePid));

                for(int i = 0; i < support.size(); i++) {
                    if(deviceVid == support.get(i).vid && devicePid == support.get(i).pid) {
                        Log.v(TAG, "Device is found");
                        //Log.v(TAG, "device = " + device);
                        //Log.v(TAG, "InterfaceCount = " + device.getInterfaceCount());
                        iface = device.getInterface(0);
                        //Log.v(TAG, "EndPointCount = " + iface.getEndpointCount());
                        for(int j = 0; j < iface.getEndpointCount(); j++) {
                            UsbEndpoint ep = iface.getEndpoint(j);
                            Log.v(TAG, "EP(" + j + "), addr = 0x" + Integer.toHexString(ep.getAddress()) +
                                    ", attr = " + ep.getAttributes() +
                                    ", dir = " + ep.getDirection() +
                                    ", num = " + ep.getEndpointNumber() +
                                    ", intval = " + ep.getInterval() +
                                    ", maxSize =" + ep.getMaxPacketSize());

                            if(j == 0)
                                epRead = iface.getEndpoint(j);
                            else if(j == 1)
                                epWrite = iface.getEndpoint(j);
                            else if(j == 2)
                                ep4TSRead = iface.getEndpoint(j);
                        }
                        
                        connection = manager.openDevice(device);
                        if(connection != null) {
                            if(connection.claimInterface(iface, true)) {
                                Datagram.reset();
                                return Error.Error_NO_ERROR;
                            }
                        }
                    }
                }
            }
        }

        Log.v(TAG, "Either PID or VID is wrong");
        return Error.Error_USB_PID_VID_WRONG;
    }

    public static long Tx (int bufferLength, byte[] buffer) {
        //if(connection.bulkTransfer(epWrite, buffer, bufferLength, TIMEOUT) != bufferLength)
        int retBufferLength = connection.bulkTransfer(epWrite, buffer, bufferLength, TIMEOUT);
        if(retBufferLength != bufferLength) {
            Log.v(TAG, "Tx bufferLength = " + bufferLength + ", return bufferLength = " + retBufferLength);
            return  Error.Error_USB_WRITE_FAIL;
        } else
            return Error.Error_NO_ERROR;
    }
    
    public static long Rx (int bufferLength, byte[] buffer) {
        //if(connection.bulkTransfer(epRead, buffer, bufferLength, TIMEOUT) != bufferLength)
        int retBufferLength = connection.bulkTransfer(epRead, buffer, bufferLength, TIMEOUT);
        if(retBufferLength != bufferLength) {
            Log.v(TAG, "Rx bufferLength = " + bufferLength + ", return bufferLength = " + retBufferLength);
            return Error.Error_USB_READ_FAIL;
        } else
            return Error.Error_NO_ERROR;
    }
    
    public static class Datagram {
        private static LinkedList<UsbRequest> requestPool = new LinkedList<UsbRequest>();
        private static Queue<ByteBuffer> tsFrameQueue = new LinkedList<ByteBuffer>();
        private ByteBuffer messageBuffer = ByteBuffer.allocate(User.IT9300User_USB20_FRAME_SIZE);
        private static final int maxUsbRequest = 20;


        public static final Object mReadBufferLock = new Object();
        private static final int USB_READ_TIMEOUT_MILLIS = 5000;
        private static byte[] mReadBuffer = new byte[User.IT9300User_USB20_FRAME_SIZE];

        public Datagram() {
        }
        
        private static UsbRequest getInRequest() {
            synchronized(requestPool) {
                if(requestPool.isEmpty()) {
                    UsbRequest request = new UsbRequest();
                    request.initialize(Bus.connection, Bus.ep4TSRead);
                    return request;
                } else {
                    return requestPool.removeFirst();
                }
            }
        }

        private boolean readCommand(UsbRequest request) {
            request.setClientData(this);
            return request.queue(messageBuffer, User.IT9300User_USB20_FRAME_SIZE);
        }

        //send UsbRequest to endpoint
        private static Datagram[] taskTemp = new Datagram[Datagram.maxUsbRequest];

        private static boolean isInit = false;
        private static void init() {
            //Receiver.resetPidFilter(0);
            //Receiver.controlPidFilter(0, 0);

            for(int i = 0; i < Datagram.maxUsbRequest; i++) {
                taskTemp[i] = new Datagram();
                if(!taskTemp[i].readCommand(Datagram.getInRequest()))
                    Log.v(TAG, Debug.getLineInfo() + ", usb request queue failed!!");
            }
        }

        public static void reset() {
            isInit = false;
        }

        public static boolean getTs(byte[] buffer, int[] bufferLength, int frameNumber) {
            int i=0;
            if (isInit == false) {
                init();
                isInit = true;
            }
            do {
                UsbRequest request = null;
                Datagram task = null;
                boolean find = false;

                //get ts data from endpoint

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    try {
                        request = Bus.connection.requestWait(TIMEOUT2);
                    } catch (TimeoutException e) {
                        // TODO Auto-generated catch block
                       // e.printStackTrace();
                    }
                } else {
                    request = Bus.connection.requestWait();
                }

                if (request != null) {
                    if (request.getEndpoint() != Bus.ep4TSRead) {
                        Log.v(TAG, "request.getEndpoint()!= ep4TSRead!!!! ");
                        return false;
                    }

                    task = (Datagram) request.getClientData();
                    request.setClientData(null);

                    if ((task.messageBuffer.array().length) != User.IT9300User_USB20_FRAME_SIZE)
                        Log.v(TAG, "############### Ts Frame size = " + task.messageBuffer.array().length);

                    for (int l = 0; l < Datagram.maxUsbRequest; l++) {
                        if (task == taskTemp[l]) {
                            find = taskTemp[l].readCommand(Datagram.getInRequest());
                            //task.readCommand(Task.getInRequest());
                        }
                        if(find == true)
                            break;
                    }

                    if (find == false) {
                        request.close();
                        return false;
                    }

                    synchronized (Datagram.requestPool) {
                        Datagram.requestPool.add(request);
                    }

                    synchronized (Datagram.tsFrameQueue) {
                        Datagram.tsFrameQueue.add(task.messageBuffer);
                    }

//                    Log.d(TAG, "task get Ts:");
                    System.arraycopy((Datagram.tsFrameQueue.remove()).array(), 0, buffer, User.IT9300User_USB20_FRAME_SIZE * i, User.IT9300User_USB20_FRAME_SIZE);
                    bufferLength[0] = User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER;
//                    return true;
                } else
                    return false;
                i++;
            } while(i < frameNumber);
            return true;
        }
    }
}
