
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



add_executable(dual_can_lib_test dual_can_lib_test.cc)
target_link_libraries(dual_can_lib_test pico_stdlib can2040 pupperv3)

pico_add_extra_outputs(dual_can_lib_test)
pico_enable_stdio_usb(dual_can_lib_test 1)
pico_enable_stdio_uart(dual_can_lib_test 0)



add_executable(mg4005_test mg4005_test.cc)
target_link_libraries(mg4005_test pico_stdlib can2040 pupperv3)

pico_add_extra_outputs(mg4005_test)
pico_enable_stdio_usb(mg4005_test 1)
pico_enable_stdio_uart(mg4005_test 0)



add_executable(serial_test serial_test.cc)
target_link_libraries(serial_test pico_stdlib can2040)

pico_add_extra_outputs(serial_test)
pico_enable_stdio_usb(serial_test 0)
pico_enable_stdio_uart(serial_test 1)