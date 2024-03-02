add_library(MotorCont
        motorController.cpp

)

target_link_libraries(MotorCont
pico_stdlib
utilities
        )
target_include_directories (MotorCont PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})