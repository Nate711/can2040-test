#include "pupperv3/mg4005.h"

#include "pupperv3/dual_can_lib.h"

namespace mg4005 {

Interface::Interface(const dualcan::SetupParams &params) {
  dualcan::setup(params);
}

inline uint8_t motor_id_to_can_id(uint8_t motor_id) { return 0x140 + motor_id; }

void Interface::send(const MotorID &motor_id, const Command &payload) {
  dualcan::Msg msg;
  for (int i = 0; i < payload.size(); i++) {
    msg.data[i] = payload[i];
  }
  msg.id = motor_id_to_can_id(motor_id.id);
  msg.dlc = 8;
  if (motor_id.bus == CANChannel::CAN_A) {
    dualcan::transmit_a(&msg);
  } else if (motor_id.bus == CANChannel::CAN_B) {
    dualcan::transmit_b(&msg);
  } else {
    std::cout << "ERROR. Unsupported CAN channel: " << to_string(motor_id.bus)
              << "\n";
  }

  //   // Record that we sent a message to this motor
  //   time_last_sent_[motor_flat_index(motor_id)] = prof_utils::now();
  //   (*messages_sent_.at(motor_flat_index(motor_id)))++;
  //   (*messages_sent_since_last_receive_.at(motor_flat_index(motor_id)))++;

  //   if (verbose_) {
  //     SPDLOG_INFO("Send @ {}:{}", to_string(motor_id.bus),
  //     motor_id.motor_id); SPDLOG_INFO("- sent: {} received: {}",
  //                 *messages_sent_.at(motor_flat_index(motor_id)),
  //                 *messages_received_.at(motor_flat_index(motor_id)));
  //   }
}
}  // namespace mg4005