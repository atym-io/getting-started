cmake_minimum_required(VERSION 3.20.0)

# Set the WASI SDK toolchain cmake with pthread support
set(CMAKE_TOOLCHAIN_FILE /opt/wasi-sdk/share/cmake/wasi-sdk-pthread.cmake)

# Verify toolchain file exists
if(NOT EXISTS ${CMAKE_TOOLCHAIN_FILE})
    message(FATAL_ERROR "WASI toolchain cmake not found at ${CMAKE_TOOLCHAIN_FILE}.")
endif()

# Set the project name (needs to come after the toolchain file) 
set(APPNAME "blinky-button")
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

# Create the executable target
add_executable(${APPNAME}.wasm main.c)