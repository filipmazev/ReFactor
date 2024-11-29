cmake_minimum_required(VERSION 3.19)
project(imgproc)

set(CMAKE_CXX_STANDARD 14)

set(OpenCV_DIR "C:\opencv\build\include\opencv2")
find_package( OpenCV 4 REQUIRED )
include_directories(include ${OpenCV_INCLUDE_DIRS} )

add_definitions(-DDEBUG)

add_executable(imgproc main.cpp)

