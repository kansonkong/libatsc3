package org.ngbp.libatsc3.middleware.android.phy.models;

public class RfPhyStatistics {

    public int tuner_lock;
    public int rssi;
    public int modcod_valid;
    public int plp_fec_type;
    public int plp_mod;
    public int plp_cod;
    public int nRfLevel1000;
    public int nSnr1000;
    public int ber_pre_ldpc_e7;
    public int ber_pre_bch_e9;
    public int fer_post_bch_e6;
    public int demod_lock_status;
    public int cpu_status;
    public int plp_any;
    public int plp_all;

    public RfPhyStatistics(int tuner_lock, int rssi, int modcod_valid, int plp_fec_type, int plp_mod, int plp_cod, int nRfLevel1000, int nSnr1000, int ber_pre_ldpc_e7, int ber_pre_bch_e9, int fer_post_bch_e6, int demod_lock_status, int cpu_status, int plp_any, int plp_all) {
        this.tuner_lock = tuner_lock;
        this.rssi = rssi;
        this.modcod_valid = modcod_valid;
        this.plp_fec_type = plp_fec_type;
        this.plp_mod = plp_mod;
        this.plp_cod = plp_cod;
        this.nRfLevel1000 = nRfLevel1000;
        this.nSnr1000 = nSnr1000;
        this.ber_pre_ldpc_e7 = ber_pre_ldpc_e7;
        this.ber_pre_bch_e9 = ber_pre_bch_e9;
        this.fer_post_bch_e6 = fer_post_bch_e6;
        this.demod_lock_status = demod_lock_status;
        this.cpu_status = cpu_status;
        this.plp_any = plp_any;
        this.plp_all = plp_all;
    }

    @Override
    public String toString() {
        return String.format("TunLock: %d, DemodLock: %d, CpuStatus: %s, RSSI: %d.%03d dB, SNR: %.2f, ModCod: G: %d, %s (%d), %s (%d), %s (%d), BER: pre_ldpc: %d, pre_bch: %d, post_bch: %d",
                this.tuner_lock,
                this.demod_lock_status,
                this.cpu_status == 1 ? "R" : "H",
                (this.rssi) / 1000,
                ((-this.rssi) % 1000),
                (float) this.nSnr1000 / 1000.0,
                this.modcod_valid,
                RfPhyFecModCodTypes.L1d_PlpFecType.getOrDefault(this.plp_fec_type, RfPhyFecModCodTypes.L1d_PlpFecType.get(255)),
                this.plp_fec_type,
                RfPhyFecModCodTypes.L1d_PlpMod.getOrDefault(this.plp_mod, RfPhyFecModCodTypes.L1d_PlpMod.get(255)),
                this.plp_mod,
                RfPhyFecModCodTypes.L1d_PlpCod.getOrDefault(this.plp_cod, RfPhyFecModCodTypes.L1d_PlpCod.get(255)),
                this.plp_cod,
                this.ber_pre_ldpc_e7, this.ber_pre_bch_e9, this.fer_post_bch_e6);

    }
}