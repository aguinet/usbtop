#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pcap/pcap.h>

#include <usbtop/pcap_compat.h>

bool usbtop::check_pcap_compat()
{
	// This function returns true if the pcap version
	// is newer or equal to MIN_PCAP_MAJOR.MIN_PCAP_MINOR
	
	const char* pcap_ver = pcap_lib_version();

	// String version is:
	// libpcap version X.X.X
	// Parse this thanks to sscanf... (secure here, because only integers are checked)
	uint32_t maj,min,rev;
	if (sscanf(pcap_ver, "libpcap version %u.%u.%u", &maj, &min, &rev) != 3) {
		fprintf(stderr, "Error while checking libpcap version... Exiting.\n");
		exit(1);
	}

	return ((maj >= MIN_PCAP_MAJOR) & (min >= MIN_PCAP_MINOR));
}
