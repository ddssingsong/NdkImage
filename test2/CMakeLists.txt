
cmake_minimum_required(VERSION 3.4.1)

add_library( ndk-image
             SHARED
             src/main/jni/ndk-image.c
             src/main/jni/jpegcompress.c
             src/main/jni/pngcompress.c
              )


 include_directories(src/main/jni)

#====================png========================
 add_library(
     png STATIC
     src/main/jni/libpng/pngget.c
     src/main/jni/libpng/pngread.c
     src/main/jni/libpng/pngrutil.c
     src/main/jni/libpng/pngtrans.c
     src/main/jni/libpng/pngwtran.c
     src/main/jni/libpng/png.c
     src/main/jni/libpng/pngmem.c
     src/main/jni/libpng/pngrio.c
     src/main/jni/libpng/pngset.c
     src/main/jni/libpng/pngwio.c
     src/main/jni/libpng/pngwutil.c
     src/main/jni/libpng/pngerror.c
     src/main/jni/libpng/pngpread.c
     src/main/jni/libpng/pngrtran.c
     src/main/jni/libpng/pngwrite.c
 )

 set_target_properties(
     png
     PROPERTIES
     ARCHIVE_OUTPUT_DIRECTORY
     "${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}"
 )

 include_directories(src/main/jni/libpng)

 #====================z=======================
 add_library(
      z STATIC
      src/main/jni/zlib/adler32.c
      src/main/jni/zlib/compress.c
      src/main/jni/zlib/crc32.c
      src/main/jni/zlib/deflate.c
      src/main/jni/zlib/gzclose.c
      src/main/jni/zlib/gzlib.c
      src/main/jni/zlib/gzread.c
      src/main/jni/zlib/gzwrite.c
      src/main/jni/zlib/infback.c
      src/main/jni/zlib/inflate.c
      src/main/jni/zlib/inftrees.c
      src/main/jni/zlib/inffast.c
      src/main/jni/zlib/trees.c
      src/main/jni/zlib/uncompr.c
      src/main/jni/zlib/zutil.c

  )

  set_target_properties(
      z
      PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY
      "${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}"
  )

  include_directories(src/main/jni/zlib)

 target_link_libraries(
      png
      z
 )

 #=============================libjpeg-turbo=========================


 add_library( turbojpeg
              STATIC
              IMPORTED )
 set_target_properties( turbojpeg
                        PROPERTIES IMPORTED_LOCATION
                        ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libturbojpeg.a )


  add_library( jpeg
               STATIC
               IMPORTED )
  set_target_properties( jpeg
                         PROPERTIES IMPORTED_LOCATION
                         ${CMAKE_CURRENT_SOURCE_DIR}/libs/${ANDROID_ABI}/libjpeg.a )

  include_directories(src/main/jni/jpeg)





find_library(log-lib log )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

target_link_libraries( ndk-image turbojpeg jpeg png jnigraphics ${log-lib} )


