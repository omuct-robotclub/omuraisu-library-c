#include "servo_core.h"

void om_servo_set_degree(ServoData* servo, float degree, size_t index) {
  if (index >= 8) {
    return;
  }
  uint8_t value = (uint8_t)(degree / 180.0f * 255);
  servo->data[index] = value;
}

void om_servo_set_degrees(ServoData* servo, const float degrees[8]) {
  for (size_t i = 0; i < 8; ++i) {
    om_servo_set_degree(servo, degrees[i], i);
  }
}

uint8_t om_servo_get_degree(const ServoData* servo, size_t index) {
  if (index >= 8) {
    return 0;
  }
  return servo->data[index];
}

void om_servo_get_degrees(const ServoData* servo, float degrees[8]) {
  for (size_t i = 0; i < 8; ++i) {
    degrees[i] = (float)servo->data[i] / 255.0f * 180.0f;
  }
}

CanMessage om_servo_to_can_message(const ServoData* servo) {
  CanMessage msg;
  msg.id = servo->id;
  memcpy(msg.data, servo->data, 8);
  msg.len = 8;
  return msg;
}