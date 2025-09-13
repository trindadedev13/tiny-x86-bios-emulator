cmake_minimum_required(VERSION 3.22)

include("txbe.cmake")
include(GNUInstallDirs)

project(txbe VERSION 1.0.0)

set(CORE_SOURCES
    boot.c
    cpu.c
    emu.c
    event.c
    int10.c)

add_library(libtxbe
            STATIC
            ${CORE_SOURCES})

target_include_directories(libtxbe
                           PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include/txbe>)

set(BACKEND "stdout" CACHE STRING "Backend: stdout")
set_property(CACHE BACKEND PROPERTY STRINGS stdout sdl)

add_txbe_backend(stdout
                 "backend_stdout.c"
                 "" "")

get_target_property(BACKEND_SOURCES
                    txbe_backend_${BACKEND}
                    BACKEND_SOURCES)

set(E_SOURCES
    main.c
    ${BACKEND_SOURCES})

add_executable(txbe
               ${E_SOURCES})

target_link_libraries(txbe
                     libtxbe
                     txbe_backend_${BACKEND})

install(TARGETS libtxbe
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

install(TARGETS txbe
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION include/txbe
        FILES_MATCHING PATTERN "*.h"
        PATTERN "build" EXCLUDE
        PATTERN ".*" EXCLUDE)

install(EXPORT ${PROJECT_NAME}Targets
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})