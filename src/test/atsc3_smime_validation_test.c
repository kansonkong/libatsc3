//
//  atsc3_smime_validation_test.c
//  libatsc3
//
//  Created by Jason Justman on 11/17/20.
//  Copyright © 2020 Jason Justman. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "../atsc3_utils.h"
#include "../atsc3_fdt_parser.h"
#include "../atsc3_smime_utils.h"

#define _ATSC3_SMIME_UTILS_TEST_ERROR(...)   printf("%s:%d:ERROR:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SMIME_UTILS_TEST_WARN(...)    printf("%s:%d:WARN:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SMIME_UTILS_TEST_INFO(...)    printf("%s:%d:INFO:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SMIME_UTILS_TEST_DEBUG(...)   printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);
#define _ATSC3_SMIME_UTILS_TEST_TRACE(...)   //printf("%s:%d:DEBUG:",__FILE__,__LINE__);_ATSC3_UTILS_PRINTLN(__VA_ARGS__);

int parse_signed_sls_with_test_certificate_noverify(const char* sls_fragment_filename, const char* signing_certificate_filename) {
	int ret = 0;

	atsc3_smime_entity_t* atsc3_smime_entity = atsc3_smime_entity_new_parse_from_file(sls_fragment_filename);
	atsc3_smime_validation_context_t* atsc3_smime_validation_context = atsc3_smime_validation_context_new(atsc3_smime_entity);
	
	atsc3_smime_validation_context_set_cms_noverify(atsc3_smime_validation_context, true);
	atsc3_smime_validation_context_certificate_payload_parse_from_file(atsc3_smime_validation_context, signing_certificate_filename);
	
	atsc3_smime_validation_context_t* atsc3_smime_validation_context_ret = atsc3_smime_validate_from_context(atsc3_smime_validation_context);
	
	_ATSC3_SMIME_UTILS_TEST_INFO("###");
	_ATSC3_SMIME_UTILS_TEST_INFO("--- Test Result for sls_fragment_filename: %s, signing_certificate_filename: %s", sls_fragment_filename, signing_certificate_filename);

	if(!atsc3_smime_validation_context_ret && !atsc3_smime_validation_context->cms_signature_valid) {
		_ATSC3_SMIME_UTILS_TEST_INFO("parse_signed_sls_with_test_certificate_noverify failed: from sls_fragment_filename: %s, signing_certificate_filename: %s failed!", sls_fragment_filename, signing_certificate_filename);
		ret = -1;
	} else if(!atsc3_smime_validation_context_ret || !atsc3_smime_validation_context->cms_signature_valid) {
		_ATSC3_SMIME_UTILS_TEST_ERROR("parse_signed_sls_with_test_certificate_noverify RETURN mismatch (%p) with cms_signature_valid: (%d), from: sls_fragment_filename: %s, signing_certificate_filename: %s failed!",
									  atsc3_smime_validation_context_ret,
									  atsc3_smime_validation_context->cms_signature_valid,
									  sls_fragment_filename, signing_certificate_filename);
		ret = -31337;
	} else if(atsc3_smime_validation_context_ret && atsc3_smime_validation_context->cms_signature_valid) {
		_ATSC3_SMIME_UTILS_TEST_TRACE("parse_signed_sls_with_test_certificate_noverify success: from: sls_fragment_filename: %s, signing_certificate_filename: %s, cms_verified_extracted_mime_entity_len: %d, cms_verified_extracted_mime_entity:\n%s",
									 sls_fragment_filename, signing_certificate_filename,
									 atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_size,
									 atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_buffer);
		_ATSC3_SMIME_UTILS_TEST_INFO("parse_signed_sls_with_test_certificate_noverify SUCCESS: from: sls_fragment_filename: %s, signing_certificate_filename: %s, cms_verified_extracted_mime_entity_len: %d, cms_verified_extracted_mime_entity[0-4]: %c%c%c%c",
			sls_fragment_filename, signing_certificate_filename,
			atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_size,
			atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_buffer[0],
			atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_buffer[1],
			atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_buffer[2],
			atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_buffer[3]);

		ret = 0;
	} else {
		_ATSC3_SMIME_UTILS_TEST_ERROR("parse_signed_sls_with_test_certificate_noverify: ERROR!");
		ret = -1000000;
	}

	_ATSC3_SMIME_UTILS_TEST_INFO("--- End Test Result for sls_fragment_filename: %s, signing_certificate_filename: %s", sls_fragment_filename, signing_certificate_filename);
	_ATSC3_SMIME_UTILS_TEST_INFO("###");

	return ret;
}

int main(int argc, char* argv[] ) {


	//manual test:  ./openssl cms -verify -binary -in testdata/2020-11-17-signed-sls/239.1.120.120.49152.0-458826 -noverify -certfile testdata/2020-11-17-signed-sls/signal-signing-enensys-SMT.crt

	//A/360:2019-Amend_No_1: 5.2.2.4 Signatures for Service Level Signaling carried over ROUTE/DASH
	//enensys sample 1 (SLS and cert-4.crt)
	_ATSC3_SMIME_UTILS_TEST_INFO("--- Start: static test smime and crt payloads from 2020-11-17-signed-sls");

	parse_signed_sls_with_test_certificate_noverify("testdata/2020-11-17-signed-sls/test-1-sls-smime.bin", "testdata/2020-11-17-signed-sls/test-1-cert-signer-e.crt");
	_ATSC3_SMIME_UTILS_TEST_INFO("--- Next: ");
	
	//enensys sample from ROUTE_SLS1.pcap, sls2.zip/signal-signing-enensys-SMT.crt
	parse_signed_sls_with_test_certificate_noverify("testdata/2020-11-17-signed-sls/239.1.120.120.49152.0-458826", "testdata/2020-11-17-signed-sls/signal-signing-enensys-SMT.crt");

	_ATSC3_SMIME_UTILS_TEST_INFO("--- End: static test smime and crt payloads from 2020-11-17-signed-sls");

	return 0;
}

