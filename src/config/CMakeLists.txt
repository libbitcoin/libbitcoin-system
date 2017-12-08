
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/config/*.hpp")

ADD_LIBRARY(config OBJECT
   ${SOURCES}
   ${HEADERS}
   )

set_target_properties (config PROPERTIES
    FOLDER "libs"
)
 
