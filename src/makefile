# jjustman - updated - 2019-10-05
# 
# MIT "compatable" library is built as libatsc3.o (e.g. non-gpl distribution)
#
# GPL "compatable" gpl binaries are only built for the toosl output linkages, as it will contain linking steps againt Bento4 for ISOBMFF box management
#	e.g. bento4_gpl.o and atsc3_isobmff_tools_gpl.o (with -D__ENABLE_GPL_BENTO4__)
#
# 	requirements: bento4 libary, ncurses libary and headers
#

# jjustman-2020-03-13 - add support for older cc versions (e.g. 4.8.4)
# jjustman-2020-03-26 - disabling as it causes libpacap typedef headers to fail (e.g.  typedef u_int bpf_u_int32;)
# CFLAGS=-std=c99
# $(info    CFLAGS is $(CFLAGS))
#
# jjustman-2020-03-26 - added #ifdef __MALLOC_DEBUGGING for doublefree crash debugging on linux in atsc3_alc_listener_mde_writer
# 	to enable, add -l
all: 	intermediate_lls \
		intermediate_alp \
		intermediate_baseband \
		intermediate_mbms \
		intermediate_sls \
		intermediate_mmt \
		intermediate_alc \
		intermediate_a324 \
		intermediate_a337 \
		intermediate_a344 \
		intermediate_ffplay \
		interemediate_gpl_and_mock_linkage  \
		libatsc3_core   \
		unit_tests \
		libmicrohttpd_tests \
		listener_tests \
		a324_tests \
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
	-rm -f test/atsc3_fdt_mbms_envelope_test
	-rm -f test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7
	-rm -f test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1
	-rm -f test/atsc3_libmicrohttpd_test
	-rm -f test/atsc3_mmt_signalling_packet_test
	-rm -f test/atsc3_mmt_mpu_packet_test
	-rm -f test/atsc3_hevc_nal_extractor_test	
	-rm -f test/atsc3_pcap_replay_test
	-rm -f listener_tests/atsc3_alc_listener_test
	-rm -f listener_tests/atsc3_alc_listener_test_valgrind
	-rm -f listener_tests/atsc3_lls_listener_test
	-rm -f listener_tests/atsc3_mmt_listener_test
	-rm -f listener_tests/atsc3_mmt_sls_listener_test
	-rm -f listener_tests/atsc3_stltp_listener_test
	-rm -f listener_tests/atsc3_stltp_alp_listener_test
	-rm -f listener_tests/atsc3_stltp_alp_listener_reflector_test
	-rm -f listener_tests/atsc3_mmt_context_mfu_depacketizer_test
	-rm -f listener_tests/atsc3_mmt_context_stpp_depacketizer_test
	-rm -f a324_tests/a324_dstp_lls_aeat_emission_test	
	-rm -f tools/atsc3_alc_listener_mde_writer
	-rm -f tools/atsc3_listener_metrics_ncurses
	-rm -f tools/atsc3_listener_metrics_ncurses_httpd_isobmff
	-rm -f tools/atsc3_mmt_mfu_monitor
	-rm -f tools/atsc3_mmt_listener_to_http_hls_fmp4
	-rm -f test_sl/atsc3_sl_tlv_parser_to_alp_test
	-rm -f test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test
	-rm -f test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics

intermediate_lls: 	atsc3_lls.o \
					atsc3_lls_types.o \
					atsc3_lls_sls_parser.o \
					atsc3_lls_alc_utils.o \
					atsc3_aeat_types.o

intermediate_alp: atsc3_alp_types.o atsc3_alp_parser.o

intermediate_baseband: atsc3_baseband_packet_types.o 

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
interemediate_gpl_and_mock_linkage: bento4_gpl_link \
									bento4_mock_link
libatsc3_core: libatsc3.o
listener_ncurses: atsc3_listener_metrics_ncurses

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
			test/atsc3_fdt_mbms_envelope_test \
			test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7 \
			test/atsc3_hevc_nal_extractor_test \
			test/atsc3_pcap_replay_test

libmicrohttpd_tests: test/atsc3_libmicrohttpd_test

# listener_tests: atsc3_alc_listener_test  atsc3_mmt_listener_test atsc3_listener_test atsc3_mmt_listener_test_bento atsc3_listener_metrics_test
listener_tests: atsc3_alc_listener_test \
				atsc3_alc_listener_test_valgrind \
				atsc3_lls_listener_test \
				atsc3_mmt_listener_test \
				atsc3_mmt_listener_test_bento \
				atsc3_mmt_sls_listener_test  \
				atsc3_stltp_listener_test \
				atsc3_stltp_alp_listener_test \
				atsc3_stltp_alp_listener_reflector_test \
				atsc3_mmt_context_mfu_depacketizer_test \
				atsc3_mmt_context_stpp_depacketizer_test

a324_tests: a32_dstp_lls_aeat_emission_test

# real libatsc3 tools for lls, sls, mmt/route and flow analysis
tools: atsc3_listener_metrics_ncurses \
	atsc3_listener_metrics_ncurses_httpd_isobmff \
	atsc3_mmt_mfu_monitor \
	atsc3_alc_listener_mde_writer \
	atsc3_mmt_listener_to_http_hls_fmp4

atsc3_sl_phy: atsc3_sl_tlv_demod_type.o	\
	test_sl/atsc3_sl_tlv_parser_to_alp_test \
	test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test \
	test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics

# intermediate object generation for linking into libatsc3.o

xml.o: xml.c xml.h
	cc -g -c xml.c 

strnstr.o: strnstr.c strnstr.h
	cc -g -c strnstr.c

atsc3_alp_types.o: atsc3_alp_types.h atsc3_alp_types.c
	cc -g -c atsc3_alp_types.c 

atsc3_baseband_packet_types.o: atsc3_baseband_packet_types.h atsc3_baseband_packet_types.c
	cc -g -c atsc3_baseband_packet_types.c 

atsc3_alp_parser.o: atsc3_alp_types.h atsc3_alp_parser.h atsc3_alp_parser.c
	cc -g -c atsc3_alp_parser.c 

atsc3_mbms_envelope_xml.o: atsc3_mbms_envelope_xml.h atsc3_mbms_envelope_xml.c
	cc -g -c atsc3_mbms_envelope_xml.c 

atsc3_mime_multipart_related.o: atsc3_mime_multipart_related.h atsc3_mime_multipart_related.c
	cc -g -c atsc3_mime_multipart_related.c 

atsc3_mime_multipart_related_parser.o: atsc3_mime_multipart_related_parser.h atsc3_mime_multipart_related_parser.c
	cc -g -c atsc3_mime_multipart_related_parser.c 

atsc3_lls.o: atsc3_utils.h atsc3_lls.h atsc3_lls.c
	cc -g -c atsc3_lls.c

atsc3_lls_types.o: atsc3_lls_types.h atsc3_lls_types.c
	cc -g -c atsc3_lls_types.c

atsc3_aeat_types.o: atsc3_aeat_types.h atsc3_aeat_types.c
	gcc -g -c atsc3_aeat_types.c

atsc3_aeat_parser.o: atsc3_aeat_parser.h atsc3_aeat_parser.c
	gcc -g -c atsc3_aeat_parser.c

atsc3_listener_udp.o: atsc3_listener_udp.h atsc3_listener_udp.c
	cc -g -c atsc3_listener_udp.c

atsc3_ip_udp_rtp_parser.o: atsc3_ip_udp_rtp_parser.h atsc3_ip_udp_rtp_parser.c atsc3_ip_udp_rtp_types.h
	cc -g -c atsc3_ip_udp_rtp_parser.c

atsc3_lls_mmt_utils.o: atsc3_lls_mmt_utils.h atsc3_lls_mmt_utils.c
	cc -g -c atsc3_lls_mmt_utils.c

atsc3_lls_alc_utils.o: atsc3_lls_alc_utils.h atsc3_lls_alc_utils.c
	cc -g -c atsc3_lls_alc_utils.c

atsc3_lls_sls_parser.o: atsc3_lls_sls_parser.h atsc3_lls_sls_parser.c
	cc -g -c atsc3_lls_sls_parser.c

atsc3_lls_slt_parser.o: atsc3_lls_slt_parser.h atsc3_lls_slt_parser.c
	cc -g -c atsc3_lls_slt_parser.c

atsc3_lls_sls_monitor_output_buffer.o: atsc3_lls_sls_monitor_output_buffer.h atsc3_lls_sls_monitor_output_buffer.c
	cc -g -c atsc3_lls_sls_monitor_output_buffer.c

atsc3_lls_sls_monitor_output_buffer_utils.o: atsc3_lls_sls_monitor_output_buffer_utils.h atsc3_lls_sls_monitor_output_buffer_utils.c
	cc -g -c atsc3_lls_sls_monitor_output_buffer_utils.c

atsc3_mmtp_packet_types.o: atsc3_mmtp_packet_types.h atsc3_mmtp_packet_types.c
	cc -g -c atsc3_mmtp_packet_types.c

atsc3_mmtp_packet_utils.o: atsc3_mmtp_packet_utils.h atsc3_mmtp_packet_utils.c
	cc -g -c atsc3_mmtp_packet_utils.c

atsc3_mmtp_parser.o: atsc3_mmtp_packet_types.h atsc3_mmtp_parser.h atsc3_mmtp_parser.c 
	cc -g -c atsc3_mmtp_parser.c

atsc3_mmt_mpu_parser.o: atsc3_mmtp_packet_types.h atsc3_mmtp_parser.h atsc3_mmt_mpu_parser.c
	cc -g -c atsc3_mmt_mpu_parser.c

atsc3_mmt_mpu_sample_format_parser.o: atsc3_mmt_mpu_sample_format_type.h atsc3_mmt_mpu_sample_format_parser.h atsc3_mmt_mpu_sample_format_parser.c
	cc -g -c atsc3_mmt_mpu_sample_format_parser.c

atsc3_mmt_signalling_message_type.o:atsc3_mmt_signalling_message_type.h atsc3_mmt_signalling_message_type.c
	cc -g -c atsc3_mmt_signalling_message_type.c

atsc3_mmt_signalling_message.o:atsc3_mmt_signalling_message_types.h atsc3_mmt_signalling_message.h atsc3_mmt_signalling_message.c 
	cc -g -c atsc3_mmt_signalling_message.c

atsc3_mmtp_ntp32_to_pts.o: atsc3_mmtp_ntp32_to_pts.c atsc3_mmtp_ntp32_to_pts.h
	cc -g -c atsc3_mmtp_ntp32_to_pts.c

atsc3_mmt_mpu_utils.o: atsc3_mmt_mpu_utils.h atsc3_mmt_mpu_utils.c
	cc -g -c atsc3_mmt_mpu_utils.c

atsc3_mmt_context_mfu_depacketizer.o: atsc3_mmt_context_mfu_depacketizer.h \
		atsc3_mmt_context_mpu_depacketizer.h \
		atsc3_mmt_context_signalling_information_depacketizer.h \
		atsc3_mmt_context_mfu_depacketizer.c
	cc -g -c atsc3_mmt_context_mfu_depacketizer.c

atsc3_utils.o: atsc3_utils.h atsc3_utils.c  
	cc -g -c atsc3_utils.c

fixups_timespec_get.o:  fixups.h fixups_timespec_get.c
	cc -g -c fixups_timespec_get.c

atsc3_alc_rx.o:  atsc3_alc_rx.h atsc3_alc_rx.c atsc3_lct_hdr.h
	cc -g -c atsc3_alc_rx.c

atsc3_alc_session.o: atsc3_alc_session.h atsc3_alc_session.c
	cc -g -c atsc3_alc_session.c

atsc3_alc_utils.o: atsc3_alc_utils.h atsc3_alc_utils.c
	cc -g -c atsc3_alc_utils.c -o atsc3_alc_utils.o

atsc3_gzip.o: atsc3_gzip.h atsc3_gzip.c
	cc -g -c atsc3_gzip.c -o atsc3_gzip.o

atsc3_player_ffplay.o: atsc3_player_ffplay.h atsc3_player_ffplay.c
	cc -g -c atsc3_player_ffplay.c

atsc3_logging_externs.o: atsc3_logging_externs.c atsc3_logging_externs.h
	cc -g -c atsc3_logging_externs.c

atsc3_mmt_reconstitution_from_media_sample.o: atsc3_mmt_reconstitution_from_media_sample.c atsc3_mmt_reconstitution_from_media_sample.h
	cc -g -c atsc3_mmt_reconstitution_from_media_sample.c

atsc3_stltp_types.o: atsc3_stltp_types.h atsc3_stltp_types.c
	cc -g -c atsc3_stltp_types.c

atsc3_stltp_parser.o: atsc3_stltp_parser.h atsc3_stltp_parser.c
	cc -g -c atsc3_stltp_parser.c

atsc3_fdt.o: atsc3_fdt.h atsc3_fdt.c atsc3_vector_builder.h
	cc -g -c atsc3_fdt.c  -o atsc3_fdt.o

atsc3_fdt_parser.o: atsc3_fdt.o atsc3_fdt_parser.h atsc3_fdt_parser.c
	cc -g -c atsc3_fdt_parser.c -o atsc3_fdt_parser.o

atsc3_route_usbd.o: atsc3_route_usbd.h atsc3_route_usbd.c
	cc -g -c atsc3_route_usbd.c -o atsc3_route_usbd.o

atsc3_sls_held_fragment.o: atsc3_sls_held_fragment.h atsc3_sls_held_fragment.c
	cc -g -c atsc3_sls_held_fragment.c -o atsc3_sls_held_fragment.o	

atsc3_route_mpd.o: atsc3_route_mpd.h atsc3_route_mpd.c
	cc -g -c atsc3_route_mpd.c -o atsc3_route_mpd.o

atsc3_route_s_tsid.o: atsc3_route_s_tsid.h atsc3_route_s_tsid.c
	cc -g -c atsc3_route_s_tsid.c -o atsc3_route_s_tsid.o

atsc3_sls_metadata_fragment_types.o: atsc3_sls_metadata_fragment_types.h atsc3_sls_metadata_fragment_types.c
	cc -g -c atsc3_sls_metadata_fragment_types.c -o atsc3_sls_metadata_fragment_types.o

atsc3_sls_metadata_fragment_types_parser.o: atsc3_sls_metadata_fragment_types_parser.h atsc3_sls_metadata_fragment_types_parser.c
	cc -g -c atsc3_sls_metadata_fragment_types_parser.c -o atsc3_sls_metadata_fragment_types_parser.o

atsc3_mbms_envelope_parser.o: atsc3_mbms_envelope_parser.h atsc3_mbms_envelope_parser.c
	cc -g -c atsc3_mbms_envelope_parser.c -o atsc3_mbms_envelope_parser.o

atsc3_route_sls_processor.o: atsc3_route_sls_processor.h atsc3_route_sls_processor.c
	cc -g -c atsc3_route_sls_processor.c -o atsc3_route_sls_processor.o

atsc3_dstp_types.o: atsc3_dstp_types.h atsc3_dstp_types.c
	cc -g -c atsc3_dstp_types.c -o atsc3_dstp_types.o

atsc3_a344_receiver_query_api.o: atsc3_a344_receiver_query_api.h atsc3_a344_receiver_query_api.c
	cc -g -c atsc3_a344_receiver_query_api.c -o atsc3_a344_receiver_query_api.o	

atsc3_packet_statistics_types.o: atsc3_packet_statistics.h atsc3_packet_statistics_types.c
	cc -g -c atsc3_packet_statistics_types.c -o atsc3_packet_statistics_types.o	

atsc3_hevc_nal_extractor.o: atsc3_hevc_nal_extractor.h atsc3_hevc_nal_extractor.c
	cc -g -c atsc3_hevc_nal_extractor.c -o atsc3_hevc_nal_extractor.o

atsc3_pcap_type.o: atsc3_pcap_type.h atsc3_pcap_type.c
	cc -g -c atsc3_pcap_type.c -o atsc3_pcap_type.o

# unit standalone tests with mock data

atsc3_stltp_parser_test: atsc3_stltp_parser_test.c \
	atsc3_stltp_types.o \
	atsc3_stltp_parser.o \
	atsc3_listener_udp.o \
	atsc3_ip_udp_rtp_parser.o \
	atsc3_utils.o
	cc -g atsc3_stltp_parser_test.c \
	atsc3_stltp_types.o \
	atsc3_stltp_parser.o \
	atsc3_ip_udp_rtp_parser.o \
	atsc3_listener_udp.o \
	atsc3_utils.o \
	-o atsc3_stltp_parser_test

# stats modules

atsc3_bandwidth_statistics.o: atsc3_bandwidth_statistics.h atsc3_bandwidth_statistics.c
	cc -g -c atsc3_bandwidth_statistics.c

atsc3_packet_statistics.o: atsc3_packet_statistics.h atsc3_packet_statistics.c
	cc -g -c atsc3_packet_statistics.c

# -L ../Collections-C/build/src
test_collection: test_collection.c
	cc test_collection.c -I ../Collections-C/src/include/ ../Collections-C/build/src/libcollectc.a -L ../Collections-C/build/src -o test_collection 

#build a compile time-linked WITHOUT bento4 which should not face license compliance issues
bento4_mock_link: bento4_mock.o
bento4_mock.o: bento4/ISOBMFFTrackJoiner.h bento4/ISOBMFFTrackJoiner_mock.cpp 
	g++ -g -D __ISOBMFF_LIB=true -o bento4_mock.o -c bento4/ISOBMFFTrackJoiner_mock.cpp -I../bento/include/ 


# core libatsc3 library gen
# the gpl bento4 library is NOT is not linked in, it by default will not be included 

# only link in atsc3_logging_externs.o if you are using ncurses

libatsc3_intermediate.o: atsc3_lls.o \
		atsc3_lls_types.o \
		atsc3_lls_slt_parser.o \
		atsc3_lls_sls_parser.o \
		atsc3_alp_types.o \
		atsc3_alp_parser.o \
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
		atsc3_pcap_type.o

	ld  -o libatsc3_intermediate.o -r \
		atsc3_lls.o \
		atsc3_lls_types.o \
		atsc3_lls_slt_parser.o \
		atsc3_lls_sls_parser.o \
		atsc3_alp_types.o \
		atsc3_alp_parser.o \
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
		atsc3_pcap_type.o

libatsc3.o: libatsc3_intermediate.o bento4_mock.o
	ld  -o libatsc3.o -r libatsc3_intermediate.o bento4_mock.o

# MIT-compatable linkage (no bento4) by default...
atsc3_isobmff_tools.o: atsc3_isobmff_tools.h atsc3_isobmff_tools.cpp
	g++ -g -o atsc3_isobmff_tools.o -c atsc3_isobmff_tools.cpp -I../bento/include/

# GPL-compatable linkage (w/ bento4)...
atsc3_isobmff_tools_gpl.o: atsc3_isobmff_tools.h atsc3_isobmff_tools.cpp
	g++ -D__ENABLE_GPL_BENTO4__ -g -o atsc3_isobmff_tools_gpl.o -c atsc3_isobmff_tools.cpp -I../bento/include/


# bento4 included library linkage
bento4_gpl_link: bento4_gpl.o 
bento4_gpl.o: bento4/ISOBMFFTrackJoiner.h bento4/ISOBMFFTrackJoiner.cpp
	g++ -g -D __ISOBMFF_LIB=true -o bento4_gpl.o -c bento4/ISOBMFFTrackJoiner.cpp -I../bento/include/ 

libatsc3_bento4_gpl.o: libatsc3_intermediate.o bento4_gpl.o
	ld  -o libatsc3_bento4_gpl.o -r libatsc3_intermediate.o bento4_gpl.o -L../bento/lib -lap4
#-lm -lpthread -lz

###  unit test generation - drivers are in test/

test/atsc3_lmt_test: test/atsc3_lmt_test.c libatsc3.o 
	cc -g test/atsc3_lmt_test.c libatsc3.o -lz -lm -lpthread -lpcap -o test/atsc3_lmt_test 

test/atsc3_lls_test: test/atsc3_lls_test.c libatsc3.o
	cc -g test/atsc3_lls_test.c libatsc3.o -lz  -lm -lpthread -lpcap -o test/atsc3_lls_test

test/atsc3_lls_slt_parser_test: test/atsc3_lls_slt_parser_test.c libatsc3.o 
	cc -g test/atsc3_lls_slt_parser_test.c libatsc3.o -lz -lm -lpthread -lpcap -o test/atsc3_lls_slt_parser_test 

test/atsc3_lls_SystemTime_test: test/atsc3_lls_SystemTime_test.c libatsc3.o
	cc -g test/atsc3_lls_SystemTime_test.c libatsc3.o -lz  -lm -lpthread -lpcap -o test/atsc3_lls_SystemTime_test 

test/atsc3_mmt_signalling_message_test: test/atsc3_mmt_signalling_message_test.c
	cc -g test/atsc3_mmt_signalling_message_test.c libatsc3.o -lz  -lm -lpthread -lpcap -o test/atsc3_mmt_signalling_message_test

test/atsc3_mmt_signalling_packet_test: test/atsc3_mmt_signalling_packet_test.c atsc3_utils.o
	cc -g3 -O0 test/atsc3_mmt_signalling_packet_test.c libatsc3.o -lz -lm -lpthread -lpcap -o test/atsc3_mmt_signalling_packet_test
	echo "valgrind --leak-check=full  ./atsc3_mmt_signalling_packet_test" > test/atsc3_mmt_signalling_packet_test_valgrind
	chmod +x test/atsc3_mmt_signalling_packet_test_valgrind

test/atsc3_mmt_mpu_packet_test: test/atsc3_mmt_mpu_packet_test.c atsc3_utils.o
	cc -g3 -O0 test/atsc3_mmt_mpu_packet_test.c libatsc3.o -lz -lm -lpthread -lpcap -o test/atsc3_mmt_mpu_packet_test
	echo "valgrind --leak-check=full  ./atsc3_mmt_mpu_packet_test" > test/atsc3_mmt_mpu_packet_test_valgrind
	chmod +x test/atsc3_mmt_mpu_packet_test_valgrind

test/atsc3_isobmff_box_test: test/atsc3_isobmff_box_test.c
	cc -g test/atsc3_isobmff_box_test.c  -lz  -lm -lpthread -lpcap -o test/atsc3_isobmff_box_test 

test/atsc3_fdt_test: test/atsc3_fdt_test.c libatsc3.o
	cc -g test/atsc3_fdt_test.c libatsc3.o  -lz  -lm -lpthread -lpcap -o test/atsc3_fdt_test

test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7: test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c libatsc3.o
	cc -g test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7.c libatsc3.o  -lz  -lm -lpthread -lpcap -o test/atsc3_fdt_test_2019-09-07-esg-e.1.1.7

test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1: test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1.c
	cc -g test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1.c libatsc3.o -lz -lm -lpthread -lpcap -o test/atsc3_fdt_mbms_envelope_test_2019-09-17-nab-interop-day1

test/atsc3_hevc_nal_extractor_test: test/atsc3_hevc_nal_extractor_test.c libatsc3.o
	cc -g test/atsc3_hevc_nal_extractor_test.c libatsc3.o -lz -lm -lpthread -lpcap -o test/atsc3_hevc_nal_extractor_test

test/atsc3_libmicrohttpd_test: test/atsc3_libmicrohttpd_test.c 
	cc -g test/atsc3_libmicrohttpd_test.c  \
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
	cc -g test/atsc3_mime_multipart_related_parser_test.c \
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
	cc -g test/atsc3_fdt_mbms_envelope_test.c \
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
	cc -g test/atsc3_fdt_mbms_envelope_test_2019-09-07-esg-e.1.1.7.c \
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
	cc -g test/atsc3_pcap_replay_test.c atsc3_pcap_type.o  atsc3_utils.o -lz  -lm -lpthread -lpcap -o test/atsc3_pcap_replay_test 


### integration tests
### TODO: move these into target makefile in listener_test/folder

# low level signal listener testing
atsc3_lls_listener_test: listener_tests/atsc3_lls_listener_test.cpp libatsc3.o 
	g++ -g listener_tests/atsc3_lls_listener_test.cpp libatsc3.o -lpcap -lz -lpthread -o listener_tests/atsc3_lls_listener_test


# stltp listener test
atsc3_stltp_listener_test: listener_tests/atsc3_stltp_listener_test.cpp libatsc3.o
	g++ -g listener_tests/atsc3_stltp_listener_test.cpp libatsc3.o -lpcap -lz -lpthread -o listener_tests/atsc3_stltp_listener_test


# stltp and alp listener test
atsc3_stltp_alp_listener_test: listener_tests/atsc3_stltp_alp_listener_test.cpp libatsc3.o
	g++ -g listener_tests/atsc3_stltp_alp_listener_test.cpp libatsc3.o -lpcap -lz -lpthread -o listener_tests/atsc3_stltp_alp_listener_test

# jjustman-2020-06-02 - renamed from atsc3_stltp_listener_alp_reflector.cpp to match atsc3_stltp_alp_listener_reflector_test.cpp
atsc3_stltp_alp_listener_reflector_test: listener_tests/atsc3_stltp_alp_listener_reflector_test.cpp libatsc3.o
	g++ -g listener_tests/atsc3_stltp_alp_listener_reflector_test.cpp libatsc3.o -lpcap -lz -lpthread -o listener_tests/atsc3_stltp_alp_listener_reflector_test


atsc3_mmt_sls_listener_test: listener_tests/atsc3_mmt_sls_listener_test.cpp libatsc3.o
	g++ -g listener_tests/atsc3_mmt_sls_listener_test.cpp  \
		libatsc3.o -lpcap -lz -lpthread \
		-o listener_tests/atsc3_mmt_sls_listener_test

# simple MMT listener that builds a single ISOBMFF segment with v/a muxed in for each MPU
atsc3_mmt_listener_test: listener_tests/atsc3_mmt_listener_test.cpp libatsc3_bento4_gpl.o 
	g++ -D __ISOBMFF_LIB=true -g listener_tests/atsc3_mmt_listener_test.cpp \
	libatsc3_bento4_gpl.o \
	-lz -lpcap  -lm -lpthread \
	-o listener_tests/atsc3_mmt_listener_test

# simple ALC/DASH listener that builds a single ISOBMFF segment with v/a muxed in for each MPU
atsc3_alc_listener_test: listener_tests/atsc3_alc_listener_test.cpp libatsc3_bento4_gpl.o
	g++ -g listener_tests/atsc3_alc_listener_test.cpp \
		libatsc3_bento4_gpl.o \
		-lz -lpcap -lpthread  -o listener_tests/atsc3_alc_listener_test

# simple ALC/DASH listener that builds a single ISOBMFF segment with v/a muxed in for each MPU
atsc3_alc_listener_test_valgrind: listener_tests/atsc3_alc_listener_test.cpp libatsc3_bento4_gpl.o
	g++ -D _TEST_RUN_VALGRIND_OSX_=true -g listener_tests/atsc3_alc_listener_test.cpp \
		libatsc3_bento4_gpl.o \
		-lz -lpcap -lpthread  -o listener_tests/atsc3_alc_listener_test_valgrind

# mfu context depacketizer/callback testing

atsc3_mmt_context_mfu_depacketizer_test: listener_tests/atsc3_mmt_context_mfu_depacketizer_test.cpp \
		libatsc3_bento4_gpl.o
	g++ -g listener_tests/atsc3_mmt_context_mfu_depacketizer_test.cpp \
		libatsc3_bento4_gpl.o \
		-lz -lpcap -lpthread  \
		-o listener_tests/atsc3_mmt_context_mfu_depacketizer_test


# stpp context depacketizer/callback testing

atsc3_mmt_context_stpp_depacketizer_test: listener_tests/atsc3_mmt_context_stpp_depacketizer_test.cpp \
		libatsc3_bento4_gpl.o
	g++ -g listener_tests/atsc3_mmt_context_stpp_depacketizer_test.cpp \
		libatsc3_bento4_gpl.o \
		-lz -lpcap -lpthread  \
		-o listener_tests/atsc3_mmt_context_stpp_depacketizer_test

# this one has some ncurses inclusion to support 		
# -D _TEST_RUN_VALGRIND_OSX_=true to disable pthread_create on osx due to issues with lldb startup bug


# a324_tests

a32_dstp_lls_aeat_emission_test: a324_tests/a324_dstp_lls_aeat_emission_test.cpp libatsc3_bento4_gpl.o
	g++ -g a324_tests/a324_dstp_lls_aeat_emission_test.cpp \
		libatsc3_bento4_gpl.o \
		-lz -lpcap -lpthread \
		-o a324_tests/a324_dstp_lls_aeat_emission_test

# misc utils	
# isobmfftrackjoiner: bento4/ISOBMFFTrackJoiner.cpp bento4/ISOBMFFTrackJoiner.h atsc3_utils.o \
#					atsc3_lls_sls_monitor_output_buffer_utils.o atsc3_player_ffplay.o
#	g++ -g bento4/ISOBMFFTrackJoiner.cpp atsc3_utils.o  atsc3_lls_sls_monitor_output_buffer_utils.o \
#		 atsc3_player_ffplay.o -I../bento/include/ -L../bento/lib -lap4 -lm -lpthread -o isobmfftrackjoiner 

# real tools here	
atsc3_listener_metrics_ncurses: tools/atsc3_listener_metrics_ncurses.cpp \
								atsc3_bandwidth_statistics.c \
								atsc3_packet_statistics.c \
								atsc3_output_statistics_ncurses.h \
								atsc3_output_statistics_ncurses.c \
								atsc3_mmt_reconstitution_from_media_sample.o \
								atsc3_logging_externs.o \
								atsc3_isobmff_tools_gpl.o \
								libatsc3_bento4_gpl.o

	g++  -D OUTPUT_STATISTICS=NCURSES -g tools/atsc3_listener_metrics_ncurses.cpp \
		libatsc3_bento4_gpl.o \
		atsc3_isobmff_tools_gpl.o \
		atsc3_output_statistics_ncurses.c \
		atsc3_bandwidth_statistics.c \
		atsc3_packet_statistics.c \
		atsc3_mmt_reconstitution_from_media_sample.c \
		atsc3_logging_externs.o \
		-I../bento/include/ -lpcap  -lncurses -lpcap -lz -lpthread \
		-o tools/atsc3_listener_metrics_ncurses

atsc3_listener_metrics_ncurses_httpd_isobmff: tools/atsc3_listener_metrics_ncurses_httpd_isobmff.cpp \
								atsc3_bandwidth_statistics.c \
								atsc3_packet_statistics.c \
								atsc3_output_statistics_ncurses.h \
								atsc3_output_statistics_ncurses.c  \
								atsc3_mmt_reconstitution_from_media_sample.o \
								atsc3_logging_externs.o \
								atsc3_isobmff_tools_gpl.o \
								libatsc3_bento4_gpl.o

	g++  -D OUTPUT_STATISTICS=NCURSES -g tools/atsc3_listener_metrics_ncurses_httpd_isobmff.cpp \
		libatsc3_bento4_gpl.o \
		atsc3_isobmff_tools_gpl.o \
		atsc3_output_statistics_ncurses.c \
		atsc3_bandwidth_statistics.c \
		atsc3_packet_statistics.c \
		atsc3_mmt_reconstitution_from_media_sample.c \
		atsc3_logging_externs.o \
		-I../bento/include/ -lpcap  -lncurses -lpcap -lz -lpthread \
		-I../libmicrohttpd/libmicrohttpd-0.9.63/build/include \
		-L../libmicrohttpd/libmicrohttpd-0.9.63/build/lib -lmicrohttpd \
		-o tools/atsc3_listener_metrics_ncurses_httpd_isobmff

# atsc3_mmt_mfu_monitor

atsc3_mmt_mfu_monitor: tools/atsc3_mmt_mfu_monitor.cpp  \
						atsc3_bandwidth_statistics.c \
						atsc3_packet_statistics.c \
						atsc3_output_statistics_mfu_ncurses.h \
						atsc3_output_statistics_mfu_ncurses.c \
						atsc3_mmt_reconstitution_from_media_sample.o \
						atsc3_logging_externs.o \
						atsc3_isobmff_tools_gpl.o \
						libatsc3_bento4_gpl.o

	g++  -D OUTPUT_STATISTICS=NCURSES -g tools/atsc3_mmt_mfu_monitor.cpp \
		libatsc3_bento4_gpl.o \
		atsc3_isobmff_tools_gpl.o \
		atsc3_output_statistics_mfu_ncurses.c \
		atsc3_bandwidth_statistics.c \
		atsc3_packet_statistics.c \
		atsc3_mmt_reconstitution_from_media_sample.c \
		atsc3_logging_externs.o \
		-I../bento/include/ -lpcap  -lncurses -lpcap -lz -lpthread \
		-o tools/atsc3_mmt_mfu_monitor

# jjustman-2020-03-26 - malloc/double free debugging
# atsc3_alc_listener_mde_writer: tools/atsc3_alc_listener_mde_writer.cpp	libatsc3_bento4_gpl.o
#	g++  -D OUTPUT_STATISTICS=NCURSES -D__MALLOC_DEBUGGING=1 \
#		-g tools/atsc3_alc_listener_mde_writer.cpp \
#		libatsc3_bento4_gpl.o \
#		-I../bento/include/ -lpcap  -lncurses -lpcap -lz -lpthread -lmcheck\
#		-o tools/atsc3_alc_listener_mde_writer


atsc3_alc_listener_mde_writer: tools/atsc3_alc_listener_mde_writer.cpp	libatsc3_bento4_gpl.o
	g++  -D OUTPUT_STATISTICS=NCURSES \
		-g tools/atsc3_alc_listener_mde_writer.cpp \
		libatsc3_bento4_gpl.o \
		-I../bento/include/ -lpcap  -lncurses -lpcap -lz -lpthread \
		-o tools/atsc3_alc_listener_mde_writer

atsc3_mmt_listener_to_http_hls_fmp4: tools/atsc3_mmt_listener_to_http_hls_fmp4.cpp \
								atsc3_mmt_reconstitution_from_media_sample.o  \
								atsc3_logging_externs.o \
								atsc3_isobmff_tools_gpl.o \
								libatsc3_bento4_gpl.o

	g++  -D OUTPUT_STATISTICS=NCURSES -g tools/atsc3_mmt_listener_to_http_hls_fmp4.cpp \
		libatsc3_bento4_gpl.o \
		atsc3_isobmff_tools_gpl.o \
		atsc3_mmt_reconstitution_from_media_sample.c atsc3_logging_externs.o \
		-I../bento/include/ -lpcap  -lncurses -lpcap -lz -lpthread \
		-I../libmicrohttpd/libmicrohttpd-0.9.63/build/include \
		-L../libmicrohttpd/libmicrohttpd-0.9.63/build/lib -lmicrohttpd \
		-o tools/atsc3_mmt_listener_to_http_hls_fmp4

atsc3_sl_tlv_demod_type.o: atsc3_sl_tlv_demod_type.h atsc3_sl_tlv_demod_type.c
	cc -g -c atsc3_sl_tlv_demod_type.c

test_sl/atsc3_sl_tlv_parser_to_alp_test: 	test_sl/atsc3_sl_tlv_parser_to_alp_test.c \
											atsc3_sl_tlv_demod_type.o \
											libatsc3.o
	cc -g test_sl/atsc3_sl_tlv_parser_to_alp_test.c \
		  atsc3_sl_tlv_demod_type.o \
		  libatsc3.o \
	   -o test_sl/atsc3_sl_tlv_parser_to_alp_test -lz -lm -lpthread -lpcap

test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics:	test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics.c \
														atsc3_sl_tlv_demod_type.o \
														libatsc3.o
	cc -g test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics.c \
	  	atsc3_sl_tlv_demod_type.o \
		libatsc3.o \
		-o test_sl/atsc3_sl_tlv_parser_to_alp_test_with_metrics -lz -lm -lpthread -lpcap

test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test: 	test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test.c \
													atsc3_sl_tlv_demod_type.o \
													libatsc3.o
	cc -g test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test.c \
		  atsc3_sl_tlv_demod_type.o \
		  libatsc3.o \
	   -o test_sl/atsc3_sl_tlv_parser_to_alp_reflector_test -lz -lm -lpthread -lpcap


