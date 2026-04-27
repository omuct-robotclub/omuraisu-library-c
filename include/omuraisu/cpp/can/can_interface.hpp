#ifndef OMURAISU_CPP_CAN_CAN_INTERFACE_HPP_
#define OMURAISU_CPP_CAN_CAN_INTERFACE_HPP_

#include <cstdint>

#include "can/can_interface.h"

namespace omuraisu {
namespace can {
struct CanMessage : public ::CanMessage {
  CanMessage() noexcept;
  CanMessage(uint32_t id, const uint8_t data[8], uint8_t len) noexcept;
  explicit CanMessage(const ::CanMessage& other) noexcept;
  CanMessage(const CanMessage& other) noexcept;

  CanMessage& operator=(const CanMessage& other) noexcept;
};

class ICanBus {
 public:
  virtual ~ICanBus() = default;

  virtual bool write(const CanMessage& msg) = 0;
  virtual bool read(CanMessage& msg) = 0;
  virtual void start_read() {}
  virtual void stop_read() {}
};

class CCanBusAdapter : public ICanBus {
 public:
  explicit CCanBusAdapter(::CanBus* bus) noexcept;

  bool write(const CanMessage& msg) override;
  bool read(CanMessage& msg) override;
  void start_read() override;
  void stop_read() override;

 private:
  ::CanBus* bus_;
};

/// @brief Advanced: C++ ICanBus を C CanBus インターフェースにアダプト（C
/// 側資産から使用）
/// @details このクラスは「C コード内から C++ バス実装を直接叩く」という
///          例外的なシナリオ専用です。通常は
///          CCanBusAdapter（逆方向）を使用します。
class CppCanBusBridge {
 public:
  explicit CppCanBusBridge(ICanBus& bus) noexcept;

  CppCanBusBridge(const CppCanBusBridge&) = delete;
  CppCanBusBridge& operator=(const CppCanBusBridge&) = delete;

  ::CanBus* c_bus() noexcept;
  const ::CanBus* c_bus() const noexcept;

 private:
  static bool write_thunk(void* self, const ::CanMessage* msg);
  static bool read_thunk(void* self, ::CanMessage* msg);
  static void start_read_thunk(void* self);
  static void stop_read_thunk(void* self);
  static void destroy_thunk(void* self);

  ICanBus* bus_;
  ::CanBus c_bus_;
};

}  // namespace can
}  // namespace omuraisu

#endif  // OMURAISU_CPP_CAN_CAN_INTERFACE_HPP_