cmake_minimum_required(VERSION 3.13)

set(ENV{PICO_SDK_PATH} /Users/nathankau/pico/pico-sdk)
# set(ENV{PICO_SDK_FETCH_FROM_GIT} 1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(pico_sdk_import.cmake)

project(can2040-test-project C CXX ASM)

pico_sdk_init()

include_directories(can2040/src)
add_library(can2040 can2040/src/can2040.c can2040/src/can2040.h)
target_link_libraries(can2040 PUBLIC pico_stdlib hardware_pio cmsis_core)

add_executable(test_can test_can.cc)
target_link_libraries(test_can pico_stdlib can2040)

pico_add_extra_outputs(test_can)
pico_enable_stdio_usb(test_can 1)
pico_enable_stdio_uart(test_can 0)

add_executable(dual_can_test dual_can_test.cc)
target_link_libraries(dual_can_test pico_stdlib can2040)

pico_add_extra_outputs(dual_can_test)
pico_enable_stdio_usb(dual_can_test 1)
pico_enable_stdio_uart(dual_can_test 0)

add_executable(serial_test serial_test.cc)
target_link_libraries(serial_test pico_stdlib can2040)

pico_add_extra_outputs(serial_test)
pico_enable_stdio_usb(serial_test 1)
pico_enable_stdio_uart(serial_test 0)

# target_include_directories(test_can PRIVATE
#     ${CMAKE_CURRENT_LIST_DIR}
#     ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
# )
