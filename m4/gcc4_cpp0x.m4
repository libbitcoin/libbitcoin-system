dnl From http://dev.medozas.de/gitweb.cgi?p=m4_jengelh;a=blob;f=gcc4_cpp0x.m4;hb=HEAD
AC_DEFUN([CHECK_GXX_CPP0X], [
    AC_MSG_CHECKING([for g++ C++0x support])
    AC_LANG_PUSH([C++])
    saved_CXXFLAGS="$CXXFLAGS";
    CXXFLAGS="$CXXFLAGS -std=gnu++0x";
    AC_COMPILE_IFELSE(
        [AC_LANG_SOURCE([
            #include <cstdint>
            uint8_t xu8;
            uint16_t xu16;
            uint32_t xu32;
            uint64_t xu64;
            int8_t xi8;
            int16_t xi16;
            int32_t xi32;
            int64_t xi64;
        ])],
        [ac_cv_cxx_cpp0x_flag="-std=gnu++0x"; AC_MSG_RESULT([yes])],
        [AC_MSG_RESULT([no])]
    )
    CXXFLAGS="$saved_CXXFLAGS";
    AC_LANG_POP([C++])
])

