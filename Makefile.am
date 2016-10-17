AM_DEFAULT_SOURCE_EXT = .cpp
AM_CPPFLAGS = -I$(top_srcdir)/../sdbusplus

PYTHON = /usr/bin/python
BUILT_SOURCES = led-gen.hpp
CLEANFILES = led-gen.hpp
led-gen.hpp: ${srcdir}/parse_led.py
	$(PYTHON) $^ > $@

sbin_PROGRAMS = \
	ledmanager

ledmanager_SOURCES = \
	led-manager.cpp \
	led-main.cpp

ledmanager_LDFLAGS = $(SYSTEMD_LIBS) $(libmapper_LIBS)
