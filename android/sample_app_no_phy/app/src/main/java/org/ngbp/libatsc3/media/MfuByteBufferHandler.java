package org.ngbp.libatsc3.media;

import android.util.Log;

import org.ngbp.libatsc3.middleware.android.ATSC3PlayerFlags;
import org.ngbp.libatsc3.middleware.android.DebuggingFlags;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MmtPacketIdContext;

public class MfuByteBufferHandler {

    public static void PushMfuByteBufferFragment(MfuByteBufferFragment mfuByteBufferFragment) {
        if(!ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            mfuByteBufferFragment.myByteBuffer = null;
            return;
        }

        //jjustman-2020-08-19 - hack-ish workaround for ac-4 and mmt_atsc3_message signalling information w/ sample duration (or avoiding parsing the trun box)
        if(MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us != 0 || MmtPacketIdContext.audio_packet_statistics.extracted_sample_duration_us == 0) {
            Log.d("PushMfuByteBufferFragment:INFO", String.format(" packet_id: %d, mpu_sequence_number: %d, setting audio_packet_statistics.extracted_sample_duration_us to follow video: %d",
                    mfuByteBufferFragment.packet_id, mfuByteBufferFragment.mpu_sequence_number, MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us));
            MmtPacketIdContext.audio_packet_statistics.extracted_sample_duration_us = MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us;
        } else if(MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us == 0 || MmtPacketIdContext.audio_packet_statistics.extracted_sample_duration_us == 0) {
            Log.d("PushMfuByteBufferFragment:WARN", String.format(" packet_id: %d, mpu_sequence_number: %d, video.duration_us: %d, audio.duration_us: %d, missing extracted_sample_duration",
                    mfuByteBufferFragment.packet_id, mfuByteBufferFragment.mpu_sequence_number,
                    MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us,
                    MmtPacketIdContext.audio_packet_statistics.extracted_sample_duration_us));
        }

        if(MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsSoftFlushingFromAVPtsDiscontinuity || MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsHardCodecFlushingFromAVPtsDiscontinuity || MediaCodecInputBufferMfuByteBufferFragmentWorkerRunnable.IsResettingCodecFromDiscontinuity) {
            mfuByteBufferFragment.unreferenceByteBuffer();
            return;
        }

        if(MmtPacketIdContext.video_packet_id == mfuByteBufferFragment.packet_id) {
            // if(mfuByteBufferFragment.sample_number == 1 || firstMfuBufferVideoKeyframeSent) {
            //normal flow...
            ATSC3PlayerMMTFragments.mfuBufferQueueVideo.add(mfuByteBufferFragment);

            if(mfuByteBufferFragment.sample_number == 1) {
                if(!ATSC3PlayerFlags.FirstMfuBufferVideoKeyframeSent) {
                    Log.d("pushMfuByteBufferFragment", String.format("V: pushing FIRST: queueSize: %d, sampleNumber: %d, size: %d, mpuPresentationTimeUs: %d",
                            ATSC3PlayerMMTFragments.mfuBufferQueueVideo.size(),
                            mfuByteBufferFragment.sample_number,
                            mfuByteBufferFragment.bytebuffer_length,
                            mfuByteBufferFragment.mpu_presentation_time_uS_from_SI));
                }
                ATSC3PlayerFlags.FirstMfuBufferVideoKeyframeSent = true;

                MmtPacketIdContext.video_packet_statistics.video_mfu_i_frame_count++;
            } else {
                MmtPacketIdContext.video_packet_statistics.video_mfu_pb_frame_count++;
            }

            if(mfuByteBufferFragment.mfu_fragment_count_expected == mfuByteBufferFragment.mfu_fragment_count_rebuilt) {
                MmtPacketIdContext.video_packet_statistics.complete_mfu_samples_count++;
            } else {
                MmtPacketIdContext.video_packet_statistics.corrupt_mfu_samples_count++;
            }

            //TODO: jjustman-2019-10-23: manual missing statistics, context callback doesn't compute this properly yet.
            if(MmtPacketIdContext.video_packet_statistics.last_mpu_sequence_number != mfuByteBufferFragment.mpu_sequence_number) {
                MmtPacketIdContext.video_packet_statistics.total_mpu_count++;
                //compute trailing mfu's missing

                //compute leading mfu's missing
                if(mfuByteBufferFragment.sample_number > 1) {
                    MmtPacketIdContext.video_packet_statistics.missing_mfu_samples_count += (mfuByteBufferFragment.sample_number - 1);
                }
            } else {
                MmtPacketIdContext.video_packet_statistics.missing_mfu_samples_count += mfuByteBufferFragment.sample_number - (1 + MmtPacketIdContext.video_packet_statistics.last_mfu_sample_number);
            }

            MmtPacketIdContext.video_packet_statistics.last_mfu_sample_number = mfuByteBufferFragment.sample_number;
            MmtPacketIdContext.video_packet_statistics.last_mpu_sequence_number = mfuByteBufferFragment.mpu_sequence_number;

            //todo - build mpu stats from tail of mfuBufferQueueVideo

            MmtPacketIdContext.video_packet_statistics.total_mfu_samples_count++;

            if((MmtPacketIdContext.video_packet_statistics.total_mfu_samples_count % DebuggingFlags.DEBUG_LOG_MFU_STATS_FRAME_COUNT) == 0) {
                Log.d("pushMfuByteBufferFragment",
                        String.format("V: appending MFU: mpu_sequence_number: %d, sampleNumber: %d, size: %d, mpuPresentationTimeUs: %d, queueSize: %d",
                                mfuByteBufferFragment.mpu_sequence_number,
                                mfuByteBufferFragment.sample_number,
                                mfuByteBufferFragment.bytebuffer_length,
                                mfuByteBufferFragment.get_safe_mfu_presentation_time_uS_computed(),
                                ATSC3PlayerMMTFragments.mfuBufferQueueVideo.size()));
            }
//            } else {
//                //discard
////                Log.d("pushMfuByteBufferFragment", "discarding video: firstMfuBufferVideoKeyframeSent: "+firstMfuBufferVideoKeyframeSent
////                        +", sampleNumber: "+mfuByteBufferFragment.sample_number
////                        +", size: "+mfuByteBufferFragment.bytebuffer_length
////                        +", presentationTimeUs: "+mfuByteBufferFragment.mpu_presentation_time_us_mpu);
//                mfuByteBufferFragment.myByteBuffer = null;
//            }

        } else if(MmtPacketIdContext.audio_packet_id == mfuByteBufferFragment.packet_id) {
//            if(!firstMfuBufferVideoKeyframeSent) {
//                //discard
//                return;
//            }

            ATSC3PlayerMMTFragments.mfuBufferQueueAudio.add(mfuByteBufferFragment);

            if(mfuByteBufferFragment.mfu_fragment_count_expected == mfuByteBufferFragment.mfu_fragment_count_rebuilt) {
                MmtPacketIdContext.audio_packet_statistics.complete_mfu_samples_count++;
            } else {
                MmtPacketIdContext.audio_packet_statistics.corrupt_mfu_samples_count++;
            }

            //todo - build mpu stats from tail of mfuBufferQueueVideo

            MmtPacketIdContext.audio_packet_statistics.total_mfu_samples_count++;

            if(MmtPacketIdContext.audio_packet_statistics.last_mpu_sequence_number != mfuByteBufferFragment.mpu_sequence_number) {
                MmtPacketIdContext.audio_packet_statistics.total_mpu_count++;
                //compute trailing mfu's missing

                //compute leading mfu's missing
                if(mfuByteBufferFragment.sample_number > 1) {
                    MmtPacketIdContext.audio_packet_statistics.missing_mfu_samples_count += (mfuByteBufferFragment.sample_number - 1);
                }
            } else {
                MmtPacketIdContext.audio_packet_statistics.missing_mfu_samples_count += mfuByteBufferFragment.sample_number - (1 + MmtPacketIdContext.audio_packet_statistics.last_mfu_sample_number);
            }

            MmtPacketIdContext.audio_packet_statistics.last_mfu_sample_number = mfuByteBufferFragment.sample_number;
            MmtPacketIdContext.audio_packet_statistics.last_mpu_sequence_number = mfuByteBufferFragment.mpu_sequence_number;


            if((MmtPacketIdContext.audio_packet_statistics.total_mfu_samples_count % DebuggingFlags.DEBUG_LOG_MFU_STATS_FRAME_COUNT) == 0) {

                Log.d("pushMfuByteBufferFragment", String.format("A: appending MFU: mpu_sequence_number: %d, sampleNumber: %d, size: %d, mpuPresentationTimeUs: %d, queueSize: %d",
                        mfuByteBufferFragment.mpu_sequence_number,
                        mfuByteBufferFragment.sample_number,
                        mfuByteBufferFragment.bytebuffer_length,
                        mfuByteBufferFragment.get_safe_mfu_presentation_time_uS_computed(),
                        ATSC3PlayerMMTFragments.mfuBufferQueueVideo.size()));
            }
        } else if(MmtPacketIdContext.stpp_packet_id == mfuByteBufferFragment.packet_id) {
            if(!ATSC3PlayerFlags.FirstMfuBufferVideoKeyframeSent) {
                return;
            }

            ATSC3PlayerMMTFragments.mfuBufferQueueStpp.add(mfuByteBufferFragment);
            MmtPacketIdContext.stpp_packet_statistics.total_mfu_samples_count++;
        }
    }

    public static void clearQueues() {

        ATSC3PlayerMMTFragments.mfuBufferQueueVideo.clear();
        ATSC3PlayerMMTFragments.mfuBufferQueueAudio.clear();
    }
}
