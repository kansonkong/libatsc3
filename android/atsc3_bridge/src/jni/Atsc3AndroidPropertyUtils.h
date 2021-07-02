//
// Created by Jason Justman on 8/10/20.
//

//#include <android/log.h>
#include <sys/system_properties.h>

#ifndef LIBATSC3_ANDROID_ATSC3ANDROIDPROPERTYUTILS_H
#define LIBATSC3_ANDROID_ATSC3ANDROIDPROPERTYUTILS_H

// ...
typedef struct libatsc3_android_system_properties {
    char boot_serialno_str[PROP_VALUE_MAX];             //ro.boot.serialno - MAY BE NULL for non-MarkONE / non-rooted handsets
    char serialno_str[PROP_VALUE_MAX];                  //ro.serialno - MAY BE NULL for non-MarkONE / non-rooted handsets

    char board_platform_str[PROP_VALUE_MAX];            //ro.board.platform
    char build_description_str[PROP_VALUE_MAX];         //ro.build.description
    char build_flavor_str[PROP_VALUE_MAX];              //ro.build.flavor
    char build_product_str[PROP_VALUE_MAX];             //ro.build.product
    char build_version_incremental_str[PROP_VALUE_MAX]; //ro.build.version.incremental
    char product_cpu_abi_str[PROP_VALUE_MAX];           //ro.product.cpu.abi
    char product_mfg_str[PROP_VALUE_MAX];               //ro.product.manufacturer

    char build_version_release_str[PROP_VALUE_MAX];     //ro.build.version.release
    int android_version;

    char sdk_ver_str[PROP_VALUE_MAX];                   //ro.build.version.sdk
    int sdk_ver;



} libatsc3_android_system_properties_t;

int libatsc3_android_test_populate_system_properties(libatsc3_android_system_properties_t* libatsc3_android_system_properties) {
    int ret = 0;

    //boot.serialno
    if (__system_property_get("ro.boot.serialno", libatsc3_android_system_properties->boot_serialno_str)) {
        printf("%s:found matching ro.boot.serialno prop at: %s",  __FILE__, libatsc3_android_system_properties->boot_serialno_str);
    }

    //ro.serialno
    if (__system_property_get("ro.serialno", libatsc3_android_system_properties->serialno_str)) {
        printf("%s:found matching ro.serialno prop at: %s",  __FILE__, libatsc3_android_system_properties->serialno_str);
    }

    //ro.board.platform -> board_platform_str
    if (__system_property_get("ro.board.platform", libatsc3_android_system_properties->board_platform_str)) {
        printf("%s:found matching ro.board.platform prop at: %s",  __FILE__, libatsc3_android_system_properties->board_platform_str);
    }

    //ro.build.description
    if (__system_property_get("ro.build.description", libatsc3_android_system_properties->build_description_str)) {
        printf("%s:found matching ro.build.description prop at: %s",  __FILE__, libatsc3_android_system_properties->build_description_str);
    }

    //ro.build.flavor
    if (__system_property_get("ro.build.flavor", libatsc3_android_system_properties->build_flavor_str)) {
        printf("%s:found matching ro.build.flavor prop at: %s",  __FILE__, libatsc3_android_system_properties->build_flavor_str);
    }

    //ro.build.product
    if (__system_property_get("ro.build.product", libatsc3_android_system_properties->build_product_str)) {
        printf("%s:found matching ro.build.product prop at: %s",  __FILE__, libatsc3_android_system_properties->build_product_str);
    }

    //ro.build.version.incremental
    if (__system_property_get("ro.build.version.incremental", libatsc3_android_system_properties->build_version_incremental_str)) {
        printf("%s:found matching ro.build.version.incremental prop at: %s",  __FILE__, libatsc3_android_system_properties->build_version_incremental_str);
    }

    //ro.product.cpu.abi
    if (__system_property_get("ro.product.cpu.abi", libatsc3_android_system_properties->product_cpu_abi_str)) {
        printf("%s:found matching ro.product.cpu.abi prop at: %s",  __FILE__, libatsc3_android_system_properties->product_cpu_abi_str);
    }

    //product.manufacturer
    if (__system_property_get("ro.product.manufacturer", libatsc3_android_system_properties->product_mfg_str)) {
        printf("%s:found matching ro.product.manufacturer prop at: %s",  __FILE__, libatsc3_android_system_properties->product_mfg_str);
    }

    //ro.build.version.release
    if (__system_property_get("ro.build.version.release", libatsc3_android_system_properties->build_version_release_str)) {
        printf("%s:found matching ro.build.version.release prop at: %s",  __FILE__, libatsc3_android_system_properties->build_version_release_str);
        libatsc3_android_system_properties->android_version = atoi(libatsc3_android_system_properties->build_version_release_str);
    }

    if (__system_property_get("ro.build.version.sdk", libatsc3_android_system_properties->sdk_ver_str)) {
        printf("%s:found matching prop at: %s", __FILE__, libatsc3_android_system_properties->sdk_ver_str);
        libatsc3_android_system_properties->sdk_ver = atoi(libatsc3_android_system_properties->sdk_ver_str);
    }

    return ret;
}


#endif //LIBATSC3_ANDROID_ATSC3ANDROIDPROPERTYUTILS_H
