#include <cstdint>
#include <iostream>
#include <string>

#include "can/can_interface.hpp"

namespace {

bool ExpectTrue(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << message << std::endl;
    return false;
  }
  return true;
}

struct FakeCBusContext {
  bool write_called;
  bool read_called;
  bool start_called;
  bool stop_called;
  ::CanMessage last_written;
  ::CanMessage read_message;
  bool write_result;
  bool read_result;
};

bool FakeCWrite(void* self, const ::CanMessage* msg) {
  FakeCBusContext* ctx = static_cast<FakeCBusContext*>(self);
  ctx->write_called = true;
  ctx->last_written = *msg;
  return ctx->write_result;
}

bool FakeCRead(void* self, ::CanMessage* msg) {
  FakeCBusContext* ctx = static_cast<FakeCBusContext*>(self);
  ctx->read_called = true;
  if (!ctx->read_result) {
    return false;
  }
  *msg = ctx->read_message;
  return true;
}

void FakeCStart(void* self) {
  FakeCBusContext* ctx = static_cast<FakeCBusContext*>(self);
  ctx->start_called = true;
}

void FakeCStop(void* self) {
  FakeCBusContext* ctx = static_cast<FakeCBusContext*>(self);
  ctx->stop_called = true;
}

void FakeCDestroy(void* self) { (void)self; }

bool TestCanMessageConversion() {
  uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  omuraisu::can::CanMessage msg(0x123U, data, 12U);

  if (!ExpectTrue(msg.len == 8U, "message len should be clamped to 8")) {
    return false;
  }

  const ::CanMessage& c_msg = static_cast<const ::CanMessage&>(msg);
  if (!ExpectTrue(c_msg.id == 0x123U, "message id should be preserved")) {
    return false;
  }
  return ExpectTrue(c_msg.data[7] == 8U, "message data should be copied");
}

bool TestCCanBusAdapter() {
  FakeCBusContext ctx = {};
  ctx.write_result = true;
  ctx.read_result = true;
  ctx.read_message.id = 0x456U;
  ctx.read_message.len = 2U;
  ctx.read_message.data[0] = 0xAAU;
  ctx.read_message.data[1] = 0x55U;

  ::CanBus c_bus = {};
  c_bus.write = FakeCWrite;
  c_bus.read = FakeCRead;
  c_bus.start_read = FakeCStart;
  c_bus.stop_read = FakeCStop;
  c_bus.destroy = FakeCDestroy;
  c_bus.impl = &ctx;

  omuraisu::can::CCanBusAdapter adapter(&c_bus);

  uint8_t write_data[8] = {9, 8, 7, 6, 5, 4, 3, 2};
  omuraisu::can::CanMessage write_msg(0x111U, write_data, 8U);
  if (!ExpectTrue(adapter.write(write_msg), "adapter write should succeed")) {
    return false;
  }
  if (!ExpectTrue(ctx.write_called, "C write callback should be called")) {
    return false;
  }

  omuraisu::can::CanMessage read_msg;
  if (!ExpectTrue(adapter.read(read_msg), "adapter read should succeed")) {
    return false;
  }
  if (!ExpectTrue(ctx.read_called, "C read callback should be called")) {
    return false;
  }
  if (!ExpectTrue(read_msg.id == 0x456U && read_msg.data[0] == 0xAAU,
                  "adapter read message should match C data")) {
    return false;
  }

  adapter.start_read();
  adapter.stop_read();
  return ExpectTrue(ctx.start_called && ctx.stop_called,
                    "adapter start/stop should call C callbacks");
}

class FakeCppBus : public omuraisu::can::ICanBus {
 public:
  bool write_called = false;
  bool read_called = false;
  bool start_called = false;
  bool stop_called = false;

  omuraisu::can::CanMessage written_msg;
  omuraisu::can::CanMessage next_read_msg;
  bool write_result = true;
  bool read_result = true;

  bool write(const omuraisu::can::CanMessage& msg) override {
    write_called = true;
    written_msg = msg;
    return write_result;
  }

  bool read(omuraisu::can::CanMessage& msg) override {
    read_called = true;
    if (!read_result) {
      return false;
    }
    msg = next_read_msg;
    return true;
  }

  void start_read() override { start_called = true; }

  void stop_read() override { stop_called = true; }
};

bool TestCppCanBusBridge() {
  FakeCppBus cpp_bus;
  uint8_t read_data[8] = {1, 3, 5, 7, 9, 0, 0, 0};
  cpp_bus.next_read_msg = omuraisu::can::CanMessage(0x321U, read_data, 5U);

  omuraisu::can::CppCanBusBridge bridge(cpp_bus);
  ::CanBus* c_bus = bridge.c_bus();

  ::CanMessage c_write = {0x222U, {10, 20, 30, 40, 0, 0, 0, 0}, 4U};
  if (!ExpectTrue(can_bus_write(c_bus, &c_write),
                  "bridge write should succeed through C API")) {
    return false;
  }
  if (!ExpectTrue(cpp_bus.write_called && cpp_bus.written_msg.id == 0x222U,
                  "bridge should forward write into C++ bus")) {
    return false;
  }

  ::CanMessage c_read = {0, {0}, 0};
  if (!ExpectTrue(can_bus_read(c_bus, &c_read),
                  "bridge read should succeed through C API")) {
    return false;
  }
  if (!ExpectTrue(
          cpp_bus.read_called && c_read.id == 0x321U && c_read.len == 5U,
          "bridge should forward read from C++ bus")) {
    return false;
  }

  can_bus_start_read(c_bus);
  can_bus_stop_read(c_bus);
  return ExpectTrue(cpp_bus.start_called && cpp_bus.stop_called,
                    "bridge should forward start/stop into C++ bus");
}

}  // namespace

int main() {
  bool ok = true;

  ok = TestCanMessageConversion() && ok;
  ok = TestCCanBusAdapter() && ok;
  ok = TestCppCanBusBridge() && ok;

  if (!ok) {
    std::cerr << "can_cpp_test failed" << std::endl;
    return 1;
  }

  std::cout << "can_cpp_test passed" << std::endl;
  return 0;
}
