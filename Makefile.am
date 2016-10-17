AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I$(top_srcdir)/../sdbusplus
sbin_PROGRAMS = \
	ledmanager

ledmanager_SOURCES = \
	led-manager.cpp

ledmanager_LDFLAGS = $(SYSTEMD_LIBS) $(libmapper_LIBS)
