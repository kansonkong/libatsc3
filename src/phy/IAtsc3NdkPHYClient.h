#include <atsc3_utils.h>

#ifndef LIBATSC3_IATSC3NDKPHYCLIENT_H
#define LIBATSC3_IATSC3NDKPHYCLIENT_H


//will dispatch plp_num as 0 if the (virtual/demuxed) phy does not have support to disambiguate (e.g. no LMT reference)

typedef void(*atsc3_phy_rx_udp_packet_process_callback_f)(uint8_t plp_num, block_t* block);

class IAtsc3NdkPHYClient {

	public:
		virtual int Init() = 0;

		virtual void SetRxUdpPacketProcessCallback(atsc3_phy_rx_udp_packet_process_callback_f atsc3_phy_rx_udp_packet_process_callback) {
			this->atsc3_phy_rx_udp_packet_process_callback = atsc3_phy_rx_udp_packet_process_callback;
		}

		virtual int Open(int fd, int bus, int addr) = 0;
		virtual int Tune(int freqKhz, int plp) = 0;
		virtual int Stop()  = 0;
		virtual int Close() = 0;

		virtual ~IAtsc3NdkPHYClient() {};

		/* jjustman-2020-08-10
		 * additional methods to impl?
		 *
		 *   int TuneMultiplePLP(int freqKhz, vector<int> plpIds);
		int ListenPLP1(int plp1); //by default, we will always listen to PLP0, append additional PLP for listening
		 */

	protected:
		atsc3_phy_rx_udp_packet_process_callback_f atsc3_phy_rx_udp_packet_process_callback;
};


#endif //LIBATSC3_IATSC3NDKPHYCLIENT_H
