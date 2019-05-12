
cmake_minimum_required(VERSION 3.4.1)


include_directories(include)

# 設置輸出路徑為项目目录下
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../../../jniLibs/${CMAKE_ANDROID_ARCH_ABI})


add_library( # Sets the name of the library.
        native-image
        SHARED
        native-image.cpp
        jpegcompress.cpp
        libjpeg_jni/turbojpeg-jni.c)

add_library(libjpeg STATIC IMPORTED)
set_target_properties(libjpeg
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libturbojpeg.a)


find_library( # Sets the name of the path variable.
        log-lib
        log)

target_link_libraries( # Specifies the target library.
        native-image
        jnigraphics
        ${log-lib}
        libjpeg)