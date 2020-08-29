/*
 * atsc3_srt_live_receiver.h
 *
 *  Created on: Aug 17, 2020
 *      Author: jjustman
 */

#include <atsc3_utils.h>

#ifndef PHY_VIRTUAL_SRT_ATSC3SRT_LIVE_RECEIVER_H_
#define PHY_VIRTUAL_SRT_ATSC3SRT_LIVE_RECEIVER_H_


#if defined (__cplusplus)
extern "C" {
#endif


typedef void(*atsc3_srt_live_rx_udp_packet_process_callback_f)(block_t* block);
typedef void(*atsc3_srt_live_rx_udp_packet_process_callback_with_context_f)(block_t* block, void* context);

typedef struct atsc3_srt_live_receiver_context {
	char*	source_connection_string;

	volatile bool should_run = false;
	volatile bool is_shutdown = false;

	atsc3_srt_live_rx_udp_packet_process_callback_f 				atsc3_srt_live_rx_udp_packet_process_callback;

	atsc3_srt_live_rx_udp_packet_process_callback_with_context_f 	atsc3_srt_live_rx_udp_packet_process_callback_with_context;
	void*															atsc3_srt_live_rx_udp_packet_process_callback_context;
} atsc3_srt_live_receiver_context_t;

//jjustman-2020-08-17 - sample connection string:  "srt://bna.srt.atsc3.com:31347?passphrase=88731837-0EB5-4951-83AA-F515B3BEBC20";
atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context_new();
atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context_new_with_source_connection_string(const char* source_connection_string);
void atsc3_srt_live_receiver_context_set_srt_source_connection_string(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context, const char* source_connection_string);

void atsc3_srt_live_receiver_context_set_rx_udp_packet_process_callback_with_context(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context, atsc3_srt_live_rx_udp_packet_process_callback_with_context_f atsc3_srt_live_rx_udp_packet_process_callback_with_context, void* context);
void atsc3_srt_live_receiver_context_free(atsc3_srt_live_receiver_context_t** atsc3_srt_live_receiver_context_p);

int atsc3_srt_live_receiver_start_in_proc(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context);

void atsc3_srt_live_receiver_notify_shutdown(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context);
bool atsc3_srt_live_receiver_get_is_shutdown(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context);


#if defined (__cplusplus)
}
#endif

#endif /* PHY_VIRTUAL_SRT_ATSC3SRT_LIVE_RECEIVER_H_ */
