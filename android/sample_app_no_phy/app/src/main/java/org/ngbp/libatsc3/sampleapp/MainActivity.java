package org.ngbp.libatsc3.sampleapp;

import android.Manifest;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.hardware.usb.UsbConfiguration;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;
import android.media.MediaFormat;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.telephony.PhoneNumberFormattingTextWatcher;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.google.android.exoplayer2.ExoPlayerFactory;
import com.google.android.exoplayer2.SimpleExoPlayer;
import com.google.android.exoplayer2.source.dash.DashMediaSource;
import com.google.android.exoplayer2.source.dash.DefaultDashChunkSource;
import com.google.android.exoplayer2.ui.PlayerView;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;
import com.google.android.exoplayer2.upstream.LoadErrorHandlingPolicy;
import com.google.android.exoplayer2.util.Util;

import org.ngbp.libatsc3.a331.LLSParserSLT;
import org.ngbp.libatsc3.a331.models.Service;
import org.ngbp.libatsc3.middleware.Atsc3NdkApplicationBridge;
import org.ngbp.libatsc3.middleware.Atsc3NdkPHYBridge;
import org.ngbp.libatsc3.middleware.android.DebuggingFlags;
import org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload;
import org.ngbp.libatsc3.ServiceHandler;
import org.ngbp.libatsc3.ThingsUI;
import org.ngbp.libatsc3.middleware.android.application.interfaces.IAtsc3NdkApplicationBridgeCallbacks;
import org.ngbp.libatsc3.middleware.android.phy.interfaces.IAtsc3NdkPHYBridgeCallbacks;
import org.ngbp.libatsc3.pcapreplay.PcapFileSelectorActivity;
import org.ngbp.libatsc3.phy.RfScanUtility;
import org.ngbp.libatsc3.middleware.android.ATSC3PlayerFlags;
import org.ngbp.libatsc3.media.ATSC3PlayerMMTFragments;
import org.ngbp.libatsc3.media.DecoderHandlerThread;
import org.ngbp.libatsc3.media.ExoPlayerCustomLoadErrorHandlingPolicy;
import org.ngbp.libatsc3.media.MfuByteBufferHandler;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MmtPacketIdContext;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MpuMetadata_HEVC_NAL_Payload;

import org.ngbp.libatsc3.middleware.android.phy.virtual.PcapDemuxedVirtualPHYAndroid;
import org.ngbp.libatsc3.middleware.android.phy.models.BwPhyStatistics;
import org.ngbp.libatsc3.phy.RfPhyFecModCodTypes;
import org.ngbp.libatsc3.middleware.android.phy.models.RfPhyStatistics;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import static android.text.InputType.TYPE_CLASS_PHONE;

//import android.media.AudioTrack;
/*
import android.support.annotation.NonNull;
import android.support.constraint.ConstraintLayout;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;

 */
//import com.google.android.things.contrib.driver.ht16k33.AlphanumericDisplay;
//import com.google.android.things.contrib.driver.ht16k33.Ht16k33;
//import com.google.android.things.contrib.driver.rainbowhat.RainbowHat;
//import org.ngbp.libatsc3.SimpleTextView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, AdapterView.OnItemSelectedListener, IAtsc3NdkApplicationBridgeCallbacks, IAtsc3NdkPHYBridgeCallbacks {

    private DashMediaSource.Factory dashMediaSourceFactory;
    public static final String DASH_CONTENT_TYPE = "application/dash+xml";

    private int selectedServiceSLSProtocol = 2;
    private Integer selectedServiceId;
    private SurfaceHolder mSurfaceHolder;
    private Surface mDecoderSurface;
    private Spinner mServiceSpinner;

    public File jni_getCacheDir() {
        return super.getCacheDir();
    }

    public static Class clazz = MainActivity.class;

    private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";

    final static String TAG ="main";

    public boolean hasUsbIfSupport = true;

    AssetManager assetManager;

    private DecoderHandlerThread myDecoderHandlerThread;

    /**
     * pcap related support for built-in assets and on-device (e.g. sdcard/download) replay
     *
     * sample files:
     *
     "pcaps/2019-10-17_MMT_HEVC_AAC_IMSC1_tr_FIX-30s-fin.pcap"};
     "pcaps/2019-06-18-hv-digi-4route-1mmt.pcap";
     "pcaps/2019-09-18-cleveland-599mhz-replay.pcap";
     "pcaps/pcap_replay_test.pcap";
     "pcaps/2019-10-17_MMT_HEVC_AAC_IMSC1_tr_FIX.pcap",
     "pcaps/2019-10-17_MMT_HEVC_AAC_IMSC1_tr_FIX-30s-fin.pcap",
     "pcaps/2018-12-17-mmt-airwavz-single-drop.pcap",
     "pcaps/2018-12-17-mmt-airwavz-recalc.pcap",
     "pcaps/2019-09-17-cleveland-2nd-pcap-599-mmt-v2.pcap",
     "pcaps/2019-06-18-hv-digi-4route-1mmt.pcap"
     "pcaps/2019-06-18-hv-digi-4route-1mmt.pcap";
     "pcaps/2019-09-18-cleveland-599mhz-replay.pcap";
     "pcaps/pcap_replay_test.pcap";
    */

    private Boolean inputSelectionFromPcap = false;

    private static final String PCAP_URI_PREFIX = "pcaps/";
    private String inputSelectedPcapReplayFromAssetManager = null;
    private String pcapAssetFromAssetManager[] = { "pcaps/2019-10-29-239.0.0.18.PLP.1.decoded.pcap", "pcaps/2019-12-17-lab-digi-alp.pcap" };

    public static final String SELECT_PCAP_MESSAGE = "Select PCAP from Device...";
    private String inputSelectedPcapReplayFromFilesystem  = null;
    private List<String> pcapAssetForFilesystemReplay = new ArrayList<String>();

    /**
     * end pcap related support..for now
     */

    private EditText editServiceIDText;
    private ConstraintLayout mSurfaceLayout;

    //ButtonsSLSLayout - SLS controls
    private ToggleButton btnToggleIMSC1;
    private TextView textRawIMSC1;
    private TextView imsc1BodyView;

    private ToggleButton btnToggleRfMetrics;
    private ConstraintLayout viewRfMetrics;
    private TextView mRfRssiText;
    private TextView mRfSNRText;
    private TextView mRfBERText;
    private TextView mRfFecModCod;
    private TextView mBwTotalStatusStr;


    private ToggleButton btnToggleMfuMetrics;
    private ConstraintLayout viewMfuMetrics;
    private TextView mAvMfuPtsDelta;

    private TextView mVideoOnFrameRenderedTimeText;
    private TextView mVideoEnqueuePresentationTimeUsText;
    private TextView mAudioOnFrameRenderedTimeText;
    private TextView mAudioEnqueuePresentationTimeUsText;

    private Button btnSetService;
    private Thread viewMfuMetricsWorkerThread = null;
    private boolean viewMfuMetricsWorkerThreadShouldRun;
    private TextView mVideoMfuStatisticsText;

    private ToggleButton btnToggleRfScan;
    private ConstraintLayout rfScanLayout;

    private ToggleButton mRfScanRunToggleBtn;
    private EditText mEditFreqMhz;

    private PlayerView simpleExoPlayerView;
    private SimpleExoPlayer simpleExoPlayer = null;

    private static Size calcImageSize(MediaFormat format) {
        int cropLeft = format.getInteger("crop-left");
        int cropRight = format.getInteger("crop-right");
        int cropTop = format.getInteger("crop-top");
        int cropBottom = format.getInteger("crop-bottom");
        int width = cropRight + 1 - cropLeft;
        int height = cropBottom + 1 - cropTop;
        return new Size(width, height);
    }

    //TODO - only update this if our NAL payload init has changed...
    public void pushMpuMetadata_HEVC_NAL_Payload(MpuMetadata_HEVC_NAL_Payload mpuMetadata_hevc_nal_payload) {
        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback && ATSC3PlayerMMTFragments.InitMpuMetadata_HEVC_NAL_Payload == null) {
            ATSC3PlayerMMTFragments.InitMpuMetadata_HEVC_NAL_Payload = mpuMetadata_hevc_nal_payload;
        } else {
            mpuMetadata_hevc_nal_payload.releaseByteBuffer();
        }
    }

    //hack-ish
    public static RfPhyStatistics lastRfPhyStatistics = null;
    public boolean isRfPhyStatisticsViewVisible = false;

    public void pushRfPhyStatisticsUpdate(RfPhyStatistics rfPhyStatistics) {
        if(isRfPhyStatisticsViewVisible) {
            ServiceHandler.GetInstance().sendMessage(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.RF_PHY_STATISTICS_UPDATED, rfPhyStatistics));
            lastRfPhyStatistics = rfPhyStatistics;
        }
    }

    public void pushBwPhyStatistics(BwPhyStatistics bwPhyStatistics) {
        ServiceHandler.GetInstance().sendMessage(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.BW_PHY_STATISTICS_UPDATED, bwPhyStatistics));
    }

    //jjustman-2020-08-07 - temporary wire-up, and  remove static field accessor static
    public Atsc3NdkApplicationBridge atsc3NdkApplicationBridge;
    public Atsc3NdkPHYBridge         atsc3NdkPHYBridge;
    public PcapDemuxedVirtualPHYAndroid demuxedPcapVirtualPHY;

    public UsbManager mUsbManager;
    private PendingIntent mPermissionIntent;

    // usb device list
    private List<atsc3UsbDevice> mAt3Devices = new ArrayList<atsc3UsbDevice>();
    private atsc3UsbDevice mCurAt3Device = null;

    // log message view
    private TextView mTextView;
    private TextView mSLSView;
    private TextView mAlcObjectStatusMsg;

    //private ScrollView mScrollView; // scroll view contains text view
    private Spinner mDevListSpinner;

    public static SurfaceView mSurfaceView1;
    

    Boolean hasSetSurfaceView = false;
    Boolean isSurfaceViewFullScreen = false;


    Integer originalVisibility = null;
    Integer lastConfigurationOrientation = null;

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        if(lastConfigurationOrientation != null && newConfig.orientation == lastConfigurationOrientation) {
            return;
        }

        // Checks the orientation of the screen
        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            if (originalVisibility == null) {
                originalVisibility = decorView.getSystemUiVisibility();
            }

            // Set the content to appear under the system bars so that the
            // content doesn't resize when the system bars hide and show.
            // Hide the nav bar and status bar
            //                            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE
                            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN);

        } else if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT){

            /*
            To enable immersive mode, call setSystemUiVisibility() and pass the SYSTEM_UI_FLAG_IMMERSIVE flag in conjunction with SYSTEM_UI_FLAG_FULLSCREEN and SYSTEM_UI_FLAG_HIDE_NAVIGATION.
                https://developer.android.com/training/system-ui/immersive
             */
            int uiOptions = View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_IMMERSIVE | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
            decorView.setSystemUiVisibility(uiOptions);
//
//            DisplayMetrics displayMetrics = new DisplayMetrics();
//            getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
//            ViewGroup.LayoutParams videoParams = mSurfaceView1.getLayoutParams();
//
//            videoParams.width = 640;
//            videoParams.height = 360;
            Log.e("onConfigurationChanged", "resetting videoParams to w: 640 and h: 360");
        }

        lastConfigurationOrientation = newConfig.orientation;
        ServiceHandler.GetInstance().sendMessageDelayed(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.VIDEO_RESIZE), 1);
    }

    void videoViewFullscreen() {
        //getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }

    void videoViewResumeSize() {
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }

    Boolean hasSetOriginalSurfaceLayout = false;
    int surfaceLayoutTop, surfaceLayoutLeft, surfaceLayoutRight, surfaceLayoutBottom;

    private void videoViewResize() {
        if(!hasSetOriginalSurfaceLayout) {
            surfaceLayoutTop = mSurfaceView1.getTop();
            surfaceLayoutLeft = mSurfaceView1.getLeft();
            surfaceLayoutRight = mSurfaceView1.getRight();
            surfaceLayoutBottom = mSurfaceView1.getBottom();
            hasSetOriginalSurfaceLayout = true;
        }

        Rect myWindowRect = new Rect();
        decorView.getWindowVisibleDisplayFrame(myWindowRect);

        if(isSurfaceViewFullScreen) {
            hideNonVideoLayouts();

            ViewGroup.LayoutParams videoParams = mSurfaceLayout.getLayoutParams();
            videoParams.width = myWindowRect.width();
            videoParams.height = myWindowRect.height();
            mSurfaceLayout.setTop(0);
            mSurfaceLayout.setLeft(0);
            mSurfaceLayout.setRight(myWindowRect.width());
            mSurfaceLayout.setBottom(myWindowRect.width());
            mSurfaceView1.setTop(0);
            mSurfaceView1.setLeft(0);
            mSurfaceView1.setRight(myWindowRect.width());
            mSurfaceView1.setBottom(myWindowRect.width());
            mSurfaceHolder.setFixedSize(myWindowRect.width(), myWindowRect.height());


//            mSurfaceView1.setTop(0);
//            mSurfaceView1.setLeft(0);
//            mSurfaceView1.setRight(myWindowRect.width());
//            mSurfaceView1.setBottom(myWindowRect.height());
////            mSurfaceLayout.setMinHeight(myWindowRect.height());
////            mSurfaceLayout.setMinWidth(myWindowRect.width());
////
//            mSurfaceLayout.setRight(myWindowRect.width());
//            mSurfaceLayout.setBottom(myWindowRect.height());


            Log.i("surfaceChanged", String.format("resizing to: isSurfaceViewFullScreen: true, myWindowRect.w: %d, myWindowRect.h: %d", myWindowRect.width(), myWindowRect.height()));
        } else {
            showNonVideoLayouts();
            //doesn't do anything..myDecoderHandlerThread.myDecoderSurfaceHolder.setFixedSize(MmtPacketIdContext.video_packet_statistics.width, MmtPacketIdContext.video_packet_statistics.height);

//            mSurfaceView1.setTop(surfaceLayoutTop);
//            mSurfaceView1.setLeft(surfaceLayoutLeft);
//            mSurfaceView1.setRight(surfaceLayoutRight);
//            mSurfaceView1.setBottom(surfaceLayoutBottom);
            if(MmtPacketIdContext.video_packet_statistics.width >0 && MmtPacketIdContext.video_packet_statistics.height >0) {

                mSurfaceView1.getHolder().setFixedSize(MmtPacketIdContext.video_packet_statistics.width, MmtPacketIdContext.video_packet_statistics.height);
//                ViewGroup.LayoutParams videoParams = mSurfaceLayout.getLayoutParams();
//                videoParams.width = MmtPacketIdContext.video_packet_statistics.width;
//                videoParams.height = MmtPacketIdContext.video_packet_statistics.height;
//                mSurfaceView1.setLeft(0);
//                mSurfaceView1.setRight(myWindowRect.width());
//
//                mSurfaceLayout.setLeft(0);
//                mSurfaceLayout.setRight(myWindowRect.width());
                Log.i("surfaceChanged", String.format("resizing to: isSurfaceViewFullScreen: false, myWindowRect.w: %d, myWindowRect.h: %d", surfaceLayoutRight, surfaceLayoutBottom));
            }
        }
    }

    private void hideNonVideoLayouts() {
        ((LinearLayout)findViewById(R.id.DebugAndSLSMessages)).setVisibility(View.GONE);
        ((LinearLayout)findViewById(R.id.SelectDeviceLayout)).setVisibility(View.GONE);
        ((LinearLayout)findViewById(R.id.ButtonsPrimaryLayout)).setVisibility(View.GONE);
        ((LinearLayout)findViewById(R.id.ButtonsSLSLayout)).setVisibility(View.GONE);
    }

    private void showNonVideoLayouts() {
        ((LinearLayout)findViewById(R.id.DebugAndSLSMessages)).setVisibility(View.VISIBLE);
        ((LinearLayout)findViewById(R.id.SelectDeviceLayout)).setVisibility(View.VISIBLE);
        ((LinearLayout)findViewById(R.id.ButtonsPrimaryLayout)).setVisibility(View.VISIBLE);
        ((LinearLayout)findViewById(R.id.ButtonsSLSLayout)).setVisibility(View.VISIBLE);
    }

    private void hideNonRfScanLayouts() {
        ((LinearLayout)findViewById(R.id.DebugAndSLSMessages)).setVisibility(View.GONE);
        ((LinearLayout)findViewById(R.id.SelectDeviceLayout)).setVisibility(View.GONE);
        ((LinearLayout)findViewById(R.id.ButtonsPrimaryLayout)).setVisibility(View.GONE);
        mAlcObjectStatusMsg.setVisibility(View.GONE);
        mSurfaceLayout.setVisibility(View.GONE);
    }

    private void showNonRfScanLayouts() {
        ((LinearLayout)findViewById(R.id.DebugAndSLSMessages)).setVisibility(View.VISIBLE);
        ((LinearLayout)findViewById(R.id.SelectDeviceLayout)).setVisibility(View.VISIBLE);
        ((LinearLayout)findViewById(R.id.ButtonsPrimaryLayout)).setVisibility(View.VISIBLE);
        mAlcObjectStatusMsg.setVisibility(View.VISIBLE);

        mSurfaceLayout.setVisibility(View.VISIBLE);

    }

    public Handler serviceHandler;

    //deprecated jjustman-2019-10-10 - final static String kSpinnerNoDevice = "No device";

    final static String kSpinnerNotSelect = "Not selected";

    // update spinner items from mAt3Devices dynamically
    private void updateSpinnerFromDevList() {
        Log.d(TAG, "update spinner list..");

        List<String> items = new ArrayList<>();
        // if previously some device is selected, keep that item selected after update.
        String itemSelected = (String)mDevListSpinner.getSelectedItem();

        items.add(kSpinnerNotSelect);
        int itemCount = 1;
        int idxSelected = 0;

        for (atsc3UsbDevice ad : mAt3Devices) {
            if (mCurAt3Device != null && mCurAt3Device.toString().equalsIgnoreCase(ad.toString())) {
                idxSelected = itemCount;
            } else if(itemSelected != null && itemSelected.equals(ad.toString())) {
                idxSelected = itemCount;
            }
            items.add(ad.toString());

            Log.d(TAG, String.format("dropdown idx: %d, fd: %s, deviceName: %s, key: %s", itemCount, ad.fd, ad.dev.getDeviceName(), ad.toString()));
            itemCount++;
        }

        if(pcapAssetFromAssetManager.length > 0) {
            items.add("---");
            itemCount++;

            for(int i=0; i < pcapAssetFromAssetManager.length; i++) {
                if(itemSelected != null && itemSelected.equalsIgnoreCase(pcapAssetFromAssetManager[i])) {
                    idxSelected = itemCount;
                }
                items.add(pcapAssetFromAssetManager[i]);
                itemCount++;
            }
            items.add("---");
            itemCount++;

        }

        items.add(SELECT_PCAP_MESSAGE);
        itemCount++;

        for(int i=0; i < pcapAssetForFilesystemReplay.size(); i++) {
            if(inputSelectedPcapReplayFromFilesystem != null && inputSelectedPcapReplayFromFilesystem.equalsIgnoreCase(pcapAssetForFilesystemReplay.get(i))) {
                idxSelected = itemCount;

            } else if(itemSelected != null && itemSelected.equalsIgnoreCase(pcapAssetForFilesystemReplay.get(i))) {
                idxSelected = itemCount;
            }
            items.add(pcapAssetForFilesystemReplay.get(i));
            itemCount++;
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, items);
        adapter.setDropDownViewResource(R.layout.spinner_style);
        mDevListSpinner.setAdapter(adapter);


        if (idxSelected > 0) {
            Log.d(TAG, String.format("set spinner selection, index: %d", idxSelected));
            mDevListSpinner.setSelection(idxSelected);
        } else if(mAt3Devices.size() == 1 && idxSelected == 0) {
            //auto-select if only one i/f
            mDevListSpinner.setSelection(1);
        }
    }

    private void enableDeviceOpenButtons(boolean bEnable) {
        ((Button)findViewById(R.id.butOpen)).setEnabled(bEnable);
        ((Button)findViewById(R.id.butOpen)).requestFocus();
        ((Button)findViewById(R.id.butClose)).setEnabled(bEnable);
        disableButtonsSLSLayout();
    }

    private void enableDeviceControlButtons(boolean bEnable) {
        // this should be used when device is already open.
        ((Button)findViewById(R.id.butTune)).setEnabled(bEnable);
        mServiceSpinner.setEnabled(bEnable);

        ((Button)findViewById(R.id.butStop)).setEnabled(bEnable);
        ((Button)findViewById(R.id.butPlay)).setEnabled(bEnable);
        ((Button)findViewById(R.id.butReset)).setEnabled(bEnable);

        ((EditText)findViewById(R.id.editFreqMhz)).setEnabled(bEnable);
        ((EditText)findViewById(R.id.editPlp)).setEnabled(bEnable);

        enableButtonsSLSLayout();
        btnToggleRfScan.setEnabled(bEnable);
    }

    int oldHeight;
    int oldWidth;
    View decorView;

    Boolean hasCalledOnCreate = false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        if(hasCalledOnCreate) {
            Log.d(TAG, "OnCreate:hasCalledOnCreate is true, returning");
            return;
        }

        MmtPacketIdContext.Initialize();
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        Log.d(TAG, "OnCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        createServiceHandler();

        decorView = getWindow().getDecorView();

        simpleExoPlayerView = findViewById(R.id.player_view);

        oldHeight = decorView.getHeight();
        oldWidth = decorView.getWidth();

        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN);

        // check whether this system support usb host.
        PackageManager pm = getPackageManager();
        if (!pm.hasSystemFeature(PackageManager.FEATURE_USB_HOST)) {
            Toast.makeText(getApplicationContext(), "onCreate: FEATURE_USB_HOST not supported", Toast.LENGTH_SHORT).show();
            Log.d(TAG, "onCreate: FEATURE_USB_HOST not supported");
            hasUsbIfSupport = false;
        }

        mSurfaceView1 = (SurfaceView) findViewById(R.id.surfaceview1);
        mSurfaceLayout = (ConstraintLayout)findViewById(R.id.videoPreview);

        mSurfaceView1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                hasSetSurfaceView = true;
            if(!isSurfaceViewFullScreen) {
                isSurfaceViewFullScreen = true;
                videoViewFullscreen();

            } else {
                isSurfaceViewFullScreen = false;
                videoViewResumeSize();
            }
            }
        });

        if(DebuggingFlags.ENABLE_LAYER_TYPE_HARDWARE) {
            ((LinearLayout) findViewById(R.id.DebugAndSLSMessages)).setLayerType(View.LAYER_TYPE_HARDWARE, null);
            ((LinearLayout) findViewById(R.id.SelectDeviceLayout)).setLayerType(View.LAYER_TYPE_HARDWARE, null);
            ((LinearLayout) findViewById(R.id.ButtonsPrimaryLayout)).setLayerType(View.LAYER_TYPE_HARDWARE, null);
            ((LinearLayout) findViewById(R.id.ButtonsSLSLayout)).setLayerType(View.LAYER_TYPE_HARDWARE, null);
        }
        myDecoderHandlerThread = new DecoderHandlerThread("decoderHandlerThread");
        mSurfaceHolder = mSurfaceView1.getHolder();

        mSurfaceHolder.addCallback(new SurfaceHolder.Callback() {

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.v(TAG, String.format("surfaceChanged format=" + format + ", width=" + width + ", height=" + height));
                ServiceHandler.GetInstance().sendMessageDelayed(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.VIDEO_RESIZE), 1);
            }

            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Log.v(TAG, String.format("surfaceCreated, holder: %s", holder));
                mDecoderSurface = holder.getSurface();
                myDecoderHandlerThread.setOutputSurface(holder, mDecoderSurface);

                ServiceHandler.GetInstance().sendMessageDelayed(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.VIDEO_RESIZE), 1);
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.v(TAG, "surfaceDestroyed");
                myDecoderHandlerThread.clearOutputSurface();
                //hack
                ServiceHandler.GetInstance().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        mSurfaceView1.getHolder();

                    }
                }, 1000);

            }
        });


        myDecoderHandlerThread.start();
        mSurfaceView1.getHolder();

        if (hasUsbIfSupport) {
            // our broadcast receiver
            mUsbReceiver = new MyReceiver(ServiceHandler.GetInstance());
        }

        assetManager = getAssets();

        // UI things
        mTextView = (TextView) findViewById(R.id.textMsg);
        if(DebuggingFlags.ENABLE_LAYER_TYPE_HARDWARE) {
            mTextView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
        }

        mServiceSpinner = (Spinner) findViewById(R.id.spinServices);
        mServiceSpinner.setOnFocusChangeListener(onFocusedChangedListenerUiBtn);

        mServiceSpinner.setEnabled(false);
        mServiceSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
              @Override
              public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                  Service service = (Service) parent.getItemAtPosition(position);

                  selectedServiceId = service.serviceId;
                  editServiceIDText.setText(""+service.serviceId);

                  //notify libatsc3 service selection
                  selectedServiceSLSProtocol = atsc3NdkApplicationBridge.atsc3_slt_selectService(service.serviceId);
                  if(!serviceSpinnerLastSelectionFromArrayAdapterUpdate) {
                      clearDebugTextViewFields();
                  }
                  if (selectedServiceSLSProtocol > 0) {
                      selectedServiceId = selectedServiceId;

                      if(selectedServiceSLSProtocol == 1) {
                          //wait at least 2 seconds for MBMS to be persisted
                          try {
                              Thread.sleep(500);
                          } catch (InterruptedException e) {
                              e.printStackTrace();
                          }
                      }
                      ((Button)findViewById(R.id.butPlay)).setEnabled(true);
                      if(serviceSpinnerLastSelectionFromArrayAdapterUpdate) {
                          mServiceSpinner.requestFocus();
                          serviceSpinnerLastSelectionFromArrayAdapterUpdate = false;
                      } else {
                          ((Button) findViewById(R.id.butPlay)).requestFocus();
                      }
                  }
                  textRawIMSC1.setText("");

              }

              @Override
              public void onNothingSelected(AdapterView<?> parent) {

              }
          });

        mSLSView = (TextView) findViewById(R.id.slsMsg);
        mSLSView.setMovementMethod(new ScrollingMovementMethod());

        mAlcObjectStatusMsg = (TextView) findViewById(R.id.alcObjectStatusMsg);
        mAlcObjectStatusMsg.setMovementMethod(new ScrollingMovementMethod());

        //mScrollView = (ScrollView) findViewById(R.id.scrollView);
        mTextView.setText("");
        mTextView.setMovementMethod(new ScrollingMovementMethod());

        mDevListSpinner = (Spinner) findViewById(R.id.spinDevices);
        mDevListSpinner.setOnItemSelectedListener(this);

        // get at3drv jni interface
        //jjustman-2020-08-07 app and phy refactoring
        atsc3NdkApplicationBridge = new Atsc3NdkApplicationBridge(this);
        atsc3NdkPHYBridge = new Atsc3NdkPHYBridge(this);
        // if needed at runtime for pcap replay:
        demuxedPcapVirtualPHY = new PcapDemuxedVirtualPHYAndroid();


        // get usb manager
        mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

        // get pending intent, which will be used for requesting permission
        mPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);

        // register our own broadcast receiver instance, with filters we are interested in
        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        filter.addAction(ACTION_USB_PERMISSION);

        Log.d(TAG, "onCreate: registering intent to receiver..");
        registerReceiver(mUsbReceiver, filter);

        // init at3drv driver
        showMsg("onCreate: ApiInit\n");
        atsc3NdkPHYBridge.init();

        ThingsUI.WriteToAlphaDisplayNoEx("AINT");

        // now, scan usb devices and try to connect
        ServiceHandler.GetInstance().postDelayed(new Runnable() {
            @Override
            public void run() {
                scanDevices();
                // Spinner, https://developer.android.com/guide/topics/ui/controls/spinner.html
                ServiceHandler.GetInstance().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        mDevListSpinner.setOnFocusChangeListener(onFocusedChangedListenerUiBtn);
                        mDevListSpinner.requestFocus();
                    }
                }, 1000);

            }
        }, 500);


        // add click listener for all button and disable them
        ((Button) findViewById(R.id.butOpen)).setOnClickListener(this);
        ((Button) findViewById(R.id.butTune)).setOnClickListener(this);
        ((Button) findViewById(R.id.butPlay)).setOnClickListener(this);
        ((Button) findViewById(R.id.butStop)).setOnClickListener(this);
        ((Button) findViewById(R.id.butReset)).setOnClickListener(this);
        ((Button) findViewById(R.id.butClose)).setOnClickListener(this);

        //add focus changed listener -- onFocusedChangedListenerUiBtn
        ((Button) findViewById(R.id.butOpen)).setOnFocusChangeListener(onFocusedChangedListenerUiBtn);
        ((Button) findViewById(R.id.butTune)).setOnFocusChangeListener(onFocusedChangedListenerUiBtn);
        ((Button) findViewById(R.id.butPlay)).setOnFocusChangeListener(onFocusedChangedListenerUiBtn);
        ((Button) findViewById(R.id.butStop)).setOnFocusChangeListener(onFocusedChangedListenerUiBtn);
        ((Button) findViewById(R.id.butReset)).setOnFocusChangeListener(onFocusedChangedListenerUiBtn);
        ((Button) findViewById(R.id.butClose)).setOnFocusChangeListener(onFocusedChangedListenerUiBtn);

        /*
        rf scan function
         */

        btnToggleRfScan = (ToggleButton)findViewById(R.id.btnToggleRfScan);
        btnToggleRfScan.setEnabled(false); //only enable when we are "open" on phy
        btnToggleRfScan.setOnFocusChangeListener(onFocusedChangedListenerUiBtn);

        rfScanLayout = (ConstraintLayout) findViewById(R.id.RfScanLayout);

        btnToggleRfScan.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            if (isChecked) {
                // The toggle is enabled
                rfScanLayout.setVisibility(View.VISIBLE);

                hideNonRfScanLayouts();
                rfScanUtility.setEnabled(true);

            } else {
                // The toggle is disabled
                rfScanLayout.setVisibility(View.GONE);
                showNonRfScanLayouts();
                rfScanUtility.setEnabled(false);
            }
            }
        });

        rfScanUtility = new RfScanUtility(this);
        rfScanThread = new Thread(rfScanUtility);
        mRfScanRunToggleBtn = (ToggleButton)findViewById(R.id.RfScanRunToggleBtn);
        mRfScanRunToggleBtn.setOnFocusChangeListener(onFocusedChangedListenerUiBtn);
        mRfScanRunToggleBtn.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {

                    while(rfScanUtility != null && rfScanUtility.isRunning) {
                        rfScanUtility.shouldRun = false;
                        try {
                            Thread.sleep(100);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }

                    // The toggle is enabled
                    rfScanThread.start();
                } else {
                    // The toggle is disabled
                    if(rfScanUtility != null) {
                        rfScanUtility.shouldRun = false;
                    }
                    if(rfScanThread != null) {
                        try {
                            rfScanThread.join();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    rfScanUtility = null;
                    rfScanThread = null;
                }
            }
        });

        setupButtonsSLSLayout();

        enableDeviceOpenButtons(false);
        enableDeviceControlButtons(false);

        mEditFreqMhz = ((EditText)findViewById(R.id.editFreqMhz));
        mEditFreqMhz.setInputType(TYPE_CLASS_PHONE);
        mEditFreqMhz.addTextChangedListener(new PhoneNumberFormattingTextWatcher());

        //make sure we can read from device pcap files
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                Log.w("onCreate","android.permission.WRITE_EXTERNAL_STORAGE missing!");
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    ServiceHandler.MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE);
        }

        //auto-debug mode
        ServiceHandler.GetInstance().postDelayed(new Runnable() {
            @Override
            public void run() {
                try {
                    if(true) {
                        return;
                    }
                    //OTG dongle
                    if (mAt3Devices.size() > 0) {

                        ((Button) findViewById(R.id.butOpen)).setEnabled(true);
                        mDevListSpinner.setSelection(1);
                        onItemSelected(mDevListSpinner, mDevListSpinner, 1, 0);
                        mDevListSpinner.invalidate();
                        ((Button) findViewById(R.id.butOpen)).requestFocus();
                        Thread.sleep(250);
                        ((Button) findViewById(R.id.butOpen)).performClick();

                        if(false) {
                            Thread.sleep(250);
                            ((EditText) findViewById(R.id.editFreqMhz)).setText("575");
                            ((EditText) findViewById(R.id.editPlp)).setText("0");

                            ((Button) findViewById(R.id.butTune)).performClick();
                            ServiceHandler.GetInstance().postDelayed(new Runnable() {
                                                                         @Override
                                                                         public void run() {
                                                                         ((Button) findViewById(R.id.butPlay)).requestFocus();
                                                                         ((Button) findViewById(R.id.butPlay)).performClick();
                                                                         btnToggleRfMetrics.performClick();

                                                                     }
                                                                 }, 6000);
                        }
                    } else if(false) { //local PCAP auto-start replay
                        ((Button) findViewById(R.id.butOpen)).setEnabled(true);
                        mDevListSpinner.setSelection(2);
                        onItemSelected(mDevListSpinner, mDevListSpinner, 1, 0);
                        mDevListSpinner.invalidate();
                        ((Button) findViewById(R.id.butOpen)).requestFocus();

                        ServiceHandler.GetInstance().postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                ((Button) findViewById(R.id.butOpen)).performClick();


                                ServiceHandler.GetInstance().postDelayed(new Runnable() {
                                    @Override
                                    public void run() {
                                        ((Button) findViewById(R.id.butPlay)).performClick();

                                        enableButtonsSLSLayout();
                                        if(false) {
                                            btnToggleMfuMetrics.performClick();
                                        }
                                    }
                                }, 6000);
                            }}, 1000);



                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

            }
        }, 3000);

        hasCalledOnCreate = true;
    }


    public Drawable lastBackgroundDrawable = null;
    public View.OnFocusChangeListener onFocusedChangedListenerUiBtn = new View.OnFocusChangeListener() {

        @Override
        public void onFocusChange(View v, boolean hasFocus) {
            Drawable borderDrawable = v.getBackground();
            GradientDrawable border = null;
            if(borderDrawable instanceof GradientDrawable) {
                border = (GradientDrawable) borderDrawable;
            } else {
                border = new GradientDrawable();
            }

            if(hasFocus) {
                //border.setColor(0xFFE7E7E7); //white background
                border.setStroke(2, 0xEEf542c5); //black border with full opacity
                lastBackgroundDrawable = borderDrawable;
                v.setBackground(border);
            } else if(lastBackgroundDrawable != null){
                v.setBackground(lastBackgroundDrawable);
            }
        }
    };


    public void pushMfuByteBufferFragment(MfuByteBufferFragment mfuByteBufferFragment) {
        MfuByteBufferHandler.PushMfuByteBufferFragment(mfuByteBufferFragment);
    }

    RfScanUtility rfScanUtility;
    Thread rfScanThread;

    private void setupButtonsSLSLayout() {

        rfScanLayout.setVisibility(View.GONE);
        btnToggleIMSC1 = (ToggleButton)findViewById(R.id.btnToggleIMSC1);
        btnToggleIMSC1.setOnFocusChangeListener(onFocusedChangedListenerUiBtn);

        textRawIMSC1 = (TextView)findViewById(R.id.textRawIMSC1);
        imsc1BodyView = (TextView) findViewById(R.id.imsc1BodyView);
        imsc1BodyView.setVisibility(View.GONE);

        btnToggleRfMetrics = (ToggleButton)findViewById(R.id.btnToggleRfMetrics);
        btnToggleRfMetrics.setOnFocusChangeListener(onFocusedChangedListenerUiBtn);

        viewRfMetrics = (ConstraintLayout)findViewById(R.id.RfMetricsView);
        viewRfMetrics.setVisibility(View.GONE);
        if(DebuggingFlags.ENABLE_LAYER_TYPE_HARDWARE) {
            viewRfMetrics.setLayerType(View.LAYER_TYPE_HARDWARE, null);
        }

        mRfFecModCod = (TextView) findViewById(R.id.RfFecModCod);
        mRfRssiText = (TextView) findViewById(R.id.RfRssiText);
        mRfSNRText = (TextView) findViewById(R.id.RfSNRText);
        mRfBERText = (TextView) findViewById(R.id.RfBERText);
        mBwTotalStatusStr = (TextView) findViewById(R.id.BwTotalStatusStr);
        
        btnToggleMfuMetrics = (ToggleButton)findViewById(R.id.btnToggleMfuMetrics);
        btnToggleMfuMetrics.setOnFocusChangeListener(onFocusedChangedListenerUiBtn);

        viewMfuMetrics = (ConstraintLayout)findViewById(R.id.MfuMetricsView);
        viewMfuMetrics.setVisibility(View.GONE);
        viewMfuMetrics.setLayerType(View.LAYER_TYPE_HARDWARE, null);

        mAvMfuPtsDelta = (TextView) findViewById(R.id.avMfuPtsDelta);
        mVideoOnFrameRenderedTimeText = (TextView) findViewById(R.id.videoOnFrameRenderedTime);
        mVideoEnqueuePresentationTimeUsText = (TextView) findViewById(R.id.videoEnqueuePresentationTimeUsText);
        mAudioOnFrameRenderedTimeText = (TextView) findViewById(R.id.audioOnFrameRenderedTime);
        mAudioEnqueuePresentationTimeUsText = (TextView) findViewById(R.id.audioEnqueuePresentationTimeUsText);

        mVideoMfuStatisticsText = (TextView) findViewById(R.id.videoMfuStatisticsText);

        btnSetService = (Button)findViewById(R.id.btnSetService);
        btnSetService.setOnFocusChangeListener(onFocusedChangedListenerUiBtn);

        editServiceIDText = (EditText)findViewById(R.id.editServiceID);
        editServiceIDText.setEnabled(false);

        btnToggleIMSC1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    // The toggle is enabled
                    textRawIMSC1.setVisibility(View.VISIBLE);
                    mSLSView.setVisibility(View.GONE);
                    mAlcObjectStatusMsg.setVisibility(View.GONE);


                } else {
                    // The toggle is disabled
                    textRawIMSC1.setVisibility(View.GONE);
                    mSLSView.setVisibility(View.VISIBLE);
                    mAlcObjectStatusMsg.setVisibility(View.VISIBLE);

                }
            }
        });

        btnToggleRfMetrics.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    // The toggle is enabled
                    viewRfMetrics.setVisibility(View.VISIBLE);
                    atsc3NdkPHYBridge.setRfPhyStatisticsViewVisible(true);
                    isRfPhyStatisticsViewVisible = true;

                } else {
                    // The toggle is disabled
                    viewRfMetrics.setVisibility(View.GONE);
                    atsc3NdkPHYBridge.setRfPhyStatisticsViewVisible(false);
                    isRfPhyStatisticsViewVisible = false;
                }
            }
        });

        //set background polling thread for data re-bind...
        btnToggleMfuMetrics.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    // The toggle is enabled
                    viewMfuMetrics.setVisibility(View.VISIBLE);
                    if(DebuggingFlags.ENABLE_LAYER_TYPE_HARDWARE) {
                        viewMfuMetrics.setLayerType(View.LAYER_TYPE_HARDWARE, null);
                    }
                    if(DebuggingFlags.MFU_STATS_RENDERING) {
                        viewMfuMetricsWorkerThreadShouldRun = true;
                        viewMfuMetricsWorkerThread = new Thread(new Runnable() {

                            @Override
                            public void run() {
                                while (viewMfuMetricsWorkerThreadShouldRun) {
                                    ServiceHandler.GetInstance().sendMessage(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.DRAW_TEXT_MFU_METRICS));
                                    try {
                                        Thread.sleep(1000);
                                    } catch (InterruptedException e) {
                                        e.printStackTrace();
                                    }
                                }
                            }
                        });
                        viewMfuMetricsWorkerThread.start();
                    }

                } else {
                    // The toggle is disabled
                    viewMfuMetrics.setVisibility(View.GONE);
                    viewMfuMetricsWorkerThreadShouldRun = false;
                    if(viewMfuMetricsWorkerThread != null) {
                        try {
                            viewMfuMetricsWorkerThread.join();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        viewMfuMetricsWorkerThread = null;
                    }
                }
            }
        });


        btnSetService.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Integer selectedServiceIdToSet = Integer.parseInt(editServiceIDText.getText().toString());
                if(selectedServiceIdToSet != null) {
                    selectedServiceSLSProtocol = atsc3NdkApplicationBridge.atsc3_slt_selectService(selectedServiceIdToSet.intValue());
                    if(selectedServiceSLSProtocol > 0) {
                        selectedServiceId = selectedServiceIdToSet;
                    }
                    //clear out any pending MFUs queues, e.g. logical channel change
                    textRawIMSC1.setText("");

                }
            }
        });
        disableButtonsSLSLayout();
    }

    private void disableButtonsSLSLayout() {
        btnToggleIMSC1.setChecked(false);
        btnToggleIMSC1.setEnabled(false);

        btnToggleRfMetrics.setChecked(false);
        btnToggleRfMetrics.setEnabled(false);

        btnToggleMfuMetrics.setChecked(false);
        btnToggleMfuMetrics.setEnabled(false);

        btnSetService.setEnabled(false);
        editServiceIDText.setEnabled(false);
    }
    private void enableButtonsSLSLayout() {
        btnToggleIMSC1.setEnabled(true);
        btnToggleRfMetrics.setEnabled(true);
        btnToggleMfuMetrics.setEnabled(true);
        btnSetService.setEnabled(true);
        editServiceIDText.setEnabled(true);
    }

    private void createServiceHandler() {

        serviceHandler = new ServiceHandler() {
            public void handleMessage(Message msg) {
                if (msg.what == 1) { // update log
                    String str = (String) msg.obj;
                    showMsg("# " + str);
                } else if (msg.what == 2) { // connect device and prepare
                    UsbDevice device = (UsbDevice) msg.obj;
                    Log.d(TAG, "handler: connect device");
                    connectDeviceAndPrepare(device);
                    Log.d(TAG, "---- end of handling connect device");
                } else if (msg.what == 3) { // disconnect device and prepare
                    UsbDevice device = (UsbDevice) msg.obj;
                    Log.d(TAG, "handler: disconnected device");
                    prepareDevices();
                    Log.d(TAG, "---- end of handling discconecting device");
                } else if (msg.what == ServiceHandler.DRAW_TEXT_FRAME_VIDEO_ENQUEUE_US) {
                    mVideoEnqueuePresentationTimeUsText.setText((String) msg.obj);
                } else if (msg.what == ServiceHandler.DRAW_TEXT_FRAME_VIDEO_RELEASE_RENDERER) {
                    mVideoOnFrameRenderedTimeText.setText((String) msg.obj);
                    if (MmtPacketIdContext.audio_packet_statistics.last_output_buffer_presentationTimeUs > 0 && MmtPacketIdContext.video_packet_statistics.last_output_buffer_presentationTimeUs > 0) {
                        mAvMfuPtsDelta.setText(String.format("+A/-V deltaMS: %d", (MmtPacketIdContext.audio_packet_statistics.last_output_buffer_presentationTimeUs - MmtPacketIdContext.video_packet_statistics.last_output_buffer_presentationTimeUs) / 1000));
                    }
                } else if (msg.what == DRAW_TEXT_FRAME_AUDIO_ENQUEUE_US) {
                    mAudioEnqueuePresentationTimeUsText.setText((String) msg.obj);
                } else if (msg.what == DRAW_TEXT_FRAME_AUDIO_RELEASE_RENDERER) {
                    mAudioOnFrameRenderedTimeText.setText((String) msg.obj);
                    if (MmtPacketIdContext.audio_packet_statistics.last_output_buffer_presentationTimeUs > 0 && MmtPacketIdContext.video_packet_statistics.last_output_buffer_presentationTimeUs > 0) {
                        mAvMfuPtsDelta.setText(String.format("+A/-V deltaMS: %d", (MmtPacketIdContext.audio_packet_statistics.last_output_buffer_presentationTimeUs - MmtPacketIdContext.video_packet_statistics.last_output_buffer_presentationTimeUs) / 1000));
                    }
                } else if (msg.what == VIDEO_RESIZE) {
                    videoViewResize();
                } else if (msg.what == RF_PHY_STATISTICS_UPDATED) {
                    RfPhyStatistics rfPhyStatistics = (RfPhyStatistics) msg.obj;
                    mRfFecModCod.setText(String.format("FEC MOD/COD: Valid: %d, %s (%d), %s (%d), %s (%d)",
                            rfPhyStatistics.modcod_valid,
                            RfPhyFecModCodTypes.L1d_PlpFecType.getOrDefault(rfPhyStatistics.plp_fec_type, RfPhyFecModCodTypes.L1d_PlpFecType.get(255)),
                            rfPhyStatistics.plp_fec_type,
                            RfPhyFecModCodTypes.L1d_PlpMod.getOrDefault(rfPhyStatistics.plp_mod, RfPhyFecModCodTypes.L1d_PlpMod.get(255)),
                            rfPhyStatistics.plp_mod,
                            RfPhyFecModCodTypes.L1d_PlpCod.getOrDefault(rfPhyStatistics.plp_cod, RfPhyFecModCodTypes.L1d_PlpCod.get(255)),
                            rfPhyStatistics.plp_cod));

                    //        LogMsgF("signal %s, strength -%d.%03d dB", lock ? "Lock" : "Unlock", (-rssi)/1000, (-rssi)%1000);
                    mRfRssiText.setText(String.format("RSSI: %d.%03d dB", (rfPhyStatistics.rssi) / 1000, (-rfPhyStatistics.rssi) % 1000));
                    mRfSNRText.setText(String.format("SNR: %.2f, cpu: %s, tuner lock: %d, demod lock: %d", (float) rfPhyStatistics.nSnr1000 / 1000.0, (rfPhyStatistics.cpu_status == 1 ? "R" : "H"), rfPhyStatistics.tuner_lock, rfPhyStatistics.demod_lock_status));
                    mRfBERText.setText(String.format("BER: pre_ldpc: %d, pre_bch: %d, post_bch: %d", rfPhyStatistics.ber_pre_ldpc_e7, rfPhyStatistics.ber_pre_bch_e9, rfPhyStatistics.fer_post_bch_e6));


                } else if (msg.what == BW_PHY_STATISTICS_UPDATED) {
                    BwPhyStatistics bwPhyStatistics = (BwPhyStatistics) msg.obj;
                    //TODO: add avg Mbit/sec forward from app start time
                    double currentRuntimeDurationS = (float) (System.currentTimeMillis() - MmtPacketIdContext.libatsc_app_start_time_ms) / 1000.0;
                    float last_1s_bw_bitsSec = MmtPacketIdContext.computeLast1sBwBitsSec(bwPhyStatistics.total_bytes);
                    float last_1s_bw_pps = MmtPacketIdContext.computeLast1sBwPPS(bwPhyStatistics.total_pkts);

                    mBwTotalStatusStr.setText(String.format("\nlibatsc3 runtime duration: %.2fs\nTotal Packets: %d\nTotal Bytes: %.2f MB\n\nLast 1s: %.2f Mbit/sec, %.0f PPS\n\nSess. Avg: %.2f Mbit/sec\nTotal LMT: %d",
                            currentRuntimeDurationS,
                            bwPhyStatistics.total_pkts,
                            (float) (bwPhyStatistics.total_bytes / (1024.0 * 1024.0)),
                            (float) last_1s_bw_bitsSec / (1024.0 * 1024.0),
                            last_1s_bw_pps,
                            (float) ((bwPhyStatistics.total_bytes * 8.0) / (1024.0 * 1024.0)) / currentRuntimeDurationS,
                            bwPhyStatistics.total_lmts));

                } else if (msg.what == DRAW_TEXT_MFU_METRICS) {
                    String videoMfuMetrics = String.format("V: current mpu_seq_num: %d\nmfu complete: %d\nmfu corrupt  : %d\nmfu missing : %d\nmfu total    : %d\ndecoder input underrun: %d",
                            MmtPacketIdContext.video_packet_statistics.last_essence_mpu_sequence_number,
                            MmtPacketIdContext.video_packet_statistics.complete_mfu_samples_count,
                            MmtPacketIdContext.video_packet_statistics.corrupt_mfu_samples_count,
                            MmtPacketIdContext.video_packet_statistics.missing_mfu_samples_count,
                            MmtPacketIdContext.video_packet_statistics.total_mfu_samples_count,
                            MmtPacketIdContext.video_packet_statistics.decoder_buffer_input_mfu_underrun_count);

                    String audioMfuMetrics = String.format("\nA: current mpu_seq_num: %d\nmfu complete: %d\nmfu corrupt  : %d\nmfu missing : %d\nmfu total    : %d\ndecoder input underrun: %d",
                            MmtPacketIdContext.video_packet_statistics.last_essence_mpu_sequence_number,
                            MmtPacketIdContext.audio_packet_statistics.complete_mfu_samples_count,
                            MmtPacketIdContext.audio_packet_statistics.corrupt_mfu_samples_count,
                            MmtPacketIdContext.audio_packet_statistics.missing_mfu_samples_count,
                            MmtPacketIdContext.audio_packet_statistics.total_mfu_samples_count,
                            MmtPacketIdContext.audio_packet_statistics.decoder_buffer_input_mfu_underrun_count);

                    mVideoMfuStatisticsText.setText(videoMfuMetrics + audioMfuMetrics);

                } else if (msg.what == STPP_IMSC1_AVAILABLE) {
                    MfuByteBufferFragment imsc1_payload = (MfuByteBufferFragment) msg.obj;
                    byte myRawPayload[] = new byte[imsc1_payload.bytebuffer_length];

                    imsc1_payload.myByteBuffer.get(myRawPayload, 0, imsc1_payload.bytebuffer_length);
                    String imsc1_raw_payload = new String(myRawPayload);
                    String imsc1_payload_details_string = String.format("packet_id: %d, pts: %d, mpu: %d, sample: %d, payload:\n%s",
                            imsc1_payload.packet_id, imsc1_payload.mpu_presentation_time_uS_from_SI != null ? imsc1_payload.mpu_presentation_time_uS_from_SI : 0, imsc1_payload.mpu_sequence_number, imsc1_payload.sample_number, imsc1_raw_payload);
                    textRawIMSC1.setText(imsc1_payload_details_string);

                    //clean up newlines and tabs

                    String imsc1_raw_payload_CleanedUpTagsStr = imsc1_raw_payload.replaceAll("[\r\n\t]+", "");
                    //replace any closing </p> or <br/> tags...
                    imsc1_raw_payload_CleanedUpTagsStr = imsc1_raw_payload_CleanedUpTagsStr.replace("</p>", "\n");
                    imsc1_raw_payload_CleanedUpTagsStr = imsc1_raw_payload_CleanedUpTagsStr.replace("<br/>", "\n");

                    String imscCleanupStrRegEx = "<[^>]*>";
                    imsc1_raw_payload_CleanedUpTagsStr = imsc1_raw_payload_CleanedUpTagsStr.replaceAll(imscCleanupStrRegEx, "");
                    imsc1_raw_payload_CleanedUpTagsStr = imsc1_raw_payload_CleanedUpTagsStr.trim();

                    imsc1BodyView.setVisibility(View.VISIBLE);
                    imsc1BodyView.setBackgroundColor(Color.parseColor("#AA000000"));
                    imsc1BodyView.setText(imsc1_raw_payload_CleanedUpTagsStr);

                    MmtPacketIdContext.stpp_last_mpu = imsc1_payload.mpu_sequence_number;
                    ServiceHandler.GetInstance().sendMessageDelayed(ServiceHandler.GetInstance().obtainMessage(STPP_IMSC1_CHECK_CLEAR, imsc1_payload.mpu_sequence_number), 4 * 1000);

                } else if (msg.what == STPP_IMSC1_CHECK_CLEAR) {
                    int toCheckMpuSequenceNumberToClear = (int) msg.obj;
                    if (toCheckMpuSequenceNumberToClear == MmtPacketIdContext.stpp_last_mpu) {
                        //imsc1BodyView.setText("");
                        imsc1BodyView.setVisibility(View.GONE);

                    }
                } else if(msg.what == TOAST) {
                    Toast.makeText(getApplicationContext(), (String)msg.obj, Toast.LENGTH_SHORT).show();

                } else {
                    super.handleMessage(msg);
                    //noop
                }
            }
        };
    }

    // show log message (used in main ui thread)
    public void showMsg(String str) {
        try {
            int splitCount = mTextView.getText().toString().split("\n").length;

            if(splitCount > 10) { //if (mTextView.length() > 1024) {
                mTextView.setText(str);
            } else {
                mTextView.append(str);
            }
        } catch (Exception ex) {
            //workaround for the following ex:

            /*
                java.lang.ArrayIndexOutOfBoundsException: length=39; index=39
        at android.text.PackedObjectVector.moveRowGapTo(PackedObjectVector.java:143)
        at android.text.PackedObjectVector.deleteAt(PackedObjectVector.java:87)
        at android.text.DynamicLayout.reflow(DynamicLayout.java:620)
        at android.text.DynamicLayout$ChangeWatcher.reflow(DynamicLayout.java:1074)
        at android.text.DynamicLayout$ChangeWatcher.onTextChanged(DynamicLayout.java:1085)
        at android.text.SpannableStringBuilder.sendTextChanged(SpannableStringBuilder.java:1263)
        at android.text.SpannableStringBuilder.replace(SpannableStringBuilder.java:575)
        at android.text.SpannableStringBuilder.append(SpannableStringBuilder.java:290)
        at android.text.SpannableStringBuilder.append(SpannableStringBuilder.java:36)
        at android.widget.TextView.append(TextView.java:5804)
        at android.widget.TextView.append(TextView.java:5785)
        at org.ngbp.libatsc3.MainActivity.showMsg(MainActivity.java:1703)
        at org.ngbp.libatsc3.MainActivity$11.handleMessage(MainActivity.java:1605)
        at android.os.Handler.dispatchMessage(Handler.java:106)
        at android.os.Looper.loop(Looper.java:214)
        at android.app.ActivityThread.main(ActivityThread.java:7037)
        at java.lang.reflect.Method.invoke(Native Method)
        at com.android.internal.os.RuntimeInit$MethodAndArgsCaller.run(RuntimeInit.java:494)
        at com.android.internal.os.ZygoteInit.main(ZygoteInit.java:965)
             */

        }
    }

    // show log message (used in non-ui thread)
    public void showMsgFromNative(String str) {
        Message msg = ServiceHandler.GetInstance().obtainMessage(1, str );
        ServiceHandler.GetInstance().sendMessage(msg);
    }

    public int mfuStatsVideoICount = 0;
    public int mfuStatsVideoPBCount = 0;
    
    Thread mfuVideoStatsLedThread;

    //HACK
    public void routeDash_force_player_reload_mpd(int service_id) {
        if(selectedServiceId == service_id && selectedServiceSLSProtocol == 1) {
            Log.d("routeDash_force_player_reload_mpd","Calling R.id.butPlay.performClick");
            //trigger another butPlay click to reload exoplayer
            ServiceHandler.GetInstance().postDelayed(new Runnable() {
                @Override
                public void run() {
                    ((Button)findViewById(R.id.butPlay)).performClick();

                }
            }, 1000);
        }
    }

    ArrayAdapter<Service> slsSpinnerArrayAdapter;
    ArrayList<Service> sltServices;
    Boolean serviceSpinnerLastSelectionFromArrayAdapterUpdate = false;
    public void onSlsTablePresent(String sls_payload_xml) {
        final String to_parse_sls_payload_xml = sls_payload_xml;
        LLSParserSLT llsParserSLT = new LLSParserSLT();
        sltServices = llsParserSLT.parseXML(to_parse_sls_payload_xml);

        for(Service s : sltServices) {
            Log.d("SlsTablePresent", String.format("shortServiceName: %s, serviceId: %d, slsProtocol: %d", s.shortServiceName, s.serviceId, (s.broadcastSvcSignalingCollection.get(0) != null ? s.broadcastSvcSignalingCollection.get(0).slsProtocol : -1)));
        }

        slsSpinnerArrayAdapter = new ArrayAdapter<Service>(this, android.R.layout.simple_spinner_item, sltServices);

        ServiceHandler.GetInstance().post(new Runnable() {
            @Override
            public void run() {
                serviceSpinnerLastSelectionFromArrayAdapterUpdate = true;
                mServiceSpinner.setAdapter(slsSpinnerArrayAdapter);
                mSLSView.setText(to_parse_sls_payload_xml);
            }
        });
    }

    public void onAlcObjectStatusMessage(String alc_object_status_message) {
        final String alc_object_status_message_to_update = alc_object_status_message;
        ServiceHandler.GetInstance().post(new Runnable() {
            @Override
            public void run() {

                int splitCount = mAlcObjectStatusMsg.getText().toString().split("\n").length;

                if(splitCount > 10) {
                    mAlcObjectStatusMsg.setText(alc_object_status_message_to_update);
                } else {
                    mAlcObjectStatusMsg.append(alc_object_status_message_to_update + "\n");
                }
            }
        });

    }

    public void onPackageExtractCompleted(PackageExtractEnvelopeMetadataAndPayload packageExtractEnvelopeMetadataAndPayload) {
        if(packageExtractEnvelopeMetadataAndPayload != null && packageExtractEnvelopeMetadataAndPayload.packageExtractPath != null) {
            Log.d("onPackageExtractCompleted", String.format("packageExtractEnvelopeMetadataAndPayload: packageName: %s, path: %s, count: %d", packageExtractEnvelopeMetadataAndPayload.packageName, packageExtractEnvelopeMetadataAndPayload.packageExtractPath, packageExtractEnvelopeMetadataAndPayload.multipartRelatedPayloadList.size()));

            for(PackageExtractEnvelopeMetadataAndPayload.MultipartRelatedPayload multipartRelatedPayload : packageExtractEnvelopeMetadataAndPayload.multipartRelatedPayloadList) {
                Log.d("onPackageExtractCompleted", String.format("packageExtractEnvelopeMetadataAndPayload.multipartRelatedPayload: contentLocation: %s, contentType: %s, size: %d",
                        multipartRelatedPayload.contentLocation,
                        multipartRelatedPayload.contentType,
                        multipartRelatedPayload.extractedSize));
            }

        }
    }

    public class MfuVideoStatsLedRunnable implements Runnable {
        Boolean shouldStop = false;

        @Override
        public void run() {

            while(!shouldStop) {
                try {
                    ThingsUI.WriteToAlphaDisplay("PktC");
                    Thread.sleep(5000);

                    ThingsUI.WriteToAlphaDisplay("Ifr");
                    Thread.sleep(5000);
                    ThingsUI.WriteToAlphaDisplay(String.format("%d", mfuStatsVideoICount));
                    Thread.sleep(5000);

                    ThingsUI.WriteToAlphaDisplay("B/P");
                    Thread.sleep(5000);
                    ThingsUI.WriteToAlphaDisplay(String.format("%d", mfuStatsVideoPBCount));
                    Thread.sleep(5000);

                    ThingsUI.WriteToAlphaDisplay("Aud");
                    Thread.sleep(5000);
                    ThingsUI.WriteToAlphaDisplay(String.format("%d", MmtPacketIdContext.audio_packet_statistics.total_mfu_samples_count));
                    Thread.sleep(5000);


                } catch (Exception | java.lang.NoClassDefFoundError ex) {
                    ex.printStackTrace();
                    shouldStop = true;
                }
            }
        }
    };

    public void updateLedWithAVCount() {
        if (mfuVideoStatsLedThread == null) {
            mfuVideoStatsLedThread = new Thread(new MfuVideoStatsLedRunnable());
            mfuVideoStatsLedThread.start();
        }
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "onPause called");
        // should we continue data buffering?
        super.onPause();
    }
    @Override
    protected void onResume() {
        Log.d(TAG, "opResume called");
        super.onResume();
    }
    @Override
    protected void onStop() {
        Log.d(TAG, "opStop called");
        super.onStop();
    }
    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy called");
        atsc3NdkPHYBridge.ApiClose();
        unregisterReceiver(mUsbReceiver);
        super.onDestroy();
    }
    @Override
    public void onBackPressed() {
        Log.d(TAG, "onBackPressed called");
        atsc3NdkPHYBridge.ApiClose();
        unregisterReceiver(mUsbReceiver);
        atsc3NdkPHYBridge.ApiUninit();
        Log.d(TAG, "uninit ended");

        moveTaskToBack(true);
        android.os.Process.killProcess(android.os.Process.myPid());
        System.exit(0);
    }

    //TODO: remove this..mess...
    // return -1 if failed
    long getKeyFromUsbDevName(String devName)
    {
        // devName has form of '/dev/bus/usb/001/002'
        if (devName.length() != 20)
            return -1;
        if (!devName.substring(0, 13).equals("/dev/bus/usb/"))
            return -1;
        String[] pairs = devName.substring(13).split("/");
        if (pairs.length != 2)
            return -1;
        int bus = Integer.parseInt(pairs[0]);
        int addr =Integer.parseInt(pairs[1]);
        long key = ((bus & 0xff) << 8) + (addr & 0xff);

        Log.d(TAG, "  key " + String.format("%x", key) + " <- " + devName);
        return key;
    }

    private void scanDevices() {
        prepareDevices();

        Log.d(TAG, "scanDevices:: calling mUsbManager.getDeviceList()");
        HashMap<String, UsbDevice> deviceList = mUsbManager.getDeviceList();
        if (deviceList == null) {
            Toast.makeText(getApplicationContext(), "No USB devices detected!", Toast.LENGTH_SHORT).show();
            Log.d(TAG, "No USB devices detected!");
            return;
        }

        // find device
        List<UsbDevice> devices = new ArrayList<UsbDevice>();
        Log.d(TAG, "device list: " + deviceList.size() + " devices");
        for (UsbDevice d : deviceList.values()) {
            Log.d(TAG, " --" + d.toString());
            Log.d(TAG, "   vid/pid: " + d.getVendorId() + "/" + d.getProductId() +
                    " id " + d.getDeviceId() + " name " + d.getDeviceName());
            if (isDeviceCypressFX3(d) || isDeviceBootedFX3(d)) {
                devices.add(d);
            }
        }

        if (devices.isEmpty()) {
            showMsg("No device\n");
            Toast.makeText(getApplicationContext(), "No FX3 detected!", Toast.LENGTH_SHORT).show();
            return;
        }

        Log.d(TAG, "total " + devices.size() + " FX3 device found");

        for (UsbDevice d : devices) {
            if (!mUsbManager.hasPermission(d)) {
                Log.d(TAG, "dev " + d.getDeviceName() + " no permission yet. request..");
                mUsbManager.requestPermission(d, mPermissionIntent);
                // once we requested, next action will be continued in broadcast receiver.
                return;
            }

            Log.d(TAG, "dev " + d.getDeviceName() + " permission ok. make connection..");
            connectDeviceAndPrepare(d);
        }
    }

    private void connectDeviceAndPrepare(UsbDevice device) {
        int vid = device.getVendorId();
        int pid = device.getProductId();
        String devName = device.getDeviceName();

        Log.d(TAG, "connectDeviceAndPrepares: " + device.getDeviceName());

        if (!mUsbManager.hasPermission(device)) {
            Log.e(TAG, String.format("connectDevice: vid: %d, pid: %d - has no permission!"));

            return;
        }
        //disconnectDevice(device);

        long  key = getKeyFromUsbDevName(devName);
        if (key < 0) {
            Log.e(TAG, String.format("connectDeviceAndPrepare: get key failed from devName %s", devName));
            return;
        }

        Log.d(TAG, "now, connecting device " + devName + "...");
        UsbDeviceConnection conn = mUsbManager.openDevice(device);
        if (conn == null) {
            Log.e(TAG, "dev " + devName + " connect error");
            return;
        }

        int fd = conn.getFileDescriptor();
        Log.d(TAG, "connected, fd:" + fd);

        //check for duplicate vid/pid/devName in our at3Device list
        for (int i = 0; i < mAt3Devices.size(); i++) {
            atsc3UsbDevice ad = mAt3Devices.get(i);

            if (ad.dev.getDeviceName().equals(device.getDeviceName())) {
                ad.dev = device; //update dev instance with new device to keep track of active FD
                ad.fd = fd;
                return; //bail for duplicate
            }
        }

        // otherwise, add to our at3 device list
        mAt3Devices.add(new atsc3UsbDevice(device, conn, fd, key));

        // to enable firmware loading below, prepare here.
        //not needed...
        prepareDevices();

//        if (isDeviceCypressFX3((device))) {
//            Log.d(TAG, "this is cyfx device. try firmware loading...");
//            mAt3DrvIntf.ApiFwLoad(key);
//            return;
//        }
//        if (isAtlasPrebootDevice(device)) {
//            Log.d(TAG, "this is pre-boot atlas device. try firmware loading...");
//            mAt3DrvIntf.ApiFwLoad(key);
//            return;
//        }
        showMsg("usb connected\n");
    }

    private void disconnectDevice(UsbDevice device) {
        int nDisconnected = 0;

        if(mAt3Devices.size() == 0) {
            return;
        }

        Log.d(TAG, String.format("disconnectDevice: total devices (fx3 pre+booted): %d", mAt3Devices.size()));

        for (int i = mAt3Devices.size()-1; i>=0; i--) {
            atsc3UsbDevice ad = mAt3Devices.get(i);
            Log.d(TAG, String.format("disconnectDevice: checking dev: %d, fd: %d, name: %s", i, ad.fd, ad.dev.getDeviceName()));

            if (!ad.dev.getDeviceName().equals(device.getDeviceName()))
                continue;

            Log.d(TAG, String.format("disconnectDevice: removing device at index: %d, fd: %d, name: %s", i, ad.fd, device.getDeviceName()));
            ad.disconnect();
            mAt3Devices.remove(i);

            nDisconnected++;
        }

        if (nDisconnected == 0) {
            Log.w(TAG, String.format("disconnectDevice: no device to disconnect, but size is: %d", mAt3Devices.size()));
        }

        if (nDisconnected > 1) {
            Log.e(TAG, String.format("disconnectDevice: %d devices removed - possible duplicates?!", nDisconnected));
        }
    }

    private void disconnectAllDevices() {
        for (atsc3UsbDevice ad : mAt3Devices)
            ad.disconnect();
        mAt3Devices.clear();
        // TODO: disconnect usb connection also
    }

    private void prepareDevices() {
        String d1 = ":", d2 = ",";
        String info = "";
        if(mAt3Devices.size() > 0) {
            for (atsc3UsbDevice ad : mAt3Devices) {
                if (info.isEmpty())
                    info = ad.dev.getDeviceName() + d1 + ad.fd;
                else
                    info = info + d2 + ad.dev.getDeviceName() + d1 + ad.fd;
            }
            Log.d(TAG, "prepare: " + info);
            int r = atsc3NdkPHYBridge.ApiPrepare(info, d1.charAt(0), d2.charAt(0));

            if (r != 0) showMsg("!! prepare failed\n");
        }
        updateSpinnerFromDevList();
    }
//
//    private boolean isAtlasPrebootDevice(UsbDevice device) {
//        // full-function (fwloaded) atlas device has 2 or 3 EPs.
//        // preboot (not fwloaded) atlas device has (currently) zero EP.
//        int vid = device.getVendorId(); int pid = device.getProductId();
//        if (vid != 0xf055 || pid != 0x1e1b) return false; // not atlas device
//        UsbConfiguration conf = device.getConfiguration(0);
//        UsbInterface intf = conf.getInterface(0);
//        int numEp = intf.getEndpointCount();
//        Log.d(TAG, "atlas device: num ep = " + numEp);
//        if (numEp < 2) return true;
//        return false;
//    }

    //atlas         if (vid == 0xf055 && pid == 0x1e1b) return true;

    //mVendorId=1204,mProductId=240 CyFX3
    private boolean isDeviceBootedFX3(UsbDevice device) {
        int vid = device.getVendorId();
        int pid = device.getProductId();
        if (vid == 0x04b4 && pid == 0x00F0) {
            return true;
        } else {
            return false;
        }
    }

    //mVendorId=1204,mProductId=243 CyFX3
    //pid == 0x00f3??
    private boolean isDeviceCypressFX3(UsbDevice device) {
        int vid = device.getVendorId();     //1204 -> 0x04b4
        int pid = device.getProductId();    //0x00F0

        if (vid == 0x04b4 && pid == 0x00F3) {
            return true;
        } else {
            return false;
        }
    }

    private void dumpDevice(UsbDevice device, String action) {
        // getConfiguration requires api level >= 21
        UsbConfiguration conf = device.getConfiguration(0);
        UsbInterface intf = conf.getInterface(0);
        int numEp = intf.getEndpointCount();
        Log.d(TAG, "******* device (vid " + device.getVendorId() + ", pid " + device.getProductId() +
                ", ep " + numEp + ", " + device.getDeviceName() + ") " + action) ;

    }

    // our own broadcast receiver
    private BroadcastReceiver mUsbReceiver;
    public class MyReceiver extends BroadcastReceiver {
        private final Handler mHandler; // Handler used to execute code on the UI thread

        public MyReceiver(Handler handler) {
            this.mHandler = handler;
        }
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "get intent action " + action);

            if (ACTION_USB_PERMISSION.equals(action)) {
                synchronized (this) {
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (device == null) { Log.e(TAG, "null device!"); return; }
                    if (!isDeviceBootedFX3(device) && !isDeviceCypressFX3(device)) return;
                    dumpDevice(device, " has permission");

                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        //connectDeviceAndPrepare(device);
                        Message msg = mHandler.obtainMessage(2, device);
                        mHandler.sendMessage(msg);
                    }
                    else {
                        Log.d(TAG, "permission denied for device " + device);
                    }
                    // mPermissionRequestPending = false;
                    return;
                }
            }
            if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action)) {
                synchronized (this) {
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (device == null) { Log.e(TAG, "null device!"); return; }
                    if (!isDeviceBootedFX3(device) && !isDeviceCypressFX3(device)) return;
                    dumpDevice(device, " attached");

                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        //call method to set up device communication
                        Log.d(TAG, "permission granted");
                        //connectDeviceAndPrepare(device);
                            // warning: this can occur ANR because it touches UI
                        Message msg = mHandler.obtainMessage(2, device);
                        mHandler.sendMessage(msg);
                    }
                    else {
                        Log.d(TAG, String.format("permission denied for device, dispatching mPermissionIntent: %s, dev: %s", mPermissionIntent, device));
                        final UsbDevice myDeviceNeedingPermission = device;
                        ServiceHandler.GetInstance().postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                mUsbManager.requestPermission(myDeviceNeedingPermission, mPermissionIntent);

                            }
                        }, 1000);

                        return;
                    }
                }
            }

            if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                synchronized (this) {
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (device == null) { Log.e(TAG, "null device!"); return; }
                    if (!isDeviceBootedFX3(device) && !isDeviceCypressFX3(device)) return;
                    dumpDevice(device, " detached");
                    disconnectDevice(device);
                    //prepareDevices();

                    Message msg = mHandler.obtainMessage(3, device);
                    mHandler.sendMessage(msg);

                    return;
                }
            }

            //make sure to call super impl!
            onReceive(context, intent);
        } // onReceive
    }; // MyReceiver


    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == PcapFileSelectorActivity.CODE_READ_PCAP_FILE) {
            if (resultCode == RESULT_OK) {
                String pcapFileToProcess = data.getData().toString();
                if(pcapFileToProcess != null) {
                    //hack-ish for now..
                    pcapAssetForFilesystemReplay.clear();
                    inputSelectionFromPcap = true;

                    inputSelectedPcapReplayFromFilesystem = pcapFileToProcess;
                    pcapAssetForFilesystemReplay.add(inputSelectedPcapReplayFromFilesystem);

                    inputSelectedPcapReplayFromAssetManager = null;

                    ServiceHandler.GetInstance().post(new Runnable() {
                        @Override
                        public void run() {
                            updateSpinnerFromDevList();
                            enableDeviceOpenButtons(true);
                            enableDeviceControlButtons(false);
                        }
                    });
                }
            }
        }
    }
    @Override
    public void onClick(View view) {
        int r = 0;
        switch(view.getId())
        {
            case R.id.butOpen:
                Log.d("onClick", "Button: Open\n");
                updateLedWithAVCount();

                if(inputSelectionFromPcap) {
                    inputSelectionFromPcap = true;
                    enableDeviceControlButtons(true);
                    if(inputSelectedPcapReplayFromFilesystem != null) {
                        demuxedPcapVirtualPHY.atsc3_pcap_open_for_replay(inputSelectedPcapReplayFromFilesystem);
                    } else if(inputSelectedPcapReplayFromAssetManager != null) {
                        demuxedPcapVirtualPHY.atsc3_pcap_open_for_replay_from_assetManager(inputSelectedPcapReplayFromAssetManager, assetManager);
                    }


                    demuxedPcapVirtualPHY.atsc3_pcap_thread_run();
                    ThingsUI.WriteToAlphaDisplayNoEx("PCAP");
                    enableDeviceControlButtons(true);
                    break;
                }

                if (mCurAt3Device == null) {
                    showMsg("no FX3 device connected yet\n");
                    break;
                }
                //notify pcap thread (if running) to stop
                demuxedPcapVirtualPHY.atsc3_pcap_thread_stop();


                showMsg(String.format("opening mCurFx3Device: fd: %d, key: %d", mCurAt3Device.fd, mCurAt3Device.key));
                //ThingsUI.WriteToAlphaDisplayNoEx("OPEN");

                stopAllPlayers();

                new Thread(new Runnable() {
                    @Override
                    public void run() {

                        int re = atsc3NdkPHYBridge.ApiOpen(mCurAt3Device.fd, mCurAt3Device.key);
                        if (re < 0) {
                            showMsgFromNative(String.format("open: failed, r: %d", re));
                            return;
                        } else if(re == 240) { //SL_FX3S_I2C_AWAITING_BROADCAST_USB_ATTACHED
                            showMsgFromNative(String.format("open: pending SL_FX3S_I2C_AWAITING_BROADCAST_USB_ATTACHED event"));
                            return;
                        }

                        ServiceHandler.GetInstance().post(new Runnable() {
                            @Override
                            public void run() {
                                ((Button)findViewById(R.id.butTune)).setEnabled(true);
                                ((Button)findViewById(R.id.butTune)).requestFocus();
                                btnToggleRfScan.setEnabled(true);

                                ((EditText)findViewById(R.id.editFreqMhz)).setEnabled(true);
                                ((EditText)findViewById(R.id.editPlp)).setEnabled(true);
                            }
                        });
                    }
                }).start();

                break;

            case R.id.butTune:
                //showMsg("button Tune\n");
                if (mCurAt3Device == null) {
                    showMsg("no atlas device connected yet\n");
                    break;
                }
                EditText editFreq = (EditText)findViewById(R.id.editFreqMhz);

                final int freqMHz = Integer.parseInt(editFreq.getText().toString());
                EditText editPlp = (EditText)findViewById(R.id.editPlp);
                final int plp = Integer.parseInt(editPlp.getText().toString());
                Log.d(TAG, "tune with freq " + freqMHz + ", plp " + plp);
                //ThingsUI.WriteToAlphaDisplayNoEx(String.format("T%d", freqMHz));
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        int re = atsc3NdkPHYBridge.ApiTune(freqMHz * 1000, plp);
                        if (re != 0) {
                            showMsgFromNative(String.format("Tune failed with res: %d", re));
                            return;
                        }

                        ServiceHandler.GetInstance().post(new Runnable() {
                            @Override
                            public void run() {
                                mServiceSpinner.setEnabled(true);
                                mServiceSpinner.requestFocus();

                                editServiceIDText.setEnabled(true);

                            }
                        });
                    }
                }).start();

                break;

            case R.id.butPlay:

                //ROTUE-DASH with exoplayer
                if(selectedServiceSLSProtocol == 1) {
                    simpleExoPlayerView.setVisibility(View.VISIBLE);

                    //clean up MMT simpleExoPlayer..
                    if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
                        ATSC3PlayerFlags.ATSC3PlayerStopPlayback = true;
                        myDecoderHandlerThread.decoderHandler.sendMessage(myDecoderHandlerThread.decoderHandler.obtainMessage(DecoderHandlerThread.DESTROY));
                    }

                    //TODO: dispatch this into the decoderHandler thread...
                    if(simpleExoPlayer != null) {
                        routeDashPlayerStopAndRelease();
                    }
                    routeDashPlayerCreate();

                    if (simpleExoPlayer != null) {
                        String[] routeMPDFileName = atsc3NdkApplicationBridge.atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(selectedServiceId, DASH_CONTENT_TYPE);
                        if(routeMPDFileName.length == 0) {
                            Toast.makeText(getApplicationContext(), String.format("Unable to resolve Dash MPD path from MBMS envelope, service_id: %d", selectedServiceId), Toast.LENGTH_SHORT).show();
                            return;
                        }
                        String tempDir = String.format("file://%s/%s", jni_getCacheDir(), routeMPDFileName[0]);

                        simpleExoPlayerView.setPlayer(simpleExoPlayer);

                        DashMediaSource dashMediaSource = dashMediaSourceFactory.createMediaSource(Uri.parse(tempDir));
                        simpleExoPlayer.prepare(dashMediaSource);
                        simpleExoPlayer.setPlayWhenReady(true);
                        return;
                    }
                } else if(selectedServiceSLSProtocol == 2) {

                    if (simpleExoPlayer != null) {
                        routeDashPlayerStopAndRelease();
                    }
                    simpleExoPlayerView.setVisibility(View.GONE);

                    if (!ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
                        MfuByteBufferHandler.clearQueues();
                        myDecoderHandlerThread.decoderHandler.sendMessage(myDecoderHandlerThread.decoderHandler.obtainMessage(DecoderHandlerThread.CREATE_CODEC));
                        ThingsUI.WriteToAlphaDisplayNoEx(String.format("PLAY"));
                    } else {
                        ATSC3PlayerFlags.ATSC3PlayerStartPlayback = false;
                        ATSC3PlayerFlags.ATSC3PlayerStopPlayback = true;
                        myDecoderHandlerThread.decoderHandler.sendMessage(myDecoderHandlerThread.decoderHandler.obtainMessage(DecoderHandlerThread.DESTROY));
                        ThingsUI.WriteToAlphaDisplayNoEx(String.format("---"));

                    }
                }
                break;

            case R.id.butStop:
                showMsg("button Stop\n");
                //TODO: jjustman-2019-10-18

                routeDashPlayerStopAndRelease();
                myDecoderHandlerThread.decoderHandler.sendMessage(myDecoderHandlerThread.decoderHandler.obtainMessage(DecoderHandlerThread.DESTROY));

                if(inputSelectionFromPcap) {
                    //shutdown pcap thread
                    demuxedPcapVirtualPHY.atsc3_pcap_thread_stop();
                    showMsg("pcap thread stopped");
                    return;
                }

                if (mCurAt3Device == null) {
                    showMsg("no atlas device connected yet\n");
                    break;
                }
                r = atsc3NdkPHYBridge.ApiStop();
                ThingsUI.WriteToAlphaDisplayNoEx(String.format("ASTP"));

                //clear pending SLS window
                ServiceHandler.GetInstance().post(new Runnable() {
                    @Override
                    public void run() {
                        mSLSView.setText("");
                        mTextView.setText("Stopped");
                    }
                });

                break;

            case R.id.butReset:
                showMsg("button Reset\n");
                if (mCurAt3Device == null) {
                    showMsg("no atlas device connected yet\n");
                    break;
                }
                r = atsc3NdkPHYBridge.ApiReset();
                ThingsUI.WriteToAlphaDisplayNoEx(String.format("ARST"));
                break;

            case R.id.butClose:
                showMsg("button Close\n");
                if (mCurAt3Device == null) {
                    showMsg("no atlas device connected yet\n");
                    break;
                }
                r = atsc3NdkPHYBridge.ApiClose();
                //mTextView.setText("Closed\n"); // clear log msg
                if (r != 0) showMsg("closed\n");

                enableDeviceControlButtons(false);
                ThingsUI.WriteToAlphaDisplayNoEx(String.format("ACLS"));
                ServiceHandler.GetInstance().post(new Runnable() {
                    @Override
                    public void run() {
                        clearDebugTextViewFields();
                    }

                });

                break;

            default:
                onClick(view);
                return;
        }
    }

    private void clearDebugTextViewFields() {

        mSLSView.setText("");
        mTextView.setText("");
        textRawIMSC1.setText("");
        mAlcObjectStatusMsg.setText("");

    }

    private void routeDashPlayerCreate() {

        simpleExoPlayer = ExoPlayerFactory.newSimpleInstance(getApplicationContext());

        DataSource.Factory manifestDataSourceFactory = new DefaultDataSourceFactory(getApplicationContext(), Util.getUserAgent(getApplicationContext(), "libatsc3"));
        DataSource.Factory mediaDataSourceFactory = new DefaultDataSourceFactory(getApplicationContext(), Util.getUserAgent(getApplicationContext(), "libatsc3"));
        dashMediaSourceFactory = new DashMediaSource.Factory(
                new DefaultDashChunkSource.Factory(mediaDataSourceFactory),
                manifestDataSourceFactory);
        LoadErrorHandlingPolicy loadErrorHandlingPolicy = new ExoPlayerCustomLoadErrorHandlingPolicy(); //new DefaultLoadErrorHandlingPolicy();
        dashMediaSourceFactory.setLoadErrorHandlingPolicy(loadErrorHandlingPolicy);

    }

    private void routeDashPlayerStopAndRelease() {
        try {
            if(simpleExoPlayer != null) {
                simpleExoPlayer.stop();
                simpleExoPlayer.release();
            }
        } catch (Exception ex) {
            Log.w("routeDashPlayerStopAndRelease", "exception on stop/release: ex: "+ex);
        }

        simpleExoPlayer = null;
    }

    public void stopAllPlayers() {
        if (simpleExoPlayer != null) {
            routeDashPlayerStopAndRelease();
        }

        ATSC3PlayerFlags.ATSC3PlayerStartPlayback = false;
        ATSC3PlayerFlags.ATSC3PlayerStopPlayback = true;
        myDecoderHandlerThread.decoderHandler.sendMessage(myDecoderHandlerThread.decoderHandler.obtainMessage(DecoderHandlerThread.DESTROY));
    }



    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
        String s = (String) parent.getItemAtPosition(pos);
        Log.d(TAG, "user selected " + s);
        if (s.equals(kSpinnerNotSelect)) {
            // unchoose cur device
            if (mCurAt3Device != null) {
                atsc3NdkPHYBridge.ApiClose();
            }
            mCurAt3Device = null;
            inputSelectedPcapReplayFromFilesystem = null;
            inputSelectedPcapReplayFromAssetManager = null;
            inputSelectionFromPcap = false;
            enableDeviceControlButtons(false);
            enableDeviceOpenButtons(false);
            return;
        }

        //launch our file picker for asset selection
        if(s.startsWith(SELECT_PCAP_MESSAGE)) {
            Intent intent = new Intent(this, PcapFileSelectorActivity.class);
            startActivityForResult(intent, PcapFileSelectorActivity.CODE_READ_PCAP_FILE);
            //handle this in onActivityResult with the absolute path
            return;
        } else if(s.equals(inputSelectedPcapReplayFromFilesystem)) {
            //ignore this onSelect event, as we handle it in the onActivityResult
            return;
        } else if(s.startsWith(PCAP_URI_PREFIX)) {
            //support pre-baked in pcap assets as needed
            inputSelectedPcapReplayFromFilesystem = null;
            inputSelectedPcapReplayFromAssetManager = s;
            inputSelectionFromPcap = true;
            enableDeviceOpenButtons(true);
            return;
        } else {
            inputSelectionFromPcap = false;
            inputSelectedPcapReplayFromFilesystem = null;
            inputSelectedPcapReplayFromAssetManager = null;
        }

        try {
            // parse string and get fd
            int end = s.indexOf(":");
            if (end <= 0 && end > 10) { // fd digits are probably under 10?
                Log.d(TAG, "cannot get fd, " + s);
                return;
            }
            int fd = Integer.parseInt(s.substring(0, end));
            Log.d(TAG, "selected device's fd: " + fd);
            enableDeviceOpenButtons(true);
            enableDeviceControlButtons(false);

            // choose the device
            for (atsc3UsbDevice ad : mAt3Devices) {
                if (ad.fd == fd) {
                    if (mCurAt3Device != ad) {
                        //Log.d(TAG, "new device selected");
                        showMsg("new device selected\n");
                        atsc3NdkPHYBridge.ApiClose();

                        mCurAt3Device = ad;
                    } else {
                        Log.d(TAG, "keep previous selection");
                    }
                    return;
                }
            }


        } catch (Exception ex) {
            Log.e("onItemSelected", "exception ex: "+ex);
        }
        showMsg("device not selected\n");
    }
    @Override
    public void onNothingSelected(AdapterView<?> parent) {
        Log.d(TAG, "user selected nothing!");
    }

}
