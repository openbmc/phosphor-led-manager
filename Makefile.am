AM_DEFAULT_SOURCE_EXT = .cpp
sbin_PROGRAMS = phosphor-ledmanager

phosphor_ledmanager_SOURCES = \
                led-main.cpp  \
                manager.cpp \
                group.cpp \
                xyz.openbmc_project.Led.Group.cpp

BUILT_SOURCES = led-gen.hpp
CLEANFILES = led-gen.hpp

led-gen.hpp: ${srcdir}/parse_led.py
	$(AM_V)@LEDGEN@ > $@

phosphor_ledmanager_LDFLAGS = $(SYSTEMD_LIBS) $(SDBUSPLUS_LIBS)
phosphor_ledmanager_CFLAGS = $(SYSTEMD_CFLAGS) $(SDBUSPLUS_CFLAGS)

SUBDIRS = test
