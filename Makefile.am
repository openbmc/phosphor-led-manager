AM_DEFAULT_SOURCE_EXT = .cpp
if !INSTALL_ERROR_YAML
sbin_PROGRAMS = phosphor-ledmanager

phosphor_ledmanager_SOURCES = \
                led-main.cpp  \
                manager.cpp \
                group.cpp

BUILT_SOURCES = led-gen.hpp
CLEANFILES = led-gen.hpp

if GEN_ERRORS
nobase_nodist_include_HEADERS = \
				phosphor-logging/elog-errors.hpp
BUILT_SOURCES += phosphor-logging/elog-errors.hpp
CLEANFILES += phosphor-logging/elog-errors.hpp
endif

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

# Generate phosphor-logging/elog-errors.hpp
if GEN_ERRORS
ELOG_MAKO ?= elog-gen-template.mako.hpp
ELOG_DIR ?= ${OECORE_NATIVE_SYSROOT}${datadir}/phosphor-logging/elog
ELOG_GEN_DIR ?= ${ELOG_DIR}/tools/
ELOG_MAKO_DIR ?= ${ELOG_DIR}/tools/phosphor-logging/templates/
YAML_DIR ?= ${OECORE_NATIVE_SYSROOT}${datadir}/phosphor-dbus-yaml/yaml
phosphor-logging/elog-errors.hpp:
	@mkdir -p ${YAML_DIR}/xyz/openbmc_project/Led/Fru/
	@cp ${top_srcdir}/xyz/openbmc_project/Led/Mapper.errors.yaml ${YAML_DIR}/xyz/openbmc_project/Led/Mapper.errors.yaml
	@cp ${top_srcdir}/xyz/openbmc_project/Led/Mapper.metadata.yaml ${YAML_DIR}/xyz/openbmc_project/Led/Mapper.metadata.yaml
	@cp ${top_srcdir}/xyz/openbmc_project/Led/Fru/Monitor.errors.yaml ${YAML_DIR}/xyz/openbmc_project/Led/Fru/Monitor.errors.yaml
	@cp ${top_srcdir}/xyz/openbmc_project/Led/Fru/Monitor.metadata.yaml ${YAML_DIR}/xyz/openbmc_project/Led/Fru/Monitor.metadata.yaml
	@mkdir -p `dirname $@`
	@chmod 777 $(ELOG_GEN_DIR)/elog-gen.py
	$(AM_V_at)$(PYTHON) $(ELOG_GEN_DIR)/elog-gen.py -y ${YAML_DIR} -t ${ELOG_MAKO_DIR} -m ${ELOG_MAKO} -o $@
endif
