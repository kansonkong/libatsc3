package org.ngbp.libatsc3.middleware.android.mmt.models;

/*
    mapping from atsc3_audio_decoder_configuration_record_t to java type
 */
public class MMTAudioDecoderConfigurationRecord {
    //mmtp tracking information
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
}
