#include "controller/controller_core.h"

bool om_ctrl_is_pressed_button(const ControllerData* data,
                               ControllerButton button) {
  return data != 0 && (data->buttons & (uint16_t)button) != 0;
}

bool om_ctrl_is_pressed_dpad(const ControllerData* data,
                             ControllerDPad dpad_dir) {
  return data != 0 && (data->dpad & (uint8_t)dpad_dir) != 0;
}

bool om_ctrl_up(const ControllerData* data) {
  return om_ctrl_is_pressed_dpad(data, OM_CONTROLLER_DPAD_UP);
}

bool om_ctrl_down(const ControllerData* data) {
  return om_ctrl_is_pressed_dpad(data, OM_CONTROLLER_DPAD_DOWN);
}

bool om_ctrl_left(const ControllerData* data) {
  return om_ctrl_is_pressed_dpad(data, OM_CONTROLLER_DPAD_LEFT);
}

bool om_ctrl_right(const ControllerData* data) {
  return om_ctrl_is_pressed_dpad(data, OM_CONTROLLER_DPAD_RIGHT);
}

bool om_ctrl_cross(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_CROSS);
}

bool om_ctrl_circle(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_CIRCLE);
}

bool om_ctrl_square(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_SQUARE);
}

bool om_ctrl_triangle(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_TRIANGLE);
}

bool om_ctrl_l1(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_L1);
}

bool om_ctrl_r1(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_R1);
}

bool om_ctrl_l3(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_L3);
}

bool om_ctrl_r3(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_R3);
}

bool om_ctrl_share(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_SHARE);
}

bool om_ctrl_options(const ControllerData* data) {
  return om_ctrl_is_pressed_button(data, OM_CONTROLLER_BUTTON_OPTIONS);
}
