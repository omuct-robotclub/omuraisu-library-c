#ifndef OMURAISU_CPP_PID_PID_HPP_
#define OMURAISU_CPP_PID_PID_HPP_

#include "pid/pid.h"

namespace omuraisu {
namespace pid {
struct PidGain : public ::PidGain {};
struct PidParameter : public ::PidParameter {};

class Pid {
 public:
  Pid() noexcept;
  Pid(const PidParameter& parameter) noexcept;
  Pid(const Pid& other) noexcept;

  void reset() noexcept;
  float calc(float goal, float actual, float dt) noexcept;
  void set_limit(float min, float max) noexcept;

 private:
  PidParameter parameter_;
  PidController controller_;
};

}  // namespace pid
}  // namespace omuraisu

#endif  // OMURAISU_CPP_PID_PID_HPP_