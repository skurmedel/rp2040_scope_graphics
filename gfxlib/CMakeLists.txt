cmake_minimum_required(VERSION 3.14)

project(rp2040_scope_gfxlib)

option(BUILD_TESTS "Build Test Suite (not recommended for firmware)" ON)

add_subdirectory(src)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()