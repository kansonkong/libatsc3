package org.ngbp.libatsc3.middleware.android.application.interfaces;

import org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MfuByteBufferFragment;
import org.ngbp.libatsc3.middleware.android.application.sync.mmt.MpuMetadata_HEVC_NAL_Payload;

import java.io.File;

public interface IAtsc3NdkApplicationBridgeCallbacks {
    public void showMsgFromNative(String message);

    public File jni_getCacheDir(); //hack, todo - move to pure NDK impl

    //service selection/notification callbacks
    public void onSlsTablePresent(String slsPayloadXML);
    public void onAeatTablePresent(String aeatPayloadXML);
    public void onSlsHeldEmissionPresent(int serviceId, String heldPayloadXML);

    //MMT specific MFU callbacks
    public void pushMfuByteBufferFragment(MfuByteBufferFragment mfuByteBufferFragment);
    public void pushMpuMetadata_HEVC_NAL_Payload(MpuMetadata_HEVC_NAL_Payload mpuMetadata_hevc_nal_payload);

    //ROUTE-DASH specific callbacks
    public void onAlcObjectStatusMessage(String alc_object_status_message);
    public void onPackageExtractCompleted(PackageExtractEnvelopeMetadataAndPayload packageExtractEnvelopeMetadataAndPayload);
    public void routeDash_force_player_reload_mpd(int ServiceID);




}
