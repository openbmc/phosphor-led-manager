# Initialization
AC_PREREQ([2.69])
AC_INIT([phosphor-led-manager], [1.0], [https://github.com/openbmc/phosphor-led-manager/issues])
AC_LANG([C++])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for typedefs, structures, and compiler characteristics.
AX_CXX_COMPILE_STDCXX_14([noext])
AX_APPEND_COMPILE_FLAGS([-Wall -Werror], [CXXFLAGS])

# Python
AM_PATH_PYTHON([2.7], [AC_SUBST([PYTHON], [echo "$PYTHON"])], [AC_MSG_ERROR([Could not find python-2.7 installed...python-2.7 is required])])

# Checks for libraries.
PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 221])

# Check/set gtest specific functions.
AX_PTHREAD([GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=1"],[GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=0"])
AC_SUBST(GTEST_CPPFLAGS)

# Checks for header files.
AC_CHECK_HEADER(systemd/sd-bus.h, ,[AC_MSG_ERROR([Could not find systemd/sd-bus.h...systemd developement package required])])
AC_CHECK_HEADER(sdbusplus/server.hpp, ,[AC_MSG_ERROR([Could not find sdbusplus/server.hpp...openbmc/sdbusplus package required])])

# Dbus service name
AC_ARG_VAR(BUSNAME, [The Dbus busname to own])
AS_IF([test "x$BUSNAME" == "x"], [BUSNAME="xyz.openbmc_project.ledmanager"])
AC_DEFINE_UNQUOTED([BUSNAME], ["$BUSNAME"], [The Dbus busname to own])

# Service dbus root
AC_ARG_VAR(OBJPATH, [The Ledmanager Dbus root])
AS_IF([test "x$OBJPATH" == "x"], [OBJPATH="/xyz/openbmc_project/ledmanager/groups"])
AC_DEFINE_UNQUOTED([OBJPATH], ["$OBJPATH"], [The Ledmanager Dbus root])

AC_ARG_VAR(INTERFACE, [The Ledmanager Dbus interface])
AS_IF([test "x$INTERFACE" == "x"], [INTERFACE="xyz.openbmc_project.ledmanager"])
AC_DEFINE_UNQUOTED([INTERFACE], ["$INTERFACE"], [The Ledmanager Dbus interface])

# Create configured output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
