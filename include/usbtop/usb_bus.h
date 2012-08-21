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

	UsbBus(UsbBus&& o):
		_devices(std::move(o._devices)),
		_bus_id(o._bus_id),
		_name(std::move(o._name)),
		_desc(std::move(o._desc)),
		_stats(std::move(o._stats))
	{ }

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
