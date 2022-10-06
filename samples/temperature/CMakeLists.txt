cmake_minimum_required (VERSION 3.20.0)
set (APPNAME temperature)

project(APPNAME)


add_compile_options(-O3 -Wno-unknown-attributes)
string(APPEND CMAKE_C_FLAGS " -nostdlib")
add_link_options(
    -z stack-size=8192
    -Wl,--allow-undefined
    -Wl,--strip-all,--no-entry
    -Wl,--export=__heap_base
    -Wl,--export=__data_end
    -Wl,--export-all)

include_directories(/opt/wamr-sdk/app-sdk/wamr-app-framework/include)
find_library(LIBAPP_FRAMEWORK
    NAMES app_framework libappframework
    HINTS /opt/wamr-sdk/app-sdk/wamr-app-framework/lib)

foreach(APP sensor_service analytics_service mqtt_service)
    add_executable(${APP}.wasm ${APP}.c)
    target_link_libraries(${APP}.wasm ${LIBAPP_FRAMEWORK})
    add_custom_command(OUTPUT ${APP}.nbx
        COMMAND wamrc
            --cpu=cortex-m4
            --target=thumbv4t
            --target-abi=gnueabihf
            --size-level=3
            --opt-level=3
            -o ${APP}.nbx
            ${APP}.wasm
        COMMAND chmod a+x ${APP}.nbx
        DEPENDS ${APP}.wasm
        COMMENT "Compiling wasm module to bytecode for ARM")
    add_custom_target(wamrc_${APP} ALL DEPENDS ${APP}.nbx)
endforeach()
