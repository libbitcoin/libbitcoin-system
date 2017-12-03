
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/log/*.hpp")

ADD_LIBRARY(log OBJECT
   ${SOURCES}
   ${HEADERS}
   )

set_target_properties (log PROPERTIES
    FOLDER "libs"
)
 
