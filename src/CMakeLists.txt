cmake_minimum_required(VERSION 3.4.1)



set(LIBS_PATH ${CMAKE_SOURCE_DIR}/../thirdpart/android/libs)
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../thirdpart/src)

add_library(lib_yuv STATIC IMPORTED)
set_target_properties(lib_yuv PROPERTIES IMPORTED_LOCATION
    ${LIBS_PATH}/${ANDROID_ABI}/libyuv.a)


if (${CMAKE_BUILD_TYPE} STREQUAL "debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSR_LOG_DEBUG -DSR_MALLOC_BACKTRACE -DSR_MALLOC_PAGE_SIZE=0xA00000 -DSR_MALLOC_MAX_POOL=16")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSR_LOG_DEBUG -std=c++14")


file(GLOB sdk_sources
    sr_malloc/src/*.c
    sr_malloc/src/*.cpp
    framework/*.c
    framework/jni/*.cpp
    framework/jni/*.c
    framework/base/*.cpp
    framework/context/*.cpp
    framework/editor/*.cpp
    framework/player/*.cpp
    framework/opengles/*.cpp
    framework/opengles/android/*.cpp
    framework/opengles/globjects/*.cpp
    framework/opengles/glshapes/*.cpp
    framework/device/*.cpp
    framework/device/android/*.cpp
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
    framework/jni
    framework/base
    framework/context
    framework/editor
    framework/player
    framework/opengles
    framework/opengles/android
    framework/opengles/globjects
    framework/opengles/glshapes
    framework/device
    framework/device/android
    sr_malloc/include
    ${INCLUDE_PATH}/libyuv/include
    ${INCLUDE_PATH}/glm
#    ${INCLUDE_PATH}/glm/glm
#    ${INCLUDE_PATH}/glm/glm/gtx
#    ${INCLUDE_PATH}/glm/glm/gtc
#    ${INCLUDE_PATH}/glm/glm/detail
#    ${INCLUDE_PATH}/glm/glm/simd
#    ${INCLUDE_PATH}/glm/glm/ext
    )


target_link_libraries(
    freeeditor

    lib_yuv

    log
    EGL
    GLESv1_CM
    GLESv3
    android
    )
