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

#include <usbtop/buses.h>
#include <usbtop/usb_bus.h>
#include <usbtop/usb_device.h>
#include <usbtop/usb_stats.h>
#include <usbtop/console_output.h>
#include <usbtop/should_stop.h>

#include <iostream>
#include <iomanip>

#include <unistd.h>

int init = 2;
int lcount=1;
double*** stdBus;

void usbtop::ConsoleOutput::clear_screen() { std::cout << "\x1b[2J"; }

void usbtop::ConsoleOutput::mov_cursor(int x, int y) { std::cout << "\x1b[" << y << ";" << x << "H"; }

void usbtop::ConsoleOutput::hide_cursor() { std::cout << "\x1b[?25l"; }

void usbtop::ConsoleOutput::update_stat(int line,int col,double value){
	char pow[6] = {' ','K','M','G','T','P'};
	int eng = 0;
	while (value > 999){value = value / 1024.0;	eng++;}//while
	std::cout << "\x1b[" << line << "E";
	std::cout << "\x1b[" << col << "G\x1b[1;29m" << std::setw(6) << value << "\x1b[m\x1b["<< col+7 << "G" << pow[eng] << "B/s\x1b[m";
	std::cout << "\x1b[" << line << "F";
}

void usbtop::ConsoleOutput::idle_stat(int line,int col){
	std::cout << "\x1b[" << line << "E";
	std::cout << "\x1b[" << col << "G\x1b[1;30m  -.--\x1b[m" << "\x1b[" << col+7 << "G" << "    ";
	std::cout << "\x1b[" << line << "F";
}

void usbtop::ConsoleOutput::main(){
	stdBus = (double***)malloc(128 * sizeof(double**));
	for (int i = 0; i < 128; i++) {
		stdBus[i] = (double**)malloc(128 * sizeof(double*));
	}
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			stdBus[i][j] = (double*)malloc(3 * sizeof(double));
		}
	}
	clear_screen();
	hide_cursor();
	mov_cursor(1,1);
	std::cout.precision(2);
	std::cout.setf(std::ios::fixed);

	std::cout << "\x1b[1;1HBUS-ID/DEV-ID" << "\x1b[22GTo dev\x1b[42GFrom dev";
	// lcount++;*/

	while (true) {
		usleep(250);
		if (usbtop::ShouldStop::value()) {
			clear_screen();
			std::cout << "\x1b[?25h"<< std::endl;
			mov_cursor(1,1);
			break;
		}
		print_stats();
		if (init > 0 ) init--;
	}
}

void usbtop::ConsoleOutput::print_stats(){
	UsbBuses::list_pop([](UsbBus const* bus){
		print_dev(*bus);
	});
}

void usbtop::ConsoleOutput::print_dev(UsbBus const& bus){
	switch (init) {
		case 0:
			print_stats_bus(bus);
			break;
		case 1:
			for (int i=0 ; i <= lcount; i ++){
				printf("\n");
			}
			printf("\x1b[%iF",lcount);
			mov_cursor(1,1);
			print_dev_tree(bus);
			break;
		case 2:
			init_dev_tree(bus);
			break;
	}
}


void usbtop::ConsoleOutput::init_dev_tree(UsbBus const& bus) {

	UsbBus::list_devices_t const& devs = bus.devices();
	UsbBus::list_devices_t::const_iterator it;

	int busID=bus.id();
	int devID=0;

	stdBus[busID][devID][0]=lcount;
	stdBus[busID][devID][1]=-1;
	stdBus[busID][devID][2]=-1;
	lcount++;
	for (it = devs.begin(); it != devs.end(); it++) {
		devID=it->first;
		stdBus[busID][devID][0]=lcount;
		stdBus[busID][devID][1]=-1;
		stdBus[busID][devID][2]=-1;
		lcount++;
	}//for
}

void usbtop::ConsoleOutput::print_dev_tree(UsbBus const& bus) {

	UsbBus::list_devices_t const& devs = bus.devices();
	UsbBus::list_devices_t::const_iterator it;

	int busID=bus.id();
	int devID=0;
	int line=(int)stdBus[busID][devID][0];

	std::cout << "\x1b[" << line <<"E"<< "\x1b[1;29m#-+ BUS" << busID << "\x1b[1;30m (" << bus.desc() << ")\x1b[m\x1b[" << line << "F";
	for (it = devs.begin(); it != devs.end(); it++) {
		devID = it->first;
		line=(int)stdBus[busID][devID][0];
		std::cout << "\x1b[" << line << "E\x1b[3G\x1b[1;29m|- " << std::setw(3) << devID << " :\x1b[m\x1b[" << line << "F";
		idle_stat(line,20);
		idle_stat(line,40);
	}//for
}

void usbtop::ConsoleOutput::print_stats_bus(UsbBus const& bus){

	UsbBus::list_devices_t const& devs = bus.devices();
	UsbBus::list_devices_t::const_iterator it;

	int busID=bus.id();
	int devID=0;
	int line=(int)stdBus[busID][devID][0];

	for (it = devs.begin(); it != devs.end(); it++) {
		UsbDevice const& dev(*it->second);
		UsbStats const& stats(dev.stats());

		devID = it->first;
		line=(int)stdBus[busID][devID][0];

		double stats_to   = stats.stats_to_device().bw_instant();
		double stats_from = stats.stats_from_device().bw_instant();
		double prev_to    = stdBus[busID][devID][1];
		double prev_from  = stdBus[busID][devID][2];

		stdBus[busID][devID][1]=stats_to;
		stdBus[busID][devID][2]=stats_from;

		if ((stats_to > 0.0)|(prev_to > 0.0))	update_stat(line,20,stats_to);
		if (stats_to == 0.0){
			if (prev_to > -100.0)	stdBus[busID][devID][1]=prev_to-1.0;
			else idle_stat(line,20);
		}
		if ((stats_from > 0.0)|(prev_from > 0.0 ))	update_stat(line,40,stats_from);
		if (stats_from == 0.0){
			if (prev_from > -100.0) stdBus[busID][devID][2]=prev_from-1.0;
			else idle_stat(line,40);
		}
	}
}


