
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/utility/*.hpp")

ADD_LIBRARY(utility OBJECT
   ${SOURCES}
   ${HEADERS}
   )

set_target_properties (utility PROPERTIES
    FOLDER "libs"
)
 
