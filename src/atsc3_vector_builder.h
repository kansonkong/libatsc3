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



/**
 *
 *		ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(atsc3_fdt_instance, atsc3_fdt_file)
 *
 *	results in:
 *		atsc3_fdt_file_t* atsc3_fdt_file_new();
 *  	atsc3_fdt_instance_add_atsc3_fdt_file(atsc3_fdt_instance_t*, atsc3_fdt_file*)
 *
 */


#define ATSC3_VECTOR_BUILDER_METHODS_INTERFACE(vector_struct_name, vector_item_name) \
	PPCAT(vector_item_name,_t)* PPCAT(vector_item_name,_new)(); \
	PPCAT(vector_struct_name,_t)* PPCAT(vector_struct_name,_new)(); \
	void PPCAT(vector_struct_name,PPCAT(_clear_,vector_item_name))(PPCAT(vector_struct_name,_t)*); \
	void PPCAT(vector_struct_name,PPCAT(_add_,vector_item_name))(PPCAT(vector_struct_name,_t)*, PPCAT(vector_item_name,_t)*);

/**
 *
 *		ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(atsc3_fdt_instance, atsc3_fdt_file)
 *
 *	results in:
 *		atsc3_fdt_file_t* atsc3_fdt_file_new();
 *  	atsc3_fdt_instance_add_atsc3_fdt_file(atsc3_fdt_instance_t*, atsc3_fdt_file*)
 *
 *	todo: prealloc to N as needed
 */
#define ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE 10

#define ATSC3_VECTOR_BUILDER_METHODS_PARENT_IMPLEMENTATION(vector_struct_name) \
	PPCAT(vector_struct_name,_t)* PPCAT(vector_struct_name,_new)() { \
		PPCAT(vector_struct_name,_t)* vector_struct_name = calloc(1, sizeof(PPCAT(vector_struct_name,_t))); \
		return vector_struct_name; \
	} \
	\
\

#define ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION(vector_struct_name, vector_item_name) \
	PPCAT(vector_item_name,_t)* PPCAT(vector_item_name,_new)() { \
		PPCAT(vector_item_name,_t)* vector_item_name = calloc(1, sizeof(PPCAT(vector_item_name,_t))); \
		return vector_item_name; \
	} \
	\
	void PPCAT(vector_struct_name,PPCAT(_clear_, vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name) { \
		for(int i=0; i < vector_struct_name->PPCAT(vector_item_name, _v).count; i++) { \
			if(vector_struct_name->PPCAT(vector_item_name, _v).data[i]) { \
				free(vector_struct_name->PPCAT(vector_item_name, _v).data[i]); \
                vector_struct_name->PPCAT(vector_item_name, _v).data[i] = NULL; \
			} \
		} \
		vector_struct_name->PPCAT(vector_item_name, _v).count 	= 0; \
		\
	} \
	\
	void PPCAT(vector_struct_name,PPCAT(_add_, vector_item_name))(PPCAT(vector_struct_name,_t)* vector_struct_name, PPCAT(vector_item_name,_t)* vector_item_name) { \
		if(!vector_struct_name->PPCAT(vector_item_name, _v).size || !vector_struct_name->PPCAT(vector_item_name, _v).data) { \
			/* new alloc */ \
			vector_struct_name->PPCAT(vector_item_name, _v).data 	= calloc(ATSC3_VECTOR_BUILDER_METHODS_IMPLEMENTATION_DEFAULT_SIZE, sizeof(PPCAT(vector_item_name,_t)**)); \
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
			uint32_t to_resize = sizeof(PPCAT(vector_item_name,_t)**) * vector_struct_name->PPCAT(vector_item_name, _v).size;	\
			vector_struct_name->PPCAT(vector_item_name, _v).data = realloc(vector_struct_name->PPCAT(vector_item_name, _v).data, to_resize);	\
			vector_struct_name->PPCAT(vector_item_name, _v).data[vector_struct_name->PPCAT(vector_item_name, _v).count++] = vector_item_name;	\
			\
		}	\
		/* PPCAT(name,_t)* name = calloc(1, sizeof(PPCAT(name,_t))); */ \
		/* return name; */ \
	}


#endif /* ATSC3_VECTOR_BUILDER_H_ */
