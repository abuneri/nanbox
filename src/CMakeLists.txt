add_library(anb
    ${PROJECT_SOURCE_DIR}/include/anb/dictionary.hpp
    ${PROJECT_SOURCE_DIR}/include/anb/list.hpp
    ${PROJECT_SOURCE_DIR}/include/anb/object.hpp
    ${PROJECT_SOURCE_DIR}/include/anb/string.hpp
    
    ${PROJECT_SOURCE_DIR}/include/anb/detail/heap_object.hpp
    ${PROJECT_SOURCE_DIR}/include/anb/detail/nanbox.hpp
    ${PROJECT_SOURCE_DIR}/include/anb/detail/polyfill.hpp
    ${PROJECT_SOURCE_DIR}/include/anb/detail/util.hpp
    
    object.cpp
)
add_library(anb::anb
    ALIAS anb
)
target_include_directories(anb
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>e>
)
