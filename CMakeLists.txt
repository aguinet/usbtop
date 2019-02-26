cmake_minimum_required(VERSION 2.8)
project(usbtop)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# PCAP
find_package(PCAP REQUIRED)

include_directories(include)

add_subdirectory(src)
