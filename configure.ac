AC_INIT([gldbg],[1],[andreas@tarandi.se])

AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile])

AC_GNU_SOURCE
AC_PROG_LN_S

AC_ENABLE_SHARED
AC_DISABLE_STATIC
LT_INIT

AX_CHECK_GL
AX_GLEW
AM_PATH_SDL

AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug], [Turn on debug output @<:@default=disabled@:>@])], [], [enable_debug="no"])
AS_IF([test "x$enable_debug" == "xyes"],
			[AC_DEFINE([DEBUG], [1], [Define to 1 to turn on debug output])
			enable_debug=no
])

dnl Setup paths
AC_DEFINE_UNQUOTED([srcdir], ["${srcdir}"], [srcdir])

AC_OUTPUT
