#include "chassis/mecanum.hpp"

namespace omuraisu {
namespace chassis {
Mecanum::Mecanum(const Coordinate pos[4]) noexcept
    : mecanum_(om_mecanum_init(pos)) {}
Mecanum::Mecanum(const float radius) noexcept
    : mecanum_(om_mecanum_init_radius(radius)) {}
Mecanum::Mecanum(const CoordinatePolar pos[4]) noexcept
    : mecanum_(om_mecanum_init_polar(pos)) {}

void Mecanum::calc(const Velocity& vel, float result[4]) const noexcept {
  om_mecanum_calc(&mecanum_, &vel, result);
}
}  // namespace chassis
}  // namespace omuraisu