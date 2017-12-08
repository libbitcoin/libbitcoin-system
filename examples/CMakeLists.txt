
add_executable(example main.cpp)

set_target_properties (example PROPERTIES
    FOLDER "examples"
)
TARGET_LINK_LIBRARIES(example
   libbitcoin
)


