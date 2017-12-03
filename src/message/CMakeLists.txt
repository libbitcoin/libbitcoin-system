
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/message/*.hpp")

ADD_LIBRARY(message OBJECT
   ${SOURCES}
   ${HEADERS}
   )

set_target_properties (message PROPERTIES
    FOLDER "libs"
)
 
