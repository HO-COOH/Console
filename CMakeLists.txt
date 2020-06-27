cmake_minimum_required(VERSION 3.15)

project(Console)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Package
find_package( OpenCV CONFIG REQUIRED )

if(MSVC)
     add_definitions(-DWINDOWS)
endif()

add_executable(Console main.cpp Console.cpp  Shape.cpp "TextVideo.cpp")
target_link_libraries(Console PRIVATE opencv_ml opencv_dnn opencv_flann opencv_features2d opencv_video opencv_videoio opencv_imgcodecs opencv_imgproc opencv_core opencv_highgui)
#target_link_libraries(Console PRIVATE opencv_ml opencv_dnn opencv_core opencv_flann)


add_executable(ConsoleAssist "ConsoleAssist/main2.cpp")
