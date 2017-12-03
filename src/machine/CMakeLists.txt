
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/machine/*.hpp")

ADD_LIBRARY(machine OBJECT
   ${SOURCES}
   ${MACHINE}
   )

set_target_properties (machine PROPERTIES
    FOLDER "libs"
)
 
