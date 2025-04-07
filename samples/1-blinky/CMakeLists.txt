cmake_minimum_required (VERSION 3.20.0)
set(APPNAME blinky)   

project(APPNAME)

add_compile_options(-O0 -Wno-unknown-attributes)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-undefined")

add_link_options(
    -Wl,--strip-all # Strip all symbol/debug information from the binary
)

add_executable(${APPNAME}.wasm main.c)

target_link_libraries(${APPNAME}.wasm ${LIBAPP_FRAMEWORK})