/*
 * atsc3_utils.c
 *
 *  Created on: Jan 19, 2019
 *      Author: jjustman
 */

#include "atsc3_utils.h"

int _ATSC3_UTILS_INFO_ENABLED = 0;
int _ATSC3_UTILS_DEBUG_ENABLED = 0;
int _ATSC3_UTILS_TRACE_ENABLED = 0;


long long timediff(struct timeval t1, struct timeval t0) {
	return  (t1.tv_sec-t0.tv_sec)*1000000LL + t1.tv_usec-t0.tv_usec;
}

double gt() {
	struct timeval time_now;
	gettimeofday(&time_now, NULL);

	return time_now.tv_sec + time_now.tv_usec / 1000000.0;
}

//walk thru [] of uint8*s and move our pointer for N elements
void* extract(uint8_t *bufPosPtr, uint8_t *dest, int size) {
	for(int i=0; i < size; i++) {
		dest[i] = *bufPosPtr++;
	}
	return bufPosPtr;
}

void kvp_collection_free(kvp_collection_t* collection) {
	if(!collection || !collection->size_n) return;

	//free each entry and their corresponding key/val char*
	for(int i=0; i < collection->size_n; i++) {
		kvp_t* kvp_to_free = collection->kvp_collection[i];
		if(kvp_to_free->key) {
			free(kvp_to_free->key);
			kvp_to_free->key = NULL;
		}
		if(kvp_to_free->val) {
				free(kvp_to_free->val);
				kvp_to_free->val = NULL;
		}
		free(kvp_to_free);
		collection->kvp_collection[i] = NULL;
	}

	free(collection->kvp_collection);
	collection->kvp_collection = NULL;
	free(collection);
}

char* kvp_collection_get_reference_p(kvp_collection_t *collection, char* key) {
	for(int i=0; i < collection->size_n; i++) {
		kvp_t* check = collection->kvp_collection[i];
		_ATSC3_UTILS_TRACE("kvp_find_key: checking: %s against %s, resolved val is: %s", key, check->key, check->val);
		if(strcasecmp(key, check->key) == 0) {
			_ATSC3_UTILS_TRACE("kvp_find_key: MATCH for key: %s, resolved val is: %s", check->key, check->val);
			return check->val;
		}
	}
	return NULL;
}

char* kvp_collection_get(kvp_collection_t *collection, char* key) {
	char* val = NULL;
	val = kvp_collection_get_reference_p(collection, key);

	if(!val) return NULL;

	//don't forget our null terminator
	int len = strlen(val) + 1;
	char* newval = (char*)calloc(len, sizeof(char));

	if(!newval) {
		_ATSC3_UTILS_ERROR("kvp_collection_get: unable to clone val return!");
		return NULL;
	}
	memcpy(newval, val, len);
	_ATSC3_UTILS_TRACE("kvp_collection_get: cloning len: %d, val: %s, newval: %s", len, val, newval);
	return newval;
}


kvp_collection_t* kvp_collection_parse(uint8_t* input_string) {
	int input_len = strlen((const char*)input_string);
	_ATSC3_UTILS_TRACE("kvp_parse_string: input string len: %d, input string:\n\n%s\n\n", input_len, input_string);
	kvp_collection_t* collection = (kvp_collection_t*)calloc(1, sizeof(kvp_collection_t));

	//a= is not valid, must be at least 3 chars
	//return an empty collection
	if(input_len < 3)
			return collection;

	//find out how many ='s we have, as that will tell us how many kvp_t entries to create
	//first position can never be =
	int quote_depth = 0;
	int equals_count = 0;
	for(int i=1; i < input_len; i++) {
		if(input_string[i] == '"') {
			if(quote_depth)
				quote_depth--;
			else
				quote_depth++;
		} else if(input_string[i] == '=') {
			if(!quote_depth)
				equals_count++;
		}
	}

	_ATSC3_UTILS_TRACE("parse_kvp_string: creating %d entries", equals_count);

	equals_count = equals_count < 0 ? 0 : equals_count;

	//if we couldn't parse this, just return the empty (0'd collection)
	if(!equals_count) return collection;

	collection->kvp_collection = (kvp_t**)calloc(equals_count, sizeof(kvp_t*));
	collection->size_n = 0;

	quote_depth = 0;
	int token_key_start = 0;
	int token_val_start = 0;

	kvp_t* current_kvp = NULL;

	for(int i=1; i < input_len && collection->size_n < equals_count; i++) {
		if(!current_kvp) {
			//alloc our entry
			collection->kvp_collection[collection->size_n] = (kvp_t*)calloc(1, sizeof(kvp_t));
			current_kvp = collection->kvp_collection[collection->size_n];
		}
		if(isspace(input_string[i]) && !quote_depth) {
			token_key_start = i + 1; //walk forward
		} else {
			if(input_string[i] == '"' && input_string[i-1] != '\\') {
				if(quote_depth) {
					quote_depth--;

					//extract value here
					int len = i - token_val_start;
					assert(current_kvp);

					current_kvp->val = (char*) calloc(len + 1, sizeof(char));
					strncpy(current_kvp->val, (const char*)&input_string[token_val_start], len);
					current_kvp->val[len] = '\0';

					_ATSC3_UTILS_TRACE("parse_kvp_string: marking key: %s, token_val_start: %d, len: %d, val: %s", current_kvp->key, token_val_start, len, current_kvp->val);

					collection->size_n++;
					current_kvp = NULL;

				} else {
					quote_depth++;
					token_val_start = i + 1;
				}
			} else if(input_string[i] == '=') {
				if(!quote_depth) {
					//extract key here
					int len = i - token_key_start;
					assert(current_kvp);
					current_kvp->key = (char*)calloc(len + 1, sizeof(char));
					strncpy(current_kvp->key, (const char*)&input_string[token_key_start], len);
					current_kvp->key[len] = '\0';

					_ATSC3_UTILS_TRACE("parse_kvp_string: marking token_key_start: %d, len: %d, val is: %s", token_key_start, len, current_kvp->key);


				} else {
					//ignore it if we are in a quote value
				}
			}
		}
	}

	_ATSC3_UTILS_TRACE("kvp_parse_string - size is: %d", collection->size_n);
	return collection;
}



block_t* block_Alloc(int len) {
	block_t* new_block = (block_t*)calloc(1, sizeof(block_t));
	assert(new_block);

	//calloc an extra byte in case we forget to add in null padding for strings, but don't update the p_size with this margin of saftey
	new_block->p_buffer = (uint8_t*)calloc(len + 8, sizeof(uint8_t));
	assert(new_block->p_buffer);

	new_block->p_size = len;
	new_block->i_pos = 0;

	return new_block;
}

//todo: make this a marco define?
block_t* __block_check_bounaries(const char* method_name, block_t* src) {
	//these are FATAL conditions, return NULL

	if(!src->p_buffer) {
		_ATSC3_UTILS_ERROR("%s: block: %p, p_buffer is NULL, p_size is: %u, i_pos: %u", method_name, src, src->p_size, src->i_pos);
		src->p_size = 0;
		src->i_pos = 0;
		return NULL;
	}

	if(src->p_size <= 0) {
		_ATSC3_UTILS_ERROR("%s: block: %p, invalid p_size for p_buffer: %p, p_size is: %u, i_pos: %u", method_name, src, src->p_buffer, src->p_size, src->i_pos);
		src->p_size = 0;
		src->i_pos = 0;
		if(src->p_buffer) {
			//let this leak
		}
		src->p_buffer = NULL;
		return NULL;
	}

	//these are under/over-bounary errors and *may* be problematic
	//re-clamp this position
	if(src->i_pos < 0) {
		_ATSC3_UTILS_WARN("%s: block: %p, invalid i_pos, clamping to 0, for p_buffer: %p, p_size is: %u, i_pos: %u", method_name, src, src->p_buffer, src->p_size, src->i_pos);
		src->i_pos = 0;
	}

	if(src->i_pos > src->p_size) {
		uint32_t new_i_pos = src->p_size - 1;
		_ATSC3_UTILS_WARN("%s: block: %p, i_pos is past size for p_buffer: %p, p_size is: %u, i_pos: %u, setting to: %u ", method_name, src, src->p_buffer, src->p_size, src->i_pos, new_i_pos);
		src->i_pos = new_i_pos;
	}

	return src;
}

uint32_t block_Seek(block_t* block, int32_t seek_pos) {
	if(!__block_check_bounaries(__FUNCTION__, block)) {
		block->i_pos = 0;
	}

	if(seek_pos < 0 ) {
		_ATSC3_UTILS_WARN("block_Seek: block: %p, invalid seek_pos to: %u, clamping to 0",
				block->p_buffer, seek_pos);
		block->i_pos = 0;
	} else if(seek_pos > block->p_size) {
		_ATSC3_UTILS_WARN("block_Seek: block: %p, invalid seek_pos to: %u, clamping to %u",
				block->p_buffer, seek_pos, block->p_size);
		block->i_pos = block->p_size;
	} else {
		block->i_pos = seek_pos;
	}

	return block->i_pos;
}


block_t* block_Write(block_t* dest, uint8_t* src_buf, uint32_t src_size) {
	if(!__block_check_bounaries(__FUNCTION__, dest)) return NULL;

	int dest_size_required = dest->i_pos + src_size;
	if(dest->p_size < dest_size_required) {
		block_t* ret_block = block_Resize(dest, dest_size_required);
		if(!ret_block) {
			_ATSC3_UTILS_ERROR("block_Write: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
			return NULL;
		}
	}
	memcpy(&dest->p_buffer[dest->i_pos], src_buf, src_size);
	dest->i_pos += src_size;

	return dest;
}


uint32_t block_Append(block_t* dest, block_t* src) {
	if(!__block_check_bounaries(__FUNCTION__, dest)) return 0;

	int dest_size_required = dest->i_pos + src->i_pos;
	if(dest->p_size < dest_size_required) {
		block_t* ret_block = block_Resize(dest, dest_size_required);
		if(!ret_block) {
			_ATSC3_UTILS_ERROR("block_Write: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
			return 0;
		}
	}
	memcpy(&dest->p_buffer[dest->i_pos], src->p_buffer, src->i_pos);
	dest->i_pos += src->i_pos;

	return dest->i_pos;
}


block_t* block_Rewind(block_t* dest) {
	if(!__block_check_bounaries(__FUNCTION__, dest)) return NULL;

	if(dest->i_pos) {
		uint32_t to_scrub_len = dest->i_pos > 0 ?  __CLIP(dest->i_pos, 0, dest->p_size) : dest->p_size;
		_ATSC3_UTILS_TRACE("block_Rewind, block: %p, zeroing out %u bytes", dest, to_scrub_len)
		memset(dest->p_buffer, 0, to_scrub_len);
	}

	dest->i_pos = 0;
	return dest;
}
/**
 * todo, fix me to use ** to null out block_t ref
 */

/**
 * note, this will duplicate the full block size and update i_pos in the dest payload
 * if you need a subset of the payload, use block_Duplicate_from_position
 */
block_t* block_Duplicate(block_t* src) {
	if(!__block_check_bounaries(__FUNCTION__, src)) return NULL;

	uint32_t to_alloc_size = src->p_size;

	block_t* dest = block_Alloc(to_alloc_size);
	memcpy(dest->p_buffer, src->p_buffer, to_alloc_size);
	dest->i_pos = src->i_pos;

	return dest;
}

/**
 *
 * this will return a new block starting at src->i_pos to the end of the payload size
 */
block_t* block_Duplicate_from_position(block_t* src) {
	if(!__block_check_bounaries(__FUNCTION__, src)) return NULL;

	int32_t to_alloc_size = src->p_size - src->i_pos;
	if(to_alloc_size < 0) {
		_ATSC3_UTILS_WARN("block_Duplicate_from_position: block: %p, p_size was: %u, but i_pos: %u, realloc to size: %u, returning NULL", src, src->p_size, src->i_pos, to_alloc_size);
		return NULL;
	}

	block_t* dest = block_Alloc(to_alloc_size);
	memcpy(dest->p_buffer, &src->p_buffer[src->i_pos], to_alloc_size);
	dest->i_pos = 0;

	return dest;
}

//return will be NULL if realloc failed, but src will still be valid
//this has not been tested with shrinking down the size...
block_t* block_Resize(block_t* src, uint32_t src_size_requested) {
	if(!__block_check_bounaries(__FUNCTION__, src)) return NULL;

	uint32_t src_size_required = __MAX(64, src_size_requested);

	//always over alloc by 1 byte for a null pad
	void* new_block = realloc(src->p_buffer, src_size_required + 8);
	if(!new_block) {
		_ATSC3_UTILS_ERROR("block_Resize: block: %p resize to %u failed, returning NULL", src, src_size_required);
		return NULL;
	} else {
		_ATSC3_UTILS_TRACE("block_Resize: block: %p, p_buffer was: %p, now: %p, resize from %u to %u", src, src->p_buffer, new_block, src->p_size, src_size_required);
		src->p_buffer = (uint8_t*) new_block;
		src->p_size = src_size_required;
		uint32_t to_check_new_i_pos = __MIN(src->p_size - 1, src->i_pos);
		if(to_check_new_i_pos != src->i_pos) {
			_ATSC3_UTILS_WARN("block_Resize: block: %p resize to %u, old pos %u past end of new size, updating to %u", src, src->p_size, src->i_pos, to_check_new_i_pos);
			src->i_pos = to_check_new_i_pos;
		} else {
			_ATSC3_UTILS_TRACE("block_Resize: block: %p, zeroing out from: %u to %u", src, src->i_pos, src->p_size);
			uint32_t to_scrub_len = __MAX(0, (src->p_size - 1 - src->i_pos));
			memset(&src->p_buffer[src->i_pos], 0, to_scrub_len + 8);
		}
	}

	return src;
}


void block_Release(block_t** a_ptr) {
	block_t* a = *a_ptr;
	if(a) {
		if(a->p_buffer && a->p_size) {
			a->i_pos = 0;
			a->p_size = 0;
			free(a->p_buffer);
			a->p_buffer = NULL;
		}
		free(a);
		*a_ptr = NULL;
	}
}

void freesafe(void* tofree) {
	if(tofree) {
		free(tofree);
	}
}

void freeclean(void** tofree) {
	if(*tofree) {
		free(*tofree);
		tofree = NULL;
	}
}


uint32_t parseIpAddressIntoIntval(char* dst_ip_original) {
	uint32_t ipAddressAsInteger = 0;
	char* dst_ip = strlcopy(dst_ip_original);

	char* pch = strtok (dst_ip,".");
	int offset = 24;

	while (pch != NULL && offset>=0) {
		uint8_t octet = atoi(pch);
		ipAddressAsInteger |= octet << offset;
		offset-=8;
		pch = strtok (NULL, " ,.-");
	}
	freesafe(dst_ip);
	return ipAddressAsInteger;
}

uint16_t parsePortIntoIntval(char* dst_port) {

	int dst_port_filter_int = atoi(dst_port);
	uint16_t dst_port_filter = 0;
	dst_port_filter |= dst_port_filter_int & 0xFFFF;

	return dst_port_filter;
}

//alloc and copy - note limited to 16k
char* strlcopy(char* src) {
	int len = strnlen(src, 16384);
	char* dest = (char*)calloc(len, sizeof(char*));
	return strncpy(dest, src, len);
}



/**
 * destructive trim
 *
//    if (totrim > 0) {
//        size_t len = strlen(str);
//        if (totrim == len) {
//            str[0] = '\0';
//        }
//        else {
//            memmove(str, str + totrim, len + 1 - totrim);
//        }
//    }
 */
char *_ltrim(char *str)
{
    size_t totrim;
    char* seps = "\t\n\v\f\r ";

    totrim = strspn(str, seps);
    str += totrim;
    return str;
}

char* _rtrim(char *str)
{
    int i;
    char* seps = "\t\n\v\f\r ";

    i = strlen(str) - 1;
    while (i >= 0 && strchr(seps, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    return str;
}

char* __trim(char *str)
{
    return _ltrim(_rtrim(str));
}
