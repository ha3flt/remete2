#**************************************************************
# HTTPS+WSS Server by HA3FLT, HA2OS @ 2022-2023               *
#                                                             *
# License: GNU AGPLv3                                         *
#**************************************************************

cmake_minimum_required(VERSION 3.5)

FILE(GLOB_RECURSE app_sources "${CMAKE_SOURCE_DIR}/src" "*.c")
message(STATUS "The 'app_sources' list is: ${app_sources}")

# ${app_sources} - SRC_DIRS
idf_component_register(SRCS ${app_sources}
               INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include
                            ${CMAKE_SOURCE_DIR}/main/led_signals
             EMBED_TXTFILES ${CMAKE_SOURCE_DIR}/main/_certs/servercert.pem
                            ${CMAKE_SOURCE_DIR}/main/_certs/prvtkey.pem
                            ${CMAKE_SOURCE_DIR}/main/_html/root.html
                            ${CMAKE_SOURCE_DIR}/main/_html/remote_switch.css
                            ${CMAKE_SOURCE_DIR}/main/_html/remote_switch.js)

add_definitions(
    -D SRSW_DEBUG=0
    -D SAFE_REMOTE_SWITCH_VERSION="\"0.8.2\""
    -D SAFE_REMOTE_SWITCH_VERSION_MAJOR=0
    -D SAFE_REMOTE_SWITCH_VERSION_MINOR=8
    -D SAFE_REMOTE_SWITCH_VERSION_BUILD=2
    -D NEOPIXEL_PIN=48
    -D DLED_ACTUAL_LEVEL=1)
