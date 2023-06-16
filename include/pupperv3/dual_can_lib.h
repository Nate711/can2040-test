#pragma once

#include <stdint.h>
#include <stdio.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <iostream>

#include "RP2040.h"
extern "C" {
#include "can2040/src/can2040.h"
}
#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/time.h"

namespace dualcan {

constexpr uint32_t kNotifyRx = CAN2040_NOTIFY_RX;
constexpr uint32_t kNotifyTx = CAN2040_NOTIFY_TX;
constexpr uint32_t kNotifyError = CAN2040_NOTIFY_ERROR;

constexpr uint32_t kPIONumA = 0;
constexpr uint32_t kPIONumB = 1;

struct SetupParams {
  uint32_t sys_clock;
  uint32_t bit_rate;
  uint32_t rx_a;
  uint32_t tx_a;
  uint32_t rx_b;
  uint32_t tx_b;
};

using Msg = struct can2040_msg;

void setup(SetupParams params);
void transmit_a(Msg* msg);
void transmit_b(Msg* msg);
bool new_message_a();
bool new_message_b();
void reset_new_message_a();
void reset_new_message_b();
uint32_t notification_a();
uint32_t notification_b();
Msg latest_msg_a();
Msg latest_msg_b();

}  // namespace dualcan
