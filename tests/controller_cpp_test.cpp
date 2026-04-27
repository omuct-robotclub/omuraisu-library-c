#include <cstdint>
#include <iostream>
#include <string>

#include "controller/controller_core.hpp"
#include "controller/controller_transport.hpp"

namespace {

bool ExpectTrue(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << message << std::endl;
    return false;
  }
  return true;
}

bool ExpectNear(float actual, float expected, float epsilon,
                const std::string& message) {
  if (actual < expected - epsilon || actual > expected + epsilon) {
    std::cerr << message << " (actual=" << actual << ", expected=" << expected
              << ")" << std::endl;
    return false;
  }
  return true;
}

omuraisu::controller::ControllerData MakeSampleData() {
  omuraisu::controller::ControllerData data = {};
  data.left_x = 0.5f;
  data.left_y = -0.25f;
  data.right_x = 1.0f;
  data.right_y = -1.0f;
  data.l2_trigger = 0.2f;
  data.r2_trigger = 0.8f;
  data.buttons = OM_CONTROLLER_BUTTON_CROSS | OM_CONTROLLER_BUTTON_L1 |
                 OM_CONTROLLER_BUTTON_OPTIONS;
  data.dpad = OM_CONTROLLER_DPAD_UP | OM_CONTROLLER_DPAD_LEFT;
  return data;
}

bool TestControllerCoreHelpers() {
  omuraisu::controller::ControllerData data = {};
  data.buttons = OM_CONTROLLER_BUTTON_CIRCLE | OM_CONTROLLER_BUTTON_SHARE;
  data.dpad = OM_CONTROLLER_DPAD_RIGHT;

  if (!ExpectTrue(data.circle(), "circle() should be true")) {
    return false;
  }
  if (!ExpectTrue(data.share(), "share() should be true")) {
    return false;
  }
  if (!ExpectTrue(!data.cross(), "cross() should be false")) {
    return false;
  }
  if (!ExpectTrue(data.right(), "right() should be true")) {
    return false;
  }
  return ExpectTrue(!data.left(), "left() should be false");
}

bool TestSerialPacketRoundTrip() {
  omuraisu::controller::ControllerData src = MakeSampleData();
  omuraisu::controller::SerialPacket packet(src);

  if (!ExpectTrue(packet.verify_checksum(), "serial checksum should be valid")) {
    return false;
  }

  omuraisu::controller::ControllerData decoded = packet.to_data();
  if (!ExpectNear(decoded.left_x, src.left_x, 0.02f,
                  "serial left_x round-trip should match")) {
    return false;
  }
  if (!ExpectNear(decoded.left_y, src.left_y, 0.02f,
                  "serial left_y round-trip should match")) {
    return false;
  }
  if (!ExpectNear(decoded.l2_trigger, src.l2_trigger, 0.02f,
                  "serial l2 round-trip should match")) {
    return false;
  }
  if (!ExpectNear(decoded.r2_trigger, src.r2_trigger, 0.02f,
                  "serial r2 round-trip should match")) {
    return false;
  }
  if (!ExpectTrue(decoded.buttons == src.buttons,
                  "serial buttons round-trip should match")) {
    return false;
  }
  return ExpectTrue(decoded.dpad == src.dpad,
                    "serial dpad round-trip should match");
}

bool TestCanAnalogEncodeDecode() {
  omuraisu::controller::ControllerData src = MakeSampleData();
  uint8_t can_payload[8] = {0};

  if (!ExpectTrue(omuraisu::controller::data_to_can_analog(src, can_payload),
                  "data_to_can_analog should succeed")) {
    return false;
  }

  omuraisu::controller::ControllerData decoded =
      omuraisu::controller::data_from_can(OM_CONTROLLER_CAN_ID_ANALOG,
                                          can_payload);

  if (!ExpectNear(decoded.left_x, src.left_x, 0.02f,
                  "can analog left_x round-trip should match")) {
    return false;
  }
  if (!ExpectNear(decoded.right_y, src.right_y, 0.02f,
                  "can analog right_y round-trip should match")) {
    return false;
  }
  if (!ExpectNear(decoded.l2_trigger, src.l2_trigger, 0.02f,
                  "can analog l2 round-trip should match")) {
    return false;
  }
  return ExpectNear(decoded.r2_trigger, src.r2_trigger, 0.02f,
                    "can analog r2 round-trip should match");
}

bool TestCanButtonsEncodeDecode() {
  omuraisu::controller::ControllerData src = MakeSampleData();
  uint8_t can_payload[8] = {0};

  if (!ExpectTrue(omuraisu::controller::data_to_can_buttons(src, can_payload),
                  "data_to_can_buttons should succeed")) {
    return false;
  }

  omuraisu::controller::ControllerData decoded =
      omuraisu::controller::data_from_can(OM_CONTROLLER_CAN_ID_BUTTONS,
                                          can_payload);

  if (!ExpectTrue(decoded.buttons == src.buttons,
                  "can button round-trip should match")) {
    return false;
  }
  return ExpectTrue(decoded.dpad == src.dpad,
                    "can dpad round-trip should match");
}

bool TestRosJoyEncodeDecode() {
  omuraisu::controller::ControllerData src = MakeSampleData();
  float axes[6] = {0.0f};
  int32_t buttons[14] = {0};

  if (!ExpectTrue(omuraisu::controller::data_to_ros_joy(src, axes, 6, buttons,
                                                         14),
                  "data_to_ros_joy should succeed")) {
    return false;
  }

  omuraisu::controller::ControllerData decoded =
      omuraisu::controller::data_from_ros_joy(axes, 6, buttons, 14);

  if (!ExpectNear(decoded.left_x, src.left_x, 1e-6f,
                  "ros left_x round-trip should match")) {
    return false;
  }
  if (!ExpectNear(decoded.right_y, src.right_y, 1e-6f,
                  "ros right_y round-trip should match")) {
    return false;
  }
  if (!ExpectTrue(decoded.buttons == src.buttons,
                  "ros buttons round-trip should match")) {
    return false;
  }
  return ExpectTrue(decoded.dpad == src.dpad,
                    "ros dpad round-trip should match");
}

bool TestEncoderGuards() {
  omuraisu::controller::ControllerData src = MakeSampleData();

  if (!ExpectTrue(!omuraisu::controller::data_to_can_analog(src, nullptr),
                  "data_to_can_analog should fail on null output")) {
    return false;
  }
  if (!ExpectTrue(!omuraisu::controller::data_to_can_buttons(src, nullptr),
                  "data_to_can_buttons should fail on null output")) {
    return false;
  }

  float axes[5] = {0.0f};
  int32_t buttons[13] = {0};
  return ExpectTrue(
      !omuraisu::controller::data_to_ros_joy(src, axes, 5, buttons, 13),
      "data_to_ros_joy should fail on insufficient array sizes");
}

}  // namespace

int main() {
  bool ok = true;

  ok = TestControllerCoreHelpers() && ok;
  ok = TestSerialPacketRoundTrip() && ok;
  ok = TestCanAnalogEncodeDecode() && ok;
  ok = TestCanButtonsEncodeDecode() && ok;
  ok = TestRosJoyEncodeDecode() && ok;
  ok = TestEncoderGuards() && ok;

  if (!ok) {
    std::cerr << "controller_cpp_test failed" << std::endl;
    return 1;
  }

  std::cout << "controller_cpp_test passed" << std::endl;
  return 0;
}
