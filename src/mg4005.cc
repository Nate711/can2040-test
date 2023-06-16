#include "pupperv3/mg4005.h"

#include "pupperv3/dual_can_lib.h"

namespace mg4005 {

/// Constructs MG4005 interface and immediately starts both CAN networks
/// @param params CAN bus connection parameters
Interface::Interface(const dualcan::SetupParams &params) {
  dualcan::setup(params);
}

/// @brief Convert a motor_id (1-index) to can_id (140-index)
/// @param motor_id
/// @return
uint32_t Interface::motor_id_to_can_id(uint32_t motor_id) {
  return 0x140 + motor_id;
}

/// @brief Send command to motor
/// @param motor_id
/// @param payload
void Interface::send(const MotorID &motor_id, const Command &payload) {
  dualcan::Msg msg;
  //   std::cout << "msg: ";
  for (int i = 0; i < payload.size(); i++) {
    msg.data[i] = payload[i];
    //   std::cout << static_cast<int>(msg.data[i]) << " ";
  }
  msg.id = motor_id_to_can_id(motor_id.id);
  //   std::cout << "id: " << msg.id << "\n";
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

/// @brief Initialize command to motor
/// @param motor_id
void Interface::initialize_motor(const MotorID &motor_id) {
  send(motor_id, {kStartup0, 0, 0, 0, 0, 0, 0, 0});
  sleep_ms(1);
  send(motor_id, {kStartup1, 0, 0, 0, 0, 0, 0, 0});
  sleep_ms(1);
  send(motor_id, {kStartup2, 0, 0, 0, 0, 0, 0, 0});
  sleep_ms(1);
}

/// @brief Send stop command to motor
/// @param motor_id
void Interface::stop_motor(const MotorID &motor_id) {
  send(motor_id, {kCommandStop, 0, 0, 0, 0, 0, 0, 0});
  sleep_ms(1);
}

/// @brief Send velocity command to motor
/// @param motor_id
/// @param velocity_rad_per_sec Desired output velocity in radians per second
void Interface::command_velocity(const MotorID &motor_id,
                                 float velocity_rad_per_sec) {
  float output_deg_per_sec = velocity_rad_per_sec * kRadToDeg;
  float rotor_deg_per_sec = output_deg_per_sec / kSpeedReduction;
  int32_t rotor_discrete_velocity =
      static_cast<int32_t>(rotor_deg_per_sec * kVelocityMultiplier);
  uint8_t lsb = rotor_discrete_velocity & 0xFF;
  uint8_t byte1 = (rotor_discrete_velocity >> 8) & 0xFF;
  uint8_t byte2 = (rotor_discrete_velocity >> 16) & 0xFF;
  uint8_t byte3 = (rotor_discrete_velocity >> 24) & 0xFF;
  send(motor_id, {kCommandVelocity, 0, 0, 0, lsb, byte1, byte2, byte3});
}
}  // namespace mg4005