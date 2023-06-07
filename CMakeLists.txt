cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} ~/pico/pico-sdk)
# set(ENV{PICO_SDK_FETCH_FROM_GIT} 1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(pico_sdk_import.cmake)

project(can2040-test-project C CXX ASM)

pico_sdk_init()

add_subdirectory(apps)
add_subdirectory(src)
add_subdirectory(extern)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)