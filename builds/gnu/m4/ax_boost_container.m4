# ===========================================================================
#    https://www.gnu.org/software/autoconf-archive/ax_boost_container.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_CONTAINER
#
# DESCRIPTION
#
#   Test for the Container library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_CONTAINER_LIB)
#
#   And sets:
#
#     HAVE_BOOST_CONTAINER
#
#serial 1

AC_DEFUN([AX_BOOST_CONTAINER], [
    AC_ARG_WITH([boost-container],
    AS_HELP_STRING([--with-boost-container@<:@=special-lib@:>@],
        [use the Container library from boost - it is possible to specify a certain library for the linker
        e.g. --with-boost-container=boost_container-gcc-mt ]),
        [
            if test "$withval" = "no"; then
                want_boost="no"
            elif test "$withval" = "yes"; then
                want_boost="yes"
                ax_boost_user_container_lib=""
            else
                want_boost="yes"
                ax_boost_user_container_lib="$withval"
            fi
        ],
        [want_boost="yes"]
    )

    if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
        AC_REQUIRE([AC_CANONICAL_BUILD])
        CPPFLAGS_SAVED="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
        export CPPFLAGS

        LDFLAGS_SAVED="$LDFLAGS"
        LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
        export LDFLAGS

        AC_CACHE_CHECK(whether the Boost::Container library is available,
            ax_cv_boost_container, [
            AC_LANG_PUSH([C++])
            CXXFLAGS_SAVE=$CXXFLAGS

            AC_COMPILE_IFELSE([
                AC_LANG_PROGRAM([[@%:@include <boost/container/vector.hpp>
@%:@include <boost/container/flat_map.hpp>]],
                    [[boost::container::vector<int> v; v.push_back(42);
                      boost::container::flat_map<int,int> m; m[1] = 2;]])
            ],
            ax_cv_boost_container=yes,
            ax_cv_boost_container=no
            )
            CXXFLAGS=$CXXFLAGS_SAVE
            AC_LANG_POP([C++])
        ])

        if test "x$ax_cv_boost_container" = "xyes"; then
            AC_SUBST(BOOST_CPPFLAGS)

            AC_DEFINE(HAVE_BOOST_CONTAINER,,[define if the Boost::Container library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

            LDFLAGS_SAVE=$LDFLAGS
            if test "x$ax_boost_user_container_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_container*.so* $BOOSTLIBDIR/libboost_container*.dylib* $BOOSTLIBDIR/libboost_container*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_container.*\)\.so.*$;\1;' -e 's;^lib\(boost_container.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_container.*\)\.a.*$;\1;'` ; do
                    ax_lib=${libextension}
                    AC_CHECK_LIB($ax_lib, exit,
                        [BOOST_CONTAINER_LIB="-l$ax_lib"; AC_SUBST(BOOST_CONTAINER_LIB) link_container="yes"; break],
                        [link_container="no"]
                    )
                done
                if test "x$link_container" != "xyes"; then
                    for libextension in `ls $BOOSTLIBDIR/boost_container*.dll* $BOOSTLIBDIR/boost_container*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_container.*\)\.dll.*$;\1;' -e 's;^\(boost_container.*\)\.a.*$;\1;'` ; do
                        ax_lib=${libextension}
                        AC_CHECK_LIB($ax_lib, exit,
                            [BOOST_CONTAINER_LIB="-l$ax_lib"; AC_SUBST(BOOST_CONTAINER_LIB) link_container="yes"; break],
                            [link_container="no"]
                        )
                    done
                fi
            else
                for ax_lib in $ax_boost_user_container_lib boost_container-$ax_boost_user_container_lib; do
                    AC_CHECK_LIB($ax_lib, exit,
                        [BOOST_CONTAINER_LIB="-l$ax_lib"; AC_SUBST(BOOST_CONTAINER_LIB) link_container="yes"; break],
                        [link_container="no"]
                    )
                done
            fi

            if test "x$ax_lib" = "x"; then
                AC_MSG_ERROR(Could not find a version of the Boost::Container library!)
            fi
            if test "x$link_container" = "xno"; then
                AC_MSG_ERROR(Could not link against $ax_lib !)
            fi
        fi

        CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
    fi
])
