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
	// AG: packet timestamp is in the future.. ?
	//const double time = (double)h->ts.tv_sec + ((double)h->ts.tv_sec)/(1000000.0);
	const double time = tools::get_current_timestamp();

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

	if (bus_id != id()) {
		std::cerr << "[bad packet] on bus " << id() << ", captured a packet claimed to be on bus " << bus_id << "." << std::endl;
		return;
	}
	
	_stats.push(time, psize, direction);
	get_device(device_id).push(time, psize, direction);
}
