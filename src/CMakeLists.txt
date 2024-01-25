add_subdirectory(libs/raymarch/raymarch)

option (BUILD_tests "Build 'tests' application" true)
if(BUILD_tests)
    add_subdirectory(apps/raymarch_tests/raymarch_tests)
endif()
