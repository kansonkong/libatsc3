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

int parse_signed_sls_with_test_certificate_noverify(const char* sls_fragment_filename, const char* signing_certificate_filename) {
	int ret = 0;

	atsc3_smime_entity_t* atsc3_smime_entity = atsc3_smime_entity_new_parse_from_file(sls_fragment_filename);
	atsc3_smime_validation_context_t* atsc3_smime_validation_context = atsc3_smime_validation_context_new(atsc3_smime_entity);
	
	atsc3_smime_validation_context_set_cms_noverify(atsc3_smime_validation_context, true);
	atsc3_smime_validation_context_certificate_payload_parse_from_file(atsc3_smime_validation_context, signing_certificate_filename);
	
	atsc3_smime_validation_context_t* atsc3_smime_validation_context_ret = atsc3_smime_validate_from_context(atsc3_smime_validation_context);
	
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
		_ATSC3_SMIME_UTILS_TEST_INFO("parse_signed_sls_with_test_certificate_noverify success: from: sls_fragment_filename: %s, signing_certificate_filename: %s, cms_verified_extracted_mime_entity_len: %d, cms_verified_extracted_mime_entity:\n%s",
									 sls_fragment_filename, signing_certificate_filename,
									 atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_size,
									 atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_buffer);
		ret = 0;
	} else {
		_ATSC3_SMIME_UTILS_TEST_ERROR("parse_signed_sls_with_test_certificate_noverify: ERROR!");
		return -1000000;
	}

	return ret;
}
int main(int argc, char* argv[] ) {


	//manual test:  ./openssl cms -verify -binary -in testdata/2020-11-17-signed-sls/239.1.120.120.49152.0-458826 -noverify -certfile testdata/2020-11-17-signed-sls/signal-signing-enensys-SMT.crt

	 parse_signed_sls_with_test_certificate_noverify("testdata/2020-11-17-signed-sls/239.1.120.120.49152.0-458826", "testdata/2020-11-17-signed-sls/signal-signing-enensys-SMT.crt");
	 return 0;
}

