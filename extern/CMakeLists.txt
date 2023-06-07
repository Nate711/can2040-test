add_library(can2040 can2040/src/can2040.c can2040/src/can2040.h)
target_link_libraries(can2040 PUBLIC pico_stdlib hardware_pio cmsis_core)
target_include_directories(can2040 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})