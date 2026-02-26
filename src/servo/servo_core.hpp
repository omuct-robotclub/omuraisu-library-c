#ifndef SERVO_CORE_HPP
#define SERVO_CORE_HPP

#include <can/can_interface.hpp>
#include <cstdint>
#include <cstring>

namespace servo {
/// @brief サーボ制御のコアロジック（プラットフォーム非依存）
/// @details CAN通信を含まない純粋なデータ処理クラス。
///          CAN送受信は使用者側で行う。
class ServoCore {
 public:
  static constexpr size_t kDataSize = 8;

  ServoCore(uint8_t id) : id_(id) { std::memset(data_, 0, kDataSize); }

  void set_degree(float degree, size_t index) {
    if (index >= kDataSize) {
      return;  // インデックスが範囲外の場合は無視
    }
    // 0-180度を0-255の範囲に変換
    uint8_t value = static_cast<uint8_t>(degree / 180.0f * 255);
    data_[index] = value;
  }

  void set_degrees(const float degrees[kDataSize]) {
    for (size_t i = 0; i < kDataSize; ++i) {
      // 0-180度を0-255の範囲に変換
      data_[i] = static_cast<uint8_t>(degrees[i] / 180.0f * 255);
    }
  }

  uint8_t get_degree(size_t index) const {
    if (index >= kDataSize) {
      return 0;  // インデックスが範囲外の場合は0を返す
    }
    return data_[index];
  }

  void get_degrees(float degrees[kDataSize]) const {
    for (size_t i = 0; i < kDataSize; ++i) {
      // 0-255の範囲を0-180度に変換
      degrees[i] = static_cast<float>(data_[i]) / 255.0f * 180.0f;
    }
  }

  can::CANMessage to_can_message() const {
    can::CANMessage msg;
    msg.id = id_;
    std::memcpy(msg.data, data_, kDataSize);
    msg.len = kDataSize;
    return msg;
  }

 private:
  uint8_t data_[kDataSize];
  uint8_t id_;
};
}  // namespace servo
#endif  // SERVO_CORE_HPP