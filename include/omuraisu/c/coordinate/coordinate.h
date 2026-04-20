// 2D coordinate system for position and orientation of the robot. The angle is
// in radians, and the axis angle is the angle of the robot's forward direction
// relative to the x-axis.
#ifndef COORDINATE_H
#define COORDINATE_H

#define _DEFAULT_SOURCE
#include <math.h>

// Rectangular coordinate system
typedef struct {
  float x;
  float y;
  float ang;
  float axis_ang;
} Coordinate;

// Polar coordinate system
typedef struct {
  float r;
  float theta;
  float ang;
  float axis_ang;
} CoordinatePolar;

// Alias for velocity
typedef Coordinate Velocity;
typedef CoordinatePolar VelocityPolar;

// Rectangular coordinate functions
float om_coordinate_get_axis_angle(const Coordinate* self);

void om_coordinate_add(Coordinate* self, const Coordinate* other);

void om_coordinate_subtract(Coordinate* self, const Coordinate* other);

void om_coordinate_multiply(Coordinate* self, const float scalar);

void om_coordinate_divide(Coordinate* self, const float scalar);

float om_coordinate_dot_product(const Coordinate* self,
                                const Coordinate* other);

Coordinate om_coordinate_cross_product(const Coordinate* self,
                                       const Coordinate* other);

void om_coordinate_set_axis_angle(Coordinate* self, const float* offset);

Coordinate om_coordinate_init();

Coordinate om_coordinate_init_value(const float x, const float y,
                                    const float ang, const float axis_ang);

void om_coordinate_copy(Coordinate* self, const Coordinate* other);

Coordinate om_coordinate_from_polar(const CoordinatePolar* other);

// Polar coordinate functions
float om_coordinate_polar_get_axis_angle(const CoordinatePolar* self);

void om_coordinate_polar_set_axis_angle(CoordinatePolar* self,
                                        const float* offset);

void om_coordinate_polar_add(CoordinatePolar* self,
                             const CoordinatePolar* other);

void om_coordinate_polar_subtract(CoordinatePolar* self,
                                  const CoordinatePolar* other);

void om_coordinate_polar_multiply(CoordinatePolar* self, const float scalar);

void om_coordinate_polar_divide(CoordinatePolar* self, const float scalar);

float om_coordinate_polar_dot_product(const CoordinatePolar* self,
                                      const CoordinatePolar* other);

CoordinatePolar om_coordinate_polar_cross_product(const CoordinatePolar* self,
                                                  const CoordinatePolar* other);

CoordinatePolar om_coordinate_polar_init();

CoordinatePolar om_coordinate_polar_init_value(float r, float theta, float ang,
                                               float axis_ang);

void om_coordinate_polar_copy(CoordinatePolar* self,
                              const CoordinatePolar* other);

CoordinatePolar om_coordinate_polar_from_rectangular(const Coordinate* other);

void om_coordinate_polar_convert_ang(CoordinatePolar* self, const float* ang);

void om_coordinate_convert_ang(Coordinate* self, const float* ang);

float om_coordinate_distance(const Coordinate* self, const Coordinate* other);

#endif  // COORDINATE_H
