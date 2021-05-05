package org.ngbp.libatsc3.middleware.android.application.interfaces;

import org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload;

import java.io.File;

public interface IAtsc3NdkApplicationBridgeCallbacks {
    public void showMsgFromNative(String message);

    public File jni_getCacheDir(); //hack, todo - move to pure NDK impl

    //service selection/notification callbacks
    public void onSltTablePresent(String slsPayloadXML);
    public void onAeatTablePresent(String aeatPayloadXML);
    public void onSlsHeldEmissionPresent(int serviceId, String heldPayloadXML);

    //ROUTE-DASH specific callbacks
    public void onAlcObjectStatusMessage(String alc_object_status_message);

    public void onAlcObjectClosed(int service_id, int tsi, int toi, String s_tsid_content_location, String s_tsid_content_type, String cache_file_path);
    public void onPackageExtractCompleted(PackageExtractEnvelopeMetadataAndPayload packageExtractEnvelopeMetadataAndPayload);

    public void routeDash_force_player_reload_mpd(int ServiceID);


}
