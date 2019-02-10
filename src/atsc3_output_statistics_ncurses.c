/*
 * atsc3_output_statistics_ncurses.c
 *
 *  Created on: Feb 7, 2019
 *      Author: jjustman
 */

#include "atsc3_output_statistics_ncurses.h"

void ncurses_mutext_init() {
	if (pthread_mutex_init(&ncurses_writer_lock, NULL) != 0) {
		printf("ncurses_mutex_init failed");
		abort();
	}
}
void ncurses_writer_lock_mutex_acquire() {
    pthread_mutex_lock(&ncurses_writer_lock);
}
void ncurses_writer_lock_mutex_release() {
    pthread_mutex_unlock(&ncurses_writer_lock);
}
void ncurses_writer_lock_mutex_destroy() {
    pthread_mutex_destroy(&ncurses_writer_lock);
}

void lls_dump_instance_table_ncurses(lls_table_t* base_table) {

	__LLS_DUMP("LLS Base Table:");
	__LLS_DUMP("");
	__LLS_DUMP("lls_table_id       : %-3d (0x%-3x)  group_id      : %-3d (0x%-3x)", base_table->lls_table_id,	base_table->lls_table_id, base_table->lls_group_id, base_table->lls_group_id);
	__LLS_DUMP("group_count_minus1 : %-3d (0x%-3x)  table_version : %-3d (0x%-3x)", base_table->group_count_minus1, base_table->group_count_minus1, base_table->lls_table_version, base_table->lls_table_version);
	__LLS_DUMP("");

	if(base_table->lls_table_id == SLT) {

		__LLS_DUMP("SLT: Service contains %d entries:", base_table->slt_table.service_entry_n);

		for(int i=0l; i < base_table->slt_table.service_entry_n; i++) {
			lls_service_t* service = base_table->slt_table.service_entry[i];
			__LLS_DUMP("  service_id                  : %d", service->service_id);
			__LLS_DUMP("  global_service_id           : %s", service->global_service_id);
			__LLS_DUMP("  major_channel_no            : %-5d     minor_channel_no        : %d", service->major_channel_no, service->minor_channel_no);
			__LLS_DUMP("  service_category            : %-5d     slt_svc_seq_num         : %d", service->service_category, service->slt_svc_seq_num);
			__LLS_DUMP("  short_service_name          : %s", service->short_service_name);
			__LLS_DUMP("  broadcast_svc_signaling");
			__LLS_DUMP("    sls_protocol              : %d", service->broadcast_svc_signaling.sls_protocol);
			__LLS_DUMP("    sls_destination_ip_address: %s:%s", service->broadcast_svc_signaling.sls_destination_ip_address, service->broadcast_svc_signaling.sls_destination_udp_port);
			__LLS_DUMP("    sls_source_ip_address     : %s", service->broadcast_svc_signaling.sls_source_ip_address);
			__LLS_DUMP("");
		}
	}

	//decorate with instance types: hd = int16_t, hu = uint_16t, hhu = uint8_t
	if(base_table->lls_table_id == SystemTime) {
	__LLS_DUMP(" SystemTime:");
	__LLS_DUMP("  current_utc_offset       : %hd", base_table->system_time_table.current_utc_offset);
	__LLS_DUMP("  ptp_prepend              : %hu", base_table->system_time_table.ptp_prepend);
	__LLS_DUMP("  leap59                   : %d",  base_table->system_time_table.leap59);
	__LLS_DUMP("  leap61                   : %d",  base_table->system_time_table.leap61);
	__LLS_DUMP("  utc_local_offset         : %s",  base_table->system_time_table.utc_local_offset);

	__LLS_DUMP("  ds_status                : %d",  base_table->system_time_table.ds_status);
	__LLS_DUMP("  ds_day_of_month          : %hhu", base_table->system_time_table.ds_day_of_month);
	__LLS_DUMP("  ds_hour                  : %hhu", base_table->system_time_table.ds_hour);

	}

}


