cmake_minimum_required (VERSION 3.20.0)

# Set the WASI SDK toolchain file
set(CMAKE_TOOLCHAIN_FILE /opt/wasi-sdk/share/cmake/wasi-sdk-p1.cmake)

set(CMAKE_BUILD_TYPE Release)

set(APPNAME echo-server)
project(${APPNAME})

add_compile_options(
    -O0 -Wno-unknown-attributes
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-undefined -z stack-size=4096")

add_link_options(
    -Wl,--strip-all # Strip all symbol/debug information from the binary
)

add_executable(${APPNAME}.wasm src/main.c src/wasi_socket_ext.c)
