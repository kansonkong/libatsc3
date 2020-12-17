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

/*
 * From ISO23008-1:2017 - 8.3.2
 *
    aligned(8) class multiLayerInfo extends Box("muli") {
       bit(1) multilayer_flag;
       bit(7) reserved0;
       if (multilayer_flag==1) {
           //32 bits
          bit(3) dependency_id;
          bit(1) depth_flag;
          bit(4) reserved1;
          bit(3) temporal_id;
          bit(1) reserved2;
          bit(4) quality_id;
          bit(6) priority_id;
         bit(10) view_id;

       } else{
           //16bits
          bit(6) layer_id;
          bit(3) temporal_id;
          bit(7) reserved3;
       }
     }

* also, remember that 'muli' is an ISO14496 "Box", e.g.

    aligned(8) class Box (unsigned int(32) boxtype, optional unsigned int(8)[16] extended_type) {
       unsigned int(32) size;
       unsigned int(32) type = boxtype;
       if (size==1) {
          unsigned int(64) largesize;
       } else if (size==0) {
          // box extends to end of file
       }
      if (boxtype==‘uuid’) {
        unsigned int(8)[16] usertype = extended_type;
      }
    }
 */
typedef struct atsc3_mmt_multiLayerInfoBox {
    uint32_t box_size;
    uint32_t box_type;
    uint64_t box_size_large;

    uint8_t multilayer_flag:1;
    uint8_t reserved0:7;

/* if (multilayer_flag == 1) { */
        uint8_t     dependency_id:3;
        uint8_t     depth_flag:1;
        uint8_t     reserved1:4;
        uint8_t     multilayer_temporal_id:3;
        uint8_t     reserved2:1;
        uint8_t     quality_id:4;
        uint8_t     priority_id:6;
        uint16_t    view_id:10;
/* } else { */
        uint8_t     layer_id:6;
        uint8_t     temporal_id:3;
        uint8_t     reserved3:7;
/* } */

} atsc3_mmt_multiLayerInfoBox_t;


/*
 *
 * MFU mpu_fragmentation_indicator==1's are prefixed by the following box, need to remove and process
 *
        aligned(8) class MMTHSample {
           unsigned int(32) sequence_number;
           if (is_timed) {

            //interior block is 152 bits, or 19 bytes
              signed int(8) trackrefindex;
              unsigned int(32) movie_fragment_sequence_number
              unsigned int(32) samplenumber;
              unsigned int(8)  priority;
              unsigned int(8)  dependency_counter;
              unsigned int(32) offset;
              unsigned int(32) length;
            //end interior block

              multiLayerInfo();
        } else {
                //additional 2 bytes to chomp for non timed delivery
              unsigned int(16) item_ID;
           }
        }
*/

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
