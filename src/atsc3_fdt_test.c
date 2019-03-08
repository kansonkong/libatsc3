/*
 * atsc3_fdt_test.c
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include <stdio.h>
#include <string.h>

#include "atsc3_fdt.h"

int parse_fdt(const char* filename) {
	int ret = 0;
	FILE *fp = fopen(filename, "r");
	xml_document_t* fdt_xml = xml_open_document(fp);
	if(fdt_xml) {
		//validate our struct
	}

	return ret;
}
int main(int argc, char* argv[] ) {
	parse_fdt("../xml_fdt/phx-fdt-0-0.xml");

	return 0;
}
