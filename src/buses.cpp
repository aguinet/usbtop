#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <pcap/pcap.h>

#include <usbtop/buses.h>

#define USB_DEVICE_START "usbmon"

static size_t g_len_usb_dev_start = 5; // strlen(USB_DEVICE_START

std::map<usbtop::UsbBus::id_type, usbtop::UsbBus*> usbtop::UsbBuses::_buses;
bool usbtop::UsbBuses::_populated = false;

usbtop::UsbBuses::~UsbBuses()
{
	decltype(_buses)::const_iterator it;
	for (it = _buses.begin(); it != _buses.end(); it++) {
		delete it->second;
	}
}

void usbtop::UsbBuses::list(bus_func_t f, const char* filter)
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t* list_devs;
	if (pcap_findalldevs(&list_devs, errbuf) < 0) {
		std::cerr << "Unable to list available devices: " << errbuf << std::endl;
		pcap_freealldevs(list_devs);
		exit(1);
	}

	pcap_if_t* cur_dev = list_devs;
	while (cur_dev) {
		if (cur_dev->name) {
			if ((strlen(cur_dev->name) > g_len_usb_dev_start) &&
				(!filter || (filter && strcmp(cur_dev->name, filter) == 0)) &&
			    (memcmp(cur_dev->name, USB_DEVICE_START, g_len_usb_dev_start) == 0)) {
				size_t bus_id = atoll(&cur_dev->name[g_len_usb_dev_start+1]);
				f(bus_id, cur_dev->name, cur_dev->description);
			}
		}
		cur_dev = cur_dev->next;
	}

	pcap_freealldevs(list_devs);
}

void usbtop::UsbBuses::show(const char* filter)
{
	printf("Name\t\tDescription\n");
	printf("---------------------------\n");
	list([](UsbBus::id_type /*bus_id*/, const char* name, const char* desc)
		{
			printf("%s", name);
			if (desc) {
				printf("\t\t%s", desc);
			}
			printf("\n");
		}, filter);
}

void usbtop::UsbBuses::populate(const char* filter)
{
	if (_populated) {
		return;
	}

	list([](UsbBus::id_type bus_id, const char* name, const char* desc)
		{
			add_bus(bus_id, name, desc);
		}, filter);
	_populated = true;
}

void usbtop::UsbBuses::add_bus(UsbBus::id_type bus_id, const char* name, const char* desc)
{
	_buses.insert(std::make_pair(bus_id, new UsbBus(bus_id, name, desc)));
}

usbtop::UsbBus* usbtop::UsbBuses::get_bus(UsbBus::id_type bus_id)
{
	decltype(_buses)::iterator it = _buses.find(bus_id);
	if (it == _buses.end()) {
		return NULL;
	}

	return it->second;
}

size_t usbtop::UsbBuses::size()
{
	return _buses.size();
}
