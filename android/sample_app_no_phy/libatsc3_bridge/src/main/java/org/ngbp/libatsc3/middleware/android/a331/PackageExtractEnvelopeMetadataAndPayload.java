package org.ngbp.libatsc3.middleware.android.a331;

import java.util.ArrayList;
import java.util.List;

public class PackageExtractEnvelopeMetadataAndPayload {
    public String packageName;
    public int tsi;
    public int toi;
    public String appContextIdList;
    public String filterCodes;

    public String packageExtractPath;

    /*
<?xml version="1.0" encoding="UTF-8"?>
<metadataEnvelope xmlns="urn:3gpp:metadata:2005:MBMS:envelope">
    <item metadataURI="a3fa-common/apps/cta.html" version="1" contentType="text/html"/>
    <item metadataURI="a3fa-common/apps/default.html" version="1" contentType="text/html"/>
    <item metadataURI="a3fa-common/apps/trigger/instructions.png" version="1" contentType="image/png"/>
    <item metadataURI="a3fa-common/apps/trigger/alert.png" version="1" contentType="image/png"/>
    <item metadataURI="a3fa-common/index.html" version="1" contentType="text/html"/>
    <item metadataURI="a3fa-common/libs/a3fa-framework.static.min.js" version="1" contentType="application/js"/>
    <item metadataURI="a3fa-common/libs/a3fa-framework.dynamic.min.js" version="1" contentType="application/js"/>
    <item metadataURI="a3fa-common/libs/a3fa-framework.manager.min.js" version="1" contentType="application/js"/>
    <item metadataURI="a3fa-common/libs/a3fa-framework.common.min.js" version="1" contentType="application/js"/>
    <item metadataURI="a3fa-common/libs/a3fa-framework.trigger.min.js" version="1" contentType="application/js"/>
    <item metadataURI="js/alert.renderer.iframe.js" version="1" contentType="application/js"/>
    <item metadataURI="js/common.js" version="1" contentType="application/js"/>
    <item metadataURI="js/mediaalert.js" version="1" contentType="application/js"/>
    <item metadataURI="js/trigger.js" version="1" contentType="application/js"/>
    <item metadataURI="css/general-alert.png" version="1" contentType="image/png"/>
    <item metadataURI="css/styles.css" version="1" contentType="text/css"/>
    <item metadataURI="font-awesome/fonts/fontawesome-webfont.woff2" version="1" contentType="application/octet-stream"/>
    <item metadataURI="ksnv/config/default-settings.json" version="1" contentType="application/json"/>
    <item metadataURI="ksnv/config/menu-settings.json" version="1" contentType="application/json"/>
    <item metadataURI="ksnv/css/default.css" version="1" contentType="text/css"/>
    <item metadataURI="ksnv/css/skin.css" version="1" contentType="text/css"/>
    <item metadataURI="ksnv/dynamic/menu/default-menu.json" version="1" contentType="application/json"/>
    <item metadataURI="ksnv/dynamic/alert/alerts-feed.json" version="1" contentType="application/json"/>
    <item metadataURI="ksnv/dynamic/trigger/ksnv.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/dynamic/trigger/ksnv2.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/player/instructions.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/player/pause.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/player/stop.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/player/time-bar.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/player/play.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/player/background.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/pics/ksnv-logo-2020-06-10-update.jpg" version="1" contentType="image/jpeg"/>
    <item metadataURI="ksnv/img/pics/default.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/trigger/alert.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/button-prev.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/button-over.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/msg_blue.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/text-focus.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/channel.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/msg_ok_blue.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/bar.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/strip-bg.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/msg_black.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/image-blur.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/msg_ok_black.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/play-blur.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/header.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/image-focus.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/play-focus.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/text-blur.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/img/home-menu/button.png" version="1" contentType="image/png"/>
    <item metadataURI="ksnv/index.html" version="1" contentType="text/html"/>
    <item metadataURI="ksnv/tos/terms-of-service.html" version="1" contentType="text/html"/>
    <item metadataURI="ksnv/tos/privacy-policy.html" version="1" contentType="text/html"/>
    <item metadataURI="index.html" version="1" contentType="text/html"/>
    <item metadataURI="conf/app.json" version="1" contentType="application/json"/>
</metadataEnvelope>

     */
    public String mbmsEnvelopeRawXml;

    /*
    item: 0, path: a3fa-common/apps/cta.html, size: 521, content_type: text/html, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 1, path: a3fa-common/apps/default.html, size: 654, content_type: text/html, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 2, path: a3fa-common/apps/trigger/instructions.png, size: 58726, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 3, path: a3fa-common/apps/trigger/alert.png, size: 80590, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 4, path: a3fa-common/index.html, size: 4564, content_type: text/html, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 5, path: a3fa-common/libs/a3fa-framework.static.min.js, size: 981979, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 6, path: a3fa-common/libs/a3fa-framework.dynamic.min.js, size: 6513, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 7, path: a3fa-common/libs/a3fa-framework.manager.min.js, size: 606643, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 8, path: a3fa-common/libs/a3fa-framework.common.min.js, size: 85516, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 9, path: a3fa-common/libs/a3fa-framework.trigger.min.js, size: 16815, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 10, path: js/alert.renderer.iframe.js, size: 3993, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 11, path: js/common.js, size: 87974, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 12, path: js/mediaalert.js, size: 5674, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 13, path: js/trigger.js, size: 42651, content_type: application/js, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 14, path: css/general-alert.png, size: 874, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9010:item: 15, path: css/styles.css, size: 3116, content_type: text/css, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 16, path: font-awesome/fonts/fontawesome-webfont.woff2, size: 77160, content_type: application/octet-stream, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 17, path: ksnv/config/default-settings.json, size: 2912, content_type: application/json, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 18, path: ksnv/config/menu-settings.json, size: 2467, content_type: application/json, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 19, path: ksnv/css/default.css, size: 7793, content_type: text/css, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 20, path: ksnv/css/skin.css, size: 1768, content_type: text/css, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 21, path: ksnv/dynamic/menu/default-menu.json, size: 2190, content_type: application/json, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 22, path: ksnv/dynamic/alert/alerts-feed.json, size: 6129, content_type: application/json, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 23, path: ksnv/dynamic/trigger/ksnv.png, size: 10731, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 24, path: ksnv/dynamic/trigger/ksnv2.png, size: 11605, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 25, path: ksnv/img/player/instructions.png, size: 20038, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 26, path: ksnv/img/player/pause.png, size: 13688, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 27, path: ksnv/img/player/stop.png, size: 12682, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 28, path: ksnv/img/player/time-bar.png, size: 1055, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 29, path: ksnv/img/player/play.png, size: 14133, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 30, path: ksnv/img/player/background.png, size: 49145, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 31, path: ksnv/img/pics/ksnv-logo-2020-06-10-update.jpg, size: 78026, content_type: image/jpeg, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 32, path: ksnv/img/pics/default.png, size: 2634, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 33, path: ksnv/img/trigger/alert.png, size: 80590, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 34, path: ksnv/img/home-menu/button-prev.png, size: 6123, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 35, path: ksnv/img/home-menu/button-over.png, size: 8091, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 36, path: ksnv/img/home-menu/msg_blue.png, size: 22079, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 37, path: ksnv/img/home-menu/text-focus.png, size: 2766, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9011:item: 38, path: ksnv/img/home-menu/channel.png, size: 3577, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 39, path: ksnv/img/home-menu/msg_ok_blue.png, size: 3525, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 40, path: ksnv/img/home-menu/bar.png, size: 660, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 41, path: ksnv/img/home-menu/strip-bg.png, size: 43097, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 42, path: ksnv/img/home-menu/msg_black.png, size: 3340, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 43, path: ksnv/img/home-menu/image-blur.png, size: 2064, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 44, path: ksnv/img/home-menu/msg_ok_black.png, size: 1630, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 45, path: ksnv/img/home-menu/play-blur.png, size: 2025, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 46, path: ksnv/img/home-menu/header.png, size: 3119, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 47, path: ksnv/img/home-menu/image-focus.png, size: 2795, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 48, path: ksnv/img/home-menu/play-focus.png, size: 2705, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 49, path: ksnv/img/home-menu/text-blur.png, size: 2016, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 50, path: ksnv/img/home-menu/button.png, size: 11151, content_type: image/png, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 51, path: ksnv/index.html, size: 2930, content_type: text/html, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 52, path: ksnv/tos/terms-of-service.html, size: 4481, content_type: text/html, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 53, path: ksnv/tos/privacy-policy.html, size: 40170, content_type: text/html, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 54, path: index.html, size: 7334, content_type: text/html, valid_from: (null), valid_until: (null), version: 1, next_url: (null)
atsc3_route_package_util: 232:DEBUG:1595861703.9012:item: 55, path: conf/app.json, size: 2076, content_type: (null), valid_from: (null), valid_until: (null), version: 0, next_url: (null)

     */

    public List<MultipartRelatedPayload> multipartRelatedPayloadList = new ArrayList<>();

    public class MultipartRelatedPayload {
        public String contentLocation;
        public String contentType;
        public String validFrom;
        public String validUntil;
        public int version;
        public String nextUrl;
        public String availAt;
        public int extractedSize;

    }

}