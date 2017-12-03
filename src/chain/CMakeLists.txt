
file(GLOB MY_SOURCES "*.cpp")

ADD_LIBRARY(chain OBJECT
   ${MY_SOURCES}
   )

set_target_properties (chain PROPERTIES
    FOLDER "libs"
)
 
