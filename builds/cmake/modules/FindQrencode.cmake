###############################################################################
#  Copyright (c) 2014-2018 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# FindQrencode
#
# Use this module by invoking find_package with the form::
#
#   find_package( Qrencode
#     [version]              # Minimum version
#     [REQUIRED]             # Fail with error if qrencode is not found
#   )
#
#   Defines the following for use:
#
#   qrencode_FOUND                        - true if headers and requested libraries were found
#   qrencode_INCLUDE_DIRS                 - include directories for qrencode libraries
#   qrencode_LIBRARY_DIRS                 - link directories for qrencode libraries
#   qrencode_LIBRARIES                    - qrencode libraries to be linked
#   qrencode_PKG                          - qrencode pkg-config package specification.
#

if (MSVC)
    if ( Qrencode_FIND_REQUIRED )
        set( _qrencode_MSG_STATUS "SEND_ERROR" )
    else ()
        set( _qrencode_MSG_STATUS "STATUS" )
    endif()

    set( qrencode_FOUND false )
    message( ${_qrencode_MSG_STATUS} "MSVC environment detection for 'qrencode' not currently supported." )
else ()
    # required
    if ( Qrencode_FIND_REQUIRED )
        set( _qrencode_REQUIRED "REQUIRED" )
    endif()

    # quiet
    if ( Qrencode_FIND_QUIETLY )
        set( _qrencode_QUIET "QUIET" )
    endif()

    # modulespec
    if ( Qrencode_FIND_VERSION_COUNT EQUAL 0 )
        set( _qrencode_MODULE_SPEC "libqrencode" )
    else ()
        if ( Qrencode_FIND_VERSION_EXACT )
            set( _qrencode_MODULE_SPEC_OP "=" )
        else ()
            set( _qrencode_MODULE_SPEC_OP ">=" )
        endif()

        set( _qrencode_MODULE_SPEC "libqrencode ${_qrencode_MODULE_SPEC_OP} ${Qrencode_FIND_VERSION}" )
    endif()

    pkg_check_modules( qrencode ${_qrencode_REQUIRED} ${_qrencode_QUIET} "${_qrencode_MODULE_SPEC}" )
    set( qrencode_PKG "${_qrencode_MODULE_SPEC}" )
endif()
