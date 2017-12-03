#-------------------------------------------------
# CMakeLists.txt
#-------------------------------------------------

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(libbitcoin LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
set(LIBBITCOIN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/bitcoin/bitcoin)

# Currently only support building static libraries
ADD_DEFINITIONS(-DBC_STATIC)

if (MSVC)
  ADD_DEFINITIONS(-DBOOST_USE_WINAPI_VERSION=0x0601 -D_WIN32_WINNT=0x0601 -DWIN32_LEAN_AND_MEAN)
   #SET(TPN_WIN32 "/EHsc")
   SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /MP")
   set(Boost_USE_STATIC_LIBS   ON)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
   ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
   ADD_DEFINITIONS(-DNOMINMAX)
else()
   ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
endif()

FIND_PACKAGE( Secp256k1 REQUIRED )

INCLUDE_DIRECTORIES(${SECP256K1_INCLUDE_DIR})

FIND_PACKAGE( Boost REQUIRED COMPONENTS
  locale
  chrono
  date_time
  filesystem
  iostreams
  log
  program_options
  regex
  system
  unit_test_framework
  thread
  )
 

include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/include
   )

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
ADD_DEFINITIONS( "-DHAS_BOOST" )
MESSAGE( STATUS "CMAKE_SOURCE_DIR:          " ${CMAKE_SOURCE_DIR} )
MESSAGE( STATUS "Boost_LIBRARY_DIRS:        " ${Boost_LIBRARY_DIRS} )
MESSAGE( STATUS "Boost_INCLUDE_DIR:         " ${Boost_INCLUDE_DIR} )
MESSAGE( STATUS "Boost_LIBRARIES:           " ${Boost_LIBRARIES} )
MESSAGE( STATUS "SECP256K1_LIBRARIES:       " ${SECP256K1_LIBRARIES} )
MESSAGE( STATUS "SECP256K1_INCLUDE_DIR:     " ${SECP256K1_INCLUDE_DIR} )

add_subdirectory(examples)
add_subdirectory(src)
add_subdirectory(test)
