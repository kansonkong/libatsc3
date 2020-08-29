package org.ngbp.libatsc3.middleware;

import android.util.Log;

import org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload;
import org.ngbp.libatsc3.middleware.android.ATSC3PlayerFlags;
import org.ngbp.libatsc3.middleware.android.application.interfaces.IAtsc3NdkApplicationBridgeCallbacks;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MmtPacketIdContext;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MpuMetadata_HEVC_NAL_Payload;

import java.io.File;
import java.nio.ByteBuffer;

/*
 */

public class Atsc3NdkApplicationBridge extends Atsc3BridgeNdkStaticJniLoader
{
    final static String TAG ="intf";

    IAtsc3NdkApplicationBridgeCallbacks mActivity;

    //native jni methods
    @Override
    public native int init();

    //service invocation change methods
    public native int atsc3_slt_selectService(int service_id);                  //select either single MMT or ROUTE service
    public native int atsc3_slt_alc_select_additional_service(int service_id);  //listen on additional ALC service(s)
    public native int atsc3_slt_alc_clear_additional_service_selections();      //clear ALC additional service listeners

    //NOTE: these methods may return an empty collection if the MBMS carousel or S-TSID has not been received after selecting service/additional_service
    public native String[] atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(int service_id, String to_match_content_type);
    public native String[] atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id(int service_id);

    public Atsc3NdkApplicationBridge(IAtsc3NdkApplicationBridgeCallbacks iAtsc3NdkApplicationBridgeCallbacks) {
        mActivity = iAtsc3NdkApplicationBridgeCallbacks;
        init();
    }

    int onLogMsg(String msg) {
        Log.d(TAG, msg);
        mActivity.showMsgFromNative(msg+"\n");
        return 0;
    }

    int atsc3_onSlsTablePresent(String sls_payload_xml) {

        mActivity.onSlsTablePresent(sls_payload_xml);
        return 0;
    }

    int atsc3_onAeatTablePresent(String sls_payload_xml) {

        mActivity.onAeatTablePresent(sls_payload_xml);
        return 0;
    }

    int atsc3_onMfuPacket(int packet_id, int mpu_sequence_number, int sample_number, ByteBuffer byteBuffer, int length, long presentationTimeUs, int mfu_fragment_count_expected) {
        //working - 2019-10-03 - Log.d("AT3DrvIntf", "onMfuPacket, isVideo:"+isVideo+", length: "+length+ ", pts: "+presentationTimeUs);

        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            MfuByteBufferFragment mfuByteBufferFragment = new MfuByteBufferFragment(packet_id, mpu_sequence_number, sample_number, byteBuffer, length, presentationTimeUs, mfu_fragment_count_expected, mfu_fragment_count_expected);

            mActivity.pushMfuByteBufferFragment(mfuByteBufferFragment);
        } else {
            //discard...
        }
        return 0;
    }

    int atsc3_onMfuPacketCorrupt(int packet_id, int mpu_sequence_number, int sample_number, ByteBuffer byteBuffer, int length, long presentationTimeUs, int mfu_fragment_count_expected, int mfu_fragment_count_rebuilt) {
        //Log.d("AT3DrvIntf", "onMfuPacket, isVideo:"+isVideo+", length: "+length+ ", pts: "+presentationTimeUs);

        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            MfuByteBufferFragment mfuByteBufferFragment = new MfuByteBufferFragment(packet_id, mpu_sequence_number, sample_number, byteBuffer, length, presentationTimeUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

            mActivity.pushMfuByteBufferFragment(mfuByteBufferFragment);
        } else {
            //discard...
        }
        return 0;
    }

    int atsc3_onMfuPacketCorruptMmthSampleHeader(int packet_id, int mpu_sequence_number, int sample_number, ByteBuffer byteBuffer, int length, long presentationTimeUs,  int mfu_fragment_count_expected, int mfu_fragment_count_rebuilt) {
        //working - 2019-10-03 - Log.d("AT3DrvIntf", "onMfuPacket, isVideo:"+isVideo+", length: "+length+ ", pts: "+presentationTimeUs);

        if(ATSC3PlayerFlags.ATSC3PlayerStartPlayback) {
            MfuByteBufferFragment mfuByteBufferFragment = new MfuByteBufferFragment(packet_id, mpu_sequence_number, sample_number, byteBuffer, length, presentationTimeUs, mfu_fragment_count_expected, mfu_fragment_count_rebuilt);

            mActivity.pushMfuByteBufferFragment(mfuByteBufferFragment);
        } else {
            //discard...
        }
        return 0;
    }

    int atsc3_onInitHEVC_NAL_Packet(int packet_id, int mpu_sequence_number, ByteBuffer byteBuffer, int length) {
       // Log.d("AT3DrvIntf", "onInitHEVC_NAL_Packet, isVideo:"+isVideo+", mpu_sequence_number: "+mpu_sequence_number+", byteBuffer: "+byteBuffer+", length: "+length);

        MpuMetadata_HEVC_NAL_Payload mpuMetadata_HEVC_NAL_Payload = new MpuMetadata_HEVC_NAL_Payload(packet_id, mpu_sequence_number, byteBuffer, length);

        mActivity.pushMpuMetadata_HEVC_NAL_Payload(mpuMetadata_HEVC_NAL_Payload);

        return 0;
    }

    int atsc3_on_alc_object_status_message(String alc_object_status_message) {
        mActivity.onAlcObjectStatusMessage(alc_object_status_message);
        return 0;
    }

    int atsc3_lls_sls_alc_on_package_extract_completed(PackageExtractEnvelopeMetadataAndPayload packageExtractEnvelopeMetadataAndPayload) {
        mActivity.onPackageExtractCompleted(packageExtractEnvelopeMetadataAndPayload);
        return 0;
    }

    //ndk to jni methods for libatsc3 context event notifications
    //jjustman-2019-10-20: yes, i know its not "thread safe" but its step 1...

    //jjustman-2020-08-10 - TODO - move these out of "global global" scope
    public int atsc3_signallingContext_notify_video_packet_id_and_mpu_timestamp_descriptor(int video_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
        MmtPacketIdContext.video_packet_id = video_packet_id;
        MmtPacketIdContext.video_packet_signalling_information.mpu_sequence_number = mpu_sequence_number;
        MmtPacketIdContext.video_packet_signalling_information.mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        MmtPacketIdContext.video_packet_signalling_information.mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        MmtPacketIdContext.video_packet_signalling_information.mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;

        return 0;
    }

    public int atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
        MmtPacketIdContext.audio_packet_id = audio_packet_id;
        MmtPacketIdContext.audio_packet_signalling_information.mpu_sequence_number = mpu_sequence_number;
        MmtPacketIdContext.audio_packet_signalling_information.mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        MmtPacketIdContext.audio_packet_signalling_information.mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        MmtPacketIdContext.audio_packet_signalling_information.mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;

        return 0;
    }

    public int atsc3_signallingContext_notify_stpp_packet_id_and_mpu_timestamp_descriptor(int stpp_packet_id, int mpu_sequence_number, long mpu_presentation_time_ntp64, int mpu_presentation_time_seconds, int mpu_presentation_time_microseconds) {
        MmtPacketIdContext.stpp_packet_id = stpp_packet_id;
        MmtPacketIdContext.stpp_packet_signalling_information.mpu_sequence_number = mpu_sequence_number;
        MmtPacketIdContext.stpp_packet_signalling_information.mpu_presentation_time_ntp64 = mpu_presentation_time_ntp64;
        MmtPacketIdContext.stpp_packet_signalling_information.mpu_presentation_time_seconds = mpu_presentation_time_seconds;
        MmtPacketIdContext.stpp_packet_signalling_information.mpu_presentation_time_microseconds = mpu_presentation_time_microseconds;

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

    File getCacheDir() {
        return mActivity.jni_getCacheDir();
    }

    int atsc3_lls_sls_alc_on_route_mpd_patched(int service_id) {
        mActivity.routeDash_force_player_reload_mpd(service_id);
        return 0;
    }

    int atsc3_onSlsHeldEmissionPresent(int serviceId, String heldPayloadXML) {
        mActivity.onSlsHeldEmissionPresent(serviceId, heldPayloadXML);
        return 0;
    }
}

