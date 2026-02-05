cmake_minimum_required(VERSION 3.20)

project(include/segmented_buffer
        VERSION 0.1.0
        LANGUAGES CXX
)

add_library(segmented_buffer INTERFACE
        include/segmented_buffer/segmented_buffer.hpp
        tests/segmented_buffer_test.cpp)

add_library(segmented_buffer::segmented_buffer ALIAS segmented_buffer)

target_compile_features(segmented_buffer INTERFACE cxx_std_20)

target_include_directories(segmented_buffer INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

option(SEGMENTED_BUFFER_BUILD_TESTS "Build tests" ON)

if(SEGMENTED_BUFFER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
