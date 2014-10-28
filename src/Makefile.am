# src Makefile.am
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT

chip = @chip@
linker_script = platform/$(chip).ld
AM_LDFLAGS = -T $(linker_script) -Wl,-Map=image.map
EXEEXT = .elf

bin_PROGRAMS = image
extra_outputs = image.bin image.map image.hex image.lst image.use

sources = \
	platform/crt0.c \
	platform/sbrk.c \
	platform/freertos.c \
	debug.c \
	main.c

ourlibdir = $(top_srcdir)/lib/.libs
libdirs = -L$(ourlibdir)

image_LDADD = $(libdirs) -lrtos -lplatform -lposixio -lmisc -lcli -lmicrorl -lstm32
image_SOURCES = $(sources)
EXTRA_image_DEPENDENCIES = $(linker_script) \
	$(ourlibdir)/libplatform.a \
	$(ourlibdir)/librtos.a \
	$(ourlibdir)/libposixio.a \
	$(ourlibdir)/libmisc.a \
	$(ourlibdir)/libmicrorl.a \
	$(ourlibdir)/libcli.a \
	$(ourlibdir)/libstm32.a

image.map: image$(EXEEXT)

image.bin: image$(EXEEXT)
	$(OBJCOPY) -j .nvic_vector -j .info -j .text -j .data -j .data_init $< -O binary $@

image.hex: image$(EXEEXT)
	$(OBJCOPY) -j .nvic_vector -j .info -j .text -j .data -j .data_init $< -O ihex $@

image.lst: image$(EXEEXT)
	$(OBJDUMP) -wxdS $< > $@
	@printf "%-14s %12s %6s %6s\n" "Section" "Length (hex)" "(dec)" "KB"
	@$(OBJDUMP) -h image.elf | \
		gawk '/[.](nvic|text|data|bss|info)/{printf("%-14s %12x %6d %6.1f\n", $$2, \
			strtonum("0x"$$3), strtonum("0x"$$3), strtonum("0x"$$3) / 1024)}'

image.use: image.map
	@printf "%5s %8s %s\n" "Size" "Section" "Module" > $@.tmp
	@awk '/^ \.(text|data)  *0x/{print $$3 " " $$1 " " $$4}' $< | \
		sed -e "s/ \/.*[(]/ (/" | xargs printf "%5d %8s %s\n" | \
		sort -rn >> $@.tmp
	@mv $@.tmp $@
	@head -5 $@

all-local: $(extra_outputs)

clean-local:
	rm -f $(extra_outputs)
