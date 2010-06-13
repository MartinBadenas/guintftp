AC_INIT([Guintftp], [0.1], [dhernandez0@gmail.com], [guintftp], [http://code.google.com/p/guintftp/])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Werror no-define])
PKG_CHECK_MODULES(CHECK, [check >= 0.9.4],,[AC_MSG_RESULT([no, testing is disabled])]) 
AM_CONDITIONAL(HAVE_CHECK, [test "x$CHECK_LIBS" != "x"])
CFLAGS="$CFLAGS -std=gnu89 -pedantic-errors -Wall -Wextra -Wshadow -Wcast-qual -Wstrict-overflow=5 -Wformat=2 \
-Wformat-y2k -Wformat-nonliteral -Wformat-security -Wstrict-prototypes -Wmissing-prototypes -fstack-protector -O3 -Wl,-z,relro"
AC_CONFIG_HEADERS([src/daemon/config.h])
AC_CONFIG_TESTDIR([src/daemon/test])
AM_PROG_CC_C_O
AC_CONFIG_FILES([Makefile])
AC_OUTPUT