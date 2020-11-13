#include "remote_syscall.h"
#include <spi.h>
#include <console.h>
#include <stdlib.h>
#include <stdio.h>

#define BASE_TEN 10
#define BASE_HEX 16

int convert_str_to_int(char* str, int base);

int convert_str_to_int(char* str, int base) {
  char* end;
  return strtol(str, &end, base);
}

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
int syscallTwo (char* driverNum, char* subdriverNum, char* arg0, char* arg1) {
  int driver_num = convert_str_to_int(driverNum, BASE_HEX);
  printf("Driver Num: %d\n", driver_num);
  int switch_num = convert_str_to_int(subdriverNum, BASE_TEN);
  int zero_arg = convert_str_to_int(arg0, BASE_TEN);
  int one_arg = convert_str_to_int(arg1, BASE_TEN);
  return command(driver_num, switch_num, zero_arg, one_arg);
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
