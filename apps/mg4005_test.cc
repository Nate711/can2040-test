#include "pupperv3/mg4005.h"

#include "pupperv3/utils.h"
constexpr uint32_t kGPIO_RX_A = 0, kGPIO_TX_A = 1;
constexpr uint32_t kGPIO_RX_B = 14, kGPIO_TX_B = 15;
const uint kLEDPin = PICO_DEFAULT_LED_PIN;

constexpr float kLEDToggleFreq = 10.0;
constexpr float kSendFreq = 500.0;

int main() {
  gpio_init(kLEDPin);
  gpio_set_dir(kLEDPin, GPIO_OUT);
  bool led_state = 0;
  stdio_init_all();

  dualcan::SetupParams params;
  params.bit_rate = 1'000'000;
  params.sys_clock = 125'000'000;
  params.rx_a = kGPIO_RX_A;
  params.tx_a = kGPIO_TX_A;
  params.rx_b = kGPIO_RX_B;
  params.tx_b = kGPIO_TX_B;
  mg4005::Interface interface(params);

  mg4005::MotorID motor_1;
  motor_1.bus = mg4005::CANChannel::CAN_A;
  motor_1.id = 1;
  mg4005::MotorID motor_2;
  motor_2.bus = mg4005::CANChannel::CAN_A;
  motor_2.id = 2;
  mg4005::MotorID motor_3;
  motor_3.bus = mg4005::CANChannel::CAN_A;
  motor_3.id = 3;
  interface.initialize_motor(motor_1);
  interface.initialize_motor(motor_2);
  interface.initialize_motor(motor_3);

  utils::PeriodicCaller led_toggler(kLEDToggleFreq);
  int led_toggles = 0;
  utils::PeriodicCaller send_command(kSendFreq);
  while (true) {
    // Toggle LED
    led_toggler.tick([&]() {
      std::cout << "Time(ms): " << led_toggles * 100 << "\n";
      led_toggles++;
      gpio_put(kLEDPin, led_state);
      led_state = !led_state;
    });

    // Send CAN commands
    send_command.tick([&]() {
      std::cout << "Sending velocity command\n";
      interface.command_velocity(motor_1, 0.5);
      interface.command_velocity(motor_2, -0.5);
      interface.command_velocity(motor_3, 1.0);
    });

    // Check for new messages
    // Doesn't work?? The callback is running succesfully though
    if (dualcan::new_message_a()) {
      dualcan::reset_new_message_a();
      auto msg = dualcan::latest_msg_a();
      auto latest_notify_a = dualcan::notification_a();
      if (latest_notify_a == dualcan::kNotifyRx) {
        printf("Got CAN message on A:");
        std::cout << msg.data << "\n";
      } else if (latest_notify_a == dualcan::kNotifyTx) {
        printf("Sent CAN message on A!\n");
      } else if (latest_notify_a == dualcan::kNotifyError) {
        printf("CAN error on A!\n");
      } else {
        printf("INVALID NOTIFY A\n");
      }
    }
    sleep_us(1);  // 1000khz
  }
  interface.stop_motor(motor_1);
  return 0;
}