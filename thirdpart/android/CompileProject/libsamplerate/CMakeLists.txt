cmake_minimum_required(VERSION 3.4.1)

set(SRC_PATH ${CMAKE_SOURCE_DIR}/../../../src/libsamplerate-0.1.9/src)
set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/../../../src/libsamplerate-0.1.9/src)

if (${CMAKE_BUILD_TYPE} STREQUAL "debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")


file(GLOB sdk_sources
        ${SRC_PATH}/*.c
    )


add_library(
        samplerate
        STATIC
        ${sdk_sources}
    )


target_include_directories(
        samplerate
        PRIVATE
        ${INCLUDE_PATH}
    )

add_executable( samplerateexe ${CMAKE_SOURCE_DIR}/src/main/jni/samplerate.c )

target_include_directories(
        samplerateexe
        PRIVATE
        ${INCLUDE_PATH}
)

target_link_libraries(
        samplerateexe
        samplerate
)