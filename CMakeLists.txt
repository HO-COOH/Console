cmake_minimum_required(VERSION 3.15)

project(Console)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Package
find_package( OpenCV REQUIRED )



add_executable(Console main.cpp Console.cpp  Shape.cpp "TextVideo.cpp")
if( OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  # Additional Library Directories
  link_directories( ${OpenCV_LIB_DIR} )

  # Additional Dependencies
  target_link_libraries( Console ${OpenCV_LIBS} )
endif()



add_executable(ConsoleAssist "ConsoleAssist/main2.cpp")
