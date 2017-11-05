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

#include <usbtop/usb_bus.h>
#include <usbtop/tools.h>

#include <iostream>

usbtop::UsbBus::~UsbBus()
{
	list_devices_t::const_iterator it;
	for (it = _devices.begin(); it != _devices.end(); it++) {
		delete it->second;
	}
}

void usbtop::UsbBus::push(const pcap_pkthdr* h, const u_char* bytes)
{
	// Get the packet size and timestamp
	const size_t psize = h->len;
	const double time = (double)h->ts.tv_sec + ((double)h->ts.tv_usec)/(1000000.0);

	if (psize <= 14) {
		// That's a bad packet, discard it.
		std::cerr << "[bad packet] packet too small on bus " << id() << std::endl;
		return;
	}

	// Get direction. According to wireshark's source
	// (in epan/dissectors/packet-usb.c:2009, source code of release 1.8.0),
	// a packet goes from host to device iif urb_type == URB_SUBMIT ('S')
	const uint8_t urb_type = bytes[8];
	UsbStats::direction_type direction = (UsbStats::direction_type) (urb_type == 'S');

	// Get device and bus id
	const uint8_t device_id = bytes[11];
	const uint16_t bus_id = *((const uint16_t*) &bytes[12]);

	if ((bus_id != id()) && id()) {
		std::cerr << "[bad packet] on bus " << id() << ", captured a packet claimed to be on bus " << bus_id << "." << std::endl;
		return;
	}
	
	//std::cerr << "Packet size: " << psize << std::endl;
	_stats.push(time, psize, direction);
	get_device(device_id).push(time, psize, direction);
}
