#include "coordinate/coordinate.h"
#include <math.h>

float om_coordinate_get_axis_angle(Coordinate* self) {
  return self->axis_ang;
}

void om_coordinate_add(Coordinate* self, const Coordinate* other) {
  self->x += other->x;
  self->y += other->y;
  self->ang += other->ang;
}
void om_coordinate_subtract(Coordinate* self, const Coordinate* other) {
  self->x -= other->x;
  self->y -= other->y;
  self->ang -= other->ang;
}
void om_coordinate_multiply(Coordinate* self, const float scalar) {
  self->x *= scalar;
  self->y *= scalar;
  self->ang *= scalar;
}
void om_coordinate_divide(Coordinate* self, const float scalar) {
  self->x /= scalar;
  self->y /= scalar;
  self->ang /= scalar;
}
float om_coordinate_dot_product(const Coordinate* self, const Coordinate* other) {
  return self->x * other->x + self->y * other->y;
}

Coordinate om_coordinate_copy(const Coordinate* other) {
  Coordinate coord;
  coord.x = other->x;
  coord.y = other->y;
  coord.ang = other->ang;
  coord.axis_ang = other->axis_ang;
  return coord;
}

void om_coordinate_set_axis_angle(Coordinate* self, const float* offset) {
  self->axis_ang = *offset;
}

Coordinate om_coordinate_init() {
  return (Coordinate){0, 0, 0, 0};
}

Coordinate om_coordinate_init_value(const float x, const float y, const float ang,
                                    const float axis_ang) {
  return (Coordinate){x, y, ang, axis_ang};
}

Coordinate om_coordinate_from_polar(const CoordinatePolar* other) {
  Coordinate coord;
  coord.x = other->r * cos(other->theta);
  coord.y = other->r * sin(other->theta);
  coord.ang = other->ang;
  coord.axis_ang = other->axis_ang;
  return coord;
}

float om_coordinate_polar_get_axis_angle(CoordinatePolar* self) {
  return self->axis_ang;
}

void om_coordinate_polar_set_axis_angle(CoordinatePolar* self, const float* offset) {
  self->axis_ang = *offset;
}

void om_coordinate_polar_add(CoordinatePolar* self, const CoordinatePolar* other) {
  self->r += other->r;
  self->theta += other->theta;
  self->ang += other->ang;
}

void om_coordinate_polar_subtract(CoordinatePolar* self, const CoordinatePolar* other) {
  self->r -= other->r;
  self->theta -= other->theta;
  self->ang -= other->ang;
}

void om_coordinate_polar_multiply(CoordinatePolar* self, const float scalar) {
  self->r *= scalar;
  self->theta *= scalar;
  self->ang *= scalar;
}

void om_coordinate_polar_divide(CoordinatePolar* self, const float scalar) {
  self->r /= scalar;
  self->theta /= scalar;
  self->ang /= scalar;
}

void om_coordinate_polar_dot_product(const CoordinatePolar* self, const CoordinatePolar* other, float* result) {
  *result = self->r * other->r * cos(self->theta - other->theta);
}

void om_coordinate_polar_cross_product(const CoordinatePolar* self, const CoordinatePolar* other, CoordinatePolar* result) {
  result->r = self->r * other->r * sin(self->theta - other->theta);
  result->theta = self->theta + M_PI_2;
  result->ang = self->ang + other->ang;
}

CoordinatePolar om_coordinate_polar_init() {
  return (CoordinatePolar){0, 0, 0, 0};
}

CoordinatePolar om_coordinate_polar_init_value(float r, float theta, float ang, float axis_ang) {
  return (CoordinatePolar){r, theta, ang, axis_ang};
}

CoordinatePolar om_coordinate_polar_from_rectangular(const Coordinate* other) {
  CoordinatePolar polar;
  polar.r = hypot(other->x, other->y);
  polar.theta = atan2(other->y, other->x);
  polar.ang = other->ang;
  polar.axis_ang = other->axis_ang;
  return polar;
}

void om_coordinate_polar_convert_ang(CoordinatePolar* self, const float* ang) {
  const float diff = *ang - self->axis_ang;
  self->theta -= diff;
  self->axis_ang = *ang;
}

void om_coordinate_convert_ang(Coordinate* self, const float* ang) {
  CoordinatePolar tmp = om_coordinate_polar_from_rectangular(self);
  om_coordinate_polar_convert_ang(&tmp, ang);
  *self = om_coordinate_from_polar(&tmp);
}

float om_coordinate_distance(const Coordinate* self, const Coordinate* other) {
  if (self->axis_ang == other->axis_ang) {
    return hypot(self->x - other->x, self->y - other->y);
  } else {
    Coordinate tmp = *self;
    om_coordinate_convert_ang(&tmp, &other->axis_ang);
    return hypot(tmp.x - other->x, tmp.y - other->y);
  }
}