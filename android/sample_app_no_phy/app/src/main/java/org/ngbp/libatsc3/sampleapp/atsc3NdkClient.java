package org.ngbp.libatsc3.sampleapp;

import android.content.res.AssetManager;
import android.util.Log;

import org.ngbp.libatsc3.android.PackageExtractEnvelopeMetadataAndPayload;
import org.ngbp.libatsc3.media.ATSC3PlayerFlags;
import org.ngbp.libatsc3.media.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.media.sync.mmt.MmtPacketIdContext;
import org.ngbp.libatsc3.media.sync.mmt.MpuMetadata_HEVC_NAL_Payload;
import org.ngbp.libatsc3.phy.BwPhyStatistics;
import org.ngbp.libatsc3.phy.RfPhyStatistics;

import java.io.File;
import java.nio.ByteBuffer;

/**
 * Created by cafrii on 17. 9. 18.
 */

public class atsc3NdkClient {

    final static String TAG ="intf";

    MainActivity mActivity;

    atsc3NdkClient(MainActivity parent) {
        mActivity = parent;
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

    public native int ApiInit(atsc3NdkClient intf);
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
    
    //libatsc3 methods here...
    public native int atsc3_pcap_open_for_replay(String filename);
    public native int atsc3_pcap_open_for_replay_from_assetManager(String filename, AssetManager assetManager);
    public native int atsc3_pcap_thread_run();
    public native int atsc3_pcap_thread_stop();

    public native int atsc3_slt_selectService(int service_id);                  //select either single MMT or ROUTE service
    public native int atsc3_slt_alc_select_additional_service(int service_id);  //listen on additional ALC service(s)
    public native int atsc3_slt_alc_clear_additional_service_selections();      //clear ALC additional service listeners

    //NOTE: these methods may return an empty collection if the MBMS carousel or S-TSID has not been received after selecting service/additional_service
    public native String[] atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(int service_id, String to_match_content_type);
    public native String[] atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id(int service_id);

    //ndk to jni methods for libatsc3 context event notifications
    //jjustman-2019-10-20: yes, i know its not "thread safe" but its step 1...

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

    int atsc3_onExtractedSampleDuration(int packet_id, int mpu_sequence_number, int extracted_sample_duration_us) {
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

    int atsc3_updateRfBwStats(long total_pkts, long total_bytes, int total_lmts) {
        mActivity.pushBwPhyStatistics(new BwPhyStatistics(total_pkts, total_bytes, total_lmts));
        return 0;
    }

    File getCacheDir() {
        return mActivity.jni_getCacheDir();
    }

    int atsc3_lls_sls_alc_on_route_mpd_patched(int service_id) {
        mActivity.routeDash_force_player_reload_mpd(service_id);
        return 0;
    }

    static {
        //jjustman:2019-11-24: cross reference and circular dependency with NXP_Tuner_Lib and SL API methods
        //System.loadLibrary("NXP_Tuner_Lib");
        //System.loadLibrary("SiTune_Tuner_Libs");

        System.loadLibrary("atsc3NdkClient");
    }


}
