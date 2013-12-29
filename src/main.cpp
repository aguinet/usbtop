/*
 * Copyright (c) 2012 Adrien Guinet <adrien@guinet.me>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 	Redistributions of source code must retain the above copyright notice, this
 * 	list of conditions and the following disclaimer.
 * 
 * 	Redistributions in binary form must reproduce the above copyright notice,
 * 	this list of conditions and the following disclaimer in the documentation
 * 	and/or other materials provided with the distribution.
 * 
 * 	Neither the name of Adrien Guinet nor the names of its
 * 	contributors may be used to endorse or promote products derived from this
 * 	software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <errno.h>
#include <getopt.h>
#include <iostream>
#include <pcap/pcap.h>
#include <signal.h>
#include <string.h>
#include <strings.h>

#include <usbtop/buses.h>
#include <usbtop/console_output.h>
#include <usbtop/pcap_compat.h>
#include <usbtop/should_stop.h>

#include <boost/thread.hpp>

#include <vector>

struct pcap_bus
{
	pcap_bus() { }
	pcap_bus(pcap_t* const pcap_, usbtop::UsbBus* const bus_):
		pcap(pcap_), bus(bus_)
	{ }

	pcap_t* pcap;
	usbtop::UsbBus* bus;
};
typedef std::vector<pcap_bus> pcap_buses_t;

void usage(char* path)
{

	std::cerr <<  "Usage: " << path << " <options>" << std::endl << std::endl;
	std::cerr << "Options :" << std::endl;
	std::cerr << "---------" << std::endl;

	std::cerr << "\tGeneral options :" << std::endl;
	std::cerr << "\t\t--list" << std::endl;
	std::cerr << "\t\t\tList available USB buses" << std::endl;
	std::cerr << "\t\t--bus busid" << std::endl;
	std::cerr << "\t\t\tWatch only bus busid" << std::endl;
}

void pcap_loop_callback(u_char* user, const struct pcap_pkthdr *h, const u_char* bytes)
{
	usbtop::UsbBus* bus = (usbtop::UsbBus*) (user);
	assert(bus);
	bus->push(h, bytes);
}

void pcap_usb_loop(pcap_t* hs, usbtop::UsbBus* bus)
{
	std::cerr << "Thread launched for bus " << bus->id() << std::endl;
	pcap_loop(hs, 0, pcap_loop_callback, (u_char*) bus);
}

void clean_pcap_live(pcap_buses_t const& pcap_hs)
{
	for (pcap_bus const& pb: pcap_hs) {
		pcap_close(pb.pcap);
	}
}

void pcap_usb_async_loop(pcap_buses_t const& pcap_hs)
{
	fd_set fd_pcaps, fd_pcaps_org;
	FD_ZERO(&fd_pcaps_org);
	int maxfd = -1;
	std::map<int, pcap_bus> map_fd;
	for (pcap_bus const& i: pcap_hs) {
		const int fd = pcap_fileno(i.pcap);
		if (fd > maxfd) {
			maxfd = fd;
		}
		map_fd.emplace(fd, i);
		FD_SET(fd, &fd_pcaps_org);
	}

	struct timeval timeout, timeout_org;
	timeout_org.tv_sec = 0;
	timeout_org.tv_usec = 250*1000;

	while (true) {
		fd_pcaps = fd_pcaps_org;
		timeout = timeout_org;
		int ret = select(maxfd+1, &fd_pcaps, nullptr, nullptr, &timeout);
		if (ret == -1) {
			std::cerr << "Error while select: " << strerror(errno) << std::endl;
			break;
		}
		if (usbtop::ShouldStop::value()) {
			break;
		}
		auto it = map_fd.begin();
		for (; it != map_fd.end(); it++) {
			if (ret == 0) {
				break;
			}
			if (FD_ISSET(it->first, &fd_pcaps)) {
				pcap_bus const& pb = it->second;
				pcap_dispatch(pb.pcap, 0, pcap_loop_callback, (u_char*) pb.bus);
				ret--;
			}
		}
	}
}

void quit_handler(int sig)
{
	usbtop::ShouldStop::stop();
}

int show_list = 0;

int main(int argc, char** argv)
{
	if (!usbtop::check_pcap_compat()) {
		std::cerr << "libpcap version must be >= " << MIN_PCAP_MAJOR << "." << MIN_PCAP_MINOR << std::endl;
		std::cerr << "Current version is: " << pcap_lib_version() << std::endl;
		return 1;
	}
	static struct option long_opts[] = {
		// name, has_arg, *flag, val
		{"list", 0, &show_list, 1},
		{"bus", 1, 0, 'b'},
		{0, 0, 0, 0}
	};

	char* bus_filter = NULL;
	int longindex;
	while (1)
	{
		int opt = getopt_long_only(argc, argv, "", long_opts, &longindex);
		if (opt == -1) break;

		switch (opt)
		{
		case 'b':
			bus_filter = strdup(optarg);
			break;
		case '?':
			std::cerr << "Unrecognized option : " << argv[optind-1] << std::endl;
			usage(argv[0]);
			return 1;
		};
	}

	if (show_list) {
		usbtop::UsbBuses::show(bus_filter);
		if (bus_filter) free(bus_filter);
		return 0;
	}

	usbtop::ShouldStop::init();

	signal(SIGINT, &quit_handler);
	signal(SIGQUIT, &quit_handler);
	signal(SIGKILL, &quit_handler);

	// Populate USB buses
	usbtop::UsbBuses::populate(bus_filter);
	if (usbtop::UsbBuses::size() == 0) {
		std::cerr << "No USB bus can be captured thanks to libpcap. Check your name filter and make sure relevent permissions are set !" << std::endl;
		if (bus_filter) free(bus_filter);
		return 1;
	}

	// Launch pcap filtering
	pcap_buses_t pcap_hs;
	const size_t nbuses = usbtop::UsbBuses::size();
	pcap_hs.reserve(nbuses);

	usbtop::Stats::init();

	// Open pcap handles in non-blocking mode and launch a thread that will process them
	usbtop::UsbBuses::list_pop([=,&pcap_hs](usbtop::UsbBus* bus)
		{
			char errbuf[PCAP_ERRBUF_SIZE];
			pcap_t* hs = pcap_open_live(bus->name().c_str(), BUFSIZ, false, 0, errbuf);
			if (hs == NULL) {
				std::cerr << "Error while capturing traffic from " << bus->name().c_str() << ": " << errbuf << std::endl;
				return;
			}
			if (pcap_setnonblock(hs, 1, errbuf) == 1) {
				std::cerr << "Error while setting capture in non-blocking mode for " << bus->name().c_str() << ": " << errbuf << std::endl;
				return;
			}
			pcap_hs.emplace_back(hs, bus);
		}
	);

	// Launch capturing thread
	boost::thread capturing_th(boost::bind(pcap_usb_async_loop, pcap_hs));

	// Current thread will output on the main console
	usbtop::ConsoleOutput::main();

	// Wait for the capturing thread to finish
	capturing_th.join();

	clean_pcap_live(pcap_hs);

	if (bus_filter) {
		free(bus_filter);
	}

	return 0;
}
