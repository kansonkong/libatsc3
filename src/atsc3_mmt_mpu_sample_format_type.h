/*
 * atsc3_mmt_mpu_sample_format_type.h
 *
 *  ISO23008-1:2017, Section 8.3 - Sample Format
 *
 *  Created on: Aug 10, 2019
 *      Author: jjustman
 */

#ifndef ATSC3_MMT_MPU_SAMPLE_FORMAT_TYPE_H_
#define ATSC3_MMT_MPU_SAMPLE_FORMAT_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct atsc3_mmt_multiLayerInfoBox {
    uint8_t multilayer_flag:1;
    uint8_t reserved0:7;
/* if (multilayer_flag == 1) { */
        uint8_t     dependency_id:3;
        uint8_t     depth_flag:1;
        uint8_t     reserved1:4;
        uint8_t     temporal_id:3;
        uint8_t     reserved2:1;
        uint8_t     quality_id:4;
        uint8_t     priority_id:6;
        uint16_t    view_id:10;
/* } else { */
        uint8_t     layer_id:6;
        //          duplicated above
        //uint8_t   temporal_id:3;
        uint8_t     reserved3:7;
/* } */

} atsc3_mmt_multiLayerInfoBox_t;

typedef struct mmthsample_header {
    uint32_t    sequence_number;
/* if timed { */
        int8_t                          trackrefindex;
        uint32_t                        movie_fragment_sequence_number;
        uint32_t                        samplenumber;
        uint8_t                         priority;
        uint8_t                         dependency_counter;
        uint32_t                        offset;
        uint32_t                        length;
        atsc3_mmt_multiLayerInfoBox_t   atsc3_mmt_multiLayerInfoBox;
/* } else { */
        uint16_t                        item_id;
/* } */

	//derived internal libatsc3 value
	uint32_t    mfu_mmth_sample_header_size;
} mmthsample_header_t;


#ifdef __cplusplus
}
#endif
#endif /* ATSC3_MMT_MPU_SAMPLE_FORMAT_TYPE_H_ */
