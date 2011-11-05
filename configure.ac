AC_INIT([libbitcoin], [0.1], [genjix@riseup.net])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])
LT_INIT
AC_PROG_CXX
CHECK_GXX_CPP0X
CXXFLAGS="$ac_cv_cxx_cpp0x_flag -Wall -pedantic -pthread -ggdb -Wextra -fstack-protector-all"
AX_BOOST_BASE
AX_BOOST_SYSTEM
AX_BOOST_THREAD
PKG_CHECK_MODULES([SSL], [libssl >= 0.9])
AC_CONFIG_FILES([Makefile include/bitcoin/Makefile src/Makefile])
AC_OUTPUT

