#ifndef CAN_STM32_H
#define CAN_STM32_H

#include <stdbool.h>
#include <stdint.h>

#include "can/can_cube.h"

typedef enum {
  CAN_STM32_KIND_CAN = 0,
  CAN_STM32_KIND_FDCAN = 1,
} CanStm32Kind;

typedef struct {
  CanCube* cube;
  void* handle;
  CanStm32Kind kind;
  uint32_t rx_fifo;
} CanStm32Context;

void can_stm32_context_init(CanStm32Context* context, CanCube* cube,
                            void* handle, CanStm32Kind kind,
                            uint32_t rx_fifo);

void can_stm32_make_ops(CanCubeOps* ops);

bool can_stm32_register(CanStm32Context* context);

void can_stm32_unregister(CanStm32Context* context);

void can_stm32_dispatch_rx(void* handle);

#endif  // CAN_STM32_H