
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(unicode OBJECT
   ${MY_SOURCES}
   )

set_target_properties (unicode PROPERTIES
    FOLDER "libs"
)
 
