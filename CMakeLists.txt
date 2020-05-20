cmake_minimum_required(VERSION 3.15)

project(Console)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Console main.cpp Console.cpp ConsoleEngine.cpp Shape.cpp)
add_executable(ConsoleAssist main2.cpp)