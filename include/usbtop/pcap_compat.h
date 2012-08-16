#ifndef USBTOP_PCAP_COMPAT_H
#define USBTOP_PCAP_COMPAT_H

#include <stdbool.h>

// Define minimum PCAP version requirement
#define MIN_PCAP_MAJOR 1
#define MIN_PCAP_MINOR 1

namespace usbtop {

bool check_pcap_compat();

}

#endif
