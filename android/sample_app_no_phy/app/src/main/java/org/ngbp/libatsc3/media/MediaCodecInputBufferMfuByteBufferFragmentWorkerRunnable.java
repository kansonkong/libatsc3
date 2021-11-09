package org.ngbp.libatsc3.media;

import android.media.MediaCodec;
import android.media.PlaybackParams;
import android.os.Message;
import android.util.Log;

import org.ngbp.libatsc3.middleware.android.DebuggingFlags;
import org.ngbp.libatsc3.ServiceHandler;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MmtPacketIdContext;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.TimeUnit;

public class MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable implements Runnable {
    String type;
    MmtPacketIdContext.MmtMfuStatistics mmtMfuStatistics;
    MediaCodec mediaCodec;
    LinkedBlockingDeque<Integer> mediaCodecInputBufferQueue;
    LinkedBlockingDeque<MfuByteBufferFragment> mfuByteBufferQueue;

    Boolean shouldRun = true;
    Boolean isShutdown = false;

    //hack-ish
    public static MediaCodec VideoMediaCodec;
    public static MediaCodec AudioMediaCodec;

    public static LinkedBlockingDeque<Integer> VideoMediaCodecInputBufferQueue;
    public static LinkedBlockingDeque<MfuByteBufferFragment> VideoMfuByteBufferQueue;

    public static LinkedBlockingDeque<Integer> AudioMediaCodecInputBufferQueue;
    public static LinkedBlockingDeque<MfuByteBufferFragment> AudioMfuByteBufferQueue;

    public static boolean IsSoftFlushingFromAVPtsDiscontinuity = false;         //only discard the next few inputBuffer and reset our anchors
    public static boolean IsHardCodecFlushingFromAVPtsDiscontinuity = false;    //flush MediaCodec pipelines
    public static boolean IsResettingCodecFromDiscontinuity = false;    //flush MediaCodec pipelines

    public static LinkedHashMap<Long, Long> MapVideoMfuPresentationTimestampUsAnchorSystemTimeUs = new LinkedHashMap<>();
    public static LinkedHashMap<Long, Long> MapAudioMfuPresentationTimestampUsAnchorSystemTimeUs = new LinkedHashMap<>();

    public MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable(String type, MmtPacketIdContext.MmtMfuStatistics mmtMfuStatistics, MediaCodec mediaCodec, LinkedBlockingDeque<Integer> mediaCodecInputBufferQueue, LinkedBlockingDeque<MfuByteBufferFragment> mfuByteBufferQueue) {
        this.type = type;
        this.mmtMfuStatistics = mmtMfuStatistics;
        this.mediaCodec = mediaCodec;
        this.mediaCodecInputBufferQueue = mediaCodecInputBufferQueue;
        this.mfuByteBufferQueue = mfuByteBufferQueue;
        if(type.equalsIgnoreCase("video")) {
            VideoMediaCodec = mediaCodec;
            VideoMediaCodecInputBufferQueue = mediaCodecInputBufferQueue;
            VideoMfuByteBufferQueue = mfuByteBufferQueue;
        } else if(type.equalsIgnoreCase("audio")){
            AudioMediaCodec = mediaCodec;
            AudioMediaCodecInputBufferQueue = mediaCodecInputBufferQueue;
            AudioMfuByteBufferQueue = mfuByteBufferQueue;
        }
    }

    public void shutdown() {
        shouldRun = false;
        while(!isShutdown) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        mediaCodecInputBufferQueue.clear();
        mediaCodecInputBufferQueue = null;
        mfuByteBufferQueue.clear();
        mfuByteBufferQueue = null;

        mediaCodec = null;
        mmtMfuStatistics = null;
    }

    @Override
    public void run() {
        while(shouldRun) {
            Integer toProcessMediaCodecInputBufferIndex = null;
            MfuByteBufferFragment toProcessMfuByteBufferFragment = null;
            try {
                while(IsSoftFlushingFromAVPtsDiscontinuity || IsHardCodecFlushingFromAVPtsDiscontinuity || IsResettingCodecFromDiscontinuity) {
                    Thread.sleep(100);
                }

                while(shouldRun && (toProcessMediaCodecInputBufferIndex = mediaCodecInputBufferQueue.poll(100, TimeUnit.MILLISECONDS)) == null) {
                    Thread.sleep(1);
                }

                while(shouldRun && (toProcessMfuByteBufferFragment = mfuByteBufferQueue.poll(100, TimeUnit.MILLISECONDS)) == null) {
                    Thread.sleep(1);
                }

                if(!shouldRun) {
                    continue;
                }

                Long ptsOffsetUs = 66000L;

                //by default for any missing MMT SI emissions or flash-cut into MMT flow emission, use now_Us + 66000uS for our presentationTimestampUs
                Long computedPresentationTimestampUs = System.currentTimeMillis() * 1000 + ptsOffsetUs;

                if(toProcessMfuByteBufferFragment.mfu_presentation_time_uS_computed != null && toProcessMfuByteBufferFragment.mfu_presentation_time_uS_computed > 0) {
                    //default values here as fallback
                    Long anchorMfuPresentationTimestampUs = toProcessMfuByteBufferFragment.mpu_presentation_time_uS_from_SI;
                    Long anchorSystemTimeUs = System.currentTimeMillis() * 1000;

                    //todo: expand size as needed, every ~ mfu_presentation_time_uS_computed 1000000uS
                    if (toProcessMfuByteBufferFragment.packet_id == MmtPacketIdContext.video_packet_id) {

                        if (MapVideoMfuPresentationTimestampUsAnchorSystemTimeUs.size() == 0) {
                            MapVideoMfuPresentationTimestampUsAnchorSystemTimeUs.put(toProcessMfuByteBufferFragment.mfu_presentation_time_uS_computed, System.currentTimeMillis() * 1000);
                        }
                        for(Map.Entry<Long, Long> anchor : MapVideoMfuPresentationTimestampUsAnchorSystemTimeUs.entrySet()) {
                            anchorMfuPresentationTimestampUs = anchor.getKey();
                            anchorSystemTimeUs = anchor.getValue();
                        }
                    }

                    if(toProcessMfuByteBufferFragment.packet_id == MmtPacketIdContext.audio_packet_id) {
                        if (MapAudioMfuPresentationTimestampUsAnchorSystemTimeUs.size() == 0) {
                            MapAudioMfuPresentationTimestampUsAnchorSystemTimeUs.put(toProcessMfuByteBufferFragment.mfu_presentation_time_uS_computed, System.currentTimeMillis() * 1000);
                        }
                        for(Map.Entry<Long, Long> anchor : MapAudioMfuPresentationTimestampUsAnchorSystemTimeUs.entrySet()) {
                            anchorMfuPresentationTimestampUs = anchor.getKey();
                            anchorSystemTimeUs = anchor.getValue();
                        }
                    }

                    Long mpuPresentationTimestampDeltaUs = toProcessMfuByteBufferFragment.mfu_presentation_time_uS_computed - anchorMfuPresentationTimestampUs;
                    computedPresentationTimestampUs = anchorSystemTimeUs + mpuPresentationTimestampDeltaUs + ptsOffsetUs;
                }
                Log.d("computePresentationTimestampUs", String.format("type: %s, mpu_sequence_number: %d, sample_number: %d, mpu_ts: %d, mfu_ts: %d, computedPtsUs: %d",
                        this.type,
                        toProcessMfuByteBufferFragment.mpu_sequence_number,
                        toProcessMfuByteBufferFragment.sample_number,
                        toProcessMfuByteBufferFragment.mpu_presentation_time_uS_from_SI,
                        toProcessMfuByteBufferFragment.mfu_presentation_time_uS_computed,

                        computedPresentationTimestampUs));

                //timestamp discontinuity > 1000000 (1s) or mpu_sequence_number in the past, flush mediaCodec to reset
                if((mmtMfuStatistics.last_computedPresentationTimestampUs != null && computedPresentationTimestampUs < (mmtMfuStatistics.last_computedPresentationTimestampUs - 1000000)) ||
                        (mmtMfuStatistics.last_mpu_sequence_number_inputBufferQueued != null && mmtMfuStatistics.last_mpu_sequence_number_inputBufferQueued > toProcessMfuByteBufferFragment.mpu_sequence_number)){

                    softFlushAVPtsDiscontinuity(mmtMfuStatistics.last_computedPresentationTimestampUs, computedPresentationTimestampUs, mmtMfuStatistics.last_mpu_sequence_number_inputBufferQueued, toProcessMfuByteBufferFragment.mpu_sequence_number);
                    continue;
                }


                mmtMfuStatistics.last_computedPresentationTimestampUs = computedPresentationTimestampUs;
                mmtMfuStatistics.last_mpu_sequence_number_inputBufferQueued = toProcessMfuByteBufferFragment.mpu_sequence_number;

                if(MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsResettingCodecFromDiscontinuity) {
                    //skip writing this toProcessMfuByteBufferFragment
                    toProcessMfuByteBufferFragment.unreferenceByteBuffer();
                    continue;
                } else if(MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsHardCodecFlushingFromAVPtsDiscontinuity) {
                    //push front our input buffer index
                    mediaCodecInputBufferQueue.addFirst(toProcessMediaCodecInputBufferIndex);
                    toProcessMfuByteBufferFragment.unreferenceByteBuffer();
                    return;
                }

                MediaCodecInputBuffer.WriteToInputBufferFromMfuByteBufferFragment(mediaCodec, toProcessMediaCodecInputBufferIndex, toProcessMfuByteBufferFragment);

                MediaCodecInputBuffer.QueueInputBuffer(mediaCodec, toProcessMediaCodecInputBufferIndex, toProcessMfuByteBufferFragment, computedPresentationTimestampUs,  mmtMfuStatistics);

                mmtMfuStatistics.decoder_buffer_queue_input_count++;
                if(type.equalsIgnoreCase("VIDEO")) {
                    if(DebuggingFlags.MFU_STATS_RENDERING) {
                        Message msg = ServiceHandler.GetInstance().obtainMessage(ServiceHandler.DRAW_TEXT_FRAME_VIDEO_ENQUEUE_US, String.format("V:onFrameEnque: mpu_seq_num: %d\n s: %d, fr: %d, enQ: %d\n ptsUs: %d",
                                toProcessMfuByteBufferFragment.mpu_sequence_number, toProcessMfuByteBufferFragment.sample_number, MmtPacketIdContext.video_packet_statistics.decoder_buffer_queue_input_count,
                                mfuByteBufferQueue.size(), computedPresentationTimestampUs));
                        ServiceHandler.GetInstance().sendMessage(msg);
                    }
                } else if(type.equalsIgnoreCase("AUDIO")){
                    if(DebuggingFlags.MFU_STATS_RENDERING) {
                        Message msg = ServiceHandler.GetInstance().obtainMessage(ServiceHandler.DRAW_TEXT_FRAME_AUDIO_ENQUEUE_US, String.format("A:onFrameEnque: mpu_seq_num: %d\n s: %d, fr: %d, enQ: %d\n ptsUs: %d",
                                toProcessMfuByteBufferFragment.mpu_sequence_number, toProcessMfuByteBufferFragment.sample_number,
                                MmtPacketIdContext.audio_packet_statistics.decoder_buffer_queue_input_count,
                                mfuByteBufferQueue.size(),
                                computedPresentationTimestampUs));
                        ServiceHandler.GetInstance().sendMessage(msg);
                    }
                }

            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }

        isShutdown = true;
    }

    //timestamp discontinuity, flush mediaCodec to reset
    private void softFlushAVPtsDiscontinuity(long last_computedPresentationTimestampUs, long computedPresentationTimestampUs, int last_mpu_sequence_number, int current_mpu_sequence_number) {
        try {
            Log.d("MediaCodecInputBuffer", String.format("softFlushAVPtsDiscontinuity: Flushing, as last_computedPresentationTimestampUs: %d, computedPresentationTimestampUs: %d, last_mpu_sequence_number: %d, current_mpu_sequence_number: %d",
                    last_computedPresentationTimestampUs, computedPresentationTimestampUs, last_mpu_sequence_number, current_mpu_sequence_number));

            IsSoftFlushingFromAVPtsDiscontinuity = true;
            VideoMfuByteBufferQueue.clear();
            AudioMfuByteBufferQueue.clear();
            MapAudioMfuPresentationTimestampUsAnchorSystemTimeUs.clear();
            MapVideoMfuPresentationTimestampUsAnchorSystemTimeUs.clear();

            VideoMediaCodec.flush();
            AudioMediaCodec.flush();
            Thread.sleep(1000);

            //DecoderHandlerThread.sync.flush();

            DecoderHandlerThread.sync.setPlaybackParams(new PlaybackParams().setSpeed(1.0f));

            MmtPacketIdContext.video_packet_statistics.first_queueInputBuffer_mfu_essenceRenderUs = null;
            MmtPacketIdContext.video_packet_statistics.first_mfu_presentation_time = null;
            MmtPacketIdContext.video_packet_statistics.last_mfu_presentation_time = null;
            MmtPacketIdContext.video_packet_statistics.first_mfu_essenceRenderUs = null;
            MmtPacketIdContext.video_packet_statistics.first_mfu_forced_from_keyframe = false;
            //do NOT reset this value when we are flushing and using clock sync?
            MmtPacketIdContext.video_packet_statistics.first_queueInputBuffer_mfu_nanoTime = null;
            MmtPacketIdContext.video_packet_statistics.first_mfu_nanoTime = null;
            MmtPacketIdContext.video_packet_statistics.last_mpu_sequence_number_input_buffer = null;
            MmtPacketIdContext.video_packet_statistics.last_mpu_sequence_number_inputBufferQueued = null;

            MmtPacketIdContext.audio_packet_statistics.first_queueInputBuffer_mfu_essenceRenderUs = null;
            MmtPacketIdContext.audio_packet_statistics.first_mfu_presentation_time = null;
            MmtPacketIdContext.audio_packet_statistics.last_mfu_presentation_time = null;
            MmtPacketIdContext.audio_packet_statistics.first_mfu_essenceRenderUs = null;
            //do NOT reset this value when we are flushing and using clock sync?
            MmtPacketIdContext.audio_packet_statistics.first_queueInputBuffer_mfu_nanoTime = null;
            MmtPacketIdContext.audio_packet_statistics.first_mfu_nanoTime = null;
            MmtPacketIdContext.audio_packet_statistics.last_mpu_sequence_number_input_buffer = null;
            MmtPacketIdContext.audio_packet_statistics.last_mpu_sequence_number_inputBufferQueued = null;

            //VideoMediaCodecInputBufferQueue.clear();

            //AudioMediaCodecInputBufferQueue.clear();

            VideoMfuByteBufferQueue.clear();
            AudioMfuByteBufferQueue.clear();

            IsSoftFlushingFromAVPtsDiscontinuity = false;
            VideoMediaCodec.start();
            AudioMediaCodec.start();

            //todo - mark any onOutputBufferAvailable as invalidated until we restart the codec
            Log.d("MediaCodecInputBuffer", String.format("softFlushAVPtsDiscontinuity: Flushing complete"));

        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}