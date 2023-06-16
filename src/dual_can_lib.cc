#include "pupperv3/dual_can_lib.h"

namespace dualcan {

static Msg latest_msg_a_ = {};
static volatile uint32_t latest_notify_a_ = 0;
static volatile bool new_message_a_ = false;

static Msg latest_msg_b_ = {};
static volatile uint32_t latest_notify_b_ = 0;
static volatile bool new_message_b_ = false;

static struct can2040 cbus_a;
static struct can2040 cbus_b;

static volatile bool TURN_ME_ON = false;

static void can2040_cb_a(struct can2040* cd, uint32_t notify, Msg* msg) {
  new_message_a_ = true;
  latest_notify_a_ = notify;
  latest_msg_a_ = *msg;
}

static void can2040_cb_b(struct can2040* cd, uint32_t notify, Msg* msg) {
  new_message_b_ = true;
  latest_notify_b_ = notify;
  latest_msg_b_ = *msg;
}

static void PIOx_IRQHandler_A(void) { can2040_pio_irq_handler(&cbus_a); }

static void PIOx_IRQHandler_B(void) { can2040_pio_irq_handler(&cbus_b); }

void setup(SetupParams params) {
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

void transmit_a(Msg* msg) { can2040_transmit(&cbus_a, msg); }

void transmit_b(Msg* msg) { can2040_transmit(&cbus_b, msg); }

bool new_message_a() { return new_message_a_; }

bool new_message_b() { return new_message_b_; }

void reset_new_message_a() { new_message_a_ = false; }

void reset_new_message_b() { new_message_b_ = false; }

uint32_t notification_a() { return latest_notify_a_; }

uint32_t notification_b() { return latest_notify_b_; }

Msg latest_msg_a() { return latest_msg_a_; }

Msg latest_msg_b() { return latest_msg_b_; }

}  // namespace dualcan
