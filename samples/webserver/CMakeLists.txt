cmake_minimum_required(VERSION 3.20.0)

# Set the WASI SDK toolchain file
set(CMAKE_TOOLCHAIN_FILE /opt/wasi-sdk/share/cmake/wasi-sdk.cmake)

# Include the WAMR socket library CMake file (adjust path as needed)
#include(../../external/wasm-micro-runtime/core/iwasm/libraries/lib-socket/lib_socket_wasi.cmake)

set(APPNAME webserver)
project(${APPNAME})

# Set the linker flags
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--strip-all -Wl,--allow-undefined")

# Set compilation flags
add_compile_options(
    -O0 -Wno-unknown-attributes
    -O3
    -Wall
    -Wextra
    -Wno-unused-parameter
)

# Collect the source files
file(GLOB source_files ./src/mongoose.c ./src/wasi_socket_ext.c)

# Add socket support for WASM from IWASM 
#include_directories(
#  ${CMAKE_CURRENT_SOURCE_DIR}/../../external/wasm-micro-runtime/core/iwasm/libraries/lib-socket/inc
#)

# Define the app executable
add_executable(${APPNAME}.wasm
  src/main.c
  ${source_files}
)
