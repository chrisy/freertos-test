# root Makefile.am
# vim:set softtabstop=8 shiftwidth=8 tabstop=8 noexpandtab:
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT
#

ACLOCAL_AMFLAGS = -I aclocal

# Doxygen
include $(top_srcdir)/amlocal/doxygen.am

provider_lib = lib
sources = include src
SUBDIRS = $(provider_lib) $(sources)

EXTRA_DIST = README LCOPYING \
	bootstrap do-configure-arm \
	doxygen.cfg uncrustify.cfg

# Make the documentation when installing
$(man3_MANS): doxygen-doc

# Shortcut to make the documentation
doc: doxygen-doc

# Clean up the doc
mostlyclean-local: doxygen-clean

# Uncrustify the source
uncrustify: amlocal/uncrustify.cfg
	@for dir in $(sources) lib/misc lib/stm32 lib/posixio; do \
		find "$(top_srcdir)/$$dir" -type f -name '*.[ch]' -print0 | \
			xargs -0 uncrustify -c $(top_srcdir)/amlocal/uncrustify.cfg --replace --no-backup; \
	done

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

