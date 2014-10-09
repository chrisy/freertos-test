# src Makefile.am

linker_script = stm32.ld
AM_LDFLAGS = -T $(linker_script)

bin_PROGRAMS = image
extra_outputs = image.bin image.hex image.lst

sources = \
	crt0.c \
	main.c

image_LDADD = -L$(top_srcdir)/FreeRTOS/.libs -lrtos
image_SOURCES = $(sources)
EXTRA_image_DEPENDENCIES = $(linker_script)

image.bin: image
	$(OBJCOPY) $< -O binary $@

image.hex: image
	$(OBJCOPY) $< -O ihex $@

image.lst: image
	$(OBJDUMP) -S $< > $@

all-am: $(extra_outputs)

clean-local:
	rm -f $(extra_outputs)
