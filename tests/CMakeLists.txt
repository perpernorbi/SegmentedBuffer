include(FetchContent)

# On Windows, gtest recommends this to avoid overriding the parent project's compiler flags
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        # Pick a known stable tag; you can update anytime
        GIT_TAG v1.14.0
)

FetchContent_MakeAvailable(googletest)

add_executable(segmented_buffer_tests
        segmented_buffer_test.cpp
)

target_link_libraries(segmented_buffer_tests
        PRIVATE
        segmented_buffer::segmented_buffer
        GTest::gmock_main
)

target_compile_features(segmented_buffer_tests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(segmented_buffer_tests)
