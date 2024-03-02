add_library(globalInc
        global_inc.cpp

)

target_link_libraries(globalInc
pico_stdlib
pid
        )
target_include_directories (globalInc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})