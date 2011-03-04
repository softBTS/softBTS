#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([softBTS], [0.0.4], [fadil.r.berisha@gmail.com], [softBTS], [softbts.org], [xxx])
AM_INIT_AUTOMAKE
#AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h])


# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_MAKE_SET
AM_PROG_CC_C_O

# Checks for libraries.
AC_SEARCH_LIBS(glib)

#Add CFLAGS and LDFLAGS for glib-2.0
PKG_CHECK_MODULES(GLIB,glib-2.0)
	
AC_MSG_NOTICE([Adding glib-2.0 CFLAGS ${GLIB_CFLAGS}])
CFLAGS="${GLIB_CFLAGS} ${CFLAGS}"
AC_MSG_NOTICE([Adding glib-2.0 LDFLAGS ${GLIB_LIBS}])
LDFLAGS="${GLIB_LIBS} ${LDFLAGS}"

# Checks for header files.
AC_HEADER_STDC


# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions
AC_CHECK_FUNCS([atexit getcwd gettimeofday memchr memmove memset select socket strchr strerror strspn strtol strtoul uname])

AC_OUTPUT(
	Makefile
	lib/talloc/Makefile 
	lib/select/Makefile 
	lib/timer/Makefile 
	lib/vty/Makefile 
	lib/linuxlist/Makefile 
	src/Makefile)





