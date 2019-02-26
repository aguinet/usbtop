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

	UsbDevice(UsbDevice&& o):
		_device_id(o._device_id),
		_stats(std::move(o._stats)),
		_bus(o._bus)
	{ }

  UsbDevice(UsbDevice const&) = delete;

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
