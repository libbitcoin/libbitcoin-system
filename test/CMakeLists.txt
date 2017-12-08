
set(TEST_SOURCES "main.cpp")

add_subdirectory(chain)
add_subdirectory(config)
add_subdirectory(formats)
add_subdirectory(machine)
add_subdirectory(math)
add_subdirectory(message)
add_subdirectory(unicode)
add_subdirectory(utility)
add_subdirectory(wallet)

ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
add_executable(unit_tests ${TEST_SOURCES})

set_target_properties (unit_tests PROPERTIES
    FOLDER "test"
)
TARGET_LINK_LIBRARIES(unit_tests
   libbitcoin
)

