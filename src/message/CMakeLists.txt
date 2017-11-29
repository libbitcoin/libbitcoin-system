
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(message STATIC
   ${MY_SOURCES}
   )

set_target_properties (message PROPERTIES
    FOLDER "libs"
)
 
