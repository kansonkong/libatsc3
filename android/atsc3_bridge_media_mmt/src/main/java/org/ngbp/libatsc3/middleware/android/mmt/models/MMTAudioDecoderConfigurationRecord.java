package org.ngbp.libatsc3.middleware.android.mmt.models;

/*
    mapping from atsc3_audio_decoder_configuration_record_t to java type
 */
public class MMTAudioDecoderConfigurationRecord {

    //AC-4:
    public static final String AC_4_ID = "ac-4";

    //MPEG-H: A/342-3:2017
    public static final String MPEG_H_MHAS_ID = "mhm1";
    public static final String MPEG_H_HYBRID_ID = "mhm2";

    //mime types
    public static final String MPEG_H_MHAS_MIME_TYPE = "audio/" + MPEG_H_MHAS_ID;
    public static final String MPEG_H_HYBRID_MIME_TYPE = "audio/" + MPEG_H_HYBRID_ID;

    //Audio: mp4 (latm?)
    public static final String AAC_MP4A_ID = "mp4a";

    //xHE-AAC
    public static final String XHE_AAC_ID = "xhe1";

    //mmtp tracking information
    public String asset_type;

    public int packet_id;
    public long mpu_sequence_number; //uint32_t doesn't map, so use long (int64_t)

    //audio decoder configuration extracted from isobmff track initialization fragment
    public int channel_count;
    public int sample_depth;
    public int sample_rate;
    public long timebase;

    //extracted from moof/fragment metadata
    public long sample_duration;

    //AC-4 specific parameters
    public AudioAC4SampleEntryBox audioAC4SampleEntryBox = null;

    public class AudioAC4SampleEntryBox {
        public long box_size;
        public int type;
        public int channel_count;
        public int sample_size;
        public int sampling_frequency;

        public AC4SpecificBox ac4SpecificBox = null;

        public class AC4SpecificBox {
            public long box_size;
            public int type;
            public int ac4_dsi_version;
            public int bitstream_version;
            public int fs_index;
            public int frame_rate_index;
            public int n_presentations;
        }
    }

    //jjustman-2021-09-07 - todo - add mhm1 support
}
