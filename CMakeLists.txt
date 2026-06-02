cmake_minimum_required(VERSION 3.21)

project(
    LoxInterp
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(LoxInterp_core STATIC src/scanner.cpp)
target_include_directories(LoxInterp_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_compile_features(LoxInterp_core PUBLIC cxx_std_23)
target_compile_options(
    LoxInterp_core
    PUBLIC -Wall
           -Wextra
           -Wpedantic
           -Werror
           -Wshadow
           -Wconversion
           $<$<CONFIG:Debug>:-g
           -O0>
           $<$<CONFIG:Release>:-O3>
)

add_executable(LoxInterp src/main.cpp)
target_link_libraries(LoxInterp PRIVATE LoxInterp_core)

option(ENABLE_SANITIZERS "Enable Address and Undefined Behavior Sanitizers" OFF)

if (ENABLE_SANITIZERS)
    target_compile_options(LoxInterp_core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(LoxInterp_core PUBLIC -fsanitize=address,undefined)
endif ()

option(BUILD_TESTS "Build tests" ON)
if (BUILD_TESTS)
    find_package(GTest QUIET)
    if (GTest_FOUND)
        enable_testing()
        add_subdirectory(tests)
    endif ()
endif ()
