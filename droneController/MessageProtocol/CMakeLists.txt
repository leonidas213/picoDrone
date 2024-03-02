add_library(WolfLink
WolfLink.cpp

)

target_link_libraries(WolfLink
pico_stdlib
utilities
        )
target_include_directories (WolfLink PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})