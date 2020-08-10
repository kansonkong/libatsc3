package org.ngbp.libatsc3.media;

import android.media.MediaCodec;
import android.util.Log;

import org.ngbp.libatsc3.middleware.android.DebuggingFlags;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MmtPacketIdContext;

import java.nio.ByteBuffer;

public class MediaCodecInputBuffer {

    public static int queueInputBufferInvocationCount = 0;

    public static void WriteToInputBufferFromMfuByteBufferFragment(MediaCodec mediaCodec, Integer index, MfuByteBufferFragment mfuByteBufferFragment) {

        ByteBuffer byteBuffer = mediaCodec.getInputBuffer(index);
        byteBuffer.put(mfuByteBufferFragment.myByteBuffer);
    }

    public static void QueueInputBuffer(MediaCodec mediaCodec, Integer index, MfuByteBufferFragment mfuByteBufferFragment, long presentationTimeUs, MmtPacketIdContext.MmtMfuStatistics mmtMfuStatistics) throws Exception {
        queueInputBufferInvocationCount++;

        if(true || DebuggingFlags.CODEC_LOGGING) {
            Log.d("\tMediaCodecInputBuffer\tqueueInputBuffer", String.format("\ttype:%s\tPtsUS:\t%d\tPtsS\t%f\tindex:\t%d\tqueueInputBufferInvocationCount:\t%d",
                    mmtMfuStatistics.type,
                    presentationTimeUs,
                    presentationTimeUs/1000000.0,
                    index,
                    queueInputBufferInvocationCount));
        }

        mediaCodec.queueInputBuffer(index, 0, mfuByteBufferFragment.bytebuffer_length,  presentationTimeUs, 0);
        mfuByteBufferFragment.unreferenceByteBuffer();
    }
}
