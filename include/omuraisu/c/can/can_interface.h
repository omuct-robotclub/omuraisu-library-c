#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief プラットフォーム非依存のCANメッセージ構造体
typedef struct {
  uint32_t id;
  uint8_t data[8];
  uint8_t len;
} CanMessage;

typedef void (*CanRxCallback)(const CanMessage* msg, void* user_arg);

/// @brief CANバスの抽象インターフェース
/// @details 各プラットフォーム（mbed, Arduino, Linux SocketCANなど）で
///          このインターフェースを実装する。
typedef struct {
  bool (*write)(void* self, const CanMessage* msg);

  bool (*read)(void* self, CanMessage* msg);

  void (*start_read)(void* self);
  void (*stop_read)(void* self);

  void (*destroy)(void* self);

  void* impl;
} CanBus;

bool can_bus_write(CanBus* bus, const CanMessage* msg);

bool can_bus_read(CanBus* bus, CanMessage* msg);

void can_bus_start_read(CanBus* bus);

void can_bus_stop_read(CanBus* bus);

void can_bus_destroy(CanBus* bus);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CAN_INTERFACE_H
