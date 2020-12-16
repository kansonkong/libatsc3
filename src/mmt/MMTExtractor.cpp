#include "MMTExtractor.h"

#ifndef __JJ_PHY_MMT_PLAYER_BRIDGE_DISABLED

#include "atsc3_core_service_player_bridge.h"

//commandline stream filtering
uint16_t* dst_packet_id_filter = NULL;

//jjustman-2019-10-03 - context event callbacks...
lls_slt_monitor_t* lls_slt_monitor = NULL;

//mmtp/sls flow management
lls_sls_mmt_monitor_t* lls_sls_mmt_monitor = NULL;
atsc3_mmt_mfu_context_t* atsc3_mmt_mfu_context = NULL;

MMTExtractor::MMTExtractor() {
    atsc3_lls_slt_service = atsc3_lls_slt_service_new();
    atsc3_lls_slt_service->service_id = 1;

    lls_sls_mmt_monitor = lls_sls_mmt_monitor_create();
    lls_sls_mmt_monitor->atsc3_lls_slt_service = atsc3_lls_slt_service; //HACK!

    lls_slt_monitor = lls_slt_monitor_create();
    //wire up a lls event for SLS table
    lls_slt_monitor->atsc3_lls_on_sls_table_present_callback = &atsc3_lls_on_sls_table_present_ndk;
    lls_slt_monitor->atsc3_lls_on_aeat_table_present_callback = &atsc3_lls_on_aeat_table_present_ndk;

    lls_slt_service_id_t* lls_slt_service_id = lls_slt_service_id_new_from_atsc3_lls_slt_service(atsc3_lls_slt_service);
    lls_slt_monitor_add_lls_slt_service_id(lls_slt_monitor, lls_slt_service_id);

    //MMT/MFU callback contexts
    atsc3_mmt_mfu_context = atsc3_mmt_mfu_context_callbacks_default_jni_new();
}

void MMTExtractor::atsc3_core_service_bridge_process_mmt_packet(block_t* packet) {
    udp_packet_t* udp_packet = udp_packet_process_from_ptr(block_Get(packet), packet->p_size);
    if(!udp_packet) {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_ERROR("atsc3_core_service_bridge_process_mmt_packet: after udp_packet_process_from_ptr: unable to extract packet size: %d, i_pos: %d, 0x%02x 0x%02x",
                                                 packet->p_size,
                                                 packet->i_pos,
                                                 packet->p_buffer[0],
                                                 packet->p_buffer[1]);

        return;
    }

    lls_sls_mmt_session_t* lls_sls_mmt_session = lls_sls_mmt_monitor->lls_mmt_session;
    if (!lls_sls_mmt_session) {
        lls_sls_mmt_session = lls_slt_mmt_session_find_from_service_id(lls_slt_monitor, atsc3_lls_slt_service->service_id);

        if (!lls_sls_mmt_session) {
            lls_sls_mmt_session = lls_slt_mmt_session_find_or_create(lls_slt_monitor, atsc3_lls_slt_service);

            atsc3_slt_broadcast_svc_signalling_t* atsc3_slt_broadcast_svc_signalling = atsc3_slt_broadcast_svc_signalling_new();

            lls_sls_mmt_session->sls_destination_ip_address = udp_packet->udp_flow.dst_ip_addr;
            lls_sls_mmt_session->sls_destination_udp_port = udp_packet->udp_flow.dst_port;
        }

        if(!lls_sls_mmt_session) {
            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_mmt_packet: lls_slt_mmt_session_find_from_service_id: lls_sls_mmt_session is NULL!");
        }

        lls_sls_mmt_monitor->lls_mmt_session = lls_sls_mmt_session;
        lls_slt_monitor->lls_sls_mmt_monitor = lls_sls_mmt_monitor;
        lls_slt_monitor_add_lls_sls_mmt_monitor(lls_slt_monitor, lls_sls_mmt_monitor);
    }

    mmtp_asset_t* mmtp_asset = atsc3_mmt_mfu_context_mfu_depacketizer_context_update_find_or_create_mmtp_asset(atsc3_mmt_mfu_context, udp_packet, lls_slt_monitor, lls_sls_mmt_session);

    atsc3_core_service_bridge_process_mmt_udp_packet(udp_packet, mmtp_asset, lls_sls_mmt_session);

    udp_packet_free(&udp_packet);
}

int8_t MMTExtractor::atsc3_core_service_bridge_process_mmt_udp_packet(udp_packet_t* udp_packet, mmtp_asset_t* mmtp_asset, lls_sls_mmt_session_t* lls_sls_mmt_session) {
    //mmt types
    mmtp_packet_header_t*                   mmtp_packet_header = NULL;
    mmtp_packet_id_packets_container_t*     mmtp_packet_id_packets_container = NULL;

    mmtp_mpu_packet_t*                      mmtp_mpu_packet = NULL;
    mmtp_signalling_packet_t*               mmtp_signalling_packet = NULL;
    int8_t                                  mmtp_si_parsed_message_count = 0;

    mmtp_packet_header = mmtp_packet_header_parse_from_block_t(udp_packet->data);

    if(!mmtp_packet_header) {
        goto error;
    }
    mmtp_packet_header_dump(mmtp_packet_header);

    //for filtering MMT flows by a specific packet_id
    if(dst_packet_id_filter && *dst_packet_id_filter != mmtp_packet_header->mmtp_packet_id) {
        goto error;
    }

    mmtp_packet_id_packets_container = atsc3_mmt_mfu_context_mfu_depacketizer_update_find_or_create_mmtp_packet_id_packets_container(atsc3_mmt_mfu_context, mmtp_asset, mmtp_packet_header);

    if(mmtp_packet_header->mmtp_payload_type == 0x0) {
        mmtp_mpu_packet = mmtp_mpu_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
        if(!mmtp_mpu_packet) {
            goto error;
        }

        if(mmtp_mpu_packet->mpu_timed_flag == 1) {
            mmtp_mpu_dump_header(mmtp_mpu_packet);

            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_mfu_process_from_payload_with_context with udp_packet: %p, mmtp_mpu_packet: %p, atsc3_mmt_mfu_context: %p,", udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);

            mmtp_mfu_process_from_payload_with_context(udp_packet, mmtp_mpu_packet, atsc3_mmt_mfu_context);
            goto cleanup;

        } else {
            //non-timed -
            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_packet_header_parse_from_block_t - non-timed payload: packet_id: %u", mmtp_mpu_packet->mmtp_packet_id);
            goto error;
        }
    } else if(mmtp_packet_header->mmtp_payload_type == 0x2) {

        mmtp_signalling_packet = mmtp_signalling_packet_parse_and_free_packet_header_from_block_t(&mmtp_packet_header, udp_packet->data);
        if(!mmtp_signalling_packet) {
            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet_parse_and_free_packet_header_from_block_t - mmtp_signalling_packet was NULL for udp_packet: %p, udp_packet->p_size: %d", udp_packet, udp_packet->data->p_size);
            goto error;
        }

        mmtp_packet_id_packets_container = mmtp_asset_find_or_create_packets_container_from_mmtp_signalling_packet(mmtp_asset, mmtp_signalling_packet);

        if(mmtp_signalling_packet->si_fragmentation_indicator == 0x0) {
            //process this SI message in-line, no need for re-assembly
            mmtp_si_parsed_message_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, udp_packet->data);

            //but clear out any possible mmtp_signalling_packets being queued for re-assembly in mmtp_packet_id_packets_container
            mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);

        } else {
            //TODO: jjustman-2019-10-03 - if signalling_packet == MP_table, set atsc3_mmt_mfu_context->mp_table_last;

            //if mmtp_signalling_packet.sl_fragmentation_indicator != 00, then
            //  handle any fragmented signallling_information packets by packet_id,
            //  persisting for depacketization into packet_buffer[] when si_fragmentation_indicator:
            //       must start with f_i: 0x1 (01)
            //          any non 0x0 (00) or 0x1 (01) with no packet_buffer[].length should be discarded
            //       next packet contains: congruent packet_sequence_number (eg old + 1 (mod 2^32 for wraparound) == new)
            //          f_i: 0x2 (10) -> append
            //          f_i: 0x3 (11) -> check for completeness
            //                          -->should have packet_buffer[0].fragmentation_counter == packet_buffer[].length
            //   mmtp_signalling_packet_process_from_payload_with_context(udp_packet, mmtp_signalling_packet, atsc3_mmt_mfu_context);

            if(mmtp_signalling_packet->si_fragmentation_indicator != 0x1 && mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count == 0) {
                //we should never have a case where fragmentation_indicator is _not_ the first fragment of a signalling message and have 0 packets in the re-assembly vector,
                //it means we lost at least one previous DU for this si_messgae, so discard and goto error
                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet->si_fragmentation_indicator is: 0x%02x while mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count is 0, discarding!", mmtp_signalling_packet->si_fragmentation_indicator);

                goto error;
            }

            //push our first mmtp_signalling_packet for re-assembly (explicit mmtp_signalling_packet->si_fragmentation_indicator == 0x1 (01))
            if(mmtp_signalling_packet->si_fragmentation_indicator == 0x1 && mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count == 0) {
                __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_DEBUG("atsc3_core_service_bridge_process_packet_phy: mmtp_packet_id_packets_container_add_mmtp_signalling_packet - adding first entry, mmtp_signalling_packet: %p, mmtp_signalling_packet->si_fragmentation_indicator: 0x%02x, mmtp_signalling_packet->si_fragmentation_counter: %d (A: %d, H: %d)",
                                                         mmtp_signalling_packet,
                                                         mmtp_signalling_packet->si_fragmentation_indicator,
                                                         mmtp_signalling_packet->si_fragmentation_counter,
                                                         mmtp_signalling_packet->si_aggregation_flag,
                                                         mmtp_signalling_packet->si_additional_length_header);

                mmtp_packet_id_packets_container_add_mmtp_signalling_packet(mmtp_packet_id_packets_container, mmtp_signalling_packet);
                mmtp_signalling_packet = NULL;
                goto cleanup; //continue on

            } else {
                mmtp_signalling_packet_t *last_mmtp_signalling_packet = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.data[mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count - 1];

                //make sure our packet_sequence_number is sequential to our last mmtp_signalling_packet
                if((last_mmtp_signalling_packet->packet_sequence_number + 1) != mmtp_signalling_packet->packet_sequence_number) {
                    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: packet_sequence_number mismatch, discarding! last_mmtp_signalling_packet: %p, psn: %d (next: %d), frag: 0x%02x, frag_counter: %d, mmtp_signalling_packet: %p, psn: %d, frag: 0x%02x, frag_counter: %d",
                                                            last_mmtp_signalling_packet,
                                                            last_mmtp_signalling_packet->packet_sequence_number,
                                                            (uint32_t)(last_mmtp_signalling_packet->packet_sequence_number+1),
                                                            last_mmtp_signalling_packet->si_fragmentation_indicator,
                                                            last_mmtp_signalling_packet->si_fragmentation_counter,
                                                            mmtp_signalling_packet,
                                                            mmtp_signalling_packet->packet_sequence_number,
                                                            mmtp_signalling_packet->si_fragmentation_indicator,
                                                            mmtp_signalling_packet->si_fragmentation_counter);

                    mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);

                    goto error;
                }

                bool     mmtp_signalling_packet_vector_valid = true;
                bool     mmtp_signalling_packet_vector_complete = false;
                uint32_t mmtp_message_payload_final_size = 0;

                //check our vector sanity, and if we are "complete"
                mmtp_packet_id_packets_container_add_mmtp_signalling_packet(mmtp_packet_id_packets_container, mmtp_signalling_packet);
                int mmtp_signalling_packet_vector_count = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.count;

                mmtp_signalling_packet_t* mmtp_signalling_packet_temp = NULL;
                mmtp_signalling_packet_t* mmtp_signalling_packet_last_temp = NULL;

                for(int i=0; i < mmtp_signalling_packet_vector_count && mmtp_signalling_packet_vector_valid; i++) {
                    mmtp_signalling_packet_t* mmtp_signalling_packet_temp = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.data[i];

                    if(!mmtp_signalling_packet_temp || !mmtp_signalling_packet_temp->udp_packet_inner_msg_payload) {
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp is NULL, bailing!");
                        mmtp_signalling_packet_vector_valid = false;
                        break;
                    }

                    if(mmtp_signalling_packet_last_temp && mmtp_signalling_packet_last_temp->mmtp_packet_id != mmtp_signalling_packet_temp->mmtp_packet_id) {
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_last_temp->mmtp_packet_id != mmtp_signalling_packet_temp->mmtp_packet_id, %d != %d",
                                                                mmtp_signalling_packet_last_temp->mmtp_packet_id, mmtp_signalling_packet_temp->mmtp_packet_id);
                        mmtp_signalling_packet_vector_valid = false;
                        break;
                    }

                    if(i == 0 && mmtp_signalling_packet_temp->si_fragmentation_indicator != 0x1) { //sanity check (01)
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, i == 0 but mmtp_signalling_packet_temp->si_fragmentation_indicator is: 0x%02x", mmtp_signalling_packet_temp->si_fragmentation_indicator);
                        mmtp_signalling_packet_vector_valid = false;
                        break;
                    }

                    if(mmtp_signalling_packet_temp->si_fragmentation_counter != (mmtp_signalling_packet_vector_count - 1 - i)) {
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp->si_fragmentation_counter != (mmtp_signalling_packet_vector_count - 1 - i), %d != %d, bailing!",
                                                                mmtp_signalling_packet_temp->si_fragmentation_counter,
                                                                mmtp_signalling_packet_vector_count - 1 - i);

                        mmtp_signalling_packet_vector_valid = false;
                        break;
                    }

                    //anything less than the "last" packet should be fi==0x2 (10) (fragment that is neither the first nor the last fragment)
                    if(i < (mmtp_signalling_packet_vector_count - 2) && mmtp_signalling_packet_temp->si_fragmentation_indicator != 0x2) {
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_packet_phy: checking mmtp_signalling_packet vector sanity, mmtp_signalling_packet_temp->si_fragmentation_indicator: 0x%02x at index: %d, vector_count: %d",
                                                                mmtp_signalling_packet_temp->si_fragmentation_indicator, i, mmtp_signalling_packet_vector_count);

                        mmtp_signalling_packet_vector_valid = false;
                        break;
                    }

                    mmtp_message_payload_final_size += mmtp_signalling_packet_temp->udp_packet_inner_msg_payload->p_size;

                    //if we are the last index in the vector AND our fi==0x3 (11) (last fragment of a signalling message), then mark us as complete, otherwise
                    if(i == (mmtp_signalling_packet_vector_count - 1) && mmtp_signalling_packet_temp->si_fragmentation_indicator == 0x3) {
                        mmtp_signalling_packet_vector_complete = true;
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet vector is complete, packet_id: %d, vector size: %d",
                                                                 mmtp_signalling_packet_temp->mmtp_packet_id, mmtp_signalling_packet_vector_count);
                    } else {
                        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet vector not yet complete, i: %d, packet_id: %d, vector size: %d",
                                                                 i, mmtp_signalling_packet_temp->mmtp_packet_id, mmtp_signalling_packet_vector_count);
                    }

                    mmtp_signalling_packet_last_temp = mmtp_signalling_packet_temp;
                }

                if(!mmtp_signalling_packet_vector_valid) {
                    mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
                    mmtp_signalling_packet = NULL; //we will have already freed this packet by clearing the container
                    goto error;

                } else if(mmtp_signalling_packet_vector_complete) {
                    //re-assemble into MSG payload for parsing
                    block_t* msg_payload_final = block_Alloc(mmtp_message_payload_final_size);
                    mmtp_signalling_packet_t* mmtp_signalling_packet_temp = NULL;

                    for(int i=0; i < mmtp_signalling_packet_vector_count; i++) {
                        mmtp_signalling_packet_temp = mmtp_packet_id_packets_container->mmtp_signalling_packet_v.data[i];

                        block_AppendFromSrciPos(msg_payload_final, mmtp_signalling_packet_temp->udp_packet_inner_msg_payload);
                    }

                    //finally, we can now process our signalling_messagae
                    mmtp_signalling_packet = mmtp_packet_id_packets_container_pop_mmtp_signalling_packet(mmtp_packet_id_packets_container);
                    block_Destroy(&mmtp_signalling_packet->udp_packet_inner_msg_payload);
                    mmtp_signalling_packet->udp_packet_inner_msg_payload = msg_payload_final;
                    block_Rewind(mmtp_signalling_packet->udp_packet_inner_msg_payload);

                    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_DEBUG("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet msg_payload re-assembly is complete, using first mmtp_signalling_packet: %p, udp_packet_inner_msg_payload size: %d, value: %s",
                                                             mmtp_signalling_packet,
                                                             mmtp_signalling_packet->udp_packet_inner_msg_payload->p_size,
                                                             mmtp_signalling_packet->udp_packet_inner_msg_payload->p_buffer);

                    mmtp_si_parsed_message_count = mmt_signalling_message_parse_packet(mmtp_signalling_packet, mmtp_signalling_packet->udp_packet_inner_msg_payload);

                } else {
                    //noop: continue to accumulate
                    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("atsc3_core_service_bridge_process_packet_phy: mmtp_signalling_packet - adding to vector, size: %d", mmtp_signalling_packet_vector_count + 1);
                    mmtp_signalling_packet = NULL; //so we don't free pending accumulated packets
                }
            }
        }

        if(mmtp_si_parsed_message_count > 0) {
            mmt_signalling_message_dump(mmtp_signalling_packet);

            __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_TRACE("process_packet: calling mmt_signalling_message_dispatch_context_notification_callbacks with udp_packet: %p, mmtp_signalling_packet: %p, atsc3_mmt_mfu_context: %p,",
                                                     udp_packet,
                                                     mmtp_signalling_packet,
                                                     atsc3_mmt_mfu_context);


            //dispatch our wired callbacks
            mmt_signalling_message_dispatch_context_notification_callbacks(udp_packet, mmtp_signalling_packet, atsc3_mmt_mfu_context);

            //update our internal sls_mmt_session info
            mmt_signalling_message_update_lls_sls_mmt_session(mmtp_signalling_packet, lls_sls_mmt_session);

            //clear and flush out our mmtp_packet_id_packets_container if we came from re-assembly,
            // otherwise, final free of mmtp_signalling_packet packet in :cleanup
            mmtp_packet_id_packets_container_free_mmtp_signalling_packet(mmtp_packet_id_packets_container);
            goto cleanup;
        }
    } else {
        __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("process_packet: mmtp_packet_header_parse_from_block_t - unknown payload type of 0x%x", mmtp_packet_header->mmtp_payload_type);
        goto error;
    }

    error:
    __ATSC3_CORE_SERVICE_PLAYER_BRIDGE_WARN("atsc3_core_service_bridge_process_mmt_udp_packet: error, bailing processing!");

    cleanup:
    //jjustman-2020-11-12 - this should be freed already from mmtp_*_free_packet_header_from_block_t, but just in case...
    if(mmtp_packet_header) {
        mmtp_packet_header_free(&mmtp_packet_header);
    }

    //jjustman-2020-11-12 - note: do not free mmtp_mpu_packet or mmtp_signalling_packet as they may have been added to a mmtp_*_packet_collection for re-assembly
    //unless si_fragmentation_indicator == 0x0, then we can safely release, as we do not push single units to the mmtp_packet_id_packets_container->mmtp_signalling_packet_v
    if(mmtp_signalling_packet && mmtp_signalling_packet->si_fragmentation_indicator == 0x0) {
        mmtp_signalling_packet_free(&mmtp_signalling_packet);
    }

    return mmtp_si_parsed_message_count;
}

#endif
