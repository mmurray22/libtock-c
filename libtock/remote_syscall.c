#include "remote_syscall.h"
#include "spi_slave.h"

void subscribe_to_caller(subscribe_cb cb) {
  int res = spi_slave_get_chip_select();
  spi_slave_chip_selected(cb, NULL);
}

bool ack(int activeNumber) {
  /*COMPLETE*/
}

void activate_leds() {
  /*COMPLETE*/
}

void activate_temperature() {
  /*COMPLETE*/
}

void activate_qdec() {
  /*COMPLETE*/
}
