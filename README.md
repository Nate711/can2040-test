# Setup
* Install toolchain
* Download Pico SDK and set PICO_SDK_PATH environment variable (or uncomment the line in the CMakeLists.txt so that it's automatically downloaded)

# Problem
I can't build!

Steps:
1. `mkdir build; cd build`
2. `cmake ..`
3. `make`
4. I get the following
```bash
/opt/homebrew/Cellar/arm-none-eabi-gcc/10.3-2021.07/gcc/bin/../lib/gcc/arm-none-eabi/10.3.1/../../../../arm-none-eabi/bin/ld: CMakeFiles/test_can.dir/test_can.cc.obj: in function `PIOx_IRQHandler()':
/Users/nathankau/rp2040-can-test/test_can.cc:20: undefined reference to `can2040_pio_irq_handler(can2040*)'
/opt/homebrew/Cellar/arm-none-eabi-gcc/10.3-2021.07/gcc/bin/../lib/gcc/arm-none-eabi/10.3.1/../../../../arm-none-eabi/bin/ld: CMakeFiles/test_can.dir/test_can.cc.obj: in function `canbus_setup()':
/Users/nathankau/rp2040-can-test/test_can.cc:30: undefined reference to `can2040_setup(can2040*, unsigned long)'
/opt/homebrew/Cellar/arm-none-eabi-gcc/10.3-2021.07/gcc/bin/../lib/gcc/arm-none-eabi/10.3.1/../../../../arm-none-eabi/bin/ld: /Users/nathankau/rp2040-can-test/test_can.cc:31: undefined reference to `can2040_callback_config(can2040*, void (*)(can2040*, unsigned long, can2040_msg*))'
/opt/homebrew/Cellar/arm-none-eabi-gcc/10.3-2021.07/gcc/bin/../lib/gcc/arm-none-eabi/10.3.1/../../../../arm-none-eabi/bin/ld: /Users/nathankau/rp2040-can-test/test_can.cc:39: undefined reference to `can2040_start(can2040*, unsigned long, unsigned long, unsigned long, unsigned long)'
collect2: error: ld returned 1 exit status
```