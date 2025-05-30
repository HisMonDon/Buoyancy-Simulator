cmake_minimum_required(VERSION 3.20)
project(SimpleBuoyancySimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3.0 REQUIRED COMPONENTS System Window Graphics CONFIG)

add_executable(SimpleBuoyancySimulator main.cpp
)

target_link_libraries(SimpleBuoyancySimulator
        SFML::System
        SFML::Window
        SFML::Graphics
)
