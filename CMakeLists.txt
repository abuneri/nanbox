cmake_minimum_required(VERSION 3.22)

project(anb
    LANGUAGES CXX
    VERSION 0.1.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CTest)

add_subdirectory(src)
if (BUILD_TESTING)
  add_subdirectory(test)
endif()
add_subdirectory(install)
