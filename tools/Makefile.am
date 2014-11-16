# tools Makefile.am
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT

# The fontem builder is a bit of a hack - it uses autotools, but
# we don't want it to inherit the configuration we're using.

all-local: fontem/src/fontem

fontem/src/fontem: fontem/Makefile fontem/src/Makefile fontem/src/*.[ch]
	$(MAKE) -C fontem

fontem/src/Makefile: fontem/configure

fontem/Makefile: fontem/configure
	cd fontem && ./configure

fontem/configure: fontem/bootstrap fontem/configure.ac fontem/Makefile.am fontem/src/Makefile.am
	cd fontem && ./bootstrap

clean-local:
	[ -f fontem/Makefile ] && $(MAKE) -C fontem clean

distclean-local:
	[ -f fontem/Makefile ] && $(MAKE) -C fontem distclean
