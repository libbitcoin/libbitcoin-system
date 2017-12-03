
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/chain/*.hpp")

ADD_LIBRARY(chain OBJECT
   ${SOURCES}
   ${HEADERS}
   )

set_target_properties (chain PROPERTIES
    FOLDER "libs"
)
 
