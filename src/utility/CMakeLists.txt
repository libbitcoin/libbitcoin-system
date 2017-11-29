
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(utility STATIC
   ${MY_SOURCES}
   )

set_target_properties (utility PROPERTIES
    FOLDER "libs"
)
 
