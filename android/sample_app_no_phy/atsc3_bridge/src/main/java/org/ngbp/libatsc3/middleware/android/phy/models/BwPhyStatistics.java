package org.ngbp.libatsc3.middleware.android.phy.models;

public class BwPhyStatistics {
    public long total_pkts;
    public long total_bytes;
    public int total_lmts;

    public BwPhyStatistics(long total_pkts, long total_bytes, int total_lmts) {
        this.total_pkts = total_pkts;
        this.total_bytes = total_bytes;
        this.total_lmts = total_lmts;
    }
}
