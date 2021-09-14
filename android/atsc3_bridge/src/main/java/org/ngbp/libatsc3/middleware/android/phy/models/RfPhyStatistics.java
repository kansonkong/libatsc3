package org.ngbp.libatsc3.middleware.android.phy.models;

public class RfPhyStatistics {

    //jjustman-2020-12-24 - work in progress hack to refactor quickly for IAtsc3NdkPHYClientRFMetrics
    
    public int tuner_lock;
    public int demod_lock;

    public int plp_lock_any;
    public int plp_lock_all;
    public int plp_lock_by_setplp_index;

    public int cpu_status;

    public int rssi;
    public int snr1000_global;
    public int snr1000_l1b;
    public int snr1000_l1d;

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
    public int total_fec_0;
    public int total_error_fec_0;
    public int plp_snr1000_0;
    
    //jjustman-2020-12-24 - hack
    public int plp_id_1;
    public int modcod_valid_1;
    public int plp_fec_type_1;
    public int plp_mod_1;
    public int plp_cod_1;
    public int ber_pre_ldpc_1;
    public int ber_pre_bch_1;
    public int fer_post_bch_1;
    public int total_fec_1;
    public int total_error_fec_1;
    public int plp_snr1000_1;

    public int plp_id_2;
    public int modcod_valid_2;
    public int plp_fec_type_2;
    public int plp_mod_2;
    public int plp_cod_2;
    public int ber_pre_ldpc_2;
    public int ber_pre_bch_2;
    public int fer_post_bch_2;
    public int total_fec_2;
    public int total_error_fec_2;
    public int plp_snr1000_2;

    public int plp_id_3;
    public int modcod_valid_3;
    public int plp_fec_type_3;
    public int plp_mod_3;
    public int plp_cod_3;
    public int ber_pre_ldpc_3;
    public int ber_pre_bch_3;
    public int fer_post_bch_3;
    public int total_fec_3;
    public int total_error_fec_3;
    public int plp_snr1000_3;

    public RfPhyStatistics(int tuner_lock, int rssi, int modcod_valid_0, int plp_fec_type_0, int plp_mod_0, int plp_cod_0, int rfLevel1000, int snr1000_global, int ber_pre_ldpc_0, int ber_pre_bch_0, int fer_post_bch_0, int demod_lock, int cpu_status, int plp_any, int plp_all) {
        this.tuner_lock = tuner_lock;
        this.rssi = rssi;
        this.modcod_valid_0 = modcod_valid_0;
        this.plp_fec_type_0 = plp_fec_type_0;
        this.plp_mod_0 = plp_mod_0;
        this.plp_cod_0 = plp_cod_0;
        this.rfLevel1000 = rfLevel1000;
        this.snr1000_global = snr1000_global;
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
        return String.format("TunLk: %d, DmLk: %d (RF: %d, L1B: %d, L1D: %d), PLk:Any: 0x%02x, All: 0x%02x, Cpu: %s\n"+
                                "RSSI: %d.%03d dB (raw: %d), GSNR: %.2f, L1B_SNR: %.2f, L1D_SNR: %.2f\n" +
                                "P0: SNR: %.2f, M/C: G: %d, %s (%d), %s (%d), %s (%d), BER: p_ldpc: %d, p_bch: %d, post_bch: %d, t_fec: %d, t_e_fec: %d\n" +
                                "P1: SNR: %.2f, M/C: G: %d, %s (%d), %s (%d), %s (%d), BER: p_ldpc: %d, p_bch: %d, post_bch: %d, t_fec: %d, t_e_fec: %d\n" +
                                "P2: SNR: %.2f, M/C: G: %d, %s (%d), %s (%d), %s (%d), BER: p_ldpc: %d, p_bch: %d, post_bch: %d, t_fec: %d, t_e_fec: %d\n" +
                                "P3: SNR: %.2f, M/C: G: %d, %s (%d), %s (%d), %s (%d), BER: p_ldpc: %d, p_bch: %d, post_bch: %d, t_fec: %d, t_e_fec: %d\n",
                
                this.tuner_lock,
                this.demod_lock,

                (this.demod_lock >> 0) & 0x01,
                (this.demod_lock >> 1) & 0x01,
                (this.demod_lock >> 2) & 0x01,

                this.plp_lock_any,
                this.plp_lock_all,

                this.cpu_status == 1 ? "R" : "H",

                (this.rssi) / 1000,
                ((-this.rssi) % 1000),
                this.rssi,

                (float) this.snr1000_global / 1000.0,
                (float) this.snr1000_l1b / 1000.0,
                (float) this.snr1000_l1d / 1000.0,

                (float)this.plp_snr1000_0 / 1000.0,

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
                this.total_fec_0,
                this.total_error_fec_0,

                (float)this.plp_snr1000_1 / 1000.0,
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
                this.total_fec_1,
                this.total_error_fec_1,

                (float)this.plp_snr1000_2 / 1000.0,
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
                this.total_fec_2,
                this.total_error_fec_2,


                (float)this.plp_snr1000_3 / 1000.0,
                this.modcod_valid_3,

                RfPhyFecModCodTypes.L1d_PlpFecType.getOrDefault(this.plp_fec_type_3, RfPhyFecModCodTypes.L1d_PlpFecType.get(255)),
                this.plp_fec_type_3,
                RfPhyFecModCodTypes.L1d_PlpMod.getOrDefault(this.plp_mod_3, RfPhyFecModCodTypes.L1d_PlpMod.get(255)),
                this.plp_mod_3,
                RfPhyFecModCodTypes.L1d_PlpCod.getOrDefault(this.plp_cod_3, RfPhyFecModCodTypes.L1d_PlpCod.get(255)),
                this.plp_cod_3,
                this.ber_pre_ldpc_3,
                this.ber_pre_bch_3,
                this.fer_post_bch_3,
                this.total_fec_3,
                this.total_error_fec_3
                );
    }
}