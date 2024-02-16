add_library(anb
    object.cpp
)
add_library(anb::anb
    ALIAS anb
)

set(ANB_INCLUDE_ROOT_DIR ${PROJECT_SOURCE_DIR}/include)
set(ANB_INCLUDE_PROJ_DIR ${ANB_INCLUDE_ROOT_DIR}/anb)
target_sources(anb
    PUBLIC
        FILE_SET anb_public_headers
        TYPE HEADERS
        BASE_DIRS ${ANB_INCLUDE_ROOT_DIR}/
        FILES
            ${ANB_INCLUDE_PROJ_DIR}/heap_object.hpp
            ${ANB_INCLUDE_PROJ_DIR}/dictionary.hpp
            ${ANB_INCLUDE_PROJ_DIR}/list.hpp
            ${ANB_INCLUDE_PROJ_DIR}/object.hpp
            ${ANB_INCLUDE_PROJ_DIR}/string.hpp
            ${ANB_INCLUDE_PROJ_DIR}/detail/nanbox.hpp
            ${ANB_INCLUDE_PROJ_DIR}/detail/polyfill.hpp
            ${ANB_INCLUDE_PROJ_DIR}/detail/util.hpp
)
target_include_directories(anb
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)
