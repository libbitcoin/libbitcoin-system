###############################################################################
#  Copyright (c) 2014-2023 libbitcoin-consensus developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# FindSecp256K1
#
# Use this module by invoking find_package with the form::
#
#   find_package( Secp256K1
#     [version]              # Minimum version
#     [REQUIRED]             # Fail with error if secp256k1 is not found
#   )
#
#   Defines the following for use:
#
#   secp256k1_FOUND                       - true if headers and requested libraries were found
#   secp256k1_INCLUDE_DIRS                - include directories for secp256k1 libraries
#   secp256k1_LIBRARY_DIRS                - link directories for secp256k1 libraries
#   secp256k1_LIBRARIES                   - secp256k1 libraries to be linked
#   secp256k1_PKG                         - secp256k1 pkg-config package specification.
#

if (MSVC)
    if ( Secp256K1_FIND_REQUIRED )
        set( _secp256k1_MSG_STATUS "SEND_ERROR" )
    else ()
        set( _secp256k1_MSG_STATUS "STATUS" )
    endif()

    set( secp256k1_FOUND false )
    message( ${_secp256k1_MSG_STATUS} "MSVC environment detection for 'secp256k1' not currently supported." )
else ()
    # required
    if ( Secp256K1_FIND_REQUIRED )
        set( _secp256k1_REQUIRED "REQUIRED" )
    endif()

    # quiet
    if ( Secp256K1_FIND_QUIETLY )
        set( _secp256k1_QUIET "QUIET" )
    endif()

    # modulespec
    if ( Secp256K1_FIND_VERSION_COUNT EQUAL 0 )
        set( _secp256k1_MODULE_SPEC "libsecp256k1" )
    else ()
        if ( Secp256K1_FIND_VERSION_EXACT )
            set( _secp256k1_MODULE_SPEC_OP "=" )
        else ()
            set( _secp256k1_MODULE_SPEC_OP ">=" )
        endif()

        set( _secp256k1_MODULE_SPEC "libsecp256k1 ${_secp256k1_MODULE_SPEC_OP} ${Secp256K1_FIND_VERSION}" )
    endif()

    pkg_check_modules( secp256k1 ${_secp256k1_REQUIRED} ${_secp256k1_QUIET} "${_secp256k1_MODULE_SPEC}" )
    set( secp256k1_PKG "${_secp256k1_MODULE_SPEC}" )
endif()
