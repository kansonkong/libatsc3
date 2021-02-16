package org.ngbp.libatsc3.middleware.android.mmt;

import java.nio.ByteBuffer;

public class MpuMetadata_HEVC_NAL_Payload {

    public int packet_id;
    public long my_mpu_sequence_number; //technically, mpu_sequence_number is uint32_t, so we need int64_t to handle it properly in java
    public int myLength;

    public ByteBuffer myByteBuffer;

    public MpuMetadata_HEVC_NAL_Payload(int packet_id, long mpu_sequence_number, ByteBuffer nativeByteBuffer, int length) {
        this.packet_id = packet_id;
        my_mpu_sequence_number = mpu_sequence_number;
        myByteBuffer = ByteBuffer.allocate(length);
        myByteBuffer.put(nativeByteBuffer);
        myByteBuffer.rewind();

        myLength = length;
    }

    public void releaseByteBuffer() {
        if(myByteBuffer != null) {
            myByteBuffer.clear();
        }

        myByteBuffer = null;
    }
}
