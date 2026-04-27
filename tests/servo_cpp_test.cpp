#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#include "servo/servo_core.h"
#include "servo/servo_core.hpp"

namespace {

bool ExpectTrue(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << message << std::endl;
    return false;
  }
  return true;
}

bool ExpectNear(float actual, float expected, float epsilon,
                const std::string& message) {
  if (std::fabs(actual - expected) > epsilon) {
    std::cerr << message << " (actual=" << actual << ", expected=" << expected
              << ")" << std::endl;
    return false;
  }
  return true;
}

bool TestConstructorInitializesPayloadToZero() {
  const uint32_t id = 0x123U;
  omuraisu::servo::ServoCore servo(id);

  omuraisu::can::CanMessage msg = servo.to_can_message();
  if (!ExpectTrue(msg.id == id, "constructor: CAN id should match")) {
    return false;
  }
  if (!ExpectTrue(msg.len == 8U, "constructor: CAN length should be 8")) {
    return false;
  }

  for (int i = 0; i < 8; ++i) {
    if (!ExpectTrue(msg.data[i] == 0U,
                    "constructor: all payload bytes should start at zero")) {
      return false;
    }
  }
  return true;
}

bool TestSetDegreeMatchesCImplementation() {
  const uint32_t id = 0x55U;
  omuraisu::servo::ServoCore cpp_servo(id);

  ServoData c_servo{};
  c_servo.id = id;

  cpp_servo.set_degree(90.0f, 2U);
  om_servo_set_degree(&c_servo, 90.0f, 2U);

  cpp_servo.set_degree(45.0f, 99U);
  om_servo_set_degree(&c_servo, 45.0f, 99U);

  omuraisu::can::CanMessage cpp_msg = cpp_servo.to_can_message();
  CanMessage c_msg = om_servo_to_can_message(&c_servo);

  if (!ExpectTrue(cpp_msg.id == c_msg.id,
                  "set_degree: CAN id should match C")) {
    return false;
  }
  if (!ExpectTrue(cpp_msg.len == c_msg.len,
                  "set_degree: CAN length should match C")) {
    return false;
  }
  for (int i = 0; i < 8; ++i) {
    if (!ExpectTrue(cpp_msg.data[i] == c_msg.data[i],
                    "set_degree: payload should match C")) {
      return false;
    }
  }

  return ExpectTrue(
      cpp_servo.get_degree(2U) == om_servo_get_degree(&c_servo, 2U),
      "set_degree: get_degree should match C");
}

bool TestSetAndGetDegreesMatchCImplementation() {
  const float degrees[8] = {0.0f,  15.0f,  30.0f,  60.0f,
                            90.0f, 120.0f, 150.0f, 180.0f};

  omuraisu::servo::ServoCore cpp_servo(0x201U);
  ServoData c_servo{};
  c_servo.id = 0x201U;

  cpp_servo.set_degrees(degrees);
  om_servo_set_degrees(&c_servo, degrees);

  float cpp_out[8] = {0.0f};
  float c_out[8] = {0.0f};
  cpp_servo.get_degrees(cpp_out);
  om_servo_get_degrees(&c_servo, c_out);

  for (int i = 0; i < 8; ++i) {
    if (!ExpectNear(cpp_out[i], c_out[i], 1e-5f,
                    "set/get_degrees: converted degrees should match C")) {
      return false;
    }
  }

  omuraisu::can::CanMessage cpp_msg = cpp_servo.to_can_message();
  CanMessage c_msg = om_servo_to_can_message(&c_servo);
  return ExpectTrue(std::memcmp(cpp_msg.data, c_msg.data, 8) == 0,
                    "set/get_degrees: payload should match C");
}

}  // namespace

int main() {
  bool ok = true;

  ok = TestConstructorInitializesPayloadToZero() && ok;
  ok = TestSetDegreeMatchesCImplementation() && ok;
  ok = TestSetAndGetDegreesMatchCImplementation() && ok;

  if (!ok) {
    std::cerr << "servo_cpp_test failed" << std::endl;
    return 1;
  }

  std::cout << "servo_cpp_test passed" << std::endl;
  return 0;
}
