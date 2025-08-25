cmake_minimum_required(VERSION 3.22)

include("txbe.cmake")

project(txbe)

set(CORE_SOURCES
    boot.c
    cpu.c
    emu.c
    event.c
    int10.c
    thread.c
)

add_library(txbe
            STATIC
            ${CORE_SOURCES})

target_include_directories(txbe
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR})

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

add_executable(txbe_e
               ${E_SOURCES})

target_link_libraries(txbe_e
                     txbe
                     txbe_backend_${BACKEND})

install(TARGETS txbe
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

install(TARGETS txbe_e
        RUNTIME DESTINATION bin
        RENAME txbe)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION include/txbe
        FILES_MATCHING PATTERN "*.h"
        PATTERN ".*" EXCLUDE)