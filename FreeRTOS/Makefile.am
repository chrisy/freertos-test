# FreeRTOS Makefile.am

lib_LTLIBRARIES = librtos.la

sources = \
	Source/portable/GCC/ARM_CM3/port.c \
	Source/croutine.c \
	Source/event_groups.c \
	Source/list.c \
	Source/queue.c \
	Source/tasks.c \
	Source/timers.c

#librtos_la_CFLAGS = -Wno-missing-braces -Wno-missing-field-initializers -Wno-sign-compare
librtos_la_SOURCES = $(sources)

