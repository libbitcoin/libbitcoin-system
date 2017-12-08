
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/formats/*.hpp")

ADD_LIBRARY(formats OBJECT
   ${SOURCES}
   ${HEADERS}
   )

set_target_properties (formats PROPERTIES
    FOLDER "libs"
)
 
