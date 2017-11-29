
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(unicode STATIC
   ${MY_SOURCES}
   )

set_target_properties (unicode PROPERTIES
    FOLDER "libs"
)
 
