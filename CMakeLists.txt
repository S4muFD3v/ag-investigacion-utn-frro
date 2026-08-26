cmake_minimum_required(VERSION 3.5)
project(AgInvestigacion)

include(FetchContent)

if(CMAKE_HOST_WIN32)
    message("Cloning xlsxio with https")
    FetchContent_Declare(
        xlsxio
        GIT_REPOSITORY https://github.com/brechtsanders/xlsxio.git
        GIT_TAG        0.2.36
    )
elseif(CMAKE_HOST_LINUX)
    message("Cloning xlsxio with ssh")
    FetchContent_Declare(
        xlsxio
        GIT_REPOSITORY git@github.com:brechtsanders/xlsxio.git
        GIT_TAG        0.2.36
    )
endif()


set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(WITH_LIBZIP ON CACHE BOOL "" FORCE)
set(WITH_MINIZIP_NG OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(xlsxio)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_MSVC_RUNTIME_LIBRARY "Multithreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "Multithreaded$<$<CONFIG:Release>:Release>")


file (GLOB sources
    "${CMAKE_SOURCE_DIR}/src/**/*.c"
    "${CMAKE_SOURCE_DIR}/src/**/*.h"
)

add_executable( App
    ${sources}
)

set_target_properties(App PROPERTIES
    COMPILE_DEFINITIONS NIGGA
    OUTPUT_NAME "App.${CMAKE_BUILD_TYPE}"
    DEBUG_POSTFIX "Dbg"
)
