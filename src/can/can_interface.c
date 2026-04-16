#include "can/can_interface.h"

bool can_bus_write(CanBus* bus, const CanMessage* msg) {
  if (bus == 0 || bus->write == 0) {
    return false;
  }
  return bus->write(bus->impl, msg);
}

bool can_bus_read(CanBus* bus, CanMessage* msg) {
  if (bus == 0 || bus->read == 0) {
    return false;
  }
  return bus->read(bus->impl, msg);
}

void can_bus_start_read(CanBus* bus) {
  if (bus == 0 || bus->start_read == 0) {
    return;
  }
  bus->start_read(bus->impl);
}

void can_bus_stop_read(CanBus* bus) {
  if (bus == 0 || bus->stop_read == 0) {
    return;
  }
  bus->stop_read(bus->impl);
}

void can_bus_destroy(CanBus* bus) {
  if (bus == 0 || bus->destroy == 0) {
    return;
  }
  bus->destroy(bus->impl);
}
