
add_executable(tests)

target_sources(tests
    PRIVATE
        TestInterpolate.cpp
)

target_link_libraries(tests lib gtest gtest_main)
