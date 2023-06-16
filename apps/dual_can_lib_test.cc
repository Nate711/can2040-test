#include "pupperv3/dual_can_lib.h"

#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pupperv3/utils.h"

constexpr uint32_t kSysClock = 125'000'000;
constexpr uint32_t kBitRate = 1'000'000;
constexpr uint32_t kGPIO_RX_A = 0, kGPIO_TX_A = 1;
constexpr uint32_t kGPIO_RX_B = 14, kGPIO_TX_B = 15;

constexpr double kToggleLEDRate = 2.0;
constexpr double kSendMessageRate = 1000.0;
constexpr uint32_t kSleepUS = 1;

int main(void) {
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  int32_t ledState = 0;

  stdio_init_all();

  dualcan::SetupParams params;
  params.sys_clock = kSysClock;
  params.bit_rate = kBitRate;
  params.rx_a = kGPIO_RX_A;
  params.tx_a = kGPIO_TX_A;
  params.rx_b = kGPIO_RX_B;
  params.tx_b = kGPIO_TX_B;
  dualcan::setup(params);

  int count = 0;

  uint64_t last_led_toggle = time_us_64();
  uint64_t last_msg_sent = time_us_64();

  utils::PeriodicCaller led_toggler(kToggleLEDRate);
  utils::PeriodicCaller transmit_message(kSendMessageRate);

  while (1) {
    led_toggler.tick([&]() {
      std::cout << "Count: " << count << "\n";
      count++;
      gpio_put(LED_PIN, ledState);
      if (ledState == 0) {
        ledState = 1;
      } else {
        ledState = 0;
      }
    });

    transmit_message.tick([&]() {
      dualcan::Msg msg = {};
      msg.id = 142;
      msg.dlc = 8;
      int data[8] = {0xA2, 0x00, 0x00, 0x00, 0xBD, 0x11, 0x00, 0x00};
      for (int i = 0; i < 8; i++) {
        msg.data[i] = data[i];
      }
      dualcan::transmit_a(&msg);
    });

    if (dualcan::new_message_a()) {
      dualcan::reset_new_message_a();
      auto latest_notify_a = dualcan::notification_a();
      if (latest_notify_a == dualcan::kNotifyRx) {
        printf("Got CAN message on A!\n");
      } else if (latest_notify_a == dualcan::kNotifyTx) {
        printf("Sent CAN message on A!\n");
      } else if (latest_notify_a == dualcan::kNotifyError) {
        printf("CAN error on A!\n");
      } else {
        printf("INVALID NOTIFY A\n");
      }
    }

    if (dualcan::new_message_b()) {
      dualcan::reset_new_message_b();
      auto latest_notify_b = dualcan::notification_b();
      if (latest_notify_b == dualcan::kNotifyRx) {
        printf("Got CAN message on B!\n");
        printf("Time: %" PRIu64 "\n", time_us_64() / 1000);
        for (int i = 0; i < dualcan::latest_msg_b().dlc; i++) {
          printf("%d ", static_cast<int>(dualcan::latest_msg_b().data[i]));
        }
        printf("\n");
      } else if (latest_notify_b == dualcan::kNotifyTx) {
        printf("Sent CAN message on B!\n");
      } else if (latest_notify_b == dualcan::kNotifyError) {
        printf("CAN error on B!\n");
      } else {
        printf("INVALID NOTIFY B\n");
      }
    }
    sleep_us(kSleepUS);
  }
}
