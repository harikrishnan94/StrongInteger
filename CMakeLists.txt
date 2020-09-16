cmake_minimum_required(VERSION 3.12)

project(
    StongInt
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MACOSX_RPATH 1)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake-modules" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

include(SanitizerFlags)
include(WarningFlags)

find_package(Boost 1.72 COMPONENTS SYSTEM)

add_executable(StrongIntTest strong_int_test.cpp)
target_include_directories(StrongIntTest PRIVATE ${Boost_INCLUDE_DIRS})

add_sanitizer_flags(StrongIntTest)
add_warning_flags(StrongIntTest)
