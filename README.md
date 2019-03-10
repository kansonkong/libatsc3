libatsc3
==========
ATSC 3.0 NGBP Open Source Library - Parse LMT, LLS and other signaling, object delivery via ROUTE, video playback of MMT and DASH 


https://github.com/jjustman/libatsc3
 
Cross-platform build demo:
 
                https://www.dropbox.com/s/96qwgel1wp0yptd/timing%20offset%20doesnt%20re-offset%20to%20gps%2C%20nor%20can%20gps%20mode%20a%20replay%20stl.mov?dl=0
 
Project Overview
Libatsc3 is an open-source project aimed at expanding understanding of ATSC 3.0 from a software engineer’s perspective for data payloads, media processing, sample OTA transmission pcaps and media playback.  The tool is focused on the A/331 portion of the ATSC 3.0 specification dealing with Signaling, Delivery and Synchronization, which is the interface touchpoint above the physical layer on receiving devices such as connected tv’s, home gateways, and other next-gen IoT/shield devices.  The core of the library is written in “c” and should able to be cross-compiled, with only std*/libc header/linkage requirements.
 
Core libatsc3 library
The core of the library is responsible for processing A/331 IP Mulitcast traffic (raw UDP datagram) into corresponding LLS, MMT and ROUTE data payloads.  This should be able to be cross-compiled for any platforms (pending any endian-ness issues), and cpu processing on my mac runs less than 1% for mfu reassembly (HRBD is not implemented yet, but this will address memory growth). 
 
The output of the library for MMT is the elementary flows a selected service_id, containing the init/moov box (stable), MOOF (per MPU), and MDAT sample data per packet_id are available via double-buffered mutex writer/semaphore reader for handoff  directly to a video and audio decoder memory block. If MOOF’s are missing at the next mpu_sequence_number, the last MOOF box is re-used for track and sample time offsets, and data offsets in the trun box are recomputed.    The library also handles MMT signaling messages for MPT packet_id resolution for media essence, and will be adding in CRI soon.
 
The output of the library for ROUTE is the selected flow for a service_id, pre-allocated on disk based as TSI-TOI, using the EXT_FTI or  EXT_TOL (transfer object length) with a null block, and rebuilt as packets are received.  I am in the process of completing the e-fdt and mbms messages, but the tools below allow for reconstituted playback of ROUTE video and audio based upon the TSI/and init TOI objects.
 
Listener Tests and Tools
Tests for specific SLT, MMT, or ROUTE flows by ip, port, packet id or tsi are available for detailed debugging. These are built as individual cli listeners using pcap for all mulicast traffic collection for selective debugging/tracing of the component flows. 
 
The primary tool for analysis/playback is a ncurses UI for real-time analysis and playback with ffplay displaying a re-constituted and re-muxed video/audio stream in-memory pipe for both ROUTE and MMT streams.  
 
ROUTE objects are persisted on disk, but does not yet honor efdt and distribution windows.  Final elementary streams are remixed with bento4, and since this library is GPL, it is not required nor linked in the core library.  The muxed ES’s are then piped to ffplay for observation
 
Building libatsc3
 
Pre-requisites:
                The listener tests and tools require the following:
Libraries: ncurses, libpcap, cmake
If you do not have these libs, or are unsure, they can be installed via
                                                               i.      sudo apt-get install libncurses5-dev libncursesw5-dev libpcap-dev cmake
 
Binary: ffplay cli binary on your path
If you do not have ffplay, most linux distros now have this in their repo’s via:
                                                               i.      sudo apt-get install ffmpeg
                                                             ii.      otherwise, static builds with ffplay are available for most platforms here:
linux: https://johnvansickle.com/ffmpeg/
https://ffmpeg.zeranoe.com/builds/ 

partial-firewall of GPL bento4 (https://www.bento4.com/) for tool video playback:
Download and build:
                       i.   git clone https://github.com/axiomatic-systems/Bento4.git
                      ii.   mkdir cmakebuild
                    iii.   cd cmakebuild
                      iv.        cmake -DCMAKE_BUILD_TYPE=Debug ..
                       v.        make
Install library in:
                                                               i.      libatsc3/bento/lib
This is only linked
 
Building:
 
cd src; make all
you may need to add in a typedef for uin8_t if its not in your stdint.h
pull a few pcap’s down from https://github.com/jjustman/atsc-3.0-mmt-pcaps,
i’d avoid the first 12-17/19 payloads, there is a lot of MFU loss I’m still working on de-encapsulation robustness.
in a separate shell, use tcpreplay to play them to a local or remote mulicast i/f
cd tools
./ run_atsc3_listener_metrics_ncurses multicast_if_here
Type ‘m’ or ‘r’ for mmt or route, then ‘s’ for service id, optionally ‘v’,’a’ to override video packet_id/tsi, audio packet_id/tsi
 
I will write up more notes in the next few days, this should get you a good baseline for MMT decoding (slt/sls and data_type=0x2 for MMT MPT messages and will be adding in CRI messages soon), and ROUTE recovery, MPU re-assembly and remux into a single ISOBMFF segment (v+a) and pipe it to ffplay.


Updates:

## 2019-02-05 - src/

Added atsc3_listener_metrics_test to include LLS and MMT counters, including missing MFU's and selective filtering via cli wiht host and port options.
* To build, run make
* To run ./atsc3_listener_metrics_test
 ./atsc3_listener_metrics_test - a udp mulitcast listener test harness for atsc3 mmt messages
 ---
 args: dev (dst_ip) (dst_port)
  dev: device to listen for udp multicast, default listen to 0.0.0.0:0
  (dst_ip): optional, filter to specific ip address
  (dst_port): optional, filter to specific port

## 2019-01-27 - src/

Refactored out POC VLC plugin into standalone mmt sample listener.

* To build, run make
* To run, ./atsc3_mmt_listener_test vnic1
** where vnic1 is the mulitcast interface of your choice

* the listener test driver will write out mpu fragments in the mpu/ directory from all flows.  dst ip and port flows can be restricted by invoking the driver with these on the command line.  the listener will write out the packet fragments as received on the wire, and proper re-sequencing and re-assembly can be obtained by inspecting the mmtp_sub_flow_vector.  
**KNOWN ISSUES:** this code is very leaky, as the driver does not free the vector yet.

## 2019-01-21 - support_scripts/  

For ATSC 3.0 receiption with the Airwavz Redzone receiver, including RF scanning, reflection, capturing and replaying of IP multicast streams with tcpdump, tcprewrite and bittwist.  


jjustman@ngbp.org
###
