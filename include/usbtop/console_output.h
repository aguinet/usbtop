#ifndef USBTOP_CONSOLE_OUTPUT_H
#define USBTOP_CONSOLE_OUTPUT_H

namespace usbtop {

class UsbBus;

class ConsoleOutput
{
public:
	static void main();

private:
	static void clear_screen();
	static void print_stats();
	static void print_stats_bus(UsbBus const& bus);
};

}

#endif
