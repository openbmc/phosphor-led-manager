AM_DEFAULT_SOURCE_EXT = .cpp
sbin_PROGRAMS = phosphor-ledmanager

phosphor_ledmanager_SOURCES = \
                led-main.cpp  \
                manager.cpp \
                group.cpp

BUILT_SOURCES = led-gen.hpp phosphor-logging/elog-errors.hpp
CLEANFILES = led-gen.hpp

led-gen.hpp: ${srcdir}/parse_led.py
	$(AM_V)@LEDGEN@ > $@

phosphor_ledmanager_LDFLAGS = $(SDBUSPLUS_LIBS) \
                              $(PHOSPHOR_LOGGING_LIBS) \
                              $(PHOSPHOR_DBUS_INTERFACES_LIBS)
phosphor_ledmanager_CFLAGS =  $(SDBUSPLUS_CFLAGS) \
                              $(PHOSPHOR_LOGGING_CFLAGS) \
                              $(PHOSPHOR_DBUS_INTERFACES_CFLAGS)

REQ_FILES_TO_GEN ?= ${datarootdir}/phosphor-logging/elog-gen.py ${datarootdir}/phosphor-logging/elog-gen-template.mako.hpp
phosphor-logging/elog-errors.hpp: $(REQ_FILES_TO_GEN)
	@mkdir -p `dirname $@`
	$(AM_V_at)$(PYTHON) ${datarootdir}/phosphor-logging/elog-gen.py -y /usr/share/phosphor-dbus-yaml/yaml  -t /usr/share/phosphor-logging -m elog-gen-template.mako.hpp -o $@

SUBDIRS = test fault-monitor
