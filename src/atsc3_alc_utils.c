/*
 * atsc3_alc_utils.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 *
 *	< https://tools.ietf.org/html/rfc5775 >
 *      4.4.  Receiver Operation

   The receiver operation, when using ALC, includes all the points made
   about the receiver operation when using the LCT building block
   [RFC5651], the FEC building block [RFC5052], and the multiple rate
   congestion control building block.

   To be able to participate in a session, a receiver needs to obtain
   the required Session Description as listed in Section 2.4.  How
   receivers obtain a Session Description is outside the scope of this
   document.

   As described in Section 2.3, a receiver needs to obtain the required
   FEC Object Transmission Information for each object for which the
   receiver receives and processes packets.




Luby, et al.                 Standards Track                   [Page 15]

RFC 5775               ALC Protocol Instantiation             April 2010


   Upon receipt of each packet, the receiver proceeds with the following
   steps in the order listed.

   1.  The receiver MUST parse the packet header and verify that it is a
       valid header.  If it is not valid, then the packet MUST be
       discarded without further processing.

   2.  The receiver MUST verify that the sender IP address together with
       the TSI carried in the header matches one of the (sender IP
       address, TSI) pairs that was received in a Session Description
       and to which the receiver is currently joined.  If there is not a
       match, then the packet MUST be silently discarded without further
       processing.  The remaining steps are performed within the scope
       of the (sender IP address, TSI) session of the received packet.

   3.  The receiver MUST process and act on the CCI field in accordance
       with the multiple rate congestion control building block.

   4.  If more than one object is carried in the session, the receiver
       MUST verify that the TOI carried in the LCT header is valid.  If
       the TOI is not valid, the packet MUST be discarded without
       further processing.

   5.  The receiver SHOULD process the remainder of the packet,
       including interpreting the other header fields appropriately, and
       using the FEC Payload ID and the encoding symbol(s) in the
       payload to reconstruct the corresponding object.

   It is RECOMMENDED that packet authentication be used.  If packet
   authentication is used, then it is RECOMMENDED that the receiver
   immediately check the authenticity of a packet before proceeding with
   step (3) above.  If immediate checking is possible and if the packet
   fails the check, then the receiver MUST silently discard the packet.
 */
#include "atsc3_alc_utils.h"

int _ALC_PACKET_DUMP_TO_OBJECT_ENABLED = 0;

int _ALC_UTILS_DEBUG_ENABLED=0;
int _ALC_UTILS_TRACE_ENABLED=0;


bool __ALC_RECON_FILE_PTR_HAS_WRITTEN_INIT_BOX = false;

pipe_ffplay_buffer_t* __ALC_RECON_FILE_BUFFER_STRUCT = NULL;
uint32_t* __ALC_RECON_FILE_PTR_TSI = NULL;
uint32_t* __ALC_RECON_FILE_PTR_TOI_INIT = NULL;

FILE* __ALC_RECON_FILE_PTR = NULL; //deprecated
lls_sls_alc_monitor_t* __ALC_RECON_MONITOR;


char* alc_packet_dump_to_object_get_filename(alc_packet_t* alc_packet) {
	char *file_name = calloc(255, sizeof(char));

	if(alc_packet->def_lct_hdr) {
		snprintf(file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
	}

	return file_name;
}

int alc_packet_dump_to_object(alc_packet_t** alc_packet_ptr) {

	alc_packet_t* alc_packet = *alc_packet_ptr;
	int bytesWritten = 0;
    char* file_name = alc_packet_dump_to_object_get_filename(alc_packet);

	if(!_ALC_PACKET_DUMP_TO_OBJECT_ENABLED) {
			goto cleanup;
	}
    mkdir("route", 0777);

	int filename_pos = 0;
	__ALC_UTILS_TRACE("have tsi: %u, toi: %u, sbn: %x, esi: %x len: %d",
			alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi,
			alc_packet->esi, alc_packet->sbn, alc_packet->alc_len);

	FILE *f = NULL;

	//if no TSI, this is metadata and create a new object for each payload
	//The SLS fragments shall be delivered on a dedicated LCT transport channel with TSI = 0.


	if(!alc_packet->def_lct_hdr->tsi) {
		f = fopen(file_name, "w");
	} else {
		//esi will not be 0 if this is FEC protected, use sbn (Source block number instead).
		if(alc_packet->sbn>0) {
			__ALC_UTILS_TRACE("dumping to file in append mode: %s, esi: %d", file_name, alc_packet->esi);
			f = fopen(file_name, "a");
		} else {
			__ALC_UTILS_TRACE("dumping to file in write mode: %s, esi: %d", file_name, alc_packet->esi);
			//open as write
			f = fopen(file_name, "w");
		}
	}

	if(!f) {
		__ALC_UTILS_WARN("UNABLE TO OPEN FILE %s", file_name);
		goto cleanup;
	}

	__ALC_UTILS_TRACE("dumping alc packet size: %d to %p", alc_packet->alc_len, f);
	int blocks_written = fwrite(alc_packet->alc_payload, alc_packet->alc_len, 1, f);
	if(blocks_written != 1) {
		__ALC_UTILS_WARN("short packet write!");
	}

	fclose(f);

	//also investigate alc_packet->transfer_len if we dont get a close object tag
	if(alc_packet->close_object_flag) {
		__ALC_UTILS_TRACE("dumping to file done: %s, is complete: %d", file_name, alc_packet->close_object_flag);
	} else {
		__ALC_UTILS_TRACE("dumping to file step: %s, is complete: %d", file_name, alc_packet->close_object_flag);
	}
#if defined(__TESTING_PREPEND_TSI__) && defined(__TESTING_PREPEND_TOI_INIT__)
	char* tsi_prepend_init = __TESTING_PREPEND_TSI__;
	char* toi_prepend_init = __TESTING_PREPEND_TOI_INIT__;
	__ALC_UTILS_DEBUG("checking %s, %s,  %d", alc_packet->tsi_c, alc_packet->toi_c, alc_packet->close_object_flag);

	if(strncmp(alc_packet->tsi_c, tsi_prepend_init, strlen(alc_packet->tsi_c)) == 0 &&
				strncmp(alc_packet->toi_c, toi_prepend_init, strlen(alc_packet->toi_c)) &&
				alc_packet->close_object_flag) {
		__alc_prepend_fragment_with_init_box(file_name, alc_packet);


	}
#endif


#if defined(__TESTING_RECON_FILE_POINTER__) && defined(__TESTING_RECONSTITUTED_TSI__) && defined(__TESTING_RECONSTITUTED_TOI_INIT__) && defined(__TESTING_RECONSITIUTED_FILE_NAME__)
	char* tsi_recon_init = __TESTING_RECONSTITUTED_TSI__;
	char* toi_recon_init = __TESTING_RECONSTITUTED_TOI_INIT__;
	__ALC_UTILS_DEBUG("checking %s, %s,  %d", alc_packet->tsi_c, alc_packet->toi_c, alc_packet->close_object_flag);

	if(strncmp(alc_packet->tsi_c, tsi_recon_init, strlen(alc_packet->tsi_c)) == 0 &&
				strncmp(alc_packet->toi_c, toi_recon_init, strlen(alc_packet->toi_c)) &&
				alc_packet->close_object_flag) {
		__alc_recon_fragment_with_init_box(file_name, alc_packet);
	}
#endif

#ifdef __TESTING_RECON_WITH_POPEN_FILE_POINTER__
	if(__ALC_RECON_FILE_BUFFER_STRUCT && __ALC_RECON_FILE_PTR_TSI && __ALC_RECON_FILE_PTR_TOI_INIT) {
			__ALC_UTILS_DEBUG("checking %s, %s,  %d", alc_packet->tsi_c, alc_packet->toi_c, alc_packet->close_object_flag);

			if(strncmp(alc_packet->tsi_c, __ALC_RECON_FILE_PTR_TSI, strlen(alc_packet->tsi_c)) == 0 &&
						strncmp(alc_packet->toi_c, __ALC_RECON_FILE_PTR_TOI_INIT, strlen(alc_packet->toi_c)) &&
						alc_packet->close_object_flag) {
				alc_recon_file_ptr_fragment_with_init_box(__ALC_RECON_FILE_PTR, alc_packet);
			}
		}
#endif

#ifdef __OLD_TOI_MAPPING
	//push our fragments EXCEPT for the mpu fragment box, we will pull that at the start of a
	if(__ALC_RECON_FILE_BUFFER_STRUCT && __ALC_RECON_FILE_PTR_TSI && __ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_TRACE("checking tsi: %u, toi: %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

		if(alc_packet->def_lct_hdr->tsi == *__ALC_RECON_FILE_PTR_TSI && alc_packet->def_lct_hdr->toi != *__ALC_RECON_FILE_PTR_TOI_INIT && alc_packet->close_object_flag) {

			alc_recon_file_buffer_struct_fragment_with_init_box(__ALC_RECON_FILE_BUFFER_STRUCT, alc_packet);
		}
	}
#endif
	//__ALC_RECON_MONITOR
	//push our fragments EXCEPT for the mpu fragment box, we will pull that at the start of a
		if(__ALC_RECON_FILE_BUFFER_STRUCT && __ALC_RECON_MONITOR) {
			__ALC_UTILS_TRACE("checking tsi: %u, toi: %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

			if((alc_packet->def_lct_hdr->tsi == __ALC_RECON_MONITOR->video_tsi && alc_packet->def_lct_hdr->toi != __ALC_RECON_MONITOR->video_toi_init) ||
					(alc_packet->def_lct_hdr->tsi == __ALC_RECON_MONITOR->audio_tsi && alc_packet->def_lct_hdr->toi != __ALC_RECON_MONITOR->audio_toi_init)) {

					alc_recon_file_buffer_struct_monitor_fragment_with_init_box(__ALC_RECON_FILE_BUFFER_STRUCT, __ALC_RECON_MONITOR, alc_packet);
			}
		}


cleanup:
	if(file_name) {
		free(file_name);
	}
	if(alc_packet) {
		alc_packet_free(alc_packet_ptr);
	}

	return bytesWritten;
}


void __alc_prepend_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet) {

#if defined(__TESTING_PREPEND_TSI__) && defined(__TESTING_PREPEND_TOI_INIT__)

	char* tsi_init = __TESTING_PREPEND_TSI__;
	char* toi_init = __TESTING_PREPEND_TOI_INIT__;

	char* init_file_name = calloc(255, sizeof(char));
	char* fm4v_file_name = calloc(255, sizeof(char)); //.m4v == 4

	__ALC_UTILS_DEBUG(" - concat %s, %s,  %d", alc_packet->tsi_c, alc_packet->toi_c, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%s-%s", __ALC_DUMP_OUTPUT_PATH__, tsi_init, toi_init);
	snprintf(fm4v_file_name, 255, "%s%s-%s.m4v", __ALC_DUMP_OUTPUT_PATH__, alc_packet->tsi_c, alc_packet->toi_c);

	if( access( init_file_name, F_OK ) == -1 ) {
		__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
		goto cleanup;
	}
	struct stat st;
	stat(init_file_name, &st);

	uint8_t* init_payload = calloc(st.st_size, sizeof(uint8_t));
	FILE* init_file = fopen(init_file_name, "r");
	if(!init_file) {
		__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
		goto cleanup;
	}

	fread(init_payload, st.st_size, 1, init_file);
	fclose(init_file);

	FILE* fm4v_output_file = fopen(fm4v_file_name, "w");
	if(!fm4v_output_file) {
		__ALC_UTILS_ERROR("unable to open fm4v output file: %s", fm4v_file_name);
		goto cleanup;
	}

	fwrite(init_payload, st.st_size, 1, fm4v_output_file);
	uint64_t block_size = 8192;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		goto cleanup;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count=0;
	bool has_eof = false;
	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		__ALC_UTILS_TRACE("read bytes: %llu", read_bytes);

		int write_size = fwrite(m4v_payload, read_bytes, 1, fm4v_output_file);
		if(has_eof) {
			__ALC_UTILS_TRACE("write bytes: %u", write_size);

			fclose(m4v_fragment_input_file);
			fclose(fm4v_output_file);
			break;
		}
		write_count++;
	}
cleanup:
	return;
#endif

}

bool __ALC_RECON_HAS_WRITTEN_INIT_BOX = false;

void __alc_recon_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet) {

	uint32_t tsi_init = __TESTING_RECONSTITUTED_TSI__;
	uint32_t toi_init = __TESTING_RECONSTITUTED_TOI_INIT__;

	char* init_file_name = calloc(255, sizeof(char));
	char* recon_file_name = calloc(255, sizeof(char)); //.m4v == 4
	FILE* recon_output_file = NULL;

	__ALC_UTILS_DEBUG(" alc_recon_fragment_with_init_box: %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, tsi_init, toi_init);
	snprintf(recon_file_name, 255, "%s%s", __ALC_DUMP_OUTPUT_PATH__, __TESTING_RECONSITIUTED_FILE_NAME__);

	if(!__ALC_RECON_HAS_WRITTEN_INIT_BOX) {


		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			goto cleanup;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			goto cleanup;
		}

		fread(init_payload, st.st_size, 1, init_file);
		fclose(init_file);
		recon_output_file = fopen(recon_file_name, "w");
		if(!recon_output_file) {
			__ALC_UTILS_ERROR("unable to open recon_output_file for writing: %s", recon_file_name);
			goto cleanup;
		}
		fwrite(init_payload, st.st_size, 1, recon_output_file);
		__ALC_RECON_HAS_WRITTEN_INIT_BOX = true;

	} else {
		recon_output_file = fopen(recon_file_name, "a");
		if(!recon_output_file) {
		__ALC_UTILS_ERROR("unable to open recon_output_file for append: %s", recon_file_name);
			goto cleanup;
		}
	}

	uint64_t block_size = 8192;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		goto cleanup;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count=0;
	bool has_eof = false;
	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		__ALC_UTILS_TRACE("read bytes: %llu", read_bytes);

		int write_size = fwrite(m4v_payload, read_bytes, 1, recon_output_file);
		if(has_eof) {
			__ALC_UTILS_TRACE("write bytes: %u", write_size);

			fclose(m4v_fragment_input_file);
			fclose(recon_output_file);
			break;
		}
		write_count++;
	}
cleanup:
	return;
}

//watch out for leaks...
void alc_recon_file_ptr_set_tsi_toi(FILE* file_ptr, uint32_t tsi, uint32_t toi_init) {
	__ALC_RECON_FILE_PTR = file_ptr;
	if(!__ALC_RECON_FILE_PTR_TSI) {
		__ALC_RECON_FILE_PTR_TSI = calloc(1, sizeof(uint32_t));
	}
	*__ALC_RECON_FILE_PTR_TSI = tsi;


	if(!__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_RECON_FILE_PTR_TOI_INIT = calloc(1, sizeof(uint32_t));
		}
	*__ALC_RECON_FILE_PTR_TOI_INIT = toi_init;
}

void alc_recon_file_ptr_fragment_with_init_box(FILE* output_file_ptr, alc_packet_t* alc_packet) {
	int flush_ret = 0;
	if(!__ALC_RECON_FILE_PTR_TSI || !__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_WARN("alc_recon_file_ptr_fragment_with_init_box - NULL: tsi: %p, toi: %p", __ALC_RECON_FILE_PTR_TSI, __ALC_RECON_FILE_PTR_TOI_INIT);
		return;
	}

	char* file_name = alc_packet_dump_to_object_get_filename(alc_packet);
	uint32_t toi_init = __TESTING_RECONSTITUTED_TOI_INIT__;

	char* init_file_name = calloc(255, sizeof(char));

	__ALC_UTILS_DEBUG("recon %u, %u, %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, *__ALC_RECON_FILE_PTR_TSI, toi_init);

	if(!__ALC_RECON_FILE_PTR_HAS_WRITTEN_INIT_BOX) {
		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			goto cleanup;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			goto cleanup;
		}

		fread(init_payload, st.st_size, 1, init_file);
		fclose(init_file);

		fwrite(init_payload, st.st_size, 1, output_file_ptr);
		__ALC_RECON_HAS_WRITTEN_INIT_BOX = true;

	} else {
		//noop here
	}

	uint64_t block_size = 8192;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		goto cleanup;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count=0;
	bool has_eof = false;
	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		__ALC_UTILS_TRACE("read bytes: %llu", read_bytes);

		if(feof(output_file_ptr)) {
			goto broken_pipe;
		}

		int write_size = fwrite(m4v_payload, read_bytes, 1, output_file_ptr);
		if(has_eof) {
			__ALC_UTILS_TRACE("write bytes: %u", write_size);

			fclose(m4v_fragment_input_file);
			flush_ret = fflush(output_file_ptr);
			if(flush_ret || feof(output_file_ptr)) {
				goto broken_pipe;
			}
			break;
		}
		write_count++;
	}
	goto cleanup;

broken_pipe:
	__ALC_UTILS_ERROR("flush returned: %d, closing pipe", flush_ret);
	fclose(__ALC_RECON_FILE_PTR);
	__ALC_RECON_FILE_PTR = NULL;

cleanup:
	if(m4v_payload) {
		free(m4v_payload);
		m4v_payload = NULL;
	}
	if(file_name) {
		free(file_name);
		file_name = NULL;
	}


	return;
}

/*
 * mutex buffer writer
 */

void alc_recon_file_buffer_struct_set_tsi_toi(pipe_ffplay_buffer_t* pipe_ffplay_buffer, uint32_t tsi, uint32_t toi_init) {
	__ALC_RECON_FILE_BUFFER_STRUCT = pipe_ffplay_buffer;

	if(!__ALC_RECON_FILE_PTR_TSI) {
		__ALC_RECON_FILE_PTR_TSI = calloc(1, sizeof(uint32_t));
	}

	if(!__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_RECON_FILE_PTR_TOI_INIT = calloc(1, sizeof(uint32_t));
	}

	*__ALC_RECON_FILE_PTR_TSI = tsi;
	*__ALC_RECON_FILE_PTR_TOI_INIT = toi_init;
}



void alc_recon_file_buffer_struct_set_monitor(pipe_ffplay_buffer_t* pipe_ffplay_buffer, lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
	__ALC_RECON_FILE_BUFFER_STRUCT = pipe_ffplay_buffer;
	__ALC_RECON_MONITOR = lls_sls_alc_monitor;

}


/*** we take this off of disk for the reassembeled fragment metadta and mpu
 *
 *
 */
void alc_recon_file_buffer_struct_fragment_with_init_box(pipe_ffplay_buffer_t* pipe_ffplay_buffer, alc_packet_t* alc_packet) {
	int flush_ret = 0;
	if(!__ALC_RECON_FILE_PTR_TSI || !__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_WARN("alc_recon_file_ptr_fragment_with_init_box - NULL: tsi: %p, toi: %p", __ALC_RECON_FILE_PTR_TSI, __ALC_RECON_FILE_PTR_TOI_INIT);
		return;
	}

	char* file_name = alc_packet_dump_to_object_get_filename(alc_packet);
	uint32_t toi_init = *__ALC_RECON_FILE_PTR_TOI_INIT;
	char* init_file_name = calloc(255, sizeof(char));

	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - ENTER - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, *__ALC_RECON_FILE_PTR_TSI, toi_init);

	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);

	if(!pipe_ffplay_buffer->has_written_init_box) {
		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			goto cleanup;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			goto cleanup;
		}

		fread(init_payload, st.st_size, 1, init_file);
		fclose(init_file);

		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, init_payload, st.st_size);
		pipe_ffplay_buffer->has_written_init_box = true;

	} else {
		//noop here
	}

	uint64_t block_size = __PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		goto cleanup;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count = 0;
	uint64_t total_bytes_written = 0;
	bool has_eof = false;

	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		total_bytes_written += read_bytes;
		__ALC_UTILS_TRACE("read bytes: %llu, bytes written: %llu, total filesize: %llu, has eof input: %d", read_bytes, total_bytes_written, fragment_input_stat.st_size, has_eof);

		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, m4v_payload, read_bytes);

		if(has_eof) {
			fclose(m4v_fragment_input_file);
			break;
		}
		write_count++;

	}

	//signal and then unlock, docs indicate the only way to ensure a signal is not lost is to send it while holding the lock
	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - SIGNALING - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer);

	//check to see if we have shutdown
	pipe_buffer_reader_check_if_shutdown(&pipe_ffplay_buffer);

	pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - RETURN - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	goto cleanup;

broken_pipe:
	__ALC_UTILS_ERROR("flush returned: %d, closing pipe", flush_ret);
	fclose(__ALC_RECON_FILE_PTR);
	__ALC_RECON_FILE_PTR = NULL;

cleanup:
	if(m4v_payload) {
		free(m4v_payload);
		m4v_payload = NULL;
	}
	if(file_name) {
		free(file_name);
		file_name = NULL;
	}


	return;
}


void alc_recon_file_buffer_struct_monitor_fragment_with_init_box(pipe_ffplay_buffer_t* pipe_ffplay_buffer, lls_sls_alc_monitor_t* lls_slt_monitor, alc_packet_t* alc_packet) {
	int flush_ret = 0;
	if(!__ALC_RECON_FILE_PTR_TSI || !__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_WARN("alc_recon_file_ptr_fragment_with_init_box - NULL: tsi: %p, toi: %p", __ALC_RECON_FILE_PTR_TSI, __ALC_RECON_FILE_PTR_TOI_INIT);
		return;
	}
	//do this for both video_tsi and audio tsi based upon the current toi...?
	//todo fix me for different toi numbers

	char* file_name = alc_packet_dump_to_object_get_filename(alc_packet);
	uint32_t toi_init = *__ALC_RECON_FILE_PTR_TOI_INIT;
	char* init_file_name = calloc(255, sizeof(char));

	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - ENTER - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, *__ALC_RECON_FILE_PTR_TSI, toi_init);

	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);

	if(!pipe_ffplay_buffer->has_written_init_box) {
		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			goto cleanup;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			goto cleanup;
		}

		fread(init_payload, st.st_size, 1, init_file);
		fclose(init_file);

		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, init_payload, st.st_size);
		pipe_ffplay_buffer->has_written_init_box = true;

	} else {
		//noop here
	}

	uint64_t block_size = __PLAYER_FFPLAY_PIPE_WRITER_BLOCKSIZE;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		goto cleanup;
	}
	struct stat fragment_input_stat;
	stat(file_name, &fragment_input_stat);
	uint64_t write_count = 0;
	uint64_t total_bytes_written = 0;
	bool has_eof = false;

	while(!has_eof) {
		int read_size = fread(m4v_payload, block_size, 1, m4v_fragment_input_file);
		uint64_t read_bytes = read_size * block_size;
		if(!read_bytes && feof(m4v_fragment_input_file)) {
			read_bytes = fragment_input_stat.st_size - (block_size * write_count);
			has_eof = true;
		}
		total_bytes_written += read_bytes;
		__ALC_UTILS_TRACE("read bytes: %llu, bytes written: %llu, total filesize: %llu, has eof input: %d", read_bytes, total_bytes_written, fragment_input_stat.st_size, has_eof);

		pipe_buffer_unsafe_push_block(pipe_ffplay_buffer, m4v_payload, read_bytes);

		if(has_eof) {
			fclose(m4v_fragment_input_file);
			break;
		}
		write_count++;

	}

	//signal and then unlock, docs indicate the only way to ensure a signal is not lost is to send it while holding the lock
	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - SIGNALING - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	pipe_buffer_notify_semaphore_post(pipe_ffplay_buffer);

	//check to see if we have shutdown
	pipe_buffer_reader_check_if_shutdown(&pipe_ffplay_buffer);

	pipe_buffer_reader_mutex_unlock(pipe_ffplay_buffer);
	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - RETURN - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	goto cleanup;

broken_pipe:
	__ALC_UTILS_ERROR("flush returned: %d, closing pipe", flush_ret);
	fclose(__ALC_RECON_FILE_PTR);
	__ALC_RECON_FILE_PTR = NULL;

cleanup:
	if(m4v_payload) {
		free(m4v_payload);
		m4v_payload = NULL;
	}
	if(file_name) {
		free(file_name);
		file_name = NULL;
	}


	return;
}

