#include <array>
#include <cmath>
#include <iostream>
#include <string>

#include "chassis/mecanum.h"
#include "chassis/mecanum.hpp"

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

bool ExpectArrayNear(const float actual[4], const float expected[4],
                     float epsilon, const std::string& message) {
  for (int i = 0; i < 4; ++i) {
    if (!ExpectNear(actual[i], expected[i], epsilon,
                    message + " index=" + std::to_string(i))) {
      return false;
    }
  }
  return true;
}

bool TestRadiusConstructorMatchesC() {
  const float radius = 0.2f;
  omuraisu::chassis::Mecanum cpp_mecanum(radius);
  ::Mecanum c_mecanum = om_mecanum_init_radius(radius);

  Velocity vel = {0.5f, -0.3f, 1.2f, 0.0f};
  float cpp_result[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float c_result[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  cpp_mecanum.calc(vel, cpp_result);
  om_mecanum_calc(&c_mecanum, &vel, c_result);

  return ExpectArrayNear(cpp_result, c_result, 1e-4f,
                         "radius constructor: C++ result should match C");
}

bool TestCartesianConstructorMatchesC() {
  std::array<Coordinate, 4> pos = {
      Coordinate{0.2f, 0.2f, 0.0f, 0.0f},
      Coordinate{-0.2f, 0.2f, 0.0f, 0.0f},
      Coordinate{-0.2f, -0.2f, 0.0f, 0.0f},
      Coordinate{0.2f, -0.2f, 0.0f, 0.0f},
  };

  omuraisu::chassis::Mecanum cpp_mecanum(pos.data());
  ::Mecanum c_mecanum = om_mecanum_init(pos.data());

  Velocity vel = {0.1f, 0.4f, -0.8f, 0.0f};
  float cpp_result[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float c_result[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  cpp_mecanum.calc(vel, cpp_result);
  om_mecanum_calc(&c_mecanum, &vel, c_result);

  return ExpectArrayNear(cpp_result, c_result, 1e-4f,
                         "cartesian constructor: C++ result should match C");
}

bool TestPolarConstructorMatchesC() {
  std::array<CoordinatePolar, 4> pos = {
      CoordinatePolar{0.2f, static_cast<float>(M_PI / 4.0), 0.0f, 0.0f},
      CoordinatePolar{0.2f, static_cast<float>(3.0 * M_PI / 4.0), 0.0f, 0.0f},
      CoordinatePolar{0.2f, static_cast<float>(5.0 * M_PI / 4.0), 0.0f, 0.0f},
      CoordinatePolar{0.2f, static_cast<float>(7.0 * M_PI / 4.0), 0.0f, 0.0f},
  };

  omuraisu::chassis::Mecanum cpp_mecanum(pos.data());
  ::Mecanum c_mecanum = om_mecanum_init_polar(pos.data());

  Velocity vel = {-0.2f, 0.7f, 0.3f, 0.0f};
  float cpp_result[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float c_result[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  cpp_mecanum.calc(vel, cpp_result);
  om_mecanum_calc(&c_mecanum, &vel, c_result);

  return ExpectArrayNear(cpp_result, c_result, 1e-4f,
                         "polar constructor: C++ result should match C");
}

bool TestKnownForwardMotion() {
  omuraisu::chassis::Mecanum mecanum(0.2f);
  Velocity vel = {0.0f, 1.0f, 0.0f, 0.0f};
  float result[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  mecanum.calc(vel, result);

  for (int i = 0; i < 4; ++i) {
    if (!ExpectNear(result[i], 1.0f, 1e-4f,
                    "forward motion: wheel speed should be 1.0")) {
      return false;
    }
  }
  return true;
}

}  // namespace

int main() {
  bool ok = true;

  ok = TestRadiusConstructorMatchesC() && ok;
  ok = TestCartesianConstructorMatchesC() && ok;
  ok = TestPolarConstructorMatchesC() && ok;
  ok = TestKnownForwardMotion() && ok;

  if (!ok) {
    std::cerr << "chassis_cpp_test failed" << std::endl;
    return 1;
  }

  std::cout << "chassis_cpp_test passed" << std::endl;
  return 0;
}
