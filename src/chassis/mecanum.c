#include "chassis/mecanum.h"

Mecanum om_mecanum_init(const Coordinate pos[4]) {
  Mecanum mecanum;
  for (int i = 0; i < 4; ++i) {
    mecanum.wheel_pos[i] = om_coordinate_polar_from_rectangular(&pos[i]);
  }
  return mecanum;
}

Mecanum om_mecanum_init_radius(const float radius) {
  Mecanum mecanum;
  for (int i = 0; i < 4; ++i) {
    const float ofs = 2 * M_PI / 4;
    mecanum.wheel_pos[i] = (CoordinatePolar){radius, ofs * i + M_PI / 4, 0, 0};
  }
  return mecanum;
}

Mecanum om_mecanum_init_polar(const CoordinatePolar pos[4]) {
  Mecanum mecanum;
  for (int i = 0; i < 4; ++i) {
    mecanum.wheel_pos[i] = pos[i];
  }
  return mecanum;
}

void om_mecanum_calc(const Mecanum* self, const Velocity* vel, float result[4]) {
  for (int i = 0; i < 4; ++i) {
      const float ofs = 2 * M_PI / 4;
      const float vx =
          vel->x + self->wheel_pos[i].r * vel->ang * cos(self->wheel_pos[i].theta + M_PI / 2);
      const float vy =
          vel->y + self->wheel_pos[i].r * vel->ang * sin(self->wheel_pos[i].theta + M_PI / 2);
      result[i] = vy - tan(ofs * i + M_PI / 4) * vx;
    }
}
