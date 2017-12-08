
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
  $<TARGET_OBJECTS:chain>
  $<TARGET_OBJECTS:config>
  $<TARGET_OBJECTS:formats>
  $<TARGET_OBJECTS:log>
  $<TARGET_OBJECTS:machine>
  $<TARGET_OBJECTS:math>
  $<TARGET_OBJECTS:message>
  $<TARGET_OBJECTS:unicode>
  $<TARGET_OBJECTS:utility>
  $<TARGET_OBJECTS:wallet>
  )
set_target_properties (libbitcoin PROPERTIES
  FOLDER "libs"
  )
target_link_libraries(libbitcoin
  ${Boost_LIBRARIES}
  ${SECP256K1_LIBRARIES}
  )

