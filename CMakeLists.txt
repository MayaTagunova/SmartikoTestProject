cmake_minimum_required(VERSION 2.8)

project(SmartikoTestProject)

aux_source_directory(client CLIENT_SRC_LIST)
add_executable(${PROJECT_NAME}-client ${CLIENT_SRC_LIST})

aux_source_directory(server SERVER_SRC_LIST)
add_executable(${PROJECT_NAME}-server ${SERVER_SRC_LIST})

find_library(LIBPAHO paho-mqtt3as)
find_library(LIBPAHO_CPP paho-mqttpp3)
find_library(LIBJSON jsoncpp)

target_link_libraries(${PROJECT_NAME}-client ${LIBPAHO} ${LIBPAHO_CPP} ${LIBJSON})
target_link_libraries(${PROJECT_NAME}-server ${LIBPAHO} ${LIBPAHO_CPP} ${LIBJSON})

set_property(TARGET ${PROJECT_NAME}-client PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME}-server PROPERTY CXX_STANDARD 14)
