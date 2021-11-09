package org.ngbp.libatsc3;

import android.os.Handler;
import android.os.Looper;

public abstract class ServiceHandler extends Handler {


    public ServiceHandler() {
        super(Looper.getMainLooper());
        ServiceHandler.Instance = this;
    }

    private static ServiceHandler Instance;

    public static ServiceHandler GetInstance() {
        return ServiceHandler.Instance;
    }

    public static final int DRAW_TEXT_FRAME_VIDEO_ENQUEUE_US = 100;//mVideoEnqueuePresentationTimeUsText
    public static final int DRAW_TEXT_FRAME_VIDEO_RELEASE_RENDERER = 101;
    public static final int DRAW_TEXT_FRAME_AUDIO_ENQUEUE_US= 102;
    public static final int DRAW_TEXT_FRAME_AUDIO_RELEASE_RENDERER = 103;

    public static final int DRAW_TEXT_MFU_METRICS = 150;

    public static final int VIDEO_RESIZE = 200;

    public static final int STPP_IMSC1_AVAILABLE = 300;
    public static final int STPP_IMSC1_CHECK_CLEAR = 301;

    public static final int RF_PHY_STATISTICS_UPDATED = 400;
    public static final int BW_PHY_STATISTICS_UPDATED = 401;

    public static final int TOAST = 500;
    //            Toast.makeText(getApplicationContext(), "Unable to instantiate HEVC decoder!", Toast.LENGTH_SHORT).show();


    public static final int MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE = 1000;

}
