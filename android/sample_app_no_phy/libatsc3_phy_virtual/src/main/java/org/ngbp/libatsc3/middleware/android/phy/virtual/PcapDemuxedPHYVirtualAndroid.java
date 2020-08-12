package org.ngbp.libatsc3.middleware.android.phy.virtual;

import android.content.res.AssetManager;

import org.ngbp.libatsc3.middleware.android.phy.Atsc3NdkPHYClientBase;

/*

    to build JNI c interface:
        /Library/Java/JavaVirtualMachines/jdk1.8.0_231.jdk/Contents/Home/bin/javah

   /Library/Java/JavaVirtualMachines/jdk1.8.0_231.jdk/Contents/Home/bin/javah -d cpp -classpath ../../build/intermediates/classes/debug org.ngbp.libatsc3.middleware.android.phy.virtual.DemuxedPcapVirtualPHY

javah -classpath ./bin/classes -d jni com.nvidia.example.fibonacci.FibonacciActivity

    javac -h .


 */
public class PcapDemuxedPHYVirtualAndroid extends Atsc3NdkPHYClientBase {

    //libatsc3 methods here...
    public native int atsc3_pcap_open_for_replay(String filename);
    public native int atsc3_pcap_open_for_replay_from_assetManager(String filename, AssetManager assetManager);
    public native int atsc3_pcap_thread_run();
    public native int atsc3_pcap_thread_stop();

}
