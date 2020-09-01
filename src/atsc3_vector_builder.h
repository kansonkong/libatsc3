/*
 * atsc3_vector_builder.h
 *
 *  Created on: Mar 8, 2019
 *      Author: jjustman
 */

#include "atsc3_utils.h"

#ifndef ATSC3_VECTOR_BUILDER_H_
#define ATSC3_VECTOR_BUILDER_H_

/*
 * build common vector attributes for struct usage, assumes:
 * 	name: accessor attribute name will resolve to name_v with the following struct attributes:
 * 			name_t name
 * 			size_t cap
 * 			size_t size
 *        datatype will resolve to to name_t
 */
#define ATSC3_VECTOR_BUILDER_STRUCT(vector_item_name) \
 struct {\
    PPCAT(vector_item_name,_t)** data; \
    uint32_t count; \
   	uint32_t size; \
 } PPCAT(vector_item_name, _v);



#define ATSC3_VECTOR_BUILDER_DISTINCT_TYPEDEF_STRUCT_DEFINITION(vector_item_name) \
 typedef struct PPCAT(vector_item_name, _sv) {\
    PPCAT(vector_item_name,_t)** data; \
    uint32_t count; \
   	uint32_t size; \
 } PPCAT(vector_item_name, _tv);



#define ATSC3_VECTOR_BUILDER_DISTINCT_TYPEDEF_STRUCT_INSTANCE(vector_item_name) \
 PPCAT(vector_item_name, _tv) PPCAT(vector_item_name, _v);

/**
 *
 * Example:
 *

	typedef struct atsc3_fdt_file {
		char* 						content_location;
		uint32_t 					toi;
		uint32_t 					content_length;
		uint32_t 					transfer_length;
		char*						content_type;
		char*						content_encoding;
		char*						content_md5;

		char*						app_context_id_list;

		char*						filter_codes;

		atsc3_fdt_fec_attributes_t	atsc3_fdt_fec_attributes;
	} atsc3_fdt_file_t;

	typedef struct atsc3_fdt_instance {
		uint32_t 					expires;
		bool 						complete;
		char* 						content_type;
		char* 						content_encoding;
		atsc3_fdt_fec_attributes_t 	atsc3_fdt_fec_attributes;

		uint8_t						efdt_version;
		uint32_t					max_expires_delta;
		uint32_t					max_transport_size;
		char*						file_template;
		char*						app_context_id_list;
		char*						filter_codes;

		ATSC3_VECTOR_BUILDER_STRUCT(atsc3_fdt_file)

	} atsc3_fdt_instance_t;

	ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_fdt_instance, atsc3_fdt_file)
											vector_struct_name, vector_item_name
*
 *		ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_fdt_instance, atsc3_fdt_file)
 *
 *	results in method signatures for:
 *
 *		atsc3_fdt_instance_t* 	atsc3_fdt_instance_new()
 *
 *		atsc3_fdt_file_t* 		atsc3_fdt_file_new();
 *
 * 		void 			atsc3_fdt_instance_prealloc_atsc3_fdt_file(atsc3_fdt_instance_t*, uint32_t);
 *  	void 			atsc3_fdt_instance_add_atsc3_fdt_file(atsc3_fdt_instance_t*, atsc3_fdt_file*);
 *  	atsc3_fdt_file*	atsc3_fdt_instance_pop_atsc3_fdt_file(atsc3_fdt_instance_t*);
 *
 *		void 			atsc3_fdt_instance_clear_atsc3_fdt_file(atsc3_fdt_instance_t*); 	//empty out container, freeing pointer ref's, but not calling _free impl
 *		void 			atsc3_fdt_instance_free_atsc3_fdt_file(atsc3_fdt_instance_t*);		//invoke atsc3_fdt_file_free, and empty out container
 *
 * 		void 			atsc3_fdt_instance_dealloc_atsc3_fdt_file(atsc3_fdt_instance_t*);	//empty out container, leaving pointer ref's alive, only release our data* block
 *
 * 	 	void 			atsc3_fdt_file_free(atsc3_fdt_file_t**);
 * 							- default impl can be built by calling ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_fdt_file)
 * 	 						- otherwise, you must impl this signature by hand for more complex free(&ptr) use cases
 *
 */


#define ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(vector_struct_name, vector_item_name) \
	PPCAT(vector_item_name,_t)*   PPCAT(vector_item_name,_new)(); \
	PPCAT(vector_struct_name,_t)* PPCAT(vector_struct_name,_new)(); \
	void PPCAT(vector_struct_name,PPCAT(_prealloc_,vector_item_name))(PPCAT(vector_struct_name,_t)*, uint32_t size); \
	void PPCAT(vector_struct_name,PPCAT(_add_,vector_item_name))(PPCAT(vector_struct_name,_t)*, PPCAT(vector_item_name,_t)*); \
	bool PPCAT(vector_struct_name,PPCAT(_remove_,vector_item_name))(PPCAT(vector_struct_name,_t)*, PPCAT(vector_item_name,_t)*); \
	PPCAT(vector_item_name,_t)*   PPCAT(vector_struct_name,PPCAT(_pop_,vector_item_name))(PPCAT(vector_struct_name,_t)*); \
	void PPCAT(vector_struct_name,PPCAT(_clear_,vector_item_name))(PPCAT(vector_struct_name,_t)*); \
	void PPCAT(vector_struct_name,PPCAT(_free_,vector_item_name))(PPCAT(vector_struct_name,_t)*); \
	void PPCAT(vector_struct_name,PPCAT(_dealloc_,vector_item_name))(PPCAT(vector_struct_name,_t)*); \
	void PPCAT(vector_item_name,_free)(PPCAT(vector_item_name,_t)** PPCAT(vector_item_name,_p)); \




 
 /*
  * ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(atsc3_fdt_instance)
  *
  * results in:
  * 	atsc3_fdt_instance_new();
  *
 * jjustman:2019-08-09: todo - add a free method
 */
#define ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE 10

/*
 * ATSC3_VECTOR_BUILDER_METHODS_PARENT_INTERFACE_FREE
 *
 * parent interface free method signature
 *
 * note: jjustman-2020-07-27: all users of ATSC3_VECTOR_BUILDER_METHODS_INTERFACE should either:
 *
 * 	1.) vector_struct_name:  declare in .h and implement this _free method in .c ala:
 *
 *	s/vector_struct_name/atsc3_sls_html_entry_package

 	 void vector_struct_name_free(vector_struct_name_t** vector_struct_name_p) {
		if(vector_struct_name_p) {
			vector_struct_name_t* vector_struct_name = *vector_struct_name_p;
			if(vector_struct_name) {
				//other interior members here
				freesafe(vector_struct_name);
				vector_struct_name = NULL;
			}
			*vector_struct_name_p = NULL;
		}
	}

 *
 * 	and
 *
 * 	2.) vector_item_name:
 * 			no callocs/strudp's:  use default impl ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(vector_struct_name) declared in .c
 *			calloc's as needed :  implement pattern similar to above, using vector_item_name
 *
 *		s/vector_item_name/atsc3_sls_html_entry_package

	  void vector_item_name_free(vector_item_name_t** vector_item_name_p) {
			if(vector_item_name_p) {
				vector_item_name_t* vector_item_name = *vector_item_name_p;
				if(vector_item_name) {
					//other interior members here
					freesafe(vector_item_name);
					vector_item_name = NULL;
				}
				*vector_item_name_p = NULL;
			}
		}
 *
 *
 *
 *
 */

#define ATSC3_VECTOR_BUILDER_METHODS_PARENT_INTERFACE_FREE(vector_struct_name) \
	void PPCAT(vector_struct_name,_free)(PPCAT(vector_struct_name,_t)** PPCAT(vector_struct_name,_p));

//jjustman-2020-07-27 - TODO: add ATSC3_VECTOR_BUILDER_METHODS_PARENT_ITEM_FREE - todo - keep track of internal _v's that we need to dealloc



/* note, if vector_struct_name is also used as vector_item_name,
 * you do not need a METHODS_PARENT_IMPLEMENTATION for now, it will be
 * created automatically in the METHODS_IMPLEMENTATION
 */
#define ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(vector_struct_name) \
	PPCAT(vector_struct_name,_t)* PPCAT(vector_struct_name,_new)() { \
		PPCAT(vector_struct_name,_t)* vector_struct_name = calloc(1, sizeof(PPCAT(vector_struct_name,_t))); \
		return vector_struct_name; \
	} \
	\
\

/**
 *
 *
 *		ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_fdt_instance, atsc3_fdt_file)
 *
 *	results in:
 *		atsc3_fdt_file_t* atsc3_fdt_file_new();
 *  	atsc3_fdt_instance_add_atsc3_fdt_file(atsc3_fdt_instance_t*, atsc3_fdt_file*)
 *
 *	todo: prealloc to N as needed
 *
 *
 * TODO: jjustman-2019-10-03 - use a qsort to re-index keys, e.g.

int comparator_lls_slt_alc_session_t(const void *a, const void *b) {
	_ATSC3_LLS_ALC_UTILS_TRACE("comparator_lls_slt_alc_session_t with %u from %u", ((lls_sls_alc_session_t *)a)->service_id, ((lls_sls_alc_session_t *)b)->service_id);
	if ( ((lls_sls_alc_session_t*)a)->service_id <  ((lls_sls_alc_session_t*)b)->service_id ) return -1;
	if ( ((lls_sls_alc_session_t*)a)->service_id == ((lls_sls_alc_session_t*)b)->service_id ) return  0;
	if ( ((lls_sls_alc_session_t*)a)->service_id >  ((lls_sls_alc_session_t*)b)->service_id ) return  1;
	return 0;
}

 *
	qsort((void**)lls_sls_alc_session_flows->lls_slt_alc_sessions, lls_sls_alc_session_flows->lls_slt_alc_sessions_n, sizeof(lls_sls_alc_session_t**), comparator_lls_slt_alc_session_t);
 *
 *
 */

#define __ATSC3_VECTOR_UTILS_PENDANTIC__
//TODO: make sure we don't duplicate over an existing struct without at least clearing the prior instance
#define ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(vector_struct_name, vector_item_name) \
	PPCAT(vector_item_name,_t)* PPCAT(vector_item_name,_new)() { \
		PPCAT(vector_item_name,_t)* vector_item_name = calloc(1, sizeof(PPCAT(vector_item_name,_t))); \
		return vector_item_name; \
	} \
	ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_NO_CCTOR(vector_struct_name, vector_item_name)

#define ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_NO_CCTOR(vector_struct_name, vector_item_name) \
	void PPCAT(vector_struct_name,PPCAT(_prealloc_,vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name, uint32_t size) { \
		if(!vector_struct_name->PPCAT(vector_item_name, _v).size || !vector_struct_name->PPCAT(vector_item_name, _v).data) { \
			/* new alloc */ \
			vector_struct_name->PPCAT(vector_item_name, _v).data 	= calloc(size, sizeof(PPCAT(vector_item_name,_t)*)); \
			vector_struct_name->PPCAT(vector_item_name, _v).count 	= 0;	\
			vector_struct_name->PPCAT(vector_item_name, _v).size	= size;	\
		} else { \
			/* realloc */ \
			vector_struct_name->PPCAT(vector_item_name, _v).size = 	__MAX(size, vector_struct_name->PPCAT(vector_item_name, _v).count); \
			uint32_t to_resize = sizeof(PPCAT(vector_item_name,_t)*) * vector_struct_name->PPCAT(vector_item_name, _v).size;	\
			vector_struct_name->PPCAT(vector_item_name, _v).data = realloc(vector_struct_name->PPCAT(vector_item_name, _v).data, to_resize);	\
		}	\
	} \
	\
	void PPCAT(vector_struct_name,PPCAT(_add_, vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name, PPCAT(vector_item_name,_t)* vector_item_name) { \
		if(!vector_struct_name->PPCAT(vector_item_name, _v).size || !vector_struct_name->PPCAT(vector_item_name, _v).data) { \
			/* new alloc */ \
			vector_struct_name->PPCAT(vector_item_name, _v).data 	= calloc(ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE, sizeof(PPCAT(vector_item_name,_t)*)); \
			(vector_struct_name->PPCAT(vector_item_name, _v).data[0]) = vector_item_name;	\
			vector_struct_name->PPCAT(vector_item_name, _v).count 	= 1;	\
			vector_struct_name->PPCAT(vector_item_name, _v).size	= ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE;	\
		} else if(vector_struct_name->PPCAT(vector_item_name, _v).count < vector_struct_name->PPCAT(vector_item_name, _v).size) {	\
			/* push to back if we have available space (count < size) */ \
			vector_struct_name->PPCAT(vector_item_name, _v).data[vector_struct_name->PPCAT(vector_item_name, _v).count++] = vector_item_name;	\
		} else { \
			/* realloc */ \
			vector_struct_name->PPCAT(vector_item_name, _v).size = __MAX(vector_struct_name->PPCAT(vector_item_name, _v).size * 2, \
																		__MAX(vector_struct_name->PPCAT(vector_item_name, _v).count, ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE)); \
			uint32_t to_resize = sizeof(PPCAT(vector_item_name,_t)*) * vector_struct_name->PPCAT(vector_item_name, _v).size;	\
			vector_struct_name->PPCAT(vector_item_name, _v).data = realloc(vector_struct_name->PPCAT(vector_item_name, _v).data, to_resize);	\
			vector_struct_name->PPCAT(vector_item_name, _v).data[vector_struct_name->PPCAT(vector_item_name, _v).count++] = vector_item_name;	\
			\
		}	\
		/* PPCAT(name,_t)* name = calloc(1, sizeof(PPCAT(name,_t))); */ \
		/* return name; */ \
	} \
	bool PPCAT(vector_struct_name,PPCAT(_remove_, vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name, PPCAT(vector_item_name,_t)* vector_item_name) { \
		bool result = false; \
		if(!vector_struct_name->PPCAT(vector_item_name, _v).size || !vector_struct_name->PPCAT(vector_item_name, _v).data) { \
			return result; \
		} \
		for(int i=0; i < vector_struct_name->PPCAT(vector_item_name, _v).count; i++) { \
			PPCAT(vector_item_name,_t)* vector_item_name_to_check = vector_struct_name->PPCAT(vector_item_name, _v).data[i]; \
			if(vector_item_name_to_check == vector_item_name) { \
				result = true; \
				int j=i; \
				/* shift up descendents */ \
				for(; j < vector_struct_name->PPCAT(vector_item_name, _v).count-1; j++) { \
					vector_struct_name->PPCAT(vector_item_name, _v).data[j] = vector_struct_name->PPCAT(vector_item_name, _v).data[j+1]; \
				} \
				vector_struct_name->PPCAT(vector_item_name, _v).count--; \
				vector_struct_name->PPCAT(vector_item_name, _v).data[j] = NULL; \
			}\
		}\
		return result; \
	} \
	PPCAT(vector_item_name,_t)* PPCAT(vector_struct_name,PPCAT(_pop_,vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name) { \
		if(!vector_struct_name->PPCAT(vector_item_name, _v).count) { return NULL; } \
		PPCAT(vector_item_name,_t)* vector_item_name = vector_struct_name->PPCAT(vector_item_name, _v).data[0]; \
		for(int i=0; i < vector_struct_name->PPCAT(vector_item_name, _v).count - 1; i++) { \
			if(vector_struct_name->PPCAT(vector_item_name, _v).data[i+1]) { \
				vector_struct_name->PPCAT(vector_item_name, _v).data[i] = vector_struct_name->PPCAT(vector_item_name, _v).data[i+1]; \
			} \
		} \
		vector_struct_name->PPCAT(vector_item_name, _v).data[vector_struct_name->PPCAT(vector_item_name, _v).count-1] = NULL; \
		vector_struct_name->PPCAT(vector_item_name, _v).count--; \
		return vector_item_name; \
    } \
	\
	/* de-alloc vector AND _free item instances: TODO: jjustman-2020-07-27 - don't invoke _free?,  jjustman-2020-06-02 - fPPCAT(vector_item_name,_free)(&vector_struct_name->PPCAT(vector_item_name, _v).data[i]); */ \
	void PPCAT(vector_struct_name,PPCAT(_clear_, vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name) { \
		if(vector_struct_name->PPCAT(vector_item_name, _v).data) {	\
			for(int i=0; i < vector_struct_name->PPCAT(vector_item_name, _v).count; i++) { \
				if(vector_struct_name->PPCAT(vector_item_name, _v).data[i]) { \
					freesafe(vector_struct_name->PPCAT(vector_item_name, _v).data[i]); \
					vector_struct_name->PPCAT(vector_item_name, _v).data[i] = NULL; \
				} \
			} \
			vector_struct_name->PPCAT(vector_item_name, _v).count = 0; \
			\
		} \
	} \
	void PPCAT(vector_struct_name,PPCAT(_free_, vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name) { \
		if(vector_struct_name->PPCAT(vector_item_name, _v).data) {	\
			for(int i=0; i < vector_struct_name->PPCAT(vector_item_name, _v).count; i++) { \
				if(vector_struct_name->PPCAT(vector_item_name, _v).data[i]) { \
					PPCAT(vector_item_name,_free)(&vector_struct_name->PPCAT(vector_item_name, _v).data[i]); \
					vector_struct_name->PPCAT(vector_item_name, _v).data[i] = NULL; \
				} \
			} \
			vector_struct_name->PPCAT(vector_item_name, _v).count = 0; \
			vector_struct_name->PPCAT(vector_item_name, _v).size  = 0; \
			free(vector_struct_name->PPCAT(vector_item_name, _v).data); \
			vector_struct_name->PPCAT(vector_item_name, _v).data = NULL; \
		} \
	}; \
	void PPCAT(vector_struct_name,PPCAT(_dealloc_, vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name) { \
		if(vector_struct_name->PPCAT(vector_item_name, _v).data) {	\
			vector_struct_name->PPCAT(vector_item_name, _v).count = 0; \
			vector_struct_name->PPCAT(vector_item_name, _v).size  = 0; \
			free(vector_struct_name->PPCAT(vector_item_name, _v).data); \
			vector_struct_name->PPCAT(vector_item_name, _v).data = NULL; \
		} \
	} \

	/*
	 *
	 *
	 */

/*
 provide a default _free method for vectorable structs
 
 ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(atsc3_sls_html_entry_package);

 note, to override default free, implement a method similar to the following:

	void vector_item_name_free(vector_item_name_t** vector_item_name_p) {
		if(vector_item_name_p) {
			vector_item_name_t* vector_item_name = *vector_item_name_p;
			if(vector_item_name) {
				//other interior members here
				freesafe(vector_item_name);
				vector_item_name = NULL;
			}
			*vector_item_name_p = NULL;
		}
	}
 
 e.g.
 
 s/vector_item_name/atsc3_sls_html_entry_package
 
void atsc3_sls_html_entry_package_free(atsc3_sls_html_entry_package_t** atsc3_sls_html_entry_package_p) {
    if(atsc3_sls_html_entry_package_p) {
        atsc3_sls_html_entry_package_t* atsc3_sls_html_entry_package = *atsc3_sls_html_entry_package_p;
        if(atsc3_sls_html_entry_package) {
            //other interior members here
            freesafe(atsc3_sls_html_entry_package);
            atsc3_sls_html_entry_package = NULL;
        }
 	    *atsc3_sls_html_entry_package_p = NULL;
    }
}

*/

#define ATSC3_VECTOR_BUILDER_METHODS_INTERFACE_ITEM_FREE(vector_item_name) \
	void PPCAT(vector_item_name,_free)(PPCAT(vector_item_name,_t)** PPCAT(vector_item_name,_p));


#define ATSC3_VECTOR_BUILDER_METHODS_ITEM_FREE(vector_item_name) \
	void PPCAT(vector_item_name,_free)(PPCAT(vector_item_name,_t)** PPCAT(vector_item_name,_p)) { \
		if(PPCAT(vector_item_name,_p)) {	\
			PPCAT(vector_item_name,_t)* vector_item_name = *PPCAT(vector_item_name,_p);	\
			if(vector_item_name) { \
				freesafe(vector_item_name);	\
				vector_item_name = NULL;	\
			}	\
			*PPCAT(vector_item_name,_p) = NULL;	\
		}	\
	};

/**
 * jjustman-2020-06-02 - standalone vector support for generic typedef structs
 *
 */



#define ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT(vector_struct_name) \
 typedef struct  {\
    PPCAT(vector_struct_name,_t)** data; \
    uint32_t count; \
   	uint32_t size; \
 } PPCAT(vector_struct_name, _v);


#define ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT_METHODS_INTERFACE(vector_struct_name) \
	PPCAT(vector_struct_name,_t)* 	PPCAT(vector_struct_name,_new)(); \
	void 							PPCAT(vector_struct_name,_prealloc)(PPCAT(vector_struct_name,_v)*, uint32_t size); \
	void 							PPCAT(vector_struct_name,_add)     (PPCAT(vector_struct_name,_v)*, PPCAT(vector_struct_name,_t)*); \
	PPCAT(vector_struct_name,_t)* 	PPCAT(vector_struct_name,_pop)     (PPCAT(vector_struct_name,_v)*); \
	void 							PPCAT(vector_struct_name,_clear)   (PPCAT(vector_struct_name,_v)*); \
	void 							PPCAT(vector_struct_name,_free_v)  (PPCAT(vector_struct_name,_v)*); \
	void 							PPCAT(vector_struct_name,_typedef_free)  (PPCAT(vector_struct_name,_t)** PPCAT(vector_struct_name,_p));


#define ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT_METHODS_IMPLEMENTATION(vector_struct_name) \
	PPCAT(vector_struct_name,_t)* PPCAT(vector_struct_name,_new)() { \
		PPCAT(vector_struct_name,_t)* vector_struct = calloc(1, sizeof(PPCAT(vector_struct_name,_t))); \
		return vector_struct; \
	} \
	void PPCAT(vector_struct_name,_prealloc)(PPCAT(vector_struct_name,_v)* vector_struct, uint32_t size) { \
		if(!vector_struct->size || !vector_struct->data) { \
			/* new alloc */ \
			vector_struct->data = calloc(size, sizeof(PPCAT(vector_struct_name,_t)*)); \
			vector_struct->count = 0;	\
			vector_struct->size	= size;	\
		} else { \
			/* realloc */ \
			vector_struct->size = 	__MAX(size, vector_struct->count); \
			uint32_t to_resize = sizeof(PPCAT(vector_struct_name,_t)*) * vector_struct->size;	\
			vector_struct->data = realloc(vector_struct->data, to_resize);	\
		}	\
	} \
	\
	void PPCAT(vector_struct_name,_add)(PPCAT(vector_struct_name,_v)* vector_struct, PPCAT(vector_struct_name,_t)* vector_item) { \
		if(!vector_struct->size || !vector_struct->data) { \
			/* new alloc */ \
			vector_struct->data = calloc(ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE, sizeof(PPCAT(vector_struct_name,_t)*)); \
			(vector_struct->data[0]) = vector_item;	\
			vector_struct->count = 1;	\
			vector_struct->size	= ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE;	\
		} else if(vector_struct->count < vector_struct->size) {	\
			/* push to back if we have available space (count < size) */ \
			vector_struct->data[vector_struct->count++] = vector_item;	\
		} else { \
			/* realloc */ \
			vector_struct->size = __MAX(vector_struct->size * 2, \
										__MAX(vector_struct->count, ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE)); \
			uint32_t to_resize = sizeof(PPCAT(vector_struct_name,_t)*) * vector_struct->size;	\
			vector_struct->data = realloc(vector_struct->data, to_resize);	\
			vector_struct->data[vector_struct->count++] = vector_item;	\
			\
		}	\
	} \
	PPCAT(vector_struct_name,_t)* PPCAT(vector_struct_name,_pop)(PPCAT(vector_struct_name,_v)* vector_struct) { \
		if(!vector_struct->count) { return NULL; } \
		PPCAT(vector_struct_name,_t)* vector_item = vector_struct->data[0]; \
		for(int i=0; i < vector_struct->count - 1; i++) { \
			if(vector_struct->data[i+1]) { \
				vector_struct->data[i] = vector_struct->data[i+1]; \
			} \
		} \
		vector_struct->data[vector_struct->count-1] = NULL; \
		vector_struct->count--; \
		return vector_item; \
    } \
	\
	/* de-alloc vector AND _free item instances */ \
	void PPCAT(vector_struct_name,_clear)(PPCAT(vector_struct_name,_v)* vector_struct) { \
		if(vector_struct->data) {	\
			for(int i=0; i < vector_struct->count; i++) { \
				if(vector_struct->data[i]) { \
					/* freesafe(vector_struct->data[i]); */ \
					PPCAT(vector_struct_name,_typedef_free)(&vector_struct->data[i]); \
					vector_struct->data[i] = NULL; \
				} \
			} \
			vector_struct->count = 0; \
			\
		} \
	} \
	void PPCAT(vector_struct_name,_free_v)(PPCAT(vector_struct_name,_v)* vector_struct) { \
		if(vector_struct->data) {	\
			for(int i=0; i < vector_struct->count; i++) { \
				if(vector_struct->data[i]) { \
					PPCAT(vector_struct_name,_typedef_free)(&vector_struct->data[i]); \
				} \
			} \
			vector_struct->count = 0; \
			vector_struct->size  = 0; \
			free(vector_struct->data); \
			vector_struct->data = NULL; \
		} \
	};


//jjustman-2020-07-14 - re-name typedef usage of _t to _typedef_free
//
//#define ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT_METHODS_ITEM_FREE(vector_struct_name) \
//	void PPCAT(vector_struct_name,_free_t)(PPCAT(vector_struct_name,_t)* PPCAT(vector_struct_name,_p)) { \
//		if(PPCAT(vector_struct_name,_p)) {	\
//			PPCAT(vector_struct_name,_t)* vector_item = *PPCAT(vector_struct_name,_p);	\
//			if(vector_item) { \
//				freesafe(vector_item);	\
//				vector_item = NULL;	\
//			}	\
//			*PPCAT(vector_struct_name,_p) = NULL;	\
//		}	\
//	};


#define ATSC3_VECTOR_BUILDER_TYPEDEF_STRUCT_METHODS_ITEM_FREE(vector_struct_name) \
	void PPCAT(vector_struct_name,_typedef_free)(PPCAT(vector_struct_name,_t)** PPCAT(vector_struct_name,_p)) { \
		if(PPCAT(vector_struct_name,_p)) {	\
			PPCAT(vector_struct_name,_t)* vector_item = *PPCAT(vector_struct_name,_p);	\
			if(vector_item) { \
				freesafe(vector_item);	\
				vector_item = NULL;	\
			}	\
			*PPCAT(vector_struct_name,_p) = NULL;	\
		}	\
	};

#endif /* ATSC3_VECTOR_BUILDER_H_ */
