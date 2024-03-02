cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)

project(example C CXX ASM)
pico_sdk_init()

add_executable(example
        main.cpp
        )
add_subdirectory(droneController)
pico_enable_stdio_usb( example 1)
pico_enable_stdio_uart( example 1)



add_library(FreeRTOS STATIC
    FreeRTOS-Kernel/event_groups.c
    FreeRTOS-Kernel/list.c
    FreeRTOS-Kernel/queue.c
    FreeRTOS-Kernel/stream_buffer.c
    FreeRTOS-Kernel/tasks.c
    FreeRTOS-Kernel/timers.c
    FreeRTOS-Kernel/portable/MemMang/heap_3.c
    FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c
)
target_include_directories(FreeRTOS PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        FreeRTOS-Kernel/include
        FreeRTOS-Kernel/portable/GCC/ARM_CM0
)
target_link_libraries(example
        Drone
        hardware_i2c
        pico_multicore
        pico_stdlib
        pico_bootsel_via_double_reset
        FreeRTOS
        )
add_compile_options(-Ofast)
pico_add_extra_outputs(example)
