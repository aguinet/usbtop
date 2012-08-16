#include <usbtop/usb_bus.h>
#include <usbtop/usb_device.h>

void usbtop::UsbDevice::push(const double ts, const size_t size, const UsbStats::direction_type dir)
{
	_stats.push(ts, size, dir);
}
