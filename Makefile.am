sbin_PROGRAMS = phosphor-ledmanager
AM_CPPFLAGS = -I$(top_srcdir)/../sdbusplus

phosphor_ledmanager_SOURCES = led-main.cpp
phosphor_ledmanager_LDADD = libmanager.la

noinst_LTLIBRARIES = libmanager.la
libmanager_la_SOURCES = \
	xyz.openbmc_project.Led.Group.cpp \
	led-manager.cpp

PYTHON="/usr/bin/python"
led-gen.hpp: ${srcdir}/parse_led.py
	${AM_V_at}${PYTHON} $^ > $@

phosphor_ledmanager_LDFLAGS = $(SYSTEMD_LIBS)
phosphor_ledmanager_CFLAGS = $(SYSTEMD_CFLAGS)
