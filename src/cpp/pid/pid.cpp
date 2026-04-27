#include "pid/pid.hpp"

namespace omuraisu {
namespace pid {
Pid::Pid() noexcept : parameter_{}, controller_{} {}
Pid::Pid(const PidParameter& parameter) noexcept : parameter_{parameter} {
  controller_ = om_pid_init(parameter_);
}
Pid::Pid(const Pid& other) noexcept
    : parameter_{other.parameter_}, controller_{other.controller_} {}

void Pid::reset() noexcept { om_pid_reset(&controller_); }
float Pid::calc(float goal, float actual, float dt) noexcept {
  return om_pid_calc(&controller_, goal, actual, dt);
}
void Pid::set_limit(float min, float max) noexcept {
  om_pid_set_limit(&controller_, min, max);
}

}  // namespace pid
}  // namespace omuraisu