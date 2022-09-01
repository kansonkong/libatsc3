//
//  atsc3_cms_utils.c
//  libatsc3
//
//  Created by Jason Justman on 11/17/20.
//  Copyright © 2020 Jason Justman. All rights reserved.
//

#include "atsc3_cms_utils.h"

int _ATSC3_CMS_UTILS_INFO_ENABLED  = 1;
int _ATSC3_CMS_UTILS_DEBUG_ENABLED = 0;
int _ATSC3_CMS_UTILS_TRACE_ENABLED = 0;

char* ATSC3_CMS_UTILS_BEGIN_CERTIFICATE = "\n-----BEGIN CERTIFICATE-----\n";
char* ATSC3_CMS_UTILS_END_CERTIFICATE = "\n-----END CERTIFICATE-----\n";


atsc3_cms_entity_t* atsc3_cms_entity_new() {
	atsc3_cms_entity_t* atsc3_cms_entity = calloc(1, sizeof(atsc3_cms_entity_t));
	
	return atsc3_cms_entity;
}

atsc3_cms_entity_t* atsc3_cms_entity_new_parse_from_file(const char* multipart_entity_filename) {
	atsc3_cms_entity_t* atsc3_cms_entity = atsc3_cms_entity_new();
//
//	atsc3_cms_entity->raw_cms_payload_filename = strlcopy(multipart_entity_filename);
//	atsc3_cms_entity->raw_cms_payload = block_Read_from_filename(multipart_entity_filename);
//
//	if(!atsc3_cms_entity->raw_cms_payload) {
//		_ATSC3_CMS_UTILS_ERROR("atsc3_cms_entity_new_parse_from_file: unable to read raw cms payload from: %s", multipart_entity_filename);
//		return NULL;
//	}
//	block_Rewind(atsc3_cms_entity->raw_cms_payload);
//
//	//parse header
//	//parse mime entity
//	//parse signature
//
	return atsc3_cms_entity;
}

void atsc3_cms_entity_free(atsc3_cms_entity_t** atsc3_cms_entity_p) {
	if(atsc3_cms_entity_p) {
		atsc3_cms_entity_t* atsc3_cms_entity = *atsc3_cms_entity_p;
		if(atsc3_cms_entity) {


			if(atsc3_cms_entity->signature) {
				block_Destroy(&atsc3_cms_entity->signature);
			}

			if(atsc3_cms_entity->raw_binary_payload) {
				block_Destroy(&atsc3_cms_entity->raw_binary_payload);
			}

			free(atsc3_cms_entity);
			atsc3_cms_entity = NULL;
		}
		
		*atsc3_cms_entity_p = NULL;
	}
}

atsc3_cms_validation_context_t* atsc3_cms_validation_context_new(atsc3_cms_entity_t* atsc3_cms_entity) {
	atsc3_cms_validation_context_t* atsc3_cms_validation_context = calloc(1, sizeof(atsc3_cms_validation_context_t));
	atsc3_cms_validation_context->atsc3_cms_entity = atsc3_cms_entity;
	
	return atsc3_cms_validation_context;
}

void atsc3_cms_validation_context_free(atsc3_cms_validation_context_t** atsc3_cms_validation_context_p) {
	if(atsc3_cms_validation_context_p) {
		atsc3_cms_validation_context_t* atsc3_cms_validation_context = *atsc3_cms_validation_context_p;
		if(atsc3_cms_validation_context) {
			
			if(atsc3_cms_validation_context->atsc3_cms_entity) {
				atsc3_cms_entity_free(&atsc3_cms_validation_context->atsc3_cms_entity);
			}
			
//			if(atsc3_cms_validation_context->certificate_payload) {
//				block_Destroy(&atsc3_cms_validation_context->certificate_payload);
//			}
			
			free(atsc3_cms_validation_context);
			atsc3_cms_validation_context = NULL;
		}
		
		*atsc3_cms_validation_context_p = NULL;
	}
}


void atsc3_cms_validation_context_set_cms_noverify(atsc3_cms_validation_context_t* atsc3_cms_validation_context, bool noverify_flag) {
	atsc3_cms_validation_context->cms_noverify = noverify_flag;
	if(atsc3_cms_validation_context->cms_noverify) {
		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validation_context_set_cms_noverify: context: %p, setting noverify to TRUE! DO NOT DO THIS!", atsc3_cms_validation_context);
	}
}

void atsc3_cms_validation_context_set_cms_no_content_verify(atsc3_cms_validation_context_t* atsc3_cms_validation_context, bool no_content_verify_flag) {
	atsc3_cms_validation_context->cms_no_content_verify = no_content_verify_flag;
	if(atsc3_cms_validation_context->cms_no_content_verify) {
		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validation_context_set_cms_no_content_verify: context: %p, setting NO_CONTENT_VERIFY to TRUE!  Disabling all CMS validation", atsc3_cms_validation_context);
	}
}


atsc3_cms_validation_context_t* atsc3_cms_validation_context_certificate_payload_parse_from_file(atsc3_cms_validation_context_t* atsc3_cms_validation_context, const char* signing_certificate_filename) {
//	if(atsc3_cms_validation_context->certificate_payload) {
//		block_Destroy(&atsc3_cms_validation_context->certificate_payload);
//	}
//
//	if(signing_certificate_filename) {
//		atsc3_cms_validation_context->certificate_payload = block_Read_from_filename(signing_certificate_filename);
//	} else {
//		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validation_context_certificate_payload_parse_from_file: no signing_certificate_filename provided, using empty cert!");
//		atsc3_cms_validation_context->certificate_payload = block_Alloc(0);
//	}
//
//	if(!atsc3_cms_validation_context->certificate_payload) {
//		_ATSC3_CMS_UTILS_ERROR("atsc3_cms_validation_context_certificate_payload_parse_from_file: unable to read signing_certificate_filename from: %s", signing_certificate_filename);
//		return NULL;
//	}
//	block_Rewind(atsc3_cms_validation_context->certificate_payload);
//
	return atsc3_cms_validation_context;
}

/*
 
 From LLS: CDT emission
 
 Data:
	   Version: 3 (0x2)
	   Serial Number: 11185070432223617944 (0x9b395a4c0c69e398)
   Signature Algorithm: sha256WithRSAEncryption
	   Issuer: C=US, O=A3SA, OU=Root 2020, CN=A3SA Root 2020
	   Validity
		   Not Before: Sep 18 01:58:07 2020 GMT
		   Not After : Sep 17 01:58:07 2025 GMT
	   Subject: C=US, O=A3SA, OU=Root 2020, CN=A3SA Root 2020
 */
atsc3_cms_validation_context_t* atsc3_cms_validation_context_certificate_payload_parse_from_file_with_root_fallback(atsc3_cms_validation_context_t* atsc3_cms_validation_context, const char* signing_certificate_filename) {
//	if(atsc3_cms_validation_context->certificate_payload) {
//		block_Destroy(&atsc3_cms_validation_context->certificate_payload);
//	}
//
//	if(signing_certificate_filename) {
//		atsc3_cms_validation_context->certificate_payload = block_Read_from_filename(signing_certificate_filename);
//	} else {
//		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validation_context_certificate_payload_parse_from_file_with_root_fallback: no signing_certificate_filename provided, using root fallback cert!");
//		atsc3_cms_validation_context->certificate_payload = block_Alloc(strlen(ATSC3_CMS_UTILS_CDT_A3SA_ROOT_2020_CERT) + 1);
//		block_Write(atsc3_cms_validation_context->certificate_payload, (const uint8_t*)ATSC3_CMS_UTILS_CDT_A3SA_ROOT_2020_CERT, strlen(ATSC3_CMS_UTILS_CDT_A3SA_ROOT_2020_CERT));
//	}
//
//
//	if(!atsc3_cms_validation_context->certificate_payload) {
//		_ATSC3_CMS_UTILS_ERROR("atsc3_cms_validation_context_certificate_payload_parse_from_file_with_root_fallback: unable to read signing_certificate_filename from: %s", signing_certificate_filename);
//		return NULL;
//	}
//	block_Rewind(atsc3_cms_validation_context->certificate_payload);
//
	return atsc3_cms_validation_context;
}

/*
 
 1.) read in full cms payload

	mbms_toi_filename = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, 0, mbms_toi);

	block_Read_from_filename
 
 2.) extract out cms / multipart signature
 
 3.) process fragments as usual

	-> atsc3_mime_multipart_related_parser
 
	atsc3_sls_metadata_fragments_pending = atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(fp_mbms);

 
 
 */

atsc3_cms_validation_context_t* atsc3_cms_validate_from_context(atsc3_cms_validation_context_t* atsc3_cms_validation_context) {
	atsc3_cms_validation_context_t* atsc3_cms_validation_context_return = atsc3_cms_validation_context;
	
	BIO 	*signature_binary_der_in = NULL;
	BIO 	*payload_binary_in = NULL;

	BIO 	*extracted_payload_out = NULL;
	long 	extracted_payload_out_len = 0;
	
	BIO 	*cacert_cdt_payload_in = NULL;

	X509_STORE 	*st_root = NULL;
	X509 		*cacert_root = NULL;
	X509 		*cacert_intermediate = NULL;

	X509 		*cacert_cdt = NULL;
	
	STACK_OF(X509) *pcerts = NULL;
	CMS_ContentInfo *cms = NULL;
	
	unsigned int cms_verify_flags = 0;

	int ret = 1;

	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	ERR_load_crypto_strings();

	/* jjustman-2020-11-17: TODO: pin ROOT CA  */
	st_root = X509_STORE_new();
	X509_STORE_set_purpose(st_root, X509_PURPOSE_ANY);
	
	/*
	 * add our:
	 * 	ATSC3_A360_CERTIFICATES_PEARL_A3SA_ROOT_CERT_SN_0569 and
	 * 	ATSC3_A360_CERTIFICATES_PEARL_A3SA_INTERMEDIATE_SIGNING_CA_2_SN_A0D3
	 *
	 */
	BIO * cacert_root_bio = BIO_new_mem_buf(ATSC3_A360_CERTIFICATES_PEARL_A3SA_ROOT_CERT_SN_0569, strlen(ATSC3_A360_CERTIFICATES_PEARL_A3SA_ROOT_CERT_SN_0569));

	cacert_root = PEM_read_bio_X509(cacert_root_bio, NULL, 0, NULL);
	 if (!cacert_root) {
		goto err;
	 }
	 
	 if (!X509_STORE_add_cert(st_root, cacert_root)) {
		goto err;
	 }

	BIO * cacert_ca_chain_signing_ca_2 = BIO_new_mem_buf(ATSC3_A360_CERTIFICATES_PEARL_A3SA_INTERMEDIATE_SIGNING_CA_2_SN_A0D3, strlen(ATSC3_A360_CERTIFICATES_PEARL_A3SA_INTERMEDIATE_SIGNING_CA_2_SN_A0D3));
	cacert_intermediate = PEM_read_bio_X509(cacert_ca_chain_signing_ca_2, NULL, 0, NULL);
	if (!cacert_intermediate) {
		goto err;
	}

	if (!X509_STORE_add_cert(st_root, cacert_intermediate)) {
		goto err;
	}

	//intermediate certs from CDT table or testing
	pcerts = sk_X509_new_null();

	for (int i = 0; i < atsc3_cms_validation_context->transients.atsc3_certification_data->atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.count; i++) {
		atsc3_certification_data_to_be_signed_data_certificates_t *atsc3_certification_data_to_be_signed_data_certificates = atsc3_cms_validation_context->transients.atsc3_certification_data->atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.data[i];
		block_Rewind(atsc3_certification_data_to_be_signed_data_certificates->base64_payload);
		block_t* fixup_cdt_table_entry_blockt = block_Alloc(1024);
		block_Write(fixup_cdt_table_entry_blockt, (const uint8_t*) ATSC3_A360_CERTIFICATE_UTILS_BEGIN_CERTIFICATE, strlen(ATSC3_A360_CERTIFICATE_UTILS_BEGIN_CERTIFICATE));
		block_AppendFromSrciPosToSizeAndMoveIptrs(fixup_cdt_table_entry_blockt, atsc3_certification_data_to_be_signed_data_certificates->base64_payload, block_Remaining_size(atsc3_certification_data_to_be_signed_data_certificates->base64_payload));
		block_Write(fixup_cdt_table_entry_blockt, (const uint8_t*) ATSC3_A360_CERTIFICATE_UTILS_END_CERTIFICATE, strlen(ATSC3_A360_CERTIFICATE_UTILS_END_CERTIFICATE));
		block_Rewind(fixup_cdt_table_entry_blockt);
		uint8_t* cert_cdt_in_bio_mem_buf = block_Get(fixup_cdt_table_entry_blockt);
		uint32_t cert_cdt_in_bio_mem_buf_len = block_Remaining_size(fixup_cdt_table_entry_blockt);
		BIO * to_be_signed_payload = BIO_new_mem_buf(cert_cdt_in_bio_mem_buf, cert_cdt_in_bio_mem_buf_len);
		X509*	to_be_signed_x509 = PEM_read_bio_X509(to_be_signed_payload, NULL, 0, NULL);

		if (!to_be_signed_x509) {
			_ATSC3_CMS_UTILS_WARN("atsc3_cms_validate_from_context: PEM_read_bio_X509 failed, cacert_cdt_payload_in: %p, PEM_read_X509 returned: %p",
								  cacert_cdt_payload_in, cacert_cdt);
			goto err;
		}
		sk_X509_push(pcerts, to_be_signed_x509);
	}

	/* convert our block_t raw cms payload to BIO mem buff for input */
	block_Rewind(atsc3_cms_validation_context->atsc3_cms_entity->signature);
	uint8_t* signature_binary_der_in_mem_buf = block_Get(atsc3_cms_validation_context->atsc3_cms_entity->signature);
	uint32_t signature_binary_der_in_mem_buf_len = block_Remaining_size(atsc3_cms_validation_context->atsc3_cms_entity->signature);

	signature_binary_der_in = BIO_new_mem_buf(signature_binary_der_in_mem_buf, signature_binary_der_in_mem_buf_len);
	_ATSC3_CMS_UTILS_DEBUG("atsc3_cms_validate_from_context: BIO_new_mem_buf: signed_payload_in in: %p, signed_payload_in_bio_mem_buf_len: %d, signed_payload_in_bio_mem_buf:\n%s",
						   signature_binary_der_in, signature_binary_der_in_mem_buf_len, signature_binary_der_in_mem_buf);

	if (!signature_binary_der_in) {
	   goto err;
	}
	
	/* parse DER signature portion of the CMS message */
	
	cms = d2i_CMS_bio(signature_binary_der_in, &cms);

	if (!cms) {
		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validate_from_context:CMS_read_CMS: failed!");
	   goto err;
	}

	/* convert our block_t raw cms payload to BIO mem buff for input */
	block_Rewind(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload);
	uint8_t* payload_binary_in_mem_buf = block_Get(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload);
	uint32_t payload_binary_in_mem_buf_len = block_Remaining_size(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload);

	payload_binary_in = BIO_new_mem_buf(payload_binary_in_mem_buf, payload_binary_in_mem_buf_len);
	_ATSC3_CMS_UTILS_DEBUG("atsc3_cms_validate_from_context: BIO_new_mem_buf: payload_binary_in in: %p, payload_binary_in_mem_buf_len: %d, payload_binary_in_mem_buf:\n%s",
						   payload_binary_in, payload_binary_in_mem_buf_len, payload_binary_in_mem_buf);

	if (!payload_binary_in) {
		goto err;
	}

	extracted_payload_out = BIO_new(BIO_s_mem());

	if (!extracted_payload_out)
	   goto err;

	//https://www.openssl.org/docs/man1.1.0/man3/CMS_verify.html
	//will still fail if there are no signers in the CA chain, i.e. only root is not good enough to pass..
	if(atsc3_cms_validation_context->cms_no_content_verify) {
		cms_verify_flags = CMS_BINARY | CMS_NOVERIFY | CMS_NO_SIGNER_CERT_VERIFY | CMS_NOCRL | CMS_NO_ATTR_VERIFY | CMS_NO_CONTENT_VERIFY;
	} else if(atsc3_cms_validation_context->cms_noverify) {
		cms_verify_flags = CMS_BINARY | CMS_NOVERIFY | CMS_NO_SIGNER_CERT_VERIFY | CMS_NOCRL | CMS_NO_ATTR_VERIFY;
	} else {
		cms_verify_flags = CMS_BINARY;
	}
	
	if (!CMS_verify(cms, pcerts, st_root, payload_binary_in, extracted_payload_out, cms_verify_flags)) {
	   _ATSC3_CMS_UTILS_WARN("atsc3_cms_validate_from_context:CMS_verify: verification failure");
		atsc3_cms_validation_context->cms_signature_valid = false;

	//	block_Write_to_filename(atsc3_cms_validation_context->certificate_payload, "cert_payload.pem");

		block_Write_to_filename(atsc3_cms_validation_context->atsc3_cms_entity->signature, "raw_binary_payload_signature.der");
		block_Write_to_filename(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload, "raw_binary_payload.data");
	} else {
		_ATSC3_CMS_UTILS_DEBUG("atsc3_cms_validate_from_context: verification successful");
		atsc3_cms_validation_context->cms_signature_valid = true;
	}

//	//copy this to our context->cms_entity->cms_verified_extracted_mime_entity
//	char *extracted_payload_out_char_p = NULL;
//	extracted_payload_out_len = BIO_get_mem_data(extracted_payload_out, &extracted_payload_out_char_p);
//
//	atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_mime_entity = block_Alloc(extracted_payload_out_len);
//	block_Write(atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_mime_entity, (uint8_t*) extracted_payload_out_char_p, extracted_payload_out_len);
//	block_Rewind(atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_mime_entity);
//
//	_ATSC3_CMS_UTILS_DEBUG("atsc3_cms_validate_from_context: BIO_get_mem_data: extracted_payload_out_len: %d, extracted_payload_out_char_p:\n%s",
//							 atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_mime_entity->p_size,
//							 atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_mime_entity->p_buffer);

err:

	if (!atsc3_cms_validation_context->cms_signature_valid) {
		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validate_from_context: error verifying data, errors:");
		ERR_print_errors_fp(stdout);
		atsc3_cms_validation_context_return = NULL;
	}

	if(signature_binary_der_in) {
		BIO_free(signature_binary_der_in);
	}

	if(payload_binary_in) {
		BIO_free(payload_binary_in);
	}

	if(extracted_payload_out) {
		BIO_free(extracted_payload_out);
	}

	if(cacert_cdt_payload_in) {
		BIO_free(cacert_cdt_payload_in);
	}

	if(st_root) {
		X509_STORE_free(st_root);
	}

	if(cacert_cdt) {
		X509_free(cacert_cdt);
	}

	if(cacert_intermediate) {
		X509_free(cacert_intermediate);
	}

	if(cacert_root) {
		X509_free(cacert_root);
	}

	if(cms) {
		CMS_ContentInfo_free(cms);
	}


	return atsc3_cms_validation_context_return;
}

