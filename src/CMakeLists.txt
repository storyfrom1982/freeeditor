cmake_minimum_required(VERSION 3.4.1)



set(LIBS_PATH ${CMAKE_SOURCE_DIR}/../thirdpart/android/libs)
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../thirdpart/src)

add_library(lib_samplerate STATIC IMPORTED)
set_target_properties(lib_samplerate PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libsamplerate.a)

add_library(lib_fdkaac STATIC IMPORTED)
set_target_properties(lib_fdkaac PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libFraunhoferAAC.a)

add_library(lib_opus STATIC IMPORTED)
set_target_properties(lib_opus PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libopus.a)

add_library(lib_x264 STATIC IMPORTED)
set_target_properties(lib_x264 PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libx264.a)

add_library(lib_yuv STATIC IMPORTED)
set_target_properties(lib_yuv PROPERTIES IMPORTED_LOCATION
    ${LIBS_PATH}/${ANDROID_ABI}/libyuv.a)

add_library(lib_avcodec STATIC IMPORTED)
set_target_properties(lib_avcodec PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libavcodec.a)

add_library(lib_avutil STATIC IMPORTED)
set_target_properties(lib_avutil PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libavutil.a)

add_library(lib_avformat STATIC IMPORTED)
set_target_properties(lib_avformat PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libavformat.a)

add_library(lib_swresample STATIC IMPORTED)
set_target_properties(lib_swresample PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libswresample.a)

add_library(lib_swscale STATIC IMPORTED)
set_target_properties(lib_swscale PROPERTIES IMPORTED_LOCATION
        ${LIBS_PATH}/${ANDROID_ABI}/libswscale.a)


if (${CMAKE_BUILD_TYPE} STREQUAL "debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSR_LOG_DEBUG -D__SR_MALLOC__ -DSR_MALLOC_BACKTRACE -DSR_MALLOC_PAGE_SIZE=0xA00000 -DSR_MALLOC_MAX_POOL=16")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSR_LOG_DEBUG -DSR_MALLOC_PAGE_SIZE=0xA00000 -DSR_MALLOC_MAX_POOL=16")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSR_LOG_DEBUG -D__SR_MALLOC__ -std=c++14")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSR_LOG_DEBUG -std=c++14")


file(GLOB sdk_sources
        sr_malloc/src/*.c
        sr_malloc/src/*.cpp
        framework/*.c
        framework/jni/*.c
        framework/jni/*.cpp
        framework/base/*.c
        framework/base/*.cpp
        framework/message/*.cpp
        framework/context/*.c
        framework/context/*.cpp
        framework/opengles/*.c
        framework/opengles/*.cpp
        framework/media/*.c
        framework/media/*.cpp
        framework/media/ffmpeg/*.cpp
        framework/media/encoder/*.cpp
        framework/media/resampler/*.c
        framework/media/resampler/*.cpp
        framework/program/*.cpp
    )


add_library(
        freeeditor
        SHARED
        ${sdk_sources}
    )


target_include_directories(
        freeeditor
        PRIVATE
        nlohmann
        framework
        framework/base
        framework/message
        framework/media
        framework/media/ffmpeg
        framework/media/encoder
        framework/media/resampler
        framework/jni
        framework/context
        framework/opengles
        framework/program
        sr_malloc/include
        ${INCLUDE_PATH}/include
        ${INCLUDE_PATH}/libyuv/include
        ${INCLUDE_PATH}/libsamplerate-0.1.9/src
        ${INCLUDE_PATH}/glm
    )


target_link_libraries(
        freeeditor

        lib_yuv
        lib_x264
        lib_opus
        lib_fdkaac
        lib_samplerate

        lib_avformat
        lib_avcodec
        lib_swresample
        lib_swscale
        lib_avutil

        z
        log
        EGL
        GLESv1_CM
        GLESv3
        android
    )
