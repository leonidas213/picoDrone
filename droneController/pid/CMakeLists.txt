add_library(pid
PID_v1.cpp

)

target_link_libraries(pid
pico_stdlib
math3d
globalInc
utilities
        )
target_include_directories (pid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})