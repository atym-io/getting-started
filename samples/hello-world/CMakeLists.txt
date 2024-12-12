cmake_minimum_required (VERSION 3.20.0)
set(APPNAME hello-world)   

project(APPNAME)

add_compile_options(-O3 -Wno-unknown-attributes)
add_link_options(
    -Wl,--allow-undefined
    -Wl,--strip-all,--no-entry
    -Wl,--export-all)

include_directories(/opt/wamr-sdk/app-sdk/wamr-app-framework/include)
find_library(LIBAPP_FRAMEWORK
    NAMES app_framework libappframework
    HINTS /opt/wamr-sdk/app-sdk/wamr-app-framework/lib)

add_executable(${APPNAME}.wasm main.c)

target_link_libraries(${APPNAME}.wasm ${LIBAPP_FRAMEWORK})