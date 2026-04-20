#ifndef OMURAISU_CPP_COORDINATE_COORDINATE_HPP_
#define OMURAISU_CPP_COORDINATE_COORDINATE_HPP_

#include "coordinate/coordinate.h"
namespace omuraisu {
namespace coordinate {

struct Coordinate;
struct CoordinatePolar;

struct Coordinate : public ::Coordinate {
  Coordinate() noexcept;
  Coordinate(const float x, const float y, const float ang = 0.0f,
             const float axis_ang = 0.0f) noexcept;
  Coordinate(const Coordinate& other) noexcept;
  explicit Coordinate(const CoordinatePolar& other) noexcept;

  float get_axis_angle() const noexcept;
  void set_axis_angle(const float offset) noexcept;
  void convert_angle(const float& ang) noexcept;

  Coordinate& operator+=(const Coordinate& other) noexcept;
  Coordinate& operator-=(const Coordinate& other) noexcept;
  Coordinate& operator*=(const float scalar) noexcept;
  Coordinate& operator/=(const float scalar) noexcept;
  Coordinate& operator=(const Coordinate& other) noexcept;
};
struct CoordinatePolar : public ::CoordinatePolar {
  CoordinatePolar() noexcept;
  CoordinatePolar(float r, float theta, float ang = 0.0f,
                  float axis_ang = 0.0f) noexcept;
  CoordinatePolar(const CoordinatePolar& other) noexcept;
  explicit CoordinatePolar(const Coordinate& other) noexcept;

  float get_axis_angle() const noexcept;
  void set_axis_angle(const float offset) noexcept;
  void convert_angle(const float& ang) noexcept;

  CoordinatePolar& operator=(const CoordinatePolar& other) noexcept;
};

using Velocity = Coordinate;
using VelocityPolar = CoordinatePolar;

float distance(const Coordinate& a, const Coordinate& b) noexcept;
float distance(const CoordinatePolar& a, const CoordinatePolar& b) noexcept;
}  // namespace coordinate
}  // namespace omuraisu
#endif  // OMURAISU_CPP_COORDINATE_COORDINATE_HPP_