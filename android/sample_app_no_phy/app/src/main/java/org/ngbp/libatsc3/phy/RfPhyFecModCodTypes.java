package org.ngbp.libatsc3.phy;

import java.util.HashMap;

public class RfPhyFecModCodTypes {
    public static HashMap<Integer, String> L1d_PlpFecType = new HashMap<Integer, String>() {{
        put(0, "Bch+Ldpc16K");
        put(1, "Bch+Ldpc64K");
        put(2, "Crc+Ldpc16K");
        put(3, "Crc+Ldpc64K");
        put(4, "Ldpc16K");
        put(5, "Ldpc64K");
        put(254, "PLP_FEC_MAX");
        put(255, "PLP_FEC_UNKNOWN");
    }};

    public static HashMap<Integer, String> L1d_PlpMod = new HashMap<Integer, String>() {{
        put(0, "QPSK");
        put(1, "16QAM");
        put(2, "64QAM");
        put(3, "256QAM");
        put(4, "1024QAM");
        put(5, "4096QAM");
        put(254, "PLP_MOD_MAX");
        put(255, "PLP_MOD_UNKNOWN");
    }};

    public static HashMap<Integer, String> L1d_PlpCod = new HashMap<Integer, String>() {{
        put(0, "2/15");
        put(1, "3/15");
        put(2, "4/15");
        put(3, "5/15");
        put(4, "6/15");
        put(5, "7/15");
        put(6, "8/15");
        put(7, "9/15");
        put(8, "10/15");
        put(9, "11/15");
        put(10, "12/15");
        put(11, "13/15");
        put(254, "PLP_CR_MAX");
        put(255, "PLP_CR_UNKNOWN");
    }};

}
