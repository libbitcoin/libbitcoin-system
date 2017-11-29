
add_subdirectory(external)

file(GLOB MY_SOURCES "*.cpp")
ADD_LIBRARY(math STATIC
   ${MY_SOURCES}
   )

set_target_properties (math PROPERTIES
    FOLDER "libs"
)

target_link_libraries(math
  math_external
)
 
