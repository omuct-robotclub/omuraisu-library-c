#include "can/can_stm32.h"

#include <string.h>

#ifndef OMURAISU_CAN_STM32_MAX_CONTEXTS
#define OMURAISU_CAN_STM32_MAX_CONTEXTS 4
#endif

#ifndef OMURAISU_CAN_STM32_HAL_HEADER
#define OMURAISU_CAN_STM32_HAL_HEADER "main.h"
#endif

#ifdef OMURAISU_CAN_STM32_ENABLE
#include OMURAISU_CAN_STM32_HAL_HEADER

typedef struct {
  CanStm32Context* context;
} CanStm32Slot;

static CanStm32Slot g_slots[OMURAISU_CAN_STM32_MAX_CONTEXTS];

static CanStm32Context* can_stm32_find_context(void* handle) {
  for (int i = 0; i < OMURAISU_CAN_STM32_MAX_CONTEXTS; ++i) {
    if (g_slots[i].context != 0 && g_slots[i].context->handle == handle) {
      return g_slots[i].context;
    }
  }
  return 0;
}

#ifdef OMURAISU_CAN_STM32_FDCAN_ENABLE
static uint32_t can_stm32_fdcan_dlc_from_len(uint8_t len) {
  switch (len) {
    case 0:
      return FDCAN_DLC_BYTES_0;
    case 1:
      return FDCAN_DLC_BYTES_1;
    case 2:
      return FDCAN_DLC_BYTES_2;
    case 3:
      return FDCAN_DLC_BYTES_3;
    case 4:
      return FDCAN_DLC_BYTES_4;
    case 5:
      return FDCAN_DLC_BYTES_5;
    case 6:
      return FDCAN_DLC_BYTES_6;
    case 7:
      return FDCAN_DLC_BYTES_7;
    default:
      return FDCAN_DLC_BYTES_8;
  }
}

static uint8_t can_stm32_len_from_fdcan_dlc(uint32_t dlc) {
  switch (dlc) {
    case FDCAN_DLC_BYTES_0:
      return 0;
    case FDCAN_DLC_BYTES_1:
      return 1;
    case FDCAN_DLC_BYTES_2:
      return 2;
    case FDCAN_DLC_BYTES_3:
      return 3;
    case FDCAN_DLC_BYTES_4:
      return 4;
    case FDCAN_DLC_BYTES_5:
      return 5;
    case FDCAN_DLC_BYTES_6:
      return 6;
    case FDCAN_DLC_BYTES_7:
      return 7;
    default:
      return 8;
  }
}
#endif  // OMURAISU_CAN_STM32_FDCAN_ENABLE

static bool can_stm32_can_write(void* self, const CanMessage* msg) {
  CanStm32Context* context = (CanStm32Context*)self;
  CAN_HandleTypeDef* hcan = (CAN_HandleTypeDef*)context->handle;
  CAN_TxHeaderTypeDef header;
  uint32_t mailbox = 0;

  if (context->kind != CAN_STM32_KIND_CAN) {
    return false;
  }

  memset(&header, 0, sizeof(header));
  header.DLC = msg->len > 8 ? 8 : msg->len;
  if (msg->id <= 0x7FFU) {
    header.StdId = msg->id;
    header.IDE = CAN_ID_STD;
  } else {
    header.ExtId = msg->id;
    header.IDE = CAN_ID_EXT;
  }
  header.RTR = CAN_RTR_DATA;

  return HAL_CAN_AddTxMessage(hcan, &header, (uint8_t*)msg->data, &mailbox) ==
         HAL_OK;
}

#ifdef OMURAISU_CAN_STM32_FDCAN_ENABLE
static bool can_stm32_fdcan_write(void* self, const CanMessage* msg) {
  CanStm32Context* context = (CanStm32Context*)self;
  FDCAN_HandleTypeDef* hfdcan = (FDCAN_HandleTypeDef*)context->handle;
  FDCAN_TxHeaderTypeDef header;

  if (context->kind != CAN_STM32_KIND_FDCAN) {
    return false;
  }

  memset(&header, 0, sizeof(header));
  header.Identifier = msg->id;
  header.IdType = msg->id <= 0x7FFU ? FDCAN_STANDARD_ID : FDCAN_EXTENDED_ID;
  header.TxFrameType = FDCAN_DATA_FRAME;
  header.DataLength = can_stm32_fdcan_dlc_from_len(msg->len);
  header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  header.BitRateSwitch = FDCAN_BRS_OFF;
  header.FDFormat = FDCAN_CLASSIC_CAN;
  header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  header.MessageMarker = 0;

  return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &header,
                                       (uint8_t*)msg->data) == HAL_OK;
}
#endif  // OMURAISU_CAN_STM32_FDCAN_ENABLE

static bool can_stm32_write(void* self, const CanMessage* msg) {
  CanStm32Context* context = (CanStm32Context*)self;

  if (context->kind == CAN_STM32_KIND_CAN) {
    return can_stm32_can_write(self, msg);
  }

#ifdef OMURAISU_CAN_STM32_FDCAN_ENABLE
  if (context->kind == CAN_STM32_KIND_FDCAN) {
    return can_stm32_fdcan_write(self, msg);
  }
#endif  // OMURAISU_CAN_STM32_FDCAN_ENABLE

  return false;
}

static bool can_stm32_read_common_can(CanStm32Context* context,
                                      CanMessage* msg) {
  CAN_HandleTypeDef* hcan = (CAN_HandleTypeDef*)context->handle;
  CAN_RxHeaderTypeDef header;

  if (HAL_CAN_GetRxFifoFillLevel(hcan, context->rx_fifo) == 0U) {
    return false;
  }

  if (HAL_CAN_GetRxMessage(hcan, context->rx_fifo, &header, msg->data) !=
      HAL_OK) {
    return false;
  }

  msg->id = header.IDE == CAN_ID_STD ? header.StdId : header.ExtId;
  msg->len = header.DLC > 8U ? 8U : (uint8_t)header.DLC;
  return true;
}

#ifdef OMURAISU_CAN_STM32_FDCAN_ENABLE
static bool can_stm32_read_common_fdcan(CanStm32Context* context,
                                        CanMessage* msg) {
  FDCAN_HandleTypeDef* hfdcan = (FDCAN_HandleTypeDef*)context->handle;
  FDCAN_RxHeaderTypeDef header;

  if (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, context->rx_fifo) == 0U) {
    return false;
  }

  if (HAL_FDCAN_GetRxMessage(hfdcan, context->rx_fifo, &header, msg->data) !=
      HAL_OK) {
    return false;
  }

  msg->id = header.Identifier;
  msg->len = can_stm32_len_from_fdcan_dlc(header.DataLength);
  return true;
}
#endif  // OMURAISU_CAN_STM32_FDCAN_ENABLE

static bool can_stm32_read_hw(void* self, CanMessage* msg) {
  CanStm32Context* context = (CanStm32Context*)self;

  if (context->kind == CAN_STM32_KIND_CAN) {
    return can_stm32_read_common_can(context, msg);
  }

#ifdef OMURAISU_CAN_STM32_FDCAN_ENABLE
  if (context->kind == CAN_STM32_KIND_FDCAN) {
    return can_stm32_read_common_fdcan(context, msg);
  }
#endif  // OMURAISU_CAN_STM32_FDCAN_ENABLE

  return false;
}

static void can_stm32_start_read(void* self) {
  CanStm32Context* context = (CanStm32Context*)self;

  if (context->kind == CAN_STM32_KIND_CAN) {
    CAN_HandleTypeDef* hcan = (CAN_HandleTypeDef*)context->handle;
    uint32_t it_mask = context->rx_fifo == 0U ? CAN_IT_RX_FIFO0_MSG_PENDING
                                              : CAN_IT_RX_FIFO1_MSG_PENDING;
    CAN_FilterTypeDef can_filter = {0};
    can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter.FilterIdHigh = 0x0000;
    can_filter.FilterIdLow = 0x0000;
    can_filter.FilterMaskIdHigh = 0x0000;
    can_filter.FilterMaskIdLow = 0x0000;
    can_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    can_filter.FilterActivation = ENABLE;
    can_filter.SlaveStartFilterBank = 14;

    if (hcan->Instance == CAN1) {
      can_filter.FilterBank = 0;
    } else {
      can_filter.FilterBank = 14;
    }

    HAL_CAN_ConfigFilter (hcan, &can_filter);
    HAL_CAN_Start(hcan);
    HAL_CAN_ActivateNotification(hcan, it_mask);
    return;
  }

#ifdef OMURAISU_CAN_STM32_FDCAN_ENABLE
  if (context->kind == CAN_STM32_KIND_FDCAN) {
    FDCAN_HandleTypeDef* hfdcan = (FDCAN_HandleTypeDef*)context->handle;
    uint32_t it_mask = context->rx_fifo == 0U ? FDCAN_IT_RX_FIFO0_NEW_MESSAGE
                                              : FDCAN_IT_RX_FIFO1_NEW_MESSAGE;
    FDCAN_FilterTypeDef sFilter;
    sFilter.IdType       = FDCAN_STANDARD_ID;
    sFilter.FilterIndex  = 0;
    sFilter.FilterType   = FDCAN_FILTER_MASK;
    sFilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilter.FilterID1    = 0x000;
    sFilter.FilterID2    = 0x000;
    HAL_FDCAN_ConfigFilter(hfdcan, &sFilter);
    HAL_FDCAN_Start(hfdcan);
    HAL_FDCAN_ActivateNotification(hfdcan, it_mask, 0U);
  }
#endif  // OMURAISU_CAN_STM32_FDCAN_ENABLE
}

static void can_stm32_stop_read(void* self) {
  CanStm32Context* context = (CanStm32Context*)self;

  if (context->kind == CAN_STM32_KIND_CAN) {
    CAN_HandleTypeDef* hcan = (CAN_HandleTypeDef*)context->handle;
    uint32_t it_mask = context->rx_fifo == 0U ? CAN_IT_RX_FIFO0_MSG_PENDING
                                              : CAN_IT_RX_FIFO1_MSG_PENDING;
    HAL_CAN_DeactivateNotification(hcan, it_mask);
    HAL_CAN_Stop(hcan);
    return;
  }

#ifdef OMURAISU_CAN_STM32_FDCAN_ENABLE
  if (context->kind == CAN_STM32_KIND_FDCAN) {
    FDCAN_HandleTypeDef* hfdcan = (FDCAN_HandleTypeDef*)context->handle;
    uint32_t it_mask = context->rx_fifo == 0U ? FDCAN_IT_RX_FIFO0_NEW_MESSAGE
                                              : FDCAN_IT_RX_FIFO1_NEW_MESSAGE;
    HAL_FDCAN_DeactivateNotification(hfdcan, it_mask);
    HAL_FDCAN_Stop(hfdcan);
  }
#endif  // OMURAISU_CAN_STM32_FDCAN_ENABLE
}

void can_stm32_context_init(CanStm32Context* context, CanCube* cube,
                            void* handle, CanStm32Kind kind,
                            uint32_t rx_fifo) {
  context->cube = cube;
  context->handle = handle;
  context->kind = kind;
  context->rx_fifo = rx_fifo;
}

void can_stm32_make_ops(CanCubeOps* ops) {
  ops->write = can_stm32_write;
  ops->read_hw = can_stm32_read_hw;
  ops->start_read = can_stm32_start_read;
  ops->stop_read = can_stm32_stop_read;
}

bool can_stm32_register(CanStm32Context* context) {
  for (int i = 0; i < OMURAISU_CAN_STM32_MAX_CONTEXTS; ++i) {
    if (g_slots[i].context == context) {
      return true;
    }
    if (g_slots[i].context == 0) {
      g_slots[i].context = context;
      return true;
    }
  }
  return false;
}

void can_stm32_unregister(CanStm32Context* context) {
  for (int i = 0; i < OMURAISU_CAN_STM32_MAX_CONTEXTS; ++i) {
    if (g_slots[i].context == context) {
      g_slots[i].context = 0;
      return;
    }
  }
}

void can_stm32_dispatch_rx(void* handle) {
  CanStm32Context* context = can_stm32_find_context(handle);
  if (context == 0 || context->cube == 0) {
    return;
  }
  can_cube_on_rx_pending(context->cube);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
  can_stm32_dispatch_rx(hcan);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) {
  can_stm32_dispatch_rx(hcan);
}

#ifdef OMURAISU_CAN_STM32_FDCAN_ENABLE
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan,
                               uint32_t RxFifo0ITs) {
  (void)RxFifo0ITs;
  can_stm32_dispatch_rx(hfdcan);
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef* hfdcan,
                               uint32_t RxFifo1ITs) {
  (void)RxFifo1ITs;
  can_stm32_dispatch_rx(hfdcan);
}
#endif  // OMURAISU_CAN_STM32_FDCAN_ENABLE

#else

void can_stm32_context_init(CanStm32Context* context, CanCube* cube,
                            void* handle, CanStm32Kind kind,
                            uint32_t rx_fifo) {
  (void)context;
  (void)cube;
  (void)handle;
  (void)kind;
  (void)rx_fifo;
}

void can_stm32_make_ops(CanCubeOps* ops) { memset(ops, 0, sizeof(*ops)); }

bool can_stm32_register(CanStm32Context* context) {
  (void)context;
  return false;
}

void can_stm32_unregister(CanStm32Context* context) { (void)context; }

void can_stm32_dispatch_rx(void* handle) { (void)handle; }

#endif  // OMURAISU_CAN_STM32_ENABLE