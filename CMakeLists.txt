cmake_minimum_required(VERSION 2.8)
project(usbtop)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

## Libraries

# PCAP
find_package(PCAP REQUIRED)

# Boost thread
find_package(Boost COMPONENTS thread system)

##

include_directories(include ${PCAP_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

add_subdirectory(src)
