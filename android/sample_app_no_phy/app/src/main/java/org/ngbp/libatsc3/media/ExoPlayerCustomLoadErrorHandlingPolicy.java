package org.ngbp.libatsc3.media;

import android.util.Log;

import com.google.android.exoplayer2.upstream.DefaultLoadErrorHandlingPolicy;

import java.io.IOException;

public final class ExoPlayerCustomLoadErrorHandlingPolicy
        extends DefaultLoadErrorHandlingPolicy {

    @Override
    public long getRetryDelayMsFor(
            int dataType,
            long loadDurationMs,
            IOException exception,
            int errorCount) {
        Log.w("ExoPlayerCustomLoadErrorHandlingPolicy",String.format("dataType: %d, loadDurationMs: %d, exception ex: %s, errorCount: %d", dataType, loadDurationMs, exception, errorCount));

        //jjustman-2019-11-07 - retry every 1s for exoplayer errors from ROUTE/DASH
        return 1000;

        // Replace NoConnectivityException with the corresponding
        // exception for the used DataSource.
//        if (exception instanceof NoConnectivityException) {
//            return 5000; // Retry every 5 seconds.
//        } else {
//            return C.TIME_UNSET; // Anything else is surfaced.
//        }
    }

//    @Override
//    public int getMinimumLoadableRetryCount(int dataType) {
//        return Integer.MAX_VALUE;
//    }
    /**
     * See {@link #DefaultLoadErrorHandlingPolicy()} and {@link #DefaultLoadErrorHandlingPolicy(int)}
     * for documentation about the behavior of this method.
     */
    private  int minimumLoadableRetryCount = 1;

    @Override
    public int getMinimumLoadableRetryCount(int dataType) {
        return 1;
//            return dataType == C.DATA_TYPE_MEDIA_PROGRESSIVE_LIVE
//                    ? DEFAULT_MIN_LOADABLE_RETRY_COUNT_PROGRESSIVE_LIVE
//                    : DEFAULT_MIN_LOADABLE_RETRY_COUNT;
//        } else {
//            return minimumLoadableRetryCount;
//        }
    }
}

