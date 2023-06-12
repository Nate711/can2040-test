#pragma once

#include <stdint.h>
#include <stdio.h>

#include <cmath>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

extern "C" {
#include "can2040/src/can2040.h"
}

#include <array>
#include <iostream>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "pupperv3/dual_can_lib.h"

namespace mg4005 {

enum class CANChannel {
  CAN_A = 1,
  CAN_B = 2,
};

inline std::string to_string(CANChannel channel) {
  switch (channel) {
    case CANChannel::CAN_A:
      return "can_a";
    case CANChannel::CAN_B:
      return "can_b";
    default:
      return "";
  }
}

struct CommonResponse {
  int16_t encoder_counts = 0;           // counts (-2^15-1 to 2^15)
  int16_t previous_encoder_counts = 0;  // previous counts
  float velocity_degs = 0.0;            // degs per sec
  float velocity_rads = 0.0;            // rads per sec
  float current = 0.0;                  // Amps
  uint8_t temp = 0;                     // C

  int32_t rotations = 0;            // motor rotations (post-processed)
  float multi_loop_angle = 0.0;     // degs (post-processed)
  float output_rads = 0.0;          // rads (post-processed)
  float output_rads_per_sec = 0.0;  // rads per sec
};

struct MotorID {
  // CAN bus / channel
  CANChannel bus;
  // 1-indexed motor ID
  uint8_t id;

  inline bool operator==(const MotorID& other) const {
    return other.bus == this->bus && other.id == this->id;
  }

  inline friend std::ostream& operator<<(std::ostream& os,
                                         const MotorID& motor_id) {
    os << "[bus=" << to_string(motor_id.bus) << " id=" << motor_id.id << "]";
    return os;
  }
};

class Interface {
 public:
  Interface(const dualcan::SetupParams& params);
  void initialize_motor(const MotorID& motor_id);
  void stop_motor(const MotorID& motor_id);
  void command_velocity(const MotorID& motor_id, float velocity_rad_per_sec);

 private:
  static constexpr uint8_t kStartup0 = 0x76;
  static constexpr uint8_t kStartup1 = 0x88;
  static constexpr uint8_t kStartup2 = 0x77;
  static constexpr uint8_t kGetMultiAngle = 0x92;
  static constexpr uint8_t kCommandCurrent = 0xA1;
  static constexpr uint8_t kCommandVelocity = 0xA2;
  static constexpr uint8_t kCommandStop = 0x81;
  static constexpr uint8_t kWritePIDToROM = 0x32;
  static constexpr uint8_t kWritePIDToRAM = 0x31;
  static constexpr float kDegsPerTick = 0.01;
  static constexpr float kSpeedReduction = 0.1;

  static constexpr float kCurrentWriteMax = 32.0;
  static constexpr int kCurrentMultiplier = 2000;
  static constexpr int kCurrentRawReadMax = 2048;
  static constexpr float kCurrentReadMax = 33.0;
  static constexpr int kVelocityMultiplier = 100;
  static constexpr int kEncoderCountsPerRot = 65536;

  static constexpr float kRadToDeg = 180.0 / M_PI;

  using Command = std::array<uint8_t, 8>;
  void send(const MotorID& motor_id, const Command& command);

  uint32_t motor_id_to_can_id(uint32_t motor_id);

  // function to poll if received new can message -- should happen very fast
  // like > 12khz. Seems bad? Need to synchronize access to latest msg? Wrap
  // copy of data with disable interrupts? Seems that arrays are copied by
  // value
};
}  // namespace mg4005