#include "coordinate/coordinate.hpp"

namespace omuraisu {
namespace coordinate {
Coordinate::Coordinate() noexcept : ::Coordinate{0, 0, 0, 0} {}
Coordinate::Coordinate(const float x, const float y, const float ang,
                       const float axis_ang) noexcept
    : ::Coordinate{x, y, ang, axis_ang} {}
Coordinate::Coordinate(const Coordinate& other) noexcept
    : ::Coordinate(other) {}
Coordinate::Coordinate(const CoordinatePolar& other) noexcept
    : ::Coordinate(om_coordinate_from_polar(&other)) {}

float Coordinate::get_axis_angle() const noexcept {
  return om_coordinate_get_axis_angle(this);
}
void Coordinate::set_axis_angle(const float offset) noexcept {
  om_coordinate_set_axis_angle(this, &offset);
}
void Coordinate::convert_angle(const float& ang) noexcept {
  om_coordinate_convert_ang(this, &ang);
}
Coordinate& Coordinate::operator+=(const Coordinate& other) noexcept {
  om_coordinate_add(this, &other);
  return *this;
}
Coordinate& Coordinate::operator-=(const Coordinate& other) noexcept {
  om_coordinate_subtract(this, &other);
  return *this;
}
Coordinate& Coordinate::operator*=(const float scalar) noexcept {
  om_coordinate_multiply(this, scalar);
  return *this;
}
Coordinate& Coordinate::operator/=(const float scalar) noexcept {
  om_coordinate_divide(this, scalar);
  return *this;
}
Coordinate& Coordinate::operator=(const Coordinate& other) noexcept {
  om_coordinate_copy(this, &other);
  return *this;
}

CoordinatePolar::CoordinatePolar() noexcept : ::CoordinatePolar{0, 0, 0, 0} {}
CoordinatePolar::CoordinatePolar(float r, float theta, float ang,
                                 float axis_ang) noexcept
    : ::CoordinatePolar{r, theta, ang, axis_ang} {}
CoordinatePolar::CoordinatePolar(const CoordinatePolar& other) noexcept
    : ::CoordinatePolar{other} {}
CoordinatePolar::CoordinatePolar(const Coordinate& other) noexcept
    : ::CoordinatePolar(om_coordinate_polar_from_rectangular(&other)) {}
float CoordinatePolar::get_axis_angle() const noexcept {
  return om_coordinate_polar_get_axis_angle(this);
}
void CoordinatePolar::set_axis_angle(const float offset) noexcept {
  om_coordinate_polar_set_axis_angle(this, &offset);
}
void CoordinatePolar::convert_angle(const float& ang) noexcept {
  om_coordinate_polar_convert_ang(this, &ang);
}
CoordinatePolar& CoordinatePolar::operator=(
    const CoordinatePolar& other) noexcept {
  om_coordinate_polar_copy(this, &other);
  return *this;
}

float distance(const Coordinate& a, const Coordinate& b) noexcept {
  return om_coordinate_distance(&a, &b);
}
float distance(const CoordinatePolar& a, const CoordinatePolar& b) noexcept {
  Coordinate coord_a = Coordinate(a);
  Coordinate coord_b = Coordinate(b);
  return om_coordinate_distance(&coord_a, &coord_b);
}
}  // namespace coordinate
}  // namespace omuraisu