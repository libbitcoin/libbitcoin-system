###############################################################################
#  Copyright (c) 2014-2023 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# FindPng
#
# Use this module by invoking find_package with the form::
#
#   find_package( Png
#     [version]              # Minimum version
#     [REQUIRED]             # Fail with error if png is not found
#   )
#
#   Defines the following for use:
#
#   png_FOUND                             - true if headers and requested libraries were found
#   png_INCLUDE_DIRS                      - include directories for png libraries
#   png_LIBRARY_DIRS                      - link directories for png libraries
#   png_LIBRARIES                         - png libraries to be linked
#   png_PKG                               - png pkg-config package specification.
#

if (MSVC)
    if ( Png_FIND_REQUIRED )
        set( _png_MSG_STATUS "SEND_ERROR" )
    else ()
        set( _png_MSG_STATUS "STATUS" )
    endif()

    set( png_FOUND false )
    message( ${_png_MSG_STATUS} "MSVC environment detection for 'png' not currently supported." )
else ()
    # required
    if ( Png_FIND_REQUIRED )
        set( _png_REQUIRED "REQUIRED" )
    endif()

    # quiet
    if ( Png_FIND_QUIETLY )
        set( _png_QUIET "QUIET" )
    endif()

    # modulespec
    if ( Png_FIND_VERSION_COUNT EQUAL 0 )
        set( _png_MODULE_SPEC "libpng" )
    else ()
        if ( Png_FIND_VERSION_EXACT )
            set( _png_MODULE_SPEC_OP "=" )
        else ()
            set( _png_MODULE_SPEC_OP ">=" )
        endif()

        set( _png_MODULE_SPEC "libpng ${_png_MODULE_SPEC_OP} ${Png_FIND_VERSION}" )
    endif()

    pkg_check_modules( png ${_png_REQUIRED} ${_png_QUIET} "${_png_MODULE_SPEC}" )
    set( png_PKG "${_png_MODULE_SPEC}" )
endif()
