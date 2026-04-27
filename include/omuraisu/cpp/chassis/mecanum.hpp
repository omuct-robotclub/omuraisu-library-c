#ifndef OMURAISU_CPP_CHASSIS_MECANUM_HPP_
#define OMURAISU_CPP_CHASSIS_MECANUM_HPP_
#include "chassis/mecanum.h"
namespace omuraisu {
namespace chassis {
class Mecanum {
 public:
  Mecanum(const Coordinate pos[4]) noexcept;
  Mecanum(const float radius) noexcept;
  Mecanum(const CoordinatePolar pos[4]) noexcept;

  void calc(const Velocity& vel, float result[4]) const noexcept;

 private:
  ::Mecanum mecanum_;
};
}  // namespace chassis
}  // namespace omuraisu
#endif  // OMURAISU_CPP_CHASSIS_MECANUM_HPP_