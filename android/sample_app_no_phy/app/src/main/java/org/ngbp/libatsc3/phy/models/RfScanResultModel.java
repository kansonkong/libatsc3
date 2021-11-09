package org.ngbp.libatsc3.phy.models;

/*
    a subset of RfPhyStatistics values
 */
public class RfScanResultModel {

    public long scan_start_ms;
    public long scan_end_ms;

    public int frequency_mhz;

    public int tuner_lock = 0;

    public int demod_lock_status = 0;

    public int max_rssi = -120;
    public int min_rssi = 0;

    public int max_snr1000 = 120;
    public int min_snr1000 = 0;

}
