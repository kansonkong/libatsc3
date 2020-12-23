//
//  atsc3_smime_utils.c
//  libatsc3
//
//  Created by Jason Justman on 11/17/20.
//  Copyright © 2020 Jason Justman. All rights reserved.
//

#include "atsc3_smime_utils.h"

int _ATSC3_SMIME_UTILS_INFO_ENABLED  = 1;
int _ATSC3_SMIME_UTILS_DEBUG_ENABLED = 1;
int _ATSC3_SMIME_UTILS_TRACE_ENABLED = 1;

atsc3_smime_entity_t* atsc3_smime_entity_new() {
	atsc3_smime_entity_t* atsc3_smime_entity = calloc(1, sizeof(atsc3_smime_entity_t));
	
	return atsc3_smime_entity;
}

atsc3_smime_entity_t* atsc3_smime_entity_new_parse_from_file(const char* multipart_entity_filename) {
	atsc3_smime_entity_t* atsc3_smime_entity = atsc3_smime_entity_new();
	
	atsc3_smime_entity->raw_smime_payload_filename = strlcopy(multipart_entity_filename);
	atsc3_smime_entity->raw_smime_payload = block_Read_from_filename(multipart_entity_filename);
	
	if(!atsc3_smime_entity->raw_smime_payload) {
		_ATSC3_SMIME_UTILS_ERROR("atsc3_smime_entity_new_parse_from_file: unable to read raw smime payload from: %s", multipart_entity_filename);
		return NULL;
	}
	block_Rewind(atsc3_smime_entity->raw_smime_payload);
	
	//parse header
	//parse mime entity
	//parse signature
	
	return atsc3_smime_entity;
}

void atsc3_smime_entity_free(atsc3_smime_entity_t** atsc3_smime_entity_p) {
	if(atsc3_smime_entity_p) {
		atsc3_smime_entity_t* atsc3_smime_entity = *atsc3_smime_entity_p;
		if(atsc3_smime_entity) {
			if(atsc3_smime_entity->mime_version) {
				freeclean((void**)&atsc3_smime_entity->mime_version);
			}
			if(atsc3_smime_entity->content_type) {
				freeclean((void**)&atsc3_smime_entity->content_type);
			}
			if(atsc3_smime_entity->micalg) {
				freeclean((void**)&atsc3_smime_entity->micalg);
			}
			if(atsc3_smime_entity->boundary) {
				freeclean((void**)&atsc3_smime_entity->boundary);
			}
			
			if(atsc3_smime_entity->raw_smime_payload_filename) {
				freeclean((void**)&atsc3_smime_entity->raw_smime_payload_filename);
			}
			
			if(atsc3_smime_entity->raw_smime_payload) {
				block_Destroy(&atsc3_smime_entity->raw_smime_payload);
			}
			
			if(atsc3_smime_entity->cms_verified_extracted_mime_entity) {
				block_Destroy(&atsc3_smime_entity->cms_verified_extracted_mime_entity);
			}
			
			if(atsc3_smime_entity->extracted_pkcs7_signature) {
				block_Destroy(&atsc3_smime_entity->extracted_pkcs7_signature);
			}
		
			free(atsc3_smime_entity);
			atsc3_smime_entity = NULL;
		}
		
		*atsc3_smime_entity_p = NULL;
	}
}

atsc3_smime_validation_context_t* atsc3_smime_validation_context_new(atsc3_smime_entity_t* atsc3_smime_entity) {
	atsc3_smime_validation_context_t* atsc3_smime_validation_context = calloc(1, sizeof(atsc3_smime_validation_context_t));
	atsc3_smime_validation_context->atsc3_smime_entity = atsc3_smime_entity;
	
	return atsc3_smime_validation_context;
}

void atsc3_smime_validation_context_free(atsc3_smime_validation_context_t** atsc3_smime_validation_context_p) {
	if(atsc3_smime_validation_context_p) {
		atsc3_smime_validation_context_t* atsc3_smime_validation_context = *atsc3_smime_validation_context_p;
		if(atsc3_smime_validation_context) {
			
			if(atsc3_smime_validation_context->atsc3_smime_entity) {
				atsc3_smime_entity_free(&atsc3_smime_validation_context->atsc3_smime_entity);
			}
			
			if(atsc3_smime_validation_context->certificate_payload) {
				block_Destroy(&atsc3_smime_validation_context->certificate_payload);
			}
			
			free(atsc3_smime_validation_context);
			atsc3_smime_validation_context = NULL;
		}
		
		*atsc3_smime_validation_context_p = NULL;
	}
}


void atsc3_smime_validation_context_set_cms_noverify(atsc3_smime_validation_context_t* atsc3_smime_validation_context, bool noverify_flag) {
	atsc3_smime_validation_context->cms_noverify = noverify_flag;
	if(atsc3_smime_validation_context->cms_noverify) {
		_ATSC3_SMIME_UTILS_WARN("atsc3_smime_validation_context_set_cms_noverify: context: %p, setting noverify to TRUE!", atsc3_smime_validation_context);
	}
}

atsc3_smime_validation_context_t* atsc3_smime_validation_context_certificate_payload_parse_from_file(atsc3_smime_validation_context_t* atsc3_smime_validation_context, const char* signing_certificate_filename) {
	if(atsc3_smime_validation_context->certificate_payload) {
		block_Destroy(&atsc3_smime_validation_context->certificate_payload);
	}
	
	atsc3_smime_validation_context->certificate_payload = block_Read_from_filename(signing_certificate_filename);
	
	if(!atsc3_smime_validation_context->certificate_payload ) {
		_ATSC3_SMIME_UTILS_ERROR("atsc3_smime_validation_context_certificate_payload_parse_from_file: unable to read signing_certificate_filename from: %s", signing_certificate_filename);
		return NULL;
	}
	block_Rewind(atsc3_smime_validation_context->certificate_payload);
	
	return atsc3_smime_validation_context;
}

/*
 
 1.) read in full smime payload

	mbms_toi_filename = alc_packet_dump_to_object_get_filename_tsi_toi(udp_flow, 0, mbms_toi);

	block_Read_from_filename
 
 2.) extract out cms / multipart signature
 
 3.) process fragments as usual

	-> atsc3_mime_multipart_related_parser
 
	atsc3_sls_metadata_fragments_pending = atsc3_mbms_envelope_to_sls_metadata_fragments_parse_from_fdt_fp(fp_mbms);

 
 
 */

atsc3_smime_validation_context_t* atsc3_smime_validate_from_context(atsc3_smime_validation_context_t* atsc3_smime_validation_context) {
	atsc3_smime_validation_context_t* atsc3_smime_validation_context_return = atsc3_smime_validation_context;
	
	BIO 	*signed_payload_in = NULL;
	
	BIO 	*extracted_payload_out = NULL;
	long 	extracted_payload_out_len = 0;
	
	BIO 	*cacert_cdt_payload_in = NULL;
	BIO 	*cont = NULL;
	
	X509_STORE 	*st_root = NULL;
	X509 		*cacert_root = NULL;
	
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
	
	/*
	 cacert_root = PEM_read_bio_X509(tbio, NULL, 0, NULL);
	 if (!cacert_root) {
		goto err;
	 }
	 
	 if (!X509_STORE_add_cert(st_root, cacert_root)) {
		goto err;
	 }
	 */

	//intermediate certs from CDT table or testing
	pcerts = sk_X509_new_null();
	
	block_Rewind(atsc3_smime_validation_context->certificate_payload);
	uint8_t* cert_cdt_in_bio_mem_buf = block_Get(atsc3_smime_validation_context->certificate_payload);
	uint32_t cert_cdt_in_bio_mem_buf_len = block_Remaining_size(atsc3_smime_validation_context->certificate_payload);

	cacert_cdt_payload_in = BIO_new_mem_buf(cert_cdt_in_bio_mem_buf, cert_cdt_in_bio_mem_buf_len);
	_ATSC3_SMIME_UTILS_DEBUG("atsc3_smime_validate_from_context: BIO_new_mem_buf: cacert_cdt_payload_in in: %p, cert_cdt_in_bio_mem_buf_len: %d, cert_cdt_in_bio_mem_buf:\n%s",
							cacert_cdt_payload_in, cert_cdt_in_bio_mem_buf_len, cert_cdt_in_bio_mem_buf);

	if (!cacert_cdt_payload_in) {
	   goto err;
	}

	cacert_cdt = PEM_read_bio_X509(cacert_cdt_payload_in, NULL, 0, NULL);
	
	if (!cacert_cdt) {
		_ATSC3_SMIME_UTILS_WARN("atsc3_smime_validate_from_context: PEM_read_bio_X509 failed, cacert_cdt_payload_in: %p, PEM_read_X509 returned: %p",
								cacert_cdt_payload_in, cacert_cdt);
	   goto err;
	}
	sk_X509_push(pcerts, cacert_cdt);

//	if (!X509_STORE_add_cert(st, cacert)) {
//	   goto err;
//	}
	

	/* convert our block_t raw smime payload to BIO mem buff for input */
	block_Rewind(atsc3_smime_validation_context->atsc3_smime_entity->raw_smime_payload);
	uint8_t* signed_payload_in_bio_mem_buf = block_Get(atsc3_smime_validation_context->atsc3_smime_entity->raw_smime_payload);
	uint32_t signed_payload_in_bio_mem_buf_len = block_Remaining_size(atsc3_smime_validation_context->atsc3_smime_entity->raw_smime_payload);

	signed_payload_in = BIO_new_mem_buf(signed_payload_in_bio_mem_buf, signed_payload_in_bio_mem_buf_len);
	_ATSC3_SMIME_UTILS_DEBUG("atsc3_smime_validate_from_context: BIO_new_mem_buf: signed_payload_in in: %p, signed_payload_in_bio_mem_buf_len: %d, signed_payload_in_bio_mem_buf:\n%s",
							signed_payload_in, signed_payload_in_bio_mem_buf_len, signed_payload_in_bio_mem_buf);

	if (!signed_payload_in) {
	   goto err;
	}
	
	/* parse SMIME message */
	
	cms = SMIME_read_CMS(signed_payload_in, &cont);

	if (!cms) {
		_ATSC3_SMIME_UTILS_WARN("atsc3_smime_validate_from_context:SMIME_read_CMS: failed!");
	   goto err;
	}
	
	extracted_payload_out = BIO_new(BIO_s_mem());

	if (!extracted_payload_out)
	   goto err;

	//https://www.openssl.org/docs/man1.1.0/man3/CMS_verify.html
		
	if(atsc3_smime_validation_context->cms_noverify) {
		cms_verify_flags = CMS_BINARY | CMS_NOVERIFY | CMS_NO_SIGNER_CERT_VERIFY | CMS_NOCRL | CMS_NO_ATTR_VERIFY;
	} else {
		cms_verify_flags = CMS_BINARY;
	}
	
	if (!CMS_verify(cms, pcerts, st_root, cont, extracted_payload_out, cms_verify_flags)) {
	   _ATSC3_SMIME_UTILS_WARN("atsc3_smime_validate_from_context:CMS_verify: verification failure");
	   goto err;
	}

	_ATSC3_SMIME_UTILS_DEBUG("atsc3_smime_validate_from_context: verification successful");

	atsc3_smime_validation_context->cms_signature_valid = true;
	
	//copy this to our context->smime_entity->cms_verified_extracted_mime_entity
	char *extracted_payload_out_char_p = NULL;
	extracted_payload_out_len = BIO_get_mem_data(extracted_payload_out, &extracted_payload_out_char_p);

	atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity = block_Alloc(extracted_payload_out_len);
	block_Write(atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity, (uint8_t*) extracted_payload_out_char_p, extracted_payload_out_len);
	block_Rewind(atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity);
	
	_ATSC3_SMIME_UTILS_DEBUG("atsc3_smime_validate_from_context: BIO_get_mem_data: extracted_payload_out_len: %d, extracted_payload_out_char_p:\n%s",
							 atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_size,
							 atsc3_smime_validation_context->atsc3_smime_entity->cms_verified_extracted_mime_entity->p_buffer);

	

err:

	if (!atsc3_smime_validation_context->cms_signature_valid) {
		_ATSC3_SMIME_UTILS_WARN("atsc3_smime_validate_from_context: error verifying data, errors:");
		ERR_print_errors_fp(stderr);
		atsc3_smime_validation_context_return = NULL;
	}

	CMS_ContentInfo_free(cms);
	
	X509_free(cacert_cdt);
	X509_free(cacert_root);
	
	BIO_free(signed_payload_in);
	BIO_free(extracted_payload_out);
	BIO_free(cacert_cdt_payload_in);
	
	if(cont) {
		BIO_free(cont);
	}
	
	return atsc3_smime_validation_context_return;
}

