
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(formats OBJECT
   ${MY_SOURCES}
   )

set_target_properties (formats PROPERTIES
    FOLDER "libs"
)
 
