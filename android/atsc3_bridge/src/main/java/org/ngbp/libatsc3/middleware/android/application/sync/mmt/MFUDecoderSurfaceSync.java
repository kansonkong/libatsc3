//package org.ngbp.libatsc3.middleware.android.application.sync.mmt;
//
//import android.media.AudioFormat;
//import android.media.AudioManager;
//import android.media.AudioTrack;
//import android.media.MediaCodec;
//import android.media.MediaFormat;
//import android.media.MediaSync;
//import android.media.PlaybackParams;
//import android.os.Bundle;
//import android.os.Handler;
//import android.support.annotation.NonNull;
//import android.support.v7.app.AppCompatActivity;
//import android.util.Log;
//import android.view.Surface;
//import android.view.SurfaceHolder;
//import android.view.SurfaceView;
//
//import org.ngbp.libatsc3.MainActivity;
//import com.lowasis.at3drv.R;
//
//import java.io.IOException;
//import java.nio.ByteBuffer;
//
//public class MFUDecoderSurfaceSync extends AppCompatActivity implements SurfaceHolder.Callback {
//    final static String TAG = "MFUDecoderSurfaceSync";
//    protected static final int REQUEST_PERMISSION = 100;
//
//    private SurfaceView msvPlay;
//    //private MediaExtractor mExtractorVideo;
//    //private MediaExtractor mExtractorAudio;
//    private MediaFormat videoInputMediaFormat;
//    private MediaCodec videoDecoder;
//
//    private int miVideoTrack;
//    private int nFrameRate;
//
//    private MediaFormat audioInputMediaFormat;
//    private MediaCodec audioDecoder;
//
//    private int miAudioTrack;
//    private int mnSampleRate;
//    private int mnChannel;
//    private MainActivity mThis;
//
//    private MediaSync mMediaSync;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(findViewById(R.id.video));
//
//      //  mThis = this;
//
//        msvPlay = (SurfaceView)findViewById(R.id.surfaceView2);
//        SurfaceHolder sh = msvPlay.getHolder();
//        sh.addCallback(this);
//    }
//
//    @Override
//    protected void onPause() {
//        mMediaSync.setPlaybackParams(new PlaybackParams().setSpeed(0.f));
//        mMediaSync.release();
//
//        videoDecoder.stop();
//        videoDecoder.release();
//
//        audioDecoder.stop();
//        audioDecoder.release();
//
////        mExtractorVideo.release();
////        mExtractorAudio.release();
//
//        super.onPause();
//    }
//
//    /***
//     * MediaSync sample from: https://developer.android.com/reference/android/media/MediaSync.html
//     *
//     * MediaSync sync = new MediaSync();
//     *  sync.setSurface(surface);
//     *  Surface inputSurface = sync.createInputSurface();
//     *  ...
//     *  // MediaCodec videoDecoder = ...;
//     *  videoDecoder.configure(format, inputSurface, ...);
//     *  ...
//     *  sync.setAudioTrack(audioTrack);
//     *  sync.setCallback(new MediaSync.Callback() {
//     *      @Override
//     *      public void onAudioBufferConsumed(MediaSync sync, ByteBuffer audioBuffer, int bufferId) {
//     *          ...
//     *      }
//     *  }, null);
//     *  // This needs to be done since sync is paused on creation.
//     *  sync.setPlaybackParams(new PlaybackParams().setSpeed(1.f));
//     *
//     *  for (;;) {
//     *    ...
//     *    // send video frames to surface for rendering, e.g., call
//     *    // videoDecoder.releaseOutputBuffer(videoOutputBufferIx, videoPresentationTimeNs);
//     *    // More details are available as below.
//     *    ...
//     *    sync.queueAudio(audioByteBuffer, bufferId, audioPresentationTimeUs); // non-blocking.
//     *    // The audioByteBuffer and bufferId will be returned via callback.
//     *    // More details are available as below.
//     *    ...
//     *      ...
//     *  }
//     *  sync.setPlaybackParams(new PlaybackParams().setSpeed(0.f));
//     *  sync.release();
//     *  sync = null;
//     *
//     *  // The following code snippet illustrates how video/audio raw frames are created by
//     *  // MediaCodec's, how they are fed to MediaSync and how they are returned by MediaSync.
//     *  // This is the callback from MediaCodec.
//     *  onOutputBufferAvailable(MediaCodec codec, int bufferId, BufferInfo info) {
//     *      // ...
//     *      if (codec == videoDecoder) {
//     *          // surface timestamp must contain media presentation time in nanoseconds.
//     *          codec.releaseOutputBuffer(bufferId, 1000 * info.presentationTime);
//     *      } else {
//     *          ByteBuffer audioByteBuffer = codec.getOutputBuffer(bufferId);
//     *          sync.queueAudio(audioByteBuffer, bufferId, info.presentationTime);
//     *      }
//     *      // ...
//     *  }
//     *
//     *  // This is the callback from MediaSync.
//     *  onAudioBufferConsumed(MediaSync sync, ByteBuffer buffer, int bufferId) {
//     *      // ...
//     *      audioDecoder.releaseBuffer(bufferId, false);
//     *      // ...
//     *  }
//     */
//
//    /**
//     * following impl from:
//     *
//     * https://github.com/skysign/MediaSyncExample/blob/master/app/src/main/java/org/mediasyncexample/MainActivity.java
//     * @param surfaceHolder
//     */
//
//    @Override
//    public void surfaceCreated(SurfaceHolder surfaceHolder) {
//        Log.d(TAG, "surfaceCreated");
//    }
//
//    @Override
//    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
//        Log.d(TAG, "surfaceChanged " + format + " " + width + " " + height);
//        MediaSync sync = new MediaSync();
//        sync.setSurface(surfaceHolder.getSurface());
//        Surface inputSurface = sync.createInputSurface();
//
//        String audioCodecName = "";
//        try {
//            audioDecoder = MediaCodec.createByCodecName(audioCodecName);
//        } catch (Exception ex) {
//            //noop
//        }
//        MediaFormat audioInputMediaFormat = MediaFormat.createAudioFormat("", 0, 0);
//        //?audioDecoder.configure(audioInputMediaFormat);
//
//        String videoCodecName = "";
//        try {
//            videoDecoder = MediaCodec.createByCodecName(videoCodecName);
//        } catch (Exception ex) {
//            //noop
//        }
//        MediaFormat videoInputMediaFormat = MediaFormat.createVideoFormat("", 0, 0);
//
//        nFrameRate = videoInputMediaFormat.getInteger(MediaFormat.KEY_FRAME_RATE);
//
//        Surface surface = mMediaSync.createInputSurface();
//
//        videoDecoder.configure(videoInputMediaFormat, inputSurface, null, 0);
//        videoDecoder.setCallback(new MediaCodec.Callback() {
//            @Override
//            public void onInputBufferAvailable(@NonNull MediaCodec mediaCodec, int i) {
//                ByteBuffer byteBuffer = videoDecoder.getInputBuffer(i);
//
//                int nRead = 0; //TODO: marshalled libatsc3 buffer handoffs here
//                // mExtractorVideo.readSampleData(byteBuffer, 0);
//
//                Log.d("Video", "onInputBufferAvailable i " + i + " nRead " + nRead);
//                long mpu_v_sampe_time = 0L; //mExtractorVideo.getSampleTime()
//                videoDecoder.queueInputBuffer(i, 0, nRead, mpu_v_sampe_time  , 0);
//                //implict mExtractorVideo.advance();
//            }
//
//            @Override
//            public void onOutputBufferAvailable(@NonNull MediaCodec mediaCodec, int i, @NonNull MediaCodec.BufferInfo bufferInfo) {
//                if (0 != (MediaCodec.BUFFER_FLAG_END_OF_STREAM & bufferInfo.flags)) {
//                    Log.d("Video", "onOutputBufferAvailable BUFFER_FLAG_END_OF_STREAM");
////                            return;
//                }
//
//                videoDecoder.releaseOutputBuffer(i, bufferInfo.presentationTimeUs * 1000);
//                Log.d("Video", "onOutputBufferAvailable i " + i + " presentationTimeUs " + bufferInfo.presentationTimeUs);
//            }
//
//            @Override
//            public void onError(@NonNull MediaCodec mediaCodec, @NonNull MediaCodec.CodecException e) {
//                Log.d("Video", "onError");
//                e.printStackTrace();
//            }
//
//            @Override
//            public void onOutputFormatChanged(@NonNull MediaCodec mediaCodec, @NonNull MediaFormat mediaFormat) {
//                Log.d("Video", "onOutputFormatChanged");
//            }
//        }, new Handler());
//
//
//        //audio here ..
//        //may not know sample rate from ISOBMFF track data..
//        mnSampleRate = audioInputMediaFormat.getInteger(MediaFormat.KEY_SAMPLE_RATE);
//        mnChannel = audioInputMediaFormat.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
//        audioDecoder.configure(audioInputMediaFormat, null, null, 0);
//        audioDecoder.setCallback(new MediaCodec.Callback() {
//            @Override
//            public void onInputBufferAvailable(@NonNull MediaCodec mediaCodec, int i) {
//                ByteBuffer byteBuffer = audioDecoder.getInputBuffer(i);
//                //todo: int nRead = mExtractorAudio.readSampleData(byteBuffer, 0);
//                int nRead = 0;
//
//                Log.d("Audio", "onInputBufferAvailable i " + i + " nRead " + nRead);
//
//                //                if (nRead < 0) {
//                //                    mAudioDecoder.queueInputBuffer(i, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
//                //                } else {
//                //ipmlicit mExtractorAudio.getSampleTime()
//                audioDecoder.queueInputBuffer(i, 0, nRead, 0,0);
//                //implicit - mExtractorAudio.advance();
//            }
//
//            @Override
//            public void onOutputBufferAvailable(@NonNull MediaCodec mediaCodec, int i, @NonNull MediaCodec.BufferInfo bufferInfo) {
//                ByteBuffer decoderBuffer = audioDecoder.getOutputBuffer(i);
//                ByteBuffer copyBuffer = ByteBuffer.allocate(decoderBuffer.remaining());
//                copyBuffer.put(decoderBuffer);
//                copyBuffer.flip();
//                audioDecoder.releaseOutputBuffer(i, false);
//                mMediaSync.queueAudio(copyBuffer, i, bufferInfo.presentationTimeUs);
//
//                Log.d("Audio", "onOutputBufferAvailable i " + i + " presentationTimeUs " + bufferInfo.presentationTimeUs);
//            }
//
//            @Override
//            public void onError(@NonNull MediaCodec mediaCodec, @NonNull MediaCodec.CodecException e) {
//                Log.d("Audio", "onError");
//                e.printStackTrace();
//            }
//
//            @Override
//            public void onOutputFormatChanged(@NonNull MediaCodec mediaCodec, @NonNull MediaFormat mediaFormat) {
//                Log.d("Audio", "onOutputFormatChanged");
//            }
//        });
//
//        int buffsize = AudioTrack.getMinBufferSize(mnSampleRate, AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT);
//        AudioTrack audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, mnSampleRate,
//                AudioFormat.CHANNEL_OUT_STEREO,
//                AudioFormat.ENCODING_PCM_16BIT,
//                buffsize,
//                AudioTrack.MODE_STREAM);
//        mMediaSync.setAudioTrack(audioTrack);
//
//        mMediaSync.setCallback(new MediaSync.Callback() {
//            @Override
//            public void onAudioBufferConsumed(@NonNull MediaSync mediaSync, @NonNull ByteBuffer byteBuffer, int i) {
//                byteBuffer.clear();
//                Log.d("MediaSync", "onAudioBufferConsumed i " + i);
//            }
//        }, new Handler());
//
//        mMediaSync.setOnErrorListener(new MediaSync.OnErrorListener() {
//            @Override
//            public void onError(@NonNull MediaSync mediaSync, int i, int i1) {
//                Log.d("MediaSync", "onError "+i +" " + i1);
//            }
//        }, null);
//
////        SyncParams params = new SyncParams().allowDefaults();
//////        params.setFrameRate(nFrameRate);
//////        params.setAudioAdjustMode(SyncParams.AUDIO_ADJUST_MODE_DEFAULT);
//////        params.setSyncSource(SyncParams.SYNC_SOURCE_AUDIO);
//////        params.setSyncSource(SyncParams.SYNC_SOURCE_VSYNC);
////        mMediaSync.setSyncParams(params);
//
//        mMediaSync.setPlaybackParams(new PlaybackParams().setSpeed(1.0f));
//        Log.d("MediaSync", "start");
//        audioDecoder.start();
//        Log.d("mAudioDecoder", "start");
//        videoDecoder.start();
//        Log.d("mVideoDecoder", "start");
//    }
//
//    @Override
//    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
//        Log.d(TAG, "surfaceDestroyed");
//    }
//}
//
////alternative impl
////
////
////    //////
////        sync.setAudioTrack(audioTrack);
////        sync.setCallback(new MediaSync.Callback() {
////            @Override
////            public void onAudioBufferConsumed(MediaSync sync, ByteBuffer audioBuffer, int bufferId) {
////                 ...
////            }
////        }, null);
////        // This needs to be done since sync is paused on creation.
////        sync.setPlaybackParams(new PlaybackParams().setSpeed(1.f));
////
////        for (;;) {
////            // send video frames to surface for rendering, e.g., cal
////            videoDecoder.releaseOutputBuffer(videoOutputBufferIx, videoPresentationTimeNs);
////            // More details are available as below.
////            sync.queueAudio(audioByteBuffer, bufferId, audioPresentationTimeUs); // non-blocking.
////            // The audioByteBuffer and bufferId will be returned via callback.
////            // More details are available as below.
////        }
////
////        sync.setPlaybackParams(new PlaybackParams().setSpeed(0.f));
////        sync.release();
////        sync = null;
////
////        // The following code snippet illustrates how video/audio raw frames are created by
////        // MediaCodec's, how they are fed to MediaSync and how they are returned by MediaSync.
////
////        // This is the callback from MediaCodec.
////        onOutputBufferAvailable(MediaCodec codec, int bufferId, BufferInfo info) {
////            // ...
////            if (codec == videoDecoder) {
////                // surface timestamp must contain media presentation time in nanoseconds.
////                codec.releaseOutputBuffer(bufferId, 1000 * info.presentationTime);
////            } else {
////                ByteBuffer audioByteBuffer = codec.getOutputBuffer(bufferId);
////                sync.queueAudio(audioByteBuffer, bufferId, info.presentationTime);
////            }
////            // ...
////        }
////
////        // This is the callback from MediaSync.
////        onAudioBufferConsumed(MediaSync sync, ByteBuffer buffer, int bufferId) {
////            // ...
////            audioDecoder.releaseBuffer(bufferId, false);
////            // ...
////        }
////
////    }
////}
