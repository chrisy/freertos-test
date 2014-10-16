# src Makefile.am
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT

linker_script = platform/STM32F103ZE.ld
AM_LDFLAGS = -T $(linker_script)
EXEEXT = .elf

bin_PROGRAMS = image
extra_outputs = image.bin image.hex image.lst

sources = \
	platform/crt0.c \
	platform/sbrk.c \
	platform/freertos.c \
	main.c

ourlibdir = $(top_srcdir)/lib/.libs
libdirs = -L$(ourlibdir)

image_LDADD = $(libdirs) -lrtos -lplatform -lposixio -lmisc -lstm32 -lm
image_SOURCES = $(sources)
EXTRA_image_DEPENDENCIES = $(linker_script) \
	$(ourlibdir)/libplatform.a \
	$(ourlibdir)/librtos.a \
	$(ourlibdir)/libposixio.a \
	$(ourlibdir)/libmisc.a \
	$(ourlibdir)/libstm32.a

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

all-local: $(extra_outputs)

clean-local:
	rm -f $(extra_outputs)
