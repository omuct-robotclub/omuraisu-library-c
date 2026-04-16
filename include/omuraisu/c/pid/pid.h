#ifndef PID_H
#define PID_H

typedef struct {
  float kp;
  float ki;
  float kd;
} PidGain;

typedef struct {
  PidGain gain;
  float min;
  float max;
} PidParameter;

typedef struct {
  PidParameter parameter;
  float pre_error;
  float integral;
} PidController;

PidController om_pid_init(const PidParameter parameter);

float om_pid_calc(PidController* self, const float goal, const float actual,
                  const float dt_sec);

void om_pid_reset(PidController* self);

void om_pid_set_gain(PidController* self, const PidGain gain);

void om_pid_set_limit(PidController* self, const float max, const float min);

#endif  // PID_H