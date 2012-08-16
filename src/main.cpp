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
 * 	Neither the name of HPCProject, Serge Guelton nor the names of its
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

#include <iostream>
#include <cstdlib>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <strings.h>
#include <pcap/pcap.h>

#include <usbtop/buses.h>
#include <usbtop/console_output.h>
#include <usbtop/pcap_compat.h>

#include <boost/thread.hpp>

#include <vector>

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

void clean_pcap_live(std::vector<pcap_t*> const& pcap_hs)
{
	for (pcap_t* hs: pcap_hs) {
		pcap_close(hs);
	}
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

	// Populate USB buses
	usbtop::UsbBuses::populate(bus_filter);
	if (usbtop::UsbBuses::size() == 0) {
		std::cerr << "No USB bus can be captured thanks to libpcap. Check your name filter and make sure relevent permissions are set !" << std::endl;
		if (bus_filter) free(bus_filter);
		return 1;
	}

	// Launch pcap filtering
	std::vector<pcap_t*> pcap_hs;
	boost::thread_group threads;
	const size_t nbuses = usbtop::UsbBuses::size();
	pcap_hs.reserve(nbuses);

	usbtop::Stats::init();

	// Open pcap handles and launch a thread per pcap live capture
	usbtop::UsbBuses::list_pop([=,&pcap_hs,&threads](usbtop::UsbBus* bus)
		{
			char errbuf[PCAP_ERRBUF_SIZE];
			pcap_t* hs = pcap_open_live(bus->name().c_str(), BUFSIZ, false, 0, errbuf);
			if (hs == NULL) {
				std::cerr << "Error while capturing traffic from " << bus->name().c_str() << ": " << errbuf << std::endl;
			}
			else {
				pcap_hs.push_back(hs);
				threads.create_thread(boost::bind(pcap_usb_loop, hs, bus));
			}
		}
	);

	// Launch displaying thread
	boost::thread console_th(&usbtop::ConsoleOutput::main);

	// Wait for their end
	console_th.join();
	threads.join_all();

	clean_pcap_live(pcap_hs);

	if (bus_filter) {
		free(bus_filter);
	}

	return 0;
}
