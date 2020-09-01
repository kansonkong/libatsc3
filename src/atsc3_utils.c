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

long gtl() {
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
        
    return time_now.tv_sec * 1000 + time_now.tv_usec / 1000;
}

//walk thru [] of uint8*s and move our pointer for N elements
void* extract(uint8_t *bufPosPtr, uint8_t *dest, int size) {
	for(int i=0; i < size; i++) {
		dest[i] = *bufPosPtr++;
	}
	return bufPosPtr;
}

void kvp_collection_free(kvp_collection_t* collection) {
	if(!collection) {
		return;
	}

	if(!collection->size_n && !collection->kvp_collection) {
		free(collection);
		return;
	}

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

/*
 if we have a match, then return a strdup'd version of the value we match on, otherwise return null
 */
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

//0 size block allocs are allowed to create a block_t with only padding alignment for block_Write, etc...

block_t* block_Alloc(int size_requested) {
	block_t* new_block = (block_t*)calloc(1, sizeof(block_t));
	assert(new_block);

	//calloc an extra byte in case we forget to add in null padding for strings, but don't update the p_size with this margin of safety (16 bytes)
	//align if size_requested > 0, otherwise alloc 8 as a dummy alloc block
	uint32_t aligned_size = size_requested ? size_requested + 16 + (8 - (size_requested %8))    :    8;

	#ifdef __MALLOC_TRACE
	    _ATSC3_UTILS_INFO("block_Alloc: original size requested: %u, aligned size: %u, alignment factor: %f", src_size_required, aligned_size, aligned_size/8.0);
	#endif

	new_block->p_buffer = (uint8_t*)calloc(aligned_size, sizeof(uint8_t));
	assert(new_block->p_buffer);

	new_block->p_size = size_requested;
	new_block->i_pos = 0;
    new_block->_refcnt = 1;
    new_block->_is_alloc = 1;
    new_block->_a_size = aligned_size - 8; //guard edge

    return new_block;
}

bool block_IsAlloc(block_t* block) {
	return block->_is_alloc != 0;
}

static block_t* __block_Alloc_internal(int size_requested) {
	block_t* block = block_Alloc(size_requested);
	block->_is_alloc = 0;

	return block;
}

block_t* block_Promote(const char* string) {
	int string_len = strlen(string);
	block_t* new_block = __block_Alloc_internal(string_len);
	block_Write(new_block, (uint8_t*)string, string_len);

	return new_block;
}
//todo: make this a marco define?
block_t* __block_check_bounaries(const char* method_name, block_t* src) {
	//these are FATAL conditions, return NULL
    if(!src) {
        _ATSC3_UTILS_ERROR("%s: block_t is null: %p", method_name, src);
        return NULL;
    }

	if(!src->p_buffer) {
		_ATSC3_UTILS_ERROR("%s: block: %p, p_buffer is NULL, p_size is: %u, i_pos: %u", method_name, src, src->p_size, src->i_pos);
		src->p_size = 0;
		src->i_pos = 0;
		return NULL;
	}

	if(src->p_size < 0) {
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


uint32_t block_Seek_Relative(block_t* block, int32_t seek_pos) {
    if(!__block_check_bounaries(__FUNCTION__, block)) {
        block->i_pos = 0;
    }
    //keep this one as int32_t so we don't compute based upon wraparound
    int32_t new_seek_pos = block->i_pos + seek_pos;
    
    if(new_seek_pos < 0 ) {
        _ATSC3_UTILS_WARN("block_Seek: block: %p, invalid seek_pos to: %u, clamping to 0",
                          block->p_buffer, seek_pos);
        block->i_pos = 0;
    } else if(new_seek_pos > block->p_size) {
        _ATSC3_UTILS_WARN("block_Seek: block: %p, invalid seek_pos to: %u, clamping to %u",
                          block->p_buffer, seek_pos, block->p_size);
        block->i_pos = block->p_size;
    } else {
        block->i_pos = new_seek_pos;
    }
    
    return block->i_pos;
}

block_t* block_Write(block_t* dest, const uint8_t* src_buf, uint32_t src_size) {
	if(!__block_check_bounaries(__FUNCTION__, dest)) return NULL;

	uint32_t dest_size_required = dest->i_pos + src_size;
	if(dest->p_size < dest_size_required) {
		block_t* ret_block = block_Resize_Soft(dest, dest_size_required);
		if(!ret_block) {
			_ATSC3_UTILS_ERROR("block_Write: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
			return NULL;
		}
	}
	memcpy(&dest->p_buffer[dest->i_pos], src_buf, src_size);
	dest->i_pos += src_size;

	return dest;
}

//use src i_pos to append before
//this will append the leader of the source block...
uint32_t block_Append(block_t* dest, block_t* src) {
	if(!__block_check_bounaries(__FUNCTION__, dest)) return 0;

	uint32_t dest_size_required = dest->i_pos + src->i_pos;
	if(dest->p_size < dest_size_required) {
		block_t* ret_block = block_Resize_Soft(dest, dest_size_required);
		if(!ret_block) {
			_ATSC3_UTILS_ERROR("block_Append: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
			return 0;
		}
	}
	memcpy(&dest->p_buffer[dest->i_pos], src->p_buffer, src->i_pos);
	dest->i_pos += src->i_pos;

	return dest->i_pos;
}


//combine two blocks at dest->i_pos, block_Get(src), len: src->p_size - src->i_pos

block_t* block_AppendFromSrciPos(block_t* dest, block_t* src) {
	if(!__block_check_bounaries(__FUNCTION__, dest)) return 0;

	uint32_t src_len = __MAX(0, src->p_size - src->i_pos);
	if(!src_len) {
		return dest; //bail early
	}

	uint32_t dest_size_required = dest->i_pos + src_len;
	if(dest->p_size < dest_size_required) {
		block_t* ret_block = block_Resize_Soft(dest, dest_size_required);
		if(!ret_block) {
			_ATSC3_UTILS_ERROR("block_Append: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
			return 0;
		}
	}
	memcpy(&dest->p_buffer[dest->i_pos], block_Get(src), src_len);
	dest->i_pos += src_len; //src->i_pos;

	return dest;
}

block_t* block_AppendFromBuf(block_t* dest, const uint8_t* src_buf, uint32_t src_size) {
	if(!__block_check_bounaries(__FUNCTION__, dest)) return NULL;

	dest->i_pos = dest->p_size;
	uint32_t dest_original_size = dest->p_size;
	uint32_t dest_size_required = dest->p_size + src_size;

	if(dest->p_size < dest_size_required) {
		block_t* ret_block = block_Resize_Soft(dest, dest_size_required);
		if(!ret_block) {
			_ATSC3_UTILS_ERROR("block_Write: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
			return NULL;
		}
	}
	memcpy(&dest->p_buffer[dest_original_size], src_buf, src_size);
	dest->i_pos = dest_size_required;

	return dest;
}


//use src i_pos to append, with the full payload from src
uint32_t block_AppendFull(block_t* dest, block_t* src) {
    if(!__block_check_bounaries(__FUNCTION__, dest)) return 0;
    
    uint32_t dest_size_required = dest->i_pos + src->p_size;
    if(dest->p_size < dest_size_required) {
        block_t* ret_block = block_Resize_Soft(dest, dest_size_required);
        if(!ret_block) {
            _ATSC3_UTILS_ERROR("block_AppendFull: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
            return 0;
        }
    }
    memcpy(&dest->p_buffer[dest->i_pos], src->p_buffer, src->p_size);
    dest->i_pos += src->p_size;
    
    return dest->i_pos;
}

//combine 2 separate block_t's into one full payload, using p_size, rather than i_pos for appending
uint32_t block_Merge(block_t* dest, block_t* src) {
    if(!__block_check_bounaries(__FUNCTION__, dest)) return 0;

    //seek us forward so we maintain both block_t full payloads
    dest->i_pos = dest->p_size;
    uint32_t dest_original_size = dest->p_size;
    uint32_t dest_size_required = dest->p_size + src->p_size;
    
    if(dest->p_size < dest_size_required) {
        block_t* ret_block = block_Resize_Soft(dest, dest_size_required);
        if(!ret_block) {
            _ATSC3_UTILS_ERROR("block_Merge: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
            return 0;
        }
    }
    memcpy(&dest->p_buffer[dest_original_size], src->p_buffer, src->p_size);
    //rewind
    dest->i_pos = 0;
    
    return dest->p_size;
}



uint32_t block_MergeNoRewind(block_t* dest, block_t* src) {
    if(!__block_check_bounaries(__FUNCTION__, dest)) return 0;


    uint32_t dest_original_size = dest->p_size;
    uint32_t dest_size_required = dest->p_size + src->p_size;

    if(dest->p_size < dest_size_required) {
        block_t* ret_block = block_Resize_Soft(dest, dest_size_required);
        if(!ret_block) {
            _ATSC3_UTILS_ERROR("block_Merge: block: %p, unable to realloc from size: %u to %u, returning NULL", dest, dest->p_size, dest_size_required);
            return 0;
        }
    }
    memcpy(&dest->p_buffer[dest_original_size], src->p_buffer, src->p_size);
    //seek us forward so we maintain both block_t full payloads
    dest->i_pos = dest->p_size;

    return dest->p_size;
}


block_t* block_Rewind(block_t* dest) {
	if(!__block_check_bounaries(__FUNCTION__, dest)) return NULL;
	dest->i_pos = 0;
	return dest;
}
/**
 * todo, fix me to use ** to null out block_t ref
 */

/**
 * note, this will duplicate the full block size and update i_pos in the dest payload
 * if you need a head trimmed version of the payload at i_pos, use block_Duplicate_from_position
 * if you need a tail trimmed version of the payload N bytes long, block_Duplicate_to_size
 */
block_t* block_Duplicate(block_t* src) {
	if(!__block_check_bounaries(__FUNCTION__, src)) return NULL;

	uint32_t to_alloc_size = src->p_size;

	block_t* dest = __block_Alloc_internal(to_alloc_size);
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

	block_t* dest = __block_Alloc_internal(to_alloc_size);
	memcpy(dest->p_buffer, &src->p_buffer[src->i_pos], to_alloc_size);
	dest->i_pos = 0;

	return dest;
}

uint8_t* block_Get(block_t* src) {
    if(!__block_check_bounaries(__FUNCTION__, src)) return NULL;
    return &src->p_buffer[src->i_pos];
}
uint32_t block_Len(block_t* src) {
    if(!__block_check_bounaries(__FUNCTION__, src)) return 0;
    return src->p_size;
}
/**
 *
 * this will return a new block starting 0 up to target_len
 */

block_t* block_Duplicate_to_size(block_t* src, uint32_t target_len) {
	if(!__block_check_bounaries(__FUNCTION__, src)) return NULL;

	uint32_t to_alloc_size = __CLIP(target_len, 8, src->p_size);

	block_t* dest = __block_Alloc_internal(to_alloc_size);
	memcpy(dest->p_buffer, src->p_buffer, to_alloc_size);
	dest->i_pos = to_alloc_size;

	return dest;
}

// create new block_t from *data and size
block_t* block_Duplicate_from_ptr(uint8_t* data, uint32_t size) {
    block_t* block_t = __block_Alloc_internal(size);
    block_Write(block_t, data, size);
    return block_t;
}

#define __ATSC3_UTILS_BLOCK_RESIZE_DOUBLE_LIMIT__ 2048000
//perform a soft allocation to
//src->p_size * 2  where p_size < 2M
block_t* block_Resize_Soft(block_t* dest, uint32_t dest_size_min_required) {

	if(dest->_a_size >= dest_size_min_required && dest->i_pos < dest_size_min_required && dest->p_size < dest_size_min_required) {
		dest->p_size = dest_size_min_required;
    	return dest;
	}

	uint32_t target_dest_size_min_required = dest_size_min_required;

	if(dest_size_min_required * 2 < __ATSC3_UTILS_BLOCK_RESIZE_DOUBLE_LIMIT__) {
		target_dest_size_min_required = dest_size_min_required * 2;
	}

	block_Resize(dest, target_dest_size_min_required);
	dest->p_size = dest_size_min_required;

	return dest;
}


//return will be NULL if realloc failed, but src will still be valid
//grow or shrink a block,
//  if growing, null out new payload, maintain i_pos
//  if shrinking, discard end payload, i_pos will reset to 0


block_t* block_Resize(block_t* src, uint32_t src_size_requested) {
	if(!__block_check_bounaries(__FUNCTION__, src)) return NULL;
    uint32_t src_size_original = src->p_size;
    uint32_t src_i_pos_original = src->i_pos;


    //try and avoid a realloc if we have enough space from our original _a_size allocation
	if(src_size_requested <= src->_a_size) {
		if(src_size_requested > src->p_size) {
			//soft increase, so null out our "extended" p_size area
#ifdef __MALLOC_TRACE
		    _ATSC3_UTILS_WARN("block_Resize: p_size: %d, a_size: %d, src_size_requested: %d, doing memset len: %d",
		    		src->p_size, src->_a_size, src_size_requested, (src_size_requested - src->p_size));
#endif
			memset(&src->p_buffer[src->p_size], 0, (src_size_requested - src->p_size));
		} else {
			//soft decrease, set i_pos to 0 - don't null out data (lazy)
#ifdef __MALLOC_TRACE
			_ATSC3_UTILS_WARN("block_Resize: p_size: %d, a_size: %d, src_size_requested: %d, no memset",
					    		src->p_size, src->_a_size, src_size_requested);
#endif
			src->i_pos = 0;
		}

		src->p_size = src_size_requested;
    	return src;
    }

    //uint32_t src_size_required = __MAX(64, src_size_requested);
    //do not change our size, as this can cause us to leak unexpectedly
    uint32_t src_size_required = src_size_requested;

	//always over alloc by X bytes for a null pad
    //jjustman-2019-10-12: TODO - devices like aarm64 need a quad byte aligned boundary (aim for 64bit align)
    //over-allow with pad but don't set our p_size to this value as its not "accurate" when appending
    uint32_t aligned_size = src_size_required + 8 + (8 - (src_size_required %8));

#ifdef __MALLOC_TRACE
    _ATSC3_UTILS_WARN("block_Resize: p_size: %d, a_size: %d, original size requested: %u, aligned size: %u, alignment factor: %f",
    		src->p_size, src->_a_size, src_size_required, aligned_size, aligned_size/8.0);
#endif

    src->_a_size = aligned_size;
	void* new_block = realloc(src->p_buffer, aligned_size);
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
            if(src_size_original > src_size_required) {
                //shrink
                src->i_pos = 0;
            } else {
                //grow
                src->i_pos = src_size_original;
                _ATSC3_UTILS_TRACE("block_Resize: grow block: %p, zeroing out from: %u to %u", src, src->i_pos, src->p_size);
                uint32_t to_scrub_len = __MAX(0, (src->p_size - 1 - src->i_pos));
                memset(&src->p_buffer[src->i_pos], 0, to_scrub_len + 8);
                src->i_pos = src_i_pos_original;
            }
		}
	}

	return src;
}

uint32_t block_Remaining_size(block_t* src) {
    if(!__block_check_bounaries(__FUNCTION__, src)) return 0;
    return src->p_size - src->i_pos;
}

bool block_Valid(block_t* src) {
    if(!__block_check_bounaries(__FUNCTION__, src)) return false;
    return true;
}

//
//block_t* _block_Refcount(block_t* a) {
//    if(a) {
//        a->_refcnt++;
//        _ATSC3_UTILS_TRACE("block_Refcount: incrementing to: %d, block: %p (p_buffer: %p)", a->_refcnt, a, a->p_buffer);
//    }
//    return a;
//}

//adding in lazy refcounting to try and avoid doublefrees
void _block_Release(block_t** a_ptr) {
	block_t* a = *a_ptr;
	if(a) {
        if(--a->_refcnt == 0) {
            _ATSC3_UTILS_TRACE("block_Release:FREE: block: %p (p_buffer: %p)", a, a->p_buffer);

            if(a->p_buffer && a->p_size) {
                a->i_pos = 0;
                a->p_size = 0;
                free(a->p_buffer);
                a->p_buffer = NULL;
            }
            free(a);
            *a_ptr = NULL;

        } else {
            _ATSC3_UTILS_TRACE("block_Release:DEC: refcount decremented to: %d, block: %p (p_buffer: %p)", a->_refcnt, a, a->p_buffer);
        }
	}
}
void _block_Refcount(block_t* a_ptr) {
    a_ptr->_refcnt++;
}

void block_Destroy(block_t** a_ptr) {
    block_t* a = *a_ptr;
    if(a) {
        _ATSC3_UTILS_TRACE("block_Destroy: hard freeing block: %p (p_buffer: %p)", a, a->p_buffer);
            
        //jjustman-2020-08-04 - removed && a->p_size as we (can) block_Alloc(0) and still have a p_buffer
        if(a->p_buffer) {
            a->i_pos = 0;
            a->p_size = 0;
            a->_a_size = 0;
            free(a->p_buffer);
            a->p_buffer = NULL;
            free(a);
        }
        a = NULL;
        *a_ptr = NULL;
    }
}

bool block_Tail_Truncate(block_t* src, uint32_t len) {
	if(src && src->p_size > len) {
		for(int i=1; i <= len; i++) {
			src->p_buffer[src->p_size - i] = '\0';
		}
		src->p_size -= len;
		src->i_pos = src->p_size - 1;
		return true;
	} else {
		if(!src) {
			_ATSC3_UTILS_WARN("block_Tail_Truncate: src is NULL");
		} else {
			_ATSC3_UTILS_WARN("block_Tail_Truncate: src: %p, p_size is: %d, tail truncate len is: %d", src->p_buffer, src->p_size, len);

		}
		return false;
	}
}


/*
 bit-unpacking functions for parsing A/322 variable length L1(b/d) structs

 read from a bit pointer field for a variable length size field
 
 e.g. _bitpos = 0 and len = 2 then
        
    res = src->p_buffer[i] >> (8-2) && 0x2
    _bitpos = 2;

 e.g. _bitpos = 7 and len = 3 then
    res = (src->p_buffer[i] & (8 - _bitpos)) << (len - (8 - _bitpos) | (src->p_buffer[i+1]) >> (bitpos + (len - (8-bitpos) % 8) & 0x(len(8- bitpos))
 or so
 
 bit[0]
 --------
 00000000
 
 src->_bitpos is left to right (e.g. 0 to 7)
 
 
 int mask = ((((1 << (loop_read_size - 1)) - 1) << 1) | 1);

 */
uint8_t block_Read_uint8_bitlen(block_t* src, int bitlen) {
    uint8_t ret = 0;
    
    if(bitlen <= (8 - src->_bitpos)) {
        int to_shift = (8 - src->_bitpos) - bitlen;
        int to_mask = ((((1 << (bitlen - 1)) - 1) << 1) | 1);

        //single byte accessor
        ret = (src->p_buffer[src->i_pos] >> to_shift) & to_mask;
        
        src->_bitpos = (src->_bitpos + bitlen) % 8;
        if(src->_bitpos == 0) {
            src->i_pos++;
        }
    } else {
        //we need to wrap to the next byte and read
        int to_mask_msb_n = (8 - src->_bitpos);
        int to_mask_msb = ((((1 << (to_mask_msb_n - 1)) - 1) << 1) | 1);
        int to_shift_l = bitlen - to_mask_msb_n;
        int remainder_to_mask_n = bitlen - to_mask_msb_n;
        int remainder_to_mask = ((((1 << (remainder_to_mask_n - 1)) - 1) << 1) | 1);
        int remainder_to_shift = 8 - remainder_to_mask_n;
        ret = ((src->p_buffer[src->i_pos++] & to_mask_msb) << to_shift_l) | ((src->p_buffer[src->i_pos] >> remainder_to_shift) & remainder_to_mask);

        src->_bitpos = remainder_to_mask_n;
    }
    
    return ret;
}

uint16_t block_Read_uint16_bitlen(block_t* src, int bitlen) {
    uint16_t ret = 0;
    
    int bits_remaining = bitlen;
    while(bits_remaining > 0) {
        int loop_read_size = (bits_remaining > 8 ? 8 - src->_bitpos : (bits_remaining > (8 - src->_bitpos) ? 8 - src->_bitpos : bits_remaining));
        int mask = ((((1 << (loop_read_size - 1)) - 1) << 1) | 1);

        ret |= (src->p_buffer[src->i_pos] & mask) << (bits_remaining - loop_read_size);
        
        bits_remaining -= loop_read_size;
        
        src->_bitpos = (src->_bitpos + loop_read_size) % 8;
        if(src->_bitpos == 0) {
            src->i_pos++;
        }
    }

    return ret;
}

uint32_t block_Read_uint32_bitlen(block_t* src, int bitlen) {
	uint32_t ret = 0;
   
   int bits_remaining = bitlen;
   while(bits_remaining > 0) {
	   int loop_read_size = (bits_remaining > 8 ? 8 - src->_bitpos : (bits_remaining > (8 - src->_bitpos) ? 8 - src->_bitpos : bits_remaining));
	   int mask = ((((1 << (loop_read_size - 1)) - 1) << 1) | 1);

	   ret |= (src->p_buffer[src->i_pos] & mask) << (bits_remaining - loop_read_size);
	   
	   bits_remaining -= loop_read_size;
	   
	   src->_bitpos = (src->_bitpos + loop_read_size) % 8;
	   if(src->_bitpos == 0) {
		   src->i_pos++;
	   }
   }

   return ret;
}

uint64_t block_Read_uint64_bitlen(block_t* src, int bitlen) {
     uint64_t ret = 0;
   
   int bits_remaining = bitlen;
   while(bits_remaining > 0) {
	   int loop_read_size = (bits_remaining > 8 ? 8 - src->_bitpos : (bits_remaining > (8 - src->_bitpos) ? 8 - src->_bitpos : bits_remaining));
	   int mask = ((((1 << (loop_read_size - 1)) - 1) << 1) | 1);

	   ret |= (src->p_buffer[src->i_pos] & mask) << (bits_remaining - loop_read_size);
	   
	   bits_remaining -= loop_read_size;
	   
	   src->_bitpos = (src->_bitpos + loop_read_size) % 8;
	   if(src->_bitpos == 0) {
		   src->i_pos++;
	   }
   }

   return ret;
	
}

//TODO: check for _bitpos
//read from network to host aligned short/long/double long
uint16_t block_Read_uint16_ntohs(block_t* src) {
    uint16_t ret = ntohs(*((uint16_t*)(&src->p_buffer[src->i_pos])));
    src->i_pos += 2;
    return ret;
}

uint32_t block_Read_uint32_ntohl(block_t* src) {
    uint32_t ret = ntohl(*((uint32_t*)(&src->p_buffer[src->i_pos])));
    src->i_pos += 4;
    return ret;
}
uint64_t block_Read_uint64_ntohul(block_t* src) {
    return 0;
}

//read from filesyste into block_t
block_t* block_Read_from_filename(char* file_name) {

	if( access(file_name, F_OK ) == -1 ) {
		_ATSC3_UTILS_ERROR("block_Read_from_filename: unable to open file: %s", file_name);
		return NULL;
	}

	struct stat st;
	stat(file_name, &st);

	block_t* payload = block_Alloc(st.st_size);

	FILE* fp = fopen(file_name, "r");
	if(!fp || st.st_size == 0) {
		_ATSC3_UTILS_ERROR("block_Read_from_filename: size: 0 file: %s", file_name);
		return NULL;
	}

	fread(payload->p_buffer, st.st_size, 1, fp);
	payload->i_pos = st.st_size;
	fclose(fp);

	return payload;
}

void freesafe(void* tofree) {
	if(tofree) {
		free(tofree);
	}
}

void freeclean(void** tofree) {
	if(*tofree) {
		free(*tofree);
		*tofree = NULL;
	}
}


void freeclean_uint8_t(uint8_t** tofree) {
	if(*tofree) {
		free(*tofree);
		*tofree = NULL;
	}
}

uint32_t parseIpAddressIntoIntval(const char* dst_ip_original) {
	if(!dst_ip_original) {
		return 0;
	}
	uint32_t ipAddressAsInteger = 0;
	char* dst_ip = strlcopy((char*)dst_ip_original);

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

uint16_t parsePortIntoIntval(const char* dst_port) {
	if(!dst_port) {
		return 0;
	}

	int dst_port_filter_int = atoi(dst_port);
	uint16_t dst_port_filter = 0;
	dst_port_filter |= dst_port_filter_int & 0xFFFF;

	return dst_port_filter;
}

//alloc and copy - note limited to 16k
char* strlcopy(char* src) {
	int len = strnlen(src, 16384);
	char* dest = (char*)calloc(len+1, sizeof(char));
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

//jjustman-2020-08-04 - dirname() is not reliable to be portable between linux/osx/android
int mkpath(char *dir, mode_t mode)
{
    struct stat sb;
    int ret = 0;

    if (!dir) {
        return 1;
    }

	char* dir_to_process = strlcopy(dir);

	char* pch = strtok (dir_to_process,"/");
	int offset = 0;

	while (pch != NULL && offset>=0) {
		offset += strlen(pch);

		dir[offset] = '\0';
 	    ret = mkdir(dir, mode);
 	   _ATSC3_UTILS_DEBUG("calling mkdir: dir: %s, offset: %d, result: %d\n", dir, offset, ret);
	    dir[offset++] = '/';

		pch = strtok (NULL, "/");
	}
	freesafe(dir_to_process);

    return ret;
}

//don't care if it exists or not, just open the object...
FILE* atsc3_object_open(char* file_name) {
	FILE* f = fopen(file_name, "w+");
	if(f) {
		return f;
	}

	return NULL;
}
