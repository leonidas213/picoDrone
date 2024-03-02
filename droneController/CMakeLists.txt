add_library(Drone
Drone.cpp
)

add_subdirectory(globalInc)
add_subdirectory(MotorController)
add_subdirectory(MPU6050)
add_subdirectory(pid)
add_subdirectory(MessageProtocol)
add_subdirectory(SensorController)
target_link_libraries(Drone
pico_stdlib
utilities
MotorCont
pid
WolfLink
globalInc
math3d
SensCont
FreeRTOS
        )
target_include_directories (Drone PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}
)