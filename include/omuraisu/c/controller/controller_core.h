#ifndef CONTROLLER_CORE_H
#define CONTROLLER_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief PS5コントローラーのボタンビットフラグ
typedef enum {
  OM_CONTROLLER_BUTTON_CIRCLE = 0x0001,    ///< × (bit 0)
  OM_CONTROLLER_BUTTON_CROSS = 0x0002,     ///< ○ (bit 1)
  OM_CONTROLLER_BUTTON_SQUARE = 0x0004,    ///< □ (bit 2)
  OM_CONTROLLER_BUTTON_TRIANGLE = 0x0008,  ///< △ (bit 3)
  OM_CONTROLLER_BUTTON_L1 = 0x0010,        ///< L1 (bit 4)
  OM_CONTROLLER_BUTTON_R1 = 0x0020,        ///< R1 (bit 5)
  OM_CONTROLLER_BUTTON_L3 = 0x0040,        ///< L3 (bit 6)
  OM_CONTROLLER_BUTTON_R3 = 0x0080,        ///< R3 (bit 7)
  OM_CONTROLLER_BUTTON_SHARE = 0x0100,     ///< Share (bit 8)
  OM_CONTROLLER_BUTTON_OPTIONS = 0x0200,   ///< Options (bit 9)
} ControllerButton;

/// @brief D-Padのビットフラグ
typedef enum {
  OM_CONTROLLER_DPAD_UP = 0x01,     ///< 上
  OM_CONTROLLER_DPAD_RIGHT = 0x02,  ///< 右
  OM_CONTROLLER_DPAD_DOWN = 0x04,   ///< 下
  OM_CONTROLLER_DPAD_LEFT = 0x08,   ///< 左
} ControllerDPad;

/// @brief PS5コントローラーデータ（プラットフォーム非依存）
typedef struct {
  float left_x;
  float left_y;
  float right_x;
  float right_y;

  float l2_trigger;
  float r2_trigger;

  uint16_t buttons;
  uint8_t dpad;
} ControllerData;

bool om_ctrl_is_pressed_button(const ControllerData* data,
                               ControllerButton button);
bool om_ctrl_is_pressed_dpad(const ControllerData* data,
                             ControllerDPad dpad_dir);

bool om_ctrl_up(const ControllerData* data);
bool om_ctrl_down(const ControllerData* data);
bool om_ctrl_left(const ControllerData* data);
bool om_ctrl_right(const ControllerData* data);

bool om_ctrl_cross(const ControllerData* data);
bool om_ctrl_circle(const ControllerData* data);
bool om_ctrl_square(const ControllerData* data);
bool om_ctrl_triangle(const ControllerData* data);
bool om_ctrl_l1(const ControllerData* data);
bool om_ctrl_r1(const ControllerData* data);
bool om_ctrl_l3(const ControllerData* data);
bool om_ctrl_r3(const ControllerData* data);
bool om_ctrl_share(const ControllerData* data);
bool om_ctrl_options(const ControllerData* data);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CONTROLLER_CORE_H
