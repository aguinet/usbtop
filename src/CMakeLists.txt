
add_definitions("-std=c++11 -Wall -g -D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64")

set(SRC_FILES
buses.cpp
console_output.cpp
main.cpp
pcap_compat.cpp
usb_bus.cpp
usb_device.cpp
usb_stats.cpp
tools.cpp
)

add_executable(usbtop ${SRC_FILES})
target_link_libraries(usbtop pcap boost_thread-mt)
