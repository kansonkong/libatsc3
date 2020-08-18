/*
 * SRTLiveReceiver.cpp
 *
 *  Created on: Aug 11, 2020
 *      Author: jjustman
 */


/*
 * SRT - Secure, Reliable, Transport
 * Copyright (c) 2018 Haivision Systems Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

// NOTE: This application uses C++11.

// This program uses quite a simple architecture, which is mainly related to
// the way how it's invoked: srt-live-transmit <source> <target> (plus options).
//
// The media for <source> and <target> are filled by abstract classes
// named Source and Target respectively. Most important virtuals to
// be filled by the derived classes are Source::Read and Target::Write.
//
// For SRT please take a look at the SrtCommon class first. This contains
// everything that is needed for creating an SRT medium, that is, making
// a connection as listener, as caller, and as rendezvous. The listener
// and caller modes are built upon the same philosophy as those for
// BSD/POSIX socket API (bind/listen/accept or connect).
//
// The instance class is selected per details in the URI (usually scheme)
// and then this URI is used to configure the medium object. Medium-specific
// options are specified in the URI: SCHEME://HOST:PORT?opt1=val1&opt2=val2 etc.
//
// Options for connection are set by ConfigurePre and ConfigurePost.
// This is a philosophy that exists also in BSD/POSIX sockets, just not
// officially mentioned:
// - The "PRE" options must be set prior to connecting and can't be altered
//   on a connected socket, however if set on a listening socket, they are
//   derived by accept-ed socket.
// - The "POST" options can be altered any time on a connected socket.
//   They MAY have also some meaning when set prior to connecting; such
//   option is SRTO_RCVSYN, which makes connect/accept call asynchronous.
//   Because of that this option is treated special way in this app.
//
// See 'srt_options' global variable (common/socketoptions.hpp) for a list of
// all options.

// MSVS likes to complain about lots of standard C functions being unsafe.
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

//#define REQUIRE_CXX11 1

#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <cstring>
#include <csignal>
#include <chrono>
#include <thread>
#include <list>


#include "apputil.hpp"  // CreateAddr
#include "uriparser.hpp"  // UriParser
#include "socketoptions.hpp"
#include "logsupport.hpp"
#include "transmitmedia.hpp"
#include "verbose.hpp"



// NOTE: This is without "haisrt/" because it uses an internal path
// to the library. Application using the "installed" library should
// use <srt/srt.h>

#include <srtcore/srt.h>
#include <srtcore/udt.h> // This TEMPORARILY contains extra C++-only SRT API.
#include <srtcore/logging.h>

#include "atsc3_srt_live_receiver.h"
#include "atsc3_srt_target.h"


using namespace std;

atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context_new() {
	atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context = (atsc3_srt_live_receiver_context_t*) calloc(1, sizeof(atsc3_srt_live_receiver_context_t));
	return atsc3_srt_live_receiver_context;
}

atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context_new_with_source_connection_string(const char* source_connection_string) {
	atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context = atsc3_srt_live_receiver_context_new();
	atsc3_srt_live_receiver_context_set_srt_source_connection_string(atsc3_srt_live_receiver_context, source_connection_string);
	return atsc3_srt_live_receiver_context;
}

void atsc3_srt_live_receiver_context_set_srt_source_connection_string(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context, const char* source_connection_string) {
	if(atsc3_srt_live_receiver_context->source_connection_string) {
		free(atsc3_srt_live_receiver_context->source_connection_string);
	}
	atsc3_srt_live_receiver_context->source_connection_string = strdup(source_connection_string);
}
void atsc3_srt_live_receiver_context_set_rx_udp_packet_process_callback_with_context(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context, atsc3_srt_live_rx_udp_packet_process_callback_with_context_f atsc3_srt_live_rx_udp_packet_process_callback_with_context, void* context) {
	atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_with_context = atsc3_srt_live_rx_udp_packet_process_callback_with_context;
	atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_context = context;
}

void atsc3_srt_live_receiver_context_free(atsc3_srt_live_receiver_context_t** atsc3_srt_live_receiver_context_p) {
	if(atsc3_srt_live_receiver_context_p) {
		atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context = *atsc3_srt_live_receiver_context_p;
		if(atsc3_srt_live_receiver_context) {

			//cleanup
			if(atsc3_srt_live_receiver_context->source_connection_string) {
				freeclean((void**)&atsc3_srt_live_receiver_context->source_connection_string);
			}

			atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback = NULL;
			atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_context = NULL;
			atsc3_srt_live_receiver_context->atsc3_srt_live_rx_udp_packet_process_callback_with_context = NULL;

			atsc3_srt_live_receiver_context = NULL;

		}
		*atsc3_srt_live_receiver_context_p = NULL;
	}
}



struct ForcedExit: public std::runtime_error
{
    ForcedExit(const std::string& arg):
        std::runtime_error(arg)
    {
    }
};

struct AlarmExit: public std::runtime_error
{
    AlarmExit(const std::string& arg):
        std::runtime_error(arg)
    {
    }
};

volatile bool int_state = false;
volatile bool timer_state = false;


void OnINT_ForceExit(int)
{
    Verb() << "\n-------- REQUESTED INTERRUPT!\n";
    int_state = true;
}

void OnAlarm_Interrupt(int)
{
    Verb() << "\n---------- INTERRUPT ON TIMEOUT!\n";

    int_state = false; // JIC
    timer_state = true;

    if ((false))
    {
        throw AlarmExit("Watchdog bites hangup");
    }
}

extern "C" void TestLogHandler(void* opaque, int level, const char* file, int line, const char* area, const char* message);



struct LiveTransmitConfig
{
    int timeout = 0;
    int timeout_mode = 0;
    int chunk_size = -1;
    bool quiet = false;
    srt_logging::LogLevel::type loglevel = srt_logging::LogLevel::error;
    set<srt_logging::LogFA> logfas;
    bool log_internal;
    string logfile;
    int bw_report = 0;
    bool srctime = false;
    size_t buffering = 10;
    int stats_report = 0;
    string stats_out;
    SrtStatsPrintFormat stats_pf = SRTSTATS_PROFMAT_2COLS;
    bool auto_reconnect = true;
    bool full_stats = false;

    string source;
    string target;
};


int parse_args(LiveTransmitConfig &cfg)
{
    const OptionName
        o_timeout       = { "t", "to", "timeout" },
        o_timeout_mode  = { "tm", "timeout-mode" },
        o_autorecon     = { "a", "auto", "autoreconnect" },
        o_chunk         = { "c", "chunk" },
        o_bwreport      = { "r", "bwreport", "report", "bandwidth-report", "bitrate-report" },
        o_srctime       = {"st", "srctime", "sourcetime"},
        o_buffering     = {"buffering"},
        o_statsrep      = { "s", "stats", "stats-report-frequency" },
        o_statsout      = { "statsout" },
        o_statspf       = { "pf", "statspf" },
        o_statsfull     = { "f", "fullstats" },
        o_loglevel      = { "ll", "loglevel" },
        o_logfa         = { "logfa" },
        o_log_internal  = { "loginternal"},
        o_logfile       = { "logfile" },
        o_quiet         = { "q", "quiet" },
		o_verbose       = { "v", "verbose" },
        o_help          = { "h", "help" },
        o_version       = { "version" };

    const vector<OptionScheme> optargs = {
        { o_timeout,      OptionScheme::ARG_ONE },
        { o_timeout_mode, OptionScheme::ARG_ONE },
        { o_autorecon,    OptionScheme::ARG_ONE },
        { o_chunk,        OptionScheme::ARG_ONE },
        { o_bwreport,     OptionScheme::ARG_ONE },
        { o_srctime,      OptionScheme::ARG_ONE },
        { o_buffering,    OptionScheme::ARG_ONE },
        { o_statsrep,     OptionScheme::ARG_ONE },
        { o_statsout,     OptionScheme::ARG_ONE },
        { o_statspf,      OptionScheme::ARG_ONE },
        { o_statsfull,    OptionScheme::ARG_NONE },
        { o_loglevel,     OptionScheme::ARG_ONE },
        { o_logfa,        OptionScheme::ARG_ONE },
        { o_log_internal, OptionScheme::ARG_NONE },
        { o_logfile,      OptionScheme::ARG_ONE },
        { o_quiet,        OptionScheme::ARG_NONE },
        { o_verbose,      OptionScheme::ARG_NONE },
        { o_help,         OptionScheme::ARG_NONE },
        { o_version,      OptionScheme::ARG_NONE }
    };

    options_t params;

    cfg.timeout      = Option<OutNumber>(params, o_timeout);
    cfg.timeout_mode = Option<OutNumber>(params, o_timeout_mode);
    cfg.chunk_size   = Option<OutNumber>(params, "-1", o_chunk);
    cfg.srctime      = Option<OutBool>(params, cfg.srctime, o_srctime);
    const int buffering = Option<OutNumber>(params, "10", o_buffering);
    if (buffering <= 0)
    {
        cerr << "ERROR: Buffering value should be positive. Value provided: " << buffering << "." << endl;
        return 1;
    }
    else
    {
        cfg.buffering = (size_t) buffering;
    }
    cfg.bw_report    = Option<OutNumber>(params, o_bwreport);
    cfg.stats_report = Option<OutNumber>(params, o_statsrep);
    cfg.stats_out    = Option<OutString>(params, o_statsout);
    const string pf  = Option<OutString>(params, "default", o_statspf);
    cfg.stats_pf     = ParsePrintFormat(pf);
    if (cfg.stats_pf == SRTSTATS_PROFMAT_INVALID)
    {
        cfg.stats_pf = SRTSTATS_PROFMAT_2COLS;
        cerr << "ERROR: Unsupported print format: " << pf << " -- fallback to default" << endl;
        return 1;
    }

    cfg.full_stats   = OptionPresent(params, o_statsfull);
    cfg.loglevel     = SrtParseLogLevel(Option<OutString>(params, "error", o_loglevel));
    cfg.logfas       = SrtParseLogFA(Option<OutString>(params, "", o_logfa));
    cfg.log_internal = OptionPresent(params, o_log_internal);
    cfg.logfile      = Option<OutString>(params, o_logfile);
    cfg.quiet        = OptionPresent(params, o_quiet);

    if (OptionPresent(params, o_verbose))
        Verbose::on = !cfg.quiet;

    cfg.auto_reconnect = Option<OutBool>(params, true, o_autorecon);


    return 0;
}


int atsc3_srt_live_receiver_start_in_proc(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context) {

	atsc3_srt_live_receiver_context->is_shutdown = false;

	printf("atsc3srt_live_transmit_startup");

	srt_startup();

    printf("after srt_startup");

    // This is mainly required on Windows to initialize the network system,
    // for a case when the instance would use UDP. SRT does it on its own, independently.
    if (!SysInitializeNetwork())
        throw std::runtime_error("Can't initialize network!");

    printf("after SysInitializeNetwork");

    // Symmetrically, this does a cleanup; put into a local destructor to ensure that
    // it's called regardless of how this function returns.
    struct NetworkCleanup
    {
        ~NetworkCleanup()
        {
            srt_cleanup();
            SysCleanupNetwork();
        }
    } cleanupobj;



    LiveTransmitConfig cfg;

    const int parse_ret = parse_args(cfg);

    cfg.source = string(atsc3_srt_live_receiver_context->source_connection_string);

    Verbose::on = true;
    cfg.quiet = false;

    //
    // Set global config variables
    //
    if (cfg.chunk_size > 0) {
        transmit_chunk_size = cfg.chunk_size;
    }

    transmit_stats_writer = SrtStatsWriterFactory(cfg.stats_pf);
    transmit_bw_report = cfg.bw_report;
    transmit_stats_report = cfg.stats_report;
    transmit_total_stats = cfg.full_stats;

    //
    // Set SRT log levels and functional areas
    //
    srt_setloglevel(cfg.loglevel);
    for (set<srt_logging::LogFA>::iterator i = cfg.logfas.begin(); i != cfg.logfas.end(); ++i)
        srt_addlogfa(*i);

    //
    // SRT log handler
    //
    std::ofstream logfile_stream; // leave unused if not set
    char NAME[] = "SRTLIB";
    if (cfg.log_internal)
    {
        srt_setlogflags(0
            | SRT_LOGF_DISABLE_TIME
            | SRT_LOGF_DISABLE_SEVERITY
            | SRT_LOGF_DISABLE_THREADNAME
            | SRT_LOGF_DISABLE_EOL
        );
        srt_setloghandler(NAME, TestLogHandler);
    }
    else if (!cfg.logfile.empty())
    {
        logfile_stream.open(cfg.logfile.c_str());
        if (!logfile_stream)
        {
            printf("ERROR: Can't open logfile");
            cerr << "ERROR: Can't open '" << cfg.logfile.c_str() << "' for writing - fallback to cerr\n";
        }
        else
        {
            UDT::setlogstream(logfile_stream);
        }
    }


    //
    // SRT stats output
    //
    std::ofstream logfile_stats; // leave unused if not set
    if (cfg.stats_out != "")
    {
        logfile_stats.open(cfg.stats_out.c_str());
        if (!logfile_stats)
        {
            cerr << "ERROR: Can't open '" << cfg.stats_out << "' for writing stats. Fallback to stdout.\n";
            logfile_stats.close();
        }
    }
    else if (cfg.bw_report != 0 || cfg.stats_report != 0)
    {
        g_stats_are_printed_to_stdout = true;
    }

    ostream &out_stats = logfile_stats.is_open() ? logfile_stats : cout;

#ifdef _WIN32

    if (cfg.timeout != 0)
    {
        cerr << "ERROR: The -timeout option (-t) is not implemented on Windows\n";
        return EXIT_FAILURE;
    }

#else
    if (cfg.timeout > 0)
    {
        signal(SIGALRM, OnAlarm_Interrupt);
        if (!cfg.quiet)
            cerr << "TIMEOUT: will interrupt after " << cfg.timeout << "s\n";
        alarm(cfg.timeout);
    }
#endif
    signal(SIGINT, OnINT_ForceExit);
    signal(SIGTERM, OnINT_ForceExit);


    printf("Media path: %s", cfg.source.c_str());

    if (!cfg.quiet)
    {
        cerr << "Media path: '"
            << cfg.source
            << "' --> '"
            << cfg.target
            << "'\n";
    }

    unique_ptr<Source> src;
    bool srcConnected = false;

    //unique_ptr<Base> ptr;

    unique_ptr<Target> tar;
    tar.reset(new Atsc3SRT_Target(atsc3_srt_live_receiver_context));

    //jjustman-2020-08-11-direct handoff to atsc3_stltp_depacketizer
    bool tarConnected = false;

    int pollid = srt_epoll_create();
    if (pollid < 0)
    {

        printf("Can't initialize epoll");
        cerr << "Can't initialize epoll";
        return 1;
    }

    size_t receivedBytes = 0;
    size_t wroteBytes = 0;
    size_t lostBytes = 0;
    size_t lastReportedtLostBytes = 0;
    std::time_t writeErrorLogTimer(std::time(nullptr));

    try {
        // Now loop until broken
        while (!int_state && !timer_state && atsc3_srt_live_receiver_context->should_run)
        {
            if (!src.get())
            {

                printf("before source::create, cfg.source: %s", cfg.source.c_str());
                src = Source::Create(cfg.source);
                printf("after source::create, src is: %p", src.get());

                if (!src.get())
                {
                    printf("Unsupported source type");
                    cerr << "Unsupported source type" << endl;
                    return 1;
                }
                int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
                switch (src->uri.type())
                {
                case UriParser::SRT:
                    printf("before source srt_epoll_add_usock");

                        if (srt_epoll_add_usock(pollid,
                        src->GetSRTSocket(), &events))
                    {
                        cerr << "Failed to add SRT source to poll, "
                            << src->GetSRTSocket() << endl;
                        return 1;
                    }
                    break;
                case UriParser::UDP:
                    if (srt_epoll_add_ssock(pollid,
                        src->GetSysSocket(), &events))
                    {
                        cerr << "Failed to add UDP source to poll, "
                            << src->GetSysSocket() << endl;
                        return 1;
                    }
                    break;
                case UriParser::FILE:
                    if (srt_epoll_add_ssock(pollid,
                        src->GetSysSocket(), &events))
                    {
                        cerr << "Failed to add FILE source to poll, "
                            << src->GetSysSocket() << endl;
                        return 1;
                    }
                    break;
                default:
                    break;
                }

                receivedBytes = 0;
            }

            if (!tar.get())
            {
                tar = Target::Create(cfg.target);
                if (!tar.get())
                {
                    cerr << "Unsupported target type" << endl;
                    return 1;
                }

                // IN because we care for state transitions only
                // OUT - to check the connection state changes
                int events = SRT_EPOLL_IN | SRT_EPOLL_OUT | SRT_EPOLL_ERR;
                switch(tar->uri.type())
                {
                case UriParser::SRT:
                    if (srt_epoll_add_usock(pollid,
                        tar->GetSRTSocket(), &events))
                    {
                        cerr << "Failed to add SRT destination to poll, "
                            << tar->GetSRTSocket() << endl;
                        return 1;
                    }
                    break;
                default:
                    break;
                }

                wroteBytes = 0;
                lostBytes = 0;
                lastReportedtLostBytes = 0;
            }

            int srtrfdslen = 2;
            int srtwfdslen = 2;
            SRTSOCKET srtrwfds[4] = {SRT_INVALID_SOCK, SRT_INVALID_SOCK , SRT_INVALID_SOCK , SRT_INVALID_SOCK };
            int sysrfdslen = 2;
            SYSSOCKET sysrfds[2];
            if (srt_epoll_wait(pollid,
                &srtrwfds[0], &srtrfdslen, &srtrwfds[2], &srtwfdslen,
                100,
                &sysrfds[0], &sysrfdslen, 0, 0) >= 0)
            {
                bool doabort = false;
                for (size_t i = 0; i < sizeof(srtrwfds) / sizeof(SRTSOCKET); i++)
                {
                    SRTSOCKET s = srtrwfds[i];
                    if (s == SRT_INVALID_SOCK)
                        continue;

                    bool issource = false;
                    if (src && src->GetSRTSocket() == s)
                    {
                        issource = true;
                    }
                    else if (tar && tar->GetSRTSocket() != s)
                    {
                        continue;
                    }

                    const char * dirstring = (issource) ? "source" : "target";

                    SRT_SOCKSTATUS status = srt_getsockstate(s);
                    switch (status)
                    {
                    case SRTS_LISTENING:
                    {
                        const bool res = (issource) ?
                            src->AcceptNewClient() : tar->AcceptNewClient();
                        if (!res)
                        {
                            cerr << "Failed to accept SRT connection"
                                << endl;
                            doabort = true;
                            break;
                        }

                        srt_epoll_remove_usock(pollid, s);

                        SRTSOCKET ns = (issource) ?
                            src->GetSRTSocket() : tar->GetSRTSocket();
                        int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
                        if (srt_epoll_add_usock(pollid, ns, &events))
                        {
                            cerr << "Failed to add SRT client to poll, "
                                << ns << endl;
                            doabort = true;
                        }
                        else
                        {
                            if (!cfg.quiet)
                            {
                                cerr << "Accepted SRT "
                                    << dirstring
                                    <<  " connection"
                                    << endl;
                            }
#ifndef _WIN32
                            if (cfg.timeout_mode == 1 && cfg.timeout > 0)
                            {
                                if (!cfg.quiet)
                                    cerr << "TIMEOUT: cancel\n";
                                alarm(0);
                            }
#endif
                            if (issource)
                                srcConnected = true;
                            else
                                tarConnected = true;
                        }
                    }
                    break;
                    case SRTS_BROKEN:
                    case SRTS_NONEXIST:
                    case SRTS_CLOSED:
                    {
                        if (issource)
                        {
                            if (srcConnected)
                            {
                                if (!cfg.quiet)
                                {
                                    cerr << "SRT source disconnected"
                                        << endl;
                                }
                                srcConnected = false;
                            }
                        }
                        else if (tarConnected)
                        {
                            if (!cfg.quiet)
                                cerr << "SRT target disconnected" << endl;
                            tarConnected = false;
                        }

                        if(!cfg.auto_reconnect)
                        {
                            doabort = true;
                        }
                        else
                        {
                            // force re-connection
                            srt_epoll_remove_usock(pollid, s);
                            if (issource)
                                src.reset();
                            else
                                tar.reset();

#ifndef _WIN32
                            if (cfg.timeout_mode == 1 && cfg.timeout > 0)
                            {
                                if (!cfg.quiet)
                                    cerr << "TIMEOUT: will interrupt after " << cfg.timeout << "s\n";
                                alarm(cfg.timeout);
                            }
#endif
                        }
                    }
                    break;
                    case SRTS_CONNECTED:
                    {
                        if (issource)
                        {
                            if (!srcConnected)
                            {
                                if (!cfg.quiet)
                                    cerr << "SRT source connected" << endl;
                                srcConnected = true;
                            }
                        }
                        else if (!tarConnected)
                        {
                            if (!cfg.quiet)
                                cerr << "SRT target connected" << endl;
                            tarConnected = true;
                            if (tar->uri.type() == UriParser::SRT)
                            {
                                const int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
                                // Disable OUT event polling when connected
                                if (srt_epoll_update_usock(pollid,
                                    tar->GetSRTSocket(), &events))
                                {
                                    cerr << "Failed to add SRT destination to poll, "
                                        << tar->GetSRTSocket() << endl;
                                    return 1;
                                }
                            }

#ifndef _WIN32
                            if (cfg.timeout_mode == 1 && cfg.timeout > 0)
                            {
                                if (!cfg.quiet)
                                    cerr << "TIMEOUT: cancel\n";
                                alarm(0);
                            }
#endif
                        }
                    }

                    default:
                    {
                        // No-Op
                    }
                    break;
                    }
                }

                if (doabort)
                {
                    break;
                }

                // read a few chunks at a time in attempt to deplete
                // read buffers as much as possible on each read event
                // note that this implies live streams and does not
                // work for cached/file sources
                std::list<std::shared_ptr<MediaPacket>> dataqueue;
                if (src.get() && src->IsOpen() && (srtrfdslen || sysrfdslen))
                {
                    while (dataqueue.size() < cfg.buffering)
                    {
                        std::shared_ptr<MediaPacket> pkt(new MediaPacket(transmit_chunk_size));
                        const int res = src->Read(transmit_chunk_size, *pkt, out_stats);

                        if (res == SRT_ERROR && src->uri.type() == UriParser::SRT)
                        {
                            if (srt_getlasterror(NULL) == SRT_EASYNCRCV)
                                break;

                            throw std::runtime_error(
                                string("error: recvmsg: ") + string(srt_getlasterror_str())
                            );
                        }

                        if (res == 0 || pkt->payload.empty())
                        {
                            break;
                        }

                        dataqueue.push_back(pkt);
                        receivedBytes += pkt->payload.size();
                    }
                }

                // if there is no target, let the received data be lost
                while (!dataqueue.empty())
                {
                    std::shared_ptr<MediaPacket> pkt = dataqueue.front();
                    if (!tar.get() || !tar->IsOpen())
                    {
                        lostBytes += pkt->payload.size();
                    }
                    else if (!tar->Write(pkt->payload.data(), pkt->payload.size(), cfg.srctime ? pkt->time : 0, out_stats))
                    {
                        lostBytes += pkt->payload.size();
                    }
                    else
                    {
                        wroteBytes += pkt->payload.size();
                    }

                    dataqueue.pop_front();
                }

                if (!cfg.quiet && (lastReportedtLostBytes != lostBytes))
                {
                    std::time_t now(std::time(nullptr));
                    if (std::difftime(now, writeErrorLogTimer) >= 5.0)
                    {
                        cerr << lostBytes << " bytes lost, "
                            << wroteBytes << " bytes sent, "
                            << receivedBytes << " bytes received"
                            << endl;
                        writeErrorLogTimer = now;
                        lastReportedtLostBytes = lostBytes;
                    }
                }
            }
        }
    }
    catch (std::exception& x)
    {
        printf("SRT: ERROR exception: %s", x.what());

        cerr << "ERROR: " << x.what() << endl;
        atsc3_srt_live_receiver_context->is_shutdown = true;
        return 255;
    }

    atsc3_srt_live_receiver_context->is_shutdown = true;
    return 0;
}

void atsc3_srt_live_receiver_notify_shutdown(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context) {
	atsc3_srt_live_receiver_context->should_run = false;
}


bool atsc3_srt_live_receiver_get_is_shutdown(atsc3_srt_live_receiver_context_t* atsc3_srt_live_receiver_context) {
	return atsc3_srt_live_receiver_context->is_shutdown;
}

void TestLogHandler(void* opaque, int level, const char* file, int line, const char* area, const char* message)
{
    char prefix[100] = "";
    if ( opaque )
        strncpy(prefix, (char*)opaque, 99);
    time_t now;
    time(&now);
    char buf[1024];
    struct tm local = SysLocalTime(now);
    size_t pos = strftime(buf, 1024, "[%c ", &local);

#ifdef _MSC_VER
    // That's something weird that happens on Microsoft Visual Studio 2013
    // Trying to keep portability, while every version of MSVS is a different plaform.
    // On MSVS 2015 there's already a standard-compliant snprintf, whereas _snprintf
    // is available on backward compatibility and it doesn't work exactly the same way.
#define snprintf _snprintf
#endif
    snprintf(buf+pos, 1024-pos, "%s:%d(%s)]{%d} %s", file, line, area, level, message);

    cerr << buf << endl;
}
