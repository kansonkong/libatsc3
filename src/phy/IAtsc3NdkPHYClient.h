#include <atsc3_utils.h>
#include <atsc3_alp_types.h>

#ifndef LIBATSC3_IATSC3NDKPHYCLIENT_H
#define LIBATSC3_IATSC3NDKPHYCLIENT_H

#include <vector>
using namespace std;

//will dispatch plp_num as 0 if the (virtual/demuxed) phy does not have support to disambiguate (e.g. no LMT reference)
typedef void(*atsc3_phy_rx_udp_packet_process_callback_f)(uint8_t plp_num, block_t* block);
typedef atsc3_link_mapping_table_t* (*atsc3_phy_rx_link_mapping_table_process_callback_f)(atsc3_link_mapping_table_t* atsc3_link_mapping_table_pending);


class IAtsc3NdkPHYClient {

	public:
        //required methods for implementation
		virtual int  init()       = 0;
		virtual int  run()        = 0;
		virtual bool is_running() = 0;
		virtual int  stop()       = 0;
		virtual int  deinit()     = 0;

		//optional methods for subclasses to implement if needed
		virtual int  download_bootloader_firmware(int fd, string device_path)  { return INT_MIN; }
        virtual int  open(int fd, string device_path)                          { return INT_MIN; }
        virtual int  tune(int freqKhz, int single_plp)                         { return INT_MIN; }
        virtual int  listen_plps(vector<uint8_t> plps)                         { return INT_MIN; }

        virtual void setRxUdpPacketProcessCallback(atsc3_phy_rx_udp_packet_process_callback_f atsc3_phy_rx_udp_packet_process_callback) {
			this->atsc3_phy_rx_udp_packet_process_callback = atsc3_phy_rx_udp_packet_process_callback;
		}

		virtual void setRxLinkMappingTableProcessCallback(atsc3_phy_rx_link_mapping_table_process_callback_f atsc3_phy_rx_link_mapping_table_process_callback) {
		    this->atsc3_phy_rx_link_mapping_table_process_callback = atsc3_phy_rx_link_mapping_table_process_callback;
        }

		virtual ~IAtsc3NdkPHYClient() {};

	protected:
        //overloadable callbacks for Android to pin mJavaVM as needed
        virtual void pinProducerThreadAsNeeded() { };
        virtual void releasePinnedProducerThreadAsNeeded() { };

        virtual void pinConsumerThreadAsNeeded() { };
        virtual void releasePinnedConsumerThreadAsNeeded() { };

        virtual void pinStatusThreadAsNeeded() { };
        virtual void releasePinnedStatusThreadAsNeeded() { };

		atsc3_phy_rx_udp_packet_process_callback_f              atsc3_phy_rx_udp_packet_process_callback = nullptr;
		atsc3_phy_rx_link_mapping_table_process_callback_f      atsc3_phy_rx_link_mapping_table_process_callback = nullptr;

};


#endif //LIBATSC3_IATSC3NDKPHYCLIENT_H
