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
public class SRTTransmitSTLTPVirtualPHY extends Atsc3NdkPHYVirtualStaticJniLoader  {


}
