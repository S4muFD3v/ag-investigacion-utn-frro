cmake_minimum_required(VERSION 3.14)
project(AgInvestigacion LANGUAGES C)

include(FetchContent)

if(CMAKE_HOST_WIN32)
    set(DEPS_XLSXIO_URL "https://github.com/brechtsanders/xlsxio.git")
elseif(CMAKE_HOST_LINUX)
    set(DEPS_XLSXIO_URL "git@github.com:brechtsanders/xlsxio.git")
endif()

set(BUILD_STATIC ON CACHE BOOL "" FORCE)
set(BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(BUILD_PC_FILES OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(WITH_LIBZIP OFF CACHE BOOL "" FORCE)
set(WITH_MINIZIP_NG OFF CACHE BOOL "" FORCE)
set(WITH_WIDE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    xlsxio
    GIT_REPOSITORY ${DEPS_XLSXIO_URL}
    GIT_TAG 0.2.36
)
FetchContent_MakeAvailable(xlsxio)


set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

add_executable(App ${sources})

target_include_directories(App PRIVATE
    "${xlsxio_SOURCE_DIR}/include"
)

target_link_libraries(App PRIVATE
    xlsxio_read_STATIC
)

set_target_properties(App PROPERTIES
    OUTPUT_NAME "App.${CMAKE_BUILD_TYPE}"
    DEBUG_POSTFIX "Dbg"
)