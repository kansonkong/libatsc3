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

#include "atsc3_lls_sls_monitor_output_buffer_utils.h"
//shortcut hack
#include "atsc3_isobmff_tools.h"


int _ALC_PACKET_DUMP_TO_OBJECT_ENABLED = 0;

int _ALC_UTILS_DEBUG_ENABLED=0;
int _ALC_UTILS_TRACE_ENABLED=0;
int _ALC_UTILS_IOTRACE_ENABLED=0;

bool __ALC_RECON_FILE_PTR_HAS_WRITTEN_INIT_BOX = false;

pipe_ffplay_buffer_t* __ALC_RECON_FILE_BUFFER_STRUCT = NULL;
uint32_t* __ALC_RECON_FILE_PTR_TSI = NULL;
uint32_t* __ALC_RECON_FILE_PTR_TOI_INIT = NULL;

FILE* __ALC_RECON_FILE_PTR = NULL; //deprecated
lls_sls_alc_monitor_t* __ALC_RECON_MONITOR;




block_t* alc_get_payload_from_filename(char* file_name) {
	if( access(file_name, F_OK ) == -1 ) {
		__ALC_UTILS_ERROR("alc_get_payload_from_filename: unable to open file: %s", file_name);
		return NULL;
	}

	struct stat st;
	stat(file_name, &st);

	//uint8_t* payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
	block_t* payload = block_Alloc(st.st_size);

	FILE* fp = fopen(file_name, "r");
	if(st.st_size == 0) {
		__ALC_UTILS_ERROR("alc_get_payload_from_filename: size: 0 file: %s", file_name);
		return NULL;
	}

	fread(payload->p_buffer, st.st_size, 1, fp);
    payload->i_pos = st.st_size;
	fclose(fp);

	return payload;

}

char* alc_packet_dump_to_object_get_filename(alc_packet_t* alc_packet) {
	char* file_name = (char *)calloc(255, sizeof(char));

	if(alc_packet->def_lct_hdr) {
		snprintf(file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi);
	}

	return file_name;
}


char* alc_packet_dump_to_object_get_filename_tsi_toi(uint32_t tsi, uint32_t toi) {
	char *file_name = (char *)calloc(255, sizeof(char));

	snprintf(file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, tsi, toi);

	return file_name;
}

//todo - build this in memory first...

FILE* alc_object_open_or_pre_allocate(char* file_name, alc_packet_t* alc_packet) {
    if( access( file_name, F_OK ) != -1 ) {
        FILE* f = fopen(file_name, "r+");
        if(f) {
            return f;
        }
    }
    
    //otherwise, pre_allocate this object
    return alc_object_pre_allocate(file_name, alc_packet);
    
}

//nothing to see here...
uint8_t* __TO_PREALLOC_ZERO_SLAB_PTR = NULL;
FILE* alc_object_pre_allocate(char* file_name, alc_packet_t* alc_packet) {
	if(!__TO_PREALLOC_ZERO_SLAB_PTR) {
		__TO_PREALLOC_ZERO_SLAB_PTR = (uint8_t*)malloc(__TO_PREALLOC_ZERO_SLAB_SIZE);
		memset(__TO_PREALLOC_ZERO_SLAB_PTR, 0, __TO_PREALLOC_ZERO_SLAB_SIZE);
	}

    if( access( file_name, F_OK ) != -1 ) {
        __ALC_UTILS_WARN("pre_allocate: file %s exists, removing", file_name);
        // file exists
        remove(file_name);
    }
    
    FILE* f = fopen(file_name, "w");
    if(!f) {
        __ALC_UTILS_WARN("pre_allocate: unable to open %s", file_name);
        return NULL;
    }
    
    uint32_t to_allocate_size = alc_packet->transfer_len;
    if(to_allocate_size) {
    	__ALC_UTILS_WARN("pre_allocate: before: file %s to size: %d", file_name, to_allocate_size);
        uint32_t alloc_offset = 0;
        uint32_t blocksize;
        uint32_t loop_count = 0;
        while(alloc_offset < to_allocate_size) {
        	blocksize = __MIN(__TO_PREALLOC_ZERO_SLAB_SIZE, to_allocate_size - alloc_offset);
            fwrite(__TO_PREALLOC_ZERO_SLAB_PTR, blocksize, 1, f);
            alloc_offset += blocksize;
            loop_count++;
        }
        __ALC_UTILS_IOTRACE("pre_allocate: after: file %s to size: %d, wrote out: %u in %u fwrite", file_name, to_allocate_size, alloc_offset, loop_count);

    } else {
        __ALC_UTILS_WARN("pre_allocate: file %s, transfer_len is 0, not pre allocating", file_name);
    }
    fclose(f);
    f = fopen(file_name, "r+");
   
    return f;
}

int alc_packet_write_fragment(FILE* f, char* file_name, uint32_t offset, alc_packet_t* alc_packet) {
    
	__ALC_UTILS_IOTRACE("write fragment: tsi: %u, toi: %u, sbn: %x, esi: %x len: %d, complete: %d, file: %p, file name: %s, offset: %u, size: %u",  alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi,
        alc_packet->sbn, alc_packet->esi, alc_packet->alc_len, alc_packet->close_object_flag,
        f, file_name, offset, alc_packet->alc_len);

    fseek(f, offset, SEEK_SET);
    int blocks_written = fwrite(alc_packet->alc_payload, alc_packet->alc_len, 1, f);
   
    if(blocks_written != 1) {
        __ALC_UTILS_WARN("short packet write: blocks: %u", blocks_written);
        return 0;
    }
    
    return alc_packet->alc_len;
}

int alc_packet_dump_to_object(alc_packet_t** alc_packet_ptr) {

	alc_packet_t* alc_packet = *alc_packet_ptr;
	int bytesWritten = 0;

	if(!_ALC_PACKET_DUMP_TO_OBJECT_ENABLED) {
        return -1;
    }
    
    char* file_name = alc_packet_dump_to_object_get_filename(alc_packet);
    mkdir("route", 0777);

    FILE *f = NULL;

    if(alc_packet->use_sbn_esi) {
        //raptor fec, use the esi to see if we should write out to a new file vs append
        if(!alc_packet->esi) {
            f = alc_object_pre_allocate(file_name, alc_packet);
            __ALC_UTILS_IOTRACE("raptor_fec: done creating new pre-allocation for %s, size: %llu", file_name, alc_packet->transfer_len);
        } else {
            f = alc_object_open_or_pre_allocate(file_name, alc_packet);
        }
        if(!f) {
            __ALC_UTILS_WARN("alc_packet_dump_to_object, unable to open file: %s", file_name);
            return -2;
        }
        alc_packet_write_fragment(f, file_name, alc_packet->esi, alc_packet);
        __ALC_UTILS_IOTRACE("raptor_fec: done writing out fragment for %s", file_name);

    } else if(alc_packet->use_start_offset){
        if(!alc_packet->start_offset) {
            f = alc_object_pre_allocate(file_name, alc_packet);
            __ALC_UTILS_IOTRACE("done creating new pre-allocation fragment %s, size: %llu", file_name, alc_packet->transfer_len);

        } else {
            f = alc_object_open_or_pre_allocate(file_name, alc_packet);
        }
        if(!f) {
            __ALC_UTILS_WARN("alc_packet_dump_to_object, unable to open file: %s", file_name);
            return -2;
        }
        
        alc_packet_write_fragment(f, file_name, alc_packet->start_offset, alc_packet);
        __ALC_UTILS_IOTRACE("done writing out fragment for %s", file_name);

    } else {
        __ALC_UTILS_WARN("alc_packet_dump_to_object, no alc offset strategy for file: %s", file_name);
    }
	
    if(f) {
        fclose(f);
        f = NULL;
    }
    
	//also investigate alc_packet->transfer_len if we dont get a close object tag
	if(alc_packet->close_object_flag) {
		//__ALC_UTILS_DEBUG("dumping to file done: %s, is complete: %d", file_name, alc_packet->close_object_flag);
	} else {
		//__ALC_UTILS_DEBUG("dumping to file step: %s, is complete: %d", file_name, alc_packet->close_object_flag);
	}
	//__ALC_RECON_MONITOR
	//push our fragments EXCEPT for the mpu fragment box, we will pull that at the start of a
		if(__ALC_RECON_MONITOR) {
			__ALC_UTILS_IOTRACE("checking tsi: %u, toi: %u, close_object_flag: %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

			if(alc_packet->close_object_flag && ((alc_packet->def_lct_hdr->tsi == __ALC_RECON_MONITOR->video_tsi && alc_packet->def_lct_hdr->toi != __ALC_RECON_MONITOR->video_toi_init) ||
					(alc_packet->def_lct_hdr->tsi == __ALC_RECON_MONITOR->audio_tsi && alc_packet->def_lct_hdr->toi != __ALC_RECON_MONITOR->audio_toi_init))) {

					alc_recon_file_buffer_struct_monitor_fragment_with_init_box(__ALC_RECON_MONITOR, alc_packet);
			}
		}


cleanup:
	if(file_name) {
		free(file_name);
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

void __alc_recon_fragment_with_init_box(char* file_name, alc_packet_t* alc_packet, uint32_t tsi, uint32_t toi_init, const char* to_write_filename) {


	char* init_file_name = (char*)calloc(255, sizeof(char));
	char* recon_file_name = (char*)calloc(255, sizeof(char)); //.m4v == 4
	FILE* recon_output_file = NULL;

	__ALC_UTILS_DEBUG(" alc_recon_fragment_with_init_box: %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, tsi, toi_init);
	snprintf(recon_file_name, 255, "%s%s", __ALC_DUMP_OUTPUT_PATH__, to_write_filename );

	if(!__ALC_RECON_HAS_WRITTEN_INIT_BOX) {


		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		fread(init_payload, st.st_size, 1, init_file);
		fclose(init_file);
		recon_output_file = fopen(recon_file_name, "w");
		if(!recon_output_file) {
			__ALC_UTILS_ERROR("unable to open recon_output_file for writing: %s", recon_file_name);
			return;
		}
		fwrite(init_payload, st.st_size, 1, recon_output_file);
		__ALC_RECON_HAS_WRITTEN_INIT_BOX = true;

	} else {
		recon_output_file = fopen(recon_file_name, "a");
		if(!recon_output_file) {
		__ALC_UTILS_ERROR("unable to open recon_output_file for append: %s", recon_file_name);
		return;
		}
	}

	uint64_t block_size = 8192;

	FILE* m4v_fragment_input_file = fopen(file_name, "r");
	uint8_t* m4v_payload = (uint8_t*)calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		return;
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
		__ALC_RECON_FILE_PTR_TSI = (uint32_t*)calloc(1, sizeof(uint32_t));
	}
	*__ALC_RECON_FILE_PTR_TSI = tsi;


	if(!__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_RECON_FILE_PTR_TOI_INIT = (uint32_t*)calloc(1, sizeof(uint32_t));
		}
	*__ALC_RECON_FILE_PTR_TOI_INIT = toi_init;
}

void alc_recon_file_ptr_fragment_with_init_box(FILE* output_file_ptr, alc_packet_t* alc_packet, uint32_t to_match_toi_init) {
	int flush_ret = 0;
	if(!__ALC_RECON_FILE_PTR_TSI || !__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_UTILS_WARN("alc_recon_file_ptr_fragment_with_init_box - NULL: tsi: %p, toi: %p", __ALC_RECON_FILE_PTR_TSI, __ALC_RECON_FILE_PTR_TOI_INIT);
		return;
	}

	char* file_name = alc_packet_dump_to_object_get_filename(alc_packet);
	uint32_t toi_init = to_match_toi_init;

	char* init_file_name = (char* )calloc(255, sizeof(char));

	__ALC_UTILS_DEBUG("recon %u, %u, %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, *__ALC_RECON_FILE_PTR_TSI, toi_init);

	if(!__ALC_RECON_FILE_PTR_HAS_WRITTEN_INIT_BOX) {
		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
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
	uint8_t* m4v_payload = (uint8_t*)calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		return;
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
		__ALC_RECON_FILE_PTR_TSI = (uint32_t*)calloc(1, sizeof(uint32_t));
	}

	if(!__ALC_RECON_FILE_PTR_TOI_INIT) {
		__ALC_RECON_FILE_PTR_TOI_INIT = (uint32_t*)calloc(1, sizeof(uint32_t));
	}

	*__ALC_RECON_FILE_PTR_TSI = tsi;
	*__ALC_RECON_FILE_PTR_TOI_INIT = toi_init;
}



void alc_recon_file_buffer_struct_set_monitor(lls_sls_alc_monitor_t* lls_sls_alc_monitor) {
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
	char* init_file_name = (char*)calloc(255, sizeof(char));

	__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_fragment_with_init_box - ENTER - %u, %u,  %d", alc_packet->def_lct_hdr->tsi, alc_packet->def_lct_hdr->toi, alc_packet->close_object_flag);

	snprintf(init_file_name, 255, "%s%u-%u", __ALC_DUMP_OUTPUT_PATH__, *__ALC_RECON_FILE_PTR_TSI, toi_init);

	pipe_buffer_reader_mutex_lock(pipe_ffplay_buffer);

	if(!pipe_ffplay_buffer->has_written_init_box) {
		if( access( init_file_name, F_OK ) == -1 ) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
		}

		struct stat st;
		stat(init_file_name, &st);

		uint8_t* init_payload = (uint8_t*)calloc(st.st_size, sizeof(uint8_t));
		FILE* init_file = fopen(init_file_name, "r");
		if(!init_file || st.st_size == 0) {
			__ALC_UTILS_ERROR("unable to open init file: %s", init_file_name);
			return;
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
	uint8_t* m4v_payload = (uint8_t*)calloc(block_size, sizeof(uint8_t));
	if(!m4v_fragment_input_file) {
		__ALC_UTILS_ERROR("unable to open m4v fragment input: %s", file_name);
		return;
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


void alc_recon_file_buffer_struct_monitor_fragment_with_init_box(lls_sls_alc_monitor_t* lls_sls_alc_monitor, alc_packet_t* alc_packet) {
	int flush_ret = 0;
	char* audio_init_file_name = NULL;
	char* video_init_file_name = NULL;
	char* audio_fragment_file_name = NULL;
	char* video_fragment_file_name = NULL;
	block_t* audio_fragment_payload = NULL;
	block_t* video_fragment_payload = NULL;
	block_t* audio_init_payload = NULL;
	block_t* video_init_payload = NULL;

	//alc_packet->def_lct_hdr->toi hack
	if(alc_packet->def_lct_hdr->tsi == lls_sls_alc_monitor->audio_tsi) {
		lls_sls_alc_monitor->last_closed_audio_toi = alc_packet->def_lct_hdr->toi;
		if(alc_packet->ext_route_presentation_ntp_timestamp_set && !lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time_set) {
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time = alc_packet->ext_route_presentation_ntp_timestamp;
			compute_ntp64_to_seconds_microseconds(lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time, &lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time_s, &lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time_us);
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff.mpu_presentation_time_set = true;
		}
	}

	if(alc_packet->def_lct_hdr->tsi == lls_sls_alc_monitor->video_tsi) {
		lls_sls_alc_monitor->last_closed_video_toi = alc_packet->def_lct_hdr->toi;
		if(alc_packet->ext_route_presentation_ntp_timestamp_set && !lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time_set) {
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time = alc_packet->ext_route_presentation_ntp_timestamp;
			compute_ntp64_to_seconds_microseconds(lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time, &lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time_s, &lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time_us);
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff.mpu_presentation_time_set = true;

		}
	}

	//we may have short audio/video packets, so allow our buffer accumulate and then flush independently after we have written our initbox
    uint32_t audio_toi = lls_sls_alc_monitor->last_closed_audio_toi;
    uint32_t video_toi = lls_sls_alc_monitor->last_closed_video_toi;
    
	audio_fragment_file_name = alc_packet_dump_to_object_get_filename_tsi_toi(lls_sls_alc_monitor->audio_tsi, audio_toi);
	video_fragment_file_name = alc_packet_dump_to_object_get_filename_tsi_toi(lls_sls_alc_monitor->video_tsi, video_toi);

	if(!lls_sls_alc_monitor->lls_sls_monitor_output_buffer.has_written_init_box) {
		audio_init_file_name = alc_packet_dump_to_object_get_filename_tsi_toi(lls_sls_alc_monitor->audio_tsi, lls_sls_alc_monitor->audio_toi_init);
		video_init_file_name = alc_packet_dump_to_object_get_filename_tsi_toi(lls_sls_alc_monitor->video_tsi, lls_sls_alc_monitor->video_toi_init);

		audio_init_payload = alc_get_payload_from_filename(audio_init_file_name);
		video_init_payload = alc_get_payload_from_filename(video_init_file_name);

		audio_fragment_payload = alc_get_payload_from_filename(audio_fragment_file_name);
		video_fragment_payload = alc_get_payload_from_filename(video_fragment_file_name);

		if(audio_init_payload && video_init_payload &&
			audio_fragment_payload && video_fragment_payload) {

			__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - audio: %s, video: %s", audio_init_file_name, video_init_file_name);

			lls_sls_monitor_output_buffer_copy_audio_init_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer, audio_init_payload);
			lls_sls_monitor_output_buffer_copy_video_init_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer, video_init_payload);
			lls_sls_monitor_output_buffer_copy_audio_fragment_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer, audio_fragment_payload);
			lls_sls_monitor_output_buffer_copy_video_fragment_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer, video_fragment_payload);
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.has_written_init_box = true;
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer = true;
	        lls_sls_alc_monitor->last_closed_audio_toi = 0;
	        lls_sls_alc_monitor->last_closed_audio_toi = 0;

			lls_sls_alc_monitor->last_pending_flushed_audio_toi = 0;
			lls_sls_alc_monitor->last_pending_flushed_video_toi = 0;

	        lls_sls_alc_monitor->last_completed_flushed_audio_toi = audio_toi;
	        lls_sls_alc_monitor->last_completed_flushed_video_toi = video_toi;


		} else {
			__ALC_UTILS_ERROR("missing init payloads, audio: %p, video: %p", audio_init_payload, video_init_payload);
			goto cleanup;
		}
	} else {

		//append audio if we have an audio frame
		if(audio_toi && audio_fragment_file_name) {
			audio_fragment_payload = alc_get_payload_from_filename(audio_fragment_file_name);
			if(audio_fragment_payload) {
				lls_sls_monitor_output_buffer_merge_alc_fragment_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer.audio_output_buffer_isobmff, audio_fragment_payload);
				lls_sls_alc_monitor->last_closed_audio_toi = 0;
				lls_sls_alc_monitor->last_pending_flushed_audio_toi = audio_toi;
				__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - pushing audio fragment: %d, file: %s", audio_toi, audio_fragment_file_name);
			} else {
				__ALC_UTILS_ERROR("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - missing audio fragment: %d, file: %s", audio_toi, audio_fragment_file_name);
			}
		}

		//append video if we have a video frame
		if(video_toi && video_fragment_file_name) {
			video_fragment_payload = alc_get_payload_from_filename(video_fragment_file_name);
			if(video_fragment_payload) {
				lls_sls_monitor_output_buffer_merge_alc_fragment_block(&lls_sls_alc_monitor->lls_sls_monitor_output_buffer.video_output_buffer_isobmff, video_fragment_payload);

				lls_sls_alc_monitor->last_closed_video_toi = 0;
				lls_sls_alc_monitor->last_pending_flushed_video_toi = video_toi;

				__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - pushing video fragment: %d, file: %s", video_toi, video_fragment_file_name);
			} else {
				__ALC_UTILS_ERROR("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - missing video fragment: %d, file: %s", video_toi, video_fragment_file_name);
			}
		}

		if(lls_sls_alc_monitor->last_pending_flushed_audio_toi && lls_sls_alc_monitor->last_pending_flushed_video_toi) {
			lls_sls_alc_monitor->lls_sls_monitor_output_buffer.should_flush_output_buffer = true;
			__ALC_UTILS_DEBUG("alc_recon_file_buffer_struct_monitor_fragment_with_init_box - setting should_flush output buffer for: audio fragment: %d, video fragment: %d",
					lls_sls_alc_monitor->last_pending_flushed_audio_toi, lls_sls_alc_monitor->last_pending_flushed_video_toi);



			lls_sls_alc_monitor->last_completed_flushed_audio_toi = lls_sls_alc_monitor->last_pending_flushed_audio_toi;
			lls_sls_alc_monitor->last_completed_flushed_video_toi = lls_sls_alc_monitor->last_pending_flushed_video_toi;

			lls_sls_alc_monitor->last_closed_audio_toi = 0;
			lls_sls_alc_monitor->last_pending_flushed_audio_toi = 0;
			lls_sls_alc_monitor->last_closed_video_toi = 0;
			lls_sls_alc_monitor->last_pending_flushed_video_toi = 0;
		}
	}

cleanup:
	freesafe(audio_init_file_name);
	freesafe(video_init_file_name);
	freesafe(audio_fragment_file_name);
	freesafe(video_fragment_file_name);
	block_Release(&audio_fragment_payload);
	block_Release(&video_fragment_payload);
	block_Release(&audio_init_payload);
	block_Release(&video_init_payload);

	return;
}

