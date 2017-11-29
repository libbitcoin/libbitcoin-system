
add_subdirectory(chain)
add_subdirectory(config)
add_subdirectory(formats)
add_subdirectory(log)
add_subdirectory(machine)
add_subdirectory(math)
add_subdirectory(message)
add_subdirectory(unicode)
add_subdirectory(utility)
add_subdirectory(wallet)


ADD_LIBRARY(libbitcoin STATIC
  error.cpp
  )
set_target_properties (libbitcoin PROPERTIES
  FOLDER "libs"
  )
target_link_libraries(libbitcoin
  chain
  config
  formats
  log
  machine
  math
  message
  unicode
  utility
  wallet
  ${Boost_LIBRARIES}
  ${SECP256K1_LIBRARIES}
  )

