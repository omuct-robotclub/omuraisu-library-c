#include <iostream>
#include <string>

#include "pid/pid.hpp"

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
  if (actual < expected - epsilon || actual > expected + epsilon) {
    std::cerr << message << " (actual=" << actual << ", expected=" << expected
              << ")" << std::endl;
    return false;
  }
  return true;
}

bool TestDefaultConstruction() {
  omuraisu::pid::Pid pid;
  // After default construction, no specific behavior is guaranteed
  // but it should not crash
  return ExpectTrue(true, "default construction: should not crash");
}

bool TestParameterConstruction() {
  omuraisu::pid::PidParameter param{};
  param.gain.kp = 1.0f;
  param.gain.ki = 0.1f;
  param.gain.kd = 0.01f;
  param.min = -100.0f;
  param.max = 100.0f;
  omuraisu::pid::Pid pid(param);

  // Test basic calculation
  float output = pid.calc(100.0f, 0.0f, 0.01f);
  return ExpectTrue(
      output > 0.0f,
      "parameter construction: output should be positive for positive error");
}

bool TestReset() {
  omuraisu::pid::PidParameter param{};
  param.gain.kp = 2.0f;
  param.gain.ki = 0.5f;
  param.gain.kd = 0.1f;
  param.min = -100.0f;
  param.max = 100.0f;
  omuraisu::pid::Pid pid(param);

  // Perform some calculations to build up integral
  pid.calc(100.0f, 0.0f, 0.01f);
  pid.calc(100.0f, 50.0f, 0.01f);

  // Reset should clear integral and derivative state
  pid.reset();

  // After reset, calc with same error as fresh instance should give similar
  // output (since integral and derivative are reset)
  float output_after_reset = pid.calc(100.0f, 0.0f, 0.01f);
  return ExpectTrue(output_after_reset > 0.0f,
                    "reset: output should be positive after reset");
}

bool TestLimitApplication() {
  omuraisu::pid::PidParameter param{};
  param.gain.kp = 10.0f;
  param.gain.ki = 1.0f;
  param.gain.kd = 0.1f;
  param.min = -50.0f;
  param.max = 50.0f;
  omuraisu::pid::Pid pid(param);

  // With large error and high gain, output should be clamped to max
  float output = pid.calc(1000.0f, 0.0f, 0.01f);
  return ExpectNear(output, 50.0f, 0.1f,
                    "limit: output should be clamped to max limit");
}

bool TestNegativeLimitApplication() {
  omuraisu::pid::PidParameter param{};
  param.gain.kp = 10.0f;
  param.gain.ki = 1.0f;
  param.gain.kd = 0.1f;
  param.min = -50.0f;
  param.max = 50.0f;
  omuraisu::pid::Pid pid(param);

  // With large negative error, output should be clamped to min
  float output = pid.calc(-1000.0f, 0.0f, 0.01f);
  return ExpectNear(output, -50.0f, 0.1f,
                    "negative limit: output should be clamped to min limit");
}

bool TestSetLimit() {
  omuraisu::pid::PidParameter param{};
  param.gain.kp = 10.0f;
  param.gain.ki = 1.0f;
  param.gain.kd = 0.1f;
  param.min = -100.0f;
  param.max = 100.0f;
  omuraisu::pid::Pid pid(param);

  // Change limit using set_limit
  pid.set_limit(-30.0f, 30.0f);

  // With large error, output should now be clamped to new limit
  float output = pid.calc(1000.0f, 0.0f, 0.01f);
  return ExpectNear(output, 30.0f, 0.1f,
                    "set_limit: output should be clamped to new max limit");
}

bool TestConvergence() {
  omuraisu::pid::PidParameter param{};
  param.gain.kp = 1.0f;
  param.gain.ki = 0.1f;
  param.gain.kd = 0.1f;
  param.min = -100.0f;
  param.max = 100.0f;
  omuraisu::pid::Pid pid(param);

  // Simulate control loop
  float actual = 0.0f;
  float goal = 10.0f;
  float dt = 0.01f;

  for (int i = 0; i < 100; ++i) {
    float output = pid.calc(goal, actual, dt);
    // Simple plant model: next_actual = actual + 0.01 * output
    actual += 0.01f * output;
  }

  // After 100 iterations, actual should be closer to goal than initially
  return ExpectTrue(actual > 5.0f && actual < 15.0f,
                    "convergence: output should move toward goal");
}

}  // namespace

int main() {
  bool ok = true;

  ok = TestDefaultConstruction() && ok;
  ok = TestParameterConstruction() && ok;
  ok = TestReset() && ok;
  ok = TestLimitApplication() && ok;
  ok = TestNegativeLimitApplication() && ok;
  ok = TestSetLimit() && ok;
  ok = TestConvergence() && ok;

  if (!ok) {
    std::cerr << "pid_cpp_test failed" << std::endl;
    return 1;
  }

  std::cout << "pid_cpp_test passed" << std::endl;
  return 0;
}
