package org.ngbp.libatsc3.middleware.android.application.interfaces;

import org.ngbp.libatsc3.middleware.android.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.mmt.MpuMetadata_HEVC_NAL_Payload;
import org.ngbp.libatsc3.middleware.android.mmt.models.MMTAudioDecoderConfigurationRecord;

import java.io.File;

public interface IAtsc3NdkMediaMMTBridgeCallbacks {
    public void showMsgFromNative(String message);

    //MMT specific MFU callbacks
    public void pushMfuByteBufferFragment(MfuByteBufferFragment mfuByteBufferFragment);
    public void pushMpuMetadata_HEVC_NAL_Payload(MpuMetadata_HEVC_NAL_Payload mpuMetadata_hevc_nal_payload);

    public void pushAudioDecoderConfigurationRecord(MMTAudioDecoderConfigurationRecord mmtAudioDecoderConfigurationRecord);
}
