
add_executable(tests)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

target_sources(tests
    PRIVATE
        TestInterpolate.cpp
        TestTable2d.cpp
        TestTable2dRatio.cpp
        TestTableBins.cpp
)

target_link_libraries(tests lib gtest gtest_main)
