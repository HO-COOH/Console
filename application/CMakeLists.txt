# Find Package
find_package( OpenCV CONFIG REQUIRED )

add_executable(ConsoleVideo ConsoleVideo.cpp "../source/Console.cpp"  "../source/Shape.cpp" "../source/TextVideo.cpp" "FileExplorer.cpp")

add_executable(ConwayGameOfLife ConwayGameOfLife.cpp "../source/Console.cpp" "../source/Shape.cpp")

target_link_libraries(ConsoleVideo
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

add_executable(FileExplorer FileExplorer.cpp)