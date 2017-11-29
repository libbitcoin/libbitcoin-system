
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(log STATIC
   ${MY_SOURCES}
   )

set_target_properties (log PROPERTIES
    FOLDER "libs"
)
 
