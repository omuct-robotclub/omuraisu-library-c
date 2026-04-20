#ifndef MECANUM_H
#define MECANUM_H

#include "coordinate/coordinate.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  CoordinatePolar wheel_pos[4];
} Mecanum;

Mecanum om_mecanum_init(const Coordinate pos[4]);

Mecanum om_mecanum_init_radius(const float radius);

Mecanum om_mecanum_init_polar(const CoordinatePolar pos[4]);

void om_mecanum_calc(const Mecanum* self, const Velocity* vel, float result[4]);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif