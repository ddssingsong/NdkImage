
cmake_minimum_required(VERSION 3.4.1)

include_directories(lpng1637)
include_directories(zlib-1.2.11)
add_library( # Sets the name of the library.
        native-lib
        SHARED
        native-lib.cpp
        pngcompress.c
        lpng1637/png.c
        lpng1637/pngget.c
        lpng1637/pngerror.c
        lpng1637/pngmem.c
        lpng1637/pngpread.c
        lpng1637/pngrio.c
        lpng1637/pngrtran.c
        lpng1637/pngrutil.c
        lpng1637/pngwrite.c
        lpng1637/pngwutil.c
        lpng1637/pngwio.c
        lpng1637/pngtest.c
        lpng1637/pngset.c
        lpng1637/pngwtran.c
        lpng1637/pngtrans.c)


find_library(
        log-lib
        log)


target_link_libraries(
        native-lib
        ${log-lib}  android z)