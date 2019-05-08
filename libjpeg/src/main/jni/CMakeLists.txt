
cmake_minimum_required(VERSION 3.4.1)

add_library(
        native-lib1
        SHARED
        native-lib.cpp)

ADD_SUBDIRECTORY(libjpeg-turbo-2.0.2)

find_library(
        log-lib
        log)

target_link_libraries(
        native-lib1
        ${log-lib})