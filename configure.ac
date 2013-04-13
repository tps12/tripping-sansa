AC_INIT([cervice], [0])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])

PKG_CHECK_MODULES([DEPS], [libmicrohttpd])

AC_ARG_WITH([mongo-driver-include-path],
  [AS_HELP_STRING([--with-mongo-driver-include-path],
    [location of the Mongo C driver headers, defaults to /usr/local/include])],
  [MONGO_CFLAGS="-I$withval"],
  [MONGO_CFLAGS='-I/usr/include'])
AC_SUBST([MONGO_CFLAGS])

AC_ARG_WITH([mongo-driver-lib-path],
  [AS_HELP_STRING([--with-mongo-driver-lib-path], [location of the Mongo C driver library])],
  [MONGO_LIBS="-L$withval -lmongoc"],
  [MONGO_LIBS='-lmongoc'])
AC_SUBST([MONGO_LIBS])

AC_ARG_VAR(HEROKU_APP_NAME, heroku app to to deploy)

AC_PROG_CC
AC_CONFIG_FILES([Makefile tests/Makefile])
AC_OUTPUT
