package org.ngbp.libatsc3.middleware.android.phy.models;

import java.time.Instant;

public class L1D_timePhyInformation {
    /*

    jjustman-2021-08-31 - adding L1d_time PHY information from A/322 L1b_timeInfo and L1d_time_* attributes

        L1B_time_info_flag
        Value   Meaning
        -----   -----------
           00   Time information is not included in the current frame
           01   Time information is included in the current frame and signaled to ms precision
           10   Time information is included in the current frame and signaled to μs precision
           11   Time information is included in the current frame and signaled to ns precision


        L1D_time_sec – This field is the seconds portion of the precise time at which the first sample
                       of the first symbol of the most recently received bootstrap was transmitted,
                       shown as the Time Information Position in Figure 9.1.

                       L1D_time_sec shall contain the 32 least significant bits of the number of seconds elapsed
                       between the PTP epoch (see IEEE 1588 [6], Section 7.2.2) and the precise time at which
                       the first sample of the first symbol of the most recently received
                       bootstrap was transmitted.

                       (Note that this calculation is performed before leap seconds are subtracted.)

                       For example, if the precise time was:

                            17:30:48 UTC (i.e., 17:31:24 TAI) on the 12th February 2016

                       there would have been exactly 1455298284 seconds elapsed since the PTP epoch (which is 1st January 1970 00:00:00 TAI)
                       and the value transmitted in this field would be 0x56BE16EC.

                       The difference between TAI and UTC seconds is signaled in A/331 SystemTime@currentUtcOffset (see A/331 [5], Section 6.4).

                       The time value shall be transmitted at least once in every 5 second interval.

        L1D_time_msec – This field shall indicate the milliseconds component of the time information specified under L1D_time_sec. For example, if the portion of the time information less than one second is 0.123456789 this field shall be 123.
        L1D_time_usec – This field shall indicate the microseconds component of the time information specified under L1D_time_sec. For example, if the portion of the time information less than one second is 0.123456789 this field shall be 456.
        L1D_time_nsec – This field shall indicate the nanoseconds component of the time information specified under L1D_time_sec. For example, if the portion of the time information less than one second is 0.123456789 this field shall be 789.
     */

    public byte    L1B_time_info_flag;

    public long    L1D_time_sec;       //only using 32bit uimsbf
    public int     L1D_time_msec;   //10 bits
    public int     L1D_time_usec;   //10 bits
    public int     L1D_time_nsec;   //10 bits

    public double L1D_tai_calculated;
    public double L1D_unix_ts_calculated;

    public L1D_timePhyInformation(byte l1B_time_info_flag, long l1D_time_sec, int l1D_time_msec, int l1D_time_usec, int l1D_time_nsec) {
        L1B_time_info_flag = l1B_time_info_flag;
        L1D_time_sec = l1D_time_sec;
        L1D_time_msec = l1D_time_msec;
        L1D_time_usec = l1D_time_usec;
        L1D_time_nsec = l1D_time_nsec;

        L1D_tai_calculated = (L1D_time_sec) + (L1D_time_msec / 1000.0) + (L1D_time_usec / 1000000.0) + (L1D_time_nsec / 1000000000.0);
        L1D_unix_ts_calculated = L1D_tai_calculated - 37.0;
    }

    @Override
    public String toString() {
        return String.format("L1d_time_info: %d, %d.%d.%d.%d (TAI: %f, unixTs: %f)",
                L1B_time_info_flag, L1D_time_sec, L1D_time_msec, L1D_time_usec, L1D_time_nsec,
                L1D_tai_calculated, L1D_unix_ts_calculated);

    }

    //use kronosClock.getCurrentTimeMs()
    public String toStringFromAnchorNtpTimestamp(long ntpTimestampInMs) {
//        return String.format("L1d_time_info: %d, %d.%d.%d.%d (TAI: %f, unixTs: %f, delta: %f)",
//                L1B_time_info_flag,  L1D_time_sec, L1D_time_msec, L1D_time_usec, L1D_time_nsec,
//                L1D_tai_calculated, L1D_unix_ts_calculated,
//                ((ntpTimestampInMs/1000.0) - L1D_unix_ts_calculated));
        String deltaSpanOpen = "";
        String deltaSpanClosed = "";

        double deltaValue =  (ntpTimestampInMs/1000.0) - L1D_unix_ts_calculated;

        if(Math.abs(deltaValue) > 0.25) {
            deltaSpanOpen = "<span style=\"background-color:#aa0000\">";
            deltaSpanClosed = "</span>";
        } else {
            deltaSpanOpen = "<span style=\"background-color:#006600\">";
            deltaSpanClosed = "</span>";
        }

        return String.format("L1b_time_info_flag: %d\n" +
                        "L1 TAI   :% 17.6f (%s)\n" +
                        "L1 unixTs:% 17.6f (%s)\n" +
                        "NTP    Ts:% 17.6f (%s)\n" +
                        "%s" +
                        "DELTA    :% 18.6f" +
                        "%s",
                L1B_time_info_flag,
                L1D_tai_calculated, Instant.ofEpochMilli((long)(L1D_tai_calculated * 1000.0)).toString(),
                L1D_unix_ts_calculated, Instant.ofEpochMilli((long)(L1D_unix_ts_calculated * 1000.0)).toString(),
                (ntpTimestampInMs / 1000.0), Instant.ofEpochMilli(ntpTimestampInMs).toString(),
                deltaSpanOpen,
                deltaValue,
                deltaSpanClosed);

    }
}