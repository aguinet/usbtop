cmake_minimum_required(VERSION 2.8)
project(usbtop)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

## Libraries

# PCAP
find_package(PCAP REQUIRED)

# Boost thread
find_package(Boost REQUIRED COMPONENTS thread)

##

include_directories(include)

add_subdirectory(src)
