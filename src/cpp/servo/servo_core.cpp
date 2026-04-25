#include "servo/servo_core.hpp"
namespace omuraisu {
namespace servo {
ServoCore::ServoCore(uint32_t id) : core_data_{} { core_data_.id = id; }

void ServoCore::set_degree(float degree, size_t index) {
  om_servo_set_degree(&core_data_, degree, index);
}

void ServoCore::set_degrees(const float degrees[8]) {
  om_servo_set_degrees(&core_data_, degrees);
}

uint8_t ServoCore::get_degree(size_t index) const {
  return om_servo_get_degree(&core_data_, index);
}

void ServoCore::get_degrees(float degrees[8]) const {
  om_servo_get_degrees(&core_data_, degrees);
}

can::CanMessage ServoCore::to_can_message() const {
  return static_cast<can::CanMessage>(om_servo_to_can_message(&core_data_));
}
}  // namespace servo
}  // namespace omuraisu
