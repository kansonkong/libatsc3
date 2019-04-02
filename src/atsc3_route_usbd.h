/*
 * atsc3_route_usbd.h
 *
 *  Created on: Apr 2, 2019
 *      Author: jjustman
 *
 *
 *
 *<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:routeusd="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ROUTEUSD/1.0/" targetNamespace="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ROUTEUSD/1.0/" elementFormDefault="qualified">
	<xs:import namespace="http://www.w3.org/XML/1998/namespace" schemaLocation="W3C/xml.xsd"/>
	<xs:element name="BundleDescriptionROUTE" type="routeusd:BundleDescriptionROUTEType"/>
	<xs:complexType name="BundleDescriptionROUTEType">
		<xs:sequence>
			<xs:element name="UserServiceDescription" type="routeusd:UserServiceDescriptionType"/>
			<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
		</xs:sequence>
	</xs:complexType>
	<xs:complexType name="UserServiceDescriptionType">
		<xs:sequence>
			<xs:element name="Name" type="routeusd:NameType" minOccurs="0" maxOccurs="unbounded"/>
			<xs:element name="ServiceLanguage" type="routeusd:ServiceLangType" minOccurs="0" maxOccurs="unbounded"/>
			<xs:element name="DeliveryMethod" type="routeusd:DeliveryMethodType" minOccurs="0" maxOccurs="unbounded"/>
			<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
		</xs:sequence>
		<xs:attribute name="serviceId" type="xs:unsignedShort" use="required"/>
		<xs:attribute name="serviceStatus" type="xs:boolean" default="true"/>
		<xs:anyAttribute processContents="strict"/>
	</xs:complexType>
	<xs:complexType name="NameType">
		<xs:simpleContent>
			<xs:extension base="xs:string">
				<xs:attribute ref="xml:lang" use="required"/>
				<xs:anyAttribute processContents="strict"/>
			</xs:extension>
		</xs:simpleContent>
	</xs:complexType>
	<xs:complexType name="ServiceLangType">
		<xs:simpleContent>
			<xs:extension base="xs:string">
				<xs:anyAttribute processContents="strict"/>
			</xs:extension>
		</xs:simpleContent>
	</xs:complexType>
	<xs:complexType name="DeliveryMethodType">
		<xs:sequence>
			<xs:element name="BroadcastAppService" type="routeusd:AppServiceType" minOccurs="0" maxOccurs="unbounded"/>
			<xs:element name="UnicastAppService" type="routeusd:AppServiceType" minOccurs="0" maxOccurs="unbounded"/>
			<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
		</xs:sequence>
		<xs:anyAttribute processContents="strict"/>
	</xs:complexType>
	<xs:complexType name="AppServiceType">
		<xs:sequence>
			<xs:element name="BasePattern" type="xs:string" maxOccurs="unbounded"/>
			<xs:any namespace="##other" processContents="strict" minOccurs="0" maxOccurs="unbounded"/>
		</xs:sequence>
		<xs:anyAttribute processContents="strict"/>
	</xs:complexType>
</xs:schema>
 *
 *
<?xml version="1.0" encoding="UTF-8"?>
<BundleDescriptionROUTE
	xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ROUTEUSD/1.0/"
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation= "tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ROUTEUSD/1.0/ ROUTEUSD-1.0-20170920.xsd">

	<UserServiceDescription serviceId="44" serviceStatus="true">
		<Name xml:lang="en">KUSR</Name>
		<ServiceLanguage>en</ServiceLanguage>
		<DeliveryMethod>
			<BroadcastAppService>
				<BasePattern>http://kusr.com/KUSR</BasePattern>
			</BroadcastAppService>
			<UnicastAppService>
				<BasePattern>http://kusr.com</BasePattern>
			</UnicastAppService>
		</DeliveryMethod>
	</UserServiceDescription>
</BundleDescriptionROUTE>
 */

#include <stdbool.h>

#ifndef ATSC3_ROUTE_USBD_H_
#define ATSC3_ROUTE_USBD_H_

#include "atsc3_utils.h"
#include "atsc3_vector_builder.h"


/**
 *
 *
atsc3_mime_multipart_related_parser.c:318:DEBUG:type     : application/route-usd+xml
atsc3_mime_multipart_related_parser.c:319:DEBUG:location : usbd.xml
atsc3_mime_multipart_related_parser.c:320:DEBUG:payload  :
<?xml version="1.0" encoding="UTF-8"?>
<BundleDescriptionROUTE xmlns="tag:atsc.org,2016:XMLSchemas/ATSC3/Delivery/ROUTEUSD/1.0/">
    <UserServiceDescription serviceId="1">
        <Name lang="eng">ATCst1</Name>
        <ServiceLanguage>eng</ServiceLanguage>
        <DeliveryMethod>
            <BroadcastAppService>
                <BasePattern>test-0-</BasePattern>
                <BasePattern>test-1-</BasePattern>
            </BroadcastAppService>
        </DeliveryMethod>
    </UserServiceDescription>
</BundleDescriptionROUTE>
 *
 *
 */

typedef struct atsc3_user_service_broadcast_app_service_base_pattern {
	char*		base_pattern;
} atsc3_user_service_broadcast_app_service_base_pattern_t;

typedef struct atsc3_user_service_broadcast_app_service {
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_user_service_broadcast_app_service_base_pattern);
} atsc3_user_service_broadcast_app_service_t;


typedef struct atsc3_user_service_unicast_app_service {
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_user_service_broadcast_app_service_base_pattern);
} atsc3_user_service_unicast_app_service_t;


typedef struct atsc3_user_service_delivery_method {
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_user_service_broadcast_app_service);
	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_user_service_unicast_app_service);

} atsc3_user_service_delivery_method_t;

typedef struct atsc3_user_service_description_name {
	char* lang;
	char* name;
} atsc3_user_service_description_name_t;

typedef struct atsc3_user_service_description {
	uint16_t								service_id;
	bool									service_status;
	atsc3_user_service_description_name_t	name;
	char*									service_language;

	ATSC3_VECTOR_BUILDER_STRUCT(atsc3_user_service_delivery_method);

} atsc3_user_service_description_t;


ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_user_service_description, atsc3_user_service_delivery_method)
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_user_service_delivery_method, atsc3_user_service_broadcast_app_service)
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_user_service_delivery_method, atsc3_user_service_unicast_app_service)
ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_user_service_unicast_app_service, atsc3_user_service_broadcast_app_service_base_pattern)


typedef struct atsc3_route_user_service_bundle_description {
	atsc3_user_service_description_t atsc3_user_service_description;

} atsc3_route_user_service_bundle_description_t;




#endif /* ATSC3_ROUTE_USBD_H_ */
