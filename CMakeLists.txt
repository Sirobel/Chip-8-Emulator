cmake_minimum_required(VERSION 4.3)
project(Chip_8_Emulator)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 REQUIRED)

add_executable(Chip_8_Emulator
        src/CPU/Cpu.cpp
        src/CPU/Cpu.h
        src/main.cpp
        src/CPU/CpuInstructions.h
        src/Peripherals/Renderer.cpp
        src/Peripherals/Renderer.h
        src/Peripherals/Beeper.h
)

target_link_libraries(Chip_8_Emulator PRIVATE SDL3::SDL3)
