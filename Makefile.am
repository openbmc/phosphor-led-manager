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

phosphor_ledmanager_LDFLAGS = $(SDBUSPLUS_LIBS) $(PHOSPHOR_LOGGING_LIBS)
phosphor_ledmanager_CFLAGS =  $(SDBUSPLUS_CFLAGS) $(PHOSPHOR_LOGGING_CFLAGS)

SUBDIRS = test
