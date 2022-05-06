cmake_minimum_required(VERSION 3.16)

project(RabbitExperimentGUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets SerialPort REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets SerialPort REQUIRED)
find_package(OpenCV REQUIRED)

set(MAIN_PROJECT main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui)

qt_add_executable(RabbitExperimentGUI
                MANUAL_FINALIZATION
                ${MAIN_PROJECT})

add_subdirectory(src)

list(APPEND EXTRA_LIBS SerialDiag
                        RabbitDiag)
list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/src/serialdiag"
                        "${PROJECT_SOURCE_DIR}/src/rabbitdiag")

set_target_properties(RabbitExperimentGUI PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_include_directories(RabbitExperimentGUI PRIVATE ${EXTRA_INCLUDES})

target_link_libraries(RabbitExperimentGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                    Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(RabbitExperimentGUI PRIVATE ${OpenCV_LIBS}
                        ${EXTRA_LIBS})

qt_finalize_executable(RabbitExperimentGUI)
