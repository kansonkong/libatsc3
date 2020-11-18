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

atsc3_smime_entity_t* atsc3_smime_entity_new_parse_from_file(const char* filename) {
	atsc3_smime_entity_t* atsc3_smime_entity = atsc3_smime_entity_new();
	atsc3_smime_entity->raw_smime_payload_filename = strlcopy(filename);
	atsc3_smime_entity->raw_smime_payload = block_Read_from_filename(filename);
	
	if(!atsc3_smime_entity->raw_smime_payload) {
		_ATSC3_SMIME_UTILS_ERROR("atsc3_smime_entity_new_parse_from_file: unable to read raw smime payload from: %s", filename);
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
			
			if(atsc3_smime_entity->extracted_mime_entity) {
				block_Destroy(&atsc3_smime_entity->extracted_mime_entity);
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
			
			free(atsc3_smime_validation_context);
			atsc3_smime_validation_context = NULL;
		}
		
		*atsc3_smime_validation_context_p = NULL;
	}
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
	
	BIO *in = NULL, *out = NULL, *tbio = NULL, *cont = NULL;
	X509_STORE *st = NULL;
	X509 *cacert = NULL;
	
	STACK_OF(X509) *pcerts = NULL;
	CMS_ContentInfo *cms = NULL;

	int ret = 1;

	OpenSSL_add_all_algorithms();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_digests();
	ERR_load_crypto_strings();

	/* Set up trusted CA certificate store */

	st = X509_STORE_new();
	pcerts = sk_X509_new_null();
	
	//X509_STORE_set_trust(st, X509_V_FLAG_IGNORE_CRITICAL);
	/* Read in CA certificate */
	//cert4.crt
	//signal-signing-enensys-SMT.crt

	tbio = BIO_new_file("signal-signing-enensys-SMT.crt", "r");
	///Users/jjustman/Desktop/2020-11-17-signalling/sls2/signal-signing-enensys-SMT.crt
	tbio = BIO_new_file("/Users/jjustman/Desktop/2020-11-17-signalling/sls2/signal-signing-enensys-SMT.crt", "r");

	if (!tbio) {
	   goto err;
	}

	//cacert = PEM_read_bio_X509(tbio, NULL, 0, NULL);

	cacert = PEM_read_bio_X509(tbio, NULL, 0, NULL);
	/*
	 ok = X509_add_cert(*pcerts,
						OSSL_STORE_INFO_get1_CERT(info),
						X509_ADD_FLAG_DEFAULT);
	 */
	
	if (!cacert) {
	   goto err;
	}
//
	if (!X509_STORE_add_cert(st, cacert)) {
	   goto err;
	}
	sk_X509_push(pcerts, cacert);


	/* Open message being verified */
	uint8_t* bio_mem_buf_in = block_Get(atsc3_smime_validation_context->atsc3_smime_entity->raw_smime_payload);
	uint32_t bio_mem_buf_in_len = atsc3_smime_validation_context->atsc3_smime_entity->raw_smime_payload->p_size;

	in = BIO_new_mem_buf(bio_mem_buf_in, bio_mem_buf_in_len);
	//_ATSC3_SMIME_UTILS_INFO("atsc3_smime_validate_from_context: BIO_new_mem_buf: in: %p, bio_mem_buf_in_len: %d, bio_mem_buf_in:\n%s",
	//						in, bio_mem_buf_in_len, bio_mem_buf_in);

	//in = BIO_new_file("/Users/jjustman/Desktop/libatsc3/src/test/testdata/2020-11-17-signed-sls/239.1.120.120.49152.0-458826", "r");

	if (!in)
	   goto err;

	/* parse message */
	//cms = SMIME_read_CMS(in, &cont);
	//cont
	cms = SMIME_read_CMS(in, &cont);

	if (!cms) {
		_ATSC3_SMIME_UTILS_WARN("atsc3_smime_validate_from_context:SMIME_read_CMS: failed!");
	   goto err;
	}
	
//    const CMS_CTX *ctx = cms_get0_cmsctx(cms);
//	_ATSC3_SMIME_UTILS_INFO("atsc3_smime_validate_from_context: cms_get0_cmsctx, cms: %p, ctx: %p", cms, ctx);
	
//
//	const ASN1_OBJECT* ct = CMS_get0_type(cms);
//	const char* ct_long = OBJ_nid2ln(ct);
//
//	ASN1_OCTET_STRING** pconf = CMS_get0_content(cms);
//
//	_ATSC3_SMIME_UTILS_INFO("atsc3_smime_validate_from_context: cms: %p, ct: %s, pconf: %s", cms, ct_long, *pconf);
	/* File to output verified content to */
	out = BIO_new_file("smver.txt", "w");
	if (!out)
	   goto err;

	//https://www.openssl.org/docs/man1.1.0/man3/CMS_verify.html
	//	if (!CMS_verify(cms, NULL, st, cont, out, 0)) {

	//flags:
	//CMS_NO_SIGNER_CERT_VERIFY
	//| CMS_NO_ATTR_VERIFY | CMS_ASCIICRLF)
	//CMS_BINARY | CMS_NOVERIFY | CMS_NO_SIGNER_CERT_VERIFY | CMS_NOCRL | CMS_NO_ATTR_VERIFY)
	//| CMS_NOVERIFY | CMS_NO_SIGNER_CERT_VERIFY | CMS_NOCRL
	//cms_resolve_libctx(cms);
//CMS_BINARY |
	if (!CMS_verify(cms, pcerts, st, cont, out,  CMS_BINARY | CMS_NOVERIFY | CMS_NO_SIGNER_CERT_VERIFY | CMS_NOCRL | CMS_NO_ATTR_VERIFY )) {
	   _ATSC3_SMIME_UTILS_WARN("atsc3_smime_validate_from_context:CMS_verify: verification failure");
	   goto err;
	}

	_ATSC3_SMIME_UTILS_INFO("atsc3_smime_validate_from_context: verification successful");

	atsc3_smime_validation_context->cms_signature_valid = true;

err:

	if (!atsc3_smime_validation_context->cms_signature_valid) {
		_ATSC3_SMIME_UTILS_WARN("atsc3_smime_validate_from_context: error verifying data, errors:");
		ERR_print_errors_fp(stderr);
	}

	CMS_ContentInfo_free(cms);
	X509_free(cacert);
	BIO_free(in);
	BIO_free(out);
	BIO_free(tbio);
	
	return atsc3_smime_validation_context;
}

