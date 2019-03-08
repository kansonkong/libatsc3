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
#define ATSC3_VECTOR_BUILDER(name) \
 struct {\
    PPCAT(name,_t)** PPCAT_data; \
    size_t cap; \
    size_t size; \
 } PPCAT(name, _v);


#endif /* ATSC3_VECTOR_BUILDER_H_ */
