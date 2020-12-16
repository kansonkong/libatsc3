package org.ngbp.libatsc3.middleware;

import android.util.Log;

import org.ngbp.libatsc3.middleware.android.ATSC3PlayerFlags;
import org.ngbp.libatsc3.middleware.android.application.interfaces.IAtsc3NdkMediaMMTBridgeCallbacks;
import org.ngbp.libatsc3.middleware.android.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.mmt.MmtPacketIdContext;
import org.ngbp.libatsc3.middleware.android.mmt.MpuMetadata_HEVC_NAL_Payload;

import java.nio.ByteBuffer;

/*
Atsc3NdkMediaMMTBridge: for ExoPlayer plugin support
 */

public class Atsc3NdkMediaMMTBridge extends Atsc3NdkMediaMMTBridgeStaticJniLoader
{
    final static String TAG ="intf";

    IAtsc3NdkMediaMMTBridgeCallbacks mActivity;
    public static final Boolean MMT_DISCARD_CORRUPT_FRAMES = true;

    //native jni methods
    @Override
    public native int init();
    //free NDK/JNI bound AttachedThread, pseduo finalize()?
    @Override
    public native void release();

    public native int atsc3_process_mmtp_udp_packet(ByteBuffer byteBuffer, int length);

    public Atsc3NdkMediaMMTBridge(IAtsc3NdkMediaMMTBridgeCallbacks iAtsc3NdkMediaMMTBridgeCallbacks) {
        mActivity = iAtsc3NdkMediaMMTBridgeCallbacks;
        init();
    }

    int onLogMsg(String msg) {
        Log.d(TAG, msg);
        mActivity.showMsgFromNative(msg+"\n");
        return 0;
    }

    int atsc3_onInitHEVC_NAL_Packet(int packet_id, int mpu_sequence_number, ByteBuffer byteBuffer, int length) {
        // Log.d("AT3DrvIntf", "onInitHEVC_NAL_Packet, isVideo:"+isVideo+", mpu_sequence_number: "+mpu_sequence_number+", byteBuffer: "+byteBuffer+", length: "+length);

        MpuMetadata_HEVC_NAL_Payload mpuMetadata_HEVC_NAL_Payload = new MpuMetadata_HEVC_NAL_Payload(packet_id, mpu_sequence_number, byteBuffer, length);

        mActivity.pushMpuMetadata_HEVC_NAL_Payload(mpuMetadata_HEVC_NAL_Payload);

        return 0;
    }

    //jjustman-2020-08-10 - TODO - move these out of "global global" scope
    public int atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(int video_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, long mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
        MmtPacketIdContext.video_packet_id = video_packet_id;
        MmtPacketIdContext.video_packet_signalling_information.mpu_sequence_number = mpu_sequence_number;
        MmtPacketIdContext.video_packet_signalling_information.mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        MmtPacketIdContext.video_packet_signalling_information.mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        MmtPacketIdContext.video_packet_signalling_information.mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;

        return 0;
    }

    public int atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, long mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
        MmtPacketIdContext.audio_packet_id = audio_packet_id;
        MmtPacketIdContext.audio_packet_signalling_information.mpu_sequence_number = mpu_sequence_number;
        MmtPacketIdContext.audio_packet_signalling_information.mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        MmtPacketIdContext.audio_packet_signalling_information.mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        MmtPacketIdContext.audio_packet_signalling_information.mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;

        return 0;
    }

    public int atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(int stpp_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, long mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
        MmtPacketIdContext.stpp_packet_id = stpp_packet_id;
        MmtPacketIdContext.stpp_packet_signalling_information.mpu_sequence_number = mpu_sequence_number;
        MmtPacketIdContext.stpp_packet_signalling_information.mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        MmtPacketIdContext.stpp_packet_signalling_information.mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        MmtPacketIdContext.stpp_packet_signalling_information.mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;

        return 0;
    }

    int atsc3_onExtractedSampleDuration(int packet_id, int mpu_sequence_number, int extracted_sample_duration_us) {
        //jjustman-2020-08-19 - audio duration work-around for ac-4
        if (MmtPacketIdContext.audio_packet_id == packet_id && extracted_sample_duration_us <= 0) {
            extracted_sample_duration_us = MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us;
            MmtPacketIdContext.audio_packet_statistics.extracted_sample_duration_us = extracted_sample_duration_us;
            return 0;

        }
        if(extracted_sample_duration_us <= 0) {
            Log.e("atsc3_onExtractedSampleDuration", String.format("extracted sample duration for packet_id: %d, mpu_sequence_number: %d, value %d is invalid", packet_id, mpu_sequence_number, extracted_sample_duration_us));
            return 0;
        }

        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            if (MmtPacketIdContext.video_packet_id == packet_id) {
                MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us = extracted_sample_duration_us;
            } else if (MmtPacketIdContext.audio_packet_id == packet_id) {
                MmtPacketIdContext.audio_packet_statistics.extracted_sample_duration_us = extracted_sample_duration_us;
            } else if (MmtPacketIdContext.stpp_packet_id == packet_id) {
                MmtPacketIdContext.stpp_packet_statistics.extracted_sample_duration_us = extracted_sample_duration_us;
            }
        }
        return 0;
    }

    int atsc3_setVideoWidthHeightFromTrak(int width, int height) {
        MmtPacketIdContext.video_packet_statistics.width = width;
        MmtPacketIdContext.video_packet_statistics.height = height;

        return 0;
    }

    int atsc3_onMfuPacket(int packet_id, int mpu_sequence_number, int sample_number, ByteBuffer byteBuffer, int length, long presentationTimeUs, int mfu_fragment_count_expected) {

        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            if(length > 0) {
                MfuByteBufferFragment mfuByteBufferFragment = new MfuByteBufferFragment(packet_id, mpu_sequence_number, sample_number, byteBuffer, length, presentationTimeUs, mfu_fragment_count_expected, mfu_fragment_count_expected);
                mActivity.pushMfuByteBufferFragment(mfuByteBufferFragment);
            } else {
                Log.e("atsc3_onMfuPacket", String.format("packetId: %d, mpu_sequence_number: %d, sample_number: %d has no length!",
                        packet_id,
                        mpu_sequence_number,
                        sample_number));
            }
        } else {
            //discard...
        }
        return 0;
    }

    int atsc3_onMfuPacketCorrupt(int packet_id, int mpu_sequence_number, int sample_number, ByteBuffer byteBuffer, int length, long presentationTimeUs, int mfu_fragment_count_expected, int mfu_fragment_count_rebuilt) {
        if(MMT_DISCARD_CORRUPT_FRAMES) {
            return -1;
        }

        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            if(length > 0) {
                MfuByteBufferFragment mfuByteBufferFragment = new MfuByteBufferFragment(packet_id, mpu_sequence_number, sample_number, byteBuffer, length, presentationTimeUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

                mActivity.pushMfuByteBufferFragment(mfuByteBufferFragment);
            } else {
                Log.e("atsc3_onMfuPacketCorrupt", String.format("packetId: %d, mpu_sequence_number: %d, sample_number: %d has no length!",
                        packet_id,
                        mpu_sequence_number,
                        sample_number));
            }
        } else {
            //discard...
        }
        return 0;
    }

    int atsc3_onMfuPacketCorruptMmthSampleHeader(int packet_id, int mpu_sequence_number, int sample_number, ByteBuffer byteBuffer, int length, long presentationTimeUs,  int mfu_fragment_count_expected, int mfu_fragment_count_rebuilt) {
        if(MMT_DISCARD_CORRUPT_FRAMES) {
            return -1;
        }
        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            if(length > 0 ) {
                MfuByteBufferFragment mfuByteBufferFragment = new MfuByteBufferFragment(packet_id, mpu_sequence_number, sample_number, byteBuffer, length, presentationTimeUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

                mActivity.pushMfuByteBufferFragment(mfuByteBufferFragment);
            } else {
                Log.e("atsc3_onMfuPacketCorruptMmthSampleHeader", String.format("packetId: %d, mpu_sequence_number: %d, sample_number: %d has no length!",
                        packet_id,
                        mpu_sequence_number,
                        sample_number));
            }
        } else {
            //discard...
        }
        return 0;
    }

    public int atsc3_onMfuSampleMissing(int packet_id, int mpu_sequence_number, int sample_number) {
        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            if (MmtPacketIdContext.video_packet_id == packet_id) {
                MmtPacketIdContext.video_packet_statistics.missing_mfu_samples_count++;
            } else if (MmtPacketIdContext.audio_packet_id == packet_id) {
                MmtPacketIdContext.audio_packet_statistics.missing_mfu_samples_count++;
            } else {
                // ...
            }
        }
        return 0;
    }
}

