AM_DEFAULT_SOURCE_EXT = .cpp

noinst_HEADERS = fru-fault-monitor.hpp

sbin_PROGRAMS = phosphor-fru-fault-monitor

phosphor_fru_fault_monitor_SOURCES = \
               fru-fault-monitor.cpp \
               monitor-main.cpp

phosphor_fru_fault_monitor_LDFLAGS = $(SDBUSPLUS_LIBS) \
                                     $(PHOSPHOR_LOGGING_LIBS) \
                                     $(PHOSPHOR_DBUS_INTERFACES_LIBS)
phosphor_fru_fault_monitor_CFLAGS =  $(SDBUSPLUS_CFLAGS) \
                                     $(PHOSPHOR_LOGGING_CFLAGS) \
                                     $(PHOSPHOR_DBUS_INTERFACES_CFLAGS)
