add_library(SensCont
SensorController.cpp

)

target_link_libraries(SensCont
pico_stdlib
math3d
globalInc
mpu6050
        )
target_include_directories (SensCont PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})