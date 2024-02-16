install(TARGETS anb 
    EXPORT anb-targets
    FILE_SET anb_public_headers
    ARCHIVE
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "anb-config-version.cmake"
    VERSION ${anb_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT anb-targets
    FILE anb-targets.cmake
    NAMESPACE anb::
    DESTINATION lib/cmake/anb
)

install(FILES
    "anb-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/anb-config-version.cmake"
    DESTINATION lib/cmake/anb
)