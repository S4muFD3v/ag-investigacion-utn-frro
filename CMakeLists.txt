cmake_minimum_required(VERSION 3.5)
project(AgInvestigacion)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_MSVC_RUNTIME_LIBRARY "Multithreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "Multithreaded$<$<CONFIG:Release>:Release>")


file (GLOB sources
    "${CMAKE_SOURCE_DIR}/*.c"
    "${CMAKE_SOURCE_DIR}/*.h"
)

add_executable( App
    ${sources}
)

set_target_properties(App PROPERTIES
    COMPILE_DEFINITIONS NIGGA
    OUTPUT_NAME "App.${CMAKE_BUILD_TYPE}"
    DEBUG_POSTFIX "Dbg"
)
