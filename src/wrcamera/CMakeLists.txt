cmake_minimum_required(VERSION 3.10)

find_package(OpenCV REQUIRED)

project(CameraWriter VERSION 0.1)

add_library(CamerWriter
                wrcamera.h
                wrcamera.cpp)

target_link_libraries(CameraWriter PRIVATE ${OpenCV_LIBS}