###############################################################################
#  Copyright (c) 2014-2019 libbitcoin developers (see COPYING).
#
###############################################################################
# Finddl
#
# Use this module by invoking find_package with the form::
#
#   find_package( dl
#     [REQUIRED]             # Fail with error if dl is not found
#   )
#
#   Defines the following for use:
#
#   dl_FOUND        - True if headers and requested libraries were found
#   dl_LIBRARIES    - dl libraries to be linked
#   dl_LIBS         - dl libraries to be linked
#

if (DEFINED dl_FIND_VERSION)
    message( SEND_ERROR "Library 'dl' unable to process specified version: ${dl_FIND_VERSION}" )
endif()

if (MSVC)
    message( STATUS "MSVC environment detection for 'dl' not currently supported." )
    set( dl_FOUND false )
else ()
    # required
    if ( dl_FIND_REQUIRED )
        set( _dl_REQUIRED "REQUIRED" )
    endif()

    find_library(dl_LIBRARIES dl)

    if (dl_LIBRARIES-NOTFOUND)
        set( dl_FOUND false )
    else ()
        set( dl_FOUND true )
        set( dl_LIBS "-ldl" )
    endif()
endif()

if ( dl_FIND_REQUIRED AND ( NOT dl_FOUND ) )
    message( SEND_ERROR "Library 'dl'  not found." )
endif()
