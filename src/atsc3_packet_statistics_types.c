/*
 * atsc3_mmt_packet_statistics.c
 *
 *  Created on: Feb 6, 2019
 *      Author: jjustman
 */

#include "atsc3_packet_statistics.h"

static atsc3_global_statistics_t global_stats_internal;
atsc3_global_statistics_t* atsc3_global_statistics = &global_stats_internal;
