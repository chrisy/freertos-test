# src Makefile.am

linker_script = platform/STM32F103ZE.ld
AM_LDFLAGS = -T $(linker_script)
EXEEXT = .elf

bin_PROGRAMS = image
extra_outputs = image.bin image.hex image.lst

sources = \
	platform/crt0.c \
	platform/freertos.c \
	main.c

libdirs = -L$(top_srcdir)/lib/.libs

image_LDADD = $(libdirs) -lc -lm -lplatform -lrtos -lmisc
image_SOURCES = $(sources)
EXTRA_image_DEPENDENCIES = $(linker_script)

image.bin: image$(EXEEXT)
	$(OBJCOPY) -j .nvic_vector -j .info -j .text -j .data -j .data_init $< -O binary $@

image.hex: image$(EXEEXT)
	$(OBJCOPY) -j .nvic_vector -j .info -j .text -j .data -j .data_init $< -O ihex $@

image.lst: image$(EXEEXT)
	$(OBJDUMP) -S $< > $@

all-local: $(extra_outputs)

clean-local:
	rm -f $(extra_outputs)
