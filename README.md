RTOS reference implementation
=============================

This code base is an implementation of [FreeRTOS](http://www.freertos.org/)
with the aim of being a boilerplate for other embedded projects.
It requires a minimal `libc` such as [newlib](https://sourceware.org/newlib/).

It is based on the STM32F103 high-density MCU, an ARM Cortex-M3 series
of processors.

The code provides bootstrap routines and re-entrant peripheral support. In
particular it provides a lughtweight POSIX I/O interface including default
stdin/out/err file descriptors that are wired to USART1.

Wishlist
--------

* Provide a CLI interface that can be used via serial ports or network sockets.
* TCP/IP network interface and bash out the POSIX socket implementation for it.
* When external RAM is hooked up, probe it to discover size.
* RAM testing.
* Messagebus.
* Filesystem support.

Acknowledgements
----------------

Much inspiration and a lot of the peripheral code came from Partially Stapled:
https://github.com/mtharp/laureline-firmware .

License
-------

This file is distributed under the terms of the MIT License.
See the LICENSE file at the top of this tree, or if it is missing a copy can
be found at http://opensource.org/licenses/MIT
