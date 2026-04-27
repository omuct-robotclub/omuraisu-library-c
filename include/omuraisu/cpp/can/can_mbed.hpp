#ifndef OMURAISU_CPP_CAN_CAN_MBED_HPP_
#define OMURAISU_CPP_CAN_CAN_MBED_HPP_

#include "can/can_interface.hpp"

// このファイルはmbed環境でのみ使用可能
#if defined(__has_include)
#if __has_include("mbed.h")
#include "mbed.h"

namespace omuraisu {
namespace can {

/// @brief mbed用のICanBus実装
class MbedCanBus : public ICanBus {
 public:
  /// @brief コンストラクタ（ピン指定）
  /// @param rx CAN RXピン
  /// @param tx CAN TXピン
  /// @param frequency CANバス周波数（デフォルト1MHz）
  MbedCanBus(PinName rx, PinName tx, int frequency = 1000000);

  /// @brief コンストラクタ（既存CANオブジェクト使用）
  /// @param can 既存のCANオブジェクトへの参照
  explicit MbedCanBus(CAN& can);

  ~MbedCanBus() override;

  // コピー禁止
  MbedCanBus(const MbedCanBus&) = delete;
  MbedCanBus& operator=(const MbedCanBus&) = delete;

  bool write(const CanMessage& msg) override;

  bool read(CanMessage& msg) override;

  /// @brief 内部のCANオブジェクトへのアクセス
  CAN& get_can();

 private:
  CAN* can_;
  bool owned_;
};

}  // namespace can
}  // namespace omuraisu

#endif
#endif  // __has_include check

#endif  // OMURAISU_CPP_CAN_CAN_MBED_HPP_
