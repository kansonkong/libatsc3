#include <atsc3_utils.h>

#ifndef LIBATSC3_IATSC3NDKPHYCLIENT_H
#define LIBATSC3_IATSC3NDKPHYCLIENT_H

#include <vector>
using namespace std;

//will dispatch plp_num as 0 if the (virtual/demuxed) phy does not have support to disambiguate (e.g. no LMT reference)
typedef void(*atsc3_phy_rx_udp_packet_process_callback_f)(uint8_t plp_num, block_t* block);

class IAtsc3NdkPHYClient {

	public:
        //required methods for implementation
		virtual int  init()       = 0;
		virtual int  run()        = 0;
		virtual bool is_running() = 0;
		virtual int  stop()       = 0;
		virtual int  deinit()     = 0;

		//optional methods for subclasses to implement if needed
		virtual int  download_bootloader_firmware(int fd)  { return INT_MIN; }
        virtual int  open(int fd, int bus, int addr)       { return INT_MIN; }
        virtual int  tune(int freqKhz, int single_plp)     { return INT_MIN; }
        virtual int  listen_plps(vector<uint8_t> plps)     { return INT_MIN; }

        virtual void setRxUdpPacketProcessCallback(atsc3_phy_rx_udp_packet_process_callback_f atsc3_phy_rx_udp_packet_process_callback) {
			this->atsc3_phy_rx_udp_packet_process_callback = atsc3_phy_rx_udp_packet_process_callback;
		}
		virtual ~IAtsc3NdkPHYClient() {};

	protected:
        //overloadable callbacks for Android to pin mJavaVM as needed
        virtual void pinProducerThreadAsNeeded() { };
        virtual void releaseProducerThreadAsNeeded() { };

        virtual void pinConsumerThreadAsNeeded() { };
        virtual void releaseConsumerThreadAsNeeded() { };
		atsc3_phy_rx_udp_packet_process_callback_f atsc3_phy_rx_udp_packet_process_callback = nullptr;
};


#endif //LIBATSC3_IATSC3NDKPHYCLIENT_H
