#include "controller/controller_core.hpp"
namespace omuraisu {
namespace controller {
bool ControllerData::is_pressed(Button button) const {
  return om_ctrl_is_pressed_button(this, static_cast<ControllerButton>(button));
}

bool ControllerData::is_pressed(DPad dpad_dir) const {
  return om_ctrl_is_pressed_dpad(this, static_cast<ControllerDPad>(dpad_dir));
}

DPad ControllerData::get_dpad() const { return static_cast<DPad>(dpad); }

bool ControllerData::up() const { return is_pressed(DPad::Up); }
bool ControllerData::down() const { return is_pressed(DPad::Down); }
bool ControllerData::left() const { return is_pressed(DPad::Left); }
bool ControllerData::right() const { return is_pressed(DPad::Right); }

bool ControllerData::cross() const { return is_pressed(Button::Cross); }
bool ControllerData::circle() const { return is_pressed(Button::Circle); }
bool ControllerData::square() const { return is_pressed(Button::Square); }
bool ControllerData::triangle() const { return is_pressed(Button::Triangle); }

bool ControllerData::l1() const { return is_pressed(Button::L1); }
bool ControllerData::r1() const { return is_pressed(Button::R1); }
bool ControllerData::l3() const { return is_pressed(Button::L3); }
bool ControllerData::r3() const { return is_pressed(Button::R3); }

bool ControllerData::share() const { return is_pressed(Button::Share); }
bool ControllerData::options() const { return is_pressed(Button::Options); }
}  // namespace controller
}  // namespace omuraisu