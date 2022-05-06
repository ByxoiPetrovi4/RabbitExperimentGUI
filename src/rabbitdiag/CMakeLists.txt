project(RabbitDiag VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)

set(RABBITDIAG_PROJECT rabbitdiag.cpp
    rabbitdiag.h
    rabbitdiag.ui)

qt_add_library(RabbitDiag
                MANUAL_FINALIZATION
                ${RABBITDIAG_PROJECT})

target_link_libraries(RabbitDiag PRIVATE
                Qt::Core
                Qt::Gui
                Qt::Widgets
            )


qt_finalize_target(RabbitDiag)
