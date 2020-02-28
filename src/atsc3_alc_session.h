/*
 * atsc3_alc_session.h
 *
 *  Created on: Feb 28, 2019
 *      Author: jjustman
 */


#ifndef _ATSC3_ALC_SESSION_H_
#define _ATSC3_ALC_SESSION_H_

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct atsc3_alc_arguments {
    unsigned long long tsi;				/**< transport session identifier */
    unsigned long long start_time;			/**< start time of the session */
    unsigned long long stop_time;			/**< stop time of the session */

    const char *port;                     /**< base channel port number  */
    const char *addr;                     /**< base channel multicastast address */

    unsigned char nb_channel;             /**< number of channels in the session */
    unsigned char cc_id;                  /**< used congestion control, 0 = NULL, 1 = RLC */
    unsigned short es_len;                /**< encoding symbol length */
    unsigned int max_sb_len;              /**< maximum Source block length */
    unsigned char fec_enc_id;             /**< FEC encoding id */
    unsigned short fec_inst_id;           /**< FEC instance id */
} atsc3_alc_arguments_t;

typedef struct atsc3_alc_session {
  int s_id;								/**< ALC session identifier */
  int mode;								/**< mode for the ALC session (sender or receiver) */
  
  unsigned long long tsi;				/**< transport session identifier */
  unsigned long long starttime;			/**< start time of the session */
  unsigned long long stoptime;			/**< stop time of the session */
  double ftimestarttime;
  
  //struct alc_channel *ch_list[MAX_CHANNELS_IN_SESSION];	/**< channels in the session */
  int nb_channel;						/**< number of channels in the session */
  int max_channel;				       /**< number of maximum channels in the session */
  
  int fdt_instance_id;				/**< current FDT instance */

  int def_tx_rate;					/**< transmission rate in kbit/s */
  unsigned short def_eslen;			/**< encoding symbol length */
  unsigned int def_mxnbes;			/**< maximum number of encoding symbols
									that can be generated from one source block */
  unsigned int def_max_sblen;		/**< maximum source block length */

  unsigned char def_fec_enc_id;		/**< FEC encoding id */
  unsigned short def_fec_inst_id;	/**< FEC instance id  */

  int cc_id;						/**< used congestion control, 0 = NULL, 1 = RLC */
  int use_fec_oti_ext_hdr;			/**< use FEC OTI extension header */
  
  unsigned long long sent_bytes;		/**< bytes sent in the session */
  unsigned long long obj_sent_bytes;	/**< bytes sent for object */
  unsigned long long obj_start_time;	/**< start time for the transport object which sender is currently sending */
  unsigned long long tx_toi;			/**< transport object which sender is currently sending */
  
  double last_print_tx_percent;		/**< last printed transmission percent */
  
  int a_flag;						/**< send A flag in the session */


} atsc3_alc_session_t;


atsc3_alc_session_t* atsc3_open_alc_session(atsc3_alc_arguments_t *a);


#ifdef __cplusplus
}; //extern "C"
#endif

#endif /* _ATSC3_ALC_SESSION_H_ */

