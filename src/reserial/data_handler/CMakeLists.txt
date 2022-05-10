cmake_minimum_required(VERSION 3.10)

project(RE_DataHandler VERSION 0.1)

option(DEBUG_COMMANDS_ON true)

configure_file(setup_header.h.in setup_header.h)

add_library(RE_DataHandler
                data_handler.h
                protocol.h
                file_handler.cpp
		comdata_handler.cpp)
