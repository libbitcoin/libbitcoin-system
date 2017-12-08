
file(GLOB SOURCES "*.cpp")
file(GLOB EXTERNAL_SOURCES "external/*.c")
file(GLOB EXTERNAL_HEADERS "external/*.h")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/math/*.hpp")

ADD_LIBRARY(math OBJECT
   ${SOURCES}
   ${EXTERNAL_SOURCES}
   ${HEADERS}
   ${EXTERNAL_HEADERS}
   )

set_target_properties (math PROPERTIES
    FOLDER "libs"
)

