RTOS reference implementation
=============================

Dependencies
------------

This project uses the ARM GCC toolchain with the Embedded ABI. It also makes
use of the `newlib` embedded standard library.

On Ubuntu or Debian Linux the build requirements can be met with:

    sudo apt-get install -y make autoconf automake libtool \
        binutils-arm-none-eabi gcc-arm-none-eabi \
        gdb-arm-none-eabi libnewlib-arm-none-eabi

On MacOS, with MacPorts installed, the same can be achieved with:

    sudo port install gmake autoconf automake libtool \
        arm-none-eabi-binutils arm-none-eabi-gcc \
        arm-none-eabi-gdb

If you intend to make the documentation, you will also need the `doxygen` and
`graphviz` packages.


Submodules
----------

This Git repository uses submodules. If you're building from a clone of
the repository (as opposed to from a tarball or similar) then you must first:

    git submodule init
    git submodule update

To later synchronize any changes in the submodules, run these commands:

    git submodule sync
    git submodule update


Building
--------

Once the dependencies are installed, run the script `make-all` to bootstrap
and build everything. The firmware image can be found in the `src` directory,
where `image.elf`, `image.bin` and `image.hex` contain the ELF, plain binary,
and Intel Binhex images.

Documentation
-------------

The inline documentation is not complete, but can be built with `make doc`
(or `gmake doc` if `make` is not GNU make).

License
-------

This file is distributed under the terms of the MIT License.
See the LICENSE file at the top of this tree, or if it is missing a copy can
be found at http://opensource.org/licenses/MIT
