#include "pid.h"
#include <math.h>

PidController om_pid_init(const PidParameter parameter) {
  PidController controller;
  controller.parameter = parameter;
  controller.pre_error = 0;
  controller.integral = 0;
  return controller;
}

float om_pid_calc(PidController* self, const float goal, const float actual, const float dt_sec) {
  float error = goal - actual;
  self->integral += error * dt_sec;
  float deriv = dt_sec == 0 ? 0 : (error - self->pre_error) / dt_sec;
  float output = self->parameter.gain.kp * error + self->parameter.gain.ki * self->integral +
                 self->parameter.gain.kd * deriv;

  output = fmaxf(self->parameter.min, fminf(self->parameter.max, output));
  self->pre_error = error;
  return output;
}

void om_pid_reset(PidController* self) {
  self->integral = 0;
  self->pre_error = 0;
}

void om_pid_set_gain(PidController* self, const PidGain gain) {
  self->parameter.gain = gain;
}

void om_pid_set_limit(PidController* self, const float max, const float min) {
  self->parameter.max = max;
  self->parameter.min = min;
}