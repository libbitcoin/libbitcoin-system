
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(config OBJECT
   ${MY_SOURCES}
   )

set_target_properties (config PROPERTIES
    FOLDER "libs"
)
 
