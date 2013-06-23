AC_INIT([libbitcoin], [1.0.10], [genjix@riseup.net])
AC_LANG(C++)
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])
AM_PROG_AR
LT_INIT
AC_PROG_CXX
CHECK_GXX_CPP0X
CXXFLAGS="$ac_cv_cxx_cpp0x_flag -Wall -Wno-unused-parameter -pedantic -pthread -Wextra -fstack-protector-all"

AC_ARG_ENABLE(bdb,
	[AC_HELP_STRING([--enable-bdb],
			[Build Berkeley DB support.])],
	[bld_bdb="$enable_bdb"], [bld_bdb="no"])
AM_CONDITIONAL([DO_BDB], [test "$enable_bdb" = "yes"])

AC_ARG_ENABLE(leveldb,
	[AC_HELP_STRING([--enable-leveldb],
			[Build LevelDB support.])],
	[bld_leveldb="$enable_leveldb"], [bld_leveldb="no"])
AM_CONDITIONAL([DO_LEVELDB], [test "$enable_leveldb" = "yes"])

AC_ARG_ENABLE([debug],
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AX_BOOST_BASE
#AX_BOOST_SYSTEM
#AX_BOOST_THREAD

PKG_PROG_PKG_CONFIG

if test "$bld_bdb" = "yes"; then
    AX_BERKELEY_DB_CXX([5.1], [], [AC_MSG_ERROR(Could not find a version of the library)])
    PKG_CHECK_MODULES([protobuf], [protobuf >= 2.4])
	AC_SUBST(LDFLAG_BDB)
	LDFLAG_BDB="-lprotobuf $DB_CXX_LIBS"
    AC_SUBST(CFLAG_BDB)
    CFLAG_BDB="-DBDB_ENABLED"
fi

if test "$bld_leveldb" = "yes"; then
    #AX_BERKELEY_DB_CXX([5.1], [], [AC_MSG_ERROR(Could not find a version of the library)])
    PKG_CHECK_MODULES([protobuf], [protobuf >= 2.4])
	AC_SUBST(LDFLAG_LEVELDB)
	LDFLAG_LEVELDB="-lprotobuf -lleveldb"
    AC_SUBST(CFLAG_LEVELDB)
    CFLAG_LEVELDB="-DLEVELDB_ENABLED"
fi

if test x$debug = xtrue; then
    my_CXXFLAGS="-g3 -DDEBUG"
    AC_SUBST([my_CXXFLAGS])
fi

PKG_CHECK_MODULES([SSL], [libssl >= 0.9])
PKG_CHECK_MODULES([CURL], [libcurl])

AC_ARG_WITH([pkgconfigdir], AS_HELP_STRING([--with-pkgconfigdir=PATH],
    [Path to the pkgconfig directory [[LIBDIR/pkgconfig]]]),
    [pkgconfigdir="$withval"], [pkgconfigdir='${libdir}/pkgconfig'])
AC_SUBST([pkgconfigdir])

AC_CONFIG_FILES([Makefile include/bitcoin/Makefile src/Makefile libbitcoin.pc])
AC_OUTPUT

