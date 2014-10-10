# root Makefile.am

ACLOCAL_AMFLAGS = -I aclocal

provider_lib = lib
sources = include src
SUBDIRS = $(provider_lib) $(sources)

EXTRA_DIST = bootstrap do-configure-arm

# Uncrustify the source
uncrustify: uncrustify.cfg
	@for dir in $(sources); do \
		find "$(top_srcdir)/$$dir" -type f -name '*.[ch]' -print0 | \
			xargs -0 uncrustify -c $(top_srcdir)/uncrustify.cfg --replace --no-backup; \
	done

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

