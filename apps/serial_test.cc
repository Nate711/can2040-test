// source: https://gitea.predevolution-technologies.de/anme/CAN2040_Test

#include <stdint.h>
#include <stdio.h>

#include <iostream>
#include <string>

#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pupperv3/utils.h"

constexpr int kBaudRate = 1'000'000;
constexpr double kSendRate = 1000.0;  // Hz

static char message[50] = "012345670123456701234567012345670123456701234567\n";

constexpr int UART_TX_PIN = 16;
constexpr int UART_RX_PIN = 17;

int main(void) {
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  int32_t ledState = 0;
  stdio_init_all();

  uart_init(uart0, kBaudRate);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  int sent_messages_count = 0;

  utils::PeriodicCaller led_toggler(2.0);
  utils::PeriodicCaller send_message_timer(kSendRate);
  while (1) {
    led_toggler.tick([&]() {
      gpio_put(LED_PIN, ledState);
      if (ledState == 0) {
        ledState = 1;
      } else {
        ledState = 0;
      }
    });
    send_message_timer.tick([&]() {
      printf(message);
      stdio_flush();
      sent_messages_count++;
    });
  }
}