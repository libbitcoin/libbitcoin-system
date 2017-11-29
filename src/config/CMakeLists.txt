
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(config STATIC
   ${MY_SOURCES}
   )

set_target_properties (config PROPERTIES
    FOLDER "libs"
)
 
