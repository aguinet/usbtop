#ifndef USBTOP_USB_DEVICE_H
#define USBTOP_USB_DEVICE_H

#include <iostream>
#include <usbtop/usb_stats.h>

namespace usbtop {

class UsbBus;

class UsbDevice
{
	friend class UsbBus;

public:
	typedef uint32_t id_type;

protected:
	UsbDevice(UsbBus const& bus, id_type device_id):
		_device_id(device_id),
		_bus(&bus)
	{ }

public:
	void push(const double ts, const size_t size, const UsbStats::direction_type dir);

public:
	inline id_type device_id() const { return _device_id; }
	inline UsbStats const& stats() const { return _stats; }

private:
	id_type _device_id;
	UsbStats _stats;
	UsbBus const* _bus;
};

}

#endif
