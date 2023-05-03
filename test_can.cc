// source: https://gitea.predevolution-technologies.de/anme/CAN2040_Test

#include <stdint.h>
#include <stdio.h>

#include <iostream>

#include "RP2040.h"
#include "can2040.h"
#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/time.h"

static struct can2040 cbus;

static struct can2040_msg latest_msg = {};
static uint32_t latest_notify = 0;
static bool new_message = false;

static void can2040_cb(struct can2040 *cd, uint32_t notify,
                       struct can2040_msg *msg) {
  new_message = true;
  latest_notify = notify;
  latest_msg = *msg;
}

static void PIOx_IRQHandler(void) { can2040_pio_irq_handler(&cbus); }

void canbus_setup(void) {
  uint32_t pio_num = 0;
  uint32_t sys_clock = 125000000, bitrate = 125000;
  uint32_t gpio_rx = 0, gpio_tx = 1;

  // Setup canbus
  can2040_setup(&cbus, pio_num);
  can2040_callback_config(&cbus, can2040_cb);

  // Enable irqs
  irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
  NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
  NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

  // Start canbus
  can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

int main(void) {
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  int32_t ledState = 0;
  stdio_init_all();

  canbus_setup();

  int count = 0;

  constexpr uint64_t kToggleLEDTime = 500 * 1000;
  constexpr uint64_t kSendMessageTime = 4 * 1000;
  constexpr uint32_t kSleepUS = 1;
  uint64_t last_led_toggle = time_us_64();
  uint64_t last_msg_sent = time_us_64();
  while (1) {
    if (time_us_64() - last_led_toggle > kToggleLEDTime) {
      last_led_toggle = time_us_64();
      std::cout << "count: " << count << "\n";
      count++;

      gpio_put(LED_PIN, ledState);
      if (ledState == 0) {
        ledState = 1;
      } else {
        ledState = 0;
      }
    }

    if (time_us_64() - last_msg_sent > kSendMessageTime) {
      last_msg_sent = time_us_64();
      struct can2040_msg msg = {};
      msg.id = 100;
      msg.dlc = 8;
      int data[8] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
      for (int i = 0; i < 8; i++) {
        msg.data[i] = data[i];
      }

      can2040_transmit(&cbus, &msg);
    }

    if (new_message) {
      new_message = false;
      if (latest_notify == CAN2040_NOTIFY_RX) {
        printf("got can message!\n");
      } else if (latest_notify == CAN2040_NOTIFY_TX) {
        printf("sent can message!\n");
      } else if (latest_notify == CAN2040_NOTIFY_ERROR) {
        printf("can error!\n");
      }
    }
    sleep_us(kSleepUS);
  }
}
