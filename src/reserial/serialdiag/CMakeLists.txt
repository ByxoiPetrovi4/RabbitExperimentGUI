project(SerialDiag VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS SerialPort)

set(SERIALDIAG_PROJECT serialdiag.cpp
    serialdiag.h
    serialdiag.ui)

qt_add_library(SerialDiag
                MANUAL_FINALIZATION
                ${SERIALDIAG_PROJECT})

target_link_libraries(SerialDiag PRIVATE
                Qt::Core
                Qt::Gui
                Qt::SerialPort
                Qt::Widgets
            )


qt_finalize_target(SerialDiag)
