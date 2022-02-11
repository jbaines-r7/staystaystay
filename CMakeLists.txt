cmake_minimum_required(VERSION 3.2)

set(PROJECT_NAME staystaystay)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "-g -O2 -std=c++17 -Wall -Wextra -Wshadow -Wno-unused-result -Wcast-align -Wcast-qual -Woverloaded-virtual -Wstrict-null-sentinel -Wswitch-default -Winit-self -Wlogical-op -Wno-deprecated-declarations")

# compilation units
add_executable(${PROJECT_NAME}
                src/main.cpp)

# linking comp / libs
target_link_libraries(${PROJECT_NAME})