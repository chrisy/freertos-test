# extlib Makefile.am
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT

lib_LTLIBRARIES = \
	librtos.la \
	libplatform.la \
	libmicrorl.la \
	libstdperiph.la

rtos_sources = \
	FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
	FreeRTOS/Source/portable/MemMang/heap_3.c \
	FreeRTOS/Source/croutine.c \
	FreeRTOS/Source/event_groups.c \
	FreeRTOS/Source/list.c \
	FreeRTOS/Source/queue.c \
	FreeRTOS/Source/tasks.c \
	FreeRTOS/Source/timers.c

platform_sources = \
	platform/core_cm3.c \
	platform/system_stm32f10x.c

microrl_sources = \
	microrl/src/microrl.c

stdperiph_sources = \
	stdperiph/src/misc.c \
	stdperiph/src/stm32f10x_adc.c \
	stdperiph/src/stm32f10x_bkp.c \
	stdperiph/src/stm32f10x_can.c \
	stdperiph/src/stm32f10x_cec.c \
	stdperiph/src/stm32f10x_crc.c \
	stdperiph/src/stm32f10x_dac.c \
	stdperiph/src/stm32f10x_dbgmcu.c \
	stdperiph/src/stm32f10x_dma.c \
	stdperiph/src/stm32f10x_exti.c \
	stdperiph/src/stm32f10x_flash.c \
	stdperiph/src/stm32f10x_fsmc.c \
	stdperiph/src/stm32f10x_gpio.c \
	stdperiph/src/stm32f10x_i2c.c \
	stdperiph/src/stm32f10x_iwdg.c \
	stdperiph/src/stm32f10x_pwr.c \
	stdperiph/src/stm32f10x_rcc.c \
	stdperiph/src/stm32f10x_rtc.c \
	stdperiph/src/stm32f10x_sdio.c \
	stdperiph/src/stm32f10x_spi.c \
	stdperiph/src/stm32f10x_tim.c \
	stdperiph/src/stm32f10x_usart.c \
	stdperiph/src/stm32f10x_wwdg.c

#librtos_la_CFLAGS = -Wno-missing-braces -Wno-missing-field-initializers -Wno-sign-compare
librtos_la_SOURCES = $(rtos_sources)
libplatform_la_SOURCES = $(platform_sources)
libmicrorl_la_SOURCES = $(microrl_sources)
libstdperiph_la_SOURCES = $(stdperiph_sources)
