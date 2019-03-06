all: intermediate_lls intermediate_mmt intermediate_alc intermediate_ffplay libatsc3_core  isobmfftrackjoiner atsc3_listener_metrics_ncurses listener_tests
#listener_ncurses
#mpu_isobmff_bento # listener_tests unit_tests
clean:
	-rm -f *.o
	-rm -f route/*
	-rm -f mmt/*
	-rm atsc3_mmt_listener_test_bento
	
	
intermediate_lls: atsc3_lls.o atsc3_lls_sls_parser.o  atsc3_lls_alc_utils.o

intermediate_mmt: xml.o atsc3_mmtp_parser.o atsc3_mmt_mpu_parser.o atsc3_mmt_signaling_message.o \
 atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o fixups_timespec_get.o \
 atsc3_bandwidth_statistics.o atsc3_mmt_mpu_utils.o
 
 #atsc3_packet_statistics.o 
 
intermediate_alc: atsc3_alc_rx.c alc_session.c fec.c null_fec.c rs_fec.c xor_fec.c atsc3_alc_utils.c 
intermediate_ffplay: atsc3_player_ffplay.o

libatsc3_core: libatsc3.o

# unit_tests: atsc3_lmt_test atsc3_lls_slt_parser_test atsc3_lls_test atsc3_lls_SystemTime_test atsc3_mmt_signaling_message_test atsc3_isobmff_box_test

# listener_tests: atsc3_alc_listener_test atsc3_lls_listener_test atsc3_mmt_listener_test atsc3_listener_test atsc3_mmt_listener_test_bento
listener_tests: atsc3_lls_listener_test atsc3_mmt_sls_listener_test
# atsc3_listener_metrics_test

listener_ncurses: atsc3_listener_metrics_ncurses


#intermediate_mmt object gen

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


#atsc3_alc_utils_cpp.o: atsc3_alc_utils.h atsc3_alc_utils.c
#	g++ -g -c atsc3_alc_utils.c -o atsc3_alc_utils_cpp.o
		
atsc3_player_ffplay.o: atsc3_player_ffplay.h atsc3_player_ffplay.c
	cc -g -c atsc3_player_ffplay.c

		
# stats modules

#atsc3_bandwidth_statistics.o: atsc3_bandwidth_statistics.h atsc3_bandwidth_statistics.c
#	cc -g -c atsc3_bandwidth_statistics.c

#atsc3_packet_statistics.o: atsc3_packet_statistics.h atsc3_packet_statistics.c atsc3_output_statistics_ncurses.h 
#	cc -g -c atsc3_packet_statistics.c
	
atsc3_isobmff_tools.o: atsc3_isobmff_tools.h atsc3_isobmff_tools.cpp
	g++ -g -o atsc3_isobmff_tools.o -c atsc3_isobmff_tools.cpp -I../bento/include/


ISOBMFFTrackJoiner.o: bento4/ISOBMFFTrackJoiner.cpp bento4/ISOBMFFTrackJoiner.h atsc3_utils.h atsc3_lls_sls_monitor_output_buffer.h 
	g++ -g -D __ISOBMFF_LIB=true -o ISOBMFFTrackJoiner.o -c bento4/ISOBMFFTrackJoiner.cpp -I../bento/include/
	
	
isobmfftrackjoiner: bento4/ISOBMFFTrackJoiner.cpp bento4/ISOBMFFTrackJoiner.h atsc3_utils.o atsc3_lls_sls_monitor_output_buffer_utils.o atsc3_player_ffplay.o
	g++ -g bento4/ISOBMFFTrackJoiner.cpp atsc3_utils.o  atsc3_lls_sls_monitor_output_buffer_utils.o atsc3_player_ffplay.o -I../bento/include/ -L../bento/lib -lap4 -lm -lpthread -o isobmfftrackjoiner 
	
	
#core libatsc3 library gen
# do not link in atsc3_bandwidth_statistics.o atsc3_packet_statistics.o, late compile them for impl's with ncurses
# atsc3_alc_utils.o 
libatsc3.o: xml.o atsc3_lls.o atsc3_lls_slt_parser.o atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o alc_channel.o alc_list.o \
		atsc3_alc_rx.o alc_session.o fec.o null_fec.o rs_fec.o xor_fec.o mad.o mad_rlc.o transport.o \
		atsc3_lls_alc_utils.o atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_alc_utils.o \
        atsc3_lls_sls_monitor_output_buffer.o atsc3_lls_sls_monitor_output_buffer_utils.o \
		atsc3_lls_mmt_utils.o  atsc3_listener_udp.o      
	
	ld  -o libatsc3.o -r xml.o atsc3_lls.o atsc3_lls_slt_parser.o  atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o alc_channel.o alc_list.o \
		atsc3_alc_rx.o alc_session.o fec.o null_fec.o rs_fec.o xor_fec.o mad.o mad_rlc.o transport.o atsc3_alc_utils.o \
		atsc3_lls_alc_utils.o atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_lls_sls_monitor_output_buffer.o atsc3_lls_sls_monitor_output_buffer_utils.o \
		atsc3_lls_mmt_utils.o atsc3_listener_udp.o
		
#libatsc3-nogpp.o: xml.o atsc3_lls.o atsc3_lls_slt_parser.o atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
#		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o alc_channel.o alc_list.o \
#		atsc3_alc_rx.o alc_session.o fec.o null_fec.o rs_fec.o xor_fec.o mad.o mad_rlc.o transport.o \
#		atsc3_lls_alc_utils.o atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_alc_utils.o \
#        atsc3_lls_sls_monitor_output_buffer.o atsc3_lls_sls_monitor_output_buffer_utils.o \
#		atsc3_lls_mmt_utils.o        
#	
#	ld  -o libatsc3-nogpp.o -r xml.o atsc3_lls.o atsc3_lls_slt_parser.o  atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
#		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o alc_channel.o alc_list.o \
#		atsc3_alc_rx.o alc_session.o fec.o null_fec.o rs_fec.o xor_fec.o mad.o mad_rlc.o transport.o atsc3_alc_utils.o \
#		atsc3_lls_alc_utils.o atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_lls_sls_monitor_output_buffer.o atsc3_lls_sls_monitor_output_buffer_utils.o \
#		atsc3_lls_mmt_utils.o
# atsc3_bandwidth_statistics.o
# atsc3_packet_statistics.o
#unit test generation

atsc3_lmt_test: atsc3_lmt_test.c libatsc3.o
	cc -g atsc3_lmt_test.c libatsc3.o -lz -lm -lpthread -o atsc3_lmt_test
			  
atsc3_lls_test: atsc3_lls_test.c libatsc3.o
	cc -g atsc3_lls_test.c libatsc3.o -lz  -lm -lpthread -o atsc3_lls_test
		   		   
atsc3_lls_slt_parser_test: atsc3_lls_slt_parser_test.c libatsc3.o 
	cc -g atsc3_lls_slt_parser_test.c libatsc3.o -lz  -lm -lpthread -o atsc3_lls_slt_parser_test 

atsc3_lls_SystemTime_test: atsc3_lls_SystemTime_test.c libatsc3.o
	cc -g atsc3_lls_SystemTime_test.c libatsc3.o -lz  -lm -lpthread -o atsc3_lls_SystemTime_test 

# atsc3_mmt_signaling_message_test: atsc3_mmt_signaling_message_test.c libatsc3.o

#	cc -g atsc3_mmt_signaling_message_test.c libatsc3.o -lz  -lm -lpthread  -o atsc3_mmt_signaling_message_test


atsc3_isobmff_box_test: atsc3_isobmff_box_test.c
	cc -g atsc3_isobmff_box_test.c -o atsc3_isobmff_box_test 

#integration tests

# atsc3_alc_listener_test: atsc3_alc_listener_test.c libatsc3.o
#	cc -g atsc3_alc_listener_test.c libatsc3.o -lz -lpcap  -lm -lpthread -o atsc3_alc_listener_test

atsc3_lls_listener_test: atsc3_lls_listener_test.c libatsc3.o
	cc -g atsc3_lls_listener_test.c  -lz -lpcap  -lm -lpthread  xml.o atsc3_lls.o atsc3_lls_slt_parser.o  \
		atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o alc_channel.o alc_list.o \
		atsc3_alc_rx.o alc_session.o fec.o null_fec.o rs_fec.o xor_fec.o mad.o mad_rlc.o transport.o  \
		atsc3_lls_alc_utils.o atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_lls_sls_monitor_output_buffer.o  \
		atsc3_lls_mmt_utils.o -o atsc3_lls_listener_test
	
atsc3_mmt_sls_listener_test: atsc3_mmt_sls_listener_test.c xml.o atsc3_lls.o atsc3_lls_slt_parser.o  \
		atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o \
		atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_lls_sls_monitor_output_buffer.o  \
		atsc3_lls_mmt_utils.o atsc3_listener_udp.o
	cc -g atsc3_mmt_sls_listener_test.c  -lz -lpcap  -lm -lpthread  xml.o atsc3_lls.o atsc3_lls_slt_parser.o  \
		atsc3_lls_sls_parser.o atsc3_mmtp_parser.o atsc3_mmtp_ntp32_to_pts.o atsc3_utils.o \
		fixups_timespec_get.o atsc3_mmt_signaling_message.o atsc3_mmt_mpu_parser.o \
		atsc3_mmt_mpu_utils.o atsc3_player_ffplay.o atsc3_lls_sls_monitor_output_buffer.o  \
		atsc3_lls_mmt_utils.o atsc3_listener_udp.o -o atsc3_mmt_sls_listener_test
	
	
#atsc3_mmt_listener_test: atsc3_mmt_listener_test.c libatsc3.o
#	cc -g atsc3_mmt_listener_test.c libatsc3.o -lz -lpcap -lm -lpthread -o atsc3_mmt_listener_test
	
	
#atsc3_listener_test: atsc3_listener_test.c libatsc3.o
#	cc -g atsc3_listener_test.c libatsc3.o -lz -lpcap  -lm -lpthread -o atsc3_listener_test
	
#atsc3_listener_metrics_test: atsc3_lls_alc_tools.h atsc3_listener_metrics_test.c  atsc3_bandwidth_statistics.c atsc3_packet_statistics.c  libatsc3.o
#	cc -g atsc3_listener_metrics_test.c atsc3_bandwidth_statistics.c atsc3_packet_statistics.c libatsc3.o -lz -lpcap -o atsc3_listener_metrics_test
	
	
	
atsc3_listener_metrics_ncurses: atsc3_listener_metrics_ncurses.c atsc3_bandwidth_statistics.c \
								atsc3_packet_statistics.c atsc3_output_statistics_ncurses.h \
								atsc3_output_statistics_ncurses.c libatsc3.o atsc3_isobmff_tools.o \
								ISOBMFFTrackJoiner.o
	g++  -D OUTPUT_STATISTICS=NCURSES -g atsc3_listener_metrics_ncurses.c atsc3_output_statistics_ncurses.c \
							atsc3_bandwidth_statistics.c atsc3_packet_statistics.c \
                            ISOBMFFTrackJoiner.o libatsc3.o atsc3_isobmff_tools.o \
							-I../bento/include/ -L../bento/lib -I../bento/include/ -L../bento/lib -lap4 -lm -lpthread -lz -lpcap  \
							-lncurses -o atsc3_listener_metrics_ncurses
							
# cc -D _TEST_RUN_VALGRIND_OSX_=true -g atsc3_listener_metrics_ncurses.c atsc3_bandwidth_statistics.c atsc3_packet_statistics.c libatsc3.o -lz -lpcap -lncurses -o atsc3_listener_metrics_ncurses


# atsc3_mmt_listener_test_bento: atsc3_mmt_listener_test_bento.c  bento4/MPUtoISOBMFFProcessor.h bento4/MPUtoISOBMFFProcessor.cpp libatsc3.o atsc3_player_ffplay.c
#	g++ -D __ISOBMFF_LIB=true -g atsc3_mmt_listener_test_bento.c  atsc3_isobmff_tools.c bento4/ISOBMFFTrackJoiner.cpp bento4/MPUtoISOBMFFProcessor.cpp  libatsc3.o -I../bento/include/ -L../bento/lib -I../bento/include/ -L../bento/lib -lap4 -lm -lpthread   -lap4 -lz -lpcap -lm -lpthread -o atsc3_mmt_listener_test_bento


# mpu_isobmff_bento:  bento4/MPUProcessor.cpp libatsc3.o
#	g++ -g bento4/MPUProcessor.cpp -I../bento/include/ -L../bento/lib -lap4 -lm -lpthread -o MPUISOBmffProcessor
	
# bento4/MPUProcessor.cpp





