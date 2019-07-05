cmake_minimum_required(VERSION 3.4.1)



if (${CMAKE_BUILD_TYPE} STREQUAL "debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSR_LOG_DEBUG -DSR_MALLOC_BACKTRACE -DSR_MALLOC_PAGE_SIZE=0xA00000 -DSR_MALLOC_MAX_POOL=16")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


set(SRC_DIR ${CMAKE_SOURCE_DIR})


file(GLOB sdk_sources 
    sr_malloc/src/*.c
    sr_malloc/src/*.cpp
    )


add_library(
    freeeditor
    SHARED
    ${sdk_sources}
    )


target_include_directories(
    freeeditor
    PRIVATE
    sr_malloc/include
    )


target_link_libraries(
    freeeditor

    log
    android
    )
