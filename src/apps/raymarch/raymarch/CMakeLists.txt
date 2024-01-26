string(TOLOWER ${PROJECT_NAME} _lower_project_name)
set(TARGET_NAME ${_lower_project_name})

configure_file(build.h.in build.h @ONLY)

set(${TARGET_NAME}_HEADERS
    Program.h
)

set(${TARGET_NAME}_SOURCES
    main.cpp
)

add_executable(${TARGET_NAME}
               ${${TARGET_NAME}_HEADERS}
               ${${TARGET_NAME}_SOURCES}
)

cmc_target_current_include_directory(${TARGET_NAME} ADD_BINARY_DIR PRIVATE)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(${TARGET_NAME}
    glad::glad
    glfw
)

set_target_properties(${TARGET_NAME} PROPERTIES
                      VERSION "${${PROJECT_NAME}_VERSION}"
)

cmc_cpp_all_warnings_as_errors(${TARGET_NAME} ENABLED ${BUILD_CONF_WarningAsError})

cmc_cpp_sanitizer(${TARGET_NAME} ${BUILD_CONF_Sanitizer})


##
## Install
##

install(TARGETS ${TARGET_NAME})
