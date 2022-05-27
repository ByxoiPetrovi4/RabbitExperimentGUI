cmake_minimum_required(VERSION 3.10)

find_package(OpenCV REQUIRED)
find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)

project(CameraWriter VERSION 0.1)

set(WRCAMERA_PROJECT wrcamera.h
                wrwindow.h
                wrcamera.cpp
                wrwindow.cpp
                wrwindow.ui)

qt_add_library(CameraWriter
        MANUAL_FINALIZATION
        ${WRCAMERA_PROJECT})

target_link_libraries(CameraWriter PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ${OpenCV_LIBS})

qt_finalize_target(CameraWriter)
