enable_testing()

#===============================================
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # 1.14.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

#===============================================
add_executable(anb_test
    test_allocator.hpp

    test_boolean.cpp
    test_dictionary.cpp
    test_float64.cpp
    test_int32.cpp
    test_list.cpp
    test_nothing.cpp
    test_qnan.cpp
    test_string_heap.cpp
    test_string_sso.cpp
)
target_link_libraries(anb_test
    anb
    GTest::GTest
)
add_test(anb_tests
    anb_test
)
