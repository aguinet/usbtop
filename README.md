usbtop
======

usbtop is a top-like utility that shows an estimated instantaneous bandwidth on
USB buses and devices.


Project status
--------------

Current stable release is 0.2 (tag release-0.2).

The project is stable and has been tested on debian systems. It should work on all different flavors of Linux though.
Do not hesitate to report any issue you would have while trying to compile and run usbtop.

A ncurses and Qt interfaces with a Windows port are the next steps. Feel free to contribute :)


Usage
-----

Please refer to the ```INSTALL.md``` file for installation instructions.
Then, you need to have the ```usbmon``` module loaded. As root, do :

```
# modprobe usbmon
```

Moreover, on some distributions, usbtop need to be run as root!
