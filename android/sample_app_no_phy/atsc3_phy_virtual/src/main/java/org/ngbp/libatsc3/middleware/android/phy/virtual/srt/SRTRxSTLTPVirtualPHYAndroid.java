package org.ngbp.libatsc3.middleware.android.phy.virtual.srt;

import android.content.res.AssetManager;

import org.ngbp.libatsc3.middleware.android.phy.virtual.Atsc3NdkPHYVirtualStaticJniLoader;

/*

    to build JNI c interface:
        /Library/Java/JavaVirtualMachines/jdk1.8.0_231.jdk/Contents/Home/bin/javah

   /Library/Java/JavaVirtualMachines/jdk1.8.0_231.jdk/Contents/Home/bin/javah -d cpp -classpath ../../build/intermediates/classes/debug org.ngbp.libatsc3.middleware.android.phy.virtual.DemuxedPcapVirtualPHY

javah -classpath ./bin/classes -d jni com.nvidia.example.fibonacci.FibonacciActivity

    javac -h .


 */
public class SRTRxSTLTPVirtualPHYAndroid extends Atsc3NdkPHYVirtualStaticJniLoader  {

    @Override public native int init();
    @Override public native int run();
    @Override public native int stop();
    @Override public native int deinit();

    public native void setSrtSourceConnectionString(String srtSourceConnectionString);

}
