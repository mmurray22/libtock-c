#include "remote_syscall.h"
#include <spi.h>
#include <console.h>

int subscribe_to_caller(subscribe_cb cb, char* buf, size_t len) {
  int res = getnstr_async(buf, len, cb, NULL);
  return res;
}

//Yield
void syscallZero (void) {
  yield();
}

//Subscribe
int syscallOne (size_t driverNum, size_t subscribeNum, void* arg0, subscribe_cb cb) {
  return subscribe(driverNum, subscribeNum, cb, arg0);
}

//Command
int syscallTwo (size_t driverNum, size_t subdriverNum, size_t arg0, size_t arg1) {
  return command(driverNum, subdriverNum, arg0, arg1);
}

//Allow
int syscallThree (size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer) {
  return allow(driverNum, subdriverNum, buffer, arg0);
}

//Read-Only Allow
int syscallFour(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer) {
  return allow(driverNum, subdriverNum, buffer, arg0);
}

//Memop
void* syscallFive(size_t operand, size_t arg0) {
  return memop(operand, arg0);
}
