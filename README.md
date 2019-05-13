## libjepg-turbo+jnigraphics

#### 介绍

翻译一下官网的描述

libjpeg-turbo是一种jpeg图像编解码器，它使用simd指令（mmx、sse2、avx2、neon、altivec）加速x86、x86-64、ARM和PowerPC系统上的Baseline JPEG（标准型）压缩和解压缩，以及x86和x86-64系统上的渐进式progressive JPEG（渐进式）压缩。在这些系统中，libjpeg-turbo的速度通常是libjpeg的2-6倍，其他的都是相同的。在其他类型的系统上，libjpeg-turbo由于其高度优化的哈夫曼编码例程，仍然可以在很大程度上超过libjpeg 。在许多情况下，libjpeg-turbo的性能与专有的高速JPEG编解码器的性能相当。

libjpeg-turbo最初基于libjpeg/simd，这是 Miyasaka Masaru 开发的libjpeg v6b的MMX（多媒体扩展）加速衍生产品。
2009年，在TigerVnc和VirtualGL项目中对编解码器进行了大量改进，2010年初，libjpeg turbo被拆分成了一个独立的项目，其目标是为更广泛的用户和开发人员提供高速的jpeg压缩/解压缩技术。



#### 编译

编译教程：<https://github.com/libjpeg-turbo/libjpeg-turbo/blob/master/BUILDING.md>

刚开始我也是用linux编译，使用像下面的编译脚本可以编译成功

```shell
#!/bin/bash  
NDK_PATH=/home/shuai/Android/android-ndk-r13
TOOLCHAIN=gcc
ANDROID_VERSION=17
cd libjpeg-turbo-2.0.2

cmake -G "Unix Makefiles" \
 -DANDROID_ABI=armeabi-v7a \
 -DANDROID_ARM_MODE=arm \
 -DANDROID_PLATFORM=android-${ANDROID_VERSION} \
 -DANDROID_TOOLCHAIN=${TOOLCHAIN} \
 -DCMAKE_ASM_FLAGS="--target=arm-linux-androideabi${ANDROID_VERSION}" \
 -DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
 -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=android/arm-v7a \
 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=android/arm-v7a \

make

```



后来发现，可以直接在android studio中直接编译通过，真是开心的像廋了10斤一样

新建带jni的Android项目，将libjpeg-turbo整个项目直接拷贝到jni目录下，然后在CmakeLists.txt中加入这个

```cmake
cmake_minimum_required(VERSION 3.4.1)
add_library(
        native-jpeg
        SHARED
        native-jpeg.cpp)
# 编译子项目（源码路径）        
ADD_SUBDIRECTORY(libjpeg-turbo-2.0.2)

find_library(
        log-lib
        log)
target_link_libraries(
        native-jpeg
        ${log-lib})
```

build之后妥妥可以拿到动态和静态库

同样的方法，我又编译了libpng的代码



