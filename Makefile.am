AM_DEFAULT_SOURCE_EXT = .cpp
sbin_PROGRAMS = phosphor-ledmanager

phosphor_ledmanager_SOURCES = \
                led-main.cpp  \
                led-manager.cpp \
                xyz.openbmc_project.Led.Group.cpp

BUILT_SOURCES = led-gen.hpp
CLEANFILES = led-gen.hpp

PYTHON="/usr/bin/python"
led-gen.hpp: ${srcdir}/parse_led.py
	${AM_V_at}${PYTHON} $^ > $@

phosphor_ledmanager_LDFLAGS = $(SYSTEMD_LIBS)
phosphor_ledmanager_CFLAGS = $(SYSTEMD_CFLAGS)
