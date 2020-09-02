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
import android.view.WindowManager;
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
import org.ngbp.libatsc3.middleware.android.application.interfaces.IAtsc3NdkApplicationBridgeCallbacks;
import org.ngbp.libatsc3.middleware.android.phy.Atsc3NdkPHYClientBase;
import org.ngbp.libatsc3.middleware.android.phy.Atsc3UsbDevice;
import org.ngbp.libatsc3.middleware.android.phy.SaankhyaPHYAndroid;
import org.ngbp.libatsc3.middleware.android.phy.interfaces.IAtsc3NdkPHYBridgeCallbacks;
import org.ngbp.libatsc3.middleware.android.phy.virtual.PcapDemuxedVirtualPHYAndroid;
import org.ngbp.libatsc3.middleware.android.phy.virtual.PcapSTLTPVirtualPHYAndroid;
import org.ngbp.libatsc3.middleware.android.phy.virtual.srt.SRTRxSTLTPVirtualPHYAndroid;
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

import org.ngbp.libatsc3.middleware.android.phy.models.BwPhyStatistics;
import org.ngbp.libatsc3.phy.RfPhyFecModCodTypes;
import org.ngbp.libatsc3.middleware.android.phy.models.RfPhyStatistics;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import static android.text.InputType.TYPE_CLASS_PHONE;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, AdapterView.OnItemSelectedListener, IAtsc3NdkApplicationBridgeCallbacks, IAtsc3NdkPHYBridgeCallbacks {

    final static String TAG ="MainActivity";

    private String prebuiltAssetsForDeviceSelectionVirtualPHY[] = {
            "srt://las.srt.atsc3.com:31350?passphrase=A166AC45-DB7C-4B68-B957-09B8452C76A4",
            "srt://bna.srt.atsc3.com:31347?passphrase=88731837-0EB5-4951-83AA-F515B3BEBC20",
            "srt://slc.srt.atsc3.com:31341?passphrase=B9E4F7B8-3CDD-4BA2-ACA6-13088AB855C0",
            "srt://lab.srt.atsc3.com:31340?passphrase=03760631-667B-4ADB-9E04-E4491B0A7CF1"
    };

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

    private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";

    public boolean hasUsbIfSupport = true;

    AssetManager assetManager;

    private DecoderHandlerThread myDecoderHandlerThread;


//jjustman-2020-08-18

    public Atsc3NdkApplicationBridge atsc3NdkApplicationBridge;

    public Atsc3NdkPHYBridge         atsc3NdkPHYBridge;

    public Atsc3NdkPHYClientBase     atsc3NdkPHYClientInstance = null; //whomever is currently instantiated (e.g. SRTRxSTLTPVirtualPhyAndroid, etc..)

    private static final String SRT_STLTP_URI_PREFIX = "srt://";

    private String inputSelectedSRTSource;
    private Boolean inputSelectionFromSRT = false;

    public void stopAndDeInitAtsc3NdkPHYClientInstance() {
        if(atsc3NdkPHYClientInstance != null) {
            atsc3NdkPHYClientInstance.stop();
            atsc3NdkPHYClientInstance.deinit();
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            atsc3NdkPHYClientInstance = null;
        }
    }

//end jjustman-2020-08-18

    private static final String PCAP_URI_PREFIX = "pcaps/";

    public static final String SELECT_PCAP_DEMUXED_MESSAGE = "Select PCAP Demuxed from Device...";
    private Boolean pendingInputSelectionFromPcapDemuxed = false;
    private Boolean inputSelectionFromPcapDemuxed = false;
    private String inputSelectedPcapDemuxedFromFilesystem = null;
    private List<String> pcapDemuxedAssetForFilesystemReplay = new ArrayList<String>();

    public static final String SELECT_PCAP_STLTP_MESSAGE = "Select PCAP STLTP from Device...";
    private Boolean pendingInputSelectionFromPcapSTLTP = false;
    private Boolean inputSelectionFromPcapSTLTP = false;
    private String inputSelectedPcapSTLTPFromFilesystem = null;
    private List<String> pcapSTLTPAssetForFilesystemReplay = new ArrayList<String>();


    private String inputSelectedPcapReplayFromAssetManager = null;


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


    public UsbManager mUsbManager;
    private PendingIntent mPermissionIntent;

    // usb device list
    private List<Atsc3UsbDevice> mAt3Devices = new ArrayList<Atsc3UsbDevice>();
    private Atsc3UsbDevice mCurAt3Device = null;

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

    IntentFilter usbIntentFilter = new IntentFilter();

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

        if(atsc3NdkPHYClientInstance != null) {
            items.add(atsc3NdkPHYClientInstance.toString());
            idxSelected = 1;
            itemCount++;

        }

        if(prebuiltAssetsForDeviceSelectionVirtualPHY.length > 0) {
            items.add("---");
            itemCount++;

            for(int i = 0; i < prebuiltAssetsForDeviceSelectionVirtualPHY.length; i++) {
                if(itemSelected != null && itemSelected.equalsIgnoreCase(prebuiltAssetsForDeviceSelectionVirtualPHY[i])) {
                    idxSelected = itemCount;
                }
                items.add(prebuiltAssetsForDeviceSelectionVirtualPHY[i]);
                itemCount++;
            }
            items.add("---");
            itemCount++;

        }
        items.add(SELECT_PCAP_DEMUXED_MESSAGE);
        itemCount++;
        for(int i = 0; i < pcapDemuxedAssetForFilesystemReplay.size(); i++) {
            if(inputSelectedPcapDemuxedFromFilesystem != null && inputSelectedPcapDemuxedFromFilesystem.equalsIgnoreCase(pcapDemuxedAssetForFilesystemReplay.get(i))) {
                idxSelected = itemCount;

            } else if(itemSelected != null && itemSelected.equalsIgnoreCase(pcapDemuxedAssetForFilesystemReplay.get(i))) {
                idxSelected = itemCount;
            }
            items.add(pcapDemuxedAssetForFilesystemReplay.get(i));
            itemCount++;
        }

        items.add("---");
        itemCount++;
        items.add(SELECT_PCAP_STLTP_MESSAGE);
        itemCount++;

        for(int i = 0; i < pcapSTLTPAssetForFilesystemReplay.size(); i++) {
            if(inputSelectedPcapSTLTPFromFilesystem != null && inputSelectedPcapSTLTPFromFilesystem.equalsIgnoreCase(pcapSTLTPAssetForFilesystemReplay.get(i))) {
                idxSelected = itemCount;

            } else if(itemSelected != null && itemSelected.equalsIgnoreCase(pcapSTLTPAssetForFilesystemReplay.get(i))) {
                idxSelected = itemCount;
            }
            items.add(pcapSTLTPAssetForFilesystemReplay.get(i));
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
        Log.d(TAG, "OnCreate: "+savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        if(savedInstanceState != null) {
            Log.d(TAG, "OnCreate:savedInstanceState is not null, returning!");

            return;
        }


        MmtPacketIdContext.Initialize();


        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        getSupportActionBar().hide();


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
                      //clearDebugTextViewFields();
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

        // get usb manager
        mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

        // get pending intent, which will be used for requesting permission
        mPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);

        Log.d(TAG, "onCreate: registering intent to receiver, before addAction: "+ usbIntentFilter+", actions count: "+usbIntentFilter.countActions());

        // register our own broadcast receiver instance, with filters we are interested in
        usbIntentFilter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        usbIntentFilter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        usbIntentFilter.addAction(ACTION_USB_PERMISSION);

        Log.d(TAG, "onCreate: registering intent to receiver"+ usbIntentFilter);
        registerReceiver(mUsbReceiver, usbIntentFilter);

        // jjustman-2020-08-18 - wire up our applicationBridge and PHYBridge
        atsc3NdkApplicationBridge = new Atsc3NdkApplicationBridge(this);
        atsc3NdkPHYBridge = new Atsc3NdkPHYBridge(this);

        // now, scan usb devices and try to connect
        ServiceHandler.GetInstance().postDelayed(new Runnable() {
            @Override
            public void run() {
                usbPHYLayerDeviceScan();
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

//        enableDeviceOpenButtons(false);
//        enableDeviceControlButtons(false);

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

        updateSpinnerFromDevList();


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

    private UsbDevice connectedUSBDevice = null;
    private void createServiceHandler() {

        serviceHandler = new ServiceHandler() {
            public void handleMessage(final Message msg) {
                if (msg.what == 1) { // update log
                    String str = (String) msg.obj;
                    showMsg("# " + str);
                } else if (msg.what == 2) {
                    //USB: Either permission granted OR device connected, try to instantiate our atsc3NdkPHYClient instance based upon vid/pid
                    final UsbDevice device = (UsbDevice) msg.obj;

                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            Log.d(TAG, "handler: connect device");
                            dumpDevice(device, "ServiceHandler:: connected");
                            usbPHYLayerDeviceInstantiateAndUpdateAtsc3NdkPHYClientInstance(device);

                            Log.d(TAG, "---- end of handling connect device");
                        }}).start();

                } else if (msg.what == 3) { // disconnect device and prepare
                    UsbDevice device = (UsbDevice) msg.obj;
                    Log.d(TAG, "handler: disconnected device");
                    dumpDevice(device, "disconnected");
                    //jjustman-2020-08-19 - TODO: find from our Atsc3UsbDevices and shutdown our Atsc3NdkClientBase accordingly

                    if(Atsc3UsbDevice.AllAtsc3UsbDevices.containsKey(device)) {
                        Atsc3UsbDevice atsc3UsbDevice = Atsc3UsbDevice.AllAtsc3UsbDevices.get(device);
                        Log.d(TAG, "found matching usbDevice and atsc3UsbDevice that was disconnected, instantiated atsc3NdkClientBase is: "+atsc3UsbDevice.atsc3NdkPHYClientBase);
                        if(atsc3UsbDevice.atsc3NdkPHYClientBase != null) {
                            atsc3UsbDevice.atsc3NdkPHYClientBase.deinit();
                            atsc3UsbDevice.atsc3NdkPHYClientBase = null;
                        }
                        Atsc3UsbDevice.AllAtsc3UsbDevices.remove(device);

                    }
                    Log.d(TAG, "---- end of handling disconnecting device");
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

    ArrayList<Service> sltServices;
    Boolean serviceSpinnerLastSelectionFromArrayAdapterUpdate = false;
    public void onSlsTablePresent(String sls_payload_xml) {
        final String to_parse_sls_payload_xml = sls_payload_xml;
        LLSParserSLT llsParserSLT = new LLSParserSLT();
        sltServices = llsParserSLT.parseXML(to_parse_sls_payload_xml);

        for(Service s : sltServices) {
            Log.d("SlsTablePresent", String.format("shortServiceName: %s, serviceId: %d, slsProtocol: %d", s.shortServiceName, s.serviceId, (s.broadcastSvcSignalingCollection.get(0) != null ? s.broadcastSvcSignalingCollection.get(0).slsProtocol : -1)));
        }
        final ArrayAdapter<Service> slsSpinnerArrayAdapter = new ArrayAdapter<Service>(this, android.R.layout.simple_spinner_item, sltServices);

        ServiceHandler.GetInstance().post(new Runnable() {
            @Override
            public void run() {
                serviceSpinnerLastSelectionFromArrayAdapterUpdate = true;
                mServiceSpinner.setAdapter(slsSpinnerArrayAdapter);
                mSLSView.setText(to_parse_sls_payload_xml);
            }
        });
    }


    @Override
    public void onAeatTablePresent(String aeatPayloadXML) {
        Log.d("onAeatTablePresent", String.format("aeatPayloadXML:\n%s", aeatPayloadXML));
    }

    @Override
    public void onSlsHeldEmissionPresent(int serviceId, String heldPayloadXML) {
        Log.d("onSlsHeldEmissionPresent", String.format("serviceId: %d, heldPayloadXML:\n%s", serviceId, heldPayloadXML));
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
        stopAndDeInitAtsc3NdkPHYClientInstance();

        unregisterReceiver(mUsbReceiver);
        super.onDestroy();
    }
    @Override
    public void onBackPressed() {
        Log.d(TAG, "onBackPressed called");
        stopAndDeInitAtsc3NdkPHYClientInstance();

        unregisterReceiver(mUsbReceiver);
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

    private void usbPHYLayerDeviceScan() {
        Log.d(TAG, "scanDevices:: calling mUsbManager.getDeviceList()");
        HashMap<String, UsbDevice> deviceList = mUsbManager.getDeviceList();
        if (deviceList == null || deviceList.size() == 0) {
            Toast.makeText(getApplicationContext(), "No USB devices detected!", Toast.LENGTH_SHORT).show();
            Log.d(TAG, "No USB devices detected!");
            return;
        }

        for (UsbDevice usbDevice : deviceList.values()) {
            dumpDevice(usbDevice, "usbPHYLayerDeviceScan");

            usbPHYLayerDeviceInstantiateAndUpdateAtsc3NdkPHYClientInstance(usbDevice);
        }
    }

    synchronized private void usbPHYLayerDeviceInstantiateAndUpdateAtsc3NdkPHYClientInstance(UsbDevice usbDevice) {
        if(connectedUSBDevice == null || (connectedUSBDevice != null && connectedUSBDevice != usbDevice)) {
            Log.d("atsc3NdkPHYClientInstance", "usbPHYLayerDeviceInstantiateAndUpdateAtsc3NdkPHYClientInstance with usbDevice: " + usbDevice);

            Atsc3NdkPHYClientBase atsc3NdkPHYClientBaseInstanceResult = usbPHYLayerDeviceTryToInstantiateFromRegisteredPHYNDKs(usbDevice);
            Log.d("atsc3NdkPHYClientInstance", "usbPHYLayerDeviceTryToInstantiateFromRegisteredPHYNDKs returned: " + atsc3NdkPHYClientBaseInstanceResult);
            if (atsc3NdkPHYClientBaseInstanceResult != null) {
                //todo - redispatch out of our service handler queue instead of inline here?
                if (atsc3NdkPHYClientInstance != null) {
                    atsc3NdkPHYClientInstance.deinit();
                    atsc3NdkPHYClientInstance = null;

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                }
                atsc3NdkPHYClientInstance = atsc3NdkPHYClientBaseInstanceResult;
                connectedUSBDevice = usbDevice;
                Log.d("atsc3NdkPHYClientInstance", "atsc3NdkPHYClientInstance is now: " + atsc3NdkPHYClientInstance);

                ServiceHandler.GetInstance().post(new Runnable() {
                    @Override
                    public void run() {

                        enableDeviceControlButtons(true);
                    }});
            }
        }
    }

    synchronized Atsc3NdkPHYClientBase  usbPHYLayerDeviceTryToInstantiateFromRegisteredPHYNDKs(UsbDevice usbDevice) {
        Log.d("usbPHYLayerDeviceTryToInstantiateFromRegisteredPHYNDKs", "enter with usbDevice: "+usbDevice);

        Atsc3UsbDevice atsc3UsbDevice = null;
        if((atsc3UsbDevice = Atsc3UsbDevice.FindFromUsbDevice(usbDevice)) != null) {
            Log.d("usbPHYLayerDeviceTryToInstantiateFromRegisteredPHYNDKs", "Atsc3UsbDevice already instantiated: "+atsc3UsbDevice);
            return null;  //eh, hack-ish..
        }

        Atsc3NdkPHYClientBase atsc3NdkPHYClientBaseInstantiated = null;
        ArrayList<Atsc3NdkPHYClientBase.USBVendorIDProductIDSupportedPHY> candidatePHYList = Atsc3NdkPHYClientBase.GetCandidatePHYImplementations(usbDevice);

        if(candidatePHYList != null) {
            if (!mUsbManager.hasPermission(usbDevice)) {
                Log.e(TAG, String.format("requesting permission for device: name: %s, vid: %d, pid: %d", usbDevice.getProductName(), usbDevice.getVendorId(), usbDevice.getProductId()));
                mUsbManager.requestPermission(usbDevice, mPermissionIntent);

                // once we requested, next action will be continued in broadcast receiver.
                return null;
            }

            UsbDeviceConnection usbDeviceConnection = mUsbManager.openDevice(usbDevice);
            if(usbDeviceConnection != null) {
                atsc3UsbDevice = new Atsc3UsbDevice(usbDevice, usbDeviceConnection);

                for (Atsc3NdkPHYClientBase.USBVendorIDProductIDSupportedPHY candidatePHY : candidatePHYList) {
                    Atsc3NdkPHYClientBase atsc3NdkPHYClientBaseCandidate = Atsc3NdkPHYClientBase.CreateInstanceFromUSBVendorIDProductIDSupportedPHY(candidatePHY);
                    String devicePath = atsc3UsbDevice.getDeviceName();
                    int usbFd = atsc3UsbDevice.getFd();

                    if (candidatePHY.getIsBootloader(usbDevice)) {
                        int r = atsc3NdkPHYClientBaseCandidate.download_bootloader_firmware(usbFd, devicePath);
                        if (r < 0) {
                            Log.d(TAG, String.format("prepareDevices: download_bootloader_firmware with %s failed for path: %s, fd: %d", atsc3NdkPHYClientBaseCandidate, devicePath, usbFd));
                        } else {
                            Log.d(TAG, String.format("prepareDevices: download_bootloader_firmware with %s for path: %s, fd: %d, success", atsc3NdkPHYClientBaseCandidate, devicePath, usbFd));
                            //pre-boot devices should re-enumerate, so don't track this connection just yet...
                        }
                    } else {
                        int r = atsc3NdkPHYClientBaseCandidate.open(usbFd, devicePath);
                        if (r < 0) {
                            Log.d(TAG, String.format("prepareDevices: open with %s failed for path: %s, fd: %d, res: %d", atsc3NdkPHYClientBaseCandidate, devicePath, usbFd, r));
                        } else {
                            Log.d(TAG, String.format("prepareDevices: open with %s for path: %s, fd: %d, success", atsc3NdkPHYClientBaseCandidate, devicePath, usbFd));
                            atsc3NdkPHYClientBaseCandidate.setAtsc3UsbDevice(atsc3UsbDevice);
                            atsc3UsbDevice.setAtsc3NdkPHYClientBase(atsc3NdkPHYClientBaseCandidate);
                            atsc3NdkPHYClientBaseInstantiated = atsc3NdkPHYClientBaseCandidate;
                            break;
                        }
                    }
                }

                if(atsc3NdkPHYClientBaseInstantiated == null) {
                    atsc3UsbDevice.destroy();
                }
            }
            ServiceHandler.GetInstance().post(new Runnable() {
                @Override
                public void run() {
                    updateSpinnerFromDevList();
                }});
        }
        return atsc3NdkPHYClientBaseInstantiated;
    }


    private void disconnectAllDevices() {

        for (Map.Entry<UsbDevice, Atsc3UsbDevice> entry : Atsc3UsbDevice.AllAtsc3UsbDevices.entrySet()) {
            if(entry.getValue().atsc3NdkPHYClientBase != null) {
                entry.getValue().atsc3NdkPHYClientBase.deinit();
                entry.getValue().atsc3NdkPHYClientBase = null;
            }
            entry.getValue().disconnect();
        }
        Atsc3UsbDevice.AllAtsc3UsbDevices.clear();
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

            //response from our pending intent for mUsbManager.requestPermission(usbDevice, mPermissionIntent);
            if (ACTION_USB_PERMISSION.equals(action)) {
                synchronized (this) {
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (device == null) { Log.e(TAG, "null device!"); return; }


                    //dispatch a message to try to instantiate our PHY - usbPHYLayerDeviceTryToInstantiateFromRegisteredPHYNDKs(device)
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        dumpDevice(device, "ACTION_USB_PERMISSION:EXTRA_PERMISSION_GRANTED: permission granted");

                        Message msg = mHandler.obtainMessage(2, device);
                        mHandler.sendMessage(msg);
                    }
                    else {
                        Log.d(TAG, "permission denied for device " + device);
                        dumpDevice(device, "ACTION_USB_PERMISSION:EXTRA_PERMISSION_GRANTED: permission denied");

                    }
                    return;
                }
            }

            //device was already granted permissions and re-connected
            if (UsbManager.ACTION_USB_DEVICE_ATTACHED.equals(action)) {
                synchronized (this) {
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (device == null) { Log.e(TAG, "null device!"); return; }

                    dumpDevice(device, " attached");

                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        //call method to set up device communication
                        Log.d(TAG, "ACTION_USB_DEVICE_ATTACHED:EXTRA_PERMISSION_GRANTED: success");

                        //dispatch a message to try to instantiate our PHY - usbPHYLayerDeviceTryToInstantiateFromRegisteredPHYNDKs(device)

                        Message msg = mHandler.obtainMessage(2, device);
                        mHandler.sendMessage(msg);
                    }
                    else {
                        Log.d(TAG, String.format("ACTION_USB_DEVICE_ATTACHED:EXTRA_PERMISSION_GRANTED permission denied for device, dispatching mPermissionIntent: %s, dev: %s", mPermissionIntent, device));
                        final UsbDevice myDeviceNeedingPermission = device;
                        mUsbManager.requestPermission(myDeviceNeedingPermission, mPermissionIntent);
                        return;
                    }
                }
            }

            if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                synchronized (this) {
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (device == null) { Log.e(TAG, "null device!"); return; }

                    dumpDevice(device, "ACTION_USB_DEVICE_DETACHED:detached");

                    //send a device disconnected, and try to clear/shutdown our atsc3NdkPHYClientBase
                    Message msg = mHandler.obtainMessage(3, device);
                    mHandler.sendMessage(msg);

                    return;
                }
            }

            onReceive(context, intent);
        }
    };


    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == PcapFileSelectorActivity.CODE_READ_PCAP_FILE) {
            if (resultCode == RESULT_OK) {
                String pcapFileToProcess = data.getData().toString();
                if(pcapFileToProcess != null) {
                    //hack-ish for now..
                    pcapDemuxedAssetForFilesystemReplay.clear();
                    pcapSTLTPAssetForFilesystemReplay.clear();

                    if(pendingInputSelectionFromPcapDemuxed) {
                        Log.d(TAG, "onActivityResult::pendingInputSelectionFromPcapDemuxed");

                        inputSelectionFromPcapDemuxed = true;
                        inputSelectionFromPcapSTLTP = false;
                        inputSelectedPcapDemuxedFromFilesystem = pcapFileToProcess;
                        pcapDemuxedAssetForFilesystemReplay.add(inputSelectedPcapDemuxedFromFilesystem);

                        inputSelectedPcapSTLTPFromFilesystem = null;

                    } else if(pendingInputSelectionFromPcapSTLTP) {
                        Log.d(TAG, "onActivityResult::pendingInputSelectionFromPcapSTLTP");
                        inputSelectionFromPcapSTLTP = true;
                        inputSelectionFromPcapDemuxed = false;
                        inputSelectedPcapSTLTPFromFilesystem = pcapFileToProcess;
                        pcapSTLTPAssetForFilesystemReplay.add(inputSelectedPcapSTLTPFromFilesystem);

                        inputSelectedPcapDemuxedFromFilesystem = null;
                    }

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

                stopAllPlayers();
                stopAndDeInitAtsc3NdkPHYClientInstance();


                if(inputSelectionFromSRT) {
                    atsc3NdkPHYClientInstance = new SRTRxSTLTPVirtualPHYAndroid();

                    atsc3NdkPHYClientInstance.init();
                    ((SRTRxSTLTPVirtualPHYAndroid)atsc3NdkPHYClientInstance).setSrtSourceConnectionString(inputSelectedSRTSource);

                    atsc3NdkPHYClientInstance.run();
                    enableDeviceControlButtons(true);
                    return;
                }

                if(inputSelectionFromPcapDemuxed) {
                    Log.d(TAG, "open::inputSelectionFromPcapDemuxed");

                    atsc3NdkPHYClientInstance = new PcapDemuxedVirtualPHYAndroid();
                    atsc3NdkPHYClientInstance.init();

                    enableDeviceControlButtons(true);
                    if(inputSelectedPcapDemuxedFromFilesystem != null) {
                        atsc3NdkPHYClientInstance.open_from_capture(inputSelectedPcapDemuxedFromFilesystem);
                    }
//                    } else if(inputSelectedPcapReplayFromAssetManager != null) {
//                        demuxedPcapVirtualPHY.atsc3_pcap_open_for_replay_from_assetManager(inputSelectedPcapReplayFromAssetManager, assetManager);
//                    }

                    atsc3NdkPHYClientInstance.run();
                    enableDeviceControlButtons(true);
                    return;
                }

                if(inputSelectionFromPcapSTLTP) {
                    Log.d(TAG, "open::inputSelectionFromPcapSTLTP, file: "+ inputSelectedPcapSTLTPFromFilesystem);

                    atsc3NdkPHYClientInstance = new PcapSTLTPVirtualPHYAndroid();
                    atsc3NdkPHYClientInstance.init();

                    enableDeviceControlButtons(true);
                    if(inputSelectedPcapSTLTPFromFilesystem != null) {
                        atsc3NdkPHYClientInstance.open_from_capture(inputSelectedPcapSTLTPFromFilesystem);
                    }

                    atsc3NdkPHYClientInstance.run();
                    enableDeviceControlButtons(true);
                    return;
                }

                if (mCurAt3Device == null) {
                    showMsg("no FX3 device connected yet\n");
                    break;
                }

                break;

            case R.id.butTune:

                EditText editFreq = (EditText)findViewById(R.id.editFreqMhz);

                final int freqMHz = Integer.parseInt(editFreq.getText().toString());
                EditText editPlp = (EditText)findViewById(R.id.editPlp);
                final int plp = Integer.parseInt(editPlp.getText().toString());
                Log.d(TAG, "tune with freq " + freqMHz + ", plp " + plp);

                if(atsc3NdkPHYClientInstance != null) {
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            int re = atsc3NdkPHYClientInstance.tune(freqMHz * 1000, plp);
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
                }

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
                    } else {
                        ATSC3PlayerFlags.ATSC3PlayerStartPlayback = false;
                        ATSC3PlayerFlags.ATSC3PlayerStopPlayback = true;
                        myDecoderHandlerThread.decoderHandler.sendMessage(myDecoderHandlerThread.decoderHandler.obtainMessage(DecoderHandlerThread.DESTROY));
                    }
                }
                break;

            case R.id.butStop:
                showMsg("button Stop\n");
                //TODO: jjustman-2019-10-18

                routeDashPlayerStopAndRelease();
                myDecoderHandlerThread.decoderHandler.sendMessage(myDecoderHandlerThread.decoderHandler.obtainMessage(DecoderHandlerThread.DESTROY));

                if(atsc3NdkPHYClientInstance != null) {
                    atsc3NdkPHYClientInstance.stop();
                }

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
                //r = atsc3NdkPHYClientInstance.ApiReset();
                break;

            case R.id.butClose:
                showMsg("button deinit\n");

                if(atsc3NdkPHYClientInstance != null) {
                    atsc3NdkPHYClientInstance.deinit();
                }

                if (r != 0) showMsg("deinit\n");

                enableDeviceControlButtons(false);
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

        //launch our file picker for asset selection
        if(s.startsWith(SELECT_PCAP_DEMUXED_MESSAGE)) {
            pendingInputSelectionFromPcapDemuxed = true;
            pendingInputSelectionFromPcapSTLTP = false;
            Intent intent = new Intent(this, PcapFileSelectorActivity.class);
            startActivityForResult(intent, PcapFileSelectorActivity.CODE_READ_PCAP_FILE);
            //handle this in onActivityResult with the absolute path
            return;
        } else if(s.startsWith(SELECT_PCAP_STLTP_MESSAGE)) {
            pendingInputSelectionFromPcapDemuxed = false;
            pendingInputSelectionFromPcapSTLTP = true;
            Intent intent = new Intent(this, PcapFileSelectorActivity.class);
            startActivityForResult(intent, PcapFileSelectorActivity.CODE_READ_PCAP_FILE);
            //handle this in onActivityResult with the absolute path
            return;
        } else if(s.equals(inputSelectedPcapDemuxedFromFilesystem)) {
            //ignore this onSelect event, as we handle it in the onActivityResult
            return;
        } else if(s.startsWith(PCAP_URI_PREFIX)) {
            //support pre-baked in pcap assets as needed
            inputSelectedPcapDemuxedFromFilesystem = null;
            inputSelectedPcapReplayFromAssetManager = s;
            inputSelectionFromPcapDemuxed = true;
            inputSelectionFromSRT = false;

            enableDeviceOpenButtons(true);
            return;
        } else if(s.startsWith(SRT_STLTP_URI_PREFIX)) {
            //support SRT_STLTP transport
            inputSelectedPcapDemuxedFromFilesystem = null;
            inputSelectedPcapReplayFromAssetManager = null;

            pendingInputSelectionFromPcapDemuxed = false;
            inputSelectionFromPcapDemuxed = false;

            pendingInputSelectionFromPcapSTLTP = false;
            inputSelectionFromPcapSTLTP = false;

            inputSelectedSRTSource = s;
            inputSelectionFromSRT = true;

            enableDeviceOpenButtons(true);
            return;
        } else {
            //do not clear out our selected pcap (either demux or stltp here)
        }
    }
    @Override
    public void onNothingSelected(AdapterView<?> parent) {
        Log.d(TAG, "user selected nothing!");
    }

}
