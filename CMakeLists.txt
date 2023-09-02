cmake_minimum_required(VERSION 3.5)

project(RabbitExperimentGUI VERSION 0.2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS "-o3")

if(WIN32)
    set(OpenCV_DIR "C:\\OpenCV\\build\\")
endif(WIN32)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(OpenCV REQUIRED)
find_package(RtAudio REQUIRED)

set(MAIN_PROJECT main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui)

qt_add_executable(RabbitExperimentGUI
                MANUAL_FINALIZATION
                ${MAIN_PROJECT})

add_subdirectory(src)

list(APPEND EXTRA_LIBS SerialDiag
                        RabbitDiag
                        RE_DataHandler
                        RESerial
                        CameraWriter
                        QSVideo
                        QSTransform
                        QSFrameViewier
                        REVideoWindow
                        REWAVAudioWriter)

list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/src/reserial/data_handler"
                        "${PROJECT_SOURCE_DIR}/src/reserial/serialdiag"
                        "${PROJECT_SOURCE_DIR}/src/rabbitdiag"
                        "${PROJECT_SOURCE_DIR}/src/reserial"
                        "${PROJECT_SOURCE_DIR}/src/wrcamera"
                        )

set_target_properties(RabbitExperimentGUI PROPERTIES
    WIN32_EXECUTABLE TRUE
)

install(TARGETS RabbitExperimentGUI
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/)

install(TARGETS ${EXTRA_LIBS}
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin/)

link_directories(${CMAKE_SOURCE_DIR}/lib)


target_include_directories(RabbitExperimentGUI PRIVATE ${EXTRA_INCLUDES})

target_link_libraries(RabbitExperimentGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                    Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(RabbitExperimentGUI PRIVATE RtAudio::rtaudio ${OpenCV_LIBS}
                        ${EXTRA_LIBS})

qt_finalize_executable(RabbitExperimentGUI)
