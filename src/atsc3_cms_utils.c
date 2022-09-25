//
//  atsc3_cms_utils.c
//  libatsc3
//
//  Created by Jason Justman on 11/17/20.
//  Copyright © 2020 Jason Justman. All rights reserved.
//

#include "atsc3_cms_utils.h"

int _ATSC3_CMS_UTILS_INFO_ENABLED  = 1;
int _ATSC3_CMS_UTILS_DEBUG_ENABLED = 1;
int _ATSC3_CMS_UTILS_TRACE_ENABLED = 0;

atsc3_cms_entity_t* atsc3_cms_entity_new() {
	atsc3_cms_entity_t* atsc3_cms_entity = calloc(1, sizeof(atsc3_cms_entity_t));
	
	return atsc3_cms_entity;
}

void atsc3_cms_entity_free(atsc3_cms_entity_t** atsc3_cms_entity_p) {
	if(atsc3_cms_entity_p) {
		atsc3_cms_entity_t* atsc3_cms_entity = *atsc3_cms_entity_p;
		if(atsc3_cms_entity) {

			if(atsc3_cms_entity->raw_binary_payload) {
				block_Destroy(&atsc3_cms_entity->raw_binary_payload);
			}

			if(atsc3_cms_entity->signature) {
				block_Destroy(&atsc3_cms_entity->signature);
			}

			if(atsc3_cms_entity->cms_verified_extracted_payload) {
				block_Destroy(&atsc3_cms_entity->cms_verified_extracted_payload);
			}

			if(atsc3_cms_entity->extracted_pkcs7_signature) {
				block_Destroy(&atsc3_cms_entity->extracted_pkcs7_signature);
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

			atsc3_cms_validation_context->transients.atsc3_certification_data = NULL;
			
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

atsc3_cms_validation_context_t* atsc3_cms_validate_from_context(atsc3_cms_validation_context_t* atsc3_cms_validation_context) {
	atsc3_cms_validation_context_t* atsc3_cms_validation_context_return = atsc3_cms_validation_context;

	if(!atsc3_cms_validation_context->transients.atsc3_certification_data) {
		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validation_context->transients.atsc3_certification_data is NULL, returning NULL!");
		return NULL;
	}

	if(!atsc3_cms_validation_context->transients.atsc3_certification_data->atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.count) {
		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validation_context->transients.atsc3_certification_data->atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.count is 0, may not resolve entity cert");
		return NULL;
	}

	BIO 	*signature_binary_der_in = NULL;
	BIO 	*payload_binary_in = NULL;

	//for smime validation
	BIO 	*signed_multipart_payload_in = NULL;
	BIO 	*extracted_smime_payload = NULL;
	bool 	use_extracted_smime_payload = false;

	BIO 	*extracted_payload_out = NULL;
	long 	extracted_payload_out_len = 0;

	X509_STORE 	*st_root = NULL;
	X509 		*cacert_root = NULL;
	X509 		*cacert_intermediate = NULL;
	
	STACK_OF(X509) *pcerts = NULL;
	CMS_ContentInfo *cms = NULL;
	
	unsigned int cms_verify_flags = 0;

	int ret = 1;

	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	ERR_load_crypto_strings();


	struct to_free_bio_and_x509_refs_s {
		block_t* fixup_cdt_table_block;
		BIO*	bio_ptr;
		X509*	x509_ptr;
	} to_free_bio_and_509_refs[atsc3_cms_validation_context->transients.atsc3_certification_data->atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.count] ;


	memset(&to_free_bio_and_509_refs, 0, atsc3_cms_validation_context->transients.atsc3_certification_data->atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.count * sizeof(struct to_free_bio_and_x509_refs_s));

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
        if(atsc3_certification_data_to_be_signed_data_certificates->base64_payload) {

            block_Rewind(atsc3_certification_data_to_be_signed_data_certificates->base64_payload);
            block_t* fixup_cdt_table_entry_blockt = block_Alloc(1024);

            block_Write(fixup_cdt_table_entry_blockt, (const uint8_t*) ATSC3_A360_CERTIFICATE_UTILS_BEGIN_CERTIFICATE, strlen(ATSC3_A360_CERTIFICATE_UTILS_BEGIN_CERTIFICATE));
            block_AppendFromSrciPosToSizeAndMoveIptrs(fixup_cdt_table_entry_blockt, atsc3_certification_data_to_be_signed_data_certificates->base64_payload, block_Remaining_size(atsc3_certification_data_to_be_signed_data_certificates->base64_payload));
            block_Write(fixup_cdt_table_entry_blockt, (const uint8_t*) ATSC3_A360_CERTIFICATE_UTILS_END_CERTIFICATE, strlen(ATSC3_A360_CERTIFICATE_UTILS_END_CERTIFICATE));
            block_Rewind(fixup_cdt_table_entry_blockt);

            uint8_t* cert_cdt_in_bio_mem_buf = block_Get(fixup_cdt_table_entry_blockt);
            uint32_t cert_cdt_in_bio_mem_buf_len = block_Remaining_size(fixup_cdt_table_entry_blockt);

            BIO * to_be_signed_payload = BIO_new_mem_buf(cert_cdt_in_bio_mem_buf, cert_cdt_in_bio_mem_buf_len);
            X509* to_be_signed_x509 = PEM_read_bio_X509(to_be_signed_payload, NULL, 0, NULL);

            //for memory cleanup...
            to_free_bio_and_509_refs[i].fixup_cdt_table_block = fixup_cdt_table_entry_blockt;
            to_free_bio_and_509_refs[i].bio_ptr = to_be_signed_payload;
            to_free_bio_and_509_refs[i].x509_ptr = to_be_signed_x509;

    #ifdef ATSC3_CMS_UTILS_DUMP_PAYLOADS_FOR_OPENSSL_DEBUGGING
            char cms_entity_to_sign_filename_string[32 + 1] = {0};

            snprintf((char*)&cms_entity_to_sign_filename_string, 32, "cert_to_sign.%d.pem", i);
            block_Write_to_filename(fixup_cdt_table_entry_blockt, cms_entity_to_sign_filename_string);
    #endif

            if (!to_be_signed_x509) {
                _ATSC3_CMS_UTILS_WARN("atsc3_cms_validate_from_context: index: %d failed to parse as PEM_read_bio_X509! to_be_signed_payload: %p",
                                      i, to_be_signed_payload);
                goto err;
            }

            sk_X509_push(pcerts, to_be_signed_x509);
        }
	}

	//if we have a detatched signature, process it separately as a DER (for LLS SignedMultiTable)
	if(atsc3_cms_validation_context->atsc3_cms_entity->signature) {
		/* convert our block_t raw cms payload to BIO mem buff for input */
		block_Rewind(atsc3_cms_validation_context->atsc3_cms_entity->signature);
		uint8_t *signature_binary_der_in_mem_buf = block_Get(atsc3_cms_validation_context->atsc3_cms_entity->signature);
		uint32_t signature_binary_der_in_mem_buf_len = block_Remaining_size(atsc3_cms_validation_context->atsc3_cms_entity->signature);

		signature_binary_der_in = BIO_new_mem_buf(signature_binary_der_in_mem_buf, signature_binary_der_in_mem_buf_len);
		_ATSC3_CMS_UTILS_DEBUG("atsc3_cms_validate_from_context: Signature DER and Payload: BIO_new_mem_buf: signed_payload_in in: %p, signed_payload_in_bio_mem_buf_len: %d, signed_payload_in_bio_mem_buf:\n%s",
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
	} else {
		//assume as CMS multipart message

		block_Rewind(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload);
		uint8_t* signed_payload_in_bio_mem_buf = block_Get(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload);
		uint32_t signed_payload_in_bio_mem_buf_len = block_Remaining_size(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload);

		signed_multipart_payload_in = BIO_new_mem_buf(signed_payload_in_bio_mem_buf, signed_payload_in_bio_mem_buf_len);
		_ATSC3_CMS_UTILS_DEBUG("atsc3_smime_validate_from_context: CMS Multipart Message: BIO_new_mem_buf: signed_payload_in in: %p, signed_payload_in_bio_mem_buf_len: %d, signed_payload_in_bio_mem_buf:\n%s",
								 signed_multipart_payload_in, signed_payload_in_bio_mem_buf_len, signed_payload_in_bio_mem_buf);

		if (!signed_multipart_payload_in) {
			goto err;
		}

		cms = SMIME_read_CMS(signed_multipart_payload_in, &extracted_smime_payload);
		use_extracted_smime_payload = true;
	}

	if (!cms) {
		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validate_from_context:CMS_read_CMS: failed!");
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
	
	if (!CMS_verify(cms, pcerts, st_root, (use_extracted_smime_payload ? extracted_smime_payload : payload_binary_in), extracted_payload_out, cms_verify_flags)) {
	   _ATSC3_CMS_UTILS_WARN("atsc3_cms_validate_from_context:CMS_verify: verification failure");
		atsc3_cms_validation_context->cms_signature_valid = false;
	} else {
		_ATSC3_CMS_UTILS_DEBUG("atsc3_cms_validate_from_context: verification successful");
		atsc3_cms_validation_context->cms_signature_valid = true;
		//copy this to our context->cms_entity->cms_verified_extracted_mime_entity
		char *extracted_payload_out_char_p = NULL;
		extracted_payload_out_len = BIO_get_mem_data(extracted_payload_out, &extracted_payload_out_char_p);

		atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_payload = block_Alloc(extracted_payload_out_len);
		block_Write(atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_payload, (uint8_t*) extracted_payload_out_char_p, extracted_payload_out_len);
		block_Rewind(atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_payload);

		_ATSC3_CMS_UTILS_DEBUG("atsc3_cms_validate_from_context: BIO_get_mem_data: extracted_payload_out_len: %d, extracted_payload_out_char_p:\n%s",
							   atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_payload->p_size,
							   atsc3_cms_validation_context->atsc3_cms_entity->cms_verified_extracted_payload->p_buffer);
	}


err:

	if (!atsc3_cms_validation_context->cms_signature_valid) {
		_ATSC3_CMS_UTILS_WARN("atsc3_cms_validate_from_context: error verifying data, errors:");

#ifdef ATSC3_CMS_UTILS_DUMP_PAYLOADS_FOR_OPENSSL_DEBUGGING
        if(atsc3_cms_validation_context->atsc3_cms_entity->signature) {
            block_Write_to_filename(atsc3_cms_validation_context->atsc3_cms_entity->signature, "raw_binary_payload_signature.der");
        }
        
        if(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload) {
            block_Write_to_filename(atsc3_cms_validation_context->atsc3_cms_entity->raw_binary_payload, "raw_binary_payload.data");
        }
#endif
		ERR_print_errors_fp(stdout);
		atsc3_cms_validation_context_return = NULL;
	}

	//cleanup in reverse order
	if(cms) {
		CMS_ContentInfo_free(cms);
	}

	if(extracted_payload_out) {
		BIO_free(extracted_payload_out);
	}

	if(extracted_smime_payload) {
		BIO_free(extracted_smime_payload);
	}

	if(signed_multipart_payload_in) {
		BIO_free(signed_multipart_payload_in);
	}

	if(payload_binary_in) {
		BIO_free(payload_binary_in);
	}

	if(signature_binary_der_in) {
		BIO_free(signature_binary_der_in);
	}

	//clear out our intermediates/entity certs


	for(int i= 0; i < atsc3_cms_validation_context->transients.atsc3_certification_data->atsc3_certification_data_to_be_signed_data.atsc3_certification_data_to_be_signed_data_certificates_v.count; i++) {
		if(to_free_bio_and_509_refs[i].fixup_cdt_table_block) {
			block_Destroy(&to_free_bio_and_509_refs[i].fixup_cdt_table_block);
		}
		if(to_free_bio_and_509_refs[i].bio_ptr) {
			BIO_free(to_free_bio_and_509_refs[i].bio_ptr);
			to_free_bio_and_509_refs[i].bio_ptr = NULL;
		}
		if(to_free_bio_and_509_refs[i].x509_ptr) {
			X509_free(to_free_bio_and_509_refs[i].x509_ptr);
			to_free_bio_and_509_refs[i].x509_ptr = NULL;
		}
	}

	if(cacert_ca_chain_signing_ca_2) {
		BIO_free(cacert_ca_chain_signing_ca_2);
	}

	if(cacert_intermediate) {
		X509_free(cacert_intermediate);
	}

	if(cacert_root_bio) {
		BIO_free(cacert_root_bio);
	}

	if(cacert_root) {
		X509_free(cacert_root);
	}


	if(st_root) {
		X509_STORE_free(st_root);
	}


	return atsc3_cms_validation_context_return;
}

