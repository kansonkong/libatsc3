package org.ngbp.libatsc3.media;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaSync;
import android.media.MediaTimestamp;
import android.media.PlaybackParams;
import android.media.SyncParams;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

import org.ngbp.libatsc3.middleware.android.ATSC3PlayerFlags;
import org.ngbp.libatsc3.middleware.android.DebuggingFlags;
import org.ngbp.libatsc3.ServiceHandler;
import org.ngbp.libatsc3.middleware.android.application.sync.CodecAACSpecificData;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MmtPacketIdContext;
import org.ngbp.libatsc3.sampleapp.MainActivity;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.LinkedBlockingDeque;

import androidx.annotation.NonNull;

import static java.lang.System.nanoTime;

public class DecoderHandlerThread extends HandlerThread {
    public Handler decoderHandler;

    public Surface myDecoderSurface;
    public boolean hasSetVideoCodecOutputSurface = false;

    public static MediaSync sync;

    private SyncParams syncParams;

    //todo: investigate int64_t MediaSync::getRealTime(int64_t mediaTimeUs, int64_t nowUs) {
    Surface syncSurface;

    public static final int PENDING_NAL = 5;
    public static final int CREATE_CODEC = 6;
    public static final int DECODE = 7;
    public static final int DECODE_AV_TUNNEL = 8;
    public static final int DESTROY = -1;

    MediaCodec videoCodec;
    MediaFormat videoMediaFormat;
    MediaCodec.BufferInfo videoDecoderBuffInfo = new MediaCodec.BufferInfo();

    MediaCodec audioCodec;
    MediaFormat audioMediaFormat;
    MediaCodec.BufferInfo audioDecoderBuffInfo = new MediaCodec.BufferInfo();
    AudioTrack audioTrack;

    LinkedBlockingDeque<Integer> mediaCodecInputBufferVideoQueue = new LinkedBlockingDeque<Integer>();
    LinkedBlockingDeque<Integer> mediaCodecInputBufferAudioQueue = new LinkedBlockingDeque<Integer>();

    MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable videoInputFragmentRunnable;
    MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable audioInputFragmentRunnable;
    Thread videoInputFragmentRunnableThread;
    Thread audioInputFragmentRunnableThread;

    long codecTimestampStartMs;
    long codecTimestampStartSystemNS;

    private boolean syncPlaybackParamsIsPausedValue = true;
    private SurfaceHolder surfaceHolder;

    public DecoderHandlerThread(String name) {
        super(name);
    }

    @Override
    protected void onLooperPrepared() {
        //Looper.getMainLooper()  -- don't run this on mainLooper thread as we are async while waiting for data to load
        decoderHandler = new Handler(getLooper()) {
            @Override
            public void handleMessage(Message msg) {
                // process incoming messages here
                // this will run in non-ui/background thread

                switch (msg.what) {
                    case CREATE_CODEC:
                        if(!ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
                            Log.d("DecoderHandlerThread", "CREATE_CODEC: mfuBufferQueueVideo size: "+ ATSC3PlayerMMTFragments.mfuBufferQueueVideo.size()+"mfuBufferQueueAudio size: "+ATSC3PlayerMMTFragments.mfuBufferQueueAudio.size());

                            try {
                                createMfuOuterMediaCodec();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                            // decoderHandler.sendMessage(decoderHandler.obtainMessage(DECODE, "codecCreated"));
                        }
                        break;
                    case DESTROY:
                        destroyCodec();

                        break;

                    default:
                        Log.d("DecoderHandlerThread", String.format("handleMessage: unknown: %d", msg.what));
                        ServiceHandler.GetInstance().dispatchMessage(msg);
                        break;
                }
            }
        };
    }

    private void destroyCodec() {
        ATSC3PlayerFlags.ReentrantLock.lock();
        try {
            ATSC3PlayerFlags.ATSC3PlayerStopPlayback = true;
            ATSC3PlayerFlags.ATSC3PlayerStartPlayback = false;

            if (videoInputFragmentRunnable != null) {
                videoInputFragmentRunnable.shutdown();
                videoInputFragmentRunnable = null;
            }
            if (audioInputFragmentRunnable != null) {
                audioInputFragmentRunnable.shutdown();
                audioInputFragmentRunnable = null;
            }

            if (sync != null) {
                sync.setPlaybackParams(new PlaybackParams().setSpeed(0.0f));
                //sync.flush();
                sync.release();
                Log.d("DecoderHandlerThread", String.format("destroyCodec - released sync"));
                sync = null;
            }

            if (videoCodec != null) {
                videoCodec.release();
                videoCodec = null;
            }
            if (audioCodec != null) {
                audioCodec.release();
                audioCodec = null;
            }
            if (videoMediaFormat != null) {
                videoMediaFormat = null;
            }

            if (syncSurface != null) {
                syncSurface.release();
                syncSurface = null;
            }

            ATSC3PlayerMMTFragments.mfuBufferQueueVideo.clear();
            ATSC3PlayerMMTFragments.mfuBufferQueueAudio.clear();

            ATSC3PlayerFlags.FirstMfuBufferVideoKeyframeSent = false;
            ATSC3PlayerFlags.FirstMfuBuffer_presentation_time_us_mpu = 0;

            syncPlaybackParamsIsPausedValue = true;
        } finally {
            ATSC3PlayerMMTFragments.InitMpuMetadata_HEVC_NAL_Payload = null;
            ATSC3PlayerFlags.FirstMfuBufferVideoKeyframeSent = false;
            ATSC3PlayerFlags.FirstMfuBuffer_presentation_time_us_mpu = 0;

            ATSC3PlayerFlags.ReentrantLock.unlock();
        }

    }

    private void createMfuOuterMediaCodec() throws IOException {
        ATSC3PlayerFlags.ReentrantLock.lock();

        try {
            ATSC3PlayerFlags.ATSC3PlayerStartPlayback = true;
            ATSC3PlayerFlags.ATSC3PlayerStopPlayback = false;
            ATSC3PlayerFlags.FirstMfuBufferVideoKeyframeSent = false;
            ATSC3PlayerFlags.FirstMfuBuffer_presentation_time_us_mpu = 0;

            //TODO: remove this...spinlock...
            while (ATSC3PlayerFlags.ATSC3PlayerStopPlayback == false && ATSC3PlayerMMTFragments.InitMpuMetadata_HEVC_NAL_Payload == null) {
                Log.d("createMfuOuterMediaCodec", "waiting for initMpuMetadata_HEVC_NAL_Payload != null");
                try {
                    Thread.sleep(100);
                } catch (Exception ex) {
                    //
                }
            }
            //spin for at least one video and one audio frame
            while (ATSC3PlayerFlags.ATSC3PlayerStopPlayback == false && (ATSC3PlayerMMTFragments.mfuBufferQueueVideo.peek() == null || ATSC3PlayerMMTFragments.mfuBufferQueueAudio.peek() == null)) {
                Log.d("createMfuOuterMediaCodec", String.format("waiting for mfuBufferQueueVideo, size: %d, mfuBufferQueueAudio, size: %d", ATSC3PlayerMMTFragments.mfuBufferQueueVideo.size(), ATSC3PlayerMMTFragments.mfuBufferQueueAudio.size()));
                try {
                    Thread.sleep(100);
                } catch (Exception ex) {
                    //
                }
            }

            //bail early
            if (ATSC3PlayerFlags.ATSC3PlayerStopPlayback) {
                return;
            }
            //TODO: use proper values from init box avcc/hvcc/mp4a/etc...

            String videoMimeType = "video/hevc";

            int audioSampleRate = 48000;
            int audioChannelCount = 2;
            String audioMimeType = "audio/mp4a-latm";
            audioMediaFormat = CodecAACSpecificData.BuildAACCodecSpecificData(MediaCodecInfo.CodecProfileLevel.AACObjectLC, audioSampleRate, audioChannelCount);

            //jjustman-2020-08-19 - hack-ish for AC-4 audio testing on samsung S10+ with mmt
            if(true) {
                audioMimeType = "audio/ac4";
                audioMediaFormat = new MediaFormat();
                audioMediaFormat.setString(MediaFormat.KEY_MIME, audioMimeType);
                audioMediaFormat.setInteger(MediaFormat.KEY_CHANNEL_COUNT, 2);
                audioMediaFormat.setInteger(MediaFormat.KEY_SAMPLE_RATE, audioSampleRate);
                //mediaFormat.setInteger("ac4-is-sync", 1); ?
                //audioMediaFormat.setInteger("ac4-is-sync", 0);
            }




            try {
                sync = new MediaSync();
                sync.setSurface(myDecoderSurface);
                syncSurface = sync.createInputSurface();
            } catch (Exception ex) {
                Log.d("createMfuOuterMediaCodec", String.format("exception when creating mediaSync - ex: %s", ex));
                //jjustman-2019-12-29 - TODO: failed to connect to surface with error -22
            }

            try {
                videoCodec = MediaCodec.createDecoderByType(videoMimeType);
            } catch (Exception e) {
                e.printStackTrace();
                ServiceHandler.GetInstance().dispatchMessage(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.TOAST, "Unable to instantiate V: " + videoMimeType + " decoder!"));
                return;
            }
            try {
                audioCodec = MediaCodec.createDecoderByType(audioMimeType);
            } catch (Exception e) {
                e.printStackTrace();
                ServiceHandler.GetInstance().dispatchMessage(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.TOAST, "Unable to instantiate A: " + audioMimeType + " decoder!"));
                return;
            }

            if(MmtPacketIdContext.video_packet_statistics.width > 0 && MmtPacketIdContext.video_packet_statistics.height > 0) {
                videoMediaFormat = MediaFormat.createVideoFormat(videoMimeType, MmtPacketIdContext.video_packet_statistics.width, MmtPacketIdContext.video_packet_statistics.height);
            } else {
                //fallback to a "safe" size
                videoMediaFormat = MediaFormat.createVideoFormat(videoMimeType, MmtPacketIdContext.video_packet_statistics.FALLBACK_WIDTH, MmtPacketIdContext.video_packet_statistics.FALLBACK_HEIGHT);
            }
            //mf.setFeatureEnabled(MediaCodecInfo.CodecCapabilities.FEATURE_TunneledPlayback, true);


            byte[] nal_check = new byte[8];
            ATSC3PlayerMMTFragments.InitMpuMetadata_HEVC_NAL_Payload.myByteBuffer.get(nal_check, 0, 8);
            Log.d("createMfuOuterMediaCodec", String.format("HEVC NAL is: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x, len: %d",
                    nal_check[0], nal_check[1], nal_check[2], nal_check[3],
                    nal_check[4], nal_check[5], nal_check[6], nal_check[7], ATSC3PlayerMMTFragments.InitMpuMetadata_HEVC_NAL_Payload.myByteBuffer.capacity()));

            ATSC3PlayerMMTFragments.InitMpuMetadata_HEVC_NAL_Payload.myByteBuffer.rewind();

            videoMediaFormat.setByteBuffer("csd-0", ATSC3PlayerMMTFragments.InitMpuMetadata_HEVC_NAL_Payload.myByteBuffer);
            //jjustman-2019-10-19 - for testing
            //videoMediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 0);
            videoMediaFormat.setInteger("allow-frame-drop", 0);
            //videoMediaFormat.setInteger("allow-frame-drop", 1);

            HandlerThread videoCodecHandlerThread = new HandlerThread("videoCodecHandlerThread");
            videoCodecHandlerThread.start();
            Handler videoCodecHandler = new Handler(videoCodecHandlerThread.getLooper());
            //Handler videoCodecHandler = new Handler(Looper.getMainLooper());

            videoCodec.setCallback(new MediaCodec.Callback() {
                @Override
                public void onInputBufferAvailable(@NonNull MediaCodec mediaCodec, int i) {
                    /*
                    after flush:
                        we only have to ignore outputBuffers, not inputBuffers
                        if(MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsSoftFlushingFromAVPtsDiscontinuity) {
                         return;
                        }
                    */
                    mediaCodecInputBufferVideoQueue.add(i);
                    Log.d("Video", String.format("onInputBufferAvailable: %d", i));
                }

                @Override
                public void onOutputBufferAvailable(@NonNull MediaCodec mediaCodec, int i, @NonNull MediaCodec.BufferInfo bufferInfo) {
                    if (MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsSoftFlushingFromAVPtsDiscontinuity) {
                        return;
                    }
                    MediaTimestamp mediaTimestamp = sync.getTimestamp();
                    if (mediaTimestamp != null) {
                        Log.d("\tVideo", String.format("\tmediaTimestamp\tbufPts deltaMS:\t%f\tdelta anchorMedia-anchorSysMS:\t%f\tanchorMedia:\t%d\tanchorSystem:\t%d\trate:\t%f",
                                (bufferInfo.presentationTimeUs * 1000 - mediaTimestamp.getAnchorSytemNanoTime()) / 1000000.0, ((mediaTimestamp.getAnchorMediaTimeUs() * 1000) - mediaTimestamp.getAnchorSytemNanoTime()) / 1000000.0, mediaTimestamp.getAnchorMediaTimeUs() * 1000, mediaTimestamp.getAnchorSytemNanoTime(), mediaTimestamp.getMediaClockRate()));

                    }
                    if (0 != (MediaCodec.BUFFER_FLAG_END_OF_STREAM & bufferInfo.flags)) {
                        Log.d("Video", "onOutputBufferAvailable BUFFER_FLAG_END_OF_STREAM");
                        // return;
                    }
                    try {
                        long nanoTime = nanoTime();
                        //remap from bufferInfo.presentationTimeUs
                        long deltaNanoTime = ((33 + bufferInfo.presentationTimeUs) * 1000) - nanoTime;

                        if (true || DebuggingFlags.OutputBuferLoggingEnabled) {
                            Log.e("\tVideo\tonOutputBufferAvailable", String.format("\tbufferInfoNs:\t%d\tbufferInfoMS:\t%d\tnanoTime:\t%d\tnanoTimeMS:\t%f\tdeltaNanoTime\t%d\tdeltaNanoTimeMS\t%f",
                                    bufferInfo.presentationTimeUs * 1000,
                                    bufferInfo.presentationTimeUs / 1000,
                                    nanoTime,
                                    nanoTime / 1000000.0,
                                    deltaNanoTime,
                                    deltaNanoTime / 1000000.0));
                        }

                        //                    if(deltaNanoTime > 33000) {
                        //                        long toSleepMs = (deltaNanoTime - 33000) / 1000000;
                        //                        Log.e("Video", String.format("toSleepMs: %d", toSleepMs));
                        //
                        //                        Thread.sleep(toSleepMs);
                        //                    } else {
                        //                        Log.e("Video", String.format("negative deltaNanoTime: %d", deltaNanoTime));
                        //
                        //                    }

                        //TODO: update this
                        //mediaCodec.releaseOutputBuffer(i, bufferInfo.presentationTimeUs * 1000);
                        mediaCodec.releaseOutputBuffer(i, true);

                    } catch (IllegalStateException ise) {
                        Log.e("\tVideo\tonOutputBufferAvailable", String.format("illegal state exception with idx: %d, and pts ns: %d", i, bufferInfo.presentationTimeUs * 1000));
                        //throw ise;
                    }
                    //                catch (InterruptedException e) {
                    //                    e.printStackTrace();
                    //                }
                    String videoReleaseDeltaTString = "";
                    //TODO: fix me
                    if (MmtPacketIdContext.video_packet_statistics.last_mfu_release_microseconds.get(i) != null) {
                        Long videoReleaseDeltaT = bufferInfo.presentationTimeUs - MmtPacketIdContext.video_packet_statistics.last_mfu_release_microseconds.get(i);

                        // videoReleaseDeltaTString = "\n deltaT release "+videoReleaseDeltaT+"\n instant FPS: "+(videoReleaseDeltaT != 0 ? 1000000 / videoReleaseDeltaT: 0);

                    }
                    MmtPacketIdContext.video_packet_statistics.last_output_buffer_presentationTimeUs = bufferInfo.presentationTimeUs;
                    if (DebuggingFlags.MFU_STATS_RENDERING) {
                        Message msg = ServiceHandler.GetInstance().obtainMessage(ServiceHandler.DRAW_TEXT_FRAME_VIDEO_RELEASE_RENDERER, "onOutputBuffer.release ms:  " + (bufferInfo.presentationTimeUs) + " " + videoReleaseDeltaTString);
                        ServiceHandler.GetInstance().sendMessage(msg);
                    }
                    MmtPacketIdContext.video_packet_statistics.last_mfu_release_microseconds.put(i, bufferInfo.presentationTimeUs);
                    //jjustman-2019-10-20: imsc1 queue iteration
                    MfuByteBufferFragment imscFragment = null;
                    if ((imscFragment = ATSC3PlayerMMTFragments.mfuBufferQueueStpp.poll()) != null) {
                        ServiceHandler.GetInstance().sendMessage(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.STPP_IMSC1_AVAILABLE, imscFragment));
                    }
                }

                @Override
                public void onError(@NonNull MediaCodec mediaCodec, @NonNull MediaCodec.CodecException e) {
                    Log.d("Video", "onError");
                    e.printStackTrace();
                }

                @Override
                public void onOutputFormatChanged(@NonNull MediaCodec mediaCodec, @NonNull MediaFormat mediaFormat) {
                    Log.d("Video", "onOutputFormatChanged");
                }
            }, videoCodecHandler);


            HandlerThread audioCodecHandlerThread = new HandlerThread("audioCodecHandlerThread");
            audioCodecHandlerThread.start();
            Handler audioCodecHandler = new Handler(audioCodecHandlerThread.getLooper());
            //Handler audioCodecHandler = new Handler(Looper.getMainLooper());//getMainLooper()

            audioCodec.setCallback(new MediaCodec.Callback() {
                @Override
                public void onInputBufferAvailable(@NonNull MediaCodec mediaCodec, int i) {
                    mediaCodecInputBufferAudioQueue.add(i);
                    Log.d("Audio", String.format("onInputBufferAvailable: %d", i));
                }

                @Override
                public void onOutputBufferAvailable(@NonNull MediaCodec mediaCodec, int i, @NonNull MediaCodec.BufferInfo bufferInfo) {

                    if (MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsSoftFlushingFromAVPtsDiscontinuity) {
                        return;
                    }
                    MediaTimestamp mediaTimestamp = sync.getTimestamp();
                    if (mediaTimestamp != null) {
                        //Log.d("\tAudio", String.format("\tmediaTimestamp\tbufPts deltaMS:\t%f\tdelta anchorMedia-anchorSysMS:\t%f\tanchorMedia:\t%d\tanchorSystem:\t%d\trate:\t%f", (bufferInfo.presentationTimeUs*1000 - mediaTimestamp.getAnchorSytemNanoTime())/1000000.0, ((mediaTimestamp.getAnchorMediaTimeUs()*1000) -  mediaTimestamp.getAnchorSytemNanoTime())/1000000.0, mediaTimestamp.getAnchorMediaTimeUs()*1000, mediaTimestamp.getAnchorSytemNanoTime(), mediaTimestamp.getMediaClockRate()));

                    }

                    try {
                        long nanoTime = nanoTime();
                        //remap from bufferInfo.presentationTimeUs
                        long deltaNanoTime = ((33 + bufferInfo.presentationTimeUs) * 1000) - nanoTime;

                        if (true || DebuggingFlags.OutputBuferLoggingEnabled) {
                            Log.e("\tAudio\tonOutputBufferAvailable", String.format("\tbufferInfoNs:\t%d\tbufferInfoMS:\t%d\tnanoTime:\t%d\tnanoTimeMS:\t%f\tdeltaNanoTime\t%d\tdeltaNanoTimeMS\t%f",
                                    bufferInfo.presentationTimeUs * 1000,
                                    bufferInfo.presentationTimeUs / 1000,
                                    nanoTime,
                                    nanoTime / 1000000.0,
                                    deltaNanoTime,
                                    deltaNanoTime / 1000000.0));
                        }
                    } catch (Exception ex) {
                        //
                    }

                    ByteBuffer decoderBuffer = audioCodec.getOutputBuffer(i);
                    sync.queueAudio(decoderBuffer, i, bufferInfo.presentationTimeUs);

                    MmtPacketIdContext.audio_packet_statistics.last_output_buffer_presentationTimeUs = bufferInfo.presentationTimeUs;
                    if (DebuggingFlags.MFU_STATS_RENDERING) {
                        Message msg = ServiceHandler.GetInstance().obtainMessage(ServiceHandler.DRAW_TEXT_FRAME_AUDIO_RELEASE_RENDERER,
                                String.format("onOutputBuffer.release ms:\n %d", bufferInfo.presentationTimeUs));
                        ServiceHandler.GetInstance().sendMessage(msg);
                    }
                }

                @Override
                public void onError(@NonNull MediaCodec mediaCodec, @NonNull MediaCodec.CodecException e) {
                    Log.d("Audio", "onError");
                    e.printStackTrace();
                }

                @Override
                public void onOutputFormatChanged(@NonNull MediaCodec mediaCodec, @NonNull MediaFormat mediaFormat) {
                    Log.d("Audio", "onOutputFormatChanged");
                }
            }, audioCodecHandler);

            int buffsize = AudioTrack.getMinBufferSize(audioSampleRate, AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT);

            videoCodec.configure(videoMediaFormat, syncSurface, null, 0);

            audioCodec.configure(audioMediaFormat, null, null, 0);
            AudioTrack audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, audioSampleRate,
                    AudioFormat.CHANNEL_OUT_STEREO,
                    AudioFormat.ENCODING_PCM_16BIT,
                    buffsize,
                    AudioTrack.MODE_STREAM);

            sync.setAudioTrack(audioTrack);

            HandlerThread syncThread = new HandlerThread("sync");
            syncThread.start();
            Handler syncHandler = new Handler(syncThread.getLooper()); //getMainLooper() //Looper.getMainLooper()
            //Handler syncHandler = new Handler(Looper.getMainLooper()); //getMainLooper() //Looper.getMainLooper()

            sync.setCallback(new MediaSync.Callback() {
                @Override
                public void onAudioBufferConsumed(@NonNull MediaSync mediaSync, @NonNull ByteBuffer byteBuffer, int i) {
                    if (MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsSoftFlushingFromAVPtsDiscontinuity) {
                        return;
                    }
                    try {
                        audioCodec.releaseOutputBuffer(i, false);

                        if (false) {
                            Log.d("MediaSync", "onAudioBufferConsumed i " + i);
                        }
                    } catch (Exception ex) {
                        Log.w("MediaSync.callback", String.format("exception is: " + ex));
                    }
                }
            }, syncHandler);

            sync.setOnErrorListener(new MediaSync.OnErrorListener() {
                @Override
                public void onError(@NonNull MediaSync mediaSync, int i, int i1) {
                    Log.d("MediaSync", "onError " + i + " " + i1);
                }
            }, syncHandler);


            syncParams = new SyncParams().allowDefaults();
            sync.setPlaybackParams(new PlaybackParams().setSpeed(1.0f));

            //float pcapTargetRational = (float)(MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us/1000000.0);
            float pcapTargetRational = (float) 1000000.0 / MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us;
            syncParams.setFrameRate(pcapTargetRational);
            syncParams.setTolerance(0.5f);


            //sync.setPlaybackParams(new PlaybackParams().setSpeed(0.0f));
            //syncParams.setSyncSource(SyncParams.SYNC_SOURCE_VSYNC); // backlog of vframes queueing up
            // syncParams.setSyncSource(SyncParams.SYNC_SOURCE_DEFAULT);
            //syncParams.setSyncSource(SyncParams.SYNC_SOURCE_SYSTEM_CLOCK); //jitter - lots of jitter
            syncParams.setSyncSource(SyncParams.SYNC_SOURCE_AUDIO); //video frames queue up?

            syncPlaybackParamsIsPausedValue = true;
            sync.setSyncParams(syncParams);

            videoCodec.start();
            audioCodec.start();

            //codecTsStart = System.currentTimeMillis() * 1000;
            codecTimestampStartMs = System.currentTimeMillis();
            codecTimestampStartSystemNS = System.nanoTime();

            ServiceHandler.GetInstance().sendMessageDelayed(ServiceHandler.GetInstance().obtainMessage(ServiceHandler.VIDEO_RESIZE), 1);
            Thread forcedInvalidate = new Thread(new Runnable() {
                @Override
                public void run() {
                    while (true) {
                        ServiceHandler.GetInstance().post(new Runnable() {
                            @Override
                            public void run() {
                                MainActivity.mSurfaceView1.invalidate();
                            }
                        });
                        try {
                            Thread.sleep(33);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
            });

            forcedInvalidate.start();

            /*
            The client can optionally pre-fill audio/video buffers by setting playback rate to 0.0,
            and then feed audio/video buffers to corresponding components.

            This can reduce possible initial underrun.

            https://developer.android.com/reference/android/media/MediaSync
             */

            Log.d("createMfuOuterMediaCodec", String.format("codecInfo: %s, inputFormat: %s", videoCodec.getCodecInfo(), videoCodec.getInputFormat()));

            MediaFormat videoInputFormat = videoCodec.getInputFormat();
            MediaFormat audioInputFormat = audioCodec.getInputFormat();

            Log.d("createMfuOuterMediaCodec", String.format("video format: %s, audio format: %s", videoInputFormat.toString(), audioInputFormat.toString()));

            videoInputFragmentRunnable = new MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable("video", MmtPacketIdContext.video_packet_statistics, videoCodec, mediaCodecInputBufferVideoQueue, ATSC3PlayerMMTFragments.mfuBufferQueueVideo);
            audioInputFragmentRunnable = new MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable("audio", MmtPacketIdContext.audio_packet_statistics, audioCodec, mediaCodecInputBufferAudioQueue, ATSC3PlayerMMTFragments.mfuBufferQueueAudio);

            videoInputFragmentRunnableThread = new Thread(videoInputFragmentRunnable);
            videoInputFragmentRunnableThread.setName("videoMediaCodecInputMfuByteBufferThread");
            videoInputFragmentRunnableThread.start();

            audioInputFragmentRunnableThread = new Thread(audioInputFragmentRunnable);
            audioInputFragmentRunnableThread.setName("audioMediaCodecInputMfuByteBufferThread");

            audioInputFragmentRunnableThread.start();
        } finally {
            ATSC3PlayerFlags.ReentrantLock.unlock();
        }

    }

    public void setOutputSurface(SurfaceHolder surfaceHolder, Surface myDecoderSurface) {
        Log.d("DecoderHandlerThread", String.format("setOutputSurface with surfaceHolder: %s, myDecoderSurface: %s", surfaceHolder, myDecoderSurface));

        this.surfaceHolder = surfaceHolder;
        this.myDecoderSurface = myDecoderSurface;
    }

    public void clearOutputSurface() {
        Log.d("DecoderHandlerThread", String.format("clearOutputSurface - clearing myDecoderSurface / surfaceHolder!"));

        destroyCodec();
        this.myDecoderSurface = null;
        this.surfaceHolder = null;

    }
}