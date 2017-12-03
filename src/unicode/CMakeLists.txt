
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/unicode/*.hpp")

ADD_LIBRARY(unicode OBJECT
   ${SOURCES}
   ${HEADERS}
   )

set_target_properties (unicode PROPERTIES
    FOLDER "libs"
)
 
