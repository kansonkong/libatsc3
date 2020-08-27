package org.ngbp.libatsc3.middleware.android.application.sync.mmt;

import android.util.Log;

import java.nio.ByteBuffer;

public class MfuByteBufferFragment {
    //TODO: jjustman-2019-10-20 - add ServiceID and lls_group information..

    public int packet_id;
    public int mpu_sequence_number;
    public int sample_number;
    public int bytebuffer_length;

    public Long mpu_presentation_time_uS_from_SI = null;
    public Long mfu_presentation_time_uS_computed = null;
    public long get_safe_mfu_presentation_time_uS_computed() { return mfu_presentation_time_uS_computed != null ? mfu_presentation_time_uS_computed : 0; }

    public int mfu_fragment_count_expected;
    public int mfu_fragment_count_rebuilt;

    public ByteBuffer myByteBuffer;

    /* note, original pointer behind this ByteBuffer (nativeAllocation) should still be valid...? */
    public MfuByteBufferFragment(int packet_id, int mpu_sequence_number, int sample_number, ByteBuffer nativeByteBuffer, int length, long mpu_presentation_time_uS_from_SI, int mfu_fragment_count_expected, int mfu_fragment_count_rebuilt) {
        this.packet_id = packet_id;
        this.mpu_sequence_number = mpu_sequence_number;
        this.sample_number = sample_number;
        if(nativeByteBuffer != null && length > 0 ) {
            myByteBuffer = ByteBuffer.allocate(length);
            myByteBuffer.put(nativeByteBuffer);
            myByteBuffer.rewind();
        } else {
            Log.e("MfuByteBufferFragment", String.format("nativeByteBuffer was null/len: 0, packet_id: %d, mpu_sequence_number: %d, sample_number: %d", packet_id, mpu_sequence_number, sample_number));
        }
        bytebuffer_length = length;

        //TODO: move this calulcation back into libatsc3 event dispatching flow
        this.mpu_presentation_time_uS_from_SI = mpu_presentation_time_uS_from_SI;
        //only assign our mfu_presentation_time_uS_computed if we have our mpu_presentation_time from SI

        if(this.mpu_presentation_time_uS_from_SI != null && this.mpu_presentation_time_uS_from_SI > 0) {
            if (this.packet_id == MmtPacketIdContext.video_packet_id && MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us > 0) {
                mfu_presentation_time_uS_computed = mpu_presentation_time_uS_from_SI + (sample_number - 1) * MmtPacketIdContext.video_packet_statistics.extracted_sample_duration_us;
            } else if (this.packet_id == MmtPacketIdContext.audio_packet_id && MmtPacketIdContext.audio_packet_statistics.extracted_sample_duration_us > 0) {
                mfu_presentation_time_uS_computed = mpu_presentation_time_uS_from_SI + (sample_number - 1) * MmtPacketIdContext.audio_packet_statistics.extracted_sample_duration_us;
            } else if (this.packet_id == MmtPacketIdContext.stpp_packet_id && MmtPacketIdContext.stpp_packet_statistics.extracted_sample_duration_us > 0) {
                mfu_presentation_time_uS_computed = mpu_presentation_time_uS_from_SI + (sample_number - 1) * MmtPacketIdContext.stpp_packet_statistics.extracted_sample_duration_us;
            }
        }

        this.mfu_fragment_count_expected = mfu_fragment_count_expected;
        this.mfu_fragment_count_rebuilt = mfu_fragment_count_rebuilt;
    }

    public void unreferenceByteBuffer() {
        if(myByteBuffer != null) {
            myByteBuffer.clear();
        }

        myByteBuffer = null;
    }
}
