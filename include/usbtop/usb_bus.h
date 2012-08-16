#ifndef USBTOP_USB_BUS_H
#define USBTOP_USB_BUS_H

#include <usbtop/usb_device.h>
#include <usbtop/usb_stats.h>

#include <cassert>
#include <map>
#include <string>

#include <pcap/pcap.h>

namespace usbtop {

class UsbBus: boost::noncopyable
{
public:
	typedef size_t id_type;
	// AG: cf. buses.h for remarks about using a pointer for UsbDevice
	typedef std::map<UsbDevice::id_type, UsbDevice*> list_devices_t;

public:
	UsbBus(id_type bus_id, const char* name, const char* desc):
		_bus_id(bus_id)
	{
		assert(name);
		_name = name;
		if (desc) {
			_desc = desc;
		}
	}

	~UsbBus();

public:
	void push(const pcap_pkthdr* h, const u_char* bytes);

public:
	inline UsbDevice& get_device(UsbDevice::id_type device_id)
	{
		list_devices_t::iterator it = _devices.find(device_id);
		if (it == _devices.end()) {
			it = _devices.insert(std::make_pair(device_id, new UsbDevice(*this, device_id))).first;
		}
		return *it->second;
	}

public:
	inline std::string const& name() const { return _name; }
	inline std::string const& desc() const { return _desc; }
	inline list_devices_t const& devices() const { return _devices; }
	inline id_type id() const { return _bus_id; }

private:
	list_devices_t _devices;
	id_type _bus_id;
	std::string _name;
	std::string _desc;
	UsbStats _stats;
};

}

#endif
