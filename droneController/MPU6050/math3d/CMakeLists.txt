#add_subdirectory(examples)
add_library(math3d
         helper_3dmath.h
         
)

target_link_libraries(math3d
pico_stdlib 


)
target_include_directories (math3d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
