
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(utility OBJECT
   ${MY_SOURCES}
   )

set_target_properties (utility PROPERTIES
    FOLDER "libs"
)
 
