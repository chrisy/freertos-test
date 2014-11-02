# lib Makefile.am
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT

lib_LTLIBRARIES = \
	librtos.la \
	libstm32.la \
	libplatform.la \
	libposixio.la \
	libmisc.la \
	libmicrorl.la \
	libcli.la \
	libfonts.la

rtos_sources = \
	FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
	FreeRTOS/Source/portable/MemMang/heap_3.c \
	FreeRTOS/Source/croutine.c \
	FreeRTOS/Source/event_groups.c \
	FreeRTOS/Source/list.c \
	FreeRTOS/Source/queue.c \
	FreeRTOS/Source/tasks.c \
	FreeRTOS/Source/timers.c

stm32_sources = \
	stm32/dma.c \
	stm32/flash.c \
	stm32/i2c.c \
	stm32/iwdg.c \
	stm32/mmc.c \
	stm32/serial.c \
	stm32/spi.c

platform_sources = \
	platform/core_cm3.c \
	platform/system_stm32f10x.c

posixio_sources = \
	posixio/posixio.c \
	posixio/fdio.c \
	posixio/fileio.c \
	posixio/dev/serial.c

misc_sources = \
	misc/crc7.c

microrl_sources = \
	microrl/src/microrl.c

cli_sources = \
	cli/cli.c


#librtos_la_CFLAGS = -Wno-missing-braces -Wno-missing-field-initializers -Wno-sign-compare
librtos_la_SOURCES = $(rtos_sources)
libstm32_la_SOURCES = $(stm32_sources)
libplatform_la_SOURCES = $(platform_sources)
libposixio_la_SOURCES = $(posixio_sources)
libmisc_la_SOURCES = $(misc_sources)
libmicrorl_la_SOURCES = $(microrl_sources)
libcli_la_SOURCES = $(cli_sources)

fontdir := $(top_srcdir)/resources/ttf
fontemdir := $(top_srcdir)/tools/fontem

# A list of all the fonts we want to build.
# # The source .ttf is determined from the name.
fonts_sources :=
fonts_sources += fonts/font-UbuntuMonoB-7.c
fonts_sources += fonts/font-UbuntuMonoB-8.c
fonts_sources += fonts/font-UbuntuMonoB-9.c
fonts_sources += fonts/font-UbuntuMonoB-10.c
fonts_sources += fonts/font-UbuntuMonoB-11.c
fonts_sources += fonts/font-UbuntuMonoB-12.c
fonts_sources += fonts/font-UbuntuMonoB-13.c
fonts_sources += fonts/font-UbuntuMonoB-14.c
fonts_sources += fonts/font-UbuntuMonoB-16.c
fonts_sources += fonts/font-UbuntuMonoB-18.c
fonts_sources += fonts/font-UbuntuMonoB-20.c
fonts_sources += fonts/font-UbuntuMonoB-22.c
fonts_sources += fonts/font-UbuntuMonoB-24.c
fonts_sources += fonts/font-UbuntuMonoB-26.c
fonts_sources += fonts/font-UbuntuMonoB-28.c
fonts_sources += fonts/font-UbuntuMonoB-30.c

fonts_sources += fonts/font-DejaVuSansMono-7.c
fonts_sources += fonts/font-DejaVuSansMono-8.c
fonts_sources += fonts/font-DejaVuSansMono-9.c
fonts_sources += fonts/font-DejaVuSansMono-10.c
fonts_sources += fonts/font-DejaVuSansMono-11.c
fonts_sources += fonts/font-DejaVuSansMono-12.c
fonts_sources += fonts/font-DejaVuSansMono-13.c
fonts_sources += fonts/font-DejaVuSansMono-14.c
fonts_sources += fonts/font-DejaVuSansMono-16.c
fonts_sources += fonts/font-DejaVuSansMono-18.c
fonts_sources += fonts/font-DejaVuSansMono-20.c
fonts_sources += fonts/font-DejaVuSansMono-22.c
fonts_sources += fonts/font-DejaVuSansMono-24.c
fonts_sources += fonts/font-DejaVuSansMono-26.c
fonts_sources += fonts/font-DejaVuSansMono-28.c
fonts_sources += fonts/font-DejaVuSansMono-30.c

BUILT_SOURCES = $(fonts_sources)

fonts/fontem.h: $(fontemdir)/src/resource/fontem.h
	cp -p $< $@

.SECONDEXPANSION:
$(fonts_sources): %.c: $$(fontdir)/$$(word 2,$$(subst -, ,%)).ttf fonts/fontem.h $(fontemdir)/src/fontem
	$(fontemdir)/src/fontem --font=$(fontdir)/$(word 2,$(subst -, ,$(notdir $@))).ttf \
		--dir=fonts --name=$(word 2,$(subst -, ,$(notdir $@))) --size=$(word 3,$(subst -, ,$(subst .c,,$(notdir $@))))

fonts/font_all.h: $(fonts_sources)
	@echo "/* A list of all font headers. */" > $@
	for font in $(notdir $(fonts_sources:%.c=%.h)); do echo "#include \"$$font\"" >> $@; done

clean-local:
	rm -f $(fonts_sources) $(fonts_sources:.c=.h)

all-local: fonts/font_all.h

libfonts_la_SOURCES = $(fonts_sources)

