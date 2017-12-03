
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(log OBJECT
   ${MY_SOURCES}
   )

set_target_properties (log PROPERTIES
    FOLDER "libs"
)
 
