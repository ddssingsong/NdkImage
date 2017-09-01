
cmake_minimum_required(VERSION 3.4.1)

add_library( ndk-image
             SHARED
             src/main/jni/ndk-image.c )

find_library(log-lib log )

set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../libs)


add_library( jpeg
             SHARED
             IMPORTED )
set_target_properties( jpeg
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/armeabi/libjpeg.so )

include_directories(src/main/jni/jpeg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")


target_link_libraries( ndk-image jpeg jnigraphics ${log-lib} )
