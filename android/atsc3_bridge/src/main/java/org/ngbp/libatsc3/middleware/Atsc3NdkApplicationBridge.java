package org.ngbp.libatsc3.middleware;

import android.util.Log;

import org.ngbp.libatsc3.middleware.android.a331.PackageExtractEnvelopeMetadataAndPayload;
import org.ngbp.libatsc3.middleware.android.application.interfaces.IAtsc3NdkApplicationBridgeCallbacks;
import org.ngbp.libatsc3.middleware.android.application.models.AndroidSystemProperties;

import java.io.File;


/** \addtogroup Atsc3ApplicationBridge
 *  @{
 *      \defgroup Atsc3JniApplicationBridge libatsc3 JNI Bridge
 *
 *      Java interface contract for NDK method invocation and NDK bridge callback method interface reference wire-up
 *
 *      @{
 *          \defgroup Atsc3JniApplicationBridgeNdkMethods libatsc3 JNI Bridge Methods into NDK
 *
 *          Java NDK method definitions and dispatcher for NDK application bridge callbacks
 *
 *          @{
 *
 */
public class Atsc3NdkApplicationBridge extends Atsc3BridgeNdkStaticJniLoader
{
    final static String TAG ="intf";

    IAtsc3NdkApplicationBridgeCallbacks mActivity;
    public static final Boolean MMT_DISCARD_CORRUPT_FRAMES = true;

    //native jni methods
    @Override
    public native int init();

    //service invocation change methods
    public native int atsc3_slt_selectService(int service_id);                  //select either single MMT or ROUTE service
    public native int atsc3_slt_alc_select_additional_service(int service_id);  //listen on additional ALC service(s)
    public native int atsc3_slt_alc_clear_additional_service_selections();      //clear ALC additional service listeners

    //NOTE: these methods may return an empty collection if the MBMS carousel or S-TSID has not been received after selecting service/additional_service
    public native String[] atsc3_slt_alc_get_sls_metadata_fragments_content_locations_from_monitor_service_id(int service_id, String to_match_content_type);
    public native String[] atsc3_slt_alc_get_sls_route_s_tsid_fdt_file_content_locations_from_monitor_service_id(int service_id);

    public native AndroidSystemProperties atsc3_slt_alc_get_system_properties();      //clear ALC additional service listeners

    public Atsc3NdkApplicationBridge(IAtsc3NdkApplicationBridgeCallbacks iAtsc3NdkApplicationBridgeCallbacks) {
        mActivity = iAtsc3NdkApplicationBridgeCallbacks;
        init();
    }

    int onLogMsg(String msg) {
        Log.d(TAG, msg);
        mActivity.showMsgFromNative(msg+"\n");
        return 0;
    }

    int atsc3_onSltTablePresent(String slt_payload_xml) {

        mActivity.onSltTablePresent(slt_payload_xml);
        return 0;
    }

    int atsc3_onAeatTablePresent(String aeat_table_xml) {

        mActivity.onAeatTablePresent(aeat_table_xml);
        return 0;
    }

    int atsc3_on_alc_object_status_message(String alc_object_status_message) {
        mActivity.onAlcObjectStatusMessage(alc_object_status_message);
        return 0;
    }

    int atsc3_on_alc_object_closed(int service_id, int tsi, int toi, String s_tsid_content_location, String s_tsid_content_type, String cache_file_path) {
        mActivity.onAlcObjectClosed(service_id, tsi, toi, s_tsid_content_location, s_tsid_content_type, cache_file_path);
        return 0;
    }

    int atsc3_lls_sls_alc_on_package_extract_completed(PackageExtractEnvelopeMetadataAndPayload packageExtractEnvelopeMetadataAndPayload) {
        mActivity.onPackageExtractCompleted(packageExtractEnvelopeMetadataAndPayload);
        return 0;
    }


    File getCacheDir() {
        return mActivity.jni_getCacheDir();
    }

    int atsc3_lls_sls_alc_on_route_mpd_patched(int service_id) {
        mActivity.routeDash_force_player_reload_mpd(service_id);
        return 0;
    }

    int atsc3_onSlsHeldEmissionPresent(int serviceId, String heldPayloadXML) {
        mActivity.onSlsHeldEmissionPresent(serviceId, heldPayloadXML);
        return 0;
    }
}

/**
 *           @}
 *      @}
 * @}
*/