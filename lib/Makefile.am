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
fontout := fonts

# A list of all the fonts we want to build.
# # The source .ttf is determined from the name.
fonts_sources :=
fonts_sources += $(fontout)/font-UbuntuMonoR-10.c
fonts_sources += $(fontout)/font-UbuntuMonoR-12.c
fonts_sources += $(fontout)/font-UbuntuMonoR-14.c
fonts_sources += $(fontout)/font-UbuntuMonoR-16.c
fonts_sources += $(fontout)/font-UbuntuMonoR-18.c
fonts_sources += $(fontout)/font-UbuntuMonoR-20.c

fonts_sources += $(fontout)/font-UbuntuMonoB-10.c
fonts_sources += $(fontout)/font-UbuntuMonoB-12.c
fonts_sources += $(fontout)/font-UbuntuMonoB-14.c
fonts_sources += $(fontout)/font-UbuntuMonoB-16.c
fonts_sources += $(fontout)/font-UbuntuMonoB-18.c
fonts_sources += $(fontout)/font-UbuntuMonoB-20.c

fonts_sources += $(fontout)/font-DejaVuSansMono-10.c
fonts_sources += $(fontout)/font-DejaVuSansMono-11.c
fonts_sources += $(fontout)/font-DejaVuSansMono-12.c

fonts_sources += $(fontout)/font-VeraMono-10.c
fonts_sources += $(fontout)/font-VeraMono-11.c
fonts_sources += $(fontout)/font-VeraMono-12.c

fonts_sources += $(fontout)/font-LiberationMono-10.c
fonts_sources += $(fontout)/font-LiberationMono-11.c
fonts_sources += $(fontout)/font-LiberationMono-12.c

fonts_sources += $(fontout)/font-dsdigi-10.c
fonts_sources += $(fontout)/font-dsdigi-11.c
fonts_sources += $(fontout)/font-dsdigi-12.c

fonts_extra_sources = $(fontout)/font_all.c $(fontout)/fontrender.c $(fontout)/fontfinder.c
BUILT_SOURCES = $(fonts_sources) $(fonts_extra_sources)

$(fontout)/fontem.h: $(fontemdir)/src/resource/fontem.h
	cp -p $< $@

$(fontout)/fontrender.c: $(fontemdir)/src/fontrender.c
	cp -p $< $@

$(fontout)/fontfinder.c: $(fontemdir)/src/fontfinder.c $(fontout)/font_all.c
	cp -p $< $@

.SECONDEXPANSION:
$(fonts_sources): %.c: $$(fontdir)/$$(word 2,$$(subst -, ,%)).ttf $(fontout)/fontem.h $(fontemdir)/src/fontem
	$(fontemdir)/src/fontem --font=$(fontdir)/$(word 2,$(subst -, ,$(notdir $@))).ttf \
		--dir=$(fontout) --name=$(word 2,$(subst -, ,$(notdir $@))) --size=$(word 3,$(subst -, ,$(subst .c,,$(notdir $@))))

$(fontout)/font_all.h: $(fonts_sources) Makefile
	@echo "/* A list of all font headers. */" > $@
	@echo >> $@
	@echo "#include \"fontem.h\"" >> $@
	@echo >> $@
	for font in $(sort $(notdir $(fonts_sources:%.c=%.h))); do echo "#include \"$$font\"" >> $@; done
	@echo >> $@
	@echo "extern const struct font *fonts[];" >> $@
	@echo "extern const int font_count;" >> $@

$(fontout)/font_all.c: $(fontout)/font_all.h Makefile
	@echo "/* A list of all fonts. */" > $@
	@echo >> $@
	@echo "#include <stdio.h>" > $@
	@echo "#include \"fontem.h\"" >> $@
	@echo "#include \"font_all.h\"" >> $@
	@echo >> $@
	@echo "const int font_count = $(words $(fonts_sources));" >> $@
	@echo >> $@
	@echo "const struct font *fonts[] = {" >> $@
	for font in $(sort $(subst -,_,$(notdir $(fonts_sources:%.c=%)))); do printf "\t&$$font,\n" >> $@; done
	@printf "\tNULL\n" >> $@
	@echo "};" >> $@

clean-local:
	rm -f $(fonts_sources) $(fonts_sources:.c=.h) $(fontout)/font_all.h $(fontout)/font_all.c

all-local: fonts/font_all.h

libfonts_a_SOURCES = $(fonts_sources) $(fonts_extra_sources)

