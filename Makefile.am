AM_DEFAULT_SOURCE_EXT = .cpp
bin_PROGRAMS = phosphor-ledmanager
bin_SCRIPTS = scripts/led-set-all-groups-asserted.sh

phosphor_ledmanager_SOURCES = \
                led-main.cpp  \
                manager.cpp \
                group.cpp \
                serialize.cpp \
                utils.cpp

if !WANTS_JSON
BUILT_SOURCES = led-gen.hpp
CLEANFILES = led-gen.hpp

led-gen.hpp: ${srcdir}/parse_led.py
	$(AM_V)@LEDGEN@ > $@
endif

phosphor_ledmanager_LDFLAGS = $(SDBUSPLUS_LIBS) \
                              $(PHOSPHOR_LOGGING_LIBS) \
                              $(PHOSPHOR_DBUS_INTERFACES_LIBS) \
                              $(SDEVENTPLUS_LIBS)
phosphor_ledmanager_CFLAGS =  $(SDBUSPLUS_CFLAGS) \
                              $(PHOSPHOR_LOGGING_CFLAGS) \
                              $(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
                              $(SDEVENTPLUS_CFLAGS)

if WANTS_LAMP_TEST
phosphor_ledmanager_SOURCES += lamptest.cpp
endif

led_default_configdir    	 = 	${datadir}/phosphor-led-manager
led_ibm_rainier_2u_configdir = 	${datadir}/phosphor-led-manager/ibm,rainier-2u
led_ibm_rainier_4u_configdir = 	${datadir}/phosphor-led-manager/ibm,rainier-4u

led_ibm_rainier_2u_config_DATA = configs/ibm,rainier-2u/led-group-config.json
led_ibm_rainier_4u_config_DATA = configs/ibm,rainier-4u/led-group-config.json

SUBDIRS = test fault-monitor
