#ifndef CAN_CUBE_H
#define CAN_CUBE_H

#include <stdbool.h>
#include <stdint.h>

#include "can/can_interface.h"

#ifndef CAN_CUBE_RX_QUEUE_SIZE
#define CAN_CUBE_RX_QUEUE_SIZE 16
#endif

typedef struct {
  bool (*write)(void* hal_context, const CanMessage* msg);
  bool (*read_hw)(void* hal_context, CanMessage* msg);
  void (*start_read)(void* hal_context);
  void (*stop_read)(void* hal_context);
} CanCubeOps;

typedef struct {
  CanBus bus;

  void* hal_context;
  CanCubeOps ops;

  CanMessage rx_queue[CAN_CUBE_RX_QUEUE_SIZE];
  uint8_t rx_head;
  uint8_t rx_tail;
  uint8_t rx_count;

  uint32_t rx_overflow_count;

  CanRxCallback rx_callback;
  void* rx_callback_user_arg;
} CanCube;

void can_cube_init(CanCube* cube, void* hal_context, const CanCubeOps* ops);

CanBus* can_cube_bus(CanCube* cube);

void can_cube_set_rx_callback(CanCube* cube, CanRxCallback callback,
                              void* user_arg);

bool can_cube_poll(CanCube* cube, CanMessage* msg);

uint32_t can_cube_get_rx_overflow_count(const CanCube* cube);

void can_cube_start_read(CanCube* cube);

void can_cube_stop_read(CanCube* cube);

void can_cube_on_rx_pending(CanCube* cube);

#endif  // CAN_CUBE_H
