project(RESerial VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS SerialPort)

add_subdirectory(data_handler)

add_subdirectory(serialdiag)

set(RESERIAL_PROJECT reserial.cpp
    reserial.h)

qt_add_library(RESerial
                MANUAL_FINALIZATION
                ${RESERIAL_PROJECT})

list(APPEND EXTRA_LIBS SerialDiag RE_DataHandler)

list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/serialdiag"
			"${PROJECT_SOURCE_DIR}/data_handler"
                        )

target_include_directories(RESerial PRIVATE ${EXTRA_INCLUDES})

target_link_libraries(RESerial PRIVATE ${EXTRA_LIBS})

target_link_libraries(RESerial PRIVATE
                Qt::Core
                Qt::Gui
                Qt::SerialPort
                Qt::Widgets
            )


qt_finalize_target(RESerial)
