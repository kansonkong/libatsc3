/*
 * Atsc3SRT_Target.h
 *
 *  Created on: Aug 11, 2020
 *      Author: jjustman
 */

#include <stdio.h>
#include <atsc3_utils.h>

#ifndef SRT_ATSC3SRT_TARGET_H_
#define SRT_ATSC3SRT_TARGET_H_

#include "atsc3_srt_live_receiver.h"
#include "transmitbase.hpp"

/* target looks like:
 *
 *
class Target: public Location
{
public:
    virtual int  Write(const char* data, size_t size, int64_t src_time, std::ostream &out_stats = std::cout) = 0;
    virtual bool IsOpen() = 0;
    virtual bool Broken() = 0;
    virtual void Close() {}
    virtual size_t Still() { return 0; }
    static std::unique_ptr<Target> Create(const std::string& url);
    virtual ~Target() {}

    virtual SRTSOCKET GetSRTSocket() const { return SRT_INVALID_SOCK; }
    virtual int GetSysSocket() const { return -1; }
    virtual bool AcceptNewClient() { return false; }
};
 *
 *
 */


//

class Atsc3SRT_Target : public virtual Target
{
public:

	Atsc3SRT_Target(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context) {
		this->atsc3_srt_live_receiver_context = atsc3_srt_live_receiver_context;
		allocate_block_t();
	}

	/*
	 * jjustman-2020-08-17 - reconstruct an "ip/udp header frame" -> 28 bytes to prepend (42 bytes if we needed the eth frame header)
	 *
	 */
	int Write(const char* data, size_t size, int64_t src_time, std::ostream &out_stats = std::cout) override
	{
//    	printf("Atsc3SRT_Target:Android:Write - data: %p, size: %lu, src_time: %llu",
//    			data, size, src_time);

    	if(atsc3_srt_live_receiver_context &&
    		(atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback || (atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_with_context && atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_context))) {

    		check_or_allocate_block_t();

    		block_Resize(block, size + 28);
    		//unsafe...
    		memset(block->p_buffer, 0, 28);
    		block->p_buffer[9] = 0x11;
    		block_Seek(block, 28);
    		block_Write(block, (uint8_t*)data, size);
    		block_Rewind(block);

			if(atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback) {
				atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback(block);
			}

			if(atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_with_context && atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_context) {
				atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_with_context(block, atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_context);
			}
       		//don't destory, keep for repeated dispatch to callback
    	}

    	return (int) size;
	}
    void Write(const MediaPacket& data) //override
    {

    	printf("Atsc3SRT_Target:Write - mediapacket: %p, size: %lu\n", data.payload.data(), data.payload.size());
        //int stat = sendto(m_sock, data.payload.data(), data.payload.size(), 0, (sockaddr*)&sadr, sizeof sadr);

    }

    bool IsOpen() override { return true; }
    bool Broken() override { return false; }

    inline void check_or_allocate_block_t() {
    	if(!block) {
    		allocate_block_t();
    	}
    }

    inline void allocate_block_t() {
    	if(block) {
    		block_Destroy(&block);
    	}
		block = block_Alloc(MAX_ATSC3_ETHERNET_PHY_FRAME_LENGTH);
    }

protected:
    block_t* block = NULL;
    atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context = NULL;
};




#endif /* SRT_ATSC3SRT_TARGET_H_ */
