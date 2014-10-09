# root Makefile.am

ACLOCAL_AMFLAGS = -I aclocal

rtos = FreeRTOS
sources = include src
SUBDIRS = $(rtos) $(sources)

EXTRA_DIST = bootstrap do-configure-arm

# Uncrustify the source
uncrustify: uncrustify.cfg
	@for dir in $(sources); do \
		find "$(top_srcdir)/$$dir" -type f -name '*.[ch]' -print0 | xargs -0r uncrustify -c $(top_srcdir)/uncrustify.cfg --replace --no-backup; \
	done

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

