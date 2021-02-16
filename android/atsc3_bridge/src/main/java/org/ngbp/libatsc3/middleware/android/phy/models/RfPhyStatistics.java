package org.ngbp.libatsc3.middleware.android.phy.models;

import com.google.firebase.perf.FirebasePerformance;
import com.google.firebase.perf.metrics.Trace;

public class RfPhyStatistics {

    //jjustman-2020-12-24 - work in progress hack to refactor quickly for IAtsc3NdkPHYClientRFMetrics
    
    public int tuner_lock;
    public int demod_lock;

    public int plp_lock_any;
    public int plp_lock_all;
    public int plp_lock_by_setplp_index;

    public int cpu_status;

    public int rssi;
    public int snr1000;
    public int rfLevel1000;

    public int bootstrap_system_bw;
    public int bootstrap_ea_wakeup;

    public int plp_id_0;
    public int modcod_valid_0;
    public int plp_fec_type_0;
    public int plp_mod_0;
    public int plp_cod_0;
    public int ber_pre_ldpc_0;
    public int ber_pre_bch_0;
    public int fer_post_bch_0;
    
    //jjustman-2020-12-24 - hack
    public int plp_id_1;
    public int modcod_valid_1;
    public int plp_fec_type_1;
    public int plp_mod_1;
    public int plp_cod_1;
    public int ber_pre_ldpc_1;
    public int ber_pre_bch_1;
    public int fer_post_bch_1;

    public int plp_id_2;
    public int modcod_valid_2;
    public int plp_fec_type_2;
    public int plp_mod_2;
    public int plp_cod_2;
    public int ber_pre_ldpc_2;
    public int ber_pre_bch_2;
    public int fer_post_bch_2;

    public int plp_id_3;
    public int modcod_valid_3;
    public int plp_fec_type_3;
    public int plp_mod_3;
    public int plp_cod_3;
    public int ber_pre_ldpc_3;
    public int ber_pre_bch_3;
    public int fer_post_bch_3;

    public RfPhyStatistics(int tuner_lock, int rssi, int modcod_valid_0, int plp_fec_type_0, int plp_mod_0, int plp_cod_0, int rfLevel1000, int snr1000, int ber_pre_ldpc_0, int ber_pre_bch_0, int fer_post_bch_0, int demod_lock, int cpu_status, int plp_any, int plp_all) {
        this.tuner_lock = tuner_lock;
        this.rssi = rssi;
        this.modcod_valid_0 = modcod_valid_0;
        this.plp_fec_type_0 = plp_fec_type_0;
        this.plp_mod_0 = plp_mod_0;
        this.plp_cod_0 = plp_cod_0;
        this.rfLevel1000 = rfLevel1000;
        this.snr1000 = snr1000;
        this.ber_pre_ldpc_0 = ber_pre_ldpc_0;
        this.ber_pre_bch_0 = ber_pre_bch_0;
        this.fer_post_bch_0 = fer_post_bch_0;
        this.demod_lock = demod_lock;
        this.cpu_status = cpu_status;
        this.plp_lock_any = plp_any;
        this.plp_lock_all = plp_all;
    }



    @Override
    public String toString() {
        return String.format("TunLock: %d, DemodLock: %d, PlpLock:Any: 0x%02x, All: 0x%02x, CpuStatus: %s, RSSI: %d.%03d dB, SNR: %.2f\n" +
                                "PLP0: ModCod: G: %d, %s (%d), %s (%d), %s (%d), BER: pre_ldpc: %d, pre_bch: %d, post_bch: %d\n" +
                                "PLP1: ModCod: G: %d, %s (%d), %s (%d), %s (%d), BER: pre_ldpc: %d, pre_bch: %d, post_bch: %d\n" +
                                "PLP2: ModCod: G: %d, %s (%d), %s (%d), %s (%d), BER: pre_ldpc: %d, pre_bch: %d, post_bch: %d\n" +
                                "PLP3: ModCod: G: %d, %s (%d), %s (%d), %s (%d), BER: pre_ldpc: %d, pre_bch: %d, post_bch: %d\n",
                
                this.tuner_lock,
                this.demod_lock,
                this.plp_lock_any,
                this.plp_lock_all,

                this.cpu_status == 1 ? "R" : "H",
                (this.rssi) / 1000,
                ((-this.rssi) % 1000),
                (float) this.snr1000 / 1000.0,

                this.modcod_valid_0,
                RfPhyFecModCodTypes.L1d_PlpFecType.getOrDefault(this.plp_fec_type_0, RfPhyFecModCodTypes.L1d_PlpFecType.get(255)),
                this.plp_fec_type_0,
                RfPhyFecModCodTypes.L1d_PlpMod.getOrDefault(this.plp_mod_0, RfPhyFecModCodTypes.L1d_PlpMod.get(255)),
                this.plp_mod_0,
                RfPhyFecModCodTypes.L1d_PlpCod.getOrDefault(this.plp_cod_0, RfPhyFecModCodTypes.L1d_PlpCod.get(255)),
                this.plp_cod_0,
                this.ber_pre_ldpc_0,
                this.ber_pre_bch_0,
                this.fer_post_bch_0,

                this.modcod_valid_1,
                RfPhyFecModCodTypes.L1d_PlpFecType.getOrDefault(this.plp_fec_type_1, RfPhyFecModCodTypes.L1d_PlpFecType.get(255)),
                this.plp_fec_type_1,
                RfPhyFecModCodTypes.L1d_PlpMod.getOrDefault(this.plp_mod_1, RfPhyFecModCodTypes.L1d_PlpMod.get(255)),
                this.plp_mod_1,
                RfPhyFecModCodTypes.L1d_PlpCod.getOrDefault(this.plp_cod_1, RfPhyFecModCodTypes.L1d_PlpCod.get(255)),
                this.plp_cod_1,
                this.ber_pre_ldpc_1,
                this.ber_pre_bch_1,
                this.fer_post_bch_1,

                this.modcod_valid_2,
                RfPhyFecModCodTypes.L1d_PlpFecType.getOrDefault(this.plp_fec_type_2, RfPhyFecModCodTypes.L1d_PlpFecType.get(255)),
                this.plp_fec_type_2,
                RfPhyFecModCodTypes.L1d_PlpMod.getOrDefault(this.plp_mod_2, RfPhyFecModCodTypes.L1d_PlpMod.get(255)),
                this.plp_mod_2,
                RfPhyFecModCodTypes.L1d_PlpCod.getOrDefault(this.plp_cod_2, RfPhyFecModCodTypes.L1d_PlpCod.get(255)),
                this.plp_cod_2,
                this.ber_pre_ldpc_2,
                this.ber_pre_bch_2,
                this.fer_post_bch_2,

                this.modcod_valid_3,
                RfPhyFecModCodTypes.L1d_PlpFecType.getOrDefault(this.plp_fec_type_3, RfPhyFecModCodTypes.L1d_PlpFecType.get(255)),
                this.plp_fec_type_3,
                RfPhyFecModCodTypes.L1d_PlpMod.getOrDefault(this.plp_mod_3, RfPhyFecModCodTypes.L1d_PlpMod.get(255)),
                this.plp_mod_3,
                RfPhyFecModCodTypes.L1d_PlpCod.getOrDefault(this.plp_cod_3, RfPhyFecModCodTypes.L1d_PlpCod.get(255)),
                this.plp_cod_3,
                this.ber_pre_ldpc_3,
                this.ber_pre_bch_3,
                this.fer_post_bch_3
                );
    }

    public void sampleRfPhyStatisticsForTrace() {
        //jjustman-2021-01-14 - transient firebase performance trace
        Trace rfPhyStatisticsTrace = FirebasePerformance.getInstance().newTrace("phy_rf_statistics_sample");

        rfPhyStatisticsTrace.start();

        rfPhyStatisticsTrace.putAttribute("tuner_lock", tuner_lock == 1 ? "true" : "false");
        rfPhyStatisticsTrace.putAttribute("demod_lock", demod_lock == 1 ? "true" : "false");
        rfPhyStatisticsTrace.putAttribute("plp_lock_any", demod_lock == 1 ? "true" : "false");
        rfPhyStatisticsTrace.putAttribute("plp_lock_all", demod_lock == 1 ? "true" : "false");
        rfPhyStatisticsTrace.putAttribute("cpu_status", this.cpu_status == 1 ? "R" : "H");

        rfPhyStatisticsTrace.putMetric("rssi1000", (long)(1000.0 * (Double.parseDouble(String.format("%d.%03d", (this.rssi) / 1000, ((-this.rssi) % 1000))))));
        rfPhyStatisticsTrace.putMetric("snr1000", this.snr1000);

        rfPhyStatisticsTrace.putAttribute("modcod_valid_0", this.modcod_valid_0 == 1 ? "true" : "false");
        rfPhyStatisticsTrace.putAttribute("plp_fec_type_0", RfPhyFecModCodTypes.L1d_PlpFecType.getOrDefault(this.plp_fec_type_0, RfPhyFecModCodTypes.L1d_PlpFecType.get(255)));

        rfPhyStatisticsTrace.putAttribute("plp_mod_0", RfPhyFecModCodTypes.L1d_PlpMod.getOrDefault(this.plp_mod_0, RfPhyFecModCodTypes.L1d_PlpMod.get(255)));
        rfPhyStatisticsTrace.putAttribute("plp_cod_0", RfPhyFecModCodTypes.L1d_PlpCod.getOrDefault(this.plp_cod_0, RfPhyFecModCodTypes.L1d_PlpCod.get(255)));

        rfPhyStatisticsTrace.putMetric("ber_pre_ldpc_0", this.ber_pre_ldpc_0);
        rfPhyStatisticsTrace.putMetric("ber_pre_ldpc_0", this.ber_pre_bch_0);
        rfPhyStatisticsTrace.putMetric("ber_pre_ldpc_0", this.fer_post_bch_0);

        rfPhyStatisticsTrace.stop();
    }
}