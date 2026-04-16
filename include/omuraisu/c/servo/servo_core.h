#ifndef SERVO_CORE_H
#define SERVO_CORE_H

#include <can/can_interface.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  uint8_t data[8];
  uint32_t id;
} ServoData;

void om_servo_set_degree(ServoData* servo, float degree, size_t index);
void om_servo_set_degrees(ServoData* servo, const float degrees[8]);
uint8_t om_servo_get_degree(const ServoData* servo, size_t index);
void om_servo_get_degrees(const ServoData* servo, float degrees[8]);
CanMessage om_servo_to_can_message(const ServoData* servo);

#endif  // SERVO_CORE_H