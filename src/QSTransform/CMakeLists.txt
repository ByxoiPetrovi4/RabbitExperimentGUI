cmake_minimum_required(VERSION 3.5)
include(${CMAKE_SOURCE_DIR}/CMakeMacros.txt)

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

FOREACH(subdir ${SUBDIRS})
  ADD_SUBDIRECTORY(${subdir})
ENDFOREACH()

project(QSTransform VERSION 0.1 LANGUAGES CXX)

SET_DEFAULT_SETTINGS()

set(PROJECT_SOURCES
    qscoordinatesystem.cpp
    qscoordinatesystem.h
    qsorthogonalsystem.cpp
    qsorthogonalsystem.h
    qstransform.cpp
    qstransform.h
    qsorthogonaltransform.cpp
    qsorthogonaltransform.h)

list(APPEND EXTRA_LIBS "${OpenCV_LIBS}")
list(APPEND TEST_FILES "test.json")
list(APPEND TEST_FILES "test1.json")

standard_qt_lib(${PROJECT_NAME} EXTRA_LIBS TEST_FILES})
