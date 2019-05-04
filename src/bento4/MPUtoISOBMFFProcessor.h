/*
 * MPUtoISOBMFFProcessor.h
 *
 *  Created on: Feb 17, 2019
 *      Author: jjustman
 */
#include <stdint.h>
#include "../../bento4/include/Ap4.h"


#ifndef BENTO4_MPUTOISOBMFFPROCESSOR_H_
#define BENTO4_MPUTOISOBMFFPROCESSOR_H_


AP4_DataBuffer* mpuToISOBMFFProcessBoxes(uint8_t* full_mpu_payload, uint32_t full_mpu_payload_size, int mdat_size);
AP4_DataBuffer* mpuToDumpISOBMFFBoxes(uint8_t* full_mpu_payload, uint32_t full_mpu_payload_size, int mdat_size);





#endif /* BENTO4_MPUTOISOBMFFPROCESSOR_H_ */
