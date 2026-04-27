#include "can/can_mbed.hpp"

// このファイルはmbed環境でのみ使用可能
#if defined(__has_include)
#if __has_include("mbed.h")
#include "mbed.h"

namespace omuraisu {
namespace can {

MbedCanBus::MbedCanBus(PinName rx, PinName tx, int frequency)
    : can_(new CAN(rx, tx, frequency)), owned_(true) {}
MbedCanBus::MbedCanBus(CAN& can) : can_(&can), owned_(false) {}
MbedCanBus::~MbedCanBus() {
  if (owned_) {
    delete can_;
  }
}

bool MbedCanBus::write(const CanMessage& msg) {
  return can_->write(CANMessage(msg.id, msg.data, msg.len));
}
bool MbedCanBus::read(CanMessage& msg) {
  CANMessage mbed_msg;
  if (can_->read(mbed_msg)) {
    msg.id = mbed_msg.id;
    msg.len = mbed_msg.len;
    for (int i = 0; i < 8; ++i) {
      msg.data[i] = mbed_msg.data[i];
    }
    return true;
  }
  return false;
}
CAN& MbedCanBus::get_can() { return *can_; }

}  // namespace can
}  // namespace omuraisu

#endif
#endif  // __has_include check
