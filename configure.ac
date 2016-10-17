# Initialization
AC_PREREQ([2.69])
AC_INIT([phosphor-led-manager], [1.0], [https://github.com/openbmc/phosphor-led-manager/issues])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_14([noext])
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_MAKE_SET
LT_INIT

# Python
AM_PATH_PYTHON([2.7], [AC_SUBST([PYTHON], [echo "$PYTHON"])], [AC_MSG_ERROR([Could not find python-2.7 installed...python-2.7 is required])])

# Checks for libraries.
PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 221])

# Checks for header files.
AC_CHECK_HEADER(systemd/sd-bus.h, ,[AC_MSG_ERROR([Could not find systemd/sd-bus.h...systemd developement package required])])
#AC_CHECK_HEADER(sdbusplus/vtable.hpp, ,[AC_MSG_ERROR([Could not find vtable.hpp...openbmc/sdbusplus package required])])
#AC_CHECK_HEADER(dbusplus/message.hpp, ,[AC_MSG_ERROR([Could not find messsage.hpp...openbmc/sdbusplus package required])])
#AC_CHECK_HEADER(sdbusplus/bus.hpp, ,[AC_MSG_ERROR([Could not find bus.hpp...openbmc/sdbusplus package required])])

# Checks for typedefs, structures, and compiler characteristics.
AX_APPEND_COMPILE_FLAGS([-Wall -Werror], [CXXFLAGS])
AC_ARG_VAR(BUSNAME, [The Dbus busname to own])
AC_ARG_VAR(OBJPATH, [The Ledmanager Dbus root])
AC_ARG_VAR(INTERFACE, [The Ledmanager Dbus interface])
AS_IF([test "x$BUSNAME" == "x"], [BUSNAME="xyz.openbmc_project.ledmanager"])
AS_IF([test "x$OBJPATH" == "x"], [OBJPATH="/xyz/openbmc_project/ledmanager/groups"])
AS_IF([test "x$INTERFACE" == "x"], [INTERFACE="xyz.openbmc_project.Ledmanager"])
AC_DEFINE_UNQUOTED([BUSNAME], ["$BUSNAME"], [The DBus busname to own])
AC_DEFINE_UNQUOTED([OBJPATH], ["$OBJPATH"], [The Ledmanager Dbus root])
AC_DEFINE_UNQUOTED([INTERFACE], ["$INTERFACE"], [The Ledmanager Dbus interface])

# Create configured output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
