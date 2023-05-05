// source: https://gitea.predevolution-technologies.de/anme/CAN2040_Test

#include <stdint.h>
#include <stdio.h>

#include <iostream>
#include <string>

#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/time.h"

constexpr int kToggleLEDTime = 500'000;  // us
constexpr int kPrintTime = 1000;         // us
static char message[50] = "012345670123456701234567012345670123456701234567\n";

int main(void) {
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  int32_t ledState = 0;
  stdio_init_all();

  int count = 0;

  uint64_t last_led_toggle = time_us_64();
  uint64_t last_msg_sent = time_us_64();
  while (1) {
    if (time_us_64() - last_led_toggle > kToggleLEDTime) {
      last_led_toggle = time_us_64();
      gpio_put(LED_PIN, ledState);
      if (ledState == 0) {
        ledState = 1;
      } else {
        ledState = 0;
      }
    }
    if (time_us_64() - last_msg_sent > kPrintTime) {
      last_msg_sent = time_us_64();
      printf(message);
      stdio_flush();
      count++;
    }
  }
}