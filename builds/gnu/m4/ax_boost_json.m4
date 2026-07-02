# ===========================================================================
#     https://www.gnu.org/software/autoconf-archive/ax_boost_thread.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_JSON
#
# DESCRIPTION
#
#   Test for JSON library from the Boost C++ libraries. The macro requires
#   a preceding call to AX_BOOST_BASE. Modified from pattern provided by
#   autoconf-archive boost macros.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_JSON_LIB)
#
#   And sets:
#
#     HAVE_BOOST_JSON
#
# LICENSE
#
#   Copyright (c) 2022 Phillip Mienk
#
#   A derivitative work.
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

AC_DEFUN([AX_BOOST_JSON],
[
	AC_ARG_WITH([boost-json],
		AS_HELP_STRING([--with-boost-json@<:@=special-lib@:>@],
		[use the JSON library from boost - it is possible to specify a certain library for the linker
			e.g. --with-boost-json=boost_json-gcc-mt ]), [
		if test "$withval" = "no"; then
			want_boost="no"
		elif test "$withval" = "yes"; then
			want_boost="yes"
			ax_boost_user_json_lib=""
		else
			want_boost="yes"
			ax_boost_user_json_lib="$withval"
		fi
		], [want_boost="yes"]
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

		AC_CACHE_CHECK(whether the Boost::JSON library is available,
			ax_cv_boost_json,
			[AC_LANG_PUSH([C++])
			CXXFLAGS_SAVE=$CXXFLAGS

			AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
				[[@%:@include <boost/json.hpp>]],
				[[
				    std::is_nothrow_destructible<boost::json::basic_parser<int>>::value;
				    return 0;
				]])],
				ax_cv_boost_json=yes, ax_cv_boost_json=no)
				CXXFLAGS=$CXXFLAGS_SAVE
			AC_LANG_POP([C++])
		])

		if test "x$ax_cv_boost_json" = "xyes"; then
			AC_SUBST(BOOST_CPPFLAGS)

			AC_DEFINE(HAVE_BOOST_JSON,,[define if the Boost::JSON library is available])
			BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

			# set ax_lib to compensate for sequential boost m4 declaration collisions
			ax_lib=''

			if test "x$ax_boost_user_json_lib" = "x"; then
				for libextension in `ls $BOOSTLIBDIR/libboost_json*.so* $BOOSTLIBDIR/libboost_json*.dylib* $BOOSTLIBDIR/libboost_json*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_json.*\)\.so.*$;\1;' -e 's;^lib\(boost_json.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_json.*\)\.a.*$;\1;'` ; do
					ax_lib=${libextension}
					AC_CHECK_LIB($ax_lib, exit,
						[BOOST_JSON_LIB="-l$ax_lib"; AC_SUBST(BOOST_JSON_LIB) link_json="yes"; break],
					[link_json="no"])
				done

				if test "x$link_json" != "xyes"; then
					for libextension in `ls $BOOSTLIBDIR/boost_json*.dll* $BOOSTLIBDIR/boost_json*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_json.*\)\.dll.*$;\1;' -e 's;^\(boost_json.*\)\.a.*$;\1;'` ; do
						ax_lib=${libextension}
						AC_CHECK_LIB($ax_lib, exit,
							[BOOST_JSON_LIB="-l$ax_lib"; AC_SUBST(BOOST_JSON_LIB) link_json="yes"; break],
							[link_json="no"])
					done
				fi

			else
				for ax_lib in $ax_boost_user_json_lib boost_json-$ax_boost_user_json_lib; do
					AC_CHECK_LIB($ax_lib, exit,
						[BOOST_JSON_LIB="-l$ax_lib"; AC_SUBST(BOOST_JSON_LIB) link_json="yes"; break],
						[link_json="no"])
				done
			fi

			if test "x$ax_lib" = "x"; then
				AC_MSG_ERROR(Could not find a version of the Boost::JSON library!)
			fi

			if test "x$link_json" = "xno"; then
				AC_MSG_ERROR(Could not link against $ax_lib !)
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
		LDFLAGS="$LDFLAGS_SAVED"
	fi
])
