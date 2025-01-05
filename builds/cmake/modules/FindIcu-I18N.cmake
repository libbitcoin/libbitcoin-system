###############################################################################
#  Copyright (c) 2014-2025 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# FindIcu-I18N
#
# Use this module by invoking find_package with the form::
#
#   find_package( Icu-I18N
#     [version]              # Minimum version
#     [REQUIRED]             # Fail with error if icu-i18n is not found
#   )
#
#   Defines the following for use:
#
#   icu_i18n_FOUND                        - true if headers and requested libraries were found
#   icu_i18n_INCLUDE_DIRS                 - include directories for icu-i18n libraries
#   icu_i18n_LIBRARY_DIRS                 - link directories for icu-i18n libraries
#   icu_i18n_LIBRARIES                    - icu-i18n libraries to be linked
#   icu_i18n_PKG                          - icu-i18n pkg-config package specification.
#

if (MSVC)
    if ( Icu-I18N_FIND_REQUIRED )
        set( _icu_i18n_MSG_STATUS "SEND_ERROR" )
    else ()
        set( _icu_i18n_MSG_STATUS "STATUS" )
    endif()

    set( icu_i18n_FOUND false )
    message( ${_icu_i18n_MSG_STATUS} "MSVC environment detection for 'icu-i18n' not currently supported." )
else ()
    # required
    if ( Icu-I18N_FIND_REQUIRED )
        set( _icu_i18n_REQUIRED "REQUIRED" )
    endif()

    # quiet
    if ( Icu-I18N_FIND_QUIETLY )
        set( _icu_i18n_QUIET "QUIET" )
    endif()

    # modulespec
    if ( Icu-I18N_FIND_VERSION_COUNT EQUAL 0 )
        set( _icu_i18n_MODULE_SPEC "icu-i18n" )
    else ()
        if ( Icu-I18N_FIND_VERSION_EXACT )
            set( _icu_i18n_MODULE_SPEC_OP "=" )
        else ()
            set( _icu_i18n_MODULE_SPEC_OP ">=" )
        endif()

        set( _icu_i18n_MODULE_SPEC "icu-i18n ${_icu_i18n_MODULE_SPEC_OP} ${Icu-I18N_FIND_VERSION}" )
    endif()

    pkg_check_modules( icu_i18n ${_icu_i18n_REQUIRED} ${_icu_i18n_QUIET} "${_icu_i18n_MODULE_SPEC}" )
    set( icu_i18n_PKG "${_icu_i18n_MODULE_SPEC}" )
endif()
