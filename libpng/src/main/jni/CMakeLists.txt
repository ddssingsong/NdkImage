
cmake_minimum_required(VERSION 3.4.1)

add_library(ndk-image
        SHARED
        native-lib.cpp
        lpng1637/pngget.c
        lpng1637/pngread.c
        lpng1637/pngrutil.c
        lpng1637/pngtrans.c
        lpng1637/pngwtran.c
        lpng1637/png.c
        lpng1637/pngmem.c
        lpng1637/pngrio.c
        lpng1637/pngset.c
        lpng1637/pngwio.c
        lpng1637/pngwutil.c
        lpng1637/pngerror.c
        lpng1637/pngpread.c
        lpng1637/pngrtran.c
        lpng1637/pngwrite.c
        lpng1637/pngwtran.c
        lpng1637/pngwutil.c)

target_link_libraries(ndk-image log android z )


