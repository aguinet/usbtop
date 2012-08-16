#ifndef USBTOP_BUSES_H
#define USBTOP_BUSES_H

#include <usbtop/usb_bus.h>

namespace usbtop {

class UsbBuses
{
	typedef void(*bus_func_t)(UsbBus::id_type bus_id, const char* name, const char* desc);

public:
	~UsbBuses();

public:
	static void list(bus_func_t f, const char* filter);

	template <typename F>
	static size_t list_pop(F const& f, const char* filter = NULL)
	{
		size_t nfiltered = 0;
		populate();
		decltype(_buses)::const_iterator it;
		for (it = _buses.begin(); it != _buses.end(); it++) {
			UsbBus* const bus = it->second;
			if (!filter || (filter && bus->name() == filter)) {
				nfiltered++;
				f(bus);
			}
		}
		return nfiltered;
	}
	static void show(const char* filter = NULL);
	static void populate(const char* filter = NULL);
	static size_t size();

public:
	static UsbBus* get_bus(UsbBus::id_type bus_id);

private:
	static void add_bus(UsbBus::id_type bus_id, const char* name, const char* desc);
	static bool _populated;

private:
	// AG: ideally, we shouldn't store a pointer into this std::map,
	// because "emplace" can be use to create non-copyable object into
	// a container.
	// Two issues here: std::map::emplace isn't implemented in libstdc++
	// (as of August 2012), and, even with std::unordered_map, the only
	// supported syntax is :
	// map.emplace(key, UsbBus(...)), which involves a copy of the UsbBus object... (really usefull).
	// The best would be to do:
	// map.emplace(key, args_for_usb_bus)
	static std::map<UsbBus::id_type, UsbBus*> _buses;
};

}

#endif
