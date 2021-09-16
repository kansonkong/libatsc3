package org.ngbp.libatsc3.middleware.android;

public class Atsc3NdkCoreLogs {
    public native void init();
    public native String[] getAtsc3LogNames();
    public native String[] getAtsc3LogEnabledNames();
    public native void setAtsc3LogEnabledByName(String name, int value);
}