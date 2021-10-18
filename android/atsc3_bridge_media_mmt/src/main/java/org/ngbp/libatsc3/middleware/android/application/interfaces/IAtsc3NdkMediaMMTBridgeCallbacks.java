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

    public void onVideoStreamProperties(MmtVideoProperties.MmtVideoPropertiesDescriptor descriptor);
    public void onCaptionAssetProperties(MmtCaptionProperties.MmtCaptionPropertiesDescriptor descriptor);
    public void onAudioStreamProperties(MmtAudioProperties.MmtAudioPropertiesDescriptor descriptor);

    public void onMpTableComplete(MmtMpTable.MmtAssetTable table);
}
