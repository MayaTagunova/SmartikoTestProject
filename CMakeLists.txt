cmake_minimum_required(VERSION 2.8)

project(SmartikoTestProject)

aux_source_directory(client CLIENT_SRC_LIST)
add_executable(${PROJECT_NAME}-client ${CLIENT_SRC_LIST})

aux_source_directory(server SERVER_SRC_LIST)
add_executable(${PROJECT_NAME}-server ${SERVER_SRC_LIST})

include_directories(/usr/include/jsoncpp)

find_library(LIBPAHO paho-mqtt3a)
find_library(LIBPAHO_CPP paho-mqttpp3)
find_library(LIBJSON jsoncpp)
find_library(LIBPQ pq)
find_library(LIBMICROHTTPD microhttpd)

target_link_libraries(${PROJECT_NAME}-client ${LIBPAHO} ${LIBPAHO_CPP} ${LIBJSON} ${LIBMICROHTTPD})
target_link_libraries(${PROJECT_NAME}-server ${LIBPAHO} ${LIBPAHO_CPP} ${LIBJSON} ${LIBPQ} pthread)

set_property(TARGET ${PROJECT_NAME}-client PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME}-server PROPERTY CXX_STANDARD 14)
