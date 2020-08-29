/*
 * Atsc3SRT_Target.h
 *
 *  Created on: Aug 11, 2020
 *      Author: jjustman
 */

#include <stdio.h>

#ifndef SRT_ATSC3SRT_TARGET_H_
#define SRT_ATSC3SRT_TARGET_H_

#include <transmitbase.hpp>

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

	Atsc3SRT_Target() { }


	int Write(const char* data, size_t size, int64_t src_time, std::ostream &out_stats = std::cout) override
	{
    	printf("Atsc3SRT_Target:Write - data: %p, size: %lu, src_time: %llu\n",
    			data, size, src_time);

    	return (int) size;

	}
    void Write(const MediaPacket& data) //override
    {

    	printf("Atsc3SRT_Target:Write - mediapacket: %p, size: %lu\n", data.payload.data(), data.payload.size());
        //int stat = sendto(m_sock, data.payload.data(), data.payload.size(), 0, (sockaddr*)&sadr, sizeof sadr);

    }

    bool IsOpen() override { return true; }
    bool Broken() override { return false; }

};




#endif /* SRT_ATSC3SRT_TARGET_H_ */
