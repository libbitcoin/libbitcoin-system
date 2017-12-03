
file(GLOB SOURCES "*.cpp")
file(GLOB PARSE_ENCRYPTED_SOURCES "parse_encrypted_keys/*.cpp")
file(GLOB HEADERS "${LIBBITCOIN_INCLUDE_DIR}/wallet/*.hpp")

ADD_LIBRARY(wallet OBJECT
   ${SOURCES}
   ${PARSE_ENCRYPTED_SOURCES}
   ${HEADERS}
   )

set_target_properties (wallet PROPERTIES
    FOLDER "libs"
)
 
