# - Try to find ImageSecp2561k
# Once done, this will define
#
#  Secp2561k_FOUND - system has Secp2561k
#  Secp2561k_INCLUDE_DIRS - the Secp2561k include directories
#  Secp2561k_LIBRARIES - link these to use Secp2561k

find_package(PkgConfig)
pkg_check_modules(PC_SECP256 QUIET sepc256k1)

if(NOT SECP256K1_ROOT)
  set(SECP256K1_ROOT /usr/local)
endif(NOT SECP256K1_ROOT)

find_path(SECP256K1_INCLUDE_DIR secp256k1.h
HINTS ${SECP256K1_ROOT}/include
    PATH_SUFFIXES secp256k1 )

find_library(SECP256K1_LIBRARY_RELEASE NAMES secp256k1.lib libsecp256k1.so
   PATHS ${SECP256K1_ROOT}/bin/x64/Release/v141/static
   ${SECP256K1_ROOT}/.libs/
   ${SECP256K1_ROOT}/lib
   ${SECP256K1_ROOT}/
)
find_library(SECP256K1_LIBRARY_DEBUG NAMES secp256k1.lib libsecp256k1.so
   PATHS ${SECP256K1_ROOT}/bin/x64/Debug/v141/static
   ${SECP256K1_ROOT}/.libs/
   ${SECP256K1_ROOT}/lib
   ${SECP256K1_ROOT}/
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SECP256K1  DEFAULT_MSG
                                  SECP256K1_LIBRARY_RELEASE
                                  SECP256K1_LIBRARY_DEBUG
                                  SECP_INCLUDE_DIR)

set(SECP256K1_LIBRARIES optimized ${SECP256K1_LIBRARY_RELEASE} debug ${SECP256K1_LIBRARY_DEBUG})
set(SECP256K1_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR} )

