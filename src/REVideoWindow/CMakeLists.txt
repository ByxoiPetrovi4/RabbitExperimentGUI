cmake_minimum_required(VERSION 3.5)
include(${CMAKE_SOURCE_DIR}/CMakeMacros.txt)

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

FOREACH(subdir ${SUBDIRS})
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

project(REVideoWindow VERSION 0.1 LANGUAGES CXX)

SET_DEFAULT_SETTINGS()

set(PROJECT_SOURCES
    videowindow.cpp
    videowindow.h
    videowindow.ui
)

list(APPEND EXTRA_LIBS "QSVideo" "QSFrameViewier" "${OpenCV_LIBS}")
list(APPEND TEST_FILES "test.json")
list(APPEND TEST_FILES "test1.json")

standard_qt_lib(${PROJECT_NAME} EXTRA_LIBS TEST_FILES})
