#include <cstdint>
#include <iostream>
#include <string>

#include "dji/robomas.h"
#include "dji/robomas.hpp"

namespace {

bool ExpectTrue(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << message << std::endl;
    return false;
  }
  return true;
}

class FakeCanBus : public omuraisu::can::ICanBus {
 public:
  bool read_result = false;
  bool write_result_first = true;
  bool write_result_second = true;

  int write_count = 0;
  omuraisu::can::CanMessage written_msgs[2];
  omuraisu::can::CanMessage next_read_msg;

  bool write(const omuraisu::can::CanMessage& msg) override {
    if (write_count < 2) {
      written_msgs[write_count] = msg;
    }
    ++write_count;
    if (write_count == 1) {
      return write_result_first;
    }
    if (write_count == 2) {
      return write_result_second;
    }
    return true;
  }

  bool read(omuraisu::can::CanMessage& msg) override {
    if (!read_result) {
      return false;
    }
    msg = next_read_msg;
    return true;
  }
};

bool TestRobomasReadParsesMotorData() {
  FakeCanBus bus;
  omuraisu::dji::Robomas rm(bus);

  const uint8_t raw[8] = {0x12, 0x34, 0x01, 0x02, 0xFE, 0xDC, 0x55, 0x00};
  bus.next_read_msg = omuraisu::can::CanMessage(0x201U, raw, 8U);
  bus.read_result = true;

  const int parsed_index = rm.read();
  if (!ExpectTrue(parsed_index == 0, "read should parse 0x201 as index 0")) {
    return false;
  }

  omuraisu::dji::RobomasData data = rm.get_data(1);
  if (!ExpectTrue(data.angle == 0x1234U, "angle should be parsed correctly")) {
    return false;
  }
  if (!ExpectTrue(data.rpm == 0x0102, "rpm should be parsed correctly")) {
    return false;
  }
  if (!ExpectTrue(data.current == static_cast<int16_t>(0xFEDC),
                  "current should be parsed correctly")) {
    return false;
  }
  return ExpectTrue(data.temp == 0x55U, "temp should be parsed correctly");
}

bool TestRobomasWriteSendsBothGroups() {
  FakeCanBus bus;
  omuraisu::dji::Robomas rm(bus);

  rm.set_output(0x1111, 1);
  rm.set_output(0x2222, 5);

  const bool ok = rm.write();
  if (!ExpectTrue(ok, "write should succeed when both CAN writes succeed")) {
    return false;
  }
  if (!ExpectTrue(bus.write_count == 2, "write should send two CAN messages")) {
    return false;
  }

  if (!ExpectTrue(bus.written_msgs[0].id == TX_ID_GROUP1,
                  "first message should target group1 ID")) {
    return false;
  }
  if (!ExpectTrue(bus.written_msgs[1].id == TX_ID_GROUP2,
                  "second message should target group2 ID")) {
    return false;
  }

  if (!ExpectTrue(bus.written_msgs[0].data[0] == 0x11U &&
                      bus.written_msgs[0].data[1] == 0x11U,
                  "group1 payload should contain motor1 output")) {
    return false;
  }
  return ExpectTrue(bus.written_msgs[1].data[0] == 0x22U &&
                        bus.written_msgs[1].data[1] == 0x22U,
                    "group2 payload should contain motor5 output");
}

bool TestSetMaxOutputInt16MinIsHandled() {
  omuraisu::dji::RobomasCore core;
  core.set_max_output(INT16_MIN);

  core.set_output(100, 1);
  uint8_t out[8] = {0};
  core.get_output_group(out, 0);

  if (!ExpectTrue(out[0] == 0x00U && out[1] == 0x64U,
                  "INT16_MIN handling should not produce overflowed clamp")) {
    return false;
  }

  core.set_output(INT16_MAX, 1);
  core.get_output_group(out, 0);
  return ExpectTrue(out[0] == 0x7FU && out[1] == 0xFFU,
                    "max output should clamp to INT16_MAX after INT16_MIN set");
}

bool TestGetDataConstOutOfRangeReturnsNull() {
  Robomas c_rm;
  c_rm.can = NULL;
  c_rm.core = om_rm_core_init();

  if (!ExpectTrue(om_rm_get_data_const(&c_rm, 0) == NULL,
                  "id=0 should return NULL")) {
    return false;
  }
  return ExpectTrue(om_rm_get_data_const(&c_rm, 9) == NULL,
                    "id=9 should return NULL");
}

}  // namespace

int main() {
  bool ok = true;

  ok = TestRobomasReadParsesMotorData() && ok;
  ok = TestRobomasWriteSendsBothGroups() && ok;
  ok = TestSetMaxOutputInt16MinIsHandled() && ok;
  ok = TestGetDataConstOutOfRangeReturnsNull() && ok;

  if (!ok) {
    std::cerr << "dji_cpp_test failed" << std::endl;
    return 1;
  }

  std::cout << "dji_cpp_test passed" << std::endl;
  return 0;
}
