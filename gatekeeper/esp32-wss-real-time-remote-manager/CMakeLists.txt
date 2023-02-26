#**************************************************************
# HTTPS + WSS Server by HA3FLT, HA2OS @ 2022-2023             *
#                                                             *
# License: GNU AGPLv3                                         *
#**************************************************************

cmake_minimum_required(VERSION 3.5)

#set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)
#message(STATUS "The 'EXTRA_COMPONENT_DIRS' dir is: ${EXTRA_COMPONENT_DIRS}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-wss-real-time-remote-manager)
