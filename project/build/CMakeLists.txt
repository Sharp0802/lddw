cmake_minimum_required(VERSION 3.20)
project(lddw)
set(ENABLED_LANGUAGES C)

set(ROOT ..)
add_link_options(-fuse-ld=lld)

file(GLOB SRCs ${ROOT}/src/*.c)

add_executable(lddw ${SRCs})

target_include_directories(lddw PRIVATE ${ROOT}/inc)

set_target_properties(lddw PROPERTIES
    LANGUAGES C
    C_STANDARD 99
    ARCHIVE_OUTPUT_DIRECTORY ${ROOT}/bin
    LIBRARY_OUTPUT_DIRECTORY ${ROOT}/bin
    RUNTIME_OUTPUT_DIRECTORY ${ROOT}/bin
    INTERPROCEDURAL_OPTIMIZATION TRUE
)

target_compile_definitions(lddw PUBLIC _UNICODE UNICODE)

target_compile_options(lddw PRIVATE -Wall -Wextra -Os -pedantic -ansi -fuse-ld=lld -ffunction-sections -fdata-sections -fno-math-errno -fno-asynchronous-unwind-tables -march=x86-64)
target_link_options(lddw PRIVATE -s -Wl,--gc-sections -Wl,--strip-all -fno-asynchronous-unwind-tables -ffunction-sections -fdata-sections -nostartfiles -march=x86-64)
