#include "can_cube.h"

#include <string.h>

static bool can_cube_queue_push(CanCube* cube, const CanMessage* msg) {
  if (cube->rx_count >= CAN_CUBE_RX_QUEUE_SIZE) {
    cube->rx_overflow_count++;
    return false;
  }

  cube->rx_queue[cube->rx_head] = *msg;
  cube->rx_head = (uint8_t)((cube->rx_head + 1U) % CAN_CUBE_RX_QUEUE_SIZE);
  cube->rx_count++;
  return true;
}

static bool can_cube_queue_pop(CanCube* cube, CanMessage* msg) {
  if (cube->rx_count == 0U) {
    return false;
  }

  *msg = cube->rx_queue[cube->rx_tail];
  cube->rx_tail = (uint8_t)((cube->rx_tail + 1U) % CAN_CUBE_RX_QUEUE_SIZE);
  cube->rx_count--;
  return true;
}

static bool can_cube_bus_write_impl(void* self, const CanMessage* msg) {
  CanCube* cube = (CanCube*)self;
  if (cube->ops.write == 0) {
    return false;
  }
  return cube->ops.write(cube->hal_context, msg);
}

static bool can_cube_bus_read_impl(void* self, CanMessage* msg) {
  CanCube* cube = (CanCube*)self;
  return can_cube_queue_pop(cube, msg);
}

static void can_cube_bus_start_read_impl(void* self) {
  CanCube* cube = (CanCube*)self;
  if (cube->ops.start_read == 0) {
    return;
  }
  cube->ops.start_read(cube->hal_context);
}

static void can_cube_bus_stop_read_impl(void* self) {
  CanCube* cube = (CanCube*)self;
  if (cube->ops.stop_read == 0) {
    return;
  }
  cube->ops.stop_read(cube->hal_context);
}

static void can_cube_bus_destroy_impl(void* self) {
  (void)self;
}

void can_cube_init(CanCube* cube, void* hal_context, const CanCubeOps* ops) {
  memset(cube, 0, sizeof(*cube));

  cube->hal_context = hal_context;
  if (ops != 0) {
    cube->ops = *ops;
  }

  cube->bus.write = can_cube_bus_write_impl;
  cube->bus.read = can_cube_bus_read_impl;
  cube->bus.start_read = can_cube_bus_start_read_impl;
  cube->bus.stop_read = can_cube_bus_stop_read_impl;
  cube->bus.destroy = can_cube_bus_destroy_impl;
  cube->bus.impl = cube;
}

CanBus* can_cube_bus(CanCube* cube) { return &cube->bus; }

void can_cube_set_rx_callback(CanCube* cube, CanRxCallback callback,
                              void* user_arg) {
  cube->rx_callback = callback;
  cube->rx_callback_user_arg = user_arg;
}

bool can_cube_poll(CanCube* cube, CanMessage* msg) {
  return can_cube_queue_pop(cube, msg);
}

uint32_t can_cube_get_rx_overflow_count(const CanCube* cube) {
  return cube->rx_overflow_count;
}

void can_cube_start_read(CanCube* cube) {
  if (cube->ops.start_read == 0) {
    return;
  }
  cube->ops.start_read(cube->hal_context);
}

void can_cube_stop_read(CanCube* cube) {
  if (cube->ops.stop_read == 0) {
    return;
  }
  cube->ops.stop_read(cube->hal_context);
}

void can_cube_on_rx_pending(CanCube* cube) {
  CanMessage msg;

  if (cube->ops.read_hw == 0) {
    return;
  }

  while (cube->ops.read_hw(cube->hal_context, &msg)) {
    can_cube_queue_push(cube, &msg);
    if (cube->rx_callback != 0) {
      cube->rx_callback(&msg, cube->rx_callback_user_arg);
    }
  }
}
