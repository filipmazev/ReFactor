cmake_minimum_required(VERSION 3.19)
project(imgproc)

set(CMAKE_CXX_STANDARD 14)

set(OpenCV_DIR "/opt/homebrew/Cellar/opencv/4.10.0_12/include/opencv4/opencv2")
find_package( OpenCV 4 REQUIRED )
include_directories(include ${OpenCV_INCLUDE_DIRS} )

set(JNI_INCLUDE_DIRS "/Users/<username>/Library/Java/JavaVirtualMachines/openjdk-23.0.1/Contents/Home/include/jni.h")
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

add_definitions(-DDEBUG)

add_executable(imgproc main.cpp)

