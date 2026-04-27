#include <cmath>
#include <iostream>
#include <string>

#include "coordinate/coordinate.hpp"

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
  if (std::abs(actual - expected) > epsilon) {
    std::cerr << message << " (actual=" << actual << ", expected=" << expected
              << ")" << std::endl;
    return false;
  }
  return true;
}

bool TestDefaultConstruction() {
  omuraisu::coordinate::Coordinate c;
  return ExpectTrue(
      c.x == 0.0f && c.y == 0.0f && c.ang == 0.0f && c.axis_ang == 0.0f,
      "default construction: values should be zero");
}

bool TestValueConstruction() {
  omuraisu::coordinate::Coordinate c(1.0f, 2.0f, 0.5f, 0.1f);
  return ExpectTrue(ExpectNear(c.x, 1.0f, 0.001f, "x value") &&
                        ExpectNear(c.y, 2.0f, 0.001f, "y value") &&
                        ExpectNear(c.ang, 0.5f, 0.001f, "ang value") &&
                        ExpectNear(c.axis_ang, 0.1f, 0.001f, "axis_ang value"),
                    "value construction: parameters should match");
}

bool TestCopyConstruction() {
  omuraisu::coordinate::Coordinate original(3.0f, 4.0f, 1.0f, 0.2f);
  omuraisu::coordinate::Coordinate copy(original);
  return ExpectTrue(
      ExpectNear(copy.x, original.x, 0.001f, "copy x") &&
          ExpectNear(copy.y, original.y, 0.001f, "copy y") &&
          ExpectNear(copy.ang, original.ang, 0.001f, "copy ang") &&
          ExpectNear(copy.axis_ang, original.axis_ang, 0.001f, "copy axis_ang"),
      "copy construction: values should match");
}

bool TestPolarConversion() {
  // 3-4-5 triangle: r=5, theta=atan2(4, 3) ≈ 0.927 rad
  omuraisu::coordinate::CoordinatePolar polar(5.0f, std::atan2(4.0f, 3.0f),
                                              0.0f, 0.0f);
  omuraisu::coordinate::Coordinate cartesian(polar);
  return ExpectTrue(ExpectNear(cartesian.x, 3.0f, 0.01f, "cartesian x") &&
                        ExpectNear(cartesian.y, 4.0f, 0.01f, "cartesian y"),
                    "polar to cartesian: values should be close");
}

bool TestOperators() {
  omuraisu::coordinate::Coordinate c(1.0f, 2.0f, 0.5f, 0.1f);

  // Test +=
  omuraisu::coordinate::Coordinate c2(1.0f, 1.0f, 0.0f, 0.0f);
  c += c2;
  if (!ExpectNear(c.x, 2.0f, 0.001f, "+= x")) {
    return false;
  }
  if (!ExpectNear(c.y, 3.0f, 0.001f, "+= y")) {
    return false;
  }

  // Test -=
  omuraisu::coordinate::Coordinate c3(3.0f, 4.0f, 0.0f, 0.0f);
  c3 -= c2;
  if (!ExpectNear(c3.x, 2.0f, 0.001f, "-= x")) {
    return false;
  }
  if (!ExpectNear(c3.y, 3.0f, 0.001f, "-= y")) {
    return false;
  }

  // Test *=
  omuraisu::coordinate::Coordinate c4(2.0f, 3.0f, 0.0f, 0.0f);
  c4 *= 2.0f;
  if (!ExpectNear(c4.x, 4.0f, 0.001f, "*= x")) {
    return false;
  }
  if (!ExpectNear(c4.y, 6.0f, 0.001f, "*= y")) {
    return false;
  }

  // Test /=
  omuraisu::coordinate::Coordinate c5(4.0f, 6.0f, 0.0f, 0.0f);
  c5 /= 2.0f;
  if (!ExpectNear(c5.x, 2.0f, 0.001f, "/= x")) {
    return false;
  }
  if (!ExpectNear(c5.y, 3.0f, 0.001f, "/= y")) {
    return false;
  }

  return ExpectTrue(true, "operators: all tests passed");
}

bool TestDistanceFunction() {
  omuraisu::coordinate::Coordinate c1(0.0f, 0.0f, 0.0f, 0.0f);
  omuraisu::coordinate::Coordinate c2(3.0f, 4.0f, 0.0f, 0.0f);
  float dist = omuraisu::coordinate::distance(c1, c2);
  return ExpectNear(dist, 5.0f, 0.01f,
                    "distance: 3-4-5 triangle should give distance 5");
}

}  // namespace

int main() {
  bool ok = true;

  ok = TestDefaultConstruction() && ok;
  ok = TestValueConstruction() && ok;
  ok = TestCopyConstruction() && ok;
  ok = TestPolarConversion() && ok;
  ok = TestOperators() && ok;
  ok = TestDistanceFunction() && ok;

  if (!ok) {
    std::cerr << "coordinate_cpp_test failed" << std::endl;
    return 1;
  }

  std::cout << "coordinate_cpp_test passed" << std::endl;
  return 0;
}
