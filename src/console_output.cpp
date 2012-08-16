#include <usbtop/buses.h>
#include <usbtop/usb_bus.h>
#include <usbtop/usb_device.h>
#include <usbtop/usb_stats.h>
#include <usbtop/console_output.h>

#include <iostream>
#include <iomanip>

#include <unistd.h>

void usbtop::ConsoleOutput::main()
{
	std::cout.precision(2);
	std::cout.setf(std::ios::fixed);
	while (true) {
		usleep(500*1000);
		clear_screen();
		print_stats();
	}
}

void usbtop::ConsoleOutput::clear_screen()
{
	std::cout << "\033[2J\033[1;1H";
}

void usbtop::ConsoleOutput::print_stats()
{
	UsbBuses::list_pop([](UsbBus const* bus)
		{ print_stats_bus(*bus); });
}

void usbtop::ConsoleOutput::print_stats_bus(UsbBus const& bus)
{
	std::cout << "Bus ID " << bus.id() << " (" << bus.desc() << ")"; 
	std::cout << "\tTo device\tFrom device" << std::endl;
	UsbBus::list_devices_t const& devs = bus.devices();
	UsbBus::list_devices_t::const_iterator it;
	for (it = devs.begin(); it != devs.end(); it++) {
		UsbDevice const& dev(*it->second);
		UsbStats const& stats(dev.stats());
		std::cout << "  Device ID " << it->first << " :\t";
		double stats_to = stats.stats_to_device().bw_instant()/1024.0;
		double stats_from = stats.stats_from_device().bw_instant()/1024.0;
		std::cout << "\t\t" << stats_to << " kb/s\t" << stats_from << " kb/s" << std::endl;
	}
}
