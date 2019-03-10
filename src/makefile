# jjustman-2019-03-08
# 
# non-gpl "compatable" library is built as libatsc3.o
#
# gpl binaries are only built for the test output linkages, e.g. against bento4_gpl.o
#
# requirements: bento4 libary, ncurses libary and headers

all: intermediate_lls intermediate_mmt intermediate_alc intermediate_ffplay \
		interemediate_gpl_and_mock_linkage  \
		libatsc3_core   \
		unit_tests \
		listener_tests \
		tools

clean:
	-rm -f *.o
	-rm -f route/*
	-rm -f mpu/*
intermediate_lls: atsc3_lls.o atsc3_lls_sls_parser.o  atsc3_lls_alc_utils.o

intermediate_alc: atsc3_alc_rx.c alc_session.c fec.c null_fec.c rs_fec.c xor_fec.o atsc3_alc_utils.o  

intermediate_mmt: xml.o atsc3_mmtp_parser.o atsc3_mmt_mpu_parser.o atsc3_mmt_signaling_message.o \
					atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o fixups_timespec_get.o \
					atsc3_bandwidth_statistics.o atsc3_mmt_mpu_utils.o

intermediate_ffplay: atsc3_player_ffplay.o
interemediate_gpl_and_mock_linkage: bento4_gpl_link \
									bento4_mock_link
libatsc3_core: libatsc3.o
listener_ncurses: atsc3_listener_metrics_ncurses

# unit tests are standalone drivers with default or CLI parameters for processing relevant 
# ATSC 3.0 data structs, signalling messages, etc

unit_tests: atsc3_lmt_test atsc3_lls_slt_parser_test atsc3_lls_test \
			atsc3_lls_SystemTime_test atsc3_mmt_signaling_message_test \
			atsc3_isobmff_box_test atsc3_fdt_test

atsc3_mmt_signaling_message_test: atsc3_mmt_signaling_message_test.c libatsc3.o

# listener_tests: atsc3_alc_listener_test  atsc3_mmt_listener_test atsc3_listener_test atsc3_mmt_listener_test_bento atsc3_listener_metrics_test
listener_tests: atsc3_alc_listener_test atsc3_lls_listener_test atsc3_mmt_listener_test \
					atsc3_mmt_listener_test_bento atsc3_mmt_sls_listener_test 

# real libatsc3 tools for lls, sls, mmt/route and flow analysis
tools: atsc3_listener_metrics_ncurses

# intermediate object generation for linking into libatsc3.o

xml.o: xml.c xml.h
	cc -g -c xml.c 

atsc3_lls.o: atsc3_utils.h atsc3_lls.h atsc3_lls.c
	cc -g -c atsc3_lls.c

atsc3_listener_udp.o: atsc3_listener_udp.h atsc3_listener_udp.c
	cc -g -c atsc3_listener_udp.c

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

atsc3_mmtp_parser.o: atsc3_mmtp_types.h atsc3_mmtp_parser.h atsc3_mmtp_parser.c 
	cc -g -c atsc3_mmtp_parser.c

atsc3_mmt_mpu_parser.o: atsc3_mmtp_types.h atsc3_mmtp_parser.h atsc3_mmt_mpu_parser.c
	cc -g -c atsc3_mmt_mpu_parser.c

atsc3_mmt_signaling_message.o: atsc3_mmt_signaling_message.c atsc3_mmt_signaling_message.h
	cc -g -c atsc3_mmt_signaling_message.c

atsc3_mmtp_ntp32_to_pts.o: atsc3_mmtp_ntp32_to_pts.c atsc3_mmtp_ntp32_to_pts.h
	cc -g -c atsc3_mmtp_ntp32_to_pts.c

atsc3_mmt_mpu_utils.o: atsc3_mmt_mpu_utils.h atsc3_mmt_mpu_utils.c
	cc -g -c atsc3_mmt_mpu_utils.c

atsc3_utils.o: atsc3_utils.h atsc3_utils.c  
	cc -g -c atsc3_utils.c

fixups_timespec_get.o:  fixups.h fixups_timespec_get.c
	cc -g -c fixups_timespec_get.c

atsc3_alc_rx.o:  atsc3_alc_rx.h atsc3_alc_rx.c atsc3_lct_hdr.h
	cc -g -c atsc3_alc_rx.c

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

atsc3_isobmff_tools.o: atsc3_isobmff_tools.h atsc3_isobmff_tools.cpp
	g++ -g -o atsc3_isobmff_tools.o -c atsc3_isobmff_tools.cpp -I../bento/include/

# stats modules

atsc3_bandwidth_statistics.o: atsc3_bandwidth_statistics.h atsc3_bandwidth_statistics.c
	cc -g -c atsc3_bandwidth_statistics.c

atsc3_packet_statistics.o: atsc3_packet_statistics.h atsc3_packet_statistics.c
	cc -g -c atsc3_packet_statistics.c

#build a compile time-linked WITHOUT bento4 which should not face license compliance issues
bento4_mock_link: bento4_mock.o
bento4_mock.o: bento4/ISOBMFFTrackJoiner.h bento4/ISOBMFFTrackJoiner_mock.cpp 
	g++ -g -D __ISOBMFF_LIB=true -o bento4_mock.o -c bento4/ISOBMFFTrackJoiner_mock.cpp -I../bento/include/ 


# core libatsc3 library gen
# the gpl bento4 library is NOT is not linked in, it by default will not be included 

libatsc3_intermediate.o: xml.o atsc3_lls.o atsc3_lls_slt_parser.o atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o alc_channel.o alc_list.o \
		atsc3_alc_rx.o alc_session.o fec.o null_fec.o rs_fec.o xor_fec.o mad.o mad_rlc.o transport.o \
		atsc3_lls_alc_utils.o atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_alc_utils.o \
        atsc3_lls_sls_monitor_output_buffer.o atsc3_lls_sls_monitor_output_buffer_utils.o \
		atsc3_lls_mmt_utils.o  atsc3_listener_udp.o atsc3_gzip.o atsc3_logging_externs.o

	ld  -o libatsc3_intermediate.o -r xml.o atsc3_lls.o atsc3_lls_slt_parser.o  atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o alc_channel.o alc_list.o \
		atsc3_alc_rx.o alc_session.o fec.o null_fec.o rs_fec.o xor_fec.o mad.o mad_rlc.o transport.o atsc3_alc_utils.o \
		atsc3_lls_alc_utils.o atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_lls_sls_monitor_output_buffer.o atsc3_lls_sls_monitor_output_buffer_utils.o \
		atsc3_lls_mmt_utils.o atsc3_listener_udp.o  atsc3_logging_externs.o atsc3_gzip.o

libatsc3.o: libatsc3_intermediate.o bento4_mock.o
	ld  -o libatsc3.o -r libatsc3_intermediate.o bento4_mock.o

# bento4 included library linkage
bento4_gpl_link: bento4_gpl.o 
bento4_gpl.o: bento4/ISOBMFFTrackJoiner.h bento4/ISOBMFFTrackJoiner.cpp
	g++ -g -D __ISOBMFF_LIB=true -o bento4_gpl.o -c bento4/ISOBMFFTrackJoiner.cpp -I../bento/include/ 

libatsc3_bento4_gpl.o: libatsc3_intermediate.o bento4_gpl.o
	ld  -o libatsc3_bento4_gpl.o -r libatsc3_intermediate.o bento4_gpl.o -L../bento/lib -lap4
#-lm -lpthread -lz

###  unit test generation

atsc3_lmt_test: atsc3_lmt_test.c libatsc3.o 
	cc -g atsc3_lmt_test.c libatsc3.o -lz -lm -lpthread -o atsc3_lmt_test

atsc3_lls_test: atsc3_lls_test.c libatsc3.o
	cc -g atsc3_lls_test.c libatsc3.o -lz  -lm -lpthread -o atsc3_lls_test

atsc3_lls_slt_parser_test: atsc3_lls_slt_parser_test.c libatsc3.o 
	cc -g atsc3_lls_slt_parser_test.c libatsc3.o -lz  -lm -lpthread -o atsc3_lls_slt_parser_test 

atsc3_lls_SystemTime_test: atsc3_lls_SystemTime_test.c libatsc3.o
	cc -g atsc3_lls_SystemTime_test.c libatsc3.o -lz  -lm -lpthread -o atsc3_lls_SystemTime_test 

atsc3_mmt_signaling_message_test: atsc3_mmt_signaling_message_test.c
	cc -g atsc3_mmt_signaling_message_test.c libatsc3.o -lz  -lm -lpthread  -o atsc3_mmt_signaling_message_test

atsc3_isobmff_box_test: atsc3_isobmff_box_test.c
	cc -g atsc3_isobmff_box_test.c  -lz  -lm -lpthread -o atsc3_isobmff_box_test 

atsc3_fdt_test: atsc3_fdt_test.c libatsc3.o
	cc -g atsc3_fdt_test.c libatsc3.o  -lz  -lm -lpthread -o atsc3_fdt_test

### integration tests
### TODO: move these into target makefile in listener_test/folder

# low level signal listener testing
atsc3_lls_listener_test: listener_tests/atsc3_lls_listener_test.cpp libatsc3.o 
	g++ -g listener_tests/atsc3_lls_listener_test.cpp libatsc3.o -lpcap -lz -lpthread -o listener_tests/atsc3_lls_listener_test

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

# interim listener testing for new features like sls and bento4
#atsc3_mmt_sls_listener_test: atsc3_mmt_sls_listener_test.c libatsc3_bento4_gpl.o \
#	cc -g atsc3_mmt_sls_listener_test.c  \
#		libatsc3_bento4_gpl.o \
#		-lpcap \
#		-o listener_tests/atsc3_mmt_sls_listener_test
#
#atsc3_mmt_listener_test_bento: atsc3_mmt_listener_test_bento.c libatsc3_bento4_gpl.o
#	g++ -D __ISOBMFF_LIB=true -g atsc3_mmt_listener_test_bento.c  \
#		atsc3_mmt_reconstitution_from_media_sample.o atsc3_isobmff_tools.o \
#		bento4_gpl.o \
#		-I../bento/include/ -L../bento/lib -lpcap \
#		-o atsc3_mmt_listener_test_bento		
# this one has some ncurses inclusion to support 		
# -D _TEST_RUN_VALGRIND_OSX_=true to disable pthread_create on osx due to issues with lldb startup bug

# misc utils	
# isobmfftrackjoiner: bento4/ISOBMFFTrackJoiner.cpp bento4/ISOBMFFTrackJoiner.h atsc3_utils.o \
#					atsc3_lls_sls_monitor_output_buffer_utils.o atsc3_player_ffplay.o
#	g++ -g bento4/ISOBMFFTrackJoiner.cpp atsc3_utils.o  atsc3_lls_sls_monitor_output_buffer_utils.o \
#		 atsc3_player_ffplay.o -I../bento/include/ -L../bento/lib -lap4 -lm -lpthread -o isobmfftrackjoiner 

# real tools here	
atsc3_listener_metrics_ncurses: tools/atsc3_listener_metrics_ncurses.cpp \
								atsc3_bandwidth_statistics.c \
								atsc3_packet_statistics.c atsc3_output_statistics_ncurses.h \
								atsc3_output_statistics_ncurses.c  atsc3_isobmff_tools.o \
								atsc3_mmt_reconstitution_from_media_sample.o \
								libatsc3_bento4_gpl.o
	g++  -D OUTPUT_STATISTICS=NCURSES -g tools/atsc3_listener_metrics_ncurses.cpp \
		libatsc3_bento4_gpl.o \
		atsc3_output_statistics_ncurses.c \
		atsc3_bandwidth_statistics.c atsc3_packet_statistics.c \
		atsc3_isobmff_tools.o \
		atsc3_mmt_reconstitution_from_media_sample.c \
		-I../bento/include/ -lpcap  -lncurses -lpcap -lz -lpthread \
		-o tools/atsc3_listener_metrics_ncurses
