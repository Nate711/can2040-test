#pragma once
#include "pico/time.h"

namespace utils {
class PeriodicCaller {
 public:
  PeriodicCaller(double freq) : last_call_(0) { period_us_ = 1000000.0 / freq; }

  template <typename F>
  void tick(F fn) {
    if (time_us_64() - last_call_ > period_us_) {
      last_call_ = time_us_64();
      fn();
    }
  };

 private:
  uint64_t last_call_;
  int period_us_;
};
}  // namespace utils