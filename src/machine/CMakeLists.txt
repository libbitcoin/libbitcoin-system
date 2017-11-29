
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(machine STATIC
   ${MY_SOURCES}
   )

set_target_properties (machine PROPERTIES
    FOLDER "libs"
)
 
