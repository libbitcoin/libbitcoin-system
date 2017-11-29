
file(GLOB MY_SOURCES "*.cpp")
file(GLOB PARSE_ENCRYPTED_SOURCES "parse_encrypted_keys/*.cpp")

ADD_LIBRARY(wallet STATIC
   ${MY_SOURCES}
   ${PARSE_ENCRYPTED_SOURCES}
   )

set_target_properties (wallet PROPERTIES
    FOLDER "libs"
)
 
