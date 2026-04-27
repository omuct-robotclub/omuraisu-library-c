#ifndef OMURAISU_CPP_SERVO_SERVO_CORE_HPP_
#define OMURAISU_CPP_SERVO_SERVO_CORE_HPP_
#include "can/can_interface.hpp"
#include "servo/servo_core.h"
namespace omuraisu {
namespace servo {
class ServoCore {
 public:
  ServoCore(uint32_t id);
  void set_degree(float degree, size_t index);
  void set_degrees(const float degrees[8]);
  uint8_t get_degree(size_t index) const;
  void get_degrees(float degrees[8]) const;
  can::CanMessage to_can_message() const;

 private:
  ServoData core_data_;
};
}  // namespace servo
}  // namespace omuraisu
#endif  // OMURAISU_CPP_SERVO_SERVO_CORE_HPP_