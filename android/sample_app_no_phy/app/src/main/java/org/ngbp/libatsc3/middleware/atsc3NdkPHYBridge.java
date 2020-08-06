package org.ngbp.libatsc3.middleware;

import android.content.res.AssetManager;
import android.util.Log;

import org.ngbp.libatsc3.android.PackageExtractEnvelopeMetadataAndPayload;
import org.ngbp.libatsc3.media.ATSC3PlayerFlags;
import org.ngbp.libatsc3.media.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.media.sync.mmt.MmtPacketIdContext;
import org.ngbp.libatsc3.media.sync.mmt.MpuMetadata_HEVC_NAL_Payload;
import org.ngbp.libatsc3.phy.BwPhyStatistics;
import org.ngbp.libatsc3.phy.RfPhyStatistics;
import org.ngbp.libatsc3.sampleapp.MainActivity;

import java.io.File;
import java.nio.ByteBuffer;

/**
 */

public class atsc3NdkPHYBridge {

    final static String TAG ="intf";

    MainActivity mActivity;

    atsc3NdkPHYBridge(MainActivity parent) {
        mActivity = parent;
    }

    int onLogMsg(String msg) {
        Log.d(TAG, msg);
        mActivity.showMsgFromNative(msg+"\n");
        return 0;
    }

    public native int ApiInit(atsc3NdkPHYBridge intf);
    public native int ApiPrepare(String devlist, int delimiter1, int delimiter2);
    public native long[] ApiFindDeviceKey(boolean bPreBootDevice);
    public native int ApiFwLoad(long key);
    public native int ApiOpen(int fd, long key);
    public native int ApiTune(int freqKhz, int plpid);
    public native int ApiSetPLP(int[] aPlpIds);
    public native int ApiStop();
    public native int ApiClose();
    public native int ApiReset();
    public native int ApiUninit();

    public native int setRfPhyStatisticsViewVisible(boolean isRfPhyStatisticsVisible);

    int atsc3_rf_phy_status_callback(int rfstat_lock,
                                     int rssi,
                                     int modcod_valid,
                                     int plp_fec_type,
                                     int plp_mod,
                                     int plp_cod,
                                     int nRfLevel1000,
                                     int nSnr1000,
                                     int ber_pre_ldpc_e7,
                                     int ber_pre_bch_e9,
                                     int fer_post_bch_e6,
                                     int demod_lock_status,
                                     int cpu_status,
                                     int plp_any,
                                     int plp_all) {

        RfPhyStatistics rfPhyStatistics = new RfPhyStatistics(rfstat_lock,
                rssi,
                modcod_valid,
                plp_fec_type,
                plp_mod,
                plp_cod,
                nRfLevel1000,
                nSnr1000,
                ber_pre_ldpc_e7,
                ber_pre_bch_e9,
                fer_post_bch_e6,
                demod_lock_status,
                cpu_status,
                plp_any,
                plp_all);

        mActivity.pushRfPhyStatisticsUpdate(rfPhyStatistics);

        return 0;
    }

    int atsc3_updateRfBwStats(long total_pkts, long total_bytes, int total_lmts) {
        mActivity.pushBwPhyStatistics(new BwPhyStatistics(total_pkts, total_bytes, total_lmts));
        return 0;
    }

    static {
        //jjustman:2019-11-24: cross reference and circular dependency with NXP_Tuner_Lib and SL API methods
        //System.loadLibrary("NXP_Tuner_Lib");
        //System.loadLibrary("SiTune_Tuner_Libs");

        System.loadLibrary("atsc3NdkPHYBridge");
    }


}
