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
		decltype(_buses)::iterator it;
		for (it = _buses.begin(); it != _buses.end(); it++) {
			UsbBus* bus = it->second;
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
