include_directories(${Boost_INCLUDE_DIRS} ${PCAP_INCLUDE_DIRS})
add_definitions("-std=c++0x -Wall -D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64")

IF(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
	add_definitions("-g")
else()
	add_definitions("-O3")
endif()

set(SRC_FILES
buses.cpp
console_output.cpp
main.cpp
pcap_compat.cpp
should_stop.cpp
tools.cpp
usb_bus.cpp
usb_device.cpp
usb_stats.cpp
)

add_executable(usbtop ${SRC_FILES})
target_link_libraries(usbtop ${PCAP_LIBRARIES} ${Boost_LIBRARIES})
if(UNIX AND NOT APPLE)
	# Boost.Thread 1.67+ headers reference pthread_condattr_*
	target_link_libraries(usbtop pthread)
endif()

install(TARGETS usbtop DESTINATION sbin)
