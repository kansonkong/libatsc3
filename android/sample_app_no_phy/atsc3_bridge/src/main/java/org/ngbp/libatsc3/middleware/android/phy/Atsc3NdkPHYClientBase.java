package org.ngbp.libatsc3.middleware.android.phy;

/*     native int ApiInit(Atsc3NdkPHYBridge atsc3NdkPhyBridge); */
public abstract class Atsc3NdkPHYClientBase {

    /*
        state management overview

        init -> (optional phy specific configuration parameters)
              -> run
                -> stop
                    -> deinit
     */
    public native int init();
    public native int run();
    public native int stop();
    public native int deinit();

    public native int ApiPrepare(String devlist, int delimiter1, int delimiter2);
    public native long[] ApiFindDeviceKey(boolean bPreBootDevice);
    public native int ApiFwLoad(long key);
    public native int ApiOpen(int fd, long key);
    public native int ApiTune(int freqKhz, int plpid);
    public native int ApiSetPLP(int[] aPlpIds);
    public native int ApiReset();

    //public native int ApiClose();
}
