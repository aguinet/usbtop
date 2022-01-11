usbtop: installation instructions
=================================

Packages
--------

Under some Linux distributions, usbtop is directly packaged.

To install under Debian-based distributions:

For ubuntu 20.04, 21.04, 21.10 and 22.04.
```
$ sudo apt install usbtop
```

To install under Fedora:

```
$ sudo dnf install usbtop
```


From sources
------------

### Prerequisites:

  * libpcap
  * libboost >= 1.48.0

Under Debian, install these packages :

```
$ sudo apt install libboost-dev libpcap-dev
```

### Compilation instructions:

Create a ``_build`` directory and then use CMake to create the required Makefile's

```
$ cd /path/to/usbtop
$ mkdir _build && cd _build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
```

Then, compile usbtop:

```
$ make
```

And install it to /usr/local/bin (as root) and load kernel module (as root) :

```
# make install && modprobe usbmon
```

There are also more detailed instructions for Ubuntu on this stack overflow
questionn :
https://unix.stackexchange.com/questions/10671/usb-performance-traffic-monitor/489268#489268
(by https://github.com/ElectricRCAircraftGuy).
