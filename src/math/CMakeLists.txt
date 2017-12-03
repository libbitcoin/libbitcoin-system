
file(GLOB MY_SOURCES "*.cpp")
file(GLOB EXTERNAL_SOURCES "external/*.c")
ADD_LIBRARY(math OBJECT
   ${MY_SOURCES}
   ${EXTERNAL_SOURCES}
   )

set_target_properties (math PROPERTIES
    FOLDER "libs"
)

