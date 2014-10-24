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
	libmicrorl.la

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

#librtos_la_CFLAGS = -Wno-missing-braces -Wno-missing-field-initializers -Wno-sign-compare
librtos_la_SOURCES = $(rtos_sources)
libstm32_la_SOURCES = $(stm32_sources)
libplatform_la_SOURCES = $(platform_sources)
libposixio_la_SOURCES = $(posixio_sources)
libmisc_la_SOURCES = $(misc_sources)
libmicrorl_la_SOURCES = $(microrl_sources)

