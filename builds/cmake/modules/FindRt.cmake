###############################################################################
#  Copyright (c) 2014-2018 libbitcoin developers (see COPYING).
#
###############################################################################
# Findrt
#
# Use this module by invoking find_package with the form::
#
#   find_package( rt
#     [REQUIRED]             # Fail with error if rt is not found
#   )
#
#   Defines the following for use:
#
#   rt_FOUND        - True if headers and requested libraries were found
#   rt_LIBRARIES    - rt libraries to be linked
#   rt_LIBS         - rt libraries to be linked
#

if (DEFINED rt_FIND_VERSION)
    message( SEND_ERROR
        "Library 'rt' unable to process version: ${rt_FIND_VERSION}" )
endif()

if (MSVC)
    message( STATUS
        "MSVC environment detection for rt not currently supported." )

    set( rt_FOUND false )
elseif (ANDROID)
    message( STATUS
        "ANDROID enviornment detection for rt skipped, provided by libc." )

    set( rt_FOUND true )
else ()
    # required
    if ( rt_FIND_REQUIRED )
        set( _rt_REQUIRED "REQUIRED" )
    endif()

    find_library(rt_LIBRARIES rt)

    if (rt_LIBRARIES-NOTFOUND)
        set( rt_FOUND false )
    else ()
        set( rt_FOUND true )
        set( rt_LIBS "-lrt" )
    endif()
endif()

if ( rt_FIND_REQUIRED AND ( NOT rt_FOUND ) )
    message( SEND_ERROR "Library 'rt'  not found." )
endif()
