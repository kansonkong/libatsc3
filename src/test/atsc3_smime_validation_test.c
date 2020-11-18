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

int parse_signed_sls(const char* filename) {
	int ret = 0;

	atsc3_smime_entity_t* atsc3_smime_entity = atsc3_smime_entity_new_parse_from_file(filename);
	atsc3_smime_validation_context_t* atsc3_smime_validation_context = atsc3_smime_validation_context_new(atsc3_smime_entity);
	
	atsc3_smime_validate_from_context(atsc3_smime_validation_context);
	
		

	return ret;
}
int main(int argc, char* argv[] ) {


	 //parse_fdt("../test_data/xml_fdt/phx-fdt-0-0.xml");
	 parse_signed_sls("testdata/2020-11-17-signed-sls/239.1.120.120.49152.0-458826");
	 return 0;
}

