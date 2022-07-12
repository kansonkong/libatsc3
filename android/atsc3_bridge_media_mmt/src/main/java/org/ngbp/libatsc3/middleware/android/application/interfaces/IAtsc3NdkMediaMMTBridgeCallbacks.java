package org.ngbp.libatsc3.middleware.android.application.interfaces;

import org.ngbp.libatsc3.middleware.android.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.mmt.MpuMetadata_HEVC_NAL_Payload;
import org.ngbp.libatsc3.middleware.android.mmt.models.MMTAudioDecoderConfigurationRecord;
import org.ngbp.libatsc3.middleware.mmt.pb.MmtAudioProperties;
import org.ngbp.libatsc3.middleware.mmt.pb.MmtCaptionProperties;
import org.ngbp.libatsc3.middleware.mmt.pb.MmtMpTable;
import org.ngbp.libatsc3.middleware.mmt.pb.MmtVideoProperties;

import java.util.List;

public interface IAtsc3NdkMediaMMTBridgeCallbacks {
    public void showMsgFromNative(String message);

    //MMT specific MFU callbacks
    public void pushMfuByteBufferFragment(MfuByteBufferFragment mfuByteBufferFragment);
    public void pushMpuMetadata_HEVC_NAL_Payload(MpuMetadata_HEVC_NAL_Payload mpuMetadata_hevc_nal_payload);

    public void pushAudioDecoderConfigurationRecord(MMTAudioDecoderConfigurationRecord mmtAudioDecoderConfigurationRecord);

    public void atsc3_signallingContext_notify_audio_packet_id_and_mpu_timestamp_descriptor(int audio_packet_id, long mpu_sequence_number, long mpu_presentation_time_ntp64, long mpu_presentation_time_seconds, int mpu_presentation_time_microseconds);

    public void onVideoStreamProperties(MmtVideoProperties.MmtVideoPropertiesDescriptor descriptor);
    public void onCaptionAssetProperties(MmtCaptionProperties.MmtCaptionPropertiesDescriptor descriptor);
    public void onAudioStreamProperties(MmtAudioProperties.MmtAudioPropertiesDescriptor descriptor);

    public void onMpTableSubset(MmtMpTable.MmtAssetTable table);
    public void onMpTableComplete(MmtMpTable.MmtAssetTable table);

    public void notifySlHdr1Present(int service_id, int packet_id);
}
