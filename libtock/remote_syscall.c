#include "remote_syscall.h"

int subscribe_to_caller(subscribe_cb cb, char* buf, size_t len) {
  int res = getnstr_async(buf, len, cb, NULL);
  return res;
}

/*Yield*/
void syscallZero () {
  yield();
}

/*Subscribe*/
void syscallOne (size_t driverNum, size_t subscribeNum, size_t arg0, subscribe_cb cb) {
  subscribe(driverNum, subscribeNum, cb, arg0);
}

/*Command*/
void syscallTwo (size_t driverNum, size_t subdriverNum, size_t arg0, size_t arg1) {
  command(driverNum, subdriverNum, arg0, arg1);
}

/*Allow*/
void syscallThree (size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer) {
  allow(driverNum, subdriverNum, buffer, arg0);
}

/*Read-Only Allow*/
void syscallFour(size_t driverNum, size_t subdriverNum, size_t arg0, char* buffer) {
  allow(driverNum, subdriverNum, buffer, arg0);
}

/*Memop*/
void syscallFive(size_t operand, size_t arg0) {
  memop(operand, arg0);
}
