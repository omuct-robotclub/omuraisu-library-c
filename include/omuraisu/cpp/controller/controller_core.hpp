#ifndef OMURAISU_CPP_CONTROLLER_CONTROLLER_CORE_HPP_
#define OMURAISU_CPP_CONTROLLER_CONTROLLER_CORE_HPP_
#include "controller/controller_core.h"
namespace omuraisu {
namespace controller {
enum class Button : uint16_t {
  Circle = OM_CONTROLLER_BUTTON_CIRCLE,
  Cross = OM_CONTROLLER_BUTTON_CROSS,
  Square = OM_CONTROLLER_BUTTON_SQUARE,
  Triangle = OM_CONTROLLER_BUTTON_TRIANGLE,
  L1 = OM_CONTROLLER_BUTTON_L1,
  R1 = OM_CONTROLLER_BUTTON_R1,
  L3 = OM_CONTROLLER_BUTTON_L3,
  R3 = OM_CONTROLLER_BUTTON_R3,
  Share = OM_CONTROLLER_BUTTON_SHARE,
  Options = OM_CONTROLLER_BUTTON_OPTIONS
};
enum class DPad : uint8_t {
  Up = OM_CONTROLLER_DPAD_UP,
  Right = OM_CONTROLLER_DPAD_RIGHT,
  Down = OM_CONTROLLER_DPAD_DOWN,
  Left = OM_CONTROLLER_DPAD_LEFT
};

struct ControllerData : public ::ControllerData {
  bool is_pressed(Button button) const;
  bool is_pressed(DPad dpad_dir) const;

  DPad get_dpad() const;

  bool up() const;
  bool down() const;
  bool left() const;
  bool right() const;
  bool cross() const;
  bool circle() const;
  bool square() const;
  bool triangle() const;
  bool l1() const;
  bool r1() const;
  bool l3() const;
  bool r3() const;
  bool share() const;
  bool options() const;
};
}  // namespace controller
}  // namespace omuraisu
#endif  // OMURAISU_CPP_CONTROLLER_CONTROLLER_CORE_HPP_