# lib Makefile.am
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT

lib_LIBRARIES = \
	libstm32.a \
	libposixio.a \
	libmisc.a \
	libcli.a \
	libfonts.a

stm32_sources = \
	stm32/dma.c \
	stm32/flash.c \
	stm32/i2c.c \
	stm32/iwdg.c \
	stm32/mmc.c \
	stm32/serial.c \
	stm32/spi.c

posixio_sources = \
	posixio/posixio.c \
	posixio/fdio.c \
	posixio/fileio.c \
	posixio/dev/serial.c

misc_sources = \
	misc/crc7.c

cli_sources = \
	cli/cli.c

#librtos_la_CFLAGS = -Wno-missing-braces -Wno-missing-field-initializers -Wno-sign-compare
libstm32_a_SOURCES = $(stm32_sources)
libposixio_a_SOURCES = $(posixio_sources)
libmisc_a_SOURCES = $(misc_sources)
libcli_a_SOURCES = $(cli_sources)

fontdir := $(top_srcdir)/resources/ttf
fontemdir := $(top_srcdir)/tools/fontem

# A list of all the fonts we want to build.
# # The source .ttf is determined from the name.
fonts_sources :=
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

fonts_sources += fonts/font-VeraMono-10.c
fonts_sources += fonts/font-VeraMono-11.c
fonts_sources += fonts/font-VeraMono-12.c
fonts_sources += fonts/font-VeraMono-13.c
fonts_sources += fonts/font-VeraMono-14.c
fonts_sources += fonts/font-VeraMono-16.c
fonts_sources += fonts/font-VeraMono-18.c
fonts_sources += fonts/font-VeraMono-20.c
fonts_sources += fonts/font-VeraMono-22.c
fonts_sources += fonts/font-VeraMono-24.c
fonts_sources += fonts/font-VeraMono-26.c
fonts_sources += fonts/font-VeraMono-28.c
fonts_sources += fonts/font-VeraMono-30.c

fonts_sources += fonts/font-LiberationMono-10.c
fonts_sources += fonts/font-LiberationMono-11.c
fonts_sources += fonts/font-LiberationMono-12.c
fonts_sources += fonts/font-LiberationMono-13.c
fonts_sources += fonts/font-LiberationMono-14.c
fonts_sources += fonts/font-LiberationMono-16.c
fonts_sources += fonts/font-LiberationMono-18.c
fonts_sources += fonts/font-LiberationMono-20.c
fonts_sources += fonts/font-LiberationMono-22.c
fonts_sources += fonts/font-LiberationMono-24.c
fonts_sources += fonts/font-LiberationMono-26.c
fonts_sources += fonts/font-LiberationMono-28.c
fonts_sources += fonts/font-LiberationMono-30.c

fonts_sources += fonts/font-dsdigi-10.c
fonts_sources += fonts/font-dsdigi-11.c
fonts_sources += fonts/font-dsdigi-12.c
fonts_sources += fonts/font-dsdigi-13.c
fonts_sources += fonts/font-dsdigi-14.c
fonts_sources += fonts/font-dsdigi-16.c
fonts_sources += fonts/font-dsdigi-18.c
fonts_sources += fonts/font-dsdigi-20.c
fonts_sources += fonts/font-dsdigi-22.c
fonts_sources += fonts/font-dsdigi-24.c
fonts_sources += fonts/font-dsdigi-26.c
fonts_sources += fonts/font-dsdigi-28.c
fonts_sources += fonts/font-dsdigi-30.c

BUILT_SOURCES = $(fonts_sources)

fonts/fontem.h: $(fontemdir)/src/resource/fontem.h
	cp -p $< $@

fonts_extra_sources = fonts/fontrender.c

fonts/fontrender.c: $(fontemdir)/src/fontrender.c
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

libfonts_a_SOURCES = $(fonts_sources) $(fonts_extra_sources)

