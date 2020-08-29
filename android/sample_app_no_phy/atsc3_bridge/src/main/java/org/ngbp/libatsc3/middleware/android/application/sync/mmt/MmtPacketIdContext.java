package org.ngbp.libatsc3.middleware.android.application.sync.mmt;

import android.util.Log;

import java.util.HashMap;
import java.util.Map;

public class MmtPacketIdContext {
    public static long libatsc_app_start_time_ms = 0;

    //jjustman-2019-10-20 - hack-ish, should really be using lls_sls_mmt_monitor pattern here
    public static int video_packet_id = -1;
    public static MmtSignallingContext video_packet_signalling_information;
    public static MmtMfuStatistics video_packet_statistics;

    public static int audio_packet_id = -1;
    public static MmtSignallingContext audio_packet_signalling_information;
    public static MmtMfuStatistics audio_packet_statistics;

    public static int stpp_packet_id = -1;
    public static MmtSignallingContext stpp_packet_signalling_information;
    public static MmtMfuStatistics stpp_packet_statistics;
    public static int stpp_last_mpu = -1;

    public static MmtCodecContent mmt_codec_context;

    public static void Initialize() {
        MmtPacketIdContext.libatsc_app_start_time_ms = System.currentTimeMillis();

        MmtPacketIdContext.video_packet_signalling_information = new MmtSignallingContext();
        MmtPacketIdContext.audio_packet_signalling_information = new MmtSignallingContext();
        MmtPacketIdContext.stpp_packet_signalling_information = new MmtSignallingContext();

        MmtPacketIdContext.video_packet_statistics = new MmtMfuStatistics("video");
        MmtPacketIdContext.audio_packet_statistics = new MmtMfuStatistics("audio");
        MmtPacketIdContext.stpp_packet_statistics = new MmtMfuStatistics("stpp");
    };

    //TODO: jjustman-2019-10-29: refactor these methods into a more general ATSC 3.0 RF/BW stats
    //class rather than MMT specific-class
    public static long computeLast1sBwKbitTimestamp = 0;
    public static long computeLast1sBwKbitStartValue = 0;

    public static float computeLast1sBwBitsSec(long total_bytes) {
        long currentTimeMs = System.currentTimeMillis();
        float avgValue = 0;
        if(computeLast1sBwKbitTimestamp == 0) {
            computeLast1sBwKbitTimestamp = currentTimeMs;
            computeLast1sBwKbitStartValue = total_bytes;
        } else {
            avgValue = (float)(8*(total_bytes - computeLast1sBwKbitStartValue)) / ((currentTimeMs - computeLast1sBwKbitTimestamp)/1000.0f);
            if(currentTimeMs - computeLast1sBwKbitTimestamp >= 1000) {
                computeLast1sBwKbitTimestamp = currentTimeMs;
                computeLast1sBwKbitStartValue = total_bytes;
            }
        }
        return avgValue;
    }

    public static long computeLast1sBwPPSTimestamp = 0;
    public static long computeLast1sBwPPSStartValue = 0;

    public static float computeLast1sBwPPS(long total_pkts) {
        long currentTimeMs = System.currentTimeMillis();
        float avgValue = 0;
        if(computeLast1sBwPPSTimestamp == 0) {
            computeLast1sBwPPSTimestamp = currentTimeMs;
            computeLast1sBwPPSStartValue = total_pkts;
        } else {
            avgValue = (float)(total_pkts - computeLast1sBwPPSStartValue) / ((currentTimeMs - computeLast1sBwPPSTimestamp)/1000.0f);
            if(currentTimeMs - computeLast1sBwPPSTimestamp >= 1000) {
                computeLast1sBwPPSTimestamp = currentTimeMs;
                computeLast1sBwPPSStartValue = total_pkts;
            }
        }
        return avgValue;
    }

    public static class MmtSignallingContext {
        public int mpu_sequence_number;
        public long mpu_presentation_time_ntp64;
        public long mpu_presentation_time_seconds;
        public long mpu_presentation_time_microseconds;

    }

    /*
        Per monitored service flow...
     */
    public static class MmtMfuStatistics {

        public static final int FALLBACK_WIDTH = 1920;
        public static final int FALLBACK_HEIGHT = 1080;

        public String type;
        public Integer last_mpu_sequence_number_input_buffer;
        public Integer last_mpu_sequence_number_inputBufferQueued;

        public MmtMfuStatistics(String type) {
            this.type = type;
        }

        public Long last_computedPresentationTimestampUs = null;
        public int extracted_sample_duration_us = 0;

        public int last_mpu_sequence_number = 0;
        public int last_mfu_sample_number = 0;
        public Map<Integer, Long> last_mfu_release_microseconds = new HashMap<>();

        public int complete_mfu_samples_count;
        public int corrupt_mfu_samples_count;
        public int missing_mfu_samples_count;
        public int total_mfu_samples_count;

        public int decoder_buffer_queue_input_count;
        public int decoder_buffer_input_mfu_underrun_count;

        public int video_mfu_i_frame_count;
        public int video_mfu_pb_frame_count;

        public int actual_mfu_du_bytes_rx;
        public int computed_mfu_du_bytes_rx;

        public int complete_mpu_count;
        public int corrupt_mpu_count;
        public int missing_mpu_count;
        public int total_mpu_count;

        public Long first_mfu_nanoTime = null;
        public Long first_mfu_presentation_time = null;

        public Long first_mfu_essenceRenderUs = null;
        public Boolean first_mfu_presentation_ready_for_emission = false;
        public Boolean first_mfu_forced_from_keyframe = false;

        public int  last_essence_mpu_sequence_number = 0;
        public long last_essence_mpu_presentation_time_us = 0;
        public Long last_mfu_presentation_time = null;


        //computed mfu_presentation_timestamp
        public long last_essence_mfu_presentation_timestamp_us = 0;

        public long last_essence_mfu_system_time_ns;
        public Boolean last_essence_mfu_is_from_descriptor = false;

        public long last_essence_pts_offset;
        public int width;
        public int height;
        public long last_output_buffer_presentationTimeUs;
        public Long first_queueInputBuffer_mfu_essenceRenderUs;
        public Long first_queueInputBuffer_mfu_nanoTime;

        public void set_last_mfu_presentation_timestamp_us_from_descriptor(int mpu_sequence_number, long mpu_presentation_time_us, long essenceRenderTs) {
            if(first_mfu_nanoTime == null) {
                first_mfu_nanoTime = Long.valueOf(System.nanoTime());
            }
            if(first_mfu_essenceRenderUs == null) {
                first_mfu_essenceRenderUs = Long.valueOf(essenceRenderTs);
            }

            last_essence_mpu_sequence_number = mpu_sequence_number;
            last_essence_mpu_presentation_time_us  = mpu_presentation_time_us;
            last_essence_mfu_presentation_timestamp_us = essenceRenderTs;
            last_essence_mfu_is_from_descriptor = true;
            last_essence_mfu_system_time_ns = System.nanoTime();
        }

        public boolean has_last_mfu_presentation_timestamp() {
            return last_essence_mfu_presentation_timestamp_us != 0;
        }

        //gop length may be unknown, we can only rely on wall-clock free-run from an anchor point
        public long compute_new_mfu_presentation_timestamp_us_from_system_delta(int mpu_sequence_number, long essencePtsOffset) {
            long computed_new_mfu_presentation_timestamp_us = 0;
            long system_time_ns_delta = System.nanoTime() - last_essence_mfu_system_time_ns;

            if(last_essence_mfu_is_from_descriptor && last_essence_mpu_sequence_number +1 == mpu_sequence_number) {
                computed_new_mfu_presentation_timestamp_us = last_essence_mfu_presentation_timestamp_us + (system_time_ns_delta/1000);
                last_essence_mpu_presentation_time_us = computed_new_mfu_presentation_timestamp_us;
                //only update our last_essence_mpu_sequence_number if we are using mpu_presentation_timestamp_us as an anchor
                last_essence_mpu_sequence_number = mpu_sequence_number;
            } else if(!last_essence_mfu_is_from_descriptor && last_essence_mpu_sequence_number == mpu_sequence_number){
                //use our last "interpolated last_essence_mpu_presentation_time_us"
                computed_new_mfu_presentation_timestamp_us = last_essence_mpu_presentation_time_us + (system_time_ns_delta/1000) + essencePtsOffset;
                Log.w("MmtPacketIdContext", String.format("Using interpolated clock for mpu_sequence_number: %d, computedPTS: %d", mpu_sequence_number, computed_new_mfu_presentation_timestamp_us));

            } else {
                //use our free-run clock, this would be at least more than one mpu missing a mpu_timestamp_descriptor
                computed_new_mfu_presentation_timestamp_us = last_essence_mfu_presentation_timestamp_us + (system_time_ns_delta/1000);
                last_essence_mfu_presentation_timestamp_us = last_essence_mpu_presentation_time_us = computed_new_mfu_presentation_timestamp_us;
                Log.w("MmtPacketIdContext", String.format("Using free-run clock for mpu_sequence_number: %d, computedPTS: %d", mpu_sequence_number, computed_new_mfu_presentation_timestamp_us));

            }

            last_essence_pts_offset = essencePtsOffset;
            last_essence_mfu_system_time_ns = System.nanoTime();
            last_essence_mfu_is_from_descriptor = false;

            return computed_new_mfu_presentation_timestamp_us;
        }
    }
}
