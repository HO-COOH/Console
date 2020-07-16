cmake_minimum_required(VERSION 3.15)

project(Console)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Detect operating system and compiler
if(MSVC)
     add_definitions(-DWINDOWS)
else()
    add_definitions(-DLINUX)
endif()
message(${CMAKE_CURRENT_BINARY_DIR})

include_directories("include")
add_subdirectory(application)

# ConsoleAssist target
add_executable(ConsoleAssist "ConsoleAssist/ConsoleAssist.cpp")

# Main testing target


if(MSVC)
find_package( OpenCV CONFIG REQUIRED )
include_directories("include")
file(GLOB AllSource "source/*.cpp")
add_executable(main main.cpp ${AllSource})
target_link_libraries(main
    PRIVATE
    opencv_ml
    opencv_dnn
    opencv_flann
    opencv_features2d
    opencv_video
    opencv_imgcodecs
    opencv_imgproc
    opencv_core
    opencv_highgui
)
else()
find_package( OpenCV CONFIG REQUIRED )
add_executable(main main.cpp "source/Console.cpp")
target_link_libraries(main
    PRIVATE
    opencv_ml
    opencv_dnn
    opencv_flann
    opencv_features2d
    opencv_video
    opencv_imgcodecs
    opencv_imgproc
    opencv_core
    opencv_highgui
     ncurses
)
endif()