#include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/VersionsCheck.cmake")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_subdirectory(serialdiag)

add_subdirectory(rabbitdiag)

message(${QT_VERSION_MAJOR})


target_link_libraries(RabbitExperimentGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                    Qt${QT_VERSION_MAJOR}::SerialPort)
target_link_libraries(RabbitExperimentGUI PRIVATE ${OpenCV_LIBS} )

target_link_libraries(RabbitExperimentGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
