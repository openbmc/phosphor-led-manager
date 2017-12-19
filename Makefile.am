AM_DEFAULT_SOURCE_EXT = .cpp
if !INSTALL_ERROR_YAML
sbin_PROGRAMS = phosphor-ledmanager

phosphor_ledmanager_SOURCES = \
                led-main.cpp  \
                manager.cpp \
                group.cpp

BUILT_SOURCES = led-gen.hpp
CLEANFILES = led-gen.hpp

led-gen.hpp: ${srcdir}/parse_led.py
	$(AM_V)@LEDGEN@ > $@

phosphor_ledmanager_LDFLAGS = $(SDBUSPLUS_LIBS) \
                              $(PHOSPHOR_LOGGING_LIBS) \
                              $(PHOSPHOR_DBUS_INTERFACES_LIBS)
phosphor_ledmanager_CFLAGS =  $(SDBUSPLUS_CFLAGS) \
                              $(PHOSPHOR_LOGGING_CFLAGS) \
                              $(PHOSPHOR_DBUS_INTERFACES_CFLAGS)

SUBDIRS = test fault-monitor
endif
# Export error YAML to shared location
yamldir = ${datadir}/phosphor-dbus-yaml/yaml
nobase_yaml_DATA = \
	xyz/openbmc_project/Led/Mapper.errors.yaml \
	xyz/openbmc_project/Led/Mapper.metadata.yaml \
	xyz/openbmc_project/Led/Fru/Monitor.metadata.yaml \
	xyz/openbmc_project/Led/Fru/Monitor.errors.yaml

