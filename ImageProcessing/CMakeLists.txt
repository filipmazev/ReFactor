cmake_minimum_required(VERSION 3.19)
project(imgproc)

set(CMAKE_CXX_STANDARD 14)

set(OpenCV_DIR "/opt/homebrew/Cellar/opencv/4.10.0_12/include/opencv4/opencv2")
find_package( OpenCV 4 REQUIRED )
include_directories(include ${OpenCV_INCLUDE_DIRS} )



add_executable(imgproc main.cpp)