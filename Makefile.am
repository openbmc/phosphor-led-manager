AM_DEFAULT_SOURCE_EXT = .cpp
AM_CPPFLAGS = -I$(top_srcdir)/../sdbusplus

BUILT_SOURCES = led-gen.hpp
CLEANFILES = led-gen.hpp
PYTHON="/usr/bin/python"
led-gen.hpp: $(srcdir)/parse_led.py
	$(AM_V_at)$(PYTHON) $^ > $@

sbin_PROGRAMS = \
	ledmanager

ledmanager_SOURCES = \
	led-manager.cpp \
	led-main.cpp

ledmanager_LDFLAGS = $(SYSTEMD_LIBS)
