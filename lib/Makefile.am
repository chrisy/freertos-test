# lib Makefile.am

lib_LTLIBRARIES = \
	librtos.la \
	libstm32.la \
	libplatform.la \
	libmisc.la

rtos_sources = \
	FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
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
	platform/sbrk.c

misc_sources = \
	misc/crc7.c

#librtos_la_CFLAGS = -Wno-missing-braces -Wno-missing-field-initializers -Wno-sign-compare
librtos_la_SOURCES = $(rtos_sources)
libstm32_la_SOURCES = $(stm32_sources)
libplatform_la_SOURCES = $(platform_sources)
libmisc_la_SOURCES = $(misc_sources)

