AC_INIT([cervice], [0])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])

PKG_CHECK_MODULES([DEPS], [libmicrohttpd])

AC_ARG_VAR(HEROKU_APP_NAME, heroku app to to deploy)

AC_PROG_CC
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
