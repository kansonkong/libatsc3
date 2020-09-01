# jjustman - updated - 2019-10-05
# 
# MIT "compatable" library is built as libatsc3.o (e.g. non-gpl distribution)
#
# GPL "compatable" gpl binaries are only built for the toosl output linkages, as it will contain linking steps againt Bento4 for ISOBMFF box management
#	e.g. bento4_gpl.o and atsc3_isobmff_tools_gpl.o (with -D__ENABLE_GPL_BENTO4__)
#
# 	requirements: bento4 libary, ncurses libary and headers
#

# jjustman-2020-03-13 - add support for older $(CC) versions (e.g. 4.8.4)
# jjustman-2020-03-26 - disabling as it causes libpacap typedef headers to fail (e.g.  typedef u_int bpf_u_int32;)
# CFLAGS=-std=c99
# $(info    CFLAGS is $(CFLAGS))
#
# jjustman-2020-03-26 - added #ifdef __MALLOC_DEBUGGING for doublefree crash debugging on linux in atsc3_alc_listener_mde_writer
# 	to enable, add -l

# jjustman-2020-07-06 - cleanup for OSX $(CC) messages w/ Apple clang version 11.0.3 (clang-1103.0.32.62)
# /usr/local/include/stdlib.h:134:25: warning: pointer is missing a nullability type specifier (_Nonnull, _Nullable, or _Null_unspecified) [-Wnullability-completeness]
# double   atof(const char *);

# jjustman-2020-07-27 - note: for OSX users who are experencing valgrind die ala:
# 
# 	==12799== valgrind: Unrecognised instruction at address 0x1013e4a73.
#   ==12799==    at 0x1013E4A73: __pthread_init.cold.2 (in /usr/lib/system/libsystem_pthread.dylib)
#
# 	use: https://github.com/LouisBrunner/valgrind-macos
#
# 			brew remove valgrind
#				Uninstalling /usr/local/Cellar/valgrind/3.14.0... (337 files, 50.6MB)
#
#   		brew tap LouisBrunner/valgrind
#			brew install --HEAD LouisBrunner/valgrind/valgrind
#
# A: This may happen when the C++ standard library is linked statically. Prebuilt libstdc++/libc++ often do not use frame pointers, and it breaks fast (frame-pointer-based) unwinding. Either switch to the shared library with the -shared-libstdc++ flag, or use ASAN_OPTIONS=fast_unwind_on_malloc=0. The latter could be very slow.
# 
#
# env ASAN_OPTIONS=detect_container_overflow=0

CC=cc
CXX=g++

CFLAGS=-Wno-nullability-completeness -Wno-expansion-to-defined -Wno-builtin-requires-header -Wno-c++11-extensions
CXXFLAGS=-std=c++11 

DEBUG=-g
# DEBUG=-g -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls -O0 -fsanitize-address-use-after-scope

ASAN=-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls -O0 -fsanitize-address-use-after-scope

LOCAL_LD_LIBS=-lz -lm -lpthread -lpcap -lpcre2-8 -lstdc++
LOCAL_LD_LIBS_NCURSES=$(LOCAL_LD_LIBS) -lncurses

#jjustman-2020-08-17 - sorry, from OSX...
SRT_PLATFORM_BUILD_DEFS=-DHAI_ENABLE_SRT=1 -DHAI_PATCH=1 -DHAVE_INET_PTON=1 -DOSX=1 -DSRT_ENABLE_APP_READER -DSRT_ENABLE_CLOSE_SYNCH -DSRT_ENABLE_ENCRYPTION -DSRT_ENABLE_VERBOSE_LOCK -DSRT_VERSION=\"1.4.1\" -DUSE_OPENSSL=1 -D_GNU_SOURCE

#jjustman-2020-08-17 - sorry, from OSX - fixme..
#		-L /usr/local/opt/openssl@1.1/lib \
#		/Users/jjustman/Desktop/libatsc3/srt/build/lib/libsrt.a \
#		-lssl -lcrypto \

#LOCAL_LD_LIBS_SRT_PATH=-L ../srt/build/lib -L /usr/local/opt/openssl@1.1/lib 
#LOCAL_LD_LIBS_SRT=-lsrt -lssl -lcrypto
# libsrt.a

LOCAL_LD_LIBS_SRT_PATH=-L ../srt/build/lib -L /usr/local/opt/openssl@1.1/lib 
LOCAL_LD_LIBS_SRT=../srt/build/lib/libsrt.a -lssl -lcrypto


all: 	intermediate_lls \
		intermediate_lls_asan \
		intermediate_alp \
		intermediate_baseband \
		intermediate_stltp \
		intermediate_mbms \
		intermediate_sls \
		intermediate_mmt \
		intermediate_alc \
		intermediate_a324 \
		intermediate_a337 \
		intermediate_a344 \
		intermediate_ffplay \
		intermediate_virtual_phy \
		interemediate_gpl_and_mock_linkage  \
		libatsc3_core   \
		unit_tests \
		libmicrohttpd_tests \
		listener_tests \
		a324_tests \
		phy_tests \
		tools \
		atsc3_sl_phy

clean:
	-rm -f *.o
	-rm -f route/*
	-rm -f mpu/*
	-rm -f test/atsc3_lmt_test
	-rm -f test/atsc3_lls_slt_parser_test
	-rm -f test/atsc3_lls_test
	-rm -f test/atsc3_lls_SystemTime_test
	-rm -f test/atsc3_mmt_signalling_message_test
	-rm -f test/atsc3_fdt_test
	-rm -f test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7
	-rm -f test/atsc3_stltp_parser_test
	-rm -f test/atsc3_mime_multipart_related_parser_test
	-rm -f test/atsc3_package_mode_mime_multipart_related_parser_test
	-rm -f test/atsc3_route_package_extract_test
	-rm -f test/atsc3_route_package_extract_test_noasan	
	-rm -f test/atsc3_fdt_mbms_envelope_test
	-rm -f test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7
	-rm -f test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1
	-rm -f test/atsc3_libmicrohttpd_test
	-rm -f test/atsc3_mmt_signalling_packet_test
	-rm -f test/atsc3_mmt_mpu_packet_test
	-rm -f test/atsc3_hevc_nal_extractor_test	
	-rm -f test/atsc3_pcap_replay_test
	-rm -f test/atsc3_route_dash_mpd_patch_test
	-rm -f listener_tests/atsc3_alc_listener_test
	-rm -f listener_tests/atsc3_alc_listener_test_valgrind
	-rm -f listener_tests/atsc3_lls_listener_test
	-rm -f listener_tests/atsc3_mmt_listener_test
	-rm -f listener_tests/atsc3_mmt_sls_listener_test
	-rm -f listener_tests/atsc3_stltp_listener_test
	-rm -f listener_tests/atsc3_stltp_alp_listener_test
	-rm -f listener_tests/atsc3_stltp_alp_listener_reflector_test
	-rm -f listener_tests/atsc3_stltp_alp_listener_reflector_test_noasan	
	-rm -f listener_tests/atsc3_mmt_context_mfu_depacketizer_test
	-rm -f listener_tests/atsc3_mmt_context_stpp_depacketizer_test
	-rm -f a324_tests/a324_dstp_lls_aeat_emission_test	
	-rm -f phy_tests/pcap_demuxed_virtual_phy_test
	-rm -f phy_tests/pcap_demuxed_virtual_phy_test_noasan
	-rm -f phy_tests/pcap_stltp_virtual_phy_test
	-rm -f phy_tests/pcap_stltp_virtual_phy_test_noasan	
	-rm -f phy_tests/srt_stltp_virtual_phy_test
	-rm -f phy_tests/srt_stltp_virtual_phy_test_noasan	
	-rm -f tools/atsc3_alc_listener_mde_writer
	-rm -f tools/atsc3_alc_listener_mde_writer_asan
	-rm -f tools/atsc3_alc_listener_mde_writer_malloc_debug
	-rm -f tools/atsc3_alc_listener_mde_writer_from_stltp_pcap
	-rm -f tools/atsc3_alc_listener_mde_writer_from_stltp_pcap_asan
	-rm -f tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer
	-rm -f tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer_noasan	
	-rm -f tools/atsc3_listener_metrics_ncurses
	-rm -f tools/atsc3_listener_metrics_ncurses_httpd_isobmff
	-rm -f tools/atsc3_mmt_mfu_monitor
	-rm -f tools/atsc3_mmt_listener_to_http_hls_fmp4
	-rm -f tools/atsc3_pcap_stltp_preamble_timing_management_validate
	-rm -f tools/atsc3_pcap_stltp_preamble_timing_management_validate_asan
	-rm -f test_sl/atsc3_sl_tlv_parser_to_alp_test
	-rm -f test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test
	-rm -f test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics

intermediate_lls: 	atsc3_lls.o \
					atsc3_lls_types.o \
					atsc3_lls_sls_parser.o \
					atsc3_lls_alc_utils.o \
					atsc3_aeat_types.o
					
					
intermediate_lls_asan: 	atsc3_lls_asan.o \
						atsc3_lls_types_asan.o \
						atsc3_lls_sls_parser_asan.o \
						atsc3_lls_alc_utils_asan.o \
						atsc3_aeat_types_asan.o

intermediate_alp: atsc3_alp_types.o atsc3_alp_parser.o

intermediate_baseband: atsc3_baseband_packet_types.o 

intermediate_stltp:	atsc3_stltp_depacketizer_context.o \
					atsc3_stltp_depacketizer.o

intermediate_mbms: 	atsc3_mbms_envelope_xml.o \
					atsc3_mime_multipart_related.o \
					atsc3_mime_multipart_related_parser.o \
					atsc3_route_usbd.o \
					atsc3_mbms_envelope_parser.o

intermediate_sls: 	atsc3_route_mpd.o \
					atsc3_route_s_tsid.o \
					atsc3_sls_metadata_fragment_types.o \
					atsc3_sls_metadata_fragment_types_parser.o	

intermediate_alc: 	atsc3_alc_session.o \
					atsc3_alc_rx.o \
					atsc3_alc_utils.o \
					atsc3_route_sls_processor.o

intermediate_mmt: 	xml.o \
					atsc3_mmtp_packet_types.o \
					atsc3_mmtp_packet_utils.o \
					atsc3_mmtp_parser.o \
					atsc3_mmt_mpu_parser.o \
					atsc3_mmt_mpu_sample_format_parser.o \
					atsc3_mmt_signalling_message.o \
					atsc3_mmtp_ntp32_to_pts.o \
					atsc3_utils.o \
					fixups_timespec_get.o \
					atsc3_mmt_mpu_utils.o \
					atsc3_bandwidth_statistics.o

intermediate_a324: 	atsc3_dstp_types.o

intermediate_a337:	atsc3_sls_held_fragment.o		

intermediate_a344:	atsc3_a344_receiver_query_api.o				

intermediate_ffplay: atsc3_player_ffplay.o

intermediate_virtual_phy: 	PcapDemuxedVirtualPHY.o \
				 			PcapSTLTPVirtualPHY.o \
				 			atsc3_srt_live_receiver.o \
				 			SRTRxSTLTPVirtualPHY.o
				  

interemediate_gpl_and_mock_linkage: bento4_gpl_link \
									bento4_mock_link
libatsc3_core: libatsc3.o
listener_ncurses: tools/atsc3_listener_metrics_ncurses

# unit tests are standalone drivers with default or CLI parameters for processing relevant 
# ATSC 3.0 data structs, signalling messages, etc

unit_tests: test/atsc3_lmt_test \
			test/atsc3_lls_slt_parser_test \
			test/atsc3_lls_test \
			test/atsc3_lls_SystemTime_test \
			test/atsc3_mmt_mpu_packet_test \
			test/atsc3_mmt_signalling_message_test \
			test/atsc3_mmt_signalling_packet_test \
			test/atsc3_isobmff_box_test \
			test/atsc3_fdt_test \
			test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7 \
			test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1 \
			test/atsc3_stltp_parser_test \
			test/atsc3_mime_multipart_related_parser_test \
			test/atsc3_package_mode_mime_multipart_related_parser_test \
			test/atsc3_route_package_extract_test \
			test/atsc3_route_package_extract_test_noasan \
			test/atsc3_fdt_mbms_envelope_test \
			test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7 \
			test/atsc3_hevc_nal_extractor_test \
			test/atsc3_pcap_replay_test \
			test/atsc3_route_dash_mpd_patch_test

libmicrohttpd_tests: test/atsc3_libmicrohttpd_test

# listener_tests: atsc3_alc_listener_test  atsc3_mmt_listener_test atsc3_listener_test atsc3_mmt_listener_test_bento atsc3_listener_metrics_test
# jjustman-2020-07-28 - dead: 				listener_tests/atsc3_mmt_listener_test_bento \

listener_tests: listener_tests/atsc3_alc_listener_test \
				listener_tests/atsc3_alc_listener_test_valgrind \
				listener_tests/atsc3_lls_listener_test \
				listener_tests/atsc3_mmt_listener_test \
				listener_tests/atsc3_mmt_sls_listener_test  \
				listener_tests/atsc3_stltp_listener_test \
				listener_tests/atsc3_stltp_alp_listener_test \
				listener_tests/atsc3_stltp_alp_listener_reflector_test \
				listener_tests/atsc3_stltp_alp_listener_reflector_test_noasan \
				listener_tests/atsc3_mmt_context_mfu_depacketizer_test \
				listener_tests/atsc3_mmt_context_stpp_depacketizer_test \

a324_tests: a324_tests/a324_dstp_lls_aeat_emission_test

phy_tests:	phy_tests/pcap_demuxed_virtual_phy_test \
			phy_tests/pcap_demuxed_virtual_phy_test_noasan \
			phy_tests/pcap_stltp_virtual_phy_test \
			phy_tests/pcap_stltp_virtual_phy_test_noasan \
			phy_tests/srt_stltp_virtual_phy_test \
			phy_tests/srt_stltp_virtual_phy_test_noasan

# real libatsc3 tools for lls, sls, mmt/route and flow analysis
# only on linux...	tools/atsc3_alc_listener_mde_writer_malloc_debug \

tools: tools/atsc3_listener_metrics_ncurses \
	tools/atsc3_listener_metrics_ncurses_httpd_isobmff \
	tools/atsc3_mmt_mfu_monitor \
	tools/atsc3_alc_listener_mde_writer \
	tools/atsc3_alc_listener_mde_writer_asan \
	tools/atsc3_alc_listener_mde_writer_from_stltp_pcap \
	tools/atsc3_alc_listener_mde_writer_from_stltp_pcap_asan \
	tools/atsc3_mmt_listener_to_http_hls_fmp4 \
	tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer \
	tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer_noasan \
	tools/atsc3_pcap_stltp_preamble_timing_management_validate \
	tools/atsc3_pcap_stltp_preamble_timing_management_validate_asan

atsc3_sl_phy: atsc3_sl_tlv_demod_type.o	\
	test_sl/atsc3_sl_tlv_parser_to_alp_test \
	test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test \
	test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics

# intermediate object generation for linking into libatsc3.o

xml.o: xml.c xml.h
	$(CC) $(CFLAGS) $(DEBUG)  -c xml.c 

strnstr.o: strnstr.c strnstr.h
	$(CC) $(CFLAGS) $(DEBUG)  -c strnstr.c

atsc3_alp_types.o: atsc3_alp_types.h atsc3_alp_types.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_alp_types.c 

atsc3_baseband_packet_types.o: atsc3_baseband_packet_types.h atsc3_baseband_packet_types.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_baseband_packet_types.c 

atsc3_alp_parser.o: atsc3_alp_types.h atsc3_alp_parser.h atsc3_alp_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_alp_parser.c 
	
atsc3_stltp_depacketizer_context.o: atsc3_stltp_depacketizer_context.h atsc3_stltp_depacketizer_context.c atsc3_alp_types.h atsc3_alp_parser.h
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_stltp_depacketizer_context.c
	
atsc3_stltp_depacketizer.o: atsc3_stltp_depacketizer.h atsc3_stltp_depacketizer.c atsc3_stltp_parser.h atsc3_alp_parser.h
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_stltp_depacketizer.c 

atsc3_mbms_envelope_xml.o: atsc3_mbms_envelope_xml.h atsc3_mbms_envelope_xml.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mbms_envelope_xml.c 

atsc3_mime_multipart_related.o: atsc3_mime_multipart_related.h atsc3_mime_multipart_related.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mime_multipart_related.c 

atsc3_mime_multipart_related_parser.o: atsc3_mime_multipart_related_parser.h atsc3_mime_multipart_related_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mime_multipart_related_parser.c 

atsc3_lls.o: atsc3_utils.h atsc3_lls.h atsc3_lls.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_lls.c

atsc3_lls_asan.o: atsc3_utils.h atsc3_lls.h atsc3_lls.c
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  -c atsc3_lls.c -o atsc3_lls_asan.o

atsc3_lls_types.o: atsc3_lls_types.h atsc3_lls_types.c \
		xml.h \
		atsc3_aeat_types.h \
		atsc3_monitor_events_lls.h \
		atsc3_monitor_events_sls.h \
		atsc3_monitor_events_alc.h \
		atsc3_alc_session.h \
		atsc3_udp.h \
		atsc3_listener_udp.h \
		atsc3_fdt.h \
		atsc3_sls_metadata_fragment_types.h \
		atsc3_lls_sls_monitor_output_buffer.h \
		atsc3_player_ffplay.h 
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_lls_types.c
	
	
atsc3_lls_types_asan.o: atsc3_lls_types.h atsc3_lls_types.c \
		xml.h \
		atsc3_aeat_types.h \
		atsc3_monitor_events_lls.h \
		atsc3_monitor_events_sls.h \
		atsc3_monitor_events_alc.h \
		atsc3_alc_session.h \
		atsc3_udp.h \
		atsc3_listener_udp.h \
		atsc3_fdt.h \
		atsc3_sls_metadata_fragment_types.h \
		atsc3_lls_sls_monitor_output_buffer.h \
		atsc3_player_ffplay.h 
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  -c atsc3_lls_types.c -o atsc3_lls_types_asan.o

atsc3_aeat_types.o: atsc3_aeat_types.h atsc3_aeat_types.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_aeat_types.c

atsc3_aeat_types_asan.o: atsc3_aeat_types.h atsc3_aeat_types.c
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  -c atsc3_aeat_types.c -o atsc3_aeat_types_asan.o

atsc3_aeat_parser.o: atsc3_aeat_parser.h atsc3_aeat_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_aeat_parser.c

atsc3_udp.o: atsc3_udp.h atsc3_udp.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_udp.c
	
atsc3_listener_udp.o: atsc3_listener_udp.h atsc3_listener_udp.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_listener_udp.c

atsc3_ip_udp_rtp_parser.o: atsc3_ip_udp_rtp_parser.h atsc3_ip_udp_rtp_parser.c atsc3_ip_udp_rtp_types.h
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_ip_udp_rtp_parser.c

atsc3_lls_mmt_utils.o: atsc3_lls_mmt_utils.h atsc3_lls_mmt_utils.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_lls_mmt_utils.c

atsc3_lls_alc_utils.o: atsc3_lls_alc_utils.h atsc3_lls_alc_utils.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_lls_alc_utils.c

atsc3_lls_alc_utils_asan.o: atsc3_lls_alc_utils.h atsc3_lls_alc_utils.c
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  -c atsc3_lls_alc_utils.c -o atsc3_lls_alc_utils_asan.o

atsc3_lls_sls_parser.o: atsc3_lls_sls_parser.h atsc3_lls_sls_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_lls_sls_parser.c

atsc3_lls_sls_parser_asan.o: atsc3_lls_sls_parser.h atsc3_lls_sls_parser.c
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  -c atsc3_lls_sls_parser.c -o atsc3_lls_sls_parser_asan.o

atsc3_lls_slt_parser.o: atsc3_lls_slt_parser.h atsc3_lls_slt_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_lls_slt_parser.c

atsc3_lls_slt_parser_asan.o: atsc3_lls_slt_parser.h atsc3_lls_slt_parser.c
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  -c atsc3_lls_slt_parser.c -o atsc3_lls_slt_parser_asan.o

atsc3_lls_sls_monitor_output_buffer.o: atsc3_lls_sls_monitor_output_buffer.h atsc3_lls_sls_monitor_output_buffer.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_lls_sls_monitor_output_buffer.c

atsc3_lls_sls_monitor_output_buffer_utils.o: atsc3_lls_sls_monitor_output_buffer_utils.h atsc3_lls_sls_monitor_output_buffer_utils.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_lls_sls_monitor_output_buffer_utils.c

atsc3_mmtp_packet_types.o: atsc3_mmtp_packet_types.h atsc3_mmtp_packet_types.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmtp_packet_types.c

atsc3_mmtp_packet_utils.o: atsc3_mmtp_packet_utils.h atsc3_mmtp_packet_utils.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmtp_packet_utils.c

atsc3_mmtp_parser.o: atsc3_mmtp_packet_types.h atsc3_mmtp_parser.h atsc3_mmtp_parser.c 
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmtp_parser.c

atsc3_mmt_mpu_parser.o: atsc3_mmtp_packet_types.h atsc3_mmtp_parser.h atsc3_mmt_mpu_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmt_mpu_parser.c

atsc3_mmt_mpu_sample_format_parser.o: atsc3_mmt_mpu_sample_format_type.h atsc3_mmt_mpu_sample_format_parser.h atsc3_mmt_mpu_sample_format_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmt_mpu_sample_format_parser.c

atsc3_mmt_signalling_message_type.o:atsc3_mmt_signalling_message_type.h atsc3_mmt_signalling_message_type.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmt_signalling_message_type.c

atsc3_mmt_signalling_message.o:atsc3_mmt_signalling_message_types.h atsc3_mmt_signalling_message.h atsc3_mmt_signalling_message.c 
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmt_signalling_message.c

atsc3_mmtp_ntp32_to_pts.o: atsc3_mmtp_ntp32_to_pts.c atsc3_mmtp_ntp32_to_pts.h
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmtp_ntp32_to_pts.c

atsc3_mmt_mpu_utils.o: atsc3_mmt_mpu_utils.h atsc3_mmt_mpu_utils.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmt_mpu_utils.c

atsc3_mmt_context_mfu_depacketizer.o: atsc3_mmt_context_mfu_depacketizer.h \
		atsc3_mmt_context_mpu_depacketizer.h \
		atsc3_mmt_context_signalling_information_depacketizer.h \
		atsc3_mmt_context_mfu_depacketizer.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmt_context_mfu_depacketizer.c

atsc3_utils.o: atsc3_utils.h atsc3_utils.c  
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_utils.c

fixups_timespec_get.o:  fixups.h fixups_timespec_get.c
	$(CC) $(CFLAGS) $(DEBUG)  -c fixups_timespec_get.c

atsc3_alc_rx.o:  atsc3_alc_rx.h atsc3_alc_rx.c atsc3_lct_hdr.h
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_alc_rx.c

atsc3_alc_session.o: atsc3_alc_session.h atsc3_alc_session.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_alc_session.c

atsc3_alc_utils.o: atsc3_alc_utils.h atsc3_alc_utils.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_alc_utils.c -o atsc3_alc_utils.o

atsc3_gzip.o: atsc3_gzip.h atsc3_gzip.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_gzip.c -o atsc3_gzip.o

atsc3_player_ffplay.o: atsc3_player_ffplay.h atsc3_player_ffplay.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_player_ffplay.c

atsc3_logging_externs.o: atsc3_logging_externs.c atsc3_logging_externs.h
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_logging_externs.c

atsc3_mmt_reconstitution_from_media_sample.o: atsc3_mmt_reconstitution_from_media_sample.c atsc3_mmt_reconstitution_from_media_sample.h
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mmt_reconstitution_from_media_sample.c

atsc3_stltp_types.o: atsc3_stltp_types.h atsc3_stltp_types.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_stltp_types.c

atsc3_stltp_parser.o: atsc3_stltp_parser.h atsc3_stltp_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_stltp_parser.c

atsc3_fdt.o: atsc3_fdt.h atsc3_fdt.c atsc3_vector_builder.h
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_fdt.c  -o atsc3_fdt.o

atsc3_fdt_parser.o: atsc3_fdt.o atsc3_fdt_parser.h atsc3_fdt_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_fdt_parser.c -o atsc3_fdt_parser.o

atsc3_route_usbd.o: atsc3_route_usbd.h atsc3_route_usbd.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_route_usbd.c -o atsc3_route_usbd.o

atsc3_sls_held_fragment.o: atsc3_sls_held_fragment.h atsc3_sls_held_fragment.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_sls_held_fragment.c -o atsc3_sls_held_fragment.o	

atsc3_route_mpd.o: atsc3_route_mpd.h atsc3_route_mpd.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_route_mpd.c -o atsc3_route_mpd.o

atsc3_route_s_tsid.o: atsc3_route_s_tsid.h atsc3_route_s_tsid.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_route_s_tsid.c -o atsc3_route_s_tsid.o

atsc3_sls_metadata_fragment_types.o: atsc3_sls_metadata_fragment_types.h atsc3_sls_metadata_fragment_types.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_sls_metadata_fragment_types.c -o atsc3_sls_metadata_fragment_types.o

atsc3_sls_metadata_fragment_types_parser.o: atsc3_sls_metadata_fragment_types_parser.h atsc3_sls_metadata_fragment_types_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_sls_metadata_fragment_types_parser.c -o atsc3_sls_metadata_fragment_types_parser.o

atsc3_mbms_envelope_parser.o: atsc3_mbms_envelope_parser.h atsc3_mbms_envelope_parser.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_mbms_envelope_parser.c -o atsc3_mbms_envelope_parser.o

atsc3_route_sls_processor.o: atsc3_route_sls_processor.h atsc3_route_sls_processor.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_route_sls_processor.c -o atsc3_route_sls_processor.o

atsc3_dstp_types.o: atsc3_dstp_types.h atsc3_dstp_types.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_dstp_types.c -o atsc3_dstp_types.o

atsc3_a344_receiver_query_api.o: atsc3_a344_receiver_query_api.h atsc3_a344_receiver_query_api.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_a344_receiver_query_api.c -o atsc3_a344_receiver_query_api.o	

atsc3_packet_statistics_types.o: atsc3_packet_statistics.h atsc3_packet_statistics_types.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_packet_statistics_types.c -o atsc3_packet_statistics_types.o	

# jjustman-2020-07-27 - using -w as -Wno-excess-initializers doesn't stop log spamming of "warning: excess elements in struct initializer"
atsc3_hevc_nal_extractor.o: atsc3_hevc_nal_extractor.h atsc3_hevc_nal_extractor.c
	$(CC) -w $(CFLAGS) $(DEBUG)  -c atsc3_hevc_nal_extractor.c -o atsc3_hevc_nal_extractor.o

atsc3_pcap_type.o: atsc3_pcap_type.h atsc3_pcap_type.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_pcap_type.c -o atsc3_pcap_type.o

atsc3_route_package_utils.o: atsc3_route_package_utils.h atsc3_route_package_utils.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_route_package_utils.c -o atsc3_route_package_utils.o

atsc3_pcre2_regex_utils.o: atsc3_pcre2_regex_utils.h atsc3_pcre2_regex_utils.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_pcre2_regex_utils.c -o atsc3_pcre2_regex_utils.o

atsc3_route_dash_utils.o: atsc3_route_dash_utils.h atsc3_route_dash_utils.c \
							atsc3_pcre2_regex_utils.h atsc3_pcre2_regex_utils.c	
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_route_dash_utils.c -o atsc3_route_dash_utils.o

atsc3_utils_sha256.o: atsc3_utils_sha256.h atsc3_utils_sha256.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_utils_sha256.c -o atsc3_utils_sha256.o
	
atsc3_route_object.o:	atsc3_route_object.h atsc3_route_object.c
	$(CC) $(CFLAGS) $(DEBUG) \
		-c atsc3_route_object.c \
		-o atsc3_route_object.o

atsc3_sls_alc_flow.o:	atsc3_sls_alc_flow.h atsc3_sls_alc_flow.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_sls_alc_flow.c -o atsc3_sls_alc_flow.o
	
avltree.o: libtree.h avl.c
	$(CC) $(CFLAGS) $(DEBUG)  -c avl.c -o avltree.o

PcapDemuxedVirtualPHY.o: phy/virtual/PcapDemuxedVirtualPHY.h phy/virtual/PcapDemuxedVirtualPHY.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		-c phy/virtual/PcapDemuxedVirtualPHY.cpp \
		-o PcapDemuxedVirtualPHY.o  -I.

PcapSTLTPVirtualPHY.o: phy/virtual/PcapSTLTPVirtualPHY.h phy/virtual/PcapSTLTPVirtualPHY.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		-c phy/virtual/PcapSTLTPVirtualPHY.cpp \
		-o PcapSTLTPVirtualPHY.o  -I.

# jjustman-2020-08-17 - todo: add in full deps here
atsc3_srt_live_receiver.o: phy/virtual/srt/atsc3_srt_live_receiver.h \
							phy/virtual/srt/atsc3_srt_live_receiver.cpp \
							phy/virtual/srt/atsc3_srt_target.h
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
	 	-I. -I phy/virtual/srt -I ../srt -I ../srt/build/include \
		-c phy/virtual/srt/atsc3_srt_live_receiver.cpp \
		-o atsc3_srt_live_receiver.o 

		
# phy/virtual/srt/logsupport.cpp \
		 phy/virtual/srt/socketoptions.cpp \
		 phy/virtual/srt/transmitmedia.cpp \
		 phy/virtual/srt/uriparser.cpp \
		 phy/virtual/srt/verbose.cpp \		
		 
atsc3_srt_apputil.o: phy/virtual/srt/apputil.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
	 	-I. -I phy/virtual/srt -I ../srt -I ../srt/build/include \
		-c phy/virtual/srt/apputil.cpp \
		-o atsc3_srt_apputil.o 

		 
atsc3_srt_logsupport.o: phy/virtual/srt/logsupport.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
	 	-I. -I phy/virtual/srt -I ../srt -I ../srt/build/include \
		-c phy/virtual/srt/logsupport.cpp \
		-o atsc3_srt_logsupport.o 

atsc3_srt_socketoptions.o: phy/virtual/srt/socketoptions.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
	 	-I. -I phy/virtual/srt -I ../srt -I ../srt/build/include \
		-c phy/virtual/srt/socketoptions.cpp \
		-o atsc3_srt_socketoptions.o 

atsc3_srt_transmitmedia.o: phy/virtual/srt/transmitmedia.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
	 	-I. -I phy/virtual/srt -I ../srt -I ../srt/build/include \
		-c phy/virtual/srt/transmitmedia.cpp \
		-o atsc3_srt_transmitmedia.o 
		
atsc3_srt_uriparser.o: phy/virtual/srt/uriparser.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
	 	-I. -I phy/virtual/srt -I ../srt -I ../srt/build/include \
		-c phy/virtual/srt/uriparser.cpp \
		-o atsc3_srt_uriparser.o 
		
atsc3_srt_verbose.o: phy/virtual/srt/verbose.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
	 	-I. -I phy/virtual/srt -I ../srt -I ../srt/build/include \
		-c phy/virtual/srt/verbose.cpp \
		-o atsc3_srt_verbose.o 
		
atsc3_srt_app_libs.o:	atsc3_srt_apputil.o \
						atsc3_srt_logsupport.o \
						atsc3_srt_socketoptions.o \
						atsc3_srt_transmitmedia.o \
						atsc3_srt_uriparser.o \
						atsc3_srt_verbose.o 
						
	ld -o atsc3_srt_app_libs.o -r \
						atsc3_srt_apputil.o \
						atsc3_srt_logsupport.o \
						atsc3_srt_socketoptions.o \
						atsc3_srt_transmitmedia.o \
						atsc3_srt_uriparser.o \
						atsc3_srt_verbose.o 			
		
SRTRxSTLTPVirtualPHY.o: phy/virtual/SRTRxSTLTPVirtualPHY.h phy/virtual/SRTRxSTLTPVirtualPHY.cpp
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
			$(SRT_PLATFORM_BUILD_DEFS) \
	 	-I. \
		-c phy/virtual/SRTRxSTLTPVirtualPHY.cpp \
		-o SRTRxSTLTPVirtualPHY.o 


# unit standalone tests with mock data

atsc3_stltp_parser_test: atsc3_stltp_parser_test.c \
	atsc3_stltp_types.o \
	atsc3_stltp_parser.o \
	atsc3_udp.o \
	atsc3_listener_udp.o \
	atsc3_ip_udp_rtp_parser.o \
	atsc3_utils.o
	$(CC) $(CFLAGS) $(DEBUG)  atsc3_stltp_parser_test.c \
	atsc3_stltp_types.o \
	atsc3_stltp_parser.o \
	atsc3_udp.o \
	atsc3_ip_udp_rtp_parser.o \
	atsc3_listener_udp.o \
	atsc3_utils.o \
	-o atsc3_stltp_parser_test

# stats modules

atsc3_bandwidth_statistics.o: atsc3_bandwidth_statistics.h atsc3_bandwidth_statistics.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_bandwidth_statistics.c

atsc3_packet_statistics.o: atsc3_packet_statistics.h atsc3_packet_statistics.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_packet_statistics.c

# -L ../Collections-C/build/src
test_collection: test_collection.c
	$(CC) test_collection.c -I ../Collections-C/src/include/ ../Collections-C/build/src/libcollectc.a -L ../Collections-C/build/src -o test_collection 

#build a compile time-linked WITHOUT bento4 which should not face license compliance issues
bento4_mock_link: bento4_mock.o
bento4_mock.o: bento4/ISOBMFFTrackJoiner.h bento4/ISOBMFFTrackJoiner_mock.cpp 
	$(CXX)  $(CFLAGS) $(DEBUG)  -D __ISOBMFF_LIB=true -o bento4_mock.o -c bento4/ISOBMFFTrackJoiner_mock.cpp -I../bento/include/ 


# core libatsc3 library gen
# the gpl bento4 library is NOT is not linked in, it by default will not be included 

# only link in atsc3_logging_externs.o if you are using ncurses

libatsc3_intermediate.o: atsc3_lls.o \
		atsc3_lls_types.o \
		atsc3_lls_slt_parser.o \
		atsc3_lls_sls_parser.o \
		atsc3_alp_types.o \
		atsc3_alp_parser.o \
		atsc3_stltp_depacketizer_context.o \
		atsc3_stltp_depacketizer.o \
		atsc3_baseband_packet_types.o \
		atsc3_mmtp_packet_types.o \
		atsc3_mmtp_packet_utils.o \
		atsc3_mmtp_parser.o \
		atsc3_mmtp_ntp32_to_pts.o \
		atsc3_utils.o \
		atsc3_mmt_signalling_message_types.o \
		atsc3_mmt_signalling_message.o \
		atsc3_mmt_mpu_parser.o \
		atsc3_mmt_mpu_sample_format_parser.o \
		atsc3_mmt_context_mfu_depacketizer.o \
		atsc3_alc_rx.o \
		atsc3_alc_session.o \
		atsc3_lls_alc_utils.o \
		atsc3_mmt_mpu_utils.o \
		atsc3_player_ffplay.o \
		atsc3_alc_utils.o \
		atsc3_lls_sls_monitor_output_buffer.o \
		atsc3_lls_sls_monitor_output_buffer_utils.o \
		atsc3_lls_mmt_utils.o \
		atsc3_udp.o \
		atsc3_listener_udp.o \
		atsc3_ip_udp_rtp_parser.o \
		atsc3_gzip.o \
		atsc3_stltp_types.o \
		atsc3_stltp_parser.o \
		atsc3_fdt.o \
		atsc3_fdt_parser.o \
		atsc3_route_mpd.o \
		atsc3_route_usbd.o \
		atsc3_route_s_tsid.o \
		atsc3_route_sls_processor.o \
		atsc3_sls_held_fragment.o  \
		atsc3_sls_metadata_fragment_types.o \
		atsc3_sls_metadata_fragment_types_parser.o \
		atsc3_mbms_envelope_xml.o	\
		atsc3_mime_multipart_related.o \
		atsc3_mime_multipart_related_parser.o \
		atsc3_mbms_envelope_parser.o \
		strnstr.o \
		xml.o \
		fixups_timespec_get.o \
		atsc3_dstp_types.o \
		atsc3_a344_receiver_query_api.o \
		atsc3_aeat_types.o \
		atsc3_aeat_parser.o \
		atsc3_packet_statistics_types.o \
		atsc3_hevc_nal_extractor.o \
		atsc3_pcap_type.o \
		atsc3_route_package_utils.o \
		atsc3_pcre2_regex_utils.o \
		atsc3_route_dash_utils.o \
		atsc3_utils_sha256.o \
		atsc3_route_object.o \
		atsc3_sls_alc_flow.o \
		avltree.o \
		PcapDemuxedVirtualPHY.o \
		PcapSTLTPVirtualPHY.o
#		 \
#		atsc3_srt_live_receiver.o \
#		SRTRxSTLTPVirtualPHY.o 

	ld  -o libatsc3_intermediate.o -r \
		atsc3_lls.o \
		atsc3_lls_types.o \
		atsc3_lls_slt_parser.o \
		atsc3_lls_sls_parser.o \
		atsc3_alp_types.o \
		atsc3_alp_parser.o \
		atsc3_stltp_depacketizer_context.o \
		atsc3_stltp_depacketizer.o \
		atsc3_baseband_packet_types.o \
		atsc3_mmtp_packet_types.o \
		atsc3_mmtp_packet_utils.o \
		atsc3_mmtp_parser.o \
		atsc3_mmtp_ntp32_to_pts.o \
		atsc3_utils.o \
		atsc3_mmt_signalling_message_types.o \
		atsc3_mmt_signalling_message.o \
		atsc3_mmt_mpu_parser.o \
		atsc3_mmt_mpu_sample_format_parser.o \
		atsc3_mmt_context_mfu_depacketizer.o \
		atsc3_alc_rx.o \
		atsc3_alc_session.o \
		atsc3_lls_alc_utils.o \
		atsc3_mmt_mpu_utils.o \
		atsc3_player_ffplay.o \
		atsc3_alc_utils.o \
		atsc3_lls_sls_monitor_output_buffer.o \
		atsc3_lls_sls_monitor_output_buffer_utils.o \
		atsc3_lls_mmt_utils.o \
		atsc3_udp.o \
		atsc3_listener_udp.o \
		atsc3_ip_udp_rtp_parser.o \
		atsc3_gzip.o \
		atsc3_stltp_types.o \
		atsc3_stltp_parser.o \
		atsc3_fdt.o \
		atsc3_fdt_parser.o \
		atsc3_route_mpd.o \
		atsc3_route_usbd.o \
		atsc3_route_s_tsid.o \
		atsc3_route_sls_processor.o \
		atsc3_sls_held_fragment.o  \
		atsc3_sls_metadata_fragment_types.o \
		atsc3_sls_metadata_fragment_types_parser.o \
		atsc3_mbms_envelope_xml.o	\
		atsc3_mime_multipart_related.o \
		atsc3_mime_multipart_related_parser.o \
		atsc3_mbms_envelope_parser.o \
		xml.o \
		strnstr.o \
		fixups_timespec_get.o \
		atsc3_dstp_types.o \
		atsc3_a344_receiver_query_api.o \
		atsc3_aeat_types.o \
		atsc3_aeat_parser.o \
		atsc3_packet_statistics_types.o \
		atsc3_hevc_nal_extractor.o \
		atsc3_pcap_type.o \
		atsc3_route_package_utils.o \
		atsc3_pcre2_regex_utils.o \
		atsc3_route_dash_utils.o \
		atsc3_utils_sha256.o \
		atsc3_route_object.o \
		atsc3_sls_alc_flow.o \
		avltree.o \
		PcapDemuxedVirtualPHY.o \
		PcapSTLTPVirtualPHY.o 
		
#		atsc3_srt_live_receiver.o \
#		SRTRxSTLTPVirtualPHY.o 
		
libatsc3_intermediate_asan.o: \
		atsc3_lls_asan.o \
		atsc3_lls_types_asan.o \
		atsc3_lls_slt_parser_asan.o \
		atsc3_lls_sls_parser_asan.o \
		atsc3_lls_alc_utils_asan.o \
		atsc3_alp_types.o \
		atsc3_alp_parser.o \
		atsc3_stltp_depacketizer_context.o \
		atsc3_stltp_depacketizer.o \
		atsc3_baseband_packet_types.o \
		atsc3_mmtp_packet_types.o \
		atsc3_mmtp_packet_utils.o \
		atsc3_mmtp_parser.o \
		atsc3_mmtp_ntp32_to_pts.o \
		atsc3_utils.o \
		atsc3_mmt_signalling_message_types.o \
		atsc3_mmt_signalling_message.o \
		atsc3_mmt_mpu_parser.o \
		atsc3_mmt_mpu_sample_format_parser.o \
		atsc3_mmt_context_mfu_depacketizer.o \
		atsc3_alc_rx.o \
		atsc3_alc_session.o \
		atsc3_mmt_mpu_utils.o \
		atsc3_player_ffplay.o \
		atsc3_alc_utils.o \
		atsc3_lls_sls_monitor_output_buffer.o \
		atsc3_lls_sls_monitor_output_buffer_utils.o \
		atsc3_lls_mmt_utils.o \
		atsc3_udp.o \
		atsc3_listener_udp.o \
		atsc3_ip_udp_rtp_parser.o \
		atsc3_gzip.o \
		atsc3_stltp_types.o \
		atsc3_stltp_parser.o \
		atsc3_fdt.o \
		atsc3_fdt_parser.o \
		atsc3_route_mpd.o \
		atsc3_route_usbd.o \
		atsc3_route_s_tsid.o \
		atsc3_route_sls_processor.o \
		atsc3_sls_held_fragment.o  \
		atsc3_sls_metadata_fragment_types.o \
		atsc3_sls_metadata_fragment_types_parser.o \
		atsc3_mbms_envelope_xml.o	\
		atsc3_mime_multipart_related.o \
		atsc3_mime_multipart_related_parser.o \
		atsc3_mbms_envelope_parser.o \
		strnstr.o \
		xml.o \
		fixups_timespec_get.o \
		atsc3_dstp_types.o \
		atsc3_a344_receiver_query_api.o \
		atsc3_aeat_types.o \
		atsc3_aeat_parser.o \
		atsc3_packet_statistics_types.o \
		atsc3_hevc_nal_extractor.o \
		atsc3_pcap_type.o \
		atsc3_route_package_utils.o \
		atsc3_pcre2_regex_utils.o \
		atsc3_route_dash_utils.o \
		atsc3_utils_sha256.o \
		atsc3_route_object.o \
		atsc3_sls_alc_flow.o \
		avltree.o \
		PcapDemuxedVirtualPHY.o \
		PcapSTLTPVirtualPHY.o 

	ld  -o libatsc3_intermediate_asan.o -r \
		atsc3_lls_asan.o \
		atsc3_lls_types_asan.o \
		atsc3_lls_slt_parser_asan.o \
		atsc3_lls_sls_parser_asan.o \
		atsc3_lls_alc_utils_asan.o \
		atsc3_alp_types.o \
		atsc3_alp_parser.o \
		atsc3_stltp_depacketizer_context.o \
		atsc3_stltp_depacketizer.o \
		atsc3_baseband_packet_types.o \
		atsc3_mmtp_packet_types.o \
		atsc3_mmtp_packet_utils.o \
		atsc3_mmtp_parser.o \
		atsc3_mmtp_ntp32_to_pts.o \
		atsc3_utils.o \
		atsc3_mmt_signalling_message_types.o \
		atsc3_mmt_signalling_message.o \
		atsc3_mmt_mpu_parser.o \
		atsc3_mmt_mpu_sample_format_parser.o \
		atsc3_mmt_context_mfu_depacketizer.o \
		atsc3_alc_rx.o \
		atsc3_alc_session.o \
		atsc3_mmt_mpu_utils.o \
		atsc3_player_ffplay.o \
		atsc3_alc_utils.o \
		atsc3_lls_sls_monitor_output_buffer.o \
		atsc3_lls_sls_monitor_output_buffer_utils.o \
		atsc3_lls_mmt_utils.o \
		atsc3_udp.o \
		atsc3_listener_udp.o \
		atsc3_ip_udp_rtp_parser.o \
		atsc3_gzip.o \
		atsc3_stltp_types.o \
		atsc3_stltp_parser.o \
		atsc3_fdt.o \
		atsc3_fdt_parser.o \
		atsc3_route_mpd.o \
		atsc3_route_usbd.o \
		atsc3_route_s_tsid.o \
		atsc3_route_sls_processor.o \
		atsc3_sls_held_fragment.o  \
		atsc3_sls_metadata_fragment_types.o \
		atsc3_sls_metadata_fragment_types_parser.o \
		atsc3_mbms_envelope_xml.o	\
		atsc3_mime_multipart_related.o \
		atsc3_mime_multipart_related_parser.o \
		atsc3_mbms_envelope_parser.o \
		xml.o \
		strnstr.o \
		fixups_timespec_get.o \
		atsc3_dstp_types.o \
		atsc3_a344_receiver_query_api.o \
		atsc3_aeat_types.o \
		atsc3_aeat_parser.o \
		atsc3_packet_statistics_types.o \
		atsc3_hevc_nal_extractor.o \
		atsc3_pcap_type.o \
		atsc3_route_package_utils.o \
		atsc3_pcre2_regex_utils.o \
		atsc3_route_dash_utils.o \
		atsc3_utils_sha256.o \
		atsc3_route_object.o \
		atsc3_sls_alc_flow.o \
		avltree.o \
		PcapDemuxedVirtualPHY.o \
		PcapSTLTPVirtualPHY.o 
		

libatsc3.o: libatsc3_intermediate.o bento4_mock.o
	ld  -o libatsc3.o -r libatsc3_intermediate.o bento4_mock.o

# MIT-compatable linkage (no bento4) by default...
atsc3_isobmff_tools.o: atsc3_isobmff_tools.h atsc3_isobmff_tools.cpp
	$(CXX)  $(CFLAGS) $(DEBUG)  -o atsc3_isobmff_tools.o -c atsc3_isobmff_tools.cpp -I../bento/include/

# GPL-compatable linkage (w/ bento4)...
atsc3_isobmff_tools_gpl.o: atsc3_isobmff_tools.h atsc3_isobmff_tools.cpp
	$(CXX)  -D__ENABLE_GPL_BENTO4__ $(CFLAGS) $(DEBUG)  -o atsc3_isobmff_tools_gpl.o -c atsc3_isobmff_tools.cpp -I../bento/include/


# bento4 included library linkage
bento4_gpl_link: bento4_gpl.o 
bento4_gpl.o: bento4/ISOBMFFTrackJoiner.h bento4/ISOBMFFTrackJoiner.cpp
	$(CXX)  $(CFLAGS) $(DEBUG)  -D __ISOBMFF_LIB=true -o bento4_gpl.o -c bento4/ISOBMFFTrackJoiner.cpp -I../bento/include/ 

libatsc3_bento4_gpl.o: libatsc3_intermediate.o bento4_gpl.o
	ld  -o libatsc3_bento4_gpl.o -r libatsc3_intermediate.o bento4_gpl.o -L../bento/lib -lap4
#-lm -lpthread -lz

libatsc3_bento4_gpl_asan.o: libatsc3_intermediate_asan.o bento4_gpl.o
	ld  -o libatsc3_bento4_gpl_asan.o -r libatsc3_intermediate_asan.o bento4_gpl.o -L../bento/lib -lap4

###  unit test generation - drivers are in test/

test/atsc3_lmt_test: test/atsc3_lmt_test.c libatsc3.o 
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_lmt_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_lmt_test 

test/atsc3_lls_test: test/atsc3_lls_test.c libatsc3.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_lls_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_lls_test

test/atsc3_lls_slt_parser_test: test/atsc3_lls_slt_parser_test.c libatsc3.o 
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_lls_slt_parser_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_lls_slt_parser_test 

test/atsc3_lls_SystemTime_test: test/atsc3_lls_SystemTime_test.c libatsc3.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_lls_SystemTime_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_lls_SystemTime_test 

test/atsc3_mmt_signalling_message_test: test/atsc3_mmt_signalling_message_test.c
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_mmt_signalling_message_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_mmt_signalling_message_test

# jjustman-2020-07-07 - removed 3 -O0 
test/atsc3_mmt_signalling_packet_test: test/atsc3_mmt_signalling_packet_test.c atsc3_utils.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG) test/atsc3_mmt_signalling_packet_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_mmt_signalling_packet_test
	echo "valgrind --leak-check=full  ./atsc3_mmt_signalling_packet_test" > test/atsc3_mmt_signalling_packet_test_valgrind
	chmod +x test/atsc3_mmt_signalling_packet_test_valgrind

# jjustman-2020-07-07 - removed 3 -O0 
test/atsc3_mmt_mpu_packet_test: test/atsc3_mmt_mpu_packet_test.c atsc3_utils.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG) test/atsc3_mmt_mpu_packet_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_mmt_mpu_packet_test
	echo "valgrind --leak-check=full  ./atsc3_mmt_mpu_packet_test" > test/atsc3_mmt_mpu_packet_test_valgrind
	chmod +x test/atsc3_mmt_mpu_packet_test_valgrind

test/atsc3_isobmff_box_test: test/atsc3_isobmff_box_test.c
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_isobmff_box_test.c $(LOCAL_LD_LIBS) -o test/atsc3_isobmff_box_test 

test/atsc3_fdt_test: test/atsc3_fdt_test.c libatsc3.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_fdt_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_fdt_test

test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7: test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c libatsc3.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7

test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1: test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1.c
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1

test/atsc3_hevc_nal_extractor_test: test/atsc3_hevc_nal_extractor_test.c libatsc3.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_hevc_nal_extractor_test.c libatsc3.o $(LOCAL_LD_LIBS) -o test/atsc3_hevc_nal_extractor_test

test/atsc3_libmicrohttpd_test: test/atsc3_libmicrohttpd_test.c 
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_libmicrohttpd_test.c  \
		-I../libmicrohttpd/libmicrohttpd-0.9.63/build/include \
		-L../libmicrohttpd/libmicrohttpd-0.9.63/build/lib -lmicrohttpd \
		-o test/atsc3_libmicrohttpd_test

#todo - link against libatsc3 intermediate as these are almost integration tests
test/atsc3_mime_multipart_related_parser_test: test/atsc3_mime_multipart_related_parser_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_utils.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o 
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_mime_multipart_related_parser_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o \
	atsc3_utils.o -o test/atsc3_mime_multipart_related_parser_test

test/atsc3_package_mode_mime_multipart_related_parser_test: test/atsc3_package_mode_mime_multipart_related_parser_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_utils.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o 
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_package_mode_mime_multipart_related_parser_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o \
	atsc3_utils.o -o test/atsc3_package_mode_mime_multipart_related_parser_test

test/atsc3_route_package_extract_test: test/atsc3_route_package_extract_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_utils.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_route_package_utils.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o \
	atsc3_utils_sha256.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_route_package_extract_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_route_package_utils.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o \
	atsc3_utils_sha256.o \
	atsc3_utils.o -o test/atsc3_route_package_extract_test

test/atsc3_route_package_extract_test_noasan: test/atsc3_route_package_extract_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_utils.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_route_package_utils.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o \
	atsc3_utils_sha256.o
	$(CC) $(CFLAGS) $(DEBUG)  test/atsc3_route_package_extract_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_route_package_utils.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o \
	atsc3_utils_sha256.o \
	atsc3_utils.o -o test/atsc3_route_package_extract_test_noasan

test/atsc3_fdt_mbms_envelope_test: test/atsc3_fdt_mbms_envelope_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_utils.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_mbms_envelope_parser.o \
	strnstr.o \
	xml.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_fdt_mbms_envelope_test.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_mbms_envelope_parser.o \
	strnstr.o \
	xml.o \
	atsc3_utils.o -o test/atsc3_fdt_mbms_envelope_test

test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7: test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_utils.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_mbms_envelope_parser.o \
	strnstr.o \
	xml.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7.c \
	atsc3_mbms_envelope_xml.o	\
	atsc3_mime_multipart_related.o \
	atsc3_mime_multipart_related_parser.o \
	atsc3_sls_metadata_fragment_types.o \
	atsc3_sls_metadata_fragment_types_parser.o \
	atsc3_sls_held_fragment.o \
	atsc3_route_mpd.o \
	atsc3_route_s_tsid.o \
	atsc3_fdt_parser.o \
	atsc3_fdt.o \
	atsc3_route_usbd.o \
	atsc3_mbms_envelope_parser.o \
	xml.o \
	strnstr.o \
	atsc3_utils.o -o test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7

test/atsc3_pcap_replay_test: test/atsc3_pcap_replay_test.c atsc3_pcap_type.o atsc3_utils.o
	$(CC) $(ASAN) $(CFLAGS) $(DEBUG)  test/atsc3_pcap_replay_test.c atsc3_pcap_type.o  atsc3_utils.o -lz  -lm -lpthread -lpcap -o test/atsc3_pcap_replay_test 


test/atsc3_route_dash_mpd_patch_test: test/atsc3_route_dash_mpd_patch_test.c \
										atsc3_utils.o \
										atsc3_pcre2_regex_utils.h \
										atsc3_pcre2_regex_utils.o \
										atsc3_route_dash_utils.o \
										atsc3_alc_session.o \
										atsc3_lls_mmt_utils.o \
										atsc3_lls_sls_monitor_output_buffer_utils.o \
										atsc3_lls_sls_monitor_output_buffer.o \
										atsc3_player_ffplay.o \
										atsc3_route_s_tsid.o \
										atsc3_lls_types.o \
										atsc3_lls_alc_utils.o \
										atsc3_fdt.o \
										atsc3_fdt_parser.o \
										atsc3_sls_metadata_fragment_types.o \
										atsc3_route_sls_processor.o \
										atsc3_mbms_envelope_parser.o \
										atsc3_mime_multipart_related.o \
										atsc3_mime_multipart_related_parser.o \
										atsc3_mbms_envelope_xml.o \
										atsc3_route_mpd.o \
										atsc3_route_package_utils.o \
										atsc3_sls_held_fragment.o \
										atsc3_sls_metadata_fragment_types.o \
										atsc3_sls_metadata_fragment_types_parser.o \
										atsc3_route_usbd.o \
										atsc3_alc_utils.o \
										xml.o \
										atsc3_mmtp_ntp32_to_pts.o \
										atsc3_utils_sha256.o \
										atsc3_route_object.o \
										atsc3_sls_alc_flow.o \
										avltree.o \
										strnstr.o

	$(CC) $(ASAN) $(CFLAGS) $(DEBUG) test/atsc3_route_dash_mpd_patch_test.c  \
							atsc3_utils.o \
							atsc3_pcre2_regex_utils.o \
							atsc3_route_dash_utils.o \
							atsc3_alc_session.o \
							atsc3_lls_mmt_utils.o \
							atsc3_lls_sls_monitor_output_buffer_utils.o \
							atsc3_lls_sls_monitor_output_buffer.o \
							atsc3_player_ffplay.o \
							atsc3_route_s_tsid.o \
							atsc3_lls_types.o \
							atsc3_lls_alc_utils.o \
							atsc3_fdt.o \
							atsc3_fdt_parser.o \
							atsc3_route_sls_processor.o \
							atsc3_mbms_envelope_parser.o \
							atsc3_mime_multipart_related.o \
							atsc3_mime_multipart_related_parser.o \
							atsc3_mbms_envelope_xml.o \
							atsc3_route_mpd.o \
							atsc3_route_package_utils.o \
							atsc3_sls_held_fragment.o \
							atsc3_sls_metadata_fragment_types.o \
							atsc3_sls_metadata_fragment_types_parser.o \
							atsc3_route_usbd.o \
							atsc3_alc_utils.o \
							xml.o \
							atsc3_mmtp_ntp32_to_pts.o \
							atsc3_utils_sha256.o \
							atsc3_route_object.o \
							atsc3_sls_alc_flow.o \
							avltree.o \
							strnstr.o \
							$(LOCAL_LD_LIBS) \
							-o test/atsc3_route_dash_mpd_patch_test 


### integration tests
### TODO: move these into target makefile in listener_test/folder

# low level signal listener testing
listener_tests/atsc3_lls_listener_test: listener_tests/atsc3_lls_listener_test.cpp libatsc3.o 
	$(CXX)  $(ASAN) $(CFLAGS) $(DEBUG) listener_tests/atsc3_lls_listener_test.cpp libatsc3.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_lls_listener_test


# stltp listener test
listener_tests/atsc3_stltp_listener_test: listener_tests/atsc3_stltp_listener_test.cpp libatsc3.o
	$(CXX)  $(ASAN) $(CFLAGS) $(DEBUG)  listener_tests/atsc3_stltp_listener_test.cpp libatsc3.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_stltp_listener_test


# stltp and alp listener test
listener_tests/atsc3_stltp_alp_listener_test: listener_tests/atsc3_stltp_alp_listener_test.cpp libatsc3.o
	$(CXX)  $(ASAN) $(CFLAGS) $(DEBUG)  listener_tests/atsc3_stltp_alp_listener_test.cpp libatsc3.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_stltp_alp_listener_test

# jjustman-2020-06-02 - renamed from atsc3_stltp_listener_alp_reflector.cpp to match atsc3_stltp_alp_listener_reflector_test.cpp
# jjustman-2020-07-27 - link in atsc3_logging_externs.o as this is a very chatty output
listener_tests/atsc3_stltp_alp_listener_reflector_test: listener_tests/atsc3_stltp_alp_listener_reflector_test.cpp libatsc3.o atsc3_logging_externs.o
	$(CXX)  $(CFLAGS) $(DEBUG) listener_tests/atsc3_stltp_alp_listener_reflector_test.cpp libatsc3.o atsc3_logging_externs.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_stltp_alp_listener_reflector_test
		
listener_tests/atsc3_stltp_alp_listener_reflector_test_noasan: listener_tests/atsc3_stltp_alp_listener_reflector_test.cpp libatsc3.o atsc3_logging_externs.o
	g++ $(CFLAGS) $(DEBUG) listener_tests/atsc3_stltp_alp_listener_reflector_test.cpp libatsc3.o atsc3_logging_externs.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_stltp_alp_listener_reflector_test_noasan

listener_tests/atsc3_mmt_sls_listener_test: listener_tests/atsc3_mmt_sls_listener_test.cpp libatsc3.o
	$(CXX)  $(ASAN) $(CFLAGS) $(DEBUG)  listener_tests/atsc3_mmt_sls_listener_test.cpp libatsc3.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_mmt_sls_listener_test

# simple MMT listener that builds a single ISOBMFF segment with v/a muxed in for each MPU
listener_tests/atsc3_mmt_listener_test: listener_tests/atsc3_mmt_listener_test.cpp libatsc3_bento4_gpl.o 
	$(CXX)  $(ASAN) -D __ISOBMFF_LIB=true $(CFLAGS) $(DEBUG)  listener_tests/atsc3_mmt_listener_test.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_mmt_listener_test

# simple ALC/DASH listener that builds a single ISOBMFF segment with v/a muxed in for each MPU
listener_tests/atsc3_alc_listener_test: listener_tests/atsc3_alc_listener_test.cpp libatsc3_bento4_gpl.o
	$(CXX)  $(ASAN) $(CFLAGS) $(DEBUG)  listener_tests/atsc3_alc_listener_test.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_alc_listener_test

# simple ALC/DASH listener that builds a single ISOBMFF segment with v/a muxed in for each MPU
listener_tests/atsc3_alc_listener_test_valgrind: listener_tests/atsc3_alc_listener_test.cpp libatsc3_bento4_gpl.o
	$(CXX)  $(ASAN) -D _TEST_RUN_VALGRIND_OSX_=true $(CFLAGS) $(DEBUG)  listener_tests/atsc3_alc_listener_test.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_alc_listener_test_valgrind
			
# mfu context depacketizer/callback testing

listener_tests/atsc3_mmt_context_mfu_depacketizer_test: listener_tests/atsc3_mmt_context_mfu_depacketizer_test.cpp \
		libatsc3_bento4_gpl.o
	$(CXX)  $(ASAN) $(CFLAGS) $(DEBUG)  listener_tests/atsc3_mmt_context_mfu_depacketizer_test.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_mmt_context_mfu_depacketizer_test


# stpp context depacketizer/callback testing

listener_tests/atsc3_mmt_context_stpp_depacketizer_test: listener_tests/atsc3_mmt_context_stpp_depacketizer_test.cpp \
		libatsc3_bento4_gpl.o
	$(CXX)  $(ASAN) $(CFLAGS) $(DEBUG)  listener_tests/atsc3_mmt_context_stpp_depacketizer_test.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o listener_tests/atsc3_mmt_context_stpp_depacketizer_test

# this one has some ncurses inclusion to support 		
# -D _TEST_RUN_VALGRIND_OSX_=true to disable pthread_create on osx due to issues with lldb startup bug


# a324_tests

a324_tests/a324_dstp_lls_aeat_emission_test: a324_tests/a324_dstp_lls_aeat_emission_test.cpp libatsc3_bento4_gpl.o
	$(CXX)  $(ASAN) $(CFLAGS) $(DEBUG)  a324_tests/a324_dstp_lls_aeat_emission_test.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o a324_tests/a324_dstp_lls_aeat_emission_test

# jjustman-2020-08-10 - phy tests

# Demuxed pcap virtual phy
phy_tests/pcap_demuxed_virtual_phy_test: phy_tests/PcapDemuxedVirtualPHYTest.cpp libatsc3_bento4_gpl.o
	g++ $(ASAN) $(CFLAGS) $(CXXFLAGS) $(DEBUG) -I . \
		phy_tests/PcapDemuxedVirtualPHYTest.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o phy_tests/pcap_demuxed_virtual_phy_test
		
phy_tests/pcap_demuxed_virtual_phy_test_noasan: phy_tests/PcapDemuxedVirtualPHYTest.cpp libatsc3_bento4_gpl.o
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) -I . \
		phy_tests/PcapDemuxedVirtualPHYTest.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o phy_tests/pcap_demuxed_virtual_phy_test_noasan
		
		
# STLTP pcap virtual phy
phy_tests/pcap_stltp_virtual_phy_test: phy_tests/PcapSTLTPVirtualPHYTest.cpp libatsc3_bento4_gpl.o
	g++ $(ASAN) $(CFLAGS) $(CXXFLAGS) $(DEBUG) -I . \
		phy_tests/PcapSTLTPVirtualPHYTest.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o phy_tests/pcap_stltp_virtual_phy_test
		
phy_tests/pcap_stltp_virtual_phy_test_noasan: phy_tests/PcapSTLTPVirtualPHYTest.cpp libatsc3_bento4_gpl.o
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) -I . \
		phy_tests/PcapSTLTPVirtualPHYTest.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o phy_tests/pcap_stltp_virtual_phy_test_noasan
				
# jjustman-2020-08-17 - srt virtual phy test		
phy_tests/srt_stltp_virtual_phy_test: phy_tests/SRTRxSTLTPVirtualPHYTest.cpp libatsc3_bento4_gpl.o atsc3_srt_live_receiver.o atsc3_srt_app_libs.o SRTRxSTLTPVirtualPHY.o
	g++ $(ASAN) $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
		-I . \
		phy_tests/SRTRxSTLTPVirtualPHYTest.cpp \
		atsc3_srt_live_receiver.o atsc3_srt_app_libs.o SRTRxSTLTPVirtualPHY.o \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		$(LOCAL_LD_LIBS_SRT_PATH) $(LOCAL_LD_LIBS_SRT) \
		-o phy_tests/srt_stltp_virtual_phy_test
		
phy_tests/srt_stltp_virtual_phy_test_noasan: phy_tests/SRTRxSTLTPVirtualPHYTest.cpp libatsc3_bento4_gpl.o atsc3_srt_live_receiver.o atsc3_srt_app_libs.o SRTRxSTLTPVirtualPHY.o
	g++ $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
		-I . \
		phy_tests/SRTRxSTLTPVirtualPHYTest.cpp \
		atsc3_srt_live_receiver.o atsc3_srt_app_libs.o SRTRxSTLTPVirtualPHY.o \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		$(LOCAL_LD_LIBS_SRT_PATH) $(LOCAL_LD_LIBS_SRT) \
		-o phy_tests/srt_stltp_virtual_phy_test_noasan

# misc utils	
# isobmfftrackjoiner: bento4/ISOBMFFTrackJoiner.cpp bento4/ISOBMFFTrackJoiner.h atsc3_utils.o \
#					atsc3_lls_sls_monitor_output_buffer_utils.o atsc3_player_ffplay.o
#	$(CXX)  $(CFLAGS) $(DEBUG)  bento4/ISOBMFFTrackJoiner.cpp atsc3_utils.o  atsc3_lls_sls_monitor_output_buffer_utils.o \
#		 atsc3_player_ffplay.o -I../bento/include/ -L../bento/lib -lap4 -lm -lpthread -o isobmfftrackjoiner 

# real tools here	
tools/atsc3_listener_metrics_ncurses: tools/atsc3_listener_metrics_ncurses.cpp \
								atsc3_bandwidth_statistics.c \
								atsc3_packet_statistics.c \
								atsc3_output_statistics_ncurses.h \
								atsc3_output_statistics_ncurses.c \
								atsc3_mmt_reconstitution_from_media_sample.o \
								atsc3_logging_externs.o \
								atsc3_isobmff_tools_gpl.o \
								libatsc3_bento4_gpl.o

	$(CXX)  -Wno-deprecated -D OUTPUT_STATISTICS=NCURSES $(CFLAGS) $(DEBUG)  tools/atsc3_listener_metrics_ncurses.cpp \
			libatsc3_bento4_gpl.o \
			atsc3_isobmff_tools_gpl.o \
			atsc3_output_statistics_ncurses.c \
			atsc3_bandwidth_statistics.c \
			atsc3_packet_statistics.c \
			atsc3_mmt_reconstitution_from_media_sample.c \
			atsc3_logging_externs.o \
			-I../bento/include/ \
			$(LOCAL_LD_LIBS_NCURSES) \
			-o tools/atsc3_listener_metrics_ncurses

tools/atsc3_listener_metrics_ncurses_httpd_isobmff: tools/atsc3_listener_metrics_ncurses_httpd_isobmff.cpp \
								atsc3_bandwidth_statistics.c \
								atsc3_packet_statistics.c \
								atsc3_output_statistics_ncurses.h \
								atsc3_output_statistics_ncurses.c  \
								atsc3_mmt_reconstitution_from_media_sample.o \
								atsc3_logging_externs.o \
								atsc3_isobmff_tools_gpl.o \
								libatsc3_bento4_gpl.o

	$(CXX)  -Wno-deprecated -D OUTPUT_STATISTICS=NCURSES $(CFLAGS) $(DEBUG)  tools/atsc3_listener_metrics_ncurses_httpd_isobmff.cpp \
			libatsc3_bento4_gpl.o \
			atsc3_isobmff_tools_gpl.o \
			atsc3_output_statistics_ncurses.c \
			atsc3_bandwidth_statistics.c \
			atsc3_packet_statistics.c \
			atsc3_mmt_reconstitution_from_media_sample.c \
			atsc3_logging_externs.o \
			-I../bento/include/ \
			$(LOCAL_LD_LIBS_NCURSES) \
			-I../libmicrohttpd/libmicrohttpd-0.9.63/build/include \
			-L../libmicrohttpd/libmicrohttpd-0.9.63/build/lib -lmicrohttpd \
			-o tools/atsc3_listener_metrics_ncurses_httpd_isobmff

# atsc3_mmt_mfu_monitor

tools/atsc3_mmt_mfu_monitor: tools/atsc3_mmt_mfu_monitor.cpp \
						atsc3_bandwidth_statistics.c \
						atsc3_packet_statistics.c \
						atsc3_output_statistics_mfu_ncurses.h \
						atsc3_output_statistics_mfu_ncurses.c \
						atsc3_mmt_reconstitution_from_media_sample.o \
						atsc3_logging_externs.o \
						atsc3_isobmff_tools_gpl.o \
						libatsc3_bento4_gpl.o

	$(CXX)  -Wno-deprecated -D OUTPUT_STATISTICS=NCURSES $(CFLAGS) $(DEBUG) \
			tools/atsc3_mmt_mfu_monitor.cpp \
			libatsc3_bento4_gpl.o \
			atsc3_isobmff_tools_gpl.o \
			atsc3_output_statistics_mfu_ncurses.c \
			atsc3_bandwidth_statistics.c \
			atsc3_packet_statistics.c \
			atsc3_mmt_reconstitution_from_media_sample.c \
			atsc3_logging_externs.o \
			-I../bento/include/ \
			$(LOCAL_LD_LIBS_NCURSES) \
			-o tools/atsc3_mmt_mfu_monitor

# jjustman-2020-03-26 - malloc/double free debugging
# atsc3_alc_listener_mde_writer: tools/atsc3_alc_listener_mde_writer.cpp	libatsc3_bento4_gpl.o
#	$(CXX)   -D OUTPUT_STATISTICS=NCURSES -D__MALLOC_DEBUGGING=1 \
#		$(CFLAGS) $(DEBUG)  tools/atsc3_alc_listener_mde_writer.cpp \
#		libatsc3_bento4_gpl.o \
#		-I../bento/include/ -lpcap  -lncurses -lpcap -lz -lpthread -lmcheck\
#		-o tools/atsc3_alc_listener_mde_writer


tools/atsc3_alc_listener_mde_writer: tools/atsc3_alc_listener_mde_writer.cpp	libatsc3_bento4_gpl.o
	$(CXX) $(CFLAGS) $(DEBUG) tools/atsc3_alc_listener_mde_writer.cpp \
		libatsc3_bento4_gpl.o \
		-I../bento/include/ \
		$(LOCAL_LD_LIBS) \
		-o tools/atsc3_alc_listener_mde_writer
		
tools/atsc3_alc_listener_mde_writer_asan: tools/atsc3_alc_listener_mde_writer.cpp	libatsc3_bento4_gpl_asan.o
	$(CXX) $(ASAN) $(CFLAGS) $(DEBUG)  tools/atsc3_alc_listener_mde_writer.cpp \
		libatsc3_bento4_gpl_asan.o \
		-I../bento/include/ \
		$(LOCAL_LD_LIBS) \
		-o tools/atsc3_alc_listener_mde_writer_asan
		
# STLTP pcap replay and ROUTE / ALC extractor
tools/atsc3_alc_listener_mde_writer_from_stltp_pcap: tools/atsc3_alc_listener_mde_writer_from_stltp_pcap.cpp libatsc3_bento4_gpl.o
	$(CXX) $(CFLAGS) $(DEBUG) tools/atsc3_alc_listener_mde_writer_from_stltp_pcap.cpp \
		libatsc3_bento4_gpl.o \
		-I../bento/include/ \
		-I . \
		$(LOCAL_LD_LIBS) \
		-o tools/atsc3_alc_listener_mde_writer_from_stltp_pcap

# simple ALC/DASH listener that builds a single ISOBMFF segment with v/a muxed in for each MPU
tools/atsc3_alc_listener_mde_writer_from_stltp_pcap_asan: tools/atsc3_alc_listener_mde_writer_from_stltp_pcap.cpp libatsc3_bento4_gpl.o
	$(CXX) $(ASAN) $(CFLAGS) $(DEBUG)  tools/atsc3_alc_listener_mde_writer_from_stltp_pcap.cpp \
		libatsc3_bento4_gpl_asan.o \
		-I../bento/include/ \
		-I . \
		$(LOCAL_LD_LIBS) \
		-o tools/atsc3_alc_listener_mde_writer_from_stltp_pcap_asan
		

# jjustman-2020-08-18 - adding web-ba support tool: atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer

tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer: tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer.cpp libatsc3_bento4_gpl.o
	g++ $(ASAN) $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
		-I . \
		tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer.cpp \
		atsc3_srt_live_receiver.o atsc3_srt_app_libs.o SRTRxSTLTPVirtualPHY.o \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		$(LOCAL_LD_LIBS_SRT_PATH) $(LOCAL_LD_LIBS_SRT) \
		-o tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer

tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer_noasan: tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer.cpp libatsc3_bento4_gpl.o
	g++  $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
		-I . \
		tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer.cpp \
		atsc3_srt_live_receiver.o atsc3_srt_app_libs.o SRTRxSTLTPVirtualPHY.o \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		$(LOCAL_LD_LIBS_SRT_PATH) $(LOCAL_LD_LIBS_SRT) \
		-o tools/atsc3_srt_rx_stltp_virtual_phy_alc_listener_writer_noasan

tools/atsc3_pcap_stltp_preamble_timing_management_validate: tools/atsc3_pcap_stltp_preamble_timing_management_validate.cpp libatsc3_bento4_gpl.o
	g++  $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
		-I . \
		tools/atsc3_pcap_stltp_preamble_timing_management_validate.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o tools/atsc3_pcap_stltp_preamble_timing_management_validate


tools/atsc3_pcap_stltp_preamble_timing_management_validate_asan: tools/atsc3_pcap_stltp_preamble_timing_management_validate.cpp libatsc3_bento4_gpl.o
	g++ $(ASAN) $(CFLAGS) $(CXXFLAGS) $(DEBUG) \
		$(SRT_PLATFORM_BUILD_DEFS) \
		-I . \
		tools/atsc3_pcap_stltp_preamble_timing_management_validate.cpp \
		libatsc3_bento4_gpl.o \
		$(LOCAL_LD_LIBS) \
		-o tools/atsc3_pcap_stltp_preamble_timing_management_validate_asan

#		
# tools/atsc3_alc_listener_mde_writer_malloc_debug: tools/atsc3_alc_listener_mde_writer.cpp	libatsc3_bento4_gpl.o
#	$(CXX)  -D __MALLOC_DEBUGGING \
#		 $(CFLAGS) $(DEBUG)  tools/atsc3_alc_listener_mde_writer.cpp \
#		libatsc3_bento4_gpl.o \
#		-I../bento/include/ \
#		-I../gnulib/ \
#		$(LOCAL_LD_LIBS) \
#		-o tools/atsc3_alc_listener_mde_writer_malloc_debug	 	

tools/atsc3_mmt_listener_to_http_hls_fmp4: tools/atsc3_mmt_listener_to_http_hls_fmp4.cpp \
								atsc3_mmt_reconstitution_from_media_sample.o  \
								atsc3_logging_externs.o \
								atsc3_isobmff_tools_gpl.o \
								libatsc3_bento4_gpl.o

	$(CXX)  -Wno-deprecated -D OUTPUT_STATISTICS=NCURSES $(CFLAGS) $(DEBUG)  tools/atsc3_mmt_listener_to_http_hls_fmp4.cpp \
		libatsc3_bento4_gpl.o \
		atsc3_isobmff_tools_gpl.o \
		atsc3_mmt_reconstitution_from_media_sample.c atsc3_logging_externs.o \
		-I../bento/include/ \
		$(LOCAL_LD_LIBS) \
		-I../libmicrohttpd/libmicrohttpd-0.9.63/build/include \
		-L../libmicrohttpd/libmicrohttpd-0.9.63/build/lib -lmicrohttpd \
		-o tools/atsc3_mmt_listener_to_http_hls_fmp4

atsc3_sl_tlv_demod_type.o: atsc3_sl_tlv_demod_type.h atsc3_sl_tlv_demod_type.c
	$(CC) $(CFLAGS) $(DEBUG)  -c atsc3_sl_tlv_demod_type.c

test_sl/atsc3_sl_tlv_parser_to_alp_test: 	test_sl/atsc3_sl_tlv_parser_to_alp_test.c \
											atsc3_sl_tlv_demod_type.o \
											libatsc3.o
	$(CC) $(CFLAGS) $(DEBUG)  test_sl/atsc3_sl_tlv_parser_to_alp_test.c \
		  atsc3_sl_tlv_demod_type.o \
		  libatsc3.o \
	   -o test_sl/atsc3_sl_tlv_parser_to_alp_test $(LOCAL_LD_LIBS)

test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics:	test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics.c \
														atsc3_sl_tlv_demod_type.o \
														libatsc3.o
	$(CC) $(CFLAGS) $(DEBUG)  test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics.c \
	  	atsc3_sl_tlv_demod_type.o \
		libatsc3.o \
		-o test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics $(LOCAL_LD_LIBS)

test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test: 	test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test.c \
													atsc3_sl_tlv_demod_type.o \
													libatsc3.o
	$(CC) $(CFLAGS) $(DEBUG)  test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test.c \
		  atsc3_sl_tlv_demod_type.o \
		  libatsc3.o \
	   -o test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test $(LOCAL_LD_LIBS)


