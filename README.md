libatsc3 Overview
==========
ATSC 3.0 NGBP Open Source Library - Parse LMT, LLS and other signaling, object delivery via ROUTE, video playback of MMT and DASH
## August, 2020 Update: Part 2: PHY Support for Virtual SRT and LowaSIS
Additional updates for virutal PHY support with live SRT+STLTP transport, and LowaSIS android sample app integration for OTA live ATSC 3.0 support.

## August, 2020 Update: Part 1: LCT and ROUTE/DASH fixes
Lots of fixes for LCT handling for object recovery (avoiding incomplete objects being dispatched), ROUTE/DASH manifest patching against S-TSID flows, and ATSC3 ROUTE package extraction for codePoint=3 (and 4, but no signing verification at this time)

Coming soon: PHY c++ interface and dso support for USB FX(3) device support w/ prebuilt vendor driver libraries

## Jan, 2020 Update: Android Sample App included for ROUTE/DASH and MMT MFU playback
Adding android sample app for ALP pcap playback. Supports ROUTE/DASH and MMT with MFU decoding (including simple rendering of IMSC1 STPP captions)

PHY vendor support (non-mit drivers) with the android sample app is available from OneMedia 3.0

## November, 2019 Update: Additional support for ROUTE, Android PCAP replay and NDK/JNI Bindings in Jan, 2020 Release
Adding MBMS content-location output and template support for TOI segment output, additional timing adjustments for in-process PCAP replay with Android, and NDK/JNI bindings for Android sample app for MMT MFU and ROUTE/DASH playback soon.

## October, 2019 Update: MFU OOO mode with context callbacks
Adding event-drive callbacks for ATSC3 A/331 SLT table emission, along with MFU-based callbacks for A/V/S MFU payloads to decoder buffer handoff.  In progress: Android NDK/JNI Sample App for MMT playback via pcaps (or 3.0 Dongle if you have one..)


## September, 2019 Update: New MMTP Flow Vector Management

New design for MMTP de-packetizer, with full MMTP flow vector structures rather than combined union vectors
Support for ROUTE/FLUTE v2

Playback Tools:

atsc3_listener_metrics_ncurses_httpd_isobmff - atsc3.0 ROUTE/DASH and MMT playback tool

atsc3_mmt_listener_to_http_hls_fmp4 - home gateway MMT to HLS fmp4 [refragmenting for OTT playback](https://www.linkedin.com/posts/jason-justman-7662595_libatsc3-putting-atsc-30-theory-into-practice-activity-6577527461616197632-jzUT)



## August, 2019 Update: "Virtual" ATSC 3.0 Airchain with ALP IP Multicast "reflection"

Designed for real-time inspection, analysis and confidence monitoring
of STLTP flows, with full ALP IP de-encapsulation and playback support.

[In this demo](https://www.dropbox.com/s/bfy23kxscmgenv6/20190801-ATSC3.0-STLTP%20Reflector%20to%20IP%20Multicast%20and%20MMT%20Playback.mov?dl=0), a Virtual NIC is replaying a STLTP .PCAP capture which is 
received by the A/324 Reflector tool in real-time.  The reflector tool will unwrap 
the STLTP single multicast IP:Port emission, and produce re-fragmented 
ALP IP payloads emitted to a second Virtual NIC, including decoding of 
Baseband, Preamble and T&M packets.

[MMT and ROUTE playback demo](https://www.dropbox.com/s/2fap10i27mrt25z/2019-08-09-stltp%20reflector%20-%20leak%20free%20-%20mmt%20and%20route%20playback-video.mov?dl=0)

For confidence monitoring of content, the reflector tool will fully parse 
the tunnel payload consisting of A/324 Outer/Inner Tunneled packets,
 A/321 Baseband Packets, A/330 ALP packets, and emitting the IP output
on an independent Virtual NIC for service playback.


2019-08-09
jjustman@ngbp.org

More details:

STLTP:  Added new STLTP De-encapsulator/Reflector listener test tool to enable local replay of ATSC 3.0 STLTP pcaps for emission of ALP IP packets.



To run:

1. Start a test STLTP replay
 -  sample artifacts here: https://github.com/jjustman/atsc-3.0-mmt-pcaps)
2. Run the stltp reflector from the src/listener_tests folder as follows:
 - atsc3_stltp_alp_listener_reflector_test vnic1 239.0.1.3 30000 vnic0 > debug.log
   - vnic1: multicast input interface
   - 239.0.1.3 mulicast destination
   - 30000 mulicast port
   - vnic0: unicast output interface for ATSC 3.0 ALP IP packet reflection
3. Run src/tools/atsc3_listener_test/atsc3_listener_metrics_ncurses_httpd_isobmff vnic0 for MMT and ROUTE-DASH replay
   - vnic0: unicast interface for ATSC 3.0 ALP IP packet consumption

Note: The STLTP Outer, STLTP Inner, Baseband, and ALP IP de-encapsulation leaks will be fixed ASAP.


## Updates - 2019-05-09
MMT: Out-of-order support for parsing MMTHSample and trun box building for ISOBMFF rebuild.  More info soon...


## Updates - 2019-03-17
I've fixed a few bugs with Linux ncurses tools - and added initial STLTP and MMT MPT signaling messages.  Starting to see some AC-4 audio tracks in the wild, please note ffplay won't be able to decode this format...


https://github.com/jjustman/libatsc3
 
## Cross-platform build demo:

 https://www.dropbox.com/s/5fv162k05aqnc3v/2019-03-10%20libatsc3%20route%20dash%20osx%20linux.mov?dl=0
 
## Project Overview

Libatsc3 is an open-source project aimed at expanding understanding of ATSC 3.0 from a software engineer’s perspective for data payloads, media processing, sample OTA transmission pcaps and media playback.  The tool is focused on the A/331 portion of the ATSC 3.0 specification dealing with Signaling, Delivery and Synchronization, which is the interface touchpoint above the physical layer on receiving devices such as connected tv’s, home gateways, and other next-gen IoT/shield devices.  The core of the library is written in “c” and should able to be cross-compiled, with only std*/libc header/linkage requirements.
 
## Core libatsc3 library

The core of the library is responsible for processing A/331 IP Mulitcast traffic (raw UDP datagram) into corresponding LLS, MMT and ROUTE data payloads.  This should be able to be cross-compiled for any platforms (pending any endian-ness issues), and cpu processing on my mac runs less than 1% for mfu reassembly (HRBD is not implemented yet, but this will address memory growth). 
 
The output of the library for MMT is the elementary flows a selected service_id, containing the init/moov box (stable), MOOF (per MPU), and MDAT sample data per packet_id are available via double-buffered mutex writer/semaphore reader for handoff  directly to a video and audio decoder memory block. If MOOF’s are missing at the next mpu_sequence_number, the last MOOF box is re-used for track and sample time offsets, and data offsets in the trun box are recomputed.    The library also handles MMT signaling messages for MPT packet_id resolution for media essence, and will be adding in CRI soon.
 
The output of the library for ROUTE is the selected flow for a service_id, pre-allocated on disk based as TSI-TOI, using the EXT_FTI or  EXT_TOL (transfer object length) with a null block, and rebuilt as packets are received.  I am in the process of completing the e-fdt and mbms messages, but the tools below allow for reconstituted playback of ROUTE video and audio based upon the TSI/and init TOI objects.
 
## Listener Tests and Tools

Tests for specific SLT, MMT, or ROUTE flows by ip, port, packet id or tsi are available for detailed debugging. These are built as individual cli listeners using pcap for all mulicast traffic collection for selective debugging/tracing of the component flows. 
 
The primary tool for analysis/playback is a ncurses UI for real-time analysis and playback with ffplay displaying a re-constituted and re-muxed video/audio stream in-memory pipe for both ROUTE and MMT streams.  
 
ROUTE objects are persisted on disk, but does not yet honor efdt and distribution windows.  Final elementary streams are remixed with bento4, and since this library is GPL, it is not required nor linked in the core library.  The muxed ES’s are then piped to ffplay for observation
 
## Building libatsc3
 
Pre-requisites:
                The listener tests and tools require the following:
* Libraries: ncurses, libpcap, cmake
If you do not have these libs, or are unsure, they can be installed via
* sudo apt-get install libncurses5-dev libncursesw5-dev libpcap-dev cmake
 
* Binary: ffplay cli binary on your path
If you do not have ffplay, most linux distros now have this in their repo’s via:
* sudo apt-get install ffmpeg
*      otherwise, static builds with ffplay are available for most platforms here:

linux: https://johnvansickle.com/ffmpeg/
https://ffmpeg.zeranoe.com/builds/ 

partial-firewall of GPL bento4 (https://www.bento4.com/) for tool video playback:

Download and build:
*  git clone https://github.com/axiomatic-systems/Bento4.git
*   mkdir cmakebuild
*   cd cmakebuild
*        cmake -DCMAKE_BUILD_TYPE=Debug ..
*        make
Install library in:
*      libatsc3/bento/lib
This is only linked for the monitoring tool
 
# Building:
 
* cd src; make all
** you may need to add in a typedef for uin8_t if its not in your stdint.h
* pull a few pcap’s down from https://github.com/jjustman/atsc-3.0-mmt-pcaps,
** i’d avoid the first 12-17/19 payloads, there is a lot of MFU loss I’m still working on de-encapsulation robustness.
* in a separate shell, use tcpreplay to play them to a local or remote mulicast i/f
* cd tools
*./run_atsc3_listener_metrics_ncurses multicast_if_here

**Type ‘m’ or ‘r’ for mmt or route, then ‘s’ for service id, optionally ‘v’,’a’ to override video packet_id/tsi, audio packet_id/tsi
 
## Open Items
MMT decoding (slt/sls and data_type=0x2 for MMT MPT messages and will be adding in CRI messages soon), ROUTE MBMS processing


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
