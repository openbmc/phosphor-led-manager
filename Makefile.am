sbin_PROGRAMS = phosphor-ledmanager

phosphor_ledmanager_SOURCES = \
				led-main.cpp  \
				led-manager.cpp

PYTHON="/usr/bin/python"
led-gen.hpp: ${srcdir}/parse_led.py
	${AM_V_at}${PYTHON} $^ > $@

phosphor_ledmanager_LDFLAGS = $(SYSTEMD_LIBS)
phosphor_ledmanager_CFLAGS = $(SYSTEMD_CFLAGS)
