// source: https://gitea.predevolution-technologies.de/anme/CAN2040_Test
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

// I don't know how to eliminate using two global variables because all the
// callbacks require void* functions which themselves require globals
static struct can2040 cbus_a;
static struct can2040 cbus_b;

using Msg = struct can2040_msg;

static Msg latest_msg_a_ = {};
static volatile uint32_t latest_notify_a_ = 0;
static volatile bool new_message_a_ = false;

static volatile bool TURN_ME_ON = false;

static Msg latest_msg_b_ = {};
static volatile uint32_t latest_notify_b_ = 0;
static volatile bool new_message_b_ = false;

static void can2040_cb_a(struct can2040 *cd, uint32_t notify, Msg *msg) {
  new_message_a_ = true;
  TURN_ME_ON = true;
  latest_notify_a_ = notify;
  latest_msg_a_ = *msg;
}

static void can2040_cb_b(struct can2040 *cd, uint32_t notify, Msg *msg) {
  new_message_b_ = true;
  latest_notify_b_ = notify;
  latest_msg_b_ = *msg;
}

static void PIOx_IRQHandler_A(void) { can2040_pio_irq_handler(&cbus_a); }
static void PIOx_IRQHandler_B(void) { can2040_pio_irq_handler(&cbus_b); }

struct SetupParams {
  uint32_t sys_clock;
  uint32_t bit_rate;
  uint32_t rx_a;
  uint32_t tx_a;
  uint32_t rx_b;
  uint32_t tx_b;
};

static void setup(SetupParams params) {
  // Setup canbus
  can2040_setup(&cbus_a, kPIONumA);
  can2040_setup(&cbus_b, kPIONumB);

  can2040_callback_config(&cbus_a, can2040_cb_a);
  can2040_callback_config(&cbus_b, can2040_cb_b);

  // Enable irqs
  irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler_A);
  NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
  NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

  irq_set_exclusive_handler(PIO1_IRQ_0_IRQn, PIOx_IRQHandler_B);
  NVIC_SetPriority(PIO1_IRQ_0_IRQn, 1);
  NVIC_EnableIRQ(PIO1_IRQ_0_IRQn);

  // Start canbus
  can2040_start(&cbus_a, params.sys_clock, params.bit_rate, params.rx_a,
                params.tx_a);
  can2040_start(&cbus_b, params.sys_clock, params.bit_rate, params.rx_b,
                params.tx_b);
}

static void transmit_a(Msg *msg) { can2040_transmit(&cbus_a, msg); }
static void transmit_b(Msg *msg) { can2040_transmit(&cbus_b, msg); }

static bool new_message_a() { return new_message_a_; }
static bool new_message_b() { return new_message_b_; }

static void reset_new_message_a() { new_message_a_ = false; }
static void reset_new_message_b() { new_message_b_ = false; }

static uint32_t notification_a() { return latest_notify_a_; }
static uint32_t notification_b() { return latest_notify_b_; }

static can2040_msg latest_msg_a() { return latest_msg_a_; }
static can2040_msg latest_msg_b() { return latest_msg_b_; }

}  // namespace dualcan