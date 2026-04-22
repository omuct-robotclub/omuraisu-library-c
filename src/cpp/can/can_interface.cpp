#include "can/can_interface.hpp"

namespace omuraisu {
namespace can {
namespace {

uint8_t clamp_can_len(uint8_t len) { return len > 8 ? 8 : len; }

void copy_data(uint8_t dst[8], const uint8_t src[8], uint8_t len) {
  uint8_t bounded_len = clamp_can_len(len);
  for (uint8_t i = 0; i < 8; ++i) {
    dst[i] = i < bounded_len ? src[i] : 0;
  }
}

}  // namespace

CanMessage::CanMessage() noexcept : ::CanMessage{0, {0}, 0} {}

CanMessage::CanMessage(uint32_t id_value, const uint8_t raw_data[8],
                       uint8_t len_value) noexcept
    : ::CanMessage{id_value, {0}, clamp_can_len(len_value)} {
  if (raw_data != nullptr) {
    copy_data(data, raw_data, len);
  }
}

CanMessage::CanMessage(const ::CanMessage& other) noexcept
    : ::CanMessage{other.id, {0}, clamp_can_len(other.len)} {
  copy_data(data, other.data, len);
}

CanMessage::CanMessage(const CanMessage& other) noexcept
    : ::CanMessage{other.id, {0}, clamp_can_len(other.len)} {
  copy_data(data, other.data, len);
}

CanMessage& CanMessage::operator=(const CanMessage& other) noexcept {
  if (this == &other) {
    return *this;
  }
  id = other.id;
  len = clamp_can_len(other.len);
  copy_data(data, other.data, len);
  return *this;
}

CCanBusAdapter::CCanBusAdapter(::CanBus* bus) noexcept : bus_(bus) {}

bool CCanBusAdapter::write(const CanMessage& msg) {
  if (bus_ == nullptr) {
    return false;
  }
  return can_bus_write(bus_, static_cast<const ::CanMessage*>(&msg));
}

bool CCanBusAdapter::read(CanMessage& msg) {
  if (bus_ == nullptr) {
    return false;
  }
  if (!can_bus_read(bus_, static_cast<::CanMessage*>(&msg))) {
    return false;
  }
  return true;
}

void CCanBusAdapter::start_read() {
  if (bus_ == nullptr) {
    return;
  }
  can_bus_start_read(bus_);
}

void CCanBusAdapter::stop_read() {
  if (bus_ == nullptr) {
    return;
  }
  can_bus_stop_read(bus_);
}

CppCanBusBridge::CppCanBusBridge(ICanBus& bus) noexcept : bus_(&bus), c_bus_{} {
  c_bus_.write = &CppCanBusBridge::write_thunk;
  c_bus_.read = &CppCanBusBridge::read_thunk;
  c_bus_.start_read = &CppCanBusBridge::start_read_thunk;
  c_bus_.stop_read = &CppCanBusBridge::stop_read_thunk;
  c_bus_.destroy = &CppCanBusBridge::destroy_thunk;
  c_bus_.impl = this;
}

::CanBus* CppCanBusBridge::c_bus() noexcept { return &c_bus_; }

const ::CanBus* CppCanBusBridge::c_bus() const noexcept { return &c_bus_; }

bool CppCanBusBridge::write_thunk(void* self, const ::CanMessage* msg) {
  if (self == nullptr || msg == nullptr) {
    return false;
  }
  CppCanBusBridge* bridge = static_cast<CppCanBusBridge*>(self);
  if (bridge->bus_ == nullptr) {
    return false;
  }
  const CanMessage& cpp_msg = *static_cast<const CanMessage*>(msg);
  return bridge->bus_->write(cpp_msg);
}

bool CppCanBusBridge::read_thunk(void* self, ::CanMessage* msg) {
  if (self == nullptr || msg == nullptr) {
    return false;
  }
  CppCanBusBridge* bridge = static_cast<CppCanBusBridge*>(self);
  if (bridge->bus_ == nullptr) {
    return false;
  }
  CanMessage& cpp_msg = *static_cast<CanMessage*>(msg);
  return bridge->bus_->read(cpp_msg);
}

void CppCanBusBridge::start_read_thunk(void* self) {
  if (self == nullptr) {
    return;
  }
  CppCanBusBridge* bridge = static_cast<CppCanBusBridge*>(self);
  if (bridge->bus_ == nullptr) {
    return;
  }
  bridge->bus_->start_read();
}

void CppCanBusBridge::stop_read_thunk(void* self) {
  if (self == nullptr) {
    return;
  }
  CppCanBusBridge* bridge = static_cast<CppCanBusBridge*>(self);
  if (bridge->bus_ == nullptr) {
    return;
  }
  bridge->bus_->stop_read();
}

void CppCanBusBridge::destroy_thunk(void* self) { (void)self; }

}  // namespace can
}  // namespace omuraisu
